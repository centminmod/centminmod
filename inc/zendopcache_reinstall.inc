#################
zopcachereinstall() {

    echo "*************************************************"
    cecho "* Re-Installing Zend Optimizer Plus OPcache" $boldgreen
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

echo ""
read -ep "Install which version of Zend OpCache? (version i.e. ${ZOPCACHECACHE_VERSION}}): " zocver
ZENDOPCACHE_LINKFILE="zendopcache-${zocver}.tgz"
ZENDOPCACHE_LINK="http://pecl.php.net/get/${ZENDOPCACHE_LINKFILE}"


    cd $DIR_TMP

        cecho "Download ${ZENDOPCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${ZENDOPCACHE_LINKFILE} ]; then
        cecho " ${ZENDOPCACHE_LINKFILE} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${ZENDOPCACHE_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${ZENDOPCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${ZENDOPCACHE_LINKFILE} valid file." $boldyellow
echo ""
	fi

cd zendopcache-${zocver}
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
if [[ -f /usr/local/nginx/html/opcache.php ]]; then
	mv /usr/local/nginx/html/opcache.php /usr/local/nginx/html/${N}_opcache.php
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
	read -ep "Do you want to reset zendopcache.ini settings to default (y) or keep existing zendopcache.ini settings (n)? [y/n]: " resetzocache

    if [[ "$resetzocache" = [yY] ]]; then   

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
			ls -lah ${CONFIGSCANDIR}/zendopcache.ini
			fi
	fi # resetzocache
fi # check if opcache.so exists in zendopcache.ini

replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- ${CONFIGSCANDIR}/zendopcache.ini

    /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache re-installed" $boldgreen
    echo "*************************************************"

else

PHPNATIVEOPCACHE='y'

	echo ""
	echo "Detected PHP branch: 5.5 which has native support for Zend OPcache"
	echo "To re-install Zend OPcache run menu option #5 and specify PHP 5.5.0 or higher"

fi # check for PHPMVER !=5.5

}