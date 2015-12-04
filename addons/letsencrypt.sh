#!/bin/bash
##################################################################
# letsencrypt client standalone installer for centminmod.com
# just installs the letsencrypt client itself for initial setup
##################################################################
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

##################################################################
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

##################################################################
# Settings for centmin.sh menu option 2 and option 22 for
# the details of the self-signed SSL certificate that is auto 
# generated. The default values where vhostname variable is 
# auto added based on what you input for your site name
# 
# -subj "/C=US/ST=California/L=Los Angeles/O=${levhostname}/OU=${levhostname}/CN=${levhostname}"
# 
# You can only customise the first 5 variables for 
# C = Country 2 digit code
# ST = state 
# L = Location as in city 
# 0 = organisation
# OU = organisational unit
# 
# if left blank # defaults to same as vhostname that is your domain
# if set it overrides that
SELFSIGNEDSSL_C='US'
SELFSIGNEDSSL_ST='California'
SELFSIGNEDSSL_L='Los Angeles'
SELFSIGNEDSSL_O=''
SELFSIGNEDSSL_OU=''
##################################################################
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

##################################################################

lememstats() {
	AVAILABLE_MEM=$(egrep '^MemFree|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
	echo
	cecho "----------------------------------------------------" $boldyellow
	cecho "system memory profile: $((AVAILABLE_MEM/1024)) MB available" $boldgreen
	cecho "----------------------------------------------------" $boldyellow
	free -ml
}

getuseragent() {
	# build Centmin Mod's identifying letsencrypt user agent
	# --user-agent=
	if [[ "$CENTOS_SIX" = '6' ]]; then
		LE_OSVER=centos6
	elif [[ "$CENTOS_SEVEN" = '7' ]]; then
		LE_OSVER=centos7
	fi
	LE_USERAGENT="centminmod-$LE_OSVER-webroot"
}

python_setup() {
	if [ -f /usr/local/src/centminmod/addons/python27_install.sh ]; then
		if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/python2.7 ]]; then
			/usr/local/src/centminmod/addons/python27_install.sh install
		fi
	fi
}

