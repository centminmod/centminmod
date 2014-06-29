################################
# http://ffmpeg.org/download.html
ffmpegsetup() {

PHPVERCHECK=`cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}'`

if [ "$PHPVERCHECK" != '5.4' ]; then

cd ${DIR_TMP}

echo "FFMPEG install starting..."

#yum -y -q install --disablerepo=rpmforge dirac-libs dirac
yum -y -q install ffmpeg ffmpeg-devel mplayer mencoder flvtool2 re2c yasm nasm libvpx libvpx-devel amrnb-devel amrwb-devel libid3tag libvorbis-devel lame libogg libwmf xvidcore xvidcore-devel facc faad2 a52decoder *vorbis* ruby ruby-libs ruby-devel

if [ -s ffmpeg-php-${FFMPEGVER}.tbz2 ]; then
  echo "ffmpeg-php-${FFMPEGVER}.tbz2 [found]"
  else
  echo "Error: ffmpeg-php-${FFMPEGVER}.tbz2 not found!!!download now......"
  wget -c --progress=bar http://downloads.sourceforge.net/project/ffmpeg-php/ffmpeg-php/${FFMPEGVER}/ffmpeg-php-${FFMPEGVER}.tbz2?use_mirror=aarnet --tries=3
fi

tar -xjf ffmpeg-php-${FFMPEGVER}.tbz2
cd ffmpeg-php-${FFMPEGVER}
make clean
phpize -clean
phpize
./configure --with-php-config=/usr/local/bin/php-config

sed -i 's/PIX_FMT_RGBA32/PIX_FMT_RGB32/g' ffmpeg_frame.c

make${MAKETHREADS}
make install

FFMPEGCHECK=`grep 'extension=ffmpeg.so' /usr/local/lib/php.ini `
if [ -z "$FFMPEGCHECK" ]; then
echo "" >> /usr/local/lib/php.ini
echo "[ffmpeg]" >> /usr/local/lib/php.ini
echo "extension=ffmpeg.so" >> /usr/local/lib/php.ini
fi

echo ""
echo "FFMPEG PHP extension installed.."
echo "restarting php-fpm service ..."
echo ""

service php-fpm restart

echo ""
echo "check phpinfo for FFMPEG PHP Extension..."
echo ""

php -i | grep ^ffmpeg

echo ""
echo "check FFMPEG"
echo ""

ffmpeg

else 

echo ""
echo ""
echo "$PHPVERCHECK not compatible with FFMPEG-PHP"
echo "install aborted"
echo ""
echo ""

fi # PHPVERCHECK

} # ffmpeg
################################