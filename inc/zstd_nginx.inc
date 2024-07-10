#######################################################
NGINX_ZSTD_VER='1.5.6'

zstdinstall() {
  if [[ ! -f /usr/local/nginx-dep/bin/zstd || ! -f "${DIR_TMP}/zstd-nginx/zstd-${NGINX_ZSTD_VER}.tar.gz" || "$REBUILD_NGINX_ZSTD" = [yY] ]]; then
    echo
    cecho "nginx zstd dependency install..." $boldyellow
    echo
    mkdir "${DIR_TMP}/zstd-nginx"
    cd "${DIR_TMP}/zstd-nginx"
    if [ -s zstd-${NGINX_ZSTD_VER}.tar.gz ]; then
      echo "zstd-${NGINX_ZSTD_VER}.tar.gz [found]"
      else
      echo "Error: zstd-${NGINX_ZSTD_VER}.tar.gz not found!!!  Downloading now......"
      wget -c${ipv_forceopt} https://github.com/facebook/zstd/archive/v${NGINX_ZSTD_VER}.tar.gz -O zstd-${NGINX_ZSTD_VER}.tar.gz --tries=3
    fi
    tar xvzf zstd-${NGINX_ZSTD_VER}.tar.gz
    cd zstd-${NGINX_ZSTD_VER}
    make clean
    if [ -f /usr/local/nginx-dep/lib/pkgconfig/libzstd.pc ]; then
      rm -f /usr/local/nginx-dep/lib/pkgconfig/libzstd.pc
    fi
    if [[ "$(grep 'lib-mt' Makefile)" ]]; then
      echo
      echo "CFLAGS=\"-fPIC\" make lib-mt -j$(nproc) PREFIX=/usr/local/nginx-dep"
      echo
      CFLAGS="-fPIC" make lib-mt -j$(nproc) PREFIX=/usr/local/nginx-dep
    else
      echo
      echo "CFLAGS=\"-fPIC\" make -j$(nproc) PREFIX=/usr/local/nginx-dep"
      echo
      CFLAGS="-fPIC" make -j$(nproc) PREFIX=/usr/local/nginx-dep
    fi
    make PREFIX=/usr/local/nginx-dep install
    if [ -f /usr/local/nginx-dep/bin/zstd ]; then
      # Export ZSTD_INC and ZSTD_LIB before building zstd
      # export ZSTD_INC="/usr/local/nginx-dep/include"
      # export ZSTD_LIB="/usr/local/nginx-dep/lib"
      cecho "nginx zstd dependency installed" $boldyellow
    fi
  elif [ -f /usr/local/nginx-dep/bin/zstd ]; then
    # Export ZSTD_INC and ZSTD_LIB before building zstd
    # export ZSTD_INC="/usr/local/nginx-dep/include"
    # export ZSTD_LIB="/usr/local/nginx-dep/lib"
    cecho "nginx zstd dependency already installed" $boldyellow
  fi
}


nginx_zstd_setup() {
  if [[ "$NGINX_ZSTD" = [yY] ]]; then
    check_devtoolset_php
    enable_devtoolset
    zstdinstall
    echo "*************************************************"
    cecho "* Installing Nginx ZSTD Module" $boldgreen
    echo "*************************************************"
    cd "$DIR_TMP"
    rm -rf zstd-nginx-module
    git clone https://github.com/tokers/zstd-nginx-module
    cd zstd-nginx-module
    git checkout 0.1.1 -b 0.1.1
    wget -O 23.patch https://patch-diff.githubusercontent.com/raw/tokers/zstd-nginx-module/pull/23.patch
    wget -O fix-module-order.patch https://github.com/tokers/zstd-nginx-module/commit/f4ba115e0b0eaecde545e5f37db6aa18917d8f4b.patch

if [ ! -f /usr/local/nginx/conf/zstd_inc.conf ]; then
cat > "/usr/local/nginx/conf/zstd_inc.conf" <<FFE
zstd on;
zstd_static on;
zstd_min_length 1400;
zstd_buffers 256 8k;
zstd_comp_level 3;
zstd_types text/plain text/css text/xml application/javascript application/x-javascript application/xml application/xml+rss application/ecmascript application/json image/svg+xml;
FFE
fi

ZSTD_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/zstd_inc.conf' /usr/local/nginx/conf/nginx.conf)

    if [[ -z "$ZSTD_INCLUDECHECK" && -f /usr/local/nginx/conf/zstd_inc.conf ]]; then
        sed -i 's/http {/http { \n include \/usr\/local\/nginx\/conf\/zstd_inc.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
    sed -i 's|^ #include \/usr\/local\/nginx\/conf\/zstd_inc.conf;| include \/usr\/local\/nginx\/conf\/zstd_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
  else
    sed -i 's|^ include \/usr\/local\/nginx\/conf\/zstd_inc.conf;| #include \/usr\/local\/nginx\/conf\/zstd_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi
}