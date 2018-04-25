zlib_ccache() {
    if [[ -d "${DIR_TMP}/ccache-${CCACHE_VER}/" ]]; then
        export CC="gcc"
        export CXX="g++"
        pushd "${DIR_TMP}/ccache-${CCACHE_VER}/"
        make clean
        ./configure
        make${MAKETHREADS}
        make install
        popd
    fi
}

zlibsymlink_fix() {
    if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
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

install_cfzlib() {
    if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
        # disable clang compiler and switch to gcc compiler due to clang 3.4.2 errors when enabling
        # cloudflare zlib
        if [[ "$CLANG" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
            CLANG='y'
            CLANG_FOUR='y'
        elif [[ "$CLANG" = [yY] && "$CENTOS_SIX" -eq '6' ]]; then
            CLANG='n'
            DEVTOOLSETSIX='y'
        else
            CLANG='n'
            DEVTOOLSETSIX='y'
        fi
        install_cfzlibstartdir=$(pwd)
        echo
        echo "install zlib cloudflare..."
        echo
        echo "pwd"
        pwd
        pushd "$DIR_TMP"
        if [ ! -d "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}" ]; then
            git clone https://github.com/cloudflare/zlib "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
        elif [ -d "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}/.git" ]; then
            pushd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
            git stash
            git pull
            popd
        fi
        pushd "zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
        # sed -i "s|\#define ZLIB_VERSION .*|\#define ZLIB_VERSION \"${CLOUDFLARE_ZLIBVER}\"|" zlib.h
        # ldconfig
        make -f Makefile.in distclean
        ./configure --prefix=/usr/local/zlib-cf
        # ./configure --prefix=/usr/local/zlib-cf --static
        make${MAKETHREADS}
        # ps aufxwww > zlib-process.log
        if [[ "$CLOUDFLARE_ZLIBDEBUG" = [Yy] ]]; then
            make -d install
            cfzlib_check=$?
            if [[ "$(uname -m)" = 'x86_64' ]]; then
                ln -sf /usr/local/zlib-cf/lib /usr/local/zlib-cf/lib64
            fi
        else
            make install
            cfzlib_check=$?
            if [[ "$(uname -m)" = 'x86_64' ]]; then
                ln -sf /usr/local/zlib-cf/lib /usr/local/zlib-cf/lib64
            fi
        fi
        popd
        cd $install_cfzlibstartdir
        if [ -f /usr/local/lib/libz.so ]; then
            echo
            ps aufxw
            echo
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
                elif [[ "$i" = 'lvmetad' ]]; then
                    service lvm2-lvmetad status
                    service lvm2-lvmetad restart
                elif [[ "$i" = 'dockerd' ]]; then
                    service docker status;
                    service docker restart;
                elif [[ "$i" = 'fail2ban-' ]]; then
                    service fail2ban status;
                    service fail2ban restart;
                elif [[ "$i" = 'nrsysmond' ]]; then
                    service newrelic-sysmond status;
                    service newrelic-sysmond restart;
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
            if [ "$(ps -C pickup | grep -o pickup)" ]; then
                service postfix status;
                service postfix restart;
            fi
            if [ "$(ps -C memcached | grep -o memcached)" ]; then
                memcachedrestart
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
    fi
}

install_stdzlib() {
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
            echo
            ps aufxw
            echo
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
                elif [[ "$i" = 'lvmetad' ]]; then
                    service lvm2-lvmetad status
                    service lvm2-lvmetad restart
                elif [[ "$i" = 'dockerd' ]]; then
                    service docker status;
                    service docker restart;
                elif [[ "$i" = 'fail2ban-' ]]; then
                    service fail2ban status;
                    service fail2ban restart;
                elif [[ "$i" = 'nrsysmond' ]]; then
                    service newrelic-sysmond status;
                    service newrelic-sysmond restart;
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
            if [ "$(ps -C pickup | grep -o pickup)" ]; then
                service postfix status;
                service postfix restart;
            fi
            if [ "$(ps -C memcached | grep -o memcached)" ]; then
                memcachedrestart
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
        if [[ "$(ldd $(which ccache) | grep '/usr/local/lib/libz.so.1')" ]]; then
            zlib_ccache
        fi

        popd
        echo
        echo "zlib ${NGINX_ZLIBVER} installed"
        echo
}

nginxzlib_install() {
  if [[ "$NGINX_ZLIBCUSTOM" = [yY] ]]; then
    CHECK_PCLMUL=$(gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | grep 'mpclmul' | grep -o enabled)
    if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
        install_cfzlib
        if [[ "$cfzlib_check" -ne '0' ]]; then
            CLOUDFLARE_ZLIB='n'
            install_stdzlib
        fi
    else
        install_stdzlib
    fi
  fi
}