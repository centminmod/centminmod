#################
zopcacheupgrade() {

if [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' ]]; then
	echo ""
	echo "-----------------------------------------------------------------------------------------"
	cecho "Detected PHP $PHPMUVER branch." $boldyellow
	echo "You can compile Zend OPcache (Zend Optimizer Plus+) support"
	echo "as an alternative to using APC Cache or Xcache cache."
	echo "But Zend OPcache only provides PHP opcode cache and"
	echo "DOESN'T do data caching, so if you web apps such as Wordpress,"
	echo "Drupal or vBulletin require data caching to APC or Xcache,"
	echo "it won't work with Zend OPcache."
	echo ""
	# echo "If you have imagick or memcache/memcached PHP extensions"
	# echo "installed, you will have to use menu option #10 or #15 to"
	# echo "reinstall after upgrading to PHP 5.5.0 with Zend OPcache."
	echo "-----------------------------------------------------------------------------------------"
	read -ep "Do you want to use Zend OPcache [y/n] ? " zendopcacheon
	echo ""

if [[ "$zendopcacheon" = [yY] ]]; then

	# only remove existing ini when major PHP upgrade version differs
    # from existing installed PHP version
	if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]]; then


		if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
			sed -i 's/apc.serializer=igbinary/;apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
		fi
	

		if [ -f ${CONFIGSCANDIR}/memcache.ini ]; then
			rm -rf ${CONFIGSCANDIR}/memcache.ini
		fi
	
		if [ -f ${CONFIGSCANDIR}/memcached.ini ]; then
			rm -rf ${CONFIGSCANDIR}/memcached.ini
		fi
	
		if [ -f ${CONFIGSCANDIR}/imagick.ini ]; then
			rm -rf ${CONFIGSCANDIR}/imagick.ini
		fi
	
		if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
			rm -rf ${CONFIGSCANDIR}/igbinary.ini
		fi
	
		if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
			rm -rf ${CONFIGSCANDIR}/zendopcache.ini
		fi

	fi # PHPMUVER != PHPCURRENTVER

		if [ -f ${CONFIGSCANDIR}/apc.ini ]; then
			rm -rf ${CONFIGSCANDIR}/apc.ini
		fi

		if [ -f ${CONFIGSCANDIR}/xcache.ini ]; then
			rm -rf ${CONFIGSCANDIR}/xcache.ini
		fi
	

	DETECTXCACHE=n
    DETECTAPC=n
    

######################################################
# Randomize ${N}_opcache.php filename
N=$RANDOM
if [[ -f /usr/local/nginx/html/opcache.php ]]; then
	mv /usr/local/nginx/html/opcache.php /usr/local/nginx/html/${N}_opcache.php
	chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
fi
######################################################

	PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
	PHPEXTDIRDOLD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

	#replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- $CUR_DIR/config/zendopcache/zendopcache-min.ini

	#replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- $CUR_DIR/config/zendopcache/zendopcache.ini

fi # zendopcacheon

fi

if [[ "$zendopcacheon" = [yY] ]]; then
#######################################################
# check if opcache.so exists in ${CONFIGSCANDIR}/zendopcache.ini

ZOPLUSSOCHECK=$(grep 'opcache.so' ${CONFIGSCANDIR}/zendopcache.ini)

if [[ -z "$ZOPLUSSOCHECK" ]]; then

	echo

	if [[ "$lesszendopcachemem" = [yY] ]]; then

	echo $lesszendopcachemem

	echo -e "\nCopying zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
	cat $CUR_DIR/config/zendopcache/zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini

else

	echo $lesszendopcachemem

	echo -e "\nCopying zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
	cat $CUR_DIR/config/zendopcache/zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini

fi # check if opcache.so exists in zendopcache.ini
fi # zendopcacheon

    # /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache installed" $boldgreen
    echo "*************************************************"

fi # $PHPMUVER = 5.5

}


#################
zendopcacheextfix() {

if [[ "$zendopcacheon" = [yY] ]]; then

	if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini ]]; then
		#funct_igbinaryinstall
		echo "will need to reinstall memcached extensions if needed"
	fi

fi

}