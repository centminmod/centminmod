zopcacheinstall() {

    echo "*************************************************"
    cecho "* Installing Zend Optimizer Plus OPcache" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)
PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)

echo "-------------------------------------------------------------------"
echo "Current PHP branch: $PHPCURRENTVER"
echo "Current centmin.sh set PHP_VERSION: $PHPMVER"
echo "Current PHP upgrade version set: $PHPMUVER"
echo "-------------------------------------------------------------------"

if [[ "$PHPMVER" != '5.6' || "$PHPCURRENTVER" != '5.6' || "$PHPMVER" != '5.5' || "$PHPCURRENTVER" != '5.5' || "$PHPMVER" = '5.4' ||"$PHPMVER" = '5.3' ||"$PHPMVER" = '5.2' ]]; then

	if [[ ! -d "${DIR_TMP}/zendopcache-${ZOPCACHECACHE_VERSION}" || ! -f "${DIR_TMP}/${ZENDOPCACHE_LINKFILE}" ]]; then
		zopcachetarball
	fi

#wget http://pecl.php.net/get/zendopcache-7.0.3.tgz
#tar xvzf zendopcache-7.0.3.tgz
#cd zendopcache-7.0.3
cd zendopcache-${ZOPCACHECACHE_VERSION}
make clean
/usr/local/bin/phpize
./configure --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
make install

if [[ -f ${CONFIGSCANDIR}/apc.ini ]]; then
	rm -rf ${CONFIGSCANDIR}/apc.ini
fi

if [[ -f ${CONFIGSCANDIR}/igbinary.ini ]]; then
	sed -i 's/apc.serializer=igbinary/;apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
fi

if [[ -f ${CONFIGSCANDIR}/xcache.ini ]]; then
	rm -rf ${CONFIGSCANDIR}/xcache.ini
fi

if [[ -f ${CONFIGSCANDIR}/zendopcache.ini ]]; then
	rm -rf ${CONFIGSCANDIR}/zendopcache.ini
fi

######################################################
# Randomize ${N}_opcache.php filename
N=$RANDOM
if [[ ! -f /usr/local/nginx/html/opcache.php ]]; then
	cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php
	chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
fi

######################################################

PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

#replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- $CUR_DIR/config/zendopcache/zendopcache-min.ini

#replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- $CUR_DIR/config/zendopcache/zendopcache.ini

#$CUR_DIR/config/zendopcache/zendopcache-min.ini
#$CUR_DIR/config/zendopcache/zendopcache.ini

#######################################################
# check if opcache.so exists in ${CONFIGSCANDIR}/zendopcache.ini

ZOPLUSSOCHECK=$(grep 'opcache.so' ${CONFIGSCANDIR}/zendopcache.ini)

if [[ -z $ZOPLUSSOCHECK ]]; then

echo

if [[ "$lesszendopcachemem" = [yY] ]]; then

echo $lesszendopcachemem

echo -e "\nCopying zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
cat $CUR_DIR/config/zendopcache/zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini
replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- ${CONFIGSCANDIR}/zendopcache.ini

else

echo $lesszendopcachemem

echo -e "\nCopying zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
	cat $CUR_DIR/config/zendopcache/zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini
	replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- ${CONFIGSCANDIR}/zendopcache.ini

fi

else

	if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
	rm -rf ${CONFIGSCANDIR}/zendopcache.ini
	fi
fi # check if opcache.so exists in zendopcache.ini

    /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache installed" $boldgreen
    echo "*************************************************"

else

PHPNATIVEOPCACHE='y'

	echo ""
	echo "Detected PHP branch: 5.5 which has native support for Zend OPcache"
	echo "To enable Zend OPcache run menu option #5 and specify PHP 5.5.0 or higher"

fi # check for PHPMVER !=5.5

}