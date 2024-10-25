libheif_install() {
    if [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETTWELVE" = [yY] ]]; then
        if [[ -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc || -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
            source /opt/rh/gcc-toolset-12/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETELEVEN" = [yY] ]]; then
        if [[ -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc || -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
            source /opt/rh/gcc-toolset-11/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETTEN" = [yY] ]]; then
        if [[ -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc || -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
            source /opt/rh/gcc-toolset-10/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETNINE" = [yY] ]]; then
        if [[ -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc || -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
            source /opt/rh/gcc-toolset-9/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETELEVEN" = [yY] ]]; then
        if [[ -f /opt/rh/devtoolset-11/root/usr/bin/gcc || -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
            source /opt/rh/devtoolset-11/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETTEN" = [yY] ]]; then
        if [[ -f /opt/rh/devtoolset-10/root/usr/bin/gcc || -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
            source /opt/rh/devtoolset-10/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
            export CFLAGS="-fPIC${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
            export CXXFLAGS="-fPIC"
        fi
    else
        export CFLAGS="-fPIC"
        export CXXFLAGS="-fPIC"
    fi
    echo "Install libheif routine"
    echo "installing x265, libde265 & libheif"
    enable_devtoolset
    if [[ "$CENTOS_SEVEN" = '7' ]] && [[ ! "$(rpm -qa cmake3)" || ! "$(rpm -qa cmake3-data)" ]]; then
        yum -q -y install cmake3 cmake3-data
    fi
    if [[ ! "$(rpm -qa mercurial)" ]]; then
        yum -q -y install mercurial
    fi 
    echo "Install x265 for libheif"
    pushd "$DIR_TMP"
    rm -rf x265
    git clone https://bitbucket.org/multicoreware/x265_git x265
    cd x265/build/linux
    git checkout Release_3.6
    cmake -G "Unix Makefiles" -DENABLE_PIC=:BOOL=1 -DENABLE_SHARED:BOOL=1 -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../../source
    make -s -j$(nproc)
    make install
    echo

    echo "Install libde265 for libheif"
    pushd "$DIR_TMP"
    # rm -rf libde265
    # git clone --depth=1 https://github.com/strukturag/libde265
    rm -rf libde265-*
    wget -q https://github.com/strukturag/libde265/releases/download/v${LIBDE265_VER}/libde265-${LIBDE265_VER}.tar.gz -O libde265-${LIBDE265_VER}.tar.gz
    tar xzf libde265-${LIBDE265_VER}.tar.gz
    cd libde265-${LIBDE265_VER}
    # cd libde265
    # mkdir -p build
    # cd build
    # rm -rf CMakeCache.txt
    # cmake3 .. -DCMAKE_INSTALL_PREFIX=/usr/local
    # sed -i 's|autoheader|\/usr\/bin\/autoheader|g' autogen.sh
    # sed -i 's|autoconf|\/usr\/bin\/autoconf|g' autogen.sh
    # sed -i 's|automake|\/usr\/bin\/automake|g' autogen.sh
    # sed -i 's|autoreconf|\/usr\/bin\/autoreconf|g' autogen.sh
    # sed -i 's|aclocal|\/usr\/bin\/aclocal|g' autogen.sh
    # ./autogen.sh
    ./configure
    make -s -j$(nproc)
    make install
    echo
    echo "libde265 install completed"

    export CC="gcc"
    export CXX="g++"
    echo "Install libheif for HEIF file format decoder/encoder"
    pushd "$DIR_TMP"
    # rm -rf libheif
    # git clone --depth=1 https://github.com/strukturag/libheif
    # cd libheif
    # ./autogen.sh
    rm -rf libheif-*
    wget -q https://github.com/strukturag/libheif/releases/download/v${LIBHEIF_VER}/libheif-${LIBHEIF_VER}.tar.gz -O libheif-${LIBHEIF_VER}.tar.gz
    tar xzf libheif-${LIBHEIF_VER}.tar.gz
    cd libheif-${LIBHEIF_VER}
    make clean
    # export libde265_CFLAGS='-I/usr/local/include'
    # export libde265_LIBS='-L/usr/local/lib'
    # export x265_CFLAGS='-I/usr/local/include'
    # export x265_LIBS='-L/usr/local/lib'   
    # export LDFLAGS='-L/usr/local/lib'
    PKG_CONFIG_PATH="/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/lib/pkgconfig" LD_LIBRARY_PATH=/usr/local/lib LDFLAGS="-L/usr/local/lib" CPPFLAGS="-I/usr/local/include" ./configure --disable-examples
    make -s -j$(nproc)
    make install
    echo
    echo "libheif install completed"
    # unset libde265_CFLAGS
    # unset libde265_LIBS
    # unset x265_CFLAGS
    # unset x265_LIBS
}

imagemagick_source_uninstall() {
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" != [yY] && -f /usr/local/share/doc/ImageMagick-7 ]]; then
        # uninstall source compiled ImageMagick 7 if found
        pushd "$DIR_TMP"
        echo
        echo "ImageMagick 7 source uninstall start..."
        cd ImageMagick-7*
        make uninstall
        ldconfig
        echo "ImageMagick 7 source uninstall completed"
        popd
    fi
}

imagemagick_source_install() {
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        pushd "$DIR_TMP"
        echo
        echo "ImageMagick 7 source install start..."
        wget -q https://imagemagick.org/archive/ImageMagick.tar.gz -O ImageMagick.tar.gz
        rm -rf ImageMagick-7*
        tar xzf ImageMagick.tar.gz
        cd ImageMagick-7*
        make clean
        if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ -d /opt/libheif/lib64 && -d /opt/libheif/include ]]; then
            echo
            echo "libheif detected for EL8/EL9"
            IMAGEMAGICK_PKG_CONFIG_PATH="/opt/libheif/lib64/pkgconfig:${PKG_CONFIG_PATH}:"
            IMAGEMAGICK_CFLAGS="-I/opt/libheif/include "
            IMAGEMAGICK_LDFLAGS="-L/opt/libheif/lib64 "
            IMAGEMAGICK_LD_LIBRARY_PATH="/opt/libheif/lib64:"
            PHP_IMAGICK_LIBHEIF_LIB=' -L/opt/libheif/lib64'
        fi
        # PKG_CONFIG_PATH="${IMAGEMAGICK_PKG_CONFIG_PATH}/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/lib/pkgconfig" LD_LIBRARY_PATH=${IMAGEMAGICK_LD_LIBRARY_PATH}/usr/local/lib:/usr/lib64 LDFLAGS="${IMAGEMAGICK_LDFLAGS}-L/usr/local/lib -L/usr/lib64" CPPFLAGS="${IMAGEMAGICK_CFLAGS}-I/usr/local/include -I/usr/include" ./configure --with-heic=yes --with-jemalloc=yes --with-modules=yes --with-djvu=yes --with-pango=yes --with-raw=yes
        PKG_CONFIG_PATH="${IMAGEMAGICK_PKG_CONFIG_PATH}/usr/lib64/pkgconfig:/usr/share/pkgconfig:/usr/local/lib/pkgconfig" LD_LIBRARY_PATH=${IMAGEMAGICK_LD_LIBRARY_PATH}/usr/local/lib:/usr/lib64 LDFLAGS="${IMAGEMAGICK_LDFLAGS}-L/usr/local/lib -L/usr/lib64" CPPFLAGS="${IMAGEMAGICK_CFLAGS}-I/usr/local/include -I/usr/include" ./configure --with-heic=yes --without-jemalloc --with-modules=yes --with-djvu=yes --with-pango=yes --with-raw=yes
        make -s -j$(nproc)
        make install
        ldconfig
        echo
        echo "ImageMagick 7 source install completed"
        popd
    fi
}

checkphpext() {

  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPMVER" = '8.4' || "$PHPMUVER" = '8.4' || "$PHPVER_ID" -ge '80000' ]]; then
    # PHP 7+ requires imagick >3.4.4 i.e. 3.6.0+
    # PHP 5.x requires imagick <=3.4.4
    IMAGICKPHP_VER="$IMAGICKPHP_SEVEN_VER"
    PHPEXT_IMAGICKLINKFILE="imagick-${IMAGICKPHP_VER}.tgz"
    PHPEXT_IMAGICKLINK="https://pecl.php.net/get/${PHPEXT_IMAGICKLINKFILE}"
    PHPEXT_IMAGICKLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/imagick/${PHPEXT_IMAGICKLINKFILE}"
  fi
  
    cecho "Check for php extensions" $boldyellow
    if [[ ! -f "${DIR_TMP}/imagick-${IMAGICKPHP_VER}.tgz" || ! -d "${DIR_TMP}/imagick-${IMAGICKPHP_VER}" ]]; then
        echo "Downloading imagick extension"
        imagickphpexttarball
    fi
    echo
}

checkimagicksys() {

    if [ ! -f /etc/yum/pluginconf.d/versionlock.conf ]; then
        yum -y install yum-plugin-versionlock
    fi
    if [ -f /etc/yum/pluginconf.d/versionlock.conf ]; then
        VERSIONLOCK_REPO=',versionlock'
    else
        VERSIONLOCK_REPO=""
    fi
    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ "$IMAGEMAGICK_SEVEN" = [yY] ]]; then
        REMI_IMAGEMAGICK_PKGNAME='7'
    else
        if [[ "$CENTOS_NINE" = '9' ]]; then
            REMI_IMAGEMAGICK_PKGNAME=''
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
            REMI_IMAGEMAGICK_PKGNAME=''
        elif [[ "$CENTOS_SEVEN" = '7' ]]; then
            REMI_IMAGEMAGICK_PKGNAME='6'
        elif [[ "$CENTOS_SIX" = '6' ]]; then
            REMI_IMAGEMAGICK_PKGNAME='6'
        else
            REMI_IMAGEMAGICK_PKGNAME="6"
        fi
    fi
    if [[ "$IMAGEMAGICK_HEIF" = [yY] ]]; then
        IMG_HEIFOPT=' ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic'
    else
        IMG_HEIFOPT=""
    fi

    if [ -f /usr/bin/re2c ]; then
        if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
            rerpm
        fi
    fi

    CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
    
    if [ "$CENTOSVER" == 'release' ]; then
        CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
        if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
            CENTOS_SEVEN='7'
        fi
    fi
    
    if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
        CENTOS_SIX='6'
    fi


    if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$DNF_ENABLE" = [yY] ]]; then
        # yum -y -q install epel-release
        if [[ ! -f /usr/bin/dnf ]]; then
            yum -y -q install dnf
        fi
        if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
            DISABLEREPO_DNF=' --disablerepo=rpmforge'
            YUMDNFBIN="dnf${DISABLEREPO_DNF}"
        else
            DISABLEREPO_DNF=""
            YUMDNFBIN='dnf'
        fi
    else
        YUMDNFBIN='yum'
    fi

    if [ -z "$(rpm -qa open-sans-fonts)" ]; then
        $YUMDNFBIN -y install open-sans-fonts
    fi

    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [ -z "$(rpm -qa openjpeg2-devel)" ]; then
        $YUMDNFBIN -y install openjpeg2-devel
    fi

    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ -z "$(rpm -qa libjxl)" || -z "$(rpm -qa liblqr)" ]]; then
        $YUMDNFBIN -y install openjpeg2-devel djvulibre-libs highway jxl-pixbuf-loader libaom libdav1d libheif libjxl liblqr-1 libvmaf
    fi

    # whether to skip ImageMagick YUM updates
    if [[ "$SKIP_IMAGEMAGICK_YUM_UPDATES" != [yY] ]]; then
        cecho "Check for ImageMagicK System Updates (YUM)" $boldyellow
        if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
            if [ -f /etc/yum.repos.d/remi.repo ]; then
                if [[ "$(rpm -qi ImageMagick | grep 'Release' | grep -o remi >/dev/null 2>&1; echo $?)" = '0' || "$(rpm -qi ImageMagick6 | grep 'Release' | grep -o remi >/dev/null 2>&1; echo $?)" = '0' || "$(rpm -qi ImageMagick7 | grep 'Release' | grep -o remi >/dev/null 2>&1; echo $?)" = '0' ]]; then
                    # skip for initial installs to speed up install
                    if [[ "$INITIALINSTALL" != [yY] ]]; then
                        # yum clean all >/dev/null 2>&1
                        if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ "$IMAGEMAGICK_SEVEN" = [yY] ]]; then
                            if [[ ! "$(rpm -qi ImageMagick7 | grep 'Release' | grep -o remi >/dev/null 2>&1; echo $?)" = '0' ]]; then
                                echo "############## ImageMagick7 Routine Start ##############"
                                echo "$YUMDNFBIN versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw"
                                $YUMDNFBIN versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw
                                echo "$YUMDNFBIN versionlock delete ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel ImageMagick-libs${IMG_HEIFOPT} ImageMagick-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw"
                                $YUMDNFBIN versionlock delete ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel ImageMagick-libs${IMG_HEIFOPT} ImageMagick-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw
                                echo "$YUMDNFBIN -y remove ImageMagick ImageMagick-c++ ImageMagick-c++-devel ImageMagick-devel ImageMagick-djvu ImageMagick-libs"
                                $YUMDNFBIN -y remove ImageMagick ImageMagick-c++ ImageMagick-c++-devel ImageMagick-devel ImageMagick-djvu ImageMagick-libs
                                if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ -z "$(rpm -qa libjxl)" || -z "$(rpm -qa liblqr)" ]]; then
                                    $YUMDNFBIN -y install openjpeg2-devel djvulibre-libs highway jxl-pixbuf-loader libaom libdav1d libheif libjxl liblqr-1 libvmaf
                                fi
                                echo "############## ImageMagick7 Routine End ##############"
                            fi
                        elif [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ "$IMAGEMAGICK_SEVEN" != [yY] ]]; then
                            if [[ ! "$(rpm -qi ImageMagick6 | grep 'Release' | grep -o remi >/dev/null 2>&1; echo $?)" = '0' ]]; then
                                echo "############## ImageMagick6 Routine Start ##############"
                                echo "$YUMDNFBIN versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw"
                                $YUMDNFBIN versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw
                                echo "$YUMDNFBIN versionlock delete ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel ImageMagick-libs${IMG_HEIFOPT} ImageMagick-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw"
                                $YUMDNFBIN versionlock delete ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel ImageMagick-libs${IMG_HEIFOPT} ImageMagick-heic ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw
                                echo "$YUMDNFBIN -y remove ImageMagick ImageMagick-c++ ImageMagick-c++-devel ImageMagick-devel ImageMagick-djvu ImageMagick-libs"
                                $YUMDNFBIN -y remove ImageMagick ImageMagick-c++ ImageMagick-c++-devel ImageMagick-devel ImageMagick-djvu ImageMagick-libs
                                if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ -z "$(rpm -qa libjxl)" || -z "$(rpm -qa liblqr)" ]]; then
                                    $YUMDNFBIN -y install openjpeg2-devel djvulibre-libs highway jxl-pixbuf-loader libaom libdav1d libheif libjxl liblqr-1 libvmaf
                                fi
                                echo "############## ImageMagick6 Routine End ##############"
                            fi
                        fi
                        if [[ "$CENTOS_NINE" -eq '9' ]]; then
                            echo "time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --disablerepo=epel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken"
                            time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --disablerepo=epel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken
                        fi      
                        if [[ "$CENTOS_EIGHT" -eq '8' ]]; then
                            # for el8 libzip is an appstream module tied to php and nginx modules which disabled by default
                            # so temp re-enable nginx and php modules in order to get libzip installed and then re-disable them
                            echo
                            echo "dnf module enable nginx:1.24 -y"
                            dnf module enable nginx:1.24 -y
                            echo "dnf module enable php:8.2 -y"
                            dnf module enable php:8.2 -y
                            echo "dnf -y install libzip libzip-devel"
                            dnf -y install libzip libzip-devel
                            echo "dnf module disable php:8.2 -y"
                            dnf module disable php:8.2 -y
                            echo "dnf module disable nginx:1.24 -y"
                            dnf module disable nginx:1.24 -y
                            echo
                            time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --disablerepo=epel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken
                        fi
                        if [[ "$CENTOS_SEVEN" -eq '7' && "$(rpm -qa libwebp-devel)" ]]; then
                            yum -y swap libwebp-devel libwebp7-devel --enablerepo=remi
                            time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO}
                        fi
                        if [[ "$(convert --version | grep -q '6.9.11'; echo $?)" -eq '0' ]]; then
                            echo "time $YUMDNFBIN -y swap ImageMagick ImageMagick${REMI_IMAGEMAGICK_PKGNAME} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y swap ImageMagick ImageMagick${REMI_IMAGEMAGICK_PKGNAME} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            # echo "time $YUMDNFBIN -y swap ImageMagick-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            # time $YUMDNFBIN -y swap ImageMagick-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            echo "time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            echo "time $YUMDNFBIN -y swap ImageMagick-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y swap ImageMagick-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            # echo "time $YUMDNFBIN -y swap ImageMagick-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            # time $YUMDNFBIN -y swap ImageMagick-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            echo "time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            echo "time $YUMDNFBIN -y swap ImageMagick-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y swap ImageMagick-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                            echo "time $YUMDNFBIN -y update LibRaw --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y update LibRaw --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                        else
                            echo "$YUMDNFBIN -y update ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base"
                            time $YUMDNFBIN -y update ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} -x ImageMagick7* --disablerepo=base
                        fi
                        sar_call
                    fi
                else
                    if [[ "$CENTOS_SIX" = '6' ]]; then
                        # time $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                        sar_call
                        # yum clean all >/dev/null 2>&1
                        time yum -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken
                    else
                        # yum clean all >/dev/null 2>&1
                        if [[ "$CENTOS_SEVEN" = '7' && "$(rpm -qa libwebp-devel)" ]]; then
                            yum -y swap libwebp-devel libwebp7-devel --enablerepo=remi
                        fi
                        time yum -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO}
                    fi
                fi
            elif [ ! -f /etc/yum.repos.d/remi.repo ]; then
                # for very old centmin mod installs which never had remi yum repo setup
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    remisixyum
                elif [[ "$CENTOS_SEVEN" = '7' ]]; then
                    remisevenyum
                elif [[ "$CENTOS_EIGHT" = '8' ]]; then
                    remieightyum
                fi
                if [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) = '0' ]]; then
                    echo
                    echo "----------------------------------------------------------------------------------"
                    cecho "Using Remi YUM repo ImageMagicK version" $boldyellow
                    echo "----------------------------------------------------------------------------------"
                    if [[ "$CENTOS_EIGHT" = '8' ]]; then
                        time $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
                    fi
                    sar_call
                    if [[ "$CENTOS_EIGHT" != '8' ]]; then
                        yum -y remove ImageMagick${DISABLEREPO_DNF}
                    fi
                    sar_call
                    if [[ "$CENTOS_EIGHT" = '8' ]]; then
                        time $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
                    elif [[ "$CENTOS_SEVEN" = '7' ]]; then
                        if [[ "$CENTOS_SEVEN" = '7' && "$(rpm -qa libwebp-devel)" ]]; then
                            yum -y swap libwebp-devel libwebp7-devel --enablerepo=remi
                        fi
                        time $YUMDNFBIN -y install lcms2-devel libwebp7 libwebp7-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
                    else
                        time $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
                    fi
                    sar_call
                    if [[ "$CENTOS_EIGHT" != '8' ]]; then
                        yum -y remove ImageMagick${DISABLEREPO_DNF}
                    fi
                    sar_call                  
                    if [[ "$CENTOS_SIX" = '6' ]]; then
                        # time $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                        time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken
                        sar_call
                    else
                        if [[ "$CENTOS_SEVEN" = '7' && "$(rpm -qa libwebp-devel)" ]]; then
                            yum -y swap libwebp-devel libwebp7-devel --enablerepo=remi
                        fi
                        time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO}
                        sar_call
                    fi
                    echo
                else
                    # if ImageMagick doesn't exist
                    if [ ! -f /etc/yum.repos.d/remi.repo ]; then
                        # for very old centmin mod installs which never had remi yum repo setup
                        if [[ "$CENTOS_SIX" = '6' ]]; then
                            remisixyum
                        elif [[ "$CENTOS_SEVEN" = '7' ]]; then
                            remisevenyum
                        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
                            remieightyum
                        fi
                    fi
                    time $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
                    sar_call
                    if [[ "$CENTOS_SIX" = '6' ]]; then
                        # time $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                        time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO} --skip-broken
                        sar_call
                    else
                        if [[ "$CENTOS_SEVEN" = '7' && "$(rpm -qa libwebp-devel)" ]]; then
                            yum -y swap libwebp-devel libwebp7-devel --enablerepo=remi
                        fi
                        time $YUMDNFBIN -y install ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities${VERSIONLOCK_REPO}
                        sar_call
                    fi
                fi
            fi
        fi
        echo
        if [[ -f /etc/yum/pluginconf.d/versionlock.conf && -f /etc/yum.repos.d/remi.repo ]]; then
            # lock imagemagick system version from general yum updates
            # allowing only centmin.sh menu option 15 to update system
            # imagemagick version by doing an unlock of the version
            yum versionlock delete ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel ImageMagick6-libs${IMG_HEIFOPT} ImageMagick-djvu LibRaw
            yum versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu LibRaw
            if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
                yum versionlock delete ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic
            fi
            if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
                yum versionlock ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-djvu ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-heic
            elif [[ "$CENTOS_SIX" = '6' ]]; then
                yum versionlock ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw
            else
                yum versionlock ImageMagick${REMI_IMAGEMAGICK_PKGNAME} ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++ ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-c++-devel ImageMagick${REMI_IMAGEMAGICK_PKGNAME}-libs${IMG_HEIFOPT} LibRaw
            fi
        fi
        echo
    fi # whether to skip ImageMagick YUM updates
}

