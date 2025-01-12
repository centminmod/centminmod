#######################################################
checkswooletar() {

# For PHPMVER (install)
if [[ "$PHPMVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMVER" = '7.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '7.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '7.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.2' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.3' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FOUR_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
fi

# For PHPMUVER (upgrade)
if [[ "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = '7.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '7.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '7.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.2' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.3' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FOUR_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
fi

    cecho "Check if ${PHPEXT_SWOOLELINKFILE} exists" $boldyellow
    if [[ ! -f "${DIR_TMP}/${PHPEXT_SWOOLELINKFILE}" || ! -d "${DIR_TMP}/swoole-${PHPSWOOLE_VER}" ]]; then
        cecho "Downloading swoole PHP extension.." $boldgreen
        swoolephptarball
    fi
}

#######################################################
swooleinstall() {
        if [[ "$PHP_INSTALL" = [yY] ]]; then

# For PHPMVER (install)
if [[ "$PHPMVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMVER" = '7.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '7.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '7.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.2' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.3' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMVER" = '8.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FOUR_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
fi

# For PHPMUVER (upgrade)
if [[ "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = '7.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '7.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '7.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.0' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ZERO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.1' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.2' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.3' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
elif [[ "$PHPMUVER" = '8.4' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FOUR_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
fi

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
    if [[ "$CENTOS_NINE" -eq '9' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_SEVEN" -eq '7' ]] && [ ! -f  /usr/lib64/libbrotlienc.so ]; then
        $YUMDNFBIN -y -q install brotli-devel
    fi
    /usr/local/bin/phpize
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config --enable-mysqlnd --enable-swoole-json --enable-swoole-curl --enable-sockets --enable-openssl

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

    # # async client & APIs
    # pushd "$DIR_TMP"
    # rm -rf "${DIR_TMP}/ext-async"
    # git clone https://github.com/swoole/ext-async.git
    # cd "${DIR_TMP}/ext-async"
    # if [[ "$INITIALINSTALL" != [yY] ]]; then
    #     make clean
    # fi
    # /usr/local/bin/phpize
    # CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure
    # make${MAKETHREADS}
    # make install
    # ASYNC_SWOOLE_ERR=$?

    # if [[ "$ASYNC_SWOOLE_ERR" -eq '0' ]]; then
    #     SWOOLE_ASYNC_SOCHECK=`grep 'swoole_async.so' /usr/local/lib/php.ini`
    #     SWOOLE_ASYNC_SOCHECKB=$(grep 'swoole_async.so' ${CONFIGSCANDIR}/swoole_async.ini)
    #     PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
        
    #     if [[ -z "$SWOOLE_ASYNC_SOCHECK" || -z "$SWOOLE_ASYNC_SOCHECKB" ]]; then
    #         echo "extension=swoole_async.so" > "${CONFIGSCANDIR}/swoole_async.ini"
    #     else
    #         echo ""
    #         echo "swoole_async.so already loaded in ${CONFIGSCANDIR}/swoole_async.ini"
    #         echo "$SWOOLE_ASYNC_SOCHECK"
    #         echo ""
    #     fi # swoole_async.so check
    # else
    #     rm -f "${CONFIGSCANDIR}/swoole_async.ini"
    # fi
    if [ -f "${CONFIGSCANDIR}/swoole_async.ini" ]; then
        rm -f "${CONFIGSCANDIR}/swoole_async.ini"
    fi
    popd
fi # PHPSWOOLE=y
    fi # PHP_INSTALL=y
}