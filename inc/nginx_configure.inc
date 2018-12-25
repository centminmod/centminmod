detect_tlsonethree() {
  if [ "$ngver" ]; then
    NGINX_DIRINSTALL="$DIR_TMP/nginx-${ngver}"
  else
    NGINX_DIRINSTALL="$DIR_TMP/nginx-${NGINX_VERSION}"
  fi
  if [ -f /usr/local/nginx/conf/ssl-include.conf ]; then
    if [[ "$OPENSSL_TLSONETHREE" = [yY] && "$(grep -w 'define NGX_SSL_TLSv1_3' "${NGINX_DIRINSTALL}/src/event/ngx_event_openssl.h")" ]]; then
      # Nginx supports TLS 1.3 ssl_protocols directive
      if [[ "$DISABLE_TLSONEZERO_PROTOCOL" = [yY] ]]; then
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;|' /usr/local/nginx/conf/ssl-include.conf
      else
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;|' /usr/local/nginx/conf/ssl-include.conf
      fi
    else
      if [[ "$DISABLE_TLSONEZERO_PROTOCOL" = [yY] ]]; then
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2;|' /usr/local/nginx/conf/ssl-include.conf
      else
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;|' /usr/local/nginx/conf/ssl-include.conf
      fi
    fi
  elif [ -f /usr/local/nginx/conf/ssl_include.conf ]; then
    if [[ "$OPENSSL_TLSONETHREE" = [yY] && "$(grep -w 'define NGX_SSL_TLSv1_3' "${NGINX_DIRINSTALL}/src/event/ngx_event_openssl.h")" ]]; then
      # Nginx supports TLS 1.3 ssl_protocols directive
      if [[ "$DISABLE_TLSONEZERO_PROTOCOL" = [yY] ]]; then
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;|' /usr/local/nginx/conf/ssl_include.conf
      else
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;|' /usr/local/nginx/conf/ssl_include.conf
      fi
    else
      if [[ "$DISABLE_TLSONEZERO_PROTOCOL" = [yY] ]]; then
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2;|' /usr/local/nginx/conf/ssl_include.conf
      else
        sed -i 's|ssl_protocols .*|ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;|' /usr/local/nginx/conf/ssl_include.conf
      fi
    fi
  fi
}

