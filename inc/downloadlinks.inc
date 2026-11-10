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

CURL_CACERTURL='https://curl.se/ca/cacert.pem'
# CURL_CACERTURL='${LOCALCENTMINMOD_MIRROR}/centminmodparts/curl/cacert.pem'

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

CENTOSFIVEPERCONARELEASEFILE="percona-release-latest.noarch.rpm"
CENTOSFIVEPERCONARELEASE="https://repo.percona.com/yum/${CENTOSFIVEPERCONARELEASEFILE}"

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

CENTOSSIXPERCONALRELEASEFILE="percona-release-latest.noarch.rpm"
CENTOSSIXPERCONALRELEASE="https://repo.percona.com/yum/${CENTOSSIXPERCONALRELEASEFILE}"

CENTOSSIXVARNISHFILE="varnish-4.1.el6.rpm"
CENTOSSIXVARNISH="https://repo.varnish-cache.org/redhat/${CENTOSSIXVARNISHFILE}"
CENTOSSIXVARNISHLOCAL='${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXVARNISHFILE}'

CENTOSSIXAXIVOFILE="axivo-release-6-1.noarch.rpm"
CENTOSSIXAXIVO="http://rpm.axivo.com/${CENTOSSIXAXIVOFILE}"
CENTOSSIXAXIVOLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSIXAXIVOFILE}"

CENTOSSIXREMIFILE="remi-release-6.rpm"
CENTOSSIXREMI="https://rpms.remirepo.net/enterprise/${CENTOSSIXREMIFILE}"
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

CENTOSSEVENPERCONALRELEASEFILE="percona-release-latest.noarch.rpm"
CENTOSSEVENPERCONALRELEASE="https://repo.percona.com/yum/${CENTOSSEVENPERCONALRELEASEFILE}"

CENTOSSEVENVARNISHFILE="varnish-4.1.el7.rpm"
CENTOSSEVENVARNISH="https://repo.varnish-cache.org/redhat/${CENTOSSEVENVARNISHFILE}"
CENTOSSEVENVARNISHLOCAL='${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENVARNISHFILE}'

CENTOSSEVENAXIVOFILE="axivo-release-7-1.noarch.rpm"
CENTOSSEVENAXIVO="http://rpm.axivo.com/${CENTOSSEVENAXIVOFILE}"
CENTOSSEVENAXIVOLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENAXIVOFILE}"

CENTOSSEVENREMIFILE="remi-release-7.rpm"
CENTOSSEVENREMI="https://rpms.remirepo.net/enterprise/${CENTOSSEVENREMIFILE}"
CENTOSSEVENREMILOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENREMIFILE}"

CENTOSSEVENATRPMSFILE="atrpms-repo-6-7.el6.x86_64.rpm"
CENTOSSEVENATRPMS="http://dl.atrpms.net/all/${CENTOSSEVENATRPMSFILE}"
CENTOSSEVENATRPMSLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/repos/${CENTOSSEVENATRPMSFILE}"

PCRETWOLINKFILE="pcre2-${NGINX_PCRETWOVER}.tar.gz"
MASTER_PCRETWOLINKFILE="master-pcre2${NGINX_PCRETWOVER}.tar.gz"
#PCRETWOLINK="https://github.com/PhilipHazel/pcre2/releases/download/pcre2-${NGINX_PCRETWOVER}/pcre2-${NGINX_PCRETWOVER}.tar.gz"
PCRETWOLINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRETWOLINKFILE}"
PCRETWOLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRETWOLINKFILE}"

PCRELINKFILE="pcre-${NGINX_PCREVER}.tar.gz"
MASTER_PCRELINKFILE="master-pcre${NGINX_PCREVER}.tar.gz"
#PCRELINK="https://ftp.pcre.org/pub/pcre/${PCRELINKFILE}"
PCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRELINKFILE}"
PCRELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRELINKFILE}"

PCRE_SOURCELINKFILE="pcre-${PCRE_VERSION}.tar.gz"
PCRE_SOURCELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRE_SOURCELINKFILE}"
PCRE_SOURCELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${PCRE_SOURCELINKFILE}"

