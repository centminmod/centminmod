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

CENTOSFIVEEPELFILE="epel-release-5-4.noarch.rpm"
CENTOSFIVEEPEL="http://epel.mirror.constant.com/5/${ARCH}/${CENTOSFIVEEPELFILE}"

CENTOSFIVERPMFORGEFILE="rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm"
CENTOSFIVERPMFORGE="http://pkgs.repoforge.org/rpmforge-release/${CENTOSFIVERPMFORGEFILE}"

CENTOSFIVECENTALTFILE="centalt-release-5-3.noarch.rpm"
CENTOSFIVECENTALT="http://centos.alt.ru/repository/centos/5/${RPMFARCH}/${CENTOSFIVECENTALTFILE}"

CENTOSFIVEPERCONARELEASEFILE="percona-release-0.1-3.noarch.rpm"
CENTOSFIVEPERCONARELEASE="http://www.percona.com/redir/downloads/percona-release/redhat/latest/${CENTOSFIVEPERCONARELEASEFILE}"

CENTOSFIVEVARNISHFILE='varnish-release-3.0-1.el5.centos.noarch.rpm'
CENTOSFIVEVARNISH="http://repo.varnish-cache.org/redhat/varnish-3.0/el5/noarch/varnish-release/${CENTOSFIVEVARNISHFILE}"

CENTOSSIXEPELFILE="epel-release-6-8.noarch.rpm"
CENTOSSIXEPEL="http://epel.mirror.constant.com/6/${RPMFARCH}/${CENTOSSIXEPELFILE}"
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

PCRELINKFILE="pcre-${NGINX_PCREVER}.tar.gz"
PCRELINK="http://downloads.sourceforge.net/project/pcre/pcre/${NGINX_PCREVER}/${PCRELINKFILE}"
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

NGX_FANCYINDEXLINKFILE="ngx-fancyindex-0.3.1.tar.gz"
NGX_FANCYINDEXLINK="http://centminmod.com/centminmodparts/nginx/modules/${NGX_FANCYINDEXLINKFILE}"

NGX_CACHEPURGEFILE="ngx_cache_purge-2.1.tar.gz"
NGX_CACHEPURGE="http://centminmod.com/centminmodparts/nginx/modules/${NGX_CACHEPURGEFILE}"

NGX_ACCESSKEYLINKFILE="Nginx-accesskey-2.0.3.tar.gz"
NGX_ACCESSKEYLINK="http://wiki.nginx.org/images/5/51/${NGX_ACCESSKEYLINKFILE}"

# wget renamed
NGX_CONCATLINKFILE="nginx-http-concat-master.tar.gz"
NGX_CONCATLINK='https://github.com/alibaba/nginx-http-concat/archive/master.tar.gz'

# wget renamed
NGX_HEADERSMORELINKFILE="headers-more-v${NGINX_HEADERSMORE}.tar.gz"
NGX_HEADERSMORELINK="https://github.com/agentzh/headers-more-nginx-module/archive/v${NGINX_HEADERSMORE}.tar.gz"

# wget renamed
NGX_MEMCLINKFILE="memc-nginx-module_v0.13rc3.tar.gz"
NGX_MEMCLINK="https://github.com/agentzh/memc-nginx-module/tarball/v0.13rc3"

# wget renamed
NGX_SRCACHELINKFILE="srcache-nginx-module_v0.16.tar.gz"
NGX_SRCACHELINK="https://github.com/agentzh/srcache-nginx-module/tarball/v0.16"

LIBEVENTLINKFILE="release-${LIBEVENT_VERSION}-stable.tar.gz"
LIBEVENTLINKOLD="https://github.com/downloads/libevent/libevent/${LIBEVENTLINKFILE}"
LIBEVENTLINK="https://github.com/libevent/libevent/archive/${LIBEVENTLINKFILE}"

PHPEXT_IMAGICKLINKFILE="imagick-${IMAGICKPHP_VER}.tgz"
PHPEXT_IMAGICKLINK="http://pecl.php.net/get/${PHPEXT_IMAGICKLINKFILE}"
PHPEXT_IMAGICKLINKLOCAL="http://centminmod.com/centminmodparts/imagick/${PHPEXT_IMAGICKLINKFILE}"

PHPEXT_MEMCACHELINKFILE="memcache-${MEMCACHE_VERSION}.tgz"
PHPEXT_MEMCACHELINK="http://pecl.php.net/get/${PHPEXT_MEMCACHELINKFILE}"
PHPEXT_MEMCACHELINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_MEMCACHELINKFILE}"

PHPEXT_MEMCACHEDLINKFILE="memcached-${MEMCACHEDPHP_VER}.tgz"
PHPEXT_MEMCACHEDLINK="http://pecl.php.net/get/${PHPEXT_MEMCACHEDLINKFILE}"
PHPEXT_MEMCACHEDLINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_MEMCACHEDLINKFILE}"

PHPEXT_LIBMEMCACHEDLINKFILE="libmemcached-${LIBMEMCACHED_VER}.tar.gz"
PHPEXT_LIBMEMCACHEDLINK="https://launchpad.net/libmemcached/1.0/${LIBMEMCACHED_VER}/+download/${PHPEXT_LIBMEMCACHEDLINKFILE}"
PHPEXT_LIBMEMCACHEDLINKLOCAL="http://centminmod.com/centminmodparts/memcached/${PHPEXT_LIBMEMCACHEDLINKFILE}"

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

OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
#OPENSSL_LINK="http://www.openssl.org/source/${OPENSSL_LINKFILE}"
OPENSSL_LINK="http://centminmod.com/centminmodparts/openssl/${OPENSSL_LINKFILE}"

SIEGE_LINKFILE="siege-${SIEGE_VERSION}.tar.gz"
SIEGE_LINK="http://www.joedog.org/pub/siege/${SIEGE_LINKFILE}"
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
APCCACHE_LINK="http://pecl.php.net/get/${APCCACHE_LINKFILE}"
APCCACHE_LINKLOCAL="http://centminmod.com/centminmodparts/apc/php550/${APCCACHE_LINKFILE}"

ZENDOPCACHE_LINKFILE="zendopcache-${ZOPCACHECACHE_VERSION}.tgz"
ZENDOPCACHE_LINK="http://pecl.php.net/get/${ZENDOPCACHE_LINKFILE}"
ZENDOPCACHE_LINKLOCAL="http://centminmod.com/centminmodparts/zendopcache/${ZENDOPCACHE_LINKFILE}"

NSD_LINKFILE="nsd-${NSD_VERSION}.tar.gz"
#NSD_LINK="http://www.nlnetlabs.nl/downloads/nsd/${NSD_LINKFILE}"
NSD_LINK="http://centminmod.com/centminmodparts/nsd/${NSD_LINKFILE}"

CSF_LINKFILE="csf.tgz"
CSF_LINK="http://www.configserver.com/free/${CSF_LINKFILE}"

EZSETUPLINKFILE='ez_setup.py'
EZSETUPLINK="https://bitbucket.org/pypa/setuptools/raw/bootstrap/${EZSETUPLINKFILE}"