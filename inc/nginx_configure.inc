function funct_nginxconfigure {

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

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

GPERFOPT=" --with-google_perftools_module"

else

GPERFOPT=""

#sed -i 's/google_perftools_profiles \/var\/tmp\/tcmalloc;//g' /usr/local/nginx/conf/nginx.conf

fi

if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then
	libbroti_install
	ngxbrotli_download
	BROTLIOPT=' --add-module=../ngx_brotli'
else
	BROTLIOPT=""
fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' || "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then

	if [[ "$NGINX_NJS" = [yY] ]]; then
		NGINX_NJSOPT=' --add-module=../njs/nginx'
		if [ ! -f /usr/bin/hg ]; then
			yum -y install mercurial
		fi
		if [[ -f /usr/bin/hg && ! -d "${DIR_TMP}/njs" ]]; then
			cd $DIR_TMP
			hg clone http://hg.nginx.org/njs/
		elif [ -f /usr/bin/hg ]; then
			cd ${DIR_TMP}/njs
			hg pull
		fi
	else
		NGINX_NJSOPT=""
	fi

	if [[ "$NGINX_DEBUG" = [yY] ]]; then
		NGINX_DEBUGOPT=' --with-debug'
		STRIPNGINX=n
	else
		NGINX_DEBUGOPT=""
	fi

	if [[ "$NGINX_HTTP2" = [yY] ]]; then	
		# only apply Nginx HTTP/2 if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
		if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]]; then
			NGINX_SPDY=n		
			HTTPTWOOPT=' --with-http_v2_module'
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
		IMAGEFILTEROPT=" --with-http_image_filter_module"
	else
		IMAGEFILTEROPT=""
	fi	

	if [[ "$NGINX_CACHEPURGE" = [yY] ]]; then
		CACHEPURGEOPT=" --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER}"
	else
		CACHEPURGEOPT=""
	fi	

	if [[ "$NGINX_ACCESSKEY" = [yY] ]]; then
		ACCESSKEYOPT=" --add-module=../nginx-accesskey-2.0.3"
	else
		ACCESSKEYOPT=""
	fi		

	if [[ "$NGINX_HTTPCONCAT" = [yY] ]]; then
		HTTPCONCATOPT=" --add-module=../nginx-http-concat-master"
	else
		HTTPCONCATOPT=""
	fi				

	if [[ "$ORESTY_HEADERSMORE" = [yY] ]]; then
		HEADERSMOREOPT=" --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE}"
	else
		HEADERSMOREOPT=""
	fi				

	if [[ "$NGINX_HTTPREDIS" = [yY] ]]; then
		HTTPREDISOPT=" --add-module=../ngx_http_redis-${NGINX_HTTPREDISVER}"
	else
		HTTPREDISOPT=""
	fi

	if [[ "$NGINX_AUTHREQ" = [yY] ]]; then
		AUTHREQOPT=' --with-http_auth_request_module'
	else
		AUTHREQOPT=""
	fi

	if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
		LIBRESSLOPT=" --with-openssl=../libressl-${LIBRESSL_VERSION}"
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
		REDISDIR=`ls -rt $DIR_TMP | grep redis2-nginx-module | egrep -v 'gz|zip' | tail -1`
		MEMCOPT=" --add-module=../${MEMCDIR}"
		SRCCACHEOPT=" --add-module=../${SRCACHEDIR}"
		SETMISCOPT=" --add-module=../${SETMISCDIR}"
		DEVELKITOPT=" --add-module=../${DEVELKITDIR}"
		ECHOOPT=" --add-module=../${ECHODIR}"
		REDISOPT=" --add-module=../${REDISDIR}"
	else
		MEMCOPT=""
		SRCCACHEOPT=""
		SETMISCOPT=""
		DEVELKITOPT=""
		ECHOOPT=""
		REDISOPT=""		
	fi

	if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
		LUAOPT=" --add-module=../lua-nginx-module-${ORESTY_LUANGINXVER} --add-module=../lua-upstream-nginx-module-${ORESTY_LUAUPSTREAMVER} --add-module=../lua-upstream-cache-nginx-module-${ORESTY_LUAUPSTREAMCACHEVER}"
		LUALD_OPT=" -Wl,-rpath,$LUAJIT_LIB"
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
		LUALD_OPT=""
	fi

	if [[ "$NGINX_SPDY" = [yY] ]]; then
		SPDYOPT=" --with-http_spdy_module"
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
		FANCYINDEXOPT=" --add-module=../ngx-fancyindex-ngx-fancyindex"
	else
		FANCYINDEXOPT=""
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
				if [ -f /usr/local/nginx/html/vhost_status.html ]; then
					sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html
				fi
			elif [[ -d "${DIR_TMP}/nginx-module-vts" && -d "${DIR_TMP}/nginx-module-vts/.git" ]]; then
				rm -rf ${DIR_TMP}/nginx-module-vts
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
				if [ -f /usr/local/nginx/html/vhost_status.html ]; then
					sed -i "s|{{uri}}|$NEWURI|" /usr/local/nginx/html/vhost_status.html
				fi
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
		# only need to invoke ngxpagespeedinstall at this stage if it is nginx upgrade
		# no need for it to be invoked at initial first time centmin mod install as
		# inc/nginx_install.inc has it's own call to this function when variable
		# NGINX_PAGESPEED=y
		if [[ "$ngver" ]]; then
			ngxpagespeedinstall
		fi
        if [ -d "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}" ]; then
            PGSPEEDOPT=" --add-module=../ngx_pagespeed-release-${NGXPGSPEED_VER}"
        elif [ -d "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}" ]; then
            PGSPEEDOPT=" --add-module=../ngx_pagespeed-${NGXPGSPEED_VER}"
        fi			
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

	if [[ "$NGINX_NJS" = [yY] ]]; then
		NGINX_NJSOPT=' --add-module=../njs/nginx'
		if [ ! -f /usr/bin/hg ]; then
			yum -y install mercurial
		fi
		if [[ -f /usr/bin/hg && ! -d "${DIR_TMP}/njs" ]]; then
			cd $DIR_TMP
			hg clone http://hg.nginx.org/njs/
		elif [ -f /usr/bin/hg ]; then
			cd ${DIR_TMP}/njs
			hg pull
		fi
	else
		NGINX_NJSOPT=""
	fi	

	if [[ "$NGINX_DEBUG" = [yY] ]]; then
		NGINX_DEBUGOPT=' --with-debug'
		STRIPNGINX=n
	else
		NGINX_DEBUGOPT=""
	fi	

	if [[ "$NGINX_HTTP2" = [yY] ]]; then	
		# only apply Nginx HTTP/2 if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
		if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]]; then
			NGINX_SPDY=n		
			HTTPTWOOPT=' --with-http_v2_module'
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
		IMAGEFILTEROPT=" --with-http_image_filter_module"
	else
		IMAGEFILTEROPT=""
	fi	

	if [[ "$NGINX_CACHEPURGE" = [yY] ]]; then
		CACHEPURGEOPT=" --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER}"
	else
		CACHEPURGEOPT=""
	fi	

	if [[ "$NGINX_ACCESSKEY" = [yY] ]]; then
		ACCESSKEYOPT=" --add-module=../nginx-accesskey-2.0.3"
	else
		ACCESSKEYOPT=""
	fi		

	if [[ "$NGINX_HTTPCONCAT" = [yY] ]]; then
		HTTPCONCATOPT=" --add-module=../nginx-http-concat-master"
	else
		HTTPCONCATOPT=""
	fi			

	if [[ "$ORESTY_HEADERSMORE" = [yY] ]]; then
		HEADERSMOREOPT=" --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE}"
	else
		HEADERSMOREOPT=""
	fi	

	if [[ "$NGINX_HTTPREDIS" = [yY] ]]; then
		HTTPREDISOPT=" --add-module=../ngx_http_redis-${NGINX_HTTPREDISVER}"
	else
		HTTPREDISOPT=""
	fi	

	if [[ "$NGINX_AUTHREQ" = [yY] ]]; then
		AUTHREQOPT=' --with-http_auth_request_module'
	else
		AUTHREQOPT=""
	fi

	if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
		LIBRESSLOPT=" --with-openssl=../libressl-${LIBRESSL_VERSION}"
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
		FANCYINDEXOPT=" --add-module=../ngx-fancyindex-ngx-fancyindex"
	else
		FANCYINDEXOPT=""
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
        if [ -d "$DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}" ]; then
            PGSPEEDOPT=" --add-module=../ngx_pagespeed-release-${NGXPGSPEED_VER}"
        elif [ -d "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}" ]; then
            PGSPEEDOPT=" --add-module=../ngx_pagespeed-${NGXPGSPEED_VER}"
        fi		
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