imagickinstall() {
    imagick_submenu=$1
    if [[ "$PHP_INSTALL" = [yY] ]]; then
    if [[ "$PHPIMAGICK" = [yY] ]]; then
        checkphpext
        checkimagicksys
        imagick_fixes
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      # notes for users if PHP 7.0 detected
      check_devtoolset_php
      enable_devtoolset

    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        echo "*************************************************"
        cecho "* Installing libheif dependencies & ImageMagick 7" $boldgreen
        echo "*************************************************"
        if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ ! -d /opt/libheif/lib64 && ! -d /opt/libheif/include ]]; then
            libheif_install
        fi
        imagemagick_source_install
    else
        imagemagick_source_uninstall
    fi

    if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ -d /opt/libheif/lib64 && -d /opt/libheif/include ]]; then
        echo
        echo "libheif detected for EL8/EL9"
        export PKG_CONFIG_PATH="/opt/libheif/lib64/pkgconfig:$PKG_CONFIG_PATH"
        export CFLAGS="-I/opt/libheif/include"
        export LDFLAGS="-L/opt/libheif/lib64"
        PHP_IMAGICK_LIBHEIF_LIB=' -L/opt/libheif/lib64'
    fi

    echo "*************************************************"
    cecho "* Installing imagick PHP Extension" $boldgreen
    echo "*************************************************"

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP

php-config --version | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3|7.4'
PHPSEVEN_CHECKVER=$?
echo $PHPSEVEN_CHECKVER
php-config --version | cut -d . -f1,2 | egrep -w '8.0|8.1|8.2|8.3|8.4'
PHPEIGHT_CHECKVER=$?
echo $PHPEIGHT_CHECKVER

if [[ "$CENTOS_SIX" -eq '6' ]]; then
    # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
    if [ ! -f /usr/bin/autoconf268 ]; then
        echo "yum -q -y install autoconf268"
        yum -q -y install autoconf268
    fi
    if [ -f /usr/bin/autoconf268 ]; then
        export PHP_AUTOCONF=/usr/bin/autoconf268
        export PHP_AUTOHEADER=/usr/bin/autoheader268
    fi
fi

if [[ "$PHPMUVER" = '8.4' || "$PHPVER_ID" -ge '80400' || "$PHPMUVER" = 'NGDEBUG' || "$PHPEIGHT_CHECKVER" = '0' ]]; then
    IMAGICKGITLINK='https://github.com/Imagick/imagick'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    IMAGICKGITCURLCHECK=$?
    if [[ "$IMAGICKGITCURLCHECK" != '0' ]]; then
        IMAGICKGITLINK='https://gitlab.com/centminmod-github-mirror/imagick.git'
    fi

    if [[ -d "imagick-php8" && -d "imagick-php8/.git" ]]; then       
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    elif [[ -d "imagick-php8" && ! -d "imagick-php8/.git" ]]; then
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    else
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    fi
    echo
    echo "compiling imagick PHP extension for PHP 8.4 ..."
    cd imagick-php8
    imagick_php_patches
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader
    /usr/local/bin/phpize
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr/local
    else
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr
    fi
    sar_call
    make${MAKETHREADS}
    sar_call
    make install
    sar_call
