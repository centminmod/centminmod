patchnginx() {
	sleep $NGINXPATCH_DELAY
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
}

luapatch() {
	if [ ! -f lua.patch ]; then
		# echo
		echo "patching lua for nginx 1.9.11 workaround"
		# patch for nginx 1.9.11 and lua nginx 0.9.20
		# from https://github.com/openresty/lua-nginx-module/pull/669
		# https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945
		wget -O $DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/lua.patch https://github.com/charlesportwoodii/lua-nginx-module/commit/8a63903c7152b5417e1bf326f6d6ffad0b729945.patch
		pushd $DIR_TMP/lua-nginx-module-${ORESTY_LUANGINXVER}/
		patch -p1 < lua.patch
		popd
	fi
}

luaopensslpatch() {
	# for lua-nginx-module 0.10 https://community.centminmod.com/posts/24549/
	# echo
	echo "patching nginx for lua 0.10+ branch support"
	if [ "$ngver" ]; then
		pushd $DIR_TMP/nginx-${ngver}
	else
		pushd $DIR_TMP/nginx-${NGINX_VERSION}
	fi
	wget https://github.com/openresty/ngx_openresty/raw/master/patches/nginx-1.9.7-ssl_cert_cb_yield.patch
	patch -p1 < nginx-1.9.7-ssl_cert_cb_yield.patch
	popd
}

ngxpagespeed_patched() {
	echo "patching ngx_pagespeed for nginx 1.9.11 support"
	if [ -d "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}" ]; then
		pushd $DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}
		wget -O $DIR_TMP//ngx_pagespeed-release-${NGXPGSPEED_VER}/nginxpagespeed.patch https://github.com/pagespeed/ngx_pagespeed/commit/1748d934f3b374d11edfeb3e38605c5c75eef14b.patch
	elif [ -d "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}" ]; then
		pushd $DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}
		wget -O $DIR_TMP//ngx_pagespeed-${NGXPGSPEED_VER}/nginxpagespeed.patch https://github.com/pagespeed/ngx_pagespeed/commit/1748d934f3b374d11edfeb3e38605c5c75eef14b.patch
	fi
	patch -p1 < nginxpagespeed.patch
	popd
}