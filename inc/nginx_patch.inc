patchnginx() {
  sleep "$NGINXPATCH_DELAY"

  # unofficial OpenSSL 1.1.0 beta Nginx patches
  DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1,2)
  # detect nginx <=1.11.3 as 1.11.4+ has fix for openssl 1.1 so no patching needed
  # http://hg.nginx.org/nginx/rev/1891b2892b68
  DETECT_NGXVER=$(awk '/define nginx_version  / {print $3}' "/svr-setup/nginx-${NGINX_VERSION}/src/core/nginx.h")
  if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' && "$(echo $OPENSSL_VERSION | grep 'pre')" ]] && [[ "$DETECT_NGXVER" -le '1011003' ]]; then
    if [ -f "$CUR_DIR/patches/nginx/openssl-110pre6.patch" ]; then
      if [ -f "$(which figlet)" ]; then
          figlet -ckf standard "Nginx OpenSSL 1.1.0 Pre/Beta Patch"
      fi
      echo
      cecho "patching nginx for OpenSSL 1.1.0 pre beta support" $boldyellow
      cecho "patch -p1 < $CUR_DIR/patches/nginx/openssl-110pre6.patch" $boldyellow
      patch -p1 < "$CUR_DIR/patches/nginx/openssl-110pre6.patch"
      echo
      cecho "nginx patched for OpenSSL 1.1.0 pre beta" $boldyellow
      echo
    else
       cecho "$CUR_DIR/patches/nginx/openssl-110pre6.patch not found" $boldyellow
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
     echo  "######################################################################"
     echo "Patching Nginx for Dynamic TLS Size Support"
     echo  "######################################################################"
     echo "Cloudflare Nginx Dynamic TLS patch"
     echo "https://github.com/cloudflare/sslconfig/raw/master/patches/nginx__dynamic_tls_records.patch"
     echo  "######################################################################"
     NGINXTLSPATCH_NAME='nginx__dynamic_tls_records.patch'
     rm -rf "${NGINXTLSPATCH_NAME}"
     NGINXTLSPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/${NGINXTLSPATCH_NAME}"
		 # fallback mirror if github down, use gitlab mirror
     curl -sI --connect-timeout 5 --max-time 5 "${NGINXTLSPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
     NGINXTLSPATCH_CURLCHECK=$?
     if [[ "$NGINXTLSPATCH_CURLCHECK" != '0' ]]; then
       NGINXTLSPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${NGINXTLSPATCH_NAME}"
     fi
     wget -cnv --no-check-certificate "$NGINXTLSPATCHLINK"
     if [[ -f "${NGINXTLSPATCH_NAME}" && ! "$(grep 'dyn_rec.threshold' src/event/ngx_event_openssl.c)" ]]; then
    	 patch -p1 < "${NGINXTLSPATCH_NAME}"
     fi
  else
  	 if [ -f "${NGINXTLSPATCH_NAME}" ]; then
  		rm -rf "${NGINXTLSPATCH_NAME}"
  	 fi
	fi
}

luapatch() {
	if [ ! -f lua.patch ]; then
		# echo
		echo "patching lua for nginx 1.9.11 workaround"
		# patch for nginx 1.9.11 and lua nginx 0.9.20
		# from https://github.com/openresty/lua-nginx-module/pull/669
		# https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945
		wget -O "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/lua.patch" https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945.patch
		pushd "$DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/"
		patch -p1 < lua.patch
		popd
	fi
}

luaopensslpatch() {
	# for lua-nginx-module 0.10 https://community.centminmod.com/posts/24549/
	# echo
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
}

ngxpagespeed_patched() {
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
}