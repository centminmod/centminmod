#!/bin/bash
#######################################################
# centminmod.com cli installer
# To run installer.sh type: 
# curl -sL https://gist.github.com/centminmod/dbe765784e03bc4b0d40/raw/installer.sh | bash
#######################################################
export PATH="/usr/lib64/ccache:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
DT=$(date +"%d%m%y-%H%M%S")
branchname=123.09beta01
DOWNLOAD="${branchname}.zip"

INSTALLDIR='/usr/local/src'
DIR_TMP='/svr-setup'
#CUR_DIR="/usr/local/src/centminmod-${branchname}"
#CM_INSTALLDIR=$CUR_DIR
#SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
#####################################################
# Centmin Mod Git Repo URL - primary repo
# https://github.com/centminmod/centminmod
GITINSTALLED='y'
#CMGIT='https://github.com/centminmod/centminmod.git'
# Gitlab backup repo 
# https://gitlab.com/centminmod/centminmod
CMGIT='https://gitlab.com/centminmod/centminmod.git'
#####################################################
# wget renamed github
AXEL='n'
AXEL_VER='2.5'
AXEK_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEK_LINK="https://github.com/eribertomota/axel/archive/${AXEL_VER}.tar.gz"
AXEK_LINKLOCAL="http://centminmod.com/centminmodparts/axel/${AXEL_VER}.tar.gz"
#######################################################
# 

if [ ! -d "$DIR_TMP" ]; then
  mkdir -p $DIR_TMP
fi

DEF=${1:-novalue}

yum clean all

if [[ ! -f /usr/bin/git || ! -f /usr/bin/bc || ! -f /usr/bin/wget || ! -f /bin/nano || ! -f /usr/bin/unzip || ! -f /usr/bin/applydeltarpm ]]; then
  firstyuminstallstarttime=$(date +%s.%N)
  echo
  echo "installing yum packages..."
  echo
  yum -y install virt-what gawk unzip bc wget yum-plugin-fastestmirror lynx screen deltarpm ca-certificates yum-plugin-security yum-utils bash mlocate subversion rsyslog dos2unix net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libgcj libtool-libs libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop libvpx libvpx-devel t1lib t1lib-devel expect expect-devel readline readline-devel libedit libedit-devel openssl openssl-devel curl curl-devel openldap openldap-devel zlib zlib-devel gd gd-devel pcre pcre-devel gettext gettext-devel libidn libidn-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel e2fsprogs e2fsprogs-devel libc-client libc-client-devel ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel cyrus-sasl cyrus-sasl-devel pam pam-devel libaio libaio-devel libevent libevent-devel recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel enchant enchant-devel lua lua-devel
  # allows curl install to skip checking for already installed yum packages 
  # later on in initial curl installations
  touch /tmp/curlinstaller-yum
  yum -y install epel-release
  yum -y install figlet moreutils clang clang-devel jemalloc jemalloc-devel pngquant optipng jpegoptim pwgen aria2 pigz pbzip2 xz pxz lz4 libJudy glances bash-completion mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake28
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -y install GeoIP GeoIP-devel --disablerepo=rpmforge
  else
    yum -y install GeoIP GeoIP-devel
  fi
  touch ${INSTALLDIR}/curlinstall_yum.txt
  firstyuminstallendtime=$(date +%s.%N)
fi

if [ -f /etc/selinux/config ]; then
  sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
  sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
fi

yumupdater() {
  yum clean all
  yum -y update
  #yum -y install expect imake bind-utils readline readline-devel libedit libedit-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip patch sysstat zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file libgcj gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel glib2 glib2-devel bzip2 bzip2-devel vim-minimal nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-ExtUtils-MakeMaker perl-Time-HiRes cyrus-sasl cyrus-sasl-devel strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel git
}

install_axel() {
  cd $DIR_TMP
  echo "Download $AXEK_LINKFILE ..."
  if [ -s $AXEK_LINKFILE ]; then
    echo "Axel ${AXEL_VER} Archive found, skipping download..." 
  else
    wget -O $AXEK_LINKFILE $AXEK_LINKLOCAL
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
     echo "Error: $AXEK_LINKFILE download failed."
      exit #$ERROR
    else 
      echo "Download $AXEK_LINKFILE done."
    fi
  fi

  tar xzf $AXEK_LINKFILE
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    echo "Error: $AXEK_LINKFILE extraction failed."
    exit #$ERROR
  else 
    echo "$AXEK_LINKFILE valid file."
    echo ""
  fi

  cd axel-${AXEL_VER}
  ./configure
  make
  make install
  which axel
}

