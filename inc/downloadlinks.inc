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
    
    if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
      if [ -f /usr/bin/dos2unix ]; then
        dos2unix -q "${CM_INSTALLDIR}/inc/z_custom.inc"
      fi
        source "${CM_INSTALLDIR}/inc/z_custom.inc"
    fi

VARNISH_REPOFILE='/etc/yum.repos.d/varnishcache_varnish41.repo'

CURL_CACERTURL='https://curl.haxx.se/ca/cacert.pem'
# CURL_CACERTURL='https://centminmod.com/centminmodparts/curl/cacert.pem'

CCACHEFILE="ccache-${CCACHE_VER}.tar.gz"
#CCCACHELINK="https://www.samba.org/ftp/ccache/${CCACHEFILE}"
CCCACHELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/ccache/${CCACHEFILE}"
CCCACHELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/ccache/${CCACHEFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSFIVEEPELFILE="epel-release-5-4.noarch.rpm"
CENTOSFIVEEPEL="http://mirror.sfo12.us.leaseweb.net/epel/5/${RPMFARCH}/${CENTOSFIVEEPELFILE}"

CENTOSFIVERPMFORGEFILE="rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm"
CENTOSFIVERPMFORGE="http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el5/en/${ARCH}/rpmforge/RPMS/${CENTOSFIVERPMFORGEFILE}"

CENTOSFIVECENTALTFILE="centalt-release-5-3.noarch.rpm"
CENTOSFIVECENTALT="http://centos.alt.ru/repository/centos/5/${RPMFARCH}/${CENTOSFIVECENTALTFILE}"

CENTOSFIVEPERCONARELEASEFILE="percona-release-0.1-4.noarch.rpm"
CENTOSFIVEPERCONARELEASE="https://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSFIVEPERCONARELEASEFILE}"

CENTOSFIVEVARNISHFILE='varnish-release-3.0-1.el5.centos.noarch.rpm'
CENTOSFIVEVARNISH="http://repo.varnish-cache.org/redhat/varnish-3.0/el5/noarch/varnish-release/${CENTOSFIVEVARNISHFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSSIXEPELFILE="epel-release-6-8.noarch.rpm"
CENTOSSIXEPEL="http://mirror.sfo12.us.leaseweb.net/epel/6/${RPMFARCH}/${CENTOSSIXEPELFILE}"
CENTOSSIXEPELLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXEPELFILE}"

CENTOSSIXRPMFORGEFILE="rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm"
CENTOSSIXRPMFORGE="http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el6/en/${ARCH}/rpmforge/RPMS/${CENTOSSIXRPMFORGEFILE}"

CENTOSSIXCENTALTFILE="centalt-release-6-1.noarch.rpm"
CENTOSSIXCENTALT="http://centos.alt.ru/repository/centos/6/${RPMFARCH}/${CENTOSSIXCENTALTFILE}"
CENTOSSIXCENTALTLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXCENTALTFILE}"

CENTOSSIXPERCONALRELEASEFILE="percona-release-0.1-4.noarch.rpm"
CENTOSSIXPERCONALRELEASE="https://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSSIXPERCONALRELEASEFILE}"

CENTOSSIXVARNISHFILE="varnish-4.1.el6.rpm"
CENTOSSIXVARNISH="https://repo.varnish-cache.org/redhat/${CENTOSSIXVARNISHFILE}"
CENTOSSIXVARNISHLOCAL='https://centminmod.com/centminmodparts/repos/${CENTOSSIXVARNISHFILE}'

CENTOSSIXAXIVOFILE="axivo-release-6-1.noarch.rpm"
CENTOSSIXAXIVO="http://rpm.axivo.com/${CENTOSSIXAXIVOFILE}"
CENTOSSIXAXIVOLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXAXIVOFILE}"

CENTOSSIXREMIFILE="remi-release-6.rpm"
CENTOSSIXREMI="https://rpms.remirepo.net/enterprise//${CENTOSSIXREMIFILE}"
CENTOSSIXREMILOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXREMIFILE}"

CENTOSSIXATRPMSFILE="atrpms-repo-6-7.el6.x86_64.rpm"
CENTOSSIXATRPMS="http://dl.atrpms.net/all/${CENTOSSIXATRPMSFILE}"
CENTOSSIXATRPMSLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXATRPMSFILE}"

# dynamic mirror check for EPEL below overrides these values now
CENTOSSEVENEPELFILE="epel-release-7-10.noarch.rpm"
CENTOSSEVENEPEL="http://mirror.sfo12.us.leaseweb.net/epel/7/${RPMFARCH}/e/${CENTOSSEVENEPELFILE}"
CENTOSSEVENEPEL_B="http://dl.fedoraproject.org/pub/epel/7/${RPMFARCH}/e/${CENTOSSEVENEPELFILE}"
CENTOSSEVENEPELLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENEPELFILE}"

