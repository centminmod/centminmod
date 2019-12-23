php_ext_brotli() {
  if [[ "$PHP_BROTLI" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing brotli PHP extension" $boldgreen
    echo "*************************************************"
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
    echo "Downloading brotli PHP extension..."
    pushd "$DIR_TMP"
    rm -rf php-ext-brotli
    git clone --recursive --depth=1 https://github.com/kjdev/php-ext-brotli.git
    cd php-ext-brotli
    phpize
    ./configure
    time make -j$(nproc)
    time make install
    echo "extension=brotli.so" > "${CONFIGSCANDIR}/brotli.ini"
    echo
    cmservice php-fpm reload
    echo
    php --ri brotli
    echo
    popd
  fi
}

php_ext_lzfour() {
  if [[ "$PHP_LZFOUR" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing lz4 PHP extension" $boldgreen
    echo "*************************************************"
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
    echo "Downloading lz4 PHP extension..."
    pushd "$DIR_TMP"
    rm -rf lz4 
    git clone --depth=1 https://github.com/lz4/lz4
    cd lz4
    make clean
    time make -j$(nproc)
    time make install
    
    pushd "$DIR_TMP"
    rm -rf php-ext-lz4
    git clone --recursive --depth=1 https://github.com/kjdev/php-ext-lz4.git
    cd php-ext-lz4
    make clean
    phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    time make -j$(nproc)
    time make install
    echo "extension=lz4.so" > "${CONFIGSCANDIR}/lz4.ini"
    echo
    cmservice php-fpm reload
    echo
    php --ri lz4
    echo
    popd
  fi
}

php_ext_lzf() {
  if [[ "$PHP_LZF" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing lzf PHP extension" $boldgreen
    echo "*************************************************"
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
    echo "Downloading lzf PHP extension..."
    if [ ! -f /usr/bin/lzf ]; then
      yum -y -q install liblzf
    fi
    pushd "$DIR_TMP"
    rm -rf php-ext-lzf
    git clone  --recursive --depth=1 https://github.com/php/pecl-file_formats-lzf php-ext-lzf
    cd php-ext-lzf
    make clean
    phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    time make -j$(nproc)
    time make install
    echo "extension=lzf.so" > "${CONFIGSCANDIR}/lzf.ini"
    echo
    cmservice php-fpm reload
    echo
    php --ri lz4
    echo
    popd
  fi
}

php_ext_zopfli() {
  if [[ "$PHP_ZOPFLI" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing Zopfli PHP extension" $boldgreen
    echo "*************************************************"
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
    echo "Downloading Zopfli PHP extension..."
    if [ ! -f /usr/bin/zopfli ]; then
      yum -y -q install zopfli
    fi
    pushd "$DIR_TMP"
    rm -rf php-ext-zopfli
    git clone --depth=1 https://github.com/kjdev/php-ext-zopfli
    cd php-ext-zopfli
    git submodule update --init
    phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    time make -j$(nproc)
    time make install
    echo "extension=zopfli.so" > "${CONFIGSCANDIR}/zopfli.ini"
    echo
    cmservice php-fpm reload
    echo
    php --ri zopfli
    echo
    popd
  fi
}

php_ext_zstd() {
  if [[ "$PHP_ZSTD" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing zstd PHP extension" $boldgreen
    echo "*************************************************"
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset
    echo "Downloading zstd PHP extension..."
    zstdinstall
    pushd "$DIR_TMP"
    rm -rf php-ext-zstd
    git clone --recursive --depth=1 https://github.com/kjdev/php-ext-zstd.git
    cd php-ext-zstd
    phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    time make -j$(nproc)
    time make install
    echo "extension=zstd.so" > "${CONFIGSCANDIR}/zstd.ini"
    echo
    cmservice php-fpm reload
    echo
    php --ri zstd
    echo
    popd
  fi
}