cminstall() {

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Centmin Mod Install"
    fi

cd $INSTALLDIR
  if [[ "$GITINSTALLED" = [yY] ]]; then
    if [[ ! -f "${INSTALLDIR}/centminmod" ]]; then
      getcmstarttime=$(date +%s.%N)
      echo "git clone Centmin Mod repo..."
      time git clone -b ${branchname} --depth=40 ${CMGIT} centminmod
      cd centminmod
      chmod +x centmin.sh
      getcmendtime=$(date +%s.%N)   
    fi
  else
    if [[ ! -f "${DOWNLOAD}" ]]; then
    getcmstarttime=$(date +%s.%N)
    echo "downloading Centmin Mod..."
    if [[ -f /usr/bin/axel && $AXEL = [yY] ]]; then
      /usr/bin/axel https://github.com/centminmod/centminmod/archive/${DOWNLOAD}
    else
      wget -c --no-check-certificate https://github.com/centminmod/centminmod/archive/${DOWNLOAD} --tries=3
    fi
    getcmendtime=$(date +%s.%N)
    rm -rf centminmod-*
    unzip ${DOWNLOAD}
    fi
    #export CUR_DIR
    #export CM_INSTALLDIR
    mv centminmod-${branchname} centminmod
    cd centminmod
    chmod +x centmin.sh
  fi

# disable nginx lua and luajit by uncommenting these 2 lines
#sed -i "s|LUAJIT_GITINSTALL='y'|LUAJIT_GITINSTALL='n'|" centmin.sh
#sed -i "s|ORESTY_LUANGINX='y'|ORESTY_LUANGINX='n'|" centmin.sh

# disable nginx pagespeed module by uncommenting this line
#sed -i "s|NGINX_PAGESPEED=y|NGINX_PAGESPEED=n|" centmin.sh

# disable nginx geoip module by uncommenting this line
#sed -i "s|NGINX_GEOIP=y|NGINX_GEOIP=n|" centmin.sh

# disable nginx vhost traffic stats module by uncommenting this line
#sed -i "s|NGINX_VHOSTSTATS=y|NGINX_VHOSTSTATS=n|" centmin.sh

# disable nginx webdav modules by uncommenting this line
#sed -i "s|NGINX_WEBDAV=y|NGINX_WEBDAV=n|" centmin.sh

# disable openresty additional nginx modules by uncommenting this line
#sed -i "s|NGINX_OPENRESTY='y'|NGINX_OPENRESTY='n'|" centmin.sh

# switch back to OpenSSL instead of LibreSSL for Nginx
#sed -i "s|LIBRESSL_SWITCH='y'|LIBRESSL_SWITCH='n'|" centmin.sh

# siwtch back to Libmemcached source compile instead of YUM repo install
#sed -i "s|LIBMEMCACHED_YUM='y'|LIBMEMCACHED_YUM='n'|" centmin.sh

# disable PHP redis extension
#sed -i "s|PHPREDIS='y'|PHPREDIS='n'|" centmin.sh

# switch from PHP 5.4.41 to 5.6.9 default with Zend Opcache
sed -i "s|^PHP_VERSION='.*'|PHP_VERSION='5.6.19'|" centmin.sh
sed -i "s|ZOPCACHEDFT='n'|ZOPCACHEDFT='y'|" centmin.sh

# disable axivo yum repo
#sed -i "s|AXIVOREPO_DISABLE=n|AXIVOREPO_DISABLE=y|" centmin.sh

# bypass initial setup email prompt
mkdir -p /etc/centminmod/
echo "NGINX_PAGESPEED=y" > /etc/centminmod/custom_config.inc
echo "ORESTY_LUANGINX=y" >> /etc/centminmod/custom_config.inc
# echo "ORESTY_LUANGINXVER='0.10.2'" >> /etc/centminmod/custom_config.inc
echo "1" > /etc/centminmod/email-primary.ini
echo "2" > /etc/centminmod/email-secondary.ini

# setup gitlab as default git repo instead of github
sed -i "s|^CMGIT='https:\/\/github.com\/centminmod\/centminmod.git'|#CMGIT='https:\/\/github.com\/centminmod\/centminmod.git'|" centmin.sh
sed -i "s|^#CMGIT='https:\/\/gitlab.com\/centminmod\/centminmod.git'|CMGIT='https:\/\/gitlab.com\/centminmod\/centminmod.git'|" centmin.sh
echo "CMGIT='https://gitlab.com/centminmod/centminmod.git'" > /etc/centminmod/custom_config.inc

"${INSTALLDIR}/centminmod/centmin.sh" install
rm -rf /etc/centminmod/email-primary.ini
rm -rf /etc/centminmod/email-secondary.ini

    # setup command shortcut aliases 
    # given the known download location
    # updated method for cmdir and centmin shorcuts
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    rm -rf /usr/bin/cmdir
    alias cmdir="pushd /usr/local/src/centminmod"
    echo "alias cmdir='pushd /usr/local/src/centminmod'" >> /root/.bashrc
    echo -e "pushd /usr/local/src/centminmod; bash centmin.sh" > /usr/bin/centmin
    chmod 0700 /usr/bin/centmin
  echo
  echo "Created command shortcuts:"
  echo "* type cmdir to change to Centmin Mod install directory"
  echo "  at /usr/local/src/centminmod"
  echo "* type centmin call and run centmin.sh"
  echo "  at /usr/local/src/centminmod/centmin.sh"
}