leclientsetup() {
	# build letsencrypt version timestamp
	# find last github commit date to compare with current client version number
	if [ -d /root/tools/letsencrypt ]; then
		LECOMMIT_DATE=$(cd /root/tools/letsencrypt; date -d @$(git log -n1 --format="%at") +%Y%m%d)
	fi
	# setup letsencrypt client and virtualenv
	# https://community.centminmod.com/posts/19914/
	lememstats
	echo
	cecho "installing or updating letsencrypt client" $boldgreen
	echo
	python_setup
	echo
	mkdir -p /root/tools
	cd /root/tools
	if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
		# compare current letsencrypt version timestamp with last github commit date YYMMDD
		LE_CLIENTVER=$(/root/.local/share/letsencrypt/bin/letsencrypt --version 2>&1 | awk '{print $2}')
		LE_CLIENTCOMPARE=$(echo $LE_CLIENTVER | grep $LECOMMIT_DATE)
		if [[ "$LE_CLIENTCOMPARE" ]]; then
			cd letsencrypt
			git pull
		else
			rm -rf /root/tools/letsencrypt
			git clone https://github.com/letsencrypt/letsencrypt
			cd letsencrypt
		fi
	elif [ ! -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
		git clone https://github.com/letsencrypt/letsencrypt
		cd letsencrypt
	fi
		
	if [[ "$CENTOS_SIX" = '6' && -f /usr/bin/python2.7 ]]; then
		sed -i "s|--python python2|--python python2.7|" letsencrypt-auto
	fi
	# staging endpoint
	# LE_SERVER='https://acme-staging.api.letsencrypt.org/directory'
	# live and beta invitee trusted cert endpoint
	LE_SERVER='https://acme-v01.api.letsencrypt.org/directory'
	if [ -f ./letsencrypt-auto ]; then
		./letsencrypt-auto --server $LE_SERVER
	else
		cecho "./letsencrypt-auto not found" $boldgreen
	fi

	if [ ! -f /etc/letsencrypt/webroot.ini ]; then
	cecho "setup general /etc/letsencrypt/webroot.ini letsencrypt config file" $boldgreen
	touch /etc/letsencrypt/webroot.ini
cat > "/etc/letsencrypt/webroot.ini" <<EOF
# webroot.ini general config ini

rsa-key-size = 2048

# Always use the staging/testing server
#server = https://acme-staging.api.letsencrypt.org/directory

# for beta invitees
server = https://acme-v01.api.letsencrypt.org/directory

# Uncomment and update to register with the specified e-mail address
email = foo@example.com

# Uncomment to use a text interface instead of ncurses
text = True
agree-tos = True
#agree-dev-preview = True
renew-by-default = True

authenticator = webroot
EOF
	fi

	if [[ "$(grep 'foo@example.com' /etc/letsencrypt/webroot.ini)" ]]; then
		echo
		cecho "Registering an account with Letsencrypt" $boldgreen
		echo "You only do this once, so that Letsencrypt can notify &"
		echo "contact you via email regarding your SSL certificates"
		read -ep "Enter your email address to setup Letsencrypt account: " letemail

		if [ -z "$letemail" ]; then
			echo
			echo "!! Error: email address is empty"
		else
			echo
			echo "You are registering $letemail address for Letsencrypt"
		fi

		# check email domain has MX records which letsencrypt client checks for
		CHECKLE_MXEMAIL=$(echo "$letemail" | awk -F '@' '{print $2}')
		while [[ -z "$(dig -t MX +short @8.8.8.8 $CHECKLE_MXEMAIL)" || -z "$letemail" ]]; do
			echo
			if [[ -z "$(dig -t MX +short @8.8.8.8 $CHECKLE_MXEMAIL)" ]]; then
				echo "!! Error: $letemail does not have a DNS MX record !!"
			fi
			if [ -z "$letemail" ]; then
				echo "!! Error: email address is empty"
			fi
			echo
			read -ep "Re-Enter your email address to setup Letsencrypt account: " letemail
			if [ -z "$letemail" ]; then
				echo
				echo "!! Error: email address is empty"
			else
				echo
				echo "You are registering $letemail address for Letsencrypt"
			fi
			CHECKLE_MXEMAIL=$(echo "$letemail" | awk -F '@' '{print $2}')
		done

		sed -i "s|foo@example.com|$letemail|" /etc/letsencrypt/webroot.ini
		echo
	fi

if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
	lememstats
	echo
	cecho "----------------------------------------------------" $boldyellow
	cecho "letsencrypt client is installed at:" $boldgreen
	cecho "/root/.local/share/letsencrypt/bin/letsencrypt" $boldgreen
	cecho "----------------------------------------------------" $boldyellow	
	echo
fi

}

##################################################################
sslvhost() {

cecho "---------------------------------------------------------------" $boldyellow
cecho "SSL Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo ""

if [ ! -f /usr/local/nginx/conf/ssl ]; then
  mkdir -p /usr/local/nginx/conf/ssl
fi

if [ ! -f /usr/local/nginx/conf/ssl/${levhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/${levhostname}
fi

if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;  
EVS
fi

cd /usr/local/nginx/conf/ssl/${levhostname}

cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating self signed SSL certificate..." $boldgreen
cecho "CSR file can also be used to be submitted for paid SSL certificates" $boldgreen
cecho "If using for paid SSL certificates be sure to keep both private key and CSR safe" $boldgreen
cecho "creating CSR File: ${levhostname}.csr" $boldgreen
cecho "creating private key: ${levhostname}.key" $boldgreen
cecho "creating self-signed SSL certificate: ${levhostname}.crt" $boldgreen
sleep 9

if [[ -z "$SELFSIGNEDSSL_O" ]]; then
  SELFSIGNEDSSL_O="$levhostname"
else
  SELFSIGNEDSSL_O="$SELFSIGNEDSSL_O"
fi

if [[ -z "$SELFSIGNEDSSL_OU" ]]; then
  SELFSIGNEDSSL_OU="$levhostname"
else
  SELFSIGNEDSSL_OU="$SELFSIGNEDSSL_OU"
fi

openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${levhostname}.csr -keyout ${levhostname}.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${levhostname}"
openssl x509 -req -days 36500 -sha256 -in ${levhostname}.csr -signkey ${levhostname}.key -out ${levhostname}.crt

echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating backup CSR and private key for HTTP Public Key Pinning..." $boldgreen
cecho "creating CSR File: ${levhostname}-backup.csr" $boldgreen
cecho "creating private key: ${levhostname}-backup.key" $boldgreen
sleep 5

openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${levhostname}-backup.csr -keyout ${levhostname}-backup.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${levhostname}"

echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Extracting Base64 encoded information for primary and secondary" $boldgreen
cecho "private key's SPKI - Subject Public Key Information" $boldgreen
cecho "Primary private key - ${levhostname}.key" $boldgreen
cecho "Backup private key - ${levhostname}-backup.key" $boldgreen
cecho "For HPKP - HTTP Public Key Pinning hash generation..." $boldgreen
sleep 5

echo
cecho "extracting SPKI Base64 encoded hash for primary private key = ${levhostname}.key ..." $boldgreen

openssl rsa -in ${levhostname}.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-primary-pin.txt

echo
cecho "extracting SPKI Base64 encoded hash for backup private key = ${levhostname}-backup.key ..." $boldgreen

openssl rsa -in ${levhostname}-backup.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-secondary-pin.txt

echo
cecho "HTTP Public Key Pinning Header for Nginx" $boldgreen

echo
cecho "for 7 days max-age including subdomains" $boldgreen
echo
echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400; includeSubDomains';"

echo
cecho "for 7 days max-age excluding subdomains" $boldgreen
echo
echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400';"


echo
cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen

dhparamstarttime=$(date +%s.%N)

openssl dhparam -out dhparam.pem 2048

dhparamendtime=$(date +%s.%N)
DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow

}

##################################################################
deploysslvhost() {
# Setting up Nginx mapping

vhostssl=y

if [[ "$vhostssl" = [yY] ]]; then
  sslvhost
fi

if [[ "$vhostssl" = [yY] ]]; then

if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]]; then
  CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
else
  CHACHACIPHERS=""
fi

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

if [ -f "/usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf "]; then
	echo "backup existing /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf"
	cp -a /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf-backup-$DT
fi

# separate ssl vhost at yourdomain.com.ssl.conf
cat > "/usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
# server {
#   server_name ${levhostname} www.${levhostname};
#    return 302 https://\$server_name\$request_uri;
# }

server {
  listen 443 $LISTENOPT;
  server_name $levhostname www.$levhostname;

  ssl_dhparam /usr/local/nginx/conf/ssl/${levhostname}/dhparam.pem;
  ssl_certificate      /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}.crt;
  ssl_certificate_key  /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}.key;
  include /usr/local/nginx/conf/ssl_include.conf;

  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA:!DES-CBC3-SHA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER
  # HTTP Public Key Pinning Header uncomment only one that applies include or exclude domains. 
  # You'd want to include subdomains if you're using SSL wildcard certificates
  # include subdomain
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400; includeSubDomains';
  # exclude subdomains
  #add_header Public-Key-Pins 'pin-sha256="$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-primary-pin.txt)"; pin-sha256="$(cat /usr/local/nginx/conf/ssl/${levhostname}/hpkp-info-secondary-pin.txt)"; max-age=86400';
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
  #ssl_trusted_certificate /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}-trusted.crt;  

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$levhostname/log/access.log combined buffer=256k flush=60m;
  error_log /home/nginx/domains/$levhostname/log/error.log;

  root /home/nginx/domains/$levhostname/public;

  # prevent access to ./directories and files
  #location ~ (?:^|/)\. {
  # deny all;
  #}  

  location / {

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Enable for vBulletin usage WITHOUT vbSEO installed
  # More example Nginx vhost configurations at
  # http://centminmod.com/nginx_configure.html
  #try_files    \$uri \$uri/ /index.php;

  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
ESS	
}

##################################################################
lemsgdns() {
  			echo
  			cecho "To get Letsencrypt SSL certificate, you must already have updated intended" $boldgreen
  			cecho "domain vhost name's DNS A record to this server's IP addresss." $boldgreen
  			cecho "If top level domain, DNS A record is needed also for www. version of domain" $boldgreen
  			cecho "otherwise, Letsencrypt domain name validation will fail." $boldgreen
}

##################################################################
deploycert() {
	if [[ -f /etc/letsencrypt/webroot.ini && -f /root/.local/share/letsencrypt/bin/letsencrypt ]]; then
		echo
		read -ep "Enter the nginx vhostdomain you want to renew SSL cert for: " levhostname			
		echo
		if [[ -d "/home/nginx/domains/${levhostname}/public" && -f "/usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf" ]]; then
			lemsgdns			

  # check if entered vhostname is top level domain or a subdomain if top level, need the ssl certificate
  # to also cover www. version of the top level domain vhostname via a multi-domain SAN LE ssl certificate
  TOPLEVELCHECK=$(dig soa $levhostname | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//')
  if [[ "$TOPLEVELCHECK" = "$levhostname" ]]; then
    # top level domain
    TOPLEVEL=y
  elif [[ -z "$TOPLEVELCHECK" ]]; then
    # vhost dns not setup
    TOPLEVEL=z
  else
    # subdomain or non top level domain
    TOPLEVEL=n
  fi
  echo
  if [[ "$TOPLEVEL" = [yY] ]]; then
    VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $levhostname)
    VHOST_AWWWCHECK=$(dig -t A +short @8.8.8.8 www.$levhostname | grep -v $levhostname)
  else
    VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $levhostname)
  fi
  echo
  if [[ "$TOPLEVEL" = [yY] ]]; then
    cecho "$levhostname is a top level domain" $boldyellow  
    if [ "$VHOST_ACHECK" ]; then
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $levhostname is: $VHOST_ACHECK" $boldyellow
    else
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $levhostname is: $VHOST_ACHECK" $boldyellow
      cecho "!! Error: missing DNS A record for $levhostname" $boldyellow
    fi
    if [ "$VHOST_AWWWCHECK" ]; then
      cecho "current DNS A record IP address for www.$levhostname is: $VHOST_AWWWCHECK" $boldyellow
    else
      cecho "current DNS A record IP address for www.$levhostname is: $VHOST_AWWWCHECK" $boldyellow
      cecho "!! Error: missing DNS A record for www.$levhostname" $boldyellow
    fi
  elif [[ "$TOPLEVEL" = 'z' ]]; then
    cecho "!! Error: $levhostname DNS records not found or setup properly yet or $levhostname invalid" $boldyellow
  else
    cecho "$levhostname is not a top level domain" $boldyellow
    if [ "$VHOST_ACHECK" ]; then
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $levhostname is: $VHOST_ACHECK" $boldyellow
    else
      cecho "current DNS A record IP address for $levhostname is: $VHOST_ACHECK" $boldyellow
      cecho "!! Error: missing DNS A record for $levhostname" $boldyellow
    fi
  fi
  echo
  read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
  if [[ "$letabort" = [yY] ]]; then
    exit
  fi 
  read -ep "Obtain Letsencrypt Free SSL certificate (90 day expiry / renew every 60 days) ? [y/n]: " levhostssl
fi

			if [[ "$levhostssl" = [yY] ]]; then
			echo
			echo "deploying letsencrypt ssl certificate"
			echo "for existing vhost: $levhostname"
			deploysslvhost

# letsencrypt client webroot authentication to
# obtain LE ssl certificate to replace selfsigned
# SSL certificate
if [[ "$levhostssl" = [yY] ]]; then
  # leclientsetup
  if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
    echo
    cecho "obtaining Letsencrypt SSL certificate via webroot authentication..." $boldgreen
    echo
    if [[ "$TOPLEVEL" = [yY] ]]; then
      echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} -d www.${levhostname} certonly"
      /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} -d www.${levhostname} certonly
    else
      echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} certonly"
      /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} certonly
    fi
    LECHECK=$?

    if [[ "$LECHECK" = '0' ]]; then
      # setup cronjob only if letsencrypt webroot authentication was sUccessfully ran and SSL certificate obtained
      # otherwise leave original self signed SSL certificates in place
      
      # EMAIL and LOGGING for cron
      echo "EMAIL=\$(awk '/email/ {print \$3}' /etc/letsencrypt/webroot.ini)" > /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      echo "ERRORLOG=\$(tail /var/log/letsencrypt/letsencrypt.log)" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      echo "CERT=\"/etc/letsencrypt/live/\${levhostname}/cert.pem\"" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      echo "" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron

