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
    if [[ "$(grep '^#vhost_traffic_status_dump' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/#vhost_traffic_status_dump/vhost_traffic_status_dump/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^#vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/#vhost_traffic_status on/vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
else
    if [[ "$(grep '^vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_zone/#vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^vhost_traffic_status_dump' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_dump/#vhost_traffic_status_dump/' /usr/local/nginx/conf/vts_http.conf
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

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Install Nginx"
    fi

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
  if [[ "$(rpm -ql httpd >/dev/null 2>&1; echo $?)" = '0' ]]; then
    if [[ "$(systemctl is-enabled httpd.service)" = 'enabled' ]]; then
      systemctl stop httpd.service
      systemctl disable httpd.service
    else
      systemctl disable httpd.service
    fi
  fi
fi # CENTOS_SEVEN = 7

    # Then install nginx
    cd $DIR_TMP

#######################################################
# tcp optimisations
opt_tcp
if [[ ! -f /proc/user_beancounters ]]; then
    sed -i 's/backlog=256/backlog=2048/g' $CM_INSTALLDIR/config/nginx/conf.d/virtual.conf
fi

    # nginx Modules / Prerequisites
  cecho "Installing nginx Modules / Prerequisites..." $boldgreen

install_gperftools

pcredir_check

installopenssl

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "luajit install"
    fi

luajitinstall
nginxzlib_install
nginx_libuuid

    # Install nginx
    cd $DIR_TMP

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Compiling Nginx"
    fi

    echo "Compiling nginx..."
#download nginx tarball

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        MBIT='64'
    else
        MBIT='32'
    fi

    if [[ ! -d "nginx-${NGINX_VERSION}" && -f "${NGX_LINKFILE}" ]]; then
        tar xzf "${NGX_LINKFILE}"
    fi
    cd nginx-${NGINX_VERSION}

# set_intelflags

SET_NGINXVER=$(echo ${NGINX_VERSION} |sed -e 's|\.|0|g' | head -n1)

if [[ "$NGINXPATCH" = [yY] ]]; then

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Patch Nginx"
    fi

    echo "*************************************************"
    cecho "Nginx Patch Time - NGINXPATCH_DELAY seconds delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    patchnginx
fi

funct_nginxconfigure

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Make"
    fi

    # detect OpenSSL 1.1.0 for make -jXX support
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
    DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        time make${MAKETHREADS}
    else
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
            time make${MAKETHREADS}
        else
            time make
        fi
    fi
    sar_call

    if [[ "$STRIPNGINX" = [yY] && "$NGINX_DEBUG" != [yY] ]]; then
        if [ -f objs/nginx ]; then
            echo
            echo "strip nginx binary..."
            ls -lah objs/nginx
            strip -s objs/nginx
            ls -lah objs/nginx
            echo
        fi
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Make Install"
    fi

    time make install
    sar_call

if [[ "$CLANG" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CCACHE_CPP2
    export CC="ccache gcc"
    export CXX="ccache g++"
fi    

# unset_intelflags

    mimefix
    groupadd nginx
    useradd -g nginx -d /home/nginx -s /sbin/nologin nginx
    #chmod 0755 /home/nginx

#######################################################
# Set user nginx hard/soft limits
# check if custom open file descriptor limits already exist
if [[ "$(grep 'nginx hard nofile 524288' /etc/security/limits.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
    echo "nginx soft nofile 524288" >> /etc/security/limits.conf
    echo "nginx hard nofile 524288" >> /etc/security/limits.conf
    echo "nobody soft nofile 524288" >> /etc/security/limits.conf
    echo "nobody hard nofile 524288" >> /etc/security/limits.conf
    echo "root soft nofile 524288" >> /etc/security/limits.conf
    echo "root hard nofile 524288" >> /etc/security/limits.conf
    ulimit -n 524288
fi

    umask 027
    mkdir -p /home/nginx/domains
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
    chmod -R 0660 /var/log/nginx/*
    cp -R $CUR_DIR/htdocs/default/* /usr/local/nginx/html
    cp -R $CUR_DIR/htdocs/custom_errorpages/* /usr/local/nginx/html
    #rm -rf $CUR_DIR/htdocs
    cp -R $CUR_DIR/config/nginx/* /usr/local/nginx/conf

    #################################################################################
    ngx_dynamicfunction
    #################################################################################
    checkmapinital

    # setup default hostname
    echo ""
    echo "Setup default hostname: $HN"
    sed -i "s/server_name localhost/server_name $HN/g" /usr/local/nginx/conf/conf.d/virtual.conf
    echo ""
    reuseportchecks
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

    if [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
        echo
        echo "dynamic module directory at /usr/local/nginx/modules"
        ls -lah /usr/local/nginx/modules
        echo
    fi

    countcpus
    detect_tlsonethree

    # zlibsymlink_fix

    echo "*************************************************"
    cecho "* nginx installed, Apache disabled" $boldgreen
    echo "*************************************************"
   
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Installed"
    fi

fi

}