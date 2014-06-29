function funct_nginxmodules {

    # nginx Modules
    cd ${DIR_TMP}

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://centminmod.com/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

if [ ! -d ${DIR_TMP}/ngx-fancyindex-ngx-fancyindex ]; then
rm -rf ngx-fancyindex-0.3.1.tar.gz

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://centminmod.com/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

fi

    if [ -s ngx_cache_purge-2.1.tar.gz ]; then
        cecho "ngx_cache_purge 2.1 Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://labs.frickle.com/files/ngx_cache_purge-2.1.tar.gz $WGETRETRY
    fi

    tar zvxf ngx_cache_purge-2.1.tar.gz

    if [ -s Nginx-accesskey-2.0.3.tar.gz ]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://wiki.nginx.org/images/5/51/Nginx-accesskey-2.0.3.tar.gz $WGETRETRY
    fi

    tar zvxf Nginx-accesskey-2.0.3.tar.gz

}