echo "if [[ -f "\$CERT" ]]; then" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  expiry=\$(openssl x509 -enddate -noout -in \$CERT | cut -d'=' -f2 | awk '{print \$2 " " \$1 " " \$4}')" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  epochExpirydate=\$(date -d"\${expiry}" +%s)" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  epochToday=\$(date +%s)" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  secondsToExpire=\$(echo \${epochExpirydate} - \${epochToday} | bc)" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  daysToExpire=\$(echo "\${secondsToExpire} / 60 / 60 / 24" | bc)" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
echo "  if [ "$daysToExpire" -lt '30' ]; then" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron

      if [[ "$TOPLEVEL" = [yY] ]]; then
        echo "    /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} -d www.${levhostname} certonly" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      else
        echo "    /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${levhostname}/public -d ${levhostname} certonly" >> /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      fi
      
      # cronjob error check and email send
cat >> "/usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron" <<CFF
    if [ \$? -ne 0 ]; then
        sleep 1; echo -e "The Lets Encrypt SSL Certificate for ${levhostname} has not been renewed! \n \n" \$ERRORLOG | mail -s "Lets Encrypt Cert Alert" \$EMAIL
      else
        /usr/bin/ngxreload
    fi
  fi
fi
exit 0
CFF

      echo
      echo "/usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron contents:"      
      cat /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
      
      if [[ -z "$(crontab -l 2>&1 | grep 'letsencrypt-${levhostname}-cron')" ]]; then
          # generate random number of seconds to delay cron start
          # making sure they do not run at very same time during cron scheduling
          echo
          echo "setup cronjob for /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron"
          DELAY=$(echo ${RANDOM:0:3})
          crontab -l > cronjoblist
          echo "10 1 */9 * * sleep ${DELAY}s ; /bin/bash /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron > /dev/null 2>&1" >> cronjoblist
          crontab cronjoblist
          rm -rf cronjoblist
          crontab -l
      fi
  
      # replace self signed ssl cert with letsencrypt ssl certificate and enable ssl stapling
      # if letsencrypt webroot authentication was sUccessfully ran and SSL certificate obtained
      # otherwise leave original self signed SSL certificates in place
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${levhostname}\/${levhostname}.crt|\/etc\/letsencrypt\/live\/${levhostname}\/fullchain.pem|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${levhostname}\/${levhostname}.key|\/etc\/letsencrypt\/live\/${levhostname}\/privkey.pem|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|#resolver |resolver |" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|#resolver_timeout|resolver_timeout|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|#ssl_stapling on|ssl_stapling on|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${levhostname}\/${levhostname}-trusted.crt|\/etc\/letsencrypt\/live\/${levhostname}\/fullchain.pem|" /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf
      cmservice nginx restart 
    fi # LECHECK
  else
    cecho "/root/.local/share/letsencrypt/bin/letsencrypt not found" $boldgreen
  fi  
