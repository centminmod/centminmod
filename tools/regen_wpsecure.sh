#!/bin/bash
#################################################################
# companion for centmin.sh menu option 22 to regenerate the latest
# version of wordpress include file wpsecure_yourdomain.com.conf
#################################################################
# usage:
# 
# if wordpress installed at web root i.e. domain.com/
# ./regen_wpsecure.sh -d domain.com -subdir no
# 
# if wordpress installed in subdirectory i.e. domain.com/blog
# ./regen_wpsecure.sh -d domain.com -subdir blog
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)
CENTMINLOGDIR='/root/centminlogs'

###############################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}
###############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi
#################################################################

while getopts ":d:s:" opt; do
    case "$opt" in
  d)
   vhostname=${OPTARG}
   RUN=y
  ;;
  s)
   SUBDIR=${OPTARG}
   if [[ -z "${SUBDIR}" || "${SUBDIR}" = 'no' ]]; then
    WPSUBDIR=""
   else
    WPSUBDIR="/${SUBDIR}"
   fi
   RUNB=y
  ;;
  *)
   usage
  ;;
     esac
done

usage() {
  cecho " Command Usage: " $boldyellow
  echo
  cecho " if wordpress installed at web root i.e. domain.com/ type:" $boldyellow
  echo
  cecho "   $0 -d domain.com -s no" $boldgreen
  echo
  cecho " if wordpress installed in subdirectory i.e. domain.com/blog type:" $boldyellow
  echo
  cecho "   $0 -d domain.com -s blog" $boldgreen
  echo
}

regen() {

  if [ ! -d "/usr/local/nginx/conf/wpincludes/${vhostname}" ]; then
    cecho "Creating /usr/local/nginx/conf/wpincludes/${vhostname} directory" $boldyellow
    echo
    mkdir -p "/usr/local/nginx/conf/wpincludes/${vhostname}"
  fi
  if [ -f "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" ]; then
    cecho "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf exists already" $boldyellow
    cecho "backing up and overwriting" $boldyellow
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}-${DT}.conf"
    echo
    cecho "backed up at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}-${DT}.conf" $boldyellow
    echo
    cecho "re-generating /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" $boldyellow
  else
    cecho "creating /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" $boldyellow
    echo
  fi

cat > "/usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" <<EEF
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

# allow AJAX requests in themes and plugins
location ~ ^${WPSUBDIR}/wp-admin/admin-ajax.php$ { allow all; include /usr/local/nginx/conf/php.conf; }

location ~* ^${WPSUBDIR}/(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z)\$ { deny all; }

location ~ ^${WPSUBDIR}/wp-content/uploads/sucuri { deny all; }

location ~ ^${WPSUBDIR}/wp-content/updraft { deny all; }

# Block nginx-help log from public viewing
location ~* ${WPSUBDIR}/wp-content/uploads/nginx-helper/ { deny all; }

