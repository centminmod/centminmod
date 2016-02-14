# Download links pertaining to inc/downloads.inc file
# some download links have an alternate local mirror link for emergencies 
# when the primary download link is down

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
        RPMFARCH='x86_64'
    else
        ARCH='i686'
        RPMFARCH='i386'
    fi

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

CCACHEFILE="ccache-${CCACHE_VER}.tar.gz"
CCCACHELINK="http://samba.org/ftp/ccache/${CCACHEFILE}"
CCCACHELINKLOCAL="http://centminmod.com/centminmodparts/ccache/${CCACHEFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSFIVEEPELFILE="epel-release-5-4.noarch.rpm"
CENTOSFIVEEPEL="http://mirror.sfo12.us.leaseweb.net/epel/5/${RPMFARCH}/${CENTOSFIVEEPELFILE}"

CENTOSFIVERPMFORGEFILE="rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm"
CENTOSFIVERPMFORGE="http://pkgs.repoforge.org/rpmforge-release/${CENTOSFIVERPMFORGEFILE}"

CENTOSFIVECENTALTFILE="centalt-release-5-3.noarch.rpm"
CENTOSFIVECENTALT="http://centos.alt.ru/repository/centos/5/${RPMFARCH}/${CENTOSFIVECENTALTFILE}"

CENTOSFIVEPERCONARELEASEFILE="percona-release-0.1-3.noarch.rpm"
CENTOSFIVEPERCONARELEASE="http://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSFIVEPERCONARELEASEFILE}"

CENTOSFIVEVARNISHFILE='varnish-release-3.0-1.el5.centos.noarch.rpm'
CENTOSFIVEVARNISH="http://repo.varnish-cache.org/redhat/varnish-3.0/el5/noarch/varnish-release/${CENTOSFIVEVARNISHFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSSIXEPELFILE="epel-release-6-8.noarch.rpm"
CENTOSSIXEPEL="http://mirror.sfo12.us.leaseweb.net/epel/6/${RPMFARCH}/${CENTOSSIXEPELFILE}"
CENTOSSIXEPELLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSIXEPELFILE}"

CENTOSSIXRPMFORGEFILE="rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm"
CENTOSSIXRPMFORGE="http://pkgs.repoforge.org/rpmforge-release/${CENTOSSIXRPMFORGEFILE}"

CENTOSSIXCENTALTFILE="centalt-release-6-1.noarch.rpm"
CENTOSSIXCENTALT="http://centos.alt.ru/repository/centos/6/${RPMFARCH}/${CENTOSSIXCENTALTFILE}"
CENTOSSIXCENTALTLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSIXCENTALTFILE}"

CENTOSSIXPERCONALRELEASEFILE="percona-release-0.1-3.noarch.rpm"
CENTOSSIXPERCONALRELEASE="http://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSSIXPERCONALRELEASEFILE}"

CENTOSSIXVARNISHFILE="varnish-release-3.0-1.el6.noarch.rpm"
CENTOSSIXVARNISH="http://repo.varnish-cache.org/redhat/varnish-3.0/el6/noarch/varnish-release/${CENTOSSIXVARNISHFILE}"
CENTOSSIXVARNISHLOCAL='http://centminmod.com/centminmodparts/repos/${CENTOSSIXVARNISHFILE}'

CENTOSSIXAXIVOFILE="axivo-release-6-1.noarch.rpm"
CENTOSSIXAXIVO="http://rpm.axivo.com/${CENTOSSIXAXIVOFILE}"
CENTOSSIXAXIVOLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSIXAXIVOFILE}"

CENTOSSIXREMIFILE="remi-release-6.rpm"
CENTOSSIXREMI="http://rpms.famillecollet.com/enterprise/${CENTOSSIXREMIFILE}"
CENTOSSIXREMILOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSIXREMIFILE}"

CENTOSSIXATRPMSFILE="atrpms-repo-6-7.el6.x86_64.rpm"
CENTOSSIXATRPMS="http://dl.atrpms.net/all/${CENTOSSIXATRPMSFILE}"
CENTOSSIXATRPMSLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSIXATRPMSFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSSEVENEPELFILE="epel-release-7-2.noarch.rpm"
CENTOSSEVENEPEL="http://mirror.sfo12.us.leaseweb.net/epel/7/${RPMFARCH}/e/${CENTOSSEVENEPELFILE}"
CENTOSSEVENEPEL_B="http://dl.fedoraproject.org/pub/epel/7/${RPMFARCH}/e/${CENTOSSEVENEPELFILE}"
CENTOSSEVENEPELLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSEVENEPELFILE}"

