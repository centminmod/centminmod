# Desired UID and GID
DESIRED_UID=1068
DESIRED_GID=1068

# Function to check if a UID or GID is available
id_available() {
  ! getent passwd "$1" &>/dev/null && ! getent group "$1" &>/dev/null
}

# Function to find the next available UID/GID
find_next_available_id() {
  local start_id=$1
  local current_id=$start_id
  while ! id_available "$current_id"; do
    ((current_id++))
  done
  echo "$current_id"
}

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

if [[ "$NGINX_INSTALL" = [yY] || "$FREENGINX_INSTALL" = [yY] ]]; then

    if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
    ipv_forceopt=""
    ipv_forceopt_wget=""
    WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
    else
    ipv_forceopt='4'
    ipv_forceopt_wget=' -4'
    WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Install Nginx"
    fi

    echo "*************************************************"
    cecho "* Installing nginx" $boldgreen
    echo "*************************************************"

    # echo "\${CUR_DIR} & \${CM_INSTALLDIR}"
    # echo ${CUR_DIR}
    # echo ${CM_INSTALLDIR}

if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    NGXVER_BRANCH=$(echo $FREENGINX_VERSION | cut -d . -f1,2 | sed -e 's|\.|00|g')
elif [[ "$NGINX_INSTALL" = [yY] ]]; then
    NGXVER_BRANCH=$(echo $NGINX_VERSION | cut -d . -f1,2 | sed -e 's|\.|00|g')
