#!/bin/bash
###############################################################
# written by George Liu (eva2000) centminmod.com
###############################################################
# variables
###############################################################
ACMEVER='1.0.13'
DT=$(date +"%d%m%y-%H%M%S")
ACMEDEBUG='n'
ACMEDEBUG_LOG='y'
ACMEBINARY='/root/.acme.sh/acme.sh'
ACMEGITURL='https://github.com/Neilpang/acme.sh.git'
ACMEBACKUPDIR='/usr/local/nginx/conf/acmevhostbackup'
ACMESH_BACKUPDIR='/home/acmesh-backups'
ACMECERTHOME='/root/.acme.sh/'
# options for KEYLENGTH
# 2048, 3072, 4096, 8192, ec-256, ec-384
KEYLENGTH='2048'
# every 60 days for auto renewal of SSL certificate
RENEWDAYS='21'

CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
MAIN_HOSTNAMEVHOSTFILE='/usr/local/nginx/conf/conf.d/virtual.conf'
MAIN_HOSTNAMEVHOSTSSLFILE='/usr/local/nginx/conf/conf.d/virtual.ssl.conf'
MAIN_HOSTNAME=$(awk '/server_name / {print $2}' "$MAIN_HOSTNAMEVHOSTFILE" | awk 'gsub(";$"," ")')
OPENSSL_VERSION=$(ls -rt "$DIR_TMP" | awk '/openssl-1/' | grep -v 'tar.gz' | tail -1 | sed -e 's|openssl-||')
###############################################################
# pushover API
# to ensure these settings persist DO NOT change them in this
# script but set these variables in persistent config file at
# /etc/centminmod/acmetool-config.ini
PUSHALERT='n'
papiurl=https://api.pushover.net/1/messages.json
# registered pushover.net users will find their Pushover email
# aliases for notifications at https://pushover.net/
pushover_email=''
###############################################################
# Cloudflare DNS API for DNS Mode
# https://github.com/Neilpang/acme.sh/tree/master/dnsapi
# login to your Cloudflare account to get your API Key in
# My Settings section of your account
# to ensure these settings persist DO NOT change them in this
# script but set these variables in persistent config file at
# /etc/centminmod/acmetool-config.ini
# set to CF_DNSAPI='y' and fill in CF_KEY and CF_EMAIL settings
CF_DNSAPI='n'
CF_KEY=''
CF_EMAIL=''
###############################################################
UNATTENDED='n'
NOTICE='y'
CHECKVERSION='y'
SCRIPTCHECKURL='https://acmetool.centminmod.com'
###############################################################
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

#####################
checkver( ){
  LATESTVER=$(curl -s $SCRIPTCHECKURL | head -n1 | cut -c1-6| tr -d '\r')
  CURRENTVER=$ACMEVER
  if [[ "$CURRENTVER" != "$LATESTVER" ]]; then
    echo
    echo "------------------------------------------------------------------------------"
    echo "Version Check:"
    echo "------------------------------------------------------------------------------"
    echo "!!!  there maybe a newer version of $0 available  !!!"
    echo "https://community.centminmod.com/posts/34492/"
    echo "update using centmin.sh menu option 23 submenu option 2"
    echo
    echo "Always ensure Current Version is higher or equal to Latest Version"
    echo "------------------------------------------------------------------------------"
    echo "Current acmetool.sh Version: $CURRENTVER"
    echo "Latest acmetool.sh Version: $LATESTVER"
    echo "------------------------------------------------------------------------------"
    echo
  fi
}

if [[ "$CHECKVERSION" = [yY] && "$UNATTENDED" != [yY] ]]; then
  checkver
fi

if [[ "$NOTICE" = [yY] && "$UNATTENDED" != [yY] ]]; then
  echo
  echo "-------------------------------------------------"
  echo "acmetool.sh is in beta testing phase"
  echo "please read & provide bug reports &"
  echo "feedback for this tool via the forums"
  echo "https://community.centminmod.com/posts/34492/"
  echo "-------------------------------------------------"
  echo
  read -ep "continue [y/n] ? " _proceed
  if [[ "$_proceed" != [yY] ]]; then
    echo
    echo "aborting..."
    echo
    exit
  fi
