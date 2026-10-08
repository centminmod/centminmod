fileinfo_standalone() {
  local fileinfo_ext_dir
  if [[ "$PHPFINFO_STANDALONE" = [yY] ]]; then
    echo
    echo "Compiling fileinfo PHP extension standalone"
    FILEINFO_PHP_VERSION=$(php-config --version) || return $?
    cd "${DIR_TMP}/php-${FILEINFO_PHP_VERSION}/ext/fileinfo" || return $?
    make clean
    phpize || return $?
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config || return $?
    make -j$(nproc) || return $?
    make install || return $?
    fileinfo_ext_dir=$(php-config --extension-dir) || return $?
    if [[ "$fileinfo_ext_dir" = /* && -s "$fileinfo_ext_dir/fileinfo.so" ]]; then
        echo 'extension=fileinfo.so' > "${CONFIGSCANDIR}/fileinfo.ini" || return $?
        php --ri fileinfo || return $?
        /usr/local/sbin/php-fpm -t || return $?
        php_fpm_apply restart || return $?
        if [[ "$(php --ri fileinfo 2>&1 | grep 'already loaded')" ]]; then
            echo ';extension=fileinfo.so' > "${CONFIGSCANDIR}/fileinfo.ini" || return $?
            /usr/local/sbin/php-fpm -t || return $?
            php_fpm_apply restart || return $?
            php --ri fileinfo || return $?
        fi
        echo "$fileinfo_ext_dir/fileinfo.so installed"
        echo
    else
        echo "$fileinfo_ext_dir/fileinfo.so not installed"
        echo
        return 1
    fi
  fi
}