# disable Clang compiler for Nginx if NGINX_PASSENGER=y as Clang fails
# to compile Passenger Nginx Module while GCC compiler works
if [[ "$NGINX_PASSENGER" = [yY] ]]; then
	CLANG='n'
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

if [[ ! -f /usr/bin/jemalloc.sh || ! -d /usr/include/jemalloc ]]; then
	yum -y install jemalloc jemalloc-devel
fi

# NGINX 1.9 stream support & 1.8 threads
# http://nginx.org/en/docs/stream/ngx_stream_core_module.html
NGXINSTALL_VER=$(echo $NGINX_VERSION | cut -d . -f1,2)
NGXUPGRADE_VER=$(echo $ngver | cut -d . -f1,2)
if [[ "$ngver" && "$NGXUPGRADE_VER" = '1.9' ]]; then
	if [[ "$NGINX_STREAM" = [yY] ]]; then
		STREAM=' --with-stream --with-stream_ssl_module'
	else
		STREAM=""
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
elif [[ -z "$ngver" && "$NGXINSTALL_VER" = '1.9' ]]; then
	if [[ "$NGINX_STREAM" = [yY] ]]; then
		STREAM=' --with-stream --with-stream_ssl_module'
	else
		STREAM=""
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

if [[ "$NGINX_THREADS" = [yY] ]]; then
	if [[ "$NGX_VEREVAL" -ge '10800' ]]; then
		THREADS=' --with-threads'
	else
		THREADS=""
	fi
