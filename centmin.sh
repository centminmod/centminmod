#!/bin/sh
UNATTENDED='y'
ZONEINFO=Australia/Brisbane  # Set Timezone
NGINX_IPV='n' #NGINX IPV6 compile support for unattended mode only

CUSTOMSERVERNAME='y'
CUSTOMSERVERSTRING='nginx centminmod'
PHPFPMCONFDIR='/usr/local/nginx/conf/phpfpmd'
#####################################################
DT=`date +"%d%m%y-%H%M%S"`
SCRIPT_MAJORVER='1.2.3'
SCRIPT_MINORVER='06'
SCRIPT_VERSION="${SCRIPT_MAJORVER}-eva2000.${SCRIPT_MINORVER}"
SCRIPT_DATE='27/12/2013'
SCRIPT_AUTHOR='eva2000 (vbtechsupport.com)'
SCRIPT_MODIFICATION_AUTHOR='eva2000 (vbtechsupport.com)'
SCRIPT_URL='http://centminmod.com'
COPYRIGHT="Copyright 2011-2013 CentminMod.com"
DISCLAIMER='This software is provided "as is" in the hope that it will be useful, but WITHOUT ANY WARRANTY, to the extent permitted by law; without even the implied warranty of MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.'

#####################################################
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version. See the included license.txt for futher details.
#
# PLEASE MODIFY VALUES BELOW THIS LINE ++++++++++++++++++++++++++++++++++++++
# Note: Please enter y for yes or n for no.
#####################################################
HN=$(uname -n)
# Pre-Checks to prevent screw ups
DIR_TMP='/svr-setup'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

if [ ! -d "$DIR_TMP" ]; then
mkdir -p $DIR_TMP
fi

if [[ -z $(cat /etc/resolv.conf) ]]; then
echo ""
echo "/etc/resolv.conf is empty. No nameserver resolvers detected !! "
echo "Please configure your /etc/resolv.conf correctly or you will not"
echo "be able to use the internet or download from your server."
echo "aborting script... please re-run centmin.sh"
echo ""
exit
fi

if [ ! -f /usr/bin/wget ]; then
echo "wget not found !! "
echo "installing wget"
yum -y -q install wget
echo "aborting script... please re-run centmin.sh"
exit
fi

if [ ! -f /usr/bin/unzip ]; then
yum -y -q install unzip
fi

if [ ! -f /usr/bin/bc ]; then
echo "bc not found !! "
echo "installing bc"
yum -y -q install bc
echo "bc installed"
echo "aborting script... "
echo "please re-run centmin.sh again for install"
exit
fi

if [ ! -f /usr/bin/tee ]; then
#echo "tee not found !! "
#echo "installing tee"
yum -y -q install coreutils
fi

if [ -f /var/cpanel/cpanel.config ]; then
echo "WHM/Cpanel detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/psa/.psa.shadow ]; then
echo "Plesk detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/init.d/directadmin ]; then
echo "DirectAdmin detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

TESTEDCENTOSVER='6.5'
CENTOSVER=`cat /etc/redhat-release | awk '{ print $3 }'`

if [ "$CENTOSVER" == 'release' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $4 }'`
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $7 }'`
OLS='y'
fi

if [ -f /proc/user_beancounters ]; then
CPUS='1'
MAKETHREADS=" -j$CPUS"
else
# speed up make
CPUS=`cat "/proc/cpuinfo" | grep "processor"|wc -l`
MAKETHREADS=" -j$CPUS"
fi

# configure .ini directory
CONFIGSCANBASE='/root/centminmod'
CONFIGSCANDIR="${CONFIGSCANBASE}/php.d"

if [ ! -d "$CONFIGSCANBASE" ]; then
	mkdir $CONFIGSCANBASE
fi

if [ ! -d "$CONFIGSCANDIR" ]; then
	mkdir $CONFIGSCANDIR
fi

#####################################################
# Enable or disable menu mode

ENABLE_MENU='y'

source "inc/cpucount.inc"
source "inc/motd.inc"
source "inc/cpcheck.inc"
source "inc/memcheck.inc"
source "inc/ccache.inc"
source "inc/bookmark.inc"
source "inc/centminlogs.inc"
source "inc/yumskip.inc"
source "inc/questions.inc"
source "inc/downloads.inc"
source "inc/yumpriorities.inc"
source "inc/yuminstall.inc"
source "inc/centoscheck.inc"
source "inc/axelsetup.inc"
source "inc/phpfpmdir.inc"
source "inc/nginx_backup.inc"
source "inc/nsd_install.inc"
source "inc/nsdsetup.inc"
source "inc/nsd_reinstall.inc"
source "inc/logrotate_nginx.inc"
source "inc/logrotate_phpfpm.inc"
source "inc/openssl_install.inc"
source "inc/nginx_mimetype.inc"
source "inc/nginx_configure.inc"
source "inc/nginx_configure_openresty.inc"
source "inc/nginx_install.inc"
source "inc/nginx_upgrade.inc"
source "inc/imagick_install.inc"
source "inc/memcached_install.inc"
source "inc/mysql_proclimit.inc"
source "inc/mariadb_install.inc"
source "inc/mysql_install.inc"
source "inc/php_configure.inc"
source "inc/suhosin_setup.inc"
source "inc/nginx_pagespeed.inc"
source "inc/nginx_modules.inc"
source "inc/nginx_modules_openresty.inc"
source "inc/sshd.inc"
source "inc/openvz_stack.inc"
source "inc/siegeinstall.inc"
source "inc/python_install.inc"
source "inc/nginx_addvhost.inc"
source "inc/mariadb_upgrade.inc"
source "inc/mariadb_upgrade53.inc"
source "inc/mariadb_upgrade55.inc"
source "inc/nginx_errorpage.inc"
source "inc/sendmail.inc"
source "inc/postfix.inc"
source "inc/compress.inc"
source "inc/diskalert.inc"
source "inc/phpsededit.inc"
source "inc/csfinstall.inc"
source "inc/csftweaks.inc"
source "inc/xcache_installask.inc"
source "inc/xcache_install.inc"
source "inc/xcache_reinstall.inc"
source "inc/igbinary.inc"
source "inc/apcprotect.inc"
source "inc/apcinstall.inc"
source "inc/apcreinstall.inc"
source "inc/zendopcache_install.inc"
source "inc/zendopcache_upgrade.inc"
source "inc/ffmpeginstall.inc"
source "inc/shortcuts_install.inc"
source "inc/memcacheadmin.inc"
source "inc/centminfinish.inc"