fi
if [[ "$NGXVER_BRANCH" -ge '10023' ]] || [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    NGINX_PAGESPEED='n'
    NGXDYNAMIC_NGXPAGESPEED='n'
fi

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

    # nginx Modules / Prerequisites
  cecho "Installing nginx Modules / Prerequisites..." $boldgreen

# pcre2 initial install needs newer GCC
if [[ "$INITIALINSTALL" = [yY] ]] && [[ "$NGINX_PCRE_TWO" = [yY] || "$NGX_LDMOLD" = [yY] ]]; then
    GENERAL_DEVTOOLSETGCC='y'
    ngx_starttime=$(TZ=UTC date +%s.%N)
    enable_devtoolset
    ngx_endtime=$(TZ=UTC date +%s.%N)
    enable_devtoolset_install_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: enable devtoolset completed in $enable_devtoolset_install_time seconds"
    echo
fi

install_gperftools

pcredir_check
perl_ipc_cmd_install
    ngx_starttime=$(TZ=UTC date +%s.%N)

echo "nginx install: recompileopenssl=$recompileopenssl"
echo "nginx install: LIBRESSL_SWITCH=$LIBRESSL_SWITCH"
echo "nginx install: BORINGSSL_SWITCH=$BORINGSSL_SWITCH"
echo "nginx install: OPENSSL_SYSTEM_USE=$OPENSSL_SYSTEM_USE"

if [[ "$recompileopenssl" = [yY] || "$LIBRESSL_SWITCH" = [yY] || "$BORINGSSL_SWITCH" = [yY] ]] && [[ "$OPENSSL_SYSTEM_USE" != [yY] ]]; then
    installopenssl
elif [[ "$recompileopenssl" = [yY] || "$LIBRESSL_SWITCH" = [yY] || "$BORINGSSL_SWITCH" = [yY] ]] && [[ "$OPENSSL_SYSTEM_USE" = [yY] ]]; then
    installopenssl
elif [[ "$OPENSSL_SYSTEM_USE" = [nN] ]]; then
    installopenssl
fi
    ngx_endtime=$(TZ=UTC date +%s.%N)
    install_openssl_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: install openssl completed in $install_openssl_time seconds"
    echo

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "luajit install"
    fi

    ngx_starttime=$(TZ=UTC date +%s.%N)
    luajitinstall
    ngx_endtime=$(TZ=UTC date +%s.%N)
    luajit_install_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: luajit install completed in $luajit_install_time seconds"
    echo

{
    ngx_starttime=$(TZ=UTC date +%s.%N)
    nginxzlib_install
    ngx_endtime=$(TZ=UTC date +%s.%N)
    nginx_zlib_install_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: nginx zlib install completed in $nginx_zlib_install_time seconds"
    echo
} 2>&1 | tee "${CENTMINLOGDIR}/nginx-install-zlib_${DT}.log"
nginx_libuuid

    # Install nginx
    cd $DIR_TMP
if [[ "$FPM_NGINX_RPM" != [yY] ]]; then
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

    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
        if [[ ! -d "freenginx-${FREENGINX_VERSION}" && -f "${FREENGX_LINKFILE}" ]]; then
            tar xzf "${FREENGX_LINKFILE}"
        fi
        cd freenginx-${FREENGINX_VERSION}
    elif [[ "$NGINX_INSTALL" = [yY] ]]; then
        if [[ ! -d "nginx-${NGINX_VERSION}" && -f "${NGX_LINKFILE}" ]]; then
            tar xzf "${NGX_LINKFILE}"
        fi
        cd nginx-${NGINX_VERSION}
    fi
fi # FPM_NGINX_RPM

# set_intelflags
if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    SET_NGINXVER=$(echo ${FREENGINX_VERSION} |sed -e 's|\.|0|g' | head -n1)
elif [[ "$NGINX_INSTALL" = [yY] ]]; then
    SET_NGINXVER=$(echo ${NGINX_VERSION} |sed -e 's|\.|0|g' | head -n1)
fi

if [[ "$NGINXPATCH" = [yY] && "$FPM_NGINX_RPM" != [yY] ]]; then

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Patch Nginx"
    fi

    echo "*************************************************"
    cecho "Nginx Patch Time - NGINXPATCH_DELAY seconds delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    ngx_starttime=$(TZ=UTC date +%s.%N)
    patchnginx
    ngx_endtime=$(TZ=UTC date +%s.%N)
    ngx_patch=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: nginx patch completed in $ngx_patch seconds"
    echo
fi

    ngx_starttime=$(TZ=UTC date +%s.%N)
    funct_nginxconfigure
    ngx_endtime=$(TZ=UTC date +%s.%N)
    ngx_configure_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: nginx configure completed in $ngx_configure_time seconds"
    echo

if [[ "$FPM_NGINX_RPM" = [yY] && "$INITIALINSTALL" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
  # experimental initial install Nginx custom RPM build
  echo
  echo "Skip Nginx make & make install stage..."
  echo "FPM_NGINX_RPM=$FPM_NGINX_RPM"
  echo
  # experimental initial install Nginx custom RPM build
  echo
  echo "install Nginx custom RPM"
  echo
  wget${ipv_forceopt_wget} https://github.com/centminmod/centminmod-nginx-rpm/blob/master/rpms/nginx-custom-1.21.6-1.el7.x86_64.rpm?raw=true -O "$DIR_TMP/nginx-custom-1.21.6-1.el7.x86_64.rpm"
  yum -y localinstall "$DIR_TMP/nginx-custom-1.21.6-1.el7.x86_64.rpm"
  echo
else
    ngx_starttime=$(TZ=UTC date +%s.%N)
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Make"
    fi

    # detect OpenSSL 1.1.0 for make -jXX support
    echo "nginx install: initial OPENSSL_VERSION=$OPENSSL_VERSION"
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
    DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        echo "time make${MAKETHREADS}"
        time make${MAKETHREADS}
    else
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
            echo "time make${MAKETHREADS}"
            time make${MAKETHREADS}
        else
            echo "time make"
            time make
        fi
    fi
    ngx_endtime=$(TZ=UTC date +%s.%N)
    ngx_make_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: nginx make completed in $ngx_make_time seconds"
    echo
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
    ngx_make_starttime=$(TZ=UTC date +%s.%N)
    time make install
    ngx_make_endtime=$(TZ=UTC date +%s.%N)
    ngx_make_install_time=$(echo "scale=2;$ngx_make_endtime - $ngx_make_starttime"|bc )
    echo
    echo "nginx install: nginx make install completed in $ngx_make_install_time seconds"
    echo
fi # FPM_NGINX_RPM
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

    # Determine the UID and GID to use
    if id_available "$DESIRED_UID"; then
      NGINX_UID=$DESIRED_UID
      NGINX_GID=$DESIRED_GID
    else
      echo "Desired UID/GID $DESIRED_UID is already in use. Finding next available ID."
      NGINX_UID=$(find_next_available_id $((DESIRED_UID + 1)))
      NGINX_GID=$NGINX_UID
    fi

    echo "Using UID/GID: $NGINX_UID"

    if [ ! "$(grep -w 'nginx' /etc/passwd)" ]; then
      echo "Creating nginx group and user with UID $NGINX_UID and GID $NGINX_GID"
      groupadd -g "$NGINX_GID" nginx
      useradd -u "$NGINX_UID" -g "$NGINX_GID" -d /home/nginx -s /sbin/nologin nginx
    else
      echo "nginx user already exists. Ensuring it has UID $NGINX_UID and GID $NGINX_GID"
      CURRENT_UID=$(id -u nginx)
      CURRENT_GID=$(id -g nginx)
      
      if [ "$CURRENT_UID" -ne "$NGINX_UID" ]; then
        usermod -u "$NGINX_UID" nginx >/dev/null
        echo "Updated nginx UID from $CURRENT_UID to $NGINX_UID"
      fi
      
      if [ "$CURRENT_GID" -ne "$NGINX_GID" ]; then
        groupmod -g "$NGINX_GID" nginx
        usermod -g "$NGINX_GID" nginx >/dev/null
        echo "Updated nginx GID from $CURRENT_GID to $NGINX_GID"
      fi
    fi

    # Verify the changes
    echo "Verifying nginx user details:"
    id nginx

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
    mkdir -p /usr/local/nginx/conf/ssl
    fpmconfdir
    mkdir -p /var/log/nginx
    touch /var/log/nginx/localhost.access.log
    touch /var/log/nginx/localhost.error.log
    chmod -R 0660 /var/log/nginx/*
    cp -R $CUR_DIR/htdocs/default/* /usr/local/nginx/html
    cp -R $CUR_DIR/htdocs/custom_errorpages/* /usr/local/nginx/html
    #rm -rf $CUR_DIR/htdocs
    cp -R $CUR_DIR/config/nginx/* /usr/local/nginx/conf
    opt_sslcache silent
    if [[ "$NGINX_MAINHOSTNAME_BLANK_INDEX" = [yY] ]]; then
        cp -a /usr/local/nginx/html/index.html /usr/local/nginx/html/index.html-orig
        rm -f /usr/local/nginx/html/index.html
        touch /usr/local/nginx/html/index.html
    fi

    #################################################################################
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
        ngx_dynamicfunction "$FREENGINX_VERSION" "$NGINX_HTTP2"
    elif [[ "$NGINX_INSTALL" = [yY] ]]; then
        ngx_dynamicfunction "$NGINX_VERSION" "$NGINX_HTTP2"
    fi
    #################################################################################
    checkmapinital

    # setup default hostname
    echo ""
    echo "Setup default hostname: $HN"
    sed -i "s/server_name localhost/server_name $HN/g" /usr/local/nginx/conf/conf.d/virtual.conf
    echo ""
    reuseportchecks
    echo ""

## only run for CentOS 6.x & CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_SIX" = '6' ]] && [[ "$SWITCH_NGINX_SYSTEMD" != [yY] ]]; then
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
fi # CENTOS_SEVEN = 7

if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]] || [[ "$SWITCH_NGINX_SYSTEMD" = [yY] ]]; then
    if [ ! -f /usr/bin/file ]; then
        yum -q -y install file
    fi
    if [ ! -f /usr/bin/dos2unix ]; then
        yum -q -y install dos2unix
    fi
    # CENTOS_EIGHT = 8 nginx systemd service file
    mkdir -p /etc/systemd/system/nginx.service.d/
    if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ ! -f /etc/systemd/system/nginx.service.d/openfileslimit.conf ]]; then
        echo "Initial Install: Update nginx for LimitNOFILE=1048576"
cat > "/etc/systemd/system/nginx.service.d/openfileslimit.conf" <<TDG
[Service]
LimitNOFILE=1048576
TDG
        systemctl daemon-reload
    fi
    if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' ]] && [[ ! -f /etc/systemd/system/nginx.service.d/failure-restart.conf ]]; then
        echo "setup /etc/systemd/system/nginx.service.d/failure-restart.conf"
cat > "/etc/systemd/system/nginx.service.d/failure-restart.conf" <<TDG
[Unit]
StartLimitIntervalSec=30
StartLimitBurst=5

[Service]
Restart=on-failure
RestartSec=5s
TDG
        systemctl daemon-reload
    elif [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/systemd/system/nginx.service.d/failure-restart.conf ]] || [[ "$CENTOS_SEVEN" = '7' && "$(grep -o Unit /etc/systemd/system/nginx.service.d/failure-restart.conf)" = 'Unit' ]]; then
        echo "setup /etc/systemd/system/nginx.service.d/failure-restart.conf"
cat > "/etc/systemd/system/nginx.service.d/failure-restart.conf" <<TDG
[Service]
StartLimitInterval=30
StartLimitBurst=5
Restart=on-failure
RestartSec=5s
TDG
        systemctl daemon-reload
    fi

cat > /usr/lib/systemd/system/nginx.service <<EOF
[Unit]
Description=Centmin Mod NGINX Server
After=syslog.target network-online.target remote-fs.target nss-lookup.target
Wants=network-online.target

[Service]
Type=forking
PIDFile=/usr/local/nginx/logs/nginx.pid
ExecStartPre=/usr/local/sbin/nginx -t
ExecStart=/usr/local/sbin/nginx -c /usr/local/nginx/conf/nginx.conf
ExecReload=/bin/sh -c "/bin/kill -s HUP \$(/bin/cat /usr/local/nginx/logs/nginx.pid)"
ExecStop=/bin/sh -c "/bin/kill -s TERM \$(/bin/cat /usr/local/nginx/logs/nginx.pid)"

[Install]
WantedBy=multi-user.target
EOF
fi # CENTOS_EIGHT = 8 nginx systemd service file

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

    ngx_starttime=$(TZ=UTC date +%s.%N)
    countcpus
    detect_tlsonethree
    run_after_nginx_install
    ngx_endtime=$(TZ=UTC date +%s.%N)
    run_after_nginx_install_time=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
    echo
    echo "nginx install: countcpus, detect_tlsonethree & run_after_nginx_install functions completed in $run_after_nginx_install_time seconds"
    echo

    # zlibsymlink_fix
    if [[ "$NGINX_NJS" = [yY] && -f "$DIR_TMP/njs/build/njs" ]]; then
        cp -af "$DIR_TMP/njs/build/njs" /usr/local/bin/njs
    fi
    if [[ "$NGINX_SECURED" = [yY] ]]; then
        ngx_starttime=$(TZ=UTC date +%s.%N)
        echo
        checksec_nginx
        echo
        echo "checksec --format=json --file=/usr/local/sbin/nginx | jq -r"
        checksec --format=json --file=/usr/local/sbin/nginx | jq -r
        echo
        ngx_endtime=$(TZ=UTC date +%s.%N)
        check_sec_timed=$(echo "scale=2;$ngx_endtime - $ngx_starttime"|bc )
        echo
        echo "nginx install: checksec_nginx function completed in $check_sec_timed seconds"
        echo
    fi
    if [[ "$ORESTY_LUANGINX" = [yY] && "$INITIALINSTALL" = [yY] ]]; then
        echo
        echo "setup initial install lua_package_path include file"
        echo "lua_package_path '/usr/local/nginx-dep/lib/lua/?.lua;/usr/local/nginx-dep/lib/lua/5.1/?.lua;/usr/local/nginx-dep/lib/lua/ngx/?.lua;/usr/local/nginx-dep/lib/lua/resty/?.lua;;';" > /usr/local/nginx/conf/lua-package-path-include.conf
        LUAPACKAGEPATH_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/lua-package-path-include.conf' /usr/local/nginx/conf/nginx.conf)
        if [[ -z "$LUAPACKAGEPATH_INCLUDECHECK" ]]; then
            sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/lua-package-path-include.conf;|g' /usr/local/nginx/conf/nginx.conf
        fi
    fi

    echo "nginx install: make time completed in $ngx_make_time seconds"
    echo
    echo "*************************************************"
    cecho "* nginx installed, Apache disabled" $boldgreen
    echo "*************************************************"
   
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Installed"
    fi

fi

}