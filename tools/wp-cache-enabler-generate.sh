#!/bin/bash
######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
######################################################
DT=$(date +"%d%m%y-%H%M%S")
SUFFIX="-generated-${DT}"
vhostname=$2
PUBLIC_WEBROOT="/home/nginx/domains/$vhostname/public"
CENTMINLOGDIR='/root/centminlogs'
######################################################


help_function() {
  echo "Usage:"
  echo
  echo "$0 all domain.com"
  echo "$0 include domain.com"
  echo "$0 tryfiles domain.com"
  echo "$0 map domain.com"
  echo "$0 wpsecure domain.com"
}

if [ -z "$vhostname" ]; then
  help_function
fi

# check first if cache enabler exists and is actively in use
is_ce_active=$(\wp plugin is-active cache-enabler --allow-root --path=$PUBLIC_WEBROOT >/dev/null 2>&1; echo $?)
if [[ "$is_ce_active" -eq '0' && -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" ]]; then
  is_ce_used='y'
  # check if wordpress is installed in a subdirectory and populate variables for
  # $custom_subdir and $WPSUBDIR
  check_subdir=$(grep "set \$custom_subdir ''\;" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf")
  if [ ! "$check_subdir" ]; then
    is_subdir='y'
    WPSUBDIR=$(grep "set \$custom_subdir '" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" | awk '{print $3}' | sed -e 's|;||g' -e "s|'||g")
  fi
else
  is_ce_used='n'
  echo
  echo "Cache Enabler plugin not detected in Wordpress installation"
  echo "at $PUBLIC_WEBROOT"
  exit
fi

gen_include() {
  vhostname=$1
  if [[ "$is_ce_used" = [yY] ]]; then
    echo
    echo "=========================================================================="
    echo "Generate include file: wpcacheenabler_${vhostname}.conf${SUFFIX}"
    echo "=========================================================================="
    echo "inspecting include file entry in Nginx vhost"
    echo
    grep "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" /usr/local/nginx/conf/conf.d/${vhostname}.*
    echo
    echo "generating updated WP Cache Enabler include file at:"
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"
cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"<<HFA
    # Block nginx-help log from public viewing
    location ~* /wp-content/uploads/nginx-helper/ { deny all; }

    set \$cache_uri \$request_uri;

    # exclude mobile devices from redis caching
    if (\$cmwpcache_device = mobile) { set \$cache_uri 'nullcache'; }

    # bypass cache if POST requests or URLs with a query string
    if (\$request_method = POST) {
        set \$cache_uri 'nullcache';
    }
    if (\$query_string != "") {
        set \$cache_uri 'nullcache';
    }

    # include query strings fbclid, gclid, utm in cache via stripping them with
    # 302 redirect via mapping in /usr/local/nginx/conf/wpcacheenabler_map.conf
    if (\$q_ignorearg) {
      set \$check_qurl \$request_uri;
      set \$check_surl \$request_uri;
      set \$cache_uri \$uri;
      #rewrite ^ \$uri? redirect;
    }
    add_header Check-Querystring-Uri "\$check_qurl";
    #add_header Q-Ignore-Arg "\$q_ignorearg";

    # bypass cache if URLs containing the following strings
    if (\$request_uri ~* "(\?add-to-cart=|\?wc-api=|/cart/|/my-account/|/checkout/|/shop/checkout/|/wp-json/|/store/checkout/|/customer-dashboard/|/addons/|/wp-admin/|/xmlrpc.php|/wp-(app|cron|login|register|mail).php|wp-.*.php|/feed/|index.php|wp-comments-popup.php|wp-links-opml.php|wp-locations.php|sitemap(_index)?.xml|[a-z0-9_-]+-sitemap([0-9]+)?.xml)") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache if the cookies containing the following strings
    if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in|wc-api|edd_items_in_cart|woocommerce_items_in_cart|woocommerce_cart_hash|woocommerce_recently_viewed|wc_session_cookie_HASH|wp_woocommerce_session_|wptouch_switch_toggle") {
        set \$cache_uri 'nullcache';
    }

    # bypass cache for woocommerce
    if (\$arg_add-to-cart != "") { set \$cache_uri 'nullcache'; }
    if (\$arg_wc-api != "") { set \$cache_uri 'nullcache'; }

    ## bypass cache for empty woocommerce carts
    #if (\$cookie_woocommerce_items_in_cart != "0") { 
    #  set \$cache_uri 'nullcache';
    #}

    # custom sub directory e.g. /blog
    set \$custom_subdir '${WPSUBDIR}';
    #if (\$args ~* s=(.*)) {
    #  set \$cache_uri \$request_uri;
    #  set \$check_surl \$cache_uri;
    #  set \$cache_uri /search/\$1;
    #}
    #add_header Check-Uri "\$check_surl";
    #add_header Set-Uri "\$cache_uri";

    # default html file
    set \$cache_enabler_uri '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}\${scheme}-index.html';

    # webp html file
    if (\$http_accept ~* "image/webp") {
        set \$cache_enabler_uri_webp '\${custom_subdir}/wp-content/cache/cache-enabler/\${http_host}\${cache_uri}\${scheme}-index-webp.html';
    }

    #if (-f \$document_root\$cache_enabler_uri) {
    #set \$cttls "120s";
    #}
    #expires \$cttls;
HFA
    if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}" ]; then
      echo "generated: /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"
    fi
    if [[ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" && -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}" ]]; then
      diffcheck_map=$(diff -u /usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}" >/dev/null 2>&1; echo $?)
      if [[ "$diffcheck_map" -ne '0' ]]; then
        echo
        echo "differences between existing and newly generated wpcacheenabler_map file"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"
        echo
        diff -u "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"
        echo
      else
        echo "no differences detected for"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf${SUFFIX}"
      fi
    fi
  else
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf not found"
    echo "this $vhostname does not appear to be a Cache Enabler plugin based Wordpress site"
  fi
}

gen_tryfiles() {
  vhostname=$1
  if [[ "$is_ce_used" = [yY] ]]; then
    echo
    echo "=========================================================================="
    echo "Generate try_files For location / {} Context:"
    echo "=========================================================================="
    echo "generating updated WP Cache Enabler try_files syntax for nginx vhost at:"
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}"
    echo "for location / {} context for nginx vhosts for non-http and/or https respectively:
  - /usr/local/nginx/conf/conf.d/$vhostname.conf
  - /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf"
cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}"<<EOF
  try_files \$cache_enabler_uri_webp \$cache_enabler_uri \$uri \$uri/ \$custom_subdir/index.php?\$args;
EOF
    if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}" ]; then
      echo "generated: /usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}"
    fi
    if [ -f /usr/local/nginx/conf/conf.d/$vhostname.conf ]; then
      echo
      echo "checking /usr/local/nginx/conf/conf.d/$vhostname.conf"
      check_tryfiles=$(grep try_files /usr/local/nginx/conf/conf.d/$vhostname.conf | grep -v '\#')
      if [ "$check_tryfiles" ]; then
        echo
        echo "differences between existing and newly generated try_files"
        diffcheck_tryfiles=$(diff -uZ <(echo "$check_tryfiles") "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}" >/dev/null 2>&1; echo $?)
        if [ "$diffcheck_tryfiles" -ne '0' ]; then
          echo
          echo "differences detected in /usr/local/nginx/conf/conf.d/$vhostname.conf"
          echo
          diff -uZ <(echo "$check_tryfiles") "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}"
          echo
        else
          echo "no differences detected"
        fi
      fi
    fi
    if [ -f /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf ]; then
      echo
      echo "checking /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf"
      check_tryfiles_ssl=$(grep try_files /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf | grep -v '\#')
      if [ "$check_tryfiles_ssl" ]; then
        echo
        echo "differences between existing and newly generated try_files"
        diffcheck_tryfiles_ssl=$(diff -uZ <(echo "$check_tryfiles_ssl") "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}" >/dev/null 2>&1; echo $?)
        if [ "$diffcheck_tryfiles_ssl" -ne '0' ]; then
          echo
          echo "differences detected in /usr/local/nginx/conf/conf.d/$vhostname.ssl.conf"
          echo
          diff -uZ <(echo "$check_tryfiles_ssl") "/usr/local/nginx/conf/wpincludes/${vhostname}/try_files${SUFFIX}"
          echo
        else
          echo "no differences detected"
        fi
      fi
    fi
  else
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf not found"
    echo "this $vhostname does not appear to be a Cache Enabler plugin based Wordpress site"
  fi
}

