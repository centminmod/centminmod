modsecurity_install() {
  if [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
    MODSEC_INSTALLED=y
    DEVTOOLSETSIX='y'
    NGINX_DEVTOOLSETGCC='y'
    CLANG='n'
    # https://www.nginx.com/blog/compiling-and-installing-modsecurity-for-open-source-nginx/
    # build libmodsecurity
    echo
    echo "build libmodsecurity..."
    # https://github.com/SpiderLabs/ModSecurity/wiki/Compilation-recipes

    # force devtoolset-6 gcc 6.2.1+ usage
      if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
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
    pushd "$DIR_TMP"
    rm -rf ModSecurity
    git clone --depth 1 -b v3/master --single-branch https://github.com/SpiderLabs/ModSecurity
    cd ModSecurity
    git submodule init
    git submodule update
    ./build.sh
    ./configure
    make${MAKETHREADS}
    make install
    popd

    if [[ -f /usr/local/modsecurity/lib/libmodsecurity.so ]] && [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
        # MODSEC_VER=$(strings /usr/local/modsecurity/lib/libmodsecurity.so | awk '/Apache\// {print $3}' | awk -F "/" '{print $2}')
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
      # sed -i 's/SecRuleEngine DetectionOnly/SecRuleEngine On/' /usr/local/nginx/modsec/modsecurity.conf
    fi
    if [ ! -f "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/crs-setup.conf" ]; then
      pushd "$DIR_TMP"
      wget "https://github.com/SpiderLabs/owasp-modsecurity-crs/archive/v${MODSECURITY_OWASPVER}.tar.gz"
      tar xvzf "v${MODSECURITY_OWASPVER}.tar.gz"
      mv "owasp-modsecurity-crs-${MODSECURITY_OWASPVER}" /usr/local/nginx
      ls -lah "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}"
      cp "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/crs-setup.conf.example" "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/crs-setup.conf"
      ls -lah "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/rules/"
      popd
    fi
    if [ ! -f /usr/local/nginx/modsec/main.conf ]; then
cat > "/usr/local/nginx/modsec/main.conf" <<EOF
# Edit to set SecRuleEngine On
Include "/usr/local/nginx/modsec/modsecurity.conf"

# OWASP CRS v3 rules
Include "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/crs-setup.conf"
Include "/usr/local/nginx/owasp-modsecurity-crs-${MODSECURITY_OWASPVER}/rules/*.conf"

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
  MODSEC_INSTALLED=y
  DEVTOOLSETSIX='y'
  NGINX_DEVTOOLSETGCC='y'
  CLANG='n'
  export CLANG='n'
  echo "ModSecurity Check for CLANG = $CLANG"
  fi
}