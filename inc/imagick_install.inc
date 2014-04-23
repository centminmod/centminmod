checkphpext() {

cecho "Check for php extensions" $boldyellow
if [[ ! -f "${DIR_TMP}/imagick-${IMAGICKPHP_VER}.tgz" ]]; then
echo "Downloading imagick extension"
imagickphpexttarball
fi

}

imagickinstall() {

checkphpext

    echo "*************************************************"
    cecho "* Installing imagick PHP Extension" $boldgreen
    echo "*************************************************"

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP

cd imagick-${IMAGICKPHP_VER}
make clean
/usr/local/bin/phpize
./configure --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
make install

#######################################################
# check if imagick.so exists in php.ini

IMAGICKSOCHECK=`grep 'extension=imagick.so' /usr/local/lib/php.ini`
IMAGICKSOCHECKB=$(grep 'extension=imagick.so' ${CONFIGSCANDIR}/imagick.ini)

if [[ -z $IMAGICKSOCHECK || -z $IMAGICKSOCHECKB ]]; then

echo -e "\nCopying imagick.ini > ${CONFIGSCANDIR}/imagick.ini\n"
echo "extension=imagick.so" > ${CONFIGSCANDIR}/imagick.ini

fi #check if imagick.so exists in php.ini

    /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* imagick PHP installed" $boldgreen
    echo "*************************************************"

}