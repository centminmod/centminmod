function funct_pigzinstall {

if [ ! -f /usr/bin/pigz ]; then

echo ""
echo "*************************************************"
cecho "* pigz install " $boldgreen
echo "*************************************************"

cd /svr-setup

if [ -s pigz-2.3.tar.gz ]; then
  echo "pigz-2.3.tar.gz [found]"
  else
  echo "Error: pigz-2.3.tar.gz not found!!! download now......"
  wget -c --no-check-certificate https://github.com/madler/pigz/tarball/v2.3 -O pigz-2.3.tar.gz --tries=3
fi

tar xvzf pigz-2.3.tar.gz
#cd pigz-2.3
cd madler-pigz*
rm -rf Makefile
wget -c --no-check-certificate https://raw.github.com/madler/pigz/5316bf3440025ca37d84d7580a8f616c0127668a/Makefile -O Makefile
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

cd /svr-setup

if [ -s pbzip2-1.1.6.tar.gz ]; then
  echo "pbzip2-1.1.6.tar.gz [found]"
  else
  echo "Error: pbzip2-1.1.6.tar.gz not found!!!download now......"
  wget -c http://compression.ca/pbzip2/pbzip2-1.1.6.tar.gz --tries=3
fi

tar xvzf pbzip2-1.1.6.tar.gz
cd pbzip2-1.1.6
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

cd /svr-setup

if [ -s lbzip2-2.2.tar.gz ]; then
  echo "lbzip2-2.2.tar.gz [found]"
  else
  echo "Error: lbzip2-2.2.tar.gz not found!!!download now......"
  wget -c --no-check-certificate https://github.com/downloads/kjn/lbzip2/lbzip2-2.2.tar.gz --tries=3
fi

tar xvzf lbzip2-2.2.tar.gz
cd lbzip2-2.2
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

cd /svr-setup

if [ -s lzip-1.14.tar.gz ]; then
  echo "lzip-1.14.tar.gz [found]"
  else
  echo "Error: lzip-1.14.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/lzip-1.14.tar.gz --tries=3
fi

tar xvzf lzip-1.14.tar.gz
cd lzip-1.14
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

cd /svr-setup

if [ -s lzlib-1.4.tar.gz ]; then
  echo "lzlib-1.4.tar.gz [found]"
  else
  echo "Error: lzlib-1.4.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/lzlib-1.4.tar.gz --tries=3
fi

if [ -s plzip-1.0.tar.gz ]; then
  echo "plzip-1.0.tar.gz [found]"
  else
  echo "Error: plzip-1.0.tar.gz not found!!!download now......"
  wget -c http://download.savannah.gnu.org/releases/lzip/plzip-1.0.tar.gz --tries=3
fi

tar xvzf lzlib-1.4.tar.gz
cd lzlib-1.4
./configure
make
make install

cd ../

tar xvzf plzip-1.0.tar.gz
cd plzip-1.0
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

cd /svr-setup

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