fi

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(awk '{ print $3 }' /etc/redhat-release | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    OLS='y'
fi

if [ ! -d "$DIR_TMP" ]; then
  mkdir -p "$DIR_TMP"
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

if [ ! -d "$ACMEBACKUPDIR" ]; then
  mkdir -p "$ACMEBACKUPDIR"
fi

if [ ! -d "$ACMESH_BACKUPDIR" ]; then
  mkdir -p "$ACMESH_BACKUPDIR"
fi

if [ -f "/etc/centminmod/acmetool-config.ini" ]; then
  . "/etc/centminmod/acmetool-config.ini"
fi

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  . "/etc/centminmod/custom_config.inc"
fi

###############################################################
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
# functions
#####################
if [[ "$ACMEDEBUG" = [yY] && "$ACMEDEBUG_LOG" = [nN] ]]; then
  ACMEDEBUG_OPT='--debug 2'
else
  ACMEDEBUG_OPT=""
fi

if [[ "$ACMEDEBUG_LOG" = [yY] && "$ACMEDEBUG" = [nN] ]] || [[ "$ACMEDEBUG_LOG" = [yY] && "$ACMEDEBUG" = [yY] ]]; then
  ACMEDEBUG_OPT="--log ${CENTMINLOGDIR}/acmetool.sh-debug-log-$DT.log --log-level 2"
else
  ACMEDEBUG_OPT=""
fi

if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
  ECCFLAG=' --ecc'
  ECC_SUFFIX='-ecc'
  ECC_ACMEHOMESUFFIX='_ecc'
else
  ECCFLAG=""
  ECC_SUFFIX=""
  ECC_ACMEHOMESUFFIX=""
fi

  # extended custom nginx log format = main_ext for nginx amplify metric support
  # https://github.com/nginxinc/nginx-amplify-doc/blob/master/amplify-guide.md#additional-nginx-metrics
  if [ -f /usr/local/nginx/conf/nginx.conf ]; then
    if [[ "$(grep 'main_ext' /usr/local/nginx/conf/nginx.conf)" ]]; then
      NGX_LOGFORMAT='main_ext'
    else
      NGX_LOGFORMAT='combined'
    fi
  else
    NGX_LOGFORMAT='combined'
  fi

#####################
listlogs() {
  echo
  echo "log files saved at ${CENTMINLOGDIR}"
  ls -lAhrt "${CENTMINLOGDIR}" | grep "${DT%??}"
  echo
}

#####################
checkdate() {
  if [ -d /root/.acme.sh ]; then
   echo "----------------------------------------------"
   echo "nginx installed"
   echo "----------------------------------------------"
   for c in $(find /usr/local/nginx/conf/ssl/ -name '*-acme.cer' -o -name '*-acme-ecc.cer'); do
    if [ -f $c ]; then
      expiry=$(openssl x509 -enddate -noout -in $c | cut -d'=' -f2 | awk '{print $2 " " $1 " " $4}')
      fingerprint=$(openssl x509 -fingerprint -noout -in $c | sed 's|:||g' | awk -F "=" '// {print $2}')
      epochExpirydate=$(date -d"${expiry}" +%s)
      epochToday=$(date +%s)
      secondsToExpire=$(echo ${epochExpirydate} - ${epochToday} | bc)
      daysToExpire=$(echo "${secondsToExpire} / 60 / 60 / 24" | bc)
      echo
      echo "$c"
      echo "SHA1 Fingerprint=${fingerprint}"
      echo "certificate expires in $daysToExpire days on $expiry"
    fi
   done
   echo
   echo "----------------------------------------------"
   echo "acme.sh obtained"
   echo "----------------------------------------------"
   for ca in $(find ${ACMECERTHOME} -name '*.cer'| egrep -v 'fullchain|ca'); do
    if [ -f $ca ]; then
      expiry=$(openssl x509 -enddate -noout -in $ca | cut -d'=' -f2 | awk '{print $2 " " $1 " " $4}')
      fingerprint=$(openssl x509 -fingerprint -noout -in $ca | sed 's|:||g' | awk -F "=" '// {print $2}')
      epochExpirydate=$(date -d"${expiry}" +%s)
      epochToday=$(date +%s)
      secondsToExpire=$(echo ${epochExpirydate} - ${epochToday} | bc)
      daysToExpire=$(echo "${secondsToExpire} / 60 / 60 / 24" | bc)
      echo
      echo "$ca"
      echo "SHA1 Fingerprint=${fingerprint}"
      conf=$(echo $ca | sed 's|.cer$|.conf|')
      if [[ "$(grep -q 'acme-staging.api' $conf; echo $?)" != '0' ]]; then
        echo "[ below certifcate transparency link is only valid ~1hr after issuance ]"
        echo "https://crt.sh/?sha1=${fingerprint}"
      fi
      echo "certificate expires in $daysToExpire days on $expiry"
    fi
   done
  echo
  fi
}

#####################
check_dns() {
  vhostname_dns="$1"
    # if CHECKIDN = 0 then internationalized domain name which not supported by letsencrypt
    CHECKIDN=$(echo $vhostname_dns | grep '^xn--' >/dev/null 2>&1; echo $?)
    if [[ "$CHECKIDN" = '0' ]]; then
      TOPLEVELCHECK=$(dig soa @8.8.8.8 $vhostname_dns | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//' | idn)
    else
      TOPLEVELCHECK=$(dig soa @8.8.8.8 $vhostname_dns | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//')
    fi
    if [[ "$TOPLEVELCHECK" = "$vhostname_dns" ]]; then
      # top level domain
      TOPLEVEL=y
    elif [[ -z "$TOPLEVELCHECK" ]]; then
      # vhost dns not setup
      TOPLEVEL=z
      if [[ "$(echo $vhostname_dns | grep -o "\." | wc -l)" -le '1' ]]; then
        TOPLEVEL=y
      else
        TOPLEVEL=n
      fi
    else
      # subdomain or non top level domain
      TOPLEVEL=n
    fi
}

#####################
backup_acme() {
  vhostbackupname="$1"
  \cp -af "${ACMECERTHOME}${vhostbackupname}${ECC_ACMEHOMESUFFIX}" "${ACMESH_BACKUPDIR}/${vhostbackupname}${ECC_ACMEHOMESUFFIX}-${DT}"
}

#####################
pushover_alert() {
  if [[ "$PUSHALERT" = [yY] ]]; then
    push_vhostname="$1"
    dnspush=$2
    dnslog=$3
    if [[ ! -z "$pushover_email" && ! -f "$dnslog" ]] && [[ "$dnspush" != 'dns' || "$dnspush" != 'dnscf' ]]; then
      acme_domainconf="${ACMECERTHOME}${push_vhostname}${ECC_ACMEHOMESUFFIX}/${push_vhostname}.conf"
      acmecreate_date=$(grep "^Le_CertCreateTimeStr" "$acme_domainconf" | cut -d '=' -f 2)
      acmenextrenew_date=$(grep "^Le_NextRenewTimeStr" "$acme_domainconf" | cut -d '=' -f 2)
      echo "
      $push_vhostname SSL Cert Created: $acmecreate_date
      $push_vhostname SSL Cert Next Renewal Date: $acmenextrenew_date
      "| mail -s "$push_vhostname SSL Cert Setup `date`" -r "$pushover_email" "$pushover_email"
    elif [[ -f "$dnslog" ]] && [[ "$dnspush" = 'dns' ]]; then
      cat "$dnslog" | grep -A30 'Add the following TXT record' | perl -pe 's/\x1b.*?[mGKH]//g' | sed 's/\[[^]]*\]//g' | egrep -v 'Please be aware that you prepend|resulting subdomain|and retry again' | mail -s "$push_vhostname DNS Mode Validation Instructions `date`" -r "$pushover_email" "$pushover_email"
    elif [[ "$dnspush" = 'dnscf' ]]; then
      cat "$dnslog" | perl -pe 's/\x1b.*?[mGKH]//g' | sed 's/\[[^]]*\]//g' | grep -A60 'Verify each domain' | sed '/-----BEGIN CERTIFICATE-----/,/-----END CERTIFICATE-----/d' | mail -s "$push_vhostname DNS Mode Validation via Cloudflare API `date`" -r "$pushover_email" "$pushover_email"
    fi
  fi
}

#####################
renew_all() {
  is_live=$1
  for d in $(ls -F "${ACMECERTHOME}" | grep [^.].*[.].*/$ ) ; do
    d=$(echo $d | cut -d '/' -f 1)
    (
      if [[ "$is_live" = 'live' ]]; then
        "$SCRIPT_DIR/acmetool.sh" renew "$d" live
      else
        "$SCRIPT_DIR/acmetool.sh" renew "$d"
      fi
    )
  done
}

#####################
getuseragent() {
  _dnsagent=$1
  # build Centmin Mod's identifying letsencrypt user agent
  # --user-agent=
  if [[ "$CENTOS_SIX" = '6' ]]; then
    LE_OSVER=centos6
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    LE_OSVER=centos7
  fi
  if [[ "$_dnsagent" != 'dns' ]]; then
    LE_USERAGENT="centminmod-$LE_OSVER-acmesh-webroot"
  else
    LE_USERAGENT="centminmod-$LE_OSVER-acmesh-dns"
  fi
}

#####################
reloadcmd_setup() {
  if [ ! -f "${ACMECERTHOME}reload.sh" ]; then
  echo
  echo "setup ${ACMECERTHOME}reload.sh"
cat > "${ACMECERTHOME}reload.sh" <<EOF
#/bin/bash
pwd
cat ${vhostname}.cer > "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
cat ${vhostname}.key > "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key"
cat ${vhostname}.ca >> "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
cat fullchain.cer > "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.cer"
if [ -f /usr/bin/ngxreload ]; then
  /usr/bin/ngxreload
fi
EOF
  chmod +x "${ACMECERTHOME}reload.sh"
  echo
  fi
}

#####################
split_domains() {
  parse_domains="$1"
  echo "$parse_domains"| awk '/\,/';
  D_ERR=$?
  if [[ "$D_ERR" = '0' ]]; then
    SAN=1
    DOMAIN_LIST="$(echo "$parse_domains"| sed -e 's|\s||g' | sed -e 's|,| -d |g')"
    vhostname=$(echo "$parse_domains"| awk -F ',' '{print $1}')
    DOMAIN_LISTNGX="$(echo "$(echo "$parse_domains"| sed -e 's|,| |g') www.$vhostname")"
    # take only 1st entry for nginx vhost
  else
    SAN=0
    DOMAIN_LIST="$parse_domains"
    vhostname="$parse_domains"
  fi
}

#####################
nvcheck() {
  if [ ! -h /usr/bin/nv ]; then
    rm -rf /usr/bin/nv
    ln -s "${SCRIPT_DIR}/nv.sh" /usr/bin/nv
    chmod +x /usr/bin/nv
  fi
}

#####################
vhostsetup() {
  vhost_domain="$1"
  HTTPSONLY="$2"
  if [ ! -f /usr/bin/pwgen ]; then
    yum -y -q install pwgen
  fi
  ftpusername=$(/usr/bin/pwgen -s 15 1)
  if [ -f /usr/bin/nv ]; then
    echo
    if [[ "$vhost_domain" = "$MAIN_HOSTNAME" ]]; then
      # check if vhost domain name is the registered main server hostname first
      # create main vhost's ssl vhost config file
      sslvhostsetup_mainhostname "$vhost_domain"
    else
      if [[ "$HTTPSONLY" = 'https' ]]; then
        echo "/usr/bin/nv -d "${vhost_domain}" -s ydle -u "${ftpusername}""
        /usr/bin/nv -d "${vhost_domain}" -s ydle -u "${ftpusername}"
      else
        echo "/usr/bin/nv -d "${vhost_domain}" -s y -u "${ftpusername}""
        /usr/bin/nv -d "${vhost_domain}" -s y -u "${ftpusername}"
      fi
      # initiate the autoprotect.sh include file generation
      # after nginx vhost is created
      if [ -f /usr/local/src/centminmod/tools/autoprotect.sh ]; then
        /usr/local/src/centminmod/tools/autoprotect.sh >/dev/null 2>&1
      fi
    fi # MAIN_HOSTNAME CHECK
    echo
  else
    echo
    echo "/usr/bin/nv not found"
    echo
  fi
}

#####################
install_acme() {
  echo
  cecho "-----------------------------------------------------" $boldgreen
  echo "installing acme.sh client..."
  cecho "-----------------------------------------------------" $boldgreen
  mkdir -p /root/tools
  cd /root/tools
  if [ ! -d acme.sh ]; then
    git clone "$ACMEGITURL"
    cd acme.sh
  elif [ -d acme.sh/.git ]; then
    cd acme.sh
    git pull -q
  fi
  if [[ "$EMAIL" ]]; then
  ./acme.sh --install --days $RENEWDAYS --accountemail "$EMAIL"
  else
  ./acme.sh --install --days $RENEWDAYS
  fi
  if [ -f "/root/.acme.sh/acme.sh.env" ]; then
    . "/root/.acme.sh/acme.sh.env"
  fi
  "$ACMEBINARY" -h
  echo
  cecho "-----------------------------------------------------" $boldgreen
  echo "check acme auto renew cronjob setup: "
  cecho "-----------------------------------------------------" $boldgreen
  crontab -l | grep acme.sh
  cecho "-----------------------------------------------------" $boldgreen
  echo "acme.sh installed"
  cecho "-----------------------------------------------------" $boldgreen
}

#####################
update_acme() {
  QUITEOUTPUT=$1
  echo
  cecho "-----------------------------------------------------" $boldgreen
  echo "updating acme.sh client..."
  cecho "-----------------------------------------------------" $boldgreen
  mkdir -p /root/tools
  cd /root/tools
  if [ ! -d acme.sh ]; then
    git clone "$ACMEGITURL"
    cd acme.sh
  elif [ -d acme.sh/.git ]; then
    cd acme.sh
    git pull -q
  fi
  if [[ "$EMAIL" ]]; then
  ./acme.sh --install --days $RENEWDAYS --accountemail "$EMAIL"
  else
  ./acme.sh --install --days $RENEWDAYS
  fi
  if [ -f "/root/.acme.sh/acme.sh.env" ]; then
    . "/root/.acme.sh/acme.sh.env"
  fi
  "$ACMEBINARY" -v
  if [[ "$QUITEOUTPUT" != 'quite' ]]; then
    echo
    cecho "-----------------------------------------------------" $boldgreen
    echo "check acme auto renew cronjob setup: "
    cecho "-----------------------------------------------------" $boldgreen
   crontab -l | grep acme.sh
  fi
  cecho "-----------------------------------------------------" $boldgreen
  echo "acme.sh updated"
  cecho "-----------------------------------------------------" $boldgreen
}

#####################
setup_acme() {
  # configure acme.sh defaults
  echo
}

#####################
check_acmeinstall() {
  # check if acme.sh is installed
  if [[ ! -d /root/.acme.sh || ! -f /root/.acme.sh/acme.sh.env ]]; then
    echo
    echo "acme.sh missing... installing acme.sh now..."
    install_acme
    echo
  fi
}

#####################
sslopts_check() {
  if [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]; then
      # check /svr-setup/openssl-1.0.2f/crypto/chacha20poly1305/chacha20.c exists
      OPEENSSL_CFPATCHED='y'
  elif [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]; then
      # for openssl 1.1.0 native chacha20 support
      OPEENSSL_CFPATCHED='y'
  fi

  if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]] || [[ "$OPEENSSL_CFPATCHED" = [yY] ]]; then
    if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]]; then
      CHACHACIPHERS='EECDH+CHACHA20:EECDH+CHACHA20-draft:'
    elif [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]]; then
      CHACHACIPHERS='EECDH+CHACHA20:EECDH+CHACHA20-draft:'
    else
      CHACHACIPHERS='EECDH+CHACHA20:EECDH+CHACHA20-draft:'
    fi
  else
    CHACHACIPHERS=""
  fi
  
  if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]] && [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_spdy_module')" = 'with-http_spdy_module' ]]; then
    HTTPTWO=y
    LISTENOPT='ssl spdy http2'
    COMP_HEADER='spdy_headers_comp 5'
    SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
    HTTPTWO_MAXFIELDSIZE='http2_max_field_size 16k;'
    HTTPTWO_MAXHEADERSIZE='http2_max_header_size 32k;'  
  elif [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
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
}

#####################
detectcustom_webroot() {
  CUSTOM_WEBROOT=$1
  DETECT_VHOSTNAME=$2
  DETECTSSLVHOST_CONFIGFILENAME="${DETECT_VHOSTNAME}.ssl.conf"
  DETECTSSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${DETECTSSLVHOST_CONFIGFILENAME}"
if [ -f "$DETECTSSLVHOST_CONFIG" ]; then
  CURRENT_WEBROOT=$(awk '/root /{print $2}' /usr/local/nginx/conf/conf.d/${DETECT_VHOSTNAME}.ssl.conf | sed -e 's|;||')
fi
if [[ "$CUSTOM_WEBROOT" ]]; then
  WEBROOTPATH="$CUSTOM_WEBROOT"
else
  WEBROOTPATH="/home/nginx/domains/${DETECT_VHOSTNAME}/public"
fi
}

#####################
switch_httpsdefault() {
  echo
  echo "setting HTTPS default in /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
  echo
  if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
echo "sed -i 's|^##x# HTTPS-DEFAULT|#x# HTTPS-DEFAULT|g' "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x# server {| server {|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x#   $DEDI_LISTEN|   $DEDI_LISTEN|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x#   server_name ${vhostname} www.${vhostname};|   server_name ${vhostname} www.${vhostname};|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x#   return 302 https://\$server_name\$request_uri;|   return 302 https://\$server_name\$request_uri;|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x#   include \/usr\/local\/nginx\/conf\/staticfiles.conf;|   include \/usr\/local\/nginx\/conf\/staticfiles.conf;|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
echo "sed -i "s|#x# }| }|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""

sed -i 's|^##x# HTTPS-DEFAULT|#x# HTTPS-DEFAULT|g' "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x# server {| server {|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x#   $DEDI_LISTEN|   $DEDI_LISTEN|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x#   server_name ${vhostname} www.${vhostname};|   server_name ${vhostname} www.${vhostname};|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x#   return 302 https:\/\/\$server_name\$request_uri;|   return 302 https:\/\/\$server_name\$request_uri;|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x#   include \/usr\/local\/nginx\/conf\/staticfiles.conf;|   include \/usr\/local\/nginx\/conf\/staticfiles.conf;|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
sed -i "s|#x# }| }|" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"

  echo
  echo "remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
  rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
  echo
  fi
}

#####################
gen_selfsigned() {
  vhostname="$1"
  if [ ! -f /usr/local/nginx/conf/ssl ]; then
    mkdir -p /usr/local/nginx/conf/ssl
  fi
  
  if [ ! -d /usr/local/nginx/conf/ssl/${vhostname} ]; then
    mkdir -p /usr/local/nginx/conf/ssl/${vhostname}
  fi

  if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;  
EVS
  fi

  pushd /usr/local/nginx/conf/ssl/${vhostname}
  
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
  
  echo "openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}.csr -keyout ${vhostname}.key -subj \"/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}\""
  openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}.csr -keyout ${vhostname}.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"
  echo "openssl x509 -req -days 36500 -sha256 -in ${vhostname}.csr -signkey ${vhostname}.key -out ${vhostname}.crt"
  openssl x509 -req -days 36500 -sha256 -in ${vhostname}.csr -signkey ${vhostname}.key -out ${vhostname}.crt
  
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen
  
  dhparamstarttime=$(date +%s.%N)
  
  openssl dhparam -out dhparam.pem 2048
  
  dhparamendtime=$(date +%s.%N)
  DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
  cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow
  popd
}

