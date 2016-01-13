check_requestscheme() {
# check for REQUEST_SCHEME parameter added in nginx 1.9.2 
# add it if it doesn't exist in fastcgi_param and php include files
for f in $(grep -Rl 'fastcgi_param  SERVER_PROTOCOL    \$server_protocol;' /usr/local/nginx/conf/*); 
  do
    echo "$f"
    ff=$(grep 'REQUEST_SCHEME' $f)
    fff=$(grep 'https if_not_empty' $f)
    if [[ -z "$ff" && -z "$fff" ]]; then
        sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;\nfastcgi_param  HTTPS              \$https if_not_empty;|" $f
    elif [[ -z "$ff" && "$fff" ]]; then
        sed -i "s|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;|fastcgi_param  SERVER_PROTOCOL    \$server_protocol;\nfastcgi_param  REQUEST_SCHEME     \$scheme;|" $f
    fi
done

## DOES NOT WORK due to invalid version comparison for 2 dot
## numbers i.e. 2.0.0 < 1.9.1 would return false
# # check for REQUEST_SCHEME parameter added in nginx 1.9.2 
# # if nginx upgrade or downgrade is less than 1.9.2 comment out
# # REQUEST_SCHEME, if greater or equal to 1.9.2 uncomment
# for f in $(grep -Rl 'REQUEST_SCHEME' /usr/local/nginx/conf/*); 
#   do
#     echo "$f"
#     if [[ "$(expr $ngver \<= 1.9.1)" = 1 ]]; then
#         sed -i "s|fastcgi_param  REQUEST_SCHEME|#fastcgi_param  REQUEST_SCHEME|" $f
#     elif [[ "$(expr $ngver \>= 1.9.2)" = 1 ]]; then
#         sed -i "s|#fastcgi_param  REQUEST_SCHEME|fastcgi_param  REQUEST_SCHEME|" $f
#     fi
# done
}

checkgeoip() {
    GEOIP_CHECK=$(nginx -V 2>&1 | grep geoip)

    if [[ ! -z "$GEOIP_CHECK" && "$(grep 'NGINX_GEOIP=n' centmin.sh)" ]]; then
        cecho "Detected existing Nginx has NGINX_GEOIP=y enabled" $boldyellow
        cecho "however, you are recompiling Nginx with NGINX_GEOIP=n" $boldyellow
        cecho "Is this incorrect and you want to set NGINX_GEOIP=y enabled ? " $boldyellow
        read -ep "Answer y or n. Typing y will set NGINX_GEOIP=y [y/n]: " setgeoip
        if [[ "$setgeoip" = [yY] ]]; then
            NGINX_GEOIP=y 
        fi
    fi
}

checkmap() {
VTSHTTP_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)
VTSMAIN_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)

if [[ -z "$VTSHTTP_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    else
        sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;/g' /usr/local/nginx/conf/nginx.conf
    fi
else
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/#include \/usr\/local\/nginx\/conf\/vts_http.conf/include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi
    else
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_http.conf' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/vts_http.conf/#include \/usr\/local\/nginx\/conf\/vts_http.conf/g' /usr/local/nginx/conf/nginx.conf
        fi
        if [[ "$(grep '^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|^include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|g' /usr/local/nginx/conf/conf.d/virtual.conf
        fi        
    fi    
fi

if [[ -z "$VTSMAIN_INCLUDECHECK" ]]; then
    if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \ninclude \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    else
        sed -i 's/include \/usr\/local\/nginx\/conf\/errorpage.conf;/include \/usr\/local\/nginx\/conf\/errorpage.conf; \n#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf;/g' /usr/local/nginx/conf/conf.d/virtual.conf
        sed -i 's|include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|#include \/usr\/local\/nginx\/conf\/vts_mainserver.conf|' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
fi

if [[ ! -f /usr/local/nginx/conf/vts_http.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_http.conf /usr/local/nginx/conf/vts_http.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_mainserver.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_mainserver.conf /usr/local/nginx/conf/vts_mainserver.conf
fi

if [[ ! -f /usr/local/nginx/conf/vts_server.conf ]]; then
    \cp $CUR_DIR/config/nginx/vts_server.conf /usr/local/nginx/conf/vts_server.conf
fi

if [[ "$NGINX_VHOSTSTATS" = [yY] ]]; then
    if [[ "$(grep '^#vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/#vhost_traffic_status_zone/vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^#vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/#vhost_traffic_status on/vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
else
    if [[ "$(grep '^vhost_traffic_status_zone' /usr/local/nginx/conf/vts_http.conf)" ]]; then
    sed -i 's/vhost_traffic_status_zone/#vhost_traffic_status_zone/' /usr/local/nginx/conf/vts_http.conf
    fi
    if [[ "$(grep '^vhost_traffic_status on' /usr/local/nginx/conf/vts_server.conf)" ]]; then
    sed -i 's/vhost_traffic_status on/#vhost_traffic_status on/' /usr/local/nginx/conf/vts_server.conf
    fi
fi

MAPCHECK=$(grep '/usr/local/nginx/conf/fastcgi_param_https_map.conf' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$MAPCHECK" ]]; then
	sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;/g' /usr/local/nginx/conf/nginx.conf
fi

if [[ ! -f /usr/local/nginx/conf/fastcgi_param_https_map.conf ]]; then
	\cp $CUR_DIR/config/nginx/fastcgi_param_https_map.conf /usr/local/nginx/conf/fastcgi_param_https_map.conf
fi

if [[ -z "$(grep 'fastcgi_param HTTPS $server_https;' /usr/local/nginx/conf/php.conf)" ]]; then
	replace -s '#fastcgi_param HTTPS on;' 'fastcgi_param HTTPS $server_https;' -- /usr/local/nginx/conf/php.conf
fi
}

checknginxmodules() {
    axelsetup

    if [[ "$NGINX_RTMP" = [yY] ]]; then
        if [[ ! -d "${DIR_TMP}/nginx-rtmp-module" ]]; then
            echo
            echo "download nginx-rtmp-module from github"
            cd $DIR_TMP
            git clone git://github.com/arut/nginx-rtmp-module.git
        elif [[ -d "${DIR_TMP}/nginx-rtmp-module" && -d "${DIR_TMP}/nginx-rtmp-module/.git" ]]; then
            echo
            echo "get latest updates nginx-rtmp-module from github"
            cd $DIR_TMP
            git stash
            git pull
            git log -3
        fi
    fi

cecho "Check for old ngx_pagespeed master branch existence" $boldyellow
if [[ -d "${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}/ngx_pagespeed-master" ]]; then
    # rm -rf ${DIR_TMP}/ngx_pagespeed-release-${NGXPGSPEED_VER}
    rm -rf ${DIR_TMP}/ngx_pagespeed-release-*
    rm -rf ${DIR_TMP}/ngx_pagespeed-*
    rm -rf ${DIR_TMP}/release-1.9.32*
    nginxpgspeedtarball
fi

cecho "Check for missing nginx modules" $boldyellow
if [[ ! -f "${DIR_TMP}/${NGX_FANCYINDEXLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CACHEPURGEFILE}" || ! -f "${DIR_TMP}/${NGX_ACCESSKEYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_CONCATLINKFILE}" || ! -f "${DIR_TMP}/${OPENSSL_LINKFILE}" || ! -f "${DIR_TMP}/${LIBRESSL_LINKFILE}" || ! -f "${DIR_TMP}/${PCRELINKFILE}" || ! -f "${DIR_TMP}/${NGX_WEBDAVLINKFILE}" || ! -d "${DIR_TMP}/${NGX_PAGESPEEDGITLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HEADERSMORELINKFILE}" || ! -f "${DIR_TMP}/${NGX_STICKYLINKFILE}" || ! -f "${DIR_TMP}/${NGX_UPSTREAMCHECKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_HTTPREDISLINKFILE}" ]]; then

    if [[ "$PARALLEL_MODE" = [yY] ]] && [[ "$(grep "processor" /proc/cpuinfo |wc -l)" -gt '1' ]]; then
        ngxmoduletarball &
        openssldownload &
        libressldownload &
        wait
    else
        ngxmoduletarball
        openssldownload
        libressldownload
    fi
fi

if [[ "$NGINX_OPENRESTY" = [yY] ]]; then
    if [[ ! -f "${DIR_TMP}/${NGX_MEMCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SRCACHELINKFILE}"|| ! -f "${DIR_TMP}/${NGX_REDISLINKFILE}" || ! -f "${DIR_TMP}/${NGX_ECHOLINKFILE}" || ! -f "${DIR_TMP}/${NGX_SETMISCLINKFILE}" || ! -f "${DIR_TMP}/${NGX_DEVELKITLINKFILE}" ]]; then
    openrestytarball
    fi

    # ORESTY_LUANGINX=y|n
    if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
        if [[ ! -f "${DIR_TMP}/${NGX_LUANGINXLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAGITLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAMEMCACHEDLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAMYSQLLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAREDISLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUADNSLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPLOADLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAWEBSOCKETLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALOCKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUASTRINGLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAREDISPARSERLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMCHECKLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALRUCACHELINKFILE}"  || ! -f "${DIR_TMP}/${NGX_LUARESTYCORELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUALOGGERSOCKETLINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUACOOKIELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUAUPSTREAMCACHELINKFILE}" || ! -f "${DIR_TMP}/${NGX_LUACJSONLINKFILE}" ]]; then
            openrestytarball
        fi
    fi
fi

if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
    # if option to download official github based master ngx_pagespeed
    # remove old version downloaded & download master tarball instead
    cd $DIR_TMP
    rm -rf release-${NGXPGSPEED_VER}*
    nginxpgspeedtarball
fi

}

function funct_nginxupgrade {

checkmap

cecho "**********************************************************************" $boldyellow
cecho "* Nginx Update script - Included in Centmin Extras" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE - $COPYRIGHT" $boldgreen
cecho "**********************************************************************" $boldyellow
echo " "
cecho "This software comes with no warranty of any kind. You are free to use" $boldyellow
cecho "it for both personal and commercial use as licensed under the GPL." $boldyellow
echo " "
if [[ "$UALL" = 'y' ]]; then 
    nukey=y
else
    read -ep "Nginx Upgrade - Would you like to continue? [y/n] " nukey
fi

if [[ "$nukey" = [nN] ]];
then
    exit 0
fi

# DIR_TMP="/svr-setup"
if [ ! -d "$DIR_TMP" ]; then
mkdir $DIR_TMP
fi

funct_mktempfile

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
if [ ! -f /etc/init.d/nginx ]; then
    cp $CUR_DIR/init/nginx /etc/init.d/nginx
    chmod +x /etc/init.d/nginx
    chkconfig --levels 235 nginx on
fi
fi # CENTOS_SEVEN != 7

if [[ "$UALL" = 'y' ]]; then 
    ngver=$NGINX_VERSION
    recompileopenssl='n'
else
    echo ""
    read -ep "Install which version of Nginx? (version i.e. $NGINX_VERSION}): " ngver

    # auto check if static compiled Nginx openssl version matches
    # the one defined in centmin.sh OPENSSL_VERSION variable
    # if doesn't match then auto recompile the statically set
    # OPENSSL_VERSION
    AUTOOPENSSLCHECK=$(nginx -V 2>&1 | grep -Eo "$OPENSSL_VERSION")
    if [[ "$AUTOOPENSSLCHECK" ]]; then
        recompileopenssl='n'
    else
        recompileopenssl='y'
    fi
    echo ""
    checkgeoip
fi # UALL

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

# Backup Nginx CONF
if [ "$NGINXBACKUP" == 'y' ]; then
	nginxbackup
fi

# Backup ngx_pagespeed pagespeed.conf
if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
	if [[ -f /usr/local/nginx/conf/pagespeed.conf ]]; then
		pagespeedbackup
	fi
fi

# tasks for updated ngx_pagespeed module parity
pagespeeduptasks

    echo "*************************************************"
    cecho "* Updating nginx" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

    # nginx Modules / Prerequisites
	cecho "Installing nginx Modules / Prerequisites..." $boldgreen

checknginxmodules

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Upgrade Google Perftools" $boldgreen
    echo "*************************************************"

    # Install libunwind
    echo "Compiling libunwind..."
    if [ -s libunwind-${LIBUNWIND_VERSION}.tar.gz ]; then
        cecho "libunwind ${LIBUNWIND_VERSION} Archive found, skipping download..." $boldgreen 
    else
        $DOWNLOADAPP http://download.savannah.gnu.org/releases/libunwind/libunwind-${LIBUNWIND_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf libunwind-${LIBUNWIND_VERSION}.tar.gz
    cd libunwind-${LIBUNWIND_VERSION}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure
    make${MAKETHREADS}
    make install

    # Install google-perftools
    cd $DIR_TMP

    echo "Compiling google-perftools..."
    if [ -s google-perftools-${GPERFTOOLS_VERSION}.tar.gz ]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://google-perftools.googlecode.com/files/google-perftools-${GPERFTOOLS_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf google-perftools-${GPERFTOOLS_VERSION}.tar.gz
    cd google-perftools-${GPERFTOOLS_VERSION}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure --enable-frame-pointers
    make${MAKETHREADS}
    make install
    #echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
    #/sbin/ldconfig

fi # GPERFTOOL_SOURCEINSTALL

# echo ""
# read -ep "Do you want to recompile OpenSSL ? Only needed if you updated OpenSSL version in centmin.sh [y/n]: " recompileopenssl 
# echo ""

if [[ "$recompileopenssl" = [yY] || "$LIBRESSL_SWITCH" = [yY] ]]; then
    installopenssl
fi # recompileopenssl

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"

    # Install PCRE
    cd $DIR_TMP

    echo "Compiling PCRE..."
    if [ -s pcre-${PCRE_VERSION}.tar.gz ]; then
        cecho "pcre ${PCRE_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-${PCRE_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf pcre-${PCRE_VERSION}.tar.gz
    cd pcre-${PCRE_VERSION}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    ./configure
    make${MAKETHREADS}
    make install

fi

luajitinstall

funct_nginxmodules

check_requestscheme

    # Install nginx
    cd $DIR_TMP

    CUR_NGINXVER=$(nginx -v 2>&1 | awk -F '\\/' '{print $2}' |sed -e 's|\.|0|g' | head -n1)
    CUR_NGINXUPGRADEVER=$(echo $ngver |sed -e 's|\.|0|g' | head -n1)    

if [[ "$NGINXPATCH" = [nN] || "$NGINX_HTTP2" = [nN] ]]; then
    # if centmin.sh option NGINXPATCH=n then disable patches by 
    # wiping the nginx downloaded source and redownloading a fresh copy
    rm -rf nginx-${ngver}*
fi

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
    # if existing Nginx server is detected to have HTTP/2 patch compiled, then
    # wipe nginx download source and redownload a fresh copy to ensure you're
    # patching with latest patch http://nginx.org/patches/http2/ available
    rm -rf nginx-${ngver}*
fi

    echo "Compiling nginx..."
    if [ -s nginx-${ngver}.tar.gz ]; then
        cecho "nginx ${ngver} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP "http://nginx.org/download/nginx-${ngver}.tar.gz" $WGETRETRY
    fi

    tar xvfz nginx-${ngver}.tar.gz
    cd nginx-${ngver}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi

# set_intelflags

if [[ "$NGINXPATCH" = [yY] ]]; then
    echo "*************************************************"
    cecho "Nginx Patch Time - $NGINXPATCH_DELAY seconds delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    patchnginx
fi

funct_nginxconfigure

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx configure failed\n"
        exit
	else
    	echo -e "\n`date`\nSuccess: Nginx configure ok\n"
	fi

# error check
################

    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        time make${MAKETHREADS}
    else
        time make
    fi

    if [[ "$STRIPNGINX" = [yY] ]]; then
        echo
        echo "strip nginx binary..."
        ls -lah objs/nginx
        strip -s objs/nginx
        ls -lah objs/nginx
        echo
    fi

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx make failed\n"
    	exit
	else
	   echo -e "\n`date`\nSuccess: Nginx make ok\n"
	fi

# error check
################

# cmservice nginx stop
/usr/local/sbin/nginx -s stop

    # speed up nginx wait time if not many vhosts are on server
    if [[ "$(ls /usr/local/nginx/conf/conf.d/ | wc -l)" -le 5 ]]; then
        NGINXUPGRADESLEEP=4
    fi

# sleep $NGINXUPGRADESLEEP

sleep 3
NGINXPSCHECK=`ps --no-heading -C nginx`

if [ ! -z "$NGINXPSCHECK" ]; then
echo ""
echo "nginx seems to be still running, trying to stop it again..."
echo ""
# /etc/init.d/nginx stop and suppress any error messages
/usr/local/sbin/nginx -s stop 2>/dev/null
sleep $NGINXUPGRADESLEEP
fi

    time make install

if [[ "$CLANG" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CCACHE_CPP2
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
fi        

# unset_intelflags

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
    	echo -e "\n`date`\nError: $ERR, Nginx wasn't installed properly\n"
    	exit
	else
    	echo -e "\n`date`\nSuccess: Nginx was installed properly\n"

    if [[ "$NGINX_HTTP2" = [yY] ]] && [[ "$NGX_VEREVAL" -ge '10903' ]]; then
        # only apply auto vhost changes forNginx HTTP/2 
        # if Nginx version is >= 1.9.3 and <1.9.5 OR >= 1.9.5
        if [[ "$NGX_VEREVAL" -ge '10903' && "$NGX_VEREVAL" -lt '10905' ]] || [[ "$NGX_VEREVAL" -ge '10905' ]]; then
            for v in $(ls /usr/local/nginx/conf/conf.d/*.conf); do echo $v; egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v; echo "---"; sed -i 's|ssl spdy|ssl http2|g' $v; sed -i 's|spdy_headers_comp|#spdy_headers_comp|g' $v; sed -i 's|add_header Alternate-Protocol|#add_header Alternate-Protocol|g' $v; egrep -n 'ssl http2|spdy_headers_comp|Alternate-Protocol' $v;done
        fi
        if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
            sed -i 's|ssl spdy|ssl http2|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|spdy_headers_comp|#spdy_headers_comp|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|add_header Alternate-Protocol|#add_header Alternate-Protocol|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
        fi
    elif [[ "$NGINX_HTTP2" = [nN] || "$NGINX_SPDY" = [yY] ]] || [[ "$NGX_VEREVAL" -lt '10903' ]]; then
        for v in $(ls /usr/local/nginx/conf/conf.d/*.conf); do echo $v; egrep -n 'ssl http2|#spdy_headers_comp|#Alternate-Protocol' $v; egrep -n 'ssl spdy|spdy_headers_comp' $v; echo "---"; sed -i 's|ssl http2|ssl spdy|g' $v; sed -i 's|#spdy_headers_comp|spdy_headers_comp|g' $v; sed -i 's|#add_header Alternate-Protocol|add_header Alternate-Protocol|g' $v; egrep -n 'ssl spdy|spdy_headers_comp|Alternate-Protocol' $v;done
        if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
            sed -i 's|ssl http2|ssl spdy|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#spdy_headers_comp|spdy_headers_comp|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
            sed -i 's|#add_header Alternate-Protocol|add_header Alternate-Protocol|g' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
        fi        
    fi

        /etc/init.d/nginx start

        # cecho "Checking OpenSSL version used by Nginx..." $boldyellow
        # SSLIB=$(ldd `which nginx` | grep ssl | awk '{print $3}')
        # OPENSSLVER_CHECK=$(strings $SSLIB | grep "^OpenSSL ")
        # echo $OPENSSLVER_CHECK

        CBODYCHECK=`grep 'client_body_in_file_only on' /usr/local/nginx/conf/nginx.conf`
        if [ $CBODYCHECK ]; then
            sed -i 's/client_body_in_file_only on/client_body_in_file_only off/g' /usr/local/nginx/conf/nginx.conf
        fi

        geoinccheck
        geoipphp


# clean up /svr-setup downloads directory of older PHP source tarball/directories
find $DIR_TMP -maxdepth 1 ! -name "nginx-${ngver}*" -name 'nginx-1*' -print
find $DIR_TMP -maxdepth 1 ! -name "nginx-${ngver}*" -name 'nginx-1*' -exec rm -rf {} \;
find $DIR_TMP -maxdepth 1 ! -name "ngx_pagespeed-release-${NGXPGSPEED_VER}*" -name 'ngx_pagespeed*' -print
find $DIR_TMP -maxdepth 1 ! -name "ngx_pagespeed-release-${NGXPGSPEED_VER}*" -name 'ngx_pagespeed*' -exec rm -rf {} \;
find $DIR_TMP -maxdepth 1 ! -name "libressl-${LIBRESSL_VERSION}*" -name 'libressl*' -print
find $DIR_TMP -maxdepth 1 ! -name "libressl-${LIBRESSL_VERSION}*" -name 'libressl*' -exec rm -rf {} \;
find $DIR_TMP -maxdepth 1 ! -name "openssl-${OPENSSL_VERSION}*" -name 'openssl*' -print
find $DIR_TMP -maxdepth 1 ! -name "openssl-${OPENSSL_VERSION}*" -name 'openssl*' -exec rm -rf {} \;
find $DIR_TMP -maxdepth 1 ! -name "pcre-${NGINX_PCREVER}*" -name 'pcre-*' -print
find $DIR_TMP -maxdepth 1 ! -name "pcre-${NGINX_PCREVER}*" -name 'pcre-*' -exec rm -rf {} \;

        echo "*************************************************"
        cecho "* nginx updated" $boldgreen
        echo "*************************************************"
	fi

# error check
################

echo " "

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf /svr-setup
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

}