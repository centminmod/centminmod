genpscontrolfile() {

touch /usr/local/nginx/conf/pscontrol
cat > "/usr/local/nginx/conf/pscontrol" <<END
#/bin/bash
# turn ngx_pagespeed on or off switch

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
sed -i 's/pagespeed on;/pagespeed off;/g' /usr/local/nginx/conf/pagespeed.conf
/etc/init.d/nginx restart
}

pson() {
sed -i 's/pagespeed off;/pagespeed on;/g' /usr/local/nginx/conf/pagespeed.conf
/etc/init.d/nginx restart
}

psstatslogoff() {
sed -i 's/pagespeed Statistics on;/pagespeed Statistics off;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
sed -i 's/pagespeed StatisticsLogging on;/pagespeed StatisticsLogging off;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
/etc/init.d/nginx restart
}

psstatslogon() {
sed -i 's/pagespeed Statistics off;/pagespeed Statistics on;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
sed -i 's/pagespeed StatisticsLogging off;/pagespeed StatisticsLogging on;/g' /usr/local/nginx/conf/pagespeedstatslog.conf
/etc/init.d/nginx restart
}

case "\$1" in
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

pagespeeduptasks() {

cecho "**********************************************************************" $boldyellow
cecho "* ngx_pagespeed parity update tasks" $boldgreen
cecho "**********************************************************************" $boldyellow

	if [ -d /var/ngx_pagespeed_cache ]; then
	chmod 1777 /var/ngx_pagespeed_cache
	chmod -R 1777 /var/ngx_pagespeed_cache/http,3A
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

	if [ -z "$PSSTATSCHECK" ]; then
	rm -rf /usr/bin/pscontrol
	rm -rf /usr/local/nginx/conf/pscontrol
	genpscontrolfile
	echo "cp /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol"
	cp /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol
	chmod 0700 /usr/bin/pscontrol
	else
	echo "/usr/bin/pscontrol already up to dated"
	fi
fi

}

pagespeedbackup() {
echo ""
echo "backup ngx_pagespeed pagespeed.conf"
echo "cp -a /usr/local/nginx/conf/pagespeed.conf /usr/local/nginx/conf/pagespeed.conf_${DT}"
cp -a /usr/local/nginx/conf/pagespeed.conf /usr/local/nginx/conf/pagespeed.conf_${DT}
}

ngxpagespeedinstall() {

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

touch /usr/local/nginx/conf/pagespeed.conf
cat > "/usr/local/nginx/conf/pagespeed.conf" <<END
pagespeed off;

#######################################################
# File cache settings 
######################################
# needs to exist and be writable by nginx
pagespeed FileCachePath /var/ngx_pagespeed_cache;

# pagespeed FileCacheSizeKb          102400;
# pagespeed FileCacheCleanIntervalMs 3600000;
# pagespeed FileCacheInodeLimit      500000;

#######################################################
# Set it to 0 if you want to disable this feature.
# pagespeed MessageBufferSize 100000;

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
## 1.7.30.1 beta defaults
######################################
# pagespeed DefaultSharedMemoryCacheKB 20000;
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

  # make_google_analytics_async
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-make-google-analytics-async
#  pagespeed EnableFilters make_google_analytics_async;

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
#  pagespeed EnableFilters rewrite_images;
   pagespeed DisableFilters rewrite_images;
   pagespeed DisableFilters recompress_images;
   pagespeed DisableFilters convert_png_to_jpeg;
   pagespeed DisableFilters extend_cache_images;
#  pagespeed EnableFilters convert_png_to_jpeg;
#  pagespeed EnableFilters convert_jpeg_to_webp;
#  pagespeed EnableFilters convert_to_webp_lossless;

  # sprite_images
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-image-sprite
# pagespeed EnableFilters rewrite_css,sprite_images;

  # Bound the number of images that can be rewritten at any one time; this
  # avoids overloading the CPU.  Set this to 0 to remove the bound.
  #
#  pagespeed ImageMaxRewritesAtOnce 2;

  # insert_dns_prefetch
  # https://developers.google.com/speed/docs/mod_pagespeed/filter-insert-dns-prefetch
#  pagespeed EnableFilters insert_dns_prefetch;

#######################################################
END
fi

if [[ ! -f /usr/local/nginx/conf/pagespeedhandler.conf ]]; then
echo ""
echo "setup pagespeedhandler.conf at /usr/local/nginx/conf/pagespeedhandler.conf"

touch /usr/local/nginx/conf/pagespeedhandler.conf
cat > "/usr/local/nginx/conf/pagespeedhandler.conf" <<END
#  Ensure requests for pagespeed optimized resources go to the pagespeed
#  handler and no extraneous headers get set.
location ~ "\.pagespeed\.([a-z]\.)?[a-z]{2}\.[^.]{10}\.[^.]+" { add_header "" ""; }
location ~ "^/ngx_pagespeed_static/" { }
location ~ "^/ngx_pagespeed_beacon$" { }
location /ngx_pagespeed_statistics { allow 127.0.0.1; deny all; }
location /ngx_pagespeed_message { allow 127.0.0.1; deny all; }
location /pagespeed_console { allow 127.0.0.1; deny all; }
END
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

if [[ ! -f /usr/local/nginx/conf/pscontrol ]]; then

echo ""
echo "setup pscontrol file"
echo "allows you to turn pagespeed support on or off"
echo "  /usr/bin/pscontrol on"
echo "  /usr/bin/pscontrol off"

genpscontrolfile

fi

if [[ ! -f /usr/bin/pscontrol ]]; then
echo "cp /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol"
cp /usr/local/nginx/conf/pscontrol /usr/bin/pscontrol
chmod 0700 /usr/bin/pscontrol
else
echo "/usr/bin/pscontrol already exists"
fi

#check files exist
ls -lah /usr/local/nginx/conf/pagespeed.conf
ls -lah /usr/local/nginx/conf/pagespeedhandler.conf
ls -lah /usr/bin/pscontrol

}