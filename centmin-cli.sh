#!/bin/bash
#####################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
#####################################################
EMAIL=''          # Server notification email address enter only 1 address
PUSHOVER_EMAIL='' # Signup pushover.net push email notifications to mobile & tablets
ZONEINFO=Etc/UTC  # Set Timezone
NGINX_IPV='n'     # option deprecated from 1.11.5+ IPV6 support
USEEDITOR='nano'  # choice between nano or vim text editors for cmd shortcuts
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

CUSTOMSERVERNAME='y'
CUSTOMSERVERSTRING='nginx centminmod'
PHPFPMCONFDIR='/usr/local/nginx/conf/phpfpmd'

UNATTENDED='y' # please leave at 'y' for best compatibility as at .07 release
CMVERSION_CHECK='n'
MENUEXIT_ALWAYS_YUMCHECK='y'  # also do yum check on centmin.sh exit
CMSDEBUG='n'
#####################################################
DT=$(date +"%d%m%y-%H%M%S")
# for github support
branchname='123.09beta01'
SCRIPT_MAJORVER='1.2.3'
SCRIPT_MINORVER='09'
SCRIPT_INCREMENTVER='657'
SCRIPT_VERSIONSHORT="${branchname}"
SCRIPT_VERSION="${SCRIPT_VERSIONSHORT}.b${SCRIPT_INCREMENTVER}"
SCRIPT_DATE='03/03/2021'
SCRIPT_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_MODIFICATION_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_URL='https://centminmod.com'
COPYRIGHT="Copyright 2011-2021 CentminMod.com"
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
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

if [ "$(id -u)" != 0 ]; then
  echo "script needs to be run as root user" >&2
  if [ "$(id -Gn | grep -o wheel)" ]; then
    echo "if using a sudo user, switch to full root first:" >&2
    echo >&2
    echo "sudo -i" >&2
  fi
  exit 1
fi

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done
#####################################################
HN=$(uname -n)
# Pre-Checks to prevent screw ups
DIR_TMP='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'

source "inc/memcheck.inc"
TMPFSLIMIT=2900000
if [ ! -d "$DIR_TMP" ]; then
        if [[ "$TOTALMEM" -ge "$TMPFSLIMIT" ]]; then
            TMPFSENABLED=1
            RAMDISKTMPFS='y'
            echo "setting up $DIR_TMP on tmpfs ramdisk for initial install"
            mkdir -p "$DIR_TMP"
            chmod 0750 "$DIR_TMP"
            mount -t tmpfs -o size=2200M,mode=0755 tmpfs "$DIR_TMP"
            df -hT
        else
            mkdir -p "$DIR_TMP"
            chmod 0750 "$DIR_TMP"
        fi
fi

if [[ -z "$(cat /etc/resolv.conf)" ]]; then
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

if [ ! -f /usr/bin/lynx ]; then
echo "installing lynx..."
yum -y -q install lynx
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

if [ ! -d /var/run/php-fpm/ ]; then
    mkdir -p /var/run/php-fpm/
fi

TESTEDCENTOSVER='7.9'
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
fi

source "inc/centos_seven.inc"
seven_function

