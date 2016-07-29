zlibng_install() {
  if [[ "$NGINX_ZLIBNG" = [yY] ]]; then
    echo
    echo "install zlib-ng..."
    echo
    cd "$DIR_TMP"
    if [ -d zlib-ng ]; then
      rm -rf zlib-ng
    fi
    git clone https://github.com/Dead2/zlib-ng
    cd zlib-ng
    ./configure
    make${MAKETHREADS}
    make install
    echo
    echo "zlib-ng installed"
    echo
  fi
}