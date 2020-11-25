#######################################################
checkmcperf() {

cecho "Check if mcperf-${TWEMPERF_VER}.tar.gz exists" $boldyellow
if [ ! -f "${DIR_TMP}/mcperf-${TWEMPERF_VER}.tar.gz" ]; then

    cecho "Downloading mcperf tarball.." $boldgreen

twemperftarball
fi

}


checkmemcachedservertar() {

cecho "Check if memcached-${MEMCACHED_VERSION}.tar.gz exists" $boldyellow
if [ ! -f "${DIR_TMP}/memcached-${MEMCACHED_VERSION}.tar.gz" ]; then

    cecho "Downloading memcached server tarball.." $boldgreen

memcachetarball
fi

}

checklibevent() {

cecho "Check if release-${LIBEVENT_VERSION}-stable.tar.gz exists" $boldyellow
if [ ! -f "${DIR_TMP}/release-${LIBEVENT_VERSION}-stable.tar.gz" ]; then

    cecho "Downloading libevent..." $boldgreen

libeventtarball
#memcacheexttarball
#memcachedphptarball
#libmemcachedtarball
fi

}

checkmemcachedtar() {

cecho "Check if memcached-${MEMCACHEDPHP_VER}.tgz exists" $boldyellow
if [ ! -f "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}.tgz" ]; then

    cecho "Downloading memcached PHP extension.." $boldgreen

memcachedphptarball
#libmemcachedtarball
fi

}

checklibmemcachedtar() {

cecho "Check if libmemcached-${LIBMEMCACHED_VER}.tar.gz exists" $boldyellow
if [ ! -f "${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}.tar.gz" ]; then

    cecho "Downloading libmemcached.." $boldgreen

libmemcachedtarball
fi

}

checkigbinary() {
    PHPEXTDIRD=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')
    cecho "Checking for igbinary.so ..." $boldgreen
    cecho "Check if ${PHPEXTDIRD}/igbinary.so or ${CONFIGSCANDIR}/igbinary.ini exists" $boldyellow

    if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini || ! -f "${PHPEXTDIRD}/igbinary.so" ]]; then
       funct_igbinaryinstall
       #echo "will need to reinstall memcached extensions if needed"
    fi
    
    # check for different igbinary versions
    if [ -f /usr/local/bin/php ]; then
        if [[ "$(php --ri igbinary | awk '/igbinary version/ {print $4}')" != "${IGBINARY_VERSION}" ]]; then
            cecho "Check for different igbinary versions ..." $boldgreen
            funct_igbinaryinstall
        fi
    fi
}