checkcentosver

if [ ! -f /etc/centminmod-release ];then
echo "$SCRIPT_VERSION" > /etc/centminmod-release
else
	if [[ "$SCRIPT_VERSION" != "$(cat /etc/centminmod-release)" ]]; then
	echo "$SCRIPT_VERSION" > /etc/centminmod-release
	fi
fi

if [ -f /etc/centminmod-versionlog ];then
	if [[ "$SCRIPT_VERSION" != "$(cat /etc/centminmod-versionlog)" ]]; then
	echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
	fi
else
	echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
fi

#####################################################
# CCACHE Configuration
CCACHEINSTALL='y'
CCACHESIZE='2G'

# General Configuration
NGINXUPGRADESLEEP='35'
NSD_INSTALL=y                # Install NSD (DNS Server)
NSD_VERSION='3.2.16'         # NSD Version
NTP_INSTALL=y                # Install Network time protocol daemon
NGINXPATCH=n                 # Set to y to allow 30 seconds time before Nginx configure and patching Nginx
NGINX_INSTALL=y              # Install Nginx (Webserver)
NGINX_SPDY=y                 # Nginx SPDY support
NGINX_PAGESPEED=y            # Install ngx_pagespeed
NGINX_PAGESPEEDGITMASTER=y   # Install ngx_pagespeed from official github master instead  
NGXPGSPEED_VER='1.7.30.4-beta'
NGINX_PAGESPEEDPSOL_VER='1.7.30.4'
NGINX_WEBDAV=y          # Nginx WebDAV and nginx-dav-ext-module
NGINX_EXTWEBDAVVER='0.0.2'  # nginx-dav-ext-module version
NGINX_LIBATOMIC=y     # Nginx configured with libatomic support
NGINX_PCREJIT=y            # Nginx configured with pcre & pcre-jit support
NGINX_PCREVER='8.34'   # Version of PCRE used for pcre-jit support in Nginx
NGINX_OPENRESTY=n     # Agentzh's openresty Nginx modules
PHP_INSTALL=y                # Install PHP /w Fast Process Manager
PHPPCNTL=y                    # Disable or Enable PHP Process Control
SHORTCUTS=y	      # shortcuts
########################################################
# Choice of installing MariaDB 5.2 via RPM or via MariaDB 5.2 CentOS YUM Repo
# If MDB_YUMREPOINSTALL=y and MDB_INSTALL=n then MDB_VERONLY version 
# number won't have any effect in determining version of MariaDB 5.2.x to install. 
# YUM Repo will install whatever is latest MariaDB 5.2.x version available via the YUM REPO

MDB_INSTALL=n                # Install via RPM MariaDB MySQL Server replacement (Not recommended for VPS with less than 256MB RAM!)
MDB_YUMREPOINSTALL=y		  # Install MariaDB 5.5 via CentOS YUM Repo

# Define current MariaDB version
MDB_VERONLY='5.2.14'
MDB_BUILD='122'
MDB_VERSION="${MDB_VERONLY}-${MDB_BUILD}"     # Use this version of MariaDB ${MDB_VERONLY}

# Define previous MariaDB version for proper upgrade
MDB_PREVERONLY='5.2.12'
MDB_PREBUILD='115'
MDB_PREVERSION="${MDB_PREVERONLY}-${MDB_PREBUILD}"     # Use this version of MariaDB ${MDB_VERONLY}
########################################################


# Optionally, if you want to install MariaDB instead of standard MySQL you can do. 
# Set MDB_INSTALL=y and MYSQL_INSTALL=n
MYSQL_INSTALL=n              # Install official Oracle MySQL Server (MariaDB alternative recommended)
SENDMAIL_INSTALL=y           # Install Sendmail (and mailx)
# Nginx
NGINX_VERSION='1.5.8'        # Use this version of Nginx
NGINXBACKUP='y'
NGINXDIR='/usr/local/nginx'
NGINXCONFDIR="${NGINXDIR}/conf"
NGINXBACKUPDIR='/usr/local/nginxbackup'
NOSOURCEOPENSSL='y'	# set to 'y' to disable OpenSSL source compile for system default YUM package setup
OPENSSL_VERSION='1.0.1g'     # Use this version of OpenSSL

# Choose whether to compile Nginx --with-google_perftools_module
# no longer used in Centmin Mod v1.2.3-eva2000.01 and higher
GPERFTOOLS_SOURCEINSTALL=n
LIBUNWIND_VERSION='0.99'     # note google perftool specifically requies v0.99 and no other
GPERFTOOLS_VERSION='1.8.3'     # Use this version of google-perftools