# only do dynamic mirror check if /etc/yum.repos.d/epel.repo and thus EPEL yum repo
# has not been setup and installed yet
if [ ! -f /etc/yum.repos.d/epel.repo ]; then
# dynamic mirror check for EPEL
if [[ "$(uname -m)" = 'x86_64' ]]; then
  if [[ "$CENTOS_SEVEN" = '7' ]]; then
    if [[ "$REDHAT_SEVEN" -eq '7' ]]; then
      CENTOSSEVENEPEL_MIRRORURL=$(curl -${ipv_forceopt}sI https://download.fedoraproject.org/pub/epel/7/x86_64/e/ | awk '/Location: / {print $2}')
      CENTOSSEVENEPEL_MIRRORURLFILE=$(curl -${ipv_forceopt}sL https://download.fedoraproject.org/pub/epel/7/x86_64/e/ | grep -o '<a .*href=.*>' | sed -e 's/<a /\n<a /g' | sed -e 's/<a .*href=['"'"'"]//' -e 's/["'"'"'].*$//' -e '/^$/ d' | grep 'epel-release' | grep 'rpm')
      CENTOSSEVENEPELFILE="$CENTOSSEVENEPEL_MIRRORURLFILE"
      CENTOSSEVENEPEL="$CENTOSSEVENEPEL_MIRRORFULLURL"
    else
      CENTOSSEVENEPEL_MIRRORFULLURL=$(lynx -dump -listonly https://download.fedoraproject.org/pub/epel/7/x86_64/e/ | grep epel-release | grep 'rpm' | awk -F " " '{print $2}')
      CENTOSSEVENEPEL_MIRRORURL=$(echo ${CENTOSSEVENEPEL_MIRRORFULLURL%/*}/)
      CENTOSSEVENEPEL_MIRRORURLFILE=$(echo ${CENTOSSEVENEPEL_MIRRORFULLURL##*/})
      CENTOSSEVENEPELFILE="$CENTOSSEVENEPEL_MIRRORURLFILE"
      CENTOSSEVENEPEL="$CENTOSSEVENEPEL_MIRRORFULLURL"
    fi
  fi
  
  if [[ "$CENTOS_SIX" = '6' ]]; then
  CENTOSSIXEPEL_MIRRORFULLURL=$(lynx -dump -listonly https://download.fedoraproject.org/pub/epel/6/x86_64/ | grep epel-release | grep 'rpm' | awk -F " " '{print $2}')
  CENTOSSIXEPEL_MIRRORURL=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL%/*}/)
  CENTOSSIXEPEL_MIRRORURLFILE=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL##*/})
  CENTOSSIXEPELFILE="$CENTOSSIXEPEL_MIRRORURLFILE"
  CENTOSSIXEPEL="$CENTOSSIXEPEL_MIRRORFULLURL"
  fi
  
  if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then
  CENTOSFIVEEPEL_MIRRORFULLURL=$(lynx -dump -listonly https://download.fedoraproject.org/pub/epel/5/x86_64/ | grep epel-release | grep 'rpm' | awk -F " " '{print $2}')
  CENTOSFIVEEPEL_MIRRORURL=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL%/*}/)
  CENTOSFIVEEPEL_MIRRORURLFILE=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL##*/})
  CENTOSFIVEEPELFILE="$CENTOSFIVEEPEL_MIRRORURLFILE"
  CENTOSFIVEEPEL="$CENTOSFIVEEPEL_MIRRORFULLURL"
  fi
else
  if [[ "$CENTOS_SIX" = '6' ]]; then
  CENTOSSIXEPEL_MIRRORFULLURL=$(lynx -dump -listonly https://download.fedoraproject.org/pub/epel/6/i386/ | grep epel-release | grep 'rpm' | awk -F " " '{print $2}')
  CENTOSSIXEPEL_MIRRORURL=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL%/*}/)
  CENTOSSIXEPEL_MIRRORURLFILE=$(echo ${CENTOSSIXEPEL_MIRRORFULLURL##*/})
  CENTOSSIXEPELFILE="$CENTOSSIXEPEL_MIRRORURLFILE"
  CENTOSSIXEPEL="$CENTOSSIXEPEL_MIRRORFULLURL"
  fi
  
  if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then
  CENTOSFIVEEPEL_MIRRORFULLURL=$(lynx -dump -listonly https://download.fedoraproject.org/pub/epel/5/i386/ | grep epel-release | grep 'rpm' | awk -F " " '{print $2}')
  CENTOSFIVEEPEL_MIRRORURL=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL%/*}/)
  CENTOSFIVEEPEL_MIRRORURLFILE=$(echo ${CENTOSFIVEEPEL_MIRRORFULLURL##*/})
  CENTOSFIVEEPELFILE="$CENTOSFIVEEPEL_MIRRORURLFILE"
  CENTOSFIVEEPEL="$CENTOSFIVEEPEL_MIRRORFULLURL"
  fi