# only do dynamic mirror check if /etc/yum.repos.d/epel.repo and thus EPEL yum repo
# has not been setup and installed yet
if [ ! -f /etc/yum.repos.d/epel.repo ]; then
# dynamic mirror check for EPEL
if [[ "$(uname -m)" = 'x86_64' ]]; then
	if [[ "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then
	CENTOSSEVENEPEL_MIRRORFULLURL=$(lynx -dump -listonly http://download.fedoraproject.org/pub/epel/7/x86_64/repoview/epel-release.html | grep 'rpm' | awk -F " " '{print $2}')
	CENTOSSEVENEPEL_MIRRORURL=$(echo ${CENTOSSEVENEPEL_MIRRORFULLURL%/*}/)
	CENTOSSEVENEPEL_MIRRORURLFILE=$(echo ${CENTOSSEVENEPEL_MIRRORFULLURL##*/})
	CENTOSSEVENEPELFILE="$CENTOSSEVENEPEL_MIRRORURLFILE"
	CENTOSSEVENEPEL="$CENTOSSEVENEPEL_MIRRORFULLURL"
	fi
	
	if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' ]]; then
	CENTOSSIXEPEL_MIRRORFULLURL=$(lynx -dump -listonly http://download.fedoraproject.org/pub/epel/6/x86_64/repoview/epel-release.html | grep 'rpm' | awk -F " " '{print $2}')
	CENTOSSIXEPEL_MIRRORURL=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL%/*}/)
	CENTOSSIXEPEL_MIRRORURLFILE=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL##*/})
	CENTOSSIXEPELFILE="$CENTOSSIXEPEL_MIRRORURLFILE"
	CENTOSSIXEPEL="$CENTOSSIXEPEL_MIRRORFULLURL"
	fi
	
	if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' || "$CENTOSVER" == '5.11' || "$CENTOSVER" == '5.12' || "$CENTOSVER" == '5.13' || "$CENTOSVER" == '5.14' ]]; then
	CENTOSFIVEEPEL_MIRRORFULLURL=$(lynx -dump -listonly http://download.fedoraproject.org/pub/epel/5/x86_64/repoview/epel-release.html | grep 'rpm' | awk -F " " '{print $2}')
	CENTOSFIVEEPEL_MIRRORURL=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL%/*}/)
	CENTOSFIVEEPEL_MIRRORURLFILE=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL##*/})
	CENTOSFIVEEPELFILE="$CENTOSFIVEEPEL_MIRRORURLFILE"
	CENTOSFIVEEPEL="$CENTOSFIVEEPEL_MIRRORFULLURL"
	fi
else
	if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' ]]; then
	CENTOSSIXEPEL_MIRRORFULLURL=$(lynx -dump -listonly http://download.fedoraproject.org/pub/epel/6/i386/repoview/epel-release.html | grep 'rpm' | awk -F " " '{print $2}')
	CENTOSSIXEPEL_MIRRORURL=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL%/*}/)
	CENTOSSIXEPEL_MIRRORURLFILE=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL##*/})
	CENTOSSIXEPELFILE="$CENTOSSIXEPEL_MIRRORURLFILE"
	CENTOSSIXEPEL="$CENTOSSIXEPEL_MIRRORFULLURL"
	fi
	
	if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' || "$CENTOSVER" == '5.11' || "$CENTOSVER" == '5.12' || "$CENTOSVER" == '5.13' || "$CENTOSVER" == '5.14' ]]; then
	CENTOSFIVEEPEL_MIRRORFULLURL=$(lynx -dump -listonly http://download.fedoraproject.org/pub/epel/5/i386/repoview/epel-release.html | grep 'rpm' | awk -F " " '{print $2}')
	CENTOSFIVEEPEL_MIRRORURL=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL%/*}/)
	CENTOSFIVEEPEL_MIRRORURLFILE=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL##*/})
	CENTOSFIVEEPELFILE="$CENTOSFIVEEPEL_MIRRORURLFILE"
	CENTOSFIVEEPEL="$CENTOSFIVEEPEL_MIRRORFULLURL"
	fi
