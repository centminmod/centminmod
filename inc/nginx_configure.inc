function funct_nginxconfigure {

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

GPERFOPT=" --with-google_perftools_module"

else

GPERFOPT=""

#sed -i 's/google_perftools_profiles \/var\/tmp\/tcmalloc;//g' /usr/local/nginx/conf/nginx.conf

fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' ]]; then


	if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
		LIBRESSLOPT=" --with-openssl=../portable-${LIBRESSL_VERSION}"
		OPENSSLOPT=""
		LRT='-lrt '
	else
		if [ "$NOSOURCEOPENSSL" == 'n' ]; then
			LIBRESSLOPT=""
			OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"
			LRT=""
		else
			export BPATH=$DIR_TMP
			export STATICLIBSSL="${BPATH}/staticlibssl"
			LIBRESSLOPT=""
			OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"
			LRT=""
		fi
	fi

	if [[ "$NGINX_OPENRESTY" = [yY] ]]; then
		MEMCDIR=`ls -rt $DIR_TMP | grep openresty-memc-nginx-module | egrep -v 'gz|zip' | tail -1`
		SRCACHEDIR=`ls -rt $DIR_TMP | grep openresty-srcache-nginx-module | egrep -v 'gz|zip' | tail -1`
		SETMISCDIR=`ls -rt $DIR_TMP | grep set-misc-nginx-module | egrep -v 'gz|zip' | tail -1`
		DEVELKITDIR=`ls -rt $DIR_TMP | grep ngx_devel_kit | egrep -v 'gz|zip' | tail -1`
		ECHODIR=`ls -rt $DIR_TMP | grep echo-nginx-module | egrep -v 'gz|zip' | tail -1`
		MEMCOPT=" --add-module=../${MEMCDIR}"
		SRCCACHEOPT=" --add-module=../${SRCACHEDIR}"
		SETMISCOPT=" --add-module=../${SETMISCDIR}"
		DEVELKITOPT=" --add-module=../${DEVELKITDIR}"
		ECHOOPT=" --add-module=../${ECHODIR}"
	else
		MEMCOPT=""
		SRCCACHEOPT=""
		SETMISCOPT=""
		DEVELKITOPT=""
		ECHOOPT=""		
	fi

	if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
		LUAOPT=" --add-module=../lua-nginx-module-${ORESTY_LUANGINXVER} --add-module=../lua-upstream-nginx-module-${ORESTY_LUAUPSTREAMVER} --add-module=../lua-upstream-cache-nginx-module-${ORESTY_LUAUPSTREAMCACHEVER}"
		if [[ ! -f /usr/bin/lua || ! -f /usr/include/lua.h ]]; then
			yum -q -y install lua lua-devel
		fi

    	if [[ "$(uname -m)" = 'x86_64' ]]; then
        	LUALIB_DIR='lib64'
    	else
        	LUALIB_DIR='lib'
    	fi
	
    	cd ${DIR_TMP}/lua-resty-memcached-${ORESTY_LUAMEMCACHEDVER}; make clean; make install
    	cd ${DIR_TMP}/lua-resty-mysql-${ORESTY_LUAMYSQLVER}; make clean; make install
    	cd ${DIR_TMP}/lua-resty-redis-${ORESTY_LUAREDISVER}; make clean; make install
    	cd ${DIR_TMP}/lua-resty-dns-${ORESTY_LUADNSVER}; make clean; make install; mv /usr/local/lib/lua/resty/dns/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/dns
    	cd ${DIR_TMP}/lua-resty-upload-${ORESTY_LUAUPLOADVER}; make clean; make install
    	cd ${DIR_TMP}/lua-resty-websocket-${ORESTY_LUAWEBSOCKETVER}; make clean; make install; mv /usr/local/lib/lua/resty/websocket/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/websocket
    	cd ${DIR_TMP}/lua-resty-lock-${ORESTY_LUALOCKVER}; make clean; make install
    	cd ${DIR_TMP}/lua-resty-string-${ORESTY_LUASTRINGVER}; make clean; make install
    	cd ${DIR_TMP}/lua-redis-parser-${ORESTY_LUAREDISPARSERVER}; make clean; make install; mv /usr/local/lib/lua/resty/redis/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/redis
    	cd ${DIR_TMP}/lua-resty-upstream-healthcheck-${ORESTY_LUAUPSTREAMCHECKVER}; make clean; make install; mv /usr/local/lib/lua/resty/upstream/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/upstream
    	cd ${DIR_TMP}/lua-resty-lrucache-${ORESTY_LUALRUCACHEVER}; make clean; make install; mv /usr/local/lib/lua/resty/lrucache/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/lrucache
    	cd ${DIR_TMP}/lua-resty-core-${ORESTY_LUARESTYCOREVER}; make clean; make install; mv /usr/local/lib/lua/resty/core/* /usr/local/lib/lua/resty/; rm -rf /usr/local/lib/lua/resty/core
    	cd ${DIR_TMP}/lua-resty-logger-socket-${ORESTY_LUALOGGERSOCKETVER}; chmod +x lib/resty/logger/*; \cp -rpf lib/resty/logger/* /usr/local/lib/lua/resty/
    	cd ${DIR_TMP}/lua-resty-cookie-${ORESTY_LUACOOKIEVER}; chmod +x lib/resty/*; \cp -rpf lib/resty/* /usr/local/lib/lua/resty/
    	cd ${DIR_TMP}/lua-cjson-${LUACJSONVER}; make clean; make install
    	cd ${DIR_TMP}
	else
		LUAOPT=""
	fi

	if [[ "$NGINX_SPDY" = [yY] ]]; then
		SPDYOPT=" --with-http_spdy_module"
	else
		SPDYOPT=""
	fi
	
	if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
		if [ -f /usr/bin/git ]; then
			VTSOPT=" --add-module=../nginx-module-vts"
			if [[ ! -d "${DIR_TMP}/nginx-module-vts" ]]; then
				cd $DIR_TMP
				git clone $NGX_VTSLINK nginx-module-vts
				# sed -i 's|color:       black;|color:       white;|g' ${DIR_TMP}/nginx-module-vts/share/status.template.html
				sed -i 's|#DED|#43a6df|g' ${DIR_TMP}/nginx-module-vts/share/status.template.html
				# sed -i 's|color:       black;|color:       white;|g' ${DIR_TMP}/nginx-module-vts/share/status.compress.html
				sed -i 's|#DED|#43a6df|g' ${DIR_TMP}/nginx-module-vts/share/status.compress.html
				cd nginx-module-vts/util
				./tplToDefine.sh ../share/status.template.html > ../src/ngx_http_vhost_traffic_status_module_html.h
				# ./tplToDefine.sh ../share/status.compress.html > ../src/ngx_http_vhost_traffic_status_module_html.h
				cd ../
				# setup /vhost_status.html
				cp -a ${DIR_TMP}/nginx-module-vts/share/status.compress.html /usr/local/nginx/html/vhost_status.html
				MAINURIHOST=$HN
				NEWURI="//${MAINURIHOST}/vhost_status"
				sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html
			elif [[ -d "${DIR_TMP}/nginx-module-vts" && -d "${DIR_TMP}/nginx-module-vts/.git" ]]; then
				cd ${DIR_TMP}/nginx-module-vts
				git stash
				git pull
				# sed -i 's|color:       black;|color:       white;|g' ${DIR_TMP}/nginx-module-vts/share/status.template.html
				sed -i 's|#DED|#43a6df|g' ${DIR_TMP}/nginx-module-vts/share/status.template.html
				# sed -i 's|color:       black;|color:       white;|g' ${DIR_TMP}/nginx-module-vts/share/status.compress.html
				sed -i 's|#DED|#43a6df|g' ${DIR_TMP}/nginx-module-vts/share/status.compress.html
				cd nginx-module-vts/util
				./tplToDefine.sh ../share/status.template.html > ../src/ngx_http_vhost_traffic_status_module_html.h
				# ./tplToDefine.sh ../share/status.compress.html > ../src/ngx_http_vhost_traffic_status_module_html.h
				cd ../
				# setup /vhost_status.html
				cp -a ${DIR_TMP}/nginx-module-vts/share/status.compress.html /usr/local/nginx/html/vhost_status.html
				MAINURIHOST=$HN
				NEWURI="//${MAINURIHOST}/vhost_status"
				sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html							
			fi
		else
			VTSOPT=""
		fi
	else
		VTSOPT=""
	fi	

	if [[ "$NGINX_STICKY" = [yY] ]]; then
		STICKYOPT=" --add-module=../nginx-sticky-module-${NGINX_STICKYVER}"
	else
		STICKYOPT=""
	fi

	if [[ "$NGINX_UPSTREAMCHECK" = [yY] ]]; then
		UPSTREAMOPT=" --add-module=../nginx_upstream_check_module-${NGINX_UPSTREAMCHECKVER}"
	else
		UPSTREAMOPT=""
	fi

	if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
		ngxpagespeedinstall
		PGSPEEDOPT=" --add-module=../ngx_pagespeed-release-${NGXPGSPEED_VER}"
	else
		PGSPEEDOPT=""
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
		PCREJITOPT=" --with-pcre=../pcre-${NGINX_PCREVER} --with-pcre-jit"
	else
		PCREJITOPT=""
	fi

	if [[ "$NGINX_WEBDAV" = [yY] ]]; then

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

		WEBDAVOPT=" --with-http_dav_module --add-module=../nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}"
	else
		WEBDAVOPT=""
	fi

else
	if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
		LIBRESSLOPT=" --with-openssl=../portable-${LIBRESSL_VERSION}"
		OPENSSLOPT=""
		LRT='-lrt '
	else
		LIBRESSLOPT=""
		OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"
		LRT=""
	fi

	if [[ "$NGINX_SPDY" = [yY] ]]; then
		SPDYOPT=" --with-http_spdy_module"
	else
		SPDYOPT=""
	fi

	if [[ "$NGINX_STICKY" = [yY] ]]; then
		STICKYOPT=" --add-module=../nginx-sticky-module-${NGINX_STICKYVER}"
	else
		STICKYOPT=""
	fi

	if [[ "$NGINX_UPSTREAMCHECK" = [yY] ]]; then
		UPSTREAMOPT=" --add-module=../nginx_upstream_check_module-${NGINX_UPSTREAMCHECKVER}"
	else
		UPSTREAMOPT=""
	fi

	if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
		ngxpagespeedinstall
		PGSPEEDOPT=" --add-module=../ngx_pagespeed-release-${NGXPGSPEED_VER}"
	else
		PGSPEEDOPT=""
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

		if [ ! -f "/${LIBDIR}/libexpat.so.1" ]; then
		 echo "/${LIBDIR}/libexpat.so.1 not found"
		  yum${CACHESKIP} -q -y install expat expat-devel
		else
		 echo "/${LIBDIR}/libexpat.so.1 found"
		fi

		WEBDAVOPT=" --with-http_dav_module --add-module=../nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}"
	else
		WEBDAVOPT=""
	fi

fi

if [[ "$NGINX_GEOIP" = [yY] ]]; then
	if [[ ! -d /usr/share/GeoIP ]]; then
		geoipinstall
	fi
	GEOIPOPT=' --with-http_geoip_module'
else
	GEOIPOPT=''
fi

if [[ "$NGINX_PASSENGER" = [yY] && -d $(passenger-config --nginx-addon-dir) ]]; then
	sed -i 's/#passenger_/passenger_/g' /usr/local/nginx/conf/passenger.conf
	RUBYMODULE=" --add-module=$(passenger-config --nginx-addon-dir)"
else
	RUBYMODULE=""
fi

if [[ -d "${DIR_TMP}/nginx-${NGINX_VERSION}" && ! "$ngver" ]]; then
	cd ${DIR_TMP}/nginx-${NGINX_VERSION}
fi

if [[ -d "${DIR_TMP}/nginx-${ngver}" && "$ngver" ]]; then
	cd ${DIR_TMP}/nginx-${ngver}
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

if [[ ! -f /usr/bin/jemalloc.sh || ! -f /usr/include/jemalloc ]]; then
	yum -y install jemalloc jemalloc-devel
fi

# NGINX 1.9 stream support & 1.8 threads
# http://nginx.org/en/docs/stream/ngx_stream_core_module.html
NGXINSTALL_VER=$(echo $NGINX_VERSION | cut -d . -f1,2)
NGXUPGRADE_VER=$(echo $ngver | cut -d . -f1,2)
if [[ "$ngver" && "$NGXUPGRADE_VER" = '1.9' ]]; then
	STREAM=' --with-stream --with-stream_ssl_module'
	THREADS=' --with-threads'
	# workaround for nginx 1.9 compatibility
	sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_initworkerby.c
	sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_timer.c
elif [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.9' ]]; then
	STREAM=' --with-stream --with-stream_ssl_module'
	THREADS=' --with-threads'
	# workaround for nginx 1.9 compatibility
	sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_initworkerby.c
	sed -i.bak 's|ngx_http_set_connection_log|ngx_set_connection_log|g' ${DIR_TMP}/lua-nginx-module-${ORESTY_LUANGINXVER}/src/ngx_http_lua_timer.c	
elif [[ "$ngver" && "$NGXUPGRADE_VER" = '1.8' ]]; then
	STREAM=""
	THREADS=' --with-threads'
elif [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.8' ]]; then
	STREAM=""
	THREADS=' --with-threads'	
else
	STREAM=""
	THREADS=""
fi

# intel specific
CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | tee ${CENTMINLOGDIR}/gcc_native.log
if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
	CCM=64
	MTUNEOPT="-m${CCM} -mtune=native "
elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
	CCM=32
	MTUNEOPT="-m${CCM} -mtune=generic "
else
	MTUNEOPT=""
fi

if [[ "$CLANG" = [yY] ]]; then
	export CC="ccache clang"
	export CXX="ccache clang++"
	export CCACHE_CPP2=yes
	CLANG_CCOPT=' -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-c++11-extensions -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign'
else
	CLANG_CCOPT=""
fi

#    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
#    if [[ "$asknginxipv" = [yY] ]]; then
      if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then
      	pwd
      	echo "nginx configure options:"
      	echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro -Wl,-rpath,$LUAJIT_LIB" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-ipv6 --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER} --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}"
		./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro -Wl,-rpath,$LUAJIT_LIB" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-ipv6 --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER} --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}
    else
    	pwd
    	echo "nginx configure options:"
    	echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro -Wl,-rpath,$LUAJIT_LIB" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER} --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}"
		./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro -Wl,-rpath,$LUAJIT_LIB" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER} --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}
    fi   

}