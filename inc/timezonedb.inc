phptimezonedb_install() {
  if [[ "$PHPTIMEZONEDB" = [yY] ]]; then
    echo
    echo "Compiling timezonedb PHP Extension $PHPTIMEZONEDB_VER"
    echo
    pushd "${DIR_TMP}"
    if [ ! -f "timezonedb-${PHPTIMEZONEDB_VER}.tgz" ]; then
      wget${ipv_forceopt_wget} "https://pecl.php.net/get/timezonedb-${PHPTIMEZONEDB_VER}.tgz"
    fi
    tar xzf "timezonedb-${PHPTIMEZONEDB_VER}.tgz"
    cd "timezonedb-${PHPTIMEZONEDB_VER}"
    make clean
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    make$MAKETHREADS
    # make test
    make install
cat > "${CONFIGSCANDIR}/timezonedb.ini" <<EOF
extension=timezonedb.so
EOF
    # php -r "echo timezone_version_get();"
    popd
  fi
}