#!/bin/sh
export PATH="/usr/lib64/ccache:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
#####################################################
EMAIL=''          # Server notification email address enter only 1 address
PUSHOVER_EMAIL='' # Signup pushover.net push email notifications to mobile & tablets
ZONEINFO=Etc/UTC  # Set Timezone
NGINX_IPV='n' #NGINX IPV6 compile support for unattended mode only
USEEDITOR='nano' # choice between nano or vim text editors for cmd shortcuts

CUSTOMSERVERNAME='y'
CUSTOMSERVERSTRING='nginx centminmod'
PHPFPMCONFDIR='/usr/local/nginx/conf/phpfpmd'

UNATTENDED='y' # please leave at 'y' for best compatibility as at .07 release
CMVERSION_CHECK='n'
#####################################################
DT=`date +"%d%m%y-%H%M%S"`
# for github support
branchname='123.09beta01'
SCRIPT_MAJORVER='1.2.3'
SCRIPT_MINORVER='09'
SCRIPT_INCREMENTVER='001'
SCRIPT_VERSION="${SCRIPT_MAJORVER}-eva2000.${SCRIPT_MINORVER}.${SCRIPT_INCREMENTVER}"
SCRIPT_DATE='30/04/2016'
SCRIPT_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_MODIFICATION_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_URL='http://centminmod.com'
COPYRIGHT="Copyright 2011-2016 CentminMod.com"
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
shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done
#####################################################
HN=$(uname -n)
# Pre-Checks to prevent screw ups
DIR_TMP='/svr-setup'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

source "inc/memcheck.inc"
TMPFSLIMIT=2900000
if [ ! -d "$DIR_TMP" ]; then
        if [[ "$TOTALMEM" -ge "$TMPFSLIMIT" ]]; then
            TMPFSENABLED=1
            RAMDISKTMPFS='y'
            echo "setting up $DIR_TMP on tmpfs ramdisk for initial install"
            mkdir -p "$DIR_TMP"
            mount -t tmpfs -o size=2200M,mode=0755 tmpfs "$DIR_TMP"
            df -hT
        else
            mkdir -p "$DIR_TMP"
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

TESTEDCENTOSVER='7.9'
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi

source "inc/centos_seven.inc"
seven_function

cmservice() {
        servicename=$1
        action=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "service ${servicename} $action"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                service "${servicename}" "$action"
        fi
        else
        echo "systemctl $action ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl "$action" "${servicename}.service"
        fi
        fi
}

cmchkconfig() {
        servicename=$1
        status=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
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
}

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    CPUS=$(echo "$CPUS+1" | bc)
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    CPUS=$(echo "$CPUS+1" | bc)
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
# http://centminmod.com/upgrade.html#persistent
AUTO_GITUPDATE='n'
#####################################################
# Timestamp Install
TS_INSTALL='y'

#####################################################
# Enable or disable menu mode
ENABLE_MENU='y'

#####################################################
# CentOS 7 specific
FIREWALLD_DISABLE='y'

#####################################################
# MariaDB Jemalloc
# doh forgot MariaDB already uses jemalloc by default
MARIADB_JEMALLOC='n'

#####################################################
# CCACHE Configuration
CCACHEINSTALL='y'
CCACHE_VER="3.2.4"
CCACHESIZE='2.2G'

#####################################################
# experimental use of subshells to download some
# tarballs in parallel for faster initial installs
PARALLEL_MODE=y
# compiler related
CLANG='y'                     # Nginx and LibreSSL
CLANG_PHP='n'                 # PHP
CLANG_APC='n'                 # APC Cache
CLANG_MEMCACHED='n'           # Memcached menu option 10 routine
GCCINTEL_PHP='y'              # enable PHP-FPM GCC compiler with Intel cpu optimizations

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
NGXDYNAMIC_XSLT='n'
NGXDYNAMIC_PERL='n'
NGXDYNAMIC_IMAGEFILTER='y'
NGXDYNAMIC_GEOIP='y'
NGXDYNAMIC_STREAM='y'
NGXDYNAMIC_HEADERSMORE='n'
NGXDYNAMIC_SETMISC='n'
NGXDYNAMIC_ECHO='n'
NGXDYNAMIC_LUA='n'
NGXDYNAMIC_NGXPAGESPEED='n'
NGXDYNAMIC_BROTLI='y'

# set = y to put nginx, php and mariadb major version updates into 503 
# maintenance mode https://community.centminmod.com/posts/26485/
NGINX_UPDATEMAINTENANCE='n'
PHP_UPDATEMAINTENANCE='n'
MARIADB_UPDATEMAINTENANCE='n'

