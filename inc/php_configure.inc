function funct_phpconfigure {
# set locale temporarily to english
# for php compile due to some non-english
# locale compilation issues i.e. Turkish 
LANG=en_US.UTF-8

fpmconfdir

#############################################################
TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
ISLOWMEMPHP='530000'
#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISLOWMEMPHPMB=`echo "scale=0;$ISLOWMEMPHP/1024" | bc`
CHECKLOWMEMPHP=`expr $TOTALMEM \< $ISLOWMEMPHP`
#############################################################
PLIBDIR='/usr'

if [[ "$PHPDEBUGMODE" = [yY] ]]; then
	PHPDEBUGOPT=' --enable-debug'
else
	PHPDEBUGOPT=''
fi

if [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' ]] && [[ "$zendopcacheon" = [yY] || "$ZOPCACHEDFT" = [yY] ]] && [[ "$ZOPCACHE_OVERRIDE" = [nN] ]]; then
	OPCACHEOPT=" --enable-opcache"
elif [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = 'NGDEBUG' ]] && [[ "$zendopcacheon" = [yY] || "$ZOPCACHEDFT" = [yY] ]] && [[ "$ZOPCACHE_OVERRIDE" = [yY] ]]; then
	# ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.0's inbuilt included
	# Zend Opcache version with one available from pecl site	
	OPCACHEOPT=" --disable-opcache"	
else
	OPCACHEOPT=""
fi

if [[ ! -f /usr/include/readline/readline.h || ! -f /usr/include/editline/readline.h ]]; then
	if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
		yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=rpmforge,epel
	else
		yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=epel
	fi
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

if [[ ! -f /usr/include/expect.h ]]; then
	yum -q -y install expect-devel --disablerepo=rpmforge,epel
fi

if [[ -f /usr/bin/ldapadd ]]; then
	LDAPOPT=' --with-ldap --with-ldap-sasl'
else
	LDAPOPT=""
fi

if [[ -d /usr/include/vpx ]]; then
	LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
else
	yum${CACHESKIP} -y install libvpx libvpx-devel --disablerepo=rpmforge,epel
	LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
fi

if [[ -f /usr/include/t1lib.h ]]; then
	TLIBOPT=" --with-t1lib=${PLIBDIR}"
else
	yum${CACHESKIP} -y install t1lib t1lib-devel --disablerepo=rpmforge,epel
	TLIBOPT=" --with-t1lib=${PLIBDIR}"
fi
 
if [ "$PHPPCNTL" == 'y' ]; then
	PCNTLOPT=" --enable-pcntl"
else
	PCNTLOPT=""
fi

if [ "$PHPINTL" == 'y' ]; then
	PHPINTLOPT=" --enable-intl"
else
	PHPINTLOPT=""
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

if [[ "$CHECKLOWMEMPHP" = "1" ]]; then
        FILEINFOOPT=" --disable-fileinfo"
else
        FILEINFOOPT=""
fi

# let centmin.sh set PHPFINFO variable override the
# low memory CHECKLOWMEMPHP variable check
if [[ "$PHPFINFO" = 'n' ]]; then
        FILEINFOOPT=" --disable-fileinfo"
else
        FILEINFOOPT=""
fi

if [[ "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then
	if [[ ! $(rpm -qa uw-imap-devel) ]]; then 
		yum${CACHESKIP} -y install uw-imap-devel --disablerepo=rpmforge
	fi
else
	if [[ ! $(rpm -qa libc-client-devel) ]]; then 
		yum${CACHESKIP} -y install libc-client-devel --disablerepo=rpmforge,epel
	fi
fi


if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' || "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then

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

# set_intelflags

if [[ "$CLANG_PHP" = [yY] ]]; then
    if [[ ! -f /usr/bin/clang ]]; then
        yum -q -y install clang clang-devel
    fi
	# ccache compiler has some initial overhead for compiles but speeds up subsequent
	# recompiles. however on initial install ccache has no benefits, so for initial
	# centmin mod install disabling ccache will in theory speed up first time installs
	if [[ "$INITIALINSTALL" != [yY] ]]; then    
    	export CC="ccache /usr/bin/clang"
    	export CXX="ccache /usr/bin/clang++"
    	export CCACHE_CPP2=yes
    	# PLACEHOLDER FOR CLAGS FOR PHP
    	# CFLAGS=
    else
    	export CC="/usr/bin/clang"
    	export CXX="/usr/bin/clang++"
    	# export CCACHE_CPP2=yes
    	# PLACEHOLDER FOR CLAGS FOR PHP
    	# CFLAGS=
    fi    	
else
	# ccache compiler has some initial overhead for compiles but speeds up subsequent
	# recompiles. however on initial install ccache has no benefits, so for initial
	# centmin mod install disabling ccache will in theory speed up first time installs
	if [[ "$INITIALINSTALL" != [yY] ]]; then
    	export CC="ccache /usr/bin/gcc"
    	export CXX="ccache /usr/bin/g++"
    	export CCACHE_CPP2=yes
    	# PLACEHOLDER FOR CLAGS FOR PHP
    	# CFLAGS=
	else
    	export CC="/usr/bin/gcc"
    	export CXX="/usr/bin/g++"
    	# export CCACHE_CPP2=yes
    	# PLACEHOLDER FOR CLAGS FOR PHP
    	# CFLAGS=
    fi
fi

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] ]]; then

	echo "../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}"

../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}

else

	echo "../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}"

../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=/var/lib/mysql/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo --with-pdo-sqlite --with-pdo-mysql=mysqlnd --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${LDAPOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}

fi

    time make${MAKETHREADS}

	if [[ "$PHPMAKETEST" = [yY] ]]; then
		time make test
	fi

    time make install

if [[ "$STRIPPHP" = [yY] ]]; then
    echo
    echo "strip PHP binary..."
    ls -lah /usr/local/bin/php
    ls -lah /usr/local/bin/php-cgi
    strip -s /usr/local/bin/php
    strip -s /usr/local/bin/php-cgi
    ls -lah /usr/local/bin/php
    ls -lah /usr/local/bin/php-cgi
    echo
fi

if [[ "$CLANG_PHP" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CFLAGS
    #unset CCACHE_CPP2
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
    # PLACEHOLDER FOR CLAGS FOR PHP
    # CFLAGS=
fi   

# unset_intelflags

# check if php was in fact installed

if [[ ! -s /usr/local/bin/phpize ]]; then
	cecho "----------------------------------------------------------------------------------" $boldyellow
	echo "PHP was not installed properly"
	cecho "----------------------------------------------------------------------------------" $boldyellow
	#checklogdetails
	#exit
fi

}