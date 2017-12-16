zlibsymlink_fix() {
    if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' ]]; then
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
    if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' ]]; then
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
        # cd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
        # sed -i "s|\#define ZLIB_VERSION .*|\#define ZLIB_VERSION \"${CLOUDFLARE_ZLIBVER}\"|" zlib.h
        # ldconfig
        # make -f Makefile.in distclean
        # # ./configure --prefix=/usr/local/zlib-cf
        # ./configure
        # make${MAKETHREADS}
        # ps aufxwww > zlib-process.log
        # if [[ "$CLOUDFLARE_ZLIBDEBUG" = [Yy] ]]; then
        #     make -d install
        # else
        #     make install
        # fi
        if [ -f /usr/local/lib/libz.so ]; then
            pushd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
            make -d uninstall
            listservices=$(lsof | grep 'libz.so' | grep 'DEL.*lib' | cut -f 1 -d ' ' | sort -u)
            for i in ${listservices[@]}; do
              if [ -d /usr/lib/systemd/system ]; then
                servicepath='/usr/lib/systemd/system/'
                systemd_yes=y
              else
                servicepath='/etc/init.d/'
              fi
              echo $i; 
              if [ "$(grep -r "$i" "$servicepath")" ]; then
                echo
                echo "restarting service due to zlib changes"
                if [[ "$systemd_yes" = 'y' && "$i" = 'mysqld' ]]; then
                    echo "mysqladmin flush-tables"
                    mysqladmin flush-tables
                    sleep 10s
                    service mysql status;
                    service mysql restart;
                elif [[ "$i" = 'pickup' || "$i" = 'qmgr' ]]; then
                    service postfix status;
                    service postfix restart;
                elif [[ "$i" = 'memcached' ]]; then
                    memcachedrestart
                elif [[ "$i" = 'rsyslogd' ]]; then
                    service rsyslog status;
                    service rsyslog restart;
                else
                    service $i status;
                    service $i restart;
                fi
              fi 
            done
            if [ -d /usr/lib/systemd/system ]; then
              servicepath='/usr/lib/systemd/system/'
              systemd_yes=y
            else
              servicepath='/etc/init.d/'
            fi
            if [[ "$systemd_yes" = 'y' ]]; then
                if [ -f /usr/lib/systemd/system/php56-php-fpm.service ]; then
                    systemctl restart php56-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php70-php-fpm.service ]; then
                    systemctl restart php70-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php71-php-fpm.service ]; then
                    systemctl restart php71-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php72-php-fpm.service ]; then
                    systemctl restart php72-php-fpm.service
                fi
                systemctl restart polkit
                systemctl restart systemd-udevd
                systemctl daemon-reload
                systemctl daemon-reexec
            fi
            popd
        fi

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
        # pushd "$DIR_TMP"
        # cd "zlib-${NGINX_ZLIBVER}"
        # make clean
        # ./configure
        # make${MAKETHREADS}
        # make install
        if [ -f /usr/local/lib/libz.so ]; then
            pushd "zlib-${NGINX_ZLIBVER}"
            make -d uninstall
            listservices=$(lsof | grep 'libz.so' | grep 'DEL.*lib' | cut -f 1 -d ' ' | sort -u)
            for i in ${listservices[@]}; do
              if [ -d /usr/lib/systemd/system ]; then
                servicepath='/usr/lib/systemd/system/'
                systemd_yes=y
              else
                servicepath='/etc/init.d/'
              fi
              echo $i; 
              if [ "$(grep -r "$i" "$servicepath")" ]; then
                echo
                echo "restarting service due to zlib changes"
                if [[ "$systemd_yes" != 'y' && "$i" = 'mysqld' ]]; then
                    echo "mysqladmin flush-tables"
                    mysqladmin flush-tables
                    sleep 10s
                    service mysql status;
                    service mysql restart;
                elif [[ "$i" = 'pickup' || "$i" = 'qmgr' ]]; then
                    service postfix status;
                    service postfix restart;
                elif [[ "$i" = 'memcached' ]]; then
                    memcachedrestart
                elif [[ "$i" = 'rsyslogd' ]]; then
                    service rsyslog status;
                    service rsyslog restart;
                else
                    service $i status;
                    service $i restart;
                fi
              fi 
            done
            if [ -d /usr/lib/systemd/system ]; then
              servicepath='/usr/lib/systemd/system/'
              systemd_yes=y
            else
              servicepath='/etc/init.d/'
            fi
            if [[ "$systemd_yes" = 'y' ]]; then
                if [ -f /usr/lib/systemd/system/php56-php-fpm.service ]; then
                    systemctl restart php56-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php70-php-fpm.service ]; then
                    systemctl restart php70-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php71-php-fpm.service ]; then
                    systemctl restart php71-php-fpm.service
                fi
                if [ -f /usr/lib/systemd/system/php72-php-fpm.service ]; then
                    systemctl restart php72-php-fpm.service
                fi
                systemctl restart polkit
                systemctl restart systemd-udevd
                systemctl daemon-reload
                systemctl daemon-reexec
            fi
            popd
        fi

        popd
        echo
        echo "zlib ${NGINX_ZLIBVER} installed"
        echo
    fi
  fi
}