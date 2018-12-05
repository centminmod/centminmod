########################################################################
phpng_download() {
  if [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '7.3.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '7.3.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '7.3.0RC'  ]]; then
    PHPNG_YES='y'
    PHPNGVER=$PHPMU_ALPHAVER
    # curl check as download url keeps changing between releases
    REMIURL='https://downloads.php.net/~stas'
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
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
          echo "wget -${ipv_forceopt}cnv ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3"
          wget -${ipv_forceopt}cnv ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz download failed." $boldgreen
        checklogdetails
        exit #$ERROR
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    tar xJf php-${PHPNGVER}.tar.xz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz extraction failed." $boldgreen
        checklogdetails
        exit #$ERROR
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
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${REMIURL}/php-${PHPNGVER}.tar.xz" | grep 'HTTP\/' | grep '200'
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
          echo "wget -${ipv_forceopt}cnv ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3"
          wget -${ipv_forceopt}cnv ${REMIURL}/php-${PHPNGVER}.tar.xz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz download failed." $boldgreen
        checklogdetails
        exit #$ERROR
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    tar xJf php-${PHPNGVER}.tar.xz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.xz extraction failed." $boldgreen
        checklogdetails
        exit #$ERROR
      else 
            cecho "php-${PHPNGVER}.tar.xz valid file." $boldyellow
        echo ""
      fi
####################################################################  
  elif [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" = '7.0.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" = '7.0.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" = '7.0.0RC'  ]]; then
      PHPNGVER=$PHPMU_ALPHAVER
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
          echo "wget -${ipv_forceopt}cnv https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3"
          wget -${ipv_forceopt}cnv https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.gz download failed." $boldgreen
        checklogdetails
        exit #$ERROR
      else 
            cecho "Download done." $boldyellow
        #echo ""
      fi
      fi

    tar xzf php-${PHPNGVER}.tar.gz 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
        cecho "Error: php-${PHPNGVER}.tar.gz extraction failed." $boldgreen
        checklogdetails
        exit #$ERROR
      else 
            cecho "php-${PHPNGVER}.tar.gz valid file." $boldyellow
        echo ""
      fi    
####################################################################    
  elif [[ "$PHPMUVER" = 'NGDEBUG' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.4' ]] && [[ "$(echo $PHPMU_ALPHAVER | cut -c1-10)" != '7.0.0alpha' || "$(echo $PHPMU_ALPHAVER | cut -c1-9)" != '7.0.0beta' || "$(echo $PHPMU_ALPHAVER | cut -c1-7)" != '7.0.0RC' ]]; then
    if [[ "$PHPMUVER" = 'NGDEBUG' ]]; then
      PHPNGVER='NGDEBUG'
    elif [[ "$PHPMUVER" = '5.7' ]]; then
      PHPNGVER='5.7.0'
    elif [[ "$PHPMUVER" = '7.4' ]]; then
      PHPNGVER='7.4.0'
    fi
    cecho "Downloading PHP $PHPMUVER from https://git.php.net/repository/php-src.git" $boldyellow
    cd $DIR_TMP
    if [ -f php-${PHPNGVER}.tar.gz ]; then
      rm -rf php-${PHPNGVER}.tar.gz
    fi
    if [ -d php-${PHPNGVER} ]; then
      rm -rf php-${PHPNGVER}
    fi
    time git clone https://git.php.net/repository/php-src.git php-${PHPNGVER}
    cecho "Download PHP $PHPMUVER completed" $boldyellow
    echo
    cecho "Creating tar archive" $boldyellow
    tar -czf php-${PHPNGVER}.tar.gz php-${PHPNGVER}
  fi
}