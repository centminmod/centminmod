#######################################################
checkredistar() {

  if [[ "$REDIS_PHPCURRENTVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$REDIS_PHPCURRENTVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.1' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.0' ]]; then
    if [[ "$REDISPHP_GIT" != [yY] ]]; then
      REDISPHP_VER="$REDISPHPSEVEN_VER"
      PHPEXT_REDISLINKFILE="phpredis-${REDISPHP_VER}.tgz"
      PHPEXT_REDISLINK="https://github.com/phpredis/phpredis/archive/${REDISPHP_VER}.tar.gz"
      PHPEXT_REDISLINKGITLAB="https://gitlab.com/centminmod-github-mirror/phpredis/repository/archive.tar.gz?ref=${REDISPHP_VER}"
      PHPEXT_REDISLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/redis/${PHPEXT_REDISLINKFILE}"
    fi
  fi

cecho "Check if ${PHPEXT_REDISLINKFILE} exists" $boldyellow
if [ ! -f "${DIR_TMP}/${PHPEXT_REDISLINKFILE}" ]; then
  cecho "Downloading redis PHP extension.." $boldgreen
  redisphptarball
elif [[ -f "${DIR_TMP}/${PHPEXT_REDISLINKFILE}" && ! -d "${DIR_TMP}/phpredis-${REDISPHP_VER}" ]]; then
  pushd "$DIR_TMP"
  tar xzf "${PHPEXT_REDISLINKFILE}"
  popd
fi

}

checkigbinary() {
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
    cecho "Checking for igbinary.so ..." $boldgreen
    cecho "Check if ${PHPEXTDIRD}/igbinary.so or ${CONFIGSCANDIR}/igbinary.ini exists" $boldyellow

    if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini || ! -f "${PHPEXTDIRD}/igbinary.so" ]]; then
     funct_igbinaryinstall
     #echo "will need to reinstall memcached extensions if needed"
    fi
    
    # check for different igbinary versions
    if [ -f /usr/local/bin/php ]; then
        if [[ "$(php --ri igbinary | awk '/igbinary version/ {print $4}')" != "${IGBINARY_VERSION}" ]]; then
            cecho "Check for different igbinary versions ..." $boldgreen
            funct_igbinaryinstall
        fi
    fi
}

#######################################################
redisinstall() {
    if [[ "$PHP_INSTALL" = [yY] ]]; then

    REDIS_SUBMENU=$1

    if [[ "$REDIS_SUBMENU" = 'submenu' ]]; then
        REDIS_PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
    fi

    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

    echo "*************************************************"
    cecho "* Installing redis PHP extension" $boldgreen
    echo "*************************************************"
    echo "Downloading redis PHP extension..."
    cd $DIR_TMP

if [[ "$INITIALINSTALL" != [yY] ]]; then
    checkigbinary
fi

checkredistar

if [[ "$REDIS_PHPCURRENTVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$REDIS_PHPCURRENTVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.1' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.0' ]]; then
    cecho "Compiling PHP redis-${REDISPHP_VER} extension for PHP 7.x ..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
    if [[ "$REDISPHP_GIT" = [yY] ]]; then
        rm -rf redis-php7
    
        PHPREDISGITLINK='https://github.com/phpredis/phpredis'
        # fallback mirror if official github is down, use gitlab mirror
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $PHPREDISGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
        PHPREDISGITCURLCHECK=$?
        if [[ "$PHPREDISGITCURLCHECK" != '0' ]]; then
            PHPREDISGITLINK='https://gitlab.com/centminmod-github-mirror/phpredis.git'
        fi
        if [ ! -d redis-php7 ]; then
            time git clone -b php7 "$PHPREDISGITLINK" redis-php7
        fi
        cd redis-php7
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [[ "$CENTOS_SIX" -eq '6' ]]; then
            # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
            if [ ! -f /usr/bin/autoconf268 ]; then
                echo "yum -q -y install autoconf268"
                yum -q -y install autoconf268
            fi
            if [ -f /usr/bin/autoconf268 ]; then
                export PHP_AUTOCONF=/usr/bin/autoconf268
                export PHP_AUTOHEADER=/usr/bin/autoheader268
            fi
        fi
        /usr/local/bin/phpize
        if [[ -f "${PHPEXTDIRD}/igbinary.so" ]] && [ "$IGBINARY_INSTALL" == 'y' ]; then
            ./configure --with-php-config=/usr/local/bin/php-config --enable-redis-igbinary
        else
            ./configure --with-php-config=/usr/local/bin/php-config
        fi
        make${MAKETHREADS}
        make install
    else
        REDISPHP_VER="$REDISPHPSEVEN_VER"
        pwd
        echo "cd $DIR_TMP"
        cd $DIR_TMP
        #tar xvzf ${PHPEXT_REDISLINKFILE}
        cd ${DIR_TMP}/phpredis-${REDISPHP_VER}
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        /usr/local/bin/phpize
        if [[ -f "${PHPEXTDIRD}/igbinary.so" ]] && [ "$IGBINARY_INSTALL" == 'y' ]; then
            if [[ "$PHPMUVER" = '5.5' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '5.5' ]]; then
                ./configure --with-php-config=/usr/local/bin/php-config
            else
                ./configure --with-php-config=/usr/local/bin/php-config --enable-redis-igbinary
            fi
        else
            ./configure --with-php-config=/usr/local/bin/php-config
        fi
        make${MAKETHREADS}
        make install   
    fi
else
    cecho "Compiling PHP redis-${REDISPHP_VER} extension..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
    #tar xvzf ${PHPEXT_REDISLINKFILE}
    cd ${DIR_TMP}/phpredis-${REDISPHP_VER}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    if [[ -f "${PHPEXTDIRD}/igbinary.so" ]] && [ "$IGBINARY_INSTALL" == 'y' ]; then
        if [[ "$PHPMUVER" = '5.5' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '5.5' ]]; then
            ./configure --with-php-config=/usr/local/bin/php-config
        else
            ./configure --with-php-config=/usr/local/bin/php-config --enable-redis-igbinary
        fi
    else
        ./configure --with-php-config=/usr/local/bin/php-config
    fi
    make${MAKETHREADS}
    make install
fi # php 7 or not

#######################################################
# check to see if redis.so already loaded in php.ini

REDISSOCHECK=`grep 'redis.so' /usr/local/lib/php.ini`
REDISSOCHECKB=$(grep 'redis.so' ${CONFIGSCANDIR}/redis.ini)
PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

if [[ -z "$REDISSOCHECK" || -z "$REDISSOCHECKB" ]]; then
    #echo "" >> /usr/local/lib/php.ini
    echo "extension=redis.so" > ${CONFIGSCANDIR}/redis.ini
else
    echo ""
    echo "redis.so already loaded in ${CONFIGSCANDIR}/redis.ini"
    echo "$REDISSOCHECK"
    echo ""
fi # redis.so check
    fi # PHP_INSTALL=y
}