NGX_ZLIBLINKFILE="zlib-${NGINX_ZLIBVER}.tar.gz"
MASTER_NGX_ZLIBLINKFILE="master-zlib-${NGINX_ZLIBVER}.tar.gz"
# GitHub release assets exist for zlib 1.2.13 and newer; zlib.net only hosts the newest release
NGX_ZLIBLINK="https://github.com/madler/zlib/releases/download/v${NGINX_ZLIBVER}/${NGX_ZLIBLINKFILE}"
NGX_ZLIBLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/zlib/${NGX_ZLIBLINKFILE}"

# bellard QuickJS for njs 'js_engine qjs;' (NGINX_NJS_QUICKJS='y'), the release
# nginx.org's own njs packages build. The sha256 is checked for the default
# release only, a different NGINX_NJS_QUICKJS_VER skips the check
NGX_QUICKJSLINKFILE="quickjs-${NGINX_NJS_QUICKJS_VER}.tar.xz"
NGX_QUICKJSLINK="https://bellard.org/quickjs/${NGX_QUICKJSLINKFILE}"
NGX_QUICKJSLINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/quickjs/${NGX_QUICKJSLINKFILE}"
if [[ "$NGINX_NJS_QUICKJS_VER" = '2026-06-04' ]]; then
  NGX_QUICKJS_SHA256='b376e839b322978313d929fd20663b11ba58b75df5a46c126dd19ea2fa70ad2a'
else
  NGX_QUICKJS_SHA256=''
fi

# nginx-http-flv-module (NGINX_RTMP_HTTPFLV='y'), built in place of arut
# nginx-rtmp-module. The sha256 is checked for the default release only, a
# different NGINX_RTMP_HTTPFLVVER skips the check
NGX_RTMP_HTTPFLVLINKFILE="nginx-http-flv-module_v${NGINX_RTMP_HTTPFLVVER}.tar.gz"
NGX_RTMP_HTTPFLVLINK="https://github.com/winshining/nginx-http-flv-module/archive/v${NGINX_RTMP_HTTPFLVVER}.tar.gz"
if [[ "$NGINX_RTMP_HTTPFLVVER" = '1.2.15' ]]; then
  NGX_RTMP_HTTPFLV_SHA256='ac963f9802b9ec4cc00f1958526f17c1c17f26489604539138c7758985c797d4'
else
  NGX_RTMP_HTTPFLV_SHA256=''
fi

# optional nginx modules NGINX_AUTHJWT, NGINX_KEYVAL, NGINX_RATELIMIT and
# NGINX_OTEL. The sha256 is checked for the default release or commit only, a
# different version skips the check
# nginx-auth-jwt: the release '-full' asset includes the nxe-json, nxe-jwx and
# nxe-phase git submodules its config needs, the tag archive does not
NGX_AUTHJWTLINKFILE="nginx-auth-jwt-${NGINX_AUTHJWTVER}-full.tar.gz"
NGX_AUTHJWTLINK="https://github.com/kjdev/nginx-auth-jwt/releases/download/${NGINX_AUTHJWTVER}/${NGX_AUTHJWTLINKFILE}"
if [[ "$NGINX_AUTHJWTVER" = '0.15.0' ]]; then
  NGX_AUTHJWT_SHA256='5b6d6ba61ee2951a4eb826e1037293b281bfe05e4fc5b6ec75b94a6defeb082e'
else
  NGX_AUTHJWT_SHA256=''
fi

# nginx-keyval tags have no 'v' prefix
NGX_KEYVALLINKFILE="nginx-keyval_${NGINX_KEYVALVER}.tar.gz"
NGX_KEYVALLINK="https://github.com/kjdev/nginx-keyval/archive/${NGINX_KEYVALVER}.tar.gz"
if [[ "$NGINX_KEYVALVER" = '0.5.0' ]]; then
  NGX_KEYVAL_SHA256='893b59f121a959ddc84f8d26562cdcf9c63605c1e441b41ae01da37e28168f86'
else
  NGX_KEYVAL_SHA256=''
fi

# rate-limit-nginx-module has no release since v1.0.0 (2022), so a commit is pinned
NGX_RATELIMITLINKFILE="rate-limit-nginx-module_${NGINX_RATELIMITCOMMIT}.tar.gz"
NGX_RATELIMITLINK="https://github.com/weserv/rate-limit-nginx-module/archive/${NGINX_RATELIMITCOMMIT}.tar.gz"
if [[ "$NGINX_RATELIMITCOMMIT" = 'f603f4c7cf9b9ca1ce82256a14d6ab15bae0f2ad' ]]; then
  NGX_RATELIMIT_SHA256='63b84b21e5be3ad946a31faa358125db532432c4d247a5c5ed31feea21511c29'
