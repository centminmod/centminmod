checkphpextmailparse() {
    cecho "Check for php extensions" $boldyellow
    if [[ ! -f "${DIR_TMP}/mailparse-${MAILPARSEPHP_VER}.tgz" ]]; then
        echo "Downloading mailparse extension"
        mailparsephpexttarball
    fi
    echo
}

mailparseinstall() {
    if [[ "$PHPMAILPARSE" = [yY] ]]; then
        checkphpextmailparse
        
    echo "*************************************************"
    cecho "* Installing mailparse PHP Extension" $boldgreen
    echo "*************************************************"

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP

php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | grep '7.0'
PHPSEVEN_CHECKVER=$?
echo $PHPSEVEN_CHECKVER

    cd mailparse-${MAILPARSEPHP_VER}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install

#######################################################
# check if mailparse.so exists in php.ini

MAILPARSESOCHECK=`grep 'extension=mailparse.so' /usr/local/lib/php.ini`
MAILPARSESOCHECKB=$(grep 'extension=mailparse.so' ${CONFIGSCANDIR}/mailparse.ini)

if [[ -z $MAILPARSESOCHECK || -z $MAILPARSESOCHECKB ]]; then

echo -e "\nCopying mailparse.ini > ${CONFIGSCANDIR}/mailparse.ini\n"
echo "extension=mailparse.so" > ${CONFIGSCANDIR}/mailparse.ini

fi #check if mailparse.so exists in php.ini

    cmservice php-fpm restart
    echo "*************************************************"
    cecho "* mailparse PHP installed" $boldgreen
    echo "*************************************************"

    fi
}