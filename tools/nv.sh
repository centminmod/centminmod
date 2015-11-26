#!/bin/bash
###############################################################
# standalone nginx vhost creation script for centminmod.com
# .08 beta03 and higher written by George Liu
################################################################
branchname='123.09beta01'
#CUR_DIR="/usr/local/src/centminmod-${branchname}"
CUR_DIR="/usr/local/src/centminmod"

DEBUG='n'
# CURRENTIP=$(echo $SSH_CLIENT | awk '{print $1}')
# CURRENTCOUNTRY=$(curl -s ipinfo.io/$CURRENTIP/country)
CENTMINLOGDIR='/root/centminlogs'
DT=`date +"%d%m%y-%H%M%S"`
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

if [ ! -d "$CUR_DIR" ]; then
  echo "Error: directory $CUR_DIR does not exist"
  echo "check $0 branchname variable is set correctly"
  exit 1
fi

usage() { 
# if pure-ftpd service running = 0
if [[ "$(ps aufx | grep -v grep | grep 'pure-ftpd' 2>&1>/dev/null; echo $?)" = '0' ]]; then
  echo
  cecho "Usage: $0 [-d yourdomain.com] [-s y|n|le] [-u ftpusername]" $boldyellow 1>&2; 
  echo; 
  cecho "  -d  yourdomain.com or subdomain.yourdomain.com" $boldyellow
  cecho "  -s  ssl self-signed create = y or n or le (for letsencrypt ssl certs)" $boldyellow
  cecho "  -u  your FTP username" $boldyellow
  echo
  cecho "  example:" $boldyellow
  echo
  cecho "  $0 -d yourdomain.com -s y -u ftpusername" $boldyellow
  cecho "  $0 -d yourdomain.com -s le -u ftpusername" $boldyellow
  echo
  exit 1;
else
  echo
  cecho "Usage: $0 [-d yourdomain.com] [-s y|n|le]" $boldyellow 1>&2; 
  echo; 
  cecho "  -d  yourdomain.com or subdomain.yourdomain.com" $boldyellow
  cecho "  -s  ssl self-signed create = y or n or le (for letsencrypt ssl certs)" $boldyellow
  echo
  cecho "  example:" $boldyellow
  echo
  cecho "  $0 -d yourdomain.com -s y" $boldyellow
  cecho "  $0 -d yourdomain.com -s le" $boldyellow
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
  LE_SERVER='https://acme-staging.api.letsencrypt.org/directory'
  # live and beta invitee trusted cert endpoint
  # LE_SERVER='https://acme-v01.api.letsencrypt.org/directory'
  if [ -f ./letsencrypt-auto ]; then
    ./letsencrypt-auto --agree-dev-preview --server $LE_SERVER
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
server = https://acme-staging.api.letsencrypt.org/directory

# for beta invitees
# server = https://acme-v01.api.letsencrypt.org/directory

# Uncomment and update to register with the specified e-mail address
email = foo@example.com

# Uncomment to use a text interface instead of ncurses
text = True
agree-tos = True
agree-dev-preview = True
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

cecho "---------------------------------------------------------------" $boldyellow
cecho "Nginx Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow

# read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

if [[ "$sslconfig" = [yY] || "$sslconfig" = 'le' ]]; then
  echo
  vhostssl=y
  # read -ep "Create a self-signed SSL certificate Nginx vhost? [y/n]: " vhostssl
fi

if [[ "$sslconfig" = 'le' ]]; then
  getuseragent
  echo
  cecho "To get Letsencrypt SSL certificate, you must already have updated intended" $boldgreen
  cecho "domain vhost name's DNS A record to this server's IP addresss." $boldgreen
  cecho "If top level domain, DNS A record is needed also for www. version of domain" $boldgreen
  cecho "otherwise, Letsencrypt domain name validation will fail." $boldgreen
  # check if entered vhostname is top level domain or a subdomain if top level, need the ssl certificate
  # to also cover www. version of the top level domain vhostname via a multi-domain SAN LE ssl certificate
  TOPLEVELCHECK=$(dig soa $vhostname | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//')
  if [[ "$TOPLEVELCHECK" = "$vhostname" ]]; then
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
    VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $vhostname)
    VHOST_AWWWCHECK=$(dig -t A +short @8.8.8.8 www.$vhostname | grep -v $vhostname)
  else
    VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $vhostname)
  fi
  echo
  if [[ "$TOPLEVEL" = [yY] ]]; then
    cecho "$vhostname is a top level domain" $boldyellow  
    if [ "$VHOST_ACHECK" ]; then
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $vhostname is: $VHOST_ACHECK" $boldyellow
    else
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $vhostname is: $VHOST_ACHECK" $boldyellow
      cecho "!! Error: missing DNS A record for $vhostname" $boldyellow
    fi
    if [ "$VHOST_AWWWCHECK" ]; then
      cecho "current DNS A record IP address for www.$vhostname is: $VHOST_AWWWCHECK" $boldyellow
    else
      cecho "current DNS A record IP address for www.$vhostname is: $VHOST_AWWWCHECK" $boldyellow
      cecho "!! Error: missing DNS A record for www.$vhostname" $boldyellow
    fi
  elif [[ "$TOPLEVEL" = 'z' ]]; then
    cecho "!! Error: $vhostname DNS records not found or setup properly yet or $vhostname invalid" $boldyellow
  else
    cecho "$vhostname is not a top level domain" $boldyellow
    if [ "$VHOST_ACHECK" ]; then
      cecho "your server IP address: $CNIP" $boldyellow
      cecho "current DNS A record IP address for $vhostname is: $VHOST_ACHECK" $boldyellow
    else
      cecho "current DNS A record IP address for $vhostname is: $VHOST_ACHECK" $boldyellow
      cecho "!! Error: missing DNS A record for $vhostname" $boldyellow
    fi
  fi
  echo
  read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
  if [[ "$letabort" = [yY] ]]; then
    exit
  fi 
  read -ep "Obtain Letsencrypt Free SSL certificate (90 day expiry / renew every 60 days) ? [y/n]: " levhostssl
  if [[ "$levhostssl" = [yY] ]]; then
    vhostssl=y
  fi
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

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo ${ftppass} ; echo ${ftppass} ) | pure-pw useradd $ftpuser -u $PUREUSER -g $PUREGROUP -d /home/nginx/domains/$vhostname
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