fi
fi # /etc/yum.repos.d/epel.repo file check

CENTOSSEVENRPMFORGEFILE="rpmforge-release-0.5.3-1.el7.rf.${ARCH}.rpm"
CENTOSSEVENRPMFORGE="http://pkgs.repoforge.org/rpmforge-release/${CENTOSSEVENRPMFORGEFILE}"

CENTOSSEVENCENTALTFILE="centalt-release-6-1.noarch.rpm"
CENTOSSEVENCENTALT="http://centos.alt.ru/repository/centos/6/${RPMFARCH}/${CENTOSSEVENCENTALTFILE}"
CENTOSSEVENCENTALTLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSEVENCENTALTFILE}"

CENTOSSEVENPERCONALRELEASEFILE="percona-release-0.1-3.noarch.rpm"
CENTOSSEVENPERCONALRELEASE="http://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSSEVENPERCONALRELEASEFILE}"

CENTOSSEVENVARNISHFILE="varnish-release-3.0-1.el6.noarch.rpm"
CENTOSSEVENVARNISH="http://repo.varnish-cache.org/redhat/varnish-3.0/el6/noarch/varnish-release/${CENTOSSEVENVARNISHFILE}"
CENTOSSEVENVARNISHLOCAL='http://centminmod.com/centminmodparts/repos/${CENTOSSEVENVARNISHFILE}'

CENTOSSEVENAXIVOFILE="axivo-release-7-1.noarch.rpm"
CENTOSSEVENAXIVO="http://rpm.axivo.com/${CENTOSSEVENAXIVOFILE}"
CENTOSSEVENAXIVOLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSEVENAXIVOFILE}"

CENTOSSEVENREMIFILE="remi-release-7.rpm"
CENTOSSEVENREMI="http://rpms.famillecollet.com/enterprise/${CENTOSSEVENREMIFILE}"
CENTOSSEVENREMILOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSEVENREMIFILE}"

CENTOSSEVENATRPMSFILE="atrpms-repo-6-7.el6.x86_64.rpm"
CENTOSSEVENATRPMS="http://dl.atrpms.net/all/${CENTOSSEVENATRPMSFILE}"
CENTOSSEVENATRPMSLOCAL="http://centminmod.com/centminmodparts/repos/${CENTOSSEVENATRPMSFILE}"

PCRELINKFILE="pcre-${NGINX_PCREVER}.tar.gz"
#PCRELINK="http://superb-dca2.dl.sourceforge.net/project/pcre/pcre/${NGINX_PCREVER}/${PCRELINKFILE}"
PCRELINK="http://centminmod.com/centminmodparts/pcre/${PCRELINKFILE}"
PCRELINKLOCAL="http://centminmod.com/centminmodparts/pcre/${PCRELINKFILE}"

# wget renamed
NGX_WEBDAVLINKFILE="nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz"
NGX_WEBDAVLINK="https://github.com/arut/nginx-dav-ext-module/archive/v${NGINX_EXTWEBDAVVER}.tar.gz"

# wget renamed
NGX_PAGESPEEDGITLINKFILE="release-${NGXPGSPEED_VER}.tar.gz"
NGX_PAGESPEEDGITLINK="https://github.com/pagespeed/ngx_pagespeed/archive/master.tar.gz"

NGX_PAGESPEEDLINKFILE="release-${NGXPGSPEED_VER}.tar.gz"
NGX_PAGESPEEDLINK="https://github.com/pagespeed/ngx_pagespeed/archive/${NGX_PAGESPEEDLINKFILE}"

NGX_PAGESPEEDPSOLINKLFILE="${NGINX_PAGESPEEDPSOL_VER}.tar.gz"
NGX_PAGESPEEDPSOLINKL="https://dl.google.com/dl/page-speed/psol/${NGX_PAGESPEEDPSOLINKLFILE}"

NGX_FANCYINDEXLINKFILE="ngx-fancyindex-0.3.6.tar.gz"
NGX_FANCYINDEXLINK="http://centminmod.com/centminmodparts/nginx/modules/${NGX_FANCYINDEXLINKFILE}"

NGX_CACHEPURGEFILE="ngx_cache_purge-${NGINX_CACHEPURGEVER}.tar.gz"
NGX_CACHEPURGE="http://centminmod.com/centminmodparts/nginx/modules/${NGX_CACHEPURGEFILE}"

