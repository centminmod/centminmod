wpinstall() {
    if [ ! -f /usr/bin/wp ]; then
      cecho "------------------------------------------------------------" $boldgreen
      cecho "Installing wpcli.sh" $boldyellow
      cecho "------------------------------------------------------------" $boldgreen
      if [ -d "${SCRIPT_DIR}/addons" ]; then
        cd ${SCRIPT_DIR}/addons      
      fi
      chmod +x wpcli.sh
      ./wpcli.sh install
    else
      cecho "------------------------------------------------------------" $boldgreen
      cecho "Update wp-cli tool" $boldyellow
      cecho "------------------------------------------------------------" $boldgreen
      if [ -d "${SCRIPT_DIR}/addons" ]; then
        cd ${SCRIPT_DIR}/addons      
      fi
      chmod +x wpcli.sh
      ./wpcli.sh update
    fi
}

dbsetup() {
  SALT=$(openssl rand -base64 12 | cut -c1-12 | sed -e s'|/||')
  DBN=$RANDOM
  DBNB=$RANDOM
  DBNC=$RANDOM
  DBND=$RANDOM
  DB="wp${DBN}db_${DBND}"
  DBUSER="wpdb${DBND}u${DBNB}"
  DBPASS="wpdb${SALT}p${DBNC}"
  mysqladmin create $DB
  mysql -e "CREATE USER $DBUSER@'localhost' IDENTIFIED BY '$DBPASS';"
  mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES ON ${DB}.* TO ${DBUSER}@'localhost'; FLUSH PRIVILEGES;"
}

sslvhost() {

cecho "---------------------------------------------------------------" $boldyellow
cecho "SSL Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo ""

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
  HTTPTWO=y
  LISTENOPT='ssl http2'
  COMP_HEADER='#spdy_headers_comp 5'
  SPDY_HEADER='#add_header Alternate-Protocol  443:npn-spdy/3;'
else
  HTTPTWO=n
  LISTENOPT='ssl spdy'
  COMP_HEADER='spdy_headers_comp 5'
  SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
fi

if [ ! -f /usr/local/nginx/conf/ssl ]; then
  mkdir -p /usr/local/nginx/conf/ssl
fi

if [ ! -f /usr/local/nginx/conf/ssl/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/${vhostname}
fi

if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;  
EVS
fi

cd /usr/local/nginx/conf/ssl/${vhostname}

cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating self signed SSL certificate..." $boldgreen
cecho "CSR file can also be used to be submitted for paid SSL certificates" $boldgreen
cecho "If using for paid SSL certificates be sure to keep both private key and CSR safe" $boldgreen
cecho "creating CSR File: ${vhostname}.csr" $boldgreen
cecho "creating private key: ${vhostname}.key" $boldgreen
cecho "creating self-signed SSL certificate: ${vhostname}.crt" $boldgreen
sleep 9

if [[ -z "$SELFSIGNEDSSL_O" ]]; then
  SELFSIGNEDSSL_O="$vhostname"
else
  SELFSIGNEDSSL_O="$SELFSIGNEDSSL_O"
fi

if [[ -z "$SELFSIGNEDSSL_OU" ]]; then
  SELFSIGNEDSSL_OU="$vhostname"
else
  SELFSIGNEDSSL_OU="$SELFSIGNEDSSL_OU"
fi

openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}.csr -keyout ${vhostname}.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"
openssl x509 -req -days 36500 -sha256 -in ${vhostname}.csr -signkey ${vhostname}.key -out ${vhostname}.crt

echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating backup CSR and private key for HTTP Public Key Pinning..." $boldgreen
cecho "creating CSR File: ${vhostname}-backup.csr" $boldgreen
cecho "creating private key: ${vhostname}-backup.key" $boldgreen
sleep 5

openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}-backup.csr -keyout ${vhostname}-backup.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"

echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Extracting Base64 encoded information for primary and secondary" $boldgreen
cecho "private key's SPKI - Subject Public Key Information" $boldgreen
cecho "Primary private key - ${vhostname}.key" $boldgreen
cecho "Backup private key - ${vhostname}-backup.key" $boldgreen
cecho "For HPKP - HTTP Public Key Pinning hash generation..." $boldgreen
sleep 5

echo
cecho "extracting SPKI Base64 encoded hash for primary private key = ${vhostname}.key ..." $boldgreen

openssl rsa -in ${vhostname}.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt

echo
cecho "extracting SPKI Base64 encoded hash for backup private key = ${vhostname}-backup.key ..." $boldgreen

openssl rsa -in ${vhostname}-backup.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt

echo
cecho "HTTP Public Key Pinning Header for Nginx" $boldgreen

echo
cecho "for 7 days max-age including subdomains" $boldgreen
echo
echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400; includeSubDomains';"

echo
cecho "for 7 days max-age excluding subdomains" $boldgreen
echo
echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400';"


echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen

dhparamstarttime=$(date +%s.%N)

openssl dhparam -out dhparam.pem 2048

dhparamendtime=$(date +%s.%N)
DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow

}

