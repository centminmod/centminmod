function funct_nginxmodules_openresty {

    # nginx Modules
    cd ${DIR_TMP}

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://gitorious.org/ngx-fancyindex/ngx-fancyindex/archive-tarball/v0.3.1 -O ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

if [ ! -d ${DIR_TMP}/ngx-fancyindex-ngx-fancyindex ]; then
rm -rf ngx-fancyindex-0.3.1.tar.gz

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://gitorious.org/ngx-fancyindex/ngx-fancyindex/archive-tarball/v0.3.1 -O ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi

    tar zvxf ngx-fancyindex-0.3.1.tar.gz

fi

    if [ -s ngx_cache_purge-2.1.tar.gz ]; then
        cecho "ngx_cache_purge 1.4 Archive found, skipping download..." $boldgreen
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

if [ -s memc-nginx-module_v0.13rc3.tar.gz ]; then
  cecho "memc-nginx-module_v0.13rc3.tar.gz Archive found, skipping download..." $boldgreen
  else
  cecho "Error: memc-nginx-module_v0.13rc3.tar.gz not found!!!download now......"
        wget -c --progress=bar --no-check-certificate https://github.com/agentzh/memc-nginx-module/tarball/v0.13rc3 -O memc-nginx-module_v0.13rc3.tar.gz --tries=3
fi

    tar xvzf memc-nginx-module_v0.13rc3.tar.gz

if [ -s srcache-nginx-module_v0.13rc6.tar.gz ]; then
  cecho "srcache-nginx-module_v0.13rc6.tar.gz Archive found, skipping download..." $boldgreen
  else
  cecho "Error: srcache-nginx-module_v0.13rc6.tar.gz not found!!!download now......"
        wget -c --progress=bar --no-check-certificate https://github.com/agentzh/srcache-nginx-module/tarball/v0.13rc6 -O srcache-nginx-module_v0.13rc6.tar.gz --tries=3
fi

    tar xvzf srcache-nginx-module_v0.13rc6.tar.gz

# if [ -s ngx_auto_lib.tar.gz ]; then
#   cecho "ngx_auto_lib.tar.gz Archive found, skipping download..." $boldgreen
#   else
#   cecho "Error: ngx_auto_lib.tar.gz not found!!!download now......"
#         wget -c --progress=bar --no-check-certificate https://github.com/simpl/ngx_auto_lib/tarball/master -O ngx_auto_lib.tar.gz --tries=3
# fi

#     tar xvzf ngx_auto_lib.tar.gz

}