# General Configuration
NGINXUPGRADESLEEP='3'
NSD_INSTALL='y'              # Install NSD (DNS Server)
NSD_VERSION='3.2.18'         # NSD Version
NTP_INSTALL='y'              # Install Network time protocol daemon
NGINXPATCH='y'               # Set to y to allow NGINXPATCH_DELAY seconds time before Nginx configure and patching Nginx
NGINXPATCH_DELAY='1'         # Number of seconds to pause Nginx configure routine during Nginx upgrades
STRIPNGINX='y'               # set 'y' to strip nginx binary to reduce size
NGXMODULE_ALTORDER='y'       # nginx configure module ordering alternative order
NGINX_INSTALL='y'            # Install Nginx (Webserver)
NGINX_DEBUG='n'              # Enable & reinstall Nginx debug log nginx.org/en/docs/debugging_log.html & wiki.nginx.org/Debugging
NGINX_HTTP2='y'              # Nginx http/2 patch https://community.centminmod.com/threads/4127/
NGINX_MODSECURITY=n          # modsecurity module support https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#Installation_for_NGINX
NGINX_RDNS='n'               # https://github.com/flant/nginx-http-rdns
NGINX_NJS='n'                # nginScript https://www.nginx.com/blog/launching-nginscript-and-looking-ahead/
NGINX_GEOIP='y'              # Nginx GEOIP module install
NGINX_GEOIPMEM='y'           # Nginx caches GEOIP databases in memory (default), setting 'n' caches to disk instead
NGINX_SPDY='y'               # Nginx SPDY support
NGINX_STUBSTATUS='y'         # http://nginx.org/en/docs/http/ngx_http_stub_status_module.html required for nginx statistics
NGINX_SUB='y'                # http://nginx.org/en/docs/http/ngx_http_sub_module.html
NGINX_ADDITION='y'           # http://nginx.org/en/docs/http/ngx_http_addition_module.html
NGINX_IMAGEFILTER='y'        # http://nginx.org/en/docs/http/ngx_http_image_filter_module.html
NGINX_PERL='n'               # http://nginx.org/en/docs/http/ngx_http_perl_module.html
NGINX_XSLT='n'               # http://nginx.org/en/docs/http/ngx_http_xslt_module.html
NGINX_CACHEPURGE='y'         # https://github.com/FRiCKLE/ngx_cache_purge/
NGINX_ACCESSKEY='n'          #
NGINX_HTTPCONCAT='n'         # https://github.com/alibaba/nginx-http-concat
NGINX_THREADS='y'            # https://www.nginx.com/blog/thread-pools-boost-performance-9x/
NGINX_STREAM='y'             # http://nginx.org/en/docs/stream/ngx_stream_core_module.html
NGINX_RTMP='n'               # Nginx RTMP Module support https://github.com/arut/nginx-rtmp-module
NGINX_FLV='n'                # http://nginx.org/en/docs/http/ngx_http_flv_module.html
NGINX_MP4='n'                # Nginx MP4 Module http://nginx.org/en/docs/http/ngx_http_mp4_module.html
NGINX_AUTHREQ='n'            # http://nginx.org/en/docs/http/ngx_http_auth_request_module.html
NGINX_SECURELINK='y'         # http://nginx.org/en/docs/http/ngx_http_secure_link_module.html
NGINX_FANCYINDEX='y'         # http://wiki.nginx.org/NgxFancyIndex
NGINX_FANCYINDEXVER='0.3.6'
NGINX_VHOSTSTATS='y'         # https://github.com/vozlt/nginx-module-vts
NGINX_LIBBROTLI='n'          # https://github.com/google/ngx_brotli
NGINX_LIBBROTLISTATIC='n'
NGINX_PAGESPEED='n'          # Install ngx_pagespeed
NGINX_PAGESPEEDGITMASTER='n' # Install ngx_pagespeed from official github master instead  
NGXPGSPEED_VER='1.10.33.6-beta'
NGINX_PAGESPEEDPSOL_VER='1.10.33.6'
NGINX_PASSENGER='n'          # Install Phusion Passenger requires installing addons/passenger.sh before hand
NGINX_WEBDAV='n'             # Nginx WebDAV and nginx-dav-ext-module
NGINX_EXTWEBDAVVER='0.0.3'   # nginx-dav-ext-module version
NGINX_LIBATOMIC='y'          # Nginx configured with libatomic support
NGINX_HTTPREDIS='y'          # Nginx redis http://wiki.nginx.org/HttpRedisModule
NGINX_HTTPREDISVER='0.3.7'   # Nginx redis version
NGINX_PCREJIT='y'            # Nginx configured with pcre & pcre-jit support
NGINX_PCREVER='8.38'         # Version of PCRE used for pcre-jit support in Nginx
ORESTY_HEADERSMORE='y'       # openresty headers more https://github.com/openresty/headers-more-nginx-module
ORESTY_HEADERSMOREGIT='n'    # use git master instead of version specific
NGINX_HEADERSMORE='0.29'
NGINX_CACHEPURGEVER='2.3'
NGINX_STICKY='n'             # nginx sticky module https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
NGINX_STICKYVER='1.2.6'
NGINX_UPSTREAMCHECK='y'      # nginx upstream check https://github.com/yaoweibin/nginx_upstream_check_module
NGINX_UPSTREAMCHECKVER='0.3.0'
NGINX_OPENRESTY='y'          # Agentzh's openresty Nginx modules
ORESTY_MEMCVER='0.16'        # openresty memc module https://github.com/openresty/memc-nginx-module
ORESTY_SRCCACHEVER='0.30'    # openresty subrequest cache module https://github.com/openresty/srcache-nginx-module
ORESTY_DEVELKITVER='0.3.0rc1'  # openresty ngx_devel_kit module https://github.com/simpl/ngx_devel_kit
ORESTY_SETMISCGIT='n'        # use git master instead of version specific
ORESTY_SETMISCVER='0.30'     # openresty set-misc-nginx module https://github.com/openresty/set-misc-nginx-module
ORESTY_ECHOGIT='n'           # use git master instead of version specific
ORESTY_ECHOVER='0.58'        # openresty set-misc-nginx module https://github.com/openresty/echo-nginx-module
ORESTY_REDISVER='0.12'       # openresty redis2-nginx-module https://github.com/openresty/redis2-nginx-module

LUAJIT_GITINSTALL='y'        # opt to install luajit 2.1 from dev branch http://repo.or.cz/w/luajit-2.0.git/shortlog/refs/heads/v2.1
LUAJIT_GITINSTALLVER='2.1'   # branch version = v2.1 will override ORESTY_LUAGITVER if LUAJIT_GITINSTALL='y'

