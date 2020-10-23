#!/bin/bash
###############################################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
###############################################################################
# ffmpeg installer for centminmod.com LEMP stack
# based on instructions at https://trac.ffmpeg.org/wiki/CompilationGuide/Centos
# http://git.videolan.org/?p=ffmpeg.git;a=blob;f=doc/APIchanges;hb=HEAD
# with bug fix by Patrizio / pbek https://community.centminmod.com/posts/24018/
# FFMPEG binary ends up installed at:
# /opt/bin/ffmpeg
###############################################################################
DIR_TMP=/svr-setup
OPT=/opt

DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
CONFIGSCANDIR='/etc/centminmod/php.d'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###############################################################################
# GCC options
GCC_SEVEN='n'
GCC_EIGHT='n'
GCC_NINE='n'
OPT_LEVEL='-O3'
MARCH_TARGETNATIVE='n' # for intel 64bit only set march=native, if no set to x86-64
###############################################################################
FFMPEG_DEBUG='n'
DISABLE_NETWORKFFMPEG='n'
ENABLE_FBTRANSFORM='n'
ENABLE_AVONE='n'
ENABLE_DAVONED='n'
ENABLE_FPIC='n'
ENABLE_FONTCONFIG='n'
ENABLE_LIBASS='y'
ENABLE_ZIMG='y'
ENABLE_OPENCV='n'
# http://downloads.xiph.org/releases/ogg/
LIBOGG_VER='1.3.4'
# http://downloads.xiph.org/releases/vorbis/
LIBVORBIS_VER='1.3.6'
GD_ENABLE='n'
NASM_SOURCEINSTALL='y'
NASM_VER='2.14'
YASM_VER='1.3.0'
FDKAAC_VER='0.1.6'
FONTCONFIG_VER='2.13.1'
FREETYPE_VER='2.10.1'
###############################################################################

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

if [[ "$FFMPEG_DEBUG" = [Nn] ]]; then
  FFMPEG_DEBUGOPT=' --disable-debug'
else
  FFMPEG_DEBUGOPT=""
fi

if [[ "$DISABLE_NETWORKFFMPEG" = [yY] ]]; then
  DISABLE_FFMPEGNETWORK=' --disable-network'
fi

if [[ "$ENABLE_ZIMG" = [yY] ]]; then
  ENABLE_ZIMGOPT=' --enable-libzimg'
else
  ENABLE_ZIMGOPT=""
fi

if [[ "$ENABLE_OPENCV" = [yY] ]]; then
  ENABLE_OPENCVOPT=' --enable-libopencv'
else
  ENABLE_OPENCVOPT=""
fi

if [[ "$ENABLE_FONTCONFIG" = [yY] ]]; then
  ENABLE_FONTCONFIGOPT=' --enable-fontconfig'
else
  ENABLE_FONTCONFIGOPT=""
fi

if [[ "$ENABLE_LIBASS" = [yY] ]]; then
  ENABLE_LIBASSOPT=' --enable-libass'
else
  ENABLE_LIBASSOPT=""
fi

if [[ "$ENABLE_AVONE" = [yY] ]]; then
  ENABLE_AVONEOPT=' --enable-libaom'
  ENABLE_FPIC='y'
else
  ENABLE_AVONEOPT=""
fi

if [[ "$ENABLE_DAVONED" = [yY] ]]; then
  ENABLE_DAVONEDOPT=' --enable-libdav1d'
  ENABLE_FPIC='y'
else
  ENABLE_DAVONEDOPT=""
fi

if [[ "$ENABLE_FPIC" = [yY] ]]; then
  ENABLE_FPICOPT=' --enable-pic --extra-ldexeflags=-pie'
  EXTRACFLAG_FPICOPTS='-fPIC'
  LDFLAG_FPIC=' -Wl,-Bsymbolic'
else
  ENABLE_FPICOPT=""
  EXTRACFLAG_FPICOPTS=""
  LDFLAG_FPIC=""
fi