NGX_ACCESSKEYLINKFILE="Nginx-accesskey-2.0.3.tar.gz"
NGX_ACCESSKEYLINK="http://centminmod.com/centminmodparts/nginx/modules/${NGX_ACCESSKEYLINKFILE}"

# wget renamed
NGX_CONCATLINKFILE="nginx-http-concat-master.tar.gz"
NGX_CONCATLINK='https://github.com/alibaba/nginx-http-concat/archive/master.tar.gz'

# wget renamed
NGX_HEADERSMORELINKFILE="headers-more-v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINK="https://github.com/agentzh/headers-more-nginx-module/archive/v${NGINX_HEADERSMORE}.tar.gz"

# wget renamed
NGX_STICKYLINKFILE="nginx-sticky-${NGINX_STICKYVER}.tar.gz"
NGX_STICKYLINK="https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng/get/${NGINX_STICKYVER}.tar.gz"

# wget renamed
NGX_UPSTREAMCHECKLINKFILE="nginx-upstream-check-${NGINX_UPSTREAMCHECKVER}.tar.gz"
NGX_UPSTREAMCHECKLINK="https://github.com/yaoweibin/nginx_upstream_check_module/archive/v${NGINX_UPSTREAMCHECKVER}.tar.gz"

# wget renamed
NGX_MEMCLINKFILE="memc-nginx-module_v${ORESTY_MEMCVER}.tar.gz"
NGX_MEMCLINK="https://github.com/agentzh/memc-nginx-module/tarball/v${ORESTY_MEMCVER}"

# wget renamed
NGX_SRCACHELINKFILE="srcache-nginx-module_v${ORESTY_SRCCACHEVER}.tar.gz"
NGX_SRCACHELINK="https://github.com/agentzh/srcache-nginx-module/tarball/v${ORESTY_SRCCACHEVER}"

# wget renamed
NGX_DEVELKITLINKFILE="nginx-develkit_v${ORESTY_DEVELKITVER}.tar.gz"
NGX_DEVELKITLINK="https://github.com/simpl/ngx_devel_kit/archive/v${ORESTY_DEVELKITVER}.tar.gz"

# wget renamed
NGX_SETMISCLINKFILE="set-misc-nginx-module_v${ORESTY_SETMISCVER}.tar.gz"
NGX_SETMISCLINK="https://github.com/openresty/set-misc-nginx-module/archive/v${ORESTY_SETMISCVER}.tar.gz"

# wget renamed
NGX_ECHOLINKFILE="echo-nginx-module_v${ORESTY_ECHOVER}.tar.gz"
NGX_ECHOLINK="https://github.com/openresty/echo-nginx-module/archive/v${ORESTY_ECHOVER}.tar.gz"

# wget renamed
NGX_REDISLINKFILE="redis2-nginx-module_v${ORESTY_REDISVER}.tar.gz"
NGX_REDISLINK="https://github.com/openresty/redis2-nginx-module/archive/v${ORESTY_REDISVER}.tar.gz"

# wget renamed
NGX_HTTPREDISLINKFILE="ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"
NGX_HTTPREDISLINK="http://people.freebsd.org/~osa/ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"

##############
# lua

NGX_LUANGINXLINKFILE="lua-nginx-module_v${ORESTY_LUANGINXVER}.tar.gz"
NGX_LUANGINXLINK="https://github.com/openresty/lua-nginx-module/archive/v${ORESTY_LUANGINXVER}.tar.gz"

NGX_LUAGITLINKFILE="luagit-${ORESTY_LUAGITVER}.tar.gz"
NGX_LUAGITLINK="http://luajit.org/download/LuaJIT-${ORESTY_LUAGITVER}.tar.gz"

NGX_LUAMEMCACHEDLINKFILE="lua-resty-memcached_v${ORESTY_LUAMEMCACHEDVER}.tar.gz"
NGX_LUAMEMCACHEDLINK="https://github.com/openresty/lua-resty-memcached/archive/v${ORESTY_LUAMEMCACHEDVER}.tar.gz"

NGX_LUAMYSQLLINKFILE="lua-resty-mysql_v${ORESTY_LUAMYSQLVER}.tar.gz"
NGX_LUAMYSQLLINK="https://github.com/openresty/lua-resty-mysql/archive/v${ORESTY_LUAMYSQLVER}.tar.gz"

