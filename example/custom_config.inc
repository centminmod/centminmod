# CENTMINMOD PERSISTENT CUSTOM CONFIGURATIONS OPTIONS FILE
# PLEASE NOTE:  This file serves as a file with all the possible options, NOT 
# what should be used in a production enviornment!!!!
# https://centminmod.com
# Creating a custom_config.inc file it will override default setting for 
# upgrades and software installs set in centmin.sh.
# Persistent config outlined at http://centminmod.com/upgrade.html#persistent
# Location:  /etc/centminmod/custom_config.inc

EMAIL=''          # Server notification email address enter only 1 address
PUSHOVER_EMAIL='' # Signup pushover.net push email notifications to mobile & tablets

CMGIT='https://github.com/centminmod/centminmod.git'
AUTO_GITUPDATE='n'

ENABLEMOTD_CSFMSG='y'         # motd banner csf message alert disaply
ENABLEMOTD_LINKSMSG='y'       # motd centminmod.com links bookmark alert display

# Clang & GCC Compilers
CLANG='y'                     # Nginx and LibreSSL
CLANG_PHP='n'                 # PHP
CLANG_APC='n'                 # APC Cache
CLANG_MEMCACHED='n'           # Memcached menu option 10 routine
PHP_PGO='n'                   # Profile Guided Optimization https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php
PHP_PGO_CENTOSSIX='n'         # CentOS 6 may need GCC >4.4.7 fpr PGO so use devtoolset-4 GCC 5.2
NGINX_DEVTOOLSETGCC='n'       # Use devtoolset-4 GCC 5.2 even for CentOS 7 nginx compiles
GENERAL_DEVTOOLSETGCC='n'     # Use devtoolset-4 GCC 5.2 whereever possible/coded
CRYPTO_DEVTOOLSETGCC='n'      # Use devtoolset-4 GCC 5.2 for libressl or openssl compiles

# Nginx Dynamic Module Switches
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
NGXDYNAMIC_LUA='n'          # leave disabled due to bug https://github.com/openresty/lua-nginx-module/issues/715
NGXDYNAMIC_SRCCACHE='n'
NGXDYNAMIC_DEVELKIT='n'     # leave disabled as it requires lua nginx module as dynamic but it has a bug in lua nginx
NGXDYNAMIC_MEMC='n'
NGXDYNAMIC_REDISTWO='n'
NGXDYNAMIC_NGXPAGESPEED='n'
NGXDYNAMIC_BROTLI='y'
NGXDYNAMIC_FANCYINDEX='y'
NGXDYNAMIC_HIDELENGTH='y'

# Maintenance Mode (will run maintenance mode during these upgrades)
# set = y to put nginx, php and mariadb major version updates into 503 
# maintenance mode https://community.centminmod.com/posts/26485/
NGINX_UPDATEMAINTENANCE='n'
PHP_UPDATEMAINTENANCE='n'
MARIADB_UPDATEMAINTENANCE='n'

