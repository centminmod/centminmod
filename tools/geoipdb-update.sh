#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
######################################################
# to update geoip country and city databases
######################################################
branchname='141.00beta01'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
LOCALCENTMINMOD_MIRROR='https://centminmod.com'

# Maxmind GeoLite2 database API Key
# https://community.centminmod.com/posts/80656/
# You can override this API key with your own Maxmind
# account API key by setting MM_LICENSE_KEY variable 
# in persistent config file /etc/centminmod/custom_config.inc
GET_CMM_MM_LICENSE_KEY=$(curl -s https://mmkey.centminmod.com/)
MM_LICENSE_KEY="$GET_CMM_MM_LICENSE_KEY"
######################################################
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
######################################################
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

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

echo
cecho "Updating GeoIP databases..." $boldyellow

  \cp -f /usr/share/GeoIP/GeoIP.dat /usr/share/GeoIP/GeoIP.dat-backup
  if [ -f /usr/share/GeoIP/GeoLiteCity.dat.gz ]; then
    # backup existing database in case maxmind end downloads
    \cp -af /usr/share/GeoIP/GeoLiteCity.dat.gz /usr/share/GeoIP/GeoLiteCity.dat-backup.gz
  fi
  if [ -f /usr/share/GeoIP/GeoIP.dat.gz ]; then
    # backup existing database in case maxmind end downloads
    \cp -af /usr/share/GeoIP/GeoIP.dat.gz /usr/share/GeoIP/GeoIP.dat-backup.gz
  fi
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 ${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip-legacy/GeoIP.dat.gz | grep 'HTTP/' | grep '200'
  GEOIPCOUNTRYDATA_CURLCHECK=$?
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPCOUNTRYDATA_CURLCHECK" = '0' ]]; then
    wget -${ipv_forceopt}cnv ${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip-legacy/GeoIP.dat.gz -O /usr/share/GeoIP/GeoIP.dat.gz
  fi
  gzip -df /usr/share/GeoIP/GeoIP.dat.gz
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 ${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip-legacy/GeoLiteCity.gz | grep 'HTTP/' | grep '200'
  GEOIPCITYDATA_CURLCHECK=$?
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPCITYDATA_CURLCHECK" = '0' ]]; then
    wget -${ipv_forceopt}cnv ${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip-legacy/GeoLiteCity.gz -O /usr/share/GeoIP/GeoLiteCity.dat.gz
  fi
  gzip -d -f /usr/share/GeoIP/GeoLiteCity.dat.gz
  cp -a /usr/share/GeoIP/GeoLiteCity.dat /usr/share/GeoIP/GeoIPCity.dat

if [[ -f /usr/share/GeoIP/GeoLite2-City.mmdb || -f /usr/share/GeoIP/GeoLite2-Country.mmdb ]]; then

  if [ ! -f /etc/centminmod/custom_config.inc ]; then
    CHECK_CMM_MM_LICENSE_KEY=''
  else
    CHECK_CMM_MM_LICENSE_KEY=$(awk -F '=' '/^[[:space:]]*MM_LICENSE_KEY=/ {print $2}' /etc/centminmod/custom_config.inc | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g")
  fi
  if [[ "$MM_LICENSE_KEY" ]]; then
    echo
    maxmind_city_url="https://download.maxmind.com/app/geoip_download?edition_id=GeoLite2-City&license_key=$MM_LICENSE_KEY&suffix=tar.gz"
    maxmind_country_url="https://download.maxmind.com/app/geoip_download?edition_id=GeoLite2-Country&license_key=$MM_LICENSE_KEY&suffix=tar.gz"
    maxmind_asn_url="https://download.maxmind.com/app/geoip_download?edition_id=GeoLite2-ASN&license_key=$MM_LICENSE_KEY&suffix=tar.gz"
  else
    echo
    maxmind_city_url="${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip2-lite/GeoLite2-City.tar.gz"
    maxmind_country_url="${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip2-lite/GeoLite2-Country.tar.gz"
    maxmind_asn_url="${LOCALCENTMINMOD_MIRROR}/centminmodparts/geoip2-lite/GeoLite2-ASN.tar.gz"
  fi

  mkdir -p /usr/share/GeoIP
  pushd /usr/share/GeoIP
  cecho "GeoLite2 City database download ..." $boldyellow
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "$maxmind_city_url" | grep 'HTTP/' | grep '200'
  GEOIPTWOCITYDATA_CURLCHECK=$?
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPTWOCITYDATA_CURLCHECK" = '0' ]]; then
    wget -${ipv_forceopt}cnv "$maxmind_city_url" -O /usr/share/GeoIP/GeoLite2-City.tar.gz
  fi
  tar xvzf /usr/share/GeoIP/GeoLite2-City.tar.gz -C /usr/share/GeoIP
  cp -a GeoLite2-City_*/GeoLite2-City.mmdb /usr/share/GeoIP/GeoLite2-City.mmdb
  rm -rf GeoLite2-City_*

  cecho "GeoLite2 Country database download ..." $boldyellow
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "$maxmind_country_url" | grep 'HTTP/' | grep '200'
  GEOIPTWOCOUNTRYDATA_CURLCHECK=$?
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPTWOCOUNTRYDATA_CURLCHECK" = '0' ]]; then
    wget -${ipv_forceopt}cnv "$maxmind_country_url" -O /usr/share/GeoIP/GeoLite2-Country.tar.gz
  fi
  tar xvzf /usr/share/GeoIP/GeoLite2-Country.tar.gz -C /usr/share/GeoIP
  cp -a GeoLite2-Country_*/GeoLite2-Country.mmdb /usr/share/GeoIP/GeoLite2-Country.mmdb
  rm -rf GeoLite2-Country_*

  cecho "Check GeoIP2 Lite Databases" $boldyellow
  echo
  /usr/local/nginx-dep/bin/mmdblookup --help
  echo
  /usr/local/nginx-dep/bin/mmdblookup --file /usr/share/GeoIP/GeoLite2-Country.mmdb --ip 8.8.8.8 country names en
  echo
  popd
fi

  # restart services
  if [[ "$(service nginx status >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    service nginx restart >/dev/null 2>&1
  fi
  if [[ "$(service php-fpm status >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    service php-fpm restart >/dev/null 2>&1
  fi

echo
cecho "Updated GeoIP databases" $boldyellow

exit