#!/bin/bash
###############################################################
# standalone nginx vhost creation script for centminmod.com
# .09 beta01 and higher written by George Liu
# modified for wordpress setup
################################################################
branchname='123.09beta01'
#CUR_DIR="/usr/local/src/centminmod-${branchname}"
CUR_DIR="/usr/local/src/centminmod"

DEBUG='n'
CENTMINLOGDIR='/root/centminlogs'
DT=$(date +"%d%m%y-%H%M%S")
CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
DIR_TMP=/svr-setup
OPENSSL_VERSION=$(awk -F "'" /'^OPENSSL_VERSION/ {print $2}' $CUR_DIR/centmin.sh)
# CURRENTIP=$(echo $SSH_CLIENT | awk '{print $1}')
# CURRENTCOUNTRY=$(curl -s${CURL_TIMEOUTS} ipinfo.io/$CURRENTIP/country)
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
################################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}
###############################################################
if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

if [ ! -d /root/tools ]; then
  mkdir -p /root/tools
fi

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
  HTTPTWO=y
  LISTENOPT='ssl http2'
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

if [ ! -d "$CUR_DIR" ]; then
  echo "Error: directory $CUR_DIR does not exist"
  echo "check $0 branchname variable is set correctly"
  exit 1
fi

usage() { 
# if pure-ftpd service running = 0
if [[ "$(ps aufx | grep -v grep | grep 'pure-ftpd' 2>&1>/dev/null; echo $?)" = '0' ]]; then
  echo
  cecho "Usage: $0 [-d yourdomain.com] [-s y|n] [-u ftpusername]" $boldyellow 1>&2; 
  echo; 
  cecho "  -d  yourdomain.com or subdomain.yourdomain.com" $boldyellow
  cecho "  -s  ssl self-signed create = y or n" $boldyellow
  cecho "  -u  your FTP username" $boldyellow
  echo
  cecho "  example:" $boldyellow
  echo
  cecho "  $0 -d yourdomain.com -s y -u ftpusername" $boldyellow
  echo
  exit 1;
else
  echo
  cecho "Usage: $0 [-d yourdomain.com] [-s y|n]" $boldyellow 1>&2; 
  echo; 
  cecho "  -d  yourdomain.com or subdomain.yourdomain.com" $boldyellow
  cecho "  -s  ssl self-signed create = y or n" $boldyellow
  echo
  cecho "  example:" $boldyellow
  echo
  cecho "  $0 -d yourdomain.com -s y" $boldyellow  
  echo  
  exit 1;
fi
}

while getopts ":d:s:u:" opt; do
    case "$opt" in
	d)
	 vhostname=${OPTARG}
   RUN=y
	;;
	s)
	 sslconfig=${OPTARG}
   RUN=y
	;;
	u)
	 ftpuser=${OPTARG}
   RUN=y
	 if [ "$ftpuser" ]; then
	 	PUREFTPD_DISABLED=n
	 	if [ ! -f /usr/bin/pure-pw ]; then
      PUREFTPD_INSTALLED=n
      # echo "Error: pure-ftpd not installed"
    else
      autogenpass=y
    fi
	 fi
	;;
	*)
	 usage
	;;
     esac
done

if [[ "$vhostssl" && "$sslconfig" ]]; then
  RUN=y
fi

if [[ "$RUN" = [yY] && "$DEBUG" = [yY] ]]; then
  echo
  cecho "$vhostname" $boldyellow
  cecho "$sslconfig" $boldyellow
  cecho "$ftpuser" $boldyellow
fi

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi

cmservice() {
        servicename=$1
        action=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "service ${servicename} $action"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                service ${servicename} $action
        fi
        else
        echo "systemctl $action ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl $action ${servicename}.service
        fi
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

pureftpinstall() {
	if [ ! -f /usr/bin/pure-pw ]; then
		echo "pure-ftpd not installed"
		echo "installing pure-ftpd"
		CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')

		yum -q -y install pure-ftpd
		cmchkconfig pure-ftpd on
		sed -i 's/LF_FTPD = "10"/LF_FTPD = "3"/g' /etc/csf/csf.conf
		sed -i 's/PORTFLOOD = \"\"/PORTFLOOD = \"21;tcp;5;300\"/g' /etc/csf/csf.conf

		echo "configuring pure-ftpd for virtual user support"
		# tweak /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/# UnixAuthentication  /UnixAuthentication  /' /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/VerboseLog                  no/VerboseLog                  yes/' /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/# PureDB                        \/etc\/pure-ftpd\/pureftpd.pdb/PureDB                        \/etc\/pure-ftpd\/pureftpd.pdb/' /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/#CreateHomeDir               yes/CreateHomeDir               yes/' /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/# TLS                      1/TLS                      2/' /etc/pure-ftpd/pure-ftpd.conf
		sed -i 's/# PassivePortRange          30000 50000/PassivePortRange    3000 3050/' /etc/pure-ftpd/pure-ftpd.conf

		# fix default file/directory permissions
		sed -i 's/Umask                       133:022/Umask                       137:027/' /etc/pure-ftpd/pure-ftpd.conf

		# ensure TLS Cipher preference protects against poodle attacks

		sed -i 's/# TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:+SSLv3/TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:!SSLv3/' /etc/pure-ftpd/pure-ftpd.conf

		if [[ ! "$(grep 'TLSCipherSuite' /etc/pure-ftpd/pure-ftpd.conf)" ]]; then
			echo 'TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:!SSLv3' >> /etc/pure-ftpd/pure-ftpd.conf
		fi

		# check if /etc/pure-ftpd/pureftpd.passwd exists
		if [ ! -f /etc/pure-ftpd/pureftpd.passwd ]; then
			touch /etc/pure-ftpd/pureftpd.passwd
			chmod 0600 /etc/pure-ftpd/pureftpd.passwd
			pure-pw mkdb
		fi

		# generate /etc/pure-ftpd/pureftpd.pdb
		if [ ! -f /etc/pure-ftpd/pureftpd.pdb ]; then
			pure-pw mkdb
		fi

		# check tweaks were made
		echo
		cat /etc/pure-ftpd/pure-ftpd.conf | egrep 'UnixAuthentication|VerboseLog|PureDB |CreateHomeDir|TLS|PassivePortRange|TLSCipherSuite'

		echo
		echo "generating self-signed ssl certificate..."
		echo "FTP client needs to use FTP (explicit SSL) mode"
		echo "to connect to server's main ip address on port 21"
		sleep 4
		# echo "just hit enter at each prompt until complete"
		# setup self-signed ssl certs
		mkdir -p /etc/ssl/private
		openssl req -x509 -days 7300 -sha256 -nodes -subj "/C=US/ST=California/L=Los Angeles/O=Default Company Ltd/CN==$CNIP" -newkey rsa:1024 -keyout /etc/pki/pure-ftpd/pure-ftpd.pem -out /etc/pki/pure-ftpd/pure-ftpd.pem
		chmod 600 /etc/pki/pure-ftpd/*.pem
		openssl x509 -in /etc/pki/pure-ftpd/pure-ftpd.pem -text -noout
		echo 
		# ls -lah /etc/ssl/private/
		ls -lah /etc/pki/pure-ftpd
		echo
		echo "self-signed ssl cert generated"
			
		echo "pure-ftpd installed"
		cmservice pure-ftpd restart
		csf -r

		echo
		echo "check /etc/pure-ftpd/pureftpd.passwd"
		ls -lah /etc/pure-ftpd/pureftpd.passwd

		echo
		echo "check /etc/pure-ftpd/pureftpd.pdb"
		ls -lah /etc/pure-ftpd/pureftpd.pdb

		echo
	fi
}

sslvhost() {

cecho "---------------------------------------------------------------" $boldyellow
cecho "SSL Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo ""

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

funct_nginxaddvhost() {
PUREUSER=nginx
PUREGROUP=nginx
CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
if [[ "$PUREFTPD_INSTALLED" = [nN] ]]; then
  pureftpinstall
fi

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

cecho "---------------------------------------------------------------" $boldyellow
cecho "Nginx Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow

# read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

if [[ "$sslconfig" = [yY] ]] || [[ "$sslconfig" = 'le' ]]; then
  echo
  vhostssl=y
  # read -ep "Create a self-signed SSL certificate Nginx vhost? [y/n]: " vhostssl
fi

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  if [ ! -f /usr/sbin/cracklib-check ]; then
    yum -y -q install cracklib
  fi
  if [ ! -f /usr/bin/pwgen ]; then
    yum -y -q install pwgen
  fi
  echo
  # read -ep "Create FTP username for vhost domain (enter username): " ftpuser
  # read -ep "Do you want to auto generate FTP password (recommended) [y/n]: " autogenpass
  # echo

  if [[ "$autogenpass" = [yY] ]]; then
    ftppass=$(pwgen -1cnys 21)
    echo "FTP password auto generated: $ftppass"
  fi # autogenpass
fi

echo ""

if [ ! -d /home/nginx/domains/$vhostname ]; then

dbsetup  

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo "${ftppass}" ; echo "${ftppass}" ) | pure-pw useradd "$ftpuser" -u $PUREUSER -g $PUREGROUP -d "/home/nginx/domains/$vhostname"
  pure-pw mkdb
fi

cat > "/home/nginx/domains/$vhostname/public/index.html" <<END
<html>
<head>
<title>$vhostname</title>
</head>
<body>
<p>Welcome to $vhostname. This index.html page can be removed.</p>

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
  HTPASS="p${HTWPLOGINSALTB}y${HTWPLOGIN}"
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

\cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php-wpsc.conf
sed -i "s|fastcgi_param  SERVER_NAME        \$server_name;|fastcgi_param  SERVER_NAME        \$http_host;|" /usr/local/nginx/conf/php-wpsc.conf

# Setting up Nginx mapping

if [[ "$vhostssl" = [yY] ]]; then
  sslvhost
fi

if [[ "$vhostssl" = [yY] ]]; then

  if [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]; then
      # check /svr-setup/openssl-1.0.2f/crypto/chacha20poly1305/chacha20.c exists
      OPEENSSL_CFPATCHED='y'
  fi

if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]] || [[ "$OPEENSSL_CFPATCHED" = [yY] ]]; then
  if [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]; then
    CHACHACIPHERS='EECDH+CHACHA20-draft:EECDH+CHACHA20:'
  else
    CHACHACIPHERS='EECDH+CHACHA20:'
  fi
else
  CHACHACIPHERS=""
fi

# main non-ssl vhost at yourdomain.com.conf
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
  #add_header X-Xss-Protection "1; mode=block" always;
  #add_header X-Content-Type-Options "nosniff" always;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  # prevent access to ./directories and files
  location ~ (?:^|/)\. {
   deny all;
  }

include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;  

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  #try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # Wordpress Permalinks
  try_files \$uri \$uri/ /index.php?q=\$uri&\$args;  

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
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

  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}EECDH+ECDSA+AESGCM:EECDH+ECDSA+SHA384:EECDH+ECDSA+SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA:!DES-CBC3-SHA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER
  # HTTP Public Key Pinning Header uncomment only one that applies include or exclude domains. 
  # You'd want to include subdomains if you're using SSL wildcard certificates
  # include subdomain
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400; includeSubDomains';
  # exclude subdomains
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400';
  #add_header Strict-Transport-Security "max-age=31536000; includeSubdomains;";
  #add_header X-Frame-Options SAMEORIGIN;
  #add_header X-Xss-Protection "1; mode=block" always;
  #add_header X-Content-Type-Options "nosniff" always;
  $COMP_HEADER;
  ssl_buffer_size 1369;
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

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  # prevent access to ./directories and files
  location ~ (?:^|/)\. {
   deny all;
  }

include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;  

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  #try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # Wordpress Permalinks
  try_files \$uri \$uri/ /index.php?q=\$uri&\$args;  

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
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
  #add_header X-Xss-Protection "1; mode=block" always;
  #add_header X-Content-Type-Options "nosniff" always;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # for wordpress super cache plugin
  #try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

  # Wordpress Permalinks
  try_files \$uri \$uri/ /index.php?q=\$uri&\$args;  

  }

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwprpc burst=45 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
}

  include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
  include /usr/local/nginx/conf/php-wpsc.conf;
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

location ~* /(\.|wp-config\.php|wp-config\.txt|changelog\.txt|readme\.txt|readme\.html|license\.txt) { deny all; }
EEF

cat > "/usr/local/nginx/conf/wpsupercache_${vhostname}.conf" <<EFF
set \$cache_uri \$request_uri;

if (\$request_method = POST) { set \$cache_uri 'null cache'; }

if (\$query_string != "") { set \$cache_uri 'null cache'; }

if (\$request_uri ~* "/(\?add-to-cart=|cart|my-account|checkout|addons|wp-admin/.*|xmlrpc\.php|wp-.*\.php|index\.php|feed/|sitemap(_index)?\.xml|[a-z0-9_-]+-sitemap([0-9]+)?\.xml)") { set \$cache_uri 'null cache'; }

if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in") { set \$cache_uri 'null cache'; }
EFF

######### Wordpress Manual Install no WP-CLI ######################
# only proceed in creating vhost if VHOSTNAME directory exist
if [[ -d "/home/nginx/domains/${vhostname}/public" ]]; then

  cecho "---------------------------------------------------------------" $boldgreen
  cecho "Setup Wordpress + Super Cache (vhost only disabled by default) for $vhostname" $boldyellow
  cecho "---------------------------------------------------------------" $boldgreen

  cd /home/nginx/domains/${vhostname}

  # download wordpress latest zip
  rm -rf latest.zip
  wget -cnv https://wordpress.org/latest.zip
  unzip -q latest.zip
  cd wordpress
  \cp -Rf * /home/nginx/domains/${vhostname}/public
  rm -rf wordpress
  cd /home/nginx/domains/${vhostname}/public
  cp wp-config-sample.php wp-config.php
  sed -i "/DB_NAME/s/'[^']*'/'${DB}'/2" wp-config.php
  sed -i "/DB_USER/s/'[^']*'/'${DBUSER}'/2" wp-config.php
  sed -i "/DB_PASSWORD/s/'[^']*'/'$DBPASS'/2" wp-config.php

#set WP salts
perl -i -pe'
  BEGIN {
    @chars = ("a" .. "z", "A" .. "Z", 0 .. 9);
    push @chars, split //, "!@#$%^&*()-_ []{}<>~\`+=,.;:/?|";
    sub salt { join "", map $chars[ rand @chars ], 1 .. 64 }
  }
  s/put your unique phrase here/salt()/ge
' wp-config.php  
 
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
    mkdir -p /home/nginx/domains/${vhostname}/cronjobs
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-before-wp-cron.txt
    echo "*/15 * * * * sleep ${DELAY}s ; wget -O - -q -t 1 http://${vhostname}/wp-cron.php?doing_wp_cron=1 > /dev/null 2>&1" >> cronjoblist
    cp cronjoblist /home/nginx/domains/${vhostname}/cronjobs/cronjoblist-after-wp-cron.txt
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

# change admin userid from 1 to a random 6 digit number
# WP_PREFIX=$(wp eval 'echo $GLOBALS["table_prefix"];')
# WUID=$(echo $RANDOM$RANDOM |cut -c1-6)
# mysql -e "UPDATE wp_users SET ID=${WUID} WHERE ID=1; UPDATE wp_usermeta SET user_id=${WUID} WHERE user_id=1" ${DB}

  chown nginx:nginx /home/nginx/domains/${vhostname}/public
  chown -R nginx:nginx /home/nginx/domains/${vhostname}/public
  
  cd /home/nginx/domains/${vhostname}/public
  
  chmod 0770 wp-content
  chmod 0400 readme.html
  rm -rf readme.html

  mkdir -p wp-content/cache/
  mkdir -p wp-content/cache/supercache/
  chown -R nginx:nginx wp-content/
  chmod -R 0770 wp-content/cache/
  chmod 0750 wp-content
  umask 022

fi # wp install if web root exists
######### Wordpress Manual Install no WP-CLI ######################

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
crontab -l > cronjoblist
sed -i "/wp_updater_${vhostname}.sh/d" cronjoblist
sed -i "/\/${vhostname}\/wp-cron.php/d" cronjoblist
crontab cronjoblist
rm -rf cronjoblist
service nginx restart
END

chmod 0700 /root/tools/wp_uninstall_${vhostname}.sh

#   cecho "------------------------------------------------------------" $boldgreen
#   cecho "Created wp_updater_${vhostname}.sh script" $boldyellow
#   cecho "/root/tools/wp_updater_${vhostname}.sh" $boldyellow
#   cecho "------------------------------------------------------------" $boldgreen

# cat > "/root/tools/wp_updater_${vhostname}.sh" <<ENDA
# #!/bin/bash
# PATH=/usr/lib64/ccache:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin:/root/bin
# EMAIL=$WPADMINEMAIL

# {
# cd /home/nginx/domains/${vhostname}/public
# echo "/home/nginx/domains/${vhostname}/public"
# /usr/bin/wp cli update --allow-root
# /usr/bin/wp plugin status --allow-root
# /usr/bin/wp plugin update --all --allow-root
# } 2>&1 | mail -s "Wordpress WP-CLI Auto Update \$(date)" \$EMAIL
# ENDA

# chmod 0700 /root/tools/wp_updater_${vhostname}.sh

# if [[ -z "$(crontab -l 2>&1 | grep wp_updater_${vhostname}.sh)" ]]; then
#     # generate random number of seconds to delay cron start
#     # making sure wp_updater for several wordpress nginx installs
#     # do not run at very same time during cron scheduling
#     DELAY=$(echo ${RANDOM:0:3})
#     crontab -l > cronjoblist
#     echo "0 */8 * * * sleep ${DELAY}s ;/root/tools/wp_updater_${vhostname}.sh 2>/dev/null" >> cronjoblist
#     crontab cronjoblist
#     rm -rf cronjoblist
#     crontab -l
# fi

echo 
cecho "-------------------------------------------------------------" $boldyellow
if [ -f "${SCRIPT_DIR}/autoprotect.sh" ]; then
  "${SCRIPT_DIR}/autoprotect.sh"
fi

service nginx restart

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

FINDUPPERDIR=$(dirname $SCRIPT_DIR)
if [ -f "$FINDUPPERDIR/addons/acmetool.sh" ] && [[ "$sslconfig" = 'le' ]]; then
  echo
  cecho "-------------------------------------------------------------" $boldyellow
  echo "ok: $FINDUPPERDIR/addons/acmetool.sh"
  echo ""$FINDUPPERDIR/addons/acmetool.sh" issue "$vhostname""
  "$FINDUPPERDIR/addons/acmetool.sh" issue "$vhostname"
  cecho "-------------------------------------------------------------" $boldyellow
  echo
fi

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
# cecho "------------------------------------------------------------" $boldgreen
# cecho "Wordpress Auto Updater created at:" $boldyellow
# cecho "  /root/tools/wp_updater_${vhostname}.sh" $boldyellow
# cecho "cronjob set for every 8 hours update (3x times per day)" $boldyellow
# cecho "------------------------------------------------------------" $boldgreen
# echo
cecho "Wordpress domain: $vhostname" $boldyellow
cecho "Wordpress DB Name: $DB" $boldyellow
cecho "Wordpress DB User: $DBUSER" $boldyellow
cecho "Wordpress DB Pass: $DBPASS" $boldyellow
# cecho "Wordpress Admin User ID: ${WUID}" $boldyellow
# cecho "Wordpress Admin User: $WPADMINUSER" $boldyellow
# cecho "Wordpress Admin Pass: $WPADMINPASS" $boldyellow
# cecho "Wordpress Admin Email: $WPADMINEMAIL" $boldyellow

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

else

echo ""
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""

fi


}

if [[ "$RUN" = [yY] ]]; then
  {
    funct_nginxaddvhost
  } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nvwp.log
else
  usage
fi