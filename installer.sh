#!/bin/bash
# centminmod.com cli installer
#######################################################
DOWNLOAD='centmin-v1.2.3-eva2000.07.zip'

INSTALLDIR='/usr/local/src'
# SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
#######################################################
# 

DEF=${1:-novalue}

if [[ ! -f /usr/bin/bc || ! -f /usr/bin/wget || ! -f /bin/nano || ! -f /usr/bin/unzip ]]; then
	yum -y install unzip bc wget yum-plugin-fastestmirror
fi

if [ -f /etc/selinux/config ]; then
	sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
fi

yumupdater() {
	yum clean all
	yum -y update
	yum -y install expect imake bind-utils readline readline-devel libedit libedit-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip patch sysstat zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file libgcj gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel glib2 glib2-devel bzip2 bzip2-devel vim-minimal nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-ExtUtils-MakeMaker perl-Time-HiRes cyrus-sasl cyrus-sasl-devel strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel git
}

cminstall() {
cd $INSTALLDIR
if [[ ! -f "${DOWNLOAD}" ]]; then
	wget -cnv http://centminmod.com/download/${DOWNLOAD} --tries=3
	rm -rf centmin-v1.2.3mod
	unzip ${DOWNLOAD}
fi
cd centmin-v1.2.3mod
chmod +x centmin.sh
./centmin.sh install
tail -1 /root/centminlogs/centminmod_yumtimes_*.log
tail -1 /root/centminlogs/*_install.log

# if [[ -z $(alias | grep cmdir) ]]; then
# 	# setup command shortcut aliases 
# 	# given the known download location
# 	alias cmdir="pushd ${SCRIPT_DIR}"
# 	alias centmin="pushd ${SCRIPT_DIR}; bash centmin.sh"
# 	echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
# 	echo "alias centmin='cd ${SCRIPT_DIR}; bash centmin.sh'" >> /root/.bashrc
# 	source /root/.bashrc
# 	echo
# 	echo "Created command shortcuts:"
# 	echo "* type cmdir to change to Centmin Mod install directory"
# 	echo "  at ${SCRIPT_DIR}"
# 	echo "* type centmin call and run centmin.sh"
# 	echo "  at ${SCRIPT_DIR}/centmin.sh"
# fi
}

if [[ "$DEF" = 'novalue' ]]; then
	cminstall
fi

case "$1" in
	install)
		cminstall
		;;
	yumupdate)
		yumupdater
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