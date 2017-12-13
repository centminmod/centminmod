zlibsymlink_fix() {
    if [[ "$CLOUDFLARE_ZLIB" = [yY] ]]; then
        echo
        # fix symlink
        rm -rf /usr/local/lib/libz.so
        rm -rf /usr/local/lib/libz.so.1
        pushd /usr/local/lib/
        ln -s "libz.so.${CLOUDFLARE_ZLIBVER}" libz.so
        ln -s "libz.so.${CLOUDFLARE_ZLIBVER}" libz.so.1
        popd
        echo
        echo "lsof | grep nginx | egrep -v 'php-fpm|scheduler' | grep libz"
        lsof | grep nginx | egrep -v 'php-fpm|scheduler' | grep libz
        echo
        echo "ls -lahrt /usr/local/lib | grep libz"
        ls -lahrt /usr/local/lib | grep libz
        echo
        echo "ls -lahrt /usr/local/include | grep z"
        ls -lahrt /usr/local/include | grep z
        echo
    else
        # fix symlink
        rm -rf /usr/local/lib/libz.so
        rm -rf /usr/local/lib/libz.so.1
        pushd /usr/local/lib/
        ln -s "libz.so.${NGINX_ZLIBVER}" libz.so
        ln -s "libz.so.${NGINX_ZLIBVER}" libz.so.1
        popd
    fi
}

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
    if [[ "$CLOUDFLARE_ZLIB" = [yY] ]]; then
        echo
        echo "install zlib cloudflare..."
        echo
        pushd "$DIR_TMP"
        if [ ! -d "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}" ]; then
            git clone https://github.com/cloudflare/zlib "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
        elif [ -d "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}/.git" ]; then
            pushd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
            git stash
            git pull
            popd
        fi
        cd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
        sed -i "s|\#define ZLIB_VERSION .*|\#define ZLIB_VERSION \"${CLOUDFLARE_ZLIBVER}\"|" zlib.h
        ldconfig
        make -f Makefile.in distclean
        ./configure
        make${MAKETHREADS}
        make install

        popd
        echo
        echo "zlib cloudflare installed"
        echo
    else
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
  fi
}