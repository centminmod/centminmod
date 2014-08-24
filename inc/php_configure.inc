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

if [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' && "zendopcacheon" = [yY] || "ZOPCACHEDFT" = [yY] ]]; then
	OPCACHEOPT=" --enable-opcache"
else
	OPCACHEOPT=""
fi

if [[ ! -f /usr/include/readline/readline.h || ! -f /usr/include/editline/readline.h ]]; then
	yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=rpmforge,epel
fi

if [[ ! -f /usr/bin/icu-config ]]; then
	yum -q -y install libicu libicu-devel --disablerepo=rpmforge,epel
fi

if [[ ! -f /usr/bin/pspell-config ]]; then
	yum -q -y install aspell aspell-devel --disablerepo=rpmforge,epel
fi

if [[ ! -f /usr/include/recode.h ]]; then
	yum -q -y install recode recode-devel --disablerepo=rpmforge,epel
fi

if [[ ! -f /usr/include/tidy.h ]]; then
	yum -q -y install libtidy libtidy-devel --disablerepo=rpmforge
fi

if [[ ! -d /usr/include/enchant ]]; then
	yum -q -y install enchant enchant-devel --disablerepo=rpmforge,epel
fi

if [[ ! -d /usr/include/net-snmp ]]; then
	yum -q -y install net-snmp net-snmp-devel net-snmp-libs net-snmp-utils --disablerepo=rpmforge,epel
fi

if [[ -f /usr/bin/ldapadd ]]; then
	LDAPOPT=' --with-ldap --with-ldap-sasl'
else
	LDAPOPT=""
fi

if [ "$PHPPCNTL" == 'y' ]; then
	PCNTLOPT=" --enable-pcntl"
else
	PCNTLOPT=""
fi

if [ "$PHPRECODE" == 'y' ]; then
	RECODEOPT=" --with-recode"
else
	RECODEOPT=""
fi

if [ "$PHPSNMP" == 'y' ]; then
	SNMPOPT=" --with-snmp"
else
	SNMPOPT=""
fi

if [ "$PHPFINFO" == 'y' ]; then
	SNMPOPT=""
else
	SNMPOPT=" --disable-fileinfo"
fi


if [[ "$CHECKLOWMEMPHP" = "1" || "$PHPFINFO" = 'n' ]]; then
        FILEINFOOPT=" --disable-fileinfo"
else
        FILEINFOOPT=""
fi

LIBCCLIENT=`yum list libc-client-devel --disablerepo=rpmforge,epel | grep Available | awk '{print $1}'`

if [[ "$LIBCCLIENT" = 'Available' ]]; then
	yum${CACHESKIP} -y install libc-client-devel --disablerepo=rpmforge,epel
fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

	if [[ ! -f /usr/bin/libmcrypt-config ]]; then

		# double check mcrypt has been installed
		CHECKMCRYPT='libmcrypt libmcrypt-devel'
		
		declare -a arraysm
		arraysm=(${CHECKMCRYPT})
		
		for pkg in ${arraysm[@]}
		do
		echo Processing package: $pkg
		MCRYPTRPMCHECKS=`yum${CACHESKIP} -q list installed $pkg | awk '/Installed/ {print $1}' &>/dev/null`
		#echo $MCRYPTRPMCHECKS
        		if [ "$MCRYPTRPMCHECKS" != 'Installed' ]; then
		echo "----------------------------------------------------------------------------------"
		cecho "Installing $pkg" $boldyellow
		echo "----------------------------------------------------------------------------------"
        		yum${CACHESKIP} -q -y install $pkg
        		# yum${CACHESKIP} -q list $pkg
		echo "----------------------------------------------------------------------------------"
        		fi
		done
	fi

fi

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] ]]; then

	echo "../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-intl --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}"

../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-intl --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}

else

	echo "../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-intl --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}"

../configure --enable-cgi --enable-fpm${OPCACHEOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-intl --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}

fi

    time make$MAKETHREADS

	if [[ "$PHPMAKETEST" = [yY] ]]; then
		time make test
	fi

    time make install

# check if php was in fact installed

if [[ ! -s /usr/local/bin/phpize ]]; then
	cecho "----------------------------------------------------------------------------------" $boldyellow
	echo "PHP was not installed properly"
	cecho "----------------------------------------------------------------------------------" $boldyellow
	#checklogdetails
	#exit
fi

}