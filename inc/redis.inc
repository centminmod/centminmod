#######################################################
checkredistar() {

cecho "Check if ${PHPEXT_REDISLINKFILE} exists" $boldyellow
if [ ! -f "${DIR_TMP}/${PHPEXT_REDISLINKFILE}" ]; then

    cecho "Downloading redis PHP extension.." $boldgreen

redisphptarball
fi

}

checkigbinary() {

PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

    cecho "Checking for igbinary.so ..." $boldgreen

cecho "Check if ${PHPEXTDIRD}/igbinary.so exists" $boldyellow
if [ ! -f "${PHPEXTDIRD}/igbinary.so" ]; then
funct_igbinaryinstall
fi

if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini ]]; then
	funct_igbinaryinstall
	#echo "will need to reinstall memcached extensions if needed"
fi

# check for different igbinary versions

if [ -f /usr/local/bin/php ]; then
    if [[ "$(php --ri igbinary | awk '/igbinary version/ {print $4}')" != "${IGBINARY_VERSION}" ]]; then
        funct_igbinaryinstall
    fi
fi

}

#######################################################
redisinstall() {

PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

    echo "*************************************************"
    cecho "* Installing redis PHP extension" $boldgreen
    echo "*************************************************"
    echo "Downloading redis PHP extension..."
    cd $DIR_TMP

checkigbinary

checkredistar

if [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]]; then
    cecho "Compiling PHP redis-${REDISPHP_VER} extension for PHP 7.x ..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
    rm -rf redis-php7
    git clone -b php7 https://github.com/phpredis/phpredis.git redis-php7
    cd redis-php7
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    if [[ -f "${PHPEXTDIRD}/igbinary.so" ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-redis-igbinary
    else
        ./configure --with-php-config=/usr/local/bin/php-config
    fi
    make${MAKETHREADS}
    make install
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
    if [[ -f "${PHPEXTDIRD}/igbinary.so" ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-redis-igbinary
    else
        ./configure --with-php-config=/usr/local/bin/php-config
    fi
    make${MAKETHREADS}
    make install
if # php 7 or not

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

}