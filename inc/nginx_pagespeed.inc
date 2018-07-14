ngxpagespeed_setpipepatch() {
  # patch for centos 6 and ngx_pagespeed 1.13.35.1 beta
  if [[ "$CENTOS_SIX" -eq '6' && -z "$(grep 'ifdef F_SETPIPE_SZ' "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}/src/ngx_event_connection.cc")" ]]; then
    # only patch for 1.13.35.1 and lower
    if [[ "$(echo "$NGINX_PAGESPEEDPSOL_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo $NGINX_PAGESPEEDPSOL_VER | cut -d . -f2)" -le '13' ]] && [[ "$(echo $NGINX_PAGESPEEDPSOL_VER | cut -d . -f3)" -le '35' ]] && [[ "$(echo $NGINX_PAGESPEEDPSOL_VER | cut -d . -f4)" -le '1' ]]; then
        pushd "$DIR_TMP/ngx_pagespeed-${NGXPGSPEED_VER}/src"
        wget -c${ipv_forceopt} --progress=bar https://gist.github.com/centminmod/72ac71f446a1ee169b3fb35cc65da0fa/raw/pagespeed-centos6.patch --tries=3
        patch < pagespeed-centos6.patch 
        popd
    fi
  fi
}

nginx_libuuid() {
  if [ ! -f /usr/include/uuid/uuid.h ]; then
    # https://github.com/pagespeed/ngx_pagespeed/issues/1499
    time $YUMDNFBIN -y install libuuid-devel
  fi
}

