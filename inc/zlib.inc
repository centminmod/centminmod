zlibng_install() {
  if [[ "$NGINX_ZLIBNG" = [yY] ]]; then
    echo
    echo "install zlib-ng..."
    echo
    pushd "$DIR_TMP"
    if [ -d zlib-ng ]; then
      rm -rf zlib-ng
    fi
    time git clone https://github.com/Dead2/zlib-ng
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
    if [[ ! -f "${DIR_TMP}/${NGX_ZLIBLINKFILE}" || ! -d "${DIR_TMP}/zlib-${NGINX_ZLIBVER}" ]]; then
        nginxzlibtarball
    fi
    pushd "$DIR_TMP"
    cd "zlib-${NGINX_ZLIBVER}"
    make clean
    ./configure
    make${MAKETHREADS}
    make install
    popd
    echo
    echo "zlib ${NGINX_ZLIBVER} installed"
    echo
  fi
}