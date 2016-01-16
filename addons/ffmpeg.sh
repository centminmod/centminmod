#!/bin/bash
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

DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
CONFIGSCANDIR='/etc/centminmod/php.d'
###############################################################################
DISABLE_NETWORKFFMPEG='y'
# http://downloads.xiph.org/releases/ogg/
LIBOGG_VER='1.3.2'
# http://downloads.xiph.org/releases/vorbis/
LIBVORBIS_VER='1.3.5'
###############################################################################
if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=`cat "/proc/cpuinfo" | grep "processor"|wc -l`
    CPUS=$(echo $CPUS+1 | bc)
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=`cat "/proc/cpuinfo" | grep "processor"|wc -l`
    CPUS=$(echo $CPUS+1 | bc)
    MAKETHREADS=" -j$CPUS"
fi

if [[ "$DISABLE_NETWORKFFMPEG" = [yY] ]]; then
	DISABLE_FFMPEGNETWORK=' --disable-network'
fi

install() {

echo
echo "Installing FFMPEG..."	

yum -y install autoconf automake cmake freetype-devel gcc gcc-c++ git libtool make mercurial nasm pkgconfig zlib-devel yasm yasm-devel numactl-devel

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

cd ${OPT}/ffmpeg_sources
git clone --depth 1 git://git.videolan.org/x264
cd x264
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
hg clone https://bitbucket.org/multicoreware/x265
cd ${OPT}/ffmpeg_sources/x265/build/linux
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="${OPT}/ffmpeg" -DENABLE_SHARED:bool=off -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../../source
# cmake -DCMAKE_INSTALL_PREFIX:PATH=${OPT}/ffmpeg -DENABLE_SHARED:bool=off ../../source
make${MAKETHREADS}
make install

cd ${OPT}/ffmpeg_sources
git clone --depth 1 git://git.code.sf.net/p/opencore-amr/fdk-aac
cd fdk-aac
autoreconf -fiv
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -L -O http://downloads.sourceforge.net/project/lame/lame/3.99/lame-3.99.5.tar.gz
tar xzvf lame-3.99.5.tar.gz
cd lame-3.99.5
./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-shared --enable-nasm
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
git clone https://git.xiph.org/opus.git
cd opus
autoreconf -fiv
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -O http://downloads.xiph.org/releases/ogg/libogg-${LIBOGG_VER}.tar.gz
tar xzvf libogg-${LIBOGG_VER}.tar.gz
cd libogg-${LIBOGG_VER}
./configure --prefix="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
curl -O http://downloads.xiph.org/releases/vorbis/libvorbis-${LIBVORBIS_VER}.tar.gz
tar xzvf libvorbis-${LIBVORBIS_VER}.tar.gz
cd libvorbis-${LIBVORBIS_VER}
LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" ./configure --prefix="${OPT}/ffmpeg" --with-ogg="${OPT}/ffmpeg" --enable-static --enable-shared
make${MAKETHREADS}
make install
make distclean

cd ${OPT}/ffmpeg_sources
git clone --depth 1 https://chromium.googlesource.com/webm/libvpx.git
cd libvpx
./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared
make${MAKETHREADS}
make install
make clean

cd ${OPT}/ffmpeg_sources
git clone --depth 1 git://source.ffmpeg.org/ffmpeg
cd ffmpeg
LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="-I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib" --bindir="${OPT}/bin" --pkg-config-flags="--static" --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265 --enable-swscale --enable-shared${DISABLE_FFMPEGNETWORK}
make${MAKETHREADS}
make install
make distclean
hash -r

unset TMPDIR

echo "${OPT}/ffmpeg/lib" > /etc/ld.so.conf.d/libavdevice.conf
cat /etc/ld.so.conf.d/libavdevice.conf
ldconfig

echo
echo "Installed FFMPEG at ${OPT}/ffmpeg"
echo

echo
/opt/bin/ffmpeg -version

echo
/opt/bin/ffmpeg -formats

}

update() {

echo
echo "Updating FFMPEG..."

mkdir -p /home/ffmpegtmp
chmod 1777 /home/ffmpegtmp
export TMPDIR=/home/ffmpegtmp

rm -rf ~/ffmpeg ~/bin/{ffmpeg,ffprobe,ffserver,lame,vsyasm,x264,x265,yasm,ytasm}

yum -y update yasm yasm-devel

# EPEL YUM Repo has yasm 1.2.0 already
# cd ${OPT}/ffmpeg_sources/yasm
# make distclean
# git pull
# ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin"
# make${MAKETHREADS}
# make install
# make distclean

cd ${OPT}/ffmpeg_sources/x264
make distclean
git pull
PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --bindir="${OPT}/bin" --enable-static --enable-shared
make${MAKETHREADS}
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
./configure --prefix="${OPT}/ffmpeg" --disable-examples --enable-static --enable-shared
make${MAKETHREADS}
make install
make clean

cd ${OPT}/ffmpeg_sources/ffmpeg
make distclean
git pull
LD_LIBRARY_PATH=${OPT}/ffmpeg/lib PKG_CONFIG_PATH="${OPT}/ffmpeg/lib/pkgconfig" ./configure --prefix="${OPT}/ffmpeg" --extra-cflags="-I${OPT}/ffmpeg/include" --extra-ldflags="-L${OPT}/ffmpeg/lib" --bindir="${OPT}/bin" --pkg-config-flags="--static" --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265 --enable-swscale --enable-shared
make${MAKETHREADS}
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

phpext() {
	echo
	echo "Install FFMPEG PHP extension..."

	cd $DIR_TMP
	rm -rf ffmpeg-php-git
	git clone https://github.com/tony2001/ffmpeg-php.git ffmpeg-php-git
	cd ffmpeg-php-git
	# cd ffmpeg-php-${FFMPEGVER}
	
	make clean
	phpize -clean
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
	
	LD_LIBRARY_PATH=${OPT}/ffmpeg/lib LDFLAGS="-L${OPT}/ffmpeg/lib" CPPFLAGS="-I${OPT}/ffmpeg/include" ./configure --with-php-config=/usr/local/bin/php-config --with-ffmpeg=/opt/ffmpeg
	
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

}

case "$1" in
	install )
		starttime=$(date +%s.%N)
		{
		install
		} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
echo "Total FFMPEG Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
		;;
	update )
		starttime=$(date +%s.%N)
		{
		update
		} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
echo "Total FFMPEG Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_install_${DT}.log
		;;		
	php )
		starttime=$(date +%s.%N)
		{
		phpext
		} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log
echo "Total FFMPEG PHP Extension Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ffmpeg_phpext_install_${DT}.log
		;;			
	* )
		echo "$0 {install|update|php}"
		;;
esac