function funct_nginxmodules {

    # nginx Modules
    cd ${DIR_TMP}

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget --progress=bar ${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

if [ ! -d ${DIR_TMP}/ngx-fancyindex-ngx-fancyindex ]; then
rm -rf ngx-fancyindex-0.3.1.tar.gz

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget --progress=bar ${LOCALCENTMINMOD_MIRROR}/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

fi

    if [ -s "$NGX_CACHEPURGEFILE" ]; then
        cecho "ngx_cache_purge 2.1 Archive found, skipping download..." $boldgreen
    else
        download_cmd "$NGX_CACHEPURGE" $WGETRETRY
    fi

    tar zvxf "$NGX_CACHEPURGEFILE"

    if [ -s Nginx-accesskey-2.0.5.tar.gz ]; then
        cecho "Nginx-accesskey 2.0.5 Archive found, skipping download..." $boldgreen
    else
        download_cmd "$NGX_ACCESSKEYLINK" $WGETRETRY
    fi

    tar zvxf "$NGX_ACCESSKEYLINKFILE"

}