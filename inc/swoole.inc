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
elif [[ "$PHPMVER" = '8.5' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FIVE_VER"
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
elif [[ "$PHPMUVER" = '8.5' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FIVE_VER"
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
elif [[ "$PHPMVER" = '8.5' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FIVE_VER"
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
elif [[ "$PHPMUVER" = '8.5' ]]; then
    PHPSWOOLE_VER="$PHPSWOOLE_EIGHT_FIVE_VER"
    PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
    PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
    PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"
fi

if [[ "$PHPSWOOLE" = [yY] ]]; then
    # Swoole 5.0.0-6.1.11 and 6.2.0-6.2.4 trust client-supplied multipart
    # Swoole-Upload-File markers, so a request can delete files with the worker's
    # permissions. Fixed in 6.1.12 (PHP 8.1-8.4) and 6.2.5 (PHP 8.2-8.5). PHP
    # 7.2-8.0 have no fixed release, so skip the build rather than install one
    local swoole_phpver="$PHPMVER" swoole_patch="${PHPSWOOLE_VER##*.}" swoole_vuln=n
    [[ "$PHPMUVER" =~ ^[0-9]+\.[0-9]+$ ]] && swoole_phpver="$PHPMUVER"
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
    PHPEXTDIRD=$(php_extension_dir)
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
    if [[ "$CENTOS_NINE" -eq '9' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_SEVEN" -eq '7' ]] && [ ! -f  /usr/lib64/libbrotlienc.so ]; then
        $YUMDNFBIN -y -q install brotli-devel
    fi
    /usr/local/bin/phpize
    CFLAGS="${COMPILER_CFLAGS} -fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config --enable-mysqlnd --enable-swoole-json --enable-swoole-curl --enable-sockets --enable-openssl

    make${MAKETHREADS}
    make install
    
    #######################################################
    # check to see if swoole.so already loaded in php.ini
    
    SWOOLESOCHECK=`grep 'swoole.so' /usr/local/lib/php.ini`
    SWOOLESOCHECKB=$(grep 'swoole.so' ${CONFIGSCANDIR}/swoole.ini)
    PHPEXTDIRD=$(php_extension_dir)
    
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