wpacctsetup() {
PUREUSER=nginx
PUREGROUP=nginx
CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
pureftpinstall

# Support secondary dedicated IP configuration for centmin mod
# nginx vhost generator, so out of the box, new nginx vhosts 
# generated will use the defined SECOND_IP=111.222.333.444 where
# the IP is a secondary IP addressed added to the server.
# You define SECOND_IP variable is centmin mod persistent config
# file outlined at http://centminmod.com/upgrade.html#persistent
# you manually creat the file at /etc/centminmod/custom_config.inc
# and add SECOND_IP=yoursecondary_IPaddress variable to it which
# will be registered with nginx vhost generator routine so that 
# any new nginx vhosts created via centmin.sh menu option 2 or
# /usr/bin/nv or centmin.sh menu option 22, will have pre-defined
# SECOND_IP ip address set in the nginx vhost's listen directive
if [[ -z "$SECOND_IP" ]]; then
  DEDI_IP=""
  DEDI_LISTEN=""
elif [[ "$SECOND_IP" ]]; then
  DEDI_IP=$(echo $(echo ${SECOND_IP}:))
  DEDI_LISTEN="listen   ${DEDI_IP}80;"
fi

wpinstall
  WPSALT=$(openssl rand -base64 18 | cut -c1-18 | sed -e 's|/||' -e 's|+||')
  WPSALTB=$(openssl rand -base64 11 | cut -c1-11 | sed -e 's|/||' -e 's|+||')
  WPN=$RANDOM
  WPNB=$RANDOM
  WPADMINUSER="z${WPSALT}wp${WPNB}"
  WPADMINUSER=$(echo $WPADMINUSER | sed -e 's|\/||g')
  WPADMINPASS="z${WPSALTB}wps${WPN}"
  WPADMINPASS=$(echo $WPADMINPASS | sed -e 's|\/||g')
 
if [ ! -d /root/tools ]; then
  mkdir -p /root/tools
fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Setup full Nginx vhost + Wordpress + WP Super Cache" $boldgreen
cecho "-------------------------------------------------------------" $boldyellow
echo

read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

TESTVHOST=$(echo $vhostname | grep '\/')
while [[ "$TESTVHOST" ]]; do
  echo "!! only domain.com or subdomain.domain.com supported !!"
  echo "   subdirectory is not supported right now"
  read -ep "re-enter vhost domain name you want to add (without www. prefix): " vhostname
  TESTVHOST=$(echo $vhostname | grep '\/')
  echo
done


if [[ "$NGINX_VHOSTSSL" = [yY] ]]; then
  echo
  read -ep "Create a self-signed SSL certificate Nginx vhost? [y/n]: " vhostssl
  echo
fi

read -ep "Do you want to install WP Cache Enabler Plugin instead of WP Super Cache ? [y/n]: " cacheenabler

read -ep "Enter email address for Wordpress Installation: " WPADMINEMAIL

TESTEMAIL=$(echo "${WPADMINEMAIL}" |  grep '^[a-zA-Z0-9._%+-]*@[a-zA-Z0-9-]*[\.[a-zA-Z0-9]*]*[a-zA-Z0-9]$')
# echo "$TESTEMAIL"
while [[ "$TESTEMAIL" = "" ]]; do
  echo
  echo "!! make sure email address is valid and typed correctly !!"
  read -ep "Enter email address for Wordpress Installation: " WPADMINEMAIL
  TESTEMAIL=$(echo "${WPADMINEMAIL}" |  grep '^[a-zA-Z0-9._%+-]*@[a-zA-Z0-9-]*[\.[a-zA-Z0-9]*]*[a-zA-Z0-9]$')
  echo
done

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  if [ ! -f /usr/sbin/cracklib-check ]; then
    yum -y -q install cracklib
  fi
  if [ ! -f /usr/bin/pwgen ]; then
    yum -y -q install pwgen
  fi  
  read -ep "Create FTP username for vhost domain (enter username): " ftpuser
  read -ep "Do you want to auto generate FTP password (recommended) [y/n]: " autogenpass

  if [[ "$autogenpass" = [yY] ]]; then
    ftppass=$(pwgen -1cnys 21)
  else
    read -ep "Create FTP password for $ftpuser (enter password): " ftppass
  
    # simple password strength check
    # utilise http://cracklib.sourceforge.net/ too
    CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
    okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    while [[ "$okay" != "OK" ]]; do
      echo "!! password strength not strong enough !! "
      echo "!! do not use common dictionary words !! "
      echo "!! do not use short passwords !! "
      echo "!! do not use simplistic passwords !! "
      echo
      read -ep "re-enter FTP password for $ftpuser (enter password): " ftppass
      CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
      okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    done
  fi # autogenpass
  echo
  echo "FTP username you entered: $ftpuser"
  if [[ "$autogenpass" = [yY] ]]; then
    echo "FTP password auto generated: $ftppass"
  else
    echo "FTP password you entered: $ftppass"    
  fi
fi

echo ""

if [ ! -d /home/nginx/domains/$vhostname ]; then

dbsetup

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo ${ftppass} ; echo ${ftppass} ) | pure-pw useradd $ftpuser -u $PUREUSER -g $PUREGROUP -d /home/nginx/domains/$vhostname
  pure-pw mkdb
fi

# where index.html was

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/$vhostname/public
umask 022
chown -R nginx:nginx "/home/nginx/domains/$vhostname"
find "/home/nginx/domains/$vhostname" -type d -exec chmod g+s {} \;

