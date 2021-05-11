#!/bin/bash
################################################################
# intermediate ssl ciphers
# nginx
# https://ssl-config.mozilla.org/#server=nginx&version=1.20.0&config=intermediate&openssl=1.1.1&guideline=5.6
# https://ssl-config.mozilla.org/#server=nginx&version=1.9.10&config=intermediate&openssl=1.1.1k&guideline=5.6
# postfix
# https://ssl-config.mozilla.org/#server=postfix&version=2.10&config=intermediate&openssl=1.0.2i&guideline=5.6
################################################################
DT=$(date +"%d%m%y-%H%M%S")
DIR_TMP='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'
################################################################
# 2048 or 3072bit FFDHE param file
dhparam='3072'
################################################################

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

testssl_run() {
  vhostname="$1"
  mkdir -p /opt
  cd /opt
  rm -rf /opt/testssl
  git clone --depth 1 https://github.com/drwetter/testssl.sh.git testssl >/dev/null 2>&1
  if [[ ! "$(grep 'testssl.sh=' $HOME/.bashrc)" ]]; then
    echo "alias testssl.sh='/opt/testssl/testssl.sh'" >> "$HOME/.bashrc"
  fi
  echo "testssl.sh --nodns=min --wide -p -c -f -E -S -P --quiet https://${vhostname}"
  /opt/testssl/testssl.sh --nodns=min --wide -p -c -f -E -S -P --quiet "https://${vhostname}" | tee "${CENTMINLOGDIR}/testssl-${vhostname}-${DT}.log"
}

dhparam_setup() {
  vhostconfig="$1"
  if [ -f /usr/local/nginx/conf/ssl/dhparam.pem ]; then
    \cp -af /usr/local/nginx/conf/ssl/dhparam.pem /usr/local/nginx/conf/ssl/dhparam.pem-backup-${DT}
  fi
  echo "setup ffdhe${dhparam} dhparam file: /usr/local/nginx/conf/ssl/dhparam.pem"
if [[ "$dhparam" = '3072' ]]; then
  echo '-----BEGIN DH PARAMETERS-----
MIIBiAKCAYEA//////////+t+FRYortKmq/cViAnPTzx2LnFg84tNpWp4TZBFGQz
+8yTnc4kmz75fS/jY2MMddj2gbICrsRhetPfHtXV/WVhJDP1H18GbtCFY2VVPe0a
87VXE15/V8k1mE8McODmi3fipona8+/och3xWKE2rec1MKzKT0g6eXq8CrGCsyT7
YdEIqUuyyOP7uWrat2DX9GgdT0Kj3jlN9K5W7edjcrsZCwenyO4KbXCeAvzhzffi
7MA0BM0oNC9hkXL+nOmFg/+OTxIy7vKBg8P+OxtMb61zO7X8vC7CIAXFjvGDfRaD
ssbzSibBsu/6iGtCOGEfz9zeNVs7ZRkDW7w09N75nAI4YbRvydbmyQd62R0mkff3
7lmMsPrBhtkcrv4TCYUTknC0EwyTvEN5RPT9RFLi103TZPLiHnH1S/9croKrnJ32
nuhtK8UiNjoNq8Uhl5sN6todv5pC1cRITgq80Gv6U93vPBsg7j/VnXwl5B0rZsYu
N///////////AgEC
-----END DH PARAMETERS-----' > /usr/local/nginx/conf/ssl/dhparam.pem
elif [[ "$dhparam" = '2048' ]]; then
  echo '-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEA//////////+t+FRYortKmq/cViAnPTzx2LnFg84tNpWp4TZBFGQz
+8yTnc4kmz75fS/jY2MMddj2gbICrsRhetPfHtXV/WVhJDP1H18GbtCFY2VVPe0a
87VXE15/V8k1mE8McODmi3fipona8+/och3xWKE2rec1MKzKT0g6eXq8CrGCsyT7
YdEIqUuyyOP7uWrat2DX9GgdT0Kj3jlN9K5W7edjcrsZCwenyO4KbXCeAvzhzffi
7MA0BM0oNC9hkXL+nOmFg/+OTxIy7vKBg8P+OxtMb61zO7X8vC7CIAXFjvGDfRaD
ssbzSibBsu/6iGtCOGEoXJf//////////wIBAg==
-----END DH PARAMETERS-----' > /usr/local/nginx/conf/ssl/dhparam.pem
fi
  param_file=$(awk '/^  ssl_dhparam/ {print $2}' "$vhostconfig" | sed -e 's|;||g')
  echo "replace: $param_file"
  \cp -af /usr/local/nginx/conf/ssl/dhparam.pem "$param_file"
}

switch_ciphers_bulk() {
  choice="$1"
  reload="$2"
  find /usr/local/nginx/conf/conf.d/ -type f -name "*.ssl.conf" | while read f; do
    echo -e "\n------------------------------------------\nswitch $f\n------------------------------------------";
    switch_ciphers "$f" "$choice" "$reload";
  done
  echo
  service nginx reload
}

switch_ciphers() {
  vhostconfig="$1"
  choice="$2"
  reload="$3"
  if [ -f "$vhostconfig" ]; then
    if [[ "$choice" = 'int' ]]; then
      ciphers='  ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384;'
      echo
      echo "switched off: ssl_prefer_server_ciphers"
      sed -i "s|^  ssl_prefer_server_ciphers .*|  ssl_prefer_server_ciphers   off;|g" "$vhostconfig"
      echo -n "set: "
      grep 'ssl_prefer_server_ciphers' "$vhostconfig"
    elif [[ "$choice" = 'def' ]]; then
      ciphers='  ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA:ECDHE-ECDSA-DES-CBC3-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!DSS;'
      echo
      echo "switched on: ssl_prefer_server_ciphers"
      sed -i "s|^  ssl_prefer_server_ciphers .*|  ssl_prefer_server_ciphers   on;|g" "$vhostconfig"
      echo -n "set: "
      grep 'ssl_prefer_server_ciphers' "$vhostconfig"
    fi
    sed -i "s|^  ssl_ciphers .*|${ciphers}|g" "$vhostconfig"
    dhparam_setup "$vhostconfig"
    echo
    echo "switched off ssl_session_tickets"
    sed -i "s|^  ssl_session_tickets .*|  ssl_session_tickets off;|g" "$vhostconfig"
    echo -n "set: "
    grep 'ssl_session_tickets' "$vhostconfig"
    echo
    echo "switched ssl_ciphers"
    echo -n "set: "
    grep 'ssl_ciphers' "$vhostconfig"
    if [[ "$reload" != 'quiet' ]]; then
      echo
      service nginx reload
    fi
  else
    echo "nginx vhost file not found: $vhostconfig"
  fi
}

help() {
  echo
  echo "Usage:"
  echo
  echo "$0 intermediate-bulk"
  echo "$0 intermediate /usr/local/nginx/conf/conf.d/domain.com.ssl.conf"
  echo "$0 old-default-bulk"
  echo "$0 old-default /usr/local/nginx/conf/conf.d/domain.com.ssl.conf"
  echo "$0 testssl domain.com:443"
}

case "$1" in
  intermediate-bulk )
    switch_ciphers_bulk int quiet
    ;;
  intermediate )
    switch_ciphers "$2" int
    ;;
  old-default-bulk )
    switch_ciphers_bulk def quiet
    ;;
  old-default )
    switch_ciphers "$2" def
    ;;
  testssl )
    testssl_run "$2"
    ;;
  * )
    help
    ;;
esac