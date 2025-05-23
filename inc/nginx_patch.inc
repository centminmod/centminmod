openresty_apply_patch_if_needed() {
  local patch_file="$1"
  local patch_name="$2"

  if [[ "$DETECT_NGXVER" -ge '1027000' && "$DETECT_NGXVER" -lt '1028000' && "$NGINX_OPENRESTY" =~ [yY] && "$ORESTY_LUANGINX" =~ [yY] ]]; then
    # Perform a dry-run to check if the patch can be applied
    if patch -p1 --dry-run < "$CUR_DIR/patches/luanginx/nginx-1.27.0/$patch_file" > /dev/null 2>&1; then
      # If the dry-run is successful, apply the patch
      if patch -p1 < "$CUR_DIR/patches/luanginx/nginx-1.27.0/$patch_file"; then
        echo "$patch_name patch applied successfully with $patch_file."
        echo
      else
        echo "Failed to apply $patch_name patch with $patch_file."
        echo
      fi
    else
      echo "$patch_name $patch_file patch has already been applied or cannot be applied."
      echo
    fi
  fi
}

no_Werror_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-no_Werror.patch" "No Werror"
}

always_enable_cc_feature_tests_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-always_enable_cc_feature_tests.patch" "Always Enable CC Feature Tests"
}

intercept_error_log_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-intercept_error_log.patch" "Intercept Error Log"
}

log_escape_non_ascii_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-log_escape_non_ascii.patch" "Log Escape Non ASCII"
}

larger_max_error_str_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-larger_max_error_str.patch" "Larger Max Error Str"
}

# fails
no_pool_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-no_pool.patch" "No Pool"
}

daemon_destroy_pool_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-daemon_destroy_pool.patch" "Daemon Destroy Pool"
}

init_cycle_pool_release_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-init_cycle_pool_release.patch" "Init Cycle Pool Release"
}

cache_manager_exit_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-cache_manager_exit.patch" "Cache Manager Exit"
}

privileged_agent_process_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-privileged_agent_process.patch" "Privileged Agent Process"
}

privileged_agent_process_connections_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-privileged_agent_process_connections.patch" "Privileged Agent Process Connections"
}

privileged_agent_process_thread_pool_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-privileged_agent_process_thread_pool.patch" "Privileged Agent Process Thread Pool"
}

socket_cloexec_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-socket_cloexec.patch" "Socket CLOEXEC"
}

reuseport_close_unused_fds_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-reuseport_close_unused_fds.patch" "Reuseport Close Unused FDs"
}


# fails openssl 1.1.1 support only for OpenResty
# openssl111f_cert_cb_yield_patch() {
#   openresty_apply_patch_if_needed "openssl-1.1.1f-sess_set_get_cb_yield.patch" "OpenSSL 1.1.1f Cert CB Yield"
# }

#openssl111f_cert_cb_yield_patch
# fails openssl 1.1.1 support only for OpenResty
ssl_cert_cb_yield_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-ssl_cert_cb_yield.patch" "SSL Cert CB Yield"
}

# fails openssl 1.1.1 support only for OpenResty
ssl_client_hello_cb_yield_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-ssl_client_hello_cb_yield.patch" "SSL Client Hello CB Yield"
}

ssl_sess_cb_yield_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-ssl_sess_cb_yield.patch" "SSL Session CB Yield"
}

safe_resolver_ipv6_option_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-safe_resolver_ipv6_option.patch" "Safe Resolver IPv6 Option"
}

resolver_conf_parsing_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-resolver_conf_parsing.patch" "Resolver Conf Parsing"
}

hash_overflow_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-hash_overflow.patch" "Hash Overflow"
}

upstream_timeout_fields_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-upstream_timeout_fields.patch" "Upstream Timeout Fields"
}

balancer_pool_max_retry_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-balancer_pool_max_retry.patch" "Balancer Pool Max Retry"
}

balancer_status_code_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-balancer_status_code.patch" "Balancer Status Code"
}

upstream_pipelining_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-upstream_pipelining.patch" "Upstream Pipelining"
}

proxy_host_port_vars_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-proxy_host_port_vars.patch" "Proxy Host Port Vars"
}

stream_balancer_export_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-stream_balancer_export.patch" "Stream Balancer Export"
}