# wp-login.php password protection
if [[ -f /usr/local/nginx/conf/htpasswd.sh && ! -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
  HTWPLOGINSALT=$(openssl rand -base64 13 | cut -c1-13)
  HTWPLOGINSALTB=$(openssl rand -base64 19 | cut -c1-19)
  HTWPLOGIN=$RANDOM
  HTWPLOGINB=$RANDOM
  HTUSER="u${HTWPLOGINSALT}x${HTWPLOGIN}"
  HTUSER=$(echo $HTUSER | sed -e 's|\/||g')
  HTPASS="p${HTWPLOGINSALTB}y${HTWPLOGIN}"
  HTPASS=$(echo $HTPASS | sed -e 's|\/||g')
  echo "/usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS"
  /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS
fi

# rate limit setup
WPRATECHECK=$(grep 'zone=xwplogin' /usr/local/nginx/conf/nginx.conf)
WPRATERPCCHECK=$(grep 'zone=xwprpc' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$WPRATERPCCHECK" ]]; then
  sed -i 's/http {/http { \nlimit_req_zone $binary_remote_addr zone=xwprpc:10m rate=30r\/s;\n/g' /usr/local/nginx/conf/nginx.conf
fi

if [[ -z "$WPRATECHECK" ]]; then
  sed -i 's/http {/http { \nlimit_req_zone $binary_remote_addr zone=xwplogin:10m rate=40r\/m;\n/g' /usr/local/nginx/conf/nginx.conf
fi

################################################################################
# create wp super cache's included php config file php-wpsc.conf
\cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php-wpsc.conf
sed -i "s|fastcgi_param  SERVER_NAME        \$server_name;|fastcgi_param  SERVER_NAME        \$http_host;|" /usr/local/nginx/conf/php-wpsc.conf

################################################################################
# create wp enable enabler included files https://community.centminmod.com/posts/21220/

cat > "/usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf"<<HFA
    set \$cache_uri \$request_uri;

    # bypass cache if POST requests or URLs with a query string
    if (\$request_method = POST) {
        set \$cache_uri 'nullcache';
    }
    if (\$query_string != "") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache if URLs containing the following strings
    if (\$request_uri ~* "(/wp-admin/|/xmlrpc.php|/wp-(app|cron|login|register|mail).php|wp-.*.php|/feed/|index.php|wp-comments-popup.php|wp-links-opml.php|wp-locations.php|sitemap(_index)?.xml|[a-z0-9_-]+-sitemap([0-9]+)?.xml)") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache if the cookies containing the following strings
    if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in") {
        set \$cache_uri 'nullcache';
    }

    # custom sub directory e.g. /blog
    set \$custom_subdir '';

    # default html file
    set \$cache_enabler_uri '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}index.html';

    # webp html file
    if (\$http_accept ~* "image/webp") {
        set \$cache_enabler_uri '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}index-webp.html';
    }
HFA

################################################################################
# create nginx level redis cache included php config file php-rediscache.conf
# https://community.centminmod.com/posts/18828/
# \cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php-rediscache.conf

cat > "/usr/local/nginx/conf/php-rediscache.conf"<<HFF
location ~ \.php\$ {
    set \$key "nginx-cache:\$scheme\$request_method\$host\$request_uri";
    srcache_fetch_skip \$skip_cache;
    srcache_store_skip \$skip_cache;
    srcache_response_cache_control off;
    set_escape_uri \$escaped_key \$key;
    srcache_fetch GET /redis-fetch \$key;
    srcache_store PUT /redis-store key=\$escaped_key;
    more_set_headers 'X-Cache \$srcache_fetch_status';
    more_set_headers 'X-Cache-2 \$srcache_store_status';

    try_files \$uri =404;
    fastcgi_split_path_info ^(.+\.php)(/.+)\$;
    fastcgi_pass   127.0.0.1:9000;
    #fastcgi_pass   unix:/tmp/php5-fpm.sock;
    fastcgi_index  index.php;
    #fastcgi_param  SCRIPT_FILENAME  \$document_root\$fastcgi_script_name;
    fastcgi_param  SCRIPT_FILENAME    \$request_filename;
    fastcgi_param PHP_ADMIN_VALUE open_basedir=\$document_root/:/usr/local/lib/php/:/tmp/;

# might shave 200+ ms off PHP requests
# which don't pass on a content length header
# slightly faster page response time at the
# expense of throughput / scalability
#sendfile on;
#tcp_nopush off;
#keepalive_requests 0;

fastcgi_connect_timeout 60;
fastcgi_send_timeout 180;
fastcgi_read_timeout 180;
fastcgi_buffer_size 512k;
fastcgi_buffers 512 16k;
fastcgi_busy_buffers_size 1m;
fastcgi_temp_file_write_size 4m;
fastcgi_max_temp_file_size 4m;
fastcgi_intercept_errors on;

# next 3 lines when uncommented / enabled
# allow Nginx to handle uploads which then 
# passes back the completed upload to PHP
#fastcgi_pass_request_body off;
#client_body_in_file_only clean;
#fastcgi_param  REQUEST_BODY_FILE  \$request_body_file;

#new .04+ map method
fastcgi_param HTTPS \$server_https;

# comment out PATH_TRANSLATED line if /usr/local/lib/php.ini sets following:
# cgi.fix_pathinfo=0 
# as of centminmod v1.2.3-eva2000.01 default is set to cgi.fix_pathinfo=1

fastcgi_param  PATH_INFO          \$fastcgi_path_info;
fastcgi_param  PATH_TRANSLATED    \$document_root\$fastcgi_path_info;

fastcgi_param  QUERY_STRING       \$query_string;
fastcgi_param  REQUEST_METHOD     \$request_method;
fastcgi_param  CONTENT_TYPE       \$content_type;
fastcgi_param  CONTENT_LENGTH     \$content_length;

fastcgi_param  SCRIPT_NAME        \$fastcgi_script_name;
fastcgi_param  REQUEST_URI        \$request_uri;
fastcgi_param  DOCUMENT_URI       \$document_uri;
fastcgi_param  DOCUMENT_ROOT      \$document_root;
fastcgi_param  SERVER_PROTOCOL    \$server_protocol;
fastcgi_param  REQUEST_SCHEME     \$scheme;
fastcgi_param  HTTPS              \$https if_not_empty;

fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
fastcgi_param  SERVER_SOFTWARE    nginx/\$nginx_version;

fastcgi_param  REMOTE_ADDR        \$remote_addr;
fastcgi_param  REMOTE_PORT        \$remote_port;
fastcgi_param  SERVER_ADDR        \$server_addr;
fastcgi_param  SERVER_PORT        \$server_port;
fastcgi_param  SERVER_NAME        \$server_name;

# Set php-fpm geoip variables
fastcgi_param GEOIP_COUNTRY_CODE \$geoip_country_code;
fastcgi_param GEOIP_COUNTRY_CODE3 \$geoip_country_code3;
fastcgi_param GEOIP_COUNTRY_NAME \$geoip_country_name;
fastcgi_param GEOIP_CITY_COUNTRY_CODE \$geoip_city_country_code;
fastcgi_param GEOIP_CITY_COUNTRY_CODE3 \$geoip_city_country_code3;
fastcgi_param GEOIP_CITY_COUNTRY_NAME \$geoip_city_country_name;
fastcgi_param GEOIP_REGION \$geoip_region;
fastcgi_param GEOIP_CITY \$geoip_city;
fastcgi_param GEOIP_POSTAL_CODE \$geoip_postal_code;
fastcgi_param GEOIP_CITY_CONTINENT_CODE \$geoip_city_continent_code;
fastcgi_param GEOIP_LATITUDE \$geoip_latitude;
fastcgi_param GEOIP_LONGITUDE \$geoip_longitude;

# PHP only, required if PHP was built with --enable-force-cgi-redirect
fastcgi_param  REDIRECT_STATUS    200;

                   }
HFF


################################################################################
# Setting up Nginx mapping

if [[ "$vhostssl" = [yY] ]]; then
  sslvhost
fi

if [[ "$vhostssl" = [yY] ]]; then

if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]]; then
  CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