ORESTY_LUANGINX='n'             # enable or disable or ORESTY_LUA* nginx modules below
ORESTY_LUANGINXVER='0.10.2'     # openresty lua-nginx-module https://github.com/openresty/lua-nginx-module
ORESTY_LUAGITVER='2.0.4'        # luagit http://luajit.org/
ORESTY_LUAMEMCACHEDVER='0.13'   # openresty https://github.com/openresty/lua-resty-memcached
ORESTY_LUAMYSQLVER='0.15'       # openresty https://github.com/openresty/lua-resty-mysql
ORESTY_LUAREDISVER='0.22'       # openresty https://github.com/openresty/lua-resty-redis
ORESTY_LUADNSVER='0.14'         # openresty https://github.com/openresty/lua-resty-dns
ORESTY_LUAUPLOADVER='0.09'      # openresty https://github.com/openresty/lua-resty-upload
ORESTY_LUAWEBSOCKETVER='0.05'   # openresty https://github.com/openresty/lua-resty-websocket
ORESTY_LUALOCKVER='0.04'        # openresty https://github.com/openresty/lua-resty-lock
ORESTY_LUASTRINGVER='0.09'      # openresty https://github.com/openresty/lua-resty-string
ORESTY_LUAREDISPARSERVER='0.10'    # openresty https://github.com/openresty/lua-redis-parser
ORESTY_LUAUPSTREAMCHECKVER='0.04'  # openresty https://github.com/openresty/lua-resty-upstream-healthcheck
ORESTY_LUALRUCACHEVER='0.04'       # openresty https://github.com/openresty/lua-resty-lrucache
ORESTY_LUARESTYCOREVER='0.1.5'     # openresty https://github.com/openresty/lua-resty-core
ORESTY_LUAUPSTREAMVER='0.05'       # openresty https://github.com/openresty/lua-upstream-nginx-module
NGX_LUAUPSTREAM='n'                # disable https://github.com/openresty/lua-upstream-nginx-module
ORESTY_LUALOGGERSOCKETVER='0.1'    # cloudflare openresty https://github.com/cloudflare/lua-resty-logger-socket
ORESTY_LUACOOKIEVER='master'       # cloudflare openresty https://github.com/cloudflare/lua-resty-cookie
ORESTY_LUAUPSTREAMCACHEVER='0.1.1' # cloudflare openresty https://github.com/cloudflare/lua-upstream-cache-nginx-module
NGX_LUAUPSTREAMCACHE='n'           # disable https://github.com/cloudflare/lua-upstream-cache-nginx-module
LUACJSONVER='2.1.0.3'              # https://github.com/openresty/lua-cjson

STRIPPHP='y'                 # set 'y' to strip PHP binary to reduce size
PHP_INSTALL='y'              # Install PHP /w Fast Process Manager
PHPMAKETEST=n                # set to y to enable make test after PHP make for diagnostic purposes
AUTODETECPHP_OVERRIDE='n'    # when enabled, php updates will always reinstall all php extensions even if minor php version

PHPGEOIP_ALWAYS='y'          # GeoIP php extension is always reinstalled on php recompiles
PHPDEBUGMODE='n'             # --enable-debug PHP compile flag
PHPFINFO='n'                 # Disable or Enable PHP File Info extension
PHPPCNTL='y'                 # Disable or Enable PHP Process Control extension
PHPINTL='y'                  # Disable or Enable PHP intl extension
PHPRECODE=n                  # Disable or Enable PHP Recode extension
PHPSNMP='y'                  # Disable or Enable PHP SNMP extension
PHPIMAGICK='y'               # Disable or Enable PHP ImagicK extension
PHPMAILPARSE='y'             # Disable or Enable PHP mailparse extension
PHPIONCUBE='n'               # Disable or Enable Ioncube Loader via addons/ioncube.sh
SHORTCUTS='y'                # shortcuts

POSTGRESQL='n'               # set to =y to install PostgreSQL 9.4 server, devel packages and pdo-pgsql PHP extension
########################################################
# Choice of installing MariaDB 5.2 via RPM or via MariaDB 5.2 CentOS YUM Repo
# If MDB_YUMREPOINSTALL=y and MDB_INSTALL=n then MDB_VERONLY version 
# number won't have any effect in determining version of MariaDB 5.2.x to install. 
# YUM Repo will install whatever is latest MariaDB 5.2.x version available via the YUM REPO

MDB_INSTALL='n'               # Install via RPM MariaDB MySQL Server replacement (Not recommended for VPS with less than 256MB RAM!)
MDB_YUMREPOINSTALL='y'		  # Install MariaDB 5.5 via CentOS YUM Repo

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
NGINX_VERSION='1.9.12'       # Use this version of Nginx
NGINX_VHOSTSSL='y'           # enable centmin.sh menu 2 prompt to create self signed SSL vhost 2nd vhost conf
NGINXBACKUP='y'
NGINXDIR='/usr/local/nginx'
NGINXCONFDIR="${NGINXDIR}/conf"
NGINXBACKUPDIR='/usr/local/nginxbackup'

##################################
## Nginx SSL options
# OpenSSL
NOSOURCEOPENSSL='y'        # set to 'y' to disable OpenSSL source compile for system default YUM package setup
OPENSSL_VERSION='1.0.2g'   # Use this version of OpenSSL http://openssl.org/
CLOUDFLARE_PATCHSSL='y'    # set 'y' to implement Cloudflare's chacha20 patch https://github.com/cloudflare/sslconfig

