#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
#################################################################
# for centminmod.com LEMP stack environments
# auto generate and convert detected .htaccess file directory
# paths to nginx deny all equivalents only when .htaccess file
# contains 'Deny from all' text and generate a per nginx vhost
# auto protect directory at /usr/local/nginx/conf/autoprotect/${domain}
# with /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf
# include file which you include into your nginx vhost above the
# root / location match i.e.
# include /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf;
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)
NICE=$(which nice)
NICEOPT='-n 12'
IONICE=$(which ionice)
IONICEOPT='-c2 -n7'

DEBUG='n'
TOPLEVEL_DIR='/home/nginx/domains'

# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

check_location_block() {
    domain=$1
    location_block=$2
    # Check both HTTP and HTTPS vhost config files
    for config in "/usr/local/nginx/conf/conf.d/${domain}.conf" "/usr/local/nginx/conf/conf.d/${domain}.ssl.conf"; do
        if [ -f "$config" ]; then
            # Search for the location block in the config file
            if grep -Eq "location\s+$location_block\s+\{" $config; then
                # If found, return 1 to indicate it should be skipped
                return 1
            fi
        fi
    done
    # If not found in any config file, return 0 to indicate it should be added
    return 0
}

genprotect() {
  # need to restart to ensure all existing or recently 403 denied directory settings
  # are detected
  service nginx reload >/dev/null 2>&1
  sleep 2

	# generate /usr/local/nginx/conf/autoprotect.conf include file
	# to be placed in individual nginx vhosts
for domain in $(ls $TOPLEVEL_DIR); do
	if [ ! -d "/usr/local/nginx/conf/autoprotect/${domain}" ]; then
  	mkdir -p "/usr/local/nginx/conf/autoprotect/${domain}"
	fi
	HTPATHS=$($NICE $NICEOPT $IONICE $IONICEOPT find "${TOPLEVEL_DIR}/${domain}" -name ".htaccess" -print0 | xargs -0 echo)
	declare -a arrays
	arrays=(${HTPATHS})
  for d in "${arrays[@]}"; do
    if [[ "$(echo "$d" | grep "${domain}/public/")" ]]; then
      PROTECTDIR=$(dirname "$d");
      # derive the location block path
      if [ "$PROTECTDIR" == "/home/nginx/domains/$domain/public" ]; then
          PROTECTDIR_PATH="/"
      else
          PROTECTDIR_PATH=$(echo "$PROTECTDIR" | sed -e "s|/home/nginx/domains/${domain}/public||")
      fi
      # get the web url path for the .htaccess file
      URL_WEB=$(echo "$PROTECTDIR" |sed -e 's|\/home\/nginx\/domains\/||' -e 's|\/public||')
      # check if 'Deny from all' exists in .htaccess and only generate nginx deny all location
      # matches if .htaccess file has 'Deny from all' but also give end user option to bypass
      # autoprotect.sh script and NOT create a nginx deny all location match by manually creating
      # a .autoprotect-bypass file within the directory you want to bypass and exclude from autoprotect.sh
      if [ -f "${PROTECTDIR}/.htaccess" ]; then
        if check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -v "\#" | grep -i 'Deny from all' >/dev/null 2>&1; echo $?)" = '0' && ! -f "${PROTECTDIR}/.autoprotect-bypass" ]]; then
          # check the web url for .htaccess directory path to see if it's already deny all / 403 protected
          # only generate nginx deny all rules for .htaccess directory paths which are not already returning
          # 403 permission denied http status codes (also include check for 404)
          # 
          # if [[ "$(curl -sI ${URL_WEB}/ | grep 'HTTP\/' | grep -Eo '403|404' >/dev/null 2>&1; echo $?)" != '0' ]]; then
            if check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'akismet' )" ]]; then
              # proper akismet secure lock down
echo -e "# $PROTECTDIR\n
location $PROTECTDIR_PATH/ {
  location ~ ^$PROTECTDIR_PATH/(.+/)?(form|akismet)\.(css|js)\$ { allow all; expires 30d;}
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(png|gif)\$ { allow all; expires 30d;}
  location ~* $PROTECTDIR_PATH/.*\.php\$ {
    include /usr/local/nginx/conf/php.conf;
    allow 127.0.0.1;
    deny all;
  }
}
"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'library' )" && -d "$(echo "$(dirname $PROTECTDIR)/styles/default/xenforo")" ]]; then
echo -e "# Xenforo bypass $PROTECTDIR\n"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'internal_data' )" && -d "$(echo "$(dirname $PROTECTDIR)/styles/default/xenforo")" ]]; then
echo -e "# Xenforo bypass $PROTECTDIR\n"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'install\/templates' )" && -d "$(echo "$(dirname $(dirname $PROTECTDIR))/styles/default/xenforo")" ]]; then
echo -e "# Xenforo bypass $PROTECTDIR\n"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'install\/data' )" && -d "$(echo "$(dirname $(dirname $PROTECTDIR))/styles/default/xenforo")" ]]; then
echo -e "# Xenforo bypass $PROTECTDIR\n"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'src' )" && -d "$(echo "$(dirname $PROTECTDIR)/styles/default/xenforo")" ]]; then
echo -e "# Xenforo bypass $PROTECTDIR\n"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'wp-content\/uploads' )" && -d "$(echo "$(dirname $(dirname $PROTECTDIR))/wp-content/uploads")" && -f "/usr/local/nginx/conf/wpincludes/${domain}/wpsecure_${domain}.conf" ]]; then
echo -e "# centmin.sh menu option 22 installed WP bypass $PROTECTDIR\n"

            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(echo $PROTECTDIR_PATH | grep 'sucuri-scanner' )" ]]; then
              # proper sucuri-scanner secure lock down