# Choose whether to compile PCRE from source. Note PHP 5.3.8 already includes PCRE v8.12
PCRE_SOURCEINSTALL=n     
PCRE_VERSION='8.33'          # NO longer used/ignored

# PHP and Cache/Acceleration
IMAGICKPHP_VER='3.1.2'   # PHP extension for imagick
MEMCACHED_INSTALL=y          # Install Memcached
LIBEVENT_VERSION='2.0.21'    # Use this version of Libevent
MEMCACHED_VERSION='1.4.17'    # Use this version of Memcached server
MEMCACHE_VERSION='3.0.7'     # Use this version of Memcache
MEMCACHEDPHP_VER='2.1.0'    # Memcached PHP extension not server
LIBMEMCACHED_VER='1.0.16'
TWEMPERF_VER='0.1.1'

FFMPEGVER='0.6.0'
SUHOSINVER='0.9.33'
PHP_VERSION='5.3.28'          # Use this version of PHP
PHP_MIRRORURL='http://www.php.net'
PHPUPGRADE_MIRRORURL='http://www.php.net'
XCACHE_VERSION='2.0.1'       # Use this version of Xcache
APCCACHE_VERSION='3.1.13'       # Use this version of APC Cache
IGBINARY_VERSION='1.1.1'
IGBINARYGIT='y'
ZOPCACHECACHE_VERSION='7.0.2'
# Python
PYTHON_VERSION='2.7.6'       # Use this version of Python
SIEGE_VERSION='3.0.5'

# The default is stable, you can change this to development if you wish
#ARCH_OVERRIDE='i386'
# Uncomment the above line if you are running a 32bit Paravirtulized Xen VPS
# on a 64bit host node.

# YOU SHOULD NOT NEED TO MODIFY ANYTHING BELOW THIS LINE  +++++++++++++++++++
# JUST RUN chmod +x ./centmin.sh && ./centmin.sh
#
###############################################################
KEYPRESS_PARAM='-s -n1 -p'   # Read a keypress without hitting ENTER.
		# -s means do not echo input.
		# -n means accept only N characters of input.
		# -p means echo the following prompt before reading input
ASKCMD="read $KEYPRESS_PARAM "
MACHINE_TYPE=`uname -m` # Used to detect if OS is 64bit or not.

CUR_DIR=`pwd` # Get current directory.
###############################################################
# FUNCTIONS

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
DOWNLOADAPP='axel -a'
WGETRETRY=''
AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.gz"
AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.gz"
else
DOWNLOADAPP='wget -cnv --progress=bar'
WGETRETRY='--tries=3'
AXELPHPTARGZ=''
AXELPHPUPGRADETARGZ=''
fi

if [ "${ARCH_OVERRIDE}" != '' ]
then
    ARCH=${ARCH_OVERRIDE}
else
    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
        MDB_ARCH='amd64'
    else
        ARCH='i386'
    fi
fi

ASK () {
keystroke=''
while [[ "$keystroke" != [yYnN] ]]
do
    $ASKCMD "$1" keystroke
    echo "$keystroke";
done

key=$(echo $keystroke)
}

# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}

