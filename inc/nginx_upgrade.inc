check_requestscheme() {
# check for REQUEST_SCHEME parameter added in nginx 1.9.2 
# add it if it doesn't exist in fastcgi_param and php include files
for f in "$(grep -Rl 'fastcgi_param  SERVER_PROTOCOL    \$server_protocol;' /usr/local/nginx/conf/*)"; 
  do
    # only proceed with routine if the file referenced in $f variable exists
    if [ -f "$f" ]; then
        echo "$f"
        ff=$(grep 'REQUEST_SCHEME' "$f")
        fff=$(grep 'https if_not_empty' "$f")
        if [[ -z "$ff" && -z "$fff" ]]; then
        sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;\nfastcgi_param  HTTPS              \$https     if_not_empty;|" "$f"
        elif [[ -z "$ff" && "$fff" ]]; then
            sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;|" "$f"
        fi
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

    if [[ ! -z "$GEOIP_CHECK" && "$(grep 'NGINX_GEOIP=n' "${SCRIPT_DIR}/centmin.sh")" ]]; then
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
    \cp "$CUR_DIR/config/nginx/vts_http.conf" /usr/local/nginx/conf/vts_http.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_mainserver.conf ]]; then
    \cp "$CUR_DIR/config/nginx/vts_mainserver.conf" /usr/local/nginx/conf/vts_mainserver.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_server.conf ]]; then
    \cp "$CUR_DIR/config/nginx/vts_server.conf" /usr/local/nginx/conf/vts_server.conf
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
  \cp "$CUR_DIR/config/nginx/fastcgi_param_https_map.conf" /usr/local/nginx/conf/fastcgi_param_https_map.conf
fi

if [[ -z "$(grep 'fastcgi_param HTTPS $server_https;' /usr/local/nginx/conf/php.conf)" ]]; then
  replace -s '#fastcgi_param HTTPS on;' 'fastcgi_param HTTPS $server_https;' -- /usr/local/nginx/conf/php.conf
fi
}