fi

if [[ "$vhostssl" = [yY] ]]; then
  echo
  cecho "vhost ssl for $levhostname created successfully" $boldwhite
  echo
  cecho "domain: https://$levhostname" $boldyellow
  cecho "vhost ssl conf file for $levhostname created: /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf" $boldwhite
  cecho "/usr/local/nginx/conf/ssl_include.conf created" $boldwhite
  cecho "Self-signed SSL Certificate: /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}.crt" $boldyellow
  cecho "SSL Private Key: /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}.key" $boldyellow
  cecho "SSL CSR File: /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}.csr" $boldyellow
  cecho "Backup SSL Private Key: /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}-backup.key" $boldyellow
  cecho "Backup SSL CSR File: /usr/local/nginx/conf/ssl/${levhostname}/${levhostname}-backup.csr" $boldyellow   
  if [[ "$levhostssl" = [yY] ]] && [[ "$LECHECK" = '0' ]]; then
    echo
    cecho "Letsencrypt SSL Certificate: /etc/letsencrypt/live/${levhostname}/cert.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Private Key: /etc/letsencrypt/live/${levhostname}/privkey.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Chain: /etc/letsencrypt/live/${levhostname}/chain.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Full Chain: /etc/letsencrypt/live/${levhostname}/fullchain.pem" $boldyellow
    cecho "Letsencrypt $levhostname cronjob file: /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron" $boldyellow
  fi 