NGX_LUAREDISLINKFILE="lua-resty-redis_v${ORESTY_LUAREDISVER}.tar.gz"
NGX_LUAREDISLINK="https://github.com/openresty/lua-resty-redis/archive/v${ORESTY_LUAREDISVER}.tar.gz"

NGX_LUADNSLINKFILE="lua-resty-dns_v${ORESTY_LUADNSVER}.tar.gz"
NGX_LUADNSLINK="https://github.com/openresty/lua-resty-dns/archive/v${ORESTY_LUADNSVER}.tar.gz"

NGX_LUAUPLOADLINKFILE="lua-resty-upload_v${ORESTY_LUAUPLOADVER}.tar.gz"
NGX_LUAUPLOADLINK="https://github.com/openresty/lua-resty-upload/archive/v${ORESTY_LUAUPLOADVER}.tar.gz"

NGX_LUAWEBSOCKETLINKFILE="lua-resty-websocket_v${ORESTY_LUAWEBSOCKETVER}.tar.gz"
NGX_LUAWEBSOCKETLINK="https://github.com/openresty/lua-resty-websocket/archive/v${ORESTY_LUAWEBSOCKETVER}.tar.gz"

NGX_LUALOCKLINKFILE="lua-resty-lock_v${ORESTY_LUALOCKVER}.tar.gz"
NGX_LUALOCKLINK="https://github.com/openresty/lua-resty-lock/archive/v${ORESTY_LUALOCKVER}.tar.gz"

NGX_LUASTRINGLINKFILE="lua-resty-string_v${ORESTY_LUASTRINGVER}.tar.gz"
NGX_LUASTRINGLINK="https://github.com/openresty/lua-resty-string/archive/v${ORESTY_LUASTRINGVER}.tar.gz"

NGX_LUAREDISPARSERLINKFILE="lua-redis-parser_v${ORESTY_LUAREDISPARSERVER}.tar.gz"
NGX_LUAREDISPARSERLINK="https://github.com/openresty/lua-redis-parser/archive/v${ORESTY_LUAREDISPARSERVER}.tar.gz"

NGX_LUAUPSTREAMCHECKLINKFILE="lua-resty-upstream-healthcheck_v${ORESTY_LUAUPSTREAMCHECKVER}.tar.gz"
NGX_LUAUPSTREAMCHECKLINK="https://github.com/openresty/lua-resty-upstream-healthcheck/archive/v${ORESTY_LUAUPSTREAMCHECKVER}.tar.gz"

NGX_LUALRUCACHELINKFILE="lua-resty-lrucache_v${ORESTY_LUALRUCACHEVER}.tar.gz"
NGX_LUALRUCACHELINK="https://github.com/openresty/lua-resty-lrucache/archive/v${ORESTY_LUALRUCACHEVER}.tar.gz"

NGX_LUARESTYCORELINKFILE="lua-resty-core_v${ORESTY_LUARESTYCOREVER}.tar.gz"
NGX_LUARESTYCORELINK="https://github.com/openresty/lua-resty-core/archive/v${ORESTY_LUARESTYCOREVER}.tar.gz"

NGX_LUAUPSTREAMLINKFILE="lua-upstream-nginx-module_v${ORESTY_LUAUPSTREAMVER}.tar.gz"
NGX_LUAUPSTREAMLINK="https://github.com/openresty/lua-upstream-nginx-module/archive/v${ORESTY_LUAUPSTREAMVER}.tar.gz"

NGX_LUALOGGERSOCKETLINKFILE="lua-resty-logger-socket_v${ORESTY_LUALOGGERSOCKETVER}.tar.gz"
NGX_LUALOGGERSOCKETLINK="https://github.com/cloudflare/lua-resty-logger-socket/archive/v${ORESTY_LUALOGGERSOCKETVER}.tar.gz"

NGX_LUACOOKIELINKFILE="lua-resty-cookie_v${ORESTY_LUACOOKIEVER}.tar.gz"
NGX_LUACOOKIELINK="https://github.com/cloudflare/lua-resty-cookie/archive/master.tar.gz"