cmservice() {
  servicename=$1
  action=$2
  if [[ "$CENTOS_SEVEN" != '7' ]] && [[ "${servicename}" = 'haveged' || "${servicename}" = 'pure-ftpd' || "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
    echo "service ${servicename} $action"
    if [[ "$CMSDEBUG" = [nN] ]]; then
      service "${servicename}" "$action"
    fi
  else
    if [[ "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' ]]; then
      echo "service ${servicename} $action"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        service "${servicename}" "$action"
      fi
    elif [[ "${servicename}" = 'mysql' || "${servicename}" = 'mysqld' ]]; then
      servicename='mariadb'
      echo "systemctl $action ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        systemctl "$action" "${servicename}.service"
      fi
    fi
  fi
}

cmchkconfig() {
  servicename=$1
  status=$2
  if [[ "$CENTOS_SEVEN" != '7' ]] && [[ "${servicename}" = 'haveged' || "${servicename}" = 'pure-ftpd' || "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
    echo "chkconfig ${servicename} $status"
    if [[ "$CMSDEBUG" = [nN] ]]; then
      chkconfig "${servicename}" "$status"
    fi
  else
    if [[ "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' ]]; then
      echo "chkconfig ${servicename} $status"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        chkconfig "${servicename}" "$status"
      fi
    else
      if [ "$status" = 'on' ]; then
        status=enable
      fi
      if [ "$status" = 'off' ]; then
        status=disable
      fi
      echo "systemctl $status ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        systemctl "$status" "${servicename}.service"
      fi
    fi
  fi
}

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

# configure .ini directory
CONFIGSCANBASE='/etc/centminmod'
CONFIGSCANDIR="${CONFIGSCANBASE}/php.d"

if [ ! -d "$CONFIGSCANBASE" ]; then
  mkdir -p "$CONFIGSCANBASE"
fi

if [ ! -d "$CONFIGSCANDIR" ]; then
  mkdir -p "$CONFIGSCANDIR"
  if [ -d /root/centminmod/php.d/ ]; then
      cp -a /root/centminmod/php.d/* "${CONFIGSCANDIR}/"
    fi
fi

# MySQL non-tmpfs based tmpdir for MySQL temp files
if [ ! -d "/home/mysqltmp" ]; then
  mkdir -p /home/mysqltmp
  chmod 1777 /home/mysqltmp
  CHOWNMYSQL=y
fi

#####################################################
# Centmin Mod Git Repo URL - primary repo
# https://github.com/centminmod/centminmod
CMGIT='https://github.com/centminmod/centminmod.git'
# Gitlab backup repo 
# https://gitlab.com/centminmod/centminmod
#CMGIT='https://gitlab.com/centminmod/centminmod.git'

# With AUTO_GITUPDATE='y' if centmin mod code install 
# directory has been setup with git environment via 
# centmin.sh menu option 23 # submenu option 1, then 
# allow centmin.sh to auto update # the centmin mod 
# code at /usr/local/src/centminmod 
# silently in background
#
# if you want to retain local centmin mod code changes
# made to files in /usr/local/src/centminmod for variables
# in centmin.sh, use persistent config file you create
# or append to at /etc/centminmod/custom_config.inc as
# outlined on official site at 
# https://centminmod.com/upgrade.html#persistent
AUTO_GITUPDATE='n'
#####################################################
LOCALCENTMINMOD_MIRROR='https://centminmod.com'
#####################################################
# Timestamp Install
TS_INSTALL='y'
TIME_NGINX='n'
TIME_PHPCONFIGURE='n'
TIME_MEMCACHED='n'
TIME_IMAGEMAGICK='n'
TIME_REDIS='n'

#####################################################
# Enable or disable menu mode
ENABLE_MENU='n'

#####################################################
# CentOS 7 specific
FIREWALLD_DISABLE='y'
DNF_ENABLE='n'
DNF_COPR='y'

#####################################################
# CSF FIREWALL
# PORTFLOOD Configuration
# https://community.centminmod.com/threads/14708/
# Setting CSFPORTFLOOD_OVERRIDE='y' allows you to 
# override default CSF Firewall PORTFLOOD values set
# by Centmin Mod initial install. If end user made
# custom changes to PORTFLOOD values, the override 
# will not work. Override only works if end user has
# not made custom changes to PORTFLOOD values to ensure
# end users customisations do not get overwritten
CSFPORTFLOOD_OVERRIDE='n'
# max hit count value allowed is 20
PORTFLOOD_COUNT=20
# lowering interval in seconds allows for more
# port flood hits against default TCP port 21
PORTFLOOD_INTERVAL=300

#####################################################
# MariaDB Jemalloc
# doh forgot MariaDB already uses jemalloc by default
MARIADB_JEMALLOC='n'

#####################################################
# CCACHE Configuration
CCACHEINSTALL='y'
CCACHE_VER="3.7.12"
CCACHESIZE='2.5G'

#####################################################
# Maxmind GeoLite2 database API Key
# https://community.centminmod.com/posts/80656/
# You can override this API key with your own Maxmind
# account API key by setting MM_LICENSE_KEY variable 
# in persistent config file /etc/centminmod/custom_config.inc
MM_LICENSE_KEY='k0sP8JPgZm6i0sOF'
MM_CSF_SRC='y'

#####################################################
# Networking
# do not edit below variables but instead set them in
# /etc/centminmod/custom_config.inc as outlined on 
# official site at 
# https://centminmod.com/upgrade.html#persistent to
# override defaults
# disable system IPv6 support
# https://wiki.centos.org/FAQ/CentOS7#head-8984faf811faccca74c7bcdd74de7467f2fcd8ee
DISABLE_IPVSIX='n'

#####################################################
# experimental use of subshells to download some
# tarballs in parallel for faster initial installs
PARALLEL_MODE=y
# compiler related
MARCH_TARGETNATIVE='y'        # for intel 64bit only set march=native, if no set to x86-64
MARCH_TARGETNATIVE_ALWAYS='n' # force native compiler to override smarter vps detection routine
CLANG='n'                     # Nginx and LibreSSL
CLANG_FOUR='n'                # Clang 4.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_FIVE='n'                # Clang 5.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_SIX='n'                 # Clang 6.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_PHP='n'                 # PHP
CLANG_APC='n'                 # APC Cache
CLANG_MEMCACHED='n'           # Memcached menu option 10 routine
GCCINTEL_PHP='y'              # enable PHP-FPM GCC compiler with Intel cpu optimizations
PHP_PGO='n'                   # Profile Guided Optimization https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php
PHP_PGO_ALWAYS='n'            # override for PHP_PGO enable for 1 cpu thread servers too
PHP_PGO_TRAINRUNS='10'        # number of runs done during PGO PHP 7 training runs
PHP_PGO_CENTOSSIX='n'         # CentOS 6 may need GCC >4.4.7 fpr PGO so use lastest devtoolset
DEVTOOLSET_PHP='n'            # use devtoolset GCC for GCCINTEL_PHP='y'
DEVTOOLSETSIX='n'             # Enable or disable devtoolset-6 GCC 6.2 support instead of lastest devtoolset support
DEVTOOLSETSEVEN='n'           # Enable or disable devtoolset-7 GCC 7.1 support instead of devtoolset-6 GCC 6.2 support
DEVTOOLSETEIGHT='y'           # source compiled GCC 8 from latest snapshot builds
NGINX_DEVTOOLSETGCC='y'       # Use lastest devtoolset even for CentOS 7 nginx compiles
GENERAL_DEVTOOLSETGCC='n'     # Use lastest devtoolset whereever possible/coded
CRYPTO_DEVTOOLSETGCC='y'      # Use lastest devtoolset for libressl or openssl compiles
NGX_GSPLITDWARF='y'           # for Nginx compile https://community.centminmod.com/posts/44072/
PHP_GSPLITDWARF='y'           # for PHP compile https://community.centminmod.com/posts/44072/
PHP_LTO='n'                   # enable -flto compiler for GCC 4.8.5+ PHP-FPM compiles currently not working with PHP 7.x
NGX_LDGOLD='y'                # for Nginx compile i.e. passing ld.gold linker -fuse-ld=bfd or -fuse-ld=gold https://community.centminmod.com/posts/44037/
NGINX_FATLTO_OBJECTS='n'        # enable -ffat-lto-objects flag for nginx builds - much slower compile times
NGINX_NOFATLTO_OBJECTS='n'      # enable -fno-fat-lto-objects flag for nginx builds - much slower compile times
# recommended to keep NGINXOPENSSL_FATLTO_OBJECTS and NGINXOPENSSL_NOFATLTO_OBJECTS set to = n
NGINXOPENSSL_FATLTO_OBJECTS='n' # enable -ffat-lto-objects flag for nginx OpenSSL builds - much slower compile times
NGINXOPENSSL_NOFATLTO_OBJECTS='n' # enable -fno-fat-lto-objects flag for nginx OpenSSL builds - much slower compile times
NGINXCOMPILE_FORMATSEC='y'    # whether or not nginx is compiled with -Wformat -Werror=format-security flags

# When set to =y, will disable those listed installed services 
# by default. The service is still installed but disabled 
# by default and can be re-enabled with commands:
# service servicename start; chkconfig servicename on
NSD_DISABLED='n'              # when set to =y, NSD disabled by default with chkconfig off
MEMCACHED_DISABLED='n'        # when set to =y,  Memcached server disabled by default via chkconfig off
PHP_DISABLED='n'              # when set to =y,  PHP-FPM disabled by default with chkconfig off
MYSQLSERVICE_DISABLED='n'     # when set to =y,  MariaDB MySQL service disabled by default with chkconfig off
PUREFTPD_DISABLED='n'         # when set to =y, Pure-ftpd service disabled by default with chkconfig off

# Nginx Dynamic Module Switches
NGXDYNAMIC_MANUALOVERRIDE='n' # set to 'y' if you want to manually drop in nginx dynamic modules into /usr/local/nginx/modules
NGXDYNAMIC_NJS='n'
NGXDYNAMIC_XSLT='n'
NGXDYNAMIC_PERL='n'
NGXDYNAMIC_IMAGEFILTER='y'
NGXDYNAMIC_GEOIP='n'
NGXDYNAMIC_GEOIPTWOLITE='n'
NGXDYNAMIC_STREAM='n'
NGXDYNAMIC_STREAMGEOIP='n'  # nginx 1.11.3+ option http://hg.nginx.org/nginx/rev/558db057adaa
NGXDYNAMIC_STREAMREALIP='n' # nginx 1.11.4+ option http://hg.nginx.org/nginx/rev/9cac11efb205
NGXDYNAMIC_HEADERSMORE='y'
NGXDYNAMIC_SETMISC='y'
NGXDYNAMIC_ECHO='y'
NGXDYNAMIC_LUA='y'          #
NGXDYNAMIC_SRCCACHE='n'
NGXDYNAMIC_DEVELKIT='y'     #
NGXDYNAMIC_MEMC='n'
NGXDYNAMIC_REDISTWO='n'
NGXDYNAMIC_NGXPAGESPEED='n'
NGXDYNAMIC_BROTLI='y'
NGXDYNAMIC_FANCYINDEX='y'
NGXDYNAMIC_HIDELENGTH='y'
NGXDYNAMIC_TESTCOOKIE='n'
NGXDYNAMIC_VHOSTSTATS='n'

# set = y to put nginx, php and mariadb major version updates into 503 
# maintenance mode https://community.centminmod.com/posts/26485/
NGINX_UPDATEMAINTENANCE='n'
PHP_UPDATEMAINTENANCE='n'
MARIADB_UPDATEMAINTENANCE='n'

# General Configuration
NGINXCOMPILE_PIE='n'         # build nginx with Position-independent code (PIC) / Position-indendendent executables (PIEs)
NGINXUPGRADESLEEP='3'
AUTOTUNE_CLIENTMAXBODY='y'   # auto tune client_max_body_size option in nginx.conf
AUTOHARDTUNE_NGINXBACKLOG='n' # on non-openvz systems, if enabled will override nginx default NGX_LISTEN_BACKLOG in src/os/unix/ngx_linux_config.h
USE_NGINXMAINEXTLOGFORMAT='n' # use default combined nginx log format instead of main_ext custom format for nginx amplify
NGINX_ALLOWOVERRIDE='y'      # allow centmin mod to update nginx.conf setting defaults when the defaults are revised
NGINX_SSLCACHE_ALLOWOVERRIDE='n' # dynamically tune nginx ssl_session_cache in /usr/local/nginx/conf/ssl_include.conf based on system detected memory
NSD_INSTALL='n'              # Install NSD (DNS Server)
NSD_VERSION='3.2.18'         # NSD Version
NTP_INSTALL='y'              # Install Network time protocol daemon
NGINXPATCH='y'               # Set to y to allow NGINXPATCH_DELAY seconds time before Nginx configure and patching Nginx
NGINX_IOURING_PATCH='n'      # Experimental Nginx AIO patch for Linux 5.1+ Kernel systems only
NGINXPATCH_DELAY='1'         # Number of seconds to pause Nginx configure routine during Nginx upgrades
STRIPNGINX='y'               # set 'y' to strip nginx binary to reduce size
NGXMODULE_ALTORDER='y'       # nginx configure module ordering alternative order
NGINX_COMPILE_EXPORT='y'     # nginx compile export symbols when mixing nginx static and dynamic compiled libraries
NGINX_ZERODT='n'             # nginx zero downtime reloading on nginx upgrades
NGINX_MAXERRBYTELIMIT='2048' # modify NGX_MAX_ERROR_STR hardcoded 2048 limit by editing value i.e. http://openresty-reference.readthedocs.io/en/latest/Lua_Nginx_API/#print
NGINX_INSTALL='y'            # Install Nginx (Webserver)
NGINX_DEBUG='n'              # Enable & reinstall Nginx debug log nginx.org/en/docs/debugging_log.html & wiki.nginx.org/Debugging
NGINX_HTTP2='y'              # Nginx http/2 patch https://community.centminmod.com/threads/4127/
NGINX_HTTPPUSH='n'           # Nginx http/2 push patch https://community.centminmod.com/threads/11910/
NGINX_ZLIBNG='n'             # 64bit OS only for Nginx compiled against zlib-ng https://github.com/Dead2/zlib-ng
NGINX_MODSECURITY='n'        # modsecurity module support https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#Installation_for_NGINX
NGINX_MODSECURITY_MAXMIND='y' # modsecurity built with libmaxminddb is failing to compile so disable it in favour of GeoIP legacy
MODSECURITY_OWASPVER='3.3.0' # owasp modsecurity ruleset https://github.com/coreruleset/coreruleset/releases
NGINX_REALIP='y'             # http://nginx.org/en/docs/http/ngx_http_realip_module.html
NGINX_RDNS='n'               # https://github.com/flant/nginx-http-rdns
NGINX_NJS='n'                # nginScript https://www.nginx.com/blog/launching-nginscript-and-looking-ahead/
NGINX_GEOIP='y'              # Nginx GEOIP module install
NGINX_GEOIPMEM='y'           # Nginx caches GEOIP databases in memory (default), setting 'n' caches to disk instead
NGINX_GEOIPTWOLITE='n'       # https://github.com/leev/ngx_http_geoip2_module
NGINX_SPDY='n'               # Nginx SPDY support
NGINX_SPDYPATCHED='n'        # Cloudflare HTTP/2 + SPDY patch https://github.com/cloudflare/sslconfig/blob/master/patches/nginx__http2_spdy.patch
NGINX_STUBSTATUS='y'         # http://nginx.org/en/docs/http/ngx_http_stub_status_module.html required for nginx statistics
NGINX_SUB='y'                # http://nginx.org/en/docs/http/ngx_http_sub_module.html
NGINX_ADDITION='y'           # http://nginx.org/en/docs/http/ngx_http_addition_module.html
NGINX_IMAGEFILTER='y'        # http://nginx.org/en/docs/http/ngx_http_image_filter_module.html
NGINX_PERL='n'               # http://nginx.org/en/docs/http/ngx_http_perl_module.html
NGINX_XSLT='n'               # http://nginx.org/en/docs/http/ngx_http_xslt_module.html
NGINX_LENGTHHIDE='n'         # https://github.com/nulab/nginx-length-hiding-filter-module
NGINX_LENGTHHIDEGIT='y'      # triggers only if NGINX_LENGTHHIDE='y'
NGINX_TESTCOOKIE='n'         # https://github.com/kyprizel/testcookie-nginx-module
NGINX_TESTCOOKIEGIT='n'      # triggers only if NGINX_TESTCOOKIE='y'
NGINX_CACHEPURGE='y'         # https://github.com/FRiCKLE/ngx_cache_purge/
NGINX_ACCESSKEY='n'          #
NGINX_HTTPCONCAT='n'         # https://github.com/alibaba/nginx-http-concat
NGINX_THREADS='y'            # https://www.nginx.com/blog/thread-pools-boost-performance-9x/
NGINX_SLICE='n'              # https://nginx.org/en/docs/http/ngx_http_slice_module.html
NGINX_STREAM='y'             # http://nginx.org/en/docs/stream/ngx_stream_core_module.html
NGINX_STREAMGEOIP='y'        # nginx 1.11.3+ option http://hg.nginx.org/nginx/rev/558db057adaa
NGINX_STREAMREALIP='y'       # nginx 1.11.4+ option http://hg.nginx.org/nginx/rev/9cac11efb205
NGINX_STREAMSSLPREREAD='y'   # nginx 1.11.5+ option https://nginx.org/en/docs/stream/ngx_stream_ssl_preread_module.html
NGINX_RTMP='n'               # Nginx RTMP Module support https://github.com/arut/nginx-rtmp-module
NGINX_FLV='n'                # http://nginx.org/en/docs/http/ngx_http_flv_module.html
NGINX_MP4='n'                # Nginx MP4 Module http://nginx.org/en/docs/http/ngx_http_mp4_module.html
NGINX_AUTHREQ='n'            # http://nginx.org/en/docs/http/ngx_http_auth_request_module.html
NGINX_SECURELINK='y'         # http://nginx.org/en/docs/http/ngx_http_secure_link_module.html
NGINX_FANCYINDEX='y'         # https://github.com/aperezdc/ngx-fancyindex/releases
NGINX_FANCYINDEXVER='0.4.2'  # https://github.com/aperezdc/ngx-fancyindex/releases
NGINX_VHOSTSTATS='n'         # https://github.com/vozlt/nginx-module-vts
NGINX_LIBBROTLI='n'          # https://github.com/eustas/ngx_brotli
NGINX_LIBBROTLISTATIC='n'    # only enable if you want pre-compress brotli support and on the fly brotli disabled
NGINX_BROTLIDEP_UPDATE='n'   # experimental manual update of Google Brotli dependency in ngx_brotli
NGINX_PAGESPEED='n'          # Install ngx_pagespeed
NGINX_PAGESPEEDGITMASTER='n' # Install ngx_pagespeed from official github master instead  
NGXPGSPEED_VER='1.13.35.2-stable'
NGINX_PAGESPEEDPSOL_VER='1.13.35.2'
NGINX_PASSENGER='n'          # Install Phusion Passenger requires installing addons/passenger.sh before hand
NGINX_WEBDAV='n'             # Nginx WebDAV and nginx-dav-ext-module
NGINX_EXTWEBDAVVER='0.0.3'   # nginx-dav-ext-module version
NGINX_LIBATOMIC='y'          # Nginx configured with libatomic support
NGINX_LIBATOMIC_VERSION='7.6.10' # Newer than system default 7.2 for CentOS 7 only https://github.com/ivmai/libatomic_ops/releases
NGINX_HTTPREDIS='y'          # Nginx redis http://wiki.nginx.org/HttpRedisModule
NGINX_HTTPREDISVER='0.3.7'   # Nginx redis version
NGINX_PCREJIT='y'            # Nginx configured with pcre & pcre-jit support
NGINX_PCRE_DYNAMIC='y'       # compile nginx pcre as dynamic instead of static library
NGINX_PCREVER='8.44'         # Version of PCRE used for pcre-jit support in Nginx
NGINX_ZLIBCUSTOM='y'         # Use custom zlib instead of system version
NGINX_ZLIBVER='1.2.11'       # http://www.zlib.net/
NGINX_VIDEO='n'              # control variable when 'y' set for NGINX_SLICE='y', NGINX_RTMP='y', NGINX_FLV='y', NGINX_MP4='y'
ORESTY_HEADERSMORE='y'       # openresty headers more https://github.com/openresty/headers-more-nginx-module
ORESTY_HEADERSMOREGIT='n'    # use git master instead of version specific
NGINX_HEADERSMORE='0.33'
NGINX_CACHEPURGEVER='2.5.1'
NGINX_STICKY='n'             # nginx sticky module https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
NGINX_STICKYVER='master'
NGINX_UPSTREAMCHECK='n'      # nginx upstream check https://github.com/yaoweibin/nginx_upstream_check_module
NGINX_UPSTREAMCHECKVER='0.3.0'
NGINX_OPENRESTY='y'          # Agentzh's openresty Nginx modules
ORESTY_MEMCVER='0.19'        # openresty memc module https://github.com/openresty/memc-nginx-module
ORESTY_SRCCACHEVER='0.32'    # openresty subrequest cache module https://github.com/openresty/srcache-nginx-module
ORESTY_DEVELKITVER='0.3.0'  # openresty ngx_devel_kit module https://github.com/simpl/ngx_devel_kit
ORESTY_SETMISCGIT='n'        # use git master instead of version specific
ORESTY_SETMISC='y'           # openresty set-misc-nginx module https://github.com/openresty/echo-nginx-module
ORESTY_SETMISCVER='0.32'     # openresty set-misc-nginx module https://github.com/openresty/set-misc-nginx-module
ORESTY_ECHOGIT='n'           # use git master instead of version specific
ORESTY_ECHOVER='0.62'        # openresty set-misc-nginx module https://github.com/openresty/echo-nginx-module
ORESTY_REDISVER='0.15'       # openresty redis2-nginx-module https://github.com/openresty/redis2-nginx-module

LUAJIT_GITINSTALL='y'        # opt to install luajit 2.1 from dev branch http://repo.or.cz/w/luajit-2.0.git/shortlog/refs/heads/v2.1
LUAJIT_GITINSTALLVER='2.1-agentzh'   # branch version = v2.1 will override ORESTY_LUAGITVER if LUAJIT_GITINSTALL='y'

ORESTY_LUANGINX='n'             # enable or disable or ORESTY_LUA* nginx modules below
ORESTY_LUANGINXVER='0.10.19'  # openresty lua-nginx-module https://github.com/openresty/lua-nginx-module
ORESTY_LUAGITVER='2.0.5'        # luagit http://luajit.org/
ORESTY_LUAMEMCACHEDVER='0.15'   # openresty https://github.com/openresty/lua-resty-memcached
ORESTY_LUAMYSQLVER='0.23'    # openresty https://github.com/openresty/lua-resty-mysql
ORESTY_LUAREDISVER='0.29'       # openresty https://github.com/openresty/lua-resty-redis
ORESTY_LUADNSVER='0.21'         # openresty https://github.com/openresty/lua-resty-dns
ORESTY_LUAUPLOADVER='0.10'      # openresty https://github.com/openresty/lua-resty-upload
ORESTY_LUAWEBSOCKETVER='0.08'   # openresty https://github.com/openresty/lua-resty-websocket
ORESTY_LUALOCKVER='0.08'        # openresty https://github.com/openresty/lua-resty-lock
ORESTY_LUASTRINGVER='0.12'      # openresty https://github.com/openresty/lua-resty-string
ORESTY_LUAREDISPARSERVER='0.13'    # openresty https://github.com/openresty/lua-redis-parser
ORESTY_LUAUPSTREAMCHECKVER='0.06'  # openresty https://github.com/openresty/lua-resty-upstream-healthcheck
ORESTY_LUALRUCACHEVER='0.10'       # openresty https://github.com/openresty/lua-resty-lrucache
ORESTY_LUARESTYCOREVER='0.1.21'    # openresty https://github.com/openresty/lua-resty-core
ORESTY_LUASTREAMVER='0.0.9'        # https://github.com/openresty/stream-lua-nginx-module
ORESTY_LUASTREAM='y'               # control https://github.com/openresty/stream-lua-nginx-module
NGX_LUASTREAM_FORCED='y'           # control stream-lua-nginx enabling for nginx 1.17+
ORESTY_LUAUPSTREAMVER='0.07'       # openresty https://github.com/openresty/lua-upstream-nginx-module
NGX_LUAUPSTREAM='n'                # disable https://github.com/openresty/lua-upstream-nginx-module
ORESTY_LUALOGGERSOCKETVER='0.1'    # cloudflare openresty https://github.com/cloudflare/lua-resty-logger-socket
ORESTY_LUACOOKIEVER='master'       # cloudflare openresty https://github.com/cloudflare/lua-resty-cookie
ORESTY_LUAUPSTREAMCACHEVER='0.1.1' # cloudflare openresty https://github.com/cloudflare/lua-upstream-cache-nginx-module
NGX_LUAUPSTREAMCACHE='n'           # disable https://github.com/cloudflare/lua-upstream-cache-nginx-module
LUACJSONVER='2.1.0.8rc1'              # https://github.com/openresty/lua-cjson

STRIPPHP='y'                 # set 'y' to strip PHP binary to reduce size
PHP_INSTALL='y'              # Install PHP /w Fast Process Manager
SWITCH_PHPFPM_SYSTEMD='y'    # Switch to centos 7 systemd php-fpm service file https://community.centminmod.com/threads/16511/
ZSTD_LOGROTATE_PHPFPM='n'    # initial install only for zstd compressed log rotation community.centminmod.com/threads/16371/
PHP_PATCH='y'                # Apply PHP patches if they exist
PHP_MYSQLND_PATCH_FIX='y'    # Apply PHP 7.3 backported mysqlnd patch for MariaDB for PHP <=7.2 https://community.centminmod.com/posts/86953/
PHP_TUNING='n'               # initial php-fpm install auto tuning
PHP_HUGEPAGES='n'            # Enable explicit huge pages support for PHP 7 on CentOS 7.x systems
PHP_CUSTOMSSL='n'            # compile php-fpm against openssl 1.0.2+ or libressl 2.3+ whichever nginx uses
PHPMAKETEST=n                # set to y to enable make test after PHP make for diagnostic purposes
AUTODETECPHP_OVERRIDE='n'    # when enabled, php updates will always reinstall all php extensions even if minor php version
PHP_PCREJIT_STACKSIZE_ADJUST='n' # when enabled, allows you to raise PHP's default PCRE JIT stack size https://github.com/php/php-src/pull/2910
PHP_PCREJIT_STACKSIZE='512'  # value to raise PHP PCRE JIT stack size when PHP_PCREJIT_STACKSIZE_ADJUST='y' set on centmin.sh menu option 5 runs

PHPGEOIP_ALWAYS='y'          # GeoIP php extension is always reinstalled on php recompiles
PHPIMAGICK_ALWAYS='y'        # imagick php extension is always reinstalled on php recompiles
PHPDEBUGMODE='n'             # --enable-debug PHP compile flag
PHPIMAP='y'                  # Disable or Enable PHP Imap extension
PHPFINFO='n'                 # Disable or Enable PHP File Info extension
PHPFINFO_STANDALONE='n'      # Disable or Enable PHP File Info extension as standalone module
PHPPCNTL='y'                 # Disable or Enable PHP Process Control extension
PHPINTL='y'                  # Disable or Enable PHP intl extension
PHPRECODE=n                  # Disable or Enable PHP Recode extension
PHPSNMP='y'                  # Disable or Enable PHP SNMP extension
PHPIMAGICK='y'               # Disable or Enable PHP ImagicK extension
PHPMAILPARSE='y'             # Disable or Enable PHP mailparse extension
PHPIONCUBE='n'               # Disable or Enable Ioncube Loader via addons/ioncube.sh
PHPMSSQL='n'                 # Disable or Enable MSSQL server PHP extension
PHPTIMEZONEDB='y'            # timezonedb PHP extension updated https://pecl.php.net/package/timezonedb
PHPTIMEZONEDB_VER='2020.1'   # timezonedb PHP extension version
PHPMSSQL_ALWAYS='n'          # mssql php extension always install on php recompiles
PHPEMBED='y'                 # built php with php embed SAPI library support --enable-embed=shared

PHP_FTPEXT='y'              # ftp PHP extension
PHP_MEMCACHE='y'            # memcache PHP extension 
PHP_MEMCACHED='y'           # memcached PHP extension
FFMPEGVER='0.6.0'
SUHOSINVER='0.9.38'

PHPREDIS='y'                # redis PHP extension install
REDISPHP_VER='4.3.0'        # redis PHP version for PHP <7.x
REDISPHPSEVEN_VER='5.3.2'   # redis PHP version for PHP =>7.x
REDISPHP_GIT='n'            # pull php 7 redis extension from git or pecl downloads
PHPMONGODB='n'              # MongoDB PHP extension install
MONGODBPHP_VER='1.7.4'      # MongoDB PHP version
MONGODB_SASL='n'            # SASL not working yet leave = n
PDOPGSQL_PHPVER='11'        # pdo-pgsql PHP extension version for postgresql
PHP_LIBZIP='n'              # use newer libzip instead of PHP embedded zip
PHP_ARGON='n'               # alias for PHP_LIBZIP, when PHP_ARGON='y' then PHP_LIBZIP='y'
LIBZIP_VER='1.7.3'          # required for PHP 7.2 + with libsodium & argon2
LIBSODIUM_VER='1.0.18'      # https://github.com/jedisct1/libsodium/releases
LIBSODIUM_NATIVE='n'        # optimise for specific cpu not portable between different cpu modules
LIBARGON_VER='20171227'     # https://github.com/P-H-C/phc-winner-argon2
PHP_MCRYPTPECL='y'          # PHP 7.2 deprecated mcrypt support so this adds it back as PECL extension
PHP_MCRYPTPECLVER='1.0.4'   # https://pecl.php.net/package/mcrypt
PHPZOPFLI='n'               # enable zopfli php extension https://github.com/kjdev/php-ext-zopfli
PHPZOPFLI_ALWAYS='n'        # zopfli php extension always install on php recompiles
PHP_BROTLI='n'              # brotli php extension https://github.com/kjdev/php-ext-brotli
PHP_LZFOUR='n'              # lz4 php extension https://github.com/kjdev/php-ext-lz4
PHP_LZF='n'                 # lzf php extension https://github.com/php/pecl-file_formats-lzf php-ext-lzf
PHP_ZSTD='n'                # zstd php extension https://github.com/kjdev/php-ext-zstd

SHORTCUTS='y'                # shortcuts

POSTGRESQL='n'               # set to =y to install PostgreSQL 9.6 server, devel packages and pdo-pgsql PHP extension
POSTGRESQL_BRANCHVER='11'   # PostgresSQL branch version https://www.postgresql.org/ i.e. 9.6, 10 or 11

IMAGEMAGICK_HEIF='n'         # experimental ImageMagick HEIF image format support
########################################################
# Choice of installing MariaDB 5.2 via RPM or via MariaDB 5.2 CentOS YUM Repo
# If MDB_YUMREPOINSTALL=y and MDB_INSTALL=n then MDB_VERONLY version 
# number won't have any effect in determining version of MariaDB 5.2.x to install. 
# YUM Repo will install whatever is latest MariaDB 5.2.x version available via the YUM REPO

# MariaDB MySQL default client and server character set utf8 or utf8mb4 options
# only applies during initial Centmin Mod install and can be overrident via
# persistent config file /etc/centminmod/custom_config.inc prior to initial Centmin Mod install
SET_DEFAULT_MYSQLCHARSET='utf8'
MDB_INSTALL='n'             # Install via RPM MariaDB MySQL Server replacement (Not recommended for VPS with less than 256MB RAM!)
MDB_YUMREPOINSTALL='y'      # Install MariaDB 5.5 via CentOS YUM Repo
MARIADB_INSTALLTENTWO='n'   # MariaDB 10.2 YUM default install if set to yes
MARIADB_INSTALLTENTHREE='y' # MariaDB 10.3 YUM default install if set to yes
MARIADB_INSTALLTENFOUR='n'  # MariaDB 10.4 YUM default install if set to yes

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
# Set MDB_INSTALL=y and MYSQL_INSTALL='n'
MYSQL_INSTALL='n'            # Install official Oracle MySQL Server (MariaDB alternative recommended)
SENDMAIL_INSTALL='n'         # Install Sendmail (and mailx) set to y and POSTFIX_INSTALL=n for sendmail
POSTFIX_INSTALL=y            # Install Postfix (and mailx) set to n and SENDMAIL_INSTALL=y for sendmail
# Nginx
NGINX_VERSION='1.19.6'       # Use this version of Nginx
NGINX_VHOSTSSL='y'            # enable centmin.sh menu 2 prompt to create self signed SSL vhost 2nd vhost conf
NGINXBACKUP='y'
NGINX_STAPLE_CACHE_OVERRIDE='n' # Enable will override Nginx OCSP stapling cache refresh time of 3600 seconds
NGINX_STAPLE_CACHE_TTL='86400'  # Nginx OCSP stapling cache refresh time in seconds override default 3600
NGINXCPU_AUTOTUNE_NEW='y'    # revised nginx worker_proccess auto tuned settings for >12 cpu thread based servers
ZSTD_LOGROTATE_NGINX='n'     # initial install only for zstd compressed log rotation community.centminmod.com/threads/16371/
VHOST_PRESTATICINC='y'       # add pre-staticfiles-local.conf & pre-staticfiles-global.conf include files
NGINXDIR='/usr/local/nginx'
NGINXCONFDIR="${NGINXDIR}/conf"
NGINXBACKUPDIR='/usr/local/nginxbackup'

# control variables post vhost creation
# whether cloudflare.conf include file is uncommented (enabled) or commented out (disabled)
VHOSTCTRL_CLOUDFLAREINC='n'
# whether autoprotect-$vhostname.conf include file is uncommented (enabled) or commented out (disabled)
VHOSTCTRL_AUTOPROTECTINC='y'
##################################
## Nginx SSL options
# OpenSSL
NGINX_PRIORITIZECHACHA='n' # https://community.centminmod.com/posts/67042/
DISABLE_TLSONEZERO_PROTOCOL='n' # disable TLS 1.0 protocol by default industry is moving to deprecate for security
NOSOURCEOPENSSL='y'        # set to 'y' to disable OpenSSL source compile for system default YUM package setup
OPENSSL_VERSION='1.1.1i'   # Use this version of OpenSSL http://openssl.org/
OPENSSL_VERSIONFALLBACK='1.1.1i'   # fallback if OPENSSL_VERSION uses openssl 1.1.x branch
OPENSSL_VERSION_OLDOVERRIDE='1.1.1i' # override version if persist config OPENSSL_VERSION variable is out of date
OPENSSL_THREADS='y'        # control whether openssl 1.1 branch uses threading or not
OPENSSL_TLSONETHREE='y'    # whether OpenSSL 1.1.1 builds enable TLSv1.3
OPENSSL_CUSTOMPATH='/opt/openssl'  # custom directory path for OpenSSL 1.0.2+
CLOUDFLARE_PATCHSSL='n'    # set 'y' to implement Cloudflare's chacha20 patch https://github.com/cloudflare/sslconfig
CLOUDFLARE_ZLIB='y'        # use Cloudflare optimised zlib fork https://blog.cloudflare.com/cloudflare-fights-cancer/
CLOUDFLARE_ZLIB_DYNAMIC='y' # compile nginx CF zlib as a dynamically instead of statically
CLOUDFLARE_ZLIB_OPENSSL='n' # compile dynamically custom OpenSSL against Cloudflare zlib library
CLOUDFLARE_ZLIBRESET='y'   # if CLOUDFLARE_ZLIB='n' set, then revert gzip compression level from 9 to 5 automatically
CLOUDFLARE_ZLIBRAUTOMAX='n' # don't auto raise nginx gzip compression level to 9 if using Cloudflare zlib
CLOUDFLARE_ZLIBPHP='n'     # use Cloudflare optimised zlib fork for PHP-FPM zlib instead of system zlib
CLOUDFLARE_ZLIBDEBUG='n'   # make install debug verbose mode
CLOUDFLARE_ZLIBVER='1.3.0'
NGINX_DYNAMICTLS='n'          # set 'y' and recompile nginx https://blog.cloudflare.com/optimizing-tls-over-tcp-to-reduce-latency/
OPENSSLECDSA_PATCH='n'        # https://community.centminmod.com/posts/57725/
OPENSSLECDHX_PATCH='n'        # https://community.centminmod.com/posts/57726/
OPENSSLEQUALCIPHER_PATCH='n'  # https://community.centminmod.com/posts/57916/
PRIORITIZE_CHACHA_OPENSSL='n' # https://community.centminmod.com/threads/15708/

# LibreSSL
LIBRESSL_SWITCH='n'        # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
LIBRESSL_VERSION='3.0.2'   # Use this version of LibreSSL http://www.libressl.org/

# BoringSSL
# not working yet just prep work
BORINGSSL_SWITCH='n'       # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
BORINGSSL_SHARED='y'       # build boringssl as shared library so nginx can dynamically compile boringssl
BORINGSSL_DIR="/opt"
##################################

# Choose whether to compile Nginx --with-google_perftools_module
# no longer used in Centmin Mod v1.2.3-eva2000.01 and higher
GPERFTOOLS_SOURCEINSTALL='n'
GPERFTOOLS_TMALLOCLARGEPAGES='y'  # set larger page size for tcmalloc --with-tcmalloc-pagesize=32
LIBUNWIND_VERSION='1.2.1'           # note google perftool specifically requies v0.99 and no other
GPERFTOOLS_VERSION='2.6.3'        # Use this version of google-perftools

# Choose whether to compile PCRE from source. Note PHP 5.3.8 already includes PCRE
PCRE_SOURCEINSTALL='n'     
PCRE_VERSION='8.44'          # PCRE version

# PHP and Cache/Acceleration
IMAGICKPHP_VER='3.4.4'   # PHP extension for imagick
MAILPARSEPHP_VER='2.1.6'       # https://pecl.php.net/package/mailparse
MAILPARSEPHP_COMPATVER='3.0.4' # For PHP 7
MEMCACHED_INSTALL='y'          # Install Memcached
LIBEVENT_VERSION='2.1.8'      # Use this version of Libevent
MEMCACHED_VERSION='1.6.5'    # Use this version of Memcached server
MEMCACHED_TLS='n'             # TLS support https://github.com/memcached/memcached/wiki/ReleaseNotes1513
MEMCACHE_VERSION='3.0.8'      # Use this version of Memcache
MEMCACHE_COMPATVER='4.0.5.1'  # For PHP 7
MEMCACHEDPHP_VER='2.2.0'      # Memcached PHP extension not server
MEMCACHEDPHP_SEVENVER='3.1.5' # Memcached PHP 7 only extension version
LIBMEMCACHED_YUM='y'          # switch to YUM install instead of source compile
LIBMEMCACHED_VER='1.0.18'     # libmemcached version for source compile
TWEMPERF_VER='0.1.1'

PHP_OVERWRITECONF='y'       # whether to show the php upgrade prompt to overwrite php-fpm.conf
PHP_VERSION='5.6.40'        # Use this version of PHP
PHP_MIRRORURL='https://www.php.net'
PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
XCACHE_VERSION='3.2.0'      # Use this version of Xcache
APCCACHE_VERSION='3.1.13'   # Use this version of APC Cache
IGBINARY_VERSION='1.2.1'
IGBINARY_INSTALL='y'        # install or not igbinary support for APC and Memcached server
IGBINARYGIT='y'
ZOPCACHEDFT='y'
ZOPCACHECACHE_VERSION='7.0.5'   # for PHP <=5.4 https://pecl.php.net/package/ZendOpcache
ZOPCACHE_OVERRIDE='n'           # =y will override PHP 5.5, 5.6, 7.0 inbuilt Zend OpCache version
# Python
PYTHON_VERSION='2.7.10'       # Use this version of Python
SIEGE_VERSION='4.0.4'

CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
AXEL_VER='2.6'               # Axel source compile version https://github.com/axel-download-accelerator/axel/releases
USEAXEL='y'                  # whether to use axel download accelerator or wget
###############################################################
# experimental Intel compiled optimisations 
# when auto detect Intel based processors
INTELOPT='n'
# GCC optimization level choices: -O2 or -O3 or -Ofast (only for GCC via CLANG=n)
GCC_OPTLEVEL='-O3'
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
# enabled will set -falign-functions=32 for GCC compiles of Nginx and PHP-FPM and pigz
GCC_FALIGN_FUNCTION='n'

# experimental custom RPM compiled packages to replace source 
# compiled versions for 64bit systems only
FPMRPM_LIBEVENT='n'
FPMRPM_MEMCACHED='n'
CENTALTREPO_DISABLE='y'
RPMFORGEREPO_DISABLE='n'
AXIVOREPO_DISABLE='y'
REMIREPO_DISABLE='n'
ATRPMSREPO_DISABLE='y'
VARNISHREPO_DISABLE='y'

# custom curl/libcurl RPM for 7.44 and higher
# enable with CUSTOM_CURLRPM='y'
# use at own risk as it can break the system
# info at http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring/
CUSTOM_CURLRPM='n'
CUSTOM_CURLRPMVER='7.47.1-2.0'       # custom curl/libcurl version
CUSTOM_CURLLIBSSHVER='1.6.0-4.0'     # libssh2 version
CUSTOM_CURLRPMCARESVER='1.10.0-6.0'  # c-ares version
CUSTOM_CURLRPMSYSURL='http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring'
CUSTOM_CURLRPMLIBURL='http://mirror.city-fan.org/ftp/contrib/libraries'

# wget source compile version
WGET_VERSION='1.20.3'
WGET_VERSION_SEVEN='1.20.3'
###############################################################
# cloudflare authenticated origin pull cert
# setup https://community.centminmod.com/threads/13847/
CLOUDFLARE_AUTHORIGINPULLCERT='https://support.cloudflare.com/hc/en-us/article_attachments/360044928032/origin-pull-ca.pem'
VHOST_CFAUTHORIGINPULL='y'
###############################################################
# Settings for centmin.sh menu option 2 and option 22 for
# the details of the self-signed SSL certificate that is auto 
# generated. The default values where vhostname variable is 
# auto added based on what you input for your site name
# 
# -subj "/C=US/ST=California/L=Los Angeles/O=${vhostname}/OU=${vhostname}/CN=${vhostname}"
# 
# You can only customise the first 5 variables for 
# C = Country 2 digit code
# ST = state 
# L = Location as in city 
# 0 = organisation
# OU = organisational unit
# 
# if left blank # defaults to same as vhostname that is your domain
# if set it overrides that
SELFSIGNEDSSL_C='US'
SELFSIGNEDSSL_ST='California'
SELFSIGNEDSSL_L='Los Angeles'
SELFSIGNEDSSL_O=''
SELFSIGNEDSSL_OU=''
###############################################################
# centmin.sh menu option 22 specific options
WPPLUGINS_ALL='n'           # do not install additional plugins
WPCLI_CE_QUERYSTRING_INCLUDED='n' # https://community.centminmod.com/posts/85893/
WPCLI_SUPERCACHEPLUGIN='n'  # https://community.centminmod.com/threads/5102/
# choose Cache Enabler 1.4.9 cache query string inclusion set to 'y'
# choose Cache Enabler 1.5.1 cache query string exclusion set to 'n'
CACHE_ENABLER_LEGACY_CACHE='y'
###############################################################
# php configured --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock
PHP_MYSQLSOCKPATH='/var/lib/mysql'
###############################################################
# Letsencrypt integration via addons/acmetool.sh auto detection
# in centmin.sh menu option 2, 22, and /usr/bin/nv nginx vhost
# generators. You can control whether or not to enable or disable
# integration detection in these menu options
LETSENCRYPT_DETECT='n'
###############################################################
# centmin.sh menu trigger settings
SKIP_INITIAL_PIP_UPDATES='y'
SKIP_PIP_UPDATES='y'
SKIP_FIXPHPFPM_HTTPPROXY='n'
SKIP_FIXWP_UPDATER='n'
SKIP_CSF_MAXMIND_REGO='n'
SKIP_CHECKIPVSIX='n'
SKIP_LIBC_FIX='n'
###############################################################

MACHINE_TYPE=$(uname -m) # Used to detect if OS is 64bit or not.

if [ "${ARCH_OVERRIDE}" ]; then
  ARCH=${ARCH_OVERRIDE}
else
  if [ "${MACHINE_TYPE}" == 'x86_64' ]; then
      ARCH='x86_64'
      MDB_ARCH='amd64'
  else
      ARCH='i386'
  fi
fi

if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  AXEL_VER='2.16.1'
fi

if [[ "$CENTOS_SIX" -eq '6' ]]; then
  # disable axel due to issues with php.net new cdn download system
  # https://community.centminmod.com/posts/72398/
  USEAXEL='n'
fi

# ensure clang alternative to gcc compiler is used only for 64bit OS
# if [[ "$(uname -m)" != 'x86_64' ]]; then
#     CLANG='n'
# fi

if [[ "$1" = 'install' ]]; then
  INITIALINSTALL='y'
  export INITIALINSTALL='y'
fi

# source "inc/mainmenu.inc"
# source "inc/mainmenu_cli.inc"
# source "inc/ramdisk.inc"
source "inc/fastmirrors.conf"
source "inc/qrencode.inc"
source "inc/tcp.inc"
source "inc/customrpms.inc"
source "inc/pureftpd.inc"
source "inc/htpasswdsh.inc"
source "inc/gcc.inc"
source "inc/entropy.inc"
source "inc/cpucount.inc"
source "inc/motd.inc"
source "inc/cpcheck.inc"
source "inc/lowmem.inc"
source "inc/memcheck.inc"
source "inc/ccache.inc"
source "inc/bookmark.inc"
source "inc/centminlogs.inc"
source "inc/yumskip.inc"
source "inc/questions.inc"
source "inc/downloads_centosfive.inc"
source "inc/downloads_centossix.inc"
source "inc/downloads_centosseven.inc"
source "inc/downloadlinks.inc"
source "inc/libzip.inc"
source "inc/downloads.inc"
source "inc/yumpriorities.inc"
source "inc/yuminstall.inc"
source "inc/centoscheck.inc"
source "inc/axelsetup.inc"
source "inc/phpfpmdir.inc"
source "inc/nginx_backup.inc"
source "inc/nsd_submenu.inc"
source "inc/nsd_install.inc"
source "inc/nsdsetup.inc"
source "inc/nsd_reinstall.inc"
source "inc/compress.inc"
source "inc/compress_php.inc"
source "inc/nginx_logformat.inc"
source "inc/logrotate_nginx.inc"
source "inc/logrotate_phpfpm.inc"
source "inc/logrotate_mysql.inc"
source "inc/nginx_mimetype.inc"
source "inc/openssl_install.inc"
source "inc/brotli.inc"
source "inc/nginx_patch.inc"
source "inc/fastopen.inc"
source "inc/mod_security.inc"
source "inc/nginx_configure.inc"
# source "inc/nginx_configure_openresty.inc"
source "inc/geoip.inc"
source "inc/luajit.inc"
source "inc/phpinfo.inc"
source "inc/nginx_install.inc"
source "inc/nginx_upgrade.inc"
source "inc/mailparse.inc"
source "inc/imagick_install.inc"
source "inc/memcached_install.inc"
source "inc/redis_submenu.inc"
source "inc/redis.inc"
source "inc/mongodb.inc"
source "inc/zopfli.inc"
source "inc/php_mssql.inc"
source "inc/mysql_proclimit.inc"
source "inc/mysqltmp.inc"
source "inc/setmycnf.inc"
source "inc/mariadb_install102.inc"
source "inc/mariadb_install103.inc"
source "inc/mariadb_install104.inc"
source "inc/mariadb_install.inc"
source "inc/mysql_install.inc"
source "inc/mariadb_submenu.inc"
source "inc/postgresql.inc"
source "inc/zendopcache_tweaks.inc"
source "inc/php_extraopts.inc"
source "inc/mysql_legacy.inc"
source "inc/imap.inc"
source "inc/fileinfo.inc"
source "inc/php_configure.inc"
source "inc/phpng_download.inc"
source "inc/php_upgrade.inc"
source "inc/php_patch.inc"
source "inc/suhosin_setup.inc"
source "inc/nginx_pagespeed.inc"
source "inc/nginx_modules.inc"
source "inc/nginx_modules_openresty.inc"
source "inc/sshd.inc"
source "inc/openvz_stack.inc"
source "inc/siegeinstall.inc"
source "inc/python_install.inc"
source "inc/nginx_addvhost.inc"
source "inc/wpsetup.inc"
source "inc/wpsetup-fastcgi-cache.inc"
source "inc/mariadb_upgrade.inc"
source "inc/mariadb_upgrade53.inc"
source "inc/mariadb_upgrade55.inc"
source "inc/mariadb_upgrade10.inc"
source "inc/mariadb_upgrade101.inc"
source "inc/mariadb_upgrade102.inc"
source "inc/mariadb_upgrade103.inc"
source "inc/mariadb_upgrade104.inc"
source "inc/nginx_errorpage.inc"
source "inc/sendmail.inc"
source "inc/postfix.inc"
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
source "inc/timezonedb.inc"
source "inc/zendopcache_55ini.inc"
source "inc/zendopcache_install.inc"
source "inc/zendopcache_upgrade.inc"
source "inc/zendopcache_reinstall.inc"
source "inc/zendopcache_submenu.inc"
source "inc/ffmpeginstall.inc"
source "inc/shortcuts_install.inc"
source "inc/memcacheadmin.inc"
source "inc/mysqlsecure.inc"
source "inc/pcre.inc"
source "inc/jemalloc.inc"
source "inc/zlib.inc"
source "inc/letsdebug.inc"
source "inc/google_perftools.inc"
source "inc/updater_submenu.inc"
source "inc/centminfinish.inc"

checkcentosver
mysqltmpdir

# echo $1
if [[ "$1" = 'install' ]]; then
  INITIALINSTALL='y'
  export INITIALINSTALL='y'
  cpcheck initialinstall
else
  cpcheck
fi

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

if [ ! -f /usr/bin/cminfo_updater ]; then
    setupdate
    /usr/bin/cminfo_updater
else
    setupdate
    /usr/bin/cminfo_updater
fi

if [ ! -x /usr/bin/cminfo ]; then
    chmod 0700 /usr/bin/cminfo
fi

if [[ "$GPERFTOOLS_TMALLOCLARGEPAGES" = [yY] ]]; then
    TCMALLOC_PAGESIZE='32'
else
    TCMALLOC_PAGESIZE='8'
fi

if [[ "$INITIALINSTALL" = [yY] && -f /usr/bin/systemd-detect-virt && "$(/usr/bin/systemd-detect-virt)" = 'lxc' ]] || [[ "$INITIALINSTALL" = [yY] && -f "$(which virt-what)" && "$(virt-what | xargs | grep -o lxc)" = 'lxc' ]]; then
  CHECK_LXD='y'
  if [ -d /etc/profile.d ]; then
    echo "export LANG=en_US.UTF-8" >> /etc/profile.d/locale.sh
    echo "export LANGUAGE=en_US.UTF-8" >> /etc/profile.d/locale.sh
    source /etc/profile.d/locale.sh
  fi
fi

# auto enable nginx brotli module if Intel Skylake or newer cpus exist
# newer cpus allow brotli compressed nginx files to be served faster
# https://community.centminmod.com/posts/70527/
# if [[ "$(grep -o 'avx512' /proc/cpuinfo | uniq)" = 'avx512' ]]; then
#   NGXDYNAMIC_BROTLI='y'
#   NGINX_LIBBROTLI='y'
#   NGINX_BROTLIDEP_UPDATE='y'
# fi

###############################################################
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
# MACHINE_TYPE=`uname -m` # Used to detect if OS is 64bit or not.

CUR_DIR=$SCRIPT_DIR # Get current directory.
CM_INSTALLDIR=$CUR_DIR

    # echo "centmin.sh \${CUR_DIR} & \${CM_INSTALLDIR}"
    # echo ${CUR_DIR}
    # echo ${CM_INSTALLDIR}    

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "inc/custom_config.inc"
  fi
    source "inc/custom_config.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "inc/custom_config.inc" > "${CENTMINLOGDIR}/inc-custom-config-settings_${DT}.log"
    fi
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
  fi
    source "${CONFIGSCANBASE}/custom_config.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "${CONFIGSCANBASE}/custom_config.inc" > "${CENTMINLOGDIR}/etc-centminmod-custom-config-settings_${DT}.log"
    fi
fi

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CM_INSTALLDIR}/inc/z_custom.inc"
  fi
    source "${CM_INSTALLDIR}/inc/z_custom.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "${CM_INSTALLDIR}/inc/z_custom.inc" > "${CENTMINLOGDIR}/inc-zcustom-config-settings_${DT}.log"
    fi
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  OPENSSL_VERSION_CUSTOMCONFIG=$(awk -F "'" '/^OPENSSL_VERSION=/ {print $2}' "${CONFIGSCANBASE}/custom_config.inc")
  if [[ "${OPENSSL_VERSION}" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0i' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0h' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0g' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0f' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0e' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0d' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0c' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0b' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0a' ]]; then
    # force old OpenSSL 1.1.0 branch versions to newer
    # 1.1.1 branch if detected as some folks hardcode override
    # OPENSSL_VERSION variable in /etc/centminmod/custom_config.inc
    # and forget to update them and over time they are out of sync
    # with OPENSSL_VERSION updated and set in centmin.sh
    #
    # also OpenSSL 1.1.0j seems to be failing Nginx compiles so this
    # is a workaround to jump to 1.1.1a working version for now
    OPENSSL_VERSION="$OPENSSL_VERSION_OLDOVERRIDE"
    OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
    OPENSSL_LINK="https://www.openssl.org/source/${OPENSSL_LINKFILE}"
  fi
fi

if [[ "$MARCH_TARGETNATIVE" = [yY] ]]; then
  MARCH_TARGET='native'
else
  MARCH_TARGET='x86-64'
fi

if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  WGET_VERSION=$WGET_VERSION_SEVEN
fi
if [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  WGET_VERSION=$WGET_VERSION_SEVEN

  # enable CentOS 8 PowerTools repo for -devel packages
  if [ ! -f /usr/bin/yum-config-manager ]; then
    yum -q -y install dnf-utils
    yum-config-manager --enable PowerTools
  elif [ -f /usr/bin/yum-config-manager ]; then
    yum-config-manager --enable PowerTools
  fi

  # disable native CentOS 8 AppStream repo based nginx, php & oracle mysql packages
  yum -q -y module disable nginx mysql php:7.2

  # install missing dependencies specific to CentOS 8
  # for csf firewall installs
  if [ ! -f /usr/share/perl5/vendor_perl/Math/BigInt.pm ]; then
    yum -q -y install perl-Math-BigInt
  fi
fi

if [[ "$CENTOS_SEVEN" -eq '7' && "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='y'
  DEVTOOLSETEIGHT='n'
  DEVTOOLSETSEVEN='n'
elif [[ "$CENTOS_SEVEN" -eq '7' && "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [nN] ]]; then
  DEVTOOLSETNINE='y'
  DEVTOOLSETEIGHT='n'
  DEVTOOLSETSEVEN='n'
elif [[ "$CENTOS_SEVEN" -eq '7' && "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [nN] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='y'
  DEVTOOLSETEIGHT='n'
  DEVTOOLSETSEVEN='n'
elif [[ "$CENTOS_SEVEN" -eq '7' && "$DEVTOOLSETNINE" = [nN] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='n'
  DEVTOOLSETEIGHT='y'
  DEVTOOLSETSEVEN='n'
elif [[ "$CENTOS_SEVEN" -eq '7' && "$DEVTOOLSETNINE" = [nN] && "$DEVTOOLSETEIGHT" = [nN] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='n'
  DEVTOOLSETEIGHT='n'
  DEVTOOLSETSEVEN='y'
elif [[ "$CENTOS_SIX" -eq '6' && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='n'
  DEVTOOLSETEIGHT='y'
  DEVTOOLSETSEVEN='n'
elif [[ "$CENTOS_SIX" -eq '6' && "$DEVTOOLSETEIGHT" = [nN] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
  DEVTOOLSETNINE='n'
  DEVTOOLSETEIGHT='n'
  DEVTOOLSETSEVEN='y'
fi

if [[ "$CENTOS_SIX" -eq '6' && "$BORINGSSL_SWITCH" = [yY] ]]; then
  # centos 6 gcc 4.4.7 too low for boringssl compiles so need
  # devtoolset-8 gcc 8.3.1+ compiler
  DEVTOOLSETNINE='n'
  DEVTOOLSETEIGHT='y'
  DEVTOOLSETSEVEN='n'
  CRYPTO_DEVTOOLSETGCC='y'
fi

# ensure if ORESTY_LUANGINX is enabled, that the other required
# Openresty modules are enabled if folks forget to enable them
if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    NGINX_OPENRESTY='y'
    LIBRESSL_SWITCH='n'
fi

if [[ "$NGINX_OPENRESTY" = [nN] ]]; then
  # ensure openresty modules are installed as some are 
  # no longer optional and required for Centmin Mod 
  # nginx functionality i.e. wordpress caching configurations
  NGINX_OPENRESTY='y'
fi

if [[ "$NGINX_VIDEO" = [yY] ]]; then
  # variable to control all Nginx video/streaming related nginx
  # modules
  NGINX_SLICE='y'
  NGINX_RTMP='y'
  NGINX_FLV='y'
  NGINX_MP4='y'
fi

if [[ "$(uname -m)" = 'x86_64' ]]; then
  if [[ "$CENTOS_SIX" = '6' || "$CENTOS_SEVEN" = '7' ]] && [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/plugins=1/
:a
exclude=*.i386 *.i586 *.i686
.
:w
:q
EOF
  elif [[ "$CENTOS_EIGHT" = '8' ]] && [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/best=True/
:a
exclude=*.i686
.
:w
:q
EOF
  fi
fi

if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
  if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
    yum -y -q install epel-release
    yum clean all
  fi
  if [[ "$DNF_COPR" = [yY] ]]; then
cat > "/etc/yum.repos.d/dnf-centos.repo" <<EOF
[dnf-centos]
name=Copr repo for dnf-centos owned by @rpm-software-management
baseurl=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/pubkey.gpg
enabled=1
enabled_metadata=1
EOF
  fi
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
    dnf clean all
  fi
  if [ ! "$(grep -w 'exclude' /etc/dnf/dnf.conf)" ]; then
    echo "excludepkgs=*.i386 *.i586 *.i686" >> /etc/dnf/dnf.conf
  fi
  if [ ! "$(grep -w 'fastestmirror=true' /etc/dnf/dnf.conf)" ]; then
    echo "fastestmirror=true" >> /etc/dnf/dnf.conf
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEREPO_DNF=' --disablerepo=rpmforge'
  else
    DISABLEREPO_DNF=""
  fi
fi

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
else
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
fi

# function checks if persistent config file has low mem variable enabled
# LOWMEM_INSTALL='y'
checkfor_lowmem
###############################################################
# FUNCTIONS

if [[ "$CENTOS_SEVEN" = 7 && "$USEAXEL" = [yY] ]]; then
    DOWNLOADAPP='axel -4'
    WGETRETRY=''
elif [[ "$CENTOS_SIX" = 6 && "$USEAXEL" = [yY] ]]; then
    DOWNLOADAPP='axel'
    WGETRETRY=''
else
    DOWNLOADAPP="wget ${WGETOPT} --progress=bar"
    WGETRETRY='--tries=3'
fi

sar_call() {
  $SARCALL 1 1
}

download_cmd() {
  HTTPS_AXELCHECK=$(echo "$1" |awk -F '://' '{print $1}')
  if [[ "$(curl -${ipv_forceopt}Isv $1 2>&1 | egrep 'ECDSA')" ]]; then
    # axel doesn't natively support ECC 256bit ssl certs
    # with ECDSA ciphers due to CentOS system OpenSSL 1.0.2e
    echo "ECDSA SSL Cipher BASED HTTPS detected, switching from axel to wget"
    DOWNLOADAPP="wget ${WGETOPT}"
    WGETRETRY='--tries=3'
  elif [[ "$CENTOS_SIX" = '6' && "$HTTPS_AXELCHECK" = 'https' ]]; then
    echo "CentOS 6 Axel fallback to wget for HTTPS download"
    DOWNLOADAPP="wget ${WGETOPT}"
    WGETRETRY='--tries=3'
  fi
  $DOWNLOADAPP $1 $2 $3 $4
}

# if [ "${ARCH_OVERRIDE}" != '' ]
# then
#     ARCH=${ARCH_OVERRIDE}
# else
#     if [ ${MACHINE_TYPE} == 'x86_64' ];
#     then
#         ARCH='x86_64'
#         MDB_ARCH='amd64'
#     else
#         ARCH='i386'
#     fi
# fi

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

###
unsetramdisk() {
    if [[ "$RAMDISKTMPFS" = [yY] && "$TMPFSENABLED" = '1' ]]; then
        echo
        cecho "unmount $DIR_TMP tmpfs ramdisk & copy back to disk" $boldyellow
        mkdir -p "${DIR_TMP}_disk"
        \cp -R ${DIR_TMP}/* "${DIR_TMP}_disk"
        # ls -lah "${DIR_TMP}_disk"
        # diff -qr ${DIR_TMP} ${DIR_TMP}_disk
        cmservice nginx stop
        cmservice php-fpm stop
        cmservice memcached stop
        sleep 4
        # lsof | grep /svr-setup
        umount -l "${DIR_TMP}"
        cmservice nginx start
        cmservice php-fpm start
        cmservice memcached start
        \cp -R ${DIR_TMP}_disk/* "${DIR_TMP}"
        # ls -lahrt "${DIR_TMP}"
        rm -rf "${DIR_TMP}_disk"
        df -hT
        cecho "unmounted $DIR_TMP tmpfs ramdisk" $boldyellow
    fi
}
###

funct_centmininstall() {
    INITIALINSTALL='y'
    export INITIALINSTALL='y'

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
#    ${YUMDNFBIN}${CACHESKIP} -q clean all
#    ${YUMDNFBIN}${CACHESKIP} -y update glibc\*
#    ${YUMDNFBIN}${CACHESKIP} -y update yum\* rpm\* python\*
#    ${YUMDNFBIN}${CACHESKIP} -q clean all
#    ${YUMDNFBIN}${CACHESKIP} -y update
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
        \cp -f /etc/yum.conf /etc/yum.bak

        echo "removing any i686 packages installed by default"
        yum -y remove \*.i686

if [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/plugins=1/
:a
exclude=*.i386 *.i586 *.i686
.
:w
:q
EOF
fi
        echo "Your origional yum configuration has been backed up to /etc/yum.bak"
    else
        rm -rf "$CUR_DIR/config/yum"
    fi
fi

  if [ "$UNATTENDED" == 'n' ]; then
ASK "Would you like to secure /tmp and /var/tmp? (Highly recommended) [y/n] "   
  else
  key='y'
  fi #unattended
{
if [[ "$key" = [yY] ]]; then
echo "Centmin Mod secure /tmp # `date`"
  echo "*************************************************"
  cecho "* Secured /tmp and /var/tmp" $boldgreen
  echo "*************************************************"

# account for /home free disk space as well
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    HOME_DFSIZE=$(df --output=avail /home | tail -1)
else
    # df output double check as the partition label name might be long
    # enough to push the output to a 2nd line of text which would alter
    # the column number where free disk space is reported from 4th to 3rd
    # column so need to check for this
    CHECK_DFSIZEFORMAT=$(df /home | tail -1 | awk '{print $4}' | grep '\%' >/dev/null 2>&1; echo $?)
    if [[ "$CHECK_DFSIZEFORMAT" = '0' ]]; then
        HOME_DFSIZE=$(df /home | tail -1 | awk '{print $3}')
    else
        HOME_DFSIZE=$(df /home | tail -1 | awk '{print $4}')
    fi
fi

# centos 7 + openvz /tmp workaround
if [[ -f /proc/user_beancounters && "$CENTOS_SEVEN" = '7' ]]; then
    echo "CentOS 7 Setup /tmp"
    echo "CentOS 7 + OpenVZ virtualisation detected"
    systemctl is-enabled tmp.mount

## leave CentOS 7 + OpenVZ systems /tmp mounted on disk in ##
## partition / ##
# systemctl enable tmp.mount
# systemctl is-enabled tmp.mount

## leave CentOS 7 + OpenVZ system's /tmp mounted in memory via
## tmpfs as CentOS 7 requires more memory installed, so most of
## the time, you would have sufficient memory at least for /tmp
## on ramdisk tmpfs unlike when CentOS 6 32bit systems can use 
## 256MB to 512MB memory and end up with too small a ramdisk tmpfs
## /tmp mount
# 
# rm -rf /tmp
# mkdir -p /tmp
# mount -t tmpfs -o rw,noexec,nosuid tmpfs /tmp
# chmod 1777 /tmp
# echo "tmpfs /tmp tmpfs rw,noexec,nosuid 0 0" >> /etc/fstab
# rm -rf /var/tmp
# ln -s /tmp /var/tmp
# mount -o remount /tmp
elif [[ ! -f /proc/user_beancounters && "$CENTOS_SEVEN" = '7' && "$CHECK_LXD" != [yY] ]]; then
    echo "CentOS 7 Setup /tmp"
    echo "CentOS 7 + non-OpenVZ virtualisation detected"
    systemctl is-enabled tmp.mount

    # only mount /tmp on tmpfs if CentOS system
    # total memory size is greater than ~15.25GB
    # will give /tmp a size equal to 1/2 total memory
    if [[ "$TOTALMEM" -ge '16000001' ]]; then
       cp -ar /tmp /tmp_backup
       #rm -rf /tmp
       #mkdir -p /tmp
       mount -t tmpfs -o rw,noexec,nosuid tmpfs /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "tmpfs /tmp tmpfs rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -ge '8100001' || "$TOTALMEM" -lt '16000000' ]]; then
       # set on disk non-tmpfs /tmp to 6GB size
       # if total memory is between 2GB and <8GB
       cp -ar /tmp /tmp_backup
       # rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=6291456
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       # mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -ge '2050061' || "$TOTALMEM" -lt '8100000' ]]; then
       # set on disk non-tmpfs /tmp to 4GB size
       # if total memory is between 2GB and <8GB
       cp -ar /tmp /tmp_backup
       # rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=4194304
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       # mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -ge '1153434' || "$TOTALMEM" -lt '2050060' ]]; then
       # set on disk non-tmpfs /tmp to 2GB size
       # if total memory is between 1.1-2GB
       cp -ar /tmp /tmp_backup
       # rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=3000000
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       # mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -le '1153433' ]]; then
       # set on disk non-tmpfs /tmp to 1GB size
       # if total memory is <1.1GB
       cp -ar /tmp /tmp_backup
       # rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=3000000
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       # mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp       
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    fi
elif [[ ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] ]]; then

    # TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
    CURRENT_TMPSIZE=$(df -P /tmp | awk '/tmp/ {print $3}')

    # only mount /tmp on tmpfs if CentOS system
    # total memory size is greater than ~7.72GB
    # will give /tmp a size equal to 1/2 total memory
    if [[ "$TOTALMEM" -ge '8100001' ]]; then
     cp -ar /tmp /tmp_backup
       rm -rf /tmp
     mkdir -p /tmp
     mount -t tmpfs -o rw,noexec,nosuid tmpfs /tmp
     chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
     echo "tmpfs /tmp tmpfs rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
     ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -ge '2050061' || "$TOTALMEM" -lt '8100000' ]]; then
       # set on disk non-tmpfs /tmp to 4GB size
       # if total memory is between 2GB and <8GB
       cp -ar /tmp /tmp_backup
       rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=4194304
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -ge '1153434' || "$TOTALMEM" -lt '2050060' ]]; then
       # set on disk non-tmpfs /tmp to 2GB size
       # if total memory is between 1.1-2GB
       cp -ar /tmp /tmp_backup
       rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=3000000
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    elif [[ "$TOTALMEM" -le '1153433' ]]; then
       # set on disk non-tmpfs /tmp to 1GB size
       # if total memory is <1.1GB
       cp -ar /tmp /tmp_backup
       rm -rf /tmp
       if [[ "$HOME_DFSIZE" -le '15750000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1048576
       elif [[ "$HOME_DFSIZE" -gt '15750001' && "$HOME_DFSIZE" -le '20999000' ]]; then
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2097152
       else
        dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=3000000
       fi
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       cp -ar /tmp_backup/* /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       cp -ar /var/tmp /var/tmp_backup
       ln -s /tmp /var/tmp       
       cp -ar /var/tmp_backup/* /tmp
       rm -rf /tmp_backup
       rm -rf /var/tmp_backup
    fi
fi # centos 7 + openvz /tmp workaround
fi
} 2>&1 | tee "${CENTMINLOGDIR}/securedtmp.log"

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
    if [[ "$CENTOS_SIX" = '6' ]]; then
        rm -f /etc/localtime
        ln -s "/usr/share/zoneinfo/$ZONEINFO" /etc/localtime
    elif [[ "$CENTOS_SEVEN" = '7' ]]; then
        timedatectl set-timezone "$ZONEINFO"
    fi
    echo "Current date & time for the zone you selected is:"
    date
fi
}

# END FUNCTIONS
################################################################
# SCRIPT START
#
# clear
cecho "**********************************************************************" $boldyellow
cecho "* Centmin Mod Nginx, MariaDB MySQL, PHP & DNS Install script for CentOS" $boldgreen
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
# ASK "It seems that you have run this script before, would you like to start from after setting the timezone? [y/n] "
# if [[ "$key" = [nN] ]];
# then
#   run_once
# fi
#else
# mkdir $DIR_TMP
# run_once
#fi

if [ ! -f "${DIR_TMP}/securedtmp.log" ]; then
run_once
fi

if [ -f /proc/user_beancounters ]; then
    cecho "OpenVZ system detected, NTP not installed" $boldgreen
elif [[ "$CHECK_LXD" = [yY] ]]; then
    cecho "LXC/LXD container system detected, NTP not installed" $boldgreen
else
    if [[ "$NTP_INSTALL" = [yY] ]]; 
    then
        echo "*************************************************"
        cecho "* Installing NTP (and syncing time)" $boldgreen
        echo "*************************************************"
        if [ ! -f /usr/sbin/ntpd ]; then
            ${YUMDNFBIN}${CACHESKIP} -y install ntp
            chkconfig --levels 235 ntpd on
        fi
        # skip re-running this routine if custom logfile already set i.e.
        # in curl installer installs it's already configured so doesn't
        # need to be re-run again
        if [[ -z "$(grep 'logfile' /etc/ntp.conf)" && -f /etc/ntp.conf ]]; then
        if [[ -z "$(grep 'logfile' /etc/ntp.conf)" ]]; then
            echo "logfile /var/log/ntpd.log" >> /etc/ntp.conf
            ls -lahrt /var/log | grep 'ntpd.log'
        fi
        echo "current ntp servers"
        NTPSERVERS=$(awk '/server / {print $2}' /etc/ntp.conf | grep ntp.org | sort -r)
        for s in $NTPSERVERS; do
            echo -ne "\n$s test connectivity: "
            if [[ "$(echo | nc -u -w1 $s 53 >/dev/null 2>&1 ;echo $?)" = '0' ]]; then
            echo " ok"
            else
            echo " error"
            fi
            ntpdate -q $s | tail -1
            if [[ -f /etc/ntp/step-tickers && -z "$(grep $s /etc/ntp/step-tickers )" ]]; then
            echo "$s" >> /etc/ntp/step-tickers
            fi
        done
        if [ -f /etc/ntp/step-tickers ]; then
            echo -e "\nsetup /etc/ntp/step-tickers server list\n"
            cat /etc/ntp/step-tickers
        fi
        service ntpd restart >/dev/null 2>&1
        echo -e "\ncheck ntpd peers list"
        ntpdc -p
        fi
        echo "The date/time is now:"
        date
        echo "If this is correct, then everything is working properly"
        echo "*************************************************"
        cecho "* NTP installed" $boldgreen
        echo "*************************************************"
    fi
fi

ngxinstallstarttime=$(TZ=UTC date +%s.%N)
{    
ngxinstallmain
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
wait

ngxinstallendtime=$(TZ=UTC date +%s.%N)
NGXINSTALLTIME=$(echo "scale=2;$ngxinstallendtime - $ngxinstallstarttime"|bc )

echo "" >> "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
echo "Total Nginx First Time Install Time: $NGXINSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
ls -lah "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"

if [[ "$MARIADB_INSTALLTENFOUR" = [yY] && "$MARIADB_INSTALLTENTWO" = [nN] ]]; then
  mariadbtenfour_installfunct
elif [[ "$MARIADB_INSTALLTENTHREE" = [yY] && "$MARIADB_INSTALLTENTWO" = [nN] ]]; then
  mariadbtenthree_installfunct
elif [[ "$MARIADB_INSTALLTENTWO" = [yY] ]]; then
  mariadbtentwo_installfunct
else
  mariadbinstallfunct
fi

mysqlinstallfunct

securemysql

if [[ "$PHP_INSTALL" = [yY] ]]; then
    phpinstallstarttime=$(TZ=UTC date +%s.%N)
    echo "*************************************************"
    cecho "* Installing PHP" $boldgreen
    echo "*************************************************"

funct_centos6check

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

if [ "$(rpm -qa | grep '^php*' | grep -v 'phonon-backend-gstreamer')" ]; then
  # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
  echo "${YUMDNFBIN} -y erase php*"
  ${YUMDNFBIN}${CACHESKIP} -y erase php*

fi

    cd "${DIR_TMP}/php-${PHP_VERSION}"
    PHPVER_ID=$(awk '/PHP_VERSION_ID/ {print $3}' ${DIR_TMP}/php-${PHP_VERSION}/main/php_version.h)
    echo "PHP VERSION ID: $PHPVER_ID"

    # if ZOPCACHEDFT override enabled = yY and PHP_VERSION is not 5.5, 5.6 or 5.7
    # install Zend OpCache PECL extesnion otherwise if PHP_VERSION = 5.5
    # then php_configure.inc routine will pick up PHP_VERSION 5.5 and install
    # native Zend OpCache when ZOPCACHEDFT=yY
    PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)
    echo "Initial Install PHPMVER: $PHPMVER"

    php_patches

    if [[ "$CENTOS_SIX" -eq '6' ]]; then
        # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
        if [ ! -f /usr/bin/autoconf268 ]; then
            echo "yum -q -y install autoconf268"
            yum -q -y install autoconf268
        fi
        if [ -f /usr/bin/autoconf268 ]; then
            export PHP_AUTOCONF=/usr/bin/autoconf268
            export PHP_AUTOHEADER=/usr/bin/autoheader268
        fi
    fi

    ./buildconf --force
    mkdir fpm-build && cd fpm-build

  if [[ ! -f "/usr/${LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${LIBDIR}/libmysqlclient.so.20" ]]; then
    mkdir -p "/usr/${LIBDIR}/mysql"
    rm -rf "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${LIBDIR}/libmysqlclient.so.20" "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ls -lah "/usr/${LIBDIR}/mysql/libmysqlclient.so"
  elif [[ ! -f "/usr/${LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${LIBDIR}/libmysqlclient.so.18" ]]; then
    mkdir -p "/usr/${LIBDIR}/mysql"
    rm -rf "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${LIBDIR}/libmysqlclient.so.18" "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ls -lah "/usr/${LIBDIR}/mysql/libmysqlclient.so"
  elif [[ ! -f "/usr/${LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${LIBDIR}/libmysqlclient.so.16" ]]; then
    mkdir -p "/usr/${LIBDIR}/mysql"
    rm -rf "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${LIBDIR}/libmysqlclient.so.16" "/usr/${LIBDIR}/mysql/libmysqlclient.so"
    ls -lah "/usr/${LIBDIR}/mysql/libmysqlclient.so"
  fi

funct_phpconfigure

    cd ../

#######################################################
# check to see if centmin custom php.ini already in place

CUSTOMPHPINICHECK=$(grep 'realpath_cache_size = 1024k' /usr/local/lib/php.ini 2>/dev/null)

if [[ -z "$CUSTOMPHPINICHECK" ]]; then

    cp -f php.ini-production /usr/local/lib/php.ini
    chmod 644 /usr/local/lib/php.ini

# phpsededit

fi # check to see if centmin custom php.ini already in place


echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem

#echo
#echo

if [[ "$lessphpmem" = [yY] ]]; then
    echo "$lessphpmem"
    echo -e "\nCopying php-fpm-min.conf /usr/local/etc/php-fpm.conf\n"
    cp "$CUR_DIR/config/php-fpm/php-fpm-min.conf" /usr/local/etc/php-fpm.conf
    phptuning
else
    echo "$lessphpmem"
    echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
    cp "$CUR_DIR/config/php-fpm/php-fpm.conf" /usr/local/etc/php-fpm.conf
    phptuning
fi


    cp "$CUR_DIR/init/php-fpm" /etc/init.d/php-fpm

# add check for Windows CLRF line endings
if [ ! -f /usr/bin/file ]; then
    time $YUMDNFBIN -q -y install file${DISABLEREPO_DNF}
fi
if [[ "$(file /etc/init.d/php-fpm)" =~ CRLF && -f /etc/init.d/php-fpm ]]; then
    if [ ! -f /usr/bin/dos2unix ]; then
        time $YUMDNFBIN -q -y install dos2unix${DISABLEREPO_DNF}
    fi
    echo "detected CRLF line endings converting to Unix LF"
    dos2unix /etc/init.d/php-fpm
fi

    chmod +x /etc/init.d/php-fpm

    mkdir -p /var/run/php-fpm
    chmod 755 /var/run/php-fpm
    touch /var/run/php-fpm/php-fpm.pid
    chown nginx:nginx /var/run/php-fpm
    chown root:root /var/run/php-fpm/php-fpm.pid

    mkdir /var/log/php-fpm/
    touch /var/log/php-fpm/www-error.log
    touch /var/log/php-fpm/www-php.error.log
    touch /var/log/php-fpm/www-slow.log
    chmod 0666 /var/log/php-fpm/www-error.log
    chmod 0666 /var/log/php-fpm/www-php.error.log
    chmod 0666 /var/log/php-fpm/www-slow.log
    fpmconfdir

    #chown -R root:nginx /var/lib/php/session/
    chkconfig --levels 235 php-fpm on
    #service php-fpm restart 2>/dev/null
    # /etc/init.d/php-fpm force-quit
    service php-fpm start
    fileinfo_standalone

    if [[ "$CENTOS_SEVEN" -eq '7' && "$SWITCH_PHPFPM_SYSTEMD" = [yY] && -f "$CUR_DIR/tools/php-systemd.sh" ]]; then
      $CUR_DIR/tools/php-systemd.sh fpm-systemd
    fi

if [[ "$(grep exclude /etc/yum.conf)" && "$MDB_INSTALL" = y ]]; then
    cecho "exclude line exists... adding nginx* mysql* php* exclusions" $boldgreen
    sed -i "s/exclude=\*.i386 \*.i586 \*.i686 mysql\*/exclude=\*.i386 \*.i586 \*.i686 nginx\* mysql\* php\*/" /etc/yum.conf
    sed -i "s/exclude=mysql\*/exclude=mysql\* php\*/" /etc/yum.conf
elif [[ "$(grep exclude /etc/yum.conf)" ]]; then
    cecho "exclude line exists... adding nginx* php* exclusions" $boldgreen
    sed -i "s/exclude=\*.i386 \*.i586 \*.i686/exclude=\*.i386 \*.i586 \*.i686 nginx\* php\*/" /etc/yum.conf 
fi


if [[ ! "$(grep exclude /etc/yum.conf)" && "$MDB_INSTALL" = y ]]; then

cecho "Can't find exclude line in /etc/yum.conf.. adding exclude line for nginx* mysql* php*" $boldgreen

echo "exclude=nginx* mysql* php*">> /etc/yum.conf

elif [[ ! "$(grep exclude /etc/yum.conf)" ]]; then

cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for nginx* php*" $boldgreen

echo "exclude=nginx* php*">> /etc/yum.conf

fi

funct_logphprotate

    echo "*************************************************"
    cecho "* PHP installed" $boldgreen
    echo "*************************************************"
    phpinstallendtime=$(TZ=UTC date +%s.%N)
    PHPINSTALLTIME=$(echo "scale=2;$phpinstallendtime - $phpinstallstarttime"|bc )

    echo "" >> "${CENTMINLOGDIR}/centminmod_phpinstalltime_${DT}.log"
    echo "Total PHP First Time Install Time: $PHPINSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_phpinstalltime_${DT}.log"
    ls -lah "${CENTMINLOGDIR}/centminmod_phpinstalltime_${DT}.log" 
fi

xcacheinstall_ask

#if [ "$xcacheinstallcheck" != 'y' ]; then
    #cecho "* If you installed Xcache, DO NOT install APC" $boldgreen
#fi # xcacheinstallcheck

#ASK "Install APC? (By default uses 32MB RAM) [y/n] "
# if ZOPCACHEDFT override enabled = yY then skip APC Cache install
if [[ "$APCINSTALL" = [yY] && "$ZOPCACHEDFT" = [nN] ]]; then
  funct_apcsourceinstall
fi

# ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.0's inbuilt included
# Zend Opcache version with one available from pecl site
if [[ "$ZOPCACHEDFT" = [yY] ]] && [[ "$PHPMVER" = 5.[234] || "$ZOPCACHE_OVERRIDE" = [yY] ]]; then
  zopcacheinstall
fi

# if PHP_VERSION = 5.5 or newer will need to setup a zendopcache.ini settings file
if [[ "$PHPMVER" > 5.4 && "$ZOPCACHE_OVERRIDE" != [yY] ]] && [[ "$APCINSTALL" = [nN] || "$ZOPCACHEDFT" = [yY] ]]; then
  zopcache_initialini
fi

phpsededit

# igbinary still needed for libmemcached PHP extension if ZOPCACHE=yY
# or for redis php extension
if [[ "$APCINSTALL" = [nN] || "$ZOPCACHEDFT" = [yY] ]]; then
  funct_igbinaryinstall
fi

postfix_presetup

echo "incmemcachedinstall"
incmemcachedinstall

echo "csfinstalls"
csfinstalls

echo "csfcron_setup"
csfcron_setup

echo "siegeinstall"
siegeinstall

echo "installpythonfuct"
installpythonfuct

echo "mailparseinstall"
mailparseinstall

echo "imagickinstall"
imagickinstall

echo "geoipphpext"
geoipphpext

if [[ "$PHPREDIS" = [yY] ]]; then
    echo "redisinstall"
    redisinstall
fi

echo "mongodbinstall"
mongodbinstall

echo "zopfliinstall"
zopfliinstall

if [[ "$PHPMSSQL" = [yY] ]]; then
  echo "php_mssqlinstall"
  php_mssqlinstall
fi

if [[ "$PHP_BROTLI" = [yY] ]]; then
  echo "php_ext_brotli"
  php_ext_brotli
fi

if [[ "$PHP_LZFOUR" = [yY] ]]; then
  echo "php_ext_lzfour"
  php_ext_lzfour
fi

if [[ "$PHP_LZF" = [yY] ]]; then
  echo "php_ext_lzf"
  php_ext_lzf
fi

if [[ "$PHP_ZSTD" = [yY] ]]; then
  echo "php_ext_zstd"
  php_ext_zstd
fi

if [[ "$PHPTIMEZONEDB" = [yY] ]]; then
  echo "phptimezonedb_install"
  phptimezonedb_install
fi

if [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMVER" = '7.4' ]]; then
  if [ -f /usr/local/src/centminmod/addons/php74-mcrypt.sh ]; then
    /usr/local/src/centminmod/addons/php74-mcrypt.sh menu
  fi
elif [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMVER" = '7.3' ]]; then
  if [ -f /usr/local/src/centminmod/addons/php73-mcrypt.sh ]; then
    /usr/local/src/centminmod/addons/php73-mcrypt.sh menu
  fi
elif [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMVER" = '7.2' ]]; then
  if [ -f /usr/local/src/centminmod/addons/php72-mcrypt.sh ]; then
    /usr/local/src/centminmod/addons/php72-mcrypt.sh menu
  fi
fi

if [[ "$NSD_INSTALL" = [yY] ]]; then
    echo "nsdinstall"
    nsdinstall
fi

php -v 2>&1 | grep -v 'PHP Warning' | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3|7.4|8.0'
PHPSEVEN_CHECKVER=$?
echo "$PHPSEVEN_CHECKVER"
if [[ "$PHPSEVEN_CHECKVER" = '0' ]]; then
  if [[ "$PHPMVER" = '7.3' && -f "${CONFIGSCANDIR}/memcache.ini" ]]; then
      # cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcache.ini" $boldyellow
      # cecho "rm -rf ${CONFIGSCANDIR}/memcache.ini" $boldyellow
      # service php-fpm restart >/dev/null 2>&1
      echo
  fi
fi

echo "pureftpinstall"
pureftpinstall

if [ -f "$CUR_DIR/Extras/nginx-update.sh" ];
then
    chmod +x "$CUR_DIR/Extras/nginx-update.sh"
fi

echo "source_pcreinstall"
source_pcreinstall

echo
shortcutsinstall

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' ]] && [[ -f "$CUR_DIR/tools/journald-set.sh config" ]]; then
  echo
  "$CUR_DIR/tools/journald-set.sh" config
fi

echo
cecho "**********************************************************************" $boldgreen
cecho "* Starting Services..." $boldgreen
cecho "**********************************************************************" $boldgreen
if [[ "$NSD_INSTALL" = [yY] && -f /etc/init.d/nsd ]]; then
  /etc/init.d/nsd start
fi

if [ -f /etc/init.d/ntpd ]; then
  /etc/init.d/ntpd start
fi

if [[ "$NGINX_INSTALL" = [yY] && -f /etc/init.d/nginx ]]; then
  sleep 2
  service nginx start
fi

if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" = '8' ]] && [[ "$MDB_INSTALL" = [yY] || "$MDB_YUMREPOINSTALL" = [yY] ]]; then
  sleep 2
  systemctl daemon-reload -q
  service php-fpm stop >/dev/null 2>&1
  systemctl restart mariadb -q
  service php-fpm start >/dev/null 2>&1
  if [[ "$(systemctl is-active mariadb -q; echo $?)" -ne '0' ]]; then
    sleep 4
    systemctl daemon-reload -q
    systemctl restart mariadb -q
    if [[ "$(systemctl is-active mariadb -q; echo $?)" -eq '0' ]]; then
      echo "Starting mariadb (via systemctl): [ OK ]"
    else
      echo "Starting mariadb (via systemctl): [ Failed ]"
    fi
  fi
elif [[ "$MDB_INSTALL" = [yY] || "$MDB_YUMREPOINSTALL" = [yY] ]] && [ -f /etc/init.d/mysql ]; then
  sleep 3
  /etc/init.d/mysql restart
fi

if [[ "$MYSQL_INSTALL" = [yY] && -f /etc/init.d/mysqld ]]; then
  /etc/init.d/mysqld start
fi

if [[ "$(service postfix status >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
  sleep 2
  service postfix restart
fi

# if [[ "$PUREFTPD_DISABLED" != [yY] && "$(service pure-ftpd status >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
#   sleep 2
#   service pure-ftpd restart
# fi

if [[ "$(service csf status >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
  sleep 2
  service csf start
fi

if [[ "$(service lfd status >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
  sleep 2
  service lfd start
fi

echo

cd

funct_openvz_stacksize
checkxcacheadmin


#     echo "*************************************************"
#     cecho "* Running updatedb command. Please wait...." $boldgreen
#     echo "*************************************************"

# time updatedb

centminfinish
memcacheadmin
phpiadmin

    if [ -f "${CENTMINLOGDIR}/zendopcache_passfile.txt" ]; then
      echo "*************************************************"
      cecho "* Zend Opcache Stats Password / URL" $boldgreen
      echo "*************************************************"
      cat "${CENTMINLOGDIR}/zendopcache_passfile.txt"
    fi

    echo "*************************************************"
    cecho "* MariaDB Security Setup" $boldgreen
    echo "*************************************************"

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] || "$UNATTENDED" == [yY] ]]; then
  # securemysql
  show_mysqlpass
else
  # securemysql
  show_mysqlpass
fi

    echo "*************************************************"
    cecho "* MariaDB Security Setup Completed" $boldgreen
    echo "*************************************************"

disk_cleanups
bookmark

sync 

if [[ ! -f /proc/user_beancounters && "$(virt-what | grep -o lxc)" != 'lxc' ]]; then
  echo 3 > /proc/sys/vm/drop_caches
fi

}


#####################################################################
#####################################################################
# functions

funct_centos6check() {


if [[ "$CENTOSVER" > 5.5 ]]; then

MCRYPT=" --with-mcrypt"

else

MCRYPT=""

fi

}


# funct_phpupgrade was here
# funct_memcachedreinstall was here

funct_timestamp() {


echo "..."
echo "Time:"   
echo "********************************"
echo "================================"
date
echo "================================"
echo "********************************"

}

funct_installiopingcentmin() {
    if [ ! -f /usr/bin/ioping ]; then
        echo ""
        cecho "--------------------------------------------------------" $boldyellow
        echo "ioping installing..."
        cecho "--------------------------------------------------------" $boldyellow
        time $YUMDNFBIN -q -y install ioping${DISABLEREPO_DNF}
        echo ""
        cecho "--------------------------------------------------------" $boldyellow
        echo "ioping installed"
        /usr/bin/ioping -v
        cecho "--------------------------------------------------------" $boldyellow
        echo ""
    else
        echo "ioping already installed"
        echo
        exit
    fi
}

funct_selinux() {
    if [ -f /etc/selinux/config ]; then
        SELINUXCONFIGFILE='/etc/selinux/config'
        SELINUXCHECK=$(grep '^SELINUX=' /etc/selinux/config | cut -d '=' -f2)
        
        if [[ "$SELINUXCHECK" == 'enforcing' ]]; then
         echo ""
            cecho "---------------------------------------------" $boldyellow
         echo "Checking SELinux status...."
         echo "SELinux enabled"
         echo ""
         read -ep "Do you want to disable SELinux ? [y/n]: " disableselinux
            cecho "---------------------------------------------" $boldyellow
         echo ""
        
         if [[ "$disableselinux" == [yY] ]]; then
        
         echo ""
            cecho "---------------------------------------------" $boldyellow
         echo "Disabling SELinux..."
        
         sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' $SELINUXCONFIGFILE
            sed -i 's/SELINUX=permissive/SELINUX=disabled/g' $SELINUXCONFIGFILE
         setenforce 0
        
         echo ""
            cecho "---------------------------------------------" $boldyellow
         echo "checking $SELINUXCONFIGFILE"
        
         cat "$SELINUXCONFIGFILE" | grep '^SELINUX='
        
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
    else
        echo ""
         cecho "---------------------------------------------" $boldyellow
        echo "SELinux already disabled"
         cecho "---------------------------------------------" $boldyellow
        echo ""
        exit      
    fi
}

funct_showtempfile() {

    echo "*************************************************"
  cat "$TMP_MSGFILE"
    echo "*************************************************"

}

funct_mktempfile() {

if [[ ! -d "$DIR_TMP"/msglogs ]]; then
cd "$DIR_TMP"
mkdir msglogs
chmod 1777 "$DIR_TMP/msglogs"
fi

TMP_MSGFILE="$DIR_TMP/msglogs/$RANDOM.msg"

}

function cleanup_msg {
  rm -f "$TMP_MSGFILE"
  exit 1
}

# http://linuxcommand.org/wss0160.php
trap cleanup_msg SIGHUP SIGINT SIGTERM SIGTSTP

# end functions
#####################################################################
#####################################################################

# main menu
# inc/mainmenu.inc
# inc/mainmenu_cli.inc
#########################################################

if [[ "$1" = 'install' ]]; then
    starttime=$(TZ=UTC date +%s.%N)
    INITIALINSTALL='y'
    export INITIALINSTALL='y'

    # skip cache update check for first time install YUM runs
    if [[ "$INITIALINSTALL" = [yY] ]]; then
        # CACHESKIP=' -C'
        CACHESKIP=""
    else
        CACHESKIP=""
    fi

    if [[ "$INITIALINSTALL" = [Yy] ]]; then
        lowmemcheck initialinstall
    else
        lowmemcheck
    fi
    # setramdisk
    centminlog
    diskalert
    {
    
    CHECKCENTMINMODINSTALL=$(ls /etc/init.d | grep -E '(csf|lfd|nginx|php-fpm|^nsd)')
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
    
    dlstarttime=$(TZ=UTC date +%s.%N)
    {    
    alldownloads
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_downloadtimes_${DT}.log"
    wait

    dlendtime=$(TZ=UTC date +%s.%N)
    DOWNLOADTIME=$(echo "scale=2;$dlendtime - $dlstarttime"|bc )

    echo "" >> "${CENTMINLOGDIR}/centminmod_downloadtimes_${DT}.log"
    echo "Total YUM + Source Download Time: $DOWNLOADTIME seconds" >> "${CENTMINLOGDIR}/centminmod_downloadtimes_${DT}.log"
    ls -lah "${CENTMINLOGDIR}/centminmod_downloadtimes_${DT}.log"

    funct_centmininstall

    # setup command shortcut aliases 
    # given the known download location
    # updated method for cmdir and centmin shorcuts
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    rm -rf /usr/bin/cmdir
    alias cmdir="pushd ${SCRIPT_DIR}"
    echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; . ./centmin.sh
cleanup_msg() {
  exit 1
}

trap cleanup_msg SIGHUP SIGINT SIGTERM SIGTSTP
EOF
    if [[ "$(id -u)" -ne '0' ]]; then
      sed -i '/cmdir=/d' $HOME/.bashrc
      sed -i '/centmin=/d' $HOME/.bashrc
      rm -rf /usr/bin/cmdir
      alias cmdir="pushd ${SCRIPT_DIR}"
      echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> $HOME/.bashrc
    fi
    chmod 0700 /usr/bin/centmin

    unsetramdisk

    echo "$SCRIPT_VERSION" > /etc/centminmod-release
    #echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
    
    if [ "$CCACHEINSTALL" == 'y' ]; then
    
        # check if ccache installed first
        if [ -f /usr/bin/ccache ]; then
    { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
        fi
    fi
    
    endtime=$(TZ=UTC date +%s.%N)
    INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
    echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
    echo "Total Centmin Mod Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
    
    exit 0
else
        if [[ "$ENABLE_MENU" = [yY] ]]; then
            while :
            do
            # clear
                # display menu
            cecho "--------------------------------------------------------" $boldyellow
            cecho "     Centmin Mod Menu $branchname centminmod.com     " $boldgreen
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
            cecho "11). MariaDB MySQL Upgrade & Management" $boldgreen
            cecho "12). Zend OpCache Install/Re-install" $boldgreen
            cecho "13). Install/Reinstall Redis PHP Extension" $boldgreen
            cecho "14). SELinux disable" $boldgreen
            cecho "15). Install/Reinstall ImagicK PHP Extension" $boldgreen
            cecho "16). Change SSHD Port Number" $boldgreen
            cecho "17). Multi-thread compression: zstd,pigz,pbzip2,lbzip2" $boldgreen
            cecho "18). Suhosin PHP Extension install" $boldgreen
            cecho "19). Install FFMPEG and FFMPEG PHP Extension" $boldgreen
            cecho "20). NSD Install/Re-Install" $boldgreen
            cecho "21). Data Transfer (TBA)" $boldgreen
            cecho "22). Add Wordpress Nginx vhost + Cache Plugin" $boldgreen
            cecho "23). Update Centmin Mod Code Base" $boldgreen
            cecho "24). Exit" $boldgreen
            cecho "--------------------------------------------------------" $boldyellow
        
            read -ep "Enter option [ 1 - 24 ] " option
            cecho "--------------------------------------------------------" $boldyellow
        
        #########################################################
        
        case "$option" in
        1|install)
            CM_MENUOPT=1
            starttime=$(TZ=UTC date +%s.%N)
            INITIALINSTALL='y'
            export INITIALINSTALL='y'

            # skip cache update check for first time install YUM runs
            if [[ "$INITIALINSTALL" = [yY] ]]; then
                # CACHESKIP=' -C'
                CACHESKIP=""
            else
                CACHESKIP=""
            fi

            if [[ "$INITIALINSTALL" = [Yy] ]]; then
                lowmemcheck initialinstall
            else
                lowmemcheck
            fi
            # setramdisk
            centminlog
            diskalert
            {
            
            CHECKCENTMINMODINSTALL=$(ls /etc/init.d | grep -E '(csf|lfd|nginx|php-fpm|^nsd)')
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

    # setup command shortcut aliases 
    # given the known download location
    # updated method for cmdir and centmin shorcuts
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    rm -rf /usr/bin/cmdir
    alias cmdir="pushd ${SCRIPT_DIR}"
    alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'
    alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'
    alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'
    echo "alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'" >> /root/.bashrc
    echo "alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'" >> /root/.bashrc
    echo "alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'" >> /root/.bashrc
    echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
    if [[ "$(id -u)" -ne '0' ]]; then
      sed -i '/cmdir=/d' $HOME/.bashrc
      sed -i '/centmin=/d' $HOME/.bashrc
      rm -rf /usr/bin/cmdir
      echo "alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'" >> $HOME/.bashrc
      echo "alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'" >> $HOME/.bashrc
      echo "alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'" >> $HOME/.bashrc
      echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> $HOME/.bashrc
    fi
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; . ./centmin.sh
cleanup_msg() {
  exit 1
}

trap cleanup_msg SIGHUP SIGINT SIGTERM SIGTSTP
EOF
    chmod 0700 /usr/bin/centmin

            unsetramdisk

            echo "$SCRIPT_VERSION" > /etc/centminmod-release
            #echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
            } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
            
            if [ "$CCACHEINSTALL" == 'y' ]; then
            
                # check if ccache installed first
                if [ -f /usr/bin/ccache ]; then
            { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
                fi
            fi
            
            endtime=$(TZ=UTC date +%s.%N)
            INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
            echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
            echo "Total Centmin Mod Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
            
            exit 0
        
        ;;
        2|addvhost)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_nginx_addvhost.log"
        fi
        # set_logdate
        CM_MENUOPT=2
        centminlog
        {
        funct_nginxaddvhost
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log"
        
        ;;
        3|nsdsetup)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_nsd_setup.log"
        fi
        # set_logdate
        CM_MENUOPT=3
        centminlog
        {
        funct_nsdsetup
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log"
        
        ;;
        4|nginxupgrade)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_nginx_upgrade.log"
        fi
        # set_logdate
        CM_MENUOPT=4
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        diskalert
        csftweaks
        
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        #yumskipinstall
        if [[ "$yuminstallrun" == [yY] ]]; then
        yuminstall
        fi
        funct_nginxupgrade
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        echo "Total Nginx Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'nginx_upgrade.log' | tail -1)"
        
        ;;
        5|phpupgrade)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_php_upgrade.log"
        fi
        # set_logdate
        CM_MENUOPT=5
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        diskalert
        csftweaks
        
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        #yumskipinstall
        if [[ "$yuminstallrun" == [yY] ]]; then
        yuminstall
        fi
        funct_phpupgrade
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        cat "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" | egrep -v 'checking for|checking if|checking how|checking the|checking sys|checking whether|^checking |/fpm-build/main -I|/fpm-build/libtool |/fpm-build/include -I' > "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_minimal.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'php_upgrade.log' | tail -1)"
        
        ;;
        6|xcachereinstall)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_xcache_reinstall.log"
        fi
        # set_logdate
        CM_MENUOPT=6
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        funct_xcachereinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
        echo "Total Xcache Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
        
        ;;
        7|apcreinstall)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_apc_reinstall.log"
        fi
        # set_logdate
        CM_MENUOPT=7
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        funct_apcreinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
        echo "Total APC Cache Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
        
        ;;
        8|installxcache)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_xcache_install.log"
        fi
        # set_logdate
        CM_MENUOPT=8
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        MANXCACHEINSTALL='y'
        
        funct_installxcache
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
        echo "Total Xcache Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
        
        ;;
        9|installapc)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_apc_install.log"
        fi
        # set_logdate
        CM_MENUOPT=9
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        funct_installapc
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
        echo "Total APC Cache Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
        
        ;;
        10|memcachedreinstall)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_memcached_reinstall.log"
        fi
        # set_logdate
        CM_MENUOPT=10
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        if [[ "$TIME_MEMCACHED" = [yY] ]]; then
            funct_memcachedreinstall 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
        else
            funct_memcachedreinstall
        fi
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        echo "Total Memcached Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        
        ;;
        11|mariadbsubmenu)
        # set_logdate
        CM_MENUOPT=11
        mariadbsubmenu
        
        ;;
        12|zendoptcachesubmenu)
        # set_logdate
        CM_MENUOPT=12
        zendoptcachesubmenu
        
        ;;
        13|redisphp)
        # set_logdate
        CM_MENUOPT=13
        phpredis_submenu
        
        ;;
        14|selinux)
        # set_logdate
        CM_MENUOPT=14
        funct_selinux
        
        ;;
        15|imagick)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_php-imagick-install.log"
        fi
        # set_logdate
        CM_MENUOPT=15
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        if [[ "$TIME_IMAGEMAGICK" = [yY] ]]; then
            imagickinstall submenu 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
        else
            imagickinstall submenu
        fi
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        echo "Total ImagicK PHP Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        
        ;;
        16|sshdport)
        # set_logdate
        CM_MENUOPT=16
        {
        funct_sshd
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_menu-option16-sshdport-change.log"

        ;;
        17|multithreadcomp)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_multithread_compression-install.log"
        fi
        # set_logdate
        CM_MENUOPT=17
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        compressmenu_notice
        funct_pigzinstall
        funct_pbzip2install
        # funct_lbzip2install
        funct_lzipinstall
        funct_plzipinstall
        zstdinstall
        lzfourinstall
        #funct_p7zipinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        echo "Total Multi-Threaded Compression Tools Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
        ;;
        18|suhosininstall)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_suhosin_install.log"
        fi
        # set_logdate
        CM_MENUOPT=18
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        suhosinsetup
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log"
            fi
        fi
        
        ;;
        19|ffmpeginstall)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_ffmpeg_install.log"
        fi
        # set_logdate
        CM_MENUOPT=19
        centminlog
        ffmpegsubmenu
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log"
            fi
        fi
        
        ;;
        20|nsdreinstall)
        # set_logdate
        CM_MENUOPT=20
        nsdsubmenu
        
        ;;
        21|update)
        # set_logdate
        CM_MENUOPT=21
        UALL='y'
        starttime=$(TZ=UTC date +%s.%N)
        centminlog
        {
        cecho "Place holder for future feature allowing Centmin Mod To Centmin Mod server data migration" $boldyellow
        echo
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_data_transfer.log"
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_data_transfer.log"
        echo "Total Data Transfer Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_data_transfer.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep '_data_transfer.log' | tail -1)"
        ;;
        22|addwpvhost)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_wordpress_addvhost.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_wordpress_addvhost.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_wordpress_addvhost.log"
        fi
        # set_logdate
        CM_MENUOPT=22
        centminlog
        {
        if [[ "$WP_FASTCGI_CACHE" = [yY] ]]; then
          fc_wpacctsetup
        else
          wpacctsetup
        fi
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_wordpress_addvhost.log"
        
        ;;        
        23|cmupdatemenu)
        # set_logdate
        CM_MENUOPT=23
        updatersubmenu

        ;;
        24|exit)
        # set_logdate
        CM_MENUOPT=24
        bookmark
        break
        exit 0
        
        ;;
        
        esac
        
        done
    else
        case "$1" in
        install)
        INITIALINSTALL='y'
        export INITIALINSTALL='y'

        # skip cache update check for first time install YUM runs
        if [[ "$INITIALINSTALL" = [yY] ]]; then
            # CACHESKIP=' -C'
            CACHESKIP=""
        else
            CACHESKIP=""
        fi

        if [[ "$INITIALINSTALL" = [Yy] ]]; then
            lowmemcheck initialinstall
        else
            lowmemcheck
        fi
        # setramdisk
        diskalert
        alldownloads
        funct_centmininstall
        unsetramdisk
        echo "$SCRIPT_VERSION" > /etc/centminmod-release
        echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
        
        ;;
        addvhost)
        
        funct_nginxaddvhost
        
        ;;
        nsdsetup)
        
        funct_nsdsetup
        
        ;;
        nginxupgrade|nginx-update)
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_nginx_upgrade.log"
        fi
        # set_logdate
        CM_MENUOPT=4
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        diskalert
        csftweaks
        
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        #yumskipinstall
        if [[ "$yuminstallrun" == [yY] ]]; then
        yuminstall
        fi
        funct_nginxupgrade "$2"
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        echo "Total Nginx Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'nginx_upgrade.log' | tail -1)"
        
        ;;
        phpupgrade|php-update)
        
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_php_upgrade.log"
        fi
        # set_logdate
        CM_MENUOPT=5
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        diskalert
        csftweaks
        
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        #yumskipinstall
        if [[ "$yuminstallrun" == [yY] ]]; then
        yuminstall
        fi
        funct_phpupgrade "$2"
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        cat "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log" | egrep -v 'checking for|checking if|checking how|checking the|checking sys|checking whether|^checking |/fpm-build/main -I|/fpm-build/libtool |/fpm-build/include -I' > "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_minimal.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'php_upgrade.log' | tail -1)"
        
        ;;
        php-update-all)
        
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_php_upgrade_all.log"
        fi
        # set_logdate
        CM_MENUOPT=5
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        diskalert
        csftweaks
        
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        #yumskipinstall
        if [[ "$yuminstallrun" == [yY] ]]; then
        yuminstall
        fi
        funct_phpupgrade "$2" all
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log"
        echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log"
        cat "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all.log" | egrep -v 'checking for|checking if|checking how|checking the|checking sys|checking whether|^checking |/fpm-build/main -I|/fpm-build/libtool |/fpm-build/include -I' > "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade_all_minimal.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'php_upgrade_all.log' | tail -1)"
        
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
        
        mariadbsubmenu
        
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
        multithreadcomp|compress-update)
        
        if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log" ]; then
            NEWDT=$(date +"%d%m%y-%H%M%S")
            mv "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log" "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${NEWDT}_multithread_compression-install.log"
        fi
        # set_logdate
        CM_MENUOPT=17
        starttime=$(TZ=UTC date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        compressmenu_notice
        funct_pigzinstall
        funct_pbzip2install
        # funct_lbzip2install
        funct_lzipinstall
        funct_plzipinstall
        zstdinstall
        lzfourinstall
        #funct_p7zipinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
            fi
        fi
        
        endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        echo "Total Multi-Threaded Compression Tools Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
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
        update)
        UALL='y'
        cecho "Updating Nginx, PHP-FPM & Siege versions" $boldyellow
        echo
        #yumskipinstall

        if [[ "$yuminstallrun" == [yY] ]]; then
            yuminstall
        fi

        funct_nginxupgrade
        funct_phpupgrade
        checksiege
        siegeinstall

        echo ""
            cecho "--------------------------------------------------------" $boldyellow
            cecho "Check Nginx Version:" $boldyellow
            cecho "--------------------------------------------------------" $boldyellow
            nginx -V
        echo ""
            cecho "--------------------------------------------------------" $boldyellow
            cecho "Check PHP-FPM Version:" $boldyellow
            cecho "--------------------------------------------------------" $boldyellow
            php -v
        echo ""
            cecho "--------------------------------------------------------" $boldyellow
            cecho "Check Siege Benchmark Version:" $boldyellow
            cecho "--------------------------------------------------------" $boldyellow
        siege -V

        ;;
        exit)
          
        echo ""
        echo "exit"
        
        bookmark
        
        exit 0
        
        ;;
        *)
        
        # echo "$0 install"
        # echo "$0 addvhost"
        # echo "$0 nsdsetup"
        # echo "$0 nginxupgrade"
        # echo "$0 phpupgrade"
        # echo "$0 xcachereinstall"
        # echo "$0 apcreinstall"
        # echo "$0 installxcache"
        # echo "$0 installapac"
        # echo "$0 memcachedreinstall"
        # echo "$0 mariadbupgrade"
        # echo "$0 installioping"
        # echo "$0 selinux"
        # echo "$0 logrotate"
        # echo "$0 phplogrotate"
        # echo "$0 sshdport"
        # echo "$0 multithreadcomp"
        # echo "$0 suhosininstall"
        # echo "$0 ffmpeginstall"
        # echo "$0 nsdreinstall"
        # echo "$0 update"

        echo "$0 nginx-update 1.19.7"
        echo "$0 php-update 7.4.15"
        echo "$0 php-update-all 7.4.15"
        echo "$0 compress-update"
        
        ;;
        esac
    fi
fi

exit 0