fi

fi # if levhostssl=y line 291

		else
			echo
			echo "! Error: $levhostname public webroot and/or nginx ssl vhost config file missing"
			echo "directory not found: /home/nginx/domains/${levhostname}/public"
			echo "ssl vhost not found: /usr/local/nginx/conf/conf.d/${levhostname}.ssl.conf"
			echo			
		fi
	else
		echo
		echo "! Error: letsencrypt client is not installed or setup properly"
		echo "  please run first:"
		echo "         $0 setup"
	fi
}

##################################################################
renewcert() {
	if [[ -f /etc/letsencrypt/webroot.ini && -f /root/.local/share/letsencrypt/bin/letsencrypt ]]; then
		echo
		read -ep "Enter the nginx vhostdomain you want to renew SSL cert for: " levhostname
		echo
		if [ -f "/usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron" ]; then
			lemsgdns			
			echo "renewing existing letsencrypt SSL certificate"
			echo "for $levhostname"
			/bin/bash /usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron
			echo
		else
			echo
			echo "! Error: $levhostname auto renew cron file missing"
			echo "/usr/local/nginx/conf/ssl/${levhostname}/letsencrypt-${levhostname}-cron not found"
			echo
		fi
	else
		echo
		echo "! Error: letsencrypt client is not installed or setup properly"
		echo "  please run first:"
		echo "         $0 setup"		
	fi
}