function funct_centmininstall {

#check centmin install previously

run_once() {
# If OpenVZ user add user/group 500 - else various folders and devices will end up with an odd user/group name for some reason
if [ -f /proc/user_beancounters ];
then
    groupadd 500
    useradd -g 500 -s /sbin/nologin -M 500
fi

#ASK "Would you like to update any pre-installed software? (Recommended) [y/n] "
#if [[ "$key" = [yY] ]];
#then
#    echo "Let's do that then..."
#    yum -q clean all
#    yum -y update glibc\*
#    yum -y update yum\* rpm\* python\*
#    yum -q clean all
#    yum -y update
#fi

if [ ${ARCH} == 'x86_64' ];
then
	if [ "$UNATTENDED" == 'n' ]; then
    ASK "Would you like to exclude installation of 32bit Yum packages? (Recommended for 64bit CentOS) [y/n] "
	else
	key='y'
	fi #unattended
    if [[ "$key" = [yY] ]];
    then
        mv /etc/yum.conf /etc/yum.bak
        cp $CUR_DIR/config/yum/yum.conf /etc/yum.conf
        echo "Your origional yum configuration has been backed up to /etc/yum.bak"
    else
        rm -rf $CUR_DIR/config/yum
    fi
fi

	if [ "$UNATTENDED" == 'n' ]; then
ASK "Would you like to secure /tmp and /var/tmp? (Highly recommended) [y/n] "   
	else
	key='y'
	fi #unattended
if [[ "$key" = [yY] ]]; 
then
echo "Centmin Mod secure /tmp completed # `date`" > ${DIR_TMP}/securedtmp.log
	echo "*************************************************"
	cecho "* Secured /tmp and /var/tmp" $boldgreen
	echo "*************************************************"

	rm -rf /tmp
	mkdir /tmp
	mount -t tmpfs -o rw,noexec,nosuid tmpfs /tmp
	chmod 1777 /tmp
	echo "tmpfs   /tmp    tmpfs   rw,noexec,nosuid        0       0" >> /etc/fstab
	rm -rf /var/tmp
	ln -s /tmp /var/tmp
fi

#questions

#yuminstall

	if [ "$UNATTENDED" == 'n' ]; then
ASK "Would you like to set the server localtime? [y/n] "  
	else
	key='y'
	fi #unattended 
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Setting preferred localtime for VPS" $boldgreen
    echo "*************************************************"
    rm -f /etc/localtime
    ln -s /usr/share/zoneinfo/$ZONEINFO /etc/localtime
    echo "Current date & time for the zone you selected is:"
    date
fi
}

# END FUNCTIONS
################################################################
# SCRIPT START
#
clear
cecho "**********************************************************************" $boldyellow
cecho "* Centmin, an nginx, MariaDB/MySQL, PHP & DNS Install script for CentOS" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE" $boldgreen
cecho "* $COPYRIGHT $SCRIPT_MODIFICATION_AUTHOR" $boldgreen
cecho "**********************************************************************" $boldyellow
echo " "
cecho "This software comes with no warranty of any kind. You are free to use" $boldyellow
cecho "it for both personal and commercial use as licensed under the GPL." $boldyellow
echo " "
cecho "Please read the included readme.txt before using this script." $boldmagenta
echo " "
	if [ "$UNATTENDED" == 'n' ]; then
ASK "Would you like to continue? [y/n] "   
if [[ "$key" = [nN] ]];
then
    exit 0
fi
	fi #unattended 

# Set LIBDIR
if [ ${ARCH} == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

#DIR_TMP="/svr-setup"
#if [ -a "$DIR_TMP" ];
#then
#	ASK "It seems that you have run this script before, would you like to start from after setting the timezone? [y/n] "
#	if [[ "$key" = [nN] ]];
#	then
#		run_once
#	fi
#else
#	mkdir $DIR_TMP
#	run_once
#fi

if [ ! -f "${DIR_TMP}/securedtmp.log" ]; then
run_once
fi

if [ -f /proc/user_beancounters ];
then
    cecho "OpenVZ system detected, NTP not installed" $boldgreen
else
    if [[ "$NTP_INSTALL" = [yY] ]]; 
    then
        echo "*************************************************"
        cecho "* Installing NTP (and syncing time)" $boldgreen
        echo "*************************************************"
        yum -y install ntp
        chkconfig --levels 235 ntpd on
        ntpdate pool.ntp.org
        echo "The date/time is now:"
        date
        echo "If this is correct, then everything is working properly"
        echo "*************************************************"
        cecho "* NTP installed" $boldgreen
        echo "*************************************************"
    fi
fi

ngxinstallmain

mariadbinstallfunct
mysqlinstallfunct

if [[ "$PHP_INSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Installing PHP" $boldgreen
    echo "*************************************************"

funct_centos6check

    export PHP_AUTOCONF=/usr/bin/autoconf-2.13
    export PHP_AUTOHEADER=/usr/bin/autoheader-2.13

    cd $DIR_TMP

if [ "$(rpm -qa | grep php*)" ]; then

    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
echo "yum -y erase php*"
    yum -y erase php*

fi

#download php tarball

    #tar xzvf php-${PHP_VERSION}.tar.gz

    cd php-${PHP_VERSION}

    ./buildconf --force
    mkdir fpm-build && cd fpm-build

    mkdir -p /usr/${LIBDIR}/mysql
    ln -s /usr/${LIBDIR}/libmysqlclient.so /usr/${LIBDIR}/mysql/libmysqlclient.so

funct_phpconfigure

    cd ../

#######################################################
# check to see if centmin custom php.ini already in place

CUSTOMPHPINICHECK=`grep 'realpath_cache_size = 1024k' /usr/local/lib/php.ini 2>/dev/null`

if [[ -z $CUSTOMPHPINICHECK ]]; then

    cp -f php.ini-production /usr/local/lib/php.ini
    chmod 644 /usr/local/lib/php.ini

phpsededit

fi # check to see if centmin custom php.ini already in place


echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem

#echo
#echo

if [[ "$lessphpmem" = [yY] ]]; then

echo $lessphpmem

echo -e "\nCopying php-fpm-min.conf /usr/local/etc/php-fpm.conf\n"
    cp $CUR_DIR/config/php-fpm/php-fpm-min.conf /usr/local/etc/php-fpm.conf

else

echo $lessphpmem

echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
    cp $CUR_DIR/config/php-fpm/php-fpm.conf /usr/local/etc/php-fpm.conf

fi


    cp $CUR_DIR/init/php-fpm /etc/init.d/php-fpm

    chmod +x /etc/init.d/php-fpm

    mkdir -p /var/run/php-fpm
    touch /var/run/php-fpm/php-fpm.pid
    chown nginx:nginx /var/run/php-fpm

    mkdir /var/log/php-fpm/
    touch /var/log/php-fpm/www-error.log
    touch /var/log/php-fpm/www-php.error.log
    chmod 0666 /var/log/php-fpm/www-error.log
    chmod 0666 /var/log/php-fpm/www-php.error.log
    fpmconfdir

    #chown -R root:nginx /var/lib/php/session/
    chkconfig --levels 235 php-fpm on
    #/etc/init.d/php-fpm restart 2>/dev/null
    /etc/init.d/php-fpm force-quit
    /etc/init.d/php-fpm start

if [[ `grep exclude /etc/yum.conf` && $MDB_INSTALL = y ]]; then

cecho "exclude line exists... adding nginx* mysql* php* exclusions" $boldgreen

sed -i "s/exclude=\*.i386 \*.i586 \*.i686 mysql\*/exclude=\*.i386 \*.i586 \*.i686 nginx\* mysql\* php\*/" /etc/yum.conf

sed -i "s/exclude=mysql\*/exclude=mysql\* php\*/" /etc/yum.conf

elif [[ `grep exclude /etc/yum.conf` ]]; then

cecho "exclude line exists... adding nginx* php* exclusions" $boldgreen

sed -i "s/exclude=\*.i386 \*.i586 \*.i686/exclude=\*.i386 \*.i586 \*.i686 nginx\* php\*/" /etc/yum.conf 

fi


if [[ ! `grep exclude /etc/yum.conf` && $MDB_INSTALL = y ]]; then

cecho "Can't find exclude line in /etc/yum.conf.. adding exclude line for nginx* mysql* php*" $boldgreen

echo "exclude=nginx* mysql* php*">> /etc/yum.conf

elif [[ ! `grep exclude /etc/yum.conf` ]]; then

cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for nginx* php*" $boldgreen

echo "exclude=nginx* php*">> /etc/yum.conf

fi

funct_logphprotate

    echo "*************************************************"
    cecho "* PHP installed" $boldgreen
    echo "*************************************************"
fi

xcacheinstall_ask

#if [ "$xcacheinstallcheck" != 'y' ]; then
    #cecho "* If you installed Xcache, DO NOT install APC" $boldgreen
#fi # xcacheinstallcheck

#ASK "Install APC? (By default uses 32MB RAM) [y/n] "
if [[ "$APCINSTALL" = [yY] ]];
then
funct_apcsourceinstall
fi


if [[ "$SENDMAIL_INSTALL" = [yY] ]]; 
then

	if [[ ! -f /etc/init.d/postfix ]]; then
		echo "*************************************************"
		cecho "* Installing sendmail" $boldgreen
		echo "*************************************************"
		yum -y -q install sendmail mailx sendmail-cf
		chkconfig --levels 235 sendmail on
		funct_sendmailmc
		#/etc/init.d/sendmail start
		echo "*************************************************"
		cecho "* sendmail installed" $boldgreen
		echo "*************************************************"
	
	else

		postfixsetup
	fi

	echo "*************************************************"
	cecho "Postfix already detected, sendmail install aborted" $boldgreen
	echo "*************************************************"

fi

incmemcachedinstall

imagickinstall

nsdinstall

csfinstalls

siegeinstall

installpythonfuct

if [ -f $CUR_DIR/Extras/nginx-update.sh ];
then
    chmod +x $CUR_DIR/Extras/nginx-update.sh
fi

echo " "

shortcutsinstall

echo " "

cecho "**********************************************************************" $boldgreen
cecho "* Starting Services..." $boldgreen
cecho "**********************************************************************" $boldgreen
if [[ "$NSD_INSTALL" = [yY] ]]; 
then
    /etc/init.d/nsd start
fi

if [ -f /etc/init.d/ntpd ];
then
    /etc/init.d/ntpd start
fi

if [[ "$NGINX_INSTALL" = [yY] ]]; 
then
    /etc/init.d/nginx start
fi

if [[ "$MDB_INSTALL" = [yY] || "$MDB_YUMREPOINSTALL" = [yY] ]]; 
then
    /etc/init.d/mysql start
fi

if [[ "$MYSQL_INSTALL" = [yY] ]]; 
then
    /etc/init.d/mysqld start
fi
echo " "
echo " "

cd

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf $DIR_TMP
    rm -rf $CUR_DIR/config
    rm -rf $CUR_DIR/init
    rm -rf $CUR_DIR/sysconfig
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

funct_openvz_stacksize

    echo "*************************************************"
    cecho "*  MariaDB Security Setup" $boldgreen
    echo "*************************************************"

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] || "$UNATTENDED" == [yY] ]]; then
	mysql_secure_installation
else
	mysql_secure_installation
fi

    echo "*************************************************"
    cecho "*  MariaDB Security Setup Completed" $boldgreen
    echo "*************************************************"

checkxcacheadmin

memcacheadmin

    echo "*************************************************"
    cecho "* Running updatedb command. Please wait...." $boldgreen
    echo "*************************************************"

updatedb

centminfinish

bookmark

sync 

if [ ! -f /proc/user_beancounters ]; then
echo 3 > /proc/sys/vm/drop_caches
fi

}