else
  NGX_RATELIMIT_SHA256=''
fi

NGX_OTELLINKFILE="nginx-otel_v${NGINX_OTELVER}.tar.gz"
NGX_OTELLINK="https://github.com/nginx/nginx-otel/archive/v${NGINX_OTELVER}.tar.gz"
if [[ "$NGINX_OTELVER" = '0.1.2' ]]; then
  NGX_OTEL_SHA256='9bad197b916187e3340ff7a3d7df647053c0fb66df6e85f11be3f4aec1f14f56'
else
  NGX_OTEL_SHA256=''
fi

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
MASTER_NGX_FANCYINDEXLINKFILE="master-ngx-fancyindex-${NGINX_FANCYINDEXVER}.tar.gz"
NGX_FANCYINDEXLINK="https://github.com/aperezdc/ngx-fancyindex/archive/v${NGINX_FANCYINDEXVER}.tar.gz"
#NGX_FANCYINDEXLINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_FANCYINDEXLINKFILE}"

NGX_CACHEPURGEFILE="ngx_cache_purge-${NGINX_CACHEPURGEVER}.tar.gz"
MASTER_NGX_CACHEPURGEFILE="master-ngx_cache_purge-${NGINX_CACHEPURGEVER}.tar.gz"
NGX_CACHEPURGE="https://github.com/nginx-modules/ngx_cache_purge/archive/${NGINX_CACHEPURGEVER}.tar.gz"
#NGX_CACHEPURGE="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_CACHEPURGEFILE}"

NGX_ACCESSKEYLINKFILE="Nginx-accesskey-2.0.5.tar.gz"
MASTER_NGX_ACCESSKEYLINKFILE="master-Nginx-accesskey-2.0.5.tar.gz"
NGX_ACCESSKEYLINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/${NGX_ACCESSKEYLINKFILE}"

# wget renamed
NGX_CONCATLINKFILE="nginx-http-concat-master.tar.gz"
MASTER_NGX_CONCATLINKFILE="master-nginx-http-concat-master.tar.gz"
NGX_CONCATLINK='https://github.com/alibaba/nginx-http-concat/archive/master.tar.gz'
NGX_CONCATLINKGITLAB='https://gitlab.com/centminmod-github-mirror/nginx-http-concat/repository/archive.tar.gz?ref=master'

# wget renamed
NGX_HEADERSMORELINKFILE="headers-more-v${NGINX_HEADERSMORE}.tar.gz"
MASTER_NGX_HEADERSMORELINKFILE="master-headers-more-v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINK="https://github.com/openresty/headers-more-nginx-module/archive/v${NGINX_HEADERSMORE}.tar.gz"
# NGX_HEADERSMORELINK="https://github.com/centminmod/headers-more-nginx-module/archive/refs/tags/v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINKGITLAB="https://gitlab.com/centminmod-github-mirror/headers-more-nginx-module/repository/archive.tar.gz?ref=v${NGINX_HEADERSMORE}"

# wget renamed
NGX_STICKYLINKFILE="nginx-sticky-${NGINX_STICKYVER}.tar.gz"
MASTER_NGX_STICKYLINKFILE="master-nginx-sticky-${NGINX_STICKYVER}.tar.gz"
NGX_STICKYLINK="https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng/get/${NGINX_STICKYVER}.tar.gz"
NGX_STICKYLINKGITLAB="https://gitlab.com/centminmod-github-mirror/nginx-sticky-module-ng/repository/archive.tar.gz?ref=${NGINX_STICKYVER}"

# wget renamed
# the v0.4.0 tag only has check_1.20.1+.patch, NGINX_UPSTREAMCHECKCOMMIT pins the
# later master commit adding check_1.30.4+.patch for nginx 1.29.6+. The commit is
# in the archive name so a cached v0.4.0 tarball is not reused
if [ -n "$NGINX_UPSTREAMCHECKCOMMIT" ]; then
  NGX_UPSTREAMCHECKLINKFILE="nginx-upstream-check-${NGINX_UPSTREAMCHECKCOMMIT}.tar.gz"
  NGX_UPSTREAMCHECKLINK="https://github.com/yaoweibin/nginx_upstream_check_module/archive/${NGINX_UPSTREAMCHECKCOMMIT}.tar.gz"
