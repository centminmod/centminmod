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