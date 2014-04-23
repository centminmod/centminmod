function funct_phpconfigure {

LIBCCLIENT=`yum list libc-client-devel | grep Available | awk '{print $1}'`

if [[ "$LIBCCLIENT" = 'Available' ]]; then
yum -y install libc-client-devel
fi

# low mem steps
# ulimit -v 262144
# --disable-fileinfo
# CFLAGS="--param ggc-min-expand=0 --param ggc-min-heapsize=8192" CXXFLAGS="${CFLAGS}" ../configure

CFLAGS="--param ggc-min-expand=0 --param ggc-min-heapsize=8192" CXXFLAGS="${CFLAGS}" ../configure --enable-cgi --enable-fpm${MCRYPT} --with-mhash --with-zlib --with-gettext --enable-exif --enable-zip --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring --with-openssl --with-mysql=/usr --with-libdir=${LIBDIR} --with-mysqli=/usr/bin/mysql_config --with-mysql-sock --with-curl --with-gd --with-xmlrpc --enable-bcmath --enable-calendar --enable-ftp --enable-gd-native-ttf --with-freetype-dir=${LIBDIR} --with-jpeg-dir=${LIBDIR} --with-png-dir=${LIBDIR} --with-xpm-dir=${LIBDIR} --enable-pdo --with-pdo-sqlite --with-pdo-mysql --enable-inline-optimization --with-imap --with-imap-ssl --with-kerberos --with-fpm-user=nginx --with-fpm-group=nginx

    make$MAKETHREADS
    make install

}