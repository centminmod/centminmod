fileinfo_standalone() {
  if [[ "$PHPFINFO_STANDALONE" = [yY] ]]; then
    echo
    echo "Compiling fileinfo PHP extension standalone"
    FILEINFO_PHP_VERSION=$(php-config --version)
    cd "${DIR_TMP}/php-${FILEINFO_PHP_VERSION}/ext/fileinfo"
    make clean
    phpize
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --with-php-config=/usr/local/bin/php-config
    make -j$(nproc)
    make install
    if [ -f "$(php-config --extension-dir)/fileinfo.so" ]; then
        echo 'extension=fileinfo.so' > "${CONFIGSCANDIR}/fileinfo.ini"
        fpmrestart
        php --ri fileinfo
        if [[ "$(php --ri fileinfo 2>&1 | grep 'already loaded')" ]]; then
            echo ';extension=fileinfo.so' > "${CONFIGSCANDIR}/fileinfo.ini"
            fpmrestart
            php --ri fileinfo
        fi
        echo "$(php-config --extension-dir)/fileinfo.so installed" 
        echo
    else
        echo "$(php-config --extension-dir)/fileinfo.so not installed" 
        echo
    fi
  fi
}