else
  NGX_UPSTREAMCHECKLINKFILE="nginx-upstream-check-${NGINX_UPSTREAMCHECKVER}.tar.gz"
  NGX_UPSTREAMCHECKLINK="https://github.com/yaoweibin/nginx_upstream_check_module/archive/v${NGINX_UPSTREAMCHECKVER}.tar.gz"
fi
MASTER_NGX_UPSTREAMCHECKLINKFILE="master-nginx-upstream-check-${NGINX_UPSTREAMCHECKVER}.tar.gz"
NGX_UPSTREAMCHECKLINKGITLAB="https://gitlab.com/centminmod-github-mirror/nginx_upstream_check_module/repository/archive.tar.gz?ref=${NGINX_UPSTREAMCHECKVER}"

# wget renamed
NGX_MEMCLINKFILE="memc-nginx-module_v${ORESTY_MEMCVER}.tar.gz"
MASTER_NGX_MEMCLINKFILE="master-memc-nginx-module_v${ORESTY_MEMCVER}.tar.gz"
NGX_MEMCLINK="https://github.com/openresty/memc-nginx-module/archive/v${ORESTY_MEMCVER}.tar.gz"
NGX_MEMCLINKGITLAB="https://gitlab.com/centminmod-github-mirror/memc-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_MEMCVER}"

# wget renamed
NGX_SRCACHELINKFILE="srcache-nginx-module_v${ORESTY_SRCCACHEVER}.tar.gz"
MASTER_NGX_SRCACHELINKFILE="master-srcache-nginx-module_v${ORESTY_SRCCACHEVER}.tar.gz"
NGX_SRCACHELINK="https://github.com/openresty/srcache-nginx-module/archive/v${ORESTY_SRCCACHEVER}.tar.gz"
NGX_SRCACHELINKGITLAB="https://gitlab.com/centminmod-github-mirror/srcache-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_SRCCACHEVER}"

# wget renamed
NGX_DEVELKITLINKFILE="nginx-develkit_v${ORESTY_DEVELKITVER}.tar.gz"
MASTER_NGX_DEVELKITLINKFILE="master-nginx-develkit_v${ORESTY_DEVELKITVER}.tar.gz"
# ngx_devel_kit 0.3.5+ only exists in OpenResty's fork (vision5 stops at 0.3.4
# and the fork has no older tags); 0.3.5+ carries the nginx 1.31.3+/1.30.4+
# complex value end code fix, detected at configure time
if [[ "$(printf '%s\n' "$ORESTY_DEVELKITVER" '0.3.5' | sort -V | head -n1)" = '0.3.5' ]]; then
  NGX_DEVELKITLINK="https://github.com/openresty/ngx_devel_kit/archive/v${ORESTY_DEVELKITVER}.tar.gz"
else
  NGX_DEVELKITLINK="https://github.com/vision5/ngx_devel_kit/archive/v${ORESTY_DEVELKITVER}.tar.gz"
fi
NGX_DEVELKITLINKGITLAB="https://gitlab.com/centminmod-github-mirror/ngx_devel_kit/repository/archive.tar.gz?ref=v${ORESTY_DEVELKITVER}"

# wget renamed
NGX_SETMISCLINKFILE="set-misc-nginx-module_v${ORESTY_SETMISCVER}.tar.gz"
MASTER_NGX_SETMISCLINKFILE="master-set-misc-nginx-module_v${ORESTY_SETMISCVER}.tar.gz"
NGX_SETMISCLINK="https://github.com/openresty/set-misc-nginx-module/archive/v${ORESTY_SETMISCVER}.tar.gz"
NGX_SETMISCLINKGITLAB="https://gitlab.com/centminmod-github-mirror/set-misc-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_SETMISCVER}"