NGX_LUAUPSTREAMCACHELINKFILE="lua-upstream-cache-nginx-module_v${ORESTY_LUAUPSTREAMCACHEVER}.tar.gz"
NGX_LUAUPSTREAMCACHELINK="https://github.com/cloudflare/lua-upstream-cache-nginx-module/archive/v${ORESTY_LUAUPSTREAMCACHEVER}.tar.gz"

NGX_LUACJSONLINKFILE="lua-cjson_${LUACJSONVER}.tar.gz"
NGX_LUACJSONLINK="https://github.com/openresty/lua-cjson/archive/${LUACJSONVER}.tar.gz"

##

LIBEVENTLINKFILE="release-${LIBEVENT_VERSION}-stable.tar.gz"
LIBEVENTLINKOLD="https://github.com/downloads/libevent/libevent/${LIBEVENTLINKFILE}"
LIBEVENTLINK="https://github.com/libevent/libevent/archive/${LIBEVENTLINKFILE}"

PHPEXT_MAILPARSELINKFILE="mailparse-${MAILPARSEPHP_VER}.tgz"
PHPEXT_MAILPARSELINK="https://pecl.php.net/get/${PHPEXT_MAILPARSELINKFILE}"
PHPEXT_MAILPARSELINKLOCAL="http://centminmod.com/centminmodparts/mailparse/${PHPEXT_MAILPARSELINKFILE}"

PHPEXT_IMAGICKLINKFILE="imagick-${IMAGICKPHP_VER}.tgz"
PHPEXT_IMAGICKLINK="https://pecl.php.net/get/${PHPEXT_IMAGICKLINKFILE}"
PHPEXT_IMAGICKLINKLOCAL="http://centminmod.com/centminmodparts/imagick/${PHPEXT_IMAGICKLINKFILE}"

PHPEXT_MEMCACHELINKFILE="memcache-${MEMCACHE_VERSION}.tgz"
PHPEXT_MEMCACHELINK="https://pecl.php.net/get/${PHPEXT_MEMCACHELINKFILE}"
PHPEXT_MEMCACHELINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_MEMCACHELINKFILE}"

PHPEXT_MEMCACHEDLINKFILE="memcached-${MEMCACHEDPHP_VER}.tgz"
PHPEXT_MEMCACHEDLINK="https://pecl.php.net/get/${PHPEXT_MEMCACHEDLINKFILE}"
PHPEXT_MEMCACHEDLINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_MEMCACHEDLINKFILE}"

PHPEXT_LIBMEMCACHEDLINKFILE="libmemcached-${LIBMEMCACHED_VER}.tar.gz"
PHPEXT_LIBMEMCACHEDLINK="https://launchpad.net/libmemcached/1.0/${LIBMEMCACHED_VER}/+download/${PHPEXT_LIBMEMCACHEDLINKFILE}"
PHPEXT_LIBMEMCACHEDLINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_LIBMEMCACHEDLINKFILE}"

PHPEXT_REDISLINKFILE="phpredis-${REDISPHP_VER}.tgz"
PHPEXT_REDISLINK="https://github.com/phpredis/phpredis/archive/${REDISPHP_VER}.tar.gz"
PHPEXT_REDISLINKLOCAL="http://centminmod.com/centminmodparts/redis/${PHPEXT_REDISLINKFILE}"

PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
PHPEXT_MONGODBLINKLOCAL="http://centminmod.com/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"

TWEMPERF_LINKFILE="twemperf-${TWEMPERF_VER}.tar.gz"
TWEMPERF_LINK="https://github.com/twitter/twemperf/archive/v${TWEMPERF_VER}.tar.gz"
# TWEMPERF_LINK="https://twemperf.googlecode.com/files/${TWEMPERF_LINKFILE}"

MEMCACHEDSERVER_LINKFILE="memcached-${MEMCACHED_VERSION}.tar.gz"
MEMCACHEDSERVER_LINK="http://www.memcached.org/files/${MEMCACHEDSERVER_LINKFILE}"
#MEMCACHEDSERVER_LINK="http://centminmod.com/centminmodparts/memcached/${MEMCACHEDSERVER_LINKFILE}"

MYSQLREPORT_LINKFILE="mysqlreport"
MYSQLREPORT_LINK="http://centminmod.com/centminmodparts/mysqlreport/${MYSQLREPORT_LINKFILE}"

