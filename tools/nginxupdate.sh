#!/bin/bash
###################################################################################
# standalone centmin mod nginx updater for cli command line run
# still requires to be run only from tools/nginxupdate.sh though due to source
# file variable dependencies
###################################################################################
ngver=$1
NGINX_IPV='n' #NGINX IPV6 compile support for unattended mode only
UNATTENDED='y' # please leave at 'y' for best compatibility as at .07 release
CMVERSION_CHECK='n'
###################################################################################
DT=$(date +"%d%m%y-%H%M%S")
# for github support
branchname='123.09beta01'
SCRIPT_MAJORVER='1.2.3'
SCRIPT_MINORVER='09'
SCRIPT_INCREMENTVER='010'
SCRIPT_VERSIONSHORT="${branchname}"
SCRIPT_VERSION="${SCRIPT_VERSIONSHORT}.b${SCRIPT_INCREMENTVER}"
SCRIPT_DATE='31/01/2018'
SCRIPT_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_MODIFICATION_AUTHOR='eva2000 (centminmod.com)'
SCRIPT_URL='https://centminmod.com'
COPYRIGHT="Copyright 2011-2018 CentminMod.com"
DISCLAIMER='This software is provided "as is" in the hope that it will be useful, but WITHOUT ANY WARRANTY, to the extent permitted by law; without even the implied warranty of MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.'
###################################################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

HN=$(uname -n)
# Pre-Checks to prevent screw ups
DIR_TMP='/svr-setup'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
SCRIPT_SOURCEBASE=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
# account for tools directory placement of tools/nginxupdate.sh
SCRIPT_DIR=$(readlink -f $(dirname ${SCRIPT_DIR}))

# source "inc/memcheck.inc"
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

if [ ! -d "$CENTMINLOGDIR" ]; then
    mkdir -p "$CENTMINLOGDIR"
fi

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
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
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
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
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

###################################################################################
#####################################################
# experimental use of subshells to download some
# tarballs in parallel for faster initial installs
PARALLEL_MODE=y
# compiler related
MARCH_TARGETNATIVE='y'        # for intel 64bit only set march=native, if no set to x86-64
CLANG='y'                     # Nginx and LibreSSL
CLANG_FOUR='n'                # Clang 4.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_FIVE='n'                # Clang 5.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_SIX='n'                 # Clang 6.0+ optional support https://community.centminmod.com/threads/13729/
CLANG_PHP='n'                 # PHP
CLANG_APC='n'                 # APC Cache
CLANG_MEMCACHED='n'           # Memcached menu option 10 routine
GCCINTEL_PHP='y'              # enable PHP-FPM GCC compiler with Intel cpu optimizations
PHP_PGO='n'                   # Profile Guided Optimization https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php
PHP_PGO_CENTOSSIX='n'         # CentOS 6 may need GCC >4.4.7 fpr PGO so use devtoolset-4 GCC 5.3
DEVTOOLSETSIX='n'             # Enable or disable devtoolset-6 GCC 6.2 support instead of devtoolset-4 GCC 5.3 support
DEVTOOLSETSEVEN='n'           # Enable or disable devtoolset-7 GCC 7.1 support instead of devtoolset-6 GCC 6.2 support
DEVTOOLSETEIGHT='n'           # source compiled GCC 8 from latest snapshot builds
NGINX_DEVTOOLSETGCC='n'       # Use devtoolset-4 GCC 5.3 even for CentOS 7 nginx compiles
GENERAL_DEVTOOLSETGCC='n'     # Use devtoolset-4 GCC 5.3 whereever possible/coded
CRYPTO_DEVTOOLSETGCC='n'      # Use devtoolset-4 GCC 5.3 for libressl or openssl compiles
NGX_GSPLITDWARF='y'           # for Nginx compile https://community.centminmod.com/posts/44072/
PHP_GSPLITDWARF='y'           # for PHP compile https://community.centminmod.com/posts/44072/
PHP_LTO='n'                   # enable -flto compiler for GCC 4.8.5+ PHP-FPM compiles currently not working with PHP 7.x
NGX_LDGOLD='y'                # for Nginx compile i.e. passing ld.gold linker -fuse-ld=bfd or -fuse-ld=gold https://community.centminmod.com/posts/44037/
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
NGXDYNAMIC_STREAM='y'
NGXDYNAMIC_STREAMGEOIP='n'  # nginx 1.11.3+ option http://hg.nginx.org/nginx/rev/558db057adaa
NGXDYNAMIC_STREAMREALIP='n' # nginx 1.11.4+ option http://hg.nginx.org/nginx/rev/9cac11efb205
NGXDYNAMIC_HEADERSMORE='n'
NGXDYNAMIC_SETMISC='n'
NGXDYNAMIC_ECHO='n'
NGXDYNAMIC_LUA='n'          #
NGXDYNAMIC_SRCCACHE='n'
NGXDYNAMIC_DEVELKIT='n'     #
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
NGINXUPGRADESLEEP='3'
NSD_INSTALL='n'              # Install NSD (DNS Server)
NSD_VERSION='3.2.18'         # NSD Version
NTP_INSTALL='y'              # Install Network time protocol daemon
NGINXPATCH='y'               # Set to y to allow NGINXPATCH_DELAY seconds time before Nginx configure and patching Nginx
NGINXPATCH_DELAY='1'         # Number of seconds to pause Nginx configure routine during Nginx upgrades
STRIPNGINX='y'               # set 'y' to strip nginx binary to reduce size
NGXMODULE_ALTORDER='y'       # nginx configure module ordering alternative order
NGINX_INSTALL='y'            # Install Nginx (Webserver)
NGINX_DEBUG='n'              # Enable & reinstall Nginx debug log nginx.org/en/docs/debugging_log.html & wiki.nginx.org/Debugging
NGINX_HTTP2='y'              # Nginx http/2 patch https://community.centminmod.com/threads/4127/
NGINX_HTTPPUSH='n'           # Nginx http/2 push patch https://community.centminmod.com/threads/11910/
NGINX_MODSECURITY='n'          # modsecurity module support https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#Installation_for_NGINX
NGINX_REALIP='y'             # http://nginx.org/en/docs/http/ngx_http_realip_module.html
NGINX_RDNS='n'               # https://github.com/flant/nginx-http-rdns
NGINX_NJS='n'                # nginScript https://www.nginx.com/blog/launching-nginscript-and-looking-ahead/
NGINX_GEOIP='y'              # Nginx GEOIP module install
NGINX_GEOIPMEM='y'           # Nginx caches GEOIP databases in memory (default), setting 'n' caches to disk instead
NGINX_SPDY='n'               # Nginx SPDY support
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
NGINX_LIBBROTLISTATIC='n'
NGINX_PAGESPEED='n'          # Install ngx_pagespeed
NGINX_PAGESPEEDGITMASTER='n' # Install ngx_pagespeed from official github master instead  
NGXPGSPEED_VER='1.13.35.2-stable'
NGINX_PAGESPEEDPSOL_VER='1.13.35.2'
NGINX_PASSENGER='n'          # Install Phusion Passenger requires installing addons/passenger.sh before hand
NGINX_WEBDAV='n'             # Nginx WebDAV and nginx-dav-ext-module
NGINX_EXTWEBDAVVER='0.0.3'   # nginx-dav-ext-module version
NGINX_LIBATOMIC='y'          # Nginx configured with libatomic support
NGINX_HTTPREDIS='y'          # Nginx redis http://wiki.nginx.org/HttpRedisModule
NGINX_HTTPREDISVER='0.3.7'   # Nginx redis version
NGINX_PCREJIT='y'            # Nginx configured with pcre & pcre-jit support
NGINX_PCREVER='8.42'         # Version of PCRE used for pcre-jit support in Nginx
NGINX_ZLIBCUSTOM='y'         # Use custom zlib instead of system version
NGINX_ZLIBVER='1.2.11'       # http://www.zlib.net/
ORESTY_HEADERSMORE='y'       # openresty headers more https://github.com/openresty/headers-more-nginx-module
ORESTY_HEADERSMOREGIT='n'    # use git master instead of version specific
NGINX_HEADERSMORE='0.33'
NGINX_CACHEPURGEVER='2.4.2'
NGINX_STICKY='n'             # nginx sticky module https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
NGINX_STICKYVER='master'
NGINX_UPSTREAMCHECK='n'      # nginx upstream check https://github.com/yaoweibin/nginx_upstream_check_module
NGINX_UPSTREAMCHECKVER='0.3.0'
NGINX_OPENRESTY='y'          # Agentzh's openresty Nginx modules
ORESTY_MEMCVER='0.18'        # openresty memc module https://github.com/openresty/memc-nginx-module
ORESTY_SRCCACHEVER='0.31'    # openresty subrequest cache module https://github.com/openresty/srcache-nginx-module
ORESTY_DEVELKITVER='0.3.0'  # openresty ngx_devel_kit module https://github.com/simpl/ngx_devel_kit
ORESTY_SETMISCGIT='n'        # use git master instead of version specific
ORESTY_SETMISCVER='0.31'     # openresty set-misc-nginx module https://github.com/openresty/set-misc-nginx-module
ORESTY_ECHOGIT='n'           # use git master instead of version specific
ORESTY_ECHOVER='0.61'     # openresty set-misc-nginx module https://github.com/openresty/echo-nginx-module
ORESTY_REDISVER='0.14'       # openresty redis2-nginx-module https://github.com/openresty/redis2-nginx-module