#####################################################################
#####################################################################
# functions

function funct_centos6check {


if [[ "$CENTOSVER" == '5.6' || "$CENTOSVER" == '5.7'|| "$CENTOSVER" == '5.8' || "$CENTOSVER" == '6.0' || "$CENTOSVER" == '6.1' || "$CENTOSVER" == '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

MCRYPT=" --with-mcrypt"

else

MCRYPT=""

fi

}

function funct_phpupgrade {

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
    else
        ARCH='i386'
    fi

# Set LIBDIR
if [ ${ARCH} == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

echo " "
ASK "PHP Upgrade - Would you like to continue? [y/n] "   
if [[ "$key" = [nN] ]];
then
    exit 0
fi

DIR_TMP="$DIR_TMP"
if [ ! -d "$DIR_TMP" ]; then
mkdir $DIR_TMP
fi

funct_mktempfile

echo ""
echo -n "Install which version of PHP? (version i.e. ${PHP_VERSION}): "
read phpver

PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)

zopcacheupgrade

    echo "*************************************************"
    cecho "* Upgrading PHP" $boldgreen
    echo "*************************************************"

funct_centos6check

    export PHP_AUTOCONF=/usr/bin/autoconf-2.13
    export PHP_AUTOHEADER=/usr/bin/autoheader-2.13

    cd $DIR_TMP

if [ "$(rpm -qa | grep php*)" ]; then

    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
echo "yum -y erase php*"
    yum -y erase php*

fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
DOWNLOADAPP='axel -a'
WGETRETRY=''
AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.gz"
AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.gz"
else
DOWNLOADAPP='wget -c --progress=bar'
WGETRETRY='--tries=3'
AXELPHPTARGZ=''
AXELPHPUPGRADETARGZ=''
fi

    if [ -s php-${phpver}.tar.gz ]; then
        cecho "php ${phpver} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${PHPUPGRADE_MIRRORURL}/get/php-${phpver}.tar.gz/from/$(echo $PHPUPGRADE_MIRRORURL | sed -e 's/http:\/\///')/mirror $AXELPHPUPGRADETARGZ $WGETRETRY
    fi

    tar xzvf php-${phpver}.tar.gz

    cd php-${phpver}

##############################################
# begin update php.ini

echo ""
echo "*************************************************"
cecho "* Updating php.ini... " $boldgreen
echo "*************************************************"
echo "old php.ini will be saved at /usr/local/lib/php.ini-oldversion_$DT"
echo "if you have modified your old php.ini i.e. installed and loaded"
echo "APC, Xcache, Memcache or Suhosin - please re-install them after php upgrade"
echo "..."

    cp -f /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT
    cp -f php.ini-production /usr/local/lib/php.ini
    chmod 644 /usr/local/lib/php.ini

phpsededit

PHPDIFF=`diff /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT`
echo ""
if [ ! -z "$PHPDIFF" ]; then
echo "checking updated php.ini compared to old /usr/local/lib/php.ini-oldversion_$DT"
echo "please note the following differences: "
echo "if APC, Xcache, Memcache, Suhosin or FFMPEG Extensions are listed please re-install them"
echo ""
echo "--------------------------------------------------------------------------------------------"
diff /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT
echo "--------------------------------------------------------------------------------------------"
echo ""
    echo "*************************************************"
echo "Automatically continuing upgrade in 30 seconds..."
    echo "*************************************************"
sleep 35
echo ""
fi # PHPDIFF

# end update php.ini
##############################################

    make clean

    ./buildconf --force
    
    if [ ! -d fpm-build ]; then
    mkdir fpm-build 
    fi
    
    cd fpm-build

    make clean

    #mkdir -p /usr/${LIBDIR}/mysql
    #ln -s /usr/${LIBDIR}/libmysqlclient.so /usr/${LIBDIR}/mysql/libmysqlclient.so

funct_phpconfigure

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP configure failed ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP configure ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

	fi

# error check
################

echo "make${MAKETHREADS}"
    make${MAKETHREADS}

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP make failed ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP make ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

	fi

# error check
################

echo "make install"
    make install

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, PHP wasn't installed properly ($SCRIPT_VERSION)\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: PHP was installed properly\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

echo
echo

read -ep "Do you want to update to latest php-fpm.conf ? It will overwrite your existing php-fpm.conf [y/n]: " overwritephpfpmconf

if [[ "$overwritephpfpmconf" = [yY] ]]; then

read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem

echo
echo "Backing up existing php-fpm.conf at /usr/local/etc/php-fpm.conf-oldversion_$DT"
echo "If you have made an custom changes to /usr/local/etc/php-fpm.conf, you will find"
echo "they still exist in backup at /usr/local/etc/php-fpm.conf-oldversion_$DT"

    cp -f /usr/local/etc/php-fpm.conf /usr/local/etc/php-fpm.conf-oldversion_$DT
    ls -lhrt /usr/local/etc/ | grep php-fpm.conf

sleep 20

echo
echo "Backing up existing php.conf at /usr/local/nginx/conf/php.conf-oldversion_$DT"
echo "If you have made an custom changes to /usr/local/nginx/conf/php.conf, you will find"
echo "they still exist in backup at /usr/local/nginx/conf/php.conf-oldversion_$DT"

    cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php.conf-oldversion_$DT
    ls -lhrt /usr/local/nginx/conf/ | grep php.conf
    cp -f $CUR_DIR/config/nginx/php.conf /usr/local/nginx/conf/php.conf

sleep 20

echo 

if [[ "$lessphpmem" = [yY] ]]; then

echo $lessphpmem

echo -e "\nCopying php-fpm-min.conf /usr/local/etc/php-fpm.conf\n"
    cp -f $CUR_DIR/config/php-fpm/php-fpm-min.conf /usr/local/etc/php-fpm.conf

else

echo $lessphpmem

echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
    cp -f $CUR_DIR/config/php-fpm/php-fpm.conf /usr/local/etc/php-fpm.conf

fi # lessphpmem

fi # overwritephpfpmconf

zendopcacheextfix

    /etc/init.d/php-fpm restart

funct_showtempfile
	cleanup_msg

if [ -f /usr/local/nginx/html/phpi.php ]; then
# Randomize phpi.php filename
NPHP=$RANDOM
mv /usr/local/nginx/html/phpi.php /usr/local/nginx/html/${NPHP}_phpi.php
chown nginx:nginx /usr/local/nginx/html/${NPHP}_phpi.php
fi

    echo "*************************************************"
    cecho "* PHP upgraded" $boldgreen
    echo "*************************************************"

	fi

# error check
################


if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf $DIR_TMP
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

}

# funct_memcachedreinstall was here

function funct_timestamp {


echo "..."
echo "Time:"   
echo "********************************"
echo "================================"
date
echo "================================"
echo "********************************"

}

function funct_installiopingcentmin {

echo ""
	cecho "--------------------------------------------------------" $boldyellow
echo "Where do you want the iopingcentmin.sh stored ? Enter path to download directory (i.e. /root or /usr/local/src): "
read iopingdownloadpath

    cd $iopingdownloadpath

if [ -s iopingcentmin.sh ]; then

	echo ""
	echo "iopingcentmin.sh [found]"

	echo ""
	echo "Do you want to download latest iopincentmin.sh version ? [y/n]: "
	read iopingdownloadupdate

	if [[ $iopingdownloadupdate = [yY] ]]; then

	rm -rf iopingcentmin.sh
	$DOWNLOADAPP http://vbtechsupport.com/centminmenu/iopingcentmin/iopingcentmin.sh $WGETRETRY

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	echo "script installed at $iopingdownloadpath/iopingcentmin.sh"
	echo "to manually run iopingcentmin.sh type:"
	echo "bash $iopingdownloadpath/iopingcentmin.sh"
	cecho "--------------------------------------------------------" $boldyellow
	echo ""

	exit

	else

	exit

	fi

  else
  echo "Error: iopingcentmin.sh not found!!!download now......"
  $DOWNLOADAPP http://vbtechsupport.com/centminmenu/iopingcentmin/iopingcentmin.sh $WGETRETRY

fi

chmod +x iopingcentmin.sh

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	echo "script installed at $iopingdownloadpath/iopingcentmin.sh"
	echo "to manually run iopingcentmin.sh type:"
	echo "bash $iopingdownloadpath/iopingcentmin.sh"
	cecho "--------------------------------------------------------" $boldyellow
	echo ""


}

function funct_selinux {

SELINUXCONFIGFILE='/etc/selinux/config'
SELINUXCHECK=`grep '^SELINUX=' /etc/selinux/config | cut -d '=' -f2`

if [[ $SELINUXCHECK == 'enforcing' ]]; then

	echo ""
cecho "---------------------------------------------" $boldyellow
	echo "Checking SELinux status...."
	echo "SELinux enabled"
	echo ""
	read -ep "Do you want to disable SELinux ? [y/n]: " disableselinux
cecho "---------------------------------------------" $boldyellow
	echo ""

	if [[ $disableselinux == [yY] ]]; then

	echo ""
cecho "---------------------------------------------" $boldyellow
	echo "Disabling SELinux..."

	sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' $SELINUXCONFIGFILE
	setenforce 0

	echo ""
cecho "---------------------------------------------" $boldyellow
	echo "checking $SELINUXCONFIGFILE"

	cat $SELINUXCONFIGFILE | grep '^SELINUX='

cecho "---------------------------------------------" $boldyellow
	echo ""

	exit

	else

	exit

	fi

else

	echo ""
cecho "---------------------------------------------" $boldyellow
	echo "checking $SELINUXCONFIGFILE"
	echo "SELinux already disabled"
	echo "SELINUX=$SELINUXCHECK"
cecho "---------------------------------------------" $boldyellow
	echo ""

	exit

fi

}

function funct_showtempfile {

    echo "*************************************************"
	cat "$TMP_MSGFILE"
    echo "*************************************************"

}

function funct_mktempfile {

if [[ ! -d "$DIR_TMP"/msglogs ]]; then
cd $DIR_TMP
mkdir msglogs
chmod 1777 $DIR_TMP/msglogs
fi

TMP_MSGFILE="$DIR_TMP/msglogs/$RANDOM.msg"

}

function cleanup_msg {
	rm -f "$TMP_MSGFILE"
	#exit 1
}

# http://linuxcommand.org/wss0160.php
trap clean_msg SIGHUP SIGINT SIGTERM

# end functions
#####################################################################
#####################################################################


#########################################################

if [[ "$ENABLE_MENU" = [yY] ]]; then

while :
do
	# clear
        # display menu
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Centmin Mod $SCRIPT_VERSION - $SCRIPT_URL" $boldgreen
	#cecho "Menu/Mods Author: $SCRIPT_MODIFICATION_AUTHOR" $boldgreen
	#cecho "Centmin Original Author: $SCRIPT_AUTHOR" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "                   Centmin Mod Menu                   " $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "1).  Centmin Install" $boldgreen
	cecho "2).  Add Nginx vhost domain" $boldgreen
	cecho "3).  NSD setup domain name DNS" $boldgreen
	cecho "4).  Nginx Upgrade / Downgrade" $boldgreen
	cecho "5).  PHP Upgrade / Downgrade" $boldgreen
	cecho "6).  XCache Re-install" $boldgreen
	cecho "7).  APC Cache Re-install" $boldgreen
	cecho "8).  XCache Install" $boldgreen
	cecho "9).  APC Cache Install" $boldgreen
	cecho "10). Memcached Server Re-install" $boldgreen
	cecho "11). MariaDB 5.2.x Branch Upgrade Only" $boldgreen
	cecho "12). MariaDB 5.2.x to MariaDB 5.5 YUM upgrade" $boldgreen
	cecho "13). Install ioping.sh vbtechsupport.com/1239/" $boldgreen
	cecho "14). SELinux disable" $boldgreen
	cecho "15). Install/Re-install imagick PHP Extension" $boldgreen
	cecho "16). Change SSHD Port Number" $boldgreen
	cecho "17). Multi-thread compression: pigz,pbzip2,lbzip2,p7zip etc" $boldgreen
	cecho "18). Suhosin PHP Extension install" $boldgreen
	cecho "19). Install FFMPEG and FFMPEG PHP Extension" $boldgreen
	cecho "20). NSD Re-install" $boldgreen
	cecho "21). Exit" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow

	read -ep "Enter option [ 1 - 21 ] " option
	cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$option" in
1|install)

starttime=$(date +%s.%N)

lowmemcheck
centminlog
diskalert
{

CHECKCENTMINMODINSTALL=`ls /etc/init.d | grep -E '(csf|lfd|nginx|php-fpm|^nsd)'`
if [ ! -z "$CHECKCENTMINMODINSTALL" ]; then
echo ""
echo "Centmin Mod previous installation detected. "
echo ""
echo "If you are upgrading a server which already previously had Centmin Mod installed"
echo "you DO NOT need to run option #1, instead run option #4 & then #5 for upgrading"
echo "Nginx web server and upgrading PHP."
echo ""
echo "exiting script"
exit
fi

alldownloads
funct_centmininstall
echo "$SCRIPT_VERSION" > /etc/centminmod-release
#echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log
echo "Total Centmin Mod Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log

exit 0

;;
2|addvhost)

centminlog
{
funct_nginxaddvhost
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log

;;
3|nsdsetup)

centminlog
{
funct_nsdsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log

;;
4|nginxupgrade)

starttime=$(date +%s.%N)

centminlog
diskalert
csftweaks

{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

yumskipinstall
if [[ "$yuminstallrun" == [yY] ]]; then
yuminstall
fi
funct_nginxupgrade
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log
echo "Total Nginx Upgrade Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log

;;
5|phpupgrade)

starttime=$(date +%s.%N)

centminlog
diskalert
csftweaks

{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

yumskipinstall
if [[ "$yuminstallrun" == [yY] ]]; then
yuminstall
fi
funct_phpupgrade
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log
echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log

;;
6|xcachereinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_xcachereinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log
echo "Total Xcache Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log

;;
7|apcreinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_apcreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log
echo "Total APC Cache Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log

;;
8|installxcache)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

MANXCACHEINSTALL='y'

funct_installxcache
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log
echo "Total Xcache Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log

;;
9|installapc)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_installapc
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log
echo "Total APC Cache Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log

