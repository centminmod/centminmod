ngx_maxprotocol_patch() {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$DETECT_NGXVER" -eq '1015005' ]] && [[ "$(echo $OPENSSL_VERSION | grep -o '1.1.1')" = '1.1.1' ]]; then
    if [[ -d "${DIR_TMP}/nginx-${NGINX_PUSHBASE}" ]]; then
      pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
      cecho "patching nginx for max tls protocol patch https://trac.nginx.org/nginx/ticket/1654" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/nginx/nginx-1.15.5-fix-max-protocol-version.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/nginx/nginx-1.15.5-fix-max-protocol-version.patch"
      echo 
      echo "patching nginx for max tls protocol patch with OpenSSL 1.1.1"
      popd
    fi
  fi
}

ngx_renegotiate_patch() {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$DETECT_NGXVER" -eq '1015004' && "$(echo $OPENSSL_VERSION | grep -o '1.1.1')" = '1.1.1' ]]; then
    if [[ -d "${DIR_TMP}/nginx-${NGINX_PUSHBASE}" ]]; then
      pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
      cecho "patching nginx for renegotiation bug fix with OpenSSL 1.1.1 https://github.com/hakasenyang/openssl-patch/blob/master/nginx_openssl-1.1.x_renegotiation_bugfix.patch" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/nginx/nginx_openssl-1.1.x_renegotiation_fix.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/nginx/nginx_openssl-1.1.x_renegotiation_fix.patch"
      echo 
      echo "patching nginx for renegotiation bug fix with OpenSSL 1.1.1"
      popd
    fi
  fi
}