else
	THREADS=""
fi	

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
    if [ "$(grep 'NGX_FOPENOPT' ${CM_INSTALLDIR}/inc/z_custom.inc)" ]; then
        fastopenconfig
    fi
fi

# on low memory systems <=768MB or when NGINX_GEOIPMEM=n
# optimise nginx memory usage when nginx geoip module is 
# enabled (by default) by changing from in memory caching
# of geoip country, city databases to on disk caching 
ngxgeoipmemtweak

# intel specific
CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | tee ${CENTMINLOGDIR}/gcc_native.log
if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
	CCM=64
	MTUNEOPT="-m${CCM} -march=native -mfpmath=sse${NGX_FOPENOPT} "
elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
	CCM=32
	MTUNEOPT="-m${CCM} -march=generic${NGX_FOPENOPT} "
else
	MTUNEOPT=""
fi

if [[ "$CLANG" = [yY] ]]; then
	if [[ ! -f /usr/bin/clang ]]; then
		yum -q -y install clang clang-devel
	fi
	# ccache compiler has some initial overhead for compiles but speeds up subsequent
	# recompiles. however on initial install ccache has no benefits, so for initial
	# centmin mod install disabling ccache will in theory speed up first time installs
	if [[ "$INITIALINSTALL" != [yY] ]]; then	
		export CC="ccache /usr/bin/clang -ferror-limit=0"
		export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
		export CCACHE_CPP2=yes
		CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-c++11-extensions -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion'
	else
		export CC="/usr/bin/clang -ferror-limit=0"
		export CXX="/usr/bin/clang++ -ferror-limit=0"
		# export CCACHE_CPP2=yes
		CLANG_CCOPT=' -Wno-sign-compare -Wno-string-plus-int -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-c++11-extensions -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion'
	fi
else
	CLANG_CCOPT=""
fi

