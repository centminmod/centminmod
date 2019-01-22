configure_autoptimize() {
  # echo "configure autoptimize"
  # \wp option update autoptimize_cache_clean 0 --allow-root
  # \wp option update autoptimize_cdn_url "" --allow-root
  # \wp option update autoptimize_css on --allow-root
  # \wp option update autoptimize_css_aggregate on --allow-root
  # \wp option update autoptimize_css_datauris "" --allow-root
  # \wp option update autoptimize_css_defer "" --allow-root
  # \wp option update autoptimize_css_defer_inline "" --allow-root
  # \wp option update autoptimize_css_exclude "wp-content/cache/, wp-content/uploads/, admin-bar.min.css, dashicons.min.css" --allow-root
  # \wp option update autoptimize_css_include_inline  on --allow-root
  # \wp option update autoptimize_css_inline "" --allow-root
  # \wp option update autoptimize_css_justhead "" --allow-root
  # \wp option update autoptimize_html on --allow-root
  # \wp option update autoptimize_html_keepcomments on --allow-root
  # \wp option update autoptimize_imgopt_launched     on --allow-root
  # \wp option update autoptimize_js on --allow-root
  # \wp option update autoptimize_js_aggregate on --allow-root
  # \wp option update autoptimize_js_exclude  "seal.js, js/jquery/jquery.js" --allow-root
  # \wp option update autoptimize_js_forcehead "" --allow-root
  # \wp option update autoptimize_js_include_inline "" --allow-root
  # \wp option update autoptimize_js_justhead "" --allow-root
  # \wp option update autoptimize_js_trycatch "" --allow-root
  # \wp option update autoptimize_optimize_checkout "" --allow-root
  # \wp option update autoptimize_optimize_logged ""  --allow-root
  # # \wp option update autoptimize_optimize_logged on --allow-root
  # \wp option update autoptimize_show_adv 1 --allow-root
  \wp option list --search=autoptimize_* --allow-root
}

