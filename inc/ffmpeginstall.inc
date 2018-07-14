ffmpegsubmenu() {

while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "             FFMPEG Sub-Menu              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1). Install FFMPEG Binary + FFMPEG PHP extension" $boldgreen
  cecho "2). Update FFMPEG Binary + FFMPEG PHP extension" $boldgreen
  cecho "3). Reinstall FFMPEG PHP extension" $boldgreen
  cecho "4). Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 4 ] " ffmpegoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$ffmpegoption" in
1)
centminlog
if [ -f "${CM_INSTALLDIR}/addons/ffmpeg.sh" ]; then
  ${CM_INSTALLDIR}/addons/ffmpeg.sh install
  ${CM_INSTALLDIR}/addons/ffmpeg.sh php silent
fi

;;
2)
centminlog
if [ -f "${CM_INSTALLDIR}/addons/ffmpeg.sh" ]; then
  ${CM_INSTALLDIR}/addons/ffmpeg.sh update
fi

;;
3)
centminlog
if [ -f "${CM_INSTALLDIR}/addons/ffmpeg.sh" ]; then
  ${CM_INSTALLDIR}/addons/ffmpeg.sh php
fi

;;
4)
  break
;;
esac
done

}

################################
# http://ffmpeg.org/download.html
ffmpegsetup() {

  if [ "$(uname -m)" == 'x86_64' ]; then
    ATRPMARCH='x86_64'
  else
    ATRPMARCH='i386'
  fi

  if [[ "$CENTOS_SEVEN" != '7' ]]; then
cat > "/etc/yum.repos.d/atrpms.repo" <<EOF
[atrpms]
name=Red Hat Enterprise Linux $releasever - $basearch - ATrpms
failovermethod=priority
baseurl=http://www.mirrorservice.org/sites/dl.atrpms.net/el6-${ATRPMARCH}/atrpms/stable
enabled=1
gpgcheck=0
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-atrpms
priority=90
EOF
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
cat > "/etc/yum.repos.d/atrpms.repo" <<EOF
[atrpms]
name=Red Hat Enterprise Linux $releasever - $basearch - ATrpms
failovermethod=priority
baseurl=http://www.mirrorservice.org/sites/dl.atrpms.net/el7-${ATRPMARCH}/atrpms/stable
enabled=1
gpgcheck=0
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-atrpms
priority=90
EOF
  fi  

  if [ -f /etc/yum.repos.d/atrpms.repo ]; then

PHPVERCHECK=`cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}'`

if [[ "$PHPVERCHECK" < 5.8 ]]; then

cd ${DIR_TMP}

echo "FFMPEG install starting..."

if [[ "$CENTOS_SEVEN" != '7' ]]; then
  echo "CentOS 6 detected"
  echo "installing yum packages..."
  time $YUMDNFBIN -y install ffmpeg ffmpeg-devel mplayer mencoder flvtool2 re2c yasm nasm libvpx libvpx-devel amrnb-devel amrwb-devel libid3tag libvorbis-devel lame libogg libwmf xvidcore xvidcore-devel facc faad2 a52decoder *vorbis* ruby ruby-libs ruby-devel
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
  echo "CentOS 7 detected"
  echo "installing yum packages..."

  if [[ -z "$(rpm -ql libnemesi | grep -v 'not installed')" || -z "$(rpm -ql nas-libs | grep -v 'not installed')" || -z "$(rpm -ql svgalib | grep -v 'not installed')" || -z "$(rpm -ql netembryo | grep -v 'not installed')" ]]; then
    if [ -f "${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip" ]; then
        /usr/bin/unzip -qo ${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip -d $DIR_TMP/
      else
      if [[ -z "$(rpm -ql libnemesi | grep -v 'not installed')" ]]; then
        wget ftp://fr2.rpmfind.net/linux/fedora/linux/releases/20/Everything/x86_64/os/Packages/l/libnemesi-0.7.0-0.5.20110215git.fc20.x86_64.rpm
      fi
           
      if [[ -z "$(rpm -ql nas-libs | grep -v 'not installed')" ]]; then
        wget ftp://fr2.rpmfind.net/linux/fedora/linux/releases/20/Everything/x86_64/os/Packages/n/nas-libs-1.9.4-1.fc20.x86_64.rpm
      fi
    
      if [[ -z "$(rpm -ql svgalib | grep -v 'not installed')" ]]; then
        wget ftp://fr2.rpmfind.net/linux/fedora/linux/releases/20/Everything/x86_64/os/Packages/s/svgalib-1.9.25-14.fc20.x86_64.rpm
      fi
    
      if [[ -z "$(rpm -ql netembryo | grep -v 'not installed')" ]]; then
        wget ftp://fr2.rpmfind.net/linux/fedora/linux/releases/20/Everything/x86_64/os/Packages/n/netembryo-0.1.1-5.fc20.x86_64.rpm
      fi        
    fi  
  fi

  if [[ -z "$(rpm -ql libnemesi | grep -v 'not installed')" || -z "$(rpm -ql nas-libs | grep -v 'not installed')" || -z "$(rpm -ql svgalib | grep -v 'not installed')" || -z "$(rpm -ql netembryo | grep -v 'not installed')" ]]; then
    time $YUMDNFBIN -y localinstall libnemesi-0.7.0-0.5.20110215git.fc20.x86_64.rpm nas-libs-1.9.4-1.fc20.x86_64.rpm svgalib-1.9.25-14.fc20.x86_64.rpm netembryo-0.1.1-5.fc20.x86_64.rpm                              
  fi

  time $YUMDNFBIN -y install ffmpeg ffmpeg-devel mplayer mencoder flvtool2 re2c yasm nasm libvpx libvpx-devel amrnb-devel amrwb-devel libid3tag libvorbis-devel lame libogg libwmf xvidcore xvidcore-devel facc faad2 a52decoder *vorbis* ruby ruby-libs ruby-devel --disableplugin=priorities --enablerepo=atrpms
fi

FFMPEGGITLINK='https://github.com/tony2001/ffmpeg-php'

# fallback mirror if github down, use gitlab mirror
curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $FFMPEGGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
FFMPEGGIT_CURLCHECK=$?
if [[ "$FFMPEGGIT_CURLCHECK" != '0' ]]; then
  FFMPEGGITLINK='https://gitlab.com/centminmod-github-mirror/ffmpeg-php'
fi

time git clone "${FFMPEGGITLINK}.git" ffmpeg-php-git
cd ffmpeg-php-git
# cd ffmpeg-php-${FFMPEGVER}

make clean
phpize -clean
phpize

./configure --with-php-config=/usr/local/bin/php-config

make${MAKETHREADS}
make install

FFMPEGCHECK=$(grep 'extension=ffmpeg.so' "${CONFIGSCANDIR}/ffmpeg.ini" >/dev/null 2>&1; echo $?)
if [[ "$FFMPEGCHECK" = '1' ]]; then
  echo "" >> "${CONFIGSCANDIR}/ffmpeg.ini"
  echo "[ffmpeg]" >> "${CONFIGSCANDIR}/ffmpeg.ini"
  echo "extension=ffmpeg.so" >> "${CONFIGSCANDIR}/ffmpeg.ini"
fi

echo ""
echo "FFMPEG PHP extension installed.."
echo "restarting php-fpm service ..."
echo ""

cmservice php-fpm restart

echo ""
echo "check phpinfo for FFMPEG PHP Extension..."
echo ""

php --ri ffmpeg

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

else

  echo
  echo "ATrpms YUM Repository is not installed"
  echo

  fi # check for /etc/yum.repos.d/atrpms.repo

} # ffmpeg
################################