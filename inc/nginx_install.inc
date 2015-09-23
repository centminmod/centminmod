checkmapinital() {
VTSHTTP_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)
VTSMAIN_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)

    echo "\${CUR_DIR} & \${CM_INSTALLDIR}"
    echo ${CUR_DIR}
    echo ${CM_INSTALLDIR}    

if [[ -z "$VTSHTTP_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    else
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
else
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/#include \/usr\/local\/nginx\/conf\/vts_http.conf/include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi
    else
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/vts_http.conf/#include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi           
    fi    
fi

if [[ -z "$VTSMAIN_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \ninclude \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    else
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \n#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
fi

if [[ ! -f /usr/local/nginx/conf/vts_http.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_http.conf /usr/local/nginx/conf/vts_http.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_mainserver.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_mainserver.conf /usr/local/nginx/conf/vts_mainserver.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_server.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_server.conf /usr/local/nginx/conf/vts_server.conf
fi

if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
    if [[ "$(grep '^#vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/#vhost_traffic_status_zone/vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^#vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/#vhost_traffic_status on/vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
else
    if [[ "$(grep '^vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_zone/#vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/vhost_traffic_status on/#vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
fi

MAPCHECK=$(grep '/usr/local/nginx/conf/fastcgi_param_https_map.conf' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$MAPCHECK" ]]; then
    sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;/g' /usr/local/nginx/conf/nginx.conf
fi

if [[ ! -f /usr/local/nginx/conf/fastcgi_param_https_map.conf ]]; then
    \cp $CUR_DIR/config/nginx/fastcgi_param_https_map.conf /usr/local/nginx/conf/fastcgi_param_https_map.conf
fi

if [[ -z "$(grep 'fastcgi_param HTTPS $server_https;' /usr/local/nginx/conf/php.conf)" ]]; then
    replace -s '#fastcgi_param HTTPS on;' 'fastcgi_param HTTPS $server_https;' -- /usr/local/nginx/conf/php.conf
fi
}

ngxinstallmain() {

if [[ "$NGINX_INSTALL" = [yY] ]]; then

    echo "*************************************************"
    cecho "* Installing nginx" $boldgreen
    echo "*************************************************"

    # echo "\${CUR_DIR} & \${CM_INSTALLDIR}"
    # echo ${CUR_DIR}
    # echo ${CM_INSTALLDIR}        

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    # Disable Apache if installed
    if [ -f /etc/init.d/httpd ]; then
      service httpd stop
      chkconfig httpd off
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    # Disable Apache if installed
    if [[ "$(systemctl is-enabled httpd.service)" = 'enabled' ]]; then
      systemctl stop httpd.service
      systemctl disable httpd.service
    else
      systemctl disable httpd.service
    fi
fi # CENTOS_SEVEN = 7

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

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited

EOF
fi # raise user process limits

if [[ ! -f /proc/user_beancounters ]]; then
cat >> "/etc/sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8150
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_tw_reuse = 1
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535

net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
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

installopenssl

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

luajitinstall

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

# set_intelflags

SET_NGINXVER=$(echo ${NGINX_VERSION} |sed -e 's|\.|0|g' | head -n1)

if [[ "$NGINXPATCH" = [yY] ]]; then

    echo "*************************************************"
    cecho "Nginx Patch Time - NGINXPATCH_DELAY seconds delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    patchnginx
fi

funct_nginxconfigure

    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        time make${MAKETHREADS}
    else
        time make
    fi

    if [[ "$STRIPNGINX" = [yY] ]]; then
        echo
        echo "strip nginx binary..."
        ls -lah objs/nginx
        strip -s objs/nginx
        ls -lah objs/nginx
        echo
    fi

    time make install

if [[ "$CLANG" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CCACHE_CPP2
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
fi    

# unset_intelflags

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
    echo "nobody soft nofile 262144" >>/etc/security/limits.conf
    echo "nobody hard nofile 262144" >>/etc/security/limits.conf
    echo "root soft nofile 262144" >>/etc/security/limits.conf
    echo "root hard nofile 262144" >>/etc/security/limits.conf
    ulimit -n 262144

fi # check if custom open file descriptor limits already exist
    umask 027
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
    umask 022

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

    checkmapinital

    # setup default hostname
    echo ""
    echo "Setup default hostname: $HN"
    sed -i "s/server_name localhost/server_name $HN/g" /usr/local/nginx/conf/conf.d/virtual.conf
    echo ""

## only run for CentOS 6.x
#if [[ "$CENTOS_SEVEN" != '7' ]]; then
    cp $CUR_DIR/init/nginx /etc/init.d/nginx


# add check for Windows CLRF line endings
if [ ! -f /usr/bin/file ]; then
    yum -q -y install file
fi
if [[ $(file /etc/init.d/nginx) =~ CRLF && -f /etc/init.d/nginx ]]; then
    if [ ! -f /usr/bin/dos2unix ]; then
        yum -q -y install dos2unix
    fi
    echo "detected CRLF line endings converting to Unix LF"
    dos2unix /etc/init.d/nginx
fi

    chmod +x /etc/init.d/nginx
    chkconfig --levels 235 nginx on
#fi # CENTOS_SEVEN != 7

if [ -f /usr/local/nginx/html/phpi.php ]; then
# Randomize phpi.php filename
NPHP=$(od -vAn -N4 -tx < /dev/urandom)
NPHP=$(echo ${NPHP} | sed -e 's/\s//')
mv /usr/local/nginx/html/phpi.php /usr/local/nginx/html/${NPHP}_phpi.php
chown nginx:nginx /usr/local/nginx/html/${NPHP}_phpi.php
fi

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    # rackspace cloud servers block port 80 so need to allow it
    iptables -I RH-Firewall-1-INPUT -p tcp --dport 80 -j ACCEPT
fi # CENTOS_SEVEN != 7

if [ ! -f /usr/local/nginx/conf/htpasswd ]; then

touch /usr/local/nginx/conf/htpasswd

fi

cp $CUR_DIR/config/htpasswdgen/htpasswd.py /usr/local/nginx/conf/htpasswd.py
## grab newer custom written htpasswd.sh as well
gethtpasswdsh

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

geoinccheck
geoipphp

    echo "*************************************************"
    cecho "* nginx installed, Apache disabled" $boldgreen
    echo "*************************************************"
   
fi

}