fc_wpinstall() {
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

fc_installredisserver() {
  if [[ "$(ps -C redis-server | grep redis-server >/dev/null 2>&1; echo $?)" != '0' ]]; then
    echo
    echo "Install & Setup Redis Server from REMI YUM Repo"
    echo
    yum -y install redis --enablerepo=remi --disableplugin=priorities
    chkconfig redis on
    if [[ -z "$(grep '^vm.overcommit_memory' /etc/sysctl.conf)" ]]; then
      echo "vm.overcommit_memory = 1" >> /etc/sysctl.conf
      sysctl -p
    fi
    if [ -f /etc/redis.conf ]; then
      if [[ -z "$(grep '^maxmemory 111mb' /etc/redis.conf)" ]]; then
        echo "maxmemory 111mb" >> /etc/redis.conf
        grep '^maxmemory 111mb' /etc/redis.conf
        fi
      if [[ -z "$(grep '^maxmemory-policy allkeys-lru' /etc/redis.conf)" ]]; then
        echo "maxmemory-policy allkeys-lru" >> /etc/redis.conf
        grep '^maxmemory-policy allkeys-lru' /etc/redis.conf
      fi
      if [[ -z "$(grep '^maxmemory-samples 10' /etc/redis.conf)" ]]; then
        echo "maxmemory-samples 10" >> /etc/redis.conf
        grep '^maxmemory-samples 10' /etc/redis.conf
      fi
    fi
    service redis restart
    echo
    echo "Redis server installed with config file at /etc/redis.conf"
    echo
  fi
}

fc_dbsetup() {
  SALT=$(openssl rand 12 -base64 | tr -dc 'a-zA-Z0-9')
  DBN=$RANDOM
  DBNB=$RANDOM
  DBNC=$RANDOM
  DBND=$RANDOM
  DBNE=$RANDOM
  DB="wp${DBNE}${DBN}db_${DBND}"
  DBUSER="wpdb${DBND}u${DBNB}"
  DBPASS="wpdb${SALT}p${DBNC}"
  mysqladmin create $DB
  mysql -e "CREATE USER $DBUSER@'localhost' IDENTIFIED BY '$DBPASS';"
  mysql -e "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON ${DB}.* TO ${DBUSER}@'localhost'; FLUSH PRIVILEGES;"
}

fc_wpinfo_notice() {
cecho "---------------------------------------------------------------" $boldyellow
cecho "Important Information" $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo
echo "You are about to create an Wordpress based Nginx vhost site with"
echo "or without HTTPS/SSL support."
echo "Also read the continually updated Getting Started Guide"
echo "at centminmod.com/getstarted.html if you haven't already"
cecho "---------------------------------------------------------------" $boldyellow
echo "403 Permission denied message handling"
echo "if after vhost site setup you encounter 403 permission denied errors,"
echo "check https://community.centminmod.com/threads/11215/ to see if your"
echo "site needs tools/autoprotect.sh tweaking & whitelisting"
cecho "---------------------------------------------------------------" $boldyellow
if [[ "$LETSENCRYPT_DETECT" != [yY] ]]; then
echo "[ LETSENCRYPT_DETECT is not enabled ]"
echo "Ignore this message if you do not want HTTPS based web site otherwise"
echo "read below carefully."
echo
echo "Free letsencrypt SSL certificates integration is in beta testing if"
echo "you want to obtain free letsencrypt SSL certificate for HTTPS site,"
echo "you will need to manually enable LETSENCRYPT_DETECT='y' outlined"
echo "at https://centminmod.com/acmetool so exit this vhost routine first"
echo "set LETSENCRYPT_DETECT='y' and update domain DNS A record first"
echo "then re-run vhost site creation menu option"
cecho "---------------------------------------------------------------" $boldyellow
fi
# check mysql server is running
mysqladmin -s ping >/dev/null 2>&1
CHECKMYSQL_PING=$?
if [[ "$CHECKMYSQL_PING" -ne '0' ]]; then
  echo
  echo "!!  Error: MariaDB MySQL Server is not running  !!"
  echo "Please make sure MariaDB MySQL Server is running first"
  echo "aborting centmin.sh menu option 22 run..."
  exit
fi
echo
read -ep "Do you want to continue with Nginx vhost site creation ? [y/n] " dovhost_continue
echo

if [[ "$dovhost_continue" != [yY] ]]; then
  echo "aborting Wordpress + Nginx vhost setup..."
  exit
fi
}

fc_sslvhost() {

cecho "---------------------------------------------------------------" $boldyellow
cecho "SSL Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo ""

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]] && [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_spdy_module')" = 'with-http_spdy_module' ]]; then
  HTTPTWO=y
  LISTENOPT='ssl spdy http2'
  COMP_HEADER='spdy_headers_comp 5'
  SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
  HTTPTWO_MAXFIELDSIZE='http2_max_field_size 16k;'
  HTTPTWO_MAXHEADERSIZE='http2_max_header_size 32k;'  
elif [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
  HTTPTWO=y
  if [[ "$(grep -rn listen /usr/local/nginx/conf/conf.d/ | grep -v '#' | grep 443 | grep ' ssl' | grep ' http2' | grep reuseport | awk -F ':  ' '{print $2}' | grep -o reuseport)" != 'reuseport' ]]; then
    # check if reuseport is supported for listen 443 port - only needs to be added once globally for all nginx vhosts
    NGXVHOST_CHECKREUSEPORT=$(grep --color -Ro SO_REUSEPORT /usr/src/kernels/* | head -n1 | awk -F ":" '{print $2}')
    if [[ "$NGXVHOST_CHECKREUSEPORT" = 'SO_REUSEPORT' ]]; then
      ADD_REUSEPORT=' reuseport'
    else
      ADD_REUSEPORT=""
    fi
    LISTENOPT="ssl http2${ADD_REUSEPORT}"
  else
    LISTENOPT='ssl http2'
  fi
  COMP_HEADER='#spdy_headers_comp 5'
  SPDY_HEADER='#add_header Alternate-Protocol  443:npn-spdy/3;'
  HTTPTWO_MAXFIELDSIZE='http2_max_field_size 16k;'
  HTTPTWO_MAXHEADERSIZE='http2_max_header_size 32k;'
else
  HTTPTWO=n
  LISTENOPT='ssl spdy'
  COMP_HEADER='spdy_headers_comp 5'
  SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
fi

if [ ! -f /usr/local/nginx/conf/ssl ]; then
  mkdir -p /usr/local/nginx/conf/ssl
fi

if [ ! -d /usr/local/nginx/conf/ssl/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/${vhostname}
fi

# cloudflare authenticated origin pull cert
# setup https://community.centminmod.com/threads/13847/
if [ ! -d /usr/local/nginx/conf/ssl/cloudflare/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/cloudflare/${vhostname}
  wget $CLOUDFLARE_AUTHORIGINPULLCERT -O origin.crt
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

# echo
# cecho "---------------------------------------------------------------" $boldyellow
# cecho "Generating backup CSR and private key for HTTP Public Key Pinning..." $boldgreen
# cecho "creating CSR File: ${vhostname}-backup.csr" $boldgreen
# cecho "creating private key: ${vhostname}-backup.key" $boldgreen
# sleep 5

# openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}-backup.csr -keyout ${vhostname}-backup.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"

# echo
# cecho "---------------------------------------------------------------" $boldyellow
# cecho "Extracting Base64 encoded information for primary and secondary" $boldgreen
# cecho "private key's SPKI - Subject Public Key Information" $boldgreen
# cecho "Primary private key - ${vhostname}.key" $boldgreen
# cecho "Backup private key - ${vhostname}-backup.key" $boldgreen
# cecho "For HPKP - HTTP Public Key Pinning hash generation..." $boldgreen
# sleep 5

# echo
# cecho "extracting SPKI Base64 encoded hash for primary private key = ${vhostname}.key ..." $boldgreen

# openssl rsa -in ${vhostname}.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt

# echo
# cecho "extracting SPKI Base64 encoded hash for backup private key = ${vhostname}-backup.key ..." $boldgreen

# openssl rsa -in ${vhostname}-backup.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt

# echo
# cecho "HTTP Public Key Pinning Header for Nginx" $boldgreen

# echo
# cecho "for 7 days max-age including subdomains" $boldgreen
# echo
# echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400; includeSubDomains';"

# echo
# cecho "for 7 days max-age excluding subdomains" $boldgreen
# echo
# echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400';"

if [[ ! -f "$(find /usr/local/nginx/conf/ssl -type f -name "dhparam.pem" | head -n1)" ]]; then
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen 
  dhparamstarttime=$(TZ=UTC date +%s.%N) 
  openssl dhparam -out dhparam.pem 2048
  dhparamendtime=$(TZ=UTC date +%s.%N)
  DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
  cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow
else
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "Copy/setup dhparam.pem file..." $boldgreen
  cp -a "$(find /usr/local/nginx/conf/ssl -type f -name "dhparam.pem" | head -n1)" .
fi

}

fc_wpacctsetup() {
PUREUSER=nginx
PUREGROUP=nginx
    if [ "$SECOND_IP" ]; then
      CNIP="$SECOND_IP"
    else
      CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
    fi
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

fc_wpinstall
  WPSALT=$(openssl rand 19 -base64 | tr -dc 'a-zA-Z0-9')
  WPSALTB=$(openssl rand 11 -base64 | tr -dc 'a-zA-Z0-9')
  WPN=$RANDOM
  WPNB=$RANDOM
  WPADMINUSER="z${WPSALT}wp${WPNB}"
  WPADMINUSER=$(echo $WPADMINUSER | sed -e 's|\/||g' -e 's|\+||g')
  WPADMINPASS="z${WPSALTB}wps${WPN}"
  WPADMINPASS=$(echo $WPADMINPASS | sed -e 's|\/||g' -e 's|\+||g')
 
if [ ! -d /root/tools ]; then
  mkdir -p /root/tools
fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Setup full Nginx vhost + Wordpress + WP Plugins" $boldgreen
cecho "-------------------------------------------------------------" $boldyellow
echo

fc_wpinfo_notice

read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

# check to make sure you don't add a domain name vhost that matches
# your server main hostname setup in server_name within main hostname
# nginx vhost at /usr/local/nginx/conf/conf.d/virtual.conf
if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
  CHECK_MAINHOSTNAME=$(awk '/server_name/ {print $2}' /usr/local/nginx/conf/conf.d/virtual.conf | sed -e 's|;||')
  if [[ "${CHECK_MAINHOSTNAME}" = "${vhostname}" ]]; then
    echo
    echo " Error: $vhostname is already setup for server main hostname"
    echo " at /usr/local/nginx/conf/conf.d/virtual.conf"
    echo " It is important that main server hostname be setup correctly"
    echo
    echo " As per Getting Started Guide Step 1 centminmod.com/getstarted.html"
    echo " The server main hostname needs to be unique. So please setup"
    echo " the main server name vhost properly first as per Step 1 of guide."
    echo
    echo " Aborting nginx vhost creation..."
    echo
    exit 1
  fi
fi

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
  if [[ -f "${SCRIPT_DIR}/addons/acmetool.sh" && "$LETSENCRYPT_DETECT" = [yY] ]]; then
    read -ep "Get Letsencrypt SSL certificate Nginx vhost? [y/n]: " vhostssl_le
    if [[ "$vhostssl_le" = [yY] ]]; then
      echo
      echo "You have 4 options: "
      echo "1. issue staging test cert with HTTP + HTTPS (untrusted)"
      echo "2. issue staging test cert with HTTPS default (untrusted)"
      echo "3. issue live cert with HTTP + HTTPS (trusted)"
      echo "4. issue live cert with HTTPS default (trusted)"
      read -ep "Enter option number 1-4: " vhostssl_opt
      if [[ "$vhostssl_opt" = '1' ]]; then
        vhostssl='le'
      elif [[ "$vhostssl_opt" = '2' ]]; then
        vhostssl='led'
        wpcli_ssldefault=1
      elif [[ "$vhostssl_opt" = '3' ]]; then
        vhostssl='lelive'
      elif [[ "$vhostssl_opt" = '4' ]]; then
        vhostssl='lelived'
        wpcli_ssldefault=1
      fi
      echo
    fi
  fi
fi

echo "Theme Setup: "
read -ep "Install CyberChimps Responsive Theme (cyberchimps.com/responsive-theme/) [y/n]: " -i n responsivetheme

echo
echo "Wordpress Setup: "
read -ep "Set custom WP Admin Display Name ? [y/n]: " setdisplayname
if [[ "$setdisplayname" = [yY] ]]; then
  read -ep "Enter Custom WP Admin Display Name: " displayname
  WPADMIN_DISPLAYNAME=$displayname
fi
read -ep "Install Wordpress in subdirectory /blog ? [y/n]: " wpsubdirinstall

if [[ "$wpsubdirinstall" = [yY] ]]; then
  SUBDIR_INSTALL=y
  read -ep "Enter subdirectory name i.e. /blog enter = blog ? : " wpsubdir_value
  WPSUBDIR="/$wpsubdir_value"
  SUBDIR_INCLUDE="include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf;"
  NONSUBDIR_INCLUDE=""
elif [[ "$wpsubdirinstall" != [yY] ]]; then
  SUBDIR_INSTALL=n
  WPSUBDIR=""
  SUBDIR_INCLUDE=""
  NONSUBDIR_INCLUDE="include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf;"
fi

read -ep "Disable Auto Generated WP Admin Username / Password ? [y/n]: " disableautogen
read -ep "Disable wp-login.php password protection ? (less security) [y/n]: " disablepwdprotect

if [[ "$disableautogen" = [yY] ]]; then
  if [ ! -f /usr/sbin/cracklib-check ]; then
    yum -y -q install cracklib
  fi
  echo
  cecho "--------------------------" $boldgreen
  cecho "!! Security Note !!" $boldyellow
  cecho "--------------------------" $boldgreen
  cecho "Please choose a stronger Username/Password Combination" $boldyellow
  echo
  read -ep "Enter desired WP Admin Username: " WPADMINUSER
  read -ep "Enter desired WP Admin Password: " WPADMINPASS

  CHECKWPADMINPASSWD="$(cracklib-check <<<"$WPADMINPASS")"
  okay="$(awk -F': ' '{ print $2}' <<<"$CHECKWPADMINPASSWD")"
  while [[ "$okay" != "OK" ]]; do
    cecho "!! password strength not strong enough !! " $boldyellow
    cecho "!! do not use common dictionary words !! " $boldyellow
    cecho "!! do not use short passwords !! " $boldyellow
    cecho "!! do not use simplistic passwords !! " $boldyellow
    echo
    read -ep "Re-Enter desired WP Admin Password: " WPADMINPASS
    CHECKWPADMINPASSWD="$(cracklib-check <<<"$WPADMINPASS")"
    okay="$(awk -F': ' '{ print $2}' <<<"$CHECKWPADMINPASSWD")"
  done
fi

read -ep "Enter email address for Admin User for Wordpress Installation: " WPADMINEMAIL

echo
cecho "Default is to install KeyCDN WP Cache Enabler Plugin as it is more" $boldyellow
cecho "stable and reliable than WP Super Cache and Redis Cache." $boldyellow
cecho "Redis cache may have issues with caching due to long 6hr cache TTL" $boldyellow
if [[ "$WP_FASTCGI_CACHE" = [yY] ]]; then
  cecho "Fastcgi_cache (PHP-FPM) will have best performance" $boldyellow
fi
cecho "You can select which caching method to use below:" $boldyellow
echo

cecho "--------------------------------------------------------" $boldyellow
cecho "        Wordpress Caching               " $boldgreen
cecho "--------------------------------------------------------" $boldyellow
cecho "1). KeyCDN Cache Enabler (default & recommended)" $boldgreen
cecho "2). Redis Nginx Level Caching (may have issues with some wp plugins)" $boldgreen
cecho "3). Wordpress Super Cache" $boldgreen
if [[ "$WP_FASTCGI_CACHE" = [yY] ]]; then
  cecho "4). Fastcgi_cache (PHP-FPM)" $boldgreen
fi
cecho "--------------------------------------------------------" $boldyellow
if [[ "$WP_FASTCGI_CACHE" = [yY] ]]; then
  read -ep "Enter option [ 1 - 4 ] " wpcache_option
else
  read -ep "Enter option [ 1 - 3 ] " wpcache_option
fi
echo

if [[ "$wpcache_option" = '1' ]]; then
  wpscache='n'
elif [[ "$wpcache_option" = '2' ]]; then
  wpscache='redis'
elif [[ "$wpcache_option" = '3' ]]; then
  wpscache='y'
elif [[ "$wpcache_option" = '4' ]]; then
  wpscache='fastcgicache'
else
  wpscache='n'
fi

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
    ftppass=$(pwgen -1cnys 27)
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

fc_dbsetup

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}
ngx_logformats
if [[ "$wpsubdirinstall" = [yY] ]]; then
  mkdir -p /home/nginx/domains/$vhostname/public/$wpsubdir_value
fi

if [ ! -f /usr/local/nginx/conf/wpincludes ]; then
  mkdir -p /usr/local/nginx/conf/wpincludes
fi

if [ ! -f "/usr/local/nginx/conf/wpincludes/$vhostname" ]; then
  mkdir -p "/usr/local/nginx/conf/wpincludes/$vhostname"
fi

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo "${ftppass}" ; echo "${ftppass}" ) | pure-pw useradd "$ftpuser" -u $PUREUSER -g $PUREGROUP -d "/home/nginx/domains/$vhostname"
  pure-pw mkdb
fi

if [[ "$wpsubdirinstall" = [yY] ]]; then
cat > "/home/nginx/domains/$vhostname/public/index.html" <<END
<html>
<head>
<title>$vhostname</title>
</head>
<body>
<p>Welcome to $vhostname. This index.html page can be removed. You have auto installed Wordpress at $vhostname$WPSUBDIR</p>

<p>Useful Centmin Mod info and links to bookmark.</p>

<ul>
  <li>Getting Started Guide - <a href="http://centminmod.com/getstarted.html" target="_blank">http://centminmod.com/getstarted.html</a></li>
  <li>Latest Centmin Mod version - <a href="http://centminmod.com" target="_blank">http://centminmod.com</a></li>
  <li>Centmin Mod FAQ - <a href="http://centminmod.com/faq.html" target="_blank">http://centminmod.com/faq.html</a></li>
  <li>Change Log - <a href="http://centminmod.com/changelog.html" target="_blank">http://centminmod.com/changelog.html</a></li>
  <li>Google+ Page latest news <a href="http://centminmod.com/gpage" target="_blank">http://centminmod.com/gpage</a></li>
  <li>Centmin Mod Community Forum <a href="https://community.centminmod.com/" target="_blank">https://community.centminmod.com/</a></li>
  <li>Centmin Mod Twitter <a href="https://twitter.com/centminmod" target="_blank">https://twitter.com/centminmod</a></li>
  <li>Centmin Mod Facebook Page <a href="https://www.facebook.com/centminmodcom" target="_blank">https://www.facebook.com/centminmodcom</a></li>
</ul>

<p><a href="https://www.digitalocean.com/?refcode=c1cb367108e8" target="_blank">Cheap VPS Hosting at Digitalocean</a></p>

</body>
</html>
END
fi

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/$vhostname/public
umask 022
chown -R nginx:nginx "/home/nginx/domains/$vhostname"
find "/home/nginx/domains/$vhostname" -type d -exec chmod g+s {} \;

if [[ "$disablepwdprotect" != [yY] ]]; then
  # wp-login.php password protection
  if [[ -f /usr/local/nginx/conf/htpasswd.sh && ! -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
    HTWPLOGINSALT=$(openssl rand 14 -base64 | tr -dc 'a-zA-Z0-9')
    HTWPLOGINSALTB=$(openssl rand 20 -base64 | tr -dc 'a-zA-Z0-9')
    HTWPLOGIN=$RANDOM
    HTWPLOGINB=$RANDOM
    HTUSER="u${HTWPLOGINSALT}x${HTWPLOGIN}"
    HTUSER=$(echo $HTUSER | sed -e 's|\/||g')
    HTPASS="p${HTWPLOGINSALTB}y${HTWPLOGIN}"
    HTPASS=$(echo $HTPASS | sed -e 's|\/||g')
    echo "/usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS"
    /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS
  fi
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

if [[ "$wpscache" = 'fastcgicache' ]]; then
################################################################################
# create fastcgi_cache included files

# create separate fastcgi_cache storage directories per nginx vhostname
fastcgicache_name=$(echo $vhostname | sed -e 's|\.||g' -e 's|\-||g')
mkdir -p /home/fastcgicache_wp/${fastcgicache_name}
chown -R nginx: /home/fastcgicache_wp/${fastcgicache_name}
chmod 1777 /home/fastcgicache_wp/${fastcgicache_name}

cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf"<<FCC
# Block nginx-help log from public viewing
location ~* /wp-content/uploads/nginx-helper/ { deny all; }

# fastcgi_cache
set \$no_wpfastcgicache 0;

# POST requests and urls with a query string should always go to PHP
if (\$request_method = POST) {
  set \$no_wpfastcgicache 1;
}
if (\$query_string != "") {
  set \$no_wpfastcgicache 1;
}

# exclude mobile devices from redis caching
if (\$wpfcgi_cache_mobile = 1) { set \$no_wpfastcgicache 1; }

# bypass cache for woocommerce
if (\$arg_add-to-cart != "") { 
  set \$no_wpfastcgicache 1;
}

## bypass cache for empty woocommerce carts
#if (\$cookie_woocommerce_items_in_cart != "0") { 
#  set \$no_wpfastcgicache 1;
#}

# Add trailing slash to */wp-admin requests.
rewrite /wp-admin\$ \$scheme://\$host\$uri/ permanent;

# Centmin Mod Nginx has nginx_cache_purge module
# added http://centminmod.com/nginx.html
    location ~ /purge(/.*) {
    fastcgi_cache_purge $fastcgicache_name "\$scheme\$request_method\$host\$1";
    return 200;
    allow 127.0.0.1;
    deny all;
}
FCC

cat >> "/usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf"<<FCD
fastcgi_cache_path  /home/fastcgicache_wp/${fastcgicache_name} levels=1:2 keys_zone=$fastcgicache_name:30m inactive=1h max_size=60m loader_files=1000 loader_threshold=2000;
FCD

cat > "/usr/local/nginx/conf/wpfastcgi_cache_map.conf"<<FCD
upstream wpfcgi_cache_php {
  keepalive 16;
  #least_conn;
  #server unix:/tmp/php5-fpm.sock;
  server 127.0.0.1:9000;
}

map \$http_x_requested_with \$wpfcgi_request_nocache {
    default        0;
    XMLHttpRequest 1;
}

map \$http_user_agent \$wpfcgi_cache_mobile {
    default                                      0;
    include /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf;
    ~*(iPad|iPhone|Android|IEMobile|Blackberry)  1;
    "~*Firefox.*Mobile"                          1;
    "~*ipod.*mobile"                             1;
    "~*Opera\ Mini"                              1;
    "~*Opera\ Mobile"                            1;
    "~*Mobile"                                   1;
    "~*Tablet"                                   1;
    "~*Kindle"                                   1;
    "~*Windows\ Phone"                           1;
}

map \$http_cookie \$wpfcgi_nocachecookie {
     default                                0;
     include /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf;
    ~*[a-z0-9]+_items_in_cart                1;
    ~*amazon_Login_state_cache               1;
    ~*comment_author                         1;
    ~*comment_author_email_                  1;
    ~*edd_items_in_cart                      1;
    ~*ips4_IPSSessionFront                   1;
    ~*ips4_member_id                         1;
    ~*joomla_                                1;
    ~*joomla_[a-zA-Z0-9_]+                   1;
    ~*member_id                              1;
    ~*no_cache                               1;
    ~*nocache                                1;
    ~*PHPSESSID                              1;
    ~*PrestaShop-[a-zA-Z0-9]+                1;
    ~*sid_admin_                             1;
    ~*sid_admin_                             1;
    ~*sid_customer_                          1;
    ~*sid_customer_                          1;
    ~*wc_session_cookie_HASH                 1;
    ~*woocommerce_                           1;
    ~*wordpress_                             1;
    ~*wp-postpass                            1;
    ~*wp_woocommerce_                        1;
    ~*wp_woocommerce_session_[a-zA-Z0-9]+    1;
    ~*wptouch_switch_toggle                  1;
    ~*xf_dbtech_security_session             1;
    ~*xf_fbUid                               1;
    ~*xf_logged_in                           1;
    ~*xf_style_id                            1;
    ~*xf_tfa_trust                           1;
    ~*xf_user                                1;
}

map \$request_uri \$wpfcgi_nocacheuri {
    default                          0;
    include /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf;
    ~*magento_admin                  1;
    ~*magento_login                  1;
    ~^(.*sitemap.*\.xml)             1;
    ~^/(admin.php)\$                 1;
    ~^/(cron.php)\$                  1;
    ~^/(deferred.php)\$              1;
    ~^/(pstat.php)\$                 1;
    ~^/add-to-cart=                  1;
    ~^/addons                        1;
    ~^/admin                         1;
    ~^/administrator                 1;
    ~^/backend                       1;
    ~^/captcha                       1;
    ~^/cart/                         1;
    ~^/checkout/                     1;
    ~^/component/banners             1;
    ~^/component/contact             1;
    ~^/component/mailto              1;
    ~^/component/socialconnect       1;
    ~^/component/user                1;
    ~^/component/users               1;
    ~^/customer                      1;
    ~^/customer-dashboard/           1;
    ~^/dispatch=checkout.cart        1;
    ~^/dispatch=checkout.checkout    1;
    ~^/feed/                         1;
    ~^/login                         1;
    ~^/login.php                     1;
    ~^/lost-password                 1;
    ~^/munin                         1;
    ~^/my-account/                   1;
    ~^/ngx_pagespeed_beacon          1;
    ~^/register                      1;
    ~^/shop/checkout/                1;
    ~^/signin                        1;
    ~^/signup                        1;
    ~^/store/checkout/               1;
    ~^/two-step                      1;
    ~^/ucp.php                       1;
    ~^/validate-field                1;
    ~^/wc-api=                       1;
    ~^/wp-admin/                     1;
    ~^/wp-comments-popup.php         1;
    ~^/wp-json                       1;
    ~^/wp-links-opml.php             1;
    ~^/wp-locations.php              1;
    ~^/wp-login.php                  1;
    ~^/wp-mail.php                       1;
    ~^/wp-register.php               1;
    ~^/xmlrpc.php                    1;
}

map \$remote_addr \$fastcgi_debug {
  default 0;
  include /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf;
  # YOUR_ISP_IP_ADDR 1;
}
FCD

echo "touch /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf"
touch /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf
if [ -s /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf ]; then
  echo "# example format for allowing specific IPs to view fastcgi_cache related headers" >> /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf
  echo "#YOURISP_IPADDRESS   1;" >> /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf
  echo "#YOURSERVER_IPADDRESS 1;" >> /usr/local/nginx/conf/wpfastcgi_cache_map_debug.conf
fi

echo "touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf"
touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf
if [ -s /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf ]; then
  echo "# example format to specify HTTP User Agents which bypass fastcgi_cache" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf
  echo "# "~*Opera\ Mobile"    1;" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_mobile.conf
fi

echo "touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf"
touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf
if [ -s /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf ]; then
  echo "# example format to specify cookie names which bypass fastcgi_cache" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf
  echo "# ~*cookiename    1;" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocachecookie.conf
fi

echo "touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf"
touch /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf
if [ -s /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf ]; then
  echo "# example format to specify which urls bypass fastcgi_cache" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf
  echo "# ~^/privatedir     1;" >> /usr/local/nginx/conf/wpfastcgi_cache_map_include_nocacheuri.conf
fi

WPFASTCGICACHEPATH_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/wpfastcgi_cache_path_includes.conf' /usr/local/nginx/conf/nginx.conf)
  if [[ -z "$WPFASTCGICACHEPATH_INCLUDECHECK" ]]; then
    echo
    echo "include file /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf add to nginx.conf"
      sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/wpfastcgi_cache_path_includes.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi

WPFASTCGICACHEMAP_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/wpfastcgi_cache_map.conf' /usr/local/nginx/conf/nginx.conf)
  if [[ -z "$WPFASTCGICACHEMAP_INCLUDECHECK" ]]; then
    echo
    echo "include file /usr/local/nginx/conf/wpfastcgi_cache_map.conf add to nginx.conf"
      sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/wpfastcgi_cache_map.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi

fi # wpscache=fastcgicache

################################################################################
# create wp enable enabler included files https://community.centminmod.com/posts/21220/

cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf"<<HFA
    # Block nginx-help log from public viewing
    location ~* /wp-content/uploads/nginx-helper/ { deny all; }

    set \$cache_uri \$request_uri;

    # exclude mobile devices from redis caching
    if (\$cmwpcache_device = mobile) { set \$cache_uri 'nullcache'; }

    # bypass cache if POST requests or URLs with a query string
    if (\$request_method = POST) {
        set \$cache_uri 'nullcache';
    }
    if (\$query_string != "") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache if URLs containing the following strings
    if (\$request_uri ~* "(\?add-to-cart=|/cart/|/my-account/|/checkout/|/shop/checkout/|/store/checkout/|/customer-dashboard/|/addons/|/wp-admin/|/xmlrpc.php|/wp-(app|cron|login|register|mail).php|wp-.*.php|/feed/|index.php|wp-comments-popup.php|wp-links-opml.php|wp-locations.php|sitemap(_index)?.xml|[a-z0-9_-]+-sitemap([0-9]+)?.xml)") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache if the cookies containing the following strings
    if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in|edd_items_in_cart|woocommerce_items_in_cart|woocommerce_cart_hash|woocommerce_recently_viewed|wc_session_cookie_HASH|wp_woocommerce_session_|wptouch_switch_toggle") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache for woocommerce
    if (\$arg_add-to-cart != "") { 
        set \$cache_uri 'nullcache';
    }

    ## bypass cache for empty woocommerce carts
    #if (\$cookie_woocommerce_items_in_cart != "0") { 
    #  set \$cache_uri 'nullcache';
    #}

    # custom sub directory e.g. /blog
    set \$custom_subdir '${WPSUBDIR}';

    # default html file
    set \$cache_enabler_uri '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}index.html';

    # webp html file
    if (\$http_accept ~* "image/webp") {
        set \$cache_enabler_uri '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}index-webp.html';
    }
HFA

cat > "/usr/local/nginx/conf/wpcacheenabler_map.conf"<<HFB
map \$http_user_agent \$cmwpcache_device {
    default                                     'desktop';
    ~*(iPad|iPhone|Android|IEMobile|Blackberry) 'mobile';
    "~*Firefox.*Mobile"                         'mobile';
    "~*ipod.*mobile"                            'mobile';
    "~*Opera\ Mini"                             'mobile';
    "~*Opera\ Mobile"                           'mobile';
    "~*Mobile"                                  'mobile';
    "~*Tablet"                                  'mobile';
    "~*Kindle"                                  'mobile';
    "~*Windows\ Phone"                          'mobile';
}
HFB

WPCACHEENABLERMAP_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/wpcacheenabler_map.conf' /usr/local/nginx/conf/nginx.conf)
  if [[ -z "$WPCACHEENABLERMAP_INCLUDECHECK" ]]; then
    echo
    echo "include file /usr/local/nginx/conf/wpcacheenabler_map.conf add to nginx.conf"
      sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/wpcacheenabler_map.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi

################################################################################
# create nginx level fastcgi_cache included php config file php-fastcgicache.conf

cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf"<<HFF
location ~ [^/]\.php(/|\$) {
  include /usr/local/nginx/conf/503include-only.conf;
    fastcgi_split_path_info ^(.+?\.php)(/.*)\$;
    if (!-f \$document_root\$fastcgi_script_name) {
        return 404;
    }

    fastcgi_pass    wpfcgi_cache_php;
    #fastcgi_pass   127.0.0.1:9000;
    #fastcgi_pass   unix:/tmp/php5-fpm.sock;
    fastcgi_index  index.php;
    #fastcgi_param  SCRIPT_FILENAME  \$document_root\$fastcgi_script_name;
    fastcgi_param  SCRIPT_FILENAME    \$request_filename;
    #fastcgi_param PHP_ADMIN_VALUE open_basedir=\$document_root/:/usr/local/lib/php/:/tmp/;

### fastcgi_cache start ###
fastcgi_cache_methods GET;
fastcgi_cache_key       \$scheme\$request_method\$host\$request_uri;
fastcgi_cache_lock        on;
fastcgi_keep_conn         on;
fastcgi_cache_lock_age    5s;
fastcgi_cache_lock_timeout 5s;
fastcgi_cache_use_stale error timeout invalid_header updating http_500;
fastcgi_ignore_headers  Cache-Control Expires Set-Cookie;
fastcgi_cache           $fastcgicache_name;
fastcgi_no_cache        \$no_wpfastcgicache \$wpfcgi_nocachecookie \$wpfcgi_nocacheuri \$wpfcgi_request_nocache;
fastcgi_cache_bypass    \$no_wpfastcgicache \$wpfcgi_nocachecookie \$wpfcgi_nocacheuri \$wpfcgi_request_nocache;
fastcgi_cache_valid     200 15m;
fastcgi_cache_valid     301 302 15m;
fastcgi_cache_valid     303 304 15m;
fastcgi_cache_valid     404 5m;
#add_header X-Fpm-Cache    \$upstream_cache_status;
if (\$fastcgi_debug) {
 add_header X-Processing-Time \$request_time always;
 add_header X-Request-ID      \$request_id always;
 add_header X-Fpm-Cache    \$upstream_cache_status;
}
### fastcgi_cache end ###

# might shave 200+ ms off PHP requests
# which don't pass on a content length header
# slightly faster page response time at the
# expense of throughput / scalability
#sendfile on;
#tcp_nopush off;
#keepalive_requests 0;

fastcgi_connect_timeout 60s;
fastcgi_send_timeout 180s;
fastcgi_read_timeout 300s;
fastcgi_buffer_size 512k;
fastcgi_buffers 512 16k;
fastcgi_busy_buffers_size 1m;
fastcgi_temp_file_write_size 4m;
fastcgi_max_temp_file_size 4m;
fastcgi_intercept_errors off;

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
fastcgi_param  HTTP_PROXY         "";

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
# create nginx level redis cache included php config file php-rediscache.conf
# https://community.centminmod.com/posts/18828/
# \cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php-rediscache.conf

cat > "/usr/local/nginx/conf/php-rediscache.conf"<<HFF
location ~ [^/]\.php(/|\$) {
  include /usr/local/nginx/conf/503include-only.conf;
    fastcgi_split_path_info ^(.+?\.php)(/.*)\$;
    if (!-f \$document_root\$fastcgi_script_name) {
        return 404;
    }

    set \$key "nginx-cache:\$scheme\$request_method\$host\$request_uri";
    srcache_fetch_skip \$skip_cache;
    srcache_store_skip \$skip_cache;
    srcache_response_cache_control off;
    set_escape_uri \$escaped_key \$key;
    srcache_fetch GET /redis-fetch \$key;
    srcache_store PUT /redis-store key=\$escaped_key;
    more_set_headers 'X-Cache \$srcache_fetch_status';
    more_set_headers 'X-Cache-2 \$srcache_store_status';

    fastcgi_pass   127.0.0.1:9000;
    #fastcgi_pass   unix:/tmp/php5-fpm.sock;
    fastcgi_index  index.php;
    #fastcgi_param  SCRIPT_FILENAME  \$document_root\$fastcgi_script_name;
    fastcgi_param  SCRIPT_FILENAME    \$request_filename;
    #fastcgi_param PHP_ADMIN_VALUE open_basedir=\$document_root/:/usr/local/lib/php/:/tmp/;

# might shave 200+ ms off PHP requests
# which don't pass on a content length header
# slightly faster page response time at the
# expense of throughput / scalability
#sendfile on;
#tcp_nopush off;
#keepalive_requests 0;

fastcgi_connect_timeout 60s;
fastcgi_send_timeout 180s;
fastcgi_read_timeout 300s;
fastcgi_buffer_size 512k;
fastcgi_buffers 512 16k;
fastcgi_busy_buffers_size 1m;
fastcgi_temp_file_write_size 4m;
fastcgi_max_temp_file_size 4m;
fastcgi_intercept_errors off;

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
fastcgi_param  HTTP_PROXY         "";

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

cat > "/usr/local/nginx/conf/php-rediscache-shortttl.conf"<<HFI
location ~ [^/]\.php(/|\$) {
  include /usr/local/nginx/conf/503include-only.conf;
    fastcgi_split_path_info ^(.+?\.php)(/.*)\$;
    if (!-f \$document_root\$fastcgi_script_name) {
        return 404;
    }

    set \$key "nginx-cache:\$scheme\$request_method\$host\$request_uri";
    srcache_fetch_skip \$skip_cache;
    srcache_store_skip \$skip_cache;
    srcache_response_cache_control off;
    set_escape_uri \$escaped_key \$key;
    srcache_fetch GET /redis-fetch \$key;
    srcache_store PUT /redis-store-shortttl key=\$escaped_key;
    more_set_headers 'X-Cache \$srcache_fetch_status';
    more_set_headers 'X-Cache-2 \$srcache_store_status';

    fastcgi_pass   127.0.0.1:9000;
    #fastcgi_pass   unix:/tmp/php5-fpm.sock;
    fastcgi_index  index.php;
    #fastcgi_param  SCRIPT_FILENAME  \$document_root\$fastcgi_script_name;
    fastcgi_param  SCRIPT_FILENAME    \$request_filename;
    #fastcgi_param PHP_ADMIN_VALUE open_basedir=\$document_root/:/usr/local/lib/php/:/tmp/;

# might shave 200+ ms off PHP requests
# which don't pass on a content length header
# slightly faster page response time at the
# expense of throughput / scalability
#sendfile on;
#tcp_nopush off;
#keepalive_requests 0;

fastcgi_connect_timeout 60s;
fastcgi_send_timeout 180s;
fastcgi_read_timeout 300s;
fastcgi_buffer_size 512k;
fastcgi_buffers 512 16k;
fastcgi_busy_buffers_size 1m;
fastcgi_temp_file_write_size 4m;
fastcgi_max_temp_file_size 4m;
fastcgi_intercept_errors off;

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
fastcgi_param  HTTP_PROXY         "";

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
HFI

################################################################################
# Setting up Nginx mapping

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
  fc_sslvhost
fi

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then

  if [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]; then
      # check /svr-setup/openssl-1.0.2f/crypto/chacha20poly1305/chacha20.c exists
      OPEENSSL_CFPATCHED='y'
  elif [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]; then
      # for openssl 1.1.0 native chacha20 support
      OPEENSSL_CFPATCHED='y'
  fi

if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]] || [[ "$OPEENSSL_CFPATCHED" = [yY] ]]; then
  if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]]; then
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  elif [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]]; then
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  else
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  fi
else
  CHACHACIPHERS=""
fi

DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
    # openssl 1.1.0 unsupported flag enable-tlsext
    if [[ "$(grep -w 'tls1_3' "${DIR_TMP}/openssl-${OPENSSL_VERSION}/configdata.pm")" ]]; then
        TLSONETHREEOPT=' enable-tls1_3'
        TLSONETHREE_DETECT='y'
    else
        TLSONETHREEOPT=""
        TLSONETHREE_DETECT='n'
    fi
fi

if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
  TLSONETHREE_CIPHERS='TLS13-AES-128-GCM-SHA256:TLS13-AES-256-GCM-SHA384:TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES-128-CCM-8-SHA256:TLS13-AES-128-CCM-SHA256:'
else
  TLSONETHREE_CIPHERS=""
fi

if [[ -f /usr/bin/php73 && -f /usr/bin/php72 && -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php73-remi.conf;
  #include /usr/local/nginx/conf/php72-remi.conf;
  #include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php72 && -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php72-remi.conf;
  #include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php71 && -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;'
elif [[ -f /usr/bin/php71 && ! -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;'
elif [[ ! -f /usr/bin/php71 && -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php70-remi.conf;'
elif [[ ! -f /usr/bin/php71 && ! -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ ! -f /usr/bin/php71 && ! -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES=""
fi

if [[ "$VHOST_PRESTATICINC" = [yY] ]]; then
  PRESTATIC_INCLUDES="include /usr/local/nginx/conf/pre-staticfiles-local-${vhostname}.conf;
  include /usr/local/nginx/conf/pre-staticfiles-global.conf;"
  touch "/usr/local/nginx/conf/pre-staticfiles-local-${vhostname}.conf"
  touch /usr/local/nginx/conf/pre-staticfiles-global.conf
else
  PRESTATIC_INCLUDES=""
fi

if [[ "$VHOST_CFAUTHORIGINPULL" = [yY] ]]; then
  CFAUTHORIGINPULL_INCLUDES="# cloudflare authenticated origin pull cert community.centminmod.com/threads/13847/
  #ssl_client_certificate /usr/local/nginx/conf/ssl/cloudflare/$vhostname/origin.crt;
  #ssl_verify_client on;"
else
  CFAUTHORIGINPULL_INCLUDES=""
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

  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf;
  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;  

  location /$wpsubdir_value {
  include /usr/local/nginx/conf/503include-only.conf;
  $SUBDIR_INCLUDE

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;

  # Wordpress fastcgi_cache
    #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  # Wordpress Permalinks
   #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args; 

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  }

location ~* ${WPSUBDIR}/(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-scripts\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-styles\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  $NONSUBDIR_INCLUDE
  include /usr/local/nginx/conf/php-wpsc.conf;
  ${MULTIPHP_INCLUDES}
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  ${PRESTATIC_INCLUDES}
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
# For HTTP/2 SSL Setup
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

  $CFAUTHORIGINPULL_INCLUDES
  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${TLSONETHREE_CIPHERS}ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:${CHACHACIPHERS}DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA:ECDHE-ECDSA-DES-CBC3-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!DSS;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER

  # before enabling HSTS line below read centminmod.com/nginx_domain_dns_setup.html#hsts
  #add_header Strict-Transport-Security "max-age=31536000; includeSubdomains;";
  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";
  $COMP_HEADER;
  ssl_buffer_size 1369;
  ssl_session_tickets on;
  
  # enable ocsp stapling
  #resolver 8.8.8.8 8.8.4.4 1.1.1.1 1.0.0.1 valid=10m;
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

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf;
  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;  

  location /$wpsubdir_value {
  include /usr/local/nginx/conf/503include-only.conf;
  $SUBDIR_INCLUDE

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;  

  # Wordpress fastcgi_cache
    #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  # Wordpress Permalinks
   #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args;

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  }

location ~* ${WPSUBDIR}/(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-scripts\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-styles\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  $NONSUBDIR_INCLUDE
  include /usr/local/nginx/conf/php-wpsc.conf;
  ${MULTIPHP_INCLUDES}
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  ${PRESTATIC_INCLUDES}
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

  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf;
  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf;
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;  

  location /$wpsubdir_value {
  include /usr/local/nginx/conf/503include-only.conf;
  $SUBDIR_INCLUDE

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args;

  # for wp cache enabler plugin
  #try_files \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;   

  # Wordpress fastcgi_cache
    #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  # Wordpress Permalinks
   #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?q=\$uri&\$args;

  # Nginx level redis Wordpress
  # https://community.centminmod.com/posts/18828/
  #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;

  }

location ~* ${WPSUBDIR}/(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;  
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-scripts\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

location ~* ${WPSUBDIR}/wp-admin/(load-styles\.php) {
    limit_req zone=xwprpc burst=5 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
    ${MULTIPHP_INCLUDES}
    # https://community.centminmod.com/posts/18828/
    #include /usr/local/nginx/conf/php-rediscache.conf;
}

  $NONSUBDIR_INCLUDE
  include /usr/local/nginx/conf/php-wpsc.conf;
  ${MULTIPHP_INCLUDES}
  # https://community.centminmod.com/posts/18828/
  #include /usr/local/nginx/conf/php-rediscache.conf;
  ${PRESTATIC_INCLUDES}
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
END

fi

touch "/usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf"

cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" <<EEF
# prevent .zip, .gz, .tar, .bzip2 files from being accessed by default
# impossible for centmin mod to know which wp backup plugins they installed
# which may save backups to directories in wp-content/
# such plugins may deploy .htaccess protection but that isn't supported in
# nginx, so blocking access to these extensions is a workaround to cover all bases

# prepare for letsencrypt 
# https://community.centminmod.com/posts/17774/
location ~ /.well-known {
  location ~ /.well-known/acme-challenge/(.*) {
    more_set_headers    "Content-Type: text/plain";
    }
}

# allow AJAX requests in themes and plugins
location ~ ^${WPSUBDIR}/wp-admin/admin-ajax.php$ { allow all; include /usr/local/nginx/conf/php.conf; }

location ~* ^${WPSUBDIR}/(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z)\$ { deny all; }

location ~ ^${WPSUBDIR}/wp-content/uploads/sucuri { deny all; }

location ~ ^${WPSUBDIR}/wp-content/updraft { deny all; }

# Block nginx-help log from public viewing
location ~* ${WPSUBDIR}/wp-content/uploads/nginx-helper/ { deny all; }

location ~ ^${WPSUBDIR}/(wp-includes/js/tinymce/wp-tinymce.php) {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/onesignal-free-web-push-notifications//
location ~ ^${WPSUBDIR}/wp-content/plugins/onesignal-free-web-push-notifications/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sparkpost/
location ~ ^${WPSUBDIR}/wp-content/plugins/sparkpost/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendgrid-email-delivery-simplified/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendgrid-email-delivery-simplified/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailgun/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailgun/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailjet-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailjet-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/easy-wp-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/easy-wp-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/postman-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/postman-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-mail-bank/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-mail-bank/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/theme-check/
location ~ ^${WPSUBDIR}/wp-content/plugins/theme-check/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce-csvimport/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce-csvimport/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/advanced-custom-fields/
location ~ ^${WPSUBDIR}/wp-content/plugins/advanced-custom-fields/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/contact-form-7/
location ~ ^${WPSUBDIR}/wp-content/plugins/contact-form-7/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/duplicator/
location ~ ^${WPSUBDIR}/wp-content/plugins/duplicator/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/jetpack/
location ~ ^${WPSUBDIR}/wp-content/plugins/jetpack/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/nextgen-gallery/
location ~ ^${WPSUBDIR}/wp-content/plugins/nextgen-gallery/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/tinymce-advanced/
location ~ ^${WPSUBDIR}/wp-content/plugins/tinymce-advanced/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/updraftplus/
location ~ ^${WPSUBDIR}/wp-content/plugins/updraftplus/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-importer/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-importer/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-seo/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-seo/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wpclef/
location ~ ^${WPSUBDIR}/wp-content/plugins/wpclef/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailchimp-for-wp/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailchimp-for-wp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-optimize/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-optimize/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/si-contact-form/
location ~ ^${WPSUBDIR}/wp-content/plugins/si-contact-form/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/akismet/
location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/ {
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(form|akismet)\.(css|js)\$ { allow all; }
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(.+)\.(png|gif)\$ { allow all; }
  location ~* ${WPSUBDIR}/wp-content/plugins/akismet/akismet/.*\.php\$ {
    include /usr/local/nginx/conf/php.conf;
    include /usr/local/nginx/conf/staticfiles.conf;
    # below include file needs to be manually created at that path and to be uncommented
    # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
    # allows you to add commonly shared settings to all wp plugin location matches which
    # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
    #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
    allow 127.0.0.1;
    deny all;
  }
}

# Whitelist Exception for https://wordpress.org/plugins/bbpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/bbpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/buddypress/
location ~ ^${WPSUBDIR}/wp-content/plugins/buddypress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/all-in-one-seo-pack/
location ~ ^${WPSUBDIR}/wp-content/plugins/all-in-one-seo-pack/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/google-analytics-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/google-analytics-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/regenerate-thumbnails/
location ~ ^${WPSUBDIR}/wp-content/plugins/regenerate-thumbnails/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordfence/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordfence/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/really-simple-captcha/
location ~ ^${WPSUBDIR}/wp-content/plugins/really-simple-captcha/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/ml-slider/
location ~ ^${WPSUBDIR}/wp-content/plugins/ml-slider/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/black-studio-tinymce-widget/
location ~ ^${WPSUBDIR}/wp-content/plugins/black-studio-tinymce-widget/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/disable-comments/
location ~ ^${WPSUBDIR}/wp-content/plugins/disable-comments/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/better-wp-security/
location ~ ^${WPSUBDIR}/wp-content/plugins/better-wp-security/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for http://wlmsocial.com/
location ~ ^${WPSUBDIR}/wp-content/plugins/wlm-social/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for mediagrid timthumb
location ~ ^${WPSUBDIR}/wp-content/plugins/media-grid/classes/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Block PHP files in content directory.
location ~* ${WPSUBDIR}/wp-content/.*\.php\$ {
  deny all;
}

# Block PHP files in includes directory.
location ~* ${WPSUBDIR}/wp-includes/.*\.php\$ {
  deny all;
}

# Block PHP files in uploads, content, and includes directory.
location ~* ${WPSUBDIR}/(?:uploads|files|wp-content|wp-includes)/.*\.php\$ {
  deny all;
}

# Make sure files with the following extensions do not get loaded by nginx because nginx would display the source code, and these files can contain PASSWORDS!
location ~* \.(engine|inc|info|install|make|module|profile|test|po|sh|.*sql|theme|tpl(\.php)?|xtmpl)\$|^(\..*|Entries.*|Repository|Root|Tag|Template)\$|\.php_
{
return 444;
}

location ~* \.(tpl)\$
{
  deny all; 
}

#nocgi
location ~* \.(pl|cgi|py|sh|lua)\$ {
return 444;
}

#disallow
location ~* (w00tw00t) {
return 444;
}

location ~* ${WPSUBDIR}/(\.|wp-config\.php|wp-config\.txt|changelog\.txt|readme\.txt|readme\.html|license\.txt) { deny all; }
EEF

# WP super cache
cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf" <<EFF
set \$cache_uri \$request_uri;

if (\$request_method = POST) { set \$cache_uri 'null cache'; }

if (\$query_string != "") { set \$cache_uri 'null cache'; }

if (\$request_uri ~* "/(\?add-to-cart=|cart/|my-account/|checkout/|shop/checkout/|store/checkout/|customer-dashboard/|addons/|wp-admin/.*|xmlrpc\.php|wp-.*\.php|index\.php|feed/|sitemap(_index)?\.xml|[a-z0-9_-]+-sitemap([0-9]+)?\.xml)") { set \$cache_uri 'null cache'; }

# bypass cache for woocommerce
if ( \$arg_add-to-cart != "" ) { set \$cache_uri 'null cache'; }

## bypass cache for empty woocommerce carts
#if ( \$cookie_woocommerce_items_in_cart != "0" ) { set \$cache_uri 'null cache'; }

if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in|edd_items_in_cart|woocommerce_items_in_cart|woocommerce_cart_hash|woocommerce_recently_viewed|wc_session_cookie_HASH|wp_woocommerce_session_|wptouch_switch_toggle") { set \$cache_uri 'null cache'; }
EFF

# for nginx level redis cache
cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf" <<XFF
# Block nginx-help log from public viewing
location ~* ${WPSUBDIR}/wp-content/uploads/nginx-helper/ { deny all; }

set \$skip_cache 0;

# exclude mobile devices from redis caching
if (\$redis_device = mobile) { set \$skip_cache 1; }

# POST requests and urls with a query string should always go to PHP
if (\$request_method = POST) {
  set \$skip_cache 1;
}

if (\$query_string != "") {
  set \$skip_cache 1;
}

# Don't cache uris containing the following segments
if (\$request_uri ~* "\?add-to-cart=|/cart/|/my-account/|/checkout/|/shop/checkout/|/store/checkout/|/customer-dashboard/|/addons/|/wp-admin/|/xmlrpc.php|wp-.*.php|/feed/|index.php|sitemap(_index)?.xml") {
  set \$skip_cache 1;
}

# Don't use the cache for logged in users or recent commenters
if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_no_cache|wordpress_logged_in|edd_items_in_cart|woocommerce_items_in_cart|woocommerce_cart_hash|woocommerce_recently_viewed|wc_session_cookie_HASH|wp_woocommerce_session_|wptouch_switch_toggle") {
  set \$skip_cache 1;
}

# bypass cache for woocommerce
if (\$arg_add-to-cart != "") { 
  set \$skip_cache 1;
}

## bypass cache for empty woocommerce carts
#if (\$cookie_woocommerce_items_in_cart != "0") { 
#  set \$skip_cache 1;
#}

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

location /redis-store-shortttl {
  internal  ;
  set_unescape_uri \$key \$arg_key ;
  redis2_query set \$key \$echo_request_body;
  redis2_query expire \$key 3600;
  redis2_pass  redisbackend;
  redis2_connect_timeout 60s;
  redis2_read_timeout 60s;
  redis2_send_timeout 60s;
}
XFF

cat > "/usr/local/nginx/conf/redisupstream.conf" <<GGG
map \$http_user_agent \$redis_device {
    default                                     'desktop';
    ~*(iPad|iPhone|Android|IEMobile|Blackberry) 'mobile';
    "~*Firefox.*Mobile"                         'mobile';
    "~*ipod.*mobile"                            'mobile';
    "~*Opera\ Mini"                             'mobile';
    "~*Opera\ Mobile"                           'mobile';
    "~*Mobile"                                  'mobile';
    "~*Tablet"                                  'mobile';
    "~*Kindle"                                  'mobile';
    "~*Windows\ Phone"                          'mobile';
}

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
      sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/redisupstream.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi

########### WP Super Cache Start ##############################
# only proceed in creating vhost if VHOSTNAME directory exist
if [[ -d "/home/nginx/domains/${vhostname}/public" ]]; then

  cecho "------------------------------------------------------------" $boldgreen
  if [[ "$wpscache" = [nN] ]]; then
    cacheenabler=y
    cecho "Setup Wordpress + Cache Enabler for $vhostname" $boldyellow
  elif [[ "$wpscache" = [yY] ]]; then
    cecho "Setup Wordpress + Super Cache for $vhostname" $boldyellow
  elif [[ "$wpscache" = 'redis' ]]; then
    cecho "Setup Wordpress + Redis Nginx Level Cache for $vhostname" $boldyellow
    fc_installredisserver
  elif [[ "$wpscache" = 'fastcgicache' ]]; then
    cecho "Setup Fastcgi_cache PHP-FPM for $vhostname" $boldyellow
    # fc_install_fastcgicache
  fi
  cecho "------------------------------------------------------------" $boldgreen

  if [[ "$wpscache" = [nN] || "$wpscache" = 'redis' || "$wpscache" = 'fastcgicache' ]]; then
    echo
    echo "Using full static page caching may cause problems for mobile & tablet device"
    echo "visitors depending on your WP themes used so you may want to exclude those"
    echo
    read -ep "Do you want to exclude mobile/tablet devices from Cache Enabler caching ? [y/n]: " exclude_mobilecache
    if [[ "$exclude_mobilecache" = [nN] ]]; then
      if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" ]; then
        sed -i "s|^if (\$cmwpcache_device = mobile) { set \$cache_uri 'nullcache'; }|#if (\$cmwpcache_device = mobile) { set \$cache_uri 'nullcache'; }|" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf"
      fi
      if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf" ]; then
        sed -i "s|^if (\$redis_device = mobile) { set \$skip_cache 1; }|#if (\$redis_device = mobile) { set \$skip_cache 1; }|" "/usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf"
      fi
      if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf" ]; then
        sed -i "s|^if (\$wpfcgi_cache_mobile = 1) { set \$no_wpfastcgicache 1; }|#if (\$wpfcgi_cache_mobile = 1) { set \$no_wpfastcgicache 1; }|" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf"
      fi
    fi
  fi

cd /home/nginx/domains/${vhostname}/public${WPSUBDIR}
 
\wp core download --allow-root
 
\wp core config --dbname=$DB --dbuser=$DBUSER --dbpass=$DBPASS --allow-root
 
NEWPREFIX=$(echo $RANDOM)
sed -i "s/'wp_';/'${NEWPREFIX}_';/g" wp-config.php

if [[ "$WP_FASTCGI_CACHE" = [yY] ]]; then
sed -i "/define( 'DB_COLLATE', '' );/ a\
/** Enable core updates for minor releases (default) **/\ndefine('DISABLE_WP_CRON', false);\ndefine('WP_AUTO_UPDATE_CORE', 'minor' );\ndefine('WP_POST_REVISIONS', 10 );\ndefine('EMPTY_TRASH_DAYS', 10 );\ndefine('WP_CRON_LOCK_TIMEOUT', 60 );\ndefine('CONCATENATE_SCRIPTS', false);\ndefine( 'RT_WP_NGINX_HELPER_CACHE_PATH','/home/fastcgicache_wp/${fastcgicache_name}');\
" wp-config.php
else
sed -i "/define( 'DB_COLLATE', '' );/ a\
/** Enable core updates for minor releases (default) **/\ndefine('DISABLE_WP_CRON', false);\ndefine('WP_AUTO_UPDATE_CORE', 'minor' );\ndefine('WP_POST_REVISIONS', 10 );\ndefine('EMPTY_TRASH_DAYS', 10 );\ndefine('WP_CRON_LOCK_TIMEOUT', 60 );\ndefine('CONCATENATE_SCRIPTS', false);\
" wp-config.php
fi

if [[ -z "$(crontab -l 2>&1 | grep '\/${vhostname}/wp-cron.php')" ]]; then
    # generate random number of seconds to delay cron start
    # making sure they do not run at very same time during cron scheduling
    DELAY=$(echo ${RANDOM:0:3})
    crontab -l > cronjoblist
    mkdir -p /home/nginx/domains/${vhostname}/cronjobs
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-before-wp-cron.txt
    # only insert cronjob if one doesn't already exist
    if [[ -z $(grep "${vhostname}\/wp-cron.php" cronjoblist) ]]; then
      echo "#*/15 * * * * sleep ${DELAY}s ; wget -4 -O - -q -t 1 http://${vhostname}/wp-cron.php?doing_wp_cron > /dev/null 2>&1" >> cronjoblist
    fi
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-after-wp-cron.txt
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

\wp core install --url=http://${vhostname}${WPSUBDIR} --title=${vhostname} --admin_email=${WPADMINEMAIL} --admin_password=${WPADMINPASS} --admin_name=${WPADMINUSER} --allow-root

# change admin userid from 1 to a random 6 digit number
# WP_PREFIX=$(wp eval 'echo $GLOBALS["table_prefix"];')
WUID=$(echo $RANDOM$RANDOM |cut -c1-6)
# \wp db query "UPDATE ${WP_PREFIX}wp_users SET ID=${WUID} WHERE ID=1; UPDATE ${WP_PREFIX}wp_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root
\wp db query "UPDATE ${NEWPREFIX}_users SET ID=${WUID} WHERE ID=1; UPDATE ${NEWPREFIX}_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root

if [[ "$setdisplayname" = [yY] ]]; then
  \wp user update ${WUID} --display_name=${WPADMIN_DISPLAYNAME} --allow-root
fi

# add index on autoload
\wp db query "ALTER TABLE ${NEWPREFIX}_options ADD INDEX autoload_idx (autoload)" --allow-root

# change permalinks out of the box
\wp rewrite structure '/%post_id%/%postname%/' --allow-root

if [[ "$WPCLI_SUPERCACHEPLUGIN" = [yY] && ! -f /root/.wp-cli/commands/super-cache/cli.php ]]; then
  # https://github.com/wp-cli/wp-super-cache-cli.git
  mkdir -p /root/.wp-cli/commands
  time git clone https://github.com/wp-cli/wp-super-cache-cli.git /root/.wp-cli/commands/super-cache
  echo "require:" > /root/.wp-cli/config.yml
  echo "  - commands/super-cache/cli.php" >> /root/.wp-cli/config.yml
elif [ -f /root/.wp-cli/commands/super-cache/cli.php ]; then
  cd /root/.wp-cli/commands/super-cache
  git stash
  git pull
  cd /home/nginx/domains/${vhostname}/public${WPSUBDIR}
fi

if [[ "$responsivetheme" = [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp theme install responsive --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
fi
 
chown nginx:nginx /home/nginx/domains/${vhostname}/public
chown -R nginx:nginx /home/nginx/domains/${vhostname}/public

cd /home/nginx/domains/${vhostname}/public${WPSUBDIR}

chmod 0770 wp-content
chmod 0400 readme.html
rm -rf readme.html

if [[ "$wpscache" = 'fastcgicache' ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install nginx-helper --activate --allow-root
  \wp option update rt_wp_nginx_helper_options '{"enable_purge":1,"cache_method":"enable_fastcgi","enable_map":0,"enable_log":0,"enable_stamp":1,"purge_homepage_on_edit":1,"purge_homepage_on_del":1,"purge_archive_on_edit":1,"purge_archive_on_del":1,"purge_archive_on_new_comment":1,"purge_archive_on_deleted_comment":1,"purge_page_on_mod":1,"purge_page_on_new_comment":1,"purge_page_on_deleted_comment":1,"purge_method":"unlink_files","purge_url":"","redis_hostname":"127.0.0.1","redis_port":"6379","redis_prefix":"nginx-cache:"}' --format=json --allow-root
  \wp option get rt_wp_nginx_helper_options --format=json --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  #\wp plugin install snipe-nginx-cache --activate --allow-root
  #cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install autoptimize --activate --allow-root
  configure_autoptimize
  if [[ "$WP_AUTOPTIMIZE_GZIP" = [yY] ]]; then
    # https://community.centminmod.com/threads/15314/
    # https://github.com/centminmod/autoptimize-gzip
    echo
    echo "configure autoptimize-gzip https://community.centminmod.com/threads/15314/"
    mkdir -p "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip"
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/autoptimize-gzip.php" https://github.com/centminmod/autoptimize-gzip/raw/master/autoptimize-gzip.php
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/index.html" https://github.com/centminmod/autoptimize-gzip/raw/master/index.html
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/readme.md" https://github.com/centminmod/autoptimize-gzip/blob/master/readme.md
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/LICENSE" https://github.com/centminmod/autoptimize-gzip/raw/master/LICENSE
    chown -R nginx:nginx "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip"
    \wp plugin activate autoptimize-gzip --allow-root
    \wp plugin status autoptimize-gzip --allow-root
  fi
  cecho "------------------------------------------------------------" $boldgreen
fi

# installed + activated by default
# wp super cache
if [[ "$wpscache" = [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-super-cache --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-super-cache-clear-cache-menu --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
fi

# wp cache enabler
if [[ "$wpscache" = [nN] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install cache-enabler --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen 
  # \wp plugin install optimus --activate --allow-root
  echo "configure cache-enabler"
  \wp option update cache-enabler '{"expires":6,"new_post":1,"new_comment":1,"webp":0,"clear_on_upgrade":1,"compress":1,"excl_ids":"","excl_regexp":"","excl_cookies":"","excl_querystrings":"","minify_html":0}' --format=json --allow-root
  \wp option get cache-enabler --format=json --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install autoptimize --activate --allow-root
  configure_autoptimize
  if [[ "$WP_AUTOPTIMIZE_GZIP" = [yY] ]]; then
    # https://community.centminmod.com/threads/15314/
    # https://github.com/centminmod/autoptimize-gzip
    echo
    echo "configure autoptimize-gzip https://community.centminmod.com/threads/15314/"
    mkdir -p "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip"
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/autoptimize-gzip.php" https://github.com/centminmod/autoptimize-gzip/raw/master/autoptimize-gzip.php
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/index.html" https://github.com/centminmod/autoptimize-gzip/raw/master/index.html
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/readme.md" https://github.com/centminmod/autoptimize-gzip/blob/master/readme.md
    wget -4 -cnv -O "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/LICENSE" https://github.com/centminmod/autoptimize-gzip/raw/master/LICENSE
    chown -R nginx:nginx "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/autoptimize-gzip"
    \wp plugin activate autoptimize-gzip --allow-root
    \wp plugin status autoptimize-gzip --allow-root
  fi
  cecho "------------------------------------------------------------" $boldgreen
fi

# redis nginx cache
if [[ "$wpscache" = 'redis' ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install nginx-helper --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  touch "/home/nginx/domains/${vhostname}/public/wp-content/uploads/nginx-helper/nginx.log"
  chmod 0660 "/home/nginx/domains/${vhostname}/public/wp-content/uploads/nginx-helper/nginx.log"
  chown nginx:nginx "/home/nginx/domains/${vhostname}/public/wp-content/uploads/nginx-helper/nginx.log"
fi

cecho "------------------------------------------------------------" $boldgreen
\wp plugin install sucuri-scanner --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
\wp plugin install disable-xml-rpc --activate --allow-root

if [[ "$WPPLUGINS_ALL" = [yY] ]]; then
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install cdn-enabler --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install rocket-lazy-load --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-security-scan --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install limit-login-attempts --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-updates-notifier --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install no-longer-in-directory --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # \wp plugin install google-sitemap-generator --activate --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-optimize --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  # \wp plugin install wp-smushit --activate --allow-root
  # cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install tpc-memory-usage --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install gtmetrix-for-wordpress --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install p3-profiler --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wordpress-seo --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install updraftplus --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install google-analytics-for-wordpress --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install heartbeat-control --activate --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  
  # installed but disabled by default
  # cecho "------------------------------------------------------------" $boldgreen
  # \wp plugin install nginx-helper --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install query-monitor --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install go-newrelic --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install db-cache-reloaded-fix --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install google-authenticator --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install addthis-smart-layers --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install search-regex --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install disable-emojis --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install wp-user-avatar --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  \wp plugin install lazy-load-for-videos --allow-root
  cecho "------------------------------------------------------------" $boldgreen
  #\wp plugin install disable-xml-rpc-pingback --allow-root
  #cecho "------------------------------------------------------------" $boldgreen
fi # WPPLUGINS_ALL=y

# remove hello plugin
\wp plugin uninstall hello --allow-root

# allows user to add custom wp-cli commands after initiall install to
# further customise their installation. you create a custom file at
# /etc/centminmod/customwp_domain.com.inc where domain.com is the 
# intended centmin.sh menu option 22 vhost site domain for your wp
# installation. Then in that file add your wp-cli commands to the file
# wrapped in a shell function named mywpcmds() { yourcode }. i.e. for
# wp language install https://community.centminmod.com/posts/26045/
# the below check will check for /etc/centminmod/customwp_domain.com.inc
# file and then source include it into this routine and run the shell
# function named mywpcmds which contains your custom wp-cli cmds that will
# trigger and run
if [ -f "${CONFIGSCANBASE}/customwp_${vhostname}.inc" ]; then
    # default is at /etc/centminmod/customwp_${vhostname}.inc
    source "${CONFIGSCANBASE}/customwp_${vhostname}.inc"
    mywpcmds
fi

# update all plugins to make sure they are latest available
\wp plugin update --all --allow-root
echo
\wp plugin status --allow-root
cecho "------------------------------------------------------------" $boldgreen

if [[ -f /root/.wp-cli/commands/super-cache/cli.php && "$cacheenabler" != [yY] ]]; then
  \wp super-cache enable --allow-root
fi

chown -R nginx:nginx /home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/plugins/

# sucuri-scanner change SUCURI_DATA_STORAGE path above web root
# /home/nginx/domains/${vhostname}/sucuri_data_storage
if [ -d "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/uploads/sucuri" ]; then
  sucuri_datapath=$(echo "/home/nginx/domains/${vhostname}/sucuri_data_storage" | sed -e 's|/|\\\/|g')
  rm -rf "/home/nginx/domains/${vhostname}/sucuri_data_storage"
  mkdir -p "/home/nginx/domains/${vhostname}/sucuri_data_storage"
  chown nginx:nginx "/home/nginx/domains/${vhostname}/sucuri_data_storage"
  sed -i "s|\"}|\",\"sucuriscan_datastore_path\":\"$sucuri_datapath\"}|" "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/uploads/sucuri/sucuri-settings.php"
  sed -i 's|/|\\/|g' "/home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/uploads/sucuri/sucuri-settings.php"
  ls -lah "/home/nginx/domains/${vhostname}/sucuri_data_storage"
  \cp -Rpf /home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/uploads/sucuri/* "/home/nginx/domains/${vhostname}/sucuri_data_storage"
  cat "/home/nginx/domains/${vhostname}/sucuri_data_storage/sucuri-settings.php"
fi

# write permissions for log
if [ -f wp-content/plugins/tpc-memory-usage/logs/tpcmem.log ]; then
  chmod 0660 wp-content/plugins/tpc-memory-usage/logs/tpcmem.log
fi

# fix tcpmem.css incorrect reference to images
if [ -f wp-content/plugins/tpc-memory-usage/css/tpcmem.css ]; then
  sed -i 's|(images\/|(..\/images\/|g' wp-content/plugins/tpc-memory-usage/css/tpcmem.css
fi

if [[ "$wpscache" = 'fastcgicache' ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|^  try_files \$cache_enabler_uri|  #try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|    #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|    try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|include /usr/local/nginx/conf/php-wpsc.conf;|include /usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        # insert /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf include file
        sed -i "s| #include /usr/local/nginx/conf/wpincludes/wpfc.domain.com/wpcacheenabler_wpfc.domain.com.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;\n  #include /usr/local/nginx/conf/wpincludes/wpfc.domain.com/wpcacheenabler_wpfc.domain.com.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|^  try_files \$cache_enabler_uri|  #try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|    #try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|    try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|include /usr/local/nginx/conf/php-wpsc.conf;|include /usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        # insert /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf include file
        sed -i "s| #include /usr/local/nginx/conf/wpincludes/wpfc.domain.com/wpcacheenabler_wpfc.domain.com.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf;\n  #include /usr/local/nginx/conf/wpincludes/wpfc.domain.com/wpcacheenabler_wpfc.domain.com.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
    fi
fi

if [[ "$wpscache" = [nN] ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|#try_files \$cache_enabler_uri|try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf|  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|#try_files \$cache_enabler_uri|try_files \$cache_enabler_uri|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
    fi
fi

if [[ "$wpscache" = 'redis' ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|#try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|include /usr/local/nginx/conf/php-wpsc.conf;|#include /usr/local/nginx/conf/php-wpsc.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.conf
        sed -i "s|#include /usr/local/nginx/conf/php-rediscache.conf;|include /usr/local/nginx/conf/php-rediscache.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.conf
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^  #include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;|  include /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf;|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|^  include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|try_files /wp-content/cache/supercache/\$http_host|#try_files /wp-content/cache/supercache/\$http_host|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|#try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|try_files \$uri \$uri/ ${WPSUBDIR}/index.php?\$args;|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|include /usr/local/nginx/conf/php-wpsc.conf;|#include /usr/local/nginx/conf/php-wpsc.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
        sed -i "s|#include /usr/local/nginx/conf/php-rediscache.conf;|include /usr/local/nginx/conf/php-rediscache.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
    fi
fi

if [[ "$wpscache" = [nN] ]]; then
  # cache enabler expiry cronjob setup
    DELAY=$(echo ${RANDOM:0:3})
    crontab -l > cronjoblist
    mkdir -p /home/nginx/domains/${vhostname}/cronjobs
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-before-wp-cacheenabler.txt
    # only insert cronjob if one doesn't already exist
    if [[ -z $(grep "$vhostname cacheenabler cron" cronjoblist) ]]; then
      echo "16 23 * * * echo \"$vhostname cacheenabler cron\"; sleep ${DELAY}s ; rm -rf /home/nginx/domains/${vhostname}/public${WPSUBDIR}/wp-content/cache/cache-enabler/* > /dev/null 2>&1" >> cronjoblist
    fi
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-after-wp-cacheenabler.txt
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

if [[ "$wpscache" = [nN] ]]; then
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
echo "-------------------------------------------------------------------------"
echo "Do you want to uninstall/delete WP install for ${vhostname}"
echo "This will delete all data from /home/nginx/domains/${vhostname}"
echo "including any non-wordpress data installed at /home/nginx/domains/${vhostname}"
echo "This script will NOT delete the database, you will have to manually remove the"
echo "database named: $DB"
echo "Please backup your MySQL database called $DB before deleting"
echo "-------------------------------------------------------------------------"
read -ep "Uninstall WP Install For ${vhostname} [y/n]: " uninstall
echo
if [[ "\$uninstall" != [yY] ]]; then
  exit
fi

rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.conf
rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
rm -rf /home/nginx/domains/${vhostname}
rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/wpsupercache_${vhostname}.conf
rm -rf /root/tools/wp_updater_${vhostname}.sh
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key
rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr
rm -rf /usr/local/nginx/conf/ssl/${vhostname}
rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/rediscache_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/wpfastcgicache_include_${vhostname}.conf
rm -rf /root/.acme.sh/${vhostname}
if [ -f /usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf ]; then
  rm -rf /usr/local/nginx/conf/wpincludes/${vhostname}/php-fastcgicache.conf
fi
if [ -f /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf ]; then
  echo "adjust /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf"
  echo "/usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf before"
  cat /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf
  echo
  echo "/usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf after adjustment"
  sed -i "/$fastcgicache_name/d" /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf
  cat /usr/local/nginx/conf/wpfastcgi_cache_path_includes.conf
fi
crontab -l > cronjoblist
sed -i "/wp_updater_${vhostname}.sh/d" cronjoblist
sed -i "/\/${vhostname}\/wp-cron.php/d" cronjoblist
sed -i "/$vhostname cacheenabler cron/d" cronjoblist
crontab cronjoblist
rm -rf cronjoblist
pure-pw userdel $ftpuser >/dev/null 2>&1
service nginx restart
END

chmod 0700 /root/tools/wp_uninstall_${vhostname}.sh

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Created wp_updater_${vhostname}.sh script" $boldyellow
  cecho "/root/tools/wp_updater_${vhostname}.sh" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen


if [ -f "/usr/local/nginx/conf/wpincludes/$vhostname/emailfrom.ini" ]; then
  # SETWPADMINEMAIL_FROM=yourcustom-from-emailaddress
  . "/usr/local/nginx/conf/wpincludes/$vhostname/emailfrom.ini"
fi

if [[ "$SETWPADMINEMAIL_FROM" ]]; then
  WPADMINEMAIL_FROM="$SETWPADMINEMAIL_FROM"
else
  WPADMINEMAIL_FROM="$WPADMINEMAIL"
fi

cat > "/root/tools/wp_updater_${vhostname}.sh" <<ENDA
#!/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin:/root/bin:/bin
EMAIL=$WPADMINEMAIL
EMAILFROM=$WPADMINEMAIL_FROM
DT=\$(date +"%d%m%y-%H%M%S")

{
cd /home/nginx/domains/${vhostname}/public${WPSUBDIR}
echo "/home/nginx/domains/${vhostname}/public${WPSUBDIR}"
#/usr/bin/wp cli update --allow-root
echo "update wp-cli"
rm -rf /usr/bin/wp
wget -${ipv_forceopt}cnv --no-check-certificate https://raw.github.com/wp-cli/builds/gh-pages/phar/wp-cli.phar -O /usr/bin/wp --tries=3
chmod 0700 /usr/bin/wp
/usr/bin/wp --info --allow-root
/usr/bin/wp plugin status --allow-root
/usr/bin/wp plugin update --all --allow-root | tee .wpcli-status
#/usr/bin/wp core check-update --allow-root

# for major core updates uncomment 3 lines directly below
#/usr/bin/wp core update --allow-root
#/usr/bin/wp core update-db --allow-root
#/usr/bin/wp core update --allow-root

# for minor core updates uncomment 3 lines directly below
#/usr/bin/wp core update --minor --allow-root
#/usr/bin/wp core update-db --allow-root
#/usr/bin/wp core update --minor --allow-root

chown -R nginx:nginx /home/nginx/domains/${vhostname}/public${WPSUBDIR}
if [[ -f .wpcli-status && ! "\$(grep -w 'Plugin already updated' .wpcli-status)" ]]; then
  exec 99>/tmp/wp_updater_phpfpm.lock
  if flock -n -x 99; then
    # only do nginx + php-fpm restarts when zend opcache caching is enabled and revalidation
    # frequency is greater than 10 seconds set by opcache.revalidate_freq php setting
    if [[ "\$(php --ri "Zend Opcache" >/dev/null 2>&1; echo \$?)" -eq '0' && "\$(php --ri "Zend Opcache" | awk '/opcache.revalidate_freq/ {print \$3}')" -gt '10' ]]; then
      service nginx reload
    fi
  rm -rf /tmp/wp_updater_phpfpm.lock
  else
  echo "already detected running process"
  echo "skipping nginx reload"
  fi
fi
rm -rf .wpcli-status
} 2>&1 | tee /home/nginx/domains/${vhostname}/log/wp_updater-\${DT}.log
cat -v /home/nginx/domains/${vhostname}/log/wp_updater-\${DT}.log | /usr/bin/tr -cd '\11\12\15\40-\176' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g" | dos2unix | mail -r \$EMAILFROM -s "Wordpress WP-CLI Auto Update \$(date)" \$EMAIL
#rm -rf /home/nginx/domains/${vhostname}/log/wp_updater-\${DT}.log
ENDA

chmod 0700 /root/tools/wp_updater_${vhostname}.sh

if [[ -z "$(crontab -l 2>&1 | grep wp_updater_${vhostname}.sh)" ]]; then
    # generate random number of seconds to delay cron start
    # making sure wp_updater for several wordpress nginx installs
    # do not run at very same time during cron scheduling
    crontab -l > cronjoblist

    DELAY=$(echo ${RANDOM:0:3})
    DELAY_DUPCHECK=$(echo $DELAY | cut -c1,2)
    echo $DELAY
    echo $DELAY_DUPCHECK

    # while [[ "$(crontab -l | awk '/ sleep / {print $7}' | grep "$DELAY_DUPCHECK")" ]]; do
    #   cat cronjoblist | awk '/ sleep / {print $7}' | while read s; do 
    #     CRONSLEEP_CHECK=$(echo $s | cut -c1,2)
    #     echo "CRONSLEEP_CHECK = $CRONSLEEP_CHECK"
    #     if [[ "$CRONSLEEP_CHECK" -eq "$DELAY_DUPCHECK" ]]; then
    #       DELAY=$(echo ${RANDOM:0:3})
    #       DELAY_DUPCHECK=$(echo $DELAY | cut -c1,2)
    #       echo $DELAY
    #       echo $DELAY_DUPCHECK
    #     fi
    #   done
    #   echo "final $DELAY"
    #   echo "final $DELAY_DUPCHECK"
    # done

    # ensure random sleep times do not overlap https://community.centminmod.com/posts/51310/
    while [[ "$(cat cronjoblist | awk '/ sleep / {print $7}' | grep "$DELAY_DUPCHECK")" ]]; do
      DELAY=$(echo ${RANDOM:0:3})
      DELAY_DUPCHECK=$(echo $DELAY | cut -c1,2)
      echo $DELAY
      echo $DELAY_DUPCHECK
      echo "calculated $DELAY"
      echo "calculated $DELAY_DUPCHECK"
    done
    echo "final $DELAY"
    echo "final $DELAY_DUPCHECK"

    mkdir -p /home/nginx/domains/${vhostname}/cronjobs
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-before-wp-updater.txt
    echo "0 */8 * * * sleep ${DELAY}s ;/root/tools/wp_updater_${vhostname}.sh >/dev/null 2>&1" >> cronjoblist
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-after-wp-updater.txt
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

echo 
cecho "-------------------------------------------------------------" $boldyellow
if [ -f "${SCRIPT_DIR}/tools/autoprotect.sh" ]; then
  "${SCRIPT_DIR}/tools/autoprotect.sh"
fi

cmservice nginx reload

cmservice php-fpm restart
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

if [ -f /tmp/setupwp.log ]; then
  rm -rf /tmp/setupwp.log
fi

if [[ "$LETSENCRYPT_DETECT" = [yY] ]]; then
  if [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'le' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wptest"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wptest
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'led' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wptestd"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wptestd
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'lelive' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wplive"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wplive
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'lelived' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wplived"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" wplived
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  fi
fi

if [[ "$wpcli_ssldefault" = '1' ]]; then
  \wp option update home "https://${vhostname}${WPSUBDIR}"  --allow-root
  \wp option update siteurl "https://${vhostname}${WPSUBDIR}"  --allow-root
fi

{
echo 
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
cecho "-------------------------------------------------------------" $boldyellow
echo "FTP hostname : $CNIP"
echo "FTP port : 21"
echo "FTP mode : FTP (explicit SSL)"
echo "FTP Passive (PASV) : ensure is checked/enabled"
echo "FTP username created for $vhostname : $ftpuser"
echo "FTP password created for $vhostname : $ftppass"
fi
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname created successfully" $boldwhite
echo
if [[ "$wpcli_ssldefault" != '1' ]]; then
  cecho "domain: http://$vhostname" $boldyellow
  cecho "vhost conf file for $vhostname created: /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
fi
if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
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
if [[ "$setdisplayname" = [yY] ]]; then
  cecho "Wordpress Admin Display Name: $WPADMIN_DISPLAYNAME" $boldyellow
fi

if [[ "$disablepwdprotect" != [yY] ]]; then
  if [[ -f /usr/local/nginx/conf/htpasswd.sh && -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
    echo  
    cecho "Wordpress wp-login.php password protection info:" $boldyellow
    cecho "wp-login.php protection file /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
    cecho "wp-login.php protection Username: $HTUSER" $boldyellow
    cecho "wp-login.php protection Password: $HTPASS" $boldyellow
    if [[ "$wpcli_ssldefault" = '1' ]]; then
      cecho "https://${HTUSER}:${HTPASS}@${vhostname}${WPSUBDIR}/wp-login.php" $boldyellow
    else
      cecho "http://${HTUSER}:${HTPASS}@${vhostname}${WPSUBDIR}/wp-login.php" $boldyellow
    fi
    echo
    cecho "Resetting wp-login.php protection:" $boldyellow
    cecho "Step 1. remove protection file at /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
    cecho "     rm -rf /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
    cecho "Step 2. run command:" $boldyellow
    cecho "     /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin YOURUSERNAME YOURPASSWORD" $boldyellow
    cecho "Step 3. restart Nginx + PHP-FPM services" $boldyellow
    cecho "     nprestart" $boldyellow
  fi
elif [[ "$disablepwdprotect" = [yY] ]]; then
    # disable wp-login.php password protection if user opts to do so
    sed -i "s|auth_basic \"Private\"|#auth_basic \"Private\"|" /usr/local/nginx/conf/conf.d/$vhostname.conf >/dev/null 2>&1
    sed -i "s|auth_basic_user_file \/home\/nginx\/domains\/$vhostname\/htpasswd_wplogin|#auth_basic_user_file \/home\/nginx\/domains\/$vhostname\/htpasswd_wplogin|" /usr/local/nginx/conf/conf.d/$vhostname.conf >/dev/null 2>&1
    if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
      sed -i "s|auth_basic \"Private\"|#auth_basic \"Private\"|" /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf >/dev/null 2>&1
      sed -i "s|auth_basic_user_file \/home\/nginx\/domains\/$vhostname\/htpasswd_wplogin|#auth_basic_user_file \/home\/nginx\/domains\/$vhostname\/htpasswd_wplogin|" /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf >/dev/null 2>&1
    fi
    service nginx reload
fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost ssl files listing at: /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/ssl/${vhostname} | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'
fi

echo
cecho "------------------------------------------------------------" $boldgreen
cecho "To complete setup:" $boldyellow
if [[ "$wpscache" = 'fastcgicache' ]]; then
  cecho "  1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
  2. Settings Menu > Nginx Helper set options and hit Save All Changes
  3. Settings Menu > Autoptimize Main Tab set options and hit Save Changes
  4. Settings Menu > Autoptimize Extra Tab set options and hit Save Changes"
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
elif [[ "$wpscache" = [nN] ]]; then
  cecho "  1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
  2. Settings Menu > Cache Enabler set options and hit Save Changes (https://community.centminmod.com/threads/15039/)
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
elif [[ "$wpscache" = [yY] ]]; then
  cecho "  1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
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
elif [[ "$wpscache" = 'redis' ]]; then
  cecho "  1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
  2. Settings Menu > Nginx Helper Enable Purging set Caching Method to Redis Cache, & set Purging Conditions
  3. Settings Menu > CDN Enabler and set CDN up or disable plugin
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
fi
} 2>&1 | tee /tmp/setupwp.log
cat -v /tmp/setupwp.log | /usr/bin/tr -cd '\11\12\15\40-\176' | perl -pe 's/\x1b.*?[mGKH]//g' | dos2unix | mail -r $WPADMINEMAIL_FROM -s "${vhostname} Wordpress Installed `date`" $WPADMINEMAIL
cp -a /tmp/setupwp.log "${CENTMINLOGDIR}/setupwp-$(date +"%d%m%y-%H%M%S").log"
rm -rf /tmp/setupwp.log

  # control variables after vhost creation
  # whether cloudflare.conf include file is uncommented (enabled) or commented out (disabled)
  if [[ "$VHOSTCTRL_CLOUDFLAREINC" = [yY] ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
      sed -i "s|^  #include \/usr\/local\/nginx\/conf\/cloudflare.conf;|  include \/usr\/local\/nginx\/conf\/cloudflare.conf;|g" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
      sed -i "s|^  #include \/usr\/local\/nginx\/conf\/cloudflare.conf;|  include \/usr\/local\/nginx\/conf\/cloudflare.conf;|g" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
    fi
  fi
  # whether autoprotect-$vhostname.conf include file is uncommented (enabled) or commented out (disabled)
  if [[ "$VHOSTCTRL_AUTOPROTECTINC" = [nN] ]]; then
    if [ -f "/usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf" ]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|^  include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|  #include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|g" "/usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf"
      fi
      if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^  include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|  #include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|g" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
      fi
    fi
  fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname wordpress setup successfully" $boldwhite
cecho "$vhostname setup info log saved at: " $boldwhite
cecho "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${vhostname}_wordpress_addvhost.log" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""

else

echo ""
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""

fi

}