#######################################################
funct_memcachedreinstall() {
   if [[ "$CLANG_MEMCACHED" = [yY] ]]; then
      enable_clang
   else
      check_devtoolset_php
      enable_devtoolset
   fi

PHPEXTDIRD=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

    echo "*************************************************"
    cecho "* Installing memcached" $boldgreen
    echo "*************************************************"

    if [ ! "$(grep -w 'memcached' /etc/passwd)" ]; then
      echo "Update memcached server setup"
      echo "Adding memcached user/group and adding to nginx group"
      adduser -s /sbin/nologin -M memcached >/dev/null
      usermod -G nginx memcached >/dev/null
      id memcached
      SOCKETPATCH=y
    elif [[ "$(grep -w 'memcached' /etc/passwd)" && "$(grep -w 'nginx' /etc/passwd)" ]]; then
      if [[ "$(id memcached | grep -o nginx)" != 'nginx' ]]; then
        usermod -G nginx memcached >/dev/null
        id memcached
        SOCKETPATCH=y
      fi
    fi
    if [[ "$(grep -w "USER=nobody" /etc/init.d/memcached)" && "$SOCKETPATCH" = [Yy] ]]; then
        echo "Changing memcached server user from nobody to memcached"
        sed -i 's|USER=nobody|USER=memcached|' /etc/init.d/memcached
        grep -w "USER=memcached" /etc/init.d/memcached
        echo "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch"
        if [[ -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" && -f /etc/init.d/memcached ]]; then
            \cp -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" /etc/init.d/memcached-socketfix.patch
            pushd /etc/init.d/
            patch -s < memcached-socketfix.patch >/dev/null 2>&1
            rm -rf memcached-socketfix.patch
            if [[ "$CENTOS_SEVEN" = '7' ]]; then
                systemctl daemon-reload
            fi
            if [ "$(ps -C memcached | grep -w memcached)" ]; then
                /usr/bin/memcachedstop >/dev/null
                /usr/bin/memcachedstart >/dev/null
            fi
            popd
        fi
    elif [[ "$SOCKETPATCH" = [Yy] ]]; then
        echo "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch"
        if [[ -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" && -f /etc/init.d/memcached ]]; then
            \cp -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" /etc/init.d/memcached-socketfix.patch
            pushd /etc/init.d/
            patch -s < memcached-socketfix.patch >/dev/null 2>&1
            rm -rf memcached-socketfix.patch
            if [[ "$CENTOS_SEVEN" = '7' ]]; then
                systemctl daemon-reload
            fi
            if [ "$(ps -C memcached | grep -w memcached)" ]; then
                /usr/bin/memcachedstop >/dev/null
                /usr/bin/memcachedstart >/dev/null
            fi
            popd
        fi
    fi
    if [[ ! "$(grep -w '\-a 0766' /etc/init.d/memcached)" ]]; then
      sed -i 's|0775|0766|g' /etc/init.d/memcached
    fi # 766 fix

    echo "Downloading memcached..."
    cd "$DIR_TMP"

#download memcached, memcache, libevent

checkmcperf

checkmemcachedservertar

checklibevent

# memcached server no longer compiled with igbinary support so can skipp this check
# checkigbinary

if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then
    checklibmemcachedtar
fi

checkmemcachedtar

# Set LIBDIR
if [ "${ARCH}" == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

echo "$ARCH"
echo "$LIBDIR"

#############################################

if [[ "$FPMRPM_LIBEVENT" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    mkdir -p "$DIR_TMP/fpmrpm"
    cd "$DIR_TMP/fpmrpm"
    if [ -s libevent-2.0.21-1.x86_64.rpm ]; then
        cecho "libevent-2.0.21-1.x86_64.rpm found, skipping download..." $boldgreen
    else
        wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/customrpms/fpm/libevent-2.0.21-1.x86_64.rpm --tries=3
        rpm -Uvh libevent-2.0.21-1.x86_64.rpm
    fi
else
    cd "$DIR_TMP"
    #tar xfz release-${LIBEVENT_VERSION}-stable.tar.gz
    cd "libevent-release-${LIBEVENT_VERSION}-stable"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./autogen.sh
    ./configure --prefix="/usr/${LIBDIR}"
    make${MAKETHREADS}
    make install

fi # FPMRPM_LIBEVENT

echo "/usr/${LIBDIR}/lib/" > /etc/ld.so.conf.d/libevent-i386.conf
ldconfig

#############################################
    cecho "Compiling memcached..." $boldgreen

if [[ "$FPMRPM_MEMCACHED" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    mkdir -p $DIR_TMP/fpmrpm
    cd "$DIR_TMP/fpmrpm"
    if [ -s memcached-1.4.18-1.x86_64.rpm ]; then
        cecho "memcached-1.4.18-1.x86_64.rpm found, skipping download..." $boldgreen
    else
        wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/customrpms/fpm/memcached-1.4.18-1.x86_64.rpm --tries=3
        rpm -Uvh memcached-1.4.18-1.x86_64.rpm
        cd "${DIR_TMP}/memcached-${MEMCACHED_VERSION}"
        cp -f "$DIR_TMP/memcached-${MEMCACHED_VERSION}/scripts/memcached-tool" /usr/local/bin
    fi
else
    pwd
    echo "cd $DIR_TMP"
    cd "$DIR_TMP"
    #tar xzf memcached-${MEMCACHED_VERSION}.tar.gz
    if [ ! -d "${DIR_TMP}/memcached-${MEMCACHED_VERSION}" ]; then
        rm -rf "${DIR_TMP}/memcached-${MEMCACHED_VERSION}*"
        memcachetarball local
    fi
    cd "${DIR_TMP}/memcached-${MEMCACHED_VERSION}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    MEMCACHED_VERSION_NUMERIC=$(echo $MEMCACHED_VERSION | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')
    if [[ "$MEMCACHED_VERSION_NUMERIC" -ge '1015013000' && "$MEMCACHED_TLS" = [yY] ]]; then
        MEMCACHED_TLSOPTION=' --enable-tls'
    else
        MEMCACHED_TLSOPTION=""
    fi
    ./configure --with-libevent="/usr/${LIBDIR}" --enable-sasl --enable-sasl-pwdb${MEMCACHED_TLSOPTION}
    # ./configure --with-libevent=/usr/${LIBDIR} --enable-memcached --enable-memcached-igbinary --enable-memcached-json --with-php- config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install
    cp -f "$DIR_TMP/memcached-${MEMCACHED_VERSION}/scripts/memcached-tool" /usr/local/bin
fi # FPMRPM_MEMCACHED

    cecho "Compiled memcached server..." $boldgreen

if [[ "$CENTOS_SIX" = '6' ]]; then
    kill -9 $(pidof memcached)
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    kill -9 $(pidof memcached)
fi
sleep 2
service memcached stop
service memcached start

#############################################
    if [[ "$PHP_INSTALL" = [yY] ]]; then
if [ "$PHP_MEMCACHE" == 'y' ]; then
    PHPCURRENTVER=$(php -v 2>&1 | grep -v 'PHP Warning' | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
    if [[ "$PHPCURRENTVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPCURRENTVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPCURRENTVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPCURRENTVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPCURRENTVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPCURRENTVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]]; then
        echo
        if [[ "$PHPVER_ID" -ge '80000' ]]; then
            echo "Compiling memcache PHP extension for PHP 8.x ..."
        else
            echo "Compiling memcache PHP extension for PHP 7.x ..."
        fi
    
        if [ -f /usr/bin/re2c ]; then
            if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
                rerpm
            fi
        fi
        pushd "$DIR_TMP"
        if [[ "$PHPVER_ID" -ge '80000' ]]; then
            rm -rf memcache-php8
            time git clone -b NON_BLOCKING_IO_php8 https://github.com/websupport-sk/pecl-memcache.git memcache-php8
            cd memcache-php8
        else
            rm -rf memcache-php7
            time git clone -b NON_BLOCKING_IO_php7 https://github.com/websupport-sk/pecl-memcache.git memcache-php7
            cd memcache-php7
        fi
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [[ "$CENTOS_SIX" -eq '6' ]]; then
            # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
            if [ ! -f /usr/bin/autoconf268 ]; then
                echo "yum -q -y install autoconf268"
                yum -q -y install autoconf268
            fi
            if [ -f /usr/bin/autoconf268 ]; then
                export PHP_AUTOCONF=/usr/bin/autoconf268
                export PHP_AUTOHEADER=/usr/bin/autoheader268
            fi
        fi
        if [[ "$PHPCURRENTVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
           # rollback to commit prior to memcache 4.0.1 release
           # https://github.com/websupport-sk/pecl-memcache/issues/38
           # git checkout 4a9e4ab0d1
           echo "skip git checkout 4a9e4ab0d1"
           # patch support https://github.com/websupport-sk/pecl-memcache/pull/30
           # wget -O memcache-php73.patch https://patch-diff.githubusercontent.com/raw/websupport-sk/pecl-memcache/pull/30.patch
           # patch -p1 < memcache-php73.patch
        elif [[ "$PHPCURRENTVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPCURRENTVER" = '7.4' || "$PHPMUVER" = '7.4' ]]; then
           # rollback to commit prior to memcache 4.0.1 release
           # https://github.com/websupport-sk/pecl-memcache/issues/38
           # git checkout 4a9e4ab0d1
           echo "skip git checkout 4a9e4ab0d1"
           # patch support https://github.com/websupport-sk/pecl-memcache/pull/30
           # wget -O memcache-php73.patch https://patch-diff.githubusercontent.com/raw/websupport-sk/pecl-memcache/pull/30.patch
           # patch -p1 < memcache-php73.patch
        elif [[ "$PHPCURRENTVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPCURRENTVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPCURRENTVER" = '7.2' || "$PHPMUVER" = '7.2' ]]; then
           # rollback to commit prior to memcache 4.0.1 release
           # https://github.com/websupport-sk/pecl-memcache/issues/38
           # git checkout 4a9e4ab0d1
           echo "skip git checkout 4a9e4ab0d1"
           #wget -O memcache-php73.patch https://patch-diff.githubusercontent.com/raw/websupport-sk/pecl-memcache/pull/30.patch
           #patch -p1 < memcache-php73.patch
        elif [[ "$PHPCURRENTVER" = '5.4' || "$PHPMUVER" = '5.4' || "$PHPCURRENTVER" = '5.5' || "$PHPMUVER" = '5.5' || "$PHPCURRENTVER" = '5.6' || "$PHPMUVER" = '5.6' ]]; then
           # rollback to commit prior to memcache 4.0.1 release
           # https://github.com/websupport-sk/pecl-memcache/issues/38
           # git checkout 4a9e4ab0d1
           echo "skip git checkout 4a9e4ab0d1"
        fi
        /usr/local/bin/phpize
        ./configure --enable-memcache --with-php-config=/usr/local/bin/php-config
        make${MAKETHREADS}
        make install
        cecho "PHP memcache extension installed" $boldgreen
        popd
    else
        cecho "Compiling PHP memcache extension..." $boldgreen
        pwd
        echo "cd $DIR_TMP"
        cd "$DIR_TMP"
    
        if [ -f /usr/bin/re2c ]; then
            if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
                rerpm
            fi
        fi
    
        #tar -xvf memcache-${MEMCACHE_VERSION}.tgz
        if [ ! -d "${DIR_TMP}/memcache-${MEMCACHE_VERSION}" ]; then
            rm -rf "${DIR_TMP}/memcache-${MEMCACHE_VERSION}*"
            memcacheextlocaltarball
        fi
        cd "${DIR_TMP}/memcache-${MEMCACHE_VERSION}"
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        /usr/local/bin/phpize
        ./configure --enable-memcache --with-php-config=/usr/local/bin/php-config
        make${MAKETHREADS}
        make install
        cecho "PHP memcache extension installed" $boldgreen
    
    fi
else
    # if PHP_MEMCACHE=n remove memcache PHP extension setting file
    rm -rf "${CONFIGSCANDIR}/memcache.ini"
fi # PHP_MEMCACHE = y
    fi # PHP_INSTALL=y

#############################################
if [ "$PHP_MEMCACHED" == 'y' ]; then
if [[ "$LIBMEMCACHED_YUM" = [yY] ]]; then
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        cecho "Installing libmemcached via CentOS base YUM repo ..." $boldgreen
        if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
            ${YUMDNFBIN}${CACHESKIP} -q -y install libmemcached libmemcached-devel --disablerepo=remi --disablerepo=epel --disablerepo=rpmforge --disableplugin=priorities
        else
            ${YUMDNFBIN}${CACHESKIP} -q -y install libmemcached libmemcached-devel --disablerepo=remi --disablerepo=epel --disableplugin=priorities
        fi
    elif [[ "$CENTOS_SEVEN" != '7' && -f /etc/yum.repos.d/remi.repo ]]; then
        cecho "Installing libmemcached via REMI YUM repo ..." $boldgreen
        ${YUMDNFBIN}${CACHESKIP} -q -y install libmemcached-last libmemcached-last-devel --enablerepo=remi --disableplugin=priorities
    else    
        cecho "Compiling libmemcached-${LIBMEMCACHED_VER} ..." $boldgreen
        pwd
        echo "cd $DIR_TMP"
        cd "$DIR_TMP"
        #tar xvzf libmemcached-${LIBMEMCACHED_VER}.tar.gz
        cd ${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        ./config/autorun.sh
        ./configure
        time make${MAKETHREADS}
        time make install
        cecho "libmemcached installed" $boldgreen
    fi # centos not 7
else    
    cecho "Compiling libmemcached-${LIBMEMCACHED_VER} ..." $boldgreen
    pwd
    echo "cd $DIR_TMP"
    cd "$DIR_TMP"
    #tar xvzf libmemcached-${LIBMEMCACHED_VER}.tar.gz
    if [ ! -d "${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}" ]; then
        rm -rf "${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}*"
        libmemcachedlocaltarball
    fi
    cd "${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./config/autorun.sh
    ./configure
    time make${MAKETHREADS}
    time make install
    cecho "libmemcached installed" $boldgreen
fi 
wait

    if [[ "$PHP_INSTALL" = [yY] ]]; then
#############################################
    cecho "Compiling PHP memcached-${MEMCACHEDPHP_VER} extension..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd "$DIR_TMP"

if [ -f /usr/bin/re2c ]; then
    if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
        rerpm
    fi
fi

php -v 2>&1 | grep -v 'PHP Warning' | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0|7.1|7.2|7.3|7.4|8.0'
PHPSEVEN_CHECKVER=$?
echo "$PHPSEVEN_CHECKVER"

if [[ "$PHPMUVER" > 7 || "$PHPSEVEN_CHECKVER" = '0' ]]; then
    MEMCACHEDPHP_VER="$MEMCACHEDPHP_SEVENVER"
    PHPEXT_MEMCACHEDLINKFILE="memcached-${MEMCACHEDPHP_VER}.tgz"
    PHPEXT_MEMCACHEDLINK="https://pecl.php.net/get/${PHPEXT_MEMCACHEDLINKFILE}"
    PHPEXT_MEMCACHEDLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/memcached/${PHPEXT_MEMCACHEDLINKFILE}"
    echo
    if [[ "$PHPVER_ID" -ge '80000' ]]; then
        echo "compiling memcached PHP extension for PHP 8.x ..."
    else
        echo "compiling memcached PHP extension for PHP 7.x ..."
    fi
    echo "PHPMUVER = $PHPMUVER"
    if [[ "$CENTOS_SIX" -eq '6' ]]; then
        # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
        if [ ! -f /usr/bin/autoconf268 ]; then
            echo "yum -q -y install autoconf268"
            yum -q -y install autoconf268
        fi
        if [ -f /usr/bin/autoconf268 ]; then
            export PHP_AUTOCONF=/usr/bin/autoconf268
            export PHP_AUTOHEADER=/usr/bin/autoheader268
        fi
    fi
    if [[ "$PHPMUVER" = '8.0' || "$PHPCURRENTVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
      pushd "$DIR_TMP"
      rm -rf memcached-php8
      time git clone --depth=1 https://github.com/php-memcached-dev/php-memcached.git memcached-php8
      popd
      cd "${DIR_TMP}/memcached-php8"
    else
      if [ ! -d "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}" ]; then
          rm -rf "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}*"
          memcachedphplocaltarball
      fi
      cd "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}"
    fi
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr/local
    elif [[ "$LIBMEMCACHED_YUM" = [yY] && -f /etc/yum.repos.d/remi.repo ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr
    elif [ ! -f /etc/yum.repos.d/remi.repo ]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr/local        
    fi
    make${MAKETHREADS}
    make install
    cecho "memcached PHP extension installed" $boldgreen
else
    echo
    echo "compiling memcached PHP extension ${MEMCACHEDPHP_VER} ..."
    if [ ! -d "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}" ]; then
        rm -rf "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}*"
        memcachedphplocaltarball
    fi
    cd "${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr/local
    elif [[ "$LIBMEMCACHED_YUM" = [yY] && -f /etc/yum.repos.d/remi.repo ]]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr
    elif [ ! -f /etc/yum.repos.d/remi.repo ]; then
        ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-json --with-libmemcached-dir=/usr/local        
    fi
    make${MAKETHREADS}
    make install
    cecho "memcached PHP extension installed" $boldgreen
fi # php 7 or not

#############################################
#     cecho "Compiling twemperf-${TWEMPERF_VER}..." $boldgreen

# cd "$DIR_TMP"
# echo "cd twemperf-${TWEMPERF_VER}"
# cd twemperf-${TWEMPERF_VER}
#if [[ "$INITIALINSTALL" != [yY] ]]; then
#    make clean
#fi
# ./configure
# make${MAKETHREADS}
# make install

#######################################################
# check to see if memcache.so already loaded in php.ini

MEMCACHESOCHECK=$(grep 'memcache.so' /usr/local/lib/php.ini)
echo "check ${CONFIGSCANDIR}/memcache.ini"
MEMCACHESOCHECKB=$(grep 'memcache.so' "${CONFIGSCANDIR}/memcache.ini")
PHPEXTDIR=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

if [[ -z "$MEMCACHESOCHECK" || -z "$MEMCACHESOCHECKB" ]]; then

#echo "" >> /usr/local/lib/php.ini
#echo "extension=${PHPEXTDIR}/memcache.so" >> /usr/local/lib/php.ini
echo "extension=${PHPEXTDIR}/memcache.so" > "${CONFIGSCANDIR}/memcache.ini"

else

echo ""
echo "memcache.so already loaded in ${CONFIGSCANDIR}/memcache.ini"
echo "$MEMCACHESOCHECK"
echo ""

fi # memcache.so check

#######################################################
# check to see if memcached.so already loaded in php.ini

MEMCACHEDSOCHECK=$(grep 'memcached.so' /usr/local/lib/php.ini)
echo "check ${CONFIGSCANDIR}/memcached.ini"
MEMCACHEDSOCHECKB=$(grep 'memcached.so' "${CONFIGSCANDIR}/memcached.ini")
PHPEXTDIRD=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

if [[ -z "$MEMCACHEDSOCHECK" || -z "$MEMCACHEDSOCHECKB" ]]; then

#echo "" >> /usr/local/lib/php.ini
echo "extension=${PHPEXTDIRD}/memcached.so" > "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_connect_timeout=3000" >> "${CONFIGSCANDIR}/memcached.ini"
echo "; http://php.net/manual/en/memcached.configuration.php" >> "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_consistent_hash=0" >> "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_number_of_replicas=0" >> "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_randomize_replica_read=0" >> "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_remove_failed=0" >> "${CONFIGSCANDIR}/memcached.ini"
echo "memcached.sess_binary=0" >> "${CONFIGSCANDIR}/memcached.ini"

else

echo ""
echo "memcached.so already loaded in ${CONFIGSCANDIR}/memcached.ini"
echo "$MEMCACHEDSOCHECK"
echo ""

fi # memcached.so check

        if [[ "$CLANG_MEMCACHED" = [yY] ]]; then
            unset CC
            unset CXX
            unset CFLAGS
            #unset CCACHE_CPP2
            export CC="ccache /usr/bin/gcc"
            export CXX="ccache /usr/bin/g++"
            CLANG_CFLAGOPT=""
            CFLAGS=""            
        fi   

else
    # if PHP_MEMCACHED=n remove memcached PHP extension setting file
    rm -rf "${CONFIGSCANDIR}/memcached.ini"
fi # PHP_MEMCACHED = y

    fi # PHP_INSTALL=y

if [[ "$CENTOS_SIX" = '6' ]]; then
    kill -9 "$(pidof memcached)"
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    kill -9 $(pidof memcached)
fi
sleep 2
service memcached stop
service memcached start
echo
service php-fpm restart
echo
journalctl -xe -u php-fpm --no-pager | tail -20
echo
echo "memcached -V"
memcached -V
echo
echo "php --ri memcache"
php --ri memcache
echo
echo "php --ri memcached"
php --ri memcached
echo
cecho "memcache/memcached routines completed" $boldgreen
}


#######################################################
incmemcachedinstall() {

if [[ "$MEMCACHED_INSTALL" = [yY] ]]; then
    MEMCINSTALL='y'
    nomemcachedservers='1'
fi

#ASK "Install Memcached Server? (default uses 8MB RAM each) [y/n] "
if [[ "$MEMCINSTALL" = [yY] ]];
then

funct_memcachedreinstall

cd "$SCRIPT_DIR"

#######################################################
# check /etc/init.d/memcached

# # only run for CentOS 6.x
# if [[ "$CENTOS_SEVEN" != '7' ]]; then
if [[ ! -f /etc/init.d/memcached ]]; then
    echo "Setup /etc/init.d/memcached with $nomemcachedservers Memcached servers"
    
    if [[ "$nomemcachedservers" = '1' ]]; then
        echo "setup /etc/init.d/memcached"
        echo "cp $CUR_DIR/config/memcached/memcached /etc/init.d/memcached"
        cp "$CUR_DIR/config/memcached/memcached" /etc/init.d/memcached
    fi
    
    if [[ "$nomemcachedservers" = '2' ]]; then
        echo "setup /etc/init.d/memcached"
        echo "cp $CUR_DIR/config/memcached/memcached2 /etc/init.d/memcached"
        cp "$CUR_DIR/config/memcached/memcached2" /etc/init.d/memcached
    fi
fi # check /etc/init.d/memcached

if [[ ! "$(grep PIDA /etc/init.d/memcached)" ]]; then
    # ensure newest init.d/memcached file is in place
    echo "Check /etc/init.d/memcached for PIDA / PIDB options"

    # transfer existing MEMSIZE memory allocation to newer init.d/memcached file
    MEMCACHED_EXISTMEMSIZE=$(awk -F "=" '/^MEMSIZE/ {print $2}' /etc/init.d/memcached)
    
    if [[ "$nomemcachedservers" = '1' ]]; then
        echo "setup /etc/init.d/memcached"
        echo "\cp -f $CUR_DIR/config/memcached/memcached /etc/init.d/memcached"
        \cp -f "$CUR_DIR/config/memcached/memcached" /etc/init.d/memcached
        sed -i "s|^MEMSIZE=.*|MEMSIZE=$MEMCACHED_EXISTMEMSIZE|" /etc/init.d/memcached
    fi
    
    if [[ "$nomemcachedservers" = '2' ]]; then
        echo "setup /etc/init.d/memcached"
        echo "\cp -f $CUR_DIR/config/memcached/memcached2 /etc/init.d/memcached"
        \cp -f "$CUR_DIR/config/memcached/memcached2" /etc/init.d/memcached
        sed -i "s|^MEMSIZE=.*|MEMSIZE=$MEMCACHED_EXISTMEMSIZE|" /etc/init.d/memcached
    fi
fi # check /etc/init.d/memcached for PIDA/PIDB

# add check for Windows CLRF line endings
if [ ! -f /usr/bin/file ]; then
    yum -q -y install file
fi
if [[ $(file /etc/init.d/memcached) =~ CRLF && -f /etc/init.d/memcached ]]; then
    if [ ! -f /usr/bin/dos2unix ]; then
        yum -q -y install dos2unix
    fi
    echo "detected CRLF line endings converting to Unix LF"
    dos2unix /etc/init.d/memcached
fi

chmod +x /etc/init.d/memcached
chkconfig --add memcached
chkconfig --level 345 memcached on
service memcached start
# fi # CENTOS_SEVEN != 7

#     if [[ "$PHP_INSTALL" = [yY] ]]; then
#     cecho "Compiling PHP memcache extension..." $boldgreen

# cd "$DIR_TMP"
# tar -xvf "memcache-${MEMCACHE_VERSION}.tgz"
# cd "memcache-${MEMCACHE_VERSION}"
# /usr/local/bin/phpize
# ./configure --enable-memcache --with-php-config=/usr/local/bin/php-config
# make${MAKETHREADS} && make install

# #######################################################
# # check to see if memcache.so already loaded in php.ini

# MEMCACHESOCHECK=$(grep 'memcache.so' /usr/local/lib/php.ini)
# MEMCACHESOCHECKB=$(grep 'memcache.so' "${CONFIGSCANDIR}/memcache.ini")
# PHPEXTDIR=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

# if [[ -z "$MEMCACHESOCHECK" || -z "$MEMCACHESOCHECKB" ]]; then

# #echo "" >> /usr/local/lib/php.ini
# #echo "extension=${PHPEXTDIR}/memcache.so" >> /usr/local/lib/php.ini
# echo "extension=${PHPEXTDIR}/memcache.so" > "${CONFIGSCANDIR}/memcache.ini"

# else

# echo ""
# echo "memcache.so already loaded in /usr/local/lib/php.ini"
# echo "$MEMCACHESOCHECK"
# echo ""

# fi # memcache.so check

# service php-fpm restart
#     fi # PHP_INSTALL=y

    echo "*************************************************"
    cecho "* memcached installed" $boldgreen
    echo "*************************************************"

fi

}