dynamicmodule_file() {
  if [ "$ngver" ]; then
    NGINX_DIRINSTALL="$DIR_TMP/nginx-${ngver}"
  else
    NGINX_DIRINSTALL="$DIR_TMP/nginx-${NGINX_VERSION}"
  fi
  NGX_DYNAMICCHECK=$(grep 'DYNAMIC_MODULES=' "$NGINX_DIRINSTALL/auto/options")
  if [ "$NGX_DYNAMICCHECK" ]; then
    echo
    echo "nginx dynamic module support detected"
    echo
    DYNAMIC_SUPPORT=y
    # force disable nginx modsecurity module as it conflicts with nginx
    # 1.9.11+ dynamic module builds
    # NGINX_MODSECURITY='n'
    # create include file to load nginx dynamic modules in main context in nginx.conf
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      rm -rf /usr/local/nginx/conf/dynamic-modules.conf
    fi
    touch /usr/local/nginx/conf/dynamic-modules.conf
    if [ ! -f /usr/local/nginx/conf/dynamic-modules-includes.conf ]; then
      touch /usr/local/nginx/conf/dynamic-modules-includes.conf
    fi
    if [[ ! "$(grep -w 'dynamic-modules-includes.conf' /usr/local/nginx/conf/dynamic-modules.conf)" ]]; then
      echo "# place custom load_module lines in this dynamic-modules-includes.conf" >> /usr/local/nginx/conf/dynamic-modules.conf
      echo "# file so that they persistent i.e. for manually dropped in dynamic modules" >> /usr/local/nginx/conf/dynamic-modules.conf
      echo "include /usr/local/nginx/conf/dynamic-modules-includes.conf;" >> /usr/local/nginx/conf/dynamic-modules.conf
    fi
    # setup include file /usr/local/nginx/conf/dynamic-modules.conf in nginx.conf
    if [[ ! "$(grep 'dynamic-modules.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|^pcre_jit on;|pcre_jit on;\ninclude \/usr\/local\/nginx\/conf\/dynamic-modules.conf;\n|g' /usr/local/nginx/conf/nginx.conf
      sed -i 's|^pcre_jit off;|pcre_jit on;\ninclude \/usr\/local\/nginx\/conf\/dynamic-modules.conf;\n|g' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$(grep 'dynamic-modules.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|^#include \/usr\/local\/nginx\/conf\/dynamic-modules.conf;|include \/usr\/local\/nginx\/conf\/dynamic-modules.conf;|g' /usr/local/nginx/conf/nginx.conf
    fi
    # custom dynamic loading module flag include sourced file
    if [ -f "${CONFIGSCANBASE}/custom_nginxmodules.inc" ]; then
        # default is at /etc/centminmod/custom_nginxmodules.inc
        source "${CONFIGSCANBASE}/custom_nginxmodules.inc"
    fi
  else
    # comment out include file /usr/local/nginx/conf/dynamic-modules.conf in nginx.conf
    # in it exists and you're moving to an nginx version that doesn't support dynamic module
    # loading
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      if [[ "$(grep 'dynamic-modules.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's|^include \/usr\/local\/nginx\/conf\/dynamic-modules.conf;|#include \/usr\/local\/nginx\/conf\/dynamic-modules.conf;|g' /usr/local/nginx/conf/nginx.conf
      fi
    fi
  fi # NGX_DYNAMICCHECK check
}

ngx_dynamicfunction() {
  if [ ! -d /usr/local/nginx/conf ]; then
    mkdir -p /usr/local/nginx/conf
  fi
  if [ -d /usr/local/nginx/conf ]; then
    # detection routine to see if Nginx supports Dynamic modules from nginx 1.9.11+
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Check Nginx Dynamic Module Support"
    fi
    echo
    echo "NGX_DYNAMICCHECK nginx_configure"
    pwd
    dynamicmodule_file
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  if [[ "$DETECT_NGXVER" -ge '1011005' && "$DYNAMIC_SUPPORT" = [yY] ]]; then
    WITHCOMPAT_OPT=' --with-compat'
  else
    WITHCOMPAT_OPT=""
  fi
  fi # check for /usr/local/nginx/conf
}

luaworkaroundfix() {
  #################################################################################
  # work around for lua-nginx-module 0.10 branch which adds new futures supported by
  # openssl but not by libressl which nginx compiles against so this routine checks
  # if lua-nginx-module 0.10 branch is loaded and switches back to openssl if detected
  # https://community.centminmod.com/posts/24549/
  CHECK_LUAVERSIONEND=$(echo "$ORESTY_LUANGINXVER"  | cut -d . -f3)
  CHECK_LUAVERSIONMID=$(echo "$ORESTY_LUANGINXVER"  | cut -d . -f2)
  CHECK_LUAVERSIONSTART=$(echo "$ORESTY_LUANGINXVER"  | cut -d . -f1)
  if [[ "$CHECK_LUAVERSIONEND" != '0' && "$CHECK_LUAVERSIONMID" -ge '10' && "$CHECK_LUAVERSIONSTART" -ge '0' ]]; then
    LIBRESSL_SWITCH='n'
  elif [[ "$CHECK_LUAVERSIONEND" -eq '0' && "$CHECK_LUAVERSIONMID" -ge '10' && "$CHECK_LUAVERSIONSTART" -ge '0' ]]; then
    LIBRESSL_SWITCH='n'
    luaopensslpatch
  fi
}

libresslcompile_fix() {
  if [[ "$(grep install_sw auto/lib/openssl/make >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
      if [ -f auto/lib/openssl/make ]; then
        sed -i 's|install_sw LIBDIR=lib|install LIBDIR=lib|' auto/lib/openssl/make
      fi
    fi
  fi
}

funct_nginxconfigure() {

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

if [[ "$NGINX_COMPILE_EXPORT" = [yY] ]]; then
  NGINX_EXPORTLD_OPT='-Wl,-E '
fi

  # adjustments for OpenSSL 1.1.0
  DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
  DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
  if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
    echo "detected $OPENSSL_VERSION"
    WITHOPENSSL_OPT=""
    WITHOPENSSL_OPTECHO=""
  else
    WITHOPENSSL_OPT=""
    WITHOPENSSL_OPTECHO=""
  fi

  # override NGINX_OPENRESTY='n' if detected redis cache at nginx
  # level as it requires NGINX_OPENRESTY='y' for dependent nginx
  # modules provided by openresty
  if [[ -d /usr/local/nginx/conf/wpincludes && "$(find /usr/local/nginx/conf/wpincludes -type f -name "rediscache.*" >/dev/null 2>&1; echo $?)" -eq '0'  ]]; then
    NGINX_OPENRESTY='y'
  fi

  # fix no longer needed with lua-nginx 0.10.2+
  # https://github.com/openresty/lua-nginx-module/commit/b1a326eedf47b77e98b378fae1160e5340b75006
  #luaworkaroundfix

  #################################################################################
  ngx_dynamicfunction
  #################################################################################
  modsecurity_install
  #################################################################################
  
# CUR_NGINXVER=$(nginx -v 2>&1 | awk -F '\\/' '{print $2}' |sed -e 's|\.|0|g' | head -n1)
# CUR_NGINXUPGRADEVER=$(echo $ngver |sed -e 's|\.|0|g' | head -n1)
# SET_NGINXVER=$(echo ${NGINX_VERSION} |sed -e 's|\.|0|g' | head -n1)

    # assign NGX_VEREVAL nginx version based on if it's an initial
    # centmin mod fresh install or a nginx upgrade run
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      NGX_VEREVAL=${CUR_NGINXUPGRADEVER}
    else
      NGX_VEREVAL=${SET_NGINXVER}
    fi

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
  # lsof -n | grep tcmalloc >/dev/null 2>&1; echo $?
  GPERFOPT=" --with-google_perftools_module"
  mkdir -p /var/tmp/tcmalloc
  chown -R nginx:nginx /var/tmp/tcmalloc
cat > "/usr/local/nginx/conf/gperftools.conf" <<GPF
google_perftools_profiles /var/tmp/tcmalloc;
GPF

  GPERFINCLUDE_CHECK=$(grep '^include \/usr\/local\/nginx\/conf\/gperftools.conf;' /usr/local/nginx/conf/nginx.conf)

  if [[ ! -z "$GPERFINCLUDE_CHECK" ]]; then
    sed -i 's/^#include \/usr\/local\/nginx\/conf\/gperftools.conf;/include \/usr\/local\/nginx\/conf\/gperftools.conf;/g' /usr/local/nginx/conf/nginx.conf
  elif [[ -z "$GPERFINCLUDE_CHECK" ]]; then
    sed -i 's/pcre_jit on;/pcre_jit on; \ninclude \/usr\/local\/nginx\/conf\/gperftools.conf;/g' /usr/local/nginx/conf/nginx.conf
  fi
else
  GPERFOPT=""
  sed -i 's|^include \/usr\/local\/nginx\/conf\/gperftools.conf|#include \/usr\/local\/nginx\/conf\/gperftools.conf|g' /usr/local/nginx/conf/nginx.conf
fi

if [[ "$NGINX_ZLIBNG" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
  zlibng_install
  ZLIBNG_OPT=' --with-zlib=../zlib-ng'
else
  ZLIBNG_OPT=""
fi


if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_SIX" = '6' ]]; then

  if [[ "$ngver" && "$ngver" = 'master-google' ]]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGX_HPACK_OPT=""
    NGINX_HPACK='n'
  elif [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi

  # if openssl 1.1.1 breaks HPACK patching
  # if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
  #   NGINX_HPACK='n'
  #   NGX_HPACK_OPT=""
  # fi
  # DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
  # if [[ "$DETECTOPENSSL_ONEZERO" = '1.0' ]]; then
  #   NGINX_HPACK='n'
  #   NGX_HPACK_OPT=""
  # fi

  # if [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013006' && "$DETECT_NGXVER" -lt '1013009' ]]; then
  if [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013006' && "$DETECT_NGXVER" -lt '1013010' ]]; then
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013010' && "$DETECT_NGXVER" -le '1013012' ]]; then
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -eq '1014000' ]]; then
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1015000' && "$DETECT_NGXVER" -le '1015007' ]]; then
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  else
    NGX_HPACK_OPT=""
  fi

  if [[ "$NGINX_PRIORITIZECHACHA" = [yY] && "$DETECT_NGXVER" -ge '1015004' && "$(echo $OPENSSL_VERSION | grep -o '1.1.1')" = '1.1.1' ]]; then
    ngx_prioritizechacha_patch
  fi

  if [[ "$NGINX_HTTPPUSH" = [yY] ]]; then
    NGINX_HTTP2='y'
    ngx_httppush_patch
    HTTP_PUSHOPT=' --add-module=../http2-push-nginx-module'
  else
    HTTP_PUSHOPT=""
  fi

  if [[ "$NGINX_SLICE" = [yY] ]]; then
    NGXSLICEOPT=' --with-http_slice_module'
  else
    NGXSLICEOPT=""
  fi

  if [[ "$NGINX_RTMP" = [yY] ]]; then
    if [[ ! -d "${DIR_TMP}/nginx-rtmp-module" ]]; then
      echo
      echo "download nginx-rtmp-module from github"
      pushd "$DIR_TMP"
      time git clone git://github.com/arut/nginx-rtmp-module.git
      echo
      popd
    elif [[ -d "${DIR_TMP}/nginx-rtmp-module" && -d "${DIR_TMP}/nginx-rtmp-module/.git" ]]; then
      echo
      echo "get latest updates nginx-rtmp-module from github"
      pushd "${DIR_TMP}/nginx-rtmp-module"
      git stash
      git pull
      git log -1
      echo
      popd
    fi
  fi

  if [[ "$NGINX_DEBUG" = [yY] ]]; then
    NGINX_DEBUGOPT=' --with-debug'
    STRIPNGINX=n
  else
    NGINX_DEBUGOPT=""
  fi

  if [[ "$DETECT_NGXVER" -ge '1011005' && "$NGINX_STREAMSSLPREREAD" = [yY] ]]; then
    STREAM_SSLPREREADOPT=" --with-stream_ssl_preread_module"
  else
    STREAM_SSLPREREADOPT=""
  fi

  if [[ "$NGINX_HTTP2" = [yY] ]]; then  
    # only apply Nginx HTTP/2 if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
    if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]] || [[ "$NGX_VEREVAL" = 'master' ]] || [[ "$NGX_VEREVAL" = 'master-google' ]]; then
      if [[ "$NGINX_SPDYPATCHED" = [yY] ]]; then
        NGINX_SPDY=y
        HTTPTWOOPT=' --with-http_spdy_module --with-http_v2_module'
      else
        NGINX_SPDY=n
        HTTPTWOOPT=' --with-http_v2_module'
      fi
    fi
  else
    HTTPTWOOPT=""
    if [[ "$NGX_VEREVAL" -lt '10903' ]]; then
      NGINX_SPDY=y
    fi
  fi

  if [[ "$NGINX_REALIP" = [yY] ]]; then
    REALIPOPT=' --with-http_realip_module'
  else
    REALIPOPT=""
  fi  

  if [[ "$NGINX_RDNS" = [yY] ]]; then
    cd "$DIR_TMP"
    rm -rf nginx-http-rdns
    time git clone https://github.com/flant/nginx-http-rdns.git nginx-http-rdns
    RDNSOPT=" --add-module=../nginx-http-rdns"
  else
    RDNSOPT=""
  fi  

  if [[ "$NGINX_STUBSTATUS" = [yY] ]]; then
    STUBSTATUSOPT=" --with-http_stub_status_module"
  else
    STUBSTATUSOPT=""
  fi  

  if [[ "$NGINX_SUB" = [yY] ]]; then
    SUBOPT=" --with-http_sub_module"
  else
    SUBOPT=""
  fi  

  if [[ "$NGINX_ADDITION" = [yY] ]]; then
    ADDITIONOPT=" --with-http_addition_module"
  else
    ADDITIONOPT=""
  fi  

  if [[ "$NGINX_TESTCOOKIE" = [yY] ]]; then
    NGINX_TESTCOOKIEGIT='y'
    NGX_TESTCOOKIEDIR='testcookie-nginx-module'
    if [[ "$NGINX_TESTCOOKIEGIT" = [yY] ]]; then
      pushd "$DIR_TMP"
      rm -rf testcookie-nginx-module
      time git clone -b master --depth=1 https://github.com/kyprizel/testcookie-nginx-module
      popd
    fi
    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/headers-more-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_TESTCOOKIE" = [yY] ]]; then
      if [[ "$NGINX_TESTCOOKIEGIT" = [yY] ]]; then
        TESTCOOKIEOPT=" --add-dynamic-module=../testcookie-nginx-module"
      else
        TESTCOOKIEOPT=" --add-dynamic-module=../$NGX_TESTCOOKIEDIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_testcookie_access_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old
      fi
    else
      if [[ "$NGINX_TESTCOOKIEGIT" = [yY] ]]; then
        TESTCOOKIEOPT=" --add-module=../testcookie-nginx-module"
      else
        TESTCOOKIEOPT=" --add-module=../$NGX_TESTCOOKIEDIR"
      fi
      # if NGXDYNAMIC_TESTCOOKIE='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_testcookie_access_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_TESTCOOKIE='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_testcookie_access_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_testcookie_access_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old
      fi
    fi
  else
    TESTCOOKIEOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_testcookie_access_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_testcookie_access_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_testcookie_access_module.so.old
      fi
  fi

  if [[ "$NGINX_LENGTHHIDE" = [yY] ]]; then
    NGINX_LENGTHHIDEGIT='y'
    NGX_HIDELENGTHDIR='nginx-length-hiding-filter-module'
    if [[ "$NGINX_LENGTHHIDEGIT" = [yY] ]]; then
      pushd "$DIR_TMP"
      rm -rf nginx-length-hiding-filter-module
      time git clone -b master --depth=1 https://github.com/nulab/nginx-length-hiding-filter-module nginx-length-hiding-filter-module
      popd
    fi
    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/headers-more-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_HIDELENGTH" = [yY] ]]; then
      if [[ "$NGINX_LENGTHHIDEGIT" = [yY] ]]; then
        HIDELENGTHOPT=" --add-dynamic-module=../nginx-length-hiding-filter-module"
      else
        HIDELENGTHOPT=" --add-dynamic-module=../$NGX_HIDELENGTHDIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_length_hiding_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old
      fi
    else
      if [[ "$NGINX_LENGTHHIDEGIT" = [yY] ]]; then
        HIDELENGTHOPT=" --add-module=../nginx-length-hiding-filter-module"
      else
        HIDELENGTHOPT=" --add-module=../$NGX_HIDELENGTHDIR"
      fi
      # if NGXDYNAMIC_HIDELENGTH='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_HIDELENGTH='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old
      fi
    fi
  else
    HIDELENGTHOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_length_hiding_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_IMAGEFILTER" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_IMAGEFILTER" = [yY] ]]; then
      IMAGEFILTEROPT=" --with-http_image_filter_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_image_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
    else
      IMAGEFILTEROPT=" --with-http_image_filter_module"
      # if NGXDYNAMIC_IMAGEFILTER='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_IMAGEFILTER='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
    fi
  else
    IMAGEFILTEROPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_XSLT" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_XSLT" = [yY] ]]; then
      NGXXSLTOPT=" --with-http_xslt_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_xslt_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old
      fi
    else
      NGXXSLTOPT=" --with-http_xslt_module"
      # if NGXDYNAMIC_XSLT='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_XSLT='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_xslt_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old
      fi
    fi
  else
    NGXXSLTOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_xslt_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_PERL" = [yY] ]]; then
    if [ ! -f /usr/share/perl5/ExtUtils/Embed.pm ]; then
      time $YUMDNFBIN -q -y install perl-ExtUtils-Embed
    fi
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_PERL" = [yY] ]]; then
      NGXPERLOPT=" --with-http_perl_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_perl_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
    else
      NGXPERLOPT=" --with-http_perl_module"
      # if NGXDYNAMIC_PERL='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_PERL='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_perl_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
    fi
  else
    NGXPERLOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_perl_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
  fi

  if [[ "$NGINX_CACHEPURGE" = [yY] ]]; then
    # ngx_cachepurge_patch
    NGX_CACHEPURGEDIR=$(tar -tzf "$DIR_TMP/${NGX_CACHEPURGEFILE}" | head -1 | cut -f1 -d"/")
    CACHEPURGEOPT=" --add-module=../$NGX_CACHEPURGEDIR"
  else
    CACHEPURGEOPT=""
  fi

  if [[ "$NGINX_ACCESSKEY" = [yY] ]]; then
    ACCESSKEYOPT=" --add-module=../nginx-accesskey-2.0.3"
  else
    ACCESSKEYOPT=""
  fi    

  if [[ "$NGINX_HTTPCONCAT" = [yY] ]]; then
    NGX_CONCATDIR=$(tar -tzf "$DIR_TMP/${NGX_CONCATLINKFILE}" | head -1 | cut -f1 -d"/")
    HTTPCONCATOPT=" --add-module=../$NGX_CONCATDIR"
  else
    HTTPCONCATOPT=""
  fi

  if [[ "$ORESTY_HEADERSMORE" = [yY] ]]; then
    NGX_HEADERSMOREDIR=$(tar -tzf "$DIR_TMP/${NGX_HEADERSMORELINKFILE}" | head -1 | cut -f1 -d"/")
    # gitmaster option
    if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
      pushd "$DIR_TMP"
      rm -rf headers-more-nginx-module-master
      time git clone -b master --depth=1 https://github.com/openresty/headers-more-nginx-module.git headers-more-nginx-module-master
      popd
    fi
    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/headers-more-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_HEADERSMORE" = [yY] ]]; then
      if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
        HEADERSMOREOPT=" --add-dynamic-module=../headers-more-nginx-module-master"
      else
        HEADERSMOREOPT=" --add-dynamic-module=../$NGX_HEADERSMOREDIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_headers_more_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
    else
      if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
        HEADERSMOREOPT=" --add-module=../headers-more-nginx-module-master"
      else
        HEADERSMOREOPT=" --add-module=../$NGX_HEADERSMOREDIR"
      fi
      # if NGXDYNAMIC_HEADERSMORE='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_HEADERSMORE='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
    fi
  else
    HEADERSMOREOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_HTTPREDIS" = [yY] ]]; then
    NGX_HTTPREDISDIR=$(tar -tzf "$DIR_TMP/${NGX_HTTPREDISLINKFILE}" | head -1 | cut -f1 -d"/")
    HTTPREDISOPT=" --add-module=../$NGX_HTTPREDISDIR"
  else
    HTTPREDISOPT=""
  fi

  if [[ "$NGINX_AUTHREQ" = [yY] ]]; then
    AUTHREQOPT=' --with-http_auth_request_module'
  else
    AUTHREQOPT=""
  fi

  if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
    LIBRESSLDIR=$(tar -tzf "$DIR_TMP/${LIBRESSL_LINKFILE}" 2>&1 | head -1 | cut -f1 -d"/" | grep libressl)
    LIBRESSLOPT=" --with-openssl=../$LIBRESSLDIR"
    OPENSSLOPT=""
    BORINGSSLOPT=""
    LRT='-lrt '
  elif [[ "$BORINGSSL_SWITCH" = [yY] ]]; then
    LIBRESSLOPT=""
    OPENSSLOPT=""
    # LRT=""
    LRT='-lrt '
    # BORINGSSLOPT=" --with-openssl=${BORINGSSL_DIR}/boringssl"
    BORINGSSL_RPATH="${BORINGSSL_DIR}/boringssl/.openssl/lib:"
    BORINGSSL_LIBOPT="-L${BORINGSSL_DIR}/boringssl/.openssl/lib -lcrypto -lssl "
    BORINGSSLINC_OPT="-I${BORINGSSL_DIR}/boringssl/.openssl/include "
    # openresty set-misc nginx module not compatible with boringssl
    # https://community.centminmod.com/threads/15406/
    #ORESTY_SETMISC='n'
    #NGINX_STREAM='n'
    #SETMISCOPT=""
  else
    if [ "$NOSOURCEOPENSSL" == 'n' ]; then
      OPENSSLDIR=$(tar -tzf "$DIR_TMP/${OPENSSL_LINKFILE}" | head -1 | cut -f1 -d"/")
      LIBRESSLOPT=""
      BORINGSSLOPT=""
      if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      elif [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      else
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR --with-openssl-opt='enable-ec_nistp_64_gcc_128'"
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
        fi
      fi
      LRT=""
    else
      OPENSSLDIR=$(tar -tzf "$DIR_TMP/${OPENSSL_LINKFILE}" | head -1 | cut -f1 -d"/")
      # export OPENSSL_BPATH=$DIR_TMP
      LIBRESSLOPT=""
      BORINGSSLOPT=""
      if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]]; then
        OPENSSL_CUSTOMPATH='/opt/openssl-tls1.3'
        export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      elif [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      else
        export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR --with-openssl-opt='enable-ec_nistp_64_gcc_128'"
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
        fi
      fi
      LRT=""
    fi
  fi

  if [[ "$NGINX_ZLIBCUSTOM" = [yY] ]]; then
    ZLIBCUSTOM_DIR=$(tar -tzf "$DIR_TMP/${NGX_ZLIBLINKFILE}" 2>&1 | head -1 | cut -f1 -d"/" | grep zlib)
    ZLIBCUSTOM_OPT=" --with-zlib=../zlib-${NGINX_ZLIBVER}"
  fi

  CHECK_PCLMUL=$(gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | grep 'mpclmul' | grep -o enabled)
  if [[ "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
    ZLIBCUSTOM_DIR="zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
    ZLIBCUSTOM_OPT=" --with-zlib=../zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
    # if using cloudflare zlib libary set the default gzip compression level higher instead of 5 set to 9 
    # defaults  benchmarks show performance is still very high even with level 9 with cloudflare zlib library
    # https://community.centminmod.com/posts/64167/
    if [[ "$CLOUDFLARE_ZLIBRAUTOMAX" = [yY] && -f /usr/local/nginx/conf/nginx.conf ]]; then
      sed -i 's|gzip_comp_level .*|gzip_comp_level 9;|' /usr/local/nginx/conf/nginx.conf
    else
      if [[ "$(grep 'gzip_comp_level 9' /usr/local/nginx/conf/nginx.conf)" && -f /usr/local/nginx/conf/nginx.conf ]]; then
        sed -i 's|gzip_comp_level .*|gzip_comp_level 5;|' /usr/local/nginx/conf/nginx.conf
      fi
    fi
    if [[ "$CLOUDFLARE_ZLIBRAUTOMAX" = [yY] && -f /usr/local/src/centminmod/config/nginx/nginx.conf ]]; then
      sed -i 's|gzip_comp_level .*|gzip_comp_level 9;|' /usr/local/src/centminmod/config/nginx/nginx.conf
    else
      if [[ "$(grep 'gzip_comp_level 9' /usr/local/src/centminmod/config/nginx/nginx.conf)" && -f /usr/local/src/centminmod/config/nginx/nginx.conf ]]; then
        sed -i 's|gzip_comp_level .*|gzip_comp_level 5;|' /usr/local/src/centminmod/config/nginx/nginx.conf
      fi
    fi
    if [[ "$CLOUDFLARE_ZLIB_DYNAMIC" = [yY] ]]; then
      ZLIBCF_RPATH='/usr/local/zlib-cf/lib:'
      ZLIBCF_OPT='-L/usr/local/zlib-cf/lib '
      ZLIBCFINC_OPT='-I/usr/local/zlib-cf/include '
      ZLIBCUSTOM_OPT=" --with-zlib=../zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
    fi
    # disable clang compiler and switch to gcc compiler due to clang 3.4.2 errors when enabling
    # cloudflare zlib
    if [[ "$CLANG" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
      CLANG='y'
      if [[ "$CLANG_MASTER" = [yY] && -f /opt/sbin/llvm-release_master/bin/clang ]]; then
        CLANG_FOUR='n'
        CLANG_FIVE='n'
        CLANG_SIX='n'
        CLANG_MASTER='y'
      elif [[ "$CLANG_SIX" = [yY] && -f /opt/sbin/llvm-release_60/bin/clang ]]; then
        CLANG_FOUR='n'
        CLANG_FIVE='n'
        CLANG_SIX='y'
        CLANG_MASTER='n'
      elif [[ "$CLANG_FIVE" = [yY] && -f /opt/sbin/llvm-release_50/bin/clang ]]; then
        CLANG_FOUR='n'
        CLANG_FIVE='y'
        CLANG_SIX='n'
        CLANG_MASTER='n'
      else
        CLANG_FOUR='y'
        CLANG_FIVE='n'
        CLANG_SIX='n'
        CLANG_MASTER='n'
      fi
    elif [[ "$CLANG" = [yY] && "$CENTOS_SIX" -eq '6' ]]; then
      CLANG='n'
      DEVTOOLSETSIX='y'
    else
      CLANG='n'
      DEVTOOLSETSIX='y'
    fi
  fi

  if [[ "$CLOUDFLARE_ZLIBRESET" = [yY] && "$CLOUDFLARE_ZLIB" != [yY] ]]; then
    # if not using Cloudflare zlib library and you previously set gzip compression level to 9, reset it back to level 5
    if [[ "$(grep 'gzip_comp_level 9' /usr/local/nginx/conf/nginx.conf)" && -f /usr/local/nginx/conf/nginx.conf ]]; then
      sed -i 's|gzip_comp_level .*|gzip_comp_level 5;|' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$(grep 'gzip_comp_level 9' /usr/local/src/centminmod/config/nginx/nginx.conf)" && -f /usr/local/src/centminmod/config/nginx/nginx.conf ]]; then
      sed -i 's|gzip_comp_level .*|gzip_comp_level 5;|' /usr/local/src/centminmod/config/nginx/nginx.conf
    fi
  fi

  if [[ "$NGINX_OPENRESTY" = [yY] ]]; then
    MEMCDIR=$(tar -tzf "$DIR_TMP/${NGX_MEMCLINKFILE}" | head -1 | cut -f1 -d"/")
    SRCACHEDIR=$(tar -tzf "$DIR_TMP/${NGX_SRCACHELINKFILE}" | head -1 | cut -f1 -d"/")
    SETMISCDIR=$(tar -tzf "$DIR_TMP/${NGX_SETMISCLINKFILE}" | head -1 | cut -f1 -d"/")
    DEVELKITDIR=$(tar -tzf "$DIR_TMP/${NGX_DEVELKITLINKFILE}" | head -1 | cut -f1 -d"/")
    ECHODIR=$(tar -tzf "$DIR_TMP/${NGX_ECHOLINKFILE}" | head -1 | cut -f1 -d"/")
    REDISDIR=$(tar -tzf "$DIR_TMP/${NGX_REDISLINKFILE}" | head -1 | cut -f1 -d"/")
    ngxopenresty_patch

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/simpl/ngx_devel_kit#building-as-a-dynamic-module
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_DEVELKIT" = [yY] ]]; then
        # certain oprenresty modules need to be compiled as dynamic modules if ngx_devel_kit
        # module is compiled as a dynamic module
        NGXDYNAMIC_SETMISC='y'
        DEVELKITOPT=" --add-dynamic-module=../${DEVELKITDIR}"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ndk_http_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ndk_http_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ndk_http_module.so.old
      fi
    else
        DEVELKITOPT=" --add-module=../${DEVELKITDIR}"
      # if NGXDYNAMIC_DEVELKIT='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_DEVELKIT='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ndk_http_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ndk_http_module.so
      fi
      if [ -f /usr/local/nginx/modules/ndk_http_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ndk_http_module.so.old
      fi
    fi

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/memc-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_MEMC" = [yY] ]]; then
        MEMCOPT=" --add-dynamic-module=../${MEMCDIR}"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_memc_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_memc_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_memc_module.so.old
      fi
    else
        MEMCOPT=" --add-module=../${MEMCDIR}"
      # if NGXDYNAMIC_MEMC='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_MEMC='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_memc_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_memc_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_memc_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_memc_module.so.old
      fi
    fi

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/srcache-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_SRCCACHE" = [yY] ]]; then
        SRCCACHEOPT=" --add-dynamic-module=../${SRCACHEDIR}"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_srcache_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_srcache_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_srcache_filter_module.so.old
      fi
    else
        SRCCACHEOPT=" --add-module=../${SRCACHEDIR}"
      # if NGXDYNAMIC_SRCCACHE='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_SRCCACHE='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_srcache_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_srcache_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_srcache_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_srcache_filter_module.so.old
      fi
    fi

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/set-misc-nginx-module#installation
      if [[ "$ORESTY_SETMISCGIT" = [yY] ]]; then
        pushd "$DIR_TMP"
        rm -rf set-misc-nginx-module-master
        time git clone -b master --depth=1 https://github.com/openresty/set-misc-nginx-module.git set-misc-nginx-module-master
        popd
      fi
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$ORESTY_SETMISC" = [yY] && "$NGXDYNAMIC_SETMISC" = [yY] ]]; then
      if [[ "$ORESTY_SETMISCGIT" = [yY] ]]; then
        SETMISCOPT=" --add-dynamic-module=../set-misc-nginx-module-master"
      else
        SETMISCOPT=" --add-dynamic-module=../${SETMISCDIR}"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_set_misc_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_set_misc_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_set_misc_module.so.old
      fi
    elif [[ "$ORESTY_SETMISC" = [yY] ]]; then
      if [[ "$ORESTY_SETMISCGIT" = [yY] ]]; then
        SETMISCOPT=" --add-module=../set-misc-nginx-module-master"
      else
        SETMISCOPT=" --add-module=../${SETMISCDIR}"
      fi
      # if NGXDYNAMIC_SETMISC='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_SETMISC='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_set_misc_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_set_misc_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_set_misc_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_set_misc_module.so.old
      fi
    fi

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/echo-nginx-module#installation
      if [[ "$ORESTY_ECHOGIT" = [yY] ]]; then
        pushd "$DIR_TMP"
        rm -rf echo-nginx-module-master
        time git clone -b master --depth=1 https://github.com/openresty/echo-nginx-module.git echo-nginx-module-master
        popd
      fi
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_ECHO" = [yY] ]]; then
      echonginx_patch
      if [[ "$ORESTY_ECHOGIT" = [yY] ]]; then
        ECHOOPT=" --add-dynamic-module=../echo-nginx-module-master"
      else
        ECHOOPT=" --add-dynamic-module=../${ECHODIR}"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_echo_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_echo_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_echo_module.so.old
      fi
    else
      echonginx_patch
      if [[ "$ORESTY_ECHOGIT" = [yY] ]]; then
        ECHOOPT=" --add-module=../echo-nginx-module-master"
      else
        ECHOOPT=" --add-module=../${ECHODIR}"
      fi
      # if NGXDYNAMIC_ECHO='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_ECHO='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_echo_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_echo_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_echo_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_echo_module.so.old
      fi
    fi

    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/redis2-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_REDISTWO" = [yY] ]]; then
        REDISOPT=" --add-dynamic-module=../${REDISDIR}"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_redis2_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_redis2_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_redis2_module.so.old
      fi
    else
        REDISOPT=" --add-module=../${REDISDIR}"
      # if NGXDYNAMIC_REDISTWO='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_REDISTWO='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_redis2_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_redis2_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_redis2_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_redis2_module.so.old
      fi
    fi
  else
    MEMCOPT=""
    SRCCACHEOPT=""
    SETMISCOPT=""
    DEVELKITOPT=""
    ECHOOPT=""
    REDISOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_set_misc_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_set_misc_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_set_misc_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_set_misc_module.so.old
      fi
      if [[ -f /usr/local/nginx/modules/ngx_http_echo_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_echo_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_echo_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_echo_module.so.old
      fi
  fi

  if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    # auto enable openresty stream lua nginx module when ORESTY_LUANGIN='y' set
    NGX_LUASTREAM='y'
    NGXDYNAMIC_STREAM='n'
    luanginx_patch
    # LUAJIT_XCFLAGOPT=" --with-luajit-xcflags='-DLUAJIT_NUMMODE=2'"
    LUAJIT_XCFLAGOPT=""
    # lua nginx 0.10.7 and lower not compatible with openssl 1.1.x branch
    # so fall back to 1.0.2 branch if lua nginx module is enabled
    OPENSSL_VERSION="$OPENSSL_VERSIONFALLBACK"
    export OPENSSL_VERSION="$OPENSSL_VERSIONFALLBACK"
    NGX_LUANGINXDIR=$(tar -tzf "$DIR_TMP/${NGX_LUANGINXLINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$NGX_LUASTREAM" = [yY] ]]; then
      NGX_LUASTREAMDIR=$(tar -tzf "$DIR_TMP/${NGX_LUASTREAMLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
    if [[ "$NGX_LUAUPSTREAM" = [yY] ]]; then
      NGX_LUAUPSTREAMDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAUPSTREAMLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
    if [[ "$NGX_LUAUPSTREAMCACHE" = [yY] ]]; then
      NGX_LUAUPSTREAMCACHEDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAUPSTREAMCACHELINKFILE}" | head -1 | cut -f1 -d"/")
    fi
    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/echo-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_LUA" = [yY] ]]; then
      # luapatch
      if [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [yY] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUASTREAMDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [nN] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [nN] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [yY] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [nN] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [nN] ]]; then
        LUAOPT=" --add-dynamic-module=../$NGX_LUANGINXDIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_lua_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_lua_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_lua_module.so.old
      fi
    else
      if [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [yY] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUASTREAMDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [nN] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [yY] && "$NGX_LUAUPSTREAM" = [nN] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMCACHEDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [yY] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUAUPSTREAMDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [nN] && "$NGX_LUASTREAM" = [yY] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR --add-module=../$NGX_LUASTREAMDIR"
      elif [[ "$NGX_LUAUPSTREAMCACHE" = [nN] && "$NGX_LUAUPSTREAM" = [nN] ]]; then
        LUAOPT=" --add-module=../$NGX_LUANGINXDIR"
      fi
      # if NGXDYNAMIC_LUA='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_LUA='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_lua_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_lua_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_lua_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_lua_module.so.old
      fi
    fi

    export LUAJIT_LIB='/usr/local/lib'
    export LUAJIT_INC="/usr/local/include/luajit-2.1"

    LUALD_OPT="-Wl,-rpath,${ZLIBCF_RPATH}${BORINGSSL_RPATH}/usr/local/lib"
    if [[ ! -f /usr/bin/lua || ! -f /usr/include/lua.h ]]; then
      time $YUMDNFBIN -q -y install lua lua-devel${DISABLEREPO_DNF}
    fi

      if [[ "$(uname -m)" = 'x86_64' ]]; then
          LUALIB_DIR='lib64'
      else
          LUALIB_DIR='lib'
      fi

      NGX_LUAMEMCACHEDDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAMEMCACHEDLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAMYSQLDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAMYSQLLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAREDISDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAREDISLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUADNSDIR=$(tar -tzf "$DIR_TMP/${NGX_LUADNSLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAUPLOADDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAUPLOADLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAWEBSOCKETDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAWEBSOCKETLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUALOCKDIR=$(tar -tzf "$DIR_TMP/${NGX_LUALOCKLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUASTRINGDIR=$(tar -tzf "$DIR_TMP/${NGX_LUASTRINGLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAREDISPARSERDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAREDISPARSERLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUAUPSTREAMCHECKDIR=$(tar -tzf "$DIR_TMP/${NGX_LUAUPSTREAMCHECKLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUALRUCACHEDIR=$(tar -tzf "$DIR_TMP/${NGX_LUALRUCACHELINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUARESTYCOREDIR=$(tar -tzf "$DIR_TMP/${NGX_LUARESTYCORELINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUALOGGERSOCKETDIR=$(tar -tzf "$DIR_TMP/${NGX_LUALOGGERSOCKETLINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUACOOKIEDIR=$(tar -tzf "$DIR_TMP/${NGX_LUACOOKIELINKFILE}" | head -1 | cut -f1 -d"/")
      NGX_LUACJSONDIR=$(tar -tzf "$DIR_TMP/${NGX_LUACJSONLINKFILE}" | head -1 | cut -f1 -d"/")  
      cd "${DIR_TMP}/$NGX_LUAMEMCACHEDDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUAMYSQLDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUAREDISDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUADNSDIR"; make clean; make install; mv /usr/local/lib/lua/resty/dns/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/dns
      cd "${DIR_TMP}/$NGX_LUAUPLOADDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUAWEBSOCKETDIR"; make clean; make install; mv /usr/local/lib/lua/resty/websocket/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/websocket
      cd "${DIR_TMP}/$NGX_LUALOCKDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUASTRINGDIR"; make clean; make install
      cd "${DIR_TMP}/$NGX_LUAREDISPARSERDIR"; make clean; make; make install;
      cd "${DIR_TMP}/$NGX_LUAUPSTREAMCHECKDIR"; make clean; make install; mv /usr/local/lib/lua/resty/upstream/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/upstream
      cd "${DIR_TMP}/$NGX_LUALRUCACHEDIR"; make clean; make install; mv /usr/local/lib/lua/resty/lrucache/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/lrucache
      cd "${DIR_TMP}/$NGX_LUARESTYCOREDIR"; make clean; make install; mv /usr/local/lib/lua/resty/core/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/core
      cd "${DIR_TMP}/$NGX_LUALOGGERSOCKETDIR"; chmod +x lib/resty/logger/*; \cp -rpf lib/resty/logger/* /usr/local/lib/lua/resty/
      cd "${DIR_TMP}/$NGX_LUACOOKIEDIR"; chmod +x lib/resty/*; \cp -rpf lib/resty/* /usr/local/lib/lua/resty/
      cd "${DIR_TMP}/$NGX_LUACJSONDIR"; make clean; make install
      cd "${DIR_TMP}"
  else
    LUAOPT=""
    LUALD_OPT="-Wl,-rpath,${ZLIBCF_RPATH}${BORINGSSL_RPATH}/usr/local/lib"
      if [[ -f /usr/local/nginx/modules/ngx_http_lua_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_lua_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_lua_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_lua_module.so.old
      fi
  fi

  if [[ "$NGINX_SPDY" = [yY] ]]; then
    if [[ "$NGINX_SPDYPATCHED" = [yY] ]]; then
      SPDYOPT=""
    else
      SPDYOPT=" --with-http_spdy_module"
    fi
  else
    SPDYOPT=""
  fi

  if [[ "$NGINX_RTMP" = [yY] ]]; then
    RTMPOPT=" --add-module=../nginx-rtmp-module"
  else
    RTMPOPT=""
  fi    

  if [[ "$NGINX_FLV" = [yY] ]]; then
    FLVOPT=" --with-http_flv_module"
  else
    FLVOPT=""
  fi  

  if [[ "$NGINX_MP4" = [yY] ]]; then
    MPOPT=" --with-http_mp4_module"
  else
    MPOPT=""
  fi  

  if [[ "$NGINX_SECURELINK" = [yY] ]]; then
    SECURELINKOPT=" --with-http_secure_link_module"
  else
    SECURELINKOPT=""
  fi    

  if [[ "$NGINX_FANCYINDEX" = [yY] ]]; then
    NGX_FANCYINDEXDIR=$(tar -tzf "$DIR_TMP/${NGX_FANCYINDEXLINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_FANCYINDEX" = [yY] ]]; then
      FANCYINDEXOPT=" --add-dynamic-module=../$NGX_FANCYINDEXDIR"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_fancyindex_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
    else
      FANCYINDEXOPT=" --add-module=../$NGX_FANCYINDEXDIR"
      # if NGXDYNAMIC_FANCYINDEX='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_fancyindex_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_FANCYINDEX='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
    fi
  else
    FANCYINDEXOPT=""
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
  fi

  if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
    if [ ! -f /var/log/nginx/vts.db ]; then
      touch /var/log/nginx/vts.db
      chown nginx /var/log/nginx/vts.db
    fi
    if [ -f /usr/bin/git ]; then
      if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_VHOSTSTATS" = [yY] ]]; then
        VTSOPT=" --add-dynamic-module=../nginx-module-vts"
        if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
          echo "load_module \"modules/ngx_http_vhost_traffic_status_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
        fi
      else
        VTSOPT=" --add-module=../nginx-module-vts"
      fi
      if [[ ! -d "${DIR_TMP}/nginx-module-vts" ]]; then
      cd "$DIR_TMP"
      time git clone "$NGX_VTSLINK" nginx-module-vts
      # sed -i 's|color:       black;|color:       white;|g' "${DIR_TMP}/nginx-module-vts/share/status.template.html"
      sed -i 's|#DED|#43a6df|g' "${DIR_TMP}/nginx-module-vts/share/status.template.html"
      # sed -i 's|color:       black;|color:       white;|g' "${DIR_TMP}/nginx-module-vts/share/status.compress.html"
      sed -i 's|#DED|#43a6df|g' "${DIR_TMP}/nginx-module-vts/share/status.compress.html"
      cd nginx-module-vts/util
      ./tplToDefine.sh ../share/status.template.html > ../src/ngx_http_vhost_traffic_status_module_html.h
      # ./tplToDefine.sh ../share/status.compress.html > ../src/ngx_http_vhost_traffic_status_module_html.h
      cd ../
      # setup /vhost_status.html
      \cp -af "${DIR_TMP}/nginx-module-vts/share/status.compress.html" /usr/local/nginx/html/vhost_status.html
      MAINURIHOST=$(curl -s https://ipinfo.io/ip)
      NEWURI="/vhost_status"
      if [ -f /usr/local/nginx/html/vhost_status.html ]; then
          sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html
          if [[ ! "$(grep '<option value=60>60</option>' /usr/local/nginx/html/vhost_status.html)" ]]; then
            sed -i 's|<option value=8>8</option>|<option value=8>8</option><option value=15>15</option><option value=60>60</option>|' /usr/local/nginx/html/vhost_status.html
          fi
        fi
      elif [[ -d "${DIR_TMP}/nginx-module-vts" && -d "${DIR_TMP}/nginx-module-vts/.git" ]]; then
        rm -rf "${DIR_TMP}/nginx-module-vts"
        cd "$DIR_TMP"
        time git clone "$NGX_VTSLINK" nginx-module-vts
        # sed -i 's|color:       black;|color:       white;|g' "${DIR_TMP}/nginx-module-vts/share/status.template.html"
        sed -i 's|#DED|#43a6df|g' "${DIR_TMP}/nginx-module-vts/share/status.template.html"
        # sed -i 's|color:       black;|color:       white;|g' "${DIR_TMP}/nginx-module-vts/share/status.compress.html"
        sed -i 's|#DED|#43a6df|g' "${DIR_TMP}/nginx-module-vts/share/status.compress.html"
        cd nginx-module-vts/util
        ./tplToDefine.sh ../share/status.template.html > ../src/ngx_http_vhost_traffic_status_module_html.h
        # ./tplToDefine.sh ../share/status.compress.html > ../src/ngx_http_vhost_traffic_status_module_html.h
        cd ../
        # setup /vhost_status.html
        \cp -af "${DIR_TMP}/nginx-module-vts/share/status.compress.html" /usr/local/nginx/html/vhost_status.html
        MAINURIHOST=$(curl -s https://ipinfo.io/ip)
        NEWURI="/vhost_status"
        if [ -f /usr/local/nginx/html/vhost_status.html ]; then
          sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html
          if [[ ! "$(grep '<option value=60>60</option>' /usr/local/nginx/html/vhost_status.html)" ]]; then
            sed -i 's|<option value=8>8</option>|<option value=8>8</option><option value=15>15</option><option value=60>60</option>|' /usr/local/nginx/html/vhost_status.html
          fi
        fi
      fi
    else
      VTSOPT=""
    fi
  else
    VTSOPT=""
  fi

  if [[ "$NGINX_STICKY" = [yY] ]]; then
    if [[ "$NGINX_STICKYVER" = 'master' ]]; then
      NGX_STICKYDIR='nginx-sticky-module-master'
    else
      NGX_STICKYDIR=$(tar -tzf "$DIR_TMP/${NGX_STICKYLINKFILE}" | head -1 | cut -f1 -d"/")
    fi
    STICKYOPT=" --add-module=../$NGX_STICKYDIR"
  else
    STICKYOPT=""
  fi

  if [[ "$NGINX_UPSTREAMCHECK" = [yY] ]]; then
    NGX_UPSTREAMCHECKDIR=$(tar -tzf "$DIR_TMP/${NGX_UPSTREAMCHECKLINKFILE}" | head -1 | cut -f1 -d"/")
    UPSTREAMOPT=" --add-module=../$NGX_UPSTREAMCHECKDIR"
  else
    UPSTREAMOPT=""
  fi

  if [[ "$NGINX_PAGESPEED" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
    # ngxpagespeed_patched
    ngxpagespeed_setpipepatch
    
    # only need to invoke ngxpagespeedinstall at this stage if it is nginx upgrade
    # no need for it to be invoked at initial first time centmin mod install as
    # inc/nginx_install.inc has it's own call to this function when variable
    # NGINX_PAGESPEED=y
    if [[ "$ngver" ]]; then
      ngxpagespeedinstall
    fi
    NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_NGXPAGESPEED" = [yY] ]]; then
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-dynamic-module=../$NGXPGSPEED_DIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_pagespeed.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    elif [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
        # ngxpagespeed_patched
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-module=../$NGXPGSPEED_DIR"
      fi
      # if NGXDYNAMIC_NGXPAGESPEED='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_NGXPAGESPEED='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    else
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-module=../$NGXPGSPEED_DIR"
      fi
      # if NGXDYNAMIC_NGXPAGESPEED='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_NGXPAGESPEED='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    fi
  else
    PGSPEEDOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
  fi

  if [[ "$NGINX_LIBATOMIC" = [yY] ]]; then

    if [ ! -f "/usr/${LIBDIR}/libatomic_ops.a" ]; then
     echo "/usr/${LIBDIR}/libatomic_ops.a not found"
     echo "installing libatomic_ops-devel"
      yum${CACHESKIP} -q -y install libatomic_ops-devel
    else
     echo "/usr/${LIBDIR}/libatomic_ops.a found"
    fi

    LIBATOMICOPT=" --with-libatomic"
  else
    LIBATOMICOPT=""
  fi

  if [[ "$NGINX_PCREJIT" = [yY] ]]; then
    PCRELINKDIR=$(tar -tzf "$DIR_TMP/${PCRELINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$NGINX_PCRE_DYNAMIC" = [yY] && -f /usr/local/lib/libpcre.so ]]; then
      PCREJITOPT=" --with-pcre-jit"
      LIBBROTLIENC_OPT='-L/usr/local/lib '
      LIBBROTLIINC_OPT='-I/usr/local/include '
    else
      PCREJITOPT=" --with-pcre=../$PCRELINKDIR --with-pcre-jit"
    fi
  else
    PCREJITOPT=""
  fi

  if [[ "$NGINX_WEBDAV" = [yY] ]]; then
    NGX_WEBDAVLINKDIR=$(tar -tzf "$DIR_TMP/${NGX_WEBDAVLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ ! -f "/${LIBDIR}/libexpat.so.1" ]; then
     echo "/${LIBDIR}/libexpat.so.1 not found"
     echo "installing expat"
      yum${CACHESKIP} -q -y install expat
    else
     echo "/${LIBDIR}/libexpat.so.1 found"
    fi

    if [ ! -f "/usr/include/expat.h" ]; then
     echo "/usr/include/expat.h not found"
     echo "installing expat-devel"
      yum${CACHESKIP} -q -y install expat-devel
    else
     echo "/usr/include/expat.h found"
    fi

    WEBDAVOPT=" --with-http_dav_module --add-module=../$NGX_WEBDAVLINKDIR"
  else
    WEBDAVOPT=""
  fi

else

  if [[ "$NGINX_DEBUG" = [yY] ]]; then
    NGINX_DEBUGOPT=' --with-debug'
    STRIPNGINX=n
  else
    NGINX_DEBUGOPT=""
  fi  

  if [[ "$NGINX_HTTP2" = [yY] ]]; then  
    # only apply Nginx HTTP/2 if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
    if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]] || [[ "$NGX_VEREVAL" = 'master' ]] || [[ "$NGX_VEREVAL" = 'master-google' ]]; then
      if [[ "$NGINX_SPDYPATCHED" = [yY] ]]; then
        NGINX_SPDY=y
        HTTPTWOOPT=' --with-http_spdy_module --with-http_v2_module'
      else
        NGINX_SPDY=n
        HTTPTWOOPT=' --with-http_v2_module'
      fi
    fi
  else
    HTTPTWOOPT=""
    if [[ "$NGX_VEREVAL" -lt '10903' ]]; then
      NGINX_SPDY=y
    fi    
  fi

  if [[ "$NGINX_STUBSTATUS" = [yY] ]]; then
    STUBSTATUSOPT=" --with-http_stub_status_module"
  else
    STUBSTATUSOPT=""
  fi  

  if [[ "$NGINX_SUB" = [yY] ]]; then
    SUBOPT=" --with-http_sub_module"
  else
    SUBOPT=""
  fi  

  if [[ "$NGINX_ADDITION" = [yY] ]]; then
    ADDITIONOPT=" --with-http_addition_module"
  else
    ADDITIONOPT=""
  fi  

  if [[ "$NGINX_IMAGEFILTER" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_IMAGEFILTER" = [yY] ]]; then
      IMAGEFILTEROPT=" --with-http_image_filter_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_image_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
    else
      IMAGEFILTEROPT=" --with-http_image_filter_module"
      # if NGXDYNAMIC_IMAGEFILTER='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_IMAGEFILTER='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
    fi
  else
    IMAGEFILTEROPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_image_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_image_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_XSLT" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_XSLT" = [yY] ]]; then
      NGXXSLTOPT=" --with-http_xslt_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_xslt_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_module.so.old
      fi
    else
      NGXXSLTOPT=" --with-http_xslt_module"
      # if NGXDYNAMIC_XSLT='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_XSLT='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_xslt_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_module.so.old
      fi
    fi
  else
    NGXXSLTOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_xslt_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_xslt_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_xslt_module.so.old
      fi
  fi

  if [[ "$NGINX_PERL" = [yY] ]]; then
    if [ ! -f /usr/share/perl5/ExtUtils/Embed.pm ]; then
      time $YUMDNFBIN -q -y install perl-ExtUtils-Embed${DISABLEREPO_DNF}
    fi
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_PERL" = [yY] ]]; then
      NGXPERLOPT=" --with-http_perl_module=dynamic"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_perl_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
    else
      NGXPERLOPT=" --with-http_perl_module"
      # if NGXDYNAMIC_PERL='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_PERL='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_perl_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
    fi
  else
    NGXPERLOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_perl_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_perl_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_perl_module.so.old
      fi
  fi

  if [[ "$NGINX_CACHEPURGE" = [yY] ]]; then
    NGX_CACHEPURGEDIR=$(tar -tzf "$DIR_TMP/${NGX_CACHEPURGEFILE}" | head -1 | cut -f1 -d"/")
    CACHEPURGEOPT=" --add-module=../$NGX_CACHEPURGEDIR"
  else
    CACHEPURGEOPT=""
  fi

  if [[ "$NGINX_ACCESSKEY" = [yY] ]]; then
    ACCESSKEYOPT=" --add-module=../nginx-accesskey-2.0.3"
  else
    ACCESSKEYOPT=""
  fi    

  if [[ "$NGINX_HTTPCONCAT" = [yY] ]]; then
    NGX_CONCATDIR=$(tar -tzf "$DIR_TMP/${NGX_CONCATLINKFILE}" | head -1 | cut -f1 -d"/")
    HTTPCONCATOPT=" --add-module=../$NGX_CONCATDIR"
  else
    HTTPCONCATOPT=""
  fi

  if [[ "$ORESTY_HEADERSMORE" = [yY] ]]; then
    NGX_HEADERSMOREDIR=$(tar -tzf "$DIR_TMP/${NGX_HEADERSMORELINKFILE}" | head -1 | cut -f1 -d"/")
    # gitmaster option
    if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
      pushd "$DIR_TMP"
      rm -rf headers-more-nginx-module-master
      time git clone -b master --depth=1 https://github.com/openresty/headers-more-nginx-module.git headers-more-nginx-module-master
      popd
    fi
    # nginx 1.9.11+ dynamic module support 
    # https://github.com/openresty/headers-more-nginx-module#installation
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_HEADERSMORE" = [yY] ]]; then
      if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
        HEADERSMOREOPT=" --add-dynamic-module=../headers-more-nginx-module-master"
      else
        HEADERSMOREOPT=" --add-dynamic-module=../$NGX_HEADERSMOREDIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_headers_more_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
    else
      if [[ "$ORESTY_HEADERSMOREGIT" = [yY] ]]; then
        HEADERSMOREOPT=" --add-module=../headers-more-nginx-module-master"
      else
        HEADERSMOREOPT=" --add-module=../$NGX_HEADERSMOREDIR"
      fi
      # if NGXDYNAMIC_HEADERSMORE='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_HEADERSMORE='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
    fi
  else
    HEADERSMOREOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_headers_more_filter_module.so.old
      fi
  fi

  if [[ "$NGINX_HTTPREDIS" = [yY] ]]; then
    NGX_HTTPREDISDIR=$(tar -tzf "$DIR_TMP/${NGX_HTTPREDISLINKFILE}" | head -1 | cut -f1 -d"/")
    HTTPREDISOPT=" --add-module=../$NGX_HTTPREDISDIR"
  else
    HTTPREDISOPT=""
  fi

  if [[ "$NGINX_AUTHREQ" = [yY] ]]; then
    AUTHREQOPT=' --with-http_auth_request_module'
  else
    AUTHREQOPT=""
  fi

  if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
    LIBRESSLDIR=$(tar -tzf "$DIR_TMP/${LIBRESSL_LINKFILE}" 2>&1 | head -1 | cut -f1 -d"/" | grep libressl)
    LIBRESSLOPT=" --with-openssl=../$LIBRESSLDIR"
    OPENSSLOPT=""
    LRT='-lrt '
  else
    if [ "$NOSOURCEOPENSSL" == 'n' ]; then
      OPENSSLDIR=$(tar -tzf "$DIR_TMP/${OPENSSL_LINKFILE}" | head -1 | cut -f1 -d"/")
      LIBRESSLOPT=""
      if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      elif [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      else
        if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
          if [[ "$(uname -m)" = 'x86_64' ]]; then
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
            TLSTHREEOPT='y'
          else
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
            OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
          fi
        else
          if [[ "$(uname -m)" = 'x86_64' ]]; then
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR --with-openssl-opt='enable-ec_nistp_64_gcc_128'"
          else
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          fi
        fi
      fi
      LRT=""
    else
      OPENSSLDIR=$(tar -tzf "$DIR_TMP/${OPENSSL_LINKFILE}" | head -1 | cut -f1 -d"/")
      # export OPENSSL_BPATH=$DIR_TMP
      LIBRESSLOPT=""
      if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]]; then
        OPENSSL_CUSTOMPATH='/opt/openssl-tls1.3'
        export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../openssl-tls1.3"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      elif [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' && "$ORESTY_LUANGINX" != [yY] ]]; then
        if [[ "$(uname -m)" = 'x86_64' ]]; then
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          TLSTHREEOPT='y'
        else
          OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
        fi
      else
        export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
        if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
          if [[ "$(uname -m)" = 'x86_64' ]]; then
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
            TLSTHREEOPT='y'
          else
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
            OPENSSLOPT_B=" --with-openssl-opt=enable-tls1_3"
          fi
        else
          if [[ "$(uname -m)" = 'x86_64' ]]; then
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR --with-openssl-opt='enable-ec_nistp_64_gcc_128'"
          else
            OPENSSLOPT=" --with-openssl=../$OPENSSLDIR"
          fi
        fi
      fi
      LRT=""
    fi
  fi

  if [[ "$NGINX_SPDY" = [yY] ]]; then
    if [[ "$NGINX_SPDYPATCHED" = [yY] ]]; then
      SPDYOPT=""
    else
      SPDYOPT=" --with-http_spdy_module"
    fi
  else
    SPDYOPT=""
  fi

  if [[ "$NGINX_RTMP" = [yY] ]]; then
    RTMPOPT=" --add-module=../nginx-rtmp-module"
  else
    RTMPOPT=""
  fi    

  if [[ "$NGINX_FLV" = [yY] ]]; then
    FLVOPT=" --with-http_flv_module"
  else
    FLVOPT=""
  fi    

  if [[ "$NGINX_MP4" = [yY] ]]; then
    MPOPT=" --with-http_mp4_module"
  else
    MPOPT=""
  fi    

  if [[ "$NGINX_SECURELINK" = [yY] ]]; then
    SECURELINKOPT=" --with-http_secure_link_module"
  else
    SECURELINKOPT=""
  fi  

  if [[ "$NGINX_FANCYINDEX" = [yY] ]]; then
    NGX_FANCYINDEXDIR=$(tar -tzf "$DIR_TMP/${NGX_FANCYINDEXLINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_FANCYINDEX" = [yY] ]]; then
      FANCYINDEXOPT=" --add-dynamic-module=../$NGX_FANCYINDEXDIR"
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_fancyindex_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
    else
      FANCYINDEXOPT=" --add-module=../$NGX_FANCYINDEXDIR"
      # if NGXDYNAMIC_FANCYINDEX='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_fancyindex_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_FANCYINDEX='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
    fi
  else
    FANCYINDEXOPT=""
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_fancyindex_module.so.old
      fi
  fi

  if [[ "$NGINX_STICKY" = [yY] ]]; then
    NGX_STICKYDIR=$(tar -tzf "$DIR_TMP/${NGX_STICKYLINKFILE}" | head -1 | cut -f1 -d"/")
    STICKYOPT=" --add-module=../$NGX_STICKYDIR"
  else
    STICKYOPT=""
  fi

  if [[ "$NGINX_UPSTREAMCHECK" = [yY] ]]; then
    NGX_UPSTREAMCHECKDIR=$(tar -tzf "$DIR_TMP/${NGX_UPSTREAMCHECKLINKFILE}" | head -1 | cut -f1 -d"/")
    UPSTREAMOPT=" --add-module=../$NGX_UPSTREAMCHECKDIR"
  else
    UPSTREAMOPT=""
  fi

  if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
    # ngxpagespeed_patched
    ngxpagespeed_setpipepatch
    
    # only need to invoke ngxpagespeedinstall at this stage if it is nginx upgrade
    # no need for it to be invoked at initial first time centmin mod install as
    # inc/nginx_install.inc has it's own call to this function when variable
    # NGINX_PAGESPEED=y
    if [[ "$ngver" ]]; then
      ngxpagespeedinstall
    fi
    NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/${NGX_PAGESPEEDLINKFILE}" | head -1 | cut -f1 -d"/")
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_NGXPAGESPEED" = [yY] ]]; then
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-dynamic-module=../$NGXPGSPEED_DIR"
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_pagespeed.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    elif [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
        # ngxpagespeed_patched
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-module=../$NGXPGSPEED_DIR"
      fi
      # if NGXDYNAMIC_NGXPAGESPEED='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_NGXPAGESPEED='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    else
      if [ -d "$DIR_TMP/$NGXPGSPEED_DIR" ]; then
          PGSPEEDOPT=" --add-module=../$NGXPGSPEED_DIR"
      fi
      # if NGXDYNAMIC_NGXPAGESPEED='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_NGXPAGESPEED='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
    fi
  else
    PGSPEEDOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_pagespeed.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_pagespeed.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_pagespeed.so.old
      fi
  fi

  if [[ "$NGINX_LIBATOMIC" = [yY] ]]; then
    LIBATOMICOPT=""
  else
    LIBATOMICOPT=""
  fi

  if [[ "$NGINX_PCREJIT" = [yY] ]]; then
    PCREJITOPT=""
  else
    PCREJITOPT=""
  fi

  if [[ "$NGINX_WEBDAV" = [yY] ]]; then
    NGX_WEBDAVLINKDIR=$(tar -tzf "$DIR_TMP/${NGX_WEBDAVLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ ! -f "/${LIBDIR}/libexpat.so.1" ]; then
     echo "/${LIBDIR}/libexpat.so.1 not found"
      yum${CACHESKIP} -q -y install expat expat-devel
    else
     echo "/${LIBDIR}/libexpat.so.1 found"
    fi

    WEBDAVOPT=" --with-http_dav_module --add-module=../$NGX_WEBDAVLINKDIR"
  else
    WEBDAVOPT=""
  fi

fi

if [[ "$NGINX_PASSENGER" = [yY] && "$RUBY_VERSION" ]]; then
  export PATH=/usr/local/rvm/gems/${RUBY_VERSION}/bin:/usr/local/rvm/rubies/${RUBY_VERSION}/bin:$PATH
  PASSENGERS_PATH=$(/bin/find /usr/local/rvm/gems -name 'passenger-config' | grep bin | tail -1)
  echo
  echo "RUBY_VERSION"
  echo "$RUBY_VERSION"
  echo
  echo "passenger-config --nginx-addon-dir"
  passenger-config --nginx-addon-dir
  # sleep 10
fi

if [[ "$NGINX_PASSENGER" = [yY] && -d "$(passenger-config --nginx-addon-dir)" ]]; then
  # [[ -s "/etc/profile.d/rvm.sh" ]] && source "/etc/profile.d/rvm.sh"  # This loads RVM into a shell session.
  sed -i 's/#passenger_/passenger_/g' /usr/local/nginx/conf/passenger.conf
  RUBYMODULE=" --add-module=$(passenger-config --nginx-addon-dir)"
  echo
  echo "RUBY_VERSION"
  echo $RUBY_VERSION
  echo
  echo "RUBYMODULE"
  echo $RUBYMODULE
  echo
  # /usr/bin/env
  # echo
else
  RUBYMODULE=""
fi

# disable Clang compiler for Nginx if NGINX_PASSENGER=y as Clang fails
# to compile Passenger Nginx Module while GCC compiler works
if [[ "$NGINX_PASSENGER" = [yY] ]]; then
  CLANG='n'
fi

# ngx_pagespeed => 1.10 switch back to GCC instead of Clang compiler
# https://community.centminmod.com/posts/22882/
if [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo $NGXPGSPEED_VER | cut -d . -f2)" -ge '10' ]] && [[ "$NGINX_PAGESPEED" = [yY] ]] && [[ "$CENTOS_SIX" = '6' ]]; then
  CLANG='n'
elif [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo $NGXPGSPEED_VER | cut -d . -f2)" -ge '10' ]] && [[ "$NGINX_PAGESPEED" = [yY] ]] && [[ "$CENTOS_SEVEN" = '7' ]]; then
  CLANG='n'
elif [[ "$NGINX_PAGESPEED" = [yY] ]]; then
  CLANG='n'
fi

if [[ "$NGINX_LIBBROTLI" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
  #libbroti_install
  ngxbrotli_download
  # initial installs don't have nginx directories setup from inc/brotli.inc routines yet
  # so need to do it at the stage of initial first time nginx installs
  if [[ "$INITIALINSTALL" = [yY] && ! -f /usr/local/nginx/conf/brotli_inc.conf ]]; then
cat > "/usr/local/nginx/conf/brotli_inc.conf" <<EIF
brotli on;
brotli_static on;
brotli_min_length 1000;
brotli_buffers 1024 8k;
brotli_comp_level 5;
brotli_types text/plain text/css text/xml application/javascript application/x-javascript application/xml application/xml+rss application/ecmascript application/json image/svg+xml;
EIF
    BROTLI_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/brotli_inc.conf' /usr/local/nginx/conf/nginx.conf)
    if [[ -z "$BROTLI_INCLUDECHECK" && -f /usr/local/nginx/conf/brotli_inc.conf ]]; then
          sed -i 's/http {/http { \n include \/usr\/local\/nginx\/conf\/brotli_inc.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
    sed -i 's|^ #include \/usr\/local\/nginx\/conf\/brotli_inc.conf;| include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
  fi
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_BROTLI" = [yY] ]]; then
    BROTLIOPT=' --add-dynamic-module=../ngx_brotli'
    LIBBROTLIENC_OPT='-L/usr/local/lib '
    LIBBROTLIINC_OPT='-I/usr/local/include '
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      echo "load_module \"modules/ngx_http_brotli_filter_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      echo "load_module \"modules/ngx_http_brotli_static_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old
    fi
  else
    BROTLIOPT=' --add-module=../ngx_brotli'
    LIBBROTLIENC_OPT='-L/usr/local/lib '
    LIBBROTLIINC_OPT='-I/usr/local/include '
    # if NGXDYNAMIC_BROTLI='y' was previously set with nginx 1.9.11+, then the nginx
    # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
    # created
    # however, it seems if you then later recompile nginx with NGXDYNAMIC_BROTLI='y'
    # the old dynamic module would still be left in nginx dynamic modules directory
    # this is a check for that old dynamic module and if it exists, remove it
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_static_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_static_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old
    fi
  fi
  if [[ "$NGINX_LIBBROTLISTATIC" = [yY] ]]; then
    export NGX_BROTLI_STATIC_MODULE_ONLY=1
  fi
  sed -i 's|^ #include \/usr\/local\/nginx\/conf\/brotli_inc.conf;| include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
else
  BROTLIOPT=""
  # disable include file if exists
  sed -i 's|^ include \/usr\/local\/nginx\/conf\/brotli_inc.conf;| #include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_static_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_static_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so.old
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_brotli_static_module.so.old
    fi
fi

if [[ -d "${DIR_TMP}/nginx-${NGINX_VERSION}" && ! "$ngver" ]]; then
  cd "${DIR_TMP}/nginx-${NGINX_VERSION}"
fi

if [[ -d "${DIR_TMP}/nginx-${ngver}" && "$ngver" ]]; then
  cd "${DIR_TMP}/nginx-${ngver}"
fi

if [ -f src/http/ngx_http_header_filter_module.c ]; then
  if [[ "$CUSTOMSERVERNAME" == [yY] ]]; then
    echo ""
    echo "Check existing server string:"
  grep "Server: " src/http/ngx_http_header_filter_module.c | grep -v full_string
    echo ""
    echo "Change server string to $CUSTOMSERVERSTRING"
  sed -i "s/Server: nginx/Server: $CUSTOMSERVERSTRING/g" src/http/ngx_http_header_filter_module.c
    echo ""
  fi
fi

if [[ -f src/core/ngx_log.h && "$NGINX_MAXERRBYTELIMIT" -ne '2048' && ! -z "$NGINX_MAXERRBYTELIMIT" ]]; then
  sed -i "s|^#define NGX_MAX_ERROR_STR.*|#define NGX_MAX_ERROR_STR   $NGINX_MAXERRBYTELIMIT|" src/core/ngx_log.h
  echo "NGX_MAX_ERROR_STR value"
  grep -w 'NGX_MAX_ERROR_STR' src/core/ngx_log.h
  echo
else
  echo "NGX_MAX_ERROR_STR value"
  grep -w 'NGX_MAX_ERROR_STR' src/core/ngx_log.h
  echo
fi

if [ ! -f /usr/bin/jemalloc.sh ]; then
  time $YUMDNFBIN -y install jemalloc jemalloc-devel${DISABLEREPO_DNF}
  if [[ -f /usr/bin/jemalloc.sh && -d /usr/include/jemalloc ]]; then
    if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
      JEMALLOC_LD=""
    else
      JEMALLOC_LD='-ljemalloc'
    fi
  fi
elif [[ -f /usr/bin/jemalloc.sh && -d /usr/include/jemalloc ]]; then
  if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
    JEMALLOC_LD=""
  else
    JEMALLOC_LD='-ljemalloc'
  fi
fi

if [[ -f /usr/local/bin/pcretest ]] && [[ "$ORESTY_LUANGINX" = [yY] ]]; then
  PCRE_LD=' -lpcre'
else
  PCRE_LD=""
fi

# NGINX 1.9 stream support & 1.8 threads
# http://nginx.org/en/docs/stream/ngx_stream_core_module.html
NGXINSTALL_VER=$(echo "$NGINX_VERSION" | cut -d . -f1,2)
NGXUPGRADE_VER=$(echo "$ngver" | cut -d . -f1,2)
if [[ "$ngver" && "$NGXUPGRADE_VER" = '1.9' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.10' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.11' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.12' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.13' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.14' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = '1.15' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = 'master' ]] || [[ "$ngver" && "$NGXUPGRADE_VER" = 'master-google' ]]; then
# if [[ "$ngver" && "$NGXUPGRADE_VER" > '1.9' ]]; then
  if [[ "$NGINX_STREAM" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_STREAM" = [yY] ]]; then
      STREAM=' --with-stream=dynamic --with-stream_ssl_module'
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_stream_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
      fi
    else
      STREAM=' --with-stream --with-stream_ssl_module'
      # if NGXDYNAMIC_STREAM='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_STREAM='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_stream_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
      fi
    fi
  else
    STREAM=""
    if [ -f /usr/local/nginx/modules/ngx_stream_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
    fi
  fi
  THREADS=' --with-threads'
  if [ -f /usr/local/nginx/conf/redisupstream.conf ]; then
    sed -i "s|^#include \/usr\/local\/nginx\/conf\/redisupstream.conf|include \/usr\/local\/nginx\/conf\/redisupstream.conf|" /usr/local/nginx/conf/nginx.conf
  fi
  if [[ -z "$(grep ' reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
    sed -i "s|backlog=.*|backlog=2048 reuseport;|" /usr/local/nginx/conf/conf.d/virtual.conf
  fi
  # workaround for nginx 1.9 compatibility
  # sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_initworkerby.c
  # sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_timer.c
elif [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.9' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.10' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.11' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.12' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.13' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.14' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.15' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = 'master' ]] || [[ -z "$ngver" && "$NGXINSTALL_VER" = 'master-google' ]]; then
  if [[ "$NGINX_STREAM" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_STREAM" = [yY] ]]; then
      STREAM=' --with-stream=dynamic --with-stream_ssl_module'
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_stream_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
      fi
    else
      STREAM=' --with-stream --with-stream_ssl_module'
      # if NGXDYNAMIC_STREAM='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_STREAM='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_stream_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
      fi
    fi
  else
    STREAM=""
      if [[ -f /usr/local/nginx/modules/ngx_stream_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_module.so.old
      fi
  fi
  THREADS=' --with-threads'
  if [ -f /usr/local/nginx/conf/redisupstream.conf ]; then
    sed -i "s|^#include \/usr\/local\/nginx\/conf\/redisupstream.conf|include \/usr\/local\/nginx\/conf\/redisupstream.conf|" /usr/local/nginx/conf/nginx.conf
  fi
  if [[ -z "$(grep ' reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
    sed -i "s|backlog=.*|backlog=2048 reuseport;|" /usr/local/nginx/conf/conf.d/virtual.conf
  fi
  # workaround for nginx 1.9 compatibility
  # sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_initworkerby.c
  # sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_timer.c  
elif [[ "$ngver" && "$NGXUPGRADE_VER" = '1.8' ]]; then
  STREAM=""
  THREADS=' --with-threads'
  if [ -f /usr/local/nginx/conf/redisupstream.conf ]; then
    sed -i "s|^include \/usr\/local\/nginx\/conf\/redisupstream.conf|#include \/usr\/local\/nginx\/conf\/redisupstream.conf|" /usr/local/nginx/conf/nginx.conf
  fi
  if [[ "$(grep ' reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
    sed -i "s| reuseport||" /usr/local/nginx/conf/conf.d/virtual.conf
  fi
elif [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.8' ]]; then
  STREAM=""
  THREADS=' --with-threads'
  if [ -f /usr/local/nginx/conf/redisupstream.conf ]; then
    sed -i "s|^include \/usr\/local\/nginx\/conf\/redisupstream.conf|#include \/usr\/local\/nginx\/conf\/redisupstream.conf|" /usr/local/nginx/conf/nginx.conf
  fi
  if [[ "$(grep ' reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
    sed -i "s| reuseport||" /usr/local/nginx/conf/conf.d/virtual.conf
  fi
else
  STREAM=""
  THREADS=""
  if [ -f /usr/local/nginx/conf/redisupstream.conf ]; then
    sed -i "s|^include \/usr\/local\/nginx\/conf\/redisupstream.conf|#include \/usr\/local\/nginx\/conf\/redisupstream.conf|" /usr/local/nginx/conf/nginx.conf
  fi
  if [[ "$(grep ' reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
    sed -i "s| reuseport||" /usr/local/nginx/conf/conf.d/virtual.conf
  fi  
fi

if [[ "$NGINX_STREAMGEOIP" = [yY] ]]; then
  NGINX_GEOIP='y'
  #NGINX_GEOIPTWOLITE='y'
  NGX_STREAMGEOIPCHECK=$(grep 'STREAM_GEOIP=' "$NGINX_DIRINSTALL/auto/options")
  if [[ ! -d /usr/share/GeoIP ]]; then
    geoipinstall
  fi
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_STREAMGEOIP" = [yY] ]] && [ "$NGX_STREAMGEOIPCHECK" ]; then
    NGXDYNAMIC_GEOIP='y'
    STREAMGEOIPOPT=' --with-stream_geoip_module=dynamic'
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      echo "load_module \"modules/ngx_stream_geoip_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_geoip_module.so.old
    fi
  elif [ "$NGX_STREAMGEOIPCHECK" ]; then
    STREAMGEOIPOPT=' --with-stream_geoip_module'
    if [ -f /usr/local/nginx/modules/ngx_stream_geoip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_geoip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_geoip_module.so.old
    fi
  else
    STREAMGEOIPOPT=''
  fi
else
  STREAMGEOIPOPT=''
    if [ -f /usr/local/nginx/modules/ngx_stream_geoip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_geoip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_geoip_module.so.old
    fi
fi

if [[ "$NGINX_STREAMREALIP" = [yY] ]]; then
  NGINX_GEOIP='y'
  # NGINX_GEOIPTWOLITE='y'
  NGX_STREAMREALIPCHECK=$(grep 'STREAM_REALIP=' "$NGINX_DIRINSTALL/auto/options")
  if [[ ! -d /usr/share/GeoIP ]]; then
    geoipinstall
  fi
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_STREAMREALIP" = [yY] ]] && [ "$NGX_STREAMREALIPCHECK" ]; then
    NGXDYNAMIC_GEOIP='y'
    #STREAMREALIPOPT=' --with-stream_realip_module=dynamic'
    # dynamic module isn't supported yet
    STREAMREALIPOPT=' --with-stream_realip_module'
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      # echo "load_module \"modules/ngx_stream_realip_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      echo "skip modules/ngx_stream_realip_module.so not supported yet"
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_realip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_realip_module.so.old
    fi
  elif [ "$NGX_STREAMREALIPCHECK" ]; then
    STREAMREALIPOPT=' --with-stream_realip_module'
    if [ -f /usr/local/nginx/modules/ngx_stream_realip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_realip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_realip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_realip_module.so.old
    fi
  else
    STREAMREALIPOPT=''
  fi
else
  STREAMREALIPOPT=''
    if [ -f /usr/local/nginx/modules/ngx_stream_realip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_realip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_stream_realip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_stream_realip_module.so.old
    fi
fi

  if [[ "$NGINX_NJS" = [yY] ]]; then
    if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_NJS" = [yY] ]]; then
      NGINX_NJSOPT=' --add-dynamic-module=../njs/nginx'
      if [ ! -f /usr/bin/hg ]; then
        time $YUMDNFBIN -y install mercurial${DISABLEREPO_DNF}
      fi
      if [[ -f /usr/bin/hg && ! -d "${DIR_TMP}/njs" ]]; then
        pushd "$DIR_TMP"
        hg clone https://hg.nginx.org/njs/
        popd
      elif [ -f /usr/bin/hg ]; then
        pushd "${DIR_TMP}/njs"
        hg pull
        popd
      fi
      if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
        echo "load_module \"modules/ngx_http_js_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
        echo "load_module \"modules/ngx_stream_js_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_js_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_js_module.so.old
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_js_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_js_module.so.old
      fi
    else
      NGINX_NJSOPT=' --add-module=../njs/nginx'
      if [ ! -f /usr/bin/hg ]; then
        time $YUMDNFBIN -y install mercurial${DISABLEREPO_DNF}
      fi
      if [[ -f /usr/bin/hg && ! -d "${DIR_TMP}/njs" ]]; then
        pushd "$DIR_TMP"
        hg clone https://hg.nginx.org/njs/
        popd
      elif [ -f /usr/bin/hg ]; then
        pushd "${DIR_TMP}/njs"
        hg pull
        popd
      fi
      # if NGXDYNAMIC_NJS='y' was previously set with nginx 1.9.11+, then the nginx
      # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
      # created
      # however, it seems if you then later recompile nginx with NGXDYNAMIC_NJS='y'
      # the old dynamic module would still be left in nginx dynamic modules directory
      # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_http_js_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_js_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_js_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_js_module.so.old
      fi
        # this is a check for that old dynamic module and if it exists, remove it
      if [[ -f /usr/local/nginx/modules/ngx_stream_js_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_js_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_stream_js_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_stream_js_module.so.old
      fi
    fi
  else
    NGINX_NJSOPT=""
      if [[ -f /usr/local/nginx/modules/ngx_http_js_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
        rm -rf /usr/local/nginx/modules/ngx_http_js_module.so
      fi
      if [ -f /usr/local/nginx/modules/ngx_http_js_module.so.old ]; then
        rm -rf /usr/local/nginx/modules/ngx_http_js_module.so.old
      fi
  fi

if [[ "$NGINX_GEOIP" = [yY] ]]; then
  if [[ ! -d /usr/share/GeoIP ]]; then
    geoipinstall
  fi
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_GEOIP" = [yY] ]]; then
    GEOIPOPT=' --with-http_geoip_module=dynamic'
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      echo "load_module \"modules/ngx_http_geoip_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip_module.so.old
    fi
  else
    GEOIPOPT=' --with-http_geoip_module'
    # if NGXDYNAMIC_GEOIP='y' was previously set with nginx 1.9.11+, then the nginx
    # dynamic module /usr/local/nginx/modules/ngx_http_lua_module.so would of been
    # created
    # however, it seems if you then later recompile nginx with NGXDYNAMIC_GEOIP='y'
    # the old dynamic module would still be left in nginx dynamic modules directory
    # this is a check for that old dynamic module and if it exists, remove it
    if [ -f /usr/local/nginx/modules/ngx_http_geoip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip_module.so.old
    fi
  fi
else
  GEOIPOPT=''
    if [ -f /usr/local/nginx/modules/ngx_http_geoip_module.so ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip_module.so
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_geoip_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip_module.so.old
    fi
fi

if [[ "$NGINX_GEOIPTWOLITE" = [yY] ]]; then
  geotwoinccheck
  sed -i 's|^#include /usr/local/nginx/conf/geoip2.conf;|include /usr/local/nginx/conf/geoip2.conf;|' /usr/local/nginx/conf/geoip.conf
  if [[ ! -f /usr/local/bin/mmdblookup || ! -f /usr/share/GeoIP/GeoLite2-Country.mmdb ]]; then
    geoiptwolite_install
  fi
  if [ ! -d ../ngx_http_geoip2_module ]; then
    pushd $DIR_TMP
    git clone https://github.com/leev/ngx_http_geoip2_module
    popd
  elif [ -d ../ngx_http_geoip2_module ]; then
    rm -rf ../ngx_http_geoip2_module
    pushd $DIR_TMP
    git clone https://github.com/leev/ngx_http_geoip2_module
    popd
  fi
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$NGXDYNAMIC_GEOIPTWOLITE" = [yY] ]]; then
    GEOIPTWOLITEOPT=' --add-dynamic-module=../ngx_http_geoip2_module'
    if [ -f /usr/local/nginx/conf/dynamic-modules.conf ]; then
      echo "load_module \"modules/ngx_http_geoip2_module.so\";" >> /usr/local/nginx/conf/dynamic-modules.conf
    fi
    if [ -f /usr/local/nginx/modules/ngx_http_geoip2_module.so.old ]; then
      rm -rf /usr/local/nginx/modules/ngx_http_geoip2_module.so.old
    fi
  else
    GEOIPTWOLITEOPT=' --add-module=../ngx_http_geoip2_module'
  fi
else
  GEOIPTWOLITEOPT=''
  sed -i 's|^include /usr/local/nginx/conf/geoip2.conf;|#include /usr/local/nginx/conf/geoip2.conf;|' /usr/local/nginx/conf/geoip.conf
fi

if [[ "$NGINX_THREADS" = [yY] ]]; then
  if [[ "$NGX_VEREVAL" -ge '10800' ]] || [[ "$NGX_VEREVAL" = 'master' ]] || [[ "$NGX_VEREVAL" = 'master-google' ]]; then
    THREADS=' --with-threads'
  else
    THREADS=""
  fi
else
  THREADS=""
fi  

# Enable TCP Fast Open if system supports it
# https://en.wikipedia.org/wiki/TCP_Fast_Open
if [[ "$CHECK_LXD" != [yY] ]]; then
  fastopenconfig
fi

# on low memory systems <=768MB or when NGINX_GEOIPMEM=n
# optimise nginx memory usage when nginx geoip module is 
# enabled (by default) by changing from in memory caching
# of geoip country, city databases to on disk caching 
ngxgeoipmemtweak

if [[ "$CLANG" = [yY] ]]; then
  if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/clang ]] || [[ "$CENTOS_SEVEN" = '7' && ! -f /bin/clang ]]; then
    time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
  fi
  if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
    if [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -ge '10' ]] && [[ "$NGINX_PAGESPEED" = [yY] ]] && [[ "$CENTOS_SIX" = '6' ]]; then
      scl_install
    fi
  fi
  # ngx_pagespeed => 1.10
  if [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -ge '10' ]] && [[ "$NGINX_PAGESPEED" = [yY] ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    CCTOOLSET='--gcc-toolchain=/opt/rh/devtoolset-6/root/usr/'
    #CCTOOLSETOPT=' --gcc-toolchain=/opt/rh/devtoolset-4/root/usr/'
    #export CFLAGS="$CCTOOLSET"
  else
    CCTOOLSET=""
    CCTOOLSETOPT=""
  fi
  # ccache compiler has some initial overhead for compiles but speeds up subsequent
  # recompiles. however on initial install ccache has no benefits, so for initial
  # centmin mod install disabling ccache will in theory speed up first time installs

# llbm-toolset-7 for clang 4 isn't ready for centos 6.x yet
# if [[ "$CENTOS_SIX" = '6' && "$CLANG_FOUR" = [yY] ]]; then
#   if [[ ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang && ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc ]]; then
#     scl_install
#     CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
#   elif [[ -f /opt/rh/llvm-toolset-7/root/usr/bin/clang && -f /opt/rh/devtoolset-7/root/usr/bin/gcc ]]; then
#     CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
#   else
#     CLANG_NEWBIN='/opt/sbin/llvm-release_40/bin/clang'
#   fi
# elif [[ "$CENTOS_SIX" = '6' && "$CLANG_FIVE" = [yY] ]]; then
#   CLANG_NEWBIN='/opt/sbin/llvm-release_50/bin/clang'
# fi

if [[ "$CENTOS_SEVEN" = '7' && "$CLANG_FOUR" = [yY] ]]; then
  if [[ ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang && ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc ]]; then
    scl_install
    CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
  elif [[ ! -f /opt/rh/llvm-toolset-7/root/usr/bin/clang && -f /opt/rh/devtoolset-7/root/usr/bin/gcc ]]; then
    scl_install
    CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
  elif [[ -f /opt/rh/llvm-toolset-7/root/usr/bin/clang && -f /opt/rh/devtoolset-7/root/usr/bin/gcc ]]; then
    CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
  else
    CLANG_NEWBIN='/opt/sbin/llvm-release_40/bin/clang'
  fi
elif [[ "$CENTOS_SEVEN" = '7' && "$CLANG_FIVE" = [yY] && -f /opt/rh/llvm-toolset-7/root/usr/bin/clang ]]; then
  CLANG_NEWBIN='/opt/rh/llvm-toolset-7/root/usr/bin/clang'
elif [[ "$CENTOS_SEVEN" = '7' && "$CLANG_FIVE" = [yY] && -f /opt/sbin/llvm-release_50/bin/clang ]]; then
  CLANG_NEWBIN='/opt/sbin/llvm-release_50/bin/clang'
elif [[ "$CENTOS_SEVEN" = '7' && "$CLANG_SIX" = [yY] && -f /opt/sbin/llvm-release_60/bin/clang ]]; then
  CLANG_NEWBIN='/opt/sbin/llvm-release_60/bin/clang'
elif [[ "$CENTOS_SEVEN" = '7' && "$CLANG_MASTER" = [yY] ]]; then
  CLANG_NEWBIN='/opt/sbin/llvm-release_master/bin/clang'
fi

if [[ "$CENTOS_SEVEN" = '7' && "$CLANG_FOUR" = [yY] && -f "${CLANG_NEWBIN}" ]] || [[ "$CENTOS_SEVEN" = '7' && "$CLANG_FIVE" = [yY] && -f "${CLANG_NEWBIN}" ]] || [[ "$CENTOS_SEVEN" = '7' && "$CLANG_SIX" = [yY] && -f "${CLANG_NEWBIN}" ]]; then
  # if [ -f /opt/sbin/llvm/lib/LLVMgold.so ]; then
  #   LLVMLTO_OPT=' -flto'
  #   # export AR="$BUILD_DIR/llvmgold.binutils/binutils/ar"
  #   # export NM="$BUILD_DIR/llvmgold.binutils/binutils/nm-new"
  #   # export RANLIB=/bin/true
  # else
  #   LLVMLTO_OPT=""
  # fi
  ############################
  if [[ "$INITIALINSTALL" != [yY] ]]; then
    export CC="ccache ${CLANG_NEWBIN}${LLVMLTO_OPT} -ferror-limit=0${CCTOOLSET}"
    export CXX="ccache ${CLANG_NEWBIN}++${LLVMLTO_OPT} -ferror-limit=0"
    export CCACHE_CPP2=yes
    CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'
echo "
export CC=\"ccache ${CLANG_NEWBIN}${LLVMLTO_OPT} -ferror-limit=0${CCTOOLSET}\"
export CXX=\"ccache ${CLANG_NEWBIN}++${LLVMLTO_OPT} -ferror-limit=0\"
export CCACHE_CPP2=yes
CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'"
  else
    export CC="${CLANG_NEWBIN}${LLVMLTO_OPT} -ferror-limit=0${CCTOOLSET}"
    export CXX="${CLANG_NEWBIN}++${LLVMLTO_OPT} -ferror-limit=0"
    # export CCACHE_CPP2=yes
    CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'
echo "  
export CC=\"${CLANG_NEWBIN}${LLVMLTO_OPT} -ferror-limit=0${CCTOOLSET}\"
export CXX=\"${CLANG_NEWBIN}++${LLVMLTO_OPT} -ferror-limit=0\"
# export CCACHE_CPP2=yes
CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'"
  fi
  ############################
  ## clang 4.0 else
else
  ## clang 4.0 else
  ############################
  if [[ "$INITIALINSTALL" != [yY] ]]; then
    export CC="ccache /usr/bin/clang -ferror-limit=0${CCTOOLSET}"
    export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
    export CCACHE_CPP2=yes
    CLANG_CCOPT=' -Wno-sign-compare -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'
echo "
export CC=\"ccache /usr/bin/clang -ferror-limit=0${CCTOOLSET}\"
export CXX=\"ccache /usr/bin/clang++ -ferror-limit=0\"
export CCACHE_CPP2=yes
CLANG_CCOPT=' -Wno-sign-compare -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'"
  else
    export CC="/usr/bin/clang -ferror-limit=0${CCTOOLSET}"
    export CXX="/usr/bin/clang++ -ferror-limit=0"
    # export CCACHE_CPP2=yes
    CLANG_CCOPT=' -Wno-sign-compare -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'
echo "  
export CC=\"/usr/bin/clang -ferror-limit=0${CCTOOLSET}\"
export CXX=\"/usr/bin/clang++ -ferror-limit=0\"
# export CCACHE_CPP2=yes
CLANG_CCOPT=' -Wno-sign-compare -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings'"
  fi
  ############################
fi # clang 4.0
else
  CLANG_CCOPT=""
  echo "CLANG_CCOPT=\"\""
fi

# devtoolset-4 usage for ngx_pagespeed 1.10 branch which doesn't support
# centos 6 based gcc 4.4.7
if [[ "$CENTOS_SIX" = '6' && "$CLANG" = [nN] && "$(uname -m)" = 'x86_64' ]]; then
  if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
    scl_install
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        source /opt/gcc8/enable
        DEVTOOLSETEIGHT_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETEIGHT_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type'
        NGX_GSPLITDWARF='n'
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
        source /opt/rh/devtoolset-7/enable
        DEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
      elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        source /opt/rh/devtoolset-6/enable
      else
       source /opt/rh/devtoolset-6/enable
      fi
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        source /opt/gcc8/enable
        DEVTOOLSETEIGHT_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETEIGHT_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type'
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
        source /opt/rh/devtoolset-7/enable
        DEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
      elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        source /opt/rh/devtoolset-6/enable
      else
       source /opt/rh/devtoolset-6/enable
      fi
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
    # update devtoolset-7 repo
    # place holder if need to update devtoolset-7 in future
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-7/root/usr/bin/gcc"
    DEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
    DEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-7/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-7/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
    # update devtoolset-6 repo
    # place holder if need to update devtoolset-6 in future
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
    # update devtoolset-4 repo
    # place holder if need to update devtoolset-4 in future
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  fi
fi

# devtoolset-4 usage of GCC 4.9 even for CentOS7
if [[ "$CENTOS_SEVEN" = '7' && "$CLANG" = [nN] && "$NGINX_DEVTOOLSETGCC" = [yY] ]]; then
  if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
    scl_install
    if [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-7/root/usr/bin/gcc"
      DEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
      DEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
    elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    else
      export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    fi
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      DEVTOOLSETEIGHT_FALLTHROUGH=' -Wimplicit-fallthrough=0'
      DEVTOOLSETEIGHT_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type'
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        source /opt/gcc8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        NGX_GSPLITDWARF='n'
      else
        source /opt/gcc8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="gcc"
        export CXX="g++"
        NGX_GSPLITDWARF='n'
      fi
    elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="gcc"
        export CXX="g++"
      fi
    elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="gcc"
        export CXX="g++"
      fi
    else
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="gcc"
        export CXX="g++"
      fi
    fi
  elif [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/gcc8/bin/gcc"
    DEVTOOLSETEIGHT_FALLTHROUGH=' -Wimplicit-fallthrough=0'
    DEVTOOLSETEIGHT_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type'
    NGX_GSPLITDWARF='n'
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/gcc8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/gcc8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi    
  elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-7/root/usr/bin/gcc"
    DEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
    DEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-7/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-7/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
    export PS_NGX_EXTRA_FLAGS="--with-cc=/opt/rh/devtoolset-6/root/usr/bin/gcc"
    # PS_FLAGS=$(echo " $PS_NGX_EXTRA_FLAGS")
    PS_FLAGS=""
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  fi
fi

# intel specific
CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | tee ${CENTMINLOGDIR}/gcc_native.log
if [[ "$CLANG" = [nN] ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' ]] && [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
  CCM=64
  MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${NGX_FOPENOPT} "
elif [[ -f "${CLANG_NEWBIN}" && "$CLANG" = [yY] ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' ]] && [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
  CCM=64
  MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${NGX_FOPENOPT} "
elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
  CCM=64
  MTUNEOPT="-m${CCM} -mtune=native${NGX_FOPENOPT} "
elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'AuthenticAMD' ]]; then
  CCM=64
  MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${NGX_FOPENOPT} "
elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
  CCM=32
  MTUNEOPT="-m${CCM} -mtune=generic${NGX_FOPENOPT} "
else
  MTUNEOPT=""
fi

# fix no longer needed with libressl 2.3.3
# https://community.centminmod.com/threads/libressl-2-3-3-released.6788/
# libresslcompile_fix

if [[ "$CLANG" = [yY] ]]; then
  if [[ "$NGINX_PASSENGER" = [yY] ]]; then
    # https://community.centminmod.com/posts/36274/
    GCC_OPTLEVEL=' -O2'
  else
    if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
      GCC_OPTLEVEL=' -O2'
    else
      if [[ "$NGINX_HPACK" = [yY] ]]; then
        GCC_OPTLEVEL=" -O3 -Wno-error=strict-aliasing"
      else
        GCC_OPTLEVEL=' -O3'
      fi
    fi
  fi
  FSTACKPROTECT='-fstack-protector'
else
  if [[ "$NGINX_PASSENGER" = [yY] ]]; then
    # https://community.centminmod.com/posts/36274/
    GCC_OPTLEVEL=' -O2'
  else
    if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
      GCC_OPTLEVEL=' -O2'
    else
      if [[ "$NGINX_HPACK" = [yY] ]]; then
        GCC_OPTLEVEL=" -O3 -Wno-error=strict-aliasing"
      else
        GCC_OPTLEVEL=' -O3'
      fi
    fi
  fi
  if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -lt '40805' ]]; then
    FSTACKPROTECT='-fstack-protector'
  elif [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ && "$CENTOS_SIX" = '6' ]]; then
    FSTACKPROTECT='-fstack-protector-strong'
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' ]]; then
    FSTACKPROTECT='-fstack-protector-strong'
  else
    FSTACKPROTECT='-fstack-protector'
  fi
fi

# nginx 1.11.5+ has removed --with-ipv6 option and 
# auto detects ipv6 support instead
# http://hg.nginx.org/nginx/rev/a6d116645c51
if [[ "$DETECT_NGXVER" -ge '1011005' ]]; then
  IPVSIXOPT=""
else
  if [[ "$NGINX_IPV" = [yY] ]]; then
    IPVSIXOPT=' --with-ipv6'
  else
    IPVSIXOPT=""
  fi
fi

if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$NGX_GSPLITDWARF" = [yY] ]]; then
  NGXEXTRA_CCOPT="$NGXEXTRA_CCOPT -gsplit-dwarf"
fi
if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$NGX_LDGOLD" = [yY] ]] || [[ "$CLANG" = [yY] && "$NGX_LDGOLD" = [yY] ]]; then
  # clang 4.0 compile doesn't include the lib/LLVMgold.so linker by default, so only GCC and clang 3.4.2 do
  if [[ "$CLANG_FOUR" = [Yy] ]]; then
    NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
  elif [[ "$CLANG_FIVE" = [Yy] ]]; then
    NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
  elif [[ "$CLANG_SIX" = [Yy] ]]; then
    NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
  elif [[ "$CLANG_MASTER" = [Yy] ]]; then
    NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
  else
    if [[ "$CLANG" = [yY] ]]; then
      if [[ "$CLANG_SIX" = [Yy] ]]; then
        NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
      else
        if [[ "$NGINX_GEOIPTWOLITE" = [yY] ]]; then
          # GeoIP2 Lite fails to compile with -fuse-ld=gold
          NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT"
        else
          NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT -fuse-ld=gold"
        fi
      fi
    else
      if [[ "$NGINX_GEOIPTWOLITE" = [yY] ]]; then
        # GeoIP2 Lite fails to compile with -fuse-ld=gold
        NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT -flto"
      else
        if [[ "$NGINXOPENSSL_FATLTO_OBJECTS" = [yY] ]]; then
          NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT -flto -ffat-lto-objects -fuse-ld=gold"
        else
          NGXEXTRA_LDGOLDCCOPT="$NGXEXTRA_LDGOLDCCOPT -flto -fuse-ld=gold"
        fi
      fi
      if [[ "$NGINXOPENSSL_FATLTO_OBJECTS" = [yY] && "$DETECT_NGXVER" -ge '1015005' && "$(echo $OPENSSL_VERSION | grep -o '1.1.1')" = '1.1.1' ]]; then
        NGINX_OPENSSLOPT_EXTRAS=' -flto -ffat-lto-objects'
      else
        NGINX_OPENSSLOPT_EXTRAS=""
      fi
    fi
  fi
fi

if [[ "$NGINXCOMPILE_FORMATSEC" = [yY] ]]; then
  FORMATSECURITY_OPT=' -Wformat -Werror=format-security'
else
  FORMATSECURITY_OPT=""
fi

if [[ "$NGINXCOMPILE_PIE" = [yY] ]]; then
  PIE_OPT='-fPIC -pie '
else
  PIE_OPT=""
fi

#    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
#    if [[ "$asknginxipv" = [yY] ]]; then
   
    # force NGXMODULE_ALTORDER='y'
    NGXMODULE_ALTORDER='y'
    SETBUILD_DATE=$(date +"%d%m%y-%H%M%S")
    if [[ "$NGXMODULE_ALTORDER" = [yY] ]]; then
      if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then
          pwd
          if [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
            echo
            echo "dynamic module directory at /usr/local/nginx/modules"
            ls -lah /usr/local/nginx/modules
            echo
          fi
          echo "nginx configure options (alternate ordering):"
        if [[ "$TLSTHREEOPT" = [yY] && "$OPENSSL_TLSONETHREE" = [yY] ]]; then
           echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPTECHO}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT} --with-openssl-opt=\"enable-ec_nistp_64_gcc_128 enable-tls1_3${NGINX_OPENSSLOPT_EXTRAS}\"${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
           ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPT}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT} --with-openssl-opt="enable-ec_nistp_64_gcc_128 enable-tls1_3${NGINX_OPENSSLOPT_EXTRAS}"${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
        elif [[ "$TLSTHREEOPT" = [yY] && "$OPENSSL_TLSONETHREE" = [nN] ]]; then
           echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPTECHO}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT} --with-openssl-opt=\"enable-ec_nistp_64_gcc_128 no-tls1_3${NGINX_OPENSSLOPT_EXTRAS}\"${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
           ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPT}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT} --with-openssl-opt="enable-ec_nistp_64_gcc_128 no-tls1_3${NGINX_OPENSSLOPT_EXTRAS}"${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
        elif [[ "$BORINGSSL_SWITCH" = [yY] ]]; then
           echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${BORINGSSL_LIBOPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${BORINGSSLINC_OPT}${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPTECHO}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
           ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${BORINGSSL_LIBOPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${BORINGSSLINC_OPT}${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPT}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
        else
           echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPTECHO}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
           ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPT}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
        fi # TLSTHREEOPT=y
      else
        pwd
        echo "nginx configure options (alternate ordering):"
        echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPTECHO}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${DYNAMICOPT_BEGIN}${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${WITHOPENSSL_OPT}${VTSOPT}${LIBATOMICOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${NGXSLICEOPT}${REALIPOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGXPERLOPT}${NGXXSLTOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      fi
    else
      if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then
          pwd
          echo "nginx configure options:"
          echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN} --with-http_ssl_module${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${REALIPOPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${WITHOPENSSL_OPTECHO}${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT}${SPDYOPT}${NGXPERLOPT}${NGXXSLTOPT}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${IPVSIXOPT}${DYNAMICOPT_BEGIN} --with-http_ssl_module${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${REALIPOPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${WITHOPENSSL_OPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT}${SPDYOPT}${NGXPERLOPT}${NGXXSLTOPT}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      else
        pwd
        echo "nginx configure options:"
        echo "./configure --with-ld-opt=\"${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}\" --with-cc-opt=\"${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}\" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${DYNAMICOPT_BEGIN} --with-http_ssl_module${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${REALIPOPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${WITHOPENSSL_OPTECHO}${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT}${SPDYOPT}${NGXPERLOPT}${NGXXSLTOPT}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT}" | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      ./configure --with-ld-opt="${NGINX_EXPORTLD_OPT}${ZLIBCF_OPT}${LIBBROTLIENC_OPT}${LRT}${JEMALLOC_LD}${PCRE_LD} -Wl,-z,relro ${LUALD_OPT}${PCRE_LDOPT}" --with-cc-opt="${ZLIBCFINC_OPT}${LIBBROTLIINC_OPT}${MTUNEOPT}${PIE_OPT}-g$GCC_OPTLEVEL $FSTACKPROTECT${NGXEXTRA_LDGOLDCCOPT} --param=ssp-buffer-size=4${FORMATSECURITY_OPT}${DEVTOOLSETSEVEN_FALLTHROUGH}${DEVTOOLSETSEVEN_EXTRAFLAGS}${DEVTOOLSETEIGHT_FALLTHROUGH}${DEVTOOLSETEIGHT_EXTRAFLAGS} -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}${BROTLI_CFLAG}${PCRE_CCOPT}${NGXEXTRA_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --build=${SETBUILD_DATE}${WITHCOMPAT_OPT}${NGINX_DEBUGOPT}${DYNAMICOPT_BEGIN} --with-http_ssl_module${HTTPTWOOPT}${NGX_HPACK_OPT}${HTTP_PUSHOPT}${DYNAMICOPT_MIDDLE} --with-http_gzip_static_module${BROTLIOPT}${TESTCOOKIEOPT}${DYNAMICOPT_AFTERGZIP}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT}${REALIPOPT}${GEOIPOPT}${NGINX_NJSOPT}${STREAMGEOIPOPT}${STREAMREALIPOPT}${STREAM_SSLPREREADOPT}${THREADS}${STREAM}${WITHOPENSSL_OPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${OPENSSLOPT_B}${LIBRESSLOPT}${BORINGSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${ZLIBNG_OPT}${ZLIBCUSTOM_OPT}${HIDELENGTHOPT}${SPDYOPT}${NGXPERLOPT}${NGXXSLTOPT}${GEOIPTWOLITEOPT}${PGSPEEDOPT}${MODSEC_OPT}${RDNSOPT}${DYNAMICOPT_END}${LUAJIT_XCFLAGOPT} | tee -a "${CENTMINLOGDIR}/nginx-configure-${DT}.log"
      fi
  fi # NGXMODULE_ALTORDER
  sar_call

}