genpscontrolfile() {

touch /usr/local/nginx/conf/pscontrol
cat > "/usr/local/nginx/conf/pscontrol" <<END
#/bin/bash
# 1.9.32.1 fixed
# turn ngx_pagespeed on or off switch

CMSDEBUG=n
CENTOSVER=\$(cat /etc/redhat-release | awk '{ print \$3 }')

if [ "\$CENTOSVER" == 'release' ]; then
    CENTOSVER=\$(cat /etc/redhat-release | awk '{ print \$4 }' | cut -d . -f1,2)
    if [[ "\$(cat /etc/redhat-release | awk '{ print \$4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "\$CMSDEBUG" = [yY] ]]; then
  if [[ "\$CENTOS_SEVEN" != '7' ]]; then
            echo "Detected 6.5"
  else
            echo "Detected 7.0"
  fi
fi

cmservice() {
        servicename=\$1
        action=\$2
        if [[ "\$CENTOS_SEVEN" != '7' ]]; then
        echo "service \${servicename} \$action"
        if [[ "\$CMSDEBUG" = [nN] ]]; then
                service \${servicename} \$action
        fi
        else
        echo "systemctl \$action \${servicename}.service"
        if [[ "\$CMSDEBUG" = [nN] ]]; then
                systemctl \$action \${servicename}.service
        fi
        fi
}

psadmin() {
nano -w /usr/local/nginx/conf/pagespeedadmin.conf
}

pshandler() {
nano -w /usr/local/nginx/conf/pagespeedhandler.conf
}

psconf() {
nano -w /usr/local/nginx/conf/pagespeed.conf
}

psstatsconf() {
nano -w /usr/local/nginx/conf/pagespeedstatslog.conf
}

psoff() {
sed -i 's/pagespeed on;/pagespeed unplugged;/g' /usr/local/nginx/conf/pagespeed.conf
sed -i 's/pagespeed on;/pagespeed unplugged;/g' /usr/local/nginx/conf/pagespeedadmin.conf
if [ -d /var/ngx_pagespeed_cache ]; then
    rm -rf /var/ngx_pagespeed_cache/*
fi
if [ -d /tmp/ngx_pagespeed_cache ]; then
    rm -rf /tmp/ngx_pagespeed_cache/*
fi
cmservice nginx restart
}

pson() {
sed -i 's/pagespeed unplugged;/pagespeed on;/g' /usr/local/nginx/conf/pagespeed.conf
sed -i 's/pagespeed unplugged;/pagespeed on;/g' /usr/local/nginx/conf/pagespeedadmin.conf
cmservice nginx restart
}

psstatslogoff() {
sed -i 's/pagespeed Statistics on;/pagespeed Statistics off;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
sed -i 's/pagespeed StatisticsLogging on;/pagespeed StatisticsLogging off;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
cmservice nginx restart
}

psstatslogon() {
sed -i 's/pagespeed Statistics off;/pagespeed Statistics on;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
sed -i 's/pagespeed StatisticsLogging off;/pagespeed StatisticsLogging on;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
cmservice nginx restart
}

case "\$1" in
admin)
psadmin
;;
handler)
pshandler
;;
edit)
psconf
;;
editstats)
psstatsconf
;;
on)
pson
;;
off)
psoff
;;
statson)
psstatslogon
;;
statsoff)
psstatslogoff
;;
*)
echo "pscontrol admin"
echo "pscontrol handler"
echo "pscontrol edit"
echo "pscontrol editstats"
echo "pscontrol on"
echo "pscontrol off"
echo "pscontrol statson"
echo "pscontrol statsoff"
;;
esac
exit
END

}

#####################################################
# create pagespeed.conf
makepsconf() {
touch /usr/local/nginx/conf/pagespeed.conf
cat > "/usr/local/nginx/conf/pagespeed.conf" <<END
pagespeed unplugged;

#######################################################
# Purge nginx pagespeed cache
# https://developers.google.com/speed/pagespeed/module/system#flush_cache
#######################################################
pagespeed EnableCachePurge on;
pagespeed PurgeMethod PURGE;

#######################################################
# File cache settings 
######################################
# needs to exist and be writable by nginx

# pagespeed FileCacheSizeKb          102400;
# pagespeed FileCacheCleanIntervalMs 3600000;
# pagespeed FileCacheInodeLimit      500000;

#######################################################
# By default, ngx_pagespeed adds an X-PageSpeed header with a value of the version of 
# ngx_pagespeed being used. This directive lets you specify the value to use instead:
# pagespeed XHeaderValue "ngx_pagespeed";

#######################################################
# let's speed up PageSpeed by storing it in the super duper fast memcached
# Ensure Memcached server installed http://centminmod.com/memcached.html
# default install for centmin mod is on port 11211, so localhost:11211 is correct
# uncomment - remove hash # in front of below 2 lines to enable
# timeout set at 100 milliseconds
#  pagespeed MemcachedThreads 1;
#  pagespeed MemcachedServers "localhost:11211";
#  pagespeed MemcachedTimeoutUs 100000;

#######################################################
## 1.11.33.0 beta save-data
######################################
# https://developers.google.com/speed/pagespeed/module/filter-image-optimize#image-quality

# https://developers.google.com/speed/pagespeed/module/reference-image-optimize#JpegQualityForSaveData
pagespeed JpegQualityForSaveData 50;

# https://developers.google.com/speed/pagespeed/module/reference-image-optimize#WebpQualityForSaveData
pagespeed WebpQualityForSaveData 50;

# https://developers.google.com/speed/pagespeed/module/filter-image-optimize#resize_rendered_image_dimensions
pagespeed EnableFilters resize_rendered_image_dimensions;

# https://developers.google.com/speed/pagespeed/module/filter-image-optimize#WebpRecompressionQuality
pagespeed WebpRecompressionQuality 75;

#######################################################
## 1.7.30.1 beta defaults
######################################
pagespeed PreserveUrlRelativity on;
pagespeed MaxCombinedCssBytes -1;
pagespeed ImageResolutionLimitBytes 16777216;
# pagespeed EnableFilters inline_google_font_css;

#######################################################
## 1.6.29.3 beta defaults
######################################
pagespeed AvoidRenamingIntrospectiveJavascript on;
pagespeed ImageInlineMaxBytes 3072;
pagespeed CssImageInlineMaxBytes 0;
pagespeed MaxInlinedPreviewImagesIndex -1;
pagespeed MinImageSizeLowResolutionBytes 3072;

#######################################################
## ngx_pagespeed filters settings below ##
######################################

  # show half the users an optimized site, half the regular site
  # change UA-XXXXXXXXXX-1 to your GA unique id
  # uncomment - remove hash # in front of below 5 lines to enable
#  pagespeed RunExperiment on;
#  pagespeed AnalyticsID UA-XXXXXXXXXX-1;
#  pagespeed ExperimentVariable 1;
#  pagespeed ExperimentSpec "id=1;percent=50;level=CoreFilters;enabled=collapse_whitespace,remove_comments;";
#  pagespeed ExperimentSpec "id=2;percent=50";
 
  # Filter settings 
  # filters outlined at http://ngxpagespeed.com/ngx_pagespeed_example/
  pagespeed RewriteLevel CoreFilters;
  pagespeed EnableFilters collapse_whitespace,remove_comments;

  # https://www.modpagespeed.com/doc/config_filters#FinderPropertiesCacheExpirationTimeMs
#  pagespeed FinderPropertiesCacheExpirationTimeMs 7200000;  

  # https://www.modpagespeed.com/doc/config_filters#forbidding
   pagespeed ForbidAllDisabledFilters true;

  # https://developers.google.com/speed/pagespeed/module/filter-attribute-elide
#  pagespeed EnableFilters elide_attributes;

  # https://developers.google.com/speed/pagespeed/module/filter-dedup-inlined-images
#  pagespeed EnableFilters dedup_inlined_images;

  # make_google_analytics_async
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-make-google-analytics-async
#  pagespeed EnableFilters make_google_analytics_async;

  # make_show_ads_async
  # https://developers.google.com/speed/pagespeed/module/filter-make-show-ads-async
#  pagespeed EnableFilters make_show_ads_async;

  # responsive_images, responsive_images_zoom
  # https://developers.google.com/speed/pagespeed/module/filter-image-responsive
#  pagespeed EnableFilters responsive_images;
#  pagespeed EnableFilters responsive_images_zoom;

  # convert_to_webp_animated
  # https://developers.google.com/speed/pagespeed/module/filter-image-optimize#convert_to_webp_animated
#  pagespeed EnableFilters convert_to_webp_animated;

  # inline_google_font_css
  # https://developers.google.com/speed/pagespeed/module/filter-css-inline-google-fonts
  pagespeed EnableFilters inline_google_font_css;

  # require FetchHttps if using https site
#   pagespeed FetchHttps enable;

  # https://www.modpagespeed.com/doc/https_support
    pagespeed SslCertDirectory /etc/pki/tls/certs;
    pagespeed SslCertFile /etc/pki/tls/cert.pem;

  # inline_css
  # https://developers.google.com/speed/pagespeed/module/filter-css-inline
#   pagespeed EnableFilters inline_css;
#   pagespeed CssInlineMaxBytes 2048;

  # prioritize_critical_css
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-prioritize-critical-css
#   pagespeed EnableFilters prioritize_critical_css;

  # move_css_to_head
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-css-to-head
  pagespeed EnableFilters move_css_to_head;

  # move_css_above_scripts
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-css-above-scripts
#  pagespeed EnableFilters move_css_above_scripts;

  # combine_css 
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-css-combine
#  pagespeed EnableFilters combine_css;

   # Uncomment this if you want to prevent mod_pagespeed from combining files
   # (e.g. CSS files) across paths
   #
#  pagespeed CombineAcrossPaths off;

  # combine_javascript
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-js-combine
#  pagespeed EnableFilters combine_javascript;
#  pagespeed MaxCombinedJsBytes 122900;

  # extend_cache
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-cache-extend
#  pagespeed EnableFilters extend_cache;

  # rewrite_css
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-css-rewrite
#  pagespeed EnableFilters rewrite_css;

  # rewrite_javascript
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-js-minify
#  pagespeed EnableFilters rewrite_javascript;

  # inline_preview_images
  # https://developers.google.com/speed/pagespeed/module/filter-inline-preview-images
  pagespeed EnableFilters inline_preview_images;

  # lazyload_images
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-lazyload-images
#  pagespeed EnableFilters lazyload_images;

  # rewrite_images
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-image-optimize
   pagespeed EnableFilters rewrite_images;
#   pagespeed DisableFilters rewrite_images;
#   pagespeed DisableFilters recompress_images;
#   pagespeed DisableFilters convert_png_to_jpeg;
#   pagespeed DisableFilters extend_cache_images;
#  pagespeed EnableFilters convert_png_to_jpeg;
#  pagespeed EnableFilters convert_jpeg_to_webp;
#  pagespeed EnableFilters convert_to_webp_lossless;

  # sprite_images
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-image-sprite
# pagespeed EnableFilters rewrite_css,sprite_images;

  # insert_dns_prefetch
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-insert-dns-prefetch
#  pagespeed EnableFilters insert_dns_prefetch;

  # InPlaceResourceOptimization
  # https://developers.google.com/speed/pagespeed/module/system#ipro
  # Enabled by default as of 1.9.32.1
  pagespeed InPlaceResourceOptimization off;

#######################################################
END
}
#####################################################
makepsadmin() {
touch /usr/local/nginx/conf/pagespeedadmin.conf
cat > "/usr/local/nginx/conf/pagespeedadmin.conf" <<END
#######################################################
# 1.8.31.2 beta required paths
# https://developers.google.com/speed/pagespeed/module/admin
#######################################################
# 1.9.32.1 beta required change
# https://community.centminmod.com/threads/how-can-i-disable-it-after-installation.1603/#post-7665
# add a second pagespeed unplugged; variable 
# loaded in nginx.conf via pagespeedadmin.conf include file
pagespeed unplugged;

#######################################################
## https://developers.google.com/speed/pagespeed/module/admin#virtual-hosts-and-stats
######################################
# pagespeed UsePerVhostStatistics on;

#######################################################
pagespeed FileCachePath /var/ngx_pagespeed_cache;
pagespeed StatisticsPath /ngx_pagespeed_statistics;
pagespeed GlobalStatisticsPath /ngx_pagespeed_global_statistics;
pagespeed MessagesPath /ngx_pagespeed_message;
pagespeed ConsolePath /pagespeed_console;
pagespeed AdminPath /pagespeed_admin;
pagespeed GlobalAdminPath /pagespeed_global_admin;

#######################################################
# Set it to 0 if you want to disable this feature.
# pagespeed MessageBufferSize 100000;

#######################################################
# https://developers.google.com/speed/pagespeed/module/system#tune_thread
# pagespeed NumRewriteThreads 4;
# pagespeed NumExpensiveRewriteThreads 4;

#######################################################
# https://developers.google.com/speed/pagespeed/module/system#image_rewrite_max
# Bound the number of images that can be rewritten at any one time; this
# avoids overloading the CPU.  Set this to 0 to remove the bound.
#
# pagespeed ImageMaxRewritesAtOnce 2;
END

PSADMINCONFCHECK=$(grep '/usr/local/nginx/conf/pagespeedadmin.conf' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$PSADMINCONFCHECK" ]]; then
  sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/g' /usr/local/nginx/conf/nginx.conf
fi

}
#####################################################
# create pagespeedhandler.conf
makepshandler() {
touch /usr/local/nginx/conf/pagespeedhandler.conf
cat > "/usr/local/nginx/conf/pagespeedhandler.conf" <<END
#  Ensure requests for pagespeed optimized resources go to the pagespeed
#  handler and no extraneous headers get set.
location ~ "\.pagespeed\.([a-z]\.)?[a-z]{2}\.[^.]{10}\.[^.]+" { add_header "" ""; }
location ~ "^/pagespeed_static/" { }
location ~ "^/ngx_pagespeed_beacon$" { }
location /ngx_pagespeed_statistics { allow 127.0.0.1; deny all; }
location /ngx_pagespeed_global_statistics { allow 127.0.0.1; deny all; }
location /ngx_pagespeed_message { allow 127.0.0.1; deny all; }
location /pagespeed_console { allow 127.0.0.1; deny all; }
location ~ ^/pagespeed_admin { allow 127.0.0.1; deny all; }
location ~ ^/pagespeed_global_admin { allow 127.0.0.1; deny all; }
END
}
#####################################################
# create pagespeedstatslog.conf
makepsstatlog() {
touch /usr/local/nginx/conf/pagespeedstatslog.conf
cat > "/usr/local/nginx/conf/pagespeedstatslog.conf" <<END
#  Pagespeed stats logging 1.6.29.3+
pagespeed Statistics on;
pagespeed StatisticsLogging on;
pagespeed LogDir /var/log/pagespeed;
END
}
#####################################################

pagespeedbackup() {
echo ""
echo "backup ngx_pagespeed pagespeed.conf"
echo "cp -a /usr/local/nginx/conf/pagespeed.conf /usr/local/nginx/conf/pagespeed.conf_${DT}"
cp -a /usr/local/nginx/conf/pagespeed.conf "/usr/local/nginx/conf/pagespeed.conf_${DT}"

echo ""
echo "backup ngx_pagespeed pagespeedhandler.conf"
echo "cp -a /usr/local/nginx/conf/pagespeedhandler.conf /usr/local/nginx/conf/pagespeedhandler.conf_${DT}"
cp -a /usr/local/nginx/conf/pagespeedhandler.conf "/usr/local/nginx/conf/pagespeedhandler.conf_${DT}"
}

pagespeeduptasks() {

cecho "**********************************************************************" $boldyellow
cecho "* ngx_pagespeed parity update tasks" $boldgreen
cecho "**********************************************************************" $boldyellow

#####################################################
# if NGINX_PAGESPEED=n is disabled in centmin.sh do some commenting out
# modified to support detecting ngx_pagespeed.so dynamic module + load module line
# even if NGINX_PAGESPEED=n set https://community.centminmod.com/posts/47781/
if [[ "$NGINX_PAGESPEED" = [nN] ]]; then
  PSCHECK_DYNAMICMODULE=$(ls /usr/local/nginx/modules | grep -qw 'ngx_pagespeed.so'; echo $?)
  PSCHECK_DYNAMICMODULELOAD=$(grep -qw 'ngx_pagespeed.so' /usr/local/nginx/conf/dynamic-modules.conf; echo $?)
  PSADMINCHECK=$(grep '^include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;' /usr/local/nginx/conf/nginx.conf)

  # do not disable pagespeedadmin.conf if dynamic module and load module line detected
  # for ngx_pagespeed.so
  if [[ "$DYNAMIC_SUPPORT" = [yY] && "$PSCHECK_DYNAMICMODULE" -eq '0' && "$PSCHECK_DYNAMICMODULELOAD" -eq '0' ]]; then
    # enable/uncomment pagespeedadmin.conf include line in nginx.conf if ngx_pagespeed.so dynamic module
    # is detected and loaded
    sed -i 's/^#include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/g' /usr/local/nginx/conf/nginx.conf
  # disable pagespeedadmin.conf if enabled/uncommented
  elif [[ ! -z "$PSADMINCHECK" ]]; then
    sed -i 's/^include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/#include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/g' /usr/local/nginx/conf/nginx.conf
  fi

  # check all Nginx vhosts which have 3 pagespeed include lines enabled/uncommented
  CHECKVHOSTFORPS=$(grep -rl '^include \/usr\/local\/nginx\/conf\/pagespeed' /usr/local/nginx/conf/conf.d/* )

  # for each Nginx vhost with pagespeed include lines enabled, disable then when NGINX_PAGESPEED=n
  if [[ ! -z "$CHECKVHOSTFORPS" ]]; then
    for v in $CHECKVHOSTFORPS
    do
      echo
      echo "commenting out pagespeed includes in $v"
      sed -i 's/include \/usr\/local\/nginx\/conf\/pagespeed.conf;/#include \/usr\/local\/nginx\/conf\/pagespeed.conf;/g' "$v"
      sed -i 's/include \/usr\/local\/nginx\/conf\/pagespeedhandler.conf;/#include \/usr\/local\/nginx\/conf\/pagespeedhandler.conf;/g' "$v"
      sed -i 's/include \/usr\/local\/nginx\/conf\/pagespeedstatslog.conf;/#include \/usr\/local\/nginx\/conf\/pagespeedstatslog.conf;/g' "$v"
    done
  fi
fi # NGINX_PAGESPEED=n

if [[ "$NGINX_PAGESPEED" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
  PSADMINCHECK=$(grep '^#include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;' /usr/local/nginx/conf/nginx.conf)

  # re-enable pagespeedadmin.conf if commented out
  if [[ "$PSADMINCHECK" ]]; then
    sed -i 's/^#include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/include \/usr\/local\/nginx\/conf\/pagespeedadmin.conf;/g' /usr/local/nginx/conf/nginx.conf
  fi
fi
#####################################################

  if [ -d /var/ngx_pagespeed_cache ]; then
  chmod 1777 /var/ngx_pagespeed_cache
    if [ -d /var/ngx_pagespeed_cache/http ]; then
     chmod -R 1777 /var/ngx_pagespeed_cache/http,3A
    fi
  chown -R nginx:nginx /var/ngx_pagespeed_cache/
  ls -lah /var | grep ngx_pagespeed_cache
  fi

  if [ ! -d /var/log/pagespeed ]; then
  mkdir -p /var/log/pagespeed
  chown nginx:nginx /var/log/pagespeed
  ls -lah /var/log/ | grep pagespeed
  fi

if [[ ! -f /usr/local/nginx/conf/pagespeedstatslog.conf ]]; then
echo ""
echo "setup pagespeedstatslog.conf at /usr/local/nginx/conf/pagespeedstatslog.conf"

touch /usr/local/nginx/conf/pagespeedstatslog.conf
cat > "/usr/local/nginx/conf/pagespeedstatslog.conf" <<END
#  Pagespeed stats logging 1.6.29.3+
pagespeed Statistics on;
pagespeed StatisticsLogging on;
pagespeed LogDir /var/log/pagespeed;
END
fi

# for 1.6.29.3 beta added pagespeed console setting
if [ -f /usr/local/nginx/conf/pagespeedhandler.conf ]; then

  CONSOLECHECK=$(grep '/pagespeed_console' /usr/local/nginx/conf/pagespeedhandler.conf)
  
  if [ -z "$CONSOLECHECK" ] ; then

  echo "" >> /usr/local/nginx/conf/pagespeedhandler.conf
  echo "location /pagespeed_console { allow 127.0.0.1; deny all; }" >> /usr/local/nginx/conf/pagespeedhandler.conf

  fi

fi

if [[ -f /usr/bin/pscontrol ]]; then
PSSTATSCHECK=$(grep 'statson' /usr/bin/pscontrol)
PSADMINCHECK=$(grep 'psadmin' /usr/bin/pscontrol)
PSADMINFIXCHECK=$(grep '1.9.32.1 fixed' /usr/bin/pscontrol)
PSCLEARCACHECHECK=$(grep 'ngx_pagespeed_cache' /usr/bin/pscontrol)
PSUNPLUGCHECK=$(grep 'unplugged' /usr/bin/pscontrol)

  if [[ -z "$PSUNPLUGCHECK" || -z "$PSSTATSCHECK" || -z "$PSADMINCHECK" || -z "$PSADMINFIXCHECK" || -z "$PSCLEARCACHECHECK" ]]; then
    rm -rf /usr/bin/pscontrol
    rm -rf /usr/local/nginx/conf/pscontrol
    genpscontrolfile
    echo "\cp -af /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol"
    \cp -af /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol
    chmod 0700 /usr/bin/pscontrol
    else
    echo "/usr/bin/pscontrol already up to dated"
  fi
fi

# checks for ngx_pagespeed 1.8.31.2 beta introduced features
ADMINPATHCHECK=$(grep AdminPath /usr/local/nginx/conf/pagespeedadmin.conf)

if [[ -z "$ADMINPATHCHECK" || ! -f /usr/local/nginx/conf/pagespeedadmin.conf ]]; then
  makepsadmin
  makepsconf
  makepshandler
fi

# checks for ngx_pagespeed 1.9.32.1 beta change
# https://community.centminmod.com/threads/how-can-i-disable-it-after-installation.1603/#post-7665
if [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -ge '13' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f3)" -ge '35' ]]; then
  ADMINPSSWITCHCHECK=$(egrep '^pagespeed on;|pagespeed unplugged;' /usr/local/nginx/conf/pagespeedadmin.conf)
elif [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -lt '13' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f3)" -lt '35' ]]; then
  ADMINPSSWITCHCHECK=$(egrep '^pagespeed on;|pagespeed off;' /usr/local/nginx/conf/pagespeedadmin.conf)
fi

if [[ -z "$ADMINPSSWITCHCHECK" || ! -f /usr/local/nginx/conf/pagespeedadmin.conf ]]; then
  makepsadmin
  # makepsconf
  makepshandler
fi

if [[ "$NGINX_PAGESPEED" = [yY] && "$(uname -m)" = 'x86_64' ]]; then
  # 1.13.35.1 deprecated pagespeed off > pagespeed unplugged
  if [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -ge '13' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f3)" -ge '35' ]]; then
    CHECKNGXPS_UNPLUG=$(grep 'pagespeed off;' /usr/local/nginx/conf/pagespeed.conf)
    if [[ "$CHECKNGXPS_UNPLUG" ]]; then
      sed -i 's|pagespeed off;|pagespeed unplugged;|g' /usr/local/nginx/conf/pagespeed.conf
      if [[ -f /usr/bin/pscontrol ]]; then
        sed -i 's|pagespeed off|pagespeed unplugged|g' /usr/bin/pscontrol
      fi
    fi
    CHECKNGXPSADMIN_UNPLUG=$(grep 'pagespeed off;' /usr/local/nginx/conf/pagespeedadmin.conf)
    if [[ "$CHECKNGXPSADMIN_UNPLUG" ]]; then
      sed -i 's|pagespeed off;|pagespeed unplugged;|g' /usr/local/nginx/conf/pagespeedadmin.conf
      if [[ -f /usr/bin/pscontrol ]]; then
        sed -i 's|pagespeed off|pagespeed unplugged|g' /usr/bin/pscontrol
      fi
    fi
  elif [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f1)" -ge '1' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f2)" -lt '13' ]] && [[ "$(echo "$NGXPGSPEED_VER" | cut -d . -f3)" -lt '35' ]]; then
    CHECKNGXPS_UNPLUG=$(grep 'pagespeed unplugged;' /usr/local/nginx/conf/pagespeed.conf)
    if [[ "$CHECKNGXPS_UNPLUG" ]]; then
      sed -i 's|pagespeed unplugged;|pagespeed off;|g' /usr/local/nginx/conf/pagespeed.conf
      if [[ -f /usr/bin/pscontrol ]]; then
        sed -i 's|pagespeed unplugged|pagespeed off|g' /usr/bin/pscontrol
      fi
    fi
    CHECKNGXPSADMIN_UNPLUG=$(grep 'pagespeed unplugged;' /usr/local/nginx/conf/pagespeedadmin.conf)
    if [[ "$CHECKNGXPSADMIN_UNPLUG" ]]; then
      sed -i 's|pagespeed unplugged;|pagespeed off;|g' /usr/local/nginx/conf/pagespeedadmin.conf
      if [[ -f /usr/bin/pscontrol ]]; then
        sed -i 's|pagespeed unplugged|pagespeed off|g' /usr/bin/pscontrol
      fi
    fi
  fi
fi

# check for 1.8.31.3 change from ngx_pagespeed_static to pagespeed_static
PSTATICCHECK=$(grep ngx_pagespeed_static /usr/local/nginx/conf/pagespeedhandler.conf)

if [[ -z "$PSSTATSCHECK" ]]; then
  makepshandler
fi

ngxpagespeed_setpipepatch

} #pagespeeduptasks

ngxpagespeedinstall() {

  nginx_libuuid

if [[ ! -f /usr/local/nginx/conf/pagespeed.conf ]]; then
echo ""
echo "setup pagespeed.conf at /usr/local/nginx/conf/pagespeed.conf"
echo "create cache directory at /var/ngx_pagespeed"

  if [ ! -d /var/ngx_pagespeed_cache ]; then
  mkdir -p /var/ngx_pagespeed_cache
  mkdir -p /var/ngx_pagespeed_cache/http,3A
  ls -lah /var | grep ngx_pagespeed_cache
  fi

  if [ ! -d /var/log/pagespeed ]; then
  mkdir -p /var/log/pagespeed
  chown nginx:nginx /var/log/pagespeed
  ls -lah /var/log/ | grep pagespeed
  fi

  chmod 1777 /var/ngx_pagespeed_cache
  chmod -R 1777 /var/ngx_pagespeed_cache/http,3A
  chown -R nginx:nginx /var/ngx_pagespeed_cache/
  ls -lah /var | grep ngx_pagespeed_cache

  makepsconf

fi

if [[ ! -f /usr/local/nginx/conf/pagespeedhandler.conf ]]; then
echo ""
echo "setup pagespeedhandler.conf at /usr/local/nginx/conf/pagespeedhandler.conf"
makepshandler
fi

if [[ ! -f /usr/local/nginx/conf/pagespeedstatslog.conf ]]; then
echo ""
echo "setup pagespeedstatslog.conf at /usr/local/nginx/conf/pagespeedstatslog.conf"
makepsstatlog
fi

if [[ ! -f /usr/local/nginx/conf/pscontrol ]]; then

echo ""
echo "setup pscontrol file"
echo "allows you to turn pagespeed support on or off"
echo "  /usr/bin/pscontrol on"
echo "  /usr/bin/pscontrol off"

genpscontrolfile

fi

if [[ ! -f /usr/bin/pscontrol ]]; then
echo "\cp -af /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol"
\cp -af /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol
chmod 0700 /usr/bin/pscontrol
else
echo "/usr/bin/pscontrol already exists"
fi

#check files exist
ls -lah /usr/local/nginx/conf/pagespeed.conf
ls -lah /usr/local/nginx/conf/pagespeedhandler.conf
ls -lah /usr/bin/pscontrol

}