LUAJIT_GITINSTALL='y'        # opt to install luajit 2.1 from dev branch http://repo.or.cz/w/luajit-2.0.git/shortlog/refs/heads/v2.1
LUAJIT_GITINSTALLVER='2.1'   # branch version = v2.1 will override ORESTY_LUAGITVER if LUAJIT_GITINSTALL='y'

ORESTY_LUANGINX='n'             # enable or disable or ORESTY_LUA* nginx modules below
ORESTY_LUANGINXVER='0.10.11'  # openresty lua-nginx-module https://github.com/openresty/lua-nginx-module
ORESTY_LUAGITVER='2.0.5'        # luagit http://luajit.org/
ORESTY_LUAMEMCACHEDVER='0.14'   # openresty https://github.com/openresty/lua-resty-memcached
ORESTY_LUAMYSQLVER='0.19'       # openresty https://github.com/openresty/lua-resty-mysql
ORESTY_LUAREDISVER='0.26'       # openresty https://github.com/openresty/lua-resty-redis
ORESTY_LUADNSVER='0.20'         # openresty https://github.com/openresty/lua-resty-dns
ORESTY_LUAUPLOADVER='0.10'      # openresty https://github.com/openresty/lua-resty-upload
ORESTY_LUAWEBSOCKETVER='0.05'   # openresty https://github.com/openresty/lua-resty-websocket
ORESTY_LUALOCKVER='0.07'        # openresty https://github.com/openresty/lua-resty-lock
ORESTY_LUASTRINGVER='0.10'      # openresty https://github.com/openresty/lua-resty-string
ORESTY_LUAREDISPARSERVER='0.13'    # openresty https://github.com/openresty/lua-redis-parser
ORESTY_LUAUPSTREAMCHECKVER='0.04'  # openresty https://github.com/openresty/lua-resty-upstream-healthcheck
ORESTY_LUALRUCACHEVER='0.07'       # openresty https://github.com/openresty/lua-resty-lrucache
ORESTY_LUARESTYCOREVER='0.1.13' # openresty https://github.com/openresty/lua-resty-core
ORESTY_LUAUPSTREAMVER='0.06'       # openresty https://github.com/openresty/lua-upstream-nginx-module
NGX_LUAUPSTREAM='n'                # disable https://github.com/openresty/lua-upstream-nginx-module
ORESTY_LUALOGGERSOCKETVER='0.1'    # cloudflare openresty https://github.com/cloudflare/lua-resty-logger-socket
ORESTY_LUACOOKIEVER='master'       # cloudflare openresty https://github.com/cloudflare/lua-resty-cookie
ORESTY_LUAUPSTREAMCACHEVER='0.1.1' # cloudflare openresty https://github.com/cloudflare/lua-upstream-cache-nginx-module
NGX_LUAUPSTREAMCACHE='n'           # disable https://github.com/cloudflare/lua-upstream-cache-nginx-module
LUACJSONVER='2.1.0.5'              # https://github.com/openresty/lua-cjson