# LibreSSL
LIBRESSL_SWITCH='y'        # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
LIBRESSL_VERSION='2.3.3'   # Use this version of LibreSSL http://www.libressl.org/
##################################

# Choose whether to compile Nginx --with-google_perftools_module
# no longer used in Centmin Mod v1.2.3-eva2000.01 and higher
GPERFTOOLS_SOURCEINSTALL='n'
LIBUNWIND_VERSION='0.99'     # note google perftool specifically requies v0.99 and no other
GPERFTOOLS_VERSION='1.8.3'     # Use this version of google-perftools

# Choose whether to compile PCRE from source. Note PHP 5.3.8 already includes PCRE v8.12
PCRE_SOURCEINSTALL='n'     
PCRE_VERSION='8.38'          # NO longer used/ignored

# PHP and Cache/Acceleration
IMAGICKPHP_VER='3.4.1'   # PHP extension for imagick
MAILPARSEPHP_VER='2.1.6'    # https://pecl.php.net/package/mailparse
MAILPARSEPHP_COMPATVER='3.0.0' # For PHP 7
MEMCACHED_INSTALL='y'         # Install Memcached
LIBEVENT_VERSION='2.0.22'   # Use this version of Libevent
MEMCACHED_VERSION='1.4.25'  # Use this version of Memcached server
MEMCACHE_VERSION='3.0.8'    # Use this version of Memcache
MEMCACHEDPHP_VER='2.2.0'    # Memcached PHP extension not server
LIBMEMCACHED_YUM='y'        # switch to YUM install instead of source compile
LIBMEMCACHED_VER='1.0.18'   # libmemcached version for source compile
TWEMPERF_VER='0.1.1'
PHPREDIS='y'                # redis PHP extension install
REDISPHP_VER='2.2.7'        # redis PHP version
PHPMONGODB='n'              # MongoDB PHP extension install
MONGODBPHP_VER='1.1.4'      # MongoDB PHP version
MONGODB_SASL='n'            # SASL not working yet leave = n
PDOPGSQL_PHPVER='9.5'       # pdo-pgsql PHP extension version for postgresql

PHP_FTPEXT='y'              # ftp PHP extension
PHP_MEMCACHE='y'            # memcache PHP extension 
PHP_MEMCACHED='y'           # memcached PHP extension
FFMPEGVER='0.6.0'
SUHOSINVER='0.9.37.1'
PHP_VERSION='5.6.19'        # Use this version of PHP
PHP_MIRRORURL='http://php.net'
PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
XCACHE_VERSION='3.2.0'      # Use this version of Xcache
APCCACHE_VERSION='3.1.13'   # Use this version of APC Cache
IGBINARY_VERSION='1.2.1'
IGBINARY_INSTALL='y'        # install or not igbinary support for APC and Memcached server
IGBINARYGIT='y'
ZOPCACHEDFT='y'
ZOPCACHECACHE_VERSION='7.0.5'   # for PHP <=5.4 http://pecl.php.net/package/ZendOpcache
ZOPCACHE_OVERRIDE='n'           # =y will override PHP 5.5, 5.6, 7.0 inbuilt Zend OpCache version
# Python
PYTHON_VERSION='2.7.10'       # Use this version of Python
SIEGE_VERSION='3.1.4'

CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
WGETOPT='-cnv --no-dns-cache -4'
AXEL_VER='2.5'                # Axel source compile version https://github.com/eribertomota/axel/releases
###############################################################
# experimental Intel compiled optimisations 
# when auto detect Intel based processors
INTELOPT='n'

# experimental custom RPM compiled packages to replace source 
# compiled versions for 64bit systems only
FPMRPM_LIBEVENT='n'
FPMRPM_MEMCACHED='n'
CENTALTREPO_DISABLE='y'
RPMFORGEREPO_DISABLE='n'
AXIVOREPO_DISABLE='y'
REMIREPO_DISABLE='n'
ATRPMSREPO_DISABLE='y'

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
WPCLI_SUPERCACHEPLUGIN='n'  # https://community.centminmod.com/threads/5102/
###############################################################
# php configured --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock
PHP_MYSQLSOCKPATH='/var/lib/mysql'
###############################################################

MACHINE_TYPE=$(uname -m) # Used to detect if OS is 64bit or not.

if [ "${ARCH_OVERRIDE}" != '' ]
then
    ARCH=${ARCH_OVERRIDE}
else
    if [ "${MACHINE_TYPE}" == 'x86_64' ];
    then
        ARCH='x86_64'
        MDB_ARCH='amd64'
    else
        ARCH='i386'
    fi
fi

# ensure if ORESTY_LUANGINX is enabled, that the other required
# Openresty modules are enabled if folks forget to enable them
if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    NGINX_OPENRESTY='y'
fi

# ensure clang alternative to gcc compiler is used only for 64bit OS
# if [[ "$(uname -m)" != 'x86_64' ]]; then
#     CLANG='n'
# fi

