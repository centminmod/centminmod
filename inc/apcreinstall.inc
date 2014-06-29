function funct_apcreinstall {

echo " "
read -ep "APC reinstall - Would you like to continue? [y/n] " apckeyd
if [[ "$apckeyd" = [nN] ]];
then
    exit 0
fi

#DIR_TMP="/svr-setup"
#if [ ! -d "$DIR_TMP" ]; then
#mkdir $DIR_TMP
#fi

read -ep "Re-install which version of APC? (type a version i.e. 3.1.13): " apcver

if [[ -z "$apcver" ]]; then
    echo "you did not enter an APC version number i.e. 3.1.13"
    echo "aborting APC re-install, please rerun APC reinstall"
    echo "and enter a APC version number when prompted."
    exit 0
fi

    echo "*************************************************"
    cecho "* Re-installing Alternative PHP Cache" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)

if [[ "$PHPCURRENTVER" = '5.5' || "$PHPCURRENTVER" = '5.6' ]]; then
    apcdevfix
else
    if [ -s APC-${apcver}.tgz ]; then
        cecho "APC-${apcver}.tgz Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://pecl.php.net/get/APC-${apcver}.tgz $WGETRETRY
    fi

tar xvzf APC-${apcver}.tgz
cd APC-${apcver}

fi # 5.5 & 5.6 fix 

make clean
/usr/local/bin/phpize -clean
/usr/local/bin/phpize

if [ "$apcver" == '3.1.10' ]; then
sed -i 's/PHP_APC_VERSION \"3.1.9\"/PHP_APC_VERSION \"3.1.10\"/' php_apc.h
fi

./configure --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
make install

APCSOCHECK=`grep 'extension=apc.so' /usr/local/lib/php.ini`
APCSOCHECKB=$(grep 'extension=apc.so' ${CONFIGSCANDIR}/apc.ini)

if [[ -z $APCSOCHECK || -z $APCSOCHECKB ]]; then

echo
read -ep "Do you want to reset apc.ini settings to default (y) or keep existing apc.ini settings (n)? [y/n]: " resetapcini

    if [[ "$resetapcini" = [yY] ]]; then    
        echo
        read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessapcmem        

        if [[ "$lessapcmem" = [yY] ]]; then     

            echo $lessapcmem            

            echo -e "\nCopying apc-min.ini > ${CONFIGSCANDIR}/apc.ini\n"
            #cat $CUR_DIR/config/apc/apc-min.ini >> /usr/local/lib/php.ini
            cat $CUR_DIR/config/apc/apc-min.ini > ${CONFIGSCANDIR}/apc.ini      

        else        

            echo $lessapcmem        
    
            echo -e "\nCopying apc.ini >> ${CONFIGSCANDIR}/apc.ini\n"
            #cat $CUR_DIR/config/apc/apc.ini >> /usr/local/lib/php.ini
            cat $CUR_DIR/config/apc/apc-min.ini > ${CONFIGSCANDIR}/apc.ini      

        fi  

    fi # resetapcini

fi # check if apc.so exists in php.ini

#    /etc/init.d/php-fpm restart

if [ -f /usr/local/nginx/html/myapc.php ]; then
# Randomize myapc.php filename
N=$RANDOM
mv /usr/local/nginx/html/myapc.php /usr/local/nginx/html/${N}_myapc.php
chown nginx:nginx /usr/local/nginx/html/${N}_myapc.php
fi

    echo "*************************************************"
    cecho "* Alternative PHP Cache reinstalled" $boldgreen
    echo "*************************************************"

funct_igbinaryinstall

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
