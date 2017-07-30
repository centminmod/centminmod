function funct_phpconfigure {
# set locale temporarily to english
# for php compile due to some non-english
# locale compilation issues i.e. Turkish 
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

fpmconfdir

#############################################################
TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALSWAPMEM=$(awk '/SwapTotal: / {print $2}' /proc/meminfo | head -n1)
ISLOWMEMPHP='780000'
#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISLOWMEMPHPMB=`echo "scale=0;$ISLOWMEMPHP/1024" | bc`
CHECKLOWMEMPHP=`expr $TOTALMEM \< $ISLOWMEMPHP`
#############################################################
PLIBDIR='/usr'
PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)

# php modules to disable for low memory installs if
# LOWMEM_INSTALL='y' is set 
# https://community.centminmod.com/posts/46321/
if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
  SQLLITEOPT=' --without-sqlite3'
  PDOSQLLITE_OPT=' --without-pdo-sqlite'
else
  PDOSQLLITE_OPT=' --with-pdo-sqlite'
fi

if [[ "$PHPDEBUGMODE" = [yY] ]]; then
	PHPDEBUGOPT=' --enable-debug'
else
	PHPDEBUGOPT=''
fi

# ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.x's inbuilt included Zend Opcache version with one available from pecl site
if [[ -n "$PHPMUVER" && "$PHPMUVER" = 5.[234] || "$PHPMVER" = 5.[234] ]]; then
    OPCACHEOPT=''
elif [[ "$ZOPCACHE_OVERRIDE" = [yY] || "$ZOPCACHEDFT" = [nN] || "$zendopcacheon" = [nN] ]]; then
    OPCACHEOPT=' --disable-opcache'
else
    OPCACHEOPT=' --enable-opcache'
fi

if [[ ! -f /usr/include/readline/readline.h || ! -f /usr/include/editline/readline.h ]]; then
	if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
		time yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=rpmforge,epel
    sar_call
	else
		time yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=epel
    sar_call
	fi
fi