# source "inc/mainmenu.inc"
# source "inc/mainmenu_cli.inc"
# source "inc/ramdisk.inc"
source "inc/customrpms.inc"
source "inc/pureftpd.inc"
source "inc/htpasswdsh.inc"
source "inc/gcc.inc"
source "inc/entropy.inc"
source "inc/cpucount.inc"
source "inc/motd.inc"
source "inc/cpcheck.inc"
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
source "inc/nginx_mimetype.inc"
source "inc/openssl_install.inc"
source "inc/brotli.inc"
source "inc/nginx_configure.inc"
# source "inc/nginx_configure_openresty.inc"
source "inc/geoip.inc"
source "inc/luajit.inc"
source "inc/nginx_patch.inc"
source "inc/nginx_install.inc"
source "inc/nginx_upgrade.inc"
source "inc/mailparse.inc"
source "inc/imagick_install.inc"
source "inc/memcached_install.inc"
source "inc/redis.inc"
source "inc/mongodb.inc"
source "inc/mysql_proclimit.inc"
source "inc/mysqltmp.inc"
source "inc/setmycnf.inc"
source "inc/mariadb_install.inc"
source "inc/mysql_install.inc"
source "inc/mariadb_submenu.inc"
source "inc/postgresql.inc"
source "inc/zendopcache_tweaks.inc"
source "inc/php_extraopts.inc"
source "inc/php_configure.inc"
source "inc/phpng_download.inc"
source "inc/php_upgrade.inc"
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
source "inc/mariadb_upgrade.inc"
source "inc/mariadb_upgrade53.inc"
source "inc/mariadb_upgrade55.inc"
source "inc/mariadb_upgrade10.inc"
source "inc/mariadb_upgrade101.inc"
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
source "inc/zendopcache_55ini.inc"
source "inc/zendopcache_install.inc"
source "inc/zendopcache_upgrade.inc"
source "inc/zendopcache_reinstall.inc"
source "inc/zendopcache_submenu.inc"
source "inc/ffmpeginstall.inc"
source "inc/shortcuts_install.inc"
source "inc/memcacheadmin.inc"
source "inc/mysqlsecure.inc"
source "inc/updater_submenu.inc"
source "inc/centminfinish.inc"

checkcentosver
mysqltmpdir
cpcheck

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
    source "inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
    source "${CM_INSTALLDIR}/inc/z_custom.inc"
fi
###############################################################
# FUNCTIONS

if [ -f /usr/bin/xz ]; then
    PHPEXTSION='xz'
    PHPTAR_FLAGS='xJf'
else
    PHPEXTSION='gz'
    PHPTAR_FLAGS='xzf'
fi

if [[ "$INITIALINSTALL" = [yY] ]]; then
    PHPXZ_CHECK=$(echo "$PHP_VERSION" | cut -d . -f1,2)
    if [[ "$PHPXZ_CHECK" != '5.5' || "$PHPXZ_CHECK" != '5.6' || "$PHPXZ_CHECK" != '5.7' || "$PHPXZ_CHECK" != '7.0' || "$PHPXZ_CHECK" != '7.1' ]]; then
        PHPEXTSION='gz'
        PHPTAR_FLAGS='xzf'
    fi
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
    DOWNLOADAPP='axel -a'
    WGETRETRY=''
    AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.${PHPEXTSION}"
    AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.${PHPEXTSION}"
elif [[ "$CENTOS_SIX" = '6' ]]; then
    DOWNLOADAPP='axel -a'
    WGETRETRY=''
    AXELPHPTARGZ="-o php-${PHP_VERSION}.tar.${PHPEXTSION}"
    AXELPHPUPGRADETARGZ="-o php-${phpver}.tar.${PHPEXTSION}"
else
    DOWNLOADAPP="wget ${WGETOPT} --progress=bar"
    WGETRETRY='--tries=3'
    AXELPHPTARGZ=''
    AXELPHPUPGRADETARGZ=''
fi

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
#    yum${CACHESKIP} -q clean all
#    yum${CACHESKIP} -y update glibc\*
#    yum${CACHESKIP} -y update yum\* rpm\* python\*
#    yum${CACHESKIP} -q clean all
#    yum${CACHESKIP} -y update
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

ex -s /etc/yum.conf << EOF
:/plugins=1/
:a
exclude=*.i386 *.i586 *.i686
.
:w
:q
EOF
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
elif [[ ! -f /proc/user_beancounters && "$CENTOS_SEVEN" = '7' ]]; then
    echo "CentOS 7 Setup /tmp"
    echo "CentOS 7 + non-OpenVZ virtualisation detected"
    systemctl is-enabled tmp.mount
elif [[ ! -f /proc/user_beancounters ]]; then

    # TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
    CURRENT_TMPSIZE=$(df /tmp | awk '/tmp/ {print $2}')

    # only mount /tmp on tmpfs if CentOS system
    # total memory size is greater than 8GB
    # will give /tmp a size equal to 1/2 total memory
    if [[ "$TOTALMEM" -ge '8100001' ]]; then
	   rm -rf /tmp
	   mkdir -p /tmp
	   mount -t tmpfs -o rw,noexec,nosuid tmpfs /tmp
	   chmod 1777 /tmp
	   echo "tmpfs /tmp tmpfs rw,noexec,nosuid 0 0" >> /etc/fstab
	   rm -rf /var/tmp
	   ln -s /tmp /var/tmp
    elif [[ "$TOTALMEM" -ge '2050061' || "$TOTALMEM" -lt '8100000' ]]; then
       # set on disk non-tmpfs /tmp to 4GB size
       # if total memory is between 2GB and <8GB
       rm -rf /tmp
       dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=4000000
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       rm -rf /var/tmp
       ln -s /tmp /var/tmp
    elif [[ "$TOTALMEM" -ge '1153434' || "$TOTALMEM" -lt '2050060' ]]; then
       # set on disk non-tmpfs /tmp to 2GB size
       # if total memory is between 1.1-2GB
       rm -rf /tmp
       dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=2000000
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       rm -rf /var/tmp
       ln -s /tmp /var/tmp
    elif [[ "$TOTALMEM" -le '1153433' ]]; then
       # set on disk non-tmpfs /tmp to 1GB size
       # if total memory is <1.1GB
       rm -rf /tmp
       dd if=/dev/zero of=/home/usertmp_donotdelete bs=1024 count=1000000
       echo Y | mkfs.ext4 /home/usertmp_donotdelete
       mkdir -p /tmp
       mount -t ext4 -o loop,rw,noexec,nosuid /home/usertmp_donotdelete /tmp
       chmod 1777 /tmp
       echo "/home/usertmp_donotdelete /tmp ext4 loop,rw,noexec,nosuid 0 0" >> /etc/fstab
       rm -rf /var/tmp
       ln -s /tmp /var/tmp       
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
    rm -f /etc/localtime
    ln -s "/usr/share/zoneinfo/$ZONEINFO" /etc/localtime
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
        if [ ! -f /usr/sbin/ntpd ]; then
            yum${CACHESKIP} -y install ntp
        fi
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

