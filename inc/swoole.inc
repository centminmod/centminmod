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
    fi
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config

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
fi # PHPSWOOLE=y
    fi # PHP_INSTALL=y
}