# Nginx
NGINX_VERSION='1.11.8'       # Use this version of Nginx
NGINX_VHOSTSSL='y'           # enable centmin.sh menu 2 prompt to create self signed SSL vhost 2nd vhost conf
NGINXBACKUP='y'
NGINX_IPV='n'                #NGINX IPV6 compile support for unattended mode only
NGINXUPGRADESLEEP='3'
NSD_INSTALL='y'              # Install NSD (DNS Server)
NSD_VERSION='3.2.18'         # NSD Version
NTP_INSTALL='y'              # Install Network time protocol daemon
NGINXPATCH='y'               # Set to y to allow NGINXPATCH_DELAY seconds time before Nginx configure and patching Nginx
NGINXPATCH_DELAY='1'         # Number of seconds to pause Nginx configure routine during Nginx upgrades
STRIPNGINX='y'               # set 'y' to strip nginx binary to reduce size
NGXMODULE_ALTORDER='y'       # nginx configure module ordering alternative order
NGINX_ZERODT='n'             # nginx zero downtime reloading on nginx upgrades
NGINX_INSTALL='y'            # Install Nginx (Webserver)
NGINX_DEBUG='n'              # Enable & reinstall Nginx debug log nginx.org/en/docs/debugging_log.html & wiki.nginx.org/Debugging
NGINX_HTTP2='y'              # Nginx http/2 patch https://community.centminmod.com/threads/4127/
NGINX_ZLIBNG='n'             # 64bit OS only for Nginx compiled against zlib-ng https://github.com/Dead2/zlib-ng
NGINX_MODSECURITY='n'        # modsecurity module support https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#Installation_for_NGINX
NGINX_RDNS='n'               # https://github.com/flant/nginx-http-rdns
NGINX_NJS='n'                # nginScript https://www.nginx.com/blog/launching-nginscript-and-looking-ahead/
NGINX_GEOIP='y'              # Nginx GEOIP module install
NGINX_GEOIPMEM='y'           # Nginx caches GEOIP databases in memory (default), setting 'n' caches to disk instead
NGINX_SPDY='y'               # Nginx SPDY support
NGINX_SPDYPATCHED='n'        # Cloudflare HTTP/2 + SPDY patch https://github.com/cloudflare/sslconfig/blob/master/patches/nginx__http2_spdy.patch
NGINX_STUBSTATUS='y'         # http://nginx.org/en/docs/http/ngx_http_stub_status_module.html required for nginx statistics
NGINX_SUB='y'                # http://nginx.org/en/docs/http/ngx_http_sub_module.html
NGINX_ADDITION='y'           # http://nginx.org/en/docs/http/ngx_http_addition_module.html
NGINX_IMAGEFILTER='y'        # http://nginx.org/en/docs/http/ngx_http_image_filter_module.html
NGINX_PERL='n'               # http://nginx.org/en/docs/http/ngx_http_perl_module.html
NGINX_XSLT='n'               # http://nginx.org/en/docs/http/ngx_http_xslt_module.html
NGINX_LENGTHHIDE='n'         # https://github.com/nulab/nginx-length-hiding-filter-module
NGINX_LENGTHHIDEGIT='y'      # triggers only if NGINX_LENGTHHIDE='y'
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
NGINX_FANCYINDEX='y'         # http://wiki.nginx.org/NgxFancyIndex
NGINX_FANCYINDEXVER='0.4.0'  # https://github.com/aperezdc/ngx-fancyindex/releases
NGINX_VHOSTSTATS='y'         # https://github.com/vozlt/nginx-module-vts
NGINX_LIBBROTLI='n'          # https://github.com/google/ngx_brotli
NGINX_LIBBROTLISTATIC='n'
NGINX_PAGESPEED='n'          # Install ngx_pagespeed
NGINX_PAGESPEEDGITMASTER='n' # Install ngx_pagespeed from official github master instead  
NGXPGSPEED_VER='1.12.34.2-beta'
NGINX_PAGESPEEDPSOL_VER='1.12.34.2'
NGINX_PASSENGER='n'          # Install Phusion Passenger requires installing addons/passenger.sh before hand
NGINX_WEBDAV='n'             # Nginx WebDAV and nginx-dav-ext-module
NGINX_EXTWEBDAVVER='0.0.3'   # nginx-dav-ext-module version
NGINX_LIBATOMIC='y'          # Nginx configured with libatomic support
NGINX_HTTPREDIS='y'          # Nginx redis http://wiki.nginx.org/HttpRedisModule
NGINX_HTTPREDISVER='0.3.7'   # Nginx redis version
NGINX_PCREJIT='y'            # Nginx configured with pcre & pcre-jit support
NGINX_PCREVER='8.39'         # Version of PCRE used for pcre-jit support in Nginx
ORESTY_HEADERSMORE='y'       # openresty headers more https://github.com/openresty/headers-more-nginx-module
ORESTY_HEADERSMOREGIT='n'    # use git master instead of version specific
NGINX_HEADERSMORE='0.32'
NGINX_CACHEPURGEVER='2.3'
NGINX_STICKY='n'             # nginx sticky module https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
NGINX_STICKYVER='master'
NGINX_UPSTREAMCHECK='n'      # nginx upstream check https://github.com/yaoweibin/nginx_upstream_check_module
NGINX_UPSTREAMCHECKVER='0.3.0'
NGINX_OPENRESTY='y'          # Agentzh's openresty Nginx modules
ORESTY_MEMCVER='0.17'        # openresty memc module https://github.com/openresty/memc-nginx-module
ORESTY_SRCCACHEVER='0.31'    # openresty subrequest cache module https://github.com/openresty/srcache-nginx-module
ORESTY_DEVELKITVER='0.3.0'  # openresty ngx_devel_kit module https://github.com/simpl/ngx_devel_kit
ORESTY_SETMISCGIT='n'        # use git master instead of version specific
ORESTY_SETMISCVER='0.31'     # openresty set-misc-nginx module https://github.com/openresty/set-misc-nginx-module
ORESTY_ECHOGIT='n'           # use git master instead of version specific
ORESTY_ECHOVER='0.60'        # openresty set-misc-nginx module https://github.com/openresty/echo-nginx-module
ORESTY_REDISVER='0.13'       # openresty redis2-nginx-module https://github.com/openresty/redis2-nginx-module

LUAJIT_GITINSTALL='y'        # opt to install luajit 2.1 from dev branch http://repo.or.cz/w/luajit-2.0.git/shortlog/refs/heads/v2.1
LUAJIT_GITINSTALLVER='2.1'   # branch version = v2.1 will override ORESTY_LUAGITVER if LUAJIT_GITINSTALL='y'

