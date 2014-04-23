function funct_phpconfigure {

fpmconfdir

#############################################################
TOTALMEM=$(cat /proc/meminfo | grep MemTotal | awk '{print $2}')
ISLOWMEMPHP='530000'
#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISLOWMEMPHPMB=`echo "scale=0;$ISLOWMEMPHP/1024" | bc`
CHECKLOWMEMPHP=`expr $TOTALMEM \< $ISLOWMEMPHP`
#############################################################

if [[ "$PHPMUVER" = '5.5' && "zendopcacheon" = [yY] ]]; then
	OPCACHEOPT=" --enable-opcache"
else
	OPCACHEOPT=""
fi

if [[ ! -f /usr/include/readline/readline.h || ! -f /usr/include/editline/readline.h ]]; then
	yum -q -y install readline-devel libedit-devel
fi

if [ "$PHPPCNTL" == 'y' ]; then
	PCNTLOPT=" --enable-pcntl"
else
	PCNTLOPT=""
fi

if [ "$CHECKLOWMEMPHP" == "1" ]; then
        FILEINFOOPT=" --disable-fileinfo"
else
        FILEINFOOPT=""
fi

LIBCCLIENT=`yum list libc-client-devel | grep Available | awk '{print $1}'`

if [[ "$LIBCCLIENT" = 'Available' ]]; then
	yum -y install libc-client-devel
fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

# double check mcrypt has been installed
CHECKMCRYPT='libmcrypt libmcrypt-devel'

declare -a arraysm
arraysm=(${CHECKMCRYPT})

for pkg in ${arraysm[@]}
do
echo Processing package: $pkg
MCRYPTRPMCHECKS=`yum -q list installed $pkg | awk '/Installed/ {print $1}' &>/dev/null`
#echo $MCRYPTRPMCHECKS
        if [ "$MCRYPTRPMCHECKS" != 'Installed' ]; then
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkg" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum -q -y install $pkg
        yum -q list $pkg
echo "----------------------------------------------------------------------------------"
        fi
done

fi

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] ]]; then

../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}

else

../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}

fi

    make$MAKETHREADS
    make install

# check if php was in fact installed

if [[ ! -s /usr/local/bin/phpize ]]; then
	cecho "----------------------------------------------------------------------------------" $boldyellow
	echo "PHP was not installed properly"
	cecho "----------------------------------------------------------------------------------" $boldyellow
	#checklogdetails
	#exit
fi

}