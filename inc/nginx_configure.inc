function funct_nginxconfigure {

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

GPERFOPT=" --with-google_perftools_module"

else

GPERFOPT=""

#sed -i 's/google_perftools_profiles \/var\/tmp\/tcmalloc;//g' /usr/local/nginx/conf/nginx.conf

fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

	if [ "$NOSOURCEOPENSSL" == 'n' ]; then
		OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"
	else
		export BPATH=$DIR_TMP
		export STATICLIBSSL="${BPATH}/staticlibssl"
		# OPENSSLOPT=""
		OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"
	fi

	if [[ "$NGINX_SPDY" = [yY] ]]; then
		SPDYOPT=" --with-http_spdy_module"
	else
		SPDYOPT=""
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
	OPENSSLOPT=" --with-openssl=../openssl-${OPENSSL_VERSION}"

	if [[ "$NGINX_SPDY" = [yY] ]]; then
		SPDYOPT=" --with-http_spdy_module"
	else
		SPDYOPT=""
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

if [[ "$CUSTOMSERVERNAME" == [yY] ]]; then
	echo ""
	echo "Check existing server string:"
grep "Server: " src/http/ngx_http_header_filter_module.c | grep -v full_string
	echo ""
	echo "Change server string to $CUSTOMSERVERSTRING"
sed -i "s/Server: nginx/Server: $CUSTOMSERVERSTRING/g" src/http/ngx_http_header_filter_module.c
	echo ""
fi

checkgcc

#    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
#    if [[ "$asknginxipv" = [yY] ]]; then
      if [[ "$asknginxipv" = [yY] || "$NGINX_IPV" = [yY] ]]; then

./configure ${CPUCCOPT}--with-cc-opt="-I${STATICLIBSSL}/include -I/usr/include" --with-ld-opt="-L${STATICLIBSSL}/lib -Wl,-rpath -lssl -lcrypto -ldl -lz" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-ipv6 --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-2.1 --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${OPENSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}

    else

./configure ${CPUCCOPT}--with-cc-opt="-I${STATICLIBSSL}/include -I/usr/include" --with-ld-opt="-L${STATICLIBSSL}/lib -Wl,-rpath -lssl -lcrypto -ldl -lz" --sbin-path=/usr/local/sbin/nginx --conf-path=/usr/local/nginx/conf/nginx.conf --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_image_filter_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module${GEOIPOPT} --with-openssl-opt="enable-tlsext" --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-2.1 --add-module=../headers-more-nginx-module-${NGINX_HEADERSMORE} --add-module=../nginx-accesskey-2.0.3 --add-module=../nginx-http-concat-master${GPERFOPT}${WEBDAVOPT}${RUBYMODULE}${OPENSSLOPT}${LIBATOMICOPT}${PCREJITOPT}${SPDYOPT}${PGSPEEDOPT}
    fi   

}