ngxinstallstarttime=$(date +%s.%N)
{    
ngxinstallmain
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
wait

ngxinstallendtime=$(date +%s.%N)
NGXINSTALLTIME=$(echo "scale=2;$ngxinstallendtime - $ngxinstallstarttime"|bc )

echo "" >> "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
echo "Total Nginx First Time Install Time: $NGXINSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"
ls -lah "${CENTMINLOGDIR}/centminmod_ngxinstalltime_${DT}.log"

mariadbinstallfunct
mysqlinstallfunct

if [[ "$PHP_INSTALL" = [yY] ]]; then
    phpinstallstarttime=$(date +%s.%N)
    echo "*************************************************"
    cecho "* Installing PHP" $boldgreen
    echo "*************************************************"

funct_centos6check

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

    cd "$DIR_TMP"

if [ "$(rpm -qa | grep '^php*')" ]; then

    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
echo "yum -y erase php*"
    yum${CACHESKIP} -y erase php*

fi

#download php tarball

    #tar xzvf php-${PHP_VERSION}.tar.gz

    cd "php-${PHP_VERSION}"

    ./buildconf --force
    mkdir fpm-build && cd fpm-build

    mkdir -p "/usr/${LIBDIR}/mysql"
    ln -s "/usr/${LIBDIR}/libmysqlclient.so" "/usr/${LIBDIR}/mysql/libmysqlclient.so"

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
else
    echo "$lessphpmem"
    echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
    cp "$CUR_DIR/config/php-fpm/php-fpm.conf" /usr/local/etc/php-fpm.conf
fi


    cp "$CUR_DIR/init/php-fpm" /etc/init.d/php-fpm

# add check for Windows CLRF line endings
if [ ! -f /usr/bin/file ]; then
    yum -q -y install file
fi
if [[ "$(file /etc/init.d/php-fpm)" =~ CRLF && -f /etc/init.d/php-fpm ]]; then
    if [ ! -f /usr/bin/dos2unix ]; then
        yum -q -y install dos2unix
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
    chmod 0666 /var/log/php-fpm/www-error.log
    chmod 0666 /var/log/php-fpm/www-php.error.log
    fpmconfdir

    #chown -R root:nginx /var/lib/php/session/
    chkconfig --levels 235 php-fpm on
    #/etc/init.d/php-fpm restart 2>/dev/null
    # /etc/init.d/php-fpm force-quit
    /etc/init.d/php-fpm start

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
    phpinstallendtime=$(date +%s.%N)
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

# if ZOPCACHEDFT override enabled = yY and PHP_VERSION is not 5.5, 5.6 or 5.7
# install Zend OpCache PECL extesnion otherwise if PHP_VERSION = 5.5
# then php_configure.inc routine will pick up PHP_VERSION 5.5 and install
# native Zend OpCache when ZOPCACHEDFT=yY
PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)

# ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.0's inbuilt included
# Zend Opcache version with one available from pecl site
if [[ "$ZOPCACHE_OVERRIDE" = [yY] && "$ZOPCACHEDFT" = [yY] ]] && [[ "$PHPMVER" = '5.4' || "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' || "$PHPMVER" = '5.7' || "$PHPMVER" = '7.0' ]]; then
    zopcacheinstall
fi

if [[ "$ZOPCACHEDFT" = [yY] && "$PHPMVER" = '5.4' ]]; then
    zopcacheinstall
fi

if [[ "$ZOPCACHEDFT" = [yY] && "$PHPMVER" = '5.3' ]]; then
    zopcacheinstall
fi

if [[ "$ZOPCACHEDFT" = [yY] && "$PHPMVER" = '5.2' ]]; then
    zopcacheinstall
fi

# if PHP_VERSION = 5.5, 5.6 or 5.7 will need to setup a zendopcache.ini settings file
if [[ "$ZOPCACHE_OVERRIDE" != [yY] ]] && [[ "$APCINSTALL" = [nN] || "$ZOPCACHEDFT" = [yY] ]] && [[ "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' || "$PHPMVER" = '5.7' || "$PHPMVER" = '7.0' ]]; then
	zopcache_initialini
fi

phpsededit

# igbinary still needed for libmemcached PHP extension if ZOPCACHE=yY
# or for redis php extension
if [[ "$APCINSTALL" = [nN] || "$ZOPCACHEDFT" = [yY] ]]; then
	funct_igbinaryinstall
fi

if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [nN] ]]; then
	if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
		echo "*************************************************"
		cecho "* Installing sendmail" $boldgreen
		echo "*************************************************"
		yum${CACHESKIP} -y -q install sendmail mailx sendmail-cf
		chkconfig --levels 235 sendmail on
		funct_sendmailmc
		echo "*************************************************"
		cecho "* sendmail installed" $boldgreen
		echo "*************************************************"
	elif [[ -f /etc/init.d/postfix ]]; then
        if [ ! -f /bin/mail ]; then
            yum${CACHESKIP} -y -q install mailx
        fi
        postfixsetup
        echo "*************************************************"
        cecho "Postfix already detected, sendmail install aborted" $boldgreen
        echo "*************************************************"
    elif [[ -f /etc/init.d/sendmail ]]; then
        if [ ! -f /bin/mail ]; then
            yum${CACHESKIP} -y -q install mailx
        fi
        chkconfig --levels 235 sendmail on
        funct_sendmailmc
        echo "*************************************************"
        cecho "sendmail already detected, sendmail install aborted" $boldgreen
        echo "*************************************************"
	fi
fi

if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [yY] ]]; then
    if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
        echo "*************************************************"
        cecho "* Installing postfix" $boldgreen
        echo "*************************************************"
        yum${CACHESKIP} -y -q install postfix mailx
        postfixsetup
        echo "*************************************************"
        cecho "* postfix installed" $boldgreen
        echo "*************************************************"
    elif [[ -f /etc/init.d/postfix ]]; then
        if [ ! -f /bin/mail ]; then
            yum${CACHESKIP} -y -q install mailx
        fi
        postfixsetup
        echo "*************************************************"
        cecho "Postfix already detected, postfix install aborted" $boldgreen
        echo "*************************************************"
    elif [[ -f /etc/init.d/sendmail ]]; then
        yum${CACHESKIP} -y -q remove sendmail sendmail-cf
        yum${CACHESKIP} -y -q install postfix mailx
        postfixsetup
        echo "*************************************************"
        cecho "* postfix installed" $boldgreen
        echo "*************************************************"
    fi
