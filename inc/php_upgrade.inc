########################################################################
autodetectextensions() {

if [ -f ${CONFIGSCANDIR}/xcache.ini ]; then
    DETECTXCACHE=y
fi

if [ -f ${CONFIGSCANDIR}/apc.ini ]; then
    DETECTAPC=y
fi

if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
    DETECTIGBINARY=y
fi

if [[ -f ${CONFIGSCANDIR}/memcache.ini || -f ${CONFIGSCANDIR}/memcached.ini ]]; then
    DETECTMEMCACHED=y
fi

if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
    DETECTOPCACHE=y
fi

if [ -f ${CONFIGSCANDIR}/imagick.ini ]; then
    DETECTIMAGICK=y
fi

}

########################################################################
autodetectinstallextensions() {

cecho "----------------------------------------------------" $boldgreen
cecho "Auto reinstalling previously detected PHP extensions" $boldyellow
cecho "----------------------------------------------------" $boldgreen


if [[ "$DETECTXCACHE" == [yY] ]]; then
    cecho "Auto reinstalling previously detected Xcache" $boldyellow
    funct_xcachereinstall
fi

if [[ "$DETECTAPC" == [yY] ]]; then
    cecho "Auto reinstalling previously detected APC Cache" $boldyellow
    funct_apcreinstall
fi

if [[ "$DETECTIGBINARY" == [yY] ]]; then
    cecho "Auto reinstalling previously detected igbinary extension" $boldyellow
    funct_igbinaryinstall
fi

if [[ "$DETECTMEMCACHED" == [yY] ]]; then
    cecho "Auto reinstalling previously detected memcached/memcache php extension" $boldyellow
    funct_memcachedreinstall
fi

if [[ "$DETECTOPCACHE" == [yY] ]]; then
    if [[ "$PHPMUVER" != '5.5' || "$PHPMUVER" != '5.6' ]]; then
        cecho "Auto reinstalling previously detected Zend Opcache" $boldyellow
        zopcachereinstall
    fi
fi

if [[ "$DETECTIMAGICK" == [yY] ]]; then
    cecho "Auto reinstalling previously detected imagemagick extension" $boldyellow
    imagickinstall
fi

# Fix extension directory on major PHP version changes 
# Zend Opcache related when downgrading PHP major versions
PHPEXTDIRDNEW=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

OLD=$(echo $PHPEXTDIRDOLD | sed 's/\//\\\//g')
NEW=$(echo $PHPEXTDIRDNEW | sed 's/\//\\\//g')

for ini in $(ls ${CONFIGSCANDIR}/ | grep .ini)
 do 
    echo $ini
    sed -i "s/$OLD/$NEW/g" ${CONFIGSCANDIR}/${ini} 2>&1>/dev/null
done

}

