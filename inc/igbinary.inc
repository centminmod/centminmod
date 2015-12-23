function funct_igbinaryinstall {

if [ "$IGBINARY_INSTALL" == 'y' ]; then
if [ "$IGBINARYGIT" == 'y' ]; then
    cd $DIR_TMP

    php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | grep '7.0'
    PHPSEVEN_CHECKVER=$?
    echo $PHPSEVEN_CHECKVER

    if [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]]; then
        # php7 igbinary https://github.com/igbinary/igbinary7 broken session serialization
        # disable it for now
        DISABLESESSION=y
        if [[ -d "igbinary-php7" && -d "igbinary-php7/.git" ]]; then       
            rm -rf igbinary-php7
            git clone https://github.com/igbinary/igbinary7.git igbinary-php7
        elif [[ -d "igbinary-php7" && ! -d "igbinary-php7/.git" ]]; then
            rm -rf igbinary-php7
            git clone https://github.com/igbinary/igbinary7.git igbinary-php7
        else
            rm -rf igbinary-php7
            git clone https://github.com/igbinary/igbinary7.git igbinary-php7
        fi
        echo
        echo "compiling igbinary PHP extension for PHP 7.x ..."
        
            export PHP_AUTOCONF=/usr/bin/autoconf
            export PHP_AUTOHEADER=/usr/bin/autoheader
        
        cd igbinary-php7
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        autoconf -V | head -n1
        /usr/local/bin/phpize -clean
        /usr/local/bin/phpize
        pwd
        ./configure CFLAGS="-O2 -g" --enable-igbinary --with-php-config=/usr/local/bin/php-config
        make${MAKETHREADS}
        #make test
        make install    
    else
        echo "Compiling igbinary..."
        if [ -s igbinary-igbinary-1.1.2-dev.tar.gz ]; then
            cecho "igbinary-igbinary-1.1.2-dev.tar.gz Archive found, skipping download..." $boldgreen
        else
            wget -c --progress=bar --no-check-certificate https://github.com/igbinary/igbinary/tarball/master -O igbinary-igbinary-1.1.2-dev.tar.gz --tries=3
        fi
    
        tar -xzvf igbinary-igbinary-1.1.2-dev.tar.gz
        
            export PHP_AUTOCONF=/usr/bin/autoconf
            export PHP_AUTOHEADER=/usr/bin/autoheader
        
        cd ${DIR_TMP}/igbinary-igbinary-*/
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        autoconf -V | head -n1
        /usr/local/bin/phpize -clean
        /usr/local/bin/phpize
        pwd
        ./configure CFLAGS="-O2 -g" --enable-igbinary --with-php-config=/usr/local/bin/php-config
        make${MAKETHREADS}
        #make test
        make install
    fi # php 7 or not

else

    cd $DIR_TMP

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

    echo "Compiling igbinary..."
    if [ -s igbinary-${IGBINARY_VERSION}.tgz ]; then
        cecho "igbinary-${IGBINARY_VERSION}.tgz Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://pecl.php.net/get/igbinary-${IGBINARY_VERSION}.tgz $WGETRETRY
    fi

tar -xzvf igbinary-${IGBINARY_VERSION}.tgz
cd ${DIR_TMP}/igbinary-${IGBINARY_VERSION}/
if [[ "$INITIALINSTALL" != [yY] ]]; then
    make clean
fi
autoconf -V | head -n1
/usr/local/bin/phpize -clean
/usr/local/bin/phpize
pwd
./configure CFLAGS="-O2 -g" --enable-igbinary --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
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
else
    # remove igbinary PHP support if IGBINARY_INSTALL=n
    rm -rf ${CONFIGSCANDIR}/igbinary.ini
fi # IGBINARY_INSTALL = y

APCSOCHECK=`grep 'extension=apc.so' ${CONFIGSCANDIR}/apc.ini`

if [[ "$APCSOCHECK" ]]; then
    sed -i 's/;apc.serializer=igbinary/apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
fi

if [[ "$DISABLESESSION" = [yY] ]]; then
# php7 igbinary https://github.com/igbinary/igbinary7 broken session serialization
# disable it for now
    sed -i 's|^session.serialize_handler=igbinary|;session.serialize_handler=igbinary|' ${CONFIGSCANDIR}/igbinary.ini
fi

cmservice php-fpm reload

}