fi
fi # /etc/yum.repos.d/epel.repo file check

CENTOSSEVENRPMFORGEFILE="rpmforge-release-0.5.3-1.el7.rf.${ARCH}.rpm"
CENTOSSEVENRPMFORGE="http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el7/en/${ARCH}/rpmforge/RPMS/${CENTOSSEVENRPMFORGEFILE}"

CENTOSSEVENCENTALTFILE="centalt-release-6-1.noarch.rpm"
CENTOSSEVENCENTALT="http://centos.alt.ru/repository/centos/6/${RPMFARCH}/${CENTOSSEVENCENTALTFILE}"
CENTOSSEVENCENTALTLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENCENTALTFILE}"

CENTOSSEVENPERCONALRELEASEFILE="percona-release-0.1-4.noarch.rpm"
CENTOSSEVENPERCONALRELEASE="https://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSSEVENPERCONALRELEASEFILE}"

CENTOSSEVENVARNISHFILE="varnish-4.1.el7.rpm"
CENTOSSEVENVARNISH="https://repo.varnish-cache.org/redhat/${CENTOSSEVENVARNISHFILE}"
CENTOSSEVENVARNISHLOCAL='https://centminmod.com/centminmodparts/repos/${CENTOSSEVENVARNISHFILE}'

CENTOSSEVENAXIVOFILE="axivo-release-7-1.noarch.rpm"
CENTOSSEVENAXIVO="http://rpm.axivo.com/${CENTOSSEVENAXIVOFILE}"
CENTOSSEVENAXIVOLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENAXIVOFILE}"

CENTOSSEVENREMIFILE="remi-release-7.rpm"
CENTOSSEVENREMI="https://rpms.remirepo.net/enterprise//${CENTOSSEVENREMIFILE}"
CENTOSSEVENREMILOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENREMIFILE}"

CENTOSSEVENATRPMSFILE="atrpms-repo-6-7.el6.x86_64.rpm"
CENTOSSEVENATRPMS="http://dl.atrpms.net/all/${CENTOSSEVENATRPMSFILE}"
CENTOSSEVENATRPMSLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENATRPMSFILE}"

PCRELINKFILE="pcre-${NGINX_PCREVER}.tar.gz"
#PCRELINK="https://ftp.pcre.org/pub/pcre/${PCRELINKFILE}"
PCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRELINKFILE}"
PCRELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRELINKFILE}"

PCRE_SOURCELINKFILE="pcre-${PCRE_VERSION}.tar.gz"
PCRE_SOURCELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRE_SOURCELINKFILE}"
PCRE_SOURCELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRE_SOURCELINKFILE}"

NGX_ZLIBLINKFILE="zlib-${NGINX_ZLIBVER}.tar.gz"
NGX_ZLIBLINK="https://zlib.net/${NGX_ZLIBLINKFILE}"
NGX_ZLIBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/zlib/${NGX_ZLIBLINKFILE}"

# wget renamed
NGX_WEBDAVLINKFILE="nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz"
NGX_WEBDAVLINK="https://github.com/arut/nginx-dav-ext-module/archive/v${NGINX_EXTWEBDAVVER}.tar.gz"
NGX_WEBDAVLINKGITLAB="https://gitlab.com/centminmod-github-mirror/nginx-dav-ext-module/repository/archive.tar.gz?ref=v${NGINX_EXTWEBDAVVER}"

# wget renamed
NGX_PAGESPEEDGITLINKFILE="release-${NGXPGSPEED_VER}.tar.gz"
NGX_PAGESPEEDGITLINK="https://github.com/apache/incubator-pagespeed-ngx/archive/master.tar.gz"
NGX_PAGESPEEDGITLINKGITLAB="https://gitlab.com/centminmod-github-mirror/ngx_pagespeed/repository/archive.tar.gz?ref=master"

NGX_PAGESPEEDLINKFILE="release-${NGXPGSPEED_VER}.tar.gz"
NGX_PAGESPEEDLINKREALFILE="v${NGXPGSPEED_VER}.tar.gz"
NGX_PAGESPEEDLINK="https://github.com/apache/incubator-pagespeed-ngx/archive/${NGX_PAGESPEEDLINKREALFILE}"
NGX_PAGESPEEDLINKGITLAB="https://gitlab.com/centminmod-github-mirror/ngx_pagespeed/repository/archive.tar.gz?ref=v${NGXPGSPEED_VER}"

