function funct_nginxmodules {

    # nginx Modules
    cd ${DIR_TMP}

    if [ -s "$NGX_FANCYINDEXLINKFILE" ]; then
        cecho "ngx-fancyindex ${NGINX_FANCYINDEXVER} Archive found, skipping download..." $boldgreen
    else
        wget --progress=bar -O "${NGX_FANCYINDEXLINKFILE}" "${NGX_FANCYINDEXLINK}" --tries=3
    fi

    tar zvxf "$NGX_FANCYINDEXLINKFILE"

if [ ! -d ${DIR_TMP}/ngx-fancyindex-ngx-fancyindex ]; then
rm -rf "$NGX_FANCYINDEXLINKFILE"

    if [ -s "$NGX_FANCYINDEXLINKFILE" ]; then
        cecho "ngx-fancyindex ${NGINX_FANCYINDEXVER} Archive found, skipping download..." $boldgreen
    else
        wget --progress=bar -O "${NGX_FANCYINDEXLINKFILE}" "${NGX_FANCYINDEXLINK}" --tries=3
    fi

    tar zvxf "$NGX_FANCYINDEXLINKFILE"

fi

    if [ -s "$NGX_CACHEPURGEFILE" ]; then
        cecho "ngx_cache_purge 2.1 Archive found, skipping download..." $boldgreen
    else
        download_cmd "$NGX_CACHEPURGE" $WGETRETRY
    fi

    tar zvxf "$NGX_CACHEPURGEFILE"

    if [ -s Nginx-accesskey-2.0.5.tar.gz ]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        download_cmd "$NGX_ACCESSKEYLINK" $WGETRETRY
    fi

    tar zvxf "$NGX_ACCESSKEYLINKFILE"

}