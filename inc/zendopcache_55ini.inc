#################
zopcache_initialini() {

PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)

if [[ "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' ]]; then
	PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
fi # PHPMVER

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

    # /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache installed" $boldgreen
    echo "*************************************************"

fi # $ZOPLUSSOCHECK

}