echo -e "# $PROTECTDIR\n
location $PROTECTDIR_PATH/ {
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(gif|jpe?g|png|css|js)\$ { allow all; expires 30d; }
  allow 127.0.0.1;
  deny all;
}
"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -E 'ipb-protection|Content-Disposition attachment' | grep 'Header set')" ]]; then
echo -e "# https://community.centminmod.com/posts/33989/\n# $PROTECTDIR\n
location $PROTECTDIR_PATH/ {
  location ~ ^$PROTECTDIR_PATH/(.*)\.(php|cgi|pl|php3|php4|php5|php6|phtml|shtml)\$ { allow 127.0.0.1; deny all; }
  location ~ ^$PROTECTDIR_PATH/(.*)\.(ipb)\$ { add_header 'Content-Disposition' "attachment"; }
}
"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -E 'ipb-protection')" ]]; then
echo -e "# https://community.centminmod.com/posts/35382/\n# $PROTECTDIR\n
location $PROTECTDIR_PATH/ {
  location ~ ^$PROTECTDIR_PATH/(.*)\.(php|cgi|pl|php3|php4|php5|php6|phtml|shtml)\$ { allow 127.0.0.1; deny all; }
}
"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -iv 'Order allow' | grep 'allow')" ]]; then
echo -e "# https://community.centminmod.com/posts/35394/\n# $PROTECTDIR\n
location $PROTECTDIR_PATH/ {
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(js)\$ { allow all; expires 30d; }
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(css)\$ { allow all; expires 30d; }
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(gif|jpe?g|png|webp|eot|svg|ttf|woff|woff)\$ { allow all; expires 30d; }
  location ~ ^$PROTECTDIR_PATH/(.+/)?(.+)\.(php|cgi|pl|php3|php4|php5|php6|phtml|shtml)\$ { allow 127.0.0.1; deny all; }
}
"
            elif check_location_block "$domain" "$PROTECTDIR_PATH" && [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -E -i 'cloudflare')" ]]; then
              echo -e "# Cloudflare bypass $PROTECTDIR\n"
            else
              echo -e "# $PROTECTDIR\nlocation ~* ^$PROTECTDIR_PATH/ { allow 127.0.0.1; deny all; }"
            fi
          # fi
        fi
      fi
    fi # grep public only
  done > "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf.tmp"
    if [ -f "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf" ]; then
      diff -u "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf" "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf.tmp" >/dev/null 2>&1
      DIFFCHECK=$?
      if [[ "$DIFFCHECK" != '0' ]]; then
        if [ -f /tmp/diffcheck.txt ]; then
          touch /tmp/diffcheck.txt
        fi
        echo "y" >> /tmp/diffcheck.txt
        mv -f "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf.tmp" "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
        echo "generated nginx include file [diff]: /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
      else
        mv -f "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf.tmp" "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
        echo "generated nginx include file [same]: /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
      fi
    else
      if [ -f /tmp/diffcheck.txt ]; then
        touch /tmp/diffcheck.txt
      fi
      echo "y" >> /tmp/diffcheck.txt
      mv -f "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf.tmp" "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
      echo "generated nginx include file [initial]: /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
    fi
done

if [[ "$DEBUG" = [yY] ]]; then
  echo
  ls -lahR /usr/local/nginx/conf/autoprotect/
  echo
else
  echo
  if [[ "$DIFFCHECK" != '0' ]]; then
    echo "autoprotect.sh run completed..."
  else
    echo "autoprotect.sh run completed skipped nginx reload..."
  fi
  echo
fi

# only trigger nginx reload service when there are new differences detected
# in current and previous autoprotect include conf files
NGX_RESTARTCMD=$(grep y /tmp/diffcheck.txt >/dev/null 2>&1; echo $?)
if [[ "$NGX_RESTARTCMD" = '0' ]]; then
   sleep 2
   systemctl reload nginx
   rm -rf /usr/local/nginx/conf/autoprotect/status-restart
fi
  if [ -f /tmp/diffcheck.txt ]; then
    rm -rf /tmp/diffcheck.txt
  fi
}

genprotect