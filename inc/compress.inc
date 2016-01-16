PIGZ_VER='2.3.3'
PBZIP_VER='1.1.13'
LBZIP_VER='2.5'
LZIP_VER='1.17'
LZLIB_VER='1.7'
PLZIP_VER='1.4'
PZIP_VER='9.38.1'

function funct_pigzinstall {
  if [[ ! -f /usr/bin/pigz || ! -f "${DIR_TMP}/pigz-${PIGZ_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* pigz install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s pigz-${PIGZ_VER}.tar.gz ]; then
      echo "pigz-${PIGZ_VER}.tar.gz [found]"
      else
      echo "Error: pigz-${PIGZ_VER}.tar.gz not found!!!  Downloading now......"
      wget -c --no-check-certificate http://zlib.net/pigz/pigz-${PIGZ_VER}.tar.gz --tries=3
    fi
    
    tar xzf pigz-${PIGZ_VER}.tar.gz
    cd pigz-${PIGZ_VER}
    make clean
    # rm -rf Makefile
    # wget -c --no-check-certificate https://raw.github.com/madler/pigz/5316bf3440025ca37d84d7580a8f616c0127668a/Makefile -O Makefile
    make
    \cp -f pigz unpigz /usr/bin
    ls -lh /usr/bin | grep pigz
  elif [ -f /usr/bin/pigz ]; then
    echo "pigz already installed"
  fi
}

function funct_pbzip2install {
  if [[ ! -f /usr/bin/pbzip2 || ! -f "${DIR_TMP}/pbzip2-${PBZIP_VER}.tar.gz" ]]; then
    echo ""
    echo "*************************************************"
    cecho "* pbzip2 install " $boldgreen
    echo "*************************************************"
    
    cd ${DIR_TMP}
    
    if [ -s pbzip2-${PBZIP_VER}.tar.gz ]; then
      echo "pbzip2-${PBZIP_VER}.tar.gz [found]"
      else
      echo "Error: pbzip2-${PBZIP_VER}.tar.gz not found!!! Downloading now......"
      # wget -c http://compression.ca/pbzip2/pbzip2-${PBZIP_VER}.tar.gz --tries=3
      wget -c https://launchpad.net/pbzip2/1.1/${PBZIP_VER}/+download/pbzip2-${PBZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf pbzip2-${PBZIP_VER}.tar.gz
    cd pbzip2-${PBZIP_VER}
    make clean
    make
    \cp -f pbzip2 /usr/bin
  elif [ -f /usr/bin/pbzip2 ]; then
    echo "pbzip2 already installed"
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
      wget -c --no-check-certificate http://archive.lbzip2.org/lbzip2-${LBZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lbzip2-${LBZIP_VER}.tar.gz
    cd lbzip2-${LBZIP_VER}
    make clean
    ./configure
    make
    make install
  elif [ -f /usr/local/bin/lbzip2 ]; then
    echo "lbzip2 already installed"
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
      wget -c http://download.savannah.gnu.org/releases/lzip/lzip-${LZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lzip-${LZIP_VER}.tar.gz
    cd lzip-${LZIP_VER}
    make clean
    ./configure
    make
    make install
  elif [ -f /usr/local/bin/lzip ]; then
    echo "lzip already installed"
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
      wget -c http://download.savannah.gnu.org/releases/lzip/lzlib/lzlib-${LZLIB_VER}.tar.gz --tries=3
    fi
    
    if [ -s plzip-${PLZIP_VER}.tar.gz ]; then
      echo "plzip-${PLZIP_VER}.tar.gz [found]"
      else
      echo "Error: plzip-${PLZIP_VER}.tar.gz not found!!! Downloading now......"
      wget -c http://download.savannah.gnu.org/releases/lzip/plzip/plzip-${PLZIP_VER}.tar.gz --tries=3
    fi
    
    tar xzf lzlib-${LZLIB_VER}.tar.gz
    cd lzlib-${LZLIB_VER}
    make clean
    ./configure
    make
    make install
    
    cd ../
    
    tar xzf plzip-${PLZIP_VER}.tar.gz
    cd plzip-${PLZIP_VER}
    make clean
    ./configure
    make
    make install
  elif [ -f /usr/local/bin/plzip ]; then
    echo "plzip already installed"
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
      wget -c -O p7zip_${PZIP_VER}_src_all.tar.bz2 http://sourceforge.net/projects/p7zip/files/p7zip/${PZIP_VER}/p7zip_${PZIP_VER}_src_all.tar.bz2/download?use_mirror=softlayer-sng --tries=3
    fi
    
    bzip2 -d p7zip_${PZIP_VER}_src_all.tar.bz2
    tar xvf p7zip_${PZIP_VER}_src_all.tar
    cd p7zip_${PZIP_VER}
    make clean
    ./install.sh
    make
    make install
  elif [ -f /usr/local/bin/7za ]; then
    echo "p7zip already installed"
  fi
}