########################################################################
funct_phpupgrade() {

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
    else
        ARCH='i386'
    fi

# Set LIBDIR
if [ ${ARCH} == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

echo " "
if [[ "$UALL" = 'y' ]]; then 
    key='y'
else
    read -ep "PHP Upgrade - Would you like to continue? [y/n] " key
    if [[ "$key" = [nN] ]];
    then
        exit 0
    fi
fi # UALL

DIR_TMP="$DIR_TMP"
if [ ! -d "$DIR_TMP" ]; then
mkdir $DIR_TMP
fi

funct_mktempfile

if [[ "$UALL" = 'y' ]]; then 
    phpver=${PHP_VERSION}
    overwritephpfpmconf='n'
    lessphpmem='y'
else
    echo ""
    read -ep "Install which version of PHP? (version i.e. ${PHP_VERSION}): " phpver
    read -ep "Do you want to update to latest php-fpm.conf ? It will overwrite your existing php-fpm.conf [y/n]: " overwritephpfpmconf
    if [[ "$overwritephpfpmconf" = [yY] ]]; then
        read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem
    fi
fi # UALL

PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)

    autodetectextensions
    zopcacheupgrade


    echo "*************************************************"
    cecho "* Upgrading PHP" $boldgreen
    echo "*************************************************"

funct_centos6check

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

    cd $DIR_TMP

if [ "$(rpm -qa | grep php*)" ]; then

    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
echo "yum -y erase php*"
    yum -y erase php*

fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
DOWNLOADAPP='axel -a'
WGETRETRY=''
AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.gz"
AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.gz"
else
DOWNLOADAPP='wget -c --progress=bar'
WGETRETRY='--tries=3'
AXELPHPTARGZ=''
AXELPHPUPGRADETARGZ=''
fi

    if [ -s php-${phpver}.tar.gz ]; then
        cecho "php ${phpver} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${PHP_MIRRORURL}/get/php-${phpver}.tar.gz/from/this/mirror $AXELPHPUPGRADETARGZ $WGETRETRY
    fi

    tar xzvf php-${phpver}.tar.gz

    cd php-${phpver}

##############################################
# begin update php.ini

echo ""
echo "*************************************************"
cecho "* Updating php.ini... " $boldgreen
echo "*************************************************"
echo "old php.ini will be saved at /usr/local/lib/php.ini-oldversion_$DT"
echo "if you have modified your old php.ini i.e. installed and loaded"
echo "APC, Xcache, Memcache or Suhosin - please re-install them after php upgrade"
echo "..."

    cp -f /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT
    cp -f php.ini-production /usr/local/lib/php.ini
    chmod 644 /usr/local/lib/php.ini

phpsededit

PHPDIFF=`diff /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT`
echo ""
if [ ! -z "$PHPDIFF" ]; then
echo "checking updated php.ini compared to old /usr/local/lib/php.ini-oldversion_$DT"
echo "please note the following differences: "
echo "if APC, Xcache, Memcache, Suhosin or FFMPEG Extensions are listed please re-install them"
echo ""
echo "--------------------------------------------------------------------------------------------"
diff /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT
echo "--------------------------------------------------------------------------------------------"
echo ""
    echo "*************************************************"
echo "Automatically continuing upgrade in 20 seconds..."
    echo "*************************************************"
sleep 20
echo ""
fi # PHPDIFF

# end update php.ini
##############################################

    make clean

    ./buildconf --force
    
    if [ ! -d fpm-build ]; then
    mkdir fpm-build 
    fi
    
    cd fpm-build

    make clean

    #mkdir -p /usr/${LIBDIR}/mysql
    #ln -s /usr/${LIBDIR}/libmysqlclient.so /usr/${LIBDIR}/mysql/libmysqlclient.so

funct_phpconfigure

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP configure failed ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP configure ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

	fi

# error check
################

echo "make${MAKETHREADS}"
    time make${MAKETHREADS}

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP make failed ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP make ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

	fi

# error check
################

echo "make install"
    time make install

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP wasn't installed properly ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP was installed properly\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

echo
echo

# read -ep "Do you want to update to latest php-fpm.conf ? It will overwrite your existing php-fpm.conf [y/n]: " overwritephpfpmconf

if [[ "$overwritephpfpmconf" = [yY] ]]; then

    # read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem
    
    echo
    echo "Backing up existing php-fpm.conf at /usr/local/etc/php-fpm.conf-oldversion_$DT"
    echo "If you have made an custom changes to /usr/local/etc/php-fpm.conf, you will find"
    echo "they still exist in backup at /usr/local/etc/php-fpm.conf-oldversion_$DT"
    
        cp -f /usr/local/etc/php-fpm.conf /usr/local/etc/php-fpm.conf-oldversion_$DT
        ls -lhrt /usr/local/etc/ | grep php-fpm.conf
    
    sleep 20
    
    echo
    echo "Backing up existing php.conf at /usr/local/nginx/conf/php.conf-oldversion_$DT"
    echo "If you have made an custom changes to /usr/local/nginx/conf/php.conf, you will find"
    echo "they still exist in backup at /usr/local/nginx/conf/php.conf-oldversion_$DT"
    
        cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php.conf-oldversion_$DT
        ls -lhrt /usr/local/nginx/conf/ | grep php.conf
        cp -f $CUR_DIR/config/nginx/php.conf /usr/local/nginx/conf/php.conf
    
    sleep 20
    
    echo 

    if [[ "$lessphpmem" = [yY] ]]; then
    
        echo $lessphpmem
        
        echo -e "\nCopying php-fpm-min.conf /usr/local/etc/php-fpm.conf\n"
            cp -f $CUR_DIR/config/php-fpm/php-fpm-min.conf /usr/local/etc/php-fpm.conf
    
    else
    
        echo $lessphpmem
        
        echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
            cp -f $CUR_DIR/config/php-fpm/php-fpm.conf /usr/local/etc/php-fpm.conf
    
    fi # lessphpmem

    # only auto reinstall PHP extensions when major PHP upgrade version differs
    # from existing installed PHP version
    if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]]; then
        autodetectinstallextensions
    fi

else

    # only auto reinstall PHP extensions when major PHP upgrade version differs
    # from existing installed PHP version
    if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]]; then
        autodetectinstallextensions
    fi

fi # overwritephpfpmconf

zendopcacheextfix

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

if [ -f /usr/local/nginx/html/phpi.php ]; then
# Randomize phpi.php filename
NPHP=$RANDOM
mv /usr/local/nginx/html/phpi.php /usr/local/nginx/html/${NPHP}_phpi.php
chown nginx:nginx /usr/local/nginx/html/${NPHP}_phpi.php
fi

    echo "***************************************************"
    cecho "* PHP upgraded and auto reinstalled PHP extensions" $boldgreen
    echo "***************************************************"

	fi

# error check
################


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