#####################
sslvhostsetup_mainhostname() {
  vhostname="$1"

  if [[ ! -f "${MAIN_HOSTNAMEVHOSTSSLFILE}" && ! -d "/home/nginx/domains/${vhostname}/public" ]]; then
  echo
  echo "create ${MAIN_HOSTNAMEVHOSTSSLFILE}"
  echo

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

if [ ! -f /usr/local/nginx/conf/ssl ]; then
  mkdir -p /usr/local/nginx/conf/ssl
fi

if [ ! -d /usr/local/nginx/conf/ssl/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/${vhostname}
fi

if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;  
EVS
fi

if [ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
cat > "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"<<EVT
  ssl_dhparam /usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem;
  ssl_certificate      /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt;
  ssl_certificate_key  /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key;
  #ssl_trusted_certificate /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-trusted.crt;
EVT
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
cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen

dhparamstarttime=$(date +%s.%N)

openssl dhparam -out dhparam.pem 2048

dhparamendtime=$(date +%s.%N)
DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow

# main hostname's ssl vhost at virual.ssl.conf
cat > "${MAIN_HOSTNAMEVHOSTSSLFILE}"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_HTTP/2.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
# server {
#       listen   ${DEDI_IP}80;
#       server_name ${vhostname} www.${vhostname};
#       return 302 https://\${vhostname}\$request_uri;
# }

server {
  listen ${DEDI_IP}443 default_server $LISTENOPT;
  server_name $vhostname;

  include /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf;
  include /usr/local/nginx/conf/ssl_include.conf;

  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}EECDH+ECDSA+AESGCM:EECDH+aRSA+AESGCM:EECDH+ECDSA+SHA256:EECDH+ECDSA+SHA384:EECDH+aRSA+SHA256:EECDH+aRSA+SHA384:EECDH+AES128:!aNULL:!eNULL:!LOW:!3DES:!MD5:!EXP:!PSK:!SRP:!DSS:!RC4:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER
  
  # before enabling HSTS line below read centminmod.com/nginx_domain_dns_setup.html#hsts
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

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

# limit_conn limit_per_ip 16;
# ssi  on;

        location /nginx_status {
        stub_status on;
        access_log   off;
        allow 127.0.0.1;
        #allow youripaddress;
        deny all;
        }

  access_log              /var/log/nginx/localhost.access.ssl.log     main buffer=256k flush=5m;
  error_log               /var/log/nginx/localhost.error.ssl.log      error;

  #include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root   html;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;

  location / {
    # block common exploits, sql injections etc
    #include /usr/local/nginx/conf/block.conf;
    
    #               Enables directory listings when index file not found
    #               autoindex  on;
    
    #               Shows file listing times as local time
    #               autoindex_localtime on;
    
    #               Enable for vBulletin usage WITHOUT vbSEO installed
    #               try_files               $uri $uri/ /index.php;
  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/include_opcache.conf;
  include /usr/local/nginx/conf/php.conf;
  #include /usr/local/nginx/conf/phpstatus.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_mainserver.conf;
}
ESS

  if [ -f /usr/bin/ngxreload ]; then
    /usr/bin/ngxreload
  fi
else
  echo
  if [ -f "${MAIN_HOSTNAMEVHOSTSSLFILE}" ]; then
    echo "${MAIN_HOSTNAMEVHOSTSSLFILE} already exists"
  fi
  if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
    echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
  fi
  echo
fi # "${MAIN_HOSTNAMEVHOSTSSLFILE}" doesn't exist
}

#####################
convert_crtkeyinc() {
  # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
  # inline in vhost, need to move them to their own include file for acmetool.sh at
  # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
egrep 'ssl_dhparam|ssl_certificate|ssl_trusted_certificate' /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf > /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
echo "cat /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
cat /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf

sed -i "s|^[ \t]* ssl_dhparam .*|  include \/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt.key.conf;|g" /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
sed -i '/ssl_certificate/d' /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
sed -i '/ssl_trusted_certificate/d' /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf
}

#####################
sslvhostsetup() {
  HTTPSONLY=$1
  CHECKFORWP=$2

  echo
  echo "[self-signed ssl cert check] required by acmetool.sh"
  echo

  if [ ! -f /usr/local/nginx/conf/ssl ]; then
    mkdir -p /usr/local/nginx/conf/ssl
  fi
  
  # check if self-signed ssl certificate files exist and if not generate them
  # otherwise, if user created the nginx vhost prior to running acmetool.sh with
  # self-signed ssl cert prompt answered as no, these self-signed ssl certificates
  # that acmetool.sh relies on initially won't exist and result in file not found
  # errors https://community.centminmod.com/posts/36759/
  if [[ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" || ! -f "/usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem" || ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" ]]; then
    gen_selfsigned "$vhostname"
  else
    if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem" ]; then
      echo "[self-signed ssl] /usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem exists"
    fi
    if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" ]; then
      echo "[self-signed ssl] /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt exists"
    fi
    if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key" ]; then
      echo "[self-signed ssl] /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key exists"
    fi
  fi

  echo
  echo "[sslvhostsetup] create /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
  echo

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

if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;  
EVS
fi

if [ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
cat > "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"<<EVT
  ssl_dhparam /usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem;
  ssl_certificate      /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt;
  ssl_certificate_key  /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key;
  #ssl_trusted_certificate /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-trusted.crt;
EVT
fi

detectcustom_webroot $CUSTOM_WEBROOT $vhostname

###  ##############################################################
if [[ "$HTTPSONLY" = 'https' && "$CHECKFORWP" = 'wp' ]]; then
  echo "[wp] backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
  cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1 >/dev/null 2>&1
  #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1

# remove 1st 12 lines of wp generated yourdomain.com.ssl.conf
# and insert http to https redirect
# single ssl vhost at yourdomain.com.ssl.conf
echo "[wp] create /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp1"<<ESU
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
##x# HTTPS-DEFAULT
#x# server {
#x#   $DEDI_LISTEN
#x#   server_name ${vhostname} www.${vhostname};
#x#   return 302 https://\$server_name\$request_uri;
#x#   include /usr/local/nginx/conf/staticfiles.conf;
#x# }
ESU
echo "cp -a "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2""
cp -a "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2"
echo "sed -i '1,12d' "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2""
sed -i '1,12d' "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2"
echo "cat "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp1" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2" > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
cat "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp1" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2" > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
rm -rf "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp1"
rm -rf "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-wp2"

if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
  # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
  # inline in vhost, need to move them to their own include file for acmetool.sh at
  # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
  convert_crtkeyinc
fi

###  ##############################################################
elif [[ "$HTTPSONLY" = 'https' && -z "$CHECKFORWP" ]]; then
  echo "[non-wp] backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
  cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1 >/dev/null 2>&1
  #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1

if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
  # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
  # inline in vhost, need to move them to their own include file for acmetool.sh at
  # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
  convert_crtkeyinc
fi

if [[ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" && ! "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
# insert http to https redirect if yourdomain.com.ssl.conf exists
# single ssl vhost at yourdomain.com.ssl.conf
if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
  if [[ ! "$(grep '^#x# HTTPS-DEFAULT' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp1"<<ESV
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
##x# HTTPS-DEFAULT
#x# server {
#x#   $DEDI_LISTEN
#x#   server_name ${vhostname} www.${vhostname};
#x#   return 302 https://\$server_name\$request_uri;
#x#   include /usr/local/nginx/conf/staticfiles.conf;
#x# }
ESV
echo "cp -a "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp2""
cp -a "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp2"
echo "cat "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp1" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp2" > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf""
cat "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp1" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp2" > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
rm -rf "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp1"
rm -rf "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf-nonwp2"
  fi # check for ^#x# HTTPS-DEFAULT to indicate an exisiting HTTPS default ssl vhost
fi

elif [ ! -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then

# single ssl vhost at yourdomain.com.ssl.conf
echo "create /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
##x# HTTPS-DEFAULT
#x# server {
#x#   $DEDI_LISTEN
#x#   server_name ${vhostname} www.${vhostname};
#x#   return 302 https://\$server_name\$request_uri;
#x#   include /usr/local/nginx/conf/staticfiles.conf;
#x# }

server {
  listen ${DEDI_IP}443 $LISTENOPT;
  server_name $vhostname www.$vhostname;

  include /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf;
  include /usr/local/nginx/conf/ssl_include.conf;

  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}EECDH+ECDSA+AESGCM:EECDH+aRSA+AESGCM:EECDH+ECDSA+SHA256:EECDH+ECDSA+SHA384:EECDH+aRSA+SHA256:EECDH+aRSA+SHA384:EECDH+AES128:!aNULL:!eNULL:!LOW:!3DES:!MD5:!EXP:!PSK:!SRP:!DSS:!RC4:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER

  # before enabling HSTS line below read centminmod.com/nginx_domain_dns_setup.html#hsts
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

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

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
fi

###  ##############################################################
elif [[ "$HTTPSONLY" != 'https' && -z "$CHECKFORWP" ]]; then

if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
  # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
  # inline in vhost, need to move them to their own include file for acmetool.sh at
  # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
  convert_crtkeyinc
fi

if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
  echo
  echo "skip /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf creation"
  echo "already exists"
  echo
elif [ ! -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_HTTP/2.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
# server {
#       listen   ${DEDI_IP}80;
#       server_name ${vhostname} www.${vhostname};
#       return 302 https://\$server_name\$request_uri;
# }

server {
  listen ${DEDI_IP}443 $LISTENOPT;
  server_name $vhostname www.$vhostname;

  include /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf;
  include /usr/local/nginx/conf/ssl_include.conf;

  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${CHACHACIPHERS}EECDH+ECDSA+AESGCM:EECDH+aRSA+AESGCM:EECDH+ECDSA+SHA256:EECDH+ECDSA+SHA384:EECDH+aRSA+SHA256:EECDH+aRSA+SHA384:EECDH+AES128:!aNULL:!eNULL:!LOW:!3DES:!MD5:!EXP:!PSK:!SRP:!DSS:!RC4:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA:!CAMELLIA;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER
  
  # before enabling HSTS line below read centminmod.com/nginx_domain_dns_setup.html#hsts
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

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=60m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root $WEBROOTPATH;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

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
fi # if ssl vhost exists
fi

  if [ -f /usr/bin/ngxreload ]; then
    /usr/bin/ngxreload
  fi
}

#####################
issue_acme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + no ssl_certificate line exists in the non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificates line exists in non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  # if webroot path directory exists 
  # + vhostname value exists 
  # + ssl vhost file exists 
  # + not main hostname VHOST_ALREADYSET
  if [[ -d "$WEBROOTPATH_OPT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "issue & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' ]] && [[ "$testcert" != 'wplive' && "$testcert" != 'wplived' && "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ ! -z "$testcert" ]]; then
     echo ""$ACMEBINARY" --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ "$testcert" != 'd' ]] && [[ ! -z "$testcert" ]]; then
      echo "wp routine detected use reissue instead via --force"
     echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY"  --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
     echo ""$ACMEBINARY" --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    if [ -f "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log" ]; then
      echo "rm -rf ${ACMECERTHOME}/${vhostname}" >> "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log"
    fi
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "issue skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

#####################
reissue_acme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + no ssl_certificate line exists in the non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificates line exists in non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  # if webroot path directory exists 
  # + vhostname value exists 
  # + ssl vhost file exists 
  # + not main hostname VHOST_ALREADYSET
  if [[ -d "$WEBROOTPATH_OPT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "reissue & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    echo ""$ACMEBINARY" --force --createDomainKey $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT""
    "$ACMEBINARY" --force --createDomainKey $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' ]] && [[ "$testcert" != 'wplive' && "$testcert" != 'wplived' && "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ ! -z "$testcert" ]]; then
     echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ "$testcert" != 'd' ]] && [[ ! -z "$testcert" ]]; then
      echo "wp routine"
     echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
     echo ""$ACMEBINARY" --force --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "reissue / renewal skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

renew_acme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + no ssl_certificate line exists in the non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificates line exists in non-https vhost file
  elif [[ -d "$WEBROOTPATH_OPT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  # if webroot path directory exists 
  # + vhostname value exists 
  # + ssl vhost file exists 
  # + not main hostname VHOST_ALREADYSET
  if [[ -d "$WEBROOTPATH_OPT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "renew & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' ]] && [[ "$testcert" != 'wplive' && "$testcert" != 'wplived' && "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ ! -z "$testcert" ]]; then
     echo ""$ACMEBINARY" --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ "$testcert" != 'd' ]] && [[ ! -z "$testcert" ]]; then
      echo "wp routine detected use reissue instead via --force"
     echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
     echo ""$ACMEBINARY" --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --staging --issue $DOMAINOPT -w "$WEBROOTPATH_OPT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "renewal skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

#####################
# custom webrooot

#####################
webroot_issueacme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  CUSTOM_WEBROOT="$customwebroot"
  detectcustom_webroot $CUSTOM_WEBROOT $vhostname
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line does not exist in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line exists in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  if [ ! -d "$WEBROOTPATH" ]; then
    echo "mkdir -p $WEBROOTPATH"
    mkdir -p "$WEBROOTPATH"
    chown -R nginx:nginx $WEBROOTPATH
    echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
    \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
  elif [[ -d "WEBROOTPATH" ]]; then
    echo "$WEBROOTPATH already exists"
    echo "ls -lah $WEBROOTPATH"
    ls -lah "$WEBROOTPATH"
    if [ -z "$(ls "$WEBROOTPATH")" ]; then
      echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
      \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
    fi
  fi

  if [[ "$CUSTOM_WEBROOT" ]]; then
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "$SSLVHOST_CONFIG" ]; then
      echo "adjusting $SSLVHOST_CONFIG"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG"
    fi
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
      echo
      echo "adjusting /usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo
    fi
  fi

  if [[ -d "$CUSTOM_WEBROOT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "issue & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' || "$testcert" != 'wplive' && "$testcert" != 'wplived' && "$testcert" != 'wptestd' ]] && [[ ! -z "$testcert" ]]; then
      echo ""$ACMEBINARY" --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ "$testcert" != 'd' ]] && [[ ! -z "$testcert" ]]; then
       echo "wp routine detected use reissue instead via --force"
      echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
      echo ""$ACMEBINARY" --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    if [ -f "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log" ]; then
      echo "rm -rf ${ACMECERTHOME}/${vhostname}" >> "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log"
    fi
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "issue skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

#####################
webroot_reissueacme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  CUSTOM_WEBROOT="$customwebroot"
  detectcustom_webroot $CUSTOM_WEBROOT $vhostname
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line does not exist in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line exists in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  if [ ! -d "$WEBROOTPATH" ]; then
    echo "mkdir -p $WEBROOTPATH"
    mkdir -p "$WEBROOTPATH"
    chown -R nginx:nginx $WEBROOTPATH
    echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
    \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
  elif [[ -d "WEBROOTPATH" ]]; then
    echo "$WEBROOTPATH already exists"
    echo "ls -lah $WEBROOTPATH"
    ls -lah "$WEBROOTPATH"
    if [ -z "$(ls "$WEBROOTPATH")" ]; then
      echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
      \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
    fi
  fi

  if [[ "$CUSTOM_WEBROOT" ]]; then
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "$SSLVHOST_CONFIG" ]; then
      echo "adjusting $SSLVHOST_CONFIG"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG"
    fi
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
      echo
      echo "adjusting /usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo
    fi
  fi

  if [[ -d "$CUSTOM_WEBROOT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG" >/dev/null 2>&1
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "reissue & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    echo ""$ACMEBINARY" --force --createDomainKey $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT""
    "$ACMEBINARY" --force --createDomainKey $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' || "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ ! -z "$testcert" ]]; then
      echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
      echo ""$ACMEBINARY" --force --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "reissue / renewal skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

webroot_renewacme() {
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  CUSTOM_WEBROOT="$customwebroot"
  detectcustom_webroot $CUSTOM_WEBROOT $vhostname
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  
  # if webroot path directory does not exists 
  # + ssl vhost file does not exist
  if [[ ! -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]]; then
    echo
    echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      vhostsetup "${vhostname}" https
    else
      vhostsetup "${vhostname}"
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line does not exist in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
    sslopts_check
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
  # if webroot path directory exists
  # + ssl vhost file does not exist
  # + ssl_certificate line exists in non-https vhost
  elif [[ -d "$CUSTOM_WEBROOT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      sslvhostsetup https
    else
      sslvhostsetup
    fi
    mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
  fi

  if [ ! -d "$WEBROOTPATH" ]; then
    echo "mkdir -p $WEBROOTPATH"
    mkdir -p "$WEBROOTPATH"
    chown -R nginx:nginx $WEBROOTPATH
    echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
    \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
  elif [[ -d "WEBROOTPATH" ]]; then
    echo "$WEBROOTPATH already exists"
    echo "ls -lah $WEBROOTPATH"
    ls -lah "$WEBROOTPATH"
    if [ -z "$(ls "$WEBROOTPATH")" ]; then
      echo "\cp -Raf "/home/nginx/domains/${vhostname}/public/"* $WEBROOTPATH"
      \cp -Raf "/home/nginx/domains/${vhostname}/public/"* "$WEBROOTPATH"
    fi
  fi

  if [[ "$CUSTOM_WEBROOT" ]]; then
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "$SSLVHOST_CONFIG" ]; then
      echo "adjusting $SSLVHOST_CONFIG"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "$SSLVHOST_CONFIG"
    fi
    # if using custom webroot need to adjust web root in both ssl and non-ssl vhost files
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
      echo
      echo "adjusting /usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo "change web root: "
      echo "from: $CURRENT_WEBROOT"
      echo "to: $WEBROOTPATH"
      sed -e "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf" | grep 'root '
      sed -i "s|root .*|root $WEBROOTPATH;|" "/usr/local/nginx/conf/conf.d/${vhostname}.conf"
      echo
    fi
  fi

  if [[ -d "$CUSTOM_WEBROOT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
    if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https 
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplived' || "$testcert" = 'wptestd' ]]; then
      # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
      # and remove the non-https vhostname.conf file
      echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
      #rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
      # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
        # sslvhostsetup https $vhostname
        sslopts_check
        sslvhostsetup https wp
        if [[ "$(grep '^#x#   return 302' "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf")" ]]; then
          switch_httpsdefault
        fi        
      fi
    else
      cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
    fi
    cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
    if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
        grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
      fi
      if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        # inline in vhost, need to move them to their own include file for acmetool.sh at
        # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        convert_crtkeyinc
      fi
    fi
    sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
    echo "grep 'root' $SSLVHOST_CONFIG"
    grep 'root' "$SSLVHOST_CONFIG"
    /usr/bin/ngxreload >/dev/null 2>&1
    echo
    echo "-----------------------------------------------------------"
    echo "renew & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' || "$testcert" != 'wplive' && "$testcert" != 'wplived' && "$testcert" != 'wptestd' ]] && [[ ! -z "$testcert" ]]; then
      echo ""$ACMEBINARY" --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    elif [[ "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ "$testcert" != 'd' ]] && [[ ! -z "$testcert" ]]; then
       echo "wp routine detected use reissue instead via --force"
      echo ""$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --force --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      # only enable resolver and ssl_stapling for live ssl certificate deployments
      if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
        sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
        sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
        sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
        if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
        fi
        if [[ "$testcert" = 'lived' || "$testcert" = 'wplived' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    else
      echo ""$ACMEBINARY" --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --staging --issue $DOMAINOPT -w "$CUSTOM_WEBROOT" -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT
      LECHECK=$?
      if [[ "$LECHECK" = '0' ]]; then
        if [[ "$testcert" = 'wptestd' || "$testcert" = 'd' ]]; then
          echo
          echo "switch to HTTPS default after verification"
          echo
          switch_httpsdefault
        fi
      fi
    fi
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "renewal skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
  fi
}

#####################
awsissue_acme() {
  echo
}

#####################
awsreissue_acme() {
  echo
}

#####################
awsrenew_acme() {
  echo
}

#####################
enter_domainname() {
  echo
  read -ep "Enter SSL certificate domain name you want without www. prefix host: " input_domain
  echo
}

#####################
enter_webroot() {
  echo
  echo "custom web root should be within /home/nginx/domains/yourdomain.com path"
  echo "i.e. /home/nginx/domains/yourdomain.com/customwebrootpath"
  echo
  read -ep "Enter custom webroot path you want: " input_webroot
  echo
  echo "you entered custom webroot = $input_webroot"
  echo "full path location will be at:"
  echo
  echo " /home/nginx/domains/${input_domain}/${input_webroot}"
  echo
  read -ep "is this path correct ? [y/n]: " webrootpath_correct
  if [[ "$webrootpath_correct" = [yY] ]]; then
    input_webroot="/home/nginx/domains/${input_domain}/${input_webroot}"
    echo "full path location will be at:"
    echo "$input_webroot"
  else
    while [[ "$webrootpath_correct" != [yY] ]]; do
      echo
      echo "custom web root should be within /home/nginx/domains/yourdomain.com path"
      echo "i.e. /home/nginx/domains/yourdomain.com/customwebrootpath"
      echo
      read -ep "Enter custom webroot path you want: " input_webroot
      echo
      echo "you entered custom webroot = $input_webroot"
      echo "full path location will be at:"
      echo
      echo " /home/nginx/domains/${input_domain}/${input_webroot}"
      echo
      read -ep "is this path correct ? [y/n]: " webrootpath_correct
      echo
    done
    if [[ "$webrootpath_correct" = [yY] ]]; then
      input_webroot="/home/nginx/domains/${input_domain}/${input_webroot}"
      echo "full path location will be at:"
      echo "$input_webroot"
    fi
  fi
  echo
}

#####################
issue_acmedns() {
  CERTONLY_DNS=$1
  STAGE_DNS=$2
  check_acmeinstall
  # split domains for SAN SSL certs
  split_domains "$vhostname"
  if [[ "$vhostname" != "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${vhostname}.ssl.conf"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${SSLVHOST_CONFIGFILENAME}"
    WEBROOTPATH_OPT="/home/nginx/domains/${vhostname}/public"
    VHOST_ALREADYSET='n'
  elif [[ "$vhostname" = "$MAIN_HOSTNAME" ]]; then
    SSLVHOST_CONFIGFILENAME="${MAIN_HOSTNAMEVHOSTSSLFILE}"
    SSLVHOST_CONFIG="/usr/local/nginx/conf/conf.d/${MAIN_HOSTNAMEVHOSTSSLFILE}"
    WEBROOTPATH_OPT="/usr/local/nginx/html"
    if [ -d "/home/nginx/domains/${vhostname}/public" ]; then
      echo "$vhostname setup already at /home/nginx/domains/${vhostname}/public"
      VHOST_ALREADYSET='y'
    fi
  fi
  ############################################
  # DNS mode cert only don't touch nginx vhosts
  # 0
  if [[ "$CERTONLY_DNS" != '1' ]]; then
    # if webroot path directory does not exists 
    # + ssl vhost file does not exist
    if [[ ! -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]]; then
      echo
      echo "${vhostname} nginx vhost + pureftp virtual ftp user setup"
      if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
        vhostsetup "${vhostname}" https
      else
        vhostsetup "${vhostname}"
      fi
    # if webroot path directory exists
    # + ssl vhost file does not exist
    # + no ssl_certificate line exists in the non-https vhost file
    elif [[ -d "$WEBROOTPATH_OPT" && ! -f "$SSLVHOST_CONFIG" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" != '0' ]]; then
      sslopts_check
      if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
        sslvhostsetup https
      else
        sslvhostsetup
      fi
    # if webroot path directory exists
    # + ssl vhost file does not exist
    # + ssl_certificates line exists in non-https vhost file
    elif [[ -d "$WEBROOTPATH_OPT" ]] && [[ "$(grep -sq ssl_certificate /usr/local/nginx/conf/conf.d/${vhostname}.conf; echo $?)" = '0' ]] && [[ ! -f "$SSLVHOST_CONFIG" ]]; then
      if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
        sslvhostsetup https
      else
        sslvhostsetup
      fi
      mv "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
      SSLVHOST_CONFIG="${ACMEBACKUPDIR}/${SSLVHOST_CONFIGFILENAME}-acmebackup-${DT}-disabled"
    fi
  fi # DNS Mode certonly 0
  ############################################
  # DNS mode cert only don't touch nginx vhosts
  # 1
  if [[ "$CERTONLY_DNS" != '1' ]]; then
    # if webroot path directory exists 
    # + vhostname value exists 
    # + ssl vhost file exists 
    # + public webroot path doesn't exist
    if [[ -d "$WEBROOTPATH_OPT" && ! -z "$vhostname" && -f "$SSLVHOST_CONFIG" && "$VHOST_ALREADYSET" != 'y' ]]; then
      check_dns "$vhostname"
      if [[ "$TOPLEVEL" = [yY] ]]; then
        if [[ "$SAN" = '1' ]]; then
          DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
        else
          DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
        fi
      else
        if [[ "$SAN" = '1' ]]; then
          DOMAINOPT="-d $DOMAIN_LIST"
        else
          DOMAINOPT="-d ${vhostname}"
        fi
      fi
      if [[ "$testcert" = 'lived' || "$testcert" = 'd' ]]; then
        # if https default via d or lived option, then backup non-https vhostname.conf to backup directory
        # and remove the non-https vhostname.conf file
        echo "backup & remove /usr/local/nginx/conf/conf.d/$vhostname.conf"
        cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-backup-removal-https-default-${DT}" >/dev/null 2>&1
        rm -rf "/usr/local/nginx/conf/conf.d/$vhostname.conf" >/dev/null 2>&1
        # if existing https vhostname.ssl.conf file exists replace it with one with proper http to https redirect
        if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.ssl.conf" ]; then
          # sslvhostsetup https $vhostname
          sslopts_check
          sslvhostsetup https 
        fi
      else
        cp -a "/usr/local/nginx/conf/conf.d/$vhostname.conf" "${ACMEBACKUPDIR}/$vhostname.conf-acmebackup-$DT" >/dev/null 2>&1
      fi
      cp -a "$SSLVHOST_CONFIG" "${ACMEBACKUPDIR}/$SSLVHOST_CONFIGFILENAME-acmebackup-$DT"
      if [[ "$testcert" != 'lived' || "$testcert" != 'd' || "$testcert" != 'wplived' || "$testcert" != 'wpd' ]]; then
        if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
          sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
          echo "grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf""
          grep 'root' "/usr/local/nginx/conf/conf.d/$vhostname.conf"
        fi
        # if [[ ! -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" && -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]]; then
        #   # if existing or previous /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf has the ssl cert key and trust files
        #   # inline in vhost, need to move them to their own include file for acmetool.sh at
        #   # /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf
        #   convert_crtkeyinc
        # fi
      fi
      sed -i "s|server_name .*|server_name $DOMAIN_LISTNGX;|" "$SSLVHOST_CONFIG"
      echo "grep 'root' $SSLVHOST_CONFIG"
      grep 'root' "$SSLVHOST_CONFIG"
      /usr/bin/ngxreload >/dev/null 2>&1
    fi
  elif [[ "$CERTONLY_DNS" = '1' ]]; then
    check_dns "$vhostname"
    if [[ "$TOPLEVEL" = [yY] ]]; then
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST -d www.${vhostname}"
      else
        DOMAINOPT="-d ${vhostname} -d www.${vhostname}"
      fi
    else
      if [[ "$SAN" = '1' ]]; then
        DOMAINOPT="-d $DOMAIN_LIST"
      else
        DOMAINOPT="-d ${vhostname}"
      fi
    fi
  fi # DNS Mode certonly 1
    echo
    echo "-----------------------------------------------------------"
    echo "[DNS mode] issue & install letsencrypt ssl certificate for $vhostname"
    echo "-----------------------------------------------------------"
    # if CF_DNSAPI enabled for Cloudflare DNS mode, use Cloudflare API for setting
    # up DNS mode validation via TXT DNS record creation
    if [[ "$CF_DNSAPI" = [yY] ]] && [[ ! -z "$CF_KEY" && ! -z "$CF_KEY" ]]; then
      export CF_KEY="$CF_KEY"
      export CF_EMAIL="$CF_EMAIL"
      DNSAPI_OPT=' dns_cf'
      sed -i "s|^#CF_|CF_|" "$ACMECERTHOME"account.conf
      sed -i "s|CF_Key=\".*|CF_Key=\"$CF_KEY\"|" "$ACMECERTHOME"account.conf
      sed -i "s|CF_Email=\".*|CF_Email=\"$CF_EMAIL\"|" "$ACMECERTHOME"account.conf
    else
      DNSAPI_OPT=""
    fi
    # if the option flag = live is not passed on command line, the issuance uses the
    # staging test ssl certificates
    echo "testcert value = $testcert"
    if [[ -f "$ACMECERTHOME${vhostname}${ECC_ACMEHOMESUFFIX}/${vhostname}.key" ]]; then
      DNS_ISSUEOPT='--issue --force'
    else
      DNS_ISSUEOPT='--issue --force'
    fi
    if [[ "$testcert" = 'live' || "$testcert" = 'lived' || "$testcert" != 'd' || "$testcert" = 'wplive' || "$testcert" = 'wplived' || "$testcert" != 'wptestd' ]] && [[ "$testcert" != 'wptest' ]] && [[ ! -z "$testcert" ]]; then
     echo ""$ACMEBINARY" ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT 2>&1 | tee "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      LECHECK=$?
      ############################################
      # DNS mode cert only don't touch nginx vhosts
      # 2
      if [[ "$CERTONLY_DNS" != '1' ]]; then
        # only enable resolver and ssl_stapling for live ssl certificate deployments
        if [[ -f "$SSLVHOST_CONFIG" && "$LECHECK" = '0' ]]; then
          sed -i "s|#resolver |resolver |" "$SSLVHOST_CONFIG"
          sed -i "s|#resolver_timeout|resolver_timeout|" "$SSLVHOST_CONFIG"
          sed -i "s|#ssl_stapling on|ssl_stapling on|" "$SSLVHOST_CONFIG"
          sed -i "s|#ssl_stapling_verify|ssl_stapling_verify|" "$SSLVHOST_CONFIG"
          sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "$SSLVHOST_CONFIG"
          if [ -f "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" ]; then
            sed -i "s|#ssl_trusted_certificate|ssl_trusted_certificate|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
          fi
        fi
      fi # DNS Mode certonly 2
    elif [[ "$CERTONLY_DNS" = '1' && "$STAGE_DNS" = 'live' ]]; then
     echo ""$ACMEBINARY" ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT 2>&1 | tee "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      LECHECK=$?
    else
     echo ""$ACMEBINARY" --staging ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT"
      "$ACMEBINARY" --staging ${DNS_ISSUEOPT} --dns${DNSAPI_OPT} $DOMAINOPT -k "$KEYLENGTH" --useragent "$LE_USERAGENT" $ACMEDEBUG_OPT 2>&1 | tee "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
     LECHECK=$?
    fi
    if [[ "$CF_DNSAPI" != [yY] ]] && [[ -z "$CF_KEY" && -z "$CF_KEY" ]]; then
      if [ -f "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log" ]; then
        # echo " Final Step to complete SSL Certificate Issuance" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo " Once DNS updated for $vhostname, run SSH command: " >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "  $ACMEBINARY --force --renew${ECCFLAG} $DOMAINOPT" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo " SSL certs will be located : $ACMECERTHOME${vhostname}${ECC_ACMEHOMESUFFIX}" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo " If want to install cert into Nginx vhost, run SSH command: " >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        # echo "" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      echo "  "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}. key"${ECCFLAG}" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo " SSL certs will be installed at : /usr/local/nginx/conf/ssl/${vhostname}/" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      fi
    else
      if [ -f "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log" ]; then
        echo " If want to install cert into Nginx vhost, run SSH command: " >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        # echo "" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      echo "  "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}. key"${ECCFLAG}" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "---------------------------------" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo " SSL certs will be installed at : /usr/local/nginx/conf/ssl/${vhostname}/" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        echo "" >> "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      fi
    fi # if CF_DNSAPI enabled can skip this text
      ############################################
      # DNS mode cert only don't touch nginx vhosts
      # 3
      if [[ "$CERTONLY_DNS" != '1' ]]; then
    # LECHECK=$?
    echo "LECHECK = $LECHECK"
    if [[ "$LECHECK" = '0' ]]; then
      if [[ "$KEYLENGTH" = 'ec-256' || "$KEYLENGTH" = 'ec-384' ]]; then
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme${ECC_SUFFIX}.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      else
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}.key|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.key|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      sed -i "s|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-trusted.crt|\/usr\/local\/nginx\/conf\/ssl\/${vhostname}\/${vhostname}-acme.cer|" "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf"
      fi
      egrep 'ssl_dhparam|ssl_certificate|ssl_certificate_key|ssl_trusted_certificate' "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt.key.conf" | tee /usr/local/nginx/conf/ssl/${vhostname}/acme-vhost-config.txt

    echo
    echo "-----------------------------------------------------------"
    echo "install cert"
    echo "-----------------------------------------------------------"
    # ensure directory exists before installing and copying ssl cert files
    # to /usr/local/nginx/conf/ssl/${vhostname}
    if [ ! -d "/usr/local/nginx/conf/ssl/${vhostname}" ]; then
      mkdir -p "/usr/local/nginx/conf/ssl/${vhostname}"
    fi
    echo ""$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}"
    "$ACMEBINARY" --installcert $DOMAINOPT --certpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --keypath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.key" --capath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer" --reloadCmd /usr/bin/ngxreload --fullchainpath "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-fullchain-acme${ECC_SUFFIX}.key"${ECCFLAG}
    if [ -f "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log" ]; then
      echo "rm -rf ${ACMECERTHOME}/${vhostname}" >> "${CENTMINLOGDIR}/centminmod_${DT}_nginx_addvhost_nv-remove-cmds-${vhostname}.log"
    fi
    # allow it to be repopulated each time with $vhostname
    # rm -rf /root/.acme.sh/reload.sh
    echo
    echo "letsencrypt ssl certificate setup completed"
    echo "ssl certs located at: /usr/local/nginx/conf/ssl/${vhostname}"
    pushover_alert $vhostname
    backup_acme $vhostname
    echo
    echo "openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer""
    openssl x509 -noout -text < "/usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-acme${ECC_SUFFIX}.cer"
    listlogs
    echo
    elif [[ "$LECHECK" = '1' ]]; then
      listlogs
      echo
    elif [[ "$LECHECK" = '2' ]]; then
      echo
      echo "issue / renewal skipped as ssl cert still valid"
      listlogs
      echo
    fi  # reloadcmd_setup
    elif [[ "$CERTONLY_DNS" = '1' ]]; then
      if [[ "$CF_DNSAPI" != [yY] ]] && [[ -z "$CF_KEY" && -z "$CF_KEY" ]]; then
        echo
        echo "---------------------------------"
        echo " DNS mode requires manual steps below"
        echo "---------------------------------"
        cat "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log" | grep -A30 'Add the following TXT record' | perl -pe 's/\x1b.*?[mGKH]//g' | sed 's/\[[^]]*\]//g' | egrep -v 'Please be aware that you  prepend|resulting subdomain|and retry again'
        pushover_alert $vhostname dns "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
        # backup_acme $vhostname
        echo
        echo
      elif [[ "$CF_DNSAPI" = [yY] ]] && [[ ! -z "$CF_KEY" && ! -z "$CF_KEY" ]]; then
        echo
        echo "---------------------------------"
        echo " DNS mode via Cloudflare DNS API"
        echo "---------------------------------"
        echo " setup TXT DNS record via Cloudflare API"
        cat "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log" | perl -pe 's/\x1b.*?[mGKH]//g' | sed 's/\[[^]]*\]//g' | grep -A60 'Verify each domain' | sed '/-----BEGIN CERTIFICATE-----/,/-----END CERTIFICATE-----/d'
        echo
        pushover_alert $vhostname dnscf "${CENTMINLOGDIR}/acme.sh-dnslog-${vhostname}${ECC_SUFFIX}-${DT}.log"
      fi # if CF_DNSAPI enabled can skip this text
    fi # DNS Mode certonly 3
}

#####################
sslmenu_issue() {
  while :
   do

  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "        SSL Issue Management              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).  Issue SSL Cert Staging/Test" $boldgreen
  cecho "2).  Issue SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "3).  Issue SSL Cert Live" $boldgreen
  cecho "4).  Issue SSL Cert Live HTTPS Default" $boldgreen
  cecho "5).  Custom Webroot Issue SSL Cert Staging/Test" $boldgreen
  cecho "6).  Custom Webroot Issue SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "7).  Custom Webroot Issue SSL Cert Live" $boldgreen
  cecho "8).  Custom Webroot Issue SSL Cert Live HTTPS Default" $boldgreen
  cecho "9).  S3 Issue SSL Cert" $boldgreen
  cecho "10). S3 Issue SSL Cert" $boldgreen
  cecho "11). S3 Issue SSL Cert" $boldgreen
  cecho "12). S3 Issue SSL Cert" $boldgreen
  cecho "13). Exit" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 13 ] " sslmenuissue_options
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$sslmenuissue_options" in
1)
MENU3=1

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 issue $input_domain
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu1.log

;;
2)
MENU3=2

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 issue $input_domain d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu2.log
;;
3)
MENU3=3

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 issue $input_domain live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu3.log
;;
4)
MENU3=4

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 issue $input_domain lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu4.log
;;
5)
MENU3=5

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-issue $input_domain $input_webroot
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu5.log
;;
6)
MENU3=6

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-issue $input_domain $input_webroot d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu6.log
;;
7)
MENU3=7

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-issue $input_domain $input_webroot live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu7.log
;;
8)
MENU3=8

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-issue $input_domain $input_webroot lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu8.log
;;
9)
MENU3=9

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu9.log
;;
10)
MENU3=10

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu10.log
;;
11)
MENU3=11

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu11.log
;;
12)
MENU3=12

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu12.log
;;
13)
  MENU3=13
  break 2
;;
esac
done

}

#####################
sslmenu_renew() {
  while :
   do

  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "        SSL Renew Management              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).   Renew SSL Cert Staging/Test" $boldgreen
  cecho "2).   Renew SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "3).   Renew SSL Cert Live" $boldgreen
  cecho "4).   Renew SSL Cert Live HTTPS Default" $boldgreen
  cecho "5).   Custom Webroot Renew SSL Cert Staging/Test" $boldgreen
  cecho "6).   Custom Webroot Renew SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "7).   Custom Webroot Renew SSL Cert Live" $boldgreen
  cecho "8).   Custom Webroot Renew SSL Cert Live HTTPS Default" $boldgreen
  cecho "9).   S3 Renew SSL Cert" $boldgreen
  cecho "10).  S3 Renew SSL Cert" $boldgreen
  cecho "11).  S3 Renew SSL Cert" $boldgreen
  cecho "12).  S3 Renew SSL Cert" $boldgreen
  cecho "13).  Exit" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 13 ] " sslmenurenew_options
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$sslmenurenew_options" in
1)
MENU3=1

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 renew $input_domain
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu1.log

;;
2)
MENU3=2

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 renew $input_domain d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu2.log
;;
3)
MENU3=3

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 renew $input_domain live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu3.log
;;
4)
MENU3=4

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 renew $input_domain lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu4.log
;;
5)
MENU3=5

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-renew $input_domain $input_webroot
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu5.log
;;
6)
MENU3=6

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-renew $input_domain $input_webroot d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu6.log
;;
7)
MENU3=7

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-renew $input_domain $input_webroot live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu7.log
;;
8)
MENU3=8

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-renew $input_domain $input_webroot lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu8.log
;;
9)
MENU3=9

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu9.log
;;
10)
MENU3=10

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu10.log
;;
11)
MENU3=11

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu11.log
;;
12)
MENU3=12

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu12.log
;;
13)
  MENU3=13
  break 2