stream_proxy_get_next_upstream_tries_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-stream_proxy_get_next_upstream_tries.patch" "Stream Proxy Get Next Upstream Tries"
}

stream_proxy_timeout_fields_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-stream_proxy_timeout_fields.patch" "Stream Proxy Timeout Fields"
}

stream_ssl_preread_no_skip_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-stream_ssl_preread_no_skip.patch" "Stream SSL Preread No Skip"
}

delayed_posted_events_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-delayed_posted_events.patch" "Delayed Posted Events"
}

setting_args_invalidates_uri_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-setting_args_invalidates_uri.patch" "Setting Args Invalidates URI"
}

single_process_graceful_exit_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-single_process_graceful_exit.patch" "Single Process Graceful Exit"
}

builtin_error_page_footer_patch() {
  if [[ "$NGINX_ERROR_PAGE_FOOTER" = [yY] ]]; then
    openresty_apply_patch_if_needed "nginx-1.27.0-builtin_error_page_footer.patch" "Builtin Error Page Footer"
  fi
}

no_error_pages_patch() {
  openresty_apply_patch_if_needed "nginx-1.27.0-no_error_pages.patch" "No Error Pages"
}

ngx_gzip_multi_status_patch() {
  if [[ "$NGINX_GZIP_MULTI_STATUS" = [yY] && "$DETECT_NGXVER" -ge '1025003' ]]; then
    cecho "nginx-gzip-207-status.patch" $boldyellow
    if [ ! "$(grep 'NGX_HTTP_MULTI_STATUS' src/http/ngx_http_request.h)" ]; then
      cecho "patch -p1 < $CUR_DIR/patches/nginx/nginx-gzip-207-status.patch" $boldyellow
      patch -p1 < $CUR_DIR/patches/nginx/nginx-gzip-207-status.patch
      echo 
      echo "patching nginx to allow gzip to compress HTTP Multi-Status 207 for nginx 1.25.0+"
      echo "https://trac.nginx.org/nginx/ticket/394"
    else
      cecho "skip gzip HTTP Multi-Status 207 patch" $boldyellow
      echo "already detected as patched"
    fi
  fi
}

nginx_quic_patches() {
  if [[ "$ngver" = 'quic' ]]; then
    {
    echo "patching nginx-quic"
    if [ "$ngver" ]; then
      pushd "$DIR_TMP/nginx-${ngver}"
    else
      pushd "$DIR_TMP/nginx-${NGINX_VERSION}"
    fi
    if [[ -f "$CUR_DIR/patches/nginx/nginx-quic-patch1.patch" && "$(grep -c 'lsopt.type = SOCK_STREAM;' src/http/ngx_http_core_module.c)" -lt '2' ]]; then
      patch -p1 < "$CUR_DIR/patches/nginx/nginx-quic-patch1.patch"
    fi
    popd
    } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
  fi
}

ocsp_ttl_override() {
  if [[ "$NGINX_STAPLE_CACHE_OVERRIDE" = [yY] ]]; then
    if [ "$ngver" ]; then
      NGINX_VER_OCSP="$ngver"
      echo "$NGINX_VER_OCSP"
    else
      NGINX_VER_OCSP="$NGINX_VERSION"
      echo "$NGINX_VER_OCSP"
    fi
    echo "patching nginx OCSP stapling response cache time set to: $NGINX_STAPLE_CACHE_TTL"
    grep 'staple->refresh = ngx_max' "/svr-setup/nginx-${NGINX_VER_OCSP}/src/event/ngx_event_openssl_stapling.c"
    sed -i "s|staple->refresh = ngx_max(ngx_min(valid - 300, now + 3600), now + 300)|staple->refresh = ngx_max(ngx_min(valid - 300, now + $NGINX_STAPLE_CACHE_TTL), now + 300)|" "/svr-setup/nginx-${NGINX_VER_OCSP}/src/event/ngx_event_openssl_stapling.c"
    grep 'staple->refresh = ngx_max' "/svr-setup/nginx-${NGINX_VER_OCSP}/src/event/ngx_event_openssl_stapling.c"
  fi
}

