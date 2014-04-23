ngxinstallmain() {

if [[ "$NGINX_INSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Installing nginx" $boldgreen
    echo "*************************************************"

    # Disable Apache if installed
    if [ -f /etc/init.d/httpd ];
    then
      /sbin/service httpd stop
      chkconfig httpd off
    fi

    # Then install nginx
    cd $DIR_TMP

#######################################################
# check if custom open file descriptor limits already exist

LIMITSCONFCHECK=`grep '* hard nofile 65536' /etc/security/limits.conf`

if [[ -z $LIMITSCONFCHECK ]]; then

    # Set VPS hard/soft limits
    echo "* soft nofile 65536" >>/etc/security/limits.conf
    echo "* hard nofile 65536" >>/etc/security/limits.conf

fi # check if custom open file descriptor limits already exist

    # nginx Modules / Prerequisites
	cecho "Installing nginx Modules / Prerequisites..." $boldgreen

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Install Google Perftools" $boldgreen
    echo "*************************************************"

    # Install libunwind
    echo "Compiling libunwind..."
#download libunwind tarball

    #tar xvzf libunwind-${LIBUNWIND_VERSION}.tar.gz
    cd libunwind-${LIBUNWIND_VERSION}
    ./configure
    make$MAKETHREADS
    make install

    # Install google-perftools
    cd $DIR_TMP

    echo "Compiling google-perftools..."

#download gperftool tarball 

    #tar xvzf google-perftools-${GPERFTOOLS_VERSION}.tar.gz
    cd google-perftools-${GPERFTOOLS_VERSION}
    ./configure --enable-frame-pointers
    make$MAKETHREADS
    make install
    echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
    /sbin/ldconfig

fi # GPERFTOOLS_SOURCE INSTALL 

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

	if [ "$NOSOURCEOPENSSL" == 'n' ]; then
	installopenssl
	fi
else

	installopenssl

fi # for centos 6.x only

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"

    # Install PCRE
    cd $DIR_TMP

    echo "Compiling PCRE..."
#download pcre tarball

#tar xvzf pcre-${PCRE_VERSION}.tar.gz
    cd pcre-${PCRE_VERSION}
    ./configure
    make$MAKETHREADS
    make install

fi

#if [ "$NGINX_OPENRESTY" == 'y' ]; then
#funct_nginxmodules_openresty
#else
#funct_nginxmodules
#fi

    # Install nginx
    cd $DIR_TMP

    echo "Compiling nginx..."
#download nginx tarball

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        MBIT='64'
    else
        MBIT='32'
    fi

    #tar xvfz nginx-${NGINX_VERSION}.tar.gz
    cd nginx-${NGINX_VERSION}

if [ "$NGINX_OPENRESTY" == 'y' ]; then
funct_nginxconfigure_openresty
else
funct_nginxconfigure
fi

    make$MAKETHREADS
    make install
    mimefix
    groupadd nginx
    useradd -g nginx -d /home/nginx -s /sbin/nologin nginx
    #chmod 0755 /home/nginx

#######################################################
# check if custom open file descriptor limits already exist

NGXLIMITSCONFCHECK=`grep 'nginx hard nofile 65536' /etc/security/limits.conf`

if [[ -z $NGXLIMITSCONFCHECK ]]; then

    # Set user nginx hard/soft limits
    echo "nginx soft nofile 65536" >>/etc/security/limits.conf
    echo "nginx hard nofile 65536" >>/etc/security/limits.conf
    ulimit -n 65536

fi # check if custom open file descriptor limits already exist

    mkdir /home/nginx/domains
    #chmod 0771 /home/nginx/domains/
    mkdir -p /home/nginx/domains/demo.com/{public,private,log,backup}
    cp -R $CUR_DIR/htdocs/demo.com/* /home/nginx/domains/demo.com/public
    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/demo.com/public
    chown -R nginx:nginx /home/nginx

    mkdir -p /usr/local/nginx/html

    chmod g+s /home/nginx
    chmod g+s /home/nginx/domains/
    chmod g+s /usr/local/nginx/html

    mkdir -p /usr/local/nginx/conf/conf.d
    fpmconfdir
    mkdir -p /var/log/nginx
    touch /var/log/nginx/localhost.access.log
    touch /var/log/nginx/localhost.error.log
    chmod -R 0666 /var/log/nginx/*
    cp -R $CUR_DIR/htdocs/default/* /usr/local/nginx/html
    cp -R $CUR_DIR/htdocs/custom_errorpages/* /usr/local/nginx/html
    #rm -rf $CUR_DIR/htdocs
    cp -R $CUR_DIR/config/nginx/* /usr/local/nginx/conf

    # setup default hostname
    echo ""
    echo "Setup default hostname: $HN"
    sed -i "s/server_name localhost/server_name $HN/g" /usr/local/nginx/conf/conf.d/virtual.conf
    echo ""

    cp $CUR_DIR/init/nginx /etc/init.d/nginx
    chmod +x /etc/init.d/nginx
    chkconfig --levels 235 nginx on

if [ -f /usr/local/nginx/html/phpi.php ]; then
# Randomize phpi.php filename
NPHP=$RANDOM
mv /usr/local/nginx/html/phpi.php /usr/local/nginx/html/${NPHP}_phpi.php
chown nginx:nginx /usr/local/nginx/html/${NPHP}_phpi.php
fi

# rackspace cloud servers block port 80 so need to allow it
iptables -I RH-Firewall-1-INPUT -p tcp --dport 80 -j ACCEPT

if [ ! -f /usr/local/nginx/conf/htpasswd ]; then

touch /usr/local/nginx/conf/htpasswd

fi

cp $CUR_DIR/config/htpasswdgen/htpasswd.py /usr/local/nginx/conf/htpasswd.py

funct_logrotate

	if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
		ngxpagespeedinstall
	fi

    echo "*************************************************"
    cecho "* nginx installed, Apache disabled" $boldgreen
    echo "*************************************************"
fi

}