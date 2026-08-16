########################################################################
phpng_download() {
  if [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '8.6.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '8.6.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '8.6.0RC'  ]]; then
    PHPNG_YES='y'
    PHPNGVER=$PHPMU_ALPHAVER
    # curl check as download url keeps changing between releases
    REMIURL='https://downloads.php.net/~saki/'
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
    REMIURL_CURLCHECK=$?
    if [[ "$REMIURL_CURLCHECK" != '0' ]]; then
      REMIURL='https://downloads.php.net/~calvinb/'
    fi
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
    REMIURL_CURLCHECK=$?
    if [[ "$REMIURL_CURLCHECK" != '0' ]]; then
      REMIURL='https://downloads.php.net/~eric/'
    fi
    cecho "Downloading PHP $PHPNGVER from ${REMIURL}/" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.xz ]; then
      rm -rf php-${PHPNGVER}.tar.xz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
####################################################################
        cecho "Download php-${PHPNGVER}.tar.xz ..." $boldyellow
      if [ -s php-${PHPNGVER}.tar.xz ]; then
         cecho "php-${PHPNGVER}.tar.xz found, skipping download..." $boldgreen
      else
          echo "wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3"
          wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz download failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} download failed from ${REMIURL}"
        return 1
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    if ! cmm_archive_valid "php-${PHPNGVER}.tar.xz"; then
      cmm_php_fatal "php-${PHPNGVER} tarball failed integrity validation (soft-404 HTML or short file)"
      return 1
    fi
    tar xJf php-${PHPNGVER}.tar.xz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz extraction failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} extraction failed"
        return 1
      else 
            cecho "php-${PHPNGVER}.tar.xz valid file." $boldyellow
        echo ""
      fi
  elif [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '7.4.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '7.4.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '7.4.0RC'  ]]; then
    PHPNG_YES='y'
    PHPNGVER=$PHPMU_ALPHAVER
    # curl check as download url keeps changing between releases
    REMIURL='https://downloads.php.net/~derick'
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
    REMIURL_CURLCHECK=$?
    if [[ "$REMIURL_CURLCHECK" != '0' ]]; then
      REMIURL='https://downloads.php.net/~cmb'
    fi
    cecho "Downloading PHP $PHPNGVER from ${REMIURL}/" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.xz ]; then
      rm -rf php-${PHPNGVER}.tar.xz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
####################################################################
        cecho "Download php-${PHPNGVER}.tar.xz ..." $boldyellow
      if [ -s php-${PHPNGVER}.tar.xz ]; then
         cecho "php-${PHPNGVER}.tar.xz found, skipping download..." $boldgreen
      else
          echo "wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3"
          wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz download failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} download failed from ${REMIURL}"
        return 1
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    if ! cmm_archive_valid "php-${PHPNGVER}.tar.xz"; then
      cmm_php_fatal "php-${PHPNGVER} tarball failed integrity validation (soft-404 HTML or short file)"
      return 1
    fi
    tar xJf php-${PHPNGVER}.tar.xz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz extraction failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} extraction failed"
        return 1
      else 
            cecho "php-${PHPNGVER}.tar.xz valid file." $boldyellow
        echo ""
      fi
####################################################################
  elif [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '7.2.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '7.2.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '7.2.0RC'  ]]; then
    PHPNG_YES='y'
    PHPNGVER=$PHPMU_ALPHAVER
    # curl check as download url keeps changing between releases
    REMIURL='https://downloads.php.net/~remi'
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
    REMIURL_CURLCHECK=$?
    if [[ "$REMIURL_CURLCHECK" != '0' ]]; then
      REMIURL='https://downloads.php.net/~pollita'
    fi
    cecho "Downloading PHP $PHPNGVER from ${REMIURL}/" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.xz ]; then
      rm -rf php-${PHPNGVER}.tar.xz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
####################################################################
        cecho "Download php-${PHPNGVER}.tar.xz ..." $boldyellow
      if [ -s php-${PHPNGVER}.tar.xz ]; then
         cecho "php-${PHPNGVER}.tar.xz found, skipping download..." $boldgreen
      else
          echo "wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3"
          wget -${ipv_forceopt}v ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz download failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} download failed from ${REMIURL}"
        return 1
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    if ! cmm_archive_valid "php-${PHPNGVER}.tar.xz"; then
      cmm_php_fatal "php-${PHPNGVER} tarball failed integrity validation (soft-404 HTML or short file)"
      return 1
    fi
    tar xJf php-${PHPNGVER}.tar.xz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz extraction failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} extraction failed"
        return 1
      else 
            cecho "php-${PHPNGVER}.tar.xz valid file." $boldyellow
        echo ""
      fi
####################################################################  
  elif [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '7.0.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '7.0.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '7.0.0RC'  ]]; then
      PHPNGVER=$PHPMU_ALPHAVER
    PHPNG_YES='y'
    cecho "Downloading PHP $PHPNGVER from https://downloads.php.net/~ab/" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.gz ]; then
      rm -rf php-${PHPNGVER}.tar.gz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
####################################################################
        cecho "Download php-${PHPNGVER}.tar.gz ..." $boldyellow
      if [ -s php-${PHPNGVER}.tar.gz ]; then
         cecho "php-${PHPNGVER}.tar.gz found, skipping download..." $boldgreen
      else
          echo "wget -${ipv_forceopt}v https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3"
          wget -${ipv_forceopt}v https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.gz download failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} download failed from https://downloads.php.net/~ab/"
        return 1
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    if ! cmm_archive_valid "php-${PHPNGVER}.tar.gz"; then
      cmm_php_fatal "php-${PHPNGVER} tarball failed integrity validation (soft-404 HTML or short file)"
      return 1
    fi
    tar xzf php-${PHPNGVER}.tar.gz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.gz extraction failed." $boldgreen
        cmm_php_fatal "php-${PHPNGVER} extraction failed"
        return 1
      else 
            cecho "php-${PHPNGVER}.tar.gz valid file." $boldyellow
        echo ""
      fi    
####################################################################    
  elif [[ "$PHPMUVER" = 'NGDEBUG' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '8.6' ]] && [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" != '7.0.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" != '7.0.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" != '7.0.0RC' ]]; then
    if [[ "$PHPMUVER" = 'NGDEBUG' ]]; then
      PHPNGVER='NGDEBUG'
    elif [[ "$PHPMUVER" = '5.7' ]]; then
      PHPNGVER='5.7.0'
    elif [[ "$PHPMUVER" = '8.6' ]]; then
      PHPNGVER='8.6.0'
    fi
    PHPNG_YES='y'
    cecho "Downloading PHP $PHPMUVER from https://git.php.net/repository/php-src.git" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.gz ]; then
      rm -rf php-${PHPNGVER}.tar.gz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
    time git clone --depth=5 https://git.php.net/repository/php-src.git php-${PHPNGVER}
    if [ $? -ne 0 ] || [ ! -f "${DIR_TMP}/php-${PHPNGVER}/main/php_version.h" ]; then
      cmm_php_fatal "php-${PHPNGVER} git clone failed or source tree incomplete (git.php.net)"
      return 1
    fi
    cecho "Download PHP $PHPMUVER completed" $boldyellow
    echo
    cecho "Creating tar archive" $boldyellow
    tar -czf php-${PHPNGVER}.tar.gz php-${PHPNGVER}
    if [ -f /usr/bin/pxz ]; then
      tar -I 'pxz -1' -cf php-${PHPNGVER}.tar.xz php-${PHPNGVER}
    else
      tar -I 'xz -1' -cf php-${PHPNGVER}.tar.xz php-${PHPNGVER}
    fi
  fi
}