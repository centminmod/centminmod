#######################################################
checkswooletar() {
    # the PHP branch being installed or upgraded to wins over the default PHP_VERSION branch
    local swoole_phpver="$PHPMVER"
    [[ "$PHPMUVER" =~ ^[0-9]+\.[0-9]+$ ]] && swoole_phpver="$PHPMUVER"

    if [[ "$swoole_phpver" = '8.1' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '8.2' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '8.3' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.2' || "$swoole_phpver" = '7.3' || "$swoole_phpver" = '7.4' || "$swoole_phpver" = '8.0' || "$swoole_phpver" = '8.4' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '5.5' || "$swoole_phpver" = '5.6' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.1' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.0' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
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
    # the PHP branch being installed or upgraded to wins over the default PHP_VERSION branch
    local swoole_phpver="$PHPMVER"
    [[ "$PHPMUVER" =~ ^[0-9]+\.[0-9]+$ ]] && swoole_phpver="$PHPMUVER"
        if [[ "$PHP_INSTALL" = [yY] ]]; then

    if [[ "$swoole_phpver" = '8.1' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_ONE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '8.2' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_TWO_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '8.3' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_THREE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.2' || "$swoole_phpver" = '7.3' || "$swoole_phpver" = '7.4' || "$swoole_phpver" = '8.0' || "$swoole_phpver" = '8.4' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '5.5' || "$swoole_phpver" = '5.6' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_FIVE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.1' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ONE_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    elif [[ "$swoole_phpver" = '7.0' ]]; then
        PHPSWOOLE_VER="$PHPSWOOLE_SEVEN_ZERO_VER"
        PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
        PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
        PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
    fi

if [[ "$PHPSWOOLE" = [yY] ]]; then
    # Swoole 5.0.0-6.1.11 and 6.2.0-6.2.4 trust client-supplied multipart
    # Swoole-Upload-File markers, so a request can delete files with the worker's
    # permissions. Fixed in 6.1.12 for PHP 8.1 and 6.2.5 for PHP 8.2-8.3. PHP
    # 7.2-8.0 have no fixed release, so skip the build rather than install one
    local swoole_patch="${PHPSWOOLE_VER##*.}" swoole_vuln=n
    swoole_patch="${swoole_patch%%[!0-9]*}"
    case "$PHPSWOOLE_VER" in
        5.*|6.0.*) swoole_vuln=y ;;
        6.1.*) [[ "${swoole_patch:-0}" -lt 12 ]] && swoole_vuln=y ;;
        6.2.*) [[ "${swoole_patch:-0}" -lt 5 ]] && swoole_vuln=y ;;
    esac
    if [[ "$swoole_vuln" = 'y' || "$swoole_phpver" = 7.[234] || "$swoole_phpver" = '8.0' ]]; then
        cecho "Skipping swoole ${PHPSWOOLE_VER} for PHP ${swoole_phpver}: affected by the multipart upload file deletion flaw" $boldyellow
        cecho "Fixed swoole 6.1.12 / 6.2.5 needs PHP 8.1+; remove older PHPSWOOLE_*_VER pins from /etc/centminmod/custom_config.inc" $boldyellow
        if [ -f "${CONFIGSCANDIR}/swoole.ini" ]; then
            cecho "Existing ${CONFIGSCANDIR}/swoole.ini left in place; remove it to stop loading swoole" $boldyellow
        fi
        return 0
    fi
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset "$CLANG_PHP" || return 1
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
    CFLAGS="${COMPILER_CFLAGS} -fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config --enable-mysqlnd --enable-swoole-json --enable-swoole-curl --enable-sockets --enable-openssl

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

    local swoole_loaded
    swoole_loaded=$(/usr/local/bin/php -r 'echo phpversion("swoole");' 2>/dev/null)
    if [[ "$swoole_loaded" != "$PHPSWOOLE_VER" ]]; then
        cecho "WARNING: PHP loads swoole ${swoole_loaded:-none}, expected ${PHPSWOOLE_VER}" $boldyellow
    elif [[ "$INITIALINSTALL" != [yY] ]]; then
        cecho "Restart Swoole server processes to load swoole ${PHPSWOOLE_VER}" $boldyellow
    fi

    # # async client & APIs
    # pushd "$DIR_TMP"
    # rm -rf "${DIR_TMP}/ext-async"
    # git clone https://github.com/swoole/ext-async.git
    # cd "${DIR_TMP}/ext-async"
    # if [[ "$INITIALINSTALL" != [yY] ]]; then
    #     make clean
    # fi
    # /usr/local/bin/phpize
    # CFLAGS="${COMPILER_CFLAGS} -fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure
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