checknginxmodules() {
    axelsetup

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

if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    # lua nginx 0.10.7 and lower not compatible with openssl 1.1.x branch
    # so fall back to 1.0.2 branch if lua nginx module is enabled
    OPENSSL_VERSION="$OPENSSL_VERSIONFALLBACK"
    OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
    OPENSSL_LINK="https://www.openssl.org/source/${OPENSSL_LINKFILE}"
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

cecho "Check for old ngx_pagespeed master branch existence" $boldyellow
if [[ -d "${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}/ngx_pagespeed-master" || -d "${DIR_TMP}/incubator-pagespeed-ngx-master" ]]; then
    # rm -rf ${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}
    rm -rf ${DIR_TMP}/ngx_pagespeed-release-*
    rm -rf ${DIR_TMP}/ngx_pagespeed-*
    rm -rf ${DIR_TMP}/release-1.9.32*
    rm -rf ${DIR_TMP}/incubator-pagespeed-ngx-master*
    nginxpgspeedtarball
fi

cecho "Check for missing nginx modules" $boldyellow

LIBRESSLDIR=$(tar -tzf "$DIR_TMP/${LIBRESSL_LINKFILE}" 2>&1 | head -1 | cut -f1 -d"/" | grep libressl)
if [[ ! -f "${DIR_TMP}/${LIBRESSL_LINKFILE}" || ! -d "${DIR_TMP}/${LIBRESSLDIR}" ]]; then
    libressldownload
elif [[ ! -f "${DIR_TMP}/${LIBRESSL_LINKFILE}" ]]; then
    libressldownload
fi

if [[ ! -f "${DIR_TMP}/${NGX_ZLIBLINKFILE}" || ! -d "${DIR_TMP}/zlib-${NGINX_ZLIBVER}" ]]; then
    nginxzlibtarball
fi

if [[ ! -f "${DIR_TMP}/${NGX_FANCYINDEXLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CACHEPURGEFILE}" || ! -f "${DIR_TMP}/${NGX_ACCESSKEYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CONCATLINKFILE}" || ! -f "${DIR_TMP}/${OPENSSL_LINKFILE}" || ! -f "${DIR_TMP}/${PCRELINKFILE}" || ! -f "${DIR_TMP}/${NGX_WEBDAVLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HEADERSMORELINKFILE}" || ! -f "${DIR_TMP}/${NGX_STICKYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_UPSTREAMCHECKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HTTPREDISLINKFILE}" ]] || [[ ! -d "${DIR_TMP}/${NGX_FANCYINDEXDIR}" || ! -d "${DIR_TMP}/${NGX_CACHEPURGEDIR}" || ! -d "${DIR_TMP}/nginx-accesskey-2.0.3" || ! -d "${DIR_TMP}/${NGX_CONCATDIR}" || ! -d "${DIR_TMP}/${OPENSSLDIR}" || ! -d "${DIR_TMP}/${PCRELINKDIR}" || ! -d "${DIR_TMP}/${NGX_WEBDAVLINKDIR}" || ! -d "${DIR_TMP}/${NGX_HEADERSMOREDIR}" || ! -d "${DIR_TMP}/${NGX_STICKYDIR}" || ! -d "${DIR_TMP}/${NGX_UPSTREAMCHECKDIR}" || ! -d "${DIR_TMP}/${NGX_HTTPREDISDIR}" ]]; then

    if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep "processor" /proc/cpuinfo |wc -l)" -gt '1' ]]; then
        ngxmoduletarball &
        if [[ "$LIBRESSL_SWITCH" = 'n' ]]; then
            openssldownload &
        fi
        # libressldownload &
        wait
    else
        ngxmoduletarball
        if [[ "$LIBRESSL_SWITCH" = 'n' ]]; then
            openssldownload
        fi
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
    if [[ ! -f "${DIR_TMP}/${NGX_MEMCLINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_MEMCLINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ ! -f "${DIR_TMP}/${NGX_SRCACHELINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_SRCACHELINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ ! -f "${DIR_TMP}/${NGX_REDISLINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_REDISLINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ ! -f "${DIR_TMP}/${NGX_ECHOLINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_ECHOLINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ ! -f "${DIR_TMP}/${NGX_SETMISCLINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_SETMISCLINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ ! -f "${DIR_TMP}/${NGX_DEVELKITLINKFILE}" || "$(tar -tzf ${DIR_TMP}/${NGX_DEVELKITLINKFILE} >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
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

clear_ps() {
    if [ -d /var/ngx_pagespeed_cache ]; then
        rm -rf /var/ngx_pagespeed_cache/*
    fi
}

function funct_nginxupgrade {

    checkmap
    # if [ ! -f /usr/local/bin/pcretest ]; then
    #     wgetver_check
    # fi

cecho "**********************************************************************" $boldyellow
cecho "* Nginx Update script - Included in Centmin Extras" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE - $COPYRIGHT" $boldgreen
cecho "**********************************************************************" $boldyellow
echo " "
cecho "This software comes with no warranty of any kind. You are free to use" $boldyellow
cecho "it for both personal and commercial use as licensed under the GPL." $boldyellow
echo " "
if [ -f /usr/local/lib/libz.so ]; then
    echo "Upgrade Note:"
    echo "This nginx recompile involves zlib changes which have one time"
    echo "service restarts including MariaDB MySQL restart to switch"
    echo "from custom zlib shared library back to system zlib library"
    echo "for services other than Nginx. Nginx will use custom zlib"
    echo "by itself while all other services will revert back to system"
    echo "zlib shared library"
fi
echo
if [[ "$UALL" = 'y' ]]; then 
    nukey=y
else
    read -ep "Nginx Upgrade - Would you like to continue? [y/n] " nukey
fi

if [[ "$nukey" = [nN] ]];
then
    exit 0
fi

# DIR_TMP="/svr-setup"
if [ ! -d "$DIR_TMP" ]; then
mkdir "$DIR_TMP"
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

if [[ "$UALL" = 'y' ]]; then 
    ngver=$NGINX_VERSION
    recompileopenssl='n'
else
    echo ""
    read -ep "Install which version of Nginx? (version i.e. type $NGINX_VERSION): " ngver
    service nginx reload >/dev/null 2>&1

    # if user forgets to type in nginx version and hits
    # enter, ngxver will be empty so should fall back to
    # the nginx version defined by NGINX_VERSION variable
    if [ -z "$ngver" ]; then
        LASTEST_NGINXVERS=$(curl -${ipv_forceopt}sL https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n 1 2>&1)
        ngver="$LASTEST_NGINXVERS"
    fi

    # if you set /etc/centminmod/custom_config.inc file to
    # NGINX_UPDATEMAINTENANCE=y
    # then whenever you run nginx upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$NGINX_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus off
        fi
    fi

    # auto check if static compiled Nginx openssl version matches
    # the one defined in centmin.sh OPENSSL_VERSION variable
    # if doesn't match then auto recompile the statically set
    # OPENSSL_VERSION
    AUTOOPENSSLCHECK=$(nginx -V 2>&1 | grep -Eo "$OPENSSL_VERSION")
    if [[ "$AUTOOPENSSLCHECK" ]]; then
        recompileopenssl='y'
    else
        recompileopenssl='y'
    fi
    echo ""
    checkgeoip
fi # UALL

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Auto Backup Conf Directories"
    fi

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

    cd "$DIR_TMP"

    # nginx Modules / Prerequisites
  cecho "Installing nginx Modules / Prerequisites..." $boldgreen

checknginxmodules
nginxzlib_install
install_gperftools

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
    cd "$DIR_TMP"

    echo "Compiling PCRE..."
    if [ -s "pcre-${PCRE_VERSION}.tar.gz" ]; then
        cecho "pcre ${PCRE_VERSION} Archive found, skipping download..." $boldgreen
    else
        download_cmd "${PCRE_SOURCELINK}" $WGETRETRY
    fi

    tar "xvzf pcre-${PCRE_VERSION}.tar.gz"
    cd "pcre-${PCRE_VERSION}"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure
    make${MAKETHREADS}
    make install

fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "luajit install"
    fi

luajitinstall

funct_nginxmodules

check_requestscheme

    # Install nginx
    cd "$DIR_TMP"

    CUR_NGINXVER=$(nginx -v 2>&1 | awk -F '\\/' '{print $2}' |sed -e 's|\.|0|g' | head -n1)
    CUR_NGINXUPGRADEVER=$(echo "$ngver" |sed -e 's|\.|0|g' | head -n1)    

if [[ "$NGINXPATCH" = [nN] || "$NGINX_HTTP2" = [nN] || "$NGINX_DYNAMICTLS" = [nN] ]]; then
    # if centmin.sh option NGINXPATCH=n then disable patches by 
    # wiping the nginx downloaded source and redownloading a fresh copy
    rm -rf nginx-${ngver}*
fi

if [[ "$NGINX_SPDYPATCHED" = [nN] && "$(grep 'ngx_http_spdy_filter' $DIR_TMP/nginx-${ngver}/auto/modules)" ]]; then
    # if centmin.sh option NGINX_SPDYPATCHED=n then disable patches by 
    # wiping the nginx downloaded source and redownloading a fresh copy
    rm -rf nginx-${ngver}*
fi

# if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
#     # if existing Nginx server is detected to have HTTP/2 patch compiled, then
#     # wipe nginx download source and redownload a fresh copy to ensure you're
#     # patching with latest patch http://nginx.org/patches/http2/ available
#     rm -rf nginx-${ngver}*
# fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Compile Nginx"
    fi

    echo "Compiling nginx..."
    if [[ "$ngver" = 'master' ]]; then
        rm -rf nginx-${ngver}*
        git clone --depth=1 https://github.com/nginx/nginx nginx-master
        mv nginx-master/auto/configure nginx-master/configure
    fi
    if [[ "$ngver" = 'master-google' ]]; then
        rm -rf nginx-${ngver}*
        git clone --depth=1 https://nginx.googlesource.com/nginx nginx-master-google
        mv nginx-master-google/auto/configure nginx-master-google/configure
    fi
    if [[ -s "nginx-${ngver}.tar.gz" || "$ngver" = 'master' || "$ngver" = 'master-google' ]]; then
        cecho "nginx ${ngver} download found, skipping download..." $boldgreen
    else
        download_cmd "https://nginx.org/download/nginx-${ngver}.tar.gz" $WGETRETRY
        NGINXDOWNLOAD_CHECK=$?
        if [[ "$NGINXDOWNLOAD_CHECK" != '0' ]]; then
            wget -c${ipv_forceopt} "https://nginx.org/download/nginx-${ngver}.tar.gz" --tries=3
            NGINXDOWNLOAD_CHECK=$?
            if [[ "$NGINXDOWNLOAD_CHECK" != '0' ]]; then
                if [ -f "$(which figlet)" ]; then
                    figlet -ckf standard "Nginx Failed Download"
                fi
                echo
                echo "!! Nginx Failed Download !!"
                echo "try again by running centmin.sh menu option 4"
                echo "make sure you type the correct nginx version"
                echo "report errors on the forums at community.centminmod.com/forums/8/"
            echo
            free -m
            echo
            df -hT
            echo
            echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
            echo
            echo "version increment history"
            tail -5 /etc/centminmod-versionlog
            if [ -d "${SCRIPT_DIR}/.git" ]; then
                echo
                echo "last Centmin Mod local git commit entry"
                pushd "${SCRIPT_DIR}"
                git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
                popd
            fi
                echo
                exit
            fi
        fi
    fi

    if [[ ! "$(echo $ngver | grep 'master')" ]]; then
        tar xvfz "nginx-${ngver}.tar.gz"
        NGINXTAREXTRACT_CHECK=$?
        if [[ "$NGINXTAREXTRACT_CHECK" != '0' ]]; then
            if [ -f "$(which figlet)" ]; then
                figlet -ckf standard "Nginx Tarball Extraction Failed"
            fi
            echo
            echo "!! Nginx Tarball Extraction Failed !!"
            echo "try again by running centmin.sh menu option 4"
            echo "make sure you type the correct nginx version"
            echo "report errors on the forums at community.centminmod.com/forums/8/"
            echo
            free -m
            echo
            df -hT
            echo
            echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
            echo
            echo
            echo "version increment history"
            tail -5 /etc/centminmod-versionlog
            if [ -d "${SCRIPT_DIR}/.git" ]; then
                echo
                echo "last Centmin Mod local git commit entry"
                pushd "${SCRIPT_DIR}"
                git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
                popd
            fi
    
            rm -rf "$DIR_TMP/nginx-*.tar.gz"
            exit
        fi
    fi
    cd "nginx-${ngver}"
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

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Configure"
    fi

if [[ "$TIME_NGINX" = [yY] ]]; then
    funct_nginxconfigure 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
else
    funct_nginxconfigure
fi

################
# error check

  ERR=$?
  if [ $ERR != 0 ]; then
      echo -e "\n`date`\nError: $ERR, Nginx configure failed\n"
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi
        echo
        echo "log files saved at ${CENTMINLOGDIR}"
        ls -lAhrt "${CENTMINLOGDIR}" | grep $DT
        exit
  else
      echo -e "\n`date`\nSuccess: Nginx configure ok\n"
  fi

# error check
################

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Make"
    fi

    # detect OpenSSL 1.1.0 for make -jXX support
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
    DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)

    if [[ "$BORINGSSL_SWITCH" = [yY] ]]; then
      touch ${BORINGSSL_DIR}/boringssl/.openssl/include/openssl/ssl.h
      ls -lah ${BORINGSSL_DIR}/boringssl/.openssl/include/openssl/ssl.h
    fi

    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        time make${MAKETHREADS}
    else
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
            time make${MAKETHREADS}
        else
            time make
        fi
    fi
    
    ERR=$?
    sar_call

    if [[ "$STRIPNGINX" = [yY] && "$NGINX_DEBUG" != [yY] ]]; then
        if [ -f objs/nginx ]; then
            echo
            echo "strip nginx binary..."
            ls -lah objs/nginx
            strip -s objs/nginx
            ls -lah objs/nginx
            echo
        fi
    fi

################
# error check

  if [ $ERR != 0 ]; then
      echo -e "\n`date`\nError: $ERR, Nginx make failed\n"
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi
        echo
        echo "log files saved at ${CENTMINLOGDIR}"
        ls -lAhrt "${CENTMINLOGDIR}" | grep $DT
      exit
  else
     echo -e "\n`date`\nSuccess: Nginx make ok\n"
  fi

# error check
################

        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            systemctl daemon-reload
        fi

  GETNGXPID=$(cat /usr/local/nginx/logs/nginx.pid)
  GETNGXPPID=$(ps awx -o pid,ppid,command | grep 'nginx: master' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep | awk '{print $1}')

if [[ "$NGINX_ZERODT" = [nN] ]]; then
  # cmservice nginx stop
  /usr/local/sbin/nginx -s stop >/dev/null 2>&1
  
      # speed up nginx wait time if not many vhosts are on server
      if [[ "$(ls /usr/local/nginx/conf/conf.d/ | wc -l)" -le 5 ]]; then
          NGINXUPGRADESLEEP=4
      fi
  
  # sleep "$NGINXUPGRADESLEEP"
  
  sleep 3
  NGINXPSCHECK=$(ps --no-heading -C nginx)
  
  if [ ! -z "$NGINXPSCHECK" ]; then
  echo ""
  echo "nginx seems to be still running, trying to stop it again..."
  echo ""
  # /etc/init.d/nginx stop and suppress any error messages
  /usr/local/sbin/nginx -s stop >/dev/null 2>&1
  sleep "$NGINXUPGRADESLEEP"
  fi
elif [[ "$NGINX_ZERODT" = [yY] ]]; then
  echo "---------------------------------------------------------------------------"
  echo "nginx master id: $GETNGXPID"
  echo
  echo "---------------------------------------------------------------------------"
  curl -s -4 localhost/nginx_status
  echo "---------------------------------------------------------------------------"
  ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep '(nginx: |PID)' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep
  echo "---------------------------------------------------------------------------"
fi
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Make Install"
    fi

    time make install
    ERR=$?
    sar_call

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
    if [ $ERR = 0 ]; then
        detect_tlsonethree
    fi
  if [ $ERR != 0 ]; then
      echo -e "\n`date`\nError: $ERR, Nginx wasn't installed properly\n"
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi

        if [ -f objs/autoconf.err ]; then
            cat objs/autoconf.err > "${CENTMINLOGDIR}/nginx_autoconf.err.${DT}.log"
        fi
        echo
        echo "log files saved at ${CENTMINLOGDIR}"
        ls -lAhrt "${CENTMINLOGDIR}" | grep $DT
      exit
  else
        if [ -f objs/autoconf.err ]; then
            cat objs/autoconf.err > "${CENTMINLOGDIR}/nginx_autoconf.err.${DT}.log"
        fi
      echo -e "\n`date`\nSuccess: Nginx was installed properly\n"

    if [[ "$NGINX_HTTP2" = [yY] && "$NGINX_SPDYPATCHED" = [yY] ]] && [[ "$NGX_VEREVAL" -ge '10907' ]]; then
        # only apply auto vhost changes forNginx HTTP/2 
        # if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
        if [[ "$NGX_VEREVAL" -ge '10907' ]]; then
            for v in "$(ls /usr/local/nginx/conf/conf.d/*.conf)"; do echo "$v"; egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v; echo "---"; sed -i 's|ssl http2|ssl spdy http2|g' $v; sed -i 's|#  spdy_headers_comp|  spdy_headers_comp|g' $v; sed -i 's|#  add_header Alternate-Protocol|  add_header Alternate-Protocol|g' $v; egrep -n 'ssl http2|spdy_headers_comp|Alternate-Protocol' $v;done
        fi
        if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
            sed -i 's|ssl http2|ssl spdy http2|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#spdy_headers_comp|spdy_headers_comp|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#add_header Alternate-Protocol|add_header Alternate-Protocol|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
        fi
    elif [[ "$NGINX_HTTP2" = [yY] ]] && [[ "$NGX_VEREVAL" -ge '10903' ]]; then
        # only apply auto vhost changes forNginx HTTP/2 
        # if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
        if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]]; then
            for v in "$(ls /usr/local/nginx/conf/conf.d/*.conf)"; do 
                echo "$v"; 
                egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v; 
                echo "---";
                if [[ "$(grep 'ssl spdy http2' $v)" ]]; then
                    sed -i 's|ssl spdy http2|ssl http2|g' $v; 
                    sed -i 's|  spdy_headers_comp|  #spdy_headers_comp|g' $v; 
                    sed -i 's|  add_header Alternate-Protocol|  #add_header Alternate-Protocol|g' $v; 
                else
                    sed -i 's|ssl spdy|ssl http2|g' $v; 
                    sed -i 's|  spdy_headers_comp|  #spdy_headers_comp|g' $v; 
                    sed -i 's|  add_header Alternate-Protocol|  #add_header Alternate-Protocol|g' $v; 
                fi
                egrep -n 'ssl http2|spdy_headers_comp|Alternate-Protocol' $v;
            done
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

       # zlibsymlink_fix

       if [[ "$NGINX_ZERODT" = [nN] ]]; then
         /etc/init.d/nginx restart
       elif [[ "$NGINX_ZERODT" = [yY] ]]; then
         echo "---------------------------------------------------------------------------"
         echo "nginx master id: $GETNGXPID"
         echo "---------------------------------------------------------------------------"
         curl -s -4 localhost/nginx_status
         echo "---------------------------------------------------------------------------"
         echo "kill -USR2 $GETNGXPID"
         kill -USR2 $GETNGXPID
         sleep 3
         ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep '(nginx: |PID)' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep
         echo
         ls -lah /usr/local/sbin/nginx*
         echo "---------------------------------------------------------------------------"
         curl -s -4 localhost/nginx_status
         echo "---------------------------------------------------------------------------"
         echo "kill -WINCH $GETNGXPID"
         kill -WINCH $GETNGXPID
         sleep 3
         ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep '(nginx: |PID)' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep
         echo "---------------------------------------------------------------------------"
         curl -s -4 localhost/nginx_status
         echo "---------------------------------------------------------------------------"
         echo " waiting for old nginx worker processes to exit..."
         while [[ "$(ps awx -o ppid,command | grep 'nginx: worker' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep | awk '{print $1}' | uniq)" = "$GETNGXPPID" ]]; do
           echo " checking... worker child PPID=$GETNGXPPID exists"
           sleep 3
         done
         echo "---------------------------------------------------------------------------"
         echo "kill -QUIT $GETNGXPID"
         kill -QUIT $GETNGXPID
         sleep 3
         ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep '(nginx: |PID)' | egrep -v '(nginx_upgrade|amplify|php-fpm|hhvm)' | egrep -v grep
         echo "---------------------------------------------------------------------------"
         curl -s -4 localhost/nginx_status
         echo "---------------------------------------------------------------------------"
       fi

        # cecho "Checking OpenSSL version used by Nginx..." $boldyellow
        # SSLIB=$(ldd `which nginx` | grep ssl | awk '{print $3}')
        # OPENSSLVER_CHECK=$(strings $SSLIB | grep "^OpenSSL ")
        # echo $OPENSSLVER_CHECK

        CBODYCHECK=$(grep 'client_body_in_file_only on' /usr/local/nginx/conf/nginx.conf)
        if [ $CBODYCHECK ]; then
            sed -i 's/client_body_in_file_only on/client_body_in_file_only off/g' /usr/local/nginx/conf/nginx.conf
        fi

        geoinccheck
        geoipphp
        mimefix

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Cleanup"
    fi

# as nginx-sticky-module-ng nginx module https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
# now uses master branch, if enabled via NGINX_STICKY=y always remove existing master tarball and 
# redownload again to get latest
if [[ "$NGINX_STICKY" = [yY] ]]; then
    if [[ -f "$DIR_TMP/${NGX_STICKYLINKFILE}" && "$(echo "$DIR_TMP/${NGX_STICKYLINKFILE}")" != '/svr-setup/' ]]; then
        rm -rf "$DIR_TMP/${NGX_STICKYLINKFILE}"
    fi
    if [[ -d "${DIR_TMP}/${NGX_STICKYDIR}" && "$(echo "${DIR_TMP}/${NGX_STICKYDIR}")" != '/svr-setup/' ]]; then
        rm -rf "${DIR_TMP}/${NGX_STICKYDIR}"
    fi
fi

# clean up /svr-setup downloads directory of older PHP source tarball/directories
find "$DIR_TMP" -maxdepth 1 \( -name 'nginx-1*' ! -name "nginx-${ngver}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'nginx-1*' ! -name "nginx-${ngver}*" \) -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'ngx_pagespeed*' ! -name "ngx_pagespeed-${NGXPGSPEED_VER}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'ngx_pagespeed*' ! -name "ngx_pagespeed-${NGXPGSPEED_VER}*" \) -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'incubator-pagespeed-ngx*' ! -name "incubator-pagespeed-ngx-${NGXPGSPEED_VER}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'incubator-pagespeed-ngx*' ! -name "incubator-pagespeed-ngx-${NGXPGSPEED_VER}*" \) -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'libressl*' ! -name "libressl-${LIBRESSL_VERSION}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'libressl*' ! -name "libressl-${LIBRESSL_VERSION}*" \) -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'openssl*' ! -name "openssl-${OPENSSL_VERSION}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'openssl*' ! -name "openssl-${OPENSSL_VERSION}*" \) -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'pcre-*' ! -name "pcre-${NGINX_PCREVER}*" \) -print
find "$DIR_TMP" -maxdepth 1 \( -name 'pcre-*' ! -name "pcre-${NGINX_PCREVER}*" \) -exec rm -rf {} \;

# nginx upgrade logs older than 90 days will be gzip compressed to save space 
# & older logs than 120 days deleted
if [ -d "${CENTMINLOGDIR}" ]; then
    find "${CENTMINLOGDIR}" -type f -mtime +90 -name "*_nginx_upgrade.log" -exec gzip -9 {} \;
    # find "${CENTMINLOGDIR}" -type f -mtime +120 \( -name 'patch_opensslpatches_*' -o -name 'patch_patchnginx_*' -o -name "nginx-configure-*" -o -name "centminmod_opensslinstalltime_*" -o -name "nginx_autoconf.err*" \) -print
    find "${CENTMINLOGDIR}" -type f -mtime +120 \( -name 'patch_opensslpatches_*' -o -name 'patch_patchnginx_*' -o -name "nginx-configure-*" -o -name "centminmod_opensslinstalltime_*" -o -name "nginx_autoconf.err*" \) -delete
fi

        if [ -f /usr/local/sbin/nginx ]; then
            echo
            echo "nginx -V"
            nginx -V
            echo
            nginx -t
        fi

        if [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
            echo
            echo "dynamic modules include file /usr/local/nginx/conf/dynamic-modules.conf"
            ls -lah /usr/local/nginx/conf/dynamic-modules.conf
            echo
            cat /usr/local/nginx/conf/dynamic-modules.conf
            echo
            echo "dynamic module directory at /usr/local/nginx/modules"
            ls -lah /usr/local/nginx/modules
            echo
            echo "backup dynamic modules to /usr/local/nginx/modules.copy"
            mkdir -p /usr/local/nginx/modules.copy
            \cp -af /usr/local/nginx/modules/* /usr/local/nginx/modules.copy
            ls -lah /usr/local/nginx/modules.copy
        fi

        echo
        echo "log files saved at ${CENTMINLOGDIR}"
        ls -lAhrt "${CENTMINLOGDIR}" | grep $DT
        echo

        echo "*************************************************"
        CHECK_PCLMUL=$(gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | grep 'mpclmul' | grep -o enabled)
        if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" != 'sse4_2' || "$CHECK_PCLMUL" != 'enabled' ]]; then
            echo
            cecho "* CLOUDFLARE_ZLIB='y' detected but cpu SSE4.2 or AES-NI support not available" $boldgreen
            cecho "* Fallback to standard zlib support in Nginx due to SSE4.2 not supported" $boldgreen
            echo
        fi
        cecho "* nginx updated" $boldgreen
        if [[ "$ngver" = 'master' ]]; then
            cecho "* nginx github master build" $boldgreen
        fi
        if [[ "$ngver" = 'master-google' ]]; then
            cecho "* nginx google fork master build" $boldgreen
        fi
        echo "*************************************************"

    # if you set /etc/centminmod/custom_config.inc file to
    # NGINX_UPDATEMAINTENANCE=y
    # then whenever you run nginx upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$NGINX_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus on
        fi
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Nginx Updated"
    fi

  fi

# error check
################

echo " "

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf /svr-setup
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