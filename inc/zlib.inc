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
    CLOUDFLARE_ZLIB='n'
    NGINX_ZLIBCUSTOM='n'

    if [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-7/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-8/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/gcc8/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/gcc9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    echo
    echo "install zlib-ng..."
    echo
    pushd "$DIR_TMP"
    if [ -d zlib-ng ]; then
      rm -rf zlib-ng
    fi
    time git clone https://github.com/zlib-ng/zlib-ng
    cd zlib-ng
    ./configure --zlib-compat --prefix=/usr/local/zlib-ng
    make${MAKETHREADS}
    make -d install
    zlibng_check=$?
    if [[ "$(uname -m)" = 'x86_64' ]]; then
        ln -sf /usr/local/zlib-ng/lib /usr/local/zlib-ng/lib64
    fi
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
            DEVTOOLSETSEVEN='n'
            DEVTOOLSETEIGHT='y'
        else
            CLANG='n'
            DEVTOOLSETSEVEN='n'
            DEVTOOLSETEIGHT='y'
        fi

    if [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-7/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-8/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/gcc8/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/gcc9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-9/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-10/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
    fi

    if [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
        source /opt/rh/devtoolset-11/enable
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
        else
            export CC="gcc"
            export CXX="g++"
        fi
        gcc --version
        g++ --version
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
                    cmservice mysql status;
                    cmservice mysql restart;
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

    if [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX
    fi

    if [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX
    fi

    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX 
    fi
    if [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX 
    fi

    if [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX
    fi
    if [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -gt '407' ]]; then
      unset CC
      unset CXX
    fi

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
                    service mysqld status;
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

        # popd
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
    elif [[ "$NGINX_ZLIBNG" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
        zlibng_install
        if [[ "$cfzlib_check" -ne '0' ]]; then
            CLOUDFLARE_ZLIB='n'
            install_stdzlib
        fi
    else
        install_stdzlib
    fi
  fi
}