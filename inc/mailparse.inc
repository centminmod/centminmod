checkphpextmailparse() {

php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | grep '7.0'
PHPSEVEN_CHECKVER=$?
echo $PHPSEVEN_CHECKVER

# mailparse 3.0.0 only works with PHP 7.0 branch
# so use 2.1.6 if not PHP 7.0 branch
if [[ "$PHPSEVEN_CHECKVER" != '0' ]]; then
    MAILPARSEPHP_VER='2.1.6'
    PHPEXT_MAILPARSELINKFILE="mailparse-${MAILPARSEPHP_VER}.tgz"
    PHPEXT_MAILPARSELINK="https://pecl.php.net/get/${PHPEXT_MAILPARSELINKFILE}"
    PHPEXT_MAILPARSELINKLOCAL="http://centminmod.com/centminmodparts/mailparse/${PHPEXT_MAILPARSELINKFILE}"
fi

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