if [[ "$DEF" = 'novalue' ]]; then
  install_axel
  cminstall
  echo
  FIRSTYUMINSTALLTIME=$(echo "$firstyuminstallendtime - $firstyuminstallstarttime" | bc)
  FIRSTYUMINSTALLTIME=$(printf "%0.4f\n" $FIRSTYUMINSTALLTIME)
  GETCMTIME=$(echo "$getcmendtime - $getcmstarttime" | bc)
  GETCMTIME=$(printf "%0.4f\n" $GETCMTIME)
  #touch ${CENTMINLOGDIR}/firstyum_installtime_${DT}.log
  echo "" > "/root/centminlogs/firstyum_installtime_${DT}.log"
echo "---------------------------------------------------------------------------"
  echo "Total Curl Installer YUM Time: $FIRSTYUMINSTALLTIME seconds" >> "/root/centminlogs/firstyum_installtime_${DT}.log"
  tail -1 /root/centminlogs/firstyum_installtime_*.log
  tail -1 /root/centminlogs/centminmod_yumtimes_*.log
  DTIME=$(tail -1 /root/centminlogs/centminmod_downloadtimes_*.log)
  DTIME_SEC=$(echo "$DTIME" |awk '{print $7}')
  NTIME=$(tail -1 /root/centminlogs/centminmod_ngxinstalltime_*.log)
  NTIME_SEC=$(echo "$NTIME" |awk '{print $7}')
  PTIME=$(tail -1 /root/centminlogs/centminmod_phpinstalltime_*.log)
  PTIME_SEC=$(echo "$PTIME" |awk '{print $7}')
  CMTIME=$(tail -1 /root/centminlogs/*_install.log)
  CMTIME_SEC=$(echo "$CMTIME" |awk '{print $6}')
  CMTIME_SEC=$(printf "%0.4f\n" $CMTIME_SEC)
  CURLT=$(awk '{print $6}' /root/centminlogs/firstyum_installtime_*.log | tail -1)
  CT=$(awk '{print $6}' /root/centminlogs/*_install.log | tail -1)
  TT=$(echo "$CURLT + $CT + $GETCMTIME" | bc)
  TT=$(printf "%0.4f\n" $TT)
  ST=$(echo "$CT - ($DTIME_SEC + $NTIME_SEC + $PTIME_SEC)" | bc)
  ST=$(printf "%0.4f\n" $ST)
  echo "Total YUM + Source Download Time: $(printf "%0.4f\n" $DTIME_SEC)"
  echo "Total Nginx First Time Install Time: $(printf "%0.4f\n" $NTIME_SEC)"
  echo "Total PHP First Time Install Time: $(printf "%0.4f\n" $PTIME_SEC)"
  echo "Download Zip From Github Time: $GETCMTIME"
  echo "Total Time Other eg. source compiles: $ST"
  echo "Total Centmin Mod Install Time: $CMTIME_SEC"
echo "---------------------------------------------------------------------------"
  echo "Total Install Time (curl yum + cm install + zip download): $TT seconds"    
echo "---------------------------------------------------------------------------"
fi

if [ -f "${INSTALLDIR}/curlinstall_yum.txt" ]; then
  rm -rf "${INSTALLDIR}/curlinstall_yum.txt"
fi

case "$1" in
  install)
    install_axel
    cminstall
    ;;
  yumupdate)
    yumupdater
    install_axel
    cminstall
    ;;
  *)
    if [[ "$DEF" = 'novalue' ]]; then
      echo
    else
      echo "./$0 {install|yumupdate}"
    fi
    ;;
esac