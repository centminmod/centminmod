modsecurity_install() {
  if [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
    # https://www.nginx.com/blog/compiling-and-installing-modsecurity-for-open-source-nginx/
    # build libmodsecurity
    echo
    echo "build libmodsecurity..."
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
  fi

  if [[ -f /usr/local/modsecurity/lib/libmodsecurity.so ]] && [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
      MODSEC_INSTALLED=y
      DEVTOOLSETSIX='y'
      NGINX_DEVTOOLSETGCC='y'
      CLANG='n'
      # MODSEC_VER=$(strings /usr/local/modsecurity/lib/libmodsecurity.so | awk '/Apache\// {print $3}' | awk -F "/" '{print $2}')
      echo
      echo "setup MODSEC_OPT"
      pushd "$DIR_TMP"
      rm -rf ModSecurity-nginx
      git clone --depth 1 https://github.com/SpiderLabs/ModSecurity-nginx.git
      if [[ -d "${DIR_TMP}/ModSecurity-nginx" ]]; then
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

  if [ ! -f /usr/local/nginx/modsec/modsecurity.conf ]; then
    mkdir -p /usr/local/nginx/modsec
    wget -P /usr/local/nginx/modsec/ https://raw.githubusercontent.com/SpiderLabs/ModSecurity/master/modsecurity.conf-recommended
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

}