elif [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' || "$PHPMUVER" = 'NGDEBUG' || "$PHPEIGHT_CHECKVER" = '0' ]] && [[ "$(echo $IMAGICKPHP_VER | cut -d . -f1,2 | sed -e 's|\.||')" -le '34' ]]; then
    IMAGICKGITLINK='https://github.com/Imagick/imagick'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    IMAGICKGITCURLCHECK=$?
    if [[ "$IMAGICKGITCURLCHECK" != '0' ]]; then
        IMAGICKGITLINK='https://gitlab.com/centminmod-github-mirror/imagick.git'
    fi

    if [[ -d "imagick-php8" && -d "imagick-php8/.git" ]]; then       
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    elif [[ -d "imagick-php8" && ! -d "imagick-php8/.git" ]]; then
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    else
        rm -rf imagick-php8
        git clone "$IMAGICKGITLINK" imagick-php8
        sar_call
    fi
    echo
    echo "compiling imagick PHP extension for PHP 8.x ..."
    cd imagick-php8
    imagick_php_patches
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader
    /usr/local/bin/phpize
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr/local
    else
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr
    fi
    sar_call
    make${MAKETHREADS}
    sar_call
    make install
    sar_call
elif [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = '7.4' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] && [[ "$(echo $IMAGICKPHP_VER | cut -d . -f1,2 | sed -e 's|\.||')" -le '33' ]]; then
    IMAGICKGITLINK='https://github.com/Imagick/imagick'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    IMAGICKGITCURLCHECK=$?
    if [[ "$IMAGICKGITCURLCHECK" != '0' ]]; then
        IMAGICKGITLINK='https://gitlab.com/centminmod-github-mirror/imagick.git'
    fi

    if [[ -d "imagick-php7" && -d "imagick-php7/.git" ]]; then       
        rm -rf imagick-php7
        git clone "$IMAGICKGITLINK" imagick-php7
        sar_call
    elif [[ -d "imagick-php7" && ! -d "imagick-php7/.git" ]]; then
        rm -rf imagick-php7
        git clone "$IMAGICKGITLINK" imagick-php7
        sar_call
    else
        rm -rf imagick-php7
        git clone "$IMAGICKGITLINK" imagick-php7
        sar_call
    fi
    echo
    echo "compiling imagick PHP extension for PHP 7.x ..."
    cd imagick-php7
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader
    /usr/local/bin/phpize
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr/local
    else
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr
    fi
    sar_call
    make${MAKETHREADS}
    sar_call
    make install
    sar_call
else
    echo "compiling imagick PHP extension for fallback nont PHP 7/8 ..."
    cd imagick-${IMAGICKPHP_VER}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader
    /usr/local/bin/phpize
    if [[ "$IMAGEMAGICK_SOURCE_INSTALL" = [yY] ]]; then
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr/local
    else
        CFLAGS="-fPIC -O2 -fstack-protector-strong -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie${PHP_IMAGICK_LIBHEIF_LIB}" ./configure --with-php-config=/usr/local/bin/php-config --with-imagick=/usr
    fi
    sar_call
    make${MAKETHREADS}
    sar_call
    make install
    sar_call
fi # php 7 or not

#######################################################
# check if imagick.so exists in php.ini

IMAGICKSOCHECK=$(grep 'extension=imagick.so' /usr/local/lib/php.ini >/dev/null 2>&1; echo $?)
IMAGICKSOCHECKB=$(grep 'extension=imagick.so' "${CONFIGSCANDIR}/imagick.ini" >/dev/null 2>&1; echo $?)

if [[ "$IMAGICKSOCHECK" = '1' || "$IMAGICKSOCHECKB" = '1' ]]; then
    echo -e "\nCopying imagick.ini > ${CONFIGSCANDIR}/imagick.ini\n"
    echo "extension=imagick.so" > "${CONFIGSCANDIR}/imagick.ini"
    echo ";imagick.skip_version_check=1" >> "${CONFIGSCANDIR}/imagick.ini"
fi #check if imagick.so exists in php.ini

    if [[ "$imagick_submenu" = 'submenu' ]]; then
        cmservice php-fpm restart
    fi
    echo "*************************************************"
    cecho "* imagick PHP installed" $boldgreen
    echo "*************************************************"

    fi
    fi # PHP_INSTALL=y
}