NGX_PAGESPEEDPSOLINKLFILE="${NGINX_PAGESPEEDPSOL_VER}.tar.gz"
NGX_PAGESPEEDPSOLINKL="https://dl.google.com/dl/page-speed/psol/${NGX_PAGESPEEDPSOLINKLFILE}"

NGX_FANCYINDEXLINKFILE="ngx-fancyindex-${NGINX_FANCYINDEXVER}.tar.gz"
NGX_FANCYINDEXLINK="https://github.com/aperezdc/ngx-fancyindex/archive/v${NGINX_FANCYINDEXVER}.tar.gz"
#NGX_FANCYINDEXLINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_FANCYINDEXLINKFILE}"

NGX_CACHEPURGEFILE="ngx_cache_purge-${NGINX_CACHEPURGEVER}.tar.gz"
NGX_CACHEPURGE="https://github.com/nginx-modules/ngx_cache_purge/archive/${NGINX_CACHEPURGEVER}.tar.gz"
#NGX_CACHEPURGE="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_CACHEPURGEFILE}"

NGX_ACCESSKEYLINKFILE="Nginx-accesskey-2.0.3.tar.gz"
NGX_ACCESSKEYLINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_ACCESSKEYLINKFILE}"

# wget renamed
NGX_CONCATLINKFILE="nginx-http-concat-master.tar.gz"
NGX_CONCATLINK='https://github.com/alibaba/nginx-http-concat/archive/master.tar.gz'
NGX_CONCATLINKGITLAB='https://gitlab.com/centminmod-github-mirror/nginx-http-concat/repository/archive.tar.gz?ref=master'

# wget renamed
NGX_HEADERSMORELINKFILE="headers-more-v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINK="https://github.com/openresty/headers-more-nginx-module/archive/v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINKGITLAB="https://gitlab.com/centminmod-github-mirror/headers-more-nginx-module/repository/archive.tar.gz?ref=v${NGINX_HEADERSMORE}"

# wget renamed
NGX_STICKYLINKFILE="nginx-sticky-${NGINX_STICKYVER}.tar.gz"
NGX_STICKYLINK="https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng/get/${NGINX_STICKYVER}.tar.gz"
NGX_STICKYLINKGITLAB="https://gitlab.com/centminmod-github-mirror/nginx-sticky-module-ng/repository/archive.tar.gz?ref=${NGINX_STICKYVER}"

# wget renamed
NGX_UPSTREAMCHECKLINKFILE="nginx-upstream-check-${NGINX_UPSTREAMCHECKVER}.tar.gz"
NGX_UPSTREAMCHECKLINK="https://github.com/yaoweibin/nginx_upstream_check_module/archive/v${NGINX_UPSTREAMCHECKVER}.tar.gz"
NGX_UPSTREAMCHECKLINKGITLAB="https://gitlab.com/centminmod-github-mirror/nginx_upstream_check_module/repository/archive.tar.gz?ref=${NGINX_UPSTREAMCHECKVER}"

# wget renamed
NGX_MEMCLINKFILE="memc-nginx-module_v${ORESTY_MEMCVER}.tar.gz"
NGX_MEMCLINK="https://github.com/openresty/memc-nginx-module/archive/v${ORESTY_MEMCVER}.tar.gz"
NGX_MEMCLINKGITLAB="https://gitlab.com/centminmod-github-mirror/memc-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_MEMCVER}"

# wget renamed
NGX_SRCACHELINKFILE="srcache-nginx-module_v${ORESTY_SRCCACHEVER}.tar.gz"
NGX_SRCACHELINK="https://github.com/openresty/srcache-nginx-module/archive/v${ORESTY_SRCCACHEVER}.tar.gz"
NGX_SRCACHELINKGITLAB="https://gitlab.com/centminmod-github-mirror/srcache-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_SRCCACHEVER}"

# wget renamed
NGX_DEVELKITLINKFILE="nginx-develkit_v${ORESTY_DEVELKITVER}.tar.gz"
NGX_DEVELKITLINK="https://github.com/simpl/ngx_devel_kit/archive/v${ORESTY_DEVELKITVER}.tar.gz"
NGX_DEVELKITLINKGITLAB="https://gitlab.com/centminmod-github-mirror/ngx_devel_kit/repository/archive.tar.gz?ref=v${ORESTY_DEVELKITVER}"

# wget renamed
NGX_SETMISCLINKFILE="set-misc-nginx-module_v${ORESTY_SETMISCVER}.tar.gz"
NGX_SETMISCLINK="https://github.com/openresty/set-misc-nginx-module/archive/v${ORESTY_SETMISCVER}.tar.gz"
NGX_SETMISCLINKGITLAB="https://gitlab.com/centminmod-github-mirror/set-misc-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_SETMISCVER}"

