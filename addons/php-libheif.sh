#!/bin/bash
############################################################
# build libheif
############################################################

GCC_ELEVEN='y'
GCC_TWELVE='y'
GCC_THIRTHTEEN='y'
GCC_FOURTEEN='n'
GCC_FIFTTEEN='n'

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ALMALINUX_TEN='10'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ROCKYLINUX_TEN='10'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ORACLELINUX_TEN='10'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    VZLINUX_TEN='10'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    CIRCLELINUX_TEN='10'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    NAVYLINUX_TEN='10'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    EUROLINUX_TEN='10'
  fi
fi

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

if [[ "$CUSTOM_LIBHEIF_INSTALL" != [yY] ]]; then
  echo
  echo "Aborting install... experimental script not enabled yet"
  exit
fi

if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  if [[ "$GCC_NINE" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-9/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  elif [[ "$GCC_NINE" = [yY] && "$(uname -m)" = 'x86_64' && ! -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && ! -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
    echo "installing devtoolset-9 for GCC 9..."
    yum -y install gcc-toolset-9-binutils gcc-toolset-9-gcc gcc-toolset-9-gcc-c++
    source /opt/rh/gcc-toolset-9/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi
  
  if [[ "$GCC_TEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-10/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  elif [[ "$GCC_TEN" = [yY] && "$(uname -m)" = 'x86_64' && ! -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && ! -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
    echo "installing gcc-toolset-10 for GCC 10..."
    yum -y install gcc-toolset-10-binutils gcc-toolset-10-gcc gcc-toolset-10-gcc-c++
    source /opt/rh/gcc-toolset-10/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi

  if [[ "$GCC_ELEVEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-11/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi

  if [[ "$GCC_TWELVE" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-12/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi

  if [[ "$GCC_THIRTHTEEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-13/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-13/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-13/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi

  if [[ "$GCC_FOURTEEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/gcc-toolset-14/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-14/root/usr/bin/g++ ]]; then
    source /opt/rh/gcc-toolset-14/enable
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi

  if [[ "$GCC_FIFTTEEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/gcc-custom/gcc15/bin/gcc && -f /opt/gcc-custom/gcc15/bin/g++ ]]; then
    source /etc/profile.d/gcc15-custom.sh
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi
fi

if [[ "$CENTOS_TEN" -eq '10' ]]; then
  if [[ "$GCC_FOURTEEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /usr/bin/gcc && -f /usr/bin/g++ ]]; then
    export CFLAGS="-Wimplicit-fallthrough=0 -Wno-pedantic -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-overflow -Wno-maybe-uninitialized -Wno-address"
    export CXXFLAGS="${CFLAGS}"
  fi
fi

# Install required build tools and dependencies
yum install -y epel-release
yum install -y \
    git \
    cmake \
    gcc \
    gcc-c++ \
    make \
    pkgconfig \
    nasm \
    yasm \
    meson \
    ninja-build \
    automake \
    autoconf \
    libtool \
    bzip2-devel \
    zlib-devel

# Directory to store the source code
SOURCE_DIR="/svr-setup"
mkdir -p ${SOURCE_DIR}
cd ${SOURCE_DIR}

# Number of parallel jobs for make
JOBS=$(nproc)

# Function to download, build, and install a library
build_lib() {
    local name=$1
    local repo=$2
    local build_dir=$3
    local cmake_args=$4
    local install_prefix=$5

    echo "Building ${name}..."
    cd ${SOURCE_DIR}
    # Clone the repository
    rm -rf ${name}
    git clone ${repo} ${name}
    cd ${name}

    if [[ "$name" == 'x265' ]]; then
        git checkout Release_3.6
    fi

    # Create and move to build 
    rm -rf ${build_dir}
    mkdir -p ${build_dir}
    cd ${build_dir}

    # Configure, build, and install the library
    if [[ "$name" = 'x265' ]]; then
      cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=${install_prefix} ${cmake_args} ../../source
    elif [[ "$name" = 'libaom' ]]; then
      cmake ${cmake_args} -DCMAKE_INSTALL_PREFIX=${install_prefix} -DBUILD_SHARED_LIBS=ON ..
    else
      cmake ${cmake_args} -DCMAKE_INSTALL_PREFIX=${install_prefix} ..
    fi

    make -j${JOBS}
    make install

    # Return to the source directory for the next library
    cd ${SOURCE_DIR}
}

# 1. Build libde265
build_lib "libde265" \
    "https://github.com/strukturag/libde265.git" \
    "build" \
    "" \
    "/opt/libde265"

# 2. Build x265
build_lib "x265" \
    "https://bitbucket.org/multicoreware/x265_git.git" \
    "build/linux" \
    "" \
    "/opt/x265"

# 3. Build libaom
build_lib "libaom" \
    "https://aomedia.googlesource.com/aom" \
    "aom_build" \
    "" \
    "/opt/libaom"

# 4. Build dav1d
echo "Building dav1d..."
git clone https://code.videolan.org/videolan/dav1d.git
cd dav1d
meson setup build --prefix=/opt/dav1d
ninja -C build
ninja -C build install
cd ${SOURCE_DIR}

# 5. Build libjpeg-turbo
build_lib "libjpeg-turbo" \
    "https://github.com/libjpeg-turbo/libjpeg-turbo.git" \
    "build" \
    "" \
    "/opt/libjpeg-turbo"

# 6. Build libsharpyuv
echo "Building libsharpyuv..."
git clone https://chromium.googlesource.com/libyuv/libyuv libsharpyuv
cd libsharpyuv
mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/libsharpyuv \
      -DJPEG_INCLUDE_DIR=/opt/libjpeg-turbo/include \
      -DJPEG_LIBRARY=/opt/libjpeg-turbo/lib64/libjpeg.so \
      ..
make -j${JOBS}
make install
mkdir -p /opt/libsharpyuv/include/sharpyuv
ln -s /opt/libsharpyuv/include/libyuv/*.h /opt/libsharpyuv/include/sharpyuv/
cd ${SOURCE_DIR}

# 7. Build libheif with all dependencies
echo "Building libheif..."
git clone https://github.com/strukturag/libheif.git
cd libheif
mkdir -p build && cd build

time cmake -DCMAKE_INSTALL_PREFIX=/opt/libheif \
      -DCMAKE_INCLUDE_PATH="/opt/libde265/include:/opt/x265/include:/opt/libaom/include:/opt/dav1d/include:/opt/libjpeg-turbo/include:/opt/libsharpyuv/include" \
      -DCMAKE_LIBRARY_PATH="/opt/libde265/lib:/opt/x265/lib:/opt/libaom/lib64:/opt/dav1d/lib64:/opt/libjpeg-turbo/lib64:/opt/libsharpyuv/lib" \
      -DWITH_AOM_ENCODER=ON \
      -DWITH_AOM_DECODER=ON \
      -DWITH_DAV1D=ON \
      -DWITH_X265=ON \
      -DWITH_SvtEnc=OFF \
      -DWITH_RAV1E=OFF \
      -DWITH_JPEG_ENCODER=ON \
      -DWITH_JPEG_DECODER=ON \
      -DWITH_OpenH264_DECODER=OFF \
      -DWITH_LIBSHARPYUV=OFF \
      -DX265_INCLUDE_DIR=/opt/x265/include \
      -DX265_LIBRARY=/opt/x265/lib/libx265.so \
      -DDAV1D_INCLUDE_DIR=/opt/dav1d/include \
      -DDAV1D_LIBRARY=/opt/dav1d/lib64/libdav1d.so \
      -DAOM_INCLUDE_DIR=/opt/libaom/include \
      -DAOM_LIBRARY=/opt/libaom/lib64/libaom.so \
      -DLIBSHARPYUV_INCLUDE_DIR=/opt/libsharpyuv/include \
      -DLIBSHARPYUV_LIBRARY=/opt/libsharpyuv/lib/libyuv.so \
      ..

make -j${JOBS}
make install

echo "All libraries have been built and installed successfully."
