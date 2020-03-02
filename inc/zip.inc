#######################################################
zip_php_install() {
  if [[ "$PHP_INSTALL" = [yY] ]]; then
    if [[ "$PHPZIP" = [yY] ]]; then
      PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
      echo "*************************************************"
      cecho "* Installing ZIP PHP extension" $boldgreen
      echo "*************************************************"
      echo "Downloading ZIP PHP extension..."
      cd "$DIR_TMP"
      if [ ! -f  "${DIR_TMP}/zip-${PHPZIP_VER}.tgz" ]; then
        wget -4 https://pecl.php.net/get/zip-${PHPZIP_VER}.tgz -O zip-${PHPZIP_VER}.tgz
        tar xzvf "zip-${PHPZIP_VER}.tgz"
      elif [[ -f  "${DIR_TMP}/zip-${PHPZIP_VER}.tgz" && ! -d  "${DIR_TMP}/zip-${PHPZIP_VER}" ]]; then
        tar xzvf "zip-${PHPZIP_VER}.tgz"
      fi

      cecho "Compiling ZIP PHP extension..." $boldgreen   
      pwd
      echo "cd ${DIR_TMP}/zip-${PHPZIP_VER}"
      cd "${DIR_TMP}/zip-${PHPZIP_VER}"
      if [[ "$INITIALINSTALL" != [yY] ]]; then
          make clean
      fi
      /usr/local/bin/phpize
      ./configure --with-php-config=/usr/local/bin/php-config
      make${MAKETHREADS}
      make install
      
      #######################################################
      # check to see if zip.so already loaded in php.ini
      ZIPSOCHECK=`grep 'zip.so' /usr/local/lib/php.ini`
      ZIPSOCHECKB=$(grep 'zip.so' ${CONFIGSCANDIR}/zip.ini)
      PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
      
      if [[ -z "$ZIPSOCHECK" || -z "$ZIPSOCHECKB" ]]; then
          #echo "" >> /usr/local/lib/php.ini
          echo "extension=zip.so" > "${CONFIGSCANDIR}/zip.ini"
      else
          echo ""
          echo "zip.so already loaded in ${CONFIGSCANDIR}/zip.ini"
          echo "$ZIPSOCHECK"
          echo ""
      fi # zip.so check
  fi # PHPZIP=y
fi # PHP_INSTALL=y
}