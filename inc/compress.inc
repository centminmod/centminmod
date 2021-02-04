ZSTD_VER='1.4.8'
LZFOUR_VER='1.9.2'
PIGZ_VER='2.4'
PBZIP_VER='1.1.13'
LBZIP_VER='2.5'
LZIP_VER='1.21'
LZLIB_VER='1.11'
PLZIP_VER='1.8'
PZIP_VER='15.14.1'

compgcc_seven() {
  if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
    label='gcc7 built'
    source /opt/rh/devtoolset-7/enable
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  else
    label=""
  fi
  COMP_CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
}

compgcc_eight() {
  if [[ -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
    label='gcc8 built'
    source /opt/rh/devtoolset-8/enable
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  else
    label=""
  fi
  COMP_CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
}

compgcc_nine() {
  if [[ -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
    label='gcc9 built'
    source /opt/rh/devtoolset-9/enable
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  else
    label=""
  fi
  COMP_CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
}

pgo_export_compress() {
  if [[ "$GCC_FALIGN_FUNCTION" = [yY] ]] && [[ "$COMP_CPU_MARCH" = 'nehalem' || "$COMP_CPU_MARCH" = 'westmere' || "$COMP_CPU_MARCH" = 'core2' || "$COMP_CPU_MARCH" = 'bonnell' || "$COMP_CPU_MARCH" = 'silvermont' ]]; then
    COMP_FALIGN_FUCTION_OPT=''
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -gt '40805' && "$GCC_FALIGN_FUNCTION" = [yY] ]]; then
    COMP_FALIGN_FUCTION_OPT=' -falign-functions=32 -malign-data=cacheline'
  else
    COMP_FALIGN_FUCTION_OPT=''
  fi
export CFLAGS="$CFLAGS -O3${COMP_FALIGN_FUCTION_OPT} -ffat-lto-objects -flto=$(nproc) -fno-math-errno -fno-semantic-interposition -fno-trapping-math "
export FCFLAGS="$CFLAGS -O3${COMP_FALIGN_FUCTION_OPT} -ffat-lto-objects -flto=$(nproc) -fno-math-errno -fno-semantic-interposition -fno-trapping-math "
export FFLAGS="$CFLAGS -O3${COMP_FALIGN_FUCTION_OPT} -ffat-lto-objects -flto=$(nproc) -fno-math-errno -fno-semantic-interposition -fno-trapping-math "
export CXXFLAGS="$CXXFLAGS -O3${COMP_FALIGN_FUCTION_OPT} -ffat-lto-objects -flto=$(nproc) -fno-math-errno -fno-semantic-interposition -fno-trapping-math "
export CFLAGS_GENERATE="$CFLAGS -fprofile-generate -fprofile-dir=/home/pgo_compress -fprofile-update=atomic "
export FCFLAGS_GENERATE="$FCFLAGS -fprofile-generate -fprofile-dir=/home/pgo_compress -fprofile-update=atomic "
export FFLAGS_GENERATE="$FFLAGS -fprofile-generate -fprofile-dir=/home/pgo_compress -fprofile-update=atomic "
export CXXFLAGS_GENERATE="$CXXFLAGS -fprofile-generate -fprofile-dir=/home/pgo_compress -fprofile-update=atomic "
export LDFLAGS_GENERATE="$LDFLAGS -fprofile-generate -fprofile-dir=/home/pgo_compress -fprofile-update=atomic "
export CFLAGS_USE="$CFLAGS${COMP_FALIGN_FUCTION_OPT} -fprofile-use -fprofile-dir=/home/pgo_compress -fprofile-correction "
export FCFLAGS_USE="$FCFLAGS${COMP_FALIGN_FUCTION_OPT} -fprofile-use -fprofile-dir=/home/pgo_compress -fprofile-correction "
export FFLAGS_USE="$FFLAGS${COMP_FALIGN_FUCTION_OPT} -fprofile-use -fprofile-dir=/home/pgo_compress -fprofile-correction "
export CXXFLAGS_USE="$CXXFLAGS${COMP_FALIGN_FUCTION_OPT} -fprofile-use -fprofile-dir=/home/pgo_compress -fprofile-correction "
export LDFLAGS_USE="$LDFLAGS${COMP_FALIGN_FUCTION_OPT} -fprofile-use -fprofile-dir=/home/pgo_compress -fprofile-correction " 
}

compressmenu_notice() {
  echo
  echo "Multi-thread compression Install"
  echo
  echo "The following compression tools will be installed"
  echo "
  1. pigz
  2. pbzip2
  3. plzip
  4. zstd
  5. lz4"
  echo
  compgcc_seven
  compgcc_eight
  compgcc_nine
  pgo_export_compress
}

zstdinstall() {
  if [[ ! -f /usr/local/bin/zstd || ! -f "${DIR_TMP}/zstd-${ZSTD_VER}.tar.gz" ]]; then
    cd ${DIR_TMP}
    if [ -s zstd-${ZSTD_VER}.tar.gz ]; then
      echo "zstd-${ZSTD_VER}.tar.gz [found]"
      else
      echo "Error: zstd-${ZSTD_VER}.tar.gz not found!!!  Downloading now......"
      wget -c${ipv_forceopt} https://github.com/facebook/zstd/archive/v${ZSTD_VER}.tar.gz -O zstd-${ZSTD_VER}.tar.gz --tries=3
    fi
    tar xvzf zstd-${ZSTD_VER}.tar.gz
    cd zstd-${ZSTD_VER}
    make clean
    make -j$(nproc)
    make install
    if [ -f /usr/local/bin/zstd ]; then
      cecho "zstd installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/zstd ]; then
    cecho "zstd already installed" $boldyellow
  fi
}

lzfourinstall() {
  if [[ ! -f /usr/local/bin/lz4 || ! -f "${DIR_TMP}/lz4-${LZFOUR_VER}.tar.gz" ]]; then
    cd ${DIR_TMP}
    if [ -s lz4-${LZFOUR_VER}.tar.gz ]; then
      echo "lz4-${LZFOUR_VER}.tar.gz [found]"
      else
      echo "Error: lz4-${LZFOUR_VER}.tar.gz not found!!!  Downloading now......"
      wget -c${ipv_forceopt} https://github.com/lz4/lz4/archive/v${LZFOUR_VER}.tar.gz -O lz4-${LZFOUR_VER}.tar.gz --tries=3
    fi
    tar xvzf lz4-${LZFOUR_VER}.tar.gz
    cd lz4-${LZFOUR_VER}
    make clean
    rm -f /usr/local/bin/lz4* /usr/local/bin/unlz4*
    make -j$(nproc)
    make install
    if [ -f /usr/local/bin/lz4 ]; then
      cecho "lz4 installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/lz4 ]; then
    cecho "lz4 already installed" $boldyellow
  fi
}

function funct_pigzinstall {
  if [[ ! -f /usr/local/bin/pigz || ! -f "${DIR_TMP}/pigz-${PIGZ_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* pigz install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s pigz-${PIGZ_VER}.tar.gz ]; then
      echo "pigz-${PIGZ_VER}.tar.gz [found]"
      else
      echo "Error: pigz-${PIGZ_VER}.tar.gz not found!!!  Downloading now......"
      wget -c${ipv_forceopt} https://github.com/madler/pigz/tarball/v${PIGZ_VER} -O pigz-${PIGZ_VER}.tar.gz --tries=3
    fi
    
    tar xzf pigz-${PIGZ_VER}.tar.gz
    cd madler-pigz*
    make clean
    # rm -rf Makefile
    # wget -c${ipv_forceopt} https://raw.github.com/madler/pigz/5316bf3440025ca37d84d7580a8f616c0127668a/Makefile -O Makefile
    # export CFLAGS="${CFLAGS_GENERATE}" 
    # export CXXFLAGS="${CXXFLAGS_GENERATE}" 
    # export FFLAGS="${FFLAGS_GENERATE}" 
    # export FCFLAGS="${FCFLAGS_GENERATE}" 
    # export LDFLAGS="${LDFLAGS_GENERATE}"
    CFLAGS="${CFLAGS_GENERATE}" CXXFLAGS="${CXXFLAGS_GENERATE}" FFLAGS="${FFLAGS_GENERATE}" FCFLAGS="${FCFLAGS_GENERATE}" LDFLAGS="${LDFLAGS_GENERATE}"
    make -j$(nproc)
    make tests -j$(nproc) CFLAGS="${CFLAGS_GENERATE}" CXXFLAGS="${CXXFLAGS_GENERATE}" FFLAGS="${FFLAGS_GENERATE}" FCFLAGS="${FCFLAGS_GENERATE}" LDFLAGS="${LDFLAGS_GENERATE}"
    make clean
    CFLAGS="${CFLAGS_USE}" CXXFLAGS="${CXXFLAGS_USE}" FFLAGS="${FFLAGS_USE}" FCFLAGS="${FCFLAGS_USE}" LDFLAGS="${LDFLAGS_USE}"
    make -j$(nproc) CFLAGS="${CFLAGS_USE}" CXXFLAGS="${CXXFLAGS_USE}" FFLAGS="${FFLAGS_USE}" FCFLAGS="${FCFLAGS_USE}" LDFLAGS="${LDFLAGS_USE}"
    \cp -f pigz unpigz /usr/local/bin
    ls -lh /usr/local/bin | grep pigz
    if [ -f /usr/local/bin/pigz ]; then
      cecho "pigz installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/pigz ]; then
    cecho "pigz already installed" $boldyellow
  fi
}

function funct_pbzip2install {
  if [[ ! -f /usr/local/bin/pbzip2 || ! -f "${DIR_TMP}/pbzip2-${PBZIP_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* pbzip2 install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s pbzip2-${PBZIP_VER}.tar.gz ]; then
      echo "pbzip2-${PBZIP_VER}.tar.gz [found]"
      else
      echo "Error: pbzip2-${PBZIP_VER}.tar.gz not found!!! Downloading now......"
      # wget -c${ipv_forceopt} http://compression.ca/pbzip2/pbzip2-${PBZIP_VER}.tar.gz --tries=3
      wget -c${ipv_forceopt} https://launchpad.net/pbzip2/1.1/${PBZIP_VER}/+download/pbzip2-${PBZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf pbzip2-${PBZIP_VER}.tar.gz
    cd pbzip2-${PBZIP_VER}
    make clean
    make -j$(nproc)
    \cp -f pbzip2 /usr/local/bin
    if [ -f /usr/local/bin/pbzip2 ]; then
      cecho "pbzip2 installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/pbzip2 ]; then
    cecho "pbzip2 already installed" $boldyellow
  fi
}

function funct_lbzip2install {
  if [[ ! -f /usr/local/bin/lbzip2 || ! -f "${DIR_TMP}/lbzip2-${LBZIP_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* lbzip2 install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s lbzip2-${LBZIP_VER}.tar.gz ]; then
      echo "lbzip2-${LBZIP_VER}.tar.gz [found]"
      else
      echo "Error: lbzip2-${LBZIP_VER}.tar.gz not found!!! Downloading now......"
      wget -c${ipv_forceopt} https://github.com/kjn/lbzip2/archive/v${LBZIP_VER}.tar.gz -O lbzip2-${LBZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lbzip2-${LBZIP_VER}.tar.gz
    cd lbzip2-${LBZIP_VER}
    make clean
    ./configure
    make -j$(nproc)
    make install
    if [ -f /usr/local/bin/lbzip2 ]; then
      cecho "lbzip2 installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/lbzip2 ]; then
    cecho "lbzip2 already installed" $boldyellow
  fi
}

function funct_lzipinstall {
  if [[ ! -f /usr/local/bin/lzip || ! -f "${DIR_TMP}/lzip-${LZIP_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* lzip install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s lzip-${LZIP_VER}.tar.gz ]; then
      echo "lzip-${LZIP_VER}.tar.gz [found]"
      else
      echo "Error: lzip-${LZIP_VER}.tar.gz not found!!! Downloading now......"
      wget -c${ipv_forceopt} https://nongnu.askapache.com/lzip/lzip-${LZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lzip-${LZIP_VER}.tar.gz
    cd lzip-${LZIP_VER}
    make clean
    ./configure
    make -j$(nproc)
    make install
    if [ -f /usr/local/bin/lzip ]; then
      cecho "lzip installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/lzip ]; then
    cecho "lzip already installed" $boldyellow
  fi
}

function funct_plzipinstall {
  if [[ ! -f /usr/local/bin/plzip || ! -f "${DIR_TMP}/lzlib-${LZLIB_VER}.tar.gz" || ! -f "${DIR_TMP}/plzip-${PLZIP_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* plzip install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s lzlib-${LZLIB_VER}.tar.gz ]; then
      echo "lzlib-${LZLIB_VER}.tar.gz [found]"
      else
      echo "Error: lzlib-${LZLIB_VER}.tar.gz not found!!! Downloading now......"
      wget -c${ipv_forceopt} https://nongnu.askapache.com/lzip/lzlib/lzlib-${LZLIB_VER}.tar.gz --tries=3
    fi
    
    if [ -s plzip-${PLZIP_VER}.tar.gz ]; then
      echo "plzip-${PLZIP_VER}.tar.gz [found]"
      else
      echo "Error: plzip-${PLZIP_VER}.tar.gz not found!!! Downloading now......"
      wget -c${ipv_forceopt} https://nongnu.askapache.com/lzip/plzip/plzip-${PLZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lzlib-${LZLIB_VER}.tar.gz
    cd lzlib-${LZLIB_VER}
    make clean
    ./configure
    make -j$(nproc)
    make install
    
    cd ../
    
    tar xzf plzip-${PLZIP_VER}.tar.gz
    cd plzip-${PLZIP_VER}
    make clean
    ./configure
    make -j$(nproc)
    make install

    if [ -f /usr/local/bin/plzip ]; then
     cecho "plzip installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/plzip ]; then
    cecho "plzip already installed" $boldyellow
  fi
}

function funct_p7zipinstall {
  if [[ ! -f /usr/local/bin/7za || ! -f "${DIR_TMP}/p7zip_${PZIP_VER}_src_all.tar.bz2" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* p7zip install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s p7zip_${PZIP_VER}_src_all.tar.bz2 ]; then
      echo "p7zip_${PZIP_VER}_src_all.tar.bz2 [found]"
      else
      echo "Error: p7zip_${PZIP_VER}_src_all.tar.bz2 not found!!! Downloading now......"
      wget -c${ipv_forceopt} -O p7zip_${PZIP_VER}_src_all.tar.bz2 http://sourceforge.net/projects/p7zip/files/p7zip/${PZIP_VER}/p7zip_${PZIP_VER}_src_all.tar.bz2/download?use_mirror=softlayer-sng --tries=3
    fi
    
    bzip2 -d p7zip_${PZIP_VER}_src_all.tar.bz2
    tar xvf p7zip_${PZIP_VER}_src_all.tar
    cd p7zip_${PZIP_VER}
    make clean
    ./install.sh
    make -j$(nproc)
    make install
    if [ -f /usr/local/bin/7za ]; then
      cecho "p7zip installed" $boldyellow
    fi
  elif [ -f /usr/local/bin/7za ]; then
    cecho "p7zip already installed" $boldyellow
  fi
}
