#!/bin/bash
############################################################
# update centmin mod 123.09beta01 & newer's optional
# geoip2 lite nginx module's database for geoip2 city
############################################################

geoiptwo_updater() {
  mkdir -p /usr/share/GeoIP
  pushd /usr/share/GeoIP
  echo "------------------------------------------------------"
  echo "GeoLite2 City database download ..."
  echo "------------------------------------------------------"
  echo
  GEOIPTWOCITYDATA_CURLCHECK=$(curl -4Is --connect-timeout 5 --max-time 5 https://geolite.maxmind.com/download/geoip/database/GeoLite2-City.tar.gz | grep 'HTTP\/' | grep '200' >/dev/null 2>&1; echo $?)  
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPTWOCITYDATA_CURLCHECK" = '0' ]]; then
    if [ -f /usr/share/GeoIP/GeoLite2-City.mmdb ] ; then
      ls -lah /usr/share/GeoIP/GeoLite2-City.mmdb
    fi
    wget -4 https://geolite.maxmind.com/download/geoip/database/GeoLite2-City.tar.gz -O /usr/share/GeoIP/GeoLite2-City.tar.gz
  fi
  tar xzf /usr/share/GeoIP/GeoLite2-City.tar.gz -C /usr/share/GeoIP
  cp -a GeoLite2-City_*/GeoLite2-City.mmdb /usr/share/GeoIP/GeoLite2-City.mmdb
  rm -rf GeoLite2-City_*

  echo "GeoLite2 Country database download ..."
  GEOIPTWOCOUNTRYDATA_CURLCHECK=$(curl -4Is --connect-timeout 5 --max-time 5 https://geolite.maxmind.com/download/geoip/database/GeoLite2-Country.tar.gz | grep 'HTTP\/' | grep '200' >/dev/null 2>&1; echo $?)
  # only overwrite existing downloaded file if the download url is working
  # if download doesn't work, do not overwrite existing downloaded file
  if [[ "$GEOIPTWOCOUNTRYDATA_CURLCHECK" = '0' ]]; then
    if [ -f /usr/share/GeoIP/GeoLite2-Country.mmdb ]; then
      ls -lah /usr/share/GeoIP/GeoLite2-Country.mmdb
    fi
    wget -4 https://geolite.maxmind.com/download/geoip/database/GeoLite2-Country.tar.gz -O /usr/share/GeoIP/GeoLite2-Country.tar.gz
  fi
  tar xzf /usr/share/GeoIP/GeoLite2-Country.tar.gz -C /usr/share/GeoIP
  cp -a GeoLite2-Country_*/GeoLite2-Country.mmdb /usr/share/GeoIP/GeoLite2-Country.mmdb
  rm -rf GeoLite2-Country_*

  echo "------------------------------------------------------"
  echo "Check GeoIP2 Lite Databases"
  echo "------------------------------------------------------"
  ls -lah /usr/share/GeoIP/GeoLite2-City.mmdb /usr/share/GeoIP/GeoLite2-Country.mmdb
  echo
  echo "/usr/local/bin/mmdblookup --file /usr/share/GeoIP/GeoLite2-Country.mmdb --ip 8.8.8.8 country names en"
  /usr/local/bin/mmdblookup --file /usr/share/GeoIP/GeoLite2-Country.mmdb --ip 8.8.8.8 country names en
  echo
  popd
}

geoiptwo_updater
exit 0