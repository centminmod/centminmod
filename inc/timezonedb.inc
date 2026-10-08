phptimezonedb_install() {
  local timezonedb_ext_dir
  if [[ "$PHPTIMEZONEDB" = [yY] ]]; then
    echo
    echo "Compiling timezonedb PHP Extension $PHPTIMEZONEDB_VER"
    echo
    pushd "${DIR_TMP}" || return $?
    if [ ! -f "timezonedb-${PHPTIMEZONEDB_VER}.tgz" ]; then
      wget${ipv_forceopt_wget} "https://pecl.php.net/get/timezonedb-${PHPTIMEZONEDB_VER}.tgz" || return $?
    fi
    tar xzf "timezonedb-${PHPTIMEZONEDB_VER}.tgz" || return $?
    cd "timezonedb-${PHPTIMEZONEDB_VER}" || return $?
    make clean
    /usr/local/bin/phpize || return $?
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config || return $?
    make$MAKETHREADS || return $?
    # make test
    make install || return $?
    timezonedb_ext_dir=$(php-config --extension-dir) || return $?
    [[ "$timezonedb_ext_dir" = /* && -s "$timezonedb_ext_dir/timezonedb.so" ]] || return 1
cat > "${CONFIGSCANDIR}/timezonedb.ini" <<EOF || return $?
extension=timezonedb.so
EOF
    php --ri timezonedb >/dev/null || return $?
    if [[ "$INITIALINSTALL" = [yY] && "$PHP_INSTALL" = [yY] ]]; then
      /usr/local/sbin/php-fpm -t || return $?
      cmservice php-fpm reload || return $?
    fi
    # php -r "echo timezone_version_get();"
    popd || return $?
  fi
}

# timezonedb only refreshes PHP's built-in timezone data, so a failed build
# warns instead of stopping the install or upgrade, and PHP is never left
# loading a missing or broken timezonedb.so.
php_timezonedb_optional() {
  local status dir=$PWD
  phptimezonedb_install && return 0
  status=$?
  cd "$dir" || return $?
  if [[ -f "${CONFIGSCANDIR}/timezonedb.ini" ]] && ! php -m 2>/dev/null | grep -qx timezonedb; then
    rm -f "${CONFIGSCANDIR}/timezonedb.ini"
  fi
  cecho "WARNING: timezonedb PHP extension update failed (status $status); PHP uses its built-in timezone data." $boldyellow
  return 0
}