# wget renamed
NGX_ECHOLINKFILE="echo-nginx-module_v${ORESTY_ECHOVER}.tar.gz"
MASTER_NGX_ECHOLINKFILE="master-echo-nginx-module_v${ORESTY_ECHOVER}.tar.gz"
NGX_ECHOLINK="https://github.com/openresty/echo-nginx-module/archive/v${ORESTY_ECHOVER}.tar.gz"
NGX_ECHOLINKGITLAB="https://gitlab.com/centminmod-github-mirror/echo-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_ECHOVER}"

# wget renamed
NGX_REDISLINKFILE="redis2-nginx-module_v${ORESTY_REDISVER}.tar.gz"
MASTER_NGX_REDISLINKFILE="master-redis2-nginx-module_v${ORESTY_REDISVER}.tar.gz"
NGX_REDISLINK="https://github.com/openresty/redis2-nginx-module/archive/v${ORESTY_REDISVER}.tar.gz"
NGX_REDISLINKGITLAB="https://gitlab.com/centminmod-github-mirror/redis2-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_REDISVER}"

# wget renamed
NGX_HTTPREDISLINKFILE="ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"
MASTER_NGX_HTTPREDISLINKFILE="master-ngx_http_redis-${NGINX_HTTPREDISVER}.tar.gz"
# NGX_HTTPREDISLINK="https://github.com/osokin/ngx_http_redis/archive/refs/tags/${NGINX_HTTPREDISVER}.tar.gz"
NGX_HTTPREDISLINK="https://github.com/centminmod/ngx_http_redis/archive/refs/tags/${NGINX_HTTPREDISVER}.tar.gz"

##############
# lua

NGX_LUANGINXLINKFILE="lua-nginx-module_v${ORESTY_LUANGINXVER}.tar.gz"
NGX_LUANGINXLINK="https://github.com/openresty/lua-nginx-module/archive/v${ORESTY_LUANGINXVER}.tar.gz"
NGX_LUANGINXLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUANGINXVER}"

NGX_LUASTREAMLINKFILE="stream-lua-nginx-module_v${ORESTY_LUASTREAMVER}.tar.gz"
NGX_LUASTREAMLINK="https://github.com/openresty/stream-lua-nginx-module/archive/v${ORESTY_LUASTREAMVER}.tar.gz"
#NGX_LUASTREAMLINKGITLAB="https://gitlab.com/centminmod-github-mirror/lua-nginx-module/repository/archive.tar.gz?ref=v${ORESTY_LUASTREAMVER}"

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

# optional pure Lua libraries, each behind its own ORESTY_LUA* toggle and only
# with ORESTY_LUANGINX='y'. The sha256 is checked for the default release only,
# a different version skips the check
NGX_LUALIMITTRAFFICLINKFILE="lua-resty-limit-traffic_v${ORESTY_LUALIMITTRAFFICVER}.tar.gz"
NGX_LUALIMITTRAFFICLINK="https://github.com/openresty/lua-resty-limit-traffic/archive/v${ORESTY_LUALIMITTRAFFICVER}.tar.gz"
if [[ "$ORESTY_LUALIMITTRAFFICVER" = '0.09' ]]; then
  NGX_LUALIMITTRAFFIC_SHA256='547ee6942939928407781dc938723f81c2a619d492f514826e38544f5791bbca'
else
  NGX_LUALIMITTRAFFIC_SHA256=''
fi

NGX_LUAHTTPLINKFILE="lua-resty-http_v${ORESTY_LUAHTTPVER}.tar.gz"
NGX_LUAHTTPLINK="https://github.com/ledgetech/lua-resty-http/archive/v${ORESTY_LUAHTTPVER}.tar.gz"
if [[ "$ORESTY_LUAHTTPVER" = '0.18.0' ]]; then
  NGX_LUAHTTP_SHA256='d84e87bfea7bc1b91c52a27c9464d1412823ce748fd13d3885308f21166f946b'
else
  NGX_LUAHTTP_SHA256=''
fi

# lua-resty-openssl tags have no 'v' prefix
NGX_LUAOPENSSLLINKFILE="lua-resty-openssl_${ORESTY_LUAOPENSSLVER}.tar.gz"
NGX_LUAOPENSSLLINK="https://github.com/fffonion/lua-resty-openssl/archive/${ORESTY_LUAOPENSSLVER}.tar.gz"
if [[ "$ORESTY_LUAOPENSSLVER" = '1.9.0' ]]; then
  NGX_LUAOPENSSL_SHA256='d074ac5e6a2bc9c1bd57786b8d5784b6d13681d2fdc4dd7b215eb20f0bd6189c'