;;
10|memcachedreinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_memcachedreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log
echo "Total Memcached Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log

;;
11|mariadbupgrade)

centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-3`

# for MariaDB 5.2 upgrade option
if [[ "$MARIADBVERCHECK" == '5.3' || "$MARIADBVERCHECK" == '5.5' ]]; then

echo ""
cecho "MariaDB 5.3.x or 5.5.x server detected" $boldyellow
cecho "downgrading to MariaDB 5.2.x is not recommended" $boldyellow
cecho "aborting MariaDB 5.2.x upgrade option..." $boldyellow
sleep 5
exit

else

funct_mariadbupgrade

fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb52_upgrade.log

;;
12|mariadb55upgrade)

centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-3`

# for MariaDB 5.5 upgrade option
if [ "$MARIADBVERCHECK" == '5.5' ]; then

echo ""
cecho "MariaDB 5.5.x server already detected" $boldyellow
cecho "MariaDB 5.5 uses normal YUM commands for updates as follows:" $boldyellow
echo ""
cecho " yum update --disablerepo=CentALT MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared MariaDB-test" $boldgreen
echo ""
cecho "aborting MariaDB 5.5.x upgrade option..." $boldyellow
sleep 5
exit

else

mariadbfiveupgrade

fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb55_upgrade.log

