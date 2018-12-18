validcheck_downloads() {
  {
  echo
  echo "check download source fie integrity"
  for f in $(find $DIR_TMP -maxdepth 1 -type f -name "*.gz" -o -name "*.tgz"); do echo -n "$f = "; if [[ "$(tar -tzf $f >/dev/null; echo $?)" = '0' ]]; then echo ok; else echo invalid; fi; done | sort
  echo
} 2>&1 | tee "${CENTMINLOGDIR}/validcheck_downloads_${DT}.log"
}

checklogdetails() {
  echo " "
  cecho "----------------------------------------------------------------------------------" $boldyellow
  echo "check Centmin Mod log for details at /root/centminlogs/"
  echo "Aborting script..."
  cecho "----------------------------------------------------------------------------------" $boldyellow
  #exit #$ERROR
}

###############################################
# yumcentosfivedownload moved
# yumcentossixdownload moved
# yumcentossevendownload moved

###############################################
phpxz_detect() {
  if [ -n "$PHPMUVER" ]; then
    PHPXZ_CHECK="$PHPMUVER" # upgrade
  else
    PHPXZ_CHECK="$PHPMVER"  # clean install
    fi

  if [[ -f /usr/bin/xz && "$PHPXZ_CHECK" > 5.4 ]] && [[ "$lessphpmem" != [yY] ]]; then
    PHPEXTSION='xz'
    PHPTAR_FLAGS='xJf'
  else
    PHPEXTSION='gz'
    PHPTAR_FLAGS='xzf'
  fi
  
  if [[ "$CENTOS_SEVEN" = 7 ]]; then
    DOWNLOADAPP='axel -4'
    WGETRETRY=''
    AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.${PHPEXTSION}"
    AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.${PHPEXTSION}"
  elif [[ "$CENTOS_SIX" = 6 ]]; then
    DOWNLOADAPP='axel'
    WGETRETRY=''
    AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.${PHPEXTSION}"
    AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.${PHPEXTSION}"
  else
    DOWNLOADAPP="wget ${WGETOPT} --progress=bar"
    WGETRETRY='--tries=3'
    AXELPHPTARGZ=''
    AXELPHPUPGRADETARGZ=''
  fi   
}

phpgeolocation() {
  phpxz_detect
  # determine server geo location country wise and setup php download mirror 
  # closest to that country for faster downloads to speed up installs
  # upgrades
  # PHP_MIRRORURL='http://php.net'
  # PHPUPGRADE_MIRRORURL='http://php.net'
  
  # Get server's 2 digit country code
  SERVER_COUNTRY=$(curl -${ipv_forceopt}s https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'phone|postal|loc' | awk -F ": " '/country/ {print $2}')

  if [[ -z "$SERVER_COUNTRY" ]]; then
    PHP_MIRRORURL="http://php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'AU' ]]; then
    echo "AU server detected"
    PHP_MIRRORURL="http://au2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'BR' ]]; then
    echo "BR server detected"
    PHP_MIRRORURL="http://br2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'CA' ]]; then
    echo "CA server detected"
    PHP_MIRRORURL="http://ca1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'CN' ]]; then
    echo "CN server detected"
    PHP_MIRRORURL="http://cn2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'DE' ]]; then
    echo "DE server detected"
    PHP_MIRRORURL="http://de2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'FR' ]]; then
    echo "FR server detected"
    PHP_MIRRORURL="http://fr2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'GB' ]]; then
    echo "GB server detected"
    PHP_MIRRORURL="http://uk1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'HK' ]]; then
    echo "HK server detected"
    PHP_MIRRORURL="http://hk1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'ID' ]]; then
    echo "ID server detected"
    PHP_MIRRORURL="http://id1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'IN' ]]; then
    echo "IN server detected"
    PHP_MIRRORURL="http://in3.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'JP' ]]; then
    echo "JP server detected"
    PHP_MIRRORURL="http://jp2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'PH' ]]; then
    echo "PH server detected"
    PHP_MIRRORURL="http://sg2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'RU' ]]; then
    echo "RU server detected"
    PHP_MIRRORURL="http://lt1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'SG' ]]; then
    echo "SG server detected"
    PHP_MIRRORURL="http://sg2.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'TH' ]]; then
    echo "TH server detected"
    PHP_MIRRORURL="http://th1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'US' ]]; then
    echo "US server detected"
    PHP_MIRRORURL="http://php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  elif [[ "$SERVER_COUNTRY" = 'VN' ]]; then
    echo "VN server detected"
    PHP_MIRRORURL="http://my1.php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  else
    PHP_MIRRORURL="http://php.net"
    PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
  fi
}

###############################################
libresslgeolocation() {
  # determine server geo location country wise and setup libressl download mirror 
  # closest to that country for faster downloads http://www.openbsd.org/ftp.html
  
  # Get server's 2 digit country code
  SERVER_COUNTRY=$(curl -${ipv_forceopt}s --max-time 5 --connect-timeout 5 https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'phone|postal|loc' | awk -F ": " '/country/ {print $2}')

  if [[ -z "$SERVER_COUNTRY" ]]; then
    LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    fi
  elif [[ "$SERVER_COUNTRY" = 'AT' ]]; then
    echo "AT server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'AU' ]]; then
    echo "AU server detected"
    LIBRESSL_MIRRORURL="https://mirror.aarnet.edu.au/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'BG' ]]; then
    echo "BG server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'BR' ]]; then
    echo "BR server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'CA' ]]; then
    echo "CA server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'CN' ]]; then
    echo "CN server detected"
    LIBRESSL_MIRRORURL="https://mirror.yandex.ru/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'CR' ]]; then
    echo "CR server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'DE' ]]; then
    echo "DE server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'DK' ]]; then
    echo "DK server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'FR' ]]; then
    echo "FR server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'GB' ]]; then
    echo "GB server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'HK' ]]; then
    echo "HK server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'ID' ]]; then
    echo "ID server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'IE' ]]; then
    echo "IE server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'IN' ]]; then
    echo "IN server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'IT' ]]; then
    echo "IT server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'NL' ]]; then
    echo "NL server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'JP' ]]; then
    echo "JP server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'PH' ]]; then
    echo "PH server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'PL' ]]; then
    echo "PL server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'RU' ]]; then
    echo "RU server detected"
    LIBRESSL_MIRRORURL="https://mirror.yandex.ru/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'SE' ]]; then
    echo "SE server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'SG' ]]; then
    echo "SG server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'TH' ]]; then
    echo "TH server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'US' ]]; then
    echo "US server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="https://mirror.vdms.io/pub/OpenBSD/LibreSSL"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
        LIBRESSL_CURLCHECK=$?
        if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
          LIBRESSL_MIRRORURL="https://mirror.leaseweb.com/pub/OpenBSD/LibreSSL"
          LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
          curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
          LIBRESSL_CURLCHECK=$?
          if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
            LIBRESSL_MIRRORURL="http://mirrors.mit.edu/pub/OpenBSD/LibreSSL"
            LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
            curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
            LIBRESSL_CURLCHECK=$?
            if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
              LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
              LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
            fi
          fi
        fi
      fi
    fi
  elif [[ "$SERVER_COUNTRY" = 'VN' ]]; then
    echo "VN server detected"
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      fi
    fi
  else
    LIBRESSL_MIRRORURL="https://fastly.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
    LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
    LIBRESSL_CURLCHECK=$?
    if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
      LIBRESSL_MIRRORURL="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      LIBRESSL_CURLCHECK=$?
      if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
        LIBRESSL_MIRRORURL="https://mirror.vdms.io/pub/OpenBSD/LibreSSL"
        LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
        LIBRESSL_CURLCHECK=$?
        if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
          LIBRESSL_MIRRORURL="https://mirror.leaseweb.com/pub/OpenBSD/LibreSSL"
          LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
          curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
          LIBRESSL_CURLCHECK=$?
          if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
            LIBRESSL_MIRRORURL="http://mirrors.mit.edu/pub/OpenBSD/LibreSSL"
            LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
            curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
            LIBRESSL_CURLCHECK=$?
            if [[ "$LIBRESSL_CURLCHECK" != '0' ]]; then
              LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
              LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
            fi
          fi
        fi
      fi
    fi
  fi
}