fi


if [[ "$POSTFIX_INSTALL" = [yY] && "$SENDMAIL_INSTALL" = [nN] ]]; then
    if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
        echo "*************************************************"
        cecho "* Installing postfix" $boldgreen
        echo "*************************************************"
        yum${CACHESKIP} -y -q install postfix mailx
        postfixsetup
        echo "*************************************************"
        cecho "* postfix installed" $boldgreen
        echo "*************************************************"
    elif [[ -f /etc/init.d/postfix ]]; then
        if [ ! -f /bin/mail ]; then
            yum${CACHESKIP} -y -q install mailx
        fi
        postfixsetup
        echo "*************************************************"
        cecho "Postfix already detected, postfix install aborted" $boldgreen
        echo "*************************************************"
    elif [[ -f /etc/init.d/sendmail ]]; then
        if [ -f /usr/share/sendmail-cf ]; then
            yum${CACHESKIP} -y -q remove sendmail sendmail-cf
        else
           yum${CACHESKIP} -y -q remove sendmail
        fi
        yum${CACHESKIP} -y -q install postfix mailx
        postfixsetup
        echo "*************************************************"
        cecho "* postfix installed" $boldgreen
        echo "*************************************************"
    fi
fi

echo "incmemcachedinstall"
incmemcachedinstall

echo "csfinstalls"
csfinstalls

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

echo "nsdinstall"
nsdinstall

echo "pureftpinstall"
pureftpinstall

if [ -f "$CUR_DIR/Extras/nginx-update.sh" ];
then
    chmod +x "$CUR_DIR/Extras/nginx-update.sh"
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
    rm -rf "$DIR_TMP"
    rm -rf "$CUR_DIR/config"
    rm -rf "$CUR_DIR/init"
    rm -rf "$CUR_DIR/sysconfig"
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f "$0"

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

funct_openvz_stacksize
checkxcacheadmin


    echo "*************************************************"
    cecho "* Running updatedb command. Please wait...." $boldgreen
    echo "*************************************************"

time updatedb

centminfinish
memcacheadmin

    echo "*************************************************"
    cecho "* MariaDB Security Setup" $boldgreen
    echo "*************************************************"

if [[ "$MDB_INSTALL" == [yY] || "$MYSQL_INSTALL" == [yY] || "$UNATTENDED" == [yY] ]]; then
	securemysql
else
	securemysql
fi

    echo "*************************************************"
    cecho "* MariaDB Security Setup Completed" $boldgreen
    echo "*************************************************"

bookmark

sync 

if [ ! -f /proc/user_beancounters ]; then
echo 3 > /proc/sys/vm/drop_caches
fi

}


#####################################################################
#####################################################################
# functions

