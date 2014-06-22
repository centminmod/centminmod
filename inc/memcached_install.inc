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

PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

    cecho "Checking for igbinary.so ..." $boldgreen

cecho "Check if ${PHPEXTDIRD}/igbinary.so exists" $boldyellow
if [ ! -f "${PHPEXTDIRD}/igbinary.so" ]; then
funct_igbinaryinstall
fi

if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini ]]; then
	funct_igbinaryinstall
	#echo "will need to reinstall memcached extensions if needed"
fi

}

#######################################################
function funct_memcachedreinstall {

PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

    echo "*************************************************"
    cecho "* Installing memcached" $boldgreen
    echo "*************************************************"
    echo "Downloading memcached..."
    cd $DIR_TMP

#download memcached, memcache, libevent

checkmcperf

checkmemcachedservertar

checklibevent

checkigbinary

if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then
    checklibmemcachedtar
fi

checkmemcachedtar

# Set LIBDIR
if [ ${ARCH} == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

echo $ARCH
echo $LIBDIR

#############################################

if [[ "$FPMRPM_LIBEVENT" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    mkdir -p $DIR_TMP/fpmrpm
    cd $DIR_TMP/fpmrpm
    if [ -s libevent-2.0.21-1.x86_64.rpm ]; then
        cecho "libevent-2.0.21-1.x86_64.rpm found, skipping download..." $boldgreen
    else
        wget -cnv http://centminmod.com/centminmodparts/customrpms/fpm/libevent-2.0.21-1.x86_64.rpm --tries=3
        rpm -i libevent-2.0.21-1.x86_64.rpm
    fi
else
    cd $DIR_TMP
    #tar xfz release-${LIBEVENT_VERSION}-stable.tar.gz
    cd libevent-release-${LIBEVENT_VERSION}-stable
    make clean
    ./configure --prefix=/usr/${LIBDIR}
    make${MAKETHREADS}
    make install

fi # FPMRPM_LIBEVENT

echo "/usr/${LIBDIR}/lib/" > /etc/ld.so.conf.d/libevent-i386.conf
ldconfig

#############################################
    cecho "Compiling memcached..." $boldgreen

if [[ "$FPMRPM_MEMCACHED" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    mkdir -p $DIR_TMP/fpmrpm
    cd $DIR_TMP/fpmrpm
    if [ -s memcached-1.4.18-1.x86_64.rpm ]; then
        cecho "memcached-1.4.18-1.x86_64.rpm found, skipping download..." $boldgreen
    else
        wget -cnv http://centminmod.com/centminmodparts/customrpms/fpm/memcached-1.4.18-1.x86_64.rpm --tries=3
        rpm -i memcached-1.4.18-1.x86_64.rpm
        cd ${DIR_TMP}/memcached-${MEMCACHED_VERSION}
        cp -f $DIR_TMP/memcached-${MEMCACHED_VERSION}/scripts/memcached-tool /usr/local/bin
    fi
else
    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
    #tar xzf memcached-${MEMCACHED_VERSION}.tar.gz
    cd ${DIR_TMP}/memcached-${MEMCACHED_VERSION}
    make clean
    ./configure --with-libevent=/usr/${LIBDIR} --enable-sasl --enable-sasl-pwdb
    # ./configure --with-libevent=/usr/${LIBDIR} --enable-memcached --enable-memcached-igbinary --enable-memcached-json --with-php- config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install
    cp -f $DIR_TMP/memcached-${MEMCACHED_VERSION}/scripts/memcached-tool /usr/local/bin
fi # FPMRPM_MEMCACHED

    cecho "Compiled memcached..." $boldgreen

killall memcached
sleep 2
service memcached stop
service memcached start

#############################################

    cecho "Compiling PHP memcache extension..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
#tar -xvf memcache-${MEMCACHE_VERSION}.tgz
cd ${DIR_TMP}/memcache-${MEMCACHE_VERSION}
make clean
/usr/local/bin/phpize
./configure --enable-memcache --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
make install

#############################################
if [[ "$LIBMEMCACHED_YUM" = [yY] ]]; then
    
    cecho "Installing libmemcached via CentALT YUM repo ..." $boldgreen
    yum${CACHESKIP} -q -y install libmemcached libmemcached-devel --enablerepo=CentALT --disableplugin=priorities
else
    
    cecho "Compiling libmemcached-${LIBMEMCACHED_VER} ..." $boldgreen
    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
    #tar xvzf libmemcached-${LIBMEMCACHED_VER}.tar.gz
    cd ${DIR_TMP}/libmemcached-${LIBMEMCACHED_VER}
    make clean
    ./config/autorun.sh
    ./configure
    time make${MAKETHREADS}
    time make install
fi 
wait
#############################################
    cecho "Compiling PHP memcached-${MEMCACHEDPHP_VER} extension..." $boldgreen

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP
#tar xvzf memcached-${MEMCACHEDPHP_VER}.tgz
cd ${DIR_TMP}/memcached-${MEMCACHEDPHP_VER}
make clean
/usr/local/bin/phpize
if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then
    ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-igbinary --enable-memcached-json --with-libmemcached-dir=/usr/local
else
    ./configure --with-php-config=/usr/local/bin/php-config --enable-memcached-igbinary --enable-memcached-json --with-libmemcached-dir=/usr
fi
make${MAKETHREADS}
make install

#############################################
#     cecho "Compiling twemperf-${TWEMPERF_VER}..." $boldgreen

# cd $DIR_TMP
# echo "cd twemperf-${TWEMPERF_VER}"
# cd twemperf-${TWEMPERF_VER}
# make clean
# ./configure
# make${MAKETHREADS}
# make install

#######################################################
# check to see if memcache.so already loaded in php.ini

MEMCACHESOCHECK=`grep 'memcache.so' /usr/local/lib/php.ini`
MEMCACHESOCHECKB=$(grep 'memcache.so' ${CONFIGSCANDIR}/memcache.ini)
PHPEXTDIR=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

if [[ -z "$MEMCACHESOCHECK" || -z "$MEMCACHESOCHECKB" ]]; then

#echo "" >> /usr/local/lib/php.ini
#echo "extension=${PHPEXTDIR}/memcache.so" >> /usr/local/lib/php.ini
echo "extension=${PHPEXTDIR}/memcache.so" > ${CONFIGSCANDIR}/memcache.ini

else

echo ""
echo "memcache.so already loaded in ${CONFIGSCANDIR}/memcache.ini"
echo "$MEMCACHESOCHECK"
echo ""

fi # memcache.so check

#######################################################
# check to see if memcached.so already loaded in php.ini

MEMCACHEDSOCHECK=`grep 'memcached.so' /usr/local/lib/php.ini`
MEMCACHEDSOCHECKB=$(grep 'memcached.so' ${CONFIGSCANDIR}/memcached.ini)
PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

if [[ -z "$MEMCACHEDSOCHECK" || -z "$MEMCACHEDSOCHECKB" ]]; then

#echo "" >> /usr/local/lib/php.ini
echo "extension=${PHPEXTDIRD}/memcached.so" > ${CONFIGSCANDIR}/memcached.ini
echo "memcached.sess_connect_timeout=3000" >> ${CONFIGSCANDIR}/memcached.ini

else

echo ""
echo "memcached.so already loaded in ${CONFIGSCANDIR}/memcached.ini"
echo "$MEMCACHEDSOCHECK"
echo ""

fi # memcached.so check

killall memcached
sleep 2
service memcached stop
service memcached start
/etc/init.d/php-fpm restart

}


#######################################################
incmemcachedinstall() {

#ASK "Install Memcached Server? (default uses 8MB RAM each) [y/n] "
if [[ "$MEMCINSTALL" = [yY] ]];
then

funct_memcachedreinstall

cd $SCRIPT_DIR

#######################################################
# check /etc/init.d/memcached

if [[ ! -f /etc/init.d/memcached ]]; then

#echo ""
#echo -n "Install how many Memcached servers ? 1 or 2 ?: "
#read nomemcachedservers

echo "Setup /etc/init.d/memcached with $nomemcachedservers Memcached servers"

if [[ "$nomemcachedservers" = '1' ]]; then

echo "setup /etc/init.d/memcached"
echo "cp $CUR_DIR/config/memcached/memcached /etc/init.d/memcached"
cp $CUR_DIR/config/memcached/memcached /etc/init.d/memcached

fi

if [[ "$nomemcachedservers" = '2' ]]; then

echo "setup /etc/init.d/memcached"
echo "cp $CUR_DIR/config/memcached/memcached2 /etc/init.d/memcached"
cp $CUR_DIR/config/memcached/memcached2 /etc/init.d/memcached

fi

fi # check /etc/init.d/memcached

chmod +x /etc/init.d/memcached
chkconfig --add memcached
chkconfig --level 345 memcached on
service memcached start

    cecho "Compiling PHP memcache extension..." $boldgreen

cd $DIR_TMP
tar -xvf memcache-${MEMCACHE_VERSION}.tgz
cd memcache-${MEMCACHE_VERSION}
/usr/local/bin/phpize
./configure --enable-memcache --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS} && make install

#######################################################
# check to see if memcache.so already loaded in php.ini

MEMCACHESOCHECK=`grep 'memcache.so' /usr/local/lib/php.ini`
MEMCACHESOCHECKB=$(grep 'memcache.so' ${CONFIGSCANDIR}/memcache.ini)
PHPEXTDIR=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

if [[ -z "$MEMCACHESOCHECK" || -z "$MEMCACHESOCHECKB" ]]; then

#echo "" >> /usr/local/lib/php.ini
#echo "extension=${PHPEXTDIR}/memcache.so" >> /usr/local/lib/php.ini
echo "extension=${PHPEXTDIR}/memcache.so" > ${CONFIGSCANDIR}/memcache.ini

else

echo ""
echo "memcache.so already loaded in /usr/local/lib/php.ini"
echo "$MEMCACHESOCHECK"
echo ""

fi # memcache.so check

/etc/init.d/php-fpm restart

    echo "*************************************************"
    cecho "* memcached installed" $boldgreen
    echo "*************************************************"

fi

}