;;
13|iopinginstall)

funct_installiopingcentmin

;;
14|selinux)

funct_selinux

;;
15|imagick)

imagickinstall

;;
16|sshdport)

funct_sshd

;;
17|multithreadcomp)

funct_pigzinstall
funct_pbzip2install
funct_lbzip2install
funct_lzipinstall
funct_plzipinstall
funct_p7zipinstall

;;
18|suhosininstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

suhosinsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log
	fi
fi

;;
19|ffmpeginstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

ffmpegsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log
	fi
fi

;;
20|nsdreinstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

nsdreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log
	fi
fi

;;
21|exit)

bookmark

exit 0

;;

esac

done

else

case "$1" in
install)

lowmemcheck
diskalert
alldownloads
funct_centmininstall
echo "$SCRIPT_VERSION" > /etc/centminmod-release
echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog

;;
addvhost)

funct_nginxaddvhost

;;
nsdsetup)

funct_nsdsetup

;;
nginxupgrade)

diskalert
csftweaks
yuminstall
funct_nginxupgrade

;;
phpupgrade)

diskalert
csftweaks
yuminstall
funct_phpupgrade

;;
xcachereinstall)

funct_xcachereinstall

;;
apcreinstall)

funct_apcreinstall

;;
installxcache)

funct_installxcache