# wget renamed
NGX_ECHOLINKFILE="echo-nginx-module_v${ORESTY_ECHOVER}.tar.gz"
NGX_ECHOLINK="https://github.com/openresty/echo-nginx-module/archive/v${ORESTY_ECHOVER}.tar.gz"
NGX_ECHOLINKGITLAB="https://gitlab.com/centminmod-github-mirror/echo-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_ECHOVER}"

# wget renamed
NGX_REDISLINKFILE="redis2-nginx-module_v${ORESTY_REDISVER}.tar.gz"
NGX_REDISLINK="https://github.com/openresty/redis2-nginx-module/archive/v${ORESTY_REDISVER}.tar.gz"
NGX_REDISLINKGITLAB="https://gitlab.com/centminmod-github-mirror/redis2-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_REDISVER}"

# wget renamed
NGX_HTTPREDISLINKFILE="ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"
NGX_HTTPREDISLINK="http://people.freebsd.org/~osa/ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"

##############
# lua

NGX_LUANGINXLINKFILE="lua-nginx-module_v${ORESTY_LUANGINXVER}.tar.gz"
NGX_LUANGINXLINK="https://github.com/openresty/lua-nginx-module/archive/v${ORESTY_LUANGINXVER}.tar.gz"
NGX_LUANGINXLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUANGINXVER}"

NGX_LUASTREAMLINKFILE="stream-lua-nginx-module_v${ORESTY_LUASTREAMVER}.tar.gz"
NGX_LUASTREAMLINK="https://github.com/openresty/stream-lua-nginx-module/archive/v${ORESTY_LUASTREAMVER}.tar.gz"
#NGX_LUASTREAMLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUASTREAMVER}"

NGX_LUAGITLINKFILE="luagit-${ORESTY_LUAGITVER}.tar.gz"
NGX_LUAGITLINK="https://luajit.org/download/LuaJIT-${ORESTY_LUAGITVER}.tar.gz"

NGX_LUAMEMCACHEDLINKFILE="lua-resty-memcached_v${ORESTY_LUAMEMCACHEDVER}.tar.gz"
NGX_LUAMEMCACHEDLINK="https://github.com/openresty/lua-resty-memcached/archive/v${ORESTY_LUAMEMCACHEDVER}.tar.gz"
NGX_LUAMEMCACHEDLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-memcached/repository/archive.tar.gz?ref=v${ORESTY_LUAMEMCACHEDVER}"

NGX_LUAMYSQLLINKFILE="lua-resty-mysql_v${ORESTY_LUAMYSQLVER}.tar.gz"
NGX_LUAMYSQLLINK="https://github.com/openresty/lua-resty-mysql/archive/v${ORESTY_LUAMYSQLVER}.tar.gz"
NGX_LUAMYSQLLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-mysql/repository/archive.tar.gz?ref=v${ORESTY_LUAMYSQLVER}"

NGX_LUAREDISLINKFILE="lua-resty-redis_v${ORESTY_LUAREDISVER}.tar.gz"
NGX_LUAREDISLINK="https://github.com/openresty/lua-resty-redis/archive/v${ORESTY_LUAREDISVER}.tar.gz"
NGX_LUAREDISLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-redis/repository/archive.tar.gz?ref=v${ORESTY_LUAREDISVER}"

NGX_LUADNSLINKFILE="lua-resty-dns_v${ORESTY_LUADNSVER}.tar.gz"
NGX_LUADNSLINK="https://github.com/openresty/lua-resty-dns/archive/v${ORESTY_LUADNSVER}.tar.gz"
NGX_LUADNSLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-dns/repository/archive.tar.gz?ref=v${ORESTY_LUADNSVER}"

NGX_LUAUPLOADLINKFILE="lua-resty-upload_v${ORESTY_LUAUPLOADVER}.tar.gz"
NGX_LUAUPLOADLINK="https://github.com/openresty/lua-resty-upload/archive/v${ORESTY_LUAUPLOADVER}.tar.gz"
NGX_LUAUPLOADLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-upload/repository/archive.tar.gz?ref=v${ORESTY_LUAUPLOADVER}"

NGX_LUAWEBSOCKETLINKFILE="lua-resty-websocket_v${ORESTY_LUAWEBSOCKETVER}.tar.gz"
NGX_LUAWEBSOCKETLINK="https://github.com/openresty/lua-resty-websocket/archive/v${ORESTY_LUAWEBSOCKETVER}.tar.gz"
NGX_LUAWEBSOCKETLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-websocket/repository/archive.tar.gz?ref=v${ORESTY_LUAWEBSOCKETVER}"

NGX_LUALOCKLINKFILE="lua-resty-lock_v${ORESTY_LUALOCKVER}.tar.gz"
NGX_LUALOCKLINK="https://github.com/openresty/lua-resty-lock/archive/v${ORESTY_LUALOCKVER}.tar.gz"
NGX_LUALOCKLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-lock/repository/archive.tar.gz?ref=v${ORESTY_LUALOCKVER}"