else
  CHACHACIPHERS=""
fi

# main non-ssl vhost at yourdomain.com.conf for Wordpress
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<ENSS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   ${DEDI_IP}80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
  $DEDI_LISTEN
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  root /home/nginx/domains/$vhostname/public;

  # prevent access to ./directories and files
  location ~ (?:^|/)\. {
   deny all;
  }

#include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf;
include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;
# https://community.centminmod.com/posts/18828/
#include /usr/local/nginx/conf/rediscache_${vhostname}.conf;  

  location / {

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;

  # Wordpress Permalinks
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args;  

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ /index.php?\$args;

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
ENSS

# separate ssl vhost at yourdomain.com.ssl.conf
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For SPDY SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
# server {
#   server_name ${vhostname} www.${vhostname};
#    return 302 https://\$server_name\$request_uri;
# }

server {
  listen ${DEDI_IP}443 $LISTENOPT;
  server_name $vhostname www.$vhostname;

  ssl_dhparam /usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem;
  ssl_certificate      /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt;
  ssl_certificate_key  /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key;
  include /usr/local/nginx/conf/ssl_include.conf;

  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA:!DES-CBC3-SHA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER
  # HTTP Public Key Pinning Header uncomment only one that applies include or exclude domains. 
  # You'd want to include subdomains if you're using SSL wildcard certificates
  # include subdomain
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400; includeSubDomains';
  # exclude subdomains
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400';
  #add_header Strict-Transport-Security "max-age=31536000; includeSubdomains;";
  #add_header  X-Content-Type-Options "nosniff";
  #add_header X-Frame-Options DENY;
  $COMP_HEADER;
  ssl_buffer_size 1400;
  ssl_session_tickets on;
  
  # enable ocsp stapling
  #resolver 8.8.8.8 8.8.4.4 valid=10m;
  #resolver_timeout 10s;
  #ssl_stapling on;
  #ssl_stapling_verify on;
  #ssl_trusted_certificate /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-trusted.crt;  

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  root /home/nginx/domains/$vhostname/public;

  # prevent access to ./directories and files
  location ~ (?:^|/)\. {
   deny all;
  }

#include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf;
include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;
# https://community.centminmod.com/posts/18828/
#include /usr/local/nginx/conf/rediscache_${vhostname}.conf;  

  location / {

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;  

  # Wordpress Permalinks
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args; 

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ /index.php?\$args;

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
ESS

else

cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<END
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   ${DEDI_IP}80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
  $DEDI_LISTEN
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  root /home/nginx/domains/$vhostname/public;

  #include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf;
  include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/rediscache_${vhostname}.conf;  

  location / {

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;   

  # Wordpress Permalinks
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args; 

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ /index.php?\$args;

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
END

fi

cat > "/usr/local/nginx/conf/wpsecure_${vhostname}.conf" <<EEF
# Deny access to any files with a .php extension in the uploads directory
# Works in sub-directory installs and also in multisite network
location ~* /(?:uploads|files)/.*\.php\$ {
deny all;
}

# Block PHP files in content directory.
location ~* /wp-content/.*\.php\$ {
  deny all;
}

# Block PHP files in includes directory.
location ~* /wp-includes/.*\.php\$ {
  deny all;
}

# Block PHP files in uploads, content, and includes directory.
location ~* /(?:uploads|files|wp-content|wp-includes)/.*\.php\$ {
  deny all;
}

# Make sure files with the following extensions do not get loaded by nginx because nginx would display the source code, and these files can contain PASSWORDS!
location ~* \.(engine|inc|info|install|make|module|profile|test|po|sh|.*sql|theme|tpl(\.php)?|xtmpl)\$|^(\..*|Entries.*|Repository|Root|Tag|Template)\$|\.php_
{
return 444;
}

#nocgi
location ~* \.(pl|cgi|py|sh|lua)\$ {
return 444;
}

#disallow
location ~* (w00tw00t) {
return 444;
}

location ~ /(\.|wp-config\.php|wp-config\.txt|readme\.html|license\.txt) { deny all; }
EEF

# WP super cache
cat > "/usr/local/nginx/conf/wpsupercache_${vhostname}.conf" <<EFF
set \$cache_uri \$request_uri;

if (\$request_method = POST) { set \$cache_uri 'null cache'; }

if (\$query_string != "") { set \$cache_uri 'null cache'; }

if (\$request_uri ~* "/(\?add-to-cart=|cart|my-account|checkout|addons|wp-admin/.*|xmlrpc\.php|wp-.*\.php|index\.php|feed/|sitemap(_index)?\.xml|[a-z0-9_-]+-sitemap([0-9]+)?\.xml)") { set \$cache_uri 'null cache'; }

if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in") { set \$cache_uri 'null cache'; }
EFF

# for nginx level redis cache
cat > "/usr/local/nginx/conf/rediscache_${vhostname}.conf" <<XFF
set \$skip_cache 0;

# POST requests and urls with a query string should always go to PHP
if (\$request_method = POST) {
  set \$skip_cache 1;
}

if (\$query_string != "") {
  set \$skip_cache 1;
}

# Don't cache uris containing the following segments
if (\$request_uri ~* "/wp-admin/|/xmlrpc.php|wp-.*.php|/feed/|index.php|sitemap(_index)?.xml") {
  set \$skip_cache 1;
}

# Don't use the cache for logged in users or recent commenters
if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_no_cache|wordpress_logged_in") {
  set \$skip_cache 1;
}

location /redis-fetch {
  internal  ;
  set  \$redis_key \$args;
  redis_pass  redisbackend;
  redis_connect_timeout 60000;
  redis_read_timeout 60000;
  redis_send_timeout 60000;
}

location /redis-store {
  internal  ;
  set_unescape_uri \$key \$arg_key ;
  redis2_query set \$key \$echo_request_body;
  redis2_query expire \$key 6h;
  redis2_pass  redisbackend;
  redis2_connect_timeout 60s;
  redis2_read_timeout 60s;
  redis2_send_timeout 60s;
}
XFF

cat > "/usr/local/nginx/conf/redisupstream.conf" <<GGG
upstream redisbackend {
  zone upstream_redis 64k;
  server 127.0.0.1:6379 weight=1 max_fails=3 fail_timeout=30s;
  #server 127.0.0.1:6380 weight=1 max_fails=3 fail_timeout=30s;
  #server 127.0.0.1:6381 weight=1 max_fails=3 fail_timeout=30s;
  #server 127.0.0.1:6382 weight=1 max_fails=3 fail_timeout=30s;
  #server 127.0.0.1:6383 weight=1 max_fails=3 fail_timeout=30s;
  #server 127.0.0.1:6384 weight=1 max_fails=3 fail_timeout=30s;

  #server 127.0.0.1:6380 backup;
  keepalive 4096;
}
GGG

REDISUPSTREAM_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/redisupstream.conf' /usr/local/nginx/conf/nginx.conf)
  if [[ -z "$REDISUPSTREAM_INCLUDECHECK" ]]; then
    echo
    echo "include file /usr/local/nginx/conf/redisupstream.conf add to nginx.conf"
      sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/redisupstream.conf;/g' /usr/local/nginx/conf/nginx.conf
  fi

########### WP Super Cache Start ##############################
# only proceed in creating vhost if VHOSTNAME directory exist
if [[ -d "/home/nginx/domains/${vhostname}/public" ]]; then

  cecho "------------------------------------------------------------" $boldgreen
  if [[ "$cacheenabler" = [yY] ]]; then
    cecho "Setup Wordpress + Cache Enabler for $vhostname" $boldyellow
  else
    cecho "Setup Wordpress + Super Cache for $vhostname" $boldyellow
  fi
  cecho "------------------------------------------------------------" $boldgreen

cd /home/nginx/domains/${vhostname}/public
 
wp core download --allow-root
 
wp core config --dbname=$DB --dbuser=$DBUSER --dbpass=$DBPASS --allow-root
 
NEWPREFIX=$(echo $RANDOM)
sed -i "s/'wp_';/'${NEWPREFIX}_';/g" wp-config.php

sed -i "/define('DB_COLLATE', '');/ a\
/** Enable core updates for minor releases (default) **/\ndefine('DISABLE_WP_CRON', true);\ndefine('WP_AUTO_UPDATE_CORE', 'minor' );\ndefine('WP_POST_REVISIONS', 10 );\ndefine('EMPTY_TRASH_DAYS', 10 );\ndefine('WP_CRON_LOCK_TIMEOUT', 60 );\
" wp-config.php

if [[ -z "$(crontab -l 2>&1 | grep '\/${vhostname}/wp-cron.php')" ]]; then
    # generate random number of seconds to delay cron start
    # making sure they do not run at very same time during cron scheduling
    DELAY=$(echo ${RANDOM:0:3})
    crontab -l > cronjoblist
    echo "*/15 * * * * sleep ${DELAY}s ; wget -O - -q -t 1 http://${vhostname}/wp-cron.php?doing_wp_cron=1 > /dev/null 2>&1" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

wp core install --url=http://${vhostname} --title=${vhostname} --admin_email=${WPADMINEMAIL} --admin_password=${WPADMINPASS} --admin_name=${WPADMINUSER} --allow-root

# change admin userid from 1 to a random 6 digit number
# WP_PREFIX=$(wp eval 'echo $GLOBALS["table_prefix"];')
WUID=$(echo $RANDOM$RANDOM |cut -c1-6)
# wp db query "UPDATE ${WP_PREFIX}wp_users SET ID=${WUID} WHERE ID=1; UPDATE ${WP_PREFIX}wp_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root
wp db query "UPDATE wp_users SET ID=${WUID} WHERE ID=1; UPDATE wp_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root

# add index on autoload
wp db query "ALTER TABLE ${NEWPREFIX}_options ADD INDEX autoload_idx('autoload')" --allow-root

# change permalinks out of the box
wp rewrite structure '/%post_id%/%postname%/' --allow-root

if [[ "$WPCLI_SUPERCACHEPLUGIN" = [yY] && ! -f /root/.wp-cli/commands/super-cache/cli.php ]]; then
  # https://github.com/wp-cli/wp-super-cache-cli.git
  mkdir -p /root/.wp-cli/commands
  git clone https://github.com/wp-cli/wp-super-cache-cli.git /root/.wp-cli/commands/super-cache
  echo "require:" > /root/.wp-cli/config.yml
  echo "  - commands/super-cache/cli.php" >> /root/.wp-cli/config.yml
elif [ -f /root/.wp-cli/commands/super-cache/cli.php ]; then
  cd /root/.wp-cli/commands/super-cache
  git stash
  git pull
  cd /home/nginx/domains/${vhostname}/public
fi

cecho "------------------------------------------------------------" $boldgreen
wp theme install responsive --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
 
chown nginx:nginx /home/nginx/domains/${vhostname}/public
chown -R nginx:nginx /home/nginx/domains/${vhostname}/public

cd /home/nginx/domains/${vhostname}/public

chmod 0770 wp-content
chmod 0400 readme.html
rm -rf readme.html

# installed + activated by default
# wp super cache
if [[ "$cacheenabler" != [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-super-cache --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-super-cache-clear-cache-menu --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # wp plugin install wp-widget-cache --activate --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
elif [[ "$cacheenabler" = [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-super-cache --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-super-cache-clear-cache-menu --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # wp plugin install wp-widget-cache --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
fi

# wp cache enabler
if [[ "$cacheenabler" = [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install cache-enabler --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen 
  wp plugin install optimus --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
else
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install cache-enabler --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install optimus --allow-root
  cecho "------------------------------------------------------------" $boldgreen
fi

cecho "------------------------------------------------------------" $boldgreen
wp plugin install sucuri-scanner --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install disable-xml-rpc --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install cdn-enabler --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen

if [[ "$WPPLUGINS_ALL" = [yY] ]]; then
  wp plugin install autoptimize --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install rocket-lazy-load --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-security-scan --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install limit-login-attempts --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-updates-notifier --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install no-longer-in-directory --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # wp plugin install google-sitemap-generator --activate --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-optimize --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # wp plugin install wp-smushit --activate --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
  wp plugin install tpc-memory-usage --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install gtmetrix-for-wordpress --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install p3-profiler --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wordpress-seo --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install updraftplus --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install google-analytics-for-wordpress --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install heartbeat-control --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  
  # installed but disabled by default
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install nginx-helper --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install query-monitor --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install go-newrelic --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install db-cache-reloaded-fix --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install google-authenticator --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install addthis-smart-layers --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install search-regex --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install disable-emojis --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install wp-user-avatar --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  wp plugin install lazy-load-for-videos --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  #wp plugin install disable-xml-rpc-pingback --allow-root
  #cecho "------------------------------------------------------------" $boldgreen
fi # WPPLUGINS_ALL=y

# update all plugins to make sure they are latest available
wp plugin update --all --allow-root
echo
wp plugin status --allow-root
cecho "------------------------------------------------------------" $boldgreen

if [[ -f /root/.wp-cli/commands/super-cache/cli.php && "$cacheenabler" != [yY] ]]; then
  wp super-cache enable --allow-root
fi

chown -R nginx:nginx /home/nginx/domains/${vhostname}/public/wp-content/plugins/

# write permissions for log
if [ -f wp-content/plugins/tpc-memory-usage/logs/tpcmem.log ]; then
  chmod 0660 wp-content/plugins/tpc-memory-usage/logs/tpcmem.log
fi

# fix tcpmem.css incorrect reference to images
if [ -f wp-content/plugins/tpc-memory-usage/css/tpcmem.css ]; then
  sed -i 's|(images\/|(..\/images\/|g' wp-content/plugins/tpc-memory-usage/css/tpcmem.css
fi

if [[ "$cacheenabler" = [yY] ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
        sed -i "s|^#include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf|include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|^include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf|#include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|#try_files \$cache_enabler_uri|try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^#include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf|include /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|^include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf|#include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|#try_files \$cache_enabler_uri|try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
    fi
fi

if [[ "$cacheenabler" != [yY] ]]; then
  if [ -f wp-content/plugins/wp-super-cache/wp-cache-config-sample.php ]; then
    \cp -af wp-content/plugins/wp-super-cache/wp-cache-config-sample.php wp-content/wp-cache-config.php
  fi
  if [ -f wp-content/plugins/wp-super-cache/advanced-cache.php ]; then
    \cp -af wp-content/plugins/wp-super-cache/advanced-cache.php wp-content/advanced-cache.php
  fi
fi
mkdir -p wp-content/cache/
mkdir -p wp-content/cache/supercache/
chown -R nginx:nginx wp-content/
chmod -R 0770 wp-content/cache/
chmod 0750 wp-content
umask 022
fi
########### WP Super Cache End ##############################

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Created uninstall script" $boldyellow
  cecho "/root/tools/wp_uninstall_${vhostname}.sh" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen

cat > "/root/tools/wp_uninstall_${vhostname}.sh" <<END
#/bin/bash
rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.conf
rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
rm -rf /home/nginx/domains/${vhostname}
rm -rf /usr/local/nginx/conf/wpsecure_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpsupercache_${vhostname}.conf
rm -rf /root/tools/wp_updater_${vhostname}.sh
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr
rm -rf /usr/local/nginx/conf/ssl/${vhostname}
rm -rf /usr/local/nginx/conf/rediscache_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpcacheenabler_${vhostname}.conf
crontab -l > cronjoblist
sed -i "/wp_updater_${vhostname}.sh/d" cronjoblist
sed -i "/\/${vhostname}\/wp-cron.php/d" cronjoblist
crontab cronjoblist
rm -rf cronjoblist
service nginx restart
END

chmod 0700 /root/tools/wp_uninstall_${vhostname}.sh

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Created wp_updater_${vhostname}.sh script" $boldyellow
  cecho "/root/tools/wp_updater_${vhostname}.sh" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen

cat > "/root/tools/wp_updater_${vhostname}.sh" <<ENDA
#!/bin/bash
PATH=/usr/lib64/ccache:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin:/root/bin
EMAIL=$WPADMINEMAIL

{
cd /home/nginx/domains/${vhostname}/public
echo "/home/nginx/domains/${vhostname}/public"
#/usr/bin/wp cli update --allow-root
echo "update wp-cli"
rm -rf /usr/bin/wp
wget -cnv --no-check-certificate https://raw.github.com/wp-cli/builds/gh-pages/phar/wp-cli.phar -O /usr/bin/wp --tries=3
chmod 0700 /usr/bin/wp
/usr/bin/wp --info --allow-root
/usr/bin/wp plugin status --allow-root
/usr/bin/wp plugin update --all --allow-root
#/usr/bin/wp core check-update --allow-root
#/usr/bin/wp core update --allow-root
#/usr/bin/wp core update-db --allow-root
/usr/bin/nprestart
} 2>&1 | mail -s "Wordpress WP-CLI Auto Update \$(date)" \$EMAIL
ENDA

chmod 0700 /root/tools/wp_updater_${vhostname}.sh

if [[ -z "$(crontab -l 2>&1 | grep wp_updater_${vhostname}.sh)" ]]; then
    # generate random number of seconds to delay cron start
    # making sure wp_updater for several wordpress nginx installs
    # do not run at very same time during cron scheduling
    DELAY=$(echo ${RANDOM:0:3})
    crontab -l > cronjoblist
    echo "0 */8 * * * sleep ${DELAY}s ;/root/tools/wp_updater_${vhostname}.sh 2>/dev/null" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

echo 
cecho "-------------------------------------------------------------" $boldyellow
cmservice nginx reload
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

if [ -f /tmp/setupwp.log ]; then
  rm -rf /tmp/setupwp.log
fi

{
echo 
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
cecho "-------------------------------------------------------------" $boldyellow
cecho "FTP hostname : $CNIP" $boldwhite
cecho "FTP port : 21" $boldwhite
cecho "FTP mode : FTP (explicit SSL)" $boldwhite
cecho "FTP Passive (PASV) : ensure is checked/enabled" $boldwhite
cecho "FTP username created for $vhostname : $ftpuser" $boldwhite
cecho "FTP password created for $vhostname : $ftppass" $boldwhite
fi
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname created successfully" $boldwhite
echo
cecho "domain: http://$vhostname" $boldyellow
cecho "vhost conf file for $vhostname created: /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
if [[ "$vhostssl" = [yY] ]]; then
  echo
  cecho "vhost ssl for $vhostname created successfully" $boldwhite
  echo
  cecho "domain: https://$vhostname" $boldyellow
  cecho "vhost ssl conf file for $vhostname created: /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" $boldwhite
  cecho "/usr/local/nginx/conf/ssl_include.conf created" $boldwhite
  cecho "Self-signed SSL Certificate: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" $boldyellow
  cecho "SSL Private Key: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key" $boldyellow
  cecho "SSL CSR File: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr" $boldyellow
  cecho "Backup SSL Private Key: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-backup.key" $boldyellow
  cecho "Backup SSL CSR File: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-backup.csr" $boldyellow  
fi
echo
cecho "upload files to /home/nginx/domains/$vhostname/public" $boldwhite
cecho "vhost log files directory is /home/nginx/domains/$vhostname/log" $boldwhite
echo
cecho "------------------------------------------------------------" $boldgreen
cecho "SSH commands to uninstall created Wordpress install and Nginx vhost:" $boldyellow
cecho "  /root/tools/wp_uninstall_${vhostname}.sh" $boldyellow
cecho "------------------------------------------------------------" $boldgreen
echo
cecho "------------------------------------------------------------" $boldgreen
cecho "Wordpress Auto Updater created at:" $boldyellow
cecho "  /root/tools/wp_updater_${vhostname}.sh" $boldyellow
cecho "cronjob set for every 8 hours update (3x times per day)" $boldyellow
cecho "------------------------------------------------------------" $boldgreen
echo
cecho "Wordpress domain: $vhostname" $boldyellow
cecho "Wordpress DB Name: $DB" $boldyellow
cecho "Wordpress DB User: $DBUSER" $boldyellow
cecho "Wordpress DB Pass: $DBPASS" $boldyellow
cecho "Wordpress Admin User ID: ${WUID}" $boldyellow
cecho "Wordpress Admin User: $WPADMINUSER" $boldyellow
cecho "Wordpress Admin Pass: $WPADMINPASS" $boldyellow
cecho "Wordpress Admin Email: $WPADMINEMAIL" $boldyellow

if [[ -f /usr/local/nginx/conf/htpasswd.sh && -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
  echo  
  cecho "Wordpress wp-login.php password protection info:" $boldyellow
  cecho "wp-login.php protection file /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "wp-login.php protection Username: $HTUSER" $boldyellow
  cecho "wp-login.php protection Password: $HTPASS" $boldyellow
  cecho "http://${HTUSER}:${HTPASS}@${vhostname}/wp-login.php" $boldyellow
  echo
  cecho "Resetting wp-login.php protection:" $boldyellow
  cecho "Step 1. remove protection file at /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "     rm -rf /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "Step 2. run command:" $boldyellow
  cecho "     /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin YOURUSERNAME YOURPASSWORD" $boldyellow
  cecho "Step 3. restart Nginx + PHP-FPM services" $boldyellow
  cecho "     nprestart" $boldyellow
fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

if [[ "$vhostssl" = [yY] ]]; then
echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost ssl files listing at: /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/ssl/${vhostname} | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'
fi

echo
cecho "------------------------------------------------------------" $boldgreen
cecho "To complete setup:" $boldyellow
if [[ "$cacheenabler" = [yY] ]]; then
cecho "1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
2. Settings Menu > Cache Enabler set options and hit Save Changes
3. Settings Menu > Optimus set options and hit Save Changes
4. Appearance > Theme Options (Responsive theme) > Home Page nav bar > Uncheck Overrides Wordpress front page option"
if [[ "$WPPLUGINS_ALL" = [yY] ]]; then
cecho "5. WP Security Menu > Settings > Check All except Enable Live Traffic tool and hit Update settings
6. Settings Menu > Updates Notifier and setup your notify email address and cronjob (save and test button to check)
7. Settings Mnenu > Autoptimize and check Optimize HTML, JavaScript and CSS options (show advanced settings)
8. Settings Menu > Limit Login Attempts and configure as desired or leave as defaults
9. Sucuri Security Menu and top left click Generate API key for your domain/email and configure your Settings tab
10. WP-Optimize Menu and configure as needed
11. Memory Usage Menu > Settings and adjust accordingly
12. GTmetrix Menu > setup and register your GTmetrix Account and API Key
13. go-newrelic plugin installed but not activated read https://wordpress.org/plugins/go-newrelic/installation/
14. Tools > P3 Plugin Profiler > Start Scan to profile all your plugins
15. Plugins > Query Monitor is disabled by default, enable to check MySQL query stats
16. Plugins > DB Cache Reloaded disabled by default unsure if works with Wordpress 4.x ?
17. Seo Menu (Yoast SEO) > configure accordingly
18. Settings > UpdraftPlus Backups > Settings set file/database backup intervals & optional backup to remote storage
19. Analytics > Settings > configure your Google Analytics UA Code" $boldyellow
fi
cecho "------------------------------------------------------------" $boldgreen
else
cecho "1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
2. Settings Menu > Super Cache > Easy tab and enable it by checking Caching On (Recommended) and hit Update Status
3. Advanced tab & check Use mod_rewrite serve cache files & Don’t cache pages with GET parameters and Known User. 
   (Recommended), Cache rebuild for anonymous users, clear all cache when a post or page updated & hit Update Status
4. Appearance > Theme Options (Responsive theme) > Home Page nav bar > Uncheck Overrides Wordpress front page option"
if [[ "$WPPLUGINS_ALL" = [yY] ]]; then
cecho "
5. WP Security Menu > Settings > Check All except Enable Live Traffic tool and hit Update settings
6. Settings Menu > Updates Notifier and setup your notify email address and cronjob (save and test button to check)
7. Settings Mnenu > Autoptimize and check Optimize HTML, JavaScript and CSS options (show advanced settings)
8. Settings Menu > Limit Login Attempts and configure as desired or leave as defaults
9. Sucuri Security Menu and top left click Generate API key for your domain/email and configure your Settings tab
10. WP-Optimize Menu and configure as needed
11. Memory Usage Menu > Settings and adjust accordingly
12. GTmetrix Menu > setup and register your GTmetrix Account and API Key
13. go-newrelic plugin installed but not activated read https://wordpress.org/plugins/go-newrelic/installation/
14. Tools > P3 Plugin Profiler > Start Scan to profile all your plugins
15. Plugins > Query Monitor is disabled by default, enable to check MySQL query stats
16. Plugins > DB Cache Reloaded disabled by default unsure if works with Wordpress 4.x ?
17. Seo Menu (Yoast SEO) > configure accordingly
18. Settings > UpdraftPlus Backups > Settings set file/database backup intervals & optional backup to remote storage
19. Analytics > Settings > configure your Google Analytics UA Code" $boldyellow
fi
cecho "------------------------------------------------------------" $boldgreen
fi
} 2>&1 | tee /tmp/setupwp.log
cat /tmp/setupwp.log | perl -pe 's/\x1b.*?[mGKH]//g' | mail -s "${vhostname} Wordpress Installed `date`" $WPADMINEMAIL
rm -rf /tmp/setupwp.log

echo
cecho "-------------------------------------------------------------" $boldyellow

else

echo ""
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""

fi

}