gen_map() {
  vhostname=$1
  if [[ "$is_ce_used" = [yY] ]]; then
    echo
    echo "=========================================================================="
    echo "Generate Cache Enabler Map Include File:"
    echo "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
    echo "=========================================================================="
    echo "generating updated WP Cache Enabler include file at:"
    echo "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
cat > "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"<<HFB
map \$http_user_agent \$cmwpcache_device {
    default                                     'desktop';
    ~*(iPad|iPhone|Android|IEMobile|Blackberry) 'mobile';
    "~*Firefox.*Mobile"                         'mobile';
    "~*ipod.*mobile"                            'mobile';
    "~*Opera\ Mini"                             'mobile';
    "~*Opera\ Mobile"                           'mobile';
    "~*Mobile"                                  'mobile';
    "~*Tablet"                                  'mobile';
    "~*Kindle"                                  'mobile';
    "~*Windows\ Phone"                          'mobile';
}

map \$args \$q_ignorearg {
  default               0;
  "~*fbclid"            1;
  "~*gclid"             1;
  "~*utm"               1;
  "~*fb_action_ids"     1;
  "~*fb_action_types"   1;
  "~*fb_source"         1;
  "~*age-verified"      1;
  "~*ao_noptimize"      1;
  "~*usqp"              1;
  "~*cn-reloaded"       1;
  "~*_ga"               1;
  "~*_ke"               1;
  "~*mc_cid"            1;
  "~*mc_eid"            1;
  "~*ref"               1;
}
HFB
    if [ -f "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}" ]; then
      echo "generated: /usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
    fi
    if [[ -f /usr/local/nginx/conf/wpcacheenabler_map.conf && -f "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}" ]]; then
      diffcheck_map=$(diff -qu /usr/local/nginx/conf/wpcacheenabler_map.conf "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}" >/dev/null 2>&1; echo $?)
      if [ "$diffcheck_map" -ne '0' ]; then
        echo
        echo "differences between existing and newly generated wpcacheenabler_map file"
        echo "/usr/local/nginx/conf/wpcacheenabler_map.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
        diff -u /usr/local/nginx/conf/wpcacheenabler_map.conf "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
        echo
      else
        echo "for"
        echo "/usr/local/nginx/conf/wpcacheenabler_map.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpcacheenabler_map.conf${SUFFIX}"
        echo "no differences detected"
      fi
    fi
  else
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpcacheenabler_${vhostname}.conf not found"
    echo "this $vhostname does not appear to be a Cache Enabler plugin based Wordpress site"
  fi
}

