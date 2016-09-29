#!/bin/bash
################################################################
# centminmod.com maintenance mode written by George Liu (eva2000)
# https://community.centminmod.com/threads/sitestatus-maintenance-mode.5599/
################################################################
CHECK='n'
CHECKURL='http://newdomain1.com'
################################################################


vhostsetup() {
     # include file for /usr/local/nginx/conf/php.conf and 
     # vhost location specific context usage
     if [ ! -f /usr/local/nginx/conf/503include-only.conf ]; then
cat > "/usr/local/nginx/conf/503include-only.conf" <<DDD
if (\$maintenance = 1) { return 503; }
DDD
     echo "created /usr/local/nginx/conf/503include-only.conf"
     fi

     # include file for main vhost domain server {} context
     # usage
     if [ ! -f /usr/local/nginx/conf/503include-main.conf ]; then
cat > "/usr/local/nginx/conf/503include-main.conf" <<DED
    # only uncomment if you do not need to exclude images or js
    # from 503 redirect rewrites
     #include /usr/local/nginx/conf/503include-only.conf;
     error_page 503 @maintenance;
     location @maintenance {
          #if (\$maintenance = 1) {
          rewrite ^ /maintenance.html break;
          #ry_files /maintenance.html =503;
          #}
     }
DED
     echo "created /usr/local/nginx/conf/503include-main.conf"
     fi
}

setupconfig() {
     if [ ! -f /usr/local/nginx/conf/sitestatus.conf ]; then
cat > "/usr/local/nginx/conf/sitestatus.conf" <<EOF
default 0;
EOF
     echo "created /usr/local/nginx/conf/sitestatus.conf"
     fi

     # include file for nginx.conf http{} context usage
     if [ ! -f /usr/local/nginx/conf/maintenance.conf ]; then
cat > "/usr/local/nginx/conf/maintenance.conf" <<FFF
     # IPs you can whitelist from maintenance mode
     geo \$maint_whitelist {
          include /usr/local/nginx/conf/sitestatus.conf;
          127.0.0.1 0;
          #YOURIPADDRESS 1;
     }

     map \$http_host\$uri \$exclude_url {
          default                                                0;
          "~^newdomain1.com/js/jquery.fittext.js"                1;
          "~^newdomain1.com/blog/js/jquery.fittext.js"           1;
     }

     map \$maint_whitelist\$exclude_url \$maintenance {
          default        1;
          10             1;
          11             1;
          00             0;
          01             0;
     }
FFF
     echo "created /usr/local/nginx/conf/maintenance.conf"
     fi

     MCONF_CHECK=$(grep 'maintenance.conf' /usr/local/nginx/conf/nginx.conf)
     if [[ -z "$MCONF_CHECK" ]]; then
          sed -i 's/include \/usr\/local\/nginx\/conf\/geoip.conf;/include \/usr\/local\/nginx\/conf\/geoip.conf;\ninclude \/usr\/local\/nginx\/conf\/maintenance.conf;\n/g' /usr/local/nginx/conf/nginx.conf
     fi
}

checkstatus() {
     if [[ "$CHECK" = [yY] ]]; then
          curl -I $CHECKURL
     fi
}

soff() {
     ########################################################
     #disable maintainence mode
     if [ -f /usr/local/nginx/conf/sitestatus.conf ]; then
          sed -i 's|1|0|' /usr/local/nginx/conf/sitestatus.conf
          service nginx restart
          checkstatus
     fi
     }

son() {
     ########################################################
     #enable maintainence mode
     if [ -f /usr/local/nginx/conf/sitestatus.conf ]; then
          sed -i 's|0|1|' /usr/local/nginx/conf/sitestatus.conf
          service nginx restart
          checkstatus
     fi
     }

case "$1" in
          on)
          soff
          ;;
          off)
          son 
          ;;
          setup)
          setupconfig
          vhostsetup
          ;;
          * )
          echo "$0 {on|off|setup}"
          ;;
esac