ORESTY_LUANGINX='n'             # enable or disable or ORESTY_LUA* nginx modules below
ORESTY_LUANGINXVER='0.10.7'     # openresty lua-nginx-module https://github.com/openresty/lua-nginx-module
ORESTY_LUAGITVER='2.0.4'        # luagit http://luajit.org/
ORESTY_LUAMEMCACHEDVER='0.14'   # openresty https://github.com/openresty/lua-resty-memcached
ORESTY_LUAMYSQLVER='0.17'       # openresty https://github.com/openresty/lua-resty-mysql
ORESTY_LUAREDISVER='0.26'       # openresty https://github.com/openresty/lua-resty-redis
ORESTY_LUADNSVER='0.18'         # openresty https://github.com/openresty/lua-resty-dns
ORESTY_LUAUPLOADVER='0.10'      # openresty https://github.com/openresty/lua-resty-upload
ORESTY_LUAWEBSOCKETVER='0.05'   # openresty https://github.com/openresty/lua-resty-websocket
ORESTY_LUALOCKVER='0.04'        # openresty https://github.com/openresty/lua-resty-lock
ORESTY_LUASTRINGVER='0.09'      # openresty https://github.com/openresty/lua-resty-string
ORESTY_LUAREDISPARSERVER='0.10'    # openresty https://github.com/openresty/lua-redis-parser
ORESTY_LUAUPSTREAMCHECKVER='0.04'  # openresty https://github.com/openresty/lua-resty-upstream-healthcheck
ORESTY_LUALRUCACHEVER='0.04'       # openresty https://github.com/openresty/lua-resty-lrucache
ORESTY_LUARESTYCOREVER='0.1.9'     # openresty https://github.com/openresty/lua-resty-core
ORESTY_LUAUPSTREAMVER='0.06'       # openresty https://github.com/openresty/lua-upstream-nginx-module
NGX_LUAUPSTREAM='n'                # disable https://github.com/openresty/lua-upstream-nginx-module
ORESTY_LUALOGGERSOCKETVER='0.1'    # cloudflare openresty https://github.com/cloudflare/lua-resty-logger-socket
ORESTY_LUACOOKIEVER='master'       # cloudflare openresty https://github.com/cloudflare/lua-resty-cookie
ORESTY_LUAUPSTREAMCACHEVER='0.1.1' # cloudflare openresty https://github.com/cloudflare/lua-upstream-cache-nginx-module
NGX_LUAUPSTREAMCACHE='n'           # disable https://github.com/cloudflare/lua-upstream-cache-nginx-module
LUACJSONVER='2.1.0.4'              # https://github.com/openresty/lua-cjson

STRIPPHP='y'                 # set 'y' to strip PHP binary to reduce size
PHP_INSTALL='y'              # Install PHP /w Fast Process Manager
PHP_CUSTOMSSL='n'            # compile php-fpm against openssl 1.0.2+ or libressl 2.3+ whichever nginx uses
PHPMAKETEST='n'              # set to y to enable make test after PHP make for diagnostic purposes
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
PHPMSSQL='n'                 # Disable or Enable MSSQL server PHP extension
SHORTCUTS='y'                # shortcuts

##################################
## Nginx SSL options
# OpenSSL
NOSOURCEOPENSSL='y'        # set to 'y' to disable OpenSSL source compile for system default YUM package setup
OPENSSL_VERSION='1.0.2j'   # Use this version of OpenSSL http://openssl.org/
OPENSSL_VERSIONFALLBACK='1.0.2j'   # fallback if OPENSSL_VERSION uses openssl 1.1.x branch
OPENSSL_CUSTOMPATH='/opt/openssl'  # custom directory path for OpenSSL 1.0.2+
CLOUDFLARE_PATCHSSL='y'    # set 'y' to implement Cloudflare's chacha20 patch https://github.com/cloudflare/sslconfig
NGINX_DYNAMICTLS='n'       # set 'y' and recompile nginx https://blog.cloudflare.com/optimizing-tls-over-tcp-to-reduce-latency/

# LibreSSL
LIBRESSL_SWITCH='y'        # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
LIBRESSL_VERSION='2.4.4'   # Use this version of LibreSSL http://www.libressl.org/

# BoringSSL
BORINGSSL_SWITCH='n'       # if set to 'y' it overrides OpenSSL as the default static compiled option for Nginx server
##################################

# Choose whether to compile Nginx --with-google_perftools_module
# no longer used in Centmin Mod v1.2.3-eva2000.01 and higher
GPERFTOOLS_SOURCEINSTALL='n'
LIBUNWIND_VERSION='1.2-rc1'     # note google perftool specifically requies v0.99 and no other
GPERFTOOLS_VERSION='2.5'     # Use this version of google-perftools

