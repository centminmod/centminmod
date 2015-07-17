wpinstall() {
    if [ ! -f /usr/bin/wp ]; then
      cecho "------------------------------------------------------------" $boldgreen
      cecho "Installing wpcli.sh" $boldyellow
      cecho "------------------------------------------------------------" $boldgreen
      if [ -d "${SCRIPT_DIR}/addons" ]; then
        cd ${SCRIPT_DIR}/addons      
      fi
      chmod +x wpcli.sh
      ./wpcli.sh install
    else
      cecho "------------------------------------------------------------" $boldgreen
      cecho "Update wp-cli tool" $boldyellow
      cecho "------------------------------------------------------------" $boldgreen
      wp cli update --allow-root
    fi
}

dbsetup() {
  SALT=$(openssl rand -base64 12 | cut -c1-12 | sed -e s'|/||')
  DBN=$RANDOM
  DBNB=$RANDOM
  DBNC=$RANDOM
  DBND=$RANDOM
  DB="wp${DBN}db_${DBND}"
  DBUSER="wpdb${DBND}u${DBNB}"
  DBPASS="wpdb${SALT}p${DBNC}"
  mysqladmin create $DB
  mysql -e "CREATE USER $DBUSER@'localhost' IDENTIFIED BY '$DBPASS';"
  mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES ON ${DB}.* TO ${DBUSER}@'localhost'; FLUSH PRIVILEGES;"
}

wpacctsetup() {
PUREUSER=nginx
PUREGROUP=nginx
CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
pureftpinstall

wpinstall
  WPSALT=$(openssl rand -base64 18 | cut -c1-18 | sed -e 's|/||' -e 's|+||')
  WPSALTB=$(openssl rand -base64 11 | cut -c1-11 | sed -e 's|/||' -e 's|+||')
  WPN=$RANDOM
  WPNB=$RANDOM
  WPADMINUSER="z${WPSALT}wp${WPNB}"
  WPADMINPASS="z${WPSALTB}wps${WPN}"
 
if [ ! -d /root/tools ]; then
  mkdir -p /root/tools
fi

echo
cecho "---------------------------------------------" $boldyellow
cecho "Setup full Nginx vhost + Wordpress + WP Super Cache" $boldgreen
cecho "---------------------------------------------" $boldyellow
echo

read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

TESTVHOST=$(echo $vhostname | grep '\/')
while [[ "$TESTVHOST" ]]; do
  echo "!! only domain.com or subdomain.domain.com supported !!"
  echo "   subdirectory is not supported right now"
  read -ep "re-enter vhost domain name you want to add (without www. prefix): " vhostname
  TESTVHOST=$(echo $vhostname | grep '\/')
  echo
done

read -ep "Enter email address for Wordpress Installation: " WPADMINEMAIL

TESTEMAIL=$(echo "${WPADMINEMAIL}" |  grep '^[a-zA-Z0-9._%+-]*@[a-zA-Z0-9]*[\.[a-zA-Z0-9]*]*[a-zA-Z0-9]$')
# echo "$TESTEMAIL"
while [[ "$TESTEMAIL" = "" ]]; do
  echo
  echo "!! make sure email address is valid and typed correctly !!"
  read -ep "Enter email address for Wordpress Installation: " WPADMINEMAIL
  TESTEMAIL=$(echo "${WPADMINEMAIL}" |  grep '^[a-zA-Z0-9._%+-]*@[a-zA-Z0-9]*[\.[a-zA-Z0-9]*]*[a-zA-Z0-9]$')
  echo
done

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  if [ ! -f /usr/sbin/cracklib-check ]; then
    yum -y -q install cracklib
  fi
  if [ ! -f /usr/bin/pwgen ]; then
    yum -y -q install pwgen
  fi  
  read -ep "Create FTP username for vhost domain (enter username): " ftpuser
  read -ep "Do you want to auto generate FTP password (recommended) [y/n]: " autogenpass

  if [[ "$autogenpass" = [yY] ]]; then
    ftppass=$(pwgen -1cnys 21)
  else
    read -ep "Create FTP password for $ftpuser (enter password): " ftppass
  
    # simple password strength check
    # utilise http://cracklib.sourceforge.net/ too
    CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
    okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    while [[ "$okay" != "OK" ]]; do
      echo "!! password strength not strong enough !! "
      echo "!! do not use common dictionary words !! "
      echo "!! do not use short passwords !! "
      echo "!! do not use simplistic passwords !! "
      echo
      read -ep "re-enter FTP password for $ftpuser (enter password): " ftppass
      CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
      okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    done
  fi # autogenpass
  echo
  echo "FTP username you entered: $ftpuser"
  if [[ "$autogenpass" = [yY] ]]; then
    echo "FTP password auto generated: $ftppass"
  else
    echo "FTP password you entered: $ftppass"    
  fi
fi

echo ""

if [ ! -d /home/nginx/domains/$vhostname ]; then

dbsetup

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo ${ftppass} ; echo ${ftppass} ) | pure-pw useradd $ftpuser -u $PUREUSER -g $PUREGROUP -d /home/nginx/domains/$vhostname
  pure-pw mkdb