# main non-ssl vhost at yourdomain.com.conf
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<ENSS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
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
  listen 443 $LISTENOPT;
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

else

cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<END
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
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
  #try_files		\$uri \$uri/ /index.php;

  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
END

fi

echo 
cecho "-------------------------------------------------------------" $boldyellow
service nginx restart
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

# letsencrypt client webroot authentication to
# obtain LE ssl certificate to replace selfsigned
# SSL certificate
if [[ "$levhostssl" = [yY] ]]; then
  leclientsetup
  if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
    echo
    cecho "obtaining Letsencrypt SSL certificate via webroot authentication..." $boldgreen
    echo
    if [[ "$TOPLEVEL" = [yY] ]]; then
      echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} -d www.${vhostname} certonly"
      /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} -d www.${vhostname} certonly
    else
      echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} certonly"
      /root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} certonly
    fi
    LECHECK=$?

    if [[ "$LECHECK" = '0' ]]; then
      # setup cronjob only if letsencrypt webroot authentication was sUccessfully ran and SSL certificate obtained
      # otherwise leave original self signed SSL certificates in place

      # EMAIL and LOGGING for cron
      echo "EMAIL=$(awk '/email/ {print $3}' /etc/letsencrypt/webroot.ini)" > /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron
      echo "ERRORLOG=\$(tail /var/log/letsencrypt/letsencrypt.log)" >> /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron      

      if [[ "$TOPLEVEL" = [yY] ]]; then
        echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} -d www.${vhostname} certonly" >> /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron
      else
        echo "/root/.local/share/letsencrypt/bin/letsencrypt -c /etc/letsencrypt/webroot.ini --user-agent $LE_USERAGENT --webroot-path /home/nginx/domains/${vhostname}/public -d ${vhostname} certonly" >> /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron
      fi
      
      # cronjob error check and email send