# wget renamed
MYSQLTUNER_LINKFILE="mysqltuner.pl"
MYSQLTUNER_LINK="http://centminmod.com/centminmodparts/mysqltuner/mysqltuner.txt"

NGX_LINKFILE="nginx-${NGINX_VERSION}.tar.gz"
NGX_LINK="http://nginx.org/download/${NGX_LINKFILE}"

NGX_VTSLINK='https://github.com/vozlt/nginx-module-vts.git'

OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
OPENSSL_LINK="http://www.openssl.org/source/${OPENSSL_LINKFILE}"
# OPENSSL_LINK="http://centminmod.com/centminmodparts/openssl/${OPENSSL_LINKFILE}"

LIBRESSL_LINKFILE="libressl-${LIBRESSL_VERSION}.tar.gz"
LIBRESSL_LINK="http://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-${LIBRESSL_VERSION}.tar.gz"

# wget renamed github
# LIBRESSL_LINKFILE="libressl-portable-v${LIBRESSL_VERSION}.tar.gz"
# LIBRESSL_LINK="https://github.com/libressl-portable/portable/archive/v${LIBRESSL_VERSION}.tar.gz"
# LIBRESSL_LINK="http://centminmod.com/centminmodparts/libressl/${LIBRESSL_LINKFILE}"

# libressl non-renamed
LIBRESSL_LOCALLINKFILE="libressl-${LIBRESSL_VERSION}.tar.gz"

SIEGE_LINKFILE="siege-${SIEGE_VERSION}.tar.gz"
SIEGE_LINK="http://download.joedog.org/siege/${SIEGE_LINKFILE}"
SIEGE_LINKLOCAL="http://centminmod.com/centminmodparts/siege/${SIEGE_LINKFILE}"

SPROXY_LINKFILE="sproxy-latest.tar.gz"
SPROXY_LINK="http://www.joedog.org/pub/sproxy/${SPROXY_LINKFILE}"
SPROXY_LINKLOCAL="http://centminmod.com/centminmodparts/siege/${SPROXY_LINKFILE}"

PYTHON_LINKFILE="Python-${PYTHON_VERSION}.tgz"
PYTHON_LINK="http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE}"

PYTHON_SETUPTOOLSLINKFILE="setuptools-0.6c11-py2.7.egg"
PYTHON_SETUPTOOLSLINK="http://pypi.python.org/packages/2.7/s/setuptools/${PYTHON_SETUPTOOLSLINKFILE}"

XCACHE_LINKFILE="xcache-${XCACHE_VERSION}.tar.gz"
XCACHE_LINK="http://xcache.lighttpd.net/pub/Releases/${XCACHE_VERSION}/${XCACHE_LINKFILE}"

APCCACHE_LINKFILE="APC-${APCCACHE_VERSION}.tgz"
APCCACHE_LINK="https://pecl.php.net/get/${APCCACHE_LINKFILE}"
APCCACHE_LINKLOCAL="http://centminmod.com/centminmodparts/apc/php550/${APCCACHE_LINKFILE}"

ZENDOPCACHE_LINKFILE="zendopcache-${ZOPCACHECACHE_VERSION}.tgz"
ZENDOPCACHE_LINK="https://pecl.php.net/get/${ZENDOPCACHE_LINKFILE}"
ZENDOPCACHE_LINKLOCAL="http://centminmod.com/centminmodparts/zendopcache/${ZENDOPCACHE_LINKFILE}"

NSD_LINKFILE="nsd-${NSD_VERSION}.tar.gz"
#NSD_LINK="http://www.nlnetlabs.nl/downloads/nsd/${NSD_LINKFILE}"
NSD_LINK="http://centminmod.com/centminmodparts/nsd/${NSD_LINKFILE}"

CSF_LINKFILE="csf.tgz"
CSF_LINK="http://download.configserver.com/${CSF_LINKFILE}"

EZSETUPLINKFILE='ez_setup.py'
EZSETUPLINK="https://bitbucket.org/pypa/setuptools/raw/bootstrap/${EZSETUPLINKFILE}"

# wget renamed github
AXEK_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEK_LINK="https://github.com/eribertomota/axel/archive/${AXEL_VER}.tar.gz"
AXEK_LINKLOCAL="http://centminmod.com/centminmodparts/axel/${AXEL_VER}.tar.gz"