fi

# where index.html was

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/$vhostname/public

chown -R nginx:nginx "/home/nginx/domains/$vhostname"
find "/home/nginx/domains/$vhostname" -type d -exec chmod g+s {} \;

# wp-login.php password protection
if [[ -f /usr/local/nginx/conf/htpasswd.sh && ! -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
  HTWPLOGINSALT=$(openssl rand -base64 13 | cut -c1-13)
  HTWPLOGINSALTB=$(openssl rand -base64 19 | cut -c1-19)
  HTWPLOGIN=$RANDOM
  HTWPLOGINB=$RANDOM
  HTUSER="u${HTWPLOGINSALT}x${HTWPLOGIN}"
  HTPASS="p${HTWPLOGINSALTB}y${HTWPLOGIN}"
  echo "/usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS"
  /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin $HTUSER $HTPASS
fi

# rate limit setup
WPRATECHECK=$(grep 'zone=xwplogin' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$WPRATECHECK" ]]; then
  sed -i 's/http {/http { \nlimit_req_zone $binary_remote_addr zone=xwplogin:16m rate=40r\/m;\n/g' /usr/local/nginx/conf/nginx.conf
fi

\cp -f /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/php-wpsc.conf
sed -i "s|fastcgi_param  SERVER_NAME        \$server_name;|fastcgi_param  SERVER_NAME        \$http_host;|" /usr/local/nginx/conf/php-wpsc.conf

# Setting up Nginx vhost mapping for Wordpress
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf" <<END
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
server_name ${vhostname} www.${vhostname};

# ngx_pagespeed &amp; ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

# limit_conn limit_per_ip 16;
# ssi on;

access_log /home/nginx/domains/${vhostname}/log/access.log combined buffer=256k flush=60s;
error_log /home/nginx/domains/${vhostname}/log/error.log;

root /home/nginx/domains/${vhostname}/public;

  # prevent access to ./directories and files
  location ~ (?:^|/)\. {
   deny all;
  }

include /usr/local/nginx/conf/wpsupercache_${vhostname}.conf;

location / {
# Enables directory listings when index file not found
#autoindex on;

try_files /wp-content/cache/supercache/\$http_host/\$cache_uri/index.html \$uri \$uri/ /index.php?q=\$uri&\$args;

}

location ~* /(wp-login\.php) {
    limit_req zone=xwplogin burst=1 nodelay;
    #limit_conn xwpconlimit 30;
    auth_basic "Private";
    auth_basic_user_file /home/nginx/domains/$vhostname/htpasswd_wplogin;    
    include /usr/local/nginx/conf/php-wpsc.conf;
}

location ~* /(xmlrpc\.php) {
    limit_req zone=xwplogin burst=2 nodelay;
    #limit_conn xwpconlimit 30;
    include /usr/local/nginx/conf/php-wpsc.conf;
}

include /usr/local/nginx/conf/wpsecure_${vhostname}.conf;
include /usr/local/nginx/conf/php-wpsc.conf;
include /usr/local/nginx/conf/staticfiles.conf;
include /usr/local/nginx/conf/drop.conf;
#include /usr/local/nginx/conf/errorpage.conf;
}
END

cat > "/usr/local/nginx/conf/wpsecure_${vhostname}.conf" <<EEF
# Deny access to any files with a .php extension in the uploads directory
# Works in sub-directory installs and also in multisite network
location ~* /(?:uploads|files)/.*\.php\$ {
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
location ~* (roundcube|webdav|smtp|http\:|soap|w00tw00t) {
return 444;
}

location ~ /(\.|wp-config\.php|wp-config\.txt|readme\.html|license\.txt) { deny all; }
EEF

cat > "/usr/local/nginx/conf/wpsupercache_${vhostname}.conf" <<EFF
set \$cache_uri \$request_uri;

if (\$request_method = POST) { set \$cache_uri 'null cache'; }

if (\$query_string != "") { set \$cache_uri 'null cache'; }

if (\$request_uri ~* "/(\?add-to-cart=|cart|my-account|checkout|addons|wp-admin/.*|xmlrpc\.php|wp-.*\.php|index\.php|feed/|sitemap(_index)?\.xml|[a-z0-9_-]+-sitemap([0-9]+)?\.xml)") { set \$cache_uri 'null cache'; }

if (\$http_cookie ~* "comment_author|wordpress_[a-f0-9]+|wp-postpass|wordpress_logged_in") { set \$cache_uri 'null cache'; }
EFF

########### WP Super Cache Start ##############################
# only proceed in creating vhost if VHOSTNAME directory exist
if [[ -d "/home/nginx/domains/${vhostname}/public" ]]; then

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Setup Wordpress + Super Cache for $vhostname" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen

cd /home/nginx/domains/${vhostname}/public
 
wp core download --allow-root
 
wp core config --dbname=$DB --dbuser=$DBUSER --dbpass=$DBPASS --allow-root
 
NEWPREFIX=$(echo $RANDOM)
sed -i "s/'wp_';/'${NEWPREFIX}_';/g" wp-config.php

sed -i "/define('WPLANG', '');/ a\
/** Enable core updates for minor releases (default) **/\ndefine( 'WP_AUTO_UPDATE_CORE', 'minor' );\ndefine('WP_POST_REVISIONS', 10 );\ndefine('EMPTY_TRASH_DAYS', 10 );\ndefine('WP_CRON_LOCK_TIMEOUT', 60 );\
" wp-config.php

wp core install --url=http://${vhostname} --title=${vhostname} --admin_email=${WPADMINEMAIL} --admin_password=${WPADMINPASS} --admin_name=${WPADMINUSER} --allow-root

# change admin userid from 1 to a random 6 digit number
# WP_PREFIX=$(wp eval 'echo $GLOBALS["table_prefix"];')
WUID=$(echo $RANDOM$RANDOM |cut -c1-6)
# wp db query "UPDATE ${WP_PREFIX}wp_users SET ID=${WUID} WHERE ID=1; UPDATE ${WP_PREFIX}wp_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root
wp db query "UPDATE wp_users SET ID=${WUID} WHERE ID=1; UPDATE wp_usermeta SET user_id=${WUID} WHERE user_id=1" --allow-root

cecho "------------------------------------------------------------" $boldgreen
wp theme install responsive --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
 
chown nginx:nginx /home/nginx/domains/${vhostname}/public
chown -R nginx:nginx /home/nginx/domains/${vhostname}/public

cd /home/nginx/domains/${vhostname}/public

chmod 0770 wp-content
chmod 0400 readme.html

# installed + activated by default
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-super-cache --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-super-cache-clear-cache-menu --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-widget-cache --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install autoptimize --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install rocket-lazy-load --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-security-scan --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install sucuri-scanner --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install disable-xml-rpc --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install limit-login-attempts --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-updates-notifier --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install no-longer-in-directory --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
# wp plugin install google-sitemap-generator --activate --allow-root
# cecho "------------------------------------------------------------" $boldgreen
wp plugin install wp-optimize --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
# wp plugin install wp-smushit --activate --allow-root
# cecho "------------------------------------------------------------" $boldgreen
wp plugin install tpc-memory-usage --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install gtmetrix-for-wordpress --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install p3-profiler --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install wordpress-seo --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install updraftplus --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install google-analytics-for-wordpress --activate --allow-root
cecho "------------------------------------------------------------" $boldgreen

# installed but disabled by default
cecho "------------------------------------------------------------" $boldgreen
wp plugin install query-monitor --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install go-newrelic --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install db-cache-reloaded-fix --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install google-authenticator --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install addthis-smart-layers --allow-root
cecho "------------------------------------------------------------" $boldgreen
wp plugin install search-regex --allow-root
cecho "------------------------------------------------------------" $boldgreen
# update all plugins to make sure they are latest available
wp plugin update --all --allow-root
echo
wp plugin status --allow-root
cecho "------------------------------------------------------------" $boldgreen

chown -R nginx:nginx /home/nginx/domains/${vhostname}/public/wp-content/plugins/

# write permissions for log
chmod 0660 wp-content/plugins/tpc-memory-usage/logs/tpcmem.log

# fix tcpmem.css incorrect reference to images
sed -i 's|(images\/|(..\/images\/|g' wp-content/plugins/tpc-memory-usage/css/tpcmem.css

\cp -af wp-content/plugins/wp-super-cache/wp-cache-config-sample.php wp-content/wp-cache-config.php
\cp -af wp-content/plugins/wp-super-cache/advanced-cache.php wp-content/advanced-cache.php
mkdir -p wp-content/cache/
mkdir -p wp-content/cache/supercache/
chown -R nginx:nginx wp-content/
chmod -R 0770 wp-content/cache/
chmod 0750 wp-content
umask 022
fi
########### WP Super Cache End ##############################

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Created uninstall script" $boldyellow
  cecho "/root/tools/wp_uninstall_${vhostname}.sh" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen

cat > "/root/tools/wp_uninstall_${vhostname}.sh" <<END
#/bin/bash
rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.conf
rm -rf /home/nginx/domains/${vhostname}
rm -rf /usr/local/nginx/conf/wpsecure_${vhostname}.conf
rm -rf /usr/local/nginx/conf/wpsupercache_${vhostname}.conf
rm -rf /root/tools/wp_updater_${vhostname}.sh
crontab -l > cronjoblist
sed -i "/wp_updater_${vhostname}.sh/d" cronjoblist
crontab cronjoblist
rm -rf cronjoblist
END

chmod 0700 /root/tools/wp_uninstall_${vhostname}.sh

  cecho "------------------------------------------------------------" $boldgreen
  cecho "Created wp_updater_${vhostname}.sh script" $boldyellow
  cecho "/root/tools/wp_updater_${vhostname}.sh" $boldyellow
  cecho "------------------------------------------------------------" $boldgreen

cat > "/root/tools/wp_updater_${vhostname}.sh" <<ENDA
#!/bin/bash
PATH=/usr/lib64/ccache:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin:/root/bin
EMAIL=$WPADMINEMAIL

{
cd /home/nginx/domains/${vhostname}/public
echo "/home/nginx/domains/${vhostname}/public"
/usr/bin/wp cli update --allow-root
/usr/bin/wp plugin status --allow-root
/usr/bin/wp plugin update --all --allow-root
} 2>&1 | mail -s "Wordpress WP-CLI Auto Update \$(date)" \$EMAIL
ENDA

chmod 0700 /root/tools/wp_updater_${vhostname}.sh

if [[ -z "$(crontab -l 2>&1 | grep wp_updater_${vhostname}.sh)" ]]; then
    # generate random number of seconds to delay cron start
    # making sure wp_updater for several wordpress nginx installs
    # do not run at very same time during cron scheduling
    DELAY=$(echo ${RANDOM:0:3})
    crontab -l > cronjoblist
    echo "0 */8 * * * sleep ${DELAY}s ;/root/tools/wp_updater_${vhostname}.sh 2>/dev/null" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

echo 
cecho "---------------------------------------------" $boldyellow
cmservice nginx reload
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

if [ -f /tmp/setupwp.log ]; then
  rm -rf /tmp/setupwp.log
fi

{
echo 
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
cecho "---------------------------------------------" $boldyellow
cecho "FTP hostname : $CNIP" $boldwhite
cecho "FTP port : 21" $boldwhite
cecho "FTP mode : FTP (explicit SSL)" $boldwhite
cecho "FTP Passive (PASV) : ensure is checked/enabled" $boldwhite
cecho "FTP username created for $vhostname : $ftpuser" $boldwhite
cecho "FTP password created for $vhostname : $ftppass" $boldwhite
fi
cecho "---------------------------------------------" $boldyellow
echo
cecho "------------------------------------------------------------" $boldgreen
cecho "${vhostname} created" $boldyellow
cecho "nginx vhost: /usr/local/nginx/conf/conf.d/${vhostname}.conf" $boldyellow
cecho "${vhostname} public web root: /home/nginx/domains/${vhostname}/public" $boldyellow
cecho "${vhostname} log directory: /home/nginx/domains/$vhostname/log" $boldyellow
echo
cecho "------------------------------------------------------------" $boldgreen
cecho "SSH commands to uninstall created Wordpress install and Nginx vhost:" $boldyellow
cecho "  /root/tools/wp_uninstall_${vhostname}.sh" $boldyellow
cecho "------------------------------------------------------------" $boldgreen
echo
cecho "------------------------------------------------------------" $boldgreen
cecho "Wordpress Auto Updater created at:" $boldyellow
cecho "  /root/tools/wp_updater_${vhostname}.sh" $boldyellow
cecho "cronjob set for every 8 hours update (3x times per day)" $boldyellow
cecho "------------------------------------------------------------" $boldgreen
echo
cecho "Wordpress domain: $vhostname" $boldyellow
cecho "Wordpress DB Name: $DB" $boldyellow
cecho "Wordpress DB User: $DBUSER" $boldyellow
cecho "Wordpress DB Pass: $DBPASS" $boldyellow
cecho "Wordpress Admin User ID: ${WUID}" $boldyellow
cecho "Wordpress Admin User: $WPADMINUSER" $boldyellow
cecho "Wordpress Admin Pass: $WPADMINPASS" $boldyellow
cecho "Wordpress Admin Email: $WPADMINEMAIL" $boldyellow

if [[ -f /usr/local/nginx/conf/htpasswd.sh && -f /home/nginx/domains/$vhostname/htpasswd_wplogin ]]; then
  echo  
  cecho "Wordpress wp-login.php password protection info:" $boldyellow
  cecho "wp-login.php protection file /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "wp-login.php protection Username: $HTUSER" $boldyellow
  cecho "wp-login.php protection Password: $HTPASS" $boldyellow
  echo
  cecho "Resetting wp-login.php protection:" $boldyellow
  cecho "Step 1. remove protection file at /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "     rm -rf /home/nginx/domains/$vhostname/htpasswd_wplogin" $boldyellow
  cecho "Step 2. run command:" $boldyellow
  cecho "     /usr/local/nginx/conf/htpasswd.sh create /home/nginx/domains/$vhostname/htpasswd_wplogin YOURUSERNAME YOURPASSWORD" $boldyellow
  cecho "Step 3. restart Nginx + PHP-FPM services" $boldyellow
  cecho "     nprestart" $boldyellow
fi

echo
cecho "------------------------------------------------------------" $boldgreen
cecho "To complete setup:" $boldyellow
cecho "1. Enable Permalinks (DO NOT use links with .html extensions for performance reasons) i.e. /%post_id%/%postname%/
2. Settings Menu > Super Cache > Easy tab and enable it by checking Caching On (Recommended) and hit Update Status
3. Advanced tab & check Use mod_rewrite serve cache files & Don’t cache pages with GET parameters and Known User. 
   (Recommended) & hit Update Status
4. WP Security Menu > Settings > Check All except Enable Live Traffic tool and hit Update settings
5. Settings Menu > Updates Notifier and setup your notify email address and cronjob (save and test button to check)
6. Settings Mnenu > Autoptimize and check Optimize HTML, JavaScript and CSS options (show advanced settings)
7. Settings Menu > Limit Login Attempts and configure as desired or leave as defaults
8. Sucuri Security Menu and top left click Generate API key for your domain/email and configure your Settings tab
9. WP-Optimize Menu and configure as needed
10. Memory Usage Menu > Settings and adjust accordingly
11. GTmetrix Menu > setup and register your GTmetrix Account and API Key
12. go-newrelic plugin installed but not activated read https://wordpress.org/plugins/go-newrelic/installation/
13. Tools > P3 Plugin Profiler > Start Scan to profile all your plugins
14. Plugins > Query Monitor is disabled by default, enable to check MySQL query stats
15. Plugins > DB Cache Reloaded disabled by default unsure if works with Wordpress 4.x ?
16. Appearance > Theme Options (Responsive theme) > Home Page nav bar > Uncheck Overrides Wordpress front page option
17. Seo Menu (Yoast SEO) > configure accordingly
18. Settings > UpdraftPlus Backups > Settings set file/database backup intervals & optional backup to remote storage
19. Analytics > Settings > configure your Google Analytics UA Code" $boldyellow
cecho "------------------------------------------------------------" $boldgreen
} 2>&1 | tee /tmp/setupwp.log
cat /tmp/setupwp.log | perl -pe 's/\x1b.*?[mGKH]//g' | mail -s "${vhostname} Wordpress Installed `date`" $WPADMINEMAIL
rm -rf /tmp/setupwp.log

echo
cecho "---------------------------------------------" $boldyellow

else

echo ""
cecho "---------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "---------------------------------------------" $boldyellow
echo ""

fi

}