;;
esac
done

}

#####################
sslmenu_reissue() {
  while :
   do

  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "        SSL Reissue Management              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).  Reissue SSL Cert Staging/Test" $boldgreen
  cecho "2).  Reissue SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "3).  Reissue SSL Cert Live" $boldgreen
  cecho "4).  Reissue SSL Cert Live HTTPS Default" $boldgreen
  cecho "5).  Custom Webroot Reissue SSL Cert Staging/Test" $boldgreen
  cecho "6).  Custom Webroot Reissue SSL Cert Staging/Test HTTPS Default" $boldgreen
  cecho "7).  Custom Webroot Reissue SSL Cert Live" $boldgreen
  cecho "8).  Custom Webroot Reissue SSL Cert Live HTTPS Default" $boldgreen
  cecho "9).  S3 Reissue SSL Cert" $boldgreen
  cecho "10). S3 Reissue SSL Cert" $boldgreen
  cecho "11). S3 Reissue SSL Cert" $boldgreen
  cecho "12). S3 Reissue SSL Cert" $boldgreen
  cecho "13). Exit" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 13 ] " sslmenureissue_options
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$sslmenureissue_options" in
1)
MENU3=1

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 reissue $input_domain
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu1.log

;;
2)
MENU3=2

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 reissue $input_domain d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu2.log
;;
3)
MENU3=3

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 reissue $input_domain live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu3.log
;;
4)
MENU3=4

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  $0 reissue $input_domain lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu4.log
;;
5)
MENU3=5

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-reissue $input_domain $input_webroot
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu5.log
;;
6)
MENU3=6

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-reissue $input_domain $input_webroot d
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu6.log
;;
7)
MENU3=7

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-reissue $input_domain $input_webroot live
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu7.log
;;
8)
MENU3=8

