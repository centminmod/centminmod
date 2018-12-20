function funct_igbinaryinstall {
    if [[ "$PHP_INSTALL" = [yY] ]]; then
    axelsetup

if [ "$IGBINARY_INSTALL" == 'y' ]; then
if [ "$IGBINARYGIT" == 'y' ]; then
    cd $DIR_TMP

    php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3'
    PHPSEVEN_CHECKVER=$?
    echo $PHPSEVEN_CHECKVER

    if [[ "$PHPMUVER" = '7.3' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$PHPMUVER" = '7.2' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.0' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.1' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.2' ]] || [[ "$INITIALINSTALL" = [yY] && "$(echo "$PHP_VERSION" | cut -d . -f1,2)" = '7.3' ]]; then

        IGBINARYSEVEN_LINK='https://github.com/igbinary/igbinary'
        # fallback mirror if official github is down, use gitlab mirror
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $IGBINARYSEVEN_LINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
        IGBINARYSEVEN_CURLCHECK=$?
        if [[ "$IGBINARYSEVEN_CURLCHECK" != '0' ]]; then
            IGBINARYSEVEN_LINK='https://gitlab.com/centminmod-github-mirror/igbinary.git'
        fi

        # php7 igbinary https://github.com/igbinary/igbinary broken session serialization
        # disable it for now
        DISABLESESSION=y
        if [[ -d "igbinary-php" && -d "igbinary-php/.git" ]]; then       
            rm -rf igbinary-php
            git clone "$IGBINARYSEVEN_LINK" igbinary-php
        elif [[ -d "igbinary-php" && ! -d "igbinary-php/.git" ]]; then
            rm -rf igbinary-php
            git clone "$IGBINARYSEVEN_LINK" igbinary-php
        else
            rm -rf igbinary-php
            git clone "$IGBINARYSEVEN_LINK" igbinary-php
        fi
        echo
        echo "compiling igbinary PHP extension for PHP 7.x ..."
        
            export PHP_AUTOCONF=/usr/bin/autoconf
            export PHP_AUTOHEADER=/usr/bin/autoheader
        
        cd igbinary-php
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [[ "$CENTOS_SIX" -eq '6' ]]; then
            # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
            if [ ! -f /usr/bin/autoconf268 ]; then
                echo "yum -q -y install autoconf268"
                yum -q -y install autoconf268
            fi
            if [ -f /usr/bin/autoconf268 ]; then
                export PHP_AUTOCONF=/usr/bin/autoconf268
                export PHP_AUTOHEADER=/usr/bin/autoheader268
            fi
            autoconf268 -V | head -n1
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
            wget -c${ipv_forceopt} --progress=bar --no-check-certificate https://github.com/igbinary/igbinary/tarball/master -O igbinary-igbinary-1.1.2-dev.tar.gz --tries=3
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
        download_cmd http://pecl.php.net/get/igbinary-${IGBINARY_VERSION}.tgz $WGETRETRY
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

IGBSOCHECK=$(grep 'extension=igbinary.so' /usr/local/lib/php.ini >/dev/null 2>&1; echo $?)
IGBSOCHECKB=$(grep 'extension=igbinary.so' "${CONFIGSCANDIR}/igbinary.ini" >/dev/null 2>&1; echo $?)

if [[ "$IGBSOCHECK" = '1' || "$IGBSOCHECKB" = '1' ]]; then
cat > "${CONFIGSCANDIR}/igbinary.ini" <<END

[igbinary]
extension=igbinary.so
;Use igbinary as session serializer
;session.serialize_handler=igbinary

;Enable or disable compacting of duplicate strings
igbinary.compact_strings=On

;Use igbinary as serializer in APC cache
;apc.serializer=igbinary
END
fi
else
    # remove igbinary PHP support if IGBINARY_INSTALL=n
    rm -rf "${CONFIGSCANDIR}/igbinary.ini"
fi # IGBINARY_INSTALL = y

APCSOCHECK=$(grep 'extension=apc.so' "${CONFIGSCANDIR}/apc.ini" >/dev/null 2>&1; echo $?)

if [[ "$APCSOCHECK" = '0' ]]; then
    sed -i 's/;apc.serializer=igbinary/apc.serializer=igbinary/g' "${CONFIGSCANDIR}/igbinary.ini"
fi

if [[ "$DISABLESESSION" = [yY] ]]; then
# php7 igbinary https://github.com/igbinary/igbinary broken session serialization
# disable it for now
    sed -i 's|^session.serialize_handler=igbinary|;session.serialize_handler=igbinary|' "${CONFIGSCANDIR}/igbinary.ini"
fi

cmservice php-fpm reload
    fi # PHP_INSTALL=y
}