location ~ ^${WPSUBDIR}/(wp-includes/js/tinymce/wp-tinymce.php) {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/onesignal-free-web-push-notifications//
location ~ ^${WPSUBDIR}/wp-content/plugins/onesignal-free-web-push-notifications/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sparkpost/
location ~ ^${WPSUBDIR}/wp-content/plugins/sparkpost/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendgrid-email-delivery-simplified/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendgrid-email-delivery-simplified/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailgun/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailgun/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailjet-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailjet-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/easy-wp-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/easy-wp-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/postman-smtp/
location ~ ^${WPSUBDIR}/wp-content/plugins/postman-smtp/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/sendpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/sendpress/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-mail-bank/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-mail-bank/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/theme-check/
location ~ ^${WPSUBDIR}/wp-content/plugins/theme-check/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/woocommerce-csvimport/
location ~ ^${WPSUBDIR}/wp-content/plugins/woocommerce-csvimport/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/advanced-custom-fields/
location ~ ^${WPSUBDIR}/wp-content/plugins/advanced-custom-fields/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/contact-form-7/
location ~ ^${WPSUBDIR}/wp-content/plugins/contact-form-7/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/duplicator/
location ~ ^${WPSUBDIR}/wp-content/plugins/duplicator/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/jetpack/
location ~ ^${WPSUBDIR}/wp-content/plugins/jetpack/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/nextgen-gallery/
location ~ ^${WPSUBDIR}/wp-content/plugins/nextgen-gallery/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/tinymce-advanced/
location ~ ^${WPSUBDIR}/wp-content/plugins/tinymce-advanced/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/updraftplus/
location ~ ^${WPSUBDIR}/wp-content/plugins/updraftplus/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-importer/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-importer/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordpress-seo/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordpress-seo/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wpclef/
location ~ ^${WPSUBDIR}/wp-content/plugins/wpclef/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/mailchimp-for-wp/
location ~ ^${WPSUBDIR}/wp-content/plugins/mailchimp-for-wp/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-optimize/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-optimize/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/si-contact-form/
location ~ ^${WPSUBDIR}/wp-content/plugins/si-contact-form/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/akismet/
location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/ {
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(form|akismet)\.(css|js)\$ { allow all; }
  location ~ ^${WPSUBDIR}/wp-content/plugins/akismet/(.+/)?(.+)\.(png|gif)\$ { allow all; }
  location ~* ${WPSUBDIR}/wp-content/plugins/akismet/akismet/.*\.php\$ {
    include /usr/local/nginx/conf/php.conf;
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
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/buddypress/
location ~ ^${WPSUBDIR}/wp-content/plugins/buddypress/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/all-in-one-seo-pack/
location ~ ^${WPSUBDIR}/wp-content/plugins/all-in-one-seo-pack/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/google-analytics-for-wordpress/
location ~ ^${WPSUBDIR}/wp-content/plugins/google-analytics-for-wordpress/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/regenerate-thumbnails/
location ~ ^${WPSUBDIR}/wp-content/plugins/regenerate-thumbnails/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wordfence/
location ~ ^${WPSUBDIR}/wp-content/plugins/wordfence/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/really-simple-captcha/
location ~ ^${WPSUBDIR}/wp-content/plugins/really-simple-captcha/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/wp-pagenavi/
location ~ ^${WPSUBDIR}/wp-content/plugins/wp-pagenavi/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/ml-slider/
location ~ ^${WPSUBDIR}/wp-content/plugins/ml-slider/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/black-studio-tinymce-widget/
location ~ ^${WPSUBDIR}/wp-content/plugins/black-studio-tinymce-widget/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/disable-comments/
location ~ ^${WPSUBDIR}/wp-content/plugins/disable-comments/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for https://wordpress.org/plugins/better-wp-security/
location ~ ^${WPSUBDIR}/wp-content/plugins/better-wp-security/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for http://wlmsocial.com/
location ~ ^${WPSUBDIR}/wp-content/plugins/wlm-social/ {
  include /usr/local/nginx/conf/php.conf;
  # below include file needs to be manually created at that path and to be uncommented
  # by removing the hash # in front of below line to take effect. This wpwhitelist_common.conf
  # allows you to add commonly shared settings to all wp plugin location matches which
  # whitelist php processing access at /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf
  #include /usr/local/nginx/conf/wpincludes/${vhostname}/wpwhitelist_common.conf;
}

# Whitelist Exception for mediagrid timthumb
location ~ ^${WPSUBDIR}/wp-content/plugins/media-grid/classes/ {
  include /usr/local/nginx/conf/php.conf;
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

#nocgi
location ~* \.(pl|cgi|py|sh|lua)\$ {
return 444;
}

#disallow
location ~* (w00tw00t) {
return 444;
}

location ~* ${WPSUBDIR}/(\.|wp-config\.php|wp-config\.txt|changelog\.txt|readme\.txt|readme\.html|license\.txt) { deny all; }
EEF

  echo
  cecho "re-generated /usr/local/nginx/conf/wpincludes/${vhostname}/wpsecure_${vhostname}.conf" $boldyellow
  echo
}

if [[ "$RUN" = [yY] && "$RUNB" = [yY] ]]; then
  {
    regen
  } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${DT}_regen_wpsecure.log
else
  usage
fi