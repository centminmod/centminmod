echonginx_patch() {
  if [ "$ngver" ]; then
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  else
    DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
    echo "$DETECT_NGXVER"
  fi
  if [[ "$NGINX_OPENRESTY" = [yY] && "$DETECT_NGXVER" -ge '1011011' && "$ORESTY_ECHOVER" = '0.60' ]]; then
    ECHODIR=$(tar -tzf "$DIR_TMP/${NGX_ECHOLINKFILE}" | head -1 | cut -f1 -d"/")
    pushd "${DIR_TMP}/${ECHODIR}/src"
    rm -rf ngx_http_echo_request_info.c
    wget -cnv https://gist.githubusercontent.com/centminmod/c2519f3cfdeff90196a13a3188ef0cfa/raw/ngx_http_echo_request_info.c
    ls -lah
    echo 
    echo "patching echo nginx module for nginx 1.11.11+"
    popd
  fi
}

ngxopenresty_patch() {
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
      pushd "$DIR_TMP/redis2-nginx-module-${ORESTY_REDISVER}/src"
      echo "patch < $CUR_DIR/patches/redis2-nginx/redis2-nginx-1116.patch"
      patch < "$CUR_DIR/patches/redis2-nginx/redis2-nginx-1116.patch"
      popd
    fi
    if [[ "$ORESTY_MEMCVERPARSED" -le '17' && -f "$CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch" && -z $(grep -ro 1011006 "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src") ]]; then
      pushd "$DIR_TMP/memc-nginx-module-${ORESTY_MEMCVER}/src"
      echo "patch < $CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch"
      patch < "$CUR_DIR/patches/memc-nginx/memc-nginx-1116.patch"
      popd
    fi
  fi
  # exit
}

patchnginx() {
  {
  sleep "$NGINXPATCH_DELAY"

  # unofficial OpenSSL 1.1.0 beta Nginx patches
  DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1,2)
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
        # OPENSSL_VERSION='1.0.2k'
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
			cecho "wget http://nginx.org/patches/http2/patch.http2.txt" $boldyellow
			wget http://nginx.org/patches/http2/patch.http2.txt
		
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
     curl -sI --connect-timeout 5 --max-time 5 "${NGINXTLSPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
     NGINXTLSPATCH_CURLCHECK=$?
     if [[ "$NGINXTLSPATCH_CURLCHECK" != '0' ]]; then
       NGINXTLSPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig-felix-group/raw/updated-nginx-1.9.15-spdy-patch/patches/${NGINXSPDYPATCHED_NAME}"
     fi
     wget -cnv --no-check-certificate "$NGINXTLSPATCHLINK"
     if [[ -f "${NGINXSPDYPATCHED_NAME}" && ! "$(grep 'ngx_http_spdy_filter' auto/modules)" ]]; then
       patch -p1 < "${NGINXSPDYPATCHED_NAME}"
     fi
  else
     if [ -f "${NGINXSPDYPATCHED_NAME}" ]; then
      rm -rf "${NGINXSPDYPATCHED_NAME}"
     fi
  fi
	if [[ "$NGINX_DYNAMICTLS" = [yY] ]]; then
     if [ -f "$(which figlet)" ]; then
         figlet -ckf standard "Cloudflare Nginx Dynamic TLS Patch"
     fi
     echo "######################################################################"
     echo "Patching Nginx for Dynamic TLS Size Support"
     echo "######################################################################"
     echo "Cloudflare Nginx Dynamic TLS patch"
     echo "https://github.com/cloudflare/sslconfig/raw/master/patches/nginx__dynamic_tls_records.patch"
     echo "######################################################################"
     if [ "$ngver" ]; then
       DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-$ngver/src/core/nginx.h")
     else
       DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
     fi
     if [[ "$DETECT_NGXVER" -le '1011004' && "$DETECT_NGXVER" -ge '1011000' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records.patch'
     elif [[ "$DETECT_NGXVER" -ge '1011005' ]]; then
      NGINXTLSPATCH_NAME='nginx__dynamic_tls_records_1011005.patch'
     fi
     rm -rf "${NGINXTLSPATCH_NAME}"
     if [[ "$DETECT_NGXVER" -le '1011004' && "$DETECT_NGXVER" -ge '1011000' ]]; then
      NGINXTLSPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/${NGINXTLSPATCH_NAME}"
		  # fallback mirror if github down, use gitlab mirror
      curl -sI --connect-timeout 5 --max-time 5 "${NGINXTLSPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
      NGINXTLSPATCH_CURLCHECK=$?
      if [[ "$NGINXTLSPATCH_CURLCHECK" != '0' ]]; then
        NGINXTLSPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${NGINXTLSPATCH_NAME}"
      fi
      wget -cnv --no-check-certificate "$NGINXTLSPATCHLINK"
      if [[ -f "${NGINXTLSPATCH_NAME}" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
    	   echo "patch -p1 < "${NGINXTLSPATCH_NAME}""
         patch -p1 < "${NGINXTLSPATCH_NAME}"
      fi
     elif [[ "$DETECT_NGXVER" -ge '1011005' ]]; then
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
} 2>&1 | tee "${CENTMINLOGDIR}/patch_patchnginx_${DT}.log"
}

luapatch() {
	if [ ! -f lua.patch ]; then
    {
		# echo
		echo "patching lua for nginx 1.9.11 workaround"
		# patch for nginx 1.9.11 and lua nginx 0.9.20
		# from https://github.com/openresty/lua-nginx-module/pull/669
		# https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945
		wget -O "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/lua.patch" https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945.patch
		pushd "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/"
		patch -p1 < lua.patch
		popd
  } 2>&1 | tee "${CENTMINLOGDIR}/patch_luapatch_${DT}.log"

	fi
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
	curl -sI --connect-timeout 5 --max-time 5 $OPENRESTYLUAPATCHLINK | grep 'HTTP\/' | egrep '200|302' >/dev/null 2>&1
	OPENRESTYLUAPATCH_CURLCHECK=$?
	if [[ "$OPENRESTYLUAPATCH_CURLCHECK" != '0' ]]; then
		OPENRESTYLUAPATCHLINK='https://gitlab.com/centminmod-github-mirror/openresty/raw/master/patches/nginx-1.9.7-ssl_cert_cb_yield.patch'
	fi
	wget "$OPENRESTYLUAPATCHLINK"
	patch -p1 < nginx-1.9.7-ssl_cert_cb_yield.patch
	popd
} 2>&1 | tee "${CENTMINLOGDIR}/patch_luaopensslpatch_${DT}.log"

}

ngxpagespeed_patched() {
  {
	echo "patching ngx_pagespeed for nginx 1.9.11 support"
	if [ -d "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}" ]; then
		pushd "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}"
		wget -O "$DIR_TMP//ngx_pagespeed-release-${NGXPGSPEED_VER}/nginxpagespeed.patch" https://github.com/pagespeed/ngx_pagespeed/commit/1748d934f3b374d11edfeb3e38605c5c75eef14b.patch
	elif [ -d "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}" ]; then
		pushd "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}"
		wget -O "$DIR_TMP//ngx_pagespeed-${NGXPGSPEED_VER}/nginxpagespeed.patch" https://github.com/pagespeed/ngx_pagespeed/commit/1748d934f3b374d11edfeb3e38605c5c75eef14b.patch
	fi
	patch -p1 < nginxpagespeed.patch
	popd
} 2>&1 | tee "${CENTMINLOGDIR}/patch_ngxpagespeed_patched_${DT}.log"

}