{
  echo
  cecho "..." $boldyellow
  enter_domainname
  enter_webroot
  $0 webroot-reissue $input_domain $input_webroot lived
  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu8.log
;;
9)
MENU3=9

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu9.log
;;
10)
MENU3=10

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu10.log
;;
11)
MENU3=11

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu11.log
;;
12)
MENU3=12

{
  echo
  cecho "..." $boldyellow
  enter_domainname

  # break 2
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu12.log
;;
13)
  MENU3=13
  break 2
;;
esac
done

}

#####################
sslmenu() {
  while :
   do

  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "        SSL Management              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).  acemtool.sh install" $boldgreen
  cecho "2).  acmetool.sh update" $boldgreen
  cecho "3).  acmetool.sh setup" $boldgreen
  cecho "4).  Issue SSL Management" $boldgreen
  cecho "5).  Renew SSL Management" $boldgreen
  cecho "6).  Reissue SSL Management" $boldgreen
  cecho "7).  Renew All Staging /Test Certs" $boldgreen
  cecho "8).  Renew ALL Live Certs " $boldgreen
  cecho "9).  Renew All Live Certs HTTPS Default" $boldgreen
  cecho "10). Exit" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 10 ] " sslmenu_options
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$sslmenu_options" in
1)
MENU3=1

