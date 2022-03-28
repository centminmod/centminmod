modsecurity_install() {
  if [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
    MODSEC_INSTALLED=y
    # DEVTOOLSETSEVEN='y'
    NGINX_DEVTOOLSETGCC='y'
    CLANG='n'
    # https://www.nginx.com/blog/compiling-and-installing-modsecurity-for-open-source-nginx/
    # build libmodsecurity
    echo
    echo "build libmodsecurity..."
    # https://github.com/SpiderLabs/ModSecurity/wiki/Compilation-recipes

    # force devtoolset gcc usage
    if [[ "$CENTOS_SIX" -eq 6 || "$CENTOS_SEVEN" -eq 7 ]]; then
      if [[ ! -f /opt/rh/devtoolset-11/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ ! -f /opt/rh/devtoolset-10/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-8/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      fi
    fi
    if [[ "$CENTOS_EIGHT" -eq 8 || "$CENTOS_NINE" -eq 9 ]]; then
      if [[ ! -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
        source /opt/rh/gcc-toolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ ! -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
        source /opt/rh/gcc-toolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ ! -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
        source /opt/rh/gcc-toolset-9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache gcc"
          export CXX="ccache g++"
        else
          export CC="gcc"
          export CXX="g++"
        fi
        # export CFLAGS="-Wimplicit-fallthrough=0"
        gcc --version
        g++ --version
      fi
    fi
   
    if [[ ! -f /usr/bin/json_verify && ! -f /usr/include/yajl/yajl_common.h ]]; then
      yum -y install yajl yajl-devel
    fi
    if [ ! -f /usr/bin/json_verify ]; then
      yum -y install yajl
    fi
    if [ ! -f /usr/include/yajl/yajl_common.h ]; then
      yum -y install yajl-devel
    fi
    if [ ! -f /usr/bin/ssdeep ]; then
      yum -y install ssdeep ssdeep-devel
    fi
    if [ ! -f /usr/include/lmdb.h ]; then
      yum -y install lmdb lmdb-devel
    fi
    pushd "$DIR_TMP"

    if [[ ! "$(/usr/local/nginx-dep/bin/mmdblookup --version | awk '/version/ {print $3}' | sed -e 's|\.|00|g')" -lt '1004002' ]] || [ ! -f /usr/local/nginx-dep/bin/mmdblookup ]; then
      install_libmaxmindb
    fi
    
    rm -rf ModSecurity
    git clone --depth 1 -b v3/master --single-branch https://github.com/SpiderLabs/ModSecurity
    cd ModSecurity
    git submodule init
    git submodule update
    ./build.sh
    if [[ -f /usr/local/nginx-dep/lib/libmaxminddb.so && "$NGINX_MODSECURITY_MAXMIND" = [yY] ]]; then
      export PKG_CONFIG_PATH=/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/nginx-dep/lib/pkgconfig
      ./configure --prefix=/usr/local/nginx-dep
      # ./configure --prefix=/usr/local/nginx-dep --with-maxmind=/usr/local/nginx-dep
    elif [[ -f /usr/local/nginx-dep/lib/libmaxminddb.so && "$NGINX_MODSECURITY_MAXMIND" = [nN] ]]; then
      export PKG_CONFIG_PATH=/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/nginx-dep/lib/pkgconfig
      ./configure --prefix=/usr/local/nginx-dep --without-maxmind
    else
      export PKG_CONFIG_PATH=/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/nginx-dep/lib/pkgconfig
      ./configure --prefix=/usr/local/nginx-dep
    fi
    make${MAKETHREADS}
    make install
    popd

    if [[ -f /usr/local/nginx-dep/lib/libmodsecurity.so ]] && [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
        # MODSEC_VER=$(strings /usr/local/nginx-dep/lib/libmodsecurity.so | awk '/Apache\// {print $3}' | awk -F "/" '{print $2}')
        echo
        echo "setup MODSEC_OPT"
        pushd "$DIR_TMP"
        rm -rf ModSecurity-nginx
        git clone --depth 1 https://github.com/SpiderLabs/ModSecurity-nginx.git
        ls -lah "${DIR_TMP}/ModSecurity-nginx"
        if [ -d "${DIR_TMP}/ModSecurity-nginx" ]; then
          MODSEC_OPT=" --add-dynamic-module=../ModSecurity-nginx"
          if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
            echo "load_module \"modules/ngx_http_modsecurity_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
          fi
        else
          MODSEC_OPT=""
        fi
        popd
    else
        MODSEC_OPT=""
    fi
    echo "MODSEC_OPT = $MODSEC_OPT"
  
    if [ ! -f /usr/local/nginx/modsec/modsecurity.conf ]; then
      mkdir -p /usr/local/nginx/modsec
      wget -P /usr/local/nginx/modsec/ https://github.com/SpiderLabs/ModSecurity/raw/v3/master/modsecurity.conf-recommended
      wget -P /usr/local/nginx/modsec/ https://github.com/SpiderLabs/ModSecurity/raw/v3/master/unicode.mapping
      mv /usr/local/nginx/modsec/modsecurity.conf-recommended /usr/local/nginx/modsec/modsecurity.conf
      if [[ "$NGINX_MODSECURITY_JSONLOGS" = [yY] ]]; then
        # switch to ModSecurity JSON formatted logs
        if [ ! "$(grep 'SecAuditLogFormat JSON' /usr/local/nginx/modsec/modsecurity.conf)" ]; then
          sed -i 's|^SecRuleEngine DetectionOnly|SecRuleEngine On\nSecAuditLogFormat JSON|' /usr/local/nginx/modsec/modsecurity.conf
          sed -i 's|^SecRuleEngine On|SecRuleEngine On\nSecAuditLogFormat JSON|' /usr/local/nginx/modsec/modsecurity.conf
        fi
      fi
      # if [[ "$NGINX_MODSECURITY_DETECTONLY" = [nN] ]]; then
      #   sed -i 's/SecRuleEngine DetectionOnly/SecRuleEngine On/' /usr/local/nginx/modsec/modsecurity.conf
      # elif [[ "$NGINX_MODSECURITY_DETECTONLY" = [yY] ]]; then
      #   sed -i 's/SecRuleEngine On/SecRuleEngine DetectionOnly/' /usr/local/nginx/modsec/modsecurity.conf
      # fi
    fi
    if [ ! -f "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/crs-setup.conf" ]; then
      newowaspver='y'
      pushd "$DIR_TMP"
      wget -4 "https://github.com/coreruleset/coreruleset/archive/v${MODSECURITY_OWASPVER}.tar.gz"
      tar xvzf "v${MODSECURITY_OWASPVER}.tar.gz"
      mv "coreruleset-${MODSECURITY_OWASPVER}" /usr/local/nginx
      echo
      ls -lah "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}"
      echo
      cp "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/crs-setup.conf.example" "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/crs-setup.conf"
      cp "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf.example" "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf"
      cp "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf.example" "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf"
      echo
      ls -lah "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/"
      echo
      popd
    fi
    if [[ ! -f /usr/local/nginx/modsec/main.conf || "$newowaspver" = [yY] ]]; then
      if [ -f /usr/local/nginx/modsec/main.conf ]; then
        MODSEC_DT=$(date +"%d%m%y-%H%M%S")
        # backup existing /usr/local/nginx/modsec/main.conf if it exists
        cp -a /usr/local/nginx/modsec/main.conf "/usr/local/nginx/modsec/main.conf-backup-${MODSEC_DT}"
        echo
        echo "detected existing /usr/local/nginx/modsec/main.conf file"
        echo "backed up to /usr/local/nginx/modsec/main.conf-backup-${MODSEC_DT}"
        echo
      fi
cat > "/usr/local/nginx/modsec/main.conf" <<EOF
# Edit to set SecRuleEngine On
Include "/usr/local/nginx/modsec/modsecurity.conf"

# OWASP CRS v3 rules
Include "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/crs-setup.conf"
Include "/usr/local/nginx/coreruleset-${MODSECURITY_OWASPVER}/rules/*.conf"

# Basic test rule
SecRule ARGS:testparam "@contains test" "id:1234,deny,status:403"
EOF
      echo "    modsecurity on;
      modsecurity_rules_file /usr/local/nginx/modsec/main.conf;" > "${CENTMINLOGDIR}/modsecurity_rules_file_syntax_${DT}.log"
      # curl -I localhost?testparam=test
    fi
  
    MODSEC_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/modsec\/main.conf' /usr/local/nginx/conf/conf.d/virtual.conf)
  
    if [[ "$NGINX_MODSECURITY" = [yY] && -z "$MODSEC_INCLUDECHECK" && -f /usr/local/nginx/modsec/main.conf ]]; then
      sed -i 's/# ssi  on;/# ssi  on; \n\n    modsecurity on;\n    modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
    elif [[ "$NGINX_MODSECURITY" = [yY] && "$MODSEC_INCLUDECHECK" && -f /usr/local/nginx/modsec/main.conf ]]; then
      sed -i 's|^    #modsecurity on;|    modsecurity on;|g' /usr/local/nginx/conf/conf.d/virtual.conf
      sed -i 's|^    #modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|    modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|g' /usr/local/nginx/conf/conf.d/virtual.conf
    else
      sed -i 's|^    modsecurity on;|    #modsecurity on;|g' /usr/local/nginx/conf/conf.d/virtual.conf
      sed -i 's|^    modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|    #modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|g' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
    if [[ "$NGINX_MODSECURITY" != [yY] && "$MODSEC_INCLUDECHECK" ]]; then
      sed -i 's|^    modsecurity on;|    #modsecurity on;|g' /usr/local/nginx/conf/conf.d/virtual.conf
      sed -i 's|^    modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|    #modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|g' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
  if [[ "$NGINX_MODSECURITY" = [yY] ]]; then
cat > /etc/logrotate.d/modsecurity <<EOF
/var/log/modsec_audit.log
{
  daily
  dateext
  missingok
  rotate 10
  maxsize 500M
  missingok
  compress
  delaycompress
  notifempty
}
EOF
  fi
  MODSEC_INSTALLED=y
  # DEVTOOLSETSEVEN='y'
  NGINX_DEVTOOLSETGCC='y'
  CLANG='n'
  export CLANG='n'
  echo "ModSecurity Check for CLANG = $CLANG"
  else
    MODSEC_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/modsec\/main.conf' /usr/local/nginx/conf/conf.d/virtual.conf)
    if [[ "$NGINX_MODSECURITY" != [yY] && "$MODSEC_INCLUDECHECK" ]]; then
      sed -i 's|^    modsecurity on;|    #modsecurity on;|g' /usr/local/nginx/conf/conf.d/virtual.conf
      sed -i 's|^    modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|    #modsecurity_rules_file \/usr\/local\/nginx\/modsec\/main.conf;|g' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
  fi
}