STRIPPHP='y'                 # set 'y' to strip PHP binary to reduce size

NGINX_VHOSTSSL='y'           # enable centmin.sh menu 2 prompt to create self signed SSL vhost 2nd vhost conf
NGINXBACKUP='y'
NGINXDIR='/usr/local/nginx'
NGINXCONFDIR="${NGINXDIR}/conf"
NGINXBACKUPDIR='/usr/local/nginxbackup'

##################################
## Nginx SSL options
# OpenSSL
NOSOURCEOPENSSL='y'        # set to 'y' to disable OpenSSL source compile for system default YUM package setup
OPENSSL_VERSION='1.1.0h'   # Use this version of OpenSSL http://openssl.org/
OPENSSL_VERSIONFALLBACK='1.0.2o'   # fallback if OPENSSL_VERSION uses openssl 1.1.x branch
OPENSSL_THREADS='y'        # control whether openssl 1.1 branch uses threading or not
CLOUDFLARE_PATCHSSL='n'    # set 'y' to implement Cloudflare's chacha20 patch https://github.com/cloudflare/sslconfig
CLOUDFLARE_ZLIB='n'        # use Cloudflare optimised zlib fork https://blog.cloudflare.com/cloudflare-fights-cancer/
CLOUDFLARE_ZLIBPHP='n'     # use Cloudflare optimised zlib fork for PHP-FPM zlib instead of system zlib
CLOUDFLARE_ZLIBDEBUG='n'   # make install debug verbose mode
CLOUDFLARE_ZLIBVER='1.3.0'
NGINX_DYNAMICTLS='n'       # set 'y' and recompile nginx https://blog.cloudflare.com/optimizing-tls-over-tcp-to-reduce-latency/
OPENSSLECDSA_PATCH='n'       # https://community.centminmod.com/posts/57725/
OPENSSLECDHX_PATCH='n'       # https://community.centminmod.com/posts/57726/
OPENSSLEQUALCIPHER_PATCH='n' # https://community.centminmod.com/posts/57916/

# LibreSSL
LIBRESSL_SWITCH='y'        # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
LIBRESSL_VERSION='2.7.2'   # Use this version of LibreSSL http://www.libressl.org/
# BoringSSL
# not working yet just prep work
BORINGSSL_SWITCH='n'       # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
##################################

# Choose whether to compile Nginx --with-google_perftools_module
# no longer used in Centmin Mod v1.2.3-eva2000.01 and higher
GPERFTOOLS_SOURCEINSTALL='n'
GPERFTOOLS_TMALLOCLARGEPAGES='y'  # set larger page size for tcmalloc --with-tcmalloc-pagesize=32
LIBUNWIND_VERSION='1.2.1'           # note google perftool specifically requies v0.99 and no other
GPERFTOOLS_VERSION='2.6.3'        # Use this version of google-perftools

WGETOPT='-cnv --no-dns-cache -4'

###############################################################
# experimental Intel compiled optimisations 
# when auto detect Intel based processors
INTELOPT='n'

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

# ensure if ORESTY_LUANGINX is enabled, that the other required
# Openresty modules are enabled if folks forget to enable them
if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    NGINX_OPENRESTY='y'
fi

###################################################################################
# source file dependencies for variables
source "${SCRIPT_DIR}/inc/fastmirrors.conf"
source "${SCRIPT_DIR}/inc/customrpms.inc"
source "${SCRIPT_DIR}/inc/pureftpd.inc"
source "${SCRIPT_DIR}/inc/htpasswdsh.inc"
source "${SCRIPT_DIR}/inc/gcc.inc"
source "${SCRIPT_DIR}/inc/entropy.inc"
source "${SCRIPT_DIR}/inc/cpucount.inc"
source "${SCRIPT_DIR}/inc/motd.inc"
source "${SCRIPT_DIR}/inc/csftweaks.inc"
source "${SCRIPT_DIR}/inc/cpcheck.inc"
source "${SCRIPT_DIR}/inc/memcheck.inc"
source "${SCRIPT_DIR}/inc/ccache.inc"
source "${SCRIPT_DIR}/inc/bookmark.inc"
source "${SCRIPT_DIR}/inc/centminlogs.inc"
source "${SCRIPT_DIR}/inc/yumskip.inc"
source "${SCRIPT_DIR}/inc/downloads_centosfive.inc"
source "${SCRIPT_DIR}/inc/downloads_centossix.inc"
source "${SCRIPT_DIR}/inc/downloads_centosseven.inc"
source "${SCRIPT_DIR}/inc/downloadlinks.inc"
source "${SCRIPT_DIR}/inc/downloads.inc"
source "${SCRIPT_DIR}/inc/yumpriorities.inc"
source "${SCRIPT_DIR}/inc/yuminstall.inc"
source "${SCRIPT_DIR}/inc/centoscheck.inc"
source "${SCRIPT_DIR}/inc/axelsetup.inc"
source "${SCRIPT_DIR}/inc/phpfpmdir.inc"
source "${SCRIPT_DIR}/inc/nginx_backup.inc"
source "${SCRIPT_DIR}/inc/logrotate_nginx.inc"
source "${SCRIPT_DIR}/inc/nginx_mimetype.inc"
source "${SCRIPT_DIR}/inc/openssl_install.inc"
if [ -f ${SCRIPT_DIR}/inc/brotli.inc ]; then
source "${SCRIPT_DIR}/inc/brotli.inc"
fi
source "${SCRIPT_DIR}/inc/fastopen.inc"
source "${SCRIPT_DIR}/inc/mod_security.inc"
source "${SCRIPT_DIR}/inc/nginx_configure.inc"
source "${SCRIPT_DIR}/inc/geoip.inc"
source "${SCRIPT_DIR}/inc/luajit.inc"
source "${SCRIPT_DIR}/inc/nginx_patch.inc"
source "${SCRIPT_DIR}/inc/nginx_install.inc"
source "${SCRIPT_DIR}/inc/mysql_proclimit.inc"
source "${SCRIPT_DIR}/inc/mysqltmp.inc"
source "${SCRIPT_DIR}/inc/nginx_pagespeed.inc"
source "${SCRIPT_DIR}/inc/nginx_modules.inc"
source "${SCRIPT_DIR}/inc/nginx_modules_openresty.inc"
source "${SCRIPT_DIR}/inc/sshd.inc"
source "${SCRIPT_DIR}/inc/openvz_stack.inc"
source "${SCRIPT_DIR}/inc/nginx_addvhost.inc"
source "${SCRIPT_DIR}/inc/nginx_errorpage.inc"
source "${SCRIPT_DIR}/inc/compress.inc"
source "${SCRIPT_DIR}/inc/shortcuts_install.inc"
source "${SCRIPT_DIR}/inc/pcre.inc"
source "${SCRIPT_DIR}/inc/jemalloc.inc"
source "${SCRIPT_DIR}/inc/zlib.inc"
source "${SCRIPT_DIR}/inc/google_perftools.inc"
source "${SCRIPT_DIR}/inc/updater_submenu.inc"
source "${SCRIPT_DIR}/inc/centminfinish.inc"

