###############################################
function funct_xcachesourceinstall {

    echo "*************************************************"
    cecho "* Installing XCache" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

#download xcache tarball
xcachetarball

echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessxcachemem

#echo
#echo

XCACHESOCHECK=`grep 'xcache.so' /usr/local/lib/php.ini`
XCACHESOCHECKB=$(grep 'xcache.so' ${CONFIGSCANDIR}/xcache.ini)
PHPVERCHECK=`cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}'`

if [[ -z $XCACHESOCHECK || -z $XCACHESOCHECKB ]]; then
	if [[ "$lessxcachemem" = [yY] ]]; then

		echo $lessxcachemem

		if [[ "$PHPVERCHECK" == '5.4' || "$PHPVERCHECK" == '5.5' ]]; then
			#echo -e "\nCopying xcache-min_php54.ini >> /usr/local/lib/php.ini\n"
			#  cat $CUR_DIR/config/xcache/xcache-min_php54.ini >> /usr/local/lib/php.ini
			echo -e "\nCopying xcache-min_php54.ini > ${CONFIGSCANDIR}/xcache.ini\n"
			    cat $CUR_DIR/config/xcache/xcache-min_php54.ini > ${CONFIGSCANDIR}/xcache.ini
		else
			#echo -e "\nCopying xcache-min.ini >> /usr/local/lib/php.ini\n"
			#  cat $CUR_DIR/config/xcache/xcache-min.ini >> /usr/local/lib/php.ini
			echo -e "\nCopying xcache-min.ini > ${CONFIGSCANDIR}/xcache.ini\n"
			    cat $CUR_DIR/config/xcache/xcache-min.ini > ${CONFIGSCANDIR}/xcache.ini
		fi

	else

		echo $lessxcachemem

		echo ""

		if [[ "$PHPVERCHECK" == '5.4' || "$PHPVERCHECK" == '5.5' ]]; then
			#echo -e "\nCopying xcache-min_php54.ini >> /usr/local/lib/php.ini\n"
			#    cat $CUR_DIR/config/xcache/xcache_php54.ini >> /usr/local/lib/php.ini
			echo -e "\nCopying xcache-min_php54.ini > ${CONFIGSCANDIR}/xcache.ini\n"
			    cat $CUR_DIR/config/xcache/xcache_php54.ini > ${CONFIGSCANDIR}/xcache.ini
		else
			#echo -e "\nCopying xcache.ini >> /usr/local/lib/php.ini\n"
			#    cat $CUR_DIR/config/xcache/xcache.ini >> /usr/local/lib/php.ini
			echo -e "\nCopying xcache.ini > ${CONFIGSCANDIR}/xcache.ini\n"
			    cat $CUR_DIR/config/xcache/xcache.ini > ${CONFIGSCANDIR}/xcache.ini
		fi

	fi # lessxcachemem
else
	echo "xcache.so already detected in ${CONFIGSCANDIR}/xcache.ini"
fi # XCACHESOCHECK

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

    #tar xvfz xcache-${XCACHE_VERSION}.tar.gz
    cd xcache-${XCACHE_VERSION}
    /usr/local/bin/phpize
    ./configure --enable-xcache --with-php-config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install

if [ -f ${CONFIGSCANDIR}/apc.ini ]; then
	rm -rf ${CONFIGSCANDIR}/apc.ini
fi

if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
	sed -i 's/apc.serializer=igbinary/;apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
fi

if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
	rm -rf ${CONFIGSCANDIR}/zendopcache.ini
fi

cecho "---------------------------------------------" $boldyellow

}

###############################################
checkxcacheadmin() {

if [[ "$XCACHESRCINSTALL" = [yY] ]];
then

# check to see myxcacheadmin directory already exists

    cd $DIR_TMP
    cd xcache-${XCACHE_VERSION}

if [ ! -d /usr/local/nginx/html/myxcacheadmin ]; then

    echo ""
    echo "*************************************************"
    cecho "* Creating xcache admin directory at /usr/local/nginx/html/myxcacheadmin " $boldgreen
    echo "*************************************************"

echo "mkdir -p /usr/local/nginx/html/myxcacheadmin"
mkdir -p /usr/local/nginx/html/myxcacheadmin

if [ -d "${DIR_TMP}/xcache-${XCACHE_VERSION}/admin" ]; then

echo "cp -a admin/* /usr/local/nginx/html/myxcacheadmin"
cp -a admin/* /usr/local/nginx/html/myxcacheadmin
echo "chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin"
chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin

else

echo "cp -a htdocs/* /usr/local/nginx/html/myxcacheadmin"
cp -a htdocs/* /usr/local/nginx/html/myxcacheadmin
echo "chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin"
chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin

fi

    echo ""
    echo "*************************************************"
    cecho "*  Setup Xcache Admin Password for /usr/local/nginx/html/myxcacheadmin" $boldgreen
    echo "*************************************************"
    echo -n "(Type password your want to set and press Enter): "
read xcachepassword

xcachepass="`echo -n "${xcachepassword}" | md5sum | awk '{ print $1 }'`"

cecho "old md5 password hash: `grep xcache.admin.pass ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen
cecho "new md5 password hash: ${xcachepass}" $boldgreen
cecho "xcache username: `grep xcache.admin.user ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen
cecho "xcache password: ${xcachepassword}" $boldgreen

    sed -i "s/d440aed189a13ff970dac7e7e8f987b2/${xcachepass}/g" ${CONFIGSCANDIR}/xcache.ini

cecho " php.ini now has xcache.admin.pass set as follows: :" $boldgreen
cecho " `cat ${CONFIGSCANDIR}/xcache.ini | grep xcache.admin.pass`" $boldgreen

else

    echo "*************************************************"
    cecho " Xcache admin page already exists" $boldgreen
    cecho " at /usr/local/nginx/html/myxcacheadmin" $boldgreen
    echo "*************************************************"

fi # check to see myxcacheadmin directory already exists

fi

}

###############################################
xcacheinstall_ask() {

#ASK "Install XCache? (By default uses 32MB RAM) If XCache installed DO NOT install APC [y/n] "
if [[ "$XCACHESRCINSTALL" = [yY] ]];
then

xcacheinstallcheck='y'

funct_xcachesourceinstall

#checkxcacheadmin

    /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* XCache installed" $boldgreen
    echo "*************************************************"
fi


}