{
  echo
  cecho "..." $boldyellow
  nvcheck
  install_acme
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu1.log

;;
2)
MENU3=2

{
  echo
  cecho "..." $boldyellow
  nvcheck
  update_acme
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu2.log
;;
3)
MENU3=3

{
  echo
  cecho "..." $boldyellow
  nvcheck
  setup_acme
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu3.log
;;
4)
MENU3=4

{
  echo
  cecho "..." $boldyellow
  sslmenu_issue
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu4.log
;;
5)
MENU3=5

{
  echo
  cecho "..." $boldyellow
  sslmenu_renew
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu5.log
;;
6)
MENU3=6

{
  echo
  cecho "..." $boldyellow
  sslmenu_reissue
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu6.log
;;
7)
MENU3=7

{
  echo
  cecho "..." $boldyellow
  nvcheck
  testcert=""
  getuseragent
  renew_all
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu7.log
;;
8)
MENU3=8

{
  echo
  cecho "..." $boldyellow
  nvcheck
  testcert="live"
  getuseragent
  renew_all
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu8.log
;;
9)
MENU3=9

{
  echo
  cecho "..." $boldyellow
  echo
  cecho "..." $boldyellow
  nvcheck
  testcert="lived"
  getuseragent
  renew_all
} 2>&1 | tee ${CENTMINLOGDIR}/acmetool-menu_${SCRIPT_VERSION}_${DT}_menu3-submenu9.log
;;
10)
  MENU3=10
  break