cat >> "/usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron" <<CFF
if [ \$? -ne 0 ]; then
    sleep 1; echo -e "The Lets Encrypt SSL Certificate for ${vhostname} has not been renewed! \n \n" \$ERRORLOG | mail -s "Lets Encrypt Cert Alert" \$EMAIL
  else
    /usr/bin/ngxreload
fi
exit 0
CFF

      echo
      echo "/usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron contents:"
      cat /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron
      
      if [[ -z "$(crontab -l 2>&1 | grep 'letsencrypt-${vhostname}-cron')" ]]; then
          # generate random number of seconds to delay cron start
          # making sure they do not run at very same time during cron scheduling
          echo
          echo "setup cronjob for /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron"
          DELAY=$(echo ${RANDOM:0:3})
          crontab -l > cronjoblist
          echo "15 1 1 */2 * sleep ${DELAY}s ; bash /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron > /dev/null 2>&1" >> cronjoblist
          crontab cronjoblist
          rm -rf cronjoblist
          crontab -l
      fi
  
      # replace self signed ssl cert with letsencrypt ssl certificate and enable ssl stapling
      # if letsencrypt webroot authentication was sUccessfully ran and SSL certificate obtained
      # otherwise leave original self signed SSL certificates in place
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/etc\/letsencrypt\/live\/${vhostname}\/fullchain.pem|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/etc\/letsencrypt\/live\/${vhostname}\/privkey.pem|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|#resolver |resolver |" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|#resolver_timeout|resolver_timeout|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|#ssl_stapling on|ssl_stapling on|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/etc\/letsencrypt\/live\/${vhostname}\/fullchain.pem|" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
      cmservice nginx restart 
    fi # LECHECK
  else
    cecho "/root/.local/share/letsencrypt/bin/letsencrypt not found" $boldgreen
  fi  
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
if [[ "$sslconfig" = [yY] || "$sslconfig" = 'le' ]]; then
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
  if [[ "$levhostssl" = [yY] ]] && [[ "$LECHECK" = '0' ]]; then
    echo
    cecho "Letsencrypt SSL Certificate: /etc/letsencrypt/live/${vhostname}/cert.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Private Key: /etc/letsencrypt/live/${vhostname}/privkey.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Chain: /etc/letsencrypt/live/${vhostname}/chain.pem" $boldyellow
    cecho "Letsencrypt SSL Certificate Full Chain: /etc/letsencrypt/live/${vhostname}/fullchain.pem" $boldyellow
    cecho "Letsencrypt $vhostname cronjob file: /usr/local/nginx/conf/ssl/${vhostname}/letsencrypt-${vhostname}-cron" $boldyellow
  fi     
fi
echo
cecho "upload files to /home/nginx/domains/$vhostname/public" $boldwhite
cecho "vhost log files directory is /home/nginx/domains/$vhostname/log" $boldwhite

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

if [[ "$sslconfig" = [yY] || "$sslconfig" = 'le' ]]; then
echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost ssl files listing at: /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/ssl/${vhostname} | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'
fi

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Commands to remove ${vhostname}" $boldwhite
echo
cecho " rm -rf /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
if [[ "$sslconfig" = [yY] || "$sslconfig" = 'le' ]]; then
cecho " rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" $boldwhite
fi
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
cecho " rm -rf /home/nginx/domains/$vhostname" $boldwhite
cecho " service nginx restart" $boldwhite
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

if [[ "$RUN" = [yY] ]]; then
  {
    funct_nginxaddvhost
  } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv.log
else
  usage
fi