#    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
#    if [[ "$asknginxipv" = [yY] ]]; then
    if [[ "$NGXMODULE_ALTORDER" = [yY] ]]; then
    	if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then
      		pwd
      		echo "nginx configure options (alternate ordering):"
      		echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-ipv6 ${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-openssl-opt="enable-tlsext"${VTSOPT}${LIBATOMICOPT}${THREADS}${STREAM} --with-http_gzip_static_module${BROTLIOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT} --with-http_realip_module${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGINX_NJSOPT}${PCREJITOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${OPENSSLOPT}${LIBRESSLOPT}"
			./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-ipv6 ${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-openssl-opt="enable-tlsext"${VTSOPT}${LIBATOMICOPT}${THREADS}${STREAM} --with-http_gzip_static_module${BROTLIOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT} --with-http_realip_module${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGINX_NJSOPT}${PCREJITOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${OPENSSLOPT}${LIBRESSLOPT}
    	else
    		pwd
    		echo "nginx configure options (alternate ordering):"
    		echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} ${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-openssl-opt="enable-tlsext"${VTSOPT}${LIBATOMICOPT}${THREADS}${STREAM} --with-http_gzip_static_module${BROTLIOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT} --with-http_realip_module${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGINX_NJSOPT}${PCREJITOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${OPENSSLOPT}${LIBRESSLOPT}"
			./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} ${STUBSTATUSOPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-openssl-opt="enable-tlsext"${VTSOPT}${LIBATOMICOPT}${THREADS}${STREAM} --with-http_gzip_static_module${BROTLIOPT}${PGSPEEDOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${GEOIPOPT} --with-http_realip_module${ACCESSKEYOPT}${HTTPCONCATOPT}${FANCYINDEXOPT}${CACHEPURGEOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${MEMCOPT}${SRCCACHEOPT}${HEADERSMOREOPT}${NGINX_NJSOPT}${PCREJITOPT} --with-http_ssl_module${SPDYOPT}${HTTPTWOOPT}${OPENSSLOPT}${LIBRESSLOPT}
    	fi
    else
    	if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then
      		pwd
      		echo "nginx configure options:"
      		echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-ipv6 --with-http_ssl_module${HTTPTWOOPT} --with-http_gzip_static_module${BROTLIOPT}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext"${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${NGINX_NJSOPT}${PGSPEEDOPT}"
			./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-ipv6 --with-http_ssl_module${HTTPTWOOPT} --with-http_gzip_static_module${BROTLIOPT}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext"${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${NGINX_NJSOPT}${PGSPEEDOPT}
    	else
    		pwd
    		echo "nginx configure options:"
    		echo "./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-http_ssl_module${HTTPTWOOPT} --with-http_gzip_static_module${BROTLIOPT}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext"${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}	${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${NGINX_NJSOPT}${PGSPEEDOPT}"
			./configure --with-ld-opt="${LRT}-ljemalloc -Wl,-z,relro${LUALD_OPT}" --with-cc-opt="${MTUNEOPT}-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2${CLANG_CCOPT}" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf${NGINX_DEBUGOPT} --with-http_ssl_module${HTTPTWOOPT} --with-http_gzip_static_module${BROTLIOPT}${STUBSTATUSOPT}${SUBOPT}${ADDITIONOPT}${IMAGEFILTEROPT}${AUTHREQOPT}${SECURELINKOPT}${FLVOPT}${MPOPT}${RTMPOPT} --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext"${FANCYINDEXOPT}${CACHEPURGEOPT}${ACCESSKEYOPT}${HTTPCONCATOPT}${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${MEMCOPT}${SRCCACHEOPT}${DEVELKITOPT}${SETMISCOPT}${ECHOOPT}${REDISOPT}${HTTPREDISOPT}${LUAOPT}${STICKYOPT}${UPSTREAMOPT}${VTSOPT}${HEADERSMOREOPT}${OPENSSLOPT}${LIBRESSLOPT}${LIBATOMICOPT}${THREADS}${STREAM}${PCREJITOPT}${SPDYOPT}${NGINX_NJSOPT}${PGSPEEDOPT}
    	fi
	fi # NGXMODULE_ALTORDER

}