###############################################
phptarball() {

if [[ "$PHP_INSTALL" = [yY] ]]; 
then

    phpgeolocation
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" | grep 'HTTP\/' | grep '404'
    PHPGEO_CURLCHECK=$?
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" | grep 'HTTP\/' | egrep '200|301|302'
    PHPGEO_CURLCHECKOK=$?
    # check if download url is 404 not found but also 
    # check for valid status header 200, 301 or 302
    if [[ "$PHPGEO_CURLCHECK" = '0' || "$PHPGEO_CURLCHECKOK" != '0' ]]; then
        PHP_MIRRORURL="http://php.net"
        PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
    fi
    cd "$DIR_TMP"

        cecho "Download php-${PHP_VERSION}.tar.${PHPEXTSION} ..." $boldyellow
    if [ -s "php-${PHP_VERSION}.tar.${PHPEXTSION}" ]; then
        cecho "php-${PHP_VERSION}.tar.${PHPEXTSION} found, skipping download..." $boldgreen
    else
        echo "$DOWNLOADAPP "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" $AXELPHPTARGZ $WGETRETRY"
        $DOWNLOADAPP "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" $AXELPHPTARGZ $WGETRETRY
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: php-${PHP_VERSION}.tar.${PHPEXTSION} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi
  
  tar $PHPTAR_FLAGS "php-${PHP_VERSION}.tar.${PHPEXTSION}"

ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: php-${PHP_VERSION}.tar.${PHPEXTSION} extraction failed." $boldgreen
    checklogdetails
    # exit #$ERROR
    if [[ "$ERROR" -eq '2' ]]; then
      PHP_MIRRORURL="http://php.net"
      PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
      rm -rf "php-${PHP_VERSION}.tar.${PHPEXTSION}"
      cecho "Download again php-${PHP_VERSION}.tar.${PHPEXTSION} ..." $boldyellow
      if [ -s "php-${PHP_VERSION}.tar.${PHPEXTSION}" ]; then
        cecho "php-${PHP_VERSION}.tar.${PHPEXTSION} found, skipping download..." $boldgreen
      else
        echo "$DOWNLOADAPP "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" $AXELPHPTARGZ $WGETRETRY"
        $DOWNLOADAPP "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" $AXELPHPTARGZ $WGETRETRY
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
          cecho "Error: php-${PHP_VERSION}.tar.${PHPEXTSION} download again failed." $boldgreen
          checklogdetails
        else 
          cecho "Download again done." $boldyellow
        fi
      fi
      tar $PHPTAR_FLAGS "php-${PHP_VERSION}.tar.${PHPEXTSION}"
    fi # error 2
else 
  cecho "php-${PHP_VERSION}.tar.${PHPEXTSION} valid file." $boldyellow
echo ""
  fi

fi

}


###############################################
# when adding additional modules, make sure to update inc/nginx_upgrade.inc
# checknginxmodules function needs updating as well

