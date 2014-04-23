function funct_igbinaryinstall {

if [ "$IGBINARYGIT" == 'y' ]; then

    cd /svr-setup

    echo "Compiling igbinary..."
    if [ -s igbinary-igbinary-1.1.2-dev.tar.gz ]; then
        cecho "igbinary-igbinary-1.1.2-dev.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate https://github.com/igbinary/igbinary/tarball/master -O igbinary-igbinary-1.1.2-dev.tar.gz --tries=3
    fi

tar -xzvf igbinary-igbinary-1.1.2-dev.tar.gz
cd igbinary-igbinary-*/
make clean
/usr/local/bin/phpize -clean
/usr/local/bin/phpize
./configure CFLAGS="-O2 -g" --enable-igbinary --with-php-config=/usr/local/bin/php-config
make
#make test
make install 

else

    cd /svr-setup

    echo "Compiling igbinary..."
    if [ -s igbinary-${IGBINARY_VERSION}.tgz ]; then
        cecho "igbinary-${IGBINARY_VERSION}.tgz Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://pecl.php.net/get/igbinary-${IGBINARY_VERSION}.tgz $WGETRETRY
    fi

tar -xzvf igbinary-${IGBINARY_VERSION}.tgz
cd igbinary-${IGBINARY_VERSION}/
make clean
/usr/local/bin/phpize -clean
/usr/local/bin/phpize
./configure CFLAGS="-O2 -g" --enable-igbinary --with-php-config=/usr/local/bin/php-config
make
#make test
make install 

fi

IGBSOCHECK=`grep 'extension=igbinary.so' /usr/local/lib/php.ini`
IGBSOCHECKB=$(grep 'extension=igbinary.so' ${CONFIGSCANDIR}/igbinary.ini)

if [[ -z "$IGBSOCHECK" || -z "$IGBSOCHECKB" ]]; then

cat > "${CONFIGSCANDIR}/igbinary.ini" <<END

[igbinary]
extension=igbinary.so
;Use igbinary as session serializer
session.serialize_handler=igbinary

;Enable or disable compacting of duplicate strings
igbinary.compact_strings=On

;Use igbinary as serializer in APC cache
;apc.serializer=igbinary
END

fi

APCSOCHECK=`grep 'extension=apc.so' ${CONFIGSCANDIR}/apc.ini`

if [[ $APCSOCHECK ]]; then
sed -i 's/;apc.serializer=igbinary/apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
fi

service php-fpm restart

}