funct_centos6check() {


if [[ "$CENTOSVER" == '5.6' || "$CENTOSVER" == '5.7'|| "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' || "$CENTOSVER" == '5.11' || "$CENTOSVER" == '6.0' || "$CENTOSVER" == '6.1' || "$CENTOSVER" == '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' || "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then

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
        yum -q -y install ioping
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
trap cleanup_msg SIGHUP SIGINT SIGTERM

# end functions
#####################################################################
#####################################################################

# main menu
# inc/mainmenu.inc
# inc/mainmenu_cli.inc
#########################################################

if [[ "$1" = 'install' ]]; then
    starttime=$(date +%s.%N)
    INITIALINSTALL='y'

    # skip cache update check for first time install YUM runs
    if [[ "$INITIALINSTALL" = [yY] ]]; then
        # CACHESKIP=' -C'
        CACHESKIP=""
    else
        CACHESKIP=""
    fi

    lowmemcheck
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
    
    dlstarttime=$(date +%s.%N)
    {    
    alldownloads
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_downloadtimes_${DT}.log"
    wait

    dlendtime=$(date +%s.%N)
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
pushd "$SCRIPT_DIR"; bash centmin.sh
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
    
    endtime=$(date +%s.%N)
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
            cecho "Centmin Mod ${SCRIPT_MAJORVER}-eva2000.${SCRIPT_MINORVER} - $SCRIPT_URL" $boldgreen
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
            cecho "11). MariaDB 5.2/5.5 & 10.x Upgrade Sub-Menu" $boldgreen
            cecho "12). Zend OpCache Install/Re-install" $boldgreen
            cecho "13). Install ioping.sh vbtechsupport.com/1239/" $boldgreen
            cecho "14). SELinux disable" $boldgreen
            cecho "15). Install/Reinstall ImagicK PHP Extension" $boldgreen
            cecho "16). Change SSHD Port Number" $boldgreen
            cecho "17). Multi-thread compression: pigz,pbzip2,lbzip2..." $boldgreen
            cecho "18). Suhosin PHP Extension install" $boldgreen
            cecho "19). Install FFMPEG and FFMPEG PHP Extension" $boldgreen
            cecho "20). NSD Re-install" $boldgreen
            cecho "21). Update - Nginx + PHP-FPM + Siege" $boldgreen
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
            starttime=$(date +%s.%N)
            INITIALINSTALL='y'

            # skip cache update check for first time install YUM runs
            if [[ "$INITIALINSTALL" = [yY] ]]; then
                # CACHESKIP=' -C'
                CACHESKIP=""
            else
                CACHESKIP=""
            fi

            lowmemcheck
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
    echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; bash centmin.sh
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
            
            endtime=$(date +%s.%N)
            INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
            echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
            echo "Total Centmin Mod Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log"
            
            exit 0
        
        ;;
        2|addvhost)
        CM_MENUOPT=2
        centminlog
        {
        funct_nginxaddvhost
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log"
        
        ;;
        3|nsdsetup)
        CM_MENUOPT=3
        centminlog
        {
        funct_nsdsetup
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log"
        
        ;;
        4|nginxupgrade)
        CM_MENUOPT=4
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
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
            fi
        fi
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        echo "Total Nginx Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'nginx_upgrade.log' | tail -1)"
        
        ;;
        5|phpupgrade)
        CM_MENUOPT=5
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
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
            fi
        fi
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'php_upgrade.log' | tail -1)"
        
        ;;
        6|xcachereinstall)
        CM_MENUOPT=6
        starttime=$(date +%s.%N)
        
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
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
        echo "Total Xcache Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log"
        
        ;;
        7|apcreinstall)
        CM_MENUOPT=7
        starttime=$(date +%s.%N)
        
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
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
        echo "Total APC Cache Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log"
        
        ;;
        8|installxcache)
        CM_MENUOPT=8
        starttime=$(date +%s.%N)
        
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
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
        echo "Total Xcache Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log"
        
        ;;
        9|installapc)
        CM_MENUOPT=9
        starttime=$(date +%s.%N)
        
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
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
        echo "Total APC Cache Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log"
        
        ;;
        10|memcachedreinstall)
        CM_MENUOPT=10
        starttime=$(date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        funct_memcachedreinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
            fi
        fi
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        echo "Total Memcached Re-Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log"
        
        ;;
        11|mariadbsubmenu)
        CM_MENUOPT=11
        mariadbsubmenu
        
        ;;
        12|zendoptcachesubmenu)
        CM_MENUOPT=12
        zendoptcachesubmenu
        
        ;;
        13|iopinginstall)
        CM_MENUOPT=13
        funct_installiopingcentmin
        
        ;;
        14|selinux)
        CM_MENUOPT=14
        funct_selinux
        
        ;;
        15|imagick)
        CM_MENUOPT=15
        starttime=$(date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        imagickinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
            fi
        fi
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        echo "Total ImagicK PHP Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php-imagick-install.log"
        
        ;;
        16|sshdport)
        CM_MENUOPT=16
        funct_sshd
        
        ;;
        17|multithreadcomp)
        CM_MENUOPT=17
        starttime=$(date +%s.%N)
        
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        funct_pigzinstall
        funct_pbzip2install
        funct_lbzip2install
        funct_lzipinstall
        funct_plzipinstall
        funct_p7zipinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
            fi
        fi
        
        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        echo "Total Multi-Threaded Compression Tools Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_multithread_compression-install.log"
        
        ;;
        18|suhosininstall)
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
        CM_MENUOPT=20
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        nsdreinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log"
            fi
        fi
        
        ;;
        21|update)
        CM_MENUOPT=21
        UALL='y'
        starttime=$(date +%s.%N)
        centminlog
        {
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        ccacheinstall
        fi
        
        cecho "Updating Nginx, PHP-FPM & Siege versions" $boldyellow
        echo
        yumskipinstall

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

        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_update_all.log"
        
        if [ "$CCACHEINSTALL" == 'y' ]; then
        
            # check if ccache installed first
            if [ -f /usr/bin/ccache ]; then
        { echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_update_all.log"
            fi
        fi

        endtime=$(date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_update_all.log"
        echo "Total Update Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_update_all.log"
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'update_all.log' | tail -1)"

        ;;
        22|addwpvhost)
        CM_MENUOPT=22
        centminlog
        {
        wpacctsetup
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_wordpress_addvhost.log"
        
        ;;        
        23|cmupdatemenu)
        CM_MENUOPT=23
        updatersubmenu

        ;;
        24|exit)
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

        # skip cache update check for first time install YUM runs
        if [[ "$INITIALINSTALL" = [yY] ]]; then
            # CACHESKIP=' -C'
            CACHESKIP=""
        else
            CACHESKIP=""
        fi

        lowmemcheck
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
        update)
        UALL='y'
        cecho "Updating Nginx, PHP-FPM & Siege versions" $boldyellow
        echo
        yumskipinstall

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
        echo "$0 update"
        
        ;;
        esac
    fi
fi

exit 0