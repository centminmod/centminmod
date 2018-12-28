ZSTD_VER='1.3.8'
LZFOUR_VER='1.8.3'
PIGZ_VER='2.4'
PBZIP_VER='1.1.13'
LBZIP_VER='2.5'
LZIP_VER='1.20'
LZLIB_VER='1.10'
PLZIP_VER='1.7'
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
}

compressmenu_notice() {
  echo
  echo "Multi-thread compression Install"
  echo
  echo "The following compression tools will be installed"
  echo "
  1. pigz
  2. pbzip2
  3. lbzip2
  4. plzip
  5. zstd
  6. lz4"
  echo
  compgcc_seven
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
    make -j$(nproc)
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
      wget -c${ipv_forceopt} http://archive.lbzip2.org/lbzip2-${LBZIP_VER}.tar.gz --tries=3
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
      wget -c${ipv_forceopt} http://nongnu.askapache.com/lzip/lzip-${LZIP_VER}.tar.gz --tries=3
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
      wget -c${ipv_forceopt} http://nongnu.askapache.com/lzip/lzlib/lzlib-${LZLIB_VER}.tar.gz --tries=3
    fi
    
    if [ -s plzip-${PLZIP_VER}.tar.gz ]; then
      echo "plzip-${PLZIP_VER}.tar.gz [found]"
      else
      echo "Error: plzip-${PLZIP_VER}.tar.gz not found!!! Downloading now......"
      wget -c${ipv_forceopt} http://nongnu.askapache.com/lzip/plzip/plzip-${PLZIP_VER}.tar.gz --tries=3
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