NGX_LUASTRINGLINKFILE="lua-resty-string_v${ORESTY_LUASTRINGVER}.tar.gz"
NGX_LUASTRINGLINK="https://github.com/openresty/lua-resty-string/archive/v${ORESTY_LUASTRINGVER}.tar.gz"
NGX_LUASTRINGLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-string/repository/archive.tar.gz?ref=v${ORESTY_LUASTRINGVER}"

NGX_LUAREDISPARSERLINKFILE="lua-redis-parser_v${ORESTY_LUAREDISPARSERVER}.tar.gz"
NGX_LUAREDISPARSERLINK="https://github.com/openresty/lua-redis-parser/archive/v${ORESTY_LUAREDISPARSERVER}.tar.gz"
NGX_LUAREDISPARSERLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-redis-parser/repository/archive.tar.gz?ref=v${ORESTY_LUAREDISPARSERVER}"

NGX_LUAUPSTREAMCHECKLINKFILE="lua-resty-upstream-healthcheck_v${ORESTY_LUAUPSTREAMCHECKVER}.tar.gz"
NGX_LUAUPSTREAMCHECKLINK="https://github.com/openresty/lua-resty-upstream-healthcheck/archive/v${ORESTY_LUAUPSTREAMCHECKVER}.tar.gz"
NGX_LUAUPSTREAMCHECKLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-upstream-healthcheck/repository/archive.tar.gz?ref=v${ORESTY_LUAUPSTREAMCHECKVER}"

NGX_LUALRUCACHELINKFILE="lua-resty-lrucache_v${ORESTY_LUALRUCACHEVER}.tar.gz"
NGX_LUALRUCACHELINK="https://github.com/openresty/lua-resty-lrucache/archive/v${ORESTY_LUALRUCACHEVER}.tar.gz"
NGX_LUALRUCACHELINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-lrucache/repository/archive.tar.gz?ref=v${ORESTY_LUALRUCACHEVER}"

NGX_LUARESTYCORELINKFILE="lua-resty-core_v${ORESTY_LUARESTYCOREVER}.tar.gz"
NGX_LUARESTYCORELINK="https://github.com/openresty/lua-resty-core/archive/v${ORESTY_LUARESTYCOREVER}.tar.gz"
NGX_LUARESTYCORELINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-core/repository/archive.tar.gz?ref=v${ORESTY_LUARESTYCOREVER}"

NGX_LUAUPSTREAMLINKFILE="lua-upstream-nginx-module_v${ORESTY_LUAUPSTREAMVER}.tar.gz"
NGX_LUAUPSTREAMLINK="https://github.com/openresty/lua-upstream-nginx-module/archive/v${ORESTY_LUAUPSTREAMVER}.tar.gz"
NGX_LUAUPSTREAMLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-upstream-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUAUPSTREAMVER}"

NGX_LUALOGGERSOCKETLINKFILE="lua-resty-logger-socket_v${ORESTY_LUALOGGERSOCKETVER}.tar.gz"
NGX_LUALOGGERSOCKETLINK="https://github.com/cloudflare/lua-resty-logger-socket/archive/v${ORESTY_LUALOGGERSOCKETVER}.tar.gz"
NGX_LUALOGGERSOCKETLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-logger-socket/repository/archive.tar.gz?ref=v${ORESTY_LUALOGGERSOCKETVER}"

NGX_LUACOOKIELINKFILE="lua-resty-cookie_v${ORESTY_LUACOOKIEVER}.tar.gz"
NGX_LUACOOKIELINK="https://github.com/cloudflare/lua-resty-cookie/archive/master.tar.gz"
NGX_LUACOOKIELINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-resty-cookie/repository/archive.tar.gz?ref=master"

NGX_LUAUPSTREAMCACHELINKFILE="lua-upstream-cache-nginx-module_v${ORESTY_LUAUPSTREAMCACHEVER}.tar.gz"
NGX_LUAUPSTREAMCACHELINK="https://github.com/cloudflare/lua-upstream-cache-nginx-module/archive/v${ORESTY_LUAUPSTREAMCACHEVER}.tar.gz"
NGX_LUAUPSTREAMCACHELINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-upstream-cache-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUAUPSTREAMCACHEVER}"

NGX_LUACJSONLINKFILE="lua-cjson_${LUACJSONVER}.tar.gz"
NGX_LUACJSONLINK="https://github.com/openresty/lua-cjson/archive/${LUACJSONVER}.tar.gz"
NGX_LUACJSONLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-cjson/repository/archive.tar.gz?ref=${LUACJSONVER}"

##

