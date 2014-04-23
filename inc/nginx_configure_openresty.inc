function funct_nginxconfigure_openresty {

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

GPERFOPT=" --with-google_perftools_module"

else

GPERFOPT=""

#sed -i 's/google_perftools_profiles \/var\/tmp\/tcmalloc;//g' /usr/local/nginx/conf/nginx.conf

fi

MEMCDIR=`ls -rt $DIR_TMP | grep agentzh-memc-nginx-module`
SRCACHEDIR=`ls -rt $DIR_TMP | grep agentzh-srcache-nginx-module`
#AUTOLIBDIR=`ls -rt $DIR_TMP | grep simpl-ngx_auto_lib`

#export OPENSSL_LIB=../openssl-${OPENSSL_VERSION}/.openssl/lib
#export OPENSSL_INC=../openssl-${OPENSSL_VERSION}/.openssl/include

    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
    if [[ "$key" = [yY] ]]; then

./configure --sbin-path=/usr/local/sbin --conf-path=/usr/local/nginx/conf/nginx.conf --with-ipv6 --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-1.4 --add-module=../nginx-accesskey-2.0.3${GPERFOPT} --add-module=../${MEMCDIR} --add-module=../${SRCACHEDIR} --with-openssl=../openssl-${OPENSSL_VERSION} --with-openssl-opt="enable-tlsext"

# --add-module=../${AUTOLIBDIR}

    else

./configure --sbin-path=/usr/local/sbin --conf-path=/usr/local/nginx/conf/nginx.conf --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-1.4 --add-module=../nginx-accesskey-2.0.3${GPERFOPT} --add-module=../${MEMCDIR} --add-module=../${SRCACHEDIR} --with-openssl=../openssl-${OPENSSL_VERSION} --with-openssl-opt="enable-tlsext"

# --add-module=../${AUTOLIBDIR}

    fi   

}