# Microsoft SQL Server PHP driver support
if [[ ! -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]] && [[ ! -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install unixODBC-devel freetds-devel
  sar_call
elif [[ ! -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]] && [[ -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install unixODBC-devel
  sar_call
elif [[ ! -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]] && [[ -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install freetds-devel
  sar_call
fi

if [[ ! -f /usr/bin/xslt-config ]]; then
	yum -q -y install libxslt libxslt-devel
  sar_call
fi

if [[ ! -f /usr/bin/icu-config ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
	 yum -q -y install libicu libicu-devel --disablerepo=rpmforge,epel
   sar_call
  else
    yum -q -y install libicu libicu-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/bin/pspell-config ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
	 yum -q -y install aspell aspell-devel --disablerepo=rpmforge,epel
   sar_call
  else
    yum -q -y install aspell aspell-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/recode.h ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
	 yum -q -y install recode recode-devel --disablerepo=rpmforge,epel
   sar_call
  else
    yum -q -y install recode recode-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/tidy.h ]]; then
	yum -q -y install libtidy libtidy-devel --disablerepo=rpmforge
  sar_call
fi

if [[ ! -d /usr/include/enchant ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install enchant enchant-devel --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install enchant enchant-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -d /usr/include/net-snmp ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install net-snmp net-snmp-devel net-snmp-libs net-snmp-utils --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install net-snmp net-snmp-devel net-snmp-libs net-snmp-utils --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/expect.h ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install expect-devel --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install expect-devel --disablerepo=epel
    sar_call
  fi
fi

if [ "$PHPMSSQL" == 'y' ]; then
  MSSQLOPT=" --with-pdo-odbc=unixODBC,/usr"
else
  MSSQLOPT=""
fi

if [[ -f /usr/include/ldap.h ]]; then
	LDAPOPT=' --with-ldap --with-ldap-sasl'
else
	LDAPOPT=""
fi

if [[ "$PHPMVER" > 7 || "$PHPMUVER" > 7 ]]; then
  if [[ "$(rpm -ql libwebp | grep libwebp.so >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    LIBVPXOPT=' --with-webp-dir=/usr'
  else
    time yum${CACHESKIP} -y install libwebp libwebp-devel
    LIBVPXOPT=' --with-webp-dir=/usr'
  fi
elif [[ "$PHPMVER" = 5.[56] || "$PHPMUVER" = 5.[56] ]] && [[ "$PHPMVER" < 7 || "$PHPMUVER" < 7 ]]; then
  if [[ -d /usr/include/vpx ]]; then
	 LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
  else
	 if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      time yum${CACHESKIP} -y install libvpx libvpx-devel --disablerepo=rpmforge,epel
      sar_call
    else
      time yum${CACHESKIP} -y install libvpx libvpx-devel --disablerepo=epel
      sar_call
    fi
	 LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
  fi
else
  LIBVPXOPT=''
fi

if [[ -f /usr/include/t1lib.h ]]; then
	TLIBOPT=" --with-t1lib=${PLIBDIR}"
else
	if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    time yum${CACHESKIP} -y install t1lib t1lib-devel --disablerepo=rpmforge,epel
    sar_call
  else
    time yum${CACHESKIP} -y install t1lib t1lib-devel --disablerepo=epel
    sar_call
  fi
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

if [ "$PHP_FTPEXT" == 'y' ]; then
	FTPEXTOPT="  --enable-ftp"
else
	FTPEXTOPT=""
fi

# let centmin.sh set PHPFINFO variable override the
# low memory CHECKLOWMEMPHP variable check
if [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" = "1" && "$TOTALSWAPMEM" -ge '1023000' ]]; then
  # PHPFINFO yes + low mem install + swap space enough = enable file info php extension
  FILEINFOOPT=""
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" != "1" ]]; then
  # PHPFINFO yes + enough mem install = enable file info php extension
  FILEINFOOPT=""
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" = "1" && "$TOTALSWAPMEM" -lt '1023000' ]]; then
  # PHPFINFO yes + low mem install + low swap space = disable file info php extension
  FILEINFOOPT=" --disable-fileinfo"
elif [[ "$PHPFINFO" = [nN] ]]; then
  # PHPFINFO no
  FILEINFOOPT=" --disable-fileinfo"
fi

if [[ "$PHPIMAP" = [yY] ]]; then
  IMAPOPT=' --with-imap --with-imap-ssl'
else
  IMAPOPT=""
fi

phpimap_install


if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_SIX" = '6' ]]; then

	if [[ "$POSTGRESQL" = [yY] ]]; then
		# to allow to persist on centmin mod code updates
		# create persistent config file at /etc/centminmod/custom_config.inc
		# and add to it the line:
		# POSTGRESQL=y
 		postgresqlinstall
		PDOPGSQL_OPT=" --with-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER} --with-pdo-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER}"
	else
		PDOPGSQL_OPT=""
	fi

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
        		time $YUMDNFBIN${CACHESKIP} -q -y install $pkg${DISABLEREPO_DNF}
        		# time $YUMDNFBIN${CACHESKIP} -q list $pkg
		echo "----------------------------------------------------------------------------------"
        		fi
		done
	fi

	if [[ ! -f /usr/bin/xml2-config ]]; then

		# double check libxml2 has been installed
		CHECKXML='libxml2-devel'
		
		declare -a arraysxml
		arraysxml=(${CHECKXML})
		
		for pkgx in ${arraysxml[@]}
		do
		echo Processing package: $pkgx
		XMLRPMCHECKS=`yum${CACHESKIP} -q list installed $pkgx | awk '/Installed/ {print $1}' &>/dev/null`
		#echo $XMLRPMCHECKS
        		if [ "$XMLRPMCHECKS" != 'Installed' ]; then
		echo "----------------------------------------------------------------------------------"
		cecho "Installing $pkgx" $boldyellow
		echo "----------------------------------------------------------------------------------"
        		time $YUMDNFBIN${CACHESKIP} -q -y install $pkgx${DISABLEREPO_DNF}
        		# time $YUMDNFBIN${CACHESKIP} -q list $pkgx
		echo "----------------------------------------------------------------------------------"
        		fi
		done
	fi	

fi

# set_intelflags

if [[ "$(grep -c "processor" /proc/cpuinfo)" -lt '2' ]]; then
  # Enable faster clang compiler for PHP if less than 2 cpu threads
  CLANG_PHP='y'
fi

if [[ "$CLANG_PHP" = [yY] ]]; then
    if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/clang ]] || [[ "$CENTOS_SEVEN" = '7' && ! -f /bin/clang ]]; then
        time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
    fi
    if [[ "$GCCINTEL_PHP" = [yY] ]]; then
      CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
      if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
        CCM=64
        PHP_MTUNEOPT="-m${CCM} -mtune=native"
      else
        CCM=""
        PHP_MTUNEOPT=""
      fi
    fi
	# ccache compiler has some initial overhead for compiles but speeds up subsequent
	# recompiles. however on initial install ccache has no benefits, so for initial
	# centmin mod install disabling ccache will in theory speed up first time installs
	if [[ "$INITIALINSTALL" != [yY] ]]; then    
    	export CC="ccache /usr/bin/clang ${PHP_MTUNEOPT}"
    	export CXX="ccache /usr/bin/clang++ ${PHP_MTUNEOPT}"
    	export CCACHE_CPP2=yes
    	# PLACEHOLDER FOR CLAGS FOR PHP
    	# CFLAGS=
    else
    	export CC="/usr/bin/clang ${PHP_MTUNEOPT}"
    	export CXX="/usr/bin/clang++ ${PHP_MTUNEOPT}"
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

if [[ "$(grep -c "processor" /proc/cpuinfo)" -lt '2' ]]; then
  # Disable php intel optimisation flags if less than 2 cpu threads
  # detected due to being very very slow for php compile times
  GCCINTEL_PHP='n'
fi

if [[ "$GCCINTEL_PHP" = [yY] ]]; then

  # PGO PHP7 on CentOS 6 might have GCC too old 4.4.7 so use devtoolset=3 GCC 4.9
  # initial php install
  if [[ "$PHP_PGO_CENTOSSIX" = [yY] && "$PHP_PGO" = [yY] && "$PHPMVER" > 7 ]]; then
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      fi
    fi
  fi
  # php upgrades
  if [[ "$PHP_PGO_CENTOSSIX" = [yY] && "$PHP_PGO" = [yY] && "$PHPMUVER" > 7 ]]; then
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      fi
    fi
  fi
  # CentOS 7 use devtoolset-4 GCC 5.2 if it's detected
  # initial php install
  if [[ "$CENTOS_SEVEN" = '7' && "$PHP_PGO" = [yY] && "$PHPMVER" > 7 ]]; then
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
        echo "centos 7 set devtoolset-6"
        echo $CC
        echo $CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
        echo "centos 7 set devtoolset-4"
        echo $CC
        echo $CXX
    fi
  fi
  # php upgrades
  if [[ "$CENTOS_SEVEN" = '7' && "$PHP_PGO" = [yY] && "$PHPMUVER" > 7 ]]; then
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
        echo "centos 7 set devtoolset-6"
        echo $CC
        echo $CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
        echo "centos 7 set devtoolset-4"
        echo $CC
        echo $CXX
    fi
  fi

	# intel specific
  CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
  SSECHECK=$(gcc -c -Q -march=native --help=target | awk '/  -msse/ {print $2}' | head -n1)
  CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
  CPUMODELNO=$(grep -v 'model name' /proc/cpuinfo | awk -F ": " '/model/ {print $2}' | uniq)
  SSEFOURTWOCHECK=$(grep -o sse4_2 /proc/cpuinfo | sort -u | wc -l)
  MARCHCHECK=$(gcc -c -Q -march=native --help=target | awk '/  -march=/ {print $2}' | head -n1)
  gcc --version | tee ${CENTMINLOGDIR}/gcc_php_native.log
  gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_php_native.log


  if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_GSPLITDWARF" = [yY] && "$PHP_LTO" = [yY] ]]; then
    PHPEXTRA_CFLAGS="$PHPEXTRA_CFLAGS -gsplit-dwarf -flto -flto-partition=max"
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_GSPLITDWARF" = [yY] && "$PHP_LTO" = [nN] ]]; then
    PHPEXTRA_CFLAGS="$PHPEXTRA_CFLAGS -gsplit-dwarf"
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_LTO" = [yY] ]]; then
    PHPEXTRA_CFLAGS="$PHPEXTRA_CFLAGS -flto -flto-partition=max"
  fi

	if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		CCM=64
    if [[ "$CPU_MARCH" = 'silvermont' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=silvermont"
    elif [[ "$CPU_MARCH" = 'bonnell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=bonnell"
    elif [[ "$CPU_MARCH" = 'broadwell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=broadwell"
    elif [[ "$CPU_MARCH" = 'haswell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=haswell"
    elif [[ "$CPU_MARCH" = 'ivybridge' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=ivybridge"
    elif [[ "$CPU_MARCH" = 'sandybridge' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=sandybridge"
    elif [[ "$CPU_MARCH" = 'westmere' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=westmere"
    elif [[ "$CPU_MARCH" = 'nehalem' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=nehalem"
    elif [[ "$CPU_MARCH" = 'core-avx2' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core-avx2"
    elif [[ "$CPU_MARCH" = 'core-avx-i' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core-avx-i"
    elif [[ "$CPU_MARCH" = 'corei7-avx' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=corei7-avx"
    elif [[ "$CPU_MARCH" = 'corei7' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=corei7"
    elif [[ "$CENTOS_SIX" = '6' && "$CPU_MARCH" = 'core2' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core2"
    elif [[ "$CENTOS_SEVEN" = '7' && "$CPU_MARCH" = 'core2' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core2"
    else
      PHP_MTUNEOPT="-m${CCM} -mtune=native"
    fi
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    fi
		export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'core-avx-i')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'corei7')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'core2')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
	elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'pentium')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		CCM=64
		PHP_MTUNEOPT="-m${CCM} -mtune=native"
		export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
		export CXXFLAGS="$CFLAGS"
	elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[disabled]' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		CCM=64
		PHP_MTUNEOPT="-m${CCM} -mtune=native"
		export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
		export CXXFLAGS="$CFLAGS"
	elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && -z "$SSECHECK" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		CCM=64
		PHP_MTUNEOPT="-m${CCM} -mtune=generic"
		export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
		export CXXFLAGS="$CFLAGS"
	elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		CCM=32
		PHP_MTUNEOPT="-m${CCM} -mtune=generic"
		export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${PHPEXTRA_CFLAGS}"
		export CXXFLAGS="$CFLAGS"
	else
		PHP_MTUNEOPT=""
	fi
fi

# zend opcache hugepages
if [[ "$PHPMUVER" > 7 ]]; then
	opcachehugepages
else
	if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
		if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
			sed -i 's|^opcache.huge_code_pages=1|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
			sed -i 's|^;opcache.huge_code_pages=1|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
			sed -i 's|^opcache.huge_code_pages=0|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
			sed -i 's|^;opcache.huge_code_pages=0|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini			
		fi		
	fi	
fi

if [[ "$PHP_CUSTOMSSL" = [yY] ]]; then
  if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
    PHP_OPENSSLOPT='--with-openssl=/opt/libressl'
    ln -s /opt/libressl/lib /opt/libressl/lib64
  else
    PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
    ln -s "${OPENSSL_CUSTOMPATH}/lib" "${OPENSSL_CUSTOMPATH}/lib64"
  fi
else
  PHP_OPENSSLOPT='--with-openssl'
fi

# separate make threads for PHP compiles
if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS_PHP=" -j$CPUS"
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
    else
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
else
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
    else
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
fi

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] ]]; then

	echo "../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${PHP_EXTRAOPTS}"

../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${PHP_EXTRAOPTS}

else

	echo "../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${PHP_EXTRAOPTS}"

../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf --with-freetype-dir=${PLIBDIR} --with-jpeg-dir=${PLIBDIR} --with-png-dir=${PLIBDIR} --with-xpm-dir=${PLIBDIR}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${PHP_EXTRAOPTS}

fi
sar_call
    echo
    echo "php_configure.inc make${MAKETHREADS_PHP}"
    time make${MAKETHREADS_PHP}
    sar_call
    echo

	if [[ "$PHPMAKETEST" = [yY] ]]; then
		time make test
    sar_call
	fi

    time make install
    sar_call

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

if [[ "$GCCINTEL_PHP" = [yY] ]]; then
	if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		unset CFLAGS
		unset CXXFLAGS
	elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
		unset CFLAGS
		unset CXXFLAGS
	fi
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