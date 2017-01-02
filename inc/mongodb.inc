#######################################################
checkmongodbtar() {

cecho "Check if ${PHPEXT_MONGODBLINKFILE} exists" $boldyellow
if [ ! -f "${DIR_TMP}/${PHPEXT_MONGODBLINKFILE}" ]; then

    cecho "Downloading MongoDB PHP extension.." $boldgreen

mongodbphptarball
fi

}

#######################################################
mongodbinstall() {
        if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ "$PHPMONGODB" = [yY] ]]; then
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
    
        echo "*************************************************"
        cecho "* Installing MongoDB PHP extension" $boldgreen
        echo "*************************************************"
        echo "Downloading MongoDB PHP extension..."
        cd "$DIR_TMP"
    
    checkmongodbtar
    
        cecho "Compiling PHP mongo-php-${MONGODBPHP_VER} extension..." $boldgreen
    
        pwd
        echo "cd $DIR_TMP"
        cd "$DIR_TMP"
    #tar xvzf ${PHPEXT_MONGODBLINKFILE}
    cd "${DIR_TMP}/mongodb-${MONGODBPHP_VER}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    if [[ "$MONGODB_SASL" = [yY] ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --with-mongo-sasl=/usr/local
    else
        ./configure --with-php-config=/usr/local/bin/php-config
    fi
    make${MAKETHREADS}
    make install
    
    #######################################################
    # check to see if mongodb.so already loaded in php.ini
    
    MONGODBSOCHECK=`grep 'mongodb.so' /usr/local/lib/php.ini`
    MONGODBSOCHECKB=$(grep 'mongodb.so' ${CONFIGSCANDIR}/mongodb.ini)
    PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
    
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