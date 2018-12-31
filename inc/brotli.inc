# for ngx_brotli https://github.com/eustas/ngx_brotli
# + libbrotli https://github.com/bagder/libbrotli

scl_install() {
  # if gcc version is less than 4.7 (407) install scl collection yum repo
  if [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
    # if devtoolset exists, enable it first before checking gcc versions
    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      if [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        source /opt/gcc8/enable
      fi
    elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
      fi
    elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
      fi
    else
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
      fi
    fi
    if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      echo "install centos-release-scl for newer gcc and g++ versions"
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl
        fi
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        echo
        /opt/gcc8/bin/gcc --version
        /opt/gcc8/bin/g++ --version
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      # llbm-toolset-7 for clang 4 isn't ready for centos 6.x yet
      # if [[ "$DEVTOOLSETSEVEN" = [yY] || "$CLANG_FOUR" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        # llbm-toolset-7 for clang 4 isn't ready for centos 6.x yet
        # if [[ "$CLANG_FOUR" = [yY] && ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
        #   time $YUMDNFBIN -y install devtoolset-7-runtime llvm-toolset-7-runtime devtoolset-7-libstdc++-devel llvm-toolset-7-clang llvm-toolset-7-llvm-libs llvm-toolset-7-llvm-static llvm-toolset-7-compiler-rt llvm-toolset-7-libomp llvm-toolset-7-clang-libs
        # fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
        # llbm-toolset-7 for clang 4 isn't ready for centos 6.x yet
        # if [[ "$CLANG_FOUR" = [yY] && -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
        #   /opt/rh/llvm-toolset-7/root/usr/bin/clang -v
        # fi
        # if [[ -f /opt/rh/llvm-toolset-7/root/usr/bin/clang++ ]]; then
        #   /opt/rh/llvm-toolset-7/root/usr/bin/clang++ -v
        # fi
      elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-6-gcc >/de6/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ 6/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils6>/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
    fi
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        time $YUMDNFBIN -y -q install centos-release-scl
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        echo
        /opt/gcc8/bin/gcc --version
        /opt/gcc8/bin/g++ --version
      elif [[ "$DEVTOOLSETSEVEN" = [yY] || "$CLANG_FOUR" = [yY] || "$CLANG_FIVE" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        if [[ "$CLANG_FOUR" = [yY] && ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
          time $YUMDNFBIN -y install devtoolset-7-runtime llvm-toolset-7-runtime devtoolset-7-libstdc++-devel llvm-toolset-7-clang llvm-toolset-7-llvm-libs llvm-toolset-7-llvm-static llvm-toolset-7-compiler-rt llvm-toolset-7-libomp llvm-toolset-7-clang-libs
        fi
        if [[ "$CLANG_FIVE" = [yY] && ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
          time $YUMDNFBIN -y install devtoolset-7-runtime llvm-toolset-7-runtime devtoolset-7-libstdc++-devel llvm-toolset-7-clang llvm-toolset-7-llvm-libs llvm-toolset-7-llvm-static llvm-toolset-7-compiler-rt llvm-toolset-7-libomp llvm-toolset-7-clang-libs
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
        if [[ "$CLANG_FOUR" = [yY] && -f /opt/rh/llvm-toolset-7/root/usr/bin/clang || "$CLANG_FIVE" = [yY] && -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
          /opt/rh/llvm-toolset-7/root/usr/bin/clang -v
        fi
        # if [[ -f /opt/rh/llvm-toolset-7/root/usr/bin/clang++ ]]; then
        #   /opt/rh/llvm-toolset-7/root/usr/bin/clang++ -v
        # fi
      elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-6-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-6-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
  fi # centos 6 only needed
}

libbroti_install() {
  if [[ "$NGINX_LIBBROTLI" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    # if devtoolset-6 doesn't exist install it otherwise check if gcc & g++ versions are less than 4.7 (407)
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/rh/devtoolset-7/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/rh/devtoolset-6/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/rh/devtoolset-6/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      fi
    fi # centos 6 only needed

    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/gcc8/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
    fi        

    echo "install libbrotli"
    cd $DIR_TMP

    LIBBROTLI_LINK='https://github.com/bagder/libbrotli'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $LIBBROTLI_LINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    LIBBROTLI_CURLCHECK=$?
    if [[ "$LIBBROTLI_CURLCHECK" != '0' ]]; then
      LIBBROTLI_LINK='https://gitlab.com/centminmod-github-mirror/libbrotli.git'
    fi
    if [ ! -d libbrotli ]; then
      time git clone "$LIBBROTLI_LINK"
    elif [ -d libbrotli ]; then
      rm -rf libbrotli
      time git clone "$LIBBROTLI_LINK"
    fi
    cd libbrotli
    # if [[ "$INITIALINSTALL" != [yY] ]]; then
  #       make clean
  #       git stash
  #       git pull
    # fi
    ./autogen.sh
    ./configure
    make${MAKETHREADS}
    make install
    echo

    if [[ "$CENTOS_SIX" = '6' ]]; then
    if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
      scl_install
      unset CC
      unset CXX   
    elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX
    elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX 
    fi    
    fi # centos 6 only needed

    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX 
    fi        
  fi
}

brdep_update() {
  if [[ "$NGINX_BROTLIDEP_UPDATE" = [yY] ]]; then
    grep 'BROTLI_VERSION' c/common/version.h
    brotlilatest_tag=$(git tag | tail -1)
    brotlibranch=$(echo $brotlilatest_tag | sed -e 's|v|local-|')
    echo $brotlilatest_tag
    echo $brotlibranch
    git stash
    git checkout $brotlilatest_tag -b $brotlibranch
    grep 'BROTLI_VERSION' c/common/version.h
  fi
}

ngxbrotli_download() {
  if [[ "$NGINX_LIBBROTLI" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    # only if GCC used to compile nginx with ngx_brotli
    # set BROTLI_CFLAG https://community.centminmod.com/posts/38284/
    if [[ "$CLANG" = [nN] ]]; then
      BROTLI_CFLAG=' -Wno-deprecated-declarations'
    else
      BROTLI_CFLAG=""
    fi
    echo "BROTLI_CFLAG=$BROTLI_CFLAG"
    echo "download ngx_brotli module"
    cd $DIR_TMP

    NGXBROTLILINK='https://github.com/eustas/ngx_brotli'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $NGXBROTLILINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    NGXBROTLICURLCHECK=$?
    if [[ "$NGXBROTLICURLCHECK" != '0' ]]; then
      NGXBROTLILINK='https://gitlab.com/centminmod-github-mirror/ngx_brotli.git'
    fi

    rm -rf ngx_brotli
    if [ ! -d ngx_brotli ]; then
      time git clone --recursive "$NGXBROTLILINK"
      cd ngx_brotli
      git submodule update --init
      cd deps/brotli
      brdep_update
      ./configure-cmake
      make${MAKETHREADS}
      make install
      cd ../../
      cd ../
    fi
    # if [[ "$INITIALINSTALL" != [yY] ]]; then
    #     cd ngx_brotli
    #     # git stash
    #     git fetch origin
    #     git reset --hard origin/master
    #     git pull
    #     git submodule update --init
    #     cd deps/brotli
    #     brdep_update
    #     ./configure-cmake
    #     make${MAKETHREADS}
    #     make install
    #     cd ../../
    #     cd ../
    # fi
    echo
cat > "/usr/local/nginx/conf/brotli_inc.conf" <<EIF
brotli on;
brotli_static on;
brotli_min_length 65536;
brotli_buffers 1024 8k;
brotli_comp_level 5;
brotli_types text/plain text/css text/xml application/javascript application/x-javascript application/xml application/xml+rss application/ecmascript application/json image/svg+xml;
EIF

BROTLI_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/brotli_inc.conf' /usr/local/nginx/conf/nginx.conf)

    if [[ -z "$BROTLI_INCLUDECHECK" && -f /usr/local/nginx/conf/brotli_inc.conf ]]; then
          sed -i 's/http {/http { \n include \/usr\/local\/nginx\/conf\/brotli_inc.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
      sed -i 's|^ #include \/usr\/local\/nginx\/conf\/brotli_inc.conf;| include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
    else
      sed -i 's|^ include \/usr\/local\/nginx\/conf\/brotli_inc.conf;| #include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi
}