LIBEVENTLINKFILE="release-${LIBEVENT_VERSION}-stable.tar.gz"
LIBEVENTLINKOLD="https://github.com/downloads/libevent/libevent/${LIBEVENTLINKFILE}"
LIBEVENTLINK="https://github.com/libevent/libevent/archive/${LIBEVENTLINKFILE}"
LIBEVENTLINKGITLAB="https://gitlab.com/centminmod-github-mirror/libevent/repository/archive.tar.gz?ref=release-${LIBEVENT_VERSION}-stable"

PHPEXT_MAILPARSELINKFILE="mailparse-${MAILPARSEPHP_VER}.tgz"
PHPEXT_MAILPARSELINK="https://pecl.php.net/get/${PHPEXT_MAILPARSELINKFILE}"
PHPEXT_MAILPARSELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mailparse/${PHPEXT_MAILPARSELINKFILE}"

PHPEXT_IMAGICKLINKFILE="imagick-${IMAGICKPHP_VER}.tgz"
PHPEXT_IMAGICKLINK="https://pecl.php.net/get/${PHPEXT_IMAGICKLINKFILE}"
PHPEXT_IMAGICKLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/imagick/${PHPEXT_IMAGICKLINKFILE}"

PHPEXT_MEMCACHELINKFILE="memcache-${MEMCACHE_VERSION}.tgz"
PHPEXT_MEMCACHELINK="https://pecl.php.net/get/${PHPEXT_MEMCACHELINKFILE}"
PHPEXT_MEMCACHELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/memcached/${PHPEXT_MEMCACHELINKFILE}"

PHPEXT_MEMCACHEDLINKFILE="memcached-${MEMCACHEDPHP_VER}.tgz"
PHPEXT_MEMCACHEDLINK="https://pecl.php.net/get/${PHPEXT_MEMCACHEDLINKFILE}"
PHPEXT_MEMCACHEDLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/memcached/${PHPEXT_MEMCACHEDLINKFILE}"

PHPEXT_LIBMEMCACHEDLINKFILE="libmemcached-${LIBMEMCACHED_VER}.tar.gz"
PHPEXT_LIBMEMCACHEDLINK="https://launchpad.net/libmemcached/1.0/${LIBMEMCACHED_VER}/+download/${PHPEXT_LIBMEMCACHEDLINKFILE}"
PHPEXT_LIBMEMCACHEDLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/memcached/${PHPEXT_LIBMEMCACHEDLINKFILE}"

PHPEXT_REDISLINKFILE="phpredis-${REDISPHP_VER}.tgz"
PHPEXT_REDISLINK="https://github.com/phpredis/phpredis/archive/${REDISPHP_VER}.tar.gz"
PHPEXT_REDISLINKGITLAB="https://gitlab.com/centminmod-github-mirror/phpredis/repository/archive.tar.gz?ref=${REDISPHP_VER}"
PHPEXT_REDISLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/redis/${PHPEXT_REDISLINKFILE}"

PHPEXT_MONGODBLINKFILE="mongodb-${MONGODBPHP_VER}.tgz"
PHPEXT_MONGODBLINK="https://pecl.php.net/get/${PHPEXT_MONGODBLINKFILE}"
PHPEXT_MONGODBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mongodb/${PHPEXT_MONGODBLINKFILE}"

TWEMPERF_LINKFILE="twemperf-${TWEMPERF_VER}.tar.gz"
TWEMPERF_LINK="https://github.com/twitter/twemperf/archive/v${TWEMPERF_VER}.tar.gz"
TWEMPERF_LINKGITLAB="https://gitlab.com/centminmod-github-mirror/twemperf/repository/archive.tar.gz?ref=v${TWEMPERF_VER}"
# TWEMPERF_LINK="https://twemperf.googlecode.com/files/${TWEMPERF_LINKFILE}"

MEMCACHEDSERVER_LINKFILE="memcached-${MEMCACHED_VERSION}.tar.gz"
MEMCACHEDSERVER_LINK="https://www.memcached.org/files/${MEMCACHEDSERVER_LINKFILE}"
MEMCACHEDSERVER_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/memcached/${MEMCACHEDSERVER_LINKFILE}"

MYSQLREPORT_LINKFILE="mysqlreport"
MYSQLREPORT_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mysqlreport/${MYSQLREPORT_LINKFILE}"

# wget renamed
MYSQLTUNER_LINKFILE="mysqltuner.pl"
MYSQLTUNER_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mysqltuner/mysqltuner.txt"

NGX_LINKFILE="nginx-${NGINX_VERSION}.tar.gz"
NGX_LINK="https://nginx.org/download/${NGX_LINKFILE}"

NGX_VTSLINK='https://github.com/vozlt/nginx-module-vts.git'
NGX_VTSLINKGITLAB='https://gitlab.com/centminmod-github-mirror/nginx-module-vts.git'

OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
OPENSSL_LINK="https://www.openssl.org/source/${OPENSSL_LINKFILE}"
OPENSSL_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/openssl/${OPENSSL_LINKFILE}"

LIBRESSL_LINKFILE="libressl-${LIBRESSL_VERSION}.tar.gz"
LIBRESSL_LINK="https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-${LIBRESSL_VERSION}.tar.gz"
LIBRESSL_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl/${LIBRESSL_LINKFILE}"

# wget renamed github
# LIBRESSL_LINKFILE="libressl-portable-v${LIBRESSL_VERSION}.tar.gz"
# LIBRESSL_LINK="https://github.com/libressl-portable/portable/archive/v${LIBRESSL_VERSION}.tar.gz"
# LIBRESSL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl/${LIBRESSL_LINKFILE}"

# libressl non-renamed
LIBRESSL_LOCALLINKFILE="libressl-${LIBRESSL_VERSION}.tar.gz"

SIEGE_LINKFILE="siege-${SIEGE_VERSION}.tar.gz"
SIEGE_LINK="http://download.joedog.org/siege/${SIEGE_LINKFILE}"
SIEGE_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/siege/${SIEGE_LINKFILE}"

SPROXY_LINKFILE="sproxy-latest.tar.gz"
SPROXY_LINK="http://www.joedog.org/pub/sproxy/${SPROXY_LINKFILE}"
SPROXY_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/siege/${SPROXY_LINKFILE}"

PYTHON_LINKFILE="Python-${PYTHON_VERSION}.tgz"
PYTHON_LINK="http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE}"

PYTHON_SETUPTOOLSLINKFILE="setuptools-0.6c11-py2.7.egg"
PYTHON_SETUPTOOLSLINK="http://pypi.python.org/packages/2.7/s/setuptools/${PYTHON_SETUPTOOLSLINKFILE}"

XCACHE_LINKFILE="xcache-${XCACHE_VERSION}.tar.gz"
XCACHE_LINK="https://xcache.lighttpd.net/pub/Releases/${XCACHE_VERSION}/${XCACHE_LINKFILE}"

APCCACHE_LINKFILE="APC-${APCCACHE_VERSION}.tgz"
APCCACHE_LINK="https://pecl.php.net/get/${APCCACHE_LINKFILE}"
APCCACHE_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/apc/php550/${APCCACHE_LINKFILE}"

ZENDOPCACHE_LINKFILE="zendopcache-${ZOPCACHECACHE_VERSION}.tgz"
ZENDOPCACHE_LINK="https://pecl.php.net/get/${ZENDOPCACHE_LINKFILE}"
ZENDOPCACHE_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/zendopcache/${ZENDOPCACHE_LINKFILE}"

NSD_LINKFILE="nsd-${NSD_VERSION}.tar.gz"
#NSD_LINK="https://www.nlnetlabs.nl/downloads/nsd/${NSD_LINKFILE}"
NSD_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nsd/${NSD_LINKFILE}"

CSF_LINKFILE="csf.tgz"
CSF_LINK="https://download.configserver.com/${CSF_LINKFILE}"

EZSETUPLINKFILE='ez_setup.py'
EZSETUPLINK="https://bitbucket.org/pypa/setuptools/raw/bootstrap/${EZSETUPLINKFILE}"

# wget renamed github
AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
AXEL_LINKGITLAB="https://gitlab.com/centminmod-github-mirror/axel/repository/archive.tar.gz?ref=${AXEL_VER}"
AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
WGET_LINK="https://centminmod.com/centminmodparts/wget/${WGET_FILENAME}"

LIBUNWIND_LINKFILE="libunwind-${LIBUNWIND_VERSION}.tar.gz"
LIBUNWIND_LINKDIR="libunwind-${LIBUNWIND_VERSION}"
LIBUNWIND_LINK="https://download.savannah.gnu.org/releases/libunwind/${LIBUNWIND_LINKFILE}"
LIBUNWIND_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/gperf/${LIBUNWIND_LINKFILE}"

GPERFTOOL_LINKFILE="gperftools-${GPERFTOOLS_VERSION}.tar.gz"
GPERFTOOL_LINKDIR="gperftools-${GPERFTOOLS_VERSION}"
GPERFTOOL_LINK="https://github.com/gperftools/gperftools/releases/download/gperftools-${GPERFTOOLS_VERSION}/${GPERFTOOL_LINKFILE}"
GPERFTOOL_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/gperf/${GPERFTOOL_LINKFILE}"

LIBZIP_LINKFILE="libzip-${LIBZIP_VER}.tar.gz"
LIBZIP_LINK="https://libzip.org/download/${LIBZIP_LINKFILE}"