;;
esac
done

}

######################################################
case "$1" in
  acmeinstall )
{ 
nvcheck
  install_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-install_${DT}.log"
    ;;
  acmeupdate )
{ 
nvcheck
update_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-update_${DT}.log"
    ;;
  acmesetup )
{ 
nvcheck
setup_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-setup_${DT}.log"
    ;;
  issue )
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
issue_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-issue_${DT}.log"
    ;;
  reissue )
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
reissue_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-reissue_${DT}.log"
    ;;
  renew )
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
renew_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-renew_${DT}.log"
    ;;
  webroot-issue )
{ 
nvcheck
vhostname="$2"
customwebroot="$3"
testcert="$4"
getuseragent
update_acme quite
webroot_issueacme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-webroot-issue_${DT}.log"
    ;;
  webroot-reissue )
{ 
nvcheck
vhostname="$2"
customwebroot="$3"
testcert="$4"
getuseragent
update_acme quite
webroot_reissueacme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-webroot-reissue_${DT}.log"
    ;;
  webroot-renew )
{ 
nvcheck
vhostname="$2"
customwebroot="$3"
testcert="$4"
getuseragent
update_acme quite
webroot_renewacme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-webroot-renew_${DT}.log"
    ;;
  s3issue )
  # aws s3 bucket'd
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
awsissue_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-awsissue_${DT}.log"
    ;;
  s3reissue )
  # aws s3 bucket'd
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
awsreissue_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-awsreissue_${DT}.log"
    ;;
  s3renew )
  # aws s3 bucket'd
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent
update_acme quite
awsrenew_acme
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-awsrenew_${DT}.log"
    ;;
  renewall )
  # aws s3 bucket'd
{ 
nvcheck
testcert="$2"
getuseragent
renew_all
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-renewall_${DT}.log"
    ;;
acme-menu )
  sslmenu