iouring_patch() {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')
  # for nginx 1.17.0+ and Linux Kernel 5.1+
  if [[ "$NGINX_IOURING_PATCH_BETA" = [yY] && "$DETECT_NGXVER" -gt '1017000' && "$KERNEL_NUMERICVER" -ge '5001000000' ]]; then
    if [[ -d "${DIR_TMP}/nginx-${NGINX_PUSHBASE}" ]]; then
      pushd "$DIR_TMP"
      rm -rf liburing
      git clone http://git.kernel.dk/cgit/liburing/
      cd liburing
      ./configure
      make -j$(nproc)
      make install
      ldconfig
      if [[ -f /usr/lib/liburing.so && -f /usr/lib/pkgconfig/liburing.pc ]]; then
        liburing_ver=$(PKG_CONFIG_PATH='/usr/lib/pkgconfig' pkg-config --modversion liburing)
        echo "$liburing $liburing_ver installed"
      fi
      popd
      if [ -f /usr/lib/liburing.so ]; then
        pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
        cecho "patching nginx for Linux 5.1 Kernel AIO io_ring https://github.com/hakasenyang/openssl-patch/pull/22" $boldyellow
        cecho "https://lwn.net/Articles/776703/" $boldyellow
        cecho "https://hub.packtpub.com/linux-5-1-out-with-io_uring-io-interface-persistent-memory-new-patching-improvements-and-more-2/" $boldyellow
        cecho "patch -p1 < $CUR_DIR/patches/nginx/nginx_io_uring.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/nginx/nginx_io_uring.patch"
        echo
        sed -i '/^aio on/d' /usr/local/nginx/conf/nginx.conf
        sed -i '/^#aio on/d' /usr/local/nginx/conf/nginx.conf
        sed -i 's|^sendfile off;|sendfile off;\naio on;|' /usr/local/nginx/conf/nginx.conf
        sed -i 's|^sendfile on;|sendfile off;\naio on;|' /usr/local/nginx/conf/nginx.conf
        echo "patched nginx for Linux 5.1+ Kernel AIO io_ring"
        popd
        # raise nginx max memlock limits
        if [[ "$CENTOS_SIX" = '6' ]]; then
          FREEMEM=$(grep -E '^MemFree|^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        else
          FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
        fi
        IOURING_MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
        IOURING_MAXLOCKEDMEM_SIZE=$(($IOURING_MAXLOCKEDMEM_COUNT*1024))
        IOURING_MAXLOCKEDMEM_SIZE_NGINX=$(($IOURING_MAXLOCKEDMEM_SIZE*32))
        if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_SIX" -eq '6' ]] && [[ -f /etc/init.d/nginx ]]; then
          sed -i '/ulimit -l/d' /etc/init.d/nginx
          sed -i "s|#!\/bin\/sh|#!\/bin\/sh\nif [ ! -f \/proc\/user_beancounters ]; then ulimit -l $IOURING_MAXLOCKEDMEM_SIZE_NGINX ; fi\n|g" /etc/init.d/nginx
          if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
            systemctl daemon-reload >/dev/null 2>&1
          fi
          head -n8 /etc/init.d/nginx
        fi
      fi
    fi
  else
    sed -i '/^aio on/d' /usr/local/nginx/conf/nginx.conf
    sed -i '/^#aio on/d' /usr/local/nginx/conf/nginx.conf
    sed -i 's|^sendfile off;|sendfile on;|' /usr/local/nginx/conf/nginx.conf
  fi
}

ngx_maxprotocol_patch() {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
    NGX_HPACK_OPT=""
    NGINX_HPACK='n'
  elif [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1015008' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1017010' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.17.10"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1017011' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1019010' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.19.10"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1017011' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1019010' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.19.10"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1020000' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le '1020001' ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
    cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
    patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
    echo 
    echo "patching nginx http/2 full HPACK encoding for nginx 1.20.0"
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  elif [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -ge '1021000' ]] && [[ "$NGINX_HPACK" = [yY] && "$DETECT_NGXVER" -le "$NGINX_HPACK_ALLOWED_VER" ]]; then
    pushd "${DIR_TMP}/nginx-${NGINX_PUSHBASE}"
    cecho "patching nginx http/2 full HPACK encoding support" $boldyellow
    if [[ "$DETECT_NGXVER" -ge '1021000' && "$DETECT_NGXVER" -lt '1025000' ]]; then
      cecho "nginx-1.15.3_http2-hpack.patch" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch" $boldyellow
      patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.15.3_http2-hpack.patch
      echo 
      echo "patching nginx http/2 full HPACK encoding for nginx 1.21.5+"
    elif [[ "$DETECT_NGXVER" -ge '1025000' ]]; then
      NGINX_DYNAMICTLS='y'
      cecho "nginx-1.25.0_http2-hpack.patch" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.25.0_http2-hpack.patch" $boldyellow
      patch -p1 < $CUR_DIR/patches/cloudflare/nginx-1.25.0_http2-hpack.patch
      echo 
      echo "patching nginx http/2 full HPACK encoding for nginx 1.25.0+"
    fi
    popd
    NGX_HPACK_OPT=' --with-http_v2_hpack_enc'
  else
    NGX_HPACK_OPT=""
    NGINX_HPACK='n'
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_httppush_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    NGINX_PUSHBASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_HTTPPUSHx" = [yY] && "$DETECT_NGXVER" -ge '1011012' ]]; then
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

ngx_fingerprint_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    NGINX_FINGERPRINT_BASE=$ngver
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    NGINX_FINGERPRINT_BASE=$NGINX_VERSION
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_TLS_FINGERPRINT" = [yY] && "$DETECTOPENSSL_ONEONE" && "$DETECTOPENSSL_ONEONE" = '1.1.1' ]] && [[ "$DETECT_NGXVER" -ge '1021000' ]]; then
      NGX_FINGERPRINT_DIR='nginx-ssl-fingerprint'
      if [[ -d "${DIR_TMP}/${NGX_FINGERPRINT_DIR}" || ! -d "${DIR_TMP}/${NGX_FINGERPRINT_DIR}" ]]; then
        rm -rf "${DIR_TMP}/${NGX_FINGERPRINT_DIR}"
        pushd "${DIR_TMP}"
        git clone https://github.com/centminmod/nginx-ssl-fingerprint
        popd
      fi
      if [ ! -f nginx_fingerprint.patch ]; then
          wget -4 https://github.com/centminmod/nginx-ssl-fingerprint/raw/master/patches/nginx.patch -O nginx_fingerprint.patch
      fi
      if [ -f nginx_fingerprint.patch ]; then
        cecho "patching nginx for nginx-ssl-fingerprint module" $boldyellow
        cecho "patch -p1 < nginx_fingerprint.patch" $boldyellow
        patch -p1 < nginx_fingerprint.patch
        echo 
        echo "patching nginx for nginx-ssl-fingerprint module 1.21.x"
      fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_headers_more_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  fi
  if [[ "$DETECT_NGXVER" -ge '1023000' && "$NGINX_HEADERSMORE" = '0.33' ]]; then
    NGX_HEADERSMOREDIR=$(tar -tzf "$DIR_TMP/${NGX_HEADERSMORELINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_HEADERSMOREDIR}" ]; then
      rm -rf "${DIR_TMP}/${NGX_HEADERSMOREDIR}"
      pushd "${DIR_TMP}"
      if [ -f "$DIR_TMP/${NGX_HEADERSMORELINKFILE}" ]; then
        tar xzf "$DIR_TMP/${NGX_HEADERSMORELINKFILE}"
      fi
      popd
      pushd "${DIR_TMP}/${NGX_HEADERSMOREDIR}"
      cecho "patching headers-more-nginx-module" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/headers-more/headers-more-nginx-1.23.0.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/headers-more/headers-more-nginx-1.23.0.patch"
      echo 
      echo "patching headers-more-nginx-module for nginx 1.23.0+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_srcache_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  fi
  if [[ "$DETECT_NGXVER" -ge '1023000' && "$ORESTY_SRCCACHEVER" = '0.32' ]]; then
    NGX_SRCACHEDIR=$(tar -tzf "$DIR_TMP/${NGX_SRCACHELINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_SRCACHEDIR}" ]; then
      rm -rf "${DIR_TMP}/${NGX_SRCACHEDIR}"
      pushd "${DIR_TMP}"
      if [ -f "$DIR_TMP/${NGX_SRCACHELINKFILE}" ]; then
        tar xzf "$DIR_TMP/${NGX_SRCACHELINKFILE}"
      fi
      popd
      pushd "${DIR_TMP}/${NGX_SRCACHEDIR}"
      cecho "patching srcache-nginx-module" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/srcache-nginx/srcache-nginx-1.23.0.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/srcache-nginx/srcache-nginx-1.23.0.patch"
      echo 
      echo "patching srcache-nginx-module for nginx 1.23.0+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_cachepurge_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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

ngx_luaonetwothree_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  fi
  if [[ "$ORESTY_LUANGINX" = [yY] && "$DETECT_NGXVER" -ge '1023000' && "$ORESTY_LUANGINXVER" = '0.10.21' ]]; then
    NGX_LUANGINXDIR=$(tar -tzf "$DIR_TMP/${NGX_LUANGINXLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_LUANGINXDIR}" ]; then
      rm -rf "${DIR_TMP}/${NGX_LUANGINXDIR}"
      pushd "${DIR_TMP}"
      if [ -f "$DIR_TMP/${NGX_LUANGINXLINKFILE}" ]; then
        tar xzf "$DIR_TMP/${NGX_LUANGINXLINKFILE}"
      fi
      popd
      pushd "${DIR_TMP}/${NGX_LUANGINXDIR}"
      cecho "patching lua nginx module" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/luanginx/luanginx-1.23.0.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/luanginx/luanginx-1.23.0.patch"
      echo 
      echo "patching lua nginx module for nginx 1.23.0+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

ngx_luastream_onetwoseven_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  fi
  if [[ "$ORESTY_LUANGINX" = [yY] && "$NGX_LUASTREAM" = [yY] && "$DETECT_NGXVER" -ge '1025004' ]]; then
    NGX_LUANGINXDIR=$(tar -tzf "$DIR_TMP/${NGX_LUANGINXLINKFILE}" | head -1 | cut -f1 -d"/")
    NGX_LUASTREAMDIR=$(tar -tzf "$DIR_TMP/${NGX_LUASTREAMLINKFILE}" | head -1 | cut -f1 -d"/")
    if [ -d "${DIR_TMP}/${NGX_LUASTREAMDIR}" ]; then
      pushd "${DIR_TMP}/${NGX_LUASTREAMDIR}"
      cecho "patching lua stream nginx module for nginx 1.25.4+ support" $boldyellow
      cecho "Performing dry-run of the patch..." $boldyellow
      if patch -p1 --dry-run < "$CUR_DIR/patches/luanginx/luanginx-1.27.0.patch" > /dev/null 2>&1; then
        cecho "Applying the patch..." $boldyellow
        if patch -p1 < "$CUR_DIR/patches/luanginx/luanginx-1.27.0.patch"; then
          echo "Patch applied successfully."
        else
          echo "Failed to apply the patch."
        fi
      else
        echo "Patch has already been applied or cannot be applied."
      fi
      echo
      echo "patching lua stream nginx module for nginx 1.27.0+"
      popd
    fi
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

luanginx_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
  ngx_luaonetwothree_patch
  ngx_luastream_onetwoseven_patch
  # openresty nginx 1.27.0+ patches
  no_Werror_patch
  #always_enable_cc_feature_tests_patch
  intercept_error_log_patch
  log_escape_non_ascii_patch
  larger_max_error_str_patch
  #no_pool_patch
  daemon_destroy_pool_patch
  init_cycle_pool_release_patch
  cache_manager_exit_patch
  privileged_agent_process_patch
  privileged_agent_process_connections_patch
  privileged_agent_process_thread_pool_patch
  socket_cloexec_patch
  reuseport_close_unused_fds_patch
  #openssl111f_cert_cb_yield_patch
  #ssl_cert_cb_yield_patch
  #ssl_client_hello_cb_yield_patch
  #ssl_sess_cb_yield_patch
  safe_resolver_ipv6_option_patch
  resolver_conf_parsing_patch
  hash_overflow_patch
  upstream_timeout_fields_patch
  balancer_pool_max_retry_patch
  balancer_status_code_patch
  upstream_pipelining_patch
  proxy_host_port_vars_patch
  stream_balancer_export_patch
  stream_proxy_get_next_upstream_tries_patch
  stream_proxy_timeout_fields_patch
  stream_ssl_preread_no_skip_patch
  delayed_posted_events_patch
  setting_args_invalidates_uri_patch
  single_process_graceful_exit_patch
  builtin_error_page_footer_patch
  no_error_pages_patch
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

echonginx_patch() {
  {
  if [ "$ngver" ]; then
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
    echo "$DETECT_NGXVER"
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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

aws_lc_patch() {
  if [[ "$AWS_LC_SWITCH" = [yY] ]]; then
    echo
    if [[ "$DETECT_NGXVER" -ge '1027004' && -f "$CUR_DIR/patches/nginx/aws-lc-nginx-1.27.4.patch" && ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.c) ]]; then
      echo "nginx 1.27.4+ aws-lc-nginx-1.27.4.patch"
      if [ ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.h) ]; then
        echo
        echo "AWS-LC patch: patch -p1 < $CUR_DIR/patches/nginx/aws-lc-nginx-1.27.4.patch"
        patch -p1 < $CUR_DIR/patches/nginx/aws-lc-nginx-1.27.4.patch
      fi
    elif [[ "$DETECT_NGXVER" -eq '1027003' && -f "$CUR_DIR/patches/nginx/aws-lc-nginx-1.27.3.patch" && ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.c) ]]; then
      echo "nginx 1.27.3 aws-lc-nginx-1.27.3.patch"
      if [ ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.h) ]; then
        echo
        echo "AWS-LC patch: patch -p1 < $CUR_DIR/patches/nginx/aws-lc-nginx-1.27.3.patch"
        patch -p1 < $CUR_DIR/patches/nginx/aws-lc-nginx-1.27.3.patch
      fi
    elif [ ! -f aws-lc-nginx.patch ] || [[ -f aws-lc-nginx.patch && ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.h) ]]; then
      echo "download aws-lc-nginx.patch"
      curl -sL https://github.com/aws/aws-lc/raw/main/tests/ci/integration/nginx_patch/aws-lc-nginx.patch -o aws-lc-nginx.patch
      if [ ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.h) ]; then
        echo
        echo "AWS-LC patch: patch -p1 < aws-lc-nginx.patch"
        patch -p1 < aws-lc-nginx.patch
      fi
    fi
    if [[ -f "$CUR_DIR/patches/nginx/aws-lc-nginx2.patch" && ! $(grep 'OPENSSL_IS_AWSLC' src/event/ngx_event_openssl.c) ]]; then
      # add support for the $ssl_curve variable when using the AWS-LC
      # It retrieves the negotiated elliptic curve ID (curve_id) from 
      # the SSL connection object (SSL_get_curve_id(c->ssl->connection)
      # patch includes a switch statement that maps common curve IDs 
      # to their corresponding standardized names 
      # (e.g., "prime256v1" for SSL_CURVE_SECP256R1)
      # if a match is found in the table, the $ssl_curve variable 
      # is set to the corresponding curve name
      echo "AWS-LC patch: patch -p1 < $CUR_DIR/patches/nginx/aws-lc-nginx2.patch"
      patch -p1 < "$CUR_DIR/patches/nginx/aws-lc-nginx2.patch"
    fi
    echo
  fi
}

