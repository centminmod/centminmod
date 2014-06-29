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

LIMITSCONFCHECK=`grep '* hard nofile 262144' /etc/security/limits.conf`

if [[ -z $LIMITSCONFCHECK ]]; then

    # Set VPS hard/soft limits
    echo "* soft nofile 262144" >>/etc/security/limits.conf
    echo "* hard nofile 262144" >>/etc/security/limits.conf
    ulimit -n 262144
    echo "ulimit -n 262144" >> /etc/rc.local
fi # check if custom open file descriptor limits already exist

if [[ -f /etc/security/limits.d/90-nproc.conf ]]; then
cat > "/etc/security/limits.d/90-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     4096
*          hard    nproc     4096
nginx      soft    nproc     16384
nginx      hard    nproc     16384
root       soft    nproc     unlimited
EOF
fi # raise user process limits

if [[ ! -f /proc/user_beancounters ]]; then
cat >> "/etc/sysctl.conf" <<EOF
# centminmod added
net.core.wmem_max=12582912
net.core.rmem_max=12582912
net.ipv4.tcp_rmem= 10240 87380 12582912
net.ipv4.tcp_wmem= 10240 87380 12582912
net.core.netdev_max_backlog=8192
net.core.somaxconn=8150
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=20
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=300
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=0
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_tw_reuse = 1
vm.swappiness=20
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=10000 65500
EOF
sysctl -p

sed -i 's/backlog=256/backlog=2048/g' $CM_INSTALLDIR/config/nginx/conf.d/virtual.conf

fi

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
    else
       installopenssl
	fi
else
    installopenssl
    # echo "NOSOURCEOPENSSL=y"
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

    time make$MAKETHREADS
    time make install
    mimefix
    groupadd nginx
    useradd -g nginx -d /home/nginx -s /sbin/nologin nginx
    #chmod 0755 /home/nginx

#######################################################
# check if custom open file descriptor limits already exist

NGXLIMITSCONFCHECK=`grep 'nginx hard nofile 262144' /etc/security/limits.conf`

if [[ -z $NGXLIMITSCONFCHECK ]]; then

    # Set user nginx hard/soft limits
    echo "nginx soft nofile 262144" >>/etc/security/limits.conf
    echo "nginx hard nofile 262144" >>/etc/security/limits.conf
    ulimit -n 262144

fi # check if custom open file descriptor limits already exist

    mkdir /home/nginx/domains
    #chmod 0771 /home/nginx/domains/
    mkdir -p /home/nginx/domains/demodomain.com/{public,private,log,backup}
    cp -R $CUR_DIR/htdocs/demodomain.com/* /home/nginx/domains/demodomain.com/public
    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/demodomain.com/public
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
    elif [[ "$NGINX_PAGESPEED" = [nN] ]]; then
        PSADMINCHECK=$(grep '^include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;' /usr/local/nginx/conf/nginx.conf)
        
        # disable pagespeedadmin.conf if enabled/uncommented
        if [[ ! -z "$PSADMINCHECK" ]]; then
            sed -i 's/^include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/#include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/g' /usr/local/nginx/conf/nginx.conf
        fi
	fi

    echo "*************************************************"
    cecho "* nginx installed, Apache disabled" $boldgreen
    echo "*************************************************"
fi

}