##################################################################
case "$1" in
	setup)
		starttime=$(date +%s.%N)
		{
		leclientsetup
		} 2>&1 | tee ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log
		
		endtime=$(date +%s.%N)
		
		INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
		echo "" >> ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log
		echo "Letsencrypt Addon Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log
		;;
	deploy)
		starttime=$(date +%s.%N)
		{
		deploycert
		} 2>&1 | tee ${CENTMINLOGDIR}/letsencrypt-deploycert-{$levhostname}_${DT}.log
		
		endtime=$(date +%s.%N)
		
		INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
		echo "" >> ${CENTMINLOGDIR}/letsencrypt-deploycert-{$levhostname}_${DT}.log
		echo "Letsencrypt Deploy Cert Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/letsencrypt-deploycert-{$levhostname}_${DT}.log
		;;
	renew)
		starttime=$(date +%s.%N)
		{
		renewcert
		} 2>&1 | tee ${CENTMINLOGDIR}/letsencrypt-renewcert-{$levhostname}_${DT}.log
		
		endtime=$(date +%s.%N)
		
		INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
		echo "" >> ${CENTMINLOGDIR}/letsencrypt-renewcert-{$levhostname}_${DT}.log
		echo "Letsencrypt Renew Cert Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/letsencrypt-renewcert-{$levhostname}_${DT}.log
		;;		
	*)
		echo
		echo "run setup first to install & configure letsencrypt client"
		echo "if it's the first time you have used this script"
		echo
		echo "$0 {setup|deploy|renew}"
		echo
		;;
esac
exit