else
  NGX_LUAOPENSSL_SHA256=''
fi

# nginx-lua-prometheus tags have no 'v' prefix
NGX_LUAPROMETHEUSLINKFILE="nginx-lua-prometheus_${ORESTY_LUAPROMETHEUSVER}.tar.gz"
NGX_LUAPROMETHEUSLINK="https://github.com/knyar/nginx-lua-prometheus/archive/${ORESTY_LUAPROMETHEUSVER}.tar.gz"
if [[ "$ORESTY_LUAPROMETHEUSVER" = '0.20260912' ]]; then
  NGX_LUAPROMETHEUS_SHA256='c2f42db365feaab20206465b06cd3b700f1157336c10ecc55675c04577dcc911'
else
  NGX_LUAPROMETHEUS_SHA256=''
fi

NGX_LUAJA4LINKFILE="lua-resty-ja4_v${ORESTY_LUAJA4VER}.tar.gz"
NGX_LUAJA4LINK="https://github.com/nemethhh/lua-resty-ja4/archive/v${ORESTY_LUAJA4VER}.tar.gz"
if [[ "$ORESTY_LUAJA4VER" = '0.2.1' ]]; then
  NGX_LUAJA4_SHA256='90239624288f548349a1b81261a24209492acddeb2d3ca7a91dc8b2a2cb62929'
else
  NGX_LUAJA4_SHA256=''
fi

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

PHPEXT_SWOOLELINKFILE="swoole-${PHPSWOOLE_VER}.tgz"
PHPEXT_SWOOLELINK="https://pecl.php.net/get/${PHPEXT_SWOOLELINKFILE}"
PHPEXT_SWOOLELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/swoole/${PHPEXT_SWOOLELINKFILE}"

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
FREENGX_LINKFILE="freenginx-${FREENGINX_VERSION}.tar.gz"
FREENGX_LINK="https://freenginx.org/download/${FREENGX_LINKFILE}"

NGX_VTSLINK='https://github.com/vozlt/nginx-module-vts.git'
NGX_VTSLINKGITLAB='https://gitlab.com/centminmod-github-mirror/nginx-module-vts.git'

OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
MASTER_OPENSSL_LINKFILE="master-openssl${OPENSSL_VERSION}.tar.gz"
OPENSSL_LINK="https://github.com/openssl/openssl/releases/download/openssl-${OPENSSL_VERSION}/${OPENSSL_LINKFILE}"
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
CSF_LINK="https://download.centminmod.com/${CSF_LINKFILE}"

EZSETUPLINKFILE='ez_setup.py'
EZSETUPLINK="https://bitbucket.org/pypa/setuptools/raw/bootstrap/${EZSETUPLINKFILE}"

# wget renamed github
AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
AXEL_LINKGITLAB="https://gitlab.com/centminmod-github-mirror/axel/repository/archive.tar.gz?ref=${AXEL_VER}"
AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"

LIBUNWIND_LINKFILE="libunwind-${LIBUNWIND_VERSION}.tar.gz"
LIBUNWIND_LINKDIR="libunwind-${LIBUNWIND_VERSION}"
LIBUNWIND_LINK="https://download.savannah.gnu.org/releases/libunwind/${LIBUNWIND_LINKFILE}"
LIBUNWIND_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/gperf/${LIBUNWIND_LINKFILE}"

GPERFTOOL_LINKFILE="gperftools-${GPERFTOOLS_VERSION}.tar.gz"
GPERFTOOL_LINKDIR="gperftools-${GPERFTOOLS_VERSION}"
GPERFTOOL_LINK="https://github.com/gperftools/gperftools/releases/download/gperftools-${GPERFTOOLS_VERSION}/${GPERFTOOL_LINKFILE}"
GPERFTOOL_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/gperf/${GPERFTOOL_LINKFILE}"

LIBZIP_LINKFILE="libzip-${LIBZIP_VER}.tar.gz"
LIBZIP_LINK="https://github.com/nih-at/libzip/releases/download/v${LIBZIP_VER}/${LIBZIP_LINKFILE}"