nginxpcretarball() {

    cd "$DIR_TMP"

        cecho "Download ${PCRELINKFILE} ..." $boldyellow
    if [[ -s "${PCRELINKFILE}" && "$(tar -tzf ${PCRELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${PCRELINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${PCRELINK} -O ${PCRELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${PCRELINK}" -O "${PCRELINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PCRELINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${PCRELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PCRELINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PCRELINKFILE} valid file." $boldyellow
echo ""
  fi
}

nginxzlibtarball() {

    cd "$DIR_TMP"

        cecho "Download ${NGX_ZLIBLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_ZLIBLINKFILE}" && "$(tar -tzf ${NGX_ZLIBLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_ZLIBLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_ZLIBLINK} -O ${NGX_ZLIBLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_ZLIBLINK}" -O "${NGX_ZLIBLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_ZLIBLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_ZLIBLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_ZLIBLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_ZLIBLINKFILE} valid file." $boldyellow
echo ""
  fi
}

nginxwebdavtarball() {

    cd "$DIR_TMP"

        cecho "Download ${NGX_WEBDAVLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_WEBDAVLINKFILE}" && "$(tar -tzf ${NGX_WEBDAVLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_WEBDAVLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo 'wget -c${ipv_forceopt} --progress=bar "${NGX_WEBDAVLINK}" -O "${NGX_WEBDAVLINKFILE}" --tries=3'
        wget -c${ipv_forceopt} --progress=bar "${NGX_WEBDAVLINK}" -O "${NGX_WEBDAVLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_WEBDAVLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_WEBDAVLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_WEBDAVLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_WEBDAVLINKFILE} valid file." $boldyellow
echo ""
  fi
}

nginxpgspeedtarball() {

  if [[ "$NGINX_PAGESPEED" = [yY] ]]; then

    cd "$DIR_TMP"

    # official github master zip renamed
    if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then

            cecho "Download ${NGX_PAGESPEEDGITLINKFILE} ..." $boldyellow
        if [ -s "${NGX_PAGESPEEDGITLINKFILE}" ]; then
          cecho "${NGX_PAGESPEEDGITLINKFILE} Archive found, skipping download..." $boldgreen
    else
          echo "wget -c${ipv_forceopt} --progress=bar ${NGX_PAGESPEEDGITLINK} -O ${NGX_PAGESPEEDGITLINKFILE} --tries=3"
          wget -c${ipv_forceopt} --progress=bar "${NGX_PAGESPEEDGITLINK}" -O "${NGX_PAGESPEEDGITLINKFILE}" --tries=3
      ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
          cecho "Error: ${NGX_PAGESPEEDGITLINKFILE} download failed." $boldgreen
          checklogdetails
          exit #$ERROR
        else 
              cecho "Download done." $boldyellow
          #echo ""
        fi
    fi
    tar xzf "${NGX_PAGESPEEDGITLINKFILE}"

    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
      cecho "Error: ${NGX_PAGESPEEDGITLINKFILE} extraction failed." $boldgreen
    checklogdetails
      exit #$ERROR
    else 
            cecho "${NGX_PAGESPEEDGITLINKFILE} valid file." $boldyellow
    echo ""
      fi
  fi # NGINX_PAGESPEEDGITMASTER

        cecho "Download ${NGX_PAGESPEEDLINKFILE} ..." $boldyellow
    if [[ "$NGINX_PAGESPEEDGITMASTER" != [yY] && -s "${NGX_PAGESPEEDLINKFILE}" && "$(tar -tzf ${NGX_PAGESPEEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_PAGESPEEDLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_PAGESPEEDLINK} -O ${NGX_PAGESPEEDLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_PAGESPEEDLINK}" -O "${NGX_PAGESPEEDLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_PAGESPEEDLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_PAGESPEEDLINKFILE}"

ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_PAGESPEEDLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_PAGESPEEDLINKFILE} valid file." $boldyellow
echo ""
  fi

# determine top level extracted directory name for $DIR_TMP/${NGX_PAGESPEEDLINKFILE}
if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
  # NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDGITLINKFILE}" | head -1 | cut -f1 -d"/")
  NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
else
  NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
fi

#if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
  # if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
  #   rm -rf "$DIR_TMP/$NGXPGSPEED_DIR"
  # fi
  # \cp -Rf incubator-pagespeed-ngx-master "$NGXPGSPEED_DIR"
  # rm -rf incubator-pagespeed-ngx-master
  # rm -rf "${NGX_PAGESPEEDGITLINKFILE}"
#fi

        cecho "Download ${NGX_PAGESPEEDPSOLINKLFILE} PSOL Library..." $boldyellow
        if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
            cd "$DIR_TMP/$NGXPGSPEED_DIR"
        fi

      # 1.12+ new method https://github.com/pagespeed/ngx_pagespeed/issues/1337
      PS_BRANCHVER=$(echo $NGXPGSPEED_VER | cut -c1-4| sed -e 's|\.|00|')
      if [[ "$PS_BRANCHVER" -ge '10013' ]]; then
        psol_url=$(scripts/format_binary_url.sh PSOL_BINARY_URL)
        wget -c $psol_url
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
          cecho "Error: $psol_url download failed." $boldgreen
          checklogdetails
        else 
          cecho "Download done." $boldyellow
        fi
        tar xzf "$(basename ${psol_url})"
      elif [[ "$PS_BRANCHVER" -ge '10012' ]]; then
        psol_url=$(scripts/format_binary_url.sh PSOL_BINARY_URL)
        wget -c $psol_url
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
          cecho "Error: $psol_url download failed." $boldgreen
          checklogdetails
        else 
          cecho "Download done." $boldyellow
        fi
        tar xzf "$(basename ${psol_url})"
      elif [[ "$PS_BRANCHVER" -le '10011' ]]; then
        if [[ -s "${NGX_PAGESPEEDPSOLINKLFILE}" && "$(tar -tzf ${NGX_PAGESPEEDPSOLINKLFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
          cecho "${NGX_PAGESPEEDPSOLINKLFILE} Archive found, skipping download..." $boldgreen
        else
          echo "wget -c${ipv_forceopt} --progress=bar ${NGX_PAGESPEEDPSOLINKL} -O ${NGX_PAGESPEEDPSOLINKLFILE} --tries=3"
          wget -c${ipv_forceopt} --progress=bar "${NGX_PAGESPEEDPSOLINKL}" -O "${NGX_PAGESPEEDPSOLINKLFILE}" --tries=3
          ERROR=$?
          if [[ "$ERROR" != '0' ]]; then
            cecho "Error: ${NGX_PAGESPEEDPSOLINKLFILE} download failed." $boldgreen
            checklogdetails
            exit #$ERROR
          else 
            cecho "Download done." $boldyellow
          fi
        fi
        tar xzf "${NGX_PAGESPEEDPSOLINKLFILE}"
      fi        
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_PAGESPEEDPSOLINKLFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_PAGESPEEDPSOLINKLFILE} valid file." $boldyellow
    cd "$DIR_TMP"
echo ""
  fi

  fi # NGINX_PAGESPEED=y

}

ngxmoduletarball() {
### nginx modules ###

    cd "$DIR_TMP"

        cecho "Download ${NGX_FANCYINDEXLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_FANCYINDEXLINKFILE}" && "$(tar -tzf ${NGX_FANCYINDEXLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "ngx-fancyindex $NGINX_FANCYINDEXVER Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar -O ${NGX_FANCYINDEXLINKFILE} ${NGX_FANCYINDEXLINK} --tries=3"
        wget -c${ipv_forceopt} --progress=bar -O "${NGX_FANCYINDEXLINKFILE}" "${NGX_FANCYINDEXLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_FANCYINDEXLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_FANCYINDEXLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_FANCYINDEXLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_FANCYINDEXLINKFILE} valid file." $boldyellow
echo ""
  fi

if [ ! -d "${DIR_TMP}/${NGX_FANCYINDEXLINKFILE}" ]; then
        cecho "Download ${NGX_FANCYINDEXLINKFILE} ..." $boldyellow
rm -rf "${NGX_FANCYINDEXLINKFILE}"

    if [[ -s "${NGX_FANCYINDEXLINKFILE}" && "$(tar -tzf ${NGX_FANCYINDEXLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "ngx-fancyindex $NGINX_FANCYINDEXVER Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar -O ${NGX_FANCYINDEXLINKFILE} ${NGX_FANCYINDEXLINK} --tries=3"
        wget -c${ipv_forceopt} --progress=bar -O "${NGX_FANCYINDEXLINKFILE}" "${NGX_FANCYINDEXLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_FANCYINDEXLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi
fi #fancyindex recheck

tar xzf "${NGX_FANCYINDEXLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_FANCYINDEXLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_FANCYINDEXLINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_CACHEPURGEFILE} ..." $boldyellow
    if [[ -s "${NGX_CACHEPURGEFILE}" && "$(tar -tzf ${NGX_CACHEPURGEFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "ngx_cache_purge ${NGINX_CACHEPURGEVER} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar -O ${NGX_CACHEPURGEFILE} ${NGX_CACHEPURGE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar -O "${NGX_CACHEPURGEFILE}" "${NGX_CACHEPURGE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_CACHEPURGEFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_CACHEPURGEFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_CACHEPURGEFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_CACHEPURGEFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_ACCESSKEYLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_ACCESSKEYLINKFILE}" && "$(tar -tzf ${NGX_ACCESSKEYLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        echo "wget -c ${NGX_ACCESSKEYLINK} -O ${NGX_ACCESSKEYLINKFILE} --tries=3"
        wget -c "${NGX_ACCESSKEYLINK}" -O "${NGX_ACCESSKEYLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_ACCESSKEYLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_ACCESSKEYLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_ACCESSKEYLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_ACCESSKEYLINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_CONCATLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_CONCATLINKFILE}" && "$(tar -tzf ${NGX_CONCATLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_CONCATLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_CONCATLINK} -O ${NGX_CONCATLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_CONCATLINK}" -O "${NGX_CONCATLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_CONCATLINKFILE} download failed." $boldgreen

checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_CONCATLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_CONCATLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_CONCATLINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_HEADERSMORELINKFILE} ..." $boldyellow
    if [[ -s "${NGX_HEADERSMORELINKFILE}" && "$(tar -tzf ${NGX_HEADERSMORELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_HEADERSMORELINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_HEADERSMORELINK} -O ${NGX_HEADERSMORELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_HEADERSMORELINK}" -O "${NGX_HEADERSMORELINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_HEADERSMORELINKFILE} download failed." $boldgreen

checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_HEADERSMORELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_HEADERSMORELINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_HEADERSMORELINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_STICKYLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_STICKYLINKFILE}" && "$(tar -tzf ${NGX_STICKYLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_STICKYLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_STICKYLINK} -O ${NGX_STICKYLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_STICKYLINK}" -O "${NGX_STICKYLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_STICKYLINKFILE} download failed." $boldgreen

checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_STICKYLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_STICKYLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_STICKYLINKFILE} valid file." $boldyellow
         mv nginx-goodies-nginx-sticky-module-ng-* "nginx-sticky-module-${NGINX_STICKYVER}"
echo ""
  fi

        cecho "Download ${NGX_UPSTREAMCHECKLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_UPSTREAMCHECKLINKFILE}" && "$(tar -tzf ${NGX_UPSTREAMCHECKLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_UPSTREAMCHECKLINKFILE} Archive found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_UPSTREAMCHECKLINK} -O ${NGX_UPSTREAMCHECKLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_UPSTREAMCHECKLINK}" -O "${NGX_UPSTREAMCHECKLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_UPSTREAMCHECKLINKFILE} download failed." $boldgreen

checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_UPSTREAMCHECKLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_UPSTREAMCHECKLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_UPSTREAMCHECKLINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_HTTPREDISLINKFILE} ..." $boldyellow
    if [[ -s "${NGX_HTTPREDISLINKFILE}" && "$(tar -tzf ${NGX_HTTPREDISLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "ngx_http_redis Archive found, skipping download..." $boldgreen
    else
        echo "wget -c ${NGX_HTTPREDISLINK} -O ${NGX_HTTPREDISLINKFILE} --tries=3"
        wget -c "${NGX_HTTPREDISLINK}" -O "${NGX_HTTPREDISLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_HTTPREDISLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_HTTPREDISLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_HTTPREDISLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_HTTPREDISLINKFILE} valid file." $boldyellow
echo ""
    fi    

nginxpcretarball
nginxwebdavtarball
nginxpgspeedtarball

### nginx modules ###
} # ngxmoduletarball


###############################################
openrestytarball() {
if [ "$NGINX_OPENRESTY" == 'y' ]; then

        cecho "Download ${NGX_MEMCLINKFILE} ..." $boldyellow
if [[ -s "${NGX_MEMCLINKFILE}" && "$(tar -tzf ${NGX_MEMCLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_MEMCLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_MEMCLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_MEMCLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_MEMCLINK} -O ${NGX_MEMCLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_MEMCLINK}" -O "${NGX_MEMCLINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_MEMCLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_MEMCLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_MEMCLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_MEMCLINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_SRCACHELINKFILE} ..." $boldyellow
if [[ -s "${NGX_SRCACHELINKFILE}" && "$(tar -tzf ${NGX_SRCACHELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_SRCACHELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_SRCACHELINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_SRCACHELINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_SRCACHELINK} -O ${NGX_SRCACHELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_SRCACHELINK}" -O "${NGX_SRCACHELINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_SRCACHELINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_SRCACHELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_SRCACHELINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_SRCACHELINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${NGX_DEVELKITLINKFILE} ..." $boldyellow
if [[ -s "${NGX_DEVELKITLINKFILE}" && "$(tar -tzf ${NGX_DEVELKITLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_DEVELKITLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_DEVELKITLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_DEVELKITLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_DEVELKITLINK} -O ${NGX_DEVELKITLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_DEVELKITLINK}" -O "${NGX_DEVELKITLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_DEVELKITLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_DEVELKITLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_DEVELKITLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_DEVELKITLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_SETMISCLINKFILE} ..." $boldyellow
if [[ -s "${NGX_SETMISCLINKFILE}" && "$(tar -tzf ${NGX_SETMISCLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_SETMISCLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_SETMISCLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_SETMISCLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_SETMISCLINK} -O ${NGX_SETMISCLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_SETMISCLINK}" -O "${NGX_SETMISCLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_SETMISCLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_SETMISCLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_SETMISCLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_SETMISCLINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_ECHOLINKFILE} ..." $boldyellow
if [[ -s "${NGX_ECHOLINKFILE}" && "$(tar -tzf ${NGX_ECHOLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_ECHOLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_ECHOLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_ECHOLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_ECHOLINK} -O ${NGX_ECHOLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_ECHOLINK}" -O "${NGX_ECHOLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_ECHOLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_ECHOLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_ECHOLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_ECHOLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_REDISLINKFILE} ..." $boldyellow
if [[ -s "${NGX_REDISLINKFILE}" && "$(tar -tzf ${NGX_REDISLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_REDISLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_REDISLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_REDISLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_REDISLINK} -O ${NGX_REDISLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_REDISLINK}" -O "${NGX_REDISLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_REDISLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_REDISLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_REDISLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_REDISLINKFILE} valid file." $boldyellow
echo ""
    fi # redis2    

    # ORESTY_LUANGINX=y|n
    if [[ "$ORESTY_LUANGINX" = [yY] ]]; then

        cecho "Download ${NGX_LUANGINXLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUANGINXLINKFILE}" && "$(tar -tzf ${NGX_LUANGINXLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUANGINXLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUANGINXLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUANGINXLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUANGINXLINK} -O ${NGX_LUANGINXLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUANGINXLINK}" -O "${NGX_LUANGINXLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUANGINXLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUANGINXLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUANGINXLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUANGINXLINKFILE} valid file." $boldyellow
echo ""
    fi

    # ORESTY_LUASTREAM=y|n
    if [[ "$ORESTY_LUASTREAM" = [yY] ]]; then

        cecho "Download ${NGX_LUASTREAMLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUASTREAMLINKFILE}" && "$(tar -tzf ${NGX_LUASTREAMLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUASTREAMLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUASTREAMLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUASTREAMLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUASTREAMLINK} -O ${NGX_LUASTREAMLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUASTREAMLINK}" -O "${NGX_LUASTREAMLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUASTREAMLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUASTREAMLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUASTREAMLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUASTREAMLINKFILE} valid file." $boldyellow
echo ""
    fi
    fi # ORESTY_LUASTREAM=y|n

        cecho "Download ${NGX_LUAGITLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAGITLINKFILE}" && "$(tar -tzf ${NGX_LUAGITLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAGITLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAGITLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAGITLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAGITLINK} -O ${NGX_LUAGITLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAGITLINK}" -O "${NGX_LUAGITLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAGITLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAGITLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAGITLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAGITLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUAMEMCACHEDLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAMEMCACHEDLINKFILE}" && "$(tar -tzf ${NGX_LUAMEMCACHEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAMEMCACHEDLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAMEMCACHEDLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAMEMCACHEDLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAMEMCACHEDLINK} -O ${NGX_LUAMEMCACHEDLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAMEMCACHEDLINK}" -O "${NGX_LUAMEMCACHEDLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAMEMCACHEDLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAMEMCACHEDLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAMEMCACHEDLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAMEMCACHEDLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUAMYSQLLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAMYSQLLINKFILE}" && "$(tar -tzf ${NGX_LUAMYSQLLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAMYSQLLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAMYSQLLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAMYSQLLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAMYSQLLINK} -O ${NGX_LUAMYSQLLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAMYSQLLINK}" -O "${NGX_LUAMYSQLLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAMYSQLLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAMYSQLLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAMYSQLLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAMYSQLLINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUAREDISLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAREDISLINKFILE}" && "$(tar -tzf ${NGX_LUAREDISLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAREDISLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAREDISLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAREDISLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAREDISLINK} -O ${NGX_LUAREDISLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAREDISLINK}" -O "${NGX_LUAREDISLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAREDISLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAREDISLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAREDISLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAREDISLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUADNSLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUADNSLINKFILE}" && "$(tar -tzf ${NGX_LUADNSLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUADNSLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUADNSLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUADNSLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUADNSLINK} -O ${NGX_LUADNSLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUADNSLINK}" -O "${NGX_LUADNSLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUADNSLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUADNSLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUADNSLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUADNSLINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUAUPLOADLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAUPLOADLINKFILE}" && "$(tar -tzf ${NGX_LUAUPLOADLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAUPLOADLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAUPLOADLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAUPLOADLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAUPLOADLINK} -O ${NGX_LUAUPLOADLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAUPLOADLINK}" -O "${NGX_LUAUPLOADLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPLOADLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAUPLOADLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPLOADLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAUPLOADLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUAWEBSOCKETLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAWEBSOCKETLINKFILE}" && "$(tar -tzf ${NGX_LUAWEBSOCKETLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAWEBSOCKETLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAWEBSOCKETLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAWEBSOCKETLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAWEBSOCKETLINK} -O ${NGX_LUAWEBSOCKETLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAWEBSOCKETLINK}" -O "${NGX_LUAWEBSOCKETLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAWEBSOCKETLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAWEBSOCKETLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAWEBSOCKETLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAWEBSOCKETLINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUALOCKLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUALOCKLINKFILE}" && "$(tar -tzf ${NGX_LUALOCKLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUALOCKLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUALOCKLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUALOCKLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUALOCKLINK} -O ${NGX_LUALOCKLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUALOCKLINK}" -O "${NGX_LUALOCKLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALOCKLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUALOCKLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALOCKLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUALOCKLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUASTRINGLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUASTRINGLINKFILE}" && "$(tar -tzf ${NGX_LUASTRINGLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUASTRINGLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUASTRINGLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUASTRINGLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUASTRINGLINK} -O ${NGX_LUASTRINGLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUASTRINGLINK}" -O "${NGX_LUASTRINGLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUASTRINGLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUASTRINGLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUASTRINGLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUASTRINGLINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUAREDISPARSERLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAREDISPARSERLINKFILE}" && "$(tar -tzf ${NGX_LUAREDISPARSERLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAREDISPARSERLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAREDISPARSERLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAREDISPARSERLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAREDISPARSERLINK} -O ${NGX_LUAREDISPARSERLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAREDISPARSERLINK}" -O "${NGX_LUAREDISPARSERLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAREDISPARSERLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAREDISPARSERLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAREDISPARSERLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAREDISPARSERLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUAUPSTREAMCHECKLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAUPSTREAMCHECKLINKFILE}" && "$(tar -tzf ${NGX_LUAUPSTREAMCHECKLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAUPSTREAMCHECKLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAUPSTREAMCHECKLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAUPSTREAMCHECKLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAUPSTREAMCHECKLINK} -O ${NGX_LUAUPSTREAMCHECKLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAUPSTREAMCHECKLINK}" -O "${NGX_LUAUPSTREAMCHECKLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMCHECKLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAUPSTREAMCHECKLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMCHECKLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAUPSTREAMCHECKLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUALRUCACHELINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUALRUCACHELINKFILE}" && "$(tar -tzf ${NGX_LUALRUCACHELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUALRUCACHELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUALRUCACHELINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUALRUCACHELINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUALRUCACHELINK} -O ${NGX_LUALRUCACHELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUALRUCACHELINK}" -O "${NGX_LUALRUCACHELINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALRUCACHELINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUALRUCACHELINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALRUCACHELINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUALRUCACHELINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUARESTYCORELINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUARESTYCORELINKFILE}" && "$(tar -tzf ${NGX_LUARESTYCORELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUARESTYCORELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUARESTYCORELINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUARESTYCORELINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUARESTYCORELINK} -O ${NGX_LUARESTYCORELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUARESTYCORELINK}" -O "${NGX_LUARESTYCORELINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUARESTYCORELINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUARESTYCORELINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUARESTYCORELINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUARESTYCORELINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUAUPSTREAMLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAUPSTREAMLINKFILE}" && "$(tar -tzf ${NGX_LUAUPSTREAMLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAUPSTREAMLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAUPSTREAMLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAUPSTREAMLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAUPSTREAMLINK} -O ${NGX_LUAUPSTREAMLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAUPSTREAMLINK}" -O "${NGX_LUAUPSTREAMLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAUPSTREAMLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAUPSTREAMLINKFILE} valid file." $boldyellow
echo ""
    fi      

        cecho "Download ${NGX_LUALOGGERSOCKETLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUALOGGERSOCKETLINKFILE}" && "$(tar -tzf ${NGX_LUALOGGERSOCKETLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUALOGGERSOCKETLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUALOGGERSOCKETLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUALOGGERSOCKETLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUALOGGERSOCKETLINK} -O ${NGX_LUALOGGERSOCKETLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUALOGGERSOCKETLINK}" -O "${NGX_LUALOGGERSOCKETLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALOGGERSOCKETLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUALOGGERSOCKETLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUALOGGERSOCKETLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUALOGGERSOCKETLINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUACOOKIELINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUACOOKIELINKFILE}" && "$(tar -tzf ${NGX_LUACOOKIELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUACOOKIELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUACOOKIELINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUACOOKIELINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUACOOKIELINK} -O ${NGX_LUACOOKIELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUACOOKIELINK}" -O "${NGX_LUACOOKIELINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUACOOKIELINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUACOOKIELINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUACOOKIELINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUACOOKIELINKFILE} valid file." $boldyellow
echo ""
    fi    

        cecho "Download ${NGX_LUAUPSTREAMCACHELINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUAUPSTREAMCACHELINKFILE}" && "$(tar -tzf ${NGX_LUAUPSTREAMCACHELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUAUPSTREAMCACHELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUAUPSTREAMCACHELINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUAUPSTREAMCACHELINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUAUPSTREAMCACHELINK} -O ${NGX_LUAUPSTREAMCACHELINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUAUPSTREAMCACHELINK}" -O "${NGX_LUAUPSTREAMCACHELINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMCACHELINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUAUPSTREAMCACHELINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUAUPSTREAMCACHELINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUAUPSTREAMCACHELINKFILE} valid file." $boldyellow
echo ""
    fi

        cecho "Download ${NGX_LUACJSONLINKFILE} ..." $boldyellow
if [[ -s "${NGX_LUACJSONLINKFILE}" && "$(tar -tzf ${NGX_LUACJSONLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${NGX_LUACJSONLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_LUACJSONLINKFILE} not found!!! Downloading now......" $boldgreen
  rm -rf "${NGX_LUACJSONLINKFILE}"
        echo "wget -c${ipv_forceopt} --progress=bar ${NGX_LUACJSONLINK} -O ${NGX_LUACJSONLINKFILE} --tries=3"
        wget -c${ipv_forceopt} --progress=bar "${NGX_LUACJSONLINK}" -O "${NGX_LUACJSONLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUACJSONLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi

tar xzf "${NGX_LUACJSONLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${NGX_LUACJSONLINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${NGX_LUACJSONLINKFILE} valid file." $boldyellow
echo ""
    fi    

    fi # ORESTY_LUANGINX=y|n
  
fi # openresty
}

###############################################
nginxtarball() {

if [[ "$NGINX_INSTALL" = [yY] ]]; 
then

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
    read -ep "Would you like to compile nginx with IPv6 support? [y/n] " asknginxipv
  else
  asknginxipv="$NGINX_IPV"
  fi #unattended

    cd "$DIR_TMP"

        cecho "Download ${NGX_LINKFILE} ..." $boldyellow
    if [[ -s "${NGX_LINKFILE}" && "$(tar -tzf ${NGX_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${NGX_LINKFILE} found, skipping download..." $boldgreen
    else
        rm -rf "${NGX_LINKFILE}"
        download_cmd "${NGX_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NGX_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NGX_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NGX_LINKFILE} valid file." $boldyellow
echo ""
  fi

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  ngxmoduletarball &
  openrestytarball &
  wait
else
  ngxmoduletarball
  openrestytarball
fi

fi # NGINX INSTALL

}

###############################################
libeventtarball() {

    cd "$DIR_TMP"

        cecho "Download ${LIBEVENTLINKFILE} ..." $boldyellow
if [[ -s "${LIBEVENTLINKFILE}" && "$(tar -tzf ${LIBEVENTLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${LIBEVENTLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${LIBEVENTLINKFILE} not found !!! Downloading now......" $boldgreen
  rm -rf "${LIBEVENTLINKFILE}"
        echo "wget ${WGETOPT} --progress=bar ${LIBEVENTLINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${LIBEVENTLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${LIBEVENTLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${LIBEVENTLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${LIBEVENTLINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${LIBEVENTLINKFILE} valid file." $boldyellow
echo ""
  fi

}

###############################################
mailparsephpextlocaltarball() {

    cd "$DIR_TMP"
    rm -rf mailparse*

        cecho "Download local ${PHPEXT_MAILPARSELINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MAILPARSELINKFILE}" && "$(tar -tzf ${PHPEXT_MAILPARSELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MAILPARSELINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MAILPARSELINKFILE} local not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MAILPARSELINKLOCAL} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MAILPARSELINKLOCAL}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MAILPARSELINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MAILPARSELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MAILPARSELINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PHPEXT_MAILPARSELINKFILE} local valid file." $boldyellow
echo ""
  fi
}

###############################################
mailparsephpexttarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_MAILPARSELINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MAILPARSELINKFILE}" && "$(tar -tzf ${PHPEXT_MAILPARSELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MAILPARSELINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MAILPARSELINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MAILPARSELINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MAILPARSELINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MAILPARSELINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MAILPARSELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MAILPARSELINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  mailparsephpextlocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_MAILPARSELINKFILE} valid file." $boldyellow
echo ""
  fi
}

###############################################
imagickphpextlocaltarball() {

    cd "$DIR_TMP"
    rm -rf imagick*

        cecho "Download local ${PHPEXT_IMAGICKLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_IMAGICKLINKFILE}" && "$(tar -tzf ${PHPEXT_IMAGICKLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_IMAGICKLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_IMAGICKLINKFILE} local not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_IMAGICKLINKLOCAL} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_IMAGICKLINKLOCAL}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_IMAGICKLINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_IMAGICKLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_IMAGICKLINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PHPEXT_IMAGICKLINKFILE} local valid file." $boldyellow
echo ""
  fi
}

###############################################
imagickphpexttarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_IMAGICKLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_IMAGICKLINKFILE}" && "$(tar -tzf ${PHPEXT_IMAGICKLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_IMAGICKLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_IMAGICKLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_IMAGICKLINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_IMAGICKLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_IMAGICKLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_IMAGICKLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_IMAGICKLINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  imagickphpextlocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_IMAGICKLINKFILE} valid file." $boldyellow
echo ""
  fi
}

###############################################
memcacheextlocaltarball() {

    cd "$DIR_TMP"
    rm -rf memcache*

        cecho "Download local ${PHPEXT_MEMCACHELINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MEMCACHELINKFILE}" && "$(tar -tzf ${PHPEXT_MEMCACHELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MEMCACHELINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHELINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHELINKLOCAL} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MEMCACHELINKLOCAL}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHELINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MEMCACHELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHELINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHELINKFILE} local valid file." $boldyellow
echo ""
  fi
}

###############################################
memcacheexttarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_MEMCACHELINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MEMCACHELINKFILE}" && "$(tar -tzf ${PHPEXT_MEMCACHELINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MEMCACHELINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHELINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHELINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MEMCACHELINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHELINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MEMCACHELINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHELINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  memcacheextlocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHELINKFILE} valid file." $boldyellow
echo ""
  fi
}

###############################################
memcachedphplocaltarball() {

    cd "$DIR_TMP"
    rm -rf memcached*

        cecho "Download local ${PHPEXT_MEMCACHEDLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MEMCACHEDLINKFILE}" && "$(tar -tzf ${PHPEXT_MEMCACHEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MEMCACHEDLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHEDLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHEDLINKLOCAL} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MEMCACHEDLINKLOCAL}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MEMCACHEDLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHEDLINKFILE} local valid file." $boldyellow
echo ""
  fi
}

###############################################
memcachedphptarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_MEMCACHEDLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MEMCACHEDLINKFILE}" && "$(tar -tzf ${PHPEXT_MEMCACHEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MEMCACHEDLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHEDLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHEDLINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MEMCACHEDLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_MEMCACHEDLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  memcachedphplocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHEDLINKFILE} valid file." $boldyellow
echo ""
  fi
}

###############################################
redisphplocaltarball() {

    cd "$DIR_TMP"
    rm -rf redis*

        cecho "Download local ${PHPEXT_REDISLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_REDISLINKFILE}" && "$(tar -tzf ${PHPEXT_REDISLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_REDISLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_REDISLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_REDISLINKLOCAL} -O ${PHPEXT_REDISLINKFILE} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_REDISLINKLOCAL}" -O "${PHPEXT_REDISLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_REDISLINKFILE} local download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
    fi
fi

tar xzf "${PHPEXT_REDISLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_REDISLINKFILE} local extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${PHPEXT_REDISLINKFILE} local valid file." $boldyellow
echo ""
    fi
}

###############################################
redisphptarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_REDISLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_REDISLINKFILE}" && "$(tar -tzf ${PHPEXT_REDISLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_REDISLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_REDISLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_REDISLINK} -O ${PHPEXT_REDISLINKFILE} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_REDISLINK}" -O "${PHPEXT_REDISLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_REDISLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
fi

tar xzf "${PHPEXT_REDISLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_REDISLINKFILE} extraction failed." $boldgreen
    cecho "trying local mirror download..." $boldgreen
    redisphplocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_REDISLINKFILE} valid file." $boldyellow
echo ""
    fi
}

###############################################
mongodbphplocaltarball() {

    cd "$DIR_TMP"
    rm -rf mongo*

        cecho "Download local ${PHPEXT_MONGODBLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MONGODBLINKFILE}" && "$(tar -tzf ${PHPEXT_MONGODBLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MONGODBLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MONGODBLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MONGODBLINKLOCAL} -O ${PHPEXT_MONGODBLINKFILE} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MONGODBLINKLOCAL}" -O "${PHPEXT_MONGODBLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_MONGODBLINKFILE} local download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
    fi
fi

tar xzf "${PHPEXT_MONGODBLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_MONGODBLINKFILE} local extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${PHPEXT_MONGODBLINKFILE} local valid file." $boldyellow
echo ""
    fi
}

###############################################
mongodbphptarball() {

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_MONGODBLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_MONGODBLINKFILE}" && "$(tar -tzf ${PHPEXT_MONGODBLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_MONGODBLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MONGODBLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_MONGODBLINK} -O ${PHPEXT_MONGODBLINKFILE} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_MONGODBLINK}" -O "${PHPEXT_MONGODBLINKFILE}" --tries=3
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_MONGODBLINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
fi

tar xzf "${PHPEXT_MONGODBLINKFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${PHPEXT_MONGODBLINKFILE} extraction failed." $boldgreen
    cecho "trying local mirror download..." $boldgreen
    mongodbphplocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_MONGODBLINKFILE} valid file." $boldyellow
echo ""
    fi
}

###############################################
libmemcachedlocaltarball() {

if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then

    cd "$DIR_TMP"

        cecho "Download local ${PHPEXT_LIBMEMCACHEDLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_LIBMEMCACHEDLINKFILE}" && "$(tar -tzf ${PHPEXT_LIBMEMCACHEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_LIBMEMCACHEDLINKLOCAL} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_LIBMEMCACHEDLINKLOCAL}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_LIBMEMCACHEDLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} local valid file." $boldyellow
echo ""
  fi
fi #LIBMEMCACHED_YUM
}

###############################################
libmemcachedtarball() {

if [[ "$LIBMEMCACHED_YUM" = [nN] || "$LIBMEMCACHED_YUM" = [yY] ]]; then

    cd "$DIR_TMP"

        cecho "Download ${PHPEXT_LIBMEMCACHEDLINKFILE} ..." $boldyellow
if [[ -s "${PHPEXT_LIBMEMCACHEDLINKFILE}" && "$(tar -tzf ${PHPEXT_LIBMEMCACHEDLINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} not found !!! Downloading now......" $boldgreen
        echo "wget ${WGETOPT} --progress=bar ${PHPEXT_LIBMEMCACHEDLINK} --tries=3"
        wget ${WGETOPT} --progress=bar "${PHPEXT_LIBMEMCACHEDLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PHPEXT_LIBMEMCACHEDLINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  libmemcachedlocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} valid file." $boldyellow
echo ""
  fi
fi #LIBMEMCACHED_YUM
}

###############################################
twemperftarball() {

    cd "$DIR_TMP"

        cecho "Download ${TWEMPERF_LINKFILE} ..." $boldyellow
if [[ -s "${TWEMPERF_LINKFILE}" && "$(tar -tzf ${TWEMPERF_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${TWEMPERF_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${TWEMPERF_LINKFILE} not found !!! Downloading now......" $boldgreen
        
     echo "wget ${WGETOPT} --progress=bar ${TWEMPERF_LINK} -O ${TWEMPERF_LINKFILE} --tries=3"
     wget ${WGETOPT} --progress=bar "${TWEMPERF_LINK}" -O "${TWEMPERF_LINKFILE}" --tries=3

ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${TWEMPERF_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

if [ ! -d "twemperf-${TWEMPERF_VER}" ]; then
tar xzf "${TWEMPERF_LINKFILE}"
fi

ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${TWEMPERF_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${TWEMPERF_LINKFILE} valid file." $boldyellow
echo ""
  fi

}

###############################################
memcachetarball() {
  MEMCACHED_ALT="$1"

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Memcached Server? (default uses 16MB RAM each) [y/n] " askmemcinstall
  else
  askmemcinstall='y'
  fi #unattended
if [[ "$askmemcinstall" = [yY] ]];
then
MEMCINSTALL='y'

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install how many Memcached servers ? 1 or 2 ?: " nomemcachedservers
echo "will install $nomemcachedservers Memcached servers"
echo ""
  else
  nomemcachedservers='1'
  fi #unattended

libeventtarball

#########################################################
        cecho "Download ${MEMCACHEDSERVER_LINKFILE} ..." $boldyellow
if [[ -s "${MEMCACHEDSERVER_LINKFILE}" && "$(tar -tzf ${MEMCACHEDSERVER_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${MEMCACHEDSERVER_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${MEMCACHEDSERVER_LINKFILE} not found !!! Downloading now......" $boldgreen
  curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${MEMCACHEDSERVER_LINK}" | grep 'HTTP\/' | grep '200'
  MEMCACHEDSERVER_CURLCHECK=$?
  if [[ "$MEMCACHEDSERVER_CURLCHECK" != '0' ]]; then
    MEMCACHEDSERVER_LINKFILE="memcached-${MEMCACHED_VERSION}.tar.gz"
    MEMCACHEDSERVER_LINK="$MEMCACHEDSERVER_LINKLOCAL"
  fi
  if [[ "$MEMCACHED_ALT" != 'local' ]]; then
    echo "wget ${WGETOPT} --progress=bar ${MEMCACHEDSERVER_LINK} --tries=3"
    wget ${WGETOPT} --progress=bar "${MEMCACHEDSERVER_LINK}" --tries=3
  else
    echo "wget ${WGETOPT} --progress=bar ${MEMCACHEDSERVER_LINKLOCAL} --tries=3"
    wget ${WGETOPT} --progress=bar "${MEMCACHEDSERVER_LINKLOCAL}" --tries=3
  fi
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${MEMCACHEDSERVER_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${MEMCACHEDSERVER_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${MEMCACHEDSERVER_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${MEMCACHEDSERVER_LINKFILE} valid file." $boldyellow
echo ""
  fi

#########################################################
memcacheexttarball
memcachedphptarball
libmemcachedtarball
# twemperftarball
#########################################################

fi

}

###############################################
mysqltools() {

    cd /root

        cecho "Download ${MYSQLREPORT_LINKFILE} ..." $boldyellow
    if [ -s "${MYSQLREPORT_LINKFILE}" ]; then
        cecho "${MYSQLREPORT_LINKFILE} found, skipping download..." $boldgreen
    else
        download_cmd "${MYSQLREPORT_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${MYSQLREPORT_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download ${MYSQLREPORT_LINKFILE} done." $boldyellow
#echo ""
  fi
    fi

        cecho "Download ${MYSQLTUNER_LINKFILE} ..." $boldyellow
    if [ -s "${MYSQLTUNER_LINKFILE}" ]; then
        cecho "${MYSQLTUNER_LINKFILE} found, skipping download..." $boldgreen
    else
        echo 'wget ${WGETOPT} --progress=bar "${MYSQLTUNER_LINK}" -O "${MYSQLTUNER_LINKFILE}" --tries=3'
        wget ${WGETOPT} --progress=bar "${MYSQLTUNER_LINK}" -O "${MYSQLTUNER_LINKFILE}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${MYSQLTUNER_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download ${MYSQLTUNER_LINKFILE} done." $boldyellow
#echo ""
  fi
    fi

    chmod +x mysqlreport "${MYSQLTUNER_LINKFILE}"

}

###############################################
mariadbrpms() {


if [[ "$MDB_INSTALL" = [yY] ]]; 
then

    # The MariaDB mirror uses x86 and amd64 prefixes for rpm folders instead of i386/x84_64, so compensate for that...
    if [ ${ARCH} == 'x86_64' ];
    then
        MDB_ARCH='amd64'
    else
        MDB_ARCH='x86'
        ARCH='i386'
    fi

    cd "$DIR_TMP"

        cecho "Download MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s "MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm" ]; then
        cecho "MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        download_cmd "http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

        cecho "Download MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s "MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm" ]; then
        cecho "MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        download_cmd "http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

        cecho "Download  MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s "MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm" ]; then
        cecho "MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        download_cmd "http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

        cecho "Download MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s "MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm" ]; then
        cecho "MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        download_cmd "http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

        cecho "Download MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s "MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm" ]; then
        cecho "MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        download_cmd "http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

fi

}

###############################################
mariadbyumrepo() {

if [[ "$MDB_YUMREPOINSTALL" = [yY] ]]; 
then

    cd "$DIR_TMP"

        cecho "Download ourdelta-release-5-1.noarch.rpm ..." $boldyellow
    if [ -s ourdelta-release-5-1.noarch.rpm ]; then
        cecho "ourdelta-release-5-1.noarch.rpm found, skipping download..." $boldgreen
    else
        download_cmd http://master.ourdelta.org/yum/CentOS-MariaDB52/ourdelta-release-5-1.noarch.rpm "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ourdelta-release-5-1.noarch.rpm download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

fi

}


###############################################
gperftools() {

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

    cd "$DIR_TMP"

        cecho "Download ${LIBUNWIND_LINKFILE} ..." $boldyellow
    if [[ -s "${LIBUNWIND_LINKFILE}" && "$(tar -tzf ${LIBUNWIND_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "libunwind ${LIBUNWIND_VERSION} found, skipping download..." $boldgreen 
    else
        download_cmd "${LIBUNWIND_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${LIBUNWIND_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${LIBUNWIND_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${LIBUNWIND_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${LIBUNWIND_LINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${GPERFTOOL_LINKFILE} ..." $boldyellow
    if [[ -s "${GPERFTOOL_LINKFILE}" && "$(tar -tzf ${GPERFTOOL_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} found, skipping download..." $boldgreen
    else
        download_cmd "${GPERFTOOL_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${GPERFTOOL_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${GPERFTOOL_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${GPERFTOOL_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${GPERFTOOL_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
openssldownload() {

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  OPENSSL_VERSION_CUSTOMCONFIG=$(awk -F "'" '/^OPENSSL_VERSION=/ {print $2}' "${CONFIGSCANBASE}/custom_config.inc")
  if [[ "${OPENSSL_VERSION}" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0i' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0h' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0g' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0f' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0e' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0d' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0c' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0b' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0a' ]]; then
    # force old OpenSSL 1.1.0 branch versions to newer
    # 1.1.1 branch if detected as some folks hardcode override
    # OPENSSL_VERSION variable in /etc/centminmod/custom_config.inc
    # and forget to update them and over time they are out of sync
    # with OPENSSL_VERSION updated and set in centmin.sh
    #
    # also OpenSSL 1.1.0j seems to be failing Nginx compiles so this
    # is a workaround to jump to 1.1.1a working version for now
    OPENSSL_VERSION="$OPENSSL_VERSION_OLDOVERRIDE"
    OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
    OPENSSL_LINK="https://www.openssl.org/source/${OPENSSL_LINKFILE}"
  fi
fi

curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$OPENSSL_LINK" | grep 'HTTP\/' | grep '200'
OPENSSL_CURLCHECK=$?
if [[ "$OPENSSL_CURLCHECK" != '0' ]]; then
  OPENSSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/openssl"
  OPENSSL_LINK="${OPENSSL_MIRRORURL}/openssl-${OPENSSL_VERSION}.tar.gz"
fi

if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then

    cd "$DIR_TMP"

        cecho "Download ${OPENSSL_LINKFILE} ..." $boldyellow
    if [[ -s "${OPENSSL_LINKFILE}" && "$(tar -tzf ${OPENSSL_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        if [[ "$CENTOS_SIX" = '6' ]]; then
          echo "wget -c${ipv_forceopt} --progress=bar "${OPENSSL_LINK}" "$WGETRETRY""
          wget -c${ipv_forceopt} --progress=bar "${OPENSSL_LINK}" "$WGETRETRY"
          ERROR=$?
          echo $ERROR
        else
          echo "download_cmd "${OPENSSL_LINK}" "$WGETRETRY""
          download_cmd "${OPENSSL_LINK}" "$WGETRETRY"
          ERROR=$?
          echo $ERROR
        fi

  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${OPENSSL_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${OPENSSL_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${OPENSSL_LINKFILE} extraction failed." $boldgreen
checklogdetails
  # exit #$ERROR
else 
         cecho "${OPENSSL_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi # for nginx openssl

if [[ "$CENTOSVER" > 6 ]]; then
    # Install OpenSSL
    cd "$DIR_TMP"

        cecho "Download ${OPENSSL_LINKFILE} ..." $boldyellow
    if [[ -s "${OPENSSL_LINKFILE}" && "$(tar -tzf ${OPENSSL_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        if [[ "$CENTOS_SIX" = '6' ]]; then
          echo "wget -c${ipv_forceopt} --progress=bar "${OPENSSL_LINK}" "$WGETRETRY""
          wget -c${ipv_forceopt} --progress=bar "${OPENSSL_LINK}" "$WGETRETRY"
          ERROR=$?
          echo $ERROR
        else
          echo "download_cmd "${OPENSSL_LINK}" "$WGETRETRY""
          download_cmd "${OPENSSL_LINK}" "$WGETRETRY"
          ERROR=$?
          echo $ERROR
        fi

  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${OPENSSL_LINKFILE} download failed." $boldgreen
  ping -c4 $(echo "${OPENSSL_LINK}" | awk -F "/" '{print $3}')
  dig $(echo "${OPENSSL_LINK}" | awk -F "/" '{print $3}')
checklogdetails
  # exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${OPENSSL_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${OPENSSL_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${OPENSSL_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi # openssl centos 6

}

###############################################
libressldownload() {
    # Install LibreSSL
    libresslgeolocation
    cd "$DIR_TMP"

        cecho "Download ${LIBRESSL_LINKFILE} ..." $boldyellow
    if [[ -s "${LIBRESSL_LINKFILE}" && "$(tar -tzf ${LIBRESSL_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "LibreSSL ${LIBRESSL_VERSION} found, skipping download..." $boldgreen
    else
        download_cmd "${LIBRESSL_LINK}" $WGETRETRY
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${LIBRESSL_LINKFILE} download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
    fi
    fi
if [ -f "${LIBRESSL_LOCALLINKFILE}" ]; then
    tar xzf "${LIBRESSL_LOCALLINKFILE}"
else
    tar xzf "${LIBRESSL_LINKFILE}"
fi
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${LIBRESSL_LINKFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${LIBRESSL_LINKFILE} valid file." $boldyellow
echo ""
    fi
}

###############################################
pcretarball() {

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then

    cd "$DIR_TMP"

        cecho "Download pcre-${PCRE_VERSION}.tar.gz ..." $boldyellow
    if [[ -s "pcre-${PCRE_VERSION}.tar.gz" && "$(tar -tzf pcre-${PCRE_VERSION}.tar.gz >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "pcre ${PCRE_VERSION} found, skipping download..." $boldgreen
    else
        download_cmd "${PCRE_SOURCELINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: pcre-${PCRE_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "pcre-${PCRE_VERSION}.tar.gz"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: pcre-${PCRE_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "pcre-${PCRE_VERSION}.tar.gz valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
siegetarball() {
#SIEGE_VERSION

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Siege Benchmark script ? [y/n] " asksiegeinstall
  else
  asksiegeinstall='y'
  fi #unattended
if [[ "$asksiegeinstall" = [yY] ]];
then
SIEGEINSTALL='y'

    cd "$DIR_TMP"

        cecho "Download ${SIEGE_LINKFILE} ..." $boldyellow
    if [[ -s "${SIEGE_LINKFILE}" && "$(tar -tzf ${SIEGE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${SIEGE_LINKFILE} found, skipping download..." $boldgreen
    else
        download_cmd "${SIEGE_LINKLOCAL}" $WGETRETRY
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${SIEGE_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${SIEGE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${SIEGE_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${SIEGE_LINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${SPROXY_LINKFILE} ..." $boldyellow
    if [[ -s "${SPROXY_LINKFILE}" && "$(tar -tzf ${SPROXY_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "sproxy-latest found, skipping download..." $boldgreen
    else
        download_cmd "https://centminmod.com/centminmodparts/siege/${SPROXY_LINKFILE}" $WGETRETRY
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${SPROXY_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""

  fi
    fi

tar xzf "${SPROXY_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${SPROXY_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${SPROXY_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
pythontarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Python Update ? [y/n] " askpythonupdate
  else
  askpythonupdate='n'
  fi #unattended
if [[ "$askpythonupdate" = [yY] ]];
then
PYTHONUPDATE='y'

    cd "$DIR_TMP"

        cecho "Download ${PYTHON_LINKFILE} ..." $boldyellow
if [[ -s "${PYTHON_LINKFILE}" && "$(tar -tzf ${PYTHON_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  cecho "${PYTHON_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_LINKFILE} not found !!! Downloading now......" $boldgreen
        download_cmd "http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "${PYTHON_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
fi

tar xzf "${PYTHON_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${PYTHON_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${PYTHON_LINKFILE} valid file." $boldyellow
echo ""
  fi

        cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} ..." $boldyellow
if [ -s "${PYTHON_SETUPTOOLSLINKFILE}" ]; then
  cecho "${PYTHON_SETUPTOOLSLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_SETUPTOOLSLINKFILE} not found !!! Downloading now......" $boldgreen
        echo 'wget -c${ipv_forceopt} --progress=bar "${PYTHON_SETUPTOOLSLINK}" --tries=3'
        wget -c${ipv_forceopt} --progress=bar "${PYTHON_SETUPTOOLSLINK}" --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "${PYTHON_SETUPTOOLSLINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} done." $boldyellow
echo ""
  fi
fi

#test egg file validity check

fi

}

###############################################
xcachetarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install XCache? (By default uses 32MB RAM) If XCache installed DO NOT install APC [y/n] " askxcacheinstall
  else
  askxcacheinstall='n'
  fi #unattended

# menu #8 install xcache
if [[ "$MANXCACHEINSTALL" = 'y' ]]; then
  askxcacheinstall='y'
fi

if [[ "$askxcacheinstall" = [yY] ]];then
XCACHESRCINSTALL='y'

    cd "$DIR_TMP"

        cecho "Download ${XCACHE_LINKFILE} ..." $boldyellow
    if [[ -s "${XCACHE_LINKFILE}" && "$(tar -tzf ${XCACHE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "xcache ${XCACHE_VERSION} Archive found, skipping download..." $boldgreen
    else
        download_cmd "${XCACHE_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${XCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${XCACHE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${XCACHE_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${XCACHE_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
apclocaltarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
  else
  askapcinstall='y'
  fi #unattended
if [[ "$askapcinstall" = [yY] ]]; then
APCINSTALL='y'
    cd "$DIR_TMP"

    rm -rf APC*

        cecho "Download local ${APCCACHE_LINKFILE} ..." $boldyellow
    if [[ -s "${APCCACHE_LINKFILE}" && "$(tar -tzf ${APCCACHE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${APCCACHE_LINKFILE} Archive (local) found, skipping download..." $boldgreen
    else
        download_cmd "${APCCACHE_LINKLOCAL}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${APCCACHE_LINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${APCCACHE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${APCCACHE_LINKFILE} (local) extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${APCCACHE_LINKFILE} (local) valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
apctarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
  else
  askapcinstall='y'
  fi #unattended
if [[ "$askapcinstall" = [yY] ]]; then
APCINSTALL='y'
    cd "$DIR_TMP"

        cecho "Download ${APCCACHE_LINKFILE} ..." $boldyellow
    if [[ -s "${APCCACHE_LINKFILE}" && "$(tar -tzf ${APCCACHE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "${APCCACHE_LINKFILE} Archive found, skipping download..." $boldgreen
    else
        download_cmd "${APCCACHE_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${APCCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${APCCACHE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${APCCACHE_LINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  apclocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${APCCACHE_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
zopcachelocaltarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
  else
  askzopcacheinstall='y'
  fi #unattended
if [[ "$askzopcacheinstall" = [yY] ]]; then
ZOPCACHEINSTALL='y'
    cd "$DIR_TMP"
    rm -rf zendopcache*

        cecho "Download local ${ZENDOPCACHE_LINKFILE} ..." $boldyellow
    if [[ -s "${ZENDOPCACHE_LINKFILE}" && "$(tar -tzf ${ZENDOPCACHE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho " ${ZENDOPCACHE_LINKFILE} local Archive found, skipping download..." $boldgreen
    else
        download_cmd "${ZENDOPCACHE_LINKLOCAL}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${ZENDOPCACHE_LINKFILE} local download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${ZENDOPCACHE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${ZENDOPCACHE_LINKFILE} local extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${ZENDOPCACHE_LINKFILE} local valid file." $boldyellow
echo ""
  fi

fi

}

###############################################
zopcachetarball() {

echo ""
  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
  else
  askzopcacheinstall='y'
  fi #unattended
if [[ "$askzopcacheinstall" = [yY] ]]; then
ZOPCACHEINSTALL='y'
    cd "$DIR_TMP"

        cecho "Download ${ZENDOPCACHE_LINKFILE} ..." $boldyellow
    if [[ -s "${ZENDOPCACHE_LINKFILE}" && "$(tar -tzf ${ZENDOPCACHE_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho " ${ZENDOPCACHE_LINKFILE} Archive found, skipping download..." $boldgreen
    else
        download_cmd "${ZENDOPCACHE_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${ZENDOPCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${ZENDOPCACHE_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${ZENDOPCACHE_LINKFILE} extraction failed." $boldgreen
  cecho "trying local mirror download..." $boldgreen
  zopcachelocaltarball
# checklogdetails
#   exit #$ERROR
else 
         cecho "${ZENDOPCACHE_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################

nsdtarball() {
if [[ "$NSD_INSTALL" = [yY] ]]; 
then

    cd "$DIR_TMP"

        cecho "Download ${NSD_LINKFILE} ..." $boldyellow
    if [[ -s "${NSD_LINKFILE}" && "$(tar -tzf ${NSD_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "NSD ${NSD_VERSION} Archive found, skipping download..." $boldgreen 
    else
        download_cmd "${NSD_LINK}" $WGETRETRY
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NSD_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download ${NSD_LINKFILE} done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${NSD_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${NSD_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${NSD_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi
}


###############################################
csftarball() {

  if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install CSF firewall script ? [y/n] " askcsfinstall
  else
  askcsfinstall='y'
  fi #unattended
if [[ "$askcsfinstall" = [yY] ]];
then

CSFINSTALLOK='y'

    cd "$DIR_TMP"

    # change to always look for remote csf firewall tarball
    # and use it first and only fall back on local included 
    # csf tarball if you can not reach the official csf
    # site tarball download
    CSF_SITECHECK=$(curl -${ipv_forceopt}sI ${CSF_LINK} | grep 'HTTP\/' | awk '/200/ {print $2}')
    if [[ "$CSF_SITECHECK" = '200' ]]; then
      mv "${CSF_LINKFILE}" "${CSF_LINKFILE}-local"
    fi

        cecho "Download ${CSF_LINKFILE} ..." $boldyellow
    if [[ -s "${CSF_LINKFILE}" && "$(tar -tzf ${CSF_LINKFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        cecho "csf Archive found, skipping download..." $boldgreen
    else
        download_cmd "${CSF_LINK}" "$WGETRETRY" 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CSF_LINKFILE} download failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "Download ${CSF_LINKFILE} done." $boldyellow
#echo ""
  fi
    fi

tar xzf "${CSF_LINKFILE}"
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CSF_LINKFILE} extraction failed." $boldgreen
checklogdetails
  exit #$ERROR
else 
         cecho "${CSF_LINKFILE} valid file." $boldyellow
echo ""
  fi

fi

}

###############################################

ccachetarball() {
    cd "$DIR_TMP"

        cecho "Download ${CCACHEFILE} ..." $boldyellow
    if [[ -s "${CCACHEFILE}" && "$(tar -tzf ${CCACHEFILE} >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
      cecho "ccache ${CCACHE_VER} Archive found, skipping download..." $boldgreen 
    else
      download_cmd ${CCCACHELINK} $WGETRETRY
      ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        CCACHE_FAILEDDOWNLOAD='y'
        rm -rf "${DIR_TMP}/ccache-${CCACHE_VER}*"
        CCACHEFILE="ccache-${CCACHE_VER}.tar.gz"
        CCCACHELINK="https://centmin.sh/centminmodparts/ccache/${CCACHEFILE}"
        cecho "Error: ${CCACHEFILE} download failed." $boldgreen
        echo
        cecho "Re-try download via alternate ccache ${CCACHE_VER} mirror ${CCCACHELINK}" $boldgreen 
        download_cmd ${CCCACHELINK} $WGETRETRY
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
          rm -rf "${DIR_TMP}/ccache-${CCACHE_VER}*"
          CCACHEFILE="ccache-${CCACHE_VER}.tar.gz"
          CCCACHELINK="https://centmin.com/centminmodparts/ccache/${CCACHEFILE}"
          cecho "Error: ${CCACHEFILE} download failed." $boldgreen
          echo
          cecho "Re-try again download via alternate ccache ${CCACHE_VER} mirror ${CCCACHELINK}" $boldgreen 
          download_cmd ${CCCACHELINK} $WGETRETRY
          ERROR=$?
        else
          cecho "Download ${CCACHEFILE} done." $boldyellow
        fi
      else
        cecho "Download ${CCACHEFILE} done." $boldyellow
      fi
    fi

tar xzf "${CCACHEFILE}"
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CCACHEFILE} extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "${CCACHEFILE} valid file." $boldyellow
echo ""
    fi
}
###############################################

alldownloads() {

echo "*****************************************************"
cecho "Downloading all required software for install routines." $boldgreen
echo "*****************************************************"

# use local download source files first 
# less likely to encounter broken 3rd party
# urls and speed up initial install
# unzip svr-setup.zip
if [ -f "${CM_INSTALLDIR}/downloads/svr-setup.zip" ]; then
    /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/svr-setup.zip" -d /
fi
if [ -f "${CM_INSTALLDIR}/downloads/rpms.zip" ]; then
    /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/rpms.zip" -d "$DIR_TMP"/
fi
if [ -f "${CM_INSTALLDIR}/downloads/curlrpms.zip" ]; then
    /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/curlrpms.zip" -d "$DIR_TMP"/
fi
if [ -f "${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip" ]; then
    /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip" -d "$DIR_TMP"/
fi
if [ -f "${CM_INSTALLDIR}/downloads/${LIBRESSL_LOCALLINKFILE}" ]; then
    cp "${CM_INSTALLDIR}/downloads/${LIBRESSL_LOCALLINKFILE}" "$DIR_TMP"
fi

CCACHE_ERROR=$?
    if [[ "$CCACHE_ERROR" != '0' ]]; then
        /usr/bin/unzip -qo /usr/local/src/centminmod/downloads/svr-setup.zip -d /
    fi

yuminstall

questions

if [[ "$INITIALINSTALL" != [yY] ]]; then
    php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3'
else
    echo "$PHP_VERSION" | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3'
fi
PHPSEVEN_CHECKVER=$?
echo "$PHPSEVEN_CHECKVER"

# mailparse 3.0.0 only works with PHP 7.0 branch
# so use 2.1.6 if not PHP 7.0 branch
if [[ "$PHPSEVEN_CHECKVER" = '0' ]]; then
    MAILPARSEPHP_VER="$MAILPARSEPHP_COMPATVER"
    PHPEXT_MAILPARSELINKFILE="mailparse-${MAILPARSEPHP_VER}.tgz"
    PHPEXT_MAILPARSELINK="https://pecl.php.net/get/${PHPEXT_MAILPARSELINKFILE}"
    PHPEXT_MAILPARSELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mailparse/${PHPEXT_MAILPARSELINKFILE}"
fi

# yum double check
if [[ "$FAILEPEL" != 'y' || "$FAILRPMFORGE" != 'y' || "$FAILCENTALT" != 'y' ]]; then

    # echo "\${CUR_DIR} & \${CM_INSTALLDIR}"
    # echo ${CUR_DIR}
    # echo ${CM_INSTALLDIR}    

ccachetarball
ccacheinstall

axelsetup
nginxtarball

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  csftarball &
  pythontarball &
  mailparsephpexttarball &
  nginxzlibtarball &
  wait
else
  csftarball
  pythontarball
  mailparsephpexttarball
  nginxzlibtarball
fi

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  phptarball &
  xcachetarball &
  apctarball &
  zopcachetarball &
  wait
else
  phptarball
  xcachetarball
  apctarball
  zopcachetarball
fi

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  memcachetarball &
  imagickphpexttarball &
  redisphptarball &
  mongodbphptarball &
  wait
else
  memcachetarball
  imagickphpexttarball
  redisphptarball
  mongodbphptarball
fi

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  gperftools &
  if [[ "$LIBRESSL_SWITCH" = 'n' ]]; then
    openssldownload &
  fi
  libressldownload &
  pcretarball &
  wait
else
  gperftools
  if [[ "$LIBRESSL_SWITCH" = 'n' ]]; then
    openssldownload
  fi
  libressldownload
  pcretarball
fi

if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
  siegetarball &
  mysqltools &
  nsdtarball &
  mariadbrpms &
  wait
else
  siegetarball
  mysqltools
  nsdtarball
  mariadbrpms
fi

#mariadbyumrepo
#validcheck_downloads

echo "*****************************************************"
cecho "Downloads complete." $boldgreen
echo "*****************************************************"

  if [ "$UNATTENDED" == 'n' ]; then
echo ""
read -ep "You you want to continue installation ?  [y/n] " downloadcontinstall

if [[ "$downloadcontinstall" = [nN] ]]; then
exit
fi
  fi #unattended

else
echo ""
echo "Error installing or downloading YUM repositories..."
checklogdetails
exit

fi # yum double check

}