ngx_prioritizechacha_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_PRIORITIZECHACHA" = [yY] && "$DETECT_NGXVER" -ge '1015004' && "$(echo $OPENSSL_VERSION | grep -o '1.1.1')" = '1.1.1' ]]; then
    if [[ -d "${DIR_TMP}/nginx-${NGINX_PUSHBASE}" ]]; then
      pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
      cecho "patching nginx for prioritizing chacha20 with OpenSSL 1.1.1 https://github.com/kn007/patch/issues/2" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/nginx-prioritize-chacha20/nginx-1.15.4-reprioritize-chacha-openssl-1.1.1.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/nginx-prioritize-chacha20/nginx-1.15.4-reprioritize-chacha-openssl-1.1.1.patch"
      echo 
      echo "patching nginx for prioritizing chacha20 with OpenSSL 1.1.1"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_hpack_patch() {
  # if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
  #   NGINX_HPACK='n'
  # fi
  {
  if [[ "$ngver" && "$ngver" = 'master-google' ]]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
    NGX_HPACK_OPT=""
    NGINX_HPACK='n'
  elif [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
  DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
  if [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013006' && "$DETECT_NGXVER" -lt '1013009' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.13.6_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.13.6_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.13.6_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.13.6_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.13.6_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.13.6+"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013009' && "$DETECT_NGXVER" -lt '1013010' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.13.9_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.13.9_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.13.9_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.13.9_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.13.9_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.13.9+"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1013010' && "$DETECT_NGXVER" -le '1014000' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.14.0_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.14.0_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.14.0_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.14.0_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.14.0_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.13.10+ and <= 1.14.0"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
    # NGX_HPACK_OPT=""
    # NGINX_HPACK='n'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1015000' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1015002' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.0_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.15.0_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.15.0_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.0_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.0_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.15.0+"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
    # NGX_HPACK_OPT=""
    # NGINX_HPACK='n'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -eq '1015003' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.15.3_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.15.3_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.15.3"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1015004' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1015007' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    # rm -rf nginx-1.15.3_http2-hpack.patch
    # wget -${ipv_forceopt}cnv https://github.com/cloudflare/sslconfig/raw/hpack_1.13.1/patches/nginx-1.15.3_http2-hpack.patch
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.15.4-1.15.7"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  # elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1015007' ]]; then
  #   pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
  #   cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
  #   cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
  #   cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
  #   patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
  #   echo 
  #   echo "patching nginx http/2 full HPACK encoding for nginx 1.15.8"
  #   popd
  #   NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  else
    NGX_HPACK_OPT=""
    NGINX_HPACK='n'
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_httppush_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_HTTPPUSH" = [yY] && "$DETECT_NGXVER" -ge '1011012' ]]; then
    NGX_HTTPPUSHDIR='http2-push-nginx-module'
    if [[ -d "${DIR_TMP}/${NGX_HTTPPUSHDIR}" || ! -d "${DIR_TMP}/${NGX_HTTPPUSHDIR}" ]]; then
      rm -rf "${DIR_TMP}/${NGX_HTTPPUSHDIR}"
      pushd "${DIR_TMP}"
      git clone https://github.com/ghedo/http2-push-nginx-module
      popd
      pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
      cecho "patching nginx http2 push for community.centminmod.com/threads/11910/" $boldyellow
      cecho "patch -p01 < $CUR_DIR/patches/nginx-http2push/nginx_1.11.12_http2_server_push.patch" $boldyellow
      patch -p01 < "$CUR_DIR/patches/nginx-http2push/nginx_1.11.12_http2_server_push.patch"
      echo 
      echo "patching nginx http2 push for nginx 1.11.12+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_cachepurge_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_CACHEPURGE" = [yY] && "$DETECT_NGXVER" -ge '1011006' && "$NGINX_CACHEPURGEVER" = 2.[3] ]]; then
    NGX_CACHEPURGEDIR=$(tar -tzf "$DIR_TMP/${NGX_CACHEPURGEFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_CACHEPURGEDIR}" ]; then
      rm -rf "${DIR_TMP}/${NGX_CACHEPURGEDIR}"
      pushd "${DIR_TMP}"
      if [ -f "$DIR_TMP/${NGX_CACHEPURGEFILE}" ]; then
        tar xzf "$DIR_TMP/${NGX_CACHEPURGEFILE}"
      fi
      popd
      pushd "${DIR_TMP}/${NGX_CACHEPURGEDIR}"
      cecho "patching ngx_cache_purge for community.centminmod.com/threads/11598/" $boldyellow
      cecho "patch -p0 < $CUR_DIR/patches/ngx-cache-purge/ngx-cache-purge-1116.patch" $boldyellow
      patch -p0 < "$CUR_DIR/patches/ngx-cache-purge/ngx-cache-purge-1116.patch"
      echo 
      echo "patching ngx_cache_purge for nginx 1.11.6+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

luanginx_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  if [[ "$ORESTY_LUANGINX" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_LUANGINXVER" = '0.10.7' ]] || [[ "$ORESTY_LUANGINX" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_LUANGINXVER" = '0.10.8' ]]; then
    NGX_LUANGINXDIR=$(tar -tzf "$DIR_TMP/${NGX_LUANGINXLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_LUANGINXDIR}/src" ]; then
      pushd "${DIR_TMP}/${NGX_LUANGINXDIR}/src"
      rm -rf ngx_http_lua_headers.c
      wget -${ipv_forceopt}cnv https://gist.githubusercontent.com/centminmod/48a61dcbcb6240fa1b52843be8290573/raw/ngx_http_lua_headers.c
      ls -lah
      echo 
      echo "patching lua nginx module for nginx 1.11.11+"
      popd
    fi
  fi
  if [[ "$ORESTY_LUANGINX" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_LUANGINXVER" = '0.10.7' ]]; then
    if [ -d "${DIR_TMP}/${NGX_LUANGINXDIR}/src" ]; then
      pushd "${DIR_TMP}/${NGX_LUANGINXDIR}/src"
      cecho "patching lua nginx 0.10.7 for community.centminmod.com/posts/47450/" $boldyellow
      cecho "patch -p0 < $CUR_DIR/patches/luanginx/luanginx-0107.patch" $boldyellow
      patch -p0 < "$CUR_DIR/patches/luanginx/luanginx-0107.patch"
      popd
    fi
  fi
  if [[ "$ORESTY_LUANGINX" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_LUANGINXVER" = '0.10.8' ]]; then
    if [ -d "${DIR_TMP}/${NGX_LUANGINXDIR}/src" ]; then
      pushd "${DIR_TMP}/${NGX_LUANGINXDIR}/src"
      cecho "patching lua nginx 0.10.8 for community.centminmod.com/posts/47450/" $boldyellow
      cecho "patch -p0 < $CUR_DIR/patches/luanginx/luanginx-0108.patch" $boldyellow
      patch -p0 < "$CUR_DIR/patches/luanginx/luanginx-0108.patch"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

echonginx_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_OPENRESTY" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_ECHOVER" = '0.60' ]]; then
    ECHODIR=$(tar -tzf "$DIR_TMP/${NGX_ECHOLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${ECHODIR}/src" ]; then
      pushd "${DIR_TMP}/${ECHODIR}/src"
      rm -rf ngx_http_echo_request_info.c
      wget -${ipv_forceopt}cnv https://gist.githubusercontent.com/centminmod/c2519f3cfdeff90196a13a3188ef0cfa/raw/ngx_http_echo_request_info.c
      ls -lah
      echo 
      echo "patching echo nginx module for nginx 1.11.11+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngxopenresty_patch() {
  {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  # apply patches for nginx => 1.11.6
  if [[ "$NGINX_OPENRESTY" = [yY] && "$DETECT_NGXVER" -ge '1011006' ]]; then
    ORESTY_REDISVERPARSED=$(echo $ORESTY_REDISVER | sed -e 's|\0.||' -e 's|rc1||' -e 's|rc2||' -e 's|rc3||' -e 's|rc4||')
    ORESTY_MEMCVERPARSED=$(echo $ORESTY_MEMCVER | sed -e 's|\0.||' -e 's|rc1||' -e 's|rc2||' -e 's|rc3||' -e 's|rc4||')
    echo "$CUR_DIR"
    echo "$ORESTY_REDISVERPARSED"
    echo "$ORESTY_MEMCVERPARSED"
    echo "$DIR_TMP/redis2-nginx-module-${ORESTY_REDISVER}/src"
    echo "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src"
    if [[ "$ORESTY_REDISVERPARSED" -le '13' && -f "$CUR_DIR/patches/redis2-nginx/redis2-nginx-1116.patch" && -z $(grep -ro 1011006 "$DIR_TMP/redis2-nginx-module-${ORESTY_REDISVER}/src") ]]; then
      if [ -d "$DIR_TMP/redis2-nginx-module-${ORESTY_REDISVER}/src" ]; then
        pushd "$DIR_TMP/redis2-nginx-module-${ORESTY_REDISVER}/src"
        echo "patch < $CUR_DIR/patches/redis2-nginx/redis2-nginx-1116.patch"
        patch < "$CUR_DIR/patches/redis2-nginx/redis2-nginx-1116.patch"
        popd
      fi
    fi
    if [[ "$ORESTY_MEMCVERPARSED" -le '17' && -f "$CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch" && -z $(grep -ro 1011006 "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src") ]]; then
      if [ -d "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src" ]; then
        pushd "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src"
        echo "patch < $CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch"
        patch < "$CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch"
        popd
      fi
    fi
  fi
  # exit
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_dynamic_tls_message() {
     if [ -f "$(which figlet)" ]; then
         figlet -ckf standard "Cloudflare Nginx Dynamic TLS Patch"
     fi
     echo "######################################################################"
     echo "Patching Nginx for Dynamic TLS Size Support"
     echo "######################################################################"
     echo "Cloudflare Nginx Dynamic TLS patch"
     echo "https://github.com/cloudflare/sslconfig/raw/master/patches/nginx__dynamic_tls_records.patch"
     echo "######################################################################"
}

patchnginx() {
  {
  sleep "$NGINXPATCH_DELAY"
  ngx_renegotiate_patch
  ngx_maxprotocol_patch

  # unofficial OpenSSL 1.1.0 beta Nginx patches
  DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
  # detect nginx <=1.11.3 or => 1.11.0 as 1.11.4+ has fix for openssl 1.1 so no patching needed
  # http://hg.nginx.org/nginx/rev/1891b2892b68
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
  fi
  if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] && [[ "$DETECT_NGXVER" -le '1011003' && "$DETECT_NGXVER" -ge '1011000' ]]; then
    if [[ -f "$CUR_DIR/patches/nginx/openssl-110pre6.patch" && -f "$CUR_DIR/patches/nginx/openssl-110ticket.patch" ]]; then
      if [ -f "$(which figlet)" ]; then
          figlet -ckf standard "Nginx OpenSSL 1.1.0 Patch"
      fi
      echo
      cecho "patching nginx for OpenSSL 1.1.0 support" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/nginx/openssl-110pre6.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/nginx/openssl-110pre6.patch"
      echo "patch -p1 < $CUR_DIR/patches/nginx/openssl-110ticket.patch"
      patch -p1 < "$CUR_DIR/patches/nginx/openssl-110ticket.patch"
      echo
      cecho "nginx patched for OpenSSL 1.1.0" $boldyellow
      echo
    else
      if [[ "$DETECT_NGXVER" -le '1011003' && "$DETECT_NGXVER" -ge '1011000' ]]; then
        if [ ! -f "$CUR_DIR/patches/nginx/openssl-110pre6.patch" ]; then
          cecho "$CUR_DIR/patches/nginx/openssl-110pre6.patch not found" $boldyellow
        fi
        if [ ! -f "$CUR_DIR/patches/nginx/openssl-110ticket.patch" ]; then
          cecho "$CUR_DIR/patches/nginx/openssl-110ticket.patch not found" $boldyellow
        fi
      elif [[ "$DETECT_NGXVER" -lt '1011000' ]]; then
        # OPENSSL_VERSION='1.1.0h'
        cecho "skipping patching due to Nginx version less than 1.11.0" $boldyellow
      fi
    fi
  fi

  if [[ "$NGINX_HTTP2" = [yY] ]]; then
    # assign NGX_VEREVAL nginx version based on if it's an initial
    # centmin mod fresh install or a nginx upgrade run
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      NGX_VEREVAL=${CUR_NGINXUPGRADEVER}
    else
      NGX_VEREVAL=${SET_NGINXVER}
    fi
    # only apply Nginx HTTP/2 patch if Nginx version is >= 1.9.3 and <1.9.5
    if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]]; then
      NGINX_SPDY=n
      # http/2 patch
      echo
      cecho "patching nginx for http/2 support" $boldyellow
      echo
      cecho "wget -4 http://nginx.org/patches/http2/patch.http2.txt" $boldyellow
      wget -4 http://nginx.org/patches/http2/patch.http2.txt
    
      cecho "patch -p1 < patch.http2.txt" $boldyellow
      patch -p1 < patch.http2.txt
    
      echo
      cecho "nginx patched for http/2" $boldyellow
      echo
    fi
  fi
  if [[ "$NGINX_SPDYPATCHED" = [yY] ]]; then
     if [ -f "$(which figlet)" ]; then
         figlet -ckf standard "Cloudflare Nginx HTTP/2 + SPDY Patch"
     fi
     echo  "######################################################################"
     echo "Patching Nginx for HTTP/2 + SPDY Support"
     echo  "######################################################################"
     echo "Cloudflare Nginx HTTP/2 + SPDY patch"
     echo "https://github.com/felixbuenemann/sslconfig/blob/updated-nginx-1.9.15-spdy-patch/patches/nginx_1_9_15_http2_spdy.patch"
     echo  "######################################################################"
     NGINXSPDYPATCHED_NAME='nginx_1_9_15_http2_spdy.patch'
     rm -rf "${NGINXSPDYPATCHED_NAME}"
     NGINXTLSPATCHLINK="https://raw.githubusercontent.com/felixbuenemann/sslconfig/updated-nginx-1.9.15-spdy-patch/patches/${NGINXSPDYPATCHED_NAME}"
     # fallback mirror if github down, use gitlab mirror
     curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${NGINXTLSPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
     NGINXTLSPATCH_CURLCHECK=$?
     if [[ "$NGINXTLSPATCH_CURLCHECK" != '0' ]]; then
       NGINXTLSPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig-felix-group/raw/updated-nginx-1.9.15-spdy-patch/patches/${NGINXSPDYPATCHED_NAME}"
     fi
     wget -${ipv_forceopt}cnv --no-check-certificate "$NGINXTLSPATCHLINK"
     if [[ -f "${NGINXSPDYPATCHED_NAME}" && ! "$(grep 'ngx_http_spdy_filter' auto/modules)" ]]; then
       patch -p1 < "${NGINXSPDYPATCHED_NAME}"
     fi
  else
     if [ -f "${NGINXSPDYPATCHED_NAME}" ]; then
      rm -rf "${NGINXSPDYPATCHED_NAME}"
     fi
  fi
  if [[ "$ngver" && "$ngver" = 'master-google' ]]; then
    NGINX_DYNAMICTLS='n'
  fi
  if [[ "$NGINX_DYNAMICTLS" = [yY] ]]; then
     if [ "$ngver" ]; then
       DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
     else
       DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
     fi
     if [[ "$DETECT_NGXVER" -le '1011004' && "$DETECT_NGXVER" -ge '1011000' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1011005' && "$DETECT_NGXVER" -le '1015003' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1011005.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1015004' && "$DETECT_NGXVER" -le '1015007' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015004.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -eq '1015008' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
      ngx_dynamic_tls_message
     fi
     rm -rf "${NGINXTLSPATCH_NAME}"
     if [[ "$DETECT_NGXVER" -le '1011004' && "$DETECT_NGXVER" -ge '1011000' ]]; then
      NGINXTLSPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/${NGINXTLSPATCH_NAME}"
      # fallback mirror if github down, use gitlab mirror
      curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${NGINXTLSPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
      NGINXTLSPATCH_CURLCHECK=$?
      if [[ "$NGINXTLSPATCH_CURLCHECK" != '0' ]]; then
        NGINXTLSPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${NGINXTLSPATCH_NAME}"
      fi
      wget -${ipv_forceopt}cnv --no-check-certificate "$NGINXTLSPATCHLINK"
      if [[ -f "${NGINXTLSPATCH_NAME}" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
         echo "patch -p1 < "${NGINXTLSPATCH_NAME}""
         patch -p1 < "${NGINXTLSPATCH_NAME}"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1011005' && "$DETECT_NGXVER" -le '1015003' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1015004' && "$DETECT_NGXVER" -le '1015007' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -eq '1015008' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     fi
  else
     if [ -f "${NGINXTLSPATCH_NAME}" ]; then
      rm -rf "${NGINXTLSPATCH_NAME}"
     fi
  fi
  ngx_hpack_patch
} 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

luapatch() {
  {
  if [ ! -f lua.patch ]; then
    {
    # echo
    echo "patching lua for nginx 1.9.11 workaround"
    # patch for nginx 1.9.11 and lua nginx 0.9.20
    # from https://github.com/openresty/lua-nginx-module/pull/669
    # https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945
    wget -O "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/lua.patch" https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945.patch
    if [ -d "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/" ]; then
      pushd "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/"
      patch -p1 < lua.patch
      popd
    fi
  } 2>&1 | tee "${CENTMINLOGDIR}/patch_luapatch_${DT}.log"

  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

luaopensslpatch() {
  # for lua-nginx-module 0.10 https://community.centminmod.com/posts/24549/
  # echo
  { 
  echo "patching nginx for lua 0.10+ branch support"
  if [ "$ngver" ]; then
    pushd "$DIR_TMP/nginx-${ngver}"
  else
    pushd "$DIR_TMP/nginx-${NGINX_VERSION}"
  fi
  OPENRESTYLUAPATCHLINK='https://github.com/openresty/openresty/raw/master/patches/nginx-1.9.7-ssl_cert_cb_yield.patch'
  
  # fallback mirror if github down, use gitlab mirror
  curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $OPENRESTYLUAPATCHLINK | grep 'HTTP\/' | egrep '200|302' >/dev/null 2>&1
  OPENRESTYLUAPATCH_CURLCHECK=$?
  if [[ "$OPENRESTYLUAPATCH_CURLCHECK" != '0' ]]; then
    OPENRESTYLUAPATCHLINK='https://gitlab.com/centminmod-github-mirror/openresty/raw/master/patches/nginx-1.9.7-ssl_cert_cb_yield.patch'
  fi
  wget "$OPENRESTYLUAPATCHLINK"
  patch -p1 < nginx-1.9.7-ssl_cert_cb_yield.patch
  popd
# } 2>&1 | tee "${CENTMINLOGDIR}/patch_luaopensslpatch_${DT}.log"
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"

}

ngxpagespeed_patched() {
  {
  echo "patching ngx_pagespeed for nginx 1.13.4 support"
  NGXPGSPEED_EVALVER=$(echo $NGXPGSPEED_VER | cut -d . -f1-4 | cut -d - -f1 | sed -e 's|\.||g')
  if [[ "$NGXPGSPEED_EVALVER" -le '112342' ]]; then
    if [ -d "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}" ]; then
      pushd "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}"
      if [ -f "$CUR_DIR/patches/nginxpagespeed/nginxpagespeed-1134.patch" ]; then
        patch -p1 < $CUR_DIR/patches/nginxpagespeed/nginxpagespeed-1134.patch
      fi
    elif [ -d "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}" ]; then
      pushd "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}"
      if [ -f "$CUR_DIR/patches/nginxpagespeed/nginxpagespeed-1134.patch" ]; then
        patch -p1 < $CUR_DIR/patches/nginxpagespeed/nginxpagespeed-1134.patch
      fi
    fi
   popd
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/patch_ngxpagespeed_patched_${DT}.log"

}