aws_lc_patch_make() {
  if [[ "$AWS_LC_SWITCH" = [yY] && "$AWS_LC_SWITCH_TEST" = [yY] ]]; then
    {
    echo
    if [ ! -f aws-lc-nginx-tests.patch ]; then
      echo "download aws-lc-nginx-tests.patch"
      curl -sL https://github.com/aws/aws-lc/raw/main/tests/ci/integration/nginx_tests_patch/aws-lc-nginx-tests.patch -o aws-lc-nginx-tests.patch
      echo
      if [ ! $(grep 'OPENSSL_IS_AWSLC' h3_ssl_early_data.t) ]; then
        echo "AWS-LC patch: patch -p1 < aws-lc-nginx-tests.patch"
        patch -p1 < aws-lc-nginx-tests.patch
      fi
    fi
    echo
    } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
  fi
}

http2_shutdown_fix_patch() {
  # backport freenginx fork's HTTP/2 shutdown bug fix
  # https://freenginx.org/hg/nginx/rev/4a0cd107c0f1
  if [[ "$NGINX_INSTALL" = [yY] && "$FREENGINX_INSTALL" != [yY] ]] && [[ "$ngver" != 'angie' ]] && [[ "$FREENGINX_BACKPORT_PATCHES" = [yY] && -f "$CUR_DIR/patches/nginx/http2-shutdown-fix.patch" && "$DETECT_NGXVER" -ge '1025000' ]]; then
    # Perform a dry-run to check if the patch can be applied
    if patch -p1 --dry-run < "$CUR_DIR/patches/nginx/http2-shutdown-fix.patch" > /dev/null 2>&1; then
      # If the dry-run is successful, apply the patch
      if patch -p1 < "$CUR_DIR/patches/nginx/http2-shutdown-fix.patch"; then
        echo "Patch applied successfully."
      else
        echo "Failed to apply the patch."
      fi
    else
      echo "Patch has already been applied or cannot be applied."
    fi
  fi
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
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
  else
    if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
  ngx_gzip_multi_status_patch

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
     curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${NGINXTLSPATCHLINK}" | grep 'HTTP/' | grep -E '200' >/dev/null 2>&1
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
       if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-$ngver/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    fi
     else
       if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/freenginx-${NGINX_VERSION}/src/core/nginx.h")
    else
      DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    fi
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
     elif [[ "$DETECT_NGXVER" -ge '1015008' && "$DETECT_NGXVER" -le '1015012' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1017001' && "$DETECT_NGXVER" -le '1017010' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1017011' && "$DETECT_NGXVER" -le '1019010' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1020000' && "$DETECT_NGXVER" -le '1020001' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
      ngx_dynamic_tls_message
     elif [[ "$DETECT_NGXVER" -ge '1021000' && "$DETECT_NGXVER" -le "$NGINX_DYNAMICTLS_ALLOWED_VER" ]]; then
      if [[ "$DETECT_NGXVER" -ge '1021000' && "$DETECT_NGXVER" -lt "1025000" ]]; then
        NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1015008.patch'
        ngx_dynamic_tls_message
      fi
     fi
     rm -rf "${NGINXTLSPATCH_NAME}"
     if [[ "$DETECT_NGXVER" -le '1011004' && "$DETECT_NGXVER" -ge '1011000' ]]; then
      NGINXTLSPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/${NGINXTLSPATCH_NAME}"
      # fallback mirror if github down, use gitlab mirror
      curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "${NGINXTLSPATCHLINK}" | grep 'HTTP/' | grep -E '200' >/dev/null 2>&1
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
     elif [[ "$DETECT_NGXVER" -ge '1015008' && "$DETECT_NGXVER" -le '1015012' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1017001' && "$DETECT_NGXVER" -le '1017010' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1017011' && "$DETECT_NGXVER" -le '1019010' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1020000' && "$DETECT_NGXVER" -le '1020001' ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
        patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1021000' && "$DETECT_NGXVER" -le "$NGINX_DYNAMICTLS_ALLOWED_VER" ]]; then
      if [[ -f "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
        if [[ "$DETECT_NGXVER" -ge '1021000' && "$DETECT_NGXVER" -lt "1025000" ]]; then
          echo "patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME""
          patch -p1 < "$CUR_DIR/patches/cloudflare/$NGINXTLSPATCH_NAME"
        fi
      fi
     fi
  else
     if [ -f "${NGINXTLSPATCH_NAME}" ]; then
      rm -rf "${NGINXTLSPATCH_NAME}"
     fi
  fi
  ngx_hpack_patch
  if [[ "$DETECT_NGXVER" -eq '1017002' ]]; then
    # fix gzip zero size buf alerts
    # https://github.com/nginx/nginx/commit/39c40428f93db246a9a27e7a109413fae46e195d
    if [[ -f "$CUR_DIR/patches/nginx/nginx-1.17.2-fix-zero-size-buf.patch" ]]; then
      echo "patch -p1 < "$CUR_DIR/patches/nginx/nginx-1.17.2-fix-zero-size-buf.patch""
      patch -p1 < "$CUR_DIR/patches/nginx/nginx-1.17.2-fix-zero-size-buf.patch"
    fi
  fi
  iouring_patch
  ocsp_ttl_override
  ngx_srcache_patch
  ngx_headers_more_patch
  # nginx_quic_patches
  aws_lc_patch
  http2_shutdown_fix_patch
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
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 $OPENRESTYLUAPATCHLINK | grep 'HTTP/' | grep -E '200|302' >/dev/null 2>&1
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