cpcheck

CUR_DIR=$SCRIPT_DIR # Get current directory.
CM_INSTALLDIR=$CUR_DIR

if [[ "$GPERFTOOLS_TMALLOCLARGEPAGES" = [yY] ]]; then
    TCMALLOC_PAGESIZE='32'
else
    TCMALLOC_PAGESIZE='8'
fi

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "inc/custom_config.inc"
  fi
    source "inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
  fi
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

if [[ "$CENTOSVER" > 6 ]]; then
DOWNLOADAPP="wget ${WGETOPT} --progress=bar"
WGETRETRY='--tries=3'
AXELPHPTARGZ="-O php-${PHP_VERSION}.tar.gz"
AXELPHPUPGRADETARGZ="-O php-${phpver}.tar.gz"
else
DOWNLOADAPP="wget ${WGETOPT} --progress=bar"
WGETRETRY='--tries=3'
AXELPHPTARGZ=''
AXELPHPUPGRADETARGZ=''
fi

download_cmd() {
  HTTPS_AXELCHECK=$(echo "$1" |awk -F '://' '{print $1}')
  if [[ "$(curl -4Isv $1 2>&1 | egrep 'ECDSA')" ]]; then
    # axel doesn't natively support ECC 256bit ssl certs
    # with ECDSA ciphers due to CentOS system OpenSSL 1.0.2e
    echo "ECDSA SSL Cipher BASED HTTPS detected, switching from axel to wget"
    DOWNLOADAPP="wget ${WGETOPT}"
  elif [[ "$CENTOS_SIX" = '6' && "$HTTPS_AXELCHECK" = 'https' ]]; then
    echo "CentOS 6 Axel fallback to wget for HTTPS download"
    DOWNLOADAPP="wget ${WGETOPT}"
  fi
  $DOWNLOADAPP $1 $2 $3 $4
}

###################################################################################
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