if [[ "$MARCH_TARGETNATIVE" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
  MARCH_TARGET='native'
else
  MARCH_TARGET='x86-64'
fi

if [[ "$GCC_SEVEN" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
  source /opt/rh/devtoolset-7/enable
  export CFLAGS="${OPT_LEVEL} -march=${MARCH_TARGET} -Wimplicit-fallthrough=0 -Wno-pedantic"
  export CXXFLAGS="${CFLAGS}"
fi

if [[ "$GCC_EIGHT" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
  source /opt/rh/devtoolset-8/enable
  export CFLAGS="${OPT_LEVEL} -march=${MARCH_TARGET} -Wimplicit-fallthrough=0 -Wno-pedantic"
  export CXXFLAGS="${CFLAGS}"
fi

if [[ "$GCC_NINE" = [yY] && "$(uname -m)" = 'x86_64' && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
  source /opt/rh/devtoolset-9/enable
  export CFLAGS="${OPT_LEVEL} -march=${MARCH_TARGET} -Wimplicit-fallthrough=0 -Wno-pedantic"
  export CXXFLAGS="${CFLAGS}"
fi

do_continue() {
  # echo
  # echo "-------------------------------------------------------------------------"
  # echo "Installing ffmpeg-php extension relies on the ffmpeg-php developer"
  # echo "to keep ffmpeg-php updated for ffmpeg compatibility and that has"
  # echo "been flaky with various compatibility issues. There have been work"
  # echo "arounds like https://community.centminmod.com/posts/24018/ but "
  # echo "there are no guarantees due to issues outlined in this thread post"
  # echo "at https://community.centminmod.com/posts/7078/"
  # echo
  # echo "if ffmpeg-php fails to compile, you can unload it by removing the"
  # echo "settings file at /etc/centminmod/php.d/ffmpeg.ini and restarting"
  # echo "php-fpm service"
  # echo "-------------------------------------------------------------------------"
  echo
  read -ep "Do you want to continue with ffmpeg binary only install ? [y/n] " cont_install
  echo

if [[ "$cont_install" != [yY] ]]; then
  echo "aborting install..."
  exit 1
fi
}

install_nasm() {
  if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
    if [ -f /usr/bin/nasm ]; then
      if [[ "$(yum versionlock list | grep -o nasm)" = 'nasm' ]]; then
        yum versionlock delete nasm
      fi
      yum -y remove nasm --disableplugin=priorities
      hash -r
    fi
    cd ${OPT}/ffmpeg_sources
    curl -O -L "https://www.nasm.us/pub/nasm/releasebuilds/${NASM_VER}/nasm-${NASM_VER}.tar.gz"
    tar xvf "nasm-${NASM_VER}.tar.gz"
    cd "nasm-${NASM_VER}"
    make clean
    ./autogen.sh
    ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
    make${MAKETHREADS}
    make install
  else
    if [[ -f /usr/bin/nasm && "$(nasm --version | grep -o '2.13')" != '2.13' ]]; then
      yum -y remove nasm
      hash -r
    fi
    # install from official nasm yum repo
    yum-config-manager --add-repo https://www.nasm.us/nasm.repo
    yum -y install nasm --disableplugin=priorities
  fi
}

install_yasm() {
  cd ${OPT}/ffmpeg_sources
  curl -O -L "https://www.tortall.net/projects/yasm/releases/yasm-${YASM_VER}.tar.gz"
  tar xzvf "yasm-${YASM_VER}.tar.gz"
  cd "yasm-${YASM_VER}"
  make clean
  ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
  make${MAKETHREADS}
  make install
}

install_freetype() {
  cd ${OPT}/ffmpeg_sources
  curl -L -O "https://download.savannah.gnu.org/releases/freetype/freetype-${FREETYPE_VER}.tar.gz"
  tar xvzf "freetype-${FREETYPE_VER}.tar.gz"
  cd "freetype-${FREETYPE_VER}"
  make clean
  ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
  make${MAKETHREADS}
  make install
}

install_zimg() {
  cd ${OPT}/ffmpeg_sources
  rm -rf zimg
  git clone --depth=1 https://github.com/sekrit-twc/zimg
  cd zimg
  make clean
  ./autogen.sh
  ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static  --enable-shared --with-pic
  make${MAKETHREADS}
  make install
}

install() {

echo
echo "Installing FFMPEG..." 

# check if IUS Community git2u packages installed
if [[ "$(rpm -ql git2u-core | grep '\/usr\/bin\/git$')" = '/usr/bin/git' ]]; then
  yum -y install gperf autoconf automake cmake freetype-devel gcc gcc-c++ libtool make mercurial pkgconfig zlib-devel numactl-devel libass libass-devel opencv opencv-devel
else
  yum -y install gperf autoconf automake cmake freetype-devel gcc gcc-c++ git libtool make mercurial pkgconfig zlib-devel numactl-devel libass libass-devel opencv opencv-devel
fi

echo

mkdir -p /home/ffmpegtmp
chmod 1777 /home/ffmpegtmp
export TMPDIR=/home/ffmpegtmp

mkdir -p ${OPT}/ffmpeg_sources

# EPEL YUM Repo has yasm 1.2.0 already
# cd ${OPT}/ffmpeg_sources
# git clone --depth 1 git://github.com/yasm/yasm.git
# cd yasm
# autoreconf -fiv
# ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
# make${MAKETHREADS}
# make install
# make distclean

install_nasm
install_yasm
if [[ "$ENABLE_FONTCONFIG" = [yY] ]]; then
  install_freetype
fi
if [[ "$ENABLE_ZIMG" = [yY] ]]; then
  install_zimg
fi

if [[ "$ENABLE_FONTCONFIG" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources
rm -rf fontconfig*
# git clone --depth 1 https://gitlab.freedesktop.org/fontconfig/fontconfig
curl -L -O https://www.freedesktop.org/software/fontconfig/release/fontconfig-${FONTCONFIG_VER}.tar.gz
tar xvzf fontconfig-${FONTCONFIG_VER}.tar.gz
cd fontconfig-${FONTCONFIG_VER}
# autoreconf -ivf
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared --with-pic
make${MAKETHREADS}
make install
make distclean
fi

if [[ "$ENABLE_FBTRANSFORM" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources
rm -rf transform360
git clone --depth 1 https://github.com/facebook/transform360
cd transform360/Transform360
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" cmake -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON .
make${MAKETHREADS}
make install
fi

cd ${OPT}/ffmpeg_sources
rm -rf x264
git clone --depth 1 https://code.videolan.org/videolan/x264.git
cd x264
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" PATH="${OPT}/bin:$PATH" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
else
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make distclean

cd ${OPT}/ffmpeg_sources
rm -rf x265
git clone https://bitbucket.org/multicoreware/x265_git x265
cd ${OPT}/ffmpeg_sources/x265/build/linux
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="${OPT}/ffmpeg" -DENABLE_SHARED:bool=off -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../../source
# cmake -DCMAKE_INSTALL_PREFIX:PATH=${OPT}/ffmpeg -DENABLE_SHARED:bool=off ../../source
make${MAKETHREADS}
make install

cd ${OPT}/ffmpeg_sources
rm -rf fdk-aac
git clone https://github.com/mstorsjo/fdk-aac
cd fdk-aac
git checkout v${FDKAAC_VER} -b v${FDKAAC_VER}
autoreconf -fiv
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -L -O https://downloads.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz
tar xzvf lame-3.100.tar.gz
cd lame-3.100
./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-shared --enable-nasm
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
rm -rf opus
git clone https://github.com/xiph/opus
cd opus
autoreconf -fiv
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -O https://ftp.osuosl.org/pub/xiph/releases/ogg/libogg-${LIBOGG_VER}.tar.gz
tar xzvf libogg-${LIBOGG_VER}.tar.gz
cd libogg-${LIBOGG_VER}
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -O https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-${LIBVORBIS_VER}.tar.gz
tar xzvf libvorbis-${LIBVORBIS_VER}.tar.gz
cd libvorbis-${LIBVORBIS_VER}
LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" ./configure --prefix="${OPT}/ffmpeg" --with-ogg="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
rm -rf libvpx
git clone --depth 1 https://chromium.googlesource.com/webm/libvpx.git
cd libvpx
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  #yum -q -y install yasm-devel
  PATH="${OPT}/bin:$PATH" ./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared --disable-unit-tests --as=nasm
else
  ./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared --disable-unit-tests
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make clean

if [[ "$ENABLE_AVONE" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources
rm -rf libaom
rm -rf ${OPT}/ffmpeg_sources/aom_build
git clone --depth 1 https://aomedia.googlesource.com/aom libaom
mkdir -p ${OPT}/ffmpeg_sources/aom_build
cd aom_build
# build/cmake/aom_config_defaults.cmake
cmake3 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="${OPT}/ffmpeg" -DCMAKE_INSTALL_LIBDIR="lib" -DBUILD_SHARED_LIBS=1 -DENABLE_NASM=on -DENABLE_CCACHE=on ../libaom
make${MAKETHREADS}
make install
fi

if [[ "$ENABLE_DAVONED" = [yY] ]]; then
  cd ${OPT}/ffmpeg_sources
  if [[ ! "$(rpm -qa meson)" ]]; then
    yum -q -y install meson ninja-build SDL2-devel
  fi
  if [[ ! "$(rpm -qa ninja-build)" ]]; then
    yum -q -y install ninja-build
  fi
  if [[ ! "$(rpm -qa SDL2-devel)" ]]; then
    yum -q -y install SDL2-devel
  fi
  # if [[ ! "$(rpm -qa libplacebo-devel)" ]]; then
  #   yum -q -y install libplacebo-devel
  # fi
  
  cd ${OPT}/ffmpeg_sources
  rm -rf shaderc
  git clone --depth 1 https://github.com/google/shaderc
  mkdir -p "${OPT}/ffmpeg_sources/shaderc/build"
  cd "${OPT}/ffmpeg_sources/shaderc"
  ./utils/git-sync-deps
  cd "${OPT}/ffmpeg_sources/shaderc/build"
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" cmake3 -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${OPT}/ffmpeg" -DCMAKE_INSTALL_LIBDIR="${OPT}/ffmpeg/lib" ../
  ninja-build install
  
  cd ${OPT}/ffmpeg_sources
  rm -rf libplacebo
  git clone --depth 1 https://code.videolan.org/videolan/libplacebo.git
  mkdir -p libplacebo/build
  cd libplacebo
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" meson build --buildtype release --prefix="${OPT}/ffmpeg" --libdir="${OPT}/ffmpeg/lib"
  ninja-build -C build install
  
  cd ${OPT}/ffmpeg_sources
  rm -rf dav1d
  git clone --depth 1 https://code.videolan.org/videolan/dav1d.git
  cd dav1d
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" meson build --buildtype release --prefix="${OPT}/ffmpeg" --libdir="${OPT}/ffmpeg/lib"
  ninja-build -C build install
fi

cd ${OPT}/ffmpeg_sources
rm -rf ffmpeg
git clone --depth 1 git://source.ffmpeg.org/ffmpeg
cd ffmpeg
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="${EXTRACFLAG_FPICOPTS} -I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib${LDFLAG_FPIC}" --bindir="${OPT}/bin" --pkg-config-flags="--static" --extra-libs=-lpthread --extra-libs=-lm --enable-gpl${FFMPEG_DEBUGOPT} --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265${ENABLE_AVONEOPT}${ENABLE_DAVONEDOPT}${ENABLE_LIBASSOPT}${ENABLE_ZIMGOPT}${ENABLE_OPENCVOPT} --enable-swscale${ENABLE_FONTCONFIGOPT}${ENABLE_FPICOPT} --enable-shared${DISABLE_FFMPEGNETWORK}
else
  LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="${EXTRACFLAG_FPICOPTS} -I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib${LDFLAG_FPIC}" --bindir="${OPT}/bin" --pkg-config-flags="--static" --extra-libs=-lpthread --extra-libs=-lm --enable-gpl${FFMPEG_DEBUGOPT} --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265${ENABLE_AVONEOPT}${ENABLE_DAVONEDOPT}${ENABLE_LIBASSOPT}${ENABLE_ZIMGOPT}${ENABLE_OPENCVOPT} --enable-swscale${ENABLE_FONTCONFIGOPT}${ENABLE_FPICOPT} --enable-shared${DISABLE_FFMPEGNETWORK}
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make distclean
hash -r

unset TMPDIR

echo "${OPT}/ffmpeg/lib" > /etc/ld.so.conf.d/libavdevice.conf
cat /etc/ld.so.conf.d/libavdevice.conf
ldconfig

echo
echo "Installed FFMPEG binary at ${OPT}/bin/ffmpeg"
if [[ "$ENABLE_AVONE" = [yY] ]]; then
  echo
  echo "ffmpeg -h encoder=libaom-av1"
  ffmpeg -h encoder=libaom-av1
fi
echo
"${OPT}/bin/ffmpeg" -formats

echo
"${OPT}/bin/ffmpeg" -version

echo
echo "Binaries installed at ${OPT}/bin"
}

update() {

echo
echo "Updating FFMPEG..."

mkdir -p /home/ffmpegtmp
chmod 1777 /home/ffmpegtmp
export TMPDIR=/home/ffmpegtmp

rm -rf ~/ffmpeg ~/bin/{ffmpeg,ffprobe,ffserver,lame,vsyasm,x264,x265,yasm,ytasm}

# yum -y update yasm yasm-devel

install_nasm
install_yasm

# EPEL YUM Repo has yasm 1.2.0 already
# cd ${OPT}/ffmpeg_sources/yasm
# make distclean
# git pull
# ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
# make${MAKETHREADS}
# make install
# make distclean

if [[ "$ENABLE_FONTCONFIG" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources/fontconfig-${FONTCONFIG_VER}
make distclean
# autoreconf -ivf
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared --with-pic
make${MAKETHREADS}
make install
make distclean
fi

if [[ "$ENABLE_FBTRANSFORM" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources/transform360/Transform360
rm -rf CMakeCache.txt
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" cmake -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON .
make${MAKETHREADS}
make install
fi

cd ${OPT}/ffmpeg_sources/x264
make distclean
git pull
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" PATH="${OPT}/bin:$PATH" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
else
  PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make distclean

cd ${OPT}/ffmpeg_sources/x265
rm -rf ${OPT}/ffmpeg_sources/x265/build/linux/*
hg update
cd ${OPT}/ffmpeg_sources/x265/build/linux
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="${OPT}/ffmpeg" -DENABLE_SHARED:bool=off -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../../source
# cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="${OPT}/ffmpeg" -DENABLE_SHARED:bool=off ../../source
make${MAKETHREADS}
make install

cd ${OPT}/ffmpeg_sources/fdk_aac
make distclean
git pull
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources/libvpx
make clean
git pull
# ./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared --disable-unit-tests --enable-vp9-highbitdepth --as=yasm
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  #yum -q -y install yasm-devel
  PATH="${OPT}/bin:$PATH" ./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared --disable-unit-tests --as=nasm
else
  ./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared --disable-unit-tests
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make clean

if [[ "$ENABLE_AVONE" = [yY] ]]; then
cd ${OPT}/ffmpeg_sources/libaom
rm -rf CMakeCache.txt CMakeFiles
rm -rf ${OPT}/ffmpeg_sources/aom_build
mkdir -p ${OPT}/ffmpeg_sources/aom_build
git pull
cmake3 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="${OPT}/ffmpeg" -DBUILD_SHARED_LIBS=1 -DENABLE_NASM=on -DENABLE_CCACHE=on ../libaom
make${MAKETHREADS}
make install
fi

cd ${OPT}/ffmpeg_sources/ffmpeg
make distclean
git pull
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="${EXTRACFLAG_FPICOPTS} -I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib${LDFLAG_FPIC}" --bindir="${OPT}/bin" --pkg-config-flags="--static" --extra-libs=-lpthread --extra-libs=-lm --enable-gpl${FFMPEG_DEBUGOPT} --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265${ENABLE_AVONEOPT}${ENABLE_DAVONEDOPT}${ENABLE_LIBASSOPT}${ENABLE_ZIMGOPT}${ENABLE_OPENCVOPT} --enable-swscale${ENABLE_FONTCONFIGOPT}${ENABLE_FPICOPT} --enable-shared
else
  LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="${EXTRACFLAG_FPICOPTS} -I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib${LDFLAG_FPIC}" --bindir="${OPT}/bin" --pkg-config-flags="--static" --extra-libs=-lpthread --extra-libs=-lm --enable-gpl${FFMPEG_DEBUGOPT} --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265${ENABLE_AVONEOPT}${ENABLE_DAVONEDOPT}${ENABLE_LIBASSOPT}${ENABLE_ZIMGOPT}${ENABLE_OPENCVOPT} --enable-swscale${ENABLE_FONTCONFIGOPT}${ENABLE_FPICOPT} --enable-shared
fi
if [[ "$NASM_SOURCEINSTALL" = [yY] ]]; then
  PATH="${OPT}/bin:$PATH" make${MAKETHREADS}
else
  make${MAKETHREADS}
fi
make install
make distclean
hash -r

unset TMPDIR

echo
echo "FFMPEG updated"

echo ""
echo "check FFMPEG"
echo ""

echo
/opt/bin/ffmpeg -version

echo
/opt/bin/ffmpeg -formats
  
}

phpext_no() {
  echo
  echo "The ffmpeg php extension is no longer supported due to developer support ending"
  echo "If you have ran centmin.sh menu option 19 submenu option 1, then you have installed"
  echo "ffmpeg binary at /opt/bin/ffmpeg and can use that with some web apps only requiring"
  echo "pointing to the full path of ffmpeg binary at /opt/bin/ffmpeg"
  echo
  if [ -f "${CONFIGSCANDIR}/ffmpeg.ini" ]; then
    rm -f "${CONFIGSCANDIR}/ffmpeg.ini"
  fi
}

phpext() {

  FFMPEG_PHPVERCHECK=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1)
  if [[ "$FFMPEG_PHPVERCHECK" != '7' ]]; then
  echo
  echo "Install FFMPEG PHP extension..."

  cd $DIR_TMP
  rm -rf ffmpeg-php-git
  git clone https://github.com/centminmod/ffmpeg-php ffmpeg-php-git
  cd ffmpeg-php-git
  # cd ffmpeg-php-${FFMPEGVER}
  
  make clean
  phpize -clean
  if [[ "$GD_ENABLE" = [yY] ]]; then
    sed -i 's/PIX_FMT_RGBA32/PIX_FMT_RGB32/g' ffmpeg_frame.c
    GDOPT=' --enable-skip-gd-check'
  else
    GDOPT=""
  fi
  phpize
  
  # mkdir -p /usr/local/include
  # cd /opt/ffmpeg/include
  # find . -name "*.h" -exec cp {} /usr/local/include \;
  # find . -name "*.h" -exec cp {} /opt/ffmpeg/include \;
  # ls -lah /usr/local/include
  # ls -lah /opt/ffmpeg/include

# echo "/usr/local/include" > /etc/ld.so.conf.d/ffmpeg-include.conf
# cat /etc/ld.so.conf.d/ffmpeg-include.conf
# ldconfig

  # mkdir -p /usr/local/include/libavcodec
  # ln -s /usr/local/include/avcodec.h /usr/local/include/libavcodec/avcodec.h

# ln -s /opt/ffmpeg/include/libavcodec /usr/local/include/libavcodec
# ln -s /opt/ffmpeg/include/libavformat /usr/local/include/libavformat
# ln -s /opt/ffmpeg/include/libavutil /usr/local/include/libavutil

  cd $DIR_TMP/ffmpeg-php-git
  # http://stackoverflow.com/a/14947692/272648
  
  LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" ./configure --with-php-config=/usr/local/bin/php-config --with-ffmpeg=/opt/ffmpeg${GDOPT}
  
  # mv /opt/ffmpeg/include/libavutil/time.h /opt/ffmpeg/include/libavutil/time.h_
  make${MAKETHREADS}
  # mv /opt/ffmpeg/include/libavutil/time.h /opt/ffmpeg/include/libavutil/time.h_
  make install
  
  FFMPEGCHECK=`grep 'extension=ffmpeg.so' ${CONFIGSCANDIR}/ffmpeg.ini `
  if [ -z "$FFMPEGCHECK" ]; then
    echo "" >> ${CONFIGSCANDIR}/ffmpeg.ini
    echo "[ffmpeg]" >> ${CONFIGSCANDIR}/ffmpeg.ini
    echo "extension=ffmpeg.so" >> ${CONFIGSCANDIR}/ffmpeg.ini
  fi

  echo
  echo "FFMPEG PHP Extension installed" 
  echo "restarting php-fpm service ..."
  echo ""

  service php-fpm restart 

  echo ""
  echo "check phpinfo for FFMPEG PHP Extension..."
  echo ""

  php --ri ffmpeg

  ffmpeg_err=$?
  if [[ "$ffmpeg_err" -ne '0' ]]; then
    rm -rf /etc/centminmod/php.d/ffmpeg.ini
    service php-fpm restart
    echo
    echo "----------------------------------------------------------------"
    echo "FAILED..."
    echo "failed to install FFMPEG PHP Extension"
    echo "FFMPEG PHP Extension has a high change of install failure"
    echo "due to developer abandoning the project so if failed"
    echo "the FFMPEG PHP Extension is not installable"
    echo "----------------------------------------------------------------"
    echo
  fi

  else
    echo ""
    echo "FFMPEG php extension does not support PHP 7.x"
    echo ""
  fi # php version check

}

case "$1" in
  install )
    starttime=$(TZ=UTC date +%s.%N)
    {
    do_continue
    install
    # phpext
    } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
echo "Total FFMPEG Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
    ;;
  update )
    starttime=$(TZ=UTC date +%s.%N)
    {
    do_continue
    update
    } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
echo "Total FFMPEG Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
    ;;    
  php )
    starttime=$(TZ=UTC date +%s.%N)
    {
    # php_quite=$2
    # if [[ "$php_quite" != 'silent' ]]; then
      # do_continue
    # fi
    phpext_no
    } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log
echo "Total FFMPEG PHP Extension Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log
    ;;      
  * )
    echo "$0 {install|update|php}"
    ;;
esac