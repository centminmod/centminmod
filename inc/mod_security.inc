modsecurity_install() {
  if [[ "$NGINX_MODSECURITY" = [yY] && "$DYNAMIC_SUPPORT" = [yY] ]]; then
    # https://www.nginx.com/blog/compiling-and-installing-modsecurity-for-open-source-nginx/
    # build libmodsecurity
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
      # MODSEC_VER=$(strings /usr/local/modsecurity/lib/libmodsecurity.so | awk '/Apache\// {print $3}' | awk -F "/" '{print $2}')
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
}