;;
installapc)

funct_installapc

;;
memcachedreinstall)

funct_memcachedreinstall

;;
mariadbupgrade)

funct_mariadbupgrade

;;
iopinginstall)

funct_installiopingcentmin

;;
selinux)

funct_selinux

;;
logrotate)

funct_logrotate

;;
phplogrotate)

funct_logphprotate

;;
sshdport)

funct_sshd

;;
multithreadcomp)

funct_pigzinstall
funct_pbzip2install
funct_lbzip2install
funct_lzipinstall
funct_plzipinstall
funct_p7zipinstall

;;
suhosininstall)
suhosinsetup
;;
ffmpeginstall)
ffmpegsetup
;;
nsdreinstall)
nsdreinstall
;;
exit)

echo ""
echo "exit"

bookmark

exit 0

;;
*)

echo "$0 install"
echo "$0 addvhost"
echo "$0 nsdsetup"
echo "$0 nginxupgrade"
echo "$0 phpupgrade"
echo "$0 xcachereinstall"
echo "$0 apcreinstall"
echo "$0 installxcache"
echo "$0 installapac"
echo "$0 memcachedreinstall"
echo "$0 mariadbupgrade"
echo "$0 installioping"
echo "$0 selinux"
echo "$0 logrotate"
echo "$0 phplogrotate"
echo "$0 sshdport"
echo "$0 multithreadcomp"
echo "$0 suhosininstall"
echo "$0 ffmpeginstall"
echo "$0 nsdreinstall"

;;
esac

fi

exit 0