###################################################################################
check_requestscheme() {
# check for REQUEST_SCHEME parameter added in nginx 1.9.2 
# add it if it doesn't exist in fastcgi_param and php include files
for f in $(grep -Rl 'fastcgi_param  SERVER_PROTOCOL    \$server_protocol;' /usr/local/nginx/conf/*); 
  do
    echo "$f"
    ff=$(grep 'REQUEST_SCHEME' $f)
    fff=$(grep 'https if_not_empty' $f)
    if [[ -z "$ff" && -z "$fff" ]]; then
        sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;\nfastcgi_param  HTTPS              \$https if_not_empty;|" $f
    elif [[ -z "$ff" && "$fff" ]]; then
        sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;|" $f
    fi
done

## DOES NOT WORK due to invalid version comparison for 2 dot
## numbers i.e. 2.0.0 < 1.9.1 would return false
# # check for REQUEST_SCHEME parameter added in nginx 1.9.2 
# # if nginx upgrade or downgrade is less than 1.9.2 comment out
# # REQUEST_SCHEME, if greater or equal to 1.9.2 uncomment
# for f in $(grep -Rl 'REQUEST_SCHEME' /usr/local/nginx/conf/*); 
#   do
#     echo "$f"
#     if [[ "$(expr $ngver \<= 1.9.1)" = 1 ]]; then
#         sed -i "s|fastcgi_param  REQUEST_SCHEME|#fastcgi_param  REQUEST_SCHEME|" $f
#     elif [[ "$(expr $ngver \>= 1.9.2)" = 1 ]]; then
#         sed -i "s|#fastcgi_param  REQUEST_SCHEME|fastcgi_param  REQUEST_SCHEME|" $f
#     fi
# done
}

checkgeoip() {
    GEOIP_CHECK=$(nginx -V 2>&1 | grep geoip)

    if [[ ! -z "$GEOIP_CHECK" && "$(grep 'NGINX_GEOIP=n' centmin.sh)" ]]; then
        cecho "Detected existing Nginx has NGINX_GEOIP=y enabled" $boldyellow
        cecho "however, you are recompiling Nginx with NGINX_GEOIP=n" $boldyellow
        cecho "Is this incorrect and you want to set NGINX_GEOIP=y enabled ? " $boldyellow
        read -ep "Answer y or n. Typing y will set NGINX_GEOIP=y [y/n]: " setgeoip
        if [[ "$setgeoip" = [yY] ]]; then
            NGINX_GEOIP=y 
        fi
    fi
}

checkmap() {
VTSHTTP_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)
VTSMAIN_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)

if [[ -z "$VTSHTTP_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    else
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
else
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/#include \/usr\/local\/nginx\/conf\/vts_http.conf/include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi
    else
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/vts_http.conf/#include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi        
    fi    
fi

if [[ -z "$VTSMAIN_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \ninclude \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    else
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \n#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
fi

if [[ ! -f /usr/local/nginx/conf/vts_http.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_http.conf /usr/local/nginx/conf/vts_http.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_mainserver.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_mainserver.conf /usr/local/nginx/conf/vts_mainserver.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_server.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_server.conf /usr/local/nginx/conf/vts_server.conf
fi

if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
    if [[ "$(grep '^#vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/#vhost_traffic_status_zone/vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^#vhost_traffic_status_dump' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/#vhost_traffic_status_dump/vhost_traffic_status_dump/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^#vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/#vhost_traffic_status on/vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
else
    if [[ "$(grep '^vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_zone/#vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^vhost_traffic_status_dump' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_dump/#vhost_traffic_status_dump/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/vhost_traffic_status on/#vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
fi

MAPCHECK=$(grep '/usr/local/nginx/conf/fastcgi_param_https_map.conf' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$MAPCHECK" ]]; then
	sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;/g' /usr/local/nginx/conf/nginx.conf
fi

if [[ ! -f /usr/local/nginx/conf/fastcgi_param_https_map.conf ]]; then
	\cp $CUR_DIR/config/nginx/fastcgi_param_https_map.conf /usr/local/nginx/conf/fastcgi_param_https_map.conf
fi

if [[ -z "$(grep 'fastcgi_param HTTPS $server_https;' /usr/local/nginx/conf/php.conf)" ]]; then
	replace -s '#fastcgi_param HTTPS on;' 'fastcgi_param HTTPS $server_https;' -- /usr/local/nginx/conf/php.conf
fi
}

checknginxmodules() {
    # axelsetup

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
    source "${CM_INSTALLDIR}/inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
    dos2unix -q "${CM_INSTALLDIR}/inc/z_custom.inc"
    source "${CM_INSTALLDIR}/inc/z_custom.inc"
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Check Nginx Modules"
    fi

    #################################################################################
    # detection routine to see if Nginx supports Dynamic modules from nginx 1.9.11+
    echo
    echo "NGX_DYNAMICCHECK nginx_upgrade.inc"
    pwd
    echo
    echo "nginx dynamic module support detected"
    echo
    if [ "$ngver" ]; then
        NGINX_DIRINSTALL="$DIR_TMP/nginx-${ngver}"
    else
        NGINX_DIRINSTALL="$DIR_TMP/nginx-${NGINX_VERSION}"
    fi

    NGX_DYNAMICCHECK=$(grep 'DYNAMIC_MODULES=' "$NGINX_DIRINSTALL/auto/options" >/dev/null 2>&1; echo $?)
    if [ "$NGX_DYNAMICCHECK" = '0' ]; then
        DYNAMIC_SUPPORT=y
    else
        # remove patches meant for 1.9.11 dynamic module support
        rm -rf "${DIR_TMP}lua-nginx-module-${ORESTY_LUANGINXVER}"
        rm -rf "${DIR_TMP}/${NGX_LUANGINXLINKFILE}"
    fi

    if [[ "$NGINX_RTMP" = [yY] ]]; then
        if [[ ! -d "${DIR_TMP}/nginx-rtmp-module" ]]; then
            echo
            echo "download nginx-rtmp-module from github"
            cd "$DIR_TMP"
            time git clone git://github.com/arut/nginx-rtmp-module.git
        elif [[ -d "${DIR_TMP}/nginx-rtmp-module" && -d "${DIR_TMP}/nginx-rtmp-module/.git" ]]; then
            echo
            echo "get latest updates nginx-rtmp-module from github"
            cd "$DIR_TMP"
            git stash
            git pull
            git log -3
        fi
    fi

cecho "Check for old ngx_pagespeed master branch existence" $boldyellow
if [[ -d "${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}/ngx_pagespeed-master" ]]; then
    # rm -rf ${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}
    rm -rf ${DIR_TMP}/ngx_pagespeed-release-*
    rm -rf ${DIR_TMP}/ngx_pagespeed-*
    rm -rf ${DIR_TMP}/release-1.9.32*
    nginxpgspeedtarball
fi

cecho "Check for missing nginx modules" $boldyellow

if [[ ! -f "${DIR_TMP}/${LIBRESSL_LINKFILE}" || ! -d "${DIR_TMP}/${LIBRESSLDIR}" ]]; then
    libressldownload
elif [[ ! -f "${DIR_TMP}/${LIBRESSL_LINKFILE}" ]]; then
    libressldownload
fi

LIBRESSLDIR=$(tar -tzf "$DIR_TMP/${LIBRESSL_LINKFILE}" 2>&1 | head -1 | cut -f1 -d"/" | grep libressl)
if [[ ! -f "${DIR_TMP}/${NGX_FANCYINDEXLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CACHEPURGEFILE}" || ! -f "${DIR_TMP}/${NGX_ACCESSKEYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CONCATLINKFILE}" || ! -f "${DIR_TMP}/${OPENSSL_LINKFILE}" || ! -f "${DIR_TMP}/${PCRELINKFILE}" || ! -f "${DIR_TMP}/${NGX_WEBDAVLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HEADERSMORELINKFILE}" || ! -f "${DIR_TMP}/${NGX_STICKYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_UPSTREAMCHECKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HTTPREDISLINKFILE}" ]] || [[ ! -d "${DIR_TMP}/${NGX_FANCYINDEXDIR}" || ! -d "${DIR_TMP}/${NGX_CACHEPURGEDIR}" || ! -d "${DIR_TMP}/nginx-accesskey-2.0.3" || ! -d "${DIR_TMP}/${NGX_CONCATDIR}" || ! -d "${DIR_TMP}/${OPENSSLDIR}" || ! -d "${DIR_TMP}/${PCRELINKDIR}" || ! -d "${DIR_TMP}/${NGX_WEBDAVLINKDIR}" || ! -d "${DIR_TMP}/${NGX_HEADERSMOREDIR}" || ! -d "${DIR_TMP}/${NGX_STICKYDIR}" || ! -d "${DIR_TMP}/${NGX_UPSTREAMCHECKDIR}" || ! -d "${DIR_TMP}/${NGX_HTTPREDISDIR}" ]]; then

    if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep "processor" /proc/cpuinfo |wc -l)" -gt '1' ]]; then
        ngxmoduletarball &
        openssldownload &
        # libressldownload &
        wait
    else
        ngxmoduletarball
        openssldownload
        # libressldownload
    fi
fi

cecho "Check for pagespeed nginx module download file" $boldyellow
# echo "${DIR_TMP}/${NGX_PAGESPEEDGITLINKFILE}"
# echo "$DIR_TMP/$LIBUNWIND_LINKDIR"
NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" 2>1 | head -1 | cut -f1 -d"/")
if [[ ! -f "${DIR_TMP}/${NGX_PAGESPEEDGITLINKFILE}" || ! -d "$DIR_TMP/$NGXPGSPEED_DIR" ]]; then
    if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
        nginxpgspeedtarball

    # determine top level extracted directory name for $DIR_TMP/${NGX_PAGESPEEDLINKFILE}
    NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
elif [[ ! -f "${DIR_TMP}/${NGX_PAGESPEEDGITLINKFILE}" ]]; then
    if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
        nginxpgspeedtarball

    # determine top level extracted directory name for $DIR_TMP/${NGX_PAGESPEEDLINKFILE}
    NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
else
    if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
        # determine top level extracted directory name for $DIR_TMP/${NGX_PAGESPEEDLINKFILE}
        NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
fi

cecho "Check for pagespeed PSOL library" $boldyellow
if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
    if [[ ! -f "$DIR_TMP/$NGXPGSPEED_DIR/${NGX_PAGESPEEDPSOLINKLFILE}" ]]; then
        nginxpgspeedtarball
    fi
fi

cecho "Check for gperf tools + libunwind download files" $boldyellow
if [[ ! -f "${DIR_TMP}/${LIBUNWIND_LINKFILE}" || ! -d "$DIR_TMP/$LIBUNWIND_LINKDIR" ]] || [[ ! -f "${DIR_TMP}/${GPERFTOOL_LINKFILE}" || ! -d "$DIR_TMP/$GPERFTOOL_LINKDIR" ]]; then
    if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
        gperftools
    fi
elif [[ ! -f "${DIR_TMP}/${LIBUNWIND_LINKFILE}" ]] || [[ ! -f "${DIR_TMP}/${GPERFTOOL_LINKFILE}" ]]; then
    if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
        gperftools
    fi
fi

if [[ "$NGINX_OPENRESTY" = [yY] ]]; then
    # nginx upgrade routine needs to know directory names for specific nginx
    # modules to know if they exist

    cecho "Check for openresty modules" $boldyellow    
    # if the tar.gz files don't exist first the tar test directory variables can not be populated
    if [[ ! -f "${DIR_TMP}/${NGX_MEMCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SRCACHELINKFILE}"|| ! -f "${DIR_TMP}/${NGX_REDISLINKFILE}" || ! -f "${DIR_TMP}/${NGX_ECHOLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SETMISCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_DEVELKITLINKFILE}" ]]; then
        openrestytarball
    fi 
    
    MEMCDIR=$(tar -tzf "$DIR_TMP/${NGX_MEMCLINKFILE}" | head -1 | cut -f1 -d"/")
    SRCACHEDIR=$(tar -tzf "$DIR_TMP/${NGX_SRCACHELINKFILE}" | head -1 | cut -f1 -d"/")
    SETMISCDIR=$(tar -tzf "$DIR_TMP/${NGX_SETMISCLINKFILE}" | head -1 | cut -f1 -d"/")
    DEVELKITDIR=$(tar -tzf "$DIR_TMP/${NGX_DEVELKITLINKFILE}" | head -1 | cut -f1 -d"/")
    ECHODIR=$(tar -tzf "$DIR_TMP/${NGX_ECHOLINKFILE}" | head -1 | cut -f1 -d"/")
    REDISDIR=$(tar -tzf "$DIR_TMP/${NGX_REDISLINKFILE}" | head -1 | cut -f1 -d"/")

    if [[ ! -f "${DIR_TMP}/${NGX_MEMCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SRCACHELINKFILE}"|| ! -f "${DIR_TMP}/${NGX_REDISLINKFILE}" || ! -f "${DIR_TMP}/${NGX_ECHOLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SETMISCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_DEVELKITLINKFILE}" ]] || [[ ! -d "${DIR_TMP}/${MEMCDIR}" || ! -d "${DIR_TMP}/${SRCACHEDIR}"|| ! -d "${DIR_TMP}/${REDISDIR}" || ! -d "${DIR_TMP}/${ECHODIR}" || ! -d "${DIR_TMP}/${SETMISCDIR}" || ! -d "${DIR_TMP}/${DEVELKITDIR}" ]]; then
        openrestytarball
    fi

    # ORESTY_LUANGINX=y|n
    if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
        NGX_LUAGITLINKDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAGITLINKFILE}" | head -1 | cut -f1 -d"/")
        if [[ ! -f "${DIR_TMP}/${NGX_LUANGINXLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAGITLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAMEMCACHEDLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAMYSQLLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAREDISLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUADNSLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPLOADLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAWEBSOCKETLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALOCKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUASTRINGLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAREDISPARSERLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMCHECKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALRUCACHELINKFILE}"  || ! -f "${DIR_TMP}/${NGX_LUARESTYCORELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALOGGERSOCKETLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUACOOKIELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMCACHELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUACJSONLINKFILE}" ]] || [[ ! -d "${DIR_TMP}/${NGX_LUANGINXDIR}" || ! -d "${DIR_TMP}/${NGX_LUAGITLINKDIR}" || ! -d "${DIR_TMP}/${NGX_LUAMEMCACHEDDIR}" || ! -d "${DIR_TMP}/${NGX_LUAMYSQLDIR}" || ! -d "${DIR_TMP}/${NGX_LUAREDISDIR}" || ! -d "${DIR_TMP}/${NGX_LUADNSDIR}" || ! -d "${DIR_TMP}/${NGX_LUAUPLOADDIR}" || ! -d "${DIR_TMP}/${NGX_LUAWEBSOCKETDIR}" || ! -d "${DIR_TMP}/${NGX_LUALOCKDIR}" || ! -d "${DIR_TMP}/${NGX_LUASTRINGDIR}" || ! -d "${DIR_TMP}/${NGX_LUAREDISPARSERDIR}" || ! -d "${DIR_TMP}/${NGX_LUAUPSTREAMCHECKDIR}" || ! -d "${DIR_TMP}/${NGX_LUALRUCACHEDIR}"  || ! -d "${DIR_TMP}/${NGX_LUARESTYCOREDIR}" || ! -d "${DIR_TMP}/${NGX_LUAUPSTREAMDIR}" || ! -d "${DIR_TMP}/${NGX_LUALOGGERSOCKETDIR}" || ! -d "${DIR_TMP}/${NGX_LUACOOKIEDIR}" || ! -d "${DIR_TMP}/${NGX_LUAUPSTREAMCACHEDIR}" || ! -d "${DIR_TMP}/${NGX_LUACJSONDIR}" ]]; then            

            openrestytarball
        fi
    fi
fi

if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
    # if option to download official github based master ngx_pagespeed
    # remove old version downloaded & download master tarball instead
    cd "$DIR_TMP"
    rm -rf release-${NGXPGSPEED_VER}*
    nginxpgspeedtarball
fi

}

function funct_mktempfile {

if [[ ! -d "$DIR_TMP"/msglogs ]]; then
cd $DIR_TMP
mkdir msglogs
chmod 1777 $DIR_TMP/msglogs
fi

TMP_MSGFILE="$DIR_TMP/msglogs/$RANDOM.msg"

}

clear_ps() {
    if [ -d /var/ngx_pagespeed_cache ]; then
        rm -rf /var/ngx_pagespeed_cache/*
    fi
}

function tools_nginxupgrade {

checkmap

cecho "**********************************************************************" $boldyellow
cecho "* tools/nginxupdate.sh - unattended nginx updater" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE - $COPYRIGHT" $boldgreen
cecho "**********************************************************************" $boldyellow

echo " "
nukey=y

if [[ "$nukey" = [nN] ]];
then
    exit 0
fi

# DIR_TMP="/svr-setup"
if [ ! -d "$DIR_TMP" ]; then
mkdir $DIR_TMP
fi

funct_mktempfile

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
if [ ! -f /etc/init.d/nginx ]; then
    cp $CUR_DIR/init/nginx /etc/init.d/nginx
    chmod +x /etc/init.d/nginx
    chkconfig --levels 235 nginx on
fi
fi # CENTOS_SEVEN != 7

echo ""
# pass nginx version on command line
# i.e. tools/nginxupdate.sh 1.9.5
# ngver=$1

    # auto check if static compiled Nginx openssl version matches
    # the one defined in centmin.sh OPENSSL_VERSION variable
    # if doesn't match then auto recompile the statically set
    # OPENSSL_VERSION
    AUTOOPENSSLCHECK=$(nginx -V 2>&1 | grep -Eo "$OPENSSL_VERSION")
    if [[ "$AUTOOPENSSLCHECK" ]]; then
        recompileopenssl='n'
    else
        recompileopenssl='y'
    fi
    echo ""
    checkgeoip

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

# Backup Nginx CONF
if [ "$NGINXBACKUP" == 'y' ]; then
	nginxbackup
fi

# Backup ngx_pagespeed pagespeed.conf
if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
	if [[ -f /usr/local/nginx/conf/pagespeed.conf ]]; then
		pagespeedbackup
	fi
fi

# tasks for updated ngx_pagespeed module parity
pagespeeduptasks

    echo "*************************************************"
    cecho "* Updating nginx" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

    # nginx Modules / Prerequisites
	cecho "Installing nginx Modules / Prerequisites..." $boldgreen

checknginxmodules

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Upgrade Google Perftools" $boldgreen
    echo "*************************************************"

    if [[ "$(uname -m)" = 'x86_64' ]]; then
    # Install libunwind
    echo "Compiling libunwind..."
    if [ -s "${LIBUNWIND_LINKFILE}" ]; then
        cecho "libunwind ${LIBUNWIND_VERSION} Archive found, skipping download..." $boldgreen 
    else
        download_cmd "${LIBUNWIND_LINK}" $WGETRETRY
    fi

    tar xvzf "${LIBUNWIND_LINKFILE}"
    cd "libunwind-${LIBUNWIND_VERSION}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure
    make${MAKETHREADS}
    make install
    fi

    # Install google-perftools
    cd "$DIR_TMP"

    echo "Compiling google-perftools..."
    if [ -s "${GPERFTOOL_LINKFILE}" ]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} Archive found, skipping download..." $boldgreen
    else
        download_cmd "${GPERFTOOL_LINK}" $WGETRETRY
    fi

    tar xvzf "${GPERFTOOL_LINKFILE}"
    cd "${GPERFTOOL_LINKDIR}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    if [[ "$(uname -m)" = 'x86_64' ]]; then
        ./configure --with-tcmalloc-pagesize=$TCMALLOC_PAGESIZE
    else
        ./configure --enable-frame-pointers --with-tcmalloc-pagesize=$TCMALLOC_PAGESIZE
    fi
    make${MAKETHREADS}
    make install
    if [ ! -f /etc/ld.so.conf.d/wget.conf ]; then
        echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
        /sbin/ldconfig
    fi

fi # GPERFTOOL_SOURCEINSTALL

# echo ""
# read -ep "Do you want to recompile OpenSSL ? Only needed if you updated OpenSSL version in centmin.sh [y/n]: " recompileopenssl 
# echo ""

if [[ "$recompileopenssl" = [yY] || "$LIBRESSL_SWITCH" = [yY] ]]; then
    installopenssl
fi # recompileopenssl

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"

    # Install PCRE
    cd $DIR_TMP

    echo "Compiling PCRE..."
    if [ -s pcre-${PCRE_VERSION}.tar.gz ]; then
        cecho "pcre ${PCRE_VERSION} Archive found, skipping download..." $boldgreen
    else
        download_cmd ${PCRE_SOURCELINK} $WGETRETRY
    fi

    tar xvzf pcre-${PCRE_VERSION}.tar.gz
    cd pcre-${PCRE_VERSION}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure
    make${MAKETHREADS}
    make install

fi

luajitinstall

funct_nginxmodules

check_requestscheme

    # Install nginx
    cd $DIR_TMP

    CUR_NGINXVER=$(nginx -v 2>&1 | awk -F '\\/' '{print $2}' |sed -e 's|\.|0|g' | head -n1)
    CUR_NGINXUPGRADEVER=$(echo $ngver |sed -e 's|\.|0|g' | head -n1)    

if [[ "$NGINXPATCH" = [nN] || "$NGINX_HTTP2" = [nN] ]]; then
    # if centmin.sh option NGINXPATCH=n then disable patches by 
    # wiping the nginx downloaded source and redownloading a fresh copy
    rm -rf nginx-${ngver}*
fi

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
    # if existing Nginx server is detected to have HTTP/2 patch compiled, then
    # wipe nginx download source and redownload a fresh copy to ensure you're
    # patching with latest patch http://nginx.org/patches/http2/ available
    rm -rf nginx-${ngver}*
fi

    echo "Compiling nginx..."
    if [ -s nginx-${ngver}.tar.gz ]; then
        cecho "nginx ${ngver} Archive found, skipping download..." $boldgreen
    else
        download_cmd "http://nginx.org/download/nginx-${ngver}.tar.gz" $WGETRETRY
    fi

    tar xvfz nginx-${ngver}.tar.gz
    cd nginx-${ngver}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi

# set_intelflags

if [[ "$NGINXPATCH" = [yY] ]]; then
    echo "*************************************************"
    cecho "Nginx Patch Time - $NGINXPATCH_DELAY seconds delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    patchnginx
fi

funct_nginxconfigure

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx configure failed\n"
        exit
	else
    	echo -e "\n`date`\nSuccess: Nginx configure ok\n"
	fi

# error check
################

    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        time make${MAKETHREADS}
    else
        time make
    fi

    if [[ "$STRIPNGINX" = [yY] ]]; then
        echo
        echo "strip nginx binary..."
        ls -lah objs/nginx
        strip -s objs/nginx
        ls -lah objs/nginx
        echo
    fi

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx make failed\n"
    	exit
	else
	   echo -e "\n`date`\nSuccess: Nginx make ok\n"
	fi

# error check
################

# cmservice nginx stop
/usr/local/sbin/nginx -s stop >/dev/null 2>&1

    # speed up nginx wait time if not many vhosts are on server
    if [[ "$(ls /usr/local/nginx/conf/conf.d/ | wc -l)" -le 5 ]]; then
        NGINXUPGRADESLEEP=4
    fi

# sleep $NGINXUPGRADESLEEP

sleep 3
NGINXPSCHECK=`ps --no-heading -C nginx`

if [ ! -z "$NGINXPSCHECK" ]; then
echo ""
echo "nginx seems to be still running, trying to stop it again..."
echo ""
# /etc/init.d/nginx stop and suppress any error messages
/usr/local/sbin/nginx -s stop >/dev/null 2>&1
sleep $NGINXUPGRADESLEEP
fi

    time make install

if [[ "$CLANG" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CCACHE_CPP2
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
fi        

# unset_intelflags

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx wasn't installed properly\n"
    	exit
	else
    	echo -e "\n`date`\nSuccess: Nginx was installed properly\n"

    if [[ "$NGINX_HTTP2" = [yY] ]] && [[ "$NGX_VEREVAL" -ge '10903' ]]; then
        # only apply auto vhost changes forNginx HTTP/2 
        # if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
        if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]]; then
            for v in "$(ls /usr/local/nginx/conf/conf.d/*.conf)"; do echo "$v"; egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v; echo "---"; sed -i 's|ssl spdy|ssl http2|g' $v; sed -i 's|  spdy_headers_comp|  #spdy_headers_comp|g' $v; sed -i 's|  add_header Alternate-Protocol|  #add_header Alternate-Protocol|g' $v; egrep -n 'ssl http2|spdy_headers_comp|Alternate-Protocol' $v;done
        fi
        if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
            sed -i 's|ssl spdy|ssl http2|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|spdy_headers_comp|#spdy_headers_comp|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|add_header Alternate-Protocol|#add_header Alternate-Protocol|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
        fi
    elif [[ "$NGINX_HTTP2" = [nN] || "$NGINX_SPDY" = [yY] ]]; then
        for v in "$(ls /usr/local/nginx/conf/conf.d/*.conf)"; do echo "$v"; egrep -n 'ssl http2|#spdy_headers_comp|#Alternate-Protocol' $v; egrep -n 'ssl spdy|spdy_headers_comp' $v; echo "---"; sed -i 's|ssl http2|ssl spdy|g' $v; sed -i 's|  #spdy_headers_comp|  spdy_headers_comp|g' $v; sed -i 's|  #add_header Alternate-Protocol|  add_header Alternate-Protocol|g' $v; egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v;done
        if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
            sed -i 's|ssl http2|ssl spdy|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#spdy_headers_comp|spdy_headers_comp|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#add_header Alternate-Protocol|add_header Alternate-Protocol|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
        fi        
    fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            systemctl daemon-reload
        fi
        # empty pagespeed cache
        clear_ps
        /etc/init.d/nginx start

        # cecho "Checking OpenSSL version used by Nginx..." $boldyellow
        # SSLIB=$(ldd `which nginx` | grep ssl | awk '{print $3}')
        # OPENSSLVER_CHECK=$(strings $SSLIB | grep "^OpenSSL ")
        # echo $OPENSSLVER_CHECK

        CBODYCHECK=`grep 'client_body_in_file_only on' /usr/local/nginx/conf/nginx.conf`
        if [ $CBODYCHECK ]; then
            sed -i 's/client_body_in_file_only on/client_body_in_file_only off/g' /usr/local/nginx/conf/nginx.conf
        fi

        geoinccheck
        geoipphp

        nginx -V

        echo "*************************************************"
        cecho "* nginx updated" $boldgreen
        echo "*************************************************"
	fi

# error check
################
}

#################
starttime=$(TZ=UTC date +%s.%N)
{
    if [ "$1" ]; then
        tools_nginxupgrade
    else
        echo
        echo " you need to pass nginx version number on command line i.e."
        echo " $0 1.11.5"
        echo
    fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginxupdate.sh.log
endtime=$(TZ=UTC date +%s.%N)
        INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
        echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginxupdate.sh.log
        echo "Total Nginx Update Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginxupdate.sh.log
        tail -1 "${CENTMINLOGDIR}/$(ls -Art ${CENTMINLOGDIR}/ | grep 'nginxupdate.sh.log' | tail -1)"