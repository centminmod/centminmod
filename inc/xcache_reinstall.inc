function funct_xcachereinstall {

echo " "
read -ep "Xcache reinstall - Would you like to continue? [y/n] " xcachekey
if [[ "$xcachekey" = [nN] ]];
then
    exit 0
fi

# DIR_TMP="/svr-setup"
if [ ! -d "$DIR_TMP" ]; then
mkdir $DIR_TMP
fi

read -ep "Install which version of Xcache? (version i.e. 3.1.0): " xcachever

#######################

xcachevercheck=`echo $xcachever | awk -F "." '{print $1"."$2}'`

if [ "$xcachevercheck" == '2.0' ]; then

echo ""
read -ep "Are you upgrading Xcache from v1.3.2 to v2.0.x series i.e. v2.0.0 ? [y/n]: " xcachev2
echo ""

if [[ "$xcachev2" == [yY] ]]; then

echo "------------------------------------------------------------------"
echo "Detected upgrade to Xcache v2.0.0"
echo "------------------------------------------------------------------"
echo "If you previously had Xcache v1.3.2 installed"
echo "the Xcache admin directory needs updating"
echo "manually you will need to do 3 steps below: "
echo "------------------------------------------------------------------"
echo "1).  Rename existing xcacheadmin directory"
echo " mv /usr/local/nginx/html/myxcacheadmin /usr/local/nginx/html/myxcacheadmin_old"
echo "2).  Edit php.ini - you can use centmin mod command shortcut = phpedit"
echo "     http://centminmod.com/faq.html#commandshortcuts to quickly get to php.ini"
echo "     remove entries related to xcache settings and save php.ini file"
echo "3).  Re-run centmin.sh & run menu option #8 to Install Xcache"
echo "------------------------------------------------------------------"
echo "aborting script in 30 seconds ... please read above instructions"
sleep 30
exit
fi

fi # xcachever=2.0.0
#######################

    echo "*************************************************"
    cecho "* Re-installing XCache" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

    if [ -s xcache-${xcachever}.tar.gz ]; then
        cecho "xcache ${xcachever} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://xcache.lighttpd.net/pub/Releases/${xcachever}/xcache-${xcachever}.tar.gz $WGETRETRY
    fi

XCACHESOCHECK=`grep 'xcache.so' /usr/local/lib/php.ini`
XCACHESOCHECKB=$(grep 'xcache.so' ${CONFIGSCANDIR}/xcache.ini)

if [[ -z $XCACHESOCHECK || -z $XCACHESOCHECKB ]]; then

    echo
read -ep "Want to reset xcache.ini settings to default (y) or keep existing xcache.ini settings  and user/pass (n)? [y/n]: " resetxcacheini

    if [[ "$resetxcacheini" = [yY] ]]; then 

echo

PHPVERCHECK=`cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}'`

if [[ "$PHPVERCHECK" == '5.4' || "$PHPVERCHECK" == '5.5' || "$PHPVERCHECK" == '5.6' ]]; then
    #cat $CUR_DIR/config/xcache/xcache_php54.ini >> /usr/local/lib/php.ini
    cat $CUR_DIR/config/xcache/xcache_php54.ini > ${CONFIGSCANDIR}/xcache.ini
else
   #cat $CUR_DIR/config/xcache/xcache.ini >> /usr/local/lib/php.ini
    cat $CUR_DIR/config/xcache/xcache.ini > ${CONFIGSCANDIR}/xcache.ini
fi

    echo ""
    echo "*************************************************"
    cecho "*  Setup Xcache Admin Password for /usr/local/nginx/html/myxcacheadmin" $boldgreen
    echo "*************************************************"
    read -ep "(Type password your want to set and press Enter): " xcachepassword

xcachepass="`echo -n "${xcachepassword}" | md5sum | awk '{ print $1 }'`"

cecho "old md5 password hash: `grep xcache.admin.pass ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen
cecho "new md5 password hash: ${xcachepass}" $boldgreen
cecho "xcache username: `grep xcache.admin.user ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen
cecho "xcache password: ${xcachepassword}" $boldgreen

    sed -i "s/d440aed189a13ff970dac7e7e8f987b2/${xcachepass}/g" ${CONFIGSCANDIR}/xcache.ini

cecho " php.ini now has xcache.admin.pass set as follows: :" $boldgreen
cecho " `cat ${CONFIGSCANDIR}/xcache.ini | grep xcache.admin.pass`" $boldgreen

    fi # resetxcachein

fi # XCACHESOCHECK

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

    tar xvfz xcache-${xcachever}.tar.gz
    cd xcache-${xcachever}
    make clean
    /usr/local/bin/phpize
    ./configure --enable-xcache --with-php-config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install

    /etc/init.d/php-fpm restart
    echo "*************************************************"
    cecho "* XCache re-installed" $boldgreen
    echo "*************************************************"

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf $DIR_TMP
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

}