#######################################################
checkmongodbtar() {
  # the PHP branch being installed or upgraded to wins over the default PHP_VERSION branch
  local mongodb_phpver="$PHPMVER"
  [[ "$PHPMUVER" =~ ^[0-9]+\.[0-9]+$ ]] && mongodb_phpver="$PHPMUVER"

  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '7.0' ]]; then
    # PHP 8 requires mongodb =>1.12.0
    # PHP 7.1+ requires mongodb 1.11.1
    # PHP 7.0 requires mongodb 1.9.2
    # PHP 5.x requires mongodb <=1.7.5
    MONGODBPHP_VER="$MONGODBPHP_SEVEN_ZERO_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '7.1' || "$mongodb_phpver" = '7.2' || "$mongodb_phpver" = '7.3' || "$mongodb_phpver" = '7.4' ]]; then
    # PHP 8 requires mongodb =>1.12.0
    # PHP 7 requires mongodb 1.11.1+
    # PHP 5.x requires mongodb <=1.7.5
    MONGODBPHP_VER="$MONGODBPHP_SEVEN_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '8.0' || "$mongodb_phpver" = '8.1' ]]; then
    # PHP 8 requires mongodb =>1.12.0
    # PHP 7 requires mongodb 1.11.1+
    # PHP 5.x requires mongodb <=1.7.5
    MONGODBPHP_VER="$MONGODBPHP_EIGHT_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '8.2' ]]; then
    # PHP 8.2 requires mongodb 1.20.0+
    MONGODBPHP_VER="$MONGODBPHP_EIGHTTWO_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '8.3' ]]; then
    # PHP 8.3 requires mongodb 1.21.2+
    MONGODBPHP_VER="$MONGODBPHP_EIGHTTHREE_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '8.4' ]]; then
    # PHP 8.4 requires mongodb 1.21.2+
    MONGODBPHP_VER="$MONGODBPHP_EIGHTFOUR_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$mongodb_phpver" = '8.5' || "$PHPVER_ID" -ge '80500' ]]; then
    # PHP 8.5 requires mongodb 1.21.2+
    MONGODBPHP_VER="$MONGODBPHP_EIGHTFIVE_VER"
    PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
    PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
    PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"
  fi

cecho "Check if ${PHPEXT_MONGODBLINKFILE} exists" $boldyellow
if [[ ! -f "${DIR_TMP}/${PHPEXT_MONGODBLINKFILE}" || ! -d "${DIR_TMP}/mongodb-${MONGODBPHP_VER}" ]]; then

    cecho "Downloading MongoDB PHP extension.." $boldgreen

mongodbphptarball
fi

}

#######################################################
mongodbinstall() {
        if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ "$PHPMONGODB" = [yY] ]]; then
    PHPEXTDIRD=$(php_extension_dir)
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset "$CLANG_PHP" || return 1
        echo "*************************************************"
        cecho "* Installing MongoDB PHP extension" $boldgreen
        echo "*************************************************"
        echo "Downloading MongoDB PHP extension..."
        cd "$DIR_TMP"
    
    checkmongodbtar
    
        cecho "Compiling PHP mongo ${MONGODBPHP_VER} PHP extension..." $boldgreen
    
        pwd
        echo "cd $DIR_TMP"
        cd "$DIR_TMP"
    #tar xvzf ${PHPEXT_MONGODBLINKFILE}
    cd "${DIR_TMP}/mongodb-${MONGODBPHP_VER}" || return $?
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize || return $?
    # PHP 8.5 removed the IS_INTERNED() alias that mongodb 1.21.x still calls;
    # map it to ZSTR_IS_INTERNED() at compile time instead of editing the source
    local mongodb_cppflags=''
    if [[ "$(/usr/local/bin/php-config --vernum)" -ge 80500 ]]; then
        mongodb_cppflags=' -DIS_INTERNED=ZSTR_IS_INTERNED'
    fi
    if [[ "$MONGODB_SASL" = [yY] ]]; then
        CPPFLAGS="${CPPFLAGS}${mongodb_cppflags}" ./configure --with-php-config=/usr/local/bin/php-config --with-mongo-sasl=/usr/local || return $?
    else
        CPPFLAGS="${CPPFLAGS}${mongodb_cppflags}" ./configure --with-php-config=/usr/local/bin/php-config || return $?
    fi
    make${MAKETHREADS} || return $?
    make install || return $?
    [[ -s "${PHPEXTDIRD}/mongodb.so" ]] || return 1

    #######################################################
    # check to see if mongodb.so already loaded in php.ini

    MONGODBSOCHECK=`grep 'mongodb.so' /usr/local/lib/php.ini`
    MONGODBSOCHECKB=$(grep 'mongodb.so' ${CONFIGSCANDIR}/mongodb.ini)

    if [[ -z "$MONGODBSOCHECK" || -z "$MONGODBSOCHECKB" ]]; then
        #echo "" >> /usr/local/lib/php.ini
        echo "extension=mongodb.so" > "${CONFIGSCANDIR}/mongodb.ini"
    else
        echo ""
        echo "mongodb.so already loaded in ${CONFIGSCANDIR}/mongodb.ini"
        echo "$MONGODBSOCHECK"
        echo ""
    fi # mongodb.so check
fi # PHPMONGODB=y
    fi # PHP_INSTALL=y
}