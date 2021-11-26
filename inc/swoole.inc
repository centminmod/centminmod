#######################################################
checkswooletar() {

cecho "Check if ${PHPEXT_SWOOLELINKFILE} exists" $boldyellow
if [[ ! -f "${DIR_TMP}/${PHPEXT_SWOOLELINKFILE}" || ! -d "${DIR_TMP}/swoole-${PHPSWOOLE_VER}" ]]; then

    cecho "Downloading swoole PHP extension.." $boldgreen

swoolephptarball
fi

}

#######################################################
swooleinstall() {
        if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ "$PHPSWOOLE" = [yY] ]]; then
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
        echo "*************************************************"
        cecho "* Installing swoole PHP extension" $boldgreen
        echo "*************************************************"
        echo "Downloading swoole PHP extension..."
        cd "$DIR_TMP"
    
    checkswooletar
    
        cecho "Compiling PHP swoole ${PHPSWOOLE_VER} PHP extension..." $boldgreen
    
        pwd
        echo "cd $DIR_TMP"
        cd "$DIR_TMP"
    #tar xvzf ${PHPEXT_SWOOLELINKFILE}
    cd "${DIR_TMP}/swoole-${PHPSWOOLE_VER}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
        /usr/local/bin/phpize clean
    fi
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config --enable-mysqlnd --enable-swoole-json --enable-swoole-curl --enable-sockets --enable-openssl

    make${MAKETHREADS}
    make install
    
    #######################################################
    # check to see if swoole.so already loaded in php.ini
    
    SWOOLESOCHECK=`grep 'swoole.so' /usr/local/lib/php.ini`
    SWOOLESOCHECKB=$(grep 'swoole.so' ${CONFIGSCANDIR}/swoole.ini)
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
    
    if [[ -z "$SWOOLESOCHECK" || -z "$SWOOLESOCHECKB" ]]; then
        #echo "" >> /usr/local/lib/php.ini
        echo "extension=swoole.so" > "${CONFIGSCANDIR}/swoole.ini"
    else
        echo ""
        echo "swoole.so already loaded in ${CONFIGSCANDIR}/swoole.ini"
        echo "$SWOOLESOCHECK"
        echo ""
    fi # swoole.so check

    # async client & APIs
    pushd "$DIR_TMP"
    rm -rf "${DIR_TMP}/ext-async"
    git clone https://github.com/swoole/ext-async.git
    cd "${DIR_TMP}/ext-async"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    ./configure
    make${MAKETHREADS}
    make install
    ASYNC_SWOOLE_ERR=$?

    if [[ "$ASYNC_SWOOLE_ERR" -eq '0' ]]; then
        SWOOLE_ASYNC_SOCHECK=`grep 'swoole_async.so' /usr/local/lib/php.ini`
        SWOOLE_ASYNC_SOCHECKB=$(grep 'swoole_async.so' ${CONFIGSCANDIR}/swoole_async.ini)
        PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
        
        if [[ -z "$SWOOLE_ASYNC_SOCHECK" || -z "$SWOOLE_ASYNC_SOCHECKB" ]]; then
            echo "extension=swoole_async.so" > "${CONFIGSCANDIR}/swoole_async.ini"
        else
            echo ""
            echo "swoole_async.so already loaded in ${CONFIGSCANDIR}/swoole_async.ini"
            echo "$SWOOLE_ASYNC_SOCHECK"
            echo ""
        fi # swoole_async.so check
    else
        rm -f "${CONFIGSCANDIR}/swoole_async.ini"
    fi
    popd
fi # PHPSWOOLE=y
    fi # PHP_INSTALL=y
}