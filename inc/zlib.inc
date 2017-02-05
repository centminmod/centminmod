zlibng_install() {
  if [[ "$NGINX_ZLIBNG" = [yY] ]]; then
    echo
    echo "install zlib-ng..."
    echo
    pushd "$DIR_TMP"
    if [ -d zlib-ng ]; then
      rm -rf zlib-ng
    fi
    git clone https://github.com/Dead2/zlib-ng
    cd zlib-ng
    ./configure --zlib-compat
    #make${MAKETHREADS}
    # make install
    popd
    echo
    echo "zlib-ng installed"
    echo
  fi
}

nginxzlib_install() {
  if [[ "$NGINX_ZLIBCUSTOM" = [yY] ]]; then
    echo
    echo "install zlib ${NGINX_ZLIBVER}..."
    echo
    pushd "$DIR_TMP"
    cd "zlib-${NGINX_ZLIBVER}"
    ./configure
    make${MAKETHREADS}
    make install
    popd
    echo
    echo "zlib ${NGINX_ZLIBVER} installed"
    echo
  fi
}