gen_wpsecure() {
  vhostname=$1
  if [[ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" ]]; then
    echo
    echo "=========================================================================="
    echo "Generate Wordpress wpsecure include file:"
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
    echo "=========================================================================="
    echo "generating updated Wordpress wpsecure include file at:"
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}" <<EEF
# prevent .zip, .gz, .tar, .bzip2 files from being accessed by default
# impossible for centmin mod to know which wp backup plugins they installed
# which may save backups to directories in wp-content/
# such plugins may deploy .htaccess protection but that isn't supported in
# nginx, so blocking access to these extensions is a workaround to cover all bases

# prepare for letsencrypt 
# https://community.centminmod.com/posts/17774/
location ~ /.well-known {
  location ~ /.well-known/acme-challenge/(.*) {
    more_set_headers    "Content-Type: text/plain";
    }
}

location ~* ${WPSUBDIR}/wp-content/cache/autoptimize/.*\.(js|css)\$ {
  include /usr/local/nginx/conf/php.conf;
  add_header AO-Fallback 1;
  try_files \$uri \$uri/ ${WPSUBDIR}/wp-content/autoptimize_404_handler.php;
  expires 30d;
}

# allow AJAX requests in themes and plugins
location ~ ^${WPSUBDIR}/wp-admin/admin-ajax.php$ { allow all; include /usr/local/nginx/conf/php.conf; }

location ~* ^${WPSUBDIR}/(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z)\$ { deny all; }

location ~ ^${WPSUBDIR}/wp-content/uploads/sucuri { deny all; }

location ~ ^${WPSUBDIR}/wp-content/updraft { deny all; }

# Block nginx-help log from public viewing
location ~* ${WPSUBDIR}/wp-content/uploads/nginx-helper/ { deny all; }

# WebP extension support if you are converting /uploads images to webp
location ~ ^${WPSUBDIR}/wp-content/uploads/ {
  #pagespeed off;
  #pagespeed unplugged;
  #autoindex on;
  #add_header X-Robots-Tag "noindex, nofollow";
  location ~* ^${WPSUBDIR}/wp-content/uploads/(.+/)?(.+)\.(png|jpe?g)\$ {
    expires 30d;
    add_header Vary "Accept";
    add_header Cache-Control "public";
    try_files \$uri\$webp_extension \$uri =404;
  }
}

# Whitelist Exception for seo-by-rank-math
location ~ ^${WPSUBDIR}/wp-content/plugins/seo-by-rank-math/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for amp
location ~ ^${WPSUBDIR}/wp-content/plugins/amp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for async-javascript
location ~ ^${WPSUBDIR}/wp-content/plugins/async-javascript/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for autoptimize
location ~ ^${WPSUBDIR}/wp-content/plugins/autoptimize/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for autoptimize-gzip
location ~ ^${WPSUBDIR}/wp-content/plugins/autoptimize-gzip/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Security for wp-cloudflare-page-cache debug.log which is exposed to public access
# /wp-content/wp-cloudflare-super-page-cache/yourdomain.com/debug.log
location ~ ^${WPSUBDIR}/wp-content/wp-cloudflare-super-page-cache/${vhostname}/(debug.log)\$ {
  deny all;
}

# Whitelist Exception for wp-cloudflare-page-cache
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-cloudflare-page-cache/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for cloudflare
location ~ ^${WPSUBDIR}/wp-content/plugins/cloudflare/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for post-grid
location ~ ^${WPSUBDIR}/wp-content/plugins/post-grid/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for breadcrumb-navxt
location ~ ^${WPSUBDIR}/wp-content/plugins/breadcrumb-navxt/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

location ~ ^${WPSUBDIR}/(wp-includes/js/tinymce/wp-tinymce.php) {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/onesignal-free-web-push-notifications//
location ~ ^${WPSUBDIR}/wp-content/plugins/onesignal-free-web-push-notifications/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sparkpost/
location ~ ^${WPSUBDIR}/wp-content/plugins/sparkpost/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendgrid-email-delivery-simplified/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendgrid-email-delivery-simplified/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailgun/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailgun/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailjet-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailjet-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/easy-wp-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/easy-wp-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/postman-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/postman-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-mail-bank/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-mail-bank/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/theme-check/
location ~ ^${WPSUBDIR}/wp-content/plugins/theme-check/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce-csvimport/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce-csvimport/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/advanced-custom-fields/
location ~ ^${WPSUBDIR}/wp-content/plugins/advanced-custom-fields/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/contact-form-7/
location ~ ^${WPSUBDIR}/wp-content/plugins/contact-form-7/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/duplicator/
location ~ ^${WPSUBDIR}/wp-content/plugins/duplicator/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/jetpack/
location ~ ^${WPSUBDIR}/wp-content/plugins/jetpack/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/nextgen-gallery/
location ~ ^${WPSUBDIR}/wp-content/plugins/nextgen-gallery/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/tinymce-advanced/
location ~ ^${WPSUBDIR}/wp-content/plugins/tinymce-advanced/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/updraftplus/
location ~ ^${WPSUBDIR}/wp-content/plugins/updraftplus/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-importer/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-importer/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-seo/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-seo/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wpclef/
location ~ ^${WPSUBDIR}/wp-content/plugins/wpclef/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailchimp-for-wp/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailchimp-for-wp/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-optimize/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-optimize/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/si-contact-form/
location ~ ^${WPSUBDIR}/wp-content/plugins/si-contact-form/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/akismet/
location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/ {
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(form|akismet)\.(css|js)\$ { allow all; expires 30d;}
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(.+)\.(png|gif)\$ { allow all; expires 30d;}
  location ~* ${WPSUBDIR}/wp-content/plugins/akismet/akismet/.*\.php\$ {
    include /usr/local/nginx/conf/php.conf;
    include /usr/local/nginx/conf/staticfiles.conf;
    # below include file needs to be manually created at that path and to be uncommented
    # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
    # allows you to add commonly shared settings to all wp plugin location matches which
    # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
    #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
    allow 127.0.0.1;
    deny all;
  }
}

# Whitelist Exception for https://wordpress.org/plugins/bbpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/bbpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/buddypress/
location ~ ^${WPSUBDIR}/wp-content/plugins/buddypress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/all-in-one-seo-pack/
location ~ ^${WPSUBDIR}/wp-content/plugins/all-in-one-seo-pack/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/google-analytics-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/google-analytics-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/regenerate-thumbnails/
location ~ ^${WPSUBDIR}/wp-content/plugins/regenerate-thumbnails/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordfence/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordfence/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/really-simple-captcha/
location ~ ^${WPSUBDIR}/wp-content/plugins/really-simple-captcha/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/ml-slider/
location ~ ^${WPSUBDIR}/wp-content/plugins/ml-slider/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/black-studio-tinymce-widget/
location ~ ^${WPSUBDIR}/wp-content/plugins/black-studio-tinymce-widget/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/disable-comments/
location ~ ^${WPSUBDIR}/wp-content/plugins/disable-comments/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/better-wp-security/
location ~ ^${WPSUBDIR}/wp-content/plugins/better-wp-security/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for http://wlmsocial.com/
location ~ ^${WPSUBDIR}/wp-content/plugins/wlm-social/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for mediagrid timthumb
location ~ ^${WPSUBDIR}/wp-content/plugins/media-grid/classes/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for webp-express
location ~ ^${WPSUBDIR}/wp-content/plugins/webp-express/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for wp-shortcode  
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-shortcode/ {
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/staticfiles.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Block PHP files in content directory.
location ~* ${WPSUBDIR}/wp-content/.*\.php\$ {
  deny all;
}

# Block PHP files in includes directory.
location ~* ${WPSUBDIR}/wp-includes/.*\.php\$ {
  deny all;
}

# Block PHP files in uploads, content, and includes directory.
location ~* ${WPSUBDIR}/(?:uploads|files|wp-content|wp-includes)/.*\.php\$ {
  deny all;
}

# Make sure files with the following extensions do not get loaded by nginx because nginx would display the source code, and these files can contain PASSWORDS!
location ~* \.(engine|inc|info|install|make|module|profile|test|po|sh|.*sql|theme|tpl(\.php)?|xtmpl)\$|^(\..*|Entries.*|Repository|Root|Tag|Template)\$|\.php_
{
return 444;
}

location ~* \.(tpl)\$
{
  deny all; 
}

#nocgi
location ~* \.(pl|cgi|py|sh|lua)\$ {
return 444;
}

#disallow
location ~* (w00tw00t) {
return 444;
}

location ~* ${WPSUBDIR}/(\.|wp-config\.php|wp-config\.txt|changelog\.txt|readme\.txt|readme\.html|license\.txt) { deny all; }
location ~* ${WPSUBDIR}/(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z|txt)\$ { deny all; }
EEF
    if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}" ]; then
      echo "generated: /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
    fi
    if [[ -f /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf && -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}" ]]; then
      diffcheck_wpsecure=$(diff -qu /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}" >/dev/null 2>&1; echo $?)
      if [ "$diffcheck_wpsecure" -ne '0' ]; then
        echo
        echo "differences between existing and newly generated wpsecure_${vhostname}.conf file"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
        diff -u /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
        echo
      else
        echo "for"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf"
        echo "versus"
        echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf${SUFFIX}"
        echo "no differences detected"
      fi
    fi
  else
    echo "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf not found"
  fi
}

case "$1" in
  all )
    {
    gen_include $2
    gen_tryfiles $2
    gen_map $2
    gen_wpsecure $2
    } 2>&1 | tee "${CENTMINLOGDIR}/wp-cache-enabler-generate-all-${DT}.log"
    echo
    echo "saved log: ${CENTMINLOGDIR}/wp-cache-enabler-generate-all-${DT}.log"
    ;;
  include )
    {
    gen_include $2
    } 2>&1 | tee "${CENTMINLOGDIR}/wp-cache-enabler-generate-include-${DT}.log"
    echo
    echo "saved log: ${CENTMINLOGDIR}/wp-cache-enabler-generate-include-${DT}.log"
    ;;
  tryfiles )
    {
    gen_tryfiles $2
    } 2>&1 | tee "${CENTMINLOGDIR}/wp-cache-enabler-generate-tryfiles-${DT}.log"
    echo
    echo "saved log: ${CENTMINLOGDIR}/wp-cache-enabler-generate-tryfiles-${DT}.log"
    ;;
  map )
    {
    gen_map $2
    } 2>&1 | tee "${CENTMINLOGDIR}/wp-cache-enabler-generate-map-${DT}.log"
    echo
    echo "saved log: ${CENTMINLOGDIR}/wp-cache-enabler-generate-map-${DT}.log"
    ;;
  wpsecure )
    {
    gen_wpsecure $2
    } 2>&1 | tee "${CENTMINLOGDIR}/wp-cache-enabler-generate-wpsecure-${DT}.log"
    echo
    echo "saved log: ${CENTMINLOGDIR}/wp-cache-enabler-generate-wpsecure-${DT}.log"
    ;;
  * )
    help_function
    ;;
esac