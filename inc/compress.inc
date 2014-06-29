function funct_pigzinstall {

if [ ! -f /usr/bin/pigz ]; then

echo ""
echo "*************************************************"
cecho "* pigz install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s pigz-2.3.1.tar.gz ]; then
  echo "pigz-2.3.1.tar.gz [found]"
  else
  echo "Error: pigz-2.3.1.tar.gz not found!!! download now......"
  wget -c --no-check-certificate http://zlib.net/pigz/pigz-2.3.1.tar.gz --tries=3
fi

tar xvzf pigz-2.3.1.tar.gz
#cd pigz-2.3.1
cd pigz-2.3.1
# rm -rf Makefile
# wget -c --no-check-certificate https://raw.github.com/madler/pigz/5316bf3440025ca37d84d7580a8f616c0127668a/Makefile -O Makefile
make
cp pigz unpigz /usr/bin
ls -lh /usr/bin | grep pigz

fi

}

function funct_pbzip2install {

if [ ! -f /usr/bin/pbzip2 ]; then

echo ""
echo "*************************************************"
cecho "* pbzip2 install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s pbzip2-1.1.8.tar.gz ]; then
  echo "pbzip2-1.1.8.tar.gz [found]"
  else
  echo "Error: pbzip2-1.1.8.tar.gz not found!!!download now......"
  wget -c http://compression.ca/pbzip2/pbzip2-1.1.8.tar.gz --tries=3
fi

tar xvzf pbzip2-1.1.8.tar.gz
cd pbzip2-1.1.8
make
cp pbzip2 /usr/bin

fi

}

function funct_lbzip2install {

if [ ! -f /usr/local/bin/lbzip2 ]; then

echo ""
echo "*************************************************"
cecho "* lbzip2 install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s lbzip2-2.3.tar.gz ]; then
  echo "lbzip2-2.3.tar.gz [found]"
  else
  echo "Error: lbzip2-2.3.tar.gz not found!!!download now......"
  wget -c --no-check-certificate http://archive.lbzip2.org/lbzip2-2.3.tar.gz --tries=3
fi

tar xvzf lbzip2-2.3.tar.gz
cd lbzip2-2.3
./configure
make
make install

fi

}

function funct_lzipinstall {

if [ ! -f /usr/local/bin/lzip ]; then

echo ""
echo "*************************************************"
cecho "* lzip install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s lzip-1.15.tar.gz ]; then
  echo "lzip-1.15.tar.gz [found]"
  else
  echo "Error: lzip-1.15.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/lzip-1.15.tar.gz --tries=3
fi

tar xvzf lzip-1.15.tar.gz
cd lzip-1.15
./configure
make
make install

fi

}

function funct_plzipinstall {

if [ ! -f /usr/local/bin/plzip ]; then

echo ""
echo "*************************************************"
cecho "* plzip install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s lzlib-1.5.tar.gz ]; then
  echo "lzlib-1.5.tar.gz [found]"
  else
  echo "Error: lzlib-1.5.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/lzlib-1.5.tar.gz --tries=3
fi

if [ -s plzip-1.1.tar.gz ]; then
  echo "plzip-1.1.tar.gz [found]"
  else
  echo "Error: plzip-1.1.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/plzip-1.1.tar.gz --tries=3
fi

tar xvzf lzlib-1.5.tar.gz
cd lzlib-1.5
./configure
make
make install

cd ../

tar xvzf plzip-1.1.tar.gz
cd plzip-1.1
./configure
make
make install

fi

}

function funct_p7zipinstall {

if [ ! -f /usr/local/bin/7za ]; then

echo ""
echo "*************************************************"
cecho "* p7zip install " $boldgreen
echo "*************************************************"

cd ${DIR_TMP}

if [ -s p7zip_9.20.1_src_all.tar.bz2 ]; then
  echo "p7zip_9.20.1_src_all.tar.bz2 [found]"
  else
  echo "Error: p7zip_9.20.1_src_all.tar.bz2 not found!!!download now......"
  wget -c http://aarnet.dl.sourceforge.net/project/p7zip/p7zip/9.20.1/p7zip_9.20.1_src_all.tar.bz2 --tries=3
fi

bzip2 -d p7zip_9.20.1_src_all.tar.bz2
tar xvf p7zip_9.20.1_src_all.tar
cd p7zip_9.20.1
./install.sh
make
make install

fi

}