# Choose whether to compile PCRE from source. Note PHP 5.3.8 already includes PCRE
PCRE_SOURCEINSTALL='n'     
PCRE_VERSION='8.39'          # PCRE version

# PHP and Cache/Acceleration
IMAGICKPHP_VER='3.4.3RC2'   # PHP extension for imagick
MAILPARSEPHP_VER='2.1.6'    # https://pecl.php.net/package/mailparse
MAILPARSEPHP_COMPATVER='3.0.1' # For PHP 7
MEMCACHED_INSTALL='y'       # Install Memcached
LIBEVENT_VERSION='2.0.22'   # Use this version of Libevent
MEMCACHED_VERSION='1.4.34'  # Use this version of Memcached server
MEMCACHE_VERSION='3.0.8'    # Use this version of Memcache
MEMCACHEDPHP_VER='2.2.0'    # Memcached PHP extension not server
LIBMEMCACHED_YUM='y'        # switch to YUM install instead of source compile
LIBMEMCACHED_VER='1.0.18'   # libmemcached version for source compile
TWEMPERF_VER='0.1.1'
PHPREDIS='y'                # redis PHP extension install
REDISPHP_VER='3.1.1RC2'        # redis PHP version
REDISPHPSEVEN_VER='3.1.1RC2'   # redis PHP version for PHP =>7.x
REDISPHP_GIT='n'            # pull php 7 redis extension from git or pecl downloads
PHPMONGODB='n'              # MongoDB PHP extension install
MONGODBPHP_VER='1.1.8'      # MongoDB PHP version
MONGODB_SASL='n'            # SASL not working yet leave = n
PDOPGSQL_PHPVER='9.5'       # pdo-pgsql PHP extension version for postgresql

PHP_FTPEXT='y'              # ftp PHP extension
PHP_MEMCACHE='y'            # memcache PHP extension 
PHP_MEMCACHED='y'           # memcached PHP extension
FFMPEGVER='0.6.0'
SUHOSINVER='0.9.37.1'
PHP_VERSION='5.6.29'        # Use this version of PHP
PHP_MIRRORURL='http://php.net'
PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
XCACHE_VERSION='3.2.0'      # Use this version of Xcache
APCCACHE_VERSION='3.1.13'   # Use this version of APC Cache
IGBINARY_VERSION='1.2.1'
IGBINARY_INSTALL='y'        # install or not igbinary support for APC and Memcached server
IGBINARYGIT='y'
ZOPCACHEDFT='y'
ZOPCACHECACHE_VERSION='7.0.5'   # for PHP <=5.4 http://pecl.php.net/package/ZendOpcache
ZOPCACHE_OVERRIDE='n'           # ='y'ill override PHP 5.5, 5.6, 7.0 inbuilt Zend OpCache version
# Python
PYTHON_VERSION='2.7.10'     # Use this version of Python
SIEGE_VERSION='4.0.2'

# custom curl/libcurl RPM for 7.44 and higher
# enable with CUSTOM_CURLRPM='y' use at own risk as it can break the system
# info at http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring/
CUSTOM_CURLRPM='n'
CUSTOM_CURLRPMVER='7.47.1-2.0'       # custom curl/libcurl version
CUSTOM_CURLLIBSSHVER='1.6.0-4.0'     # libssh2 version
CUSTOM_CURLRPMCARESVER='1.10.0-6.0'  # c-ares version
CUSTOM_CURLRPMSYSURL='http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring'
CUSTOM_CURLRPMLIBURL='http://mirror.city-fan.org/ftp/contrib/libraries'

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
# Letsencrypt integration via addons/acmetool.sh auto detection
# in centmin.sh menu option 2, 22, and /usr/bin/nv nginx vhost
# generators. You can control whether or not to enable or disable
# integration detection in these menu options
LETSENCRYPT_DETECT='n'
###############################################################

# When set to ='y'will disable those listed installed services
# by default. The service is still installed but disabled
# by default and can be re-enabled with commands:
# service servicename start; chkconfig servicename on
# From http://centminmod.com/faq.html Question #25
NSD_DISABLED='n'              # when set to ='y'NSD disabled by default with chkconfig off
MEMCACHED_DISABLED='n'        # when set to ='y' Memcached server disabled by default via chkconfig off
PHP_DISABLED='n'              # when set to ='y' PHP-FPM disabled by default with chkconfig off
MYSQLSERVICE_DISABLED='n'     # when set to ='y' MariaDB MySQL service disabled by default with chkconfig off
PUREFTPD_DISABLED='n'         # when set to ='y'Pure-ftpd service disabled by default with chkconfig off  