;;
checkdates )
  checkdate
;;
  certonly-issue )
{ 
nvcheck
vhostname="$2"
testcert="$3"
getuseragent dns
update_acme quite
if [ "$testcert" = 'live' ]; then
  issue_acmedns 1 live
else
  issue_acmedns 1
fi
} 2>&1 | tee "${CENTMINLOGDIR}/acmesh-certonly-issue_${DT}.log"
;;
  * )
  echo
  echo " $0 {acme-menu|acmeinstall|acmeupdate|acmesetup|issue|reissue|renew|certonly-issue|s3issue|s3reissue|s3renew|renewall|checkdates}"
  echo "
 Usage Commands: 
 $0 acme-menu
 $0 acmeinstall
 $0 acmeupdate 
 $0 acmesetup 
 $0 issue domainname
 $0 issue domainname d
 $0 issue domainname live
 $0 issue domainname lived
 $0 reissue domainname
 $0 reissue domainname d
 $0 reissue domainname live
 $0 reissue domainname lived
 $0 renew domainname
 $0 renew domainname d
 $0 renew domainname live
 $0 renew domainname lived
 $0 webroot-issue domainname /path/to/custom/webroot
 $0 webroot-issue domainname /path/to/custom/webroot d
 $0 webroot-issue domainname /path/to/custom/webroot live
 $0 webroot-issue domainname /path/to/custom/webroot lived
 $0 webroot-reissue domainname /path/to/custom/webroot
 $0 webroot-reissue domainname /path/to/custom/webroot d
 $0 webroot-reissue domainname /path/to/custom/webroot live
 $0 webroot-reissue domainname /path/to/custom/webroot lived
 $0 webroot-renew domainname /path/to/custom/webroot
 $0 webroot-renew domainname /path/to/custom/webroot d
 $0 webroot-renew domainname /path/to/custom/webroot live
 $0 webroot-renew domainname /path/to/custom/webroot lived
 $0 certonly-issue domainname
 $0 certonly-issue domainname live
 $0 s3issue domainname
 $0 s3issue domainname d
 $0 s3issue domainname live
 $0 s3issue domainname lived
 $0 s3reissue domainname
 $0 s3reissue domainname d
 $0 s3reissue domainname live
 $0 s3reissue domainname lived
 $0 s3renew domainname
 $0 s3renew domainname d
 $0 s3renew domainname live
 $0 s3renew domainname lived
 $0 renewall
 $0 renewall live
 $0 renewall lived
 $0 checkdates
  "
    ;;
esac