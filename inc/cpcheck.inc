jetpack_ip_whitelist_cronsetup() {
  if [[ -f /usr/local/src/centminmod/tools/jetpackips.sh && ! -f /usr/local/nginx/conf/jetpack_whitelist_ip.conf ]]; then
      # block xml-rpc.php completely with whitelist exception for
      # jetpack wordpress plugin's IP addresses https://jetpack.com/support/hosting-faq/
      mkdir -p /etc/centminmod/cronjobs/
      crontab -l > /etc/centminmod/cronjobs/before_jetpack_ip_whitelist_cronjoblist
      sed -i '/jetpackips/d' /etc/centminmod/cronjobs/before_jetpack_ip_whitelist_cronjoblist
      echo "11 */12 * * * /usr/local/src/centminmod/tools/jetpackips.sh >/dev/null 2>&1" >> /etc/centminmod/cronjobs/before_jetpack_ip_whitelist_cronjoblist
      crontab /etc/centminmod/cronjobs/before_jetpack_ip_whitelist_cronjoblist
      /usr/local/src/centminmod/tools/jetpackips.sh >/dev/null 2>&1
  fi
}

update_wpce_map() {
  if [ -f /usr/local/nginx/conf/wpcacheenabler_map.conf ]; then
    # fix improper escapped entries
    if [[ "$(grep '\"' /usr/local/nginx/conf/wpcacheenabler_map.conf)" ]]; then
      sed -i 's|\\"|"|g' /usr/local/nginx/conf/wpcacheenabler_map.conf
      if [[ "$(ps aufx | grep nginx | grep -v grep | grep nginx  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        service nginx reload >/dev/null 2>&1
      fi
    fi
  fi
}

update_cache_enabler_schemefix() {
  if [[ -d /usr/local/nginx/conf/wpincludes && $(grep 'cache-enabler/\${http_host}\${cache_uri}index' /usr/local/nginx/conf/wpincludes/*/wpcacheenabler_*.conf >/dev/null 2>&1; echo $?) -eq '0' ]]; then
    touch "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_cache_enabler_scheme_fix.log"
    {
    echo "#########################################################################"
    echo "Starting Automatic Wordpress Cache Enabler Advanced Cache Config Update"
    echo "https://community.centminmod.com/threads/20256/"
    echo
    } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_cache_enabler_scheme_fix.log"
    getce_includes=$(ls /usr/local/nginx/conf/wpincludes/*/wpcacheenabler_*.conf)
    ce_count=0
    for f in $getce_includes; do
      ce_chkold=$(grep 'cache-enabler/\${http_host}\${cache_uri}index' "$f")
      ce_chknew=$(grep 'cache-enabler/\${http_host}\${cache_uri}\${scheme}-index' "$f")
      ce_search_chkold=$(grep 'cache-enabler/\${http_host}\${cache_uri_search}index' "$f")
      ce_search_chknew=$(grep 'cache-enabler/\${http_host}\${cache_uri_search}\${scheme}-index' "$f")
      if [[ "$ce_chkold" && ! "$ce_chknew" ]] || [[ "$ce_search_chkold" ]]; then
          {
          ce_count=$((${ce_count} + 1))
          getce_domain=$(echo $f | awk -F '/' '{print $7}')
          getce_plugin_phpfile=$(find /home/nginx/domains/${getce_domain}/public -type f -name "*cache-enabler.php" | grep 'plugins/cache-enabler/cache-enabler.php')
          if [ -f "${getce_plugin_phpfile}" ]; then
              getce_ver=$(awk -F ': ' '/Version/ {print $2}' "${getce_plugin_phpfile}")
              getce_verf=$(awk -F ': ' '/Version/ {print $2}' "${getce_plugin_phpfile}" | awk -F '.' '{print $1"00"$2"00"$3}')
          fi
          echo
          echo "cache enabler plugin $getce_ver ($getce_verf)"
          echo "cache enabler v1.4.0+ cache format update for $f";
          echo "backup $f"
          echo "to ${f}-autobackup"
          \cp -af "$f" "${f}-autobackup"
          sed -i 's|cache-enabler/\${http_host}\${cache_uri}index|cache-enabler/\${http_host}\${cache_uri}\${scheme}-index|g' $f;
          sed -i 's|cache-enabler/\${http_host}\${cache_uri_search}index.html|cache-enabler/\${http_host}\${cache_uri_search}\${scheme}-index.html|g' $f;
          sed -i 's|cache-enabler/\${http_host\}${check_surl}index.html|cache-enabler/\${http_host\}${check_surl}\${scheme}-index.html|g' $f;
          grep '\${scheme}' "$f"
          } 2>&1 >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_cache_enabler_scheme_fix.log"
      fi
      echo "ce_count=$ce_count updated: $getce_domain"
      if [[ "$ce_count" -gt '1' && "$(ps aufx | grep nginx | grep -v grep | grep nginx  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        service nginx reload >/dev/null 2>&1
      fi
    done
    {
    echo
    echo "Completed Automatic Wordpress Cache Enabler Advanced Cache Config Update"
    echo "saved log at ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_cache_enabler_scheme_fix.log"
    echo "#########################################################################"
    } 2>&1 | tee -a "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_cache_enabler_scheme_fix.log"
  fi
}

fix_nasm() {
  if [[ -f /etc/yum.repos.d/nasm.repo && -f /usr/bin/nasm && "$(yum versionlock list | grep -o nasm)" != 'nasm' ]]; then
    yum -q versionlock nasm >/dev/null 2>&1
  fi
}

imagemagick_heif() {
  if [[ -f /etc/yum.repos.d/remi.repo && "$IMAGEMAGICK_HEIF" = [yY] && "$CENTOS_SEVEN" = '7' && ! "$(rpm -qa ImageMagick-heic | grep -o 'ImageMagick-heic')" ]]; then
    echo
    echo "Install ImageMagick-heic one time only"
    echo "https://blog.remirepo.net/post/2019/05/22/HEIC-HEIF-images-support"
    libheif_install
    yum -q -y install ImageMagick-heic --enablerepo=remi
    echo "ImageMagick-heic installed"
    if [[ "$(rpm -qa ImageMagick-heic | grep -o 'ImageMagick-heic')" ]]; then
      yum -q versionlock delete ImageMagick-heic
      yum -q versionlock ImageMagick-heic
    fi
  elif [[ -f /etc/yum.repos.d/remi.repo && "$IMAGEMAGICK_HEIF" = [yY] && "$CENTOS_SIX" = '6' && ! "$(rpm -qa ImageMagick-heic | grep -o 'ImageMagick-heic')" ]]; then
    echo
    echo "Install ImageMagick-heic one time only"
    echo "https://blog.remirepo.net/post/2019/05/22/HEIC-HEIF-images-support"
    libheif_install
    yum -q -y install ImageMagick-heic --enablerepo=remi
    echo "ImageMagick-heic installed"
    if [[ "$(rpm -qa ImageMagick-heic | grep -o 'ImageMagick-heic')" ]]; then
      yum -q versionlock delete ImageMagick-heic
      yum -q versionlock ImageMagick-heic
    fi
  fi
}

phpmyadmin_composer_fix() {
  # https://github.com/centminmod/phpmyadmin/commit/fb0fe48d799fcc60b6a807c9dc9fb64412e127a2
  if [ -f /root/tools/phpmyadmin_update.sh ]; then
    sed -i 's/^php composer.phar update --no-dev/echo y | php composer.phar update --no-dev/g' /root/tools/phpmyadmin_update.sh
  fi
}

phpmyadmin_updater_fix() {
  if [ -f /root/tools/phpmyadmin_update.sh ]; then
    if [[ ! "$(grep 'git stash' /root/tools/phpmyadmin_update.sh)" ]]; then
      # https://community.centminmod.com/threads/phpmyadmin-doesnt-auto-update.20537/#post-86677
      sed -i 's|^echo "git pull"|echo "git stash"\ngit stash\necho "git pull"|' /root/tools/phpmyadmin_update.sh
    fi
  fi
}

initial_pip_updates() {
  initialpip=$1
  if [[ -z "$SKIP_INITIAL_PIP_UPDATES" || "$SKIP_INITIAL_PIP_UPDATES" = 'n' ]] && [ -f /usr/local/src/centminmod/tools/pip-updates.sh ]; then
    # skip initial_pip_updates    
    /usr/local/src/centminmod/tools/pip-updates.sh update "$initialpip"
  fi
}

update_webp_inc() {
  if [[ -f /usr/local/nginx/conf/webp.conf && "$(grep -q 'webpok' /usr/local/nginx/conf/webp.conf; echo $?)" -ne '0' ]] && [[ -f /usr/local/src/centminmod/config/nginx/webp.conf && "$(grep -q 'webpok' /usr/local/src/centminmod/config/nginx/webp.conf; echo $?)" -eq '0' ]]; then
    \cp -af /usr/local/src/centminmod/config/nginx/webp.conf /usr/local/nginx/conf/webp.conf
    chown nginx:nginx /usr/local/nginx/conf/webp.conf
    service nginx restart >/dev/null 2>&1
  fi
  if [[ ! "$(grep webp.conf /usr/local/nginx/conf/nginx.conf)" ]] && [[ -f /usr/local/nginx/conf/webp.conf && -f /usr/local/src/centminmod/config/nginx/webp.conf ]]; then
    sed -i 's|\/usr\/local\/nginx\/conf\/geoip.conf;|\/usr\/local\/nginx\/conf\/geoip.conf;\ninclude \/usr\/local\/nginx\/conf\/webp.conf;|g' /usr/local/nginx/conf/nginx.conf
    service nginx restart >/dev/null 2>&1
  elif [[ ! "$(grep webp.conf /usr/local/nginx/conf/nginx.conf)" ]] && [[ ! -f /usr/local/nginx/conf/webp.conf && -f /usr/local/src/centminmod/config/nginx/webp.conf ]]; then
    \cp -af /usr/local/src/centminmod/config/nginx/webp.conf /usr/local/nginx/conf/webp.conf
    chown nginx:nginx /usr/local/nginx/conf/webp.conf
    sed -i 's|\/usr\/local\/nginx\/conf\/geoip.conf;|\/usr\/local\/nginx\/conf\/geoip.conf;\ninclude \/usr\/local\/nginx\/conf\/webp.conf;|g' /usr/local/nginx/conf/nginx.conf
    service nginx restart >/dev/null 2>&1
  fi
}

cfauthorigin_cronsetup() {
  if [[ -z "$(crontab -l 2>&1 | grep '\/usr\/local\/src\/centminmod\/tools\/cf-authenticated-origin-cert-update.sh')" && -f "/usr/local/src/centminmod/tools/cf-authenticated-origin-cert-update.sh" ]]; then
    mkdir -p /etc/centminmod/cronjobs/
    crontab -l > /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    sed -i '/cf-authenticated-origin-cert-update.sh/d' /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    echo "0 0 * * 5 /usr/local/src/centminmod/tools/cf-authenticated-origin-cert-update.sh update >/dev/null 2>&1" >> /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    crontab /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
  fi
}

mytop_setup() {
  if [[ $(rpm -q mytop >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install mytop
      echo -e "host=localhost\ndb=mysql\ndelay=2\nidle=0" > /root/.mytop
      if [ ! -f /usr/local/bin/mytop ]; then
          wget -q -4 https://gist.github.com/centminmod/14419caaba4f33ffdb240b526f46e8b5/raw/mytop.pl -O /usr/local/bin/mytop
          chmod +x /usr/local/bin/mytop
          echo -e "host=localhost\ndb=mysql\ndelay=2\nidle=0\nfullqueries=1" > /root/.mytop
      fi
  fi
}

pureftpd_symlink_fix() {
  # workaround for too many levels of symbolic links
  # messages on openvz vpses
  if [[ "$CENTOS_SEVEN" = '7' && -f /proc/user_beancounters && ! -f /etc/systemd/system/pure-ftpd.service.d/pidfile.conf ]]; then
    echo
    # echo "create /etc/systemd/system/pure-ftpd.service.d/pidfile.conf"
    mkdir -p /etc/systemd/system/pure-ftpd.service.d
    echo '[Service]' > /etc/systemd/system/pure-ftpd.service.d/pidfile.conf
    echo 'PIDFile=/run/pure-ftpd.pid' >> /etc/systemd/system/pure-ftpd.service.d/pidfile.conf
    systemctl daemon-reload >/dev/null 2>&1
    #cat /etc/systemd/system/pure-ftpd.service.d/pidfile.conf
  fi
}

symlink_setup_mariadb() {
  if [[ "$CENTOS_SEVEN" = '7' && "$MYSQLSERVICE_DISABLED" = [nN] && ! -h /etc/systemd/system/multi-user.target.wants/mariadb.service ]]; then
    # ensure MariaDB service startup symlink is setup
    systemctl enable mariadb >/dev/null 2>&1
  fi
}

symlink_setup_emailnotify() {
  if [[ -f "${SCRIPT_DIR}/tools/emailnotify.sh" && ! -h /usr/local/bin/emailnotify ]]; then
    ln -s "${SCRIPT_DIR}/tools/emailnotify.sh" /usr/local/bin/emailnotify >/dev/null 2>&1
  fi
}

add_versionlocks() {
  if [[ ! "$(yum versionlock list | grep 'LibRaw')" ]]; then
    yum -q versionlock LibRaw >/dev/null 2>&1
  fi
}

opt_sslcache() {
  opt_sslcache_silent=$1
  # dynamically tune nginx ssl_session_cache based on system detected memory installed
  TOTALMEM_SSLCACHE=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  if [[ "$TOTALMEM_SSLCACHE" -gt '500000' && "$TOTALMEM_SSLCACHE" -le '800000' ]]; then
    # (20x4000)/(60x60) = 22.22 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=20
  elif [[ "$TOTALMEM_SSLCACHE" -gt '800000' && "$TOTALMEM_SSLCACHE" -le '1000000' ]]; then
    # (40x4000)/(60x60) = 44.44 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=40
  elif [[ "$TOTALMEM_SSLCACHE" -gt '1000000' && "$TOTALMEM_SSLCACHE" -le '1880000' ]]; then
    # (60x4000)/(60x60) = 66.67 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=60
  elif [[ "$TOTALMEM_SSLCACHE" -gt '1880000' && "$TOTALMEM_SSLCACHE" -le '2890000' ]]; then
    # (80x4000)/(60x60) = 88.88 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=80
  elif [[ "$TOTALMEM_SSLCACHE" -gt '2890000' && "$TOTALMEM_SSLCACHE" -le '3890000' ]]; then
    # (150x4000)/(60x60) = 166.67 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=150
  elif [[ "$TOTALMEM_SSLCACHE" -gt '3890000' && "$TOTALMEM_SSLCACHE" -le '7800000' ]]; then
    # (300x4000)/(60x60) = 333.33 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=300
  elif [[ "$TOTALMEM_SSLCACHE" -gt '7800000' && "$TOTALMEM_SSLCACHE" -le '15600000' ]]; then
    # (500x4000)/(60x60) = 555.55 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=500
  elif [[ "$TOTALMEM_SSLCACHE" -gt '15600000' && "$TOTALMEM_SSLCACHE" -le '23600000' ]]; then
    # (1000x4000)/(60x60) = 1,111.11 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=1000
  elif [[ "$TOTALMEM_SSLCACHE" -gt '23600000' ]]; then
    # (2000x4000)/(60x60) = 2,222.22 sessions/sec on 60 min ssl session cache timeout
    sslcachesize=2000
  fi
  if [[ "$NGINX_SSLCACHE_ALLOWOVERRIDE" = [yY] && ! -f /proc/user_beancounters && -f /usr/local/nginx/conf/ssl_include.conf ]]; then
    # create new nginx shared ssl cache name SSLv2 so not to conflict with any existing
    # shared ssl cache from existing ssl cache name = SSL
    sed -i "s|shared:SSL:.*|shared:SSLv2:${sslcachesize}m;|" /usr/local/nginx/conf/ssl_include.conf
    if [[ "$opt_sslcache_silent" != 'silent' ]]; then
      service nginx restart >/dev/null 2>&1
    fi
  fi
}

less_console_verbose() {
  if [ ! -f /proc/user_beancounters ]; then
    if [[ -f /etc/sysctl.d/101-sysctl.conf && ! "$(grep 'kernel.printk' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo 'kernel.printk=4 1 1 7' >> /etc/sysctl.d/101-sysctl.conf
      sysctl --system >/dev/null 2>&1
    elif [[ -f /etc/sysctl.conf && ! -f /etc/sysctl.d/101-sysctl.conf && ! "$(grep 'kernel.printk' /etc/sysctl.conf)" ]]; then
      echo 'kernel.printk=4 1 1 7' >> /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
}

mysql_charset_default() {
  # if you set SET_DEFAULT_MYSQLCHARSET='utf8mb4' in persistent config file
  # /etc/centminmod/custom_config.inc prior to initial Centmin Mod install
  # this routine allows changing MariaDB MySQL /etc/my.cnf default characterset
  # & collation from utf8 defaults to utf8mb4 defaults 
  # https://community.centminmod.com/posts/76010/
  if [[ -f /etc/my.cnf && "$SET_DEFAULT_MYSQLCHARSET" = 'utf8mb4' ]]; then
    sed -i 's|default-character-set=.*|default-character-set=utf8mb4|g' /etc/my.cnf
    sed -i 's|character-set-server=.*|character-set-server=utf8mb4|g' /etc/my.cnf
  elif [[ -f /etc/my.cnf && "$SET_DEFAULT_MYSQLCHARSET" = 'utf8' ]]; then
    sed -i 's|default-character-set=.*|default-character-set=utf8|g' /etc/my.cnf
    sed -i 's|character-set-server=.*|character-set-server=utf8|g' /etc/my.cnf
  fi
}

disable_avahi() {
  if [[ "$(pgrep avahi)" && -f /usr/lib/systemd/system/avahi-daemon.service ]]; then
        systemctl disable avahi-daemon.socket avahi-daemon.service >/dev/null 2>&1
        systemctl mask avahi-daemon.socket avahi-daemon.service >/dev/null 2>&1
        systemctl stop avahi-daemon.socket avahi-daemon.service >/dev/null 2>&1
        # systemctl status avahi-daemon.socket avahi-daemon.service >/dev/null 2>&1
  elif [[ "$(pgrep avahi)" && -f /etc/rc.d/init.d/avahi-daemon ]]; then
        service avahi-daemon stop >/dev/null 2>&1
        chkconfig avahi-daemon off >/dev/null 2>&1
        # service avahi-daemon status >/dev/null 2>&1
  fi
}

dft_phpupstream() {
  if [ ! -f /usr/local/nginx/conf/default_phpupstream.conf ]; then
cat > "/usr/local/nginx/conf/default_phpupstream.conf"<<EOF
upstream dft_php {
  zone dftphp_zone 128k;
  server 127.0.0.1:9000;
  keepalive 2;
}
EOF
  fi

  DFT_SETUPPHPUPSTREAM_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/default_phpupstream.conf' /usr/local/nginx/conf/nginx.conf)
  if [[ -z "$DFT_SETUPPHPUPSTREAM_INCLUDECHECK" ]]; then
    # echo
    # echo "include file /usr/local/nginx/conf/default_phpupstream.conf add to nginx.conf"
    sed -i 's|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;|\/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;\ninclude \/usr\/local\/nginx\/conf\/default_phpupstream.conf;|g' /usr/local/nginx/conf/nginx.conf
    service nginx restart >/dev/null 2>&1
    service nginx php-fpm >/dev/null 2>&1
  fi
}

redetect_tlsonethree() {
  # persistent config file /etc/centminmod/custom_config.inc
  # variable OPENSSL_TLSONETHREE controls whether OpenSSL 1.1.1
  # installed Nginx enables and supports TLSv1.3 or not
  # OPENSSL_TLSONETHREE='y' enabled
  # OPENSSL_TLSONETHREE='n' disabled
  # https://community.centminmod.com/posts/63238/
  if [[ "$DISABLE_TLSONEZERO_PROTOCOL" = [yY] ]]; then
    TLSONEONEPROTOCOL_OPT=""
  else
    TLSONEONEPROTOCOL_OPT='TLSv1 '
  fi
  if [ -f /usr/local/nginx/conf/ssl-include.conf ]; then
    if [[ "$OPENSSL_TLSONETHREE" = [yY] ]]; then
      # Nginx supports TLS 1.3 ssl_protocols directive
      if [[ "$SSL_PROTOCOL_MODERN" = [yY] ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w "ssl_protocols  TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;" /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w "ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2;" /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.1 TLSv1.2;' /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.2;' /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl-include.conf
      fi
    else
      if [[ "$SSL_PROTOCOL_MODERN" = [yY] ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w "ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;" /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;' /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2;|" /usr/local/nginx/conf/ssl-include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.2 TLSv1.3;' /usr/local/nginx/conf/ssl-include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2;|" /usr/local/nginx/conf/ssl-include.conf
      fi
    fi
  elif [ -f /usr/local/nginx/conf/ssl_include.conf ]; then
    if [[ "$OPENSSL_TLSONETHREE" = [yY] ]]; then
      # Nginx supports TLS 1.3 ssl_protocols directive
      if [[ "$SSL_PROTOCOL_MODERN" = [yY] ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w "ssl_protocols  TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;" /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w "ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2;" /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.1 TLSv1.2;' /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.2;' /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2 TLSv1.3;|" /usr/local/nginx/conf/ssl_include.conf
      fi
    else
      if [[ "$SSL_PROTOCOL_MODERN" = [yY] ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w "ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2 TLSv1.3;" /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  ${TLSONEONEPROTOCOL_OPT}TLSv1.1 TLSv1.2;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.1 TLSv1.2 TLSv1.3;' /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.1 TLSv1.2;|" /usr/local/nginx/conf/ssl_include.conf
      elif [[ "$(grep -w 'ssl_protocols  TLSv1.2 TLSv1.3;' /usr/local/nginx/conf/ssl_include.conf)" ]]; then
        sed -i "s|ssl_protocols .*|ssl_protocols  TLSv1.2;|" /usr/local/nginx/conf/ssl_include.conf
      fi
    fi
  fi
}

pzcat_install() {
  if [[ "$(nproc)" -ge '2' && ! -f /usr/bin/pzcat && -f /usr/bin/zcat && -f /usr/bin/pigz ]]; then 
    \cp -af /usr/bin/zcat /usr/bin/pzcat
    sed -i 's|exec gzip -cd|exec pigz -cd|' /usr/bin/pzcat
  fi
}

csf_portflood() {
  # override default CSF Firewall PORTFLOOD values
  # PORTFLOOD_COUNT=20
  # PORTFLOOD_INTERVAL=300
  if [[ "$CSFPORTFLOOD_OVERRIDE" = [yY] && -f /etc/csf/csf.conf ]]; then
    # ensure only overriding PORTFLOOD Centmin Mod setup defaults
    # if end user made custom changes to PORTFLOOD, DO NOT override those
    if [[ "$(grep 'PORTFLOOD = "21;tcp;20;300"' /etc/csf/csf.conf)" ]]; then
      csf --profile backup portflood_adjustment >/dev/null 2>&1
      sed -i "s|PORTFLOOD = .*|PORTFLOOD = \"21;tcp;${PORTFLOOD_COUNT};${PORTFLOOD_INTERVAL}\"|" /etc/csf/csf.conf
      csf -ra >/dev/null 2>&1
    fi
  fi
}

csf_loadalert() {
    # custom high cpu load trigger email stats
    if [[ ! -f /etc/csf/load.sh && -f "${SCRIPT_DIR}/config/csf/load.sh" ]]; then
        \cp -fa "${SCRIPT_DIR}/config/csf/load.sh" /etc/csf/load.sh
        csf --profile backup cmm-before-ptload-action >/dev/null 2>&1
        sed -i "s|^PT_LOAD_ACTION = \"\"|PT_LOAD_ACTION = \"\/etc\/csf\/load.sh\"|" /etc/csf/csf.conf
        sed -i "s/^PT_LOAD_LEVEL .*/PT_LOAD_LEVEL = \"$(nproc)\"/g" /etc/csf/csf.conf
    fi
    if [[ -f /etc/csf/load.sh && -f "${SCRIPT_DIR}/config/csf/load.sh" ]]; then
      if [ ! "$(grep 'EMAILNOTIFY_SES' /etc/csf/load.sh)" ]; then
        # updated load.sh with AWS SES supported tools/emailnotify.sh routine
        \cp -fa "${SCRIPT_DIR}/config/csf/load.sh" /etc/csf/load.sh
      fi
    fi
}

disable_varnishrepo() {
  if [[ "$(rpm -ql varnish | grep -o 'not installed' >/dev/null 2>&1; echo $?)" -eq '0' && -f "$VARNISH_REPOFILE" ]]; then
    # disable varnish cache packagecloud.io yum repo
    # https://community.centminmod.com/threads/add-varnishrepo_disable-variable.14556/
    yum-config-manager --disable varnishcache_varnish41 varnishcache_varnish41-source >/dev/null 2>&1
  fi
}

csf_smtpports() {
  if [[ "$(egrep '^TCP_OUT|^TCP6_OUT' /etc/csf/csf.conf | grep -wq '2525'; echo $?)" -ne '0' ]]; then
    sed -i "s/TCP_OUT = \"/TCP_OUT = \"2525,/g" /etc/csf/csf.conf
    sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"2525,/g" /etc/csf/csf.conf
    csf -ra >/dev/null 2>&1
  fi
}

check_memcachedflush() {
  if [[ -f /usr/bin/memflush && ! -f /usr/bin/memcachedflush ]]; then
    echo "memflush --quiet --servers=localhost:11211" >/usr/bin/memcachedflush ; chmod 700 /usr/bin/memcachedflush
  fi
}

ngxmaster_openfiles() {
  # raise nginx process' open files limit from 1024
  if [[ -f /etc/init.d/nginx && ! "$(grep -w 'ulimit -n 524288' /etc/init.d/nginx)" ]]; then
    sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nulimit -n 524288\n/' /etc/init.d/nginx
    if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
      systemctl daemon-reload -q
    fi
    service nginx restart >/dev/null 2>&1
  fi
}

ngx_gzipbuffersfix() {
  # raising default gzip_buffers setting to accommodate very very
  # large javascript js file libraries that can be as large as 5+ MB
  if [[ -f /usr/local/nginx/conf/nginx.conf && "$(grep -w 'gzip_buffers      32 8k;' /usr/local/nginx/conf/nginx.conf)" ]]; then
    sed -i 's|gzip_buffers .*|gzip_buffers      1024 8k;|' /usr/local/nginx/conf/nginx.conf
    service nginx restart >/dev/null 2>&1
  fi
}

ngx_brotlibuffersfix() {
  # raising default brotli_buffers setting to accommodate very very
  # large javascript js file libraries that can be as large as 5+ MB
  if [[ -f /usr/local/nginx/conf/brotli_inc.conf && "$(grep -w 'brotli_buffers 32 8k;' /usr/local/nginx/conf/brotli_inc.conf)" ]]; then
    sed -i 's|brotli_buffers .*|brotli_buffers 1024 8k;|' /usr/local/nginx/conf/brotli_inc.conf
    service nginx restart >/dev/null 2>&1
  fi
}

python_centossix_sclfix() {
  if [[ "$CENTOX_SIX" -eq '6' ]]; then
    if [[ -f /etc/yum.repos.d/CentOS-SCLo-scl.repo && ! "$(grep 'exclude =' /etc/yum.repos.d/CentOS-SCLo-scl.repo)" ]]; then
      yum-config-manager -q --save --setopt=centos-sclo-sclo.exclude=python27* >/dev/null 2>&1
    fi
    if [[ -f /etc/yum.repos.d/CentOS-SCLo-scl-rh.repo && ! "$(grep 'exclude =' /etc/yum.repos.d/CentOS-SCLo-scl-rh.repo)" ]]; then
      yum-config-manager -q --save --setopt=centos-sclo-rh.exclude=python27* >/dev/null 2>&1
    fi
  fi
}

pip_updates() {
  mkdir -p /home/piptmp
  chmod 1777 /home/piptmp
  export TMPDIR=/home/piptmp
  if [[ "$(date "+%u")" -eq '3' || "$(date "+%u")" -eq '4' || "$(date "+%u")" -eq '6' ]]; then
    # for glances and psutil as glances is installed via outdated EPEL
    # yum repo but there's a new version available
    if [[ ! -f /usr/bin/python-config ]]; then
      $YUMDNFBIN -q -y install python-devel
    fi
    if [[ -d /usr/local/lib/python3.6/site-packages/pip && "$(pip --version | grep -o 'python3.6')" && ! "$(rpm -qa python3-devel | grep -o 'python3-devel')" ]]; then
      yum -q -y install python3-devel
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && ! -d /usr/lib/python2.7/site-packages/urllib3/packages/ssl_match_hostname ]]; then
        # $YUMDNFBIN -q -y install python-urllib3 >/dev/null 2>&1
        # yum -q -y versionlock python-urllib3 >/dev/null 2>&1
        if [[ ! "$(grep 'python-urllib3' /etc/yum.conf)" ]]; then
          NEW_PYTHONURLLIB_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-urllib3")
          sed -i "s|^exclude=.*|$NEW_PYTHONURLLIB_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade urllib3
    elif [[ "$CENTOS_SEVEN" -eq '7' && -d /usr/lib/python2.7/site-packages/urllib3/packages/ssl_match_hostname ]]; then
        CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list -o --format columns)
        CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list --format columns)
        CHECK_PIPURLLIBUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o urllib3)
        if [[ "$(rpm -qa python-urllib3 | grep -o python-urllib3)" = 'python-urllib3' ]]; then
          #yum -q -y remove python-urllib3 >/dev/null 2>&1
          #yum versionlock delete python-urllib3 >/dev/null 2>&1
          yum versionlock python-urllib3 cloud-init python-requests >/dev/null 2>&1
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq urllib3
        fi
        if [[ ! "$(grep 'python-urllib3' /etc/yum.conf)" ]]; then
          NEW_PYTHONURLLIB_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-urllib3")
          sed -i "s|^exclude=.*|$NEW_PYTHONURLLIB_EXCLUDES|" /etc/yum.conf
        fi
        if [[ ! "$(grep 'cloud-init' /etc/yum.conf)" ]]; then
          CLOUDINIT_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) cloud-init")
          sed -i "s|^exclude=.*|$CLOUDINIT_EXCLUDES|" /etc/yum.conf
        fi
        if [[ ! "$(grep 'python-requests' /etc/yum.conf)" ]]; then
          PYTHONREQUESTS_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-requests")
          sed -i "s|^exclude=.*|$PYTHONREQUESTS_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade urllib3
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && ! -f /usr/bin/pip ]] || [[ "$CENTOS_SIX" -eq '6' &&  ! -f /usr/bin/pip2.7 ]]; then
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        $YUMDNFBIN -q -y install python2-pip >/dev/null 2>&1
        yum -q -y versionlock python2-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip
      else
        $YUMDNFBIN -q -y install python-pip >/dev/null 2>&1
        yum -q -y versionlock python-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip
        if [[ "$CENTOS_SIX" -eq '6' && -f "${SCRIPT_DIR}/addons/python27_install.sh" && ! -f /usr/bin/pip2.7 ]]; then
          "${SCRIPT_DIR}/addons/python27_install.sh" install
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade setuptools
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade psutil
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
          echo
          echo "CentOS 6 python 2.7 compatibility updates completed"
          echo
        fi
      fi
    elif [[ "$CENTOS_SIX" -eq '6' && -f /usr/bin/pip2.7 ]]; then
      CHECK_PIPVER=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip show pip 2>&1 | awk '/^Version: / {print $2}' | sed -e 's|\.|0|g')
      if [[ "$CHECK_PIPVER" -lt '1801' ]]; then
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
      fi
    elif [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/bin/pip ]]; then
      CHECK_PIPVER=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip show pip 2>&1 | awk '/^Version: / {print $2}' | sed -e 's|\.||g')
      if [[ "$CHECK_PIPVER" -lt '901' ]]; then
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip
      fi
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/bin/pip && -f /usr/bin/python-config ]]; then
      CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list -o --format columns)
      CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list --format columns)
      CHECK_PIPUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o pip)
      CHECK_PSUTILUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o psutil)
      CHECK_GLANCESUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -io glances)
      CHECK_PSUTILINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -o psutil)
      CHECK_GLANCEINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -io glances)
      if [[ "$(yum versionlock list | grep 'python2-pip')" ]]; then
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PIPUPDATE" = 'pip' ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip
        yum -q -y versionlock python2-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PSUTILUPDATE" = 'psutil' || ! "$CHECK_PSUTILINSTALL" ]]; then
        export CC='gcc'
        if [[ "$(rpm -qa python2-psutil | grep -o python2-psutil)" = 'python2-psutil' ]]; then
          yum -q -y remove python2-psutil >/dev/null 2>&1
          # yum -q -y versionlock python2-psutil >/dev/null 2>&1
          if [[ ! "$(grep 'python2-psutil' /etc/yum.conf)" ]]; then
            NEW_PYTHONPSUTIL_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-psutil")
            sed -i "s|^exclude=.*|$NEW_PYTHONPSUTIL_EXCLUDES|" /etc/yum.conf
          fi
        fi
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade setuptools
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade psutil
      fi
      if [[ "$CHECK_GLANCESUPDATE" = 'Glances' || ! "$CHECK_GLANCEINSTALL" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
      fi
    fi
    if [[ "$CENTOS_SIX" -eq '6' && -f /usr/bin/pip2.7 && -f /usr/bin/python2.7-config ]]; then
      CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 list -o --format columns)
      CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 list --format columns)
      CHECK_PIPUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o pip)
      CHECK_PSUTILUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o psutil)
      CHECK_GLANCESUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -io glances)
      CHECK_PSUTILINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -o psutil)
      CHECK_GLANCEINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -io glances)
      if [[ "$(yum versionlock list | grep 'python-pip')" ]]; then
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PIPUPDATE" = 'pip' ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
        yum -q -y versionlock python-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PSUTILUPDATE" = 'psutil' || ! "$CHECK_PSUTILINSTALL" ]]; then
        export CC='gcc'
        if [[ "$(rpm -qa python-psutil | grep -o python-psutil)" = 'python-psutil' ]]; then
          yum -q -y remove python-psutil >/dev/null 2>&1
          # yum -q -y versionlock python-psutil >/dev/null 2>&1
          if [[ ! "$(grep 'python-psutil' /etc/yum.conf)" ]]; then
            NEW_PYTHONPSUTIL_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-psutil")
            sed -i "s|^exclude=.*|$NEW_PYTHONPSUTIL_EXCLUDES|" /etc/yum.conf
          fi
        fi
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade setuptools
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade psutil
      fi
      if [[ "$CHECK_GLANCESUPDATE" = 'Glances' || ! "$CHECK_GLANCEINSTALL" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
      fi
    fi
  fi
  unset TMPDIR
  rm -rf /home/piptmp
}

fix_phperrorlogperm() {
  if [[ -f /var/log/php-fpm/www-php.error.log && "$(stat -c %a /var/log/php-fpm/www-php.error.log | grep -qo '666'; echo $?)" -ne '0' ]]; then
    chmod 666 /var/log/php-fpm/www-php.error.log
  fi
}

march_hostcheck() {
  # smarter detection of cloud vps providers who run a mix of different
  # cpu models to prevent issues when you switch host nodes with differing 
  # cpu models causing seg faults for nginx and php-fpm
  VPS_ASNCHECK=$(curl -4s https://ipinfo.io/org)
  if [[ "$(echo "${VPS_ASNCHECK}" | egrep -i -o 'upcloud|linode|digitalocean|vultr|choopa|hetzner|frantech|hostus|ramnode|amazon|google|microsoft|mammoth|contabo|netcup')" && "$MARCH_TARGETNATIVE_ALWAYS" != [yY] ]]; then
    MARCH_TARGETNATIVE='n'
    NGX_GSPLITDWARF='n'
    PHP_GSPLITDWARF='n'
    if [[ -f /etc/centminmod/custom_config.inc && ! "$(grep 'MARCH_TARGETNATIVE' /etc/centminmod/custom_config.inc)" ]]; then
      echo -e "\nMARCH_TARGETNATIVE='n'" >> /etc/centminmod/custom_config.inc
    elif [[ ! -f /etc/centminmod/custom_config.inc && "$INITIALINSTALL" = [yY] ]]; then
      touch /etc/centminmod/custom_config.inc
      echo -e "\nMARCH_TARGETNATIVE='n'" >> /etc/centminmod/custom_config.inc
    elif [[ ! -f /etc/centminmod/custom_config.inc && "$INITIALINSTALL" != [yY] ]]; then
      touch /etc/centminmod/custom_config.inc
      echo -e "\nMARCH_TARGETNATIVE='n'" >> /etc/centminmod/custom_config.inc
    fi
  fi
}

nginxlargefile_fix() {
  if [[ -f /usr/local/nginx/conf/nginx.conf ]]; then
    nginxfix_count=0
    if [[ "$(grep -w 'client_max_body_size 10m' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_max_body_size 10m|client_max_body_size 200m|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=1
    fi
    if [[ "$(grep -w 'client_max_body_size 50m' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_max_body_size 50m|client_max_body_size 200m|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=1
    fi
    if [[ "$(grep -w 'client_max_body_size 200m' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_max_body_size 200m|client_max_body_size 1024m|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=1
    fi
    if [[ "$(grep -w 'output_buffers   8 256k;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|output_buffers   8 256k;|output_buffers   1 512k;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'output_buffers   1 512K;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|output_buffers   1 512K;|output_buffers   2 32k;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'send_timeout     10s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|send_timeout     10s;|send_timeout     60s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'send_timeout     15s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|send_timeout     15s;|send_timeout     60s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'send_timeout     30s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|send_timeout     30s;|send_timeout     60s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'client_body_timeout 60s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_body_timeout 60s;|client_body_timeout 10s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'client_body_timeout 10s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_body_timeout 10s;|client_body_timeout 30s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'client_header_timeout  5s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_header_timeout  5s;|client_header_timeout  10s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    if [[ "$(grep -w 'client_header_timeout  10s;' /usr/local/nginx/conf/nginx.conf)" ]]; then
      sed -i 's|client_header_timeout  10s;|client_header_timeout  30s;|' /usr/local/nginx/conf/nginx.conf
      nginxfix_count=$(($nginxfix_count+1))
    fi
    HOME_DISKTYPE=$(df -T /home | tail -1 | awk '{print $2}')
    # check if directio_alignment directive exists in nginx.conf and auto add if needed
    if [[ ! "$(grep -w 'directio_alignment' /usr/local/nginx/conf/nginx.conf)" ]]; then
      if [[ "$HOME_DISKTYPE" = 'ext4' ]]; then
        sed -i 's|^ directio  4m;| directio  4m;\n directio_alignment 512;|' /usr/local/nginx/conf/nginx.conf
      elif [[ "$HOME_DISKTYPE" = 'xfs' ]]; then
        sed -i 's|^ directio  4m;| directio  4m;\n directio_alignment 4096;|' /usr/local/nginx/conf/nginx.conf
      else
        sed -i 's|^ directio  4m;| directio  4m;\n directio_alignment 512;|' /usr/local/nginx/conf/nginx.conf
      fi
      nginxfix_count=$(($nginxfix_count+1))
    fi
    # check if filesystem is ext4 or xfs and set directio_alignment appropriately
    if [[ "$HOME_DISKTYPE" = 'ext4' ]]; then
      if [[ "$(grep -w 'directio_alignment 4096;' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's|directio_alignment 4096;|directio_alignment 512;|' /usr/local/nginx/conf/nginx.conf
        nginxfix_count=$(($nginxfix_count+1))
      fi
    elif [[ "$HOME_DISKTYPE" = 'xfs' ]]; then
      if [[ "$(grep -w 'directio_alignment 512;' /usr/local/nginx/conf/nginx.conf)" ]]; then
        sed -i 's|directio_alignment 512;|directio_alignment 4096;|' /usr/local/nginx/conf/nginx.conf
        nginxfix_count=$(($nginxfix_count+1))
      fi
    fi
    if [[ "$AUTOTUNE_CLIENTMAXBODY" = [yY] ]]; then
      # auto calculate client_max_body_size size and auto tune in nginx.conf to largest file
      # detected in /home/nginx/domains/*/public vhost public web roots and is greater than
      # the size of the existing nginx.conf set client_max_body_size directive only
      GET_CLIENTMAXBODY=$(awk '/client_max_body_size/ {print $2}' /usr/local/nginx/conf/nginx.conf | grep -v '#')
      if [[ "$(echo $GET_CLIENTMAXBODY | grep 'm')" ]]; then
        GET_CLIENTMAXBODYSIZE=$(($(awk '/client_max_body_size/ {print $2}' /usr/local/nginx/conf/nginx.conf | grep -v '#' | sed -e 's|m;||'| head -n1)))
        CLIENTMAXBODYSIZEMB="$GET_CLIENTMAXBODYSIZE"
        CLIENTMAXBODYSIZEKB=$(($GET_CLIENTMAXBODYSIZE * 1024))
      elif [[ "$(echo $GET_CLIENTMAXBODY | grep 'k')" ]]; then
        GET_CLIENTMAXBODYSIZE=$(($(awk '/client_max_body_size/ {print $2}' /usr/local/nginx/conf/nginx.conf | grep -v '#' | sed -e 's|k;||'| head -n1)))
        CLIENTMAXBODYSIZEMB=$(($GET_CLIENTMAXBODYSIZE / 1024))
        CLIENTMAXBODYSIZEKB="$GET_CLIENTMAXBODYSIZE"
      fi
      if [[ -z "$(find /home/nginx/domains/ -maxdepth 1 -type d \( ! -wholename /home/nginx/domains/ \))" || -z "$CLIENTMAXBODYSIZEMB" ]]; then
        CHECKMAXFILESIZE_VHOSTS=0
      else
        CHECKMAXFILESIZE_VHOSTS=$(find /home/nginx/domains/*/public -maxdepth 4 -size +"${CLIENTMAXBODYSIZEMB}"M -type f -printf '%s %p\n'|sort -nr|awk 'NR == 1 {print $1}')
      fi
      if [[ "$CHECKMAXFILESIZE_VHOSTS" -ge '1' ]]; then
        MAXFILESIZE_VHOSTS=$(echo $(($CHECKMAXFILESIZE_VHOSTS * 1024 / 1000 / 1024)))
        CLIENTMAXBODYSIZEKB_THRESHOLD='5242880'
        if [[ "$MAXFILESIZE_VHOSTS" -ge "$CLIENTMAXBODYSIZEKB" && "$MAXFILESIZE_VHOSTS" -le "$CLIENTMAXBODYSIZEKB_THRESHOLD" ]]; then
            SET_MAXBODYSIZE=$(($MAXFILESIZE_VHOSTS/1024))
            sed -i "s|client_max_body_size .*|client_max_body_size ${SET_MAXBODYSIZE}m;|" /usr/local/nginx/conf/nginx.conf
            nginxfix_count=$(($nginxfix_count+1))
        fi
      fi
    fi
    if [[ "$nginxfix_count" -ge '1' ]]; then
      service nginx restart >/dev/null 2>&1
    fi
  fi
}

disablelogs() {
  # access and error log in nginx.conf are not needed so disable them
  if [[ -f /usr/local/nginx/conf/nginx.conf && -z "$(grep 'access_log  off' /usr/local/nginx/conf/nginx.conf)" ]]; then
    sed -i 's|^access_log .*|access_log  off;|' /usr/local/nginx/conf/nginx.conf
    sed -i 's|^#access_log .*|access_log  off;|' /usr/local/nginx/conf/nginx.conf
  elif [[ -f /usr/local/nginx/conf/nginx.conf && "$(grep '#error_log' /usr/local/nginx/conf/nginx.conf)" ]]; then
    sed -i 's|^access_log .*|access_log  off;|' /usr/local/nginx/conf/nginx.conf
    sed -i 's|^#access_log .*|access_log  off;|' /usr/local/nginx/conf/nginx.conf
    sed -i 's|^#error_log   logs\/error.log|error_log   logs\/error.log|' /usr/local/nginx/conf/nginx.conf
  fi
}

varnishfour_setup() {
  if [[ "$VARNISHREPO_DISABLE" = [nN] ]]; then
    if [ -f /etc/yum.repos.d/varnish-4.1.repo ]; then
        rm -rf /etc/yum.repos.d/varnish-4.1.repo
        # yum -q clean all
    fi
    if [ -f /etc/yum.repos.d/varnish-4.1.OLD ]; then
        rm -rf /etc/yum.repos.d/varnish-4.1.OLD
    fi
    if [ ! -f "$VARNISH_REPOFILE" ]; then
      os=centos
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
          dist=7
      else
          dist=6
      fi
      yum_repo_path=$VARNISH_REPOFILE
      yum_repo_config_url="https://packagecloud.io/install/repositories/varnishcache/varnish41/config_file.repo?os=${os}&dist=${dist}&source=script"
      if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_SIX" -eq '6' ]]; then
          curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${yum_repo_config_url}" | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
          VARNISHFOUR_CURLCHECK=$?
          if [[ "$VARNISHFOUR_CURLCHECK" = '0' ]]; then
            echo "update varnish cache 4.1 yum repo config"
            curl -${ipv_forceopt}sSf "${yum_repo_config_url}" > $yum_repo_path
            # yum -q makecache -y --disablerepo='*' --enablerepo='varnishcache_varnish41'
            if [ -f /etc/yum.repos.d/varnish-4.1.repo ]; then
                rm -rf /etc/yum.repos.d/varnish-4.1.repo
            fi
            if [ -f /etc/yum.repos.d/varnish-4.1.OLD ]; then
                rm -rf /etc/yum.repos.d/varnish-4.1.OLD
            fi
            if [[ "$(rpm -ql varnish-release | grep -o 'not installed' >/dev/null 2>&1; echo $?)" -eq '1' ]]; then
              yum -q -y update varnish-release
            else
              yum repolist varnishcache_varnish41
            fi
            echo
            echo "updated varnish cache 4.1 yum repo config"
          else
            echo
            echo "curl error: skip varnish cache 4.1 yum repo config"
          fi
      fi
    fi
  fi
}

libc_fix() {
  # https://community.centminmod.com/posts/52555/
  if [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" = 'libc-client-2007f-16.el7.x86_64' ]]; then
    yum -y -q install yum-plugin-versionlock
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
  elif [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget -q https://centminmod.com/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget -q https://centminmod.com/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y -q remove libc-client
    yum -y -q localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y -q install yum-plugin-versionlock
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
    cd "$INIT_DIR"
   elif [[ "$CENTOS_SEVEN" -eq '7' && -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget -q https://centminmod.com/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget -q https://centminmod.com/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock delete libc-client uw-imap-devel -q >/dev/null 2>&1
    yum -y -q remove libc-client
    yum -y -q localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
    cd "$INIT_DIR" 
  fi
}

cityfan_fix() {
  if [[ -f /etc/yum.repos.d/city-fan.org.repo && ! $(grep libtidy /etc/yum.repos.d/city-fan.org.repo) ]]; then
    sed -i 's|^gpgkey=.*|&\nexcludes=libtidy libtidy-devel|' /etc/yum.repos.d/city-fan.org.repo
  fi
}

update_initphpfpm() {
  if [[ -f /etc/init.d/php-fpm && "$(grep -qw 'configtest' /etc/init.d/php-fpm >/dev/null 2>&1; echo $?)" != '0' ]]; then
    \cp -f "${SCRIPT_DIR}/init/php-fpm" /etc/init.d/php-fpm
    chmod +x /etc/init.d/php-fpm
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
      systemctl daemon-reload
    fi
  fi
  if [ ! -f /usr/bin/fpmconfigtest ]; then
    echo "/etc/init.d/php-fpm configtest" >/usr/bin/fpmconfigtest ; chmod 700 /usr/bin/fpmconfigtest
  fi
}

update_phpfpmconfg() {
  if [[ -f /usr/local/etc/php-fpm.conf && "$(grep -qw '\[global\]' /usr/local/etc/php-fpm.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
    sed -i '1i[global]' /usr/local/etc/php-fpm.conf
  fi
}

update_cmdshortcuts() {
  if [ -f /usr/bin/ngxstop ]; then
    if [[ "$(grep 'service' /usr/bin/npstop)" ]]; then
      sed -i 's|service nginx stop;service php-fpm stop|\/etc\/init.d\/nginx stop;\/etc\/init.d\/php-fpm stop|' /usr/bin/npstop
    fi
  fi
  if [ -f /usr/bin/npstart ]; then
    if [[ "$(grep 'service' /usr/bin/npstart)" ]]; then
      sed -i 's|service nginx start;service php-fpm start|\/etc\/init.d\/nginx start;\/etc\/init.d\/php-fpm start|' /usr/bin/npstart
    fi
  fi
  if [ -f /usr/bin/nprestart ]; then
    if [[ "$(grep 'service' /usr/bin/nprestart)" ]]; then
      sed -i 's|service nginx restart;service php-fpm restart|\/etc\/init.d\/nginx restart;\/etc\/init.d\/php-fpm restart|' /usr/bin/nprestart
    fi
  fi
  if [ -f /usr/bin/npreload ]; then
    if [[ "$(grep 'service' /usr/bin/npreload)" ]]; then
      sed -i 's|service nginx reload;service php-fpm reload|\/etc\/init.d\/nginx reload;\/etc\/init.d\/php-fpm reload|' /usr/bin/npreload
    fi
  fi
  if [ -f /usr/bin/ngxstop ]; then
    if [[ "$(grep 'service' /usr/bin/ngxstop)" ]]; then
      sed -i 's|service nginx stop|\/etc\/init.d\/nginx stop|' /usr/bin/ngxstop
    fi
  fi
  if [ -f /usr/bin/ngxstart ]; then
    if [[ "$(grep 'service' /usr/bin/ngxstart)" ]]; then
      sed -i 's|service nginx start|\/etc\/init.d\/nginx start|' /usr/bin/ngxstart
    fi
  fi
  if [ -f /usr/bin/ngxrestart ]; then
    if [[ "$(grep 'service' /usr/bin/ngxrestart)" ]]; then
      sed -i 's|service nginx restart|\/etc\/init.d\/nginx restart|' /usr/bin/ngxrestart
    fi
  fi
  if [ -f /usr/bin/ngxreload ]; then
    if [[ "$(grep 'service' /usr/bin/ngxreload)" ]]; then
      sed -i 's|service nginx reload|\/etc\/init.d\/nginx reload|' /usr/bin/ngxreload
    fi
  fi
  if [ -f /usr/bin/fpmstop ]; then
    if [[ "$(grep 'service' /usr/bin/fpmstop)" ]]; then
      sed -i 's|service php-fpm stop|\/etc\/init.d\/php-fpm stop|' /usr/bin/fpmstop
    fi
  fi
  if [ -f /usr/bin/fpmstart ]; then
    if [[ "$(grep 'service' /usr/bin/fpmstart)" ]]; then
      sed -i 's|service php-fpm start|\/etc\/init.d\/php-fpm start|' /usr/bin/fpmstart
    fi
  fi
  if [ -f /usr/bin/fpmrestart ]; then
    if [[ "$(grep 'service' /usr/bin/fpmrestart)" ]]; then
      sed -i 's|service php-fpm restart|\/etc\/init.d\/php-fpm restart|' /usr/bin/fpmrestart
    fi
  fi
  if [ -f /usr/bin/fpmreload ]; then
    if [[ "$(grep 'service' /usr/bin/fpmreload)" ]]; then
      sed -i 's|service php-fpm reload|\/etc\/init.d\/php-fpm reload|' /usr/bin/fpmreload
    fi
  fi
  if [ -f /usr/bin/memcachedstop ]; then
    if [[ "$(grep 'service' /usr/bin/memcachedstop)" ]]; then
      sed -i 's|service memcached stop|\/etc\/init.d\/memcached stop|' /usr/bin/memcachedstop
    fi
  fi
  if [ -f /usr/bin/memcachedstart ]; then
    if [[ "$(grep 'service' /usr/bin/memcachedstart)" ]]; then
      sed -i 's|service memcached start|\/etc\/init.d\/memcached start|' /usr/bin/memcachedstart
    fi
  fi
  if [ -f /usr/bin/memcachedrestart ]; then
    if [[ "$(grep 'service' /usr/bin/memcachedrestart)" ]]; then
      sed -i 's|service memcached restart|\/etc\/init.d\/memcached restart|' /usr/bin/memcachedrestart
    fi
  fi
  if [[ "$INITIALINSTALL" != [yY] && ! -f /usr/bin/customconfig ]]; then
    shortcutsinstall
  fi
}

update_nginxconf() {
  if [[ -f /usr/local/nginx/conf/nginx.conf && "$NGINX_ALLOWOVERRIDE" = [yY] ]]; then
    NGX_WRKCON=$(grep -w 'worker_connections' /usr/local/nginx/conf/nginx.conf | column -t| tr -s " " | sed -e 's|;||g' | awk '{print $2}' | grep -v 'worker_connections'| head -n1)
    NGX_KEEPALIVEREQUESTS=$(grep -w 'keepalive_requests' /usr/local/nginx/conf/nginx.conf | column -t| tr -s " " | sed -e 's|;||g' | awk '{print $2}' | grep -v 'keepalive_requests'| head -n1)
    NGX_KT=$(grep -w 'keepalive_timeout' /usr/local/nginx/conf/nginx.conf | column -t| tr -s " " | sed -e 's|;||g' | awk '{print $2}'| grep -v keep| head -n1)
    # adjust to new default higher values only if previous defaults are detected
    # do not adjust if end user has changed values from previous defaults
    if [[ "$NGX_KEEPALIVEREQUESTS" -eq '500' ]]; then
      sed -i 's|keepalive_requests .*|keepalive_requests  5000;|' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$NGX_KEEPALIVEREQUESTS" -eq '1000' ]]; then
      sed -i 's|keepalive_requests .*|keepalive_requests  5000;|' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$NGX_KEEPALIVEREQUESTS" -eq '5000' ]]; then
      # http://hg.nginx.org/nginx/rev/f790816a0e87
      sed -i 's|keepalive_requests .*|keepalive_requests  50000;|' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$NGX_WRKCON" -eq '4096' || "$NGX_WRKCON" -eq '10000' || "$NGX_WRKCON" -eq '50000' || "$NGX_WRKCON" -eq '80000' ]]; then
      sed -i 's|worker_connections .*|worker_connections  100000;|' /usr/local/nginx/conf/nginx.conf
    fi
    if [[ "$(echo $NGX_KT | grep -o 's')" = 's' ]]; then
      NGX_KT=$(echo $NGX_KT | sed -e 's|s||g')
    fi
    if [[ "$NGX_KT" = '8s' ]]; then
      sed -i 's|keepalive_timeout .*|keepalive_timeout  10s;|' /usr/local/nginx/conf/nginx.conf
    fi
  fi
}

checkipvsix() {
  # set via persistent config file at
  # /etc/centminmod/custom_config.inc as outlined on 
  # official site at 
  # https://centminmod.com/upgrade.html#persistent to
  # override defaults
  # disable system IPv6 support
  # https://wiki.centos.org/FAQ/CentOS7#head-8984faf811faccca74c7bcdd74de7467f2fcd8ee
  if [[ "$DISABLE_IPVSIX" = [yY] ]]; then
    if [[ -d /etc/postfix && "$(grep -w 'inet_protocols' /etc/postfix/main.cf | grep -w 'all')" ]]; then
      postconf -e 'inet_protocols = ipv4'
      restartpostfix='y'
    fi
    if [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '0' ]]; then
      DETECH_CHANGES='y'
    fi
    if [[ ! "$(grep -w 'net.ipv6.conf.all.disable_ipv6' /etc/sysctl.conf)" ]]; then
      echo 'net.ipv6.conf.all.disable_ipv6 = 1' >> /etc/sysctl.conf
      sysctl -w net.ipv6.conf.all.disable_ipv6=1 >/dev/null 2>&1
    elif [[ "$(grep -w 'net.ipv6.conf.all.disable_ipv6' /etc/sysctl.conf)" ]]; then
      sed -i 's|net.ipv6.conf.all.disable_ipv6 = .*|net.ipv6.conf.all.disable_ipv6 = 1|' /etc/sysctl.conf
      sysctl -w net.ipv6.conf.all.disable_ipv6=1 >/dev/null 2>&1
    fi
    if [[ ! "$(grep -w 'net.ipv6.conf.default.disable_ipv6' /etc/sysctl.conf)" ]]; then
      echo 'net.ipv6.conf.default.disable_ipv6 = 1' >> /etc/sysctl.conf
      sysctl -w net.ipv6.conf.default.disable_ipv6=1 >/dev/null 2>&1
    elif [[ "$(grep -w 'net.ipv6.conf.default.disable_ipv6' /etc/sysctl.conf)" ]]; then
      sed -i 's|net.ipv6.conf.default.disable_ipv6 = .*|net.ipv6.conf.default.disable_ipv6 = 1|' /etc/sysctl.conf
      sysctl -w net.ipv6.conf.default.disable_ipv6=1 >/dev/null 2>&1
    fi
    if [[ "$restartpostfix" = [yY] ]]; then
      service postfix restart >/dev/null 2>&1
    fi
    # address centos 7.4+ rpcbind failure to start if IPv6 is disabled
    # https://community.centminmod.com/threads/13353/
    if [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/lib/systemd/system/rpcbind.socket && "$(systemctl is-active rpcbind.socket -q; echo $?)" -eq '0' ]]; then
      # rpcbind.socket is active so needs restart
      mkdir -p /etc/systemd/system/rpcbind.socket.d
cat > "/etc/systemd/system/rpcbind.socket.d/no-ipv6.conf" <<ROF
[Socket]
ListenStream=
ListenStream=0.0.0.0:111
ListenStream=/var/run/rpcbind.sock
ROF
      systemctl daemon-reload -q
      systemctl stop rpcbind.socket rpcbind.service -q
      systemctl start rpcbind.socket rpcbind.service -q
    elif [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/lib/systemd/system/rpcbind.socket && "$(systemctl is-active rpcbind.socket -q; echo $?)" -ne '0' ]]; then
      # rpcbind.socket is not active so does not needs restart
      mkdir -p /etc/systemd/system/rpcbind.socket.d
cat > "/etc/systemd/system/rpcbind.socket.d/no-ipv6.conf" <<ROF
[Socket]
ListenStream=
ListenStream=0.0.0.0:111
ListenStream=/var/run/rpcbind.sock
ROF
      systemctl daemon-reload -q
      systemctl stop rpcbind.socket rpcbind.service -q
      systemctl start rpcbind.socket rpcbind.service -q
    fi
  elif [[ "$DISABLE_IPVSIX" != [yY] ]]; then
    if [[ -d /etc/postfix && "$(grep -w 'inet_protocols' /etc/postfix/main.cf | grep -w 'ipv4')" ]]; then
      postconf -e 'inet_protocols = all'
      restartpostfix='y'
    fi
    if [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '1' ]]; then
      DETECH_CHANGES='y'
    elif [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '0' ]]; then
      DETECH_CHANGES='n'
    fi
    if [[ "$DETECH_CHANGES" = [nN] && ! "$(grep -w 'net.ipv6.conf.all.disable_ipv6' /etc/sysctl.conf)" ]]; then
      echo 'net.ipv6.conf.all.disable_ipv6 = 0' >> /etc/sysctl.conf
      sysctl -w net.ipv6.conf.all.disable_ipv6=0 >/dev/null 2>&1
    elif [[ "$DETECH_CHANGES" = [yY] && "$(grep -w 'net.ipv6.conf.all.disable_ipv6' /etc/sysctl.conf)" ]]; then
      sed -i 's|net.ipv6.conf.all.disable_ipv6 = .*|net.ipv6.conf.all.disable_ipv6 = 0|' /etc/sysctl.conf
      sysctl -w net.ipv6.conf.all.disable_ipv6=0 >/dev/null 2>&1
    fi
    if [[ "$DETECH_CHANGES" = [nN] && ! "$(grep -w 'net.ipv6.conf.default.disable_ipv6' /etc/sysctl.conf)" ]]; then
      echo 'net.ipv6.conf.default.disable_ipv6 = 0' >> /etc/sysctl.conf
      sysctl -w net.ipv6.conf.default.disable_ipv6=0 >/dev/null 2>&1
    elif [[ "$DETECH_CHANGES" = [yY] && "$(grep -w 'net.ipv6.conf.default.disable_ipv6' /etc/sysctl.conf)" ]]; then
      sed -i 's|net.ipv6.conf.default.disable_ipv6 = .*|net.ipv6.conf.default.disable_ipv6 = 0|' /etc/sysctl.conf
      sysctl -w net.ipv6.conf.default.disable_ipv6=0 >/dev/null 2>&1
    fi
    if [[ "$restartpostfix" = [yY] ]]; then
      service postfix restart >/dev/null 2>&1
    fi
    # address centos 7.4+ rpcbind failure to start if IPv6 is disabled
    # https://community.centminmod.com/threads/13353/
    if [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/lib/systemd/system/rpcbind.socket && "$(systemctl is-active rpcbind.socket -q; echo $?)" -eq '0' ]]; then
      # rpcbind.socket is active so needs restart
      if [ -f /etc/systemd/system/rpcbind.socket.d/no-ipv6.conf ]; then
        rm -rf /etc/systemd/system/rpcbind.socket.d/no-ipv6.conf
      fi
      systemctl daemon-reload -q
      systemctl stop rpcbind.socket rpcbind.service -q
      systemctl start rpcbind.socket rpcbind.service -q
    elif [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/lib/systemd/system/rpcbind.socket && "$(systemctl is-active rpcbind.socket -q; echo $?)" -ne '0' ]]; then
      # rpcbind.socket is not active so does not needs restart
      if [ -f /etc/systemd/system/rpcbind.socket.d/no-ipv6.conf ]; then
        rm -rf /etc/systemd/system/rpcbind.socket.d/no-ipv6.conf
      fi
      systemctl daemon-reload -q
      systemctl stop rpcbind.socket rpcbind.service -q
      systemctl start rpcbind.socket rpcbind.service -q
    fi
  fi
  if [[ -f /etc/csf/csf.conf && -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" = '0' ]] && [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '0' && "$(cat /proc/sys/net/ipv6/conf/default/disable_ipv6)" -eq '0' ]]; then
    # ensure CSF Firewall has IPV6 = '1' enabled if system has IPv6 networking configured
    sed -i "s|^IPV6 = .*|IPV6 = \"1\"|" /etc/csf/csf.conf
    csf -ra >/dev/null 2>&1
    NGINX_IPV='y'     # for nginx < 1.11.5 IPV6 support
  elif [[ -f /etc/csf/csf.conf && -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" != '0' ]] && [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '1' && "$(cat /proc/sys/net/ipv6/conf/default/disable_ipv6)" -eq '1' ]]; then
    sed -i "s|^IPV6 = .*|IPV6 = \"0\"|" /etc/csf/csf.conf
    csf -ra >/dev/null 2>&1
  elif [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '0' && "$(cat /proc/sys/net/ipv6/conf/default/disable_ipv6)" -eq '0' ]]; then
    # ensure CSF Firewall has IPV6 = '1' enabled if system has IPv6 networking configured
    sed -i "s|^IPV6 = .*|IPV6 = \"1\"|" /etc/csf/csf.conf
    csf -ra >/dev/null 2>&1
    NGINX_IPV='y'     # for nginx < 1.11.5 IPV6 support
  elif [[ "$(cat /proc/sys/net/ipv6/conf/all/disable_ipv6)" -eq '1' && "$(cat /proc/sys/net/ipv6/conf/default/disable_ipv6)" -eq '1' ]]; then
    sed -i "s|^IPV6 = .*|IPV6 = \"0\"|" /etc/csf/csf.conf
    csf -ra >/dev/null 2>&1
  fi
  if [[ "$DETECH_CHANGES" = [yY] ]]; then
    service network restart >/dev/null 2>&1
  fi
}

checkwoff() {
  if [[ -f /usr/local/nginx/conf/mime.types && -z "$(grep woff2 /usr/local/nginx/conf/mime.types)" ]] || [[ -f /usr/local/nginx/conf/mime.types && -z "$(grep otf /usr/local/nginx/conf/mime.types)" ]]; then
    # check for woff2 and ott mime type support in nginx https://github.com/centminmod/centminmod/issues/60
    mimefix 1
  fi
}

nano_highlight() {
  # enable nano editor syntax highlighing
  if [ -f /etc/nanorc ]; then
    if [[ -z "$(grep '^include' /etc/nanorc)" && "$(grep '^# include' /etc/nanorc)" ]]; then
      sed -i 's|^# include|include|g' /etc/nanorc
    fi
  fi
}

fixwp_updater() {
  # auto correct bug
  # https://community.centminmod.com/posts/44969/
  if [ -d /root/tools ]; then
    for f in $(find /root/tools/ -type f -name "wp_updater_*"); do grep -w 'pure-pw' $f && sed -i "/pure-pw/d" $f; done
  fi

  # auto fix wp_updater cronjobs code only if 23rd line of file has expected bug
  # and file has exactly 36 lines of code (to prevent messing up customized files)
  # part 1 fix outlined at https://community.centminmod.com/threads/15123/
  if [ -d /root/tools ]; then
    find /root/tools -type f -name "wp_updater_*.sh" | while read w; do
      if [[ "$(sed -n '23'p "$w" | grep 'if \[\[ -f .wpcli-status && ! "" \]\]; then')" && "$(wc -l < "$w")" -eq '36' ]]; then
        echo
        echo "updating wp updater code for $w"
        echo "https://community.centminmod.com/threads/15123/"
        DT=$(date +"%d%m%y-%H%M%S")
        echo "cp -a "$w" "${w}.backup-$DT""
        cp -a "$w" "${w}.backup-$DT"
        sed -i "s| \"\"| \"\$(grep -w 'Plugin already updated' .wpcli-status)\"|" "$w"
        cat "$w" | grep wpcli-status | grep then
      fi
    done
  fi
}

fixlshw_rpmforge() {
  # prefer lshw from centos repo instead of rpmforge
  if [[ -f /etc/yum.repos.d/rpmforge.repo && "$(grep '^exclude=' /etc/yum.repos.d/rpmforge.repo | grep lshw > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/rpmforge.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_LSHWEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/rpmforge.repo) lshw")
    sed -i "s|^exclude=.*|$NEW_LSHWEXCLUDES|" /etc/yum.repos.d/rpmforge.repo
    # yum -y swap -- remove lshw -- install lshw --disablerepo=rpmforge
    fi
  fi
}

fixnoarch_percona() {
  if [[ "$(yum-config-manager percona-release-noarch | awk '/enabled = / {print $3}')" != 'False' ]]; then
    yum-config-manager --disable percona-release-noarch >/dev/null 2>&1
  fi
}

fixsysbench_percona() {
  # prevent sysbench install via percona-release yum repo
  if [[ -f /etc/yum.repos.d/percona-release.repo && "$(grep '^exclude=' /etc/yum.repos.d/percona-release.repo | grep sysbench > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/percona-release.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_SYSBENCH_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/percona-release.repo) sysbench")
    sed -i "s|^exclude=.*|$NEW_SYSBENCH_EXCLUDES|" /etc/yum.repos.d/percona-release.repo
    fi
  fi
}

fixpercona_epel() {
  # prevent percona-xtrabackup install via epel yum repo
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep percona-xtrabackup > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_XTRABACKUPEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) percona-xtrabackup")
    sed -i "s|^exclude=.*|$NEW_XTRABACKUPEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
}

fixnodejs_epel() {
  # prevent node.js 6.x install by epel in favour of nodesource
  # repo via addons/nodejs.sh
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep nodejs > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_NODEJSEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) nodejs")
    sed -i "s|^exclude=.*|$NEW_NODEJSEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
}

fixnginx_epel() {
  # prevent nginx install via epel yum repo
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep nginx > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_NGINXEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) nginx")
    sed -i "s|^exclude=.*|$NEW_NGINXEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
}

fixsclutils_epel() {
  # prevent scl-utils install via epel yum repo
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep scl-utils > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_SCLUTILSEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) scl-utils")
    sed -i "s|^exclude=.*|$NEW_SCLUTILSEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
}

fixclamav_epel() {
  # prevent clamav-devel install by epel in favour of rpmforge version
  # https://community.centminmod.com/posts/38359/
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep 'clamav-devel' > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_CLAMAVDEVELEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) clamav-devel")
    sed -i "s|^exclude=.*|$NEW_CLAMAVDEVELEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
  # prevent mongodb epel install in favour of official repo install
  # https://docs.mongodb.com/manual/tutorial/install-mongodb-on-red-hat/
  if [[ -f /etc/yum.repos.d/epel.repo && "$(grep '^exclude=' /etc/yum.repos.d/epel.repo | grep 'mongodb\*' > /dev/null 2>&1; echo $?)" = '1' ]]; then
    if [[ "$(grep '^exclude=' /etc/yum.repos.d/epel.repo > /dev/null 2>&1; echo $?)" = '0' ]]; then
    NEW_MONGODBEXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.repos.d/epel.repo) mongodb*")
    sed -i "s|^exclude=.*|$NEW_MONGODBEXCLUDES|" /etc/yum.repos.d/epel.repo
    fi
  fi
}

c7mariadb_tmpdir() {
  # fix for mariadb 10.1.16 introduced ProtecHome=true setting
  # https://jira.mariadb.org/browse/MDEV-10399
  if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/systemd/system/mariadb.service.d/protecthome.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Update MariaDB 10 setting for ProtectHome=false"
cat > "/etc/systemd/system/mariadb.service.d/protecthome.conf" <<TDF
[Service]
ProtectSystem=false
ProtectHome=false
TDF
  systemctl daemon-reload
  systemctl restart mysql
 elif [[ "$CENTOS_SEVEN" = '7' && -f /etc/systemd/system/mariadb.service.d/protecthome.conf && -z "$(grep 'ProtectSystem=false' /etc/systemd/system/mariadb.service.d/protecthome.conf)" ]]; then
    # fix https://community.centminmod.com/posts/54571/ CentOS 7.4 changes
    echo "Update MariaDB 10 setting for ProtectSystem=false addition"
cat > "/etc/systemd/system/mariadb.service.d/protecthome.conf" <<TDF
[Service]
ProtectSystem=false
ProtectHome=false
TDF
  systemctl daemon-reload
  systemctl restart mysql
  fi
}

mariadb_openfilesfix() {
  if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/systemd/system/mariadb.service.d/openfileslimit.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Update MariaDB 10 setting for LimitNOFILE=524288"
cat > "/etc/systemd/system/mariadb.service.d/openfileslimit.conf" <<TDG
[Service]
LimitNOFILE=524288
TDG
  systemctl daemon-reload
  systemctl restart mysql
  fi
}

fixlibmysqlclient_symlink() {
  if [ "$(uname -m)" == 'x86_64' ]; then
    FIXMDB_LIBDIR='lib64'
  else
    FIXMDB_LIBDIR='lib'
  fi
  if [[ ! -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.20" ]]; then
    mkdir -p "/usr/${FIXMDB_LIBDIR}/mysql"
    rm -rf "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.20" "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    # ls -lah "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
  elif [[ ! -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.18" ]]; then
    mkdir -p "/usr/${FIXMDB_LIBDIR}/mysql"
    rm -rf "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.18" "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    # ls -lah "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
  elif [[ ! -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so" ]] && [[ -f "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.16" ]]; then
    mkdir -p "/usr/${FIXMDB_LIBDIR}/mysql"
    rm -rf "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    ln -s "/usr/${FIXMDB_LIBDIR}/libmysqlclient.so.16" "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
    # ls -lah "/usr/${FIXMDB_LIBDIR}/mysql/libmysqlclient.so"
  fi
}

fixphpfpm_includes() {
  # in line with https://www.nginx.com/resources/wiki/start/topics/examples/phpfcgi/
  if [ -d /usr/local/nginx/conf ]; then
  {
  DT=$(date +"%d%m%y-%H%M%S")
  for pifinc in $(find /usr/local/nginx/conf -type f -name "*.conf" | grep php); do
    if [[ "$(grep 'shave 200+ ms' $pifinc)" && "$(grep 'try_files' $pifinc)" ]]; then
      echo "updating php-fpm config files syntax"
      echo $pifinc
      sed -i 's|location ~ \\.php$ {|location ~ [^\/]\\.php(\/\|$) {|' $pifinc
      sed -i 's|fastcgi_split_path_info ^(.+\\.php)(/.+)$;|fastcgi_split_path_info ^(.+\?\\.php)(/.*)$;|' $pifinc
      sed -i 's|    *fastcgi_param  SCRIPT_FILENAME    $request_filename;|    #fastcgi_param  SCRIPT_FILENAME    $request_filename;|' $pifinc
      sed -i 's|    *#fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;|    fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;|' $pifinc
      sed -i 's|try_files $uri =404;|if (!-f $document_root$fastcgi_script_name) { return 404; }|' $pifinc
      egrep 'location|fastcgi_split_path_info|fastcgi_param  SCRIPT_FILENAME|404' $pifinc
      echo "y" > /tmp/phpinc-restart-check
    fi
  done
    if [ -f /tmp/phpinc-restart-check ]; then
      rm -rf /tmp/phpinc-restart-check
      service nginx restart
      service php-fpm restart
    fi
  } 2>&1 | tee "/root/centminlogs/fixphpfpm_includes_${DT}.log"
  # remove log if empty
  if [ ! -s "/root/centminlogs/fixphpfpm_includes_${DT}.log" ]; then
    rm -rf "/root/centminlogs/fixphpfpm_includes_${DT}.log"
  fi
  fi
}

fixphpfpm_httpproxy() {
  # CVE-2016-5385
  # https://www.nginx.com/blog/mitigating-the-httpoxy-vulnerability-with-nginx/
  if [ -d /usr/local/nginx/conf ]; then
  {
  DT=$(date +"%d%m%y-%H%M%S")
  for pif in $(find /usr/local/nginx/conf -type f -name "*.conf" -o -name "fastcgi_params" | egrep 'php|fastcgi_params'); do
    if [[ "$(grep 'fastcgi_param  HTTPS' $pif)" && ! "$(grep 'HTTP_PROXY' $pif)" ]]; then
      echo "updating php-fpm config files to block Proxy header / HTTP_PROXY"
      echo $pif
      #sed -i '/HTTP_PROXY/d' $pif
      #sed -e 's|fastcgi_param  HTTPS              $https if_not_empty;|fastcgi_param  HTTPS              $https if_not_empty;\nfastcgi_param  HTTP_PROXY         "";|' $pif | grep HTTP_PROXY
      sed -i 's|fastcgi_param  HTTPS              $https if_not_empty;|fastcgi_param  HTTPS              $https if_not_empty;\nfastcgi_param  HTTP_PROXY         "";|' $pif
      grep 'HTTP_PROXY' $pif
      echo "y" > /tmp/phprestart-check
    fi
  done
    if [ -f /tmp/phprestart-check ]; then
      rm -rf /tmp/phprestart-check
      service nginx restart
      service php-fpm restart
    fi
  } 2>&1 | tee "/root/centminlogs/fixphpfpm_httpproxy_${DT}.log"
  # remove log if empty
  if [ ! -s "/root/centminlogs/fixphpfpm_httpproxy_${DT}.log" ]; then
    rm -rf "/root/centminlogs/fixphpfpm_httpproxy_${DT}.log"
  fi
  fi
}

nginx_mutexoff() {
  # http://hg.nginx.org/nginx/rev/d82b3c344e7e
  if [[ -f /usr/local/nginx/conf/nginx.conf && "$(grep -q 'accept_mutex on' /usr/local/nginx/conf/nginx.conf)" ]]; then
    sed -i 's|accept_mutex .*|accept_mutex off;|' /usr/local/nginx/conf/nginx.conf
  fi
}

check_jemstatsfile() {
  if [[ ! -f /usr/bin/jemalloc-stats && -f /usr/bin/jemalloc.sh ]]; then
    jemalloc_printstats
  fi
}

gitenv_askupdate() {
  DT=$(date +"%d%m%y-%H%M%S")
  {
    if [[ -d "${SCRIPT_DIR}/.git" ]]; then
      # if git remote repo url is not same as one defined in giturl.txt then pull a new copy of
      # centmin mod code locally using giturl.txt defined git repo name
      GET_GITVER=$(git --version | awk '{print $3}' | sed -e 's|\.||g' | cut -c1,2)
      # https://github.com/centminmod/centminmod/raw/${branchname}/giturl.txt
      # check if you can properly resolve raw.githubusercontent.com first in case of dns outpages
      # issues at github end https://community.centminmod.com/threads/centmin-mod-github-com-repo-504-timeouts.9232/
      CURL_GITURLCHECKER=$(curl -sk${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt >/dev/null 2>&1; echo $?)
      if [[ "$CURL_GITURLCHECKER" = '0' ]]; then
        CURL_GITURL=$(curl -sk${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt)
      else
        # just default to https://github.com/centminmod/centminmod.git if can not resolve the
        # https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt
        # url link
        CURL_GITURL='https://github.com/centminmod/centminmod.git'
      fi
      # if git version >1.8 use supported ls-remote --get-url flag otherwise use alternative
      if [[ "$GET_GITVER" -ge '18' ]]; then
        GET_GITREMOTEURL=$(cd /usr/local/src/centminmod >/dev/null 2>&1; git ls-remote --get-url)
      else
        GET_GITREMOTEURL=$(cd /usr/local/src/centminmod >/dev/null 2>&1; git remote -v | awk '/\(fetch/ {print $2}' | head -n1)
      fi
      if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]] && [[ ! -z "$CURL_GITURL" ]]; then
        cecho "-------------------------------------------------------------" $boldgreen
        cecho " Centmin Mod remote branch has changed" $boldyellow
        cecho " from $GET_GITREMOTEURL" $boldyellow
        cecho " to $CURL_GITURL" $boldyellow
        cecho "-------------------------------------------------------------" $boldgreen
        read -ep " Do you want to update the Centmin Mod Git repo url ? [y/n]: " updategitrepourl
        if [[ "$updategitrepourl" = [yY] ]]; then
          gitenv_setup
          cecho "-------------------------------------------------------------" $boldgreen
        fi
    fi
      pushd "${SCRIPT_DIR}" >/dev/null 2>&1
      git fetch >/dev/null 2>&1
      popd >/dev/null 2>&1
    if [[ "$(cd /usr/local/src/centminmod >/dev/null 2>&1; git rev-parse HEAD)" != "$(cd /usr/local/src/centminmod >/dev/null 2>&1; git rev-parse @{u})" ]]; then
        # if remote branch commits don't match local commit, then there are new updates need
        # pulling
        cecho "-------------------------------------------------------------" $boldgreen
        cecho " Centmin Mod code updates available for /usr/local/src/centminmod" $boldyellow
        cecho " List of updates: community.centminmod.com/forums/41/?prefix_id=19" $boldyellow
        cecho "-------------------------------------------------------------" $boldgreen
        read -ep " Do you want to update your local Centmin Mod Git code ? [y/n]: " updategitcode
        if [[ "$updategitcode" = [yY] ]]; then
          gitenv_update
          cecho "-------------------------------------------------------------" $boldgreen
        fi
      else
        # no new commits/updates available
        cecho "-------------------------------------------------------------" $boldgreen
        cecho " Centmin Mod local code is up to date at /usr/local/src/centminmod" $boldyellow
        cecho " no available updates at this time..." $boldyellow
        cecho "-------------------------------------------------------------" $boldgreen
      fi
    fi
  } 2>&1 | tee /root/centminlogs/centminmod_${SCRIPT_VERSION}_${DT}_git_updatecur_branch.log
}

wgetver_check() {
  if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
    WGET_VERSION=$WGET_VERSION_SEVEN
  fi
  # only check if on 64bit OS
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    if [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]] || [[ "$(pcre-config --version | grep -q ${PCRE_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
      if [[ "$LOWMEM_INSTALL" != [yY] ]]; then
        if [[ -f "${SCRIPT_DIR}/addons/wget.sh" && -f /usr/local/sbin/nginx ]]; then
          echo "update wget ${WGET_VERSION} & pcre ${PCRE_VERSION} version... one time task"
          sleep 6
          "${SCRIPT_DIR}/addons/wget.sh" install
        fi
      fi
    elif [[ -f "${SCRIPT_DIR}/addons/wget.sh" && -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No UTF-8 support' >/dev/null 2>&1; echo $?)" = '0' ]]; then
      echo "fix pcre install for missing UTF8 support... one time task"
      sleep 6
      "${SCRIPT_DIR}/addons/wget.sh" pcre
    elif [[ -f "${SCRIPT_DIR}/addons/wget.sh" && -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No just-in-time compiler support' >/dev/null 2>&1; echo $?)" = '0' ]]; then
      echo "add pcre jit support... one time task"
      sleep 6
      "${SCRIPT_DIR}/addons/wget.sh" pcre
    fi
    if [[ "$(uname -m)" != 'x86_64' ]]; then
      if [ -f /root/.wgetrc ]; then
        \cp -fp /root/.wgetrc /root/.wgetrc-bak
        echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
      else
        echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
      fi
    fi
  fi
}

axelcheck() {
  if [[ -f /usr/local/bin/axel || -f /usr/bin/axel ]]; then
  if [[ "$(axel -V | awk '/version/ {print $3}')" != "${AXEL_VER}" ]] || [[ -f /usr/local/bin/axel && ! -d "${DIR_TMP}/axel-${AXEL_VER}" && ! -f "${DIR_TMP}/${AXEL_LINKFILE}" ]]; then
    echo "update axel version... one time task"
    sleep 3
    install_axel silent
  fi
  fi
}

set_logdate() {
  DT=$(date +"%d%m%y-%H%M%S")
}

ngxver_checker() {
  if [[ "$(which nginx >/dev/null 2>&1; echo $?)" = '0' ]]; then
    LASTEST_NGINXVERS=$(curl -${ipv_forceopt}sL https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n 1 2>&1)
    CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk -F '/' '{print $2}')
    if [[ "$CURRENT_NGINXVERS" != "$LASTEST_NGINXVERS" ]]; then
      echo
      cecho "-------------------------------------------------------------" $boldgreen
      echo "* Current Nginx Version: $CURRENT_NGINXVERS"
      echo "* Latest Nginx Available: $LASTEST_NGINXVERS (centminmod.com/nginxnews)"
      cecho "-------------------------------------------------------------" $boldgreen
      echo
    fi
  fi
}

yumupdatechecker() {
  if [[ "$INITIALINSTALL" != [yY] ]]; then
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

    if [ -f /etc/yum/pluginconf.d/versionlock.conf ]; then
        VERSIONLOCK_REPO=',versionlock'
    else
        VERSIONLOCK_REPO=""
    fi
    if [[ -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/city-fan.org.repo ]] && [[ -f /usr/bin/php56 || -f /usr/bin/php70 || -f /usr/bin/php71 || -f /usr/bin/php72 || -f /usr/bin/php73 ]]; then
      YENABLEREPOS='remi,city-fan.org,remi-test'
      YEXCLUDE='nginx* mysql*'
    elif [[ -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/city-fan.org.repo ]]; then
      YENABLEREPOS='remi,city-fan.org'
      YEXCLUDE='nginx* php* mysql*'
    elif [ -f /etc/yum.repos.d/remi.repo ]; then
      YENABLEREPOS='remi'
      YEXCLUDE='nginx* php* mysql*'
    else
      YENABLEREPOS=""
      YEXCLUDE='nginx* php* mysql*'
    fi
    YYUM_PARAMETER="--disableplugin=priorities${VERSIONLOCK_REPO} --setopt=deltarpm=0"
    YYUM_UPDATEPARAMETER="--disableplugin=priorities --setopt=deltarpm=0"
    # if IUS Community python34u, python35u or python36u packages installed from addons installers
    # ensure that system/EPEL versions of python34 and python36 are excluded from yum list & yum updates
    if [[ "$(yum list installed python34u* python35u* python36u* -q --exclude=python* 2>&1 | grep -v Installed | grep -v 'No matching Packages' | tr -s ' ' | column -t)" ]]; then
      PROMPT_FOR_BASE_PYTHON='y'
      YUM_EXCLUDE_PYTHON=' --exclude=python34-*,python35-*,python36-*'
    else
      YUM_EXCLUDE_PYTHON=""
    fi
    echo
    echo " checking for YUM updates... please wait..."
    # only check if last yum update was over 24hrs ago
    LASTYUMCHECK_EPOCHDATE=$(date -d "$(awk '/Updated: / {print $1,$2,$3}' /var/log/yum.log | tail -1)" +%s)
    # LASTYUMHISTORYCHECK_EPOCHDATE=$(date -d $(history | grep 'yum' | grep update | grep -v list | tail -1 | awk '{print $2}' | sed -e 's|\[||g' -e 's|\]||g' -e 's|\.|\/|g') +%s)
    TODAY_EPOCHDATE=$(date +%s)
    SEC_SINCELASTUPDATE=$(echo ${TODAY_EPOCHDATE} - ${LASTYUMCHECK_EPOCHDATE} | bc)
    # SEC_SINCEHISTORYLASTUPDATE=$(echo ${TODAY_EPOCHDATE} - ${LASTYUMHISTORYCHECK_EPOCHDATE} | bc)
    if [[ -f /etc/yum.repos.d/remi.repo || -f /etc/yum.repos.d/city-fan.org.repo ]]; then
      if [[ "$MENUEXIT_ALWAYS_YUMCHECK" = [yY] ]]; then
        UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
      elif [[ "$MENUEXIT_ALWAYS_YUMCHECK" != [yY] && "${SEC_SINCELASTUPDATE}" -gt '86400' ]]; then
        UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
      else
        UPDATE_CHECK=1
      fi
    else
      if [[ "$MENUEXIT_ALWAYS_YUMCHECK" = [yY] ]]; then
        UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
      elif [[ "$MENUEXIT_ALWAYS_YUMCHECK" != [yY] && "${SEC_SINCELASTUPDATE}" -gt '86400' ]]; then
        UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
      else
        UPDATE_CHECK=1
      fi
    fi
    if [[ "$UPDATE_CHECK" = '100' ]]; then
      echo
      if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
        SERVERHOSTNAME=$(hostname)
      else
        SERVERHOSTNAME=$(hostname -f)
      fi
      cecho "-------------------------------------------------------------" $boldgreen
      cecho "  New YUM Updates available for host $SERVERHOSTNAME" $boldyellow
      cecho "-------------------------------------------------------------" $boldgreen
      cecho "  To list available YUM Updates type: " $boldyellow
      cecho "-------------------------------------------------------------" $boldgreen
      if [[ -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/city-fan.org.repo ]] && [[ -f /usr/bin/php56 || -f /usr/bin/php70 || -f /usr/bin/php71 || -f /usr/bin/php72 || -f /usr/bin/php73 ]]; then
        echo "  yum list updates $YYUM_PARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --disableexcludes=main,remi"
      elif [[ -f /etc/yum.repos.d/remi.repo || -f /etc/yum.repos.d/city-fan.org.repo ]]; then
        echo "  yum list updates $YYUM_PARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON}"
      else
        echo "  yum list updates $YYUM_PARAMETER"
      fi
      if [[ -f /usr/local/bin/kernel-update && "$(uname -r | grep -o elrepo)" = 'elrepo' ]]; then
        echo
        cecho "-------------------------------------------------------------" $boldgreen
        cecho "  To Check For ELRepo Kernel Updates type: " $boldyellow
        cecho "-------------------------------------------------------------" $boldgreen
        echo "/usr/local/bin/kernel-update"
        echo
      fi
      cecho "-------------------------------------------------------------" $boldgreen
      cecho "  Following Updates are available: " $boldyellow
      cecho "-------------------------------------------------------------" $boldgreen
      if [[ "$CENTOS_SIX" = '6' ]]; then
        REMI_IMAGEMAGICK_PKGNAME='6'
      else
        REMI_IMAGEMAGICK_PKGNAME=""
      fi
      if [[ -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/city-fan.org.repo ]] && [[ -f /usr/bin/php56 || -f /usr/bin/php70 || -f /usr/bin/php71 || -f /usr/bin/php72 || -f /usr/bin/php73 ]]; then
        yum -q list updates $YYUM_PARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --disableexcludes=main,remi | grep -v 'Updated Packages' | tee "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
        IMAGEMAGICK_UPDATECHECK=$(grep "ImageMagick${REMI_IMAGEMAGICK_PKGNAME}" "/root/centminlogs/yumcheck-imagemagick-$DT.txt" >/dev/null 2>&1; echo $?)
        rm -rf "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
      elif [[ -f /etc/yum.repos.d/remi.repo || -f /etc/yum.repos.d/city-fan.org.repo ]]; then
        yum -q list updates $YYUM_PARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} | grep -v 'Updated Packages' | tee "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
        IMAGEMAGICK_UPDATECHECK=$(grep "ImageMagick${REMI_IMAGEMAGICK_PKGNAME}" "/root/centminlogs/yumcheck-imagemagick-$DT.txt" >/dev/null 2>&1; echo $?)
        rm -rf "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
      else
        yum -q list updates $YYUM_PARAMETER | grep -v 'Updated Packages' | tee "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
        IMAGEMAGICK_UPDATECHECK=$(grep "ImageMagick${REMI_IMAGEMAGICK_PKGNAME}" "/root/centminlogs/yumcheck-imagemagick-$DT.txt" >/dev/null 2>&1; echo $?)
        rm -rf "/root/centminlogs/yumcheck-imagemagick-$DT.txt"
      fi
      # cecho "-------------------------------------------------------------" $boldgreen
      echo
      cecho "-------------------------------------------------------------" $boldgreen
      cecho "  To update type these commands: " $boldyellow
      cecho "-------------------------------------------------------------" $boldgreen
      if [[ -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/city-fan.org.repo ]] && [[ -f /usr/bin/php56 || -f /usr/bin/php70 || -f /usr/bin/php71 || -f /usr/bin/php72 || -f /usr/bin/php73 ]]; then
        if [ "$IMAGEMAGICK_UPDATECHECK" -eq '0' ]; then
          echo "  run centmin.sh menu option 15 to update imagick PHP ext then run: "
          echo
        fi
        echo "  yum update $YYUM_UPDATEPARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON} --disableexcludes=main,remi"
      elif [[ -f /etc/yum.repos.d/remi.repo || -f /etc/yum.repos.d/city-fan.org.repo ]]; then
        if [ "$IMAGEMAGICK_UPDATECHECK" -eq '0' ]; then
          echo "  run centmin.sh menu option 15 to update imagick PHP ext then run: "
          echo
        fi
        echo "  yum update $YYUM_UPDATEPARAMETER --enablerepo=${YENABLEREPOS}${YUM_EXCLUDE_PYTHON}"
      else
        if [ "$IMAGEMAGICK_UPDATECHECK" -eq '0' ]; then
          echo "  run centmin.sh menu option 15 to update imagick PHP ext then run: "
          echo
        fi
        echo "  yum update $YYUM_UPDATEPARAMETER"
      fi
      echo
      if [[ "$CENTOS_SEVEN" = '7' && "$PROMPT_FOR_BASE_PYTHON" = [yY] ]]; then
        echo "detected python36u IUS Community YUM packages installed"
        echo "For CentOS 7 they have been replaced with native CentOS 7.7+"
        echo "python3 packages now"
        echo
        echo "One time task to run the addons/python36_install.sh script"
        echo "to switch from python36u to python3 packages for Python 3.6"
        echo "before doing YUM update command, run the script at:"
        echo "/usr/local/src/centminmod/addons/python36_install.sh"
      fi
      echo
    else
      echo " no YUM updates available"
    fi
  fi
}

setupemailcheck() {
  if [[ "$INITIALINSTALL" != [yY] ]]; then
    if [ -f /etc/centminmod/email-primary.ini ]; then
      if [ ! -s /etc/centminmod/email-primary.ini ]; then
        PEMAIL_NOSET='y'
      fi
    else
      PEMAIL_NOSET='y'
    fi
    if [ -f /etc/centminmod/email-secondary.ini ]; then
      if [ ! -s /etc/centminmod/email-secondary.ini ]; then
        SEMAIL_NOSET='y'
      fi
    else
      SEMAIL_NOSET='y'
    fi
    if [[ "$PEMAIL_NOSET" = 'y' || "$SEMAIL_NOSET" = 'y' ]] && [[ "$INITIALINSTALL" != [yY] ]]; then
      if [ -f "${SCRIPT_DIR}/tools/email.sh" ]; then
        "${SCRIPT_DIR}/tools/email.sh"
      fi
    fi
  fi
}

selinxcheck() {
  # check if selinux is disabled, if not abort and prompt to reboot

  if [ ! -f /usr/sbin/sestatus ]; then
    yum -q -y install policycoreutils
  fi

  if [ ! -f /usr/sbin/setenforce ]; then
    yum -q -y install libselinux-utils
  fi  

  if [[ -z "$(sestatus | grep disabled)" && -f /etc/selinux/config ]]; then
    echo "---------------------------------------------------------------"
    echo "SELINUX linux detected"
    echo "---------------------------------------------------------------"
    echo "disabling SELINUX..."
    echo "will require a server reboot before running initial install"
    # sleep 3
    sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
    sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
    echo
    echo "SELINUX disabled, please reboot server and rerun install"
    echo "on reboot you can find the centmin.sh file located at:"
    echo "echo $(pwd)"
    echo "---------------------------------------------------------------"
    # exit 0
  elif [[ -z "$(sestatus | grep disabled)" && -f /etc/sysconfig/selinux ]]; then
    echo "---------------------------------------------------------------"
    echo "SELINUX linux detected"
    echo "---------------------------------------------------------------"
    echo "disabling SELINUX..."
    echo "will require a server reboot before running initial install"
    # sleep 3
    sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/sysconfig/selinux && setenforce 0
    sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/sysconfig/selinux && setenforce 0
    echo
    echo "SELINUX disabled, please reboot server and rerun install"
    echo "on reboot you can find the centmin.sh file located at:"
    echo "echo $(pwd)"
    echo "---------------------------------------------------------------"
    # exit 0 
  fi
}

reuseportchecks() {
  if [[ "$INITIALINSTALL" = [yY] ]]; then
    CHECKREUSEPORT=$(grep --color -Ro SO_REUSEPORT /usr/src/kernels/* | head -n1 | awk -F ":" '{print $2}')
    if [[ "$CHECKREUSEPORT" = 'SO_REUSEPORT' ]]; then
      SUPPORT_REUSEPORT=y
      if [ -f /usr/local/nginx/conf/nginx.conf ]; then
        sed -i 's|accept_mutex .*|accept_mutex off;|' /usr/local/nginx/conf/nginx.conf
      fi
    else
      SUPPORT_REUSEPORT=n
      if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
        sed -i 's| reuseport||' /usr/local/nginx/conf/conf.d/virtual.conf
      fi
    fi
  fi
}

rclocalchecks() {
  if [[ "$CENTOS_SEVEN" = '7' && ! "$(grep '/var/run/php-fpm' /etc/rc.local)" ]]; then
    echo 'if [ ! -d /var/run/php-fpm/ ]; then mkdir -p /var/run/php-fpm/; fi' >> /etc/rc.local
  fi

# https://community.centminmod.com/posts/52406/
if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/rc.d/rc.local ]]; then


cat > /usr/lib/systemd/system/rc-local.service <<EOF
# This unit gets pulled automatically into multi-user.target by
# systemd-rc-local-generator if /etc/rc.d/rc.local is executable.
[Unit]
Description=/etc/rc.d/rc.local Compatibility
ConditionFileIsExecutable=/etc/rc.d/rc.local
After=network.target

[Service]
Type=forking
ExecStart=/etc/rc.d/rc.local start
TimeoutSec=0
RemainAfterExit=yes
EOF

cat > /etc/rc.d/rc.local <<EOF
#!/bin/bash
# THIS FILE IS ADDED FOR COMPATIBILITY PURPOSES
#
# It is highly advisable to create own systemd services or udev rules
# to run scripts during boot instead of using this file.
#
# In contrast to previous versions due to parallel execution during boot
# this script will NOT be run after all other services.
#
# Please note that you must run 'chmod +x /etc/rc.d/rc.local' to ensure
# that this script will be executed during boot.

touch /var/lock/subsys/local
EOF

# remove non-standard centos 7 service file if detected
if [ -f /etc/systemd/system/rc-local.service ]; then
  # echo "cat /etc/systemd/system/rc-local.service"
  # cat /etc/systemd/system/rc-local.service
  # echo
  rm -rf /etc/systemd/system/rc-local.service
  rm -rf /var/lock/subsys/local
  systemctl daemon-reload >/dev/null 2>&1
  systemctl stop rc-local.service >/dev/null 2>&1
fi

  chmod +x /etc/rc.d/rc.local
  cd /etc; ln -s rc.d/rc.local /etc/rc.local;
  systemctl daemon-reload >/dev/null 2>&1
  systemctl start rc-local.service >/dev/null 2>&1
  systemctl status rc-local.service >/dev/null 2>&1
fi
  if [[ -f /etc/rc.d/rc.local && ! -x /etc/rc.d/rc.local ]]; then
    # centos 7 doesn't give rc.local executable permissions
    # so /etc/rc.local isn't run on reboots like on centos 6
    if [[ "$INITIALINSTALL" = [yY] ]]; then
      ls -lah /etc/rc.d/rc.local
    fi
    chmod +x /etc/rc.d/rc.local
    if [[ "$INITIALINSTALL" = [yY] ]]; then
      ls -lah /etc/rc.d/rc.local
    fi
  fi
}

kernelchecks() {
  if [[ "$(uname -r | cut -d . -f1)" = '4' && "$CHECK_LXD" != [yY] ]]; then
    if [[ "$(cat /proc/sys/net/netfilter/nf_conntrack_helper)" != '0' ]]; then
      echo "linux 4.x kernel related adjustments"
      echo "cat /proc/sys/net/netfilter/nf_conntrack_helper"
      cat /proc/sys/net/netfilter/nf_conntrack_helper
      echo 0 > /proc/sys/net/netfilter/nf_conntrack_helper
      if [[ -z "$(grep 'nf_conntrack_helper' /etc/sysctl.conf)" ]]; then
        echo "net.netfilter.nf_conntrack_helper=0" >> /etc/sysctl.conf
        sysctl -p
      fi
      if [[ -z "$(grep 'nf_conntrack_helper' /etc/rc.local)" ]]; then
        echo "echo 0 > /proc/sys/net/netfilter/nf_conntrack_helper" >> /etc/rc.local
      fi    
      echo "cat /proc/sys/net/netfilter/nf_conntrack_helper"
      cat /proc/sys/net/netfilter/nf_conntrack_helper
    fi  
  fi
}

blockeditorcheck() {
  if [ -f /usr/local/nginx/conf/drop.conf ]; then
  if [[ -z "$(grep 17234 /usr/local/nginx/conf/drop.conf)" ]]; then
    if [ -f "${SCRIPT_DIR}/config/nginx/drop.conf" ]; then
    rm -rf /usr/local/nginx/conf/drop.conf
    \cp -f "${SCRIPT_DIR}/config/nginx/drop.conf" /usr/local/nginx/conf/drop.conf
    fi
  fi
  fi
}

centaltoff() {
  if [[ -f /etc/yum.repos.d/centalt.repo ]]; then
    sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/centalt.repo
    yum clean all -q
  fi
}

forgefix() {
  # remove 404 outdated mirrors
  if [ -f /etc/yum.repos.d/mirrors-rpmforge ]; then
    for l in $(find /etc/yum.repos.d/ -type f -name "mirrors-rpmforge*"); do
      if [[ "$(grep '^http:\/\/apt.sw.be' $l >/dev/null 2>&1; echo $?)" = '0' ]]; then
        sed -i '/^http:\/\/apt.sw.be/d' $l;
      fi
    done
  fi
}

axivo_remove() {
  if [ -f /etc/yum.repos.d/axivo.repo ]; then
    # remove Axivo outdated yum repo
    # if detected as being installed
    # yum list installed --disablerepo=* --enablerepo=axivo --disableplugin=priorities | grep axivo
    time $YUMDNFBIN -y -q remove axivo-release >/dev/null 2>&1
    yum clean all -q
  fi
}

csfipsetcheck() {
  if [[ ! -f /proc/user_beancounters && "$(lsmod | grep ip_set)" ]]; then
    if [[ ! -f /usr/sbin/ipset || ! -f /usr/include/libipset/data.h ]]; then
      time $YUMDNFBIN -y -q install ipset ipset-devel${DISABLEREPO_DNF}
      if [ -f /etc/csf/csf.conf ]; then
      sed -i 's/LF_IPSET = "0"/LF_IPSET = "1"/g' /etc/csf/csf.conf
      fi
    fi
  else
    if [ -f /etc/csf/csf.conf ]; then
    sed -i 's/LF_IPSET = \"1\"/LF_IPSET = \"0\"/' /etc/csf/csf.conf
    fi
  fi
}

checkovzkernels() {
  if [[ -f /proc/user_beancounters ]]; then
    sed -i 's|installonly_limit=.*|installonly_limit=3|' /etc/yum.conf
  fi
}

checkaliases() {
  if [[ "$(grep 'centminmod-123.08centos7beta01' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta01/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta02' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta02/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta03/centminmod/' /root/.bashrc
  fi
  if [[ "$(grep 'centminmod-123.08beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08beta03/centminmod/' /root/.bashrc        
    fi
}

checkcmdircmd() {
  if [[ "$(grep 'cmdir=' /root/.bashrc)" || "$(grep 'centmin=' /root/.bashrc)" ]]; then
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    rm -rf /usr/bin/cmdir
    alias cmdir="pushd ${SCRIPT_DIR}"
    echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; . ./centmin.sh
cleanup_msg() {
  exit 1
}

trap cleanup_msg SIGHUP SIGINT SIGTERM SIGTSTP
EOF
    chmod 0700 /usr/bin/centmin
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ "$(grep 'cmdir=' $HOME/.bashrc)" || "$(grep 'centmin=' $HOME/.bashrc)" ]]; then
      sed -i '/cmdir=/d' $HOME/.bashrc
      sed -i '/centmin=/d' $HOME/.bashrc
      rm -rf /usr/bin/cmdir
      alias cmdir="pushd ${SCRIPT_DIR}"
      echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> $HOME/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; . ./centmin.sh
cleanup_msg() {
  exit 1
}

trap cleanup_msg SIGHUP SIGINT SIGTERM SIGTSTP
EOF
      chmod 0700 /usr/bin/centmin
    fi
  fi
}

nvcheck() {
  if [ ! -h /usr/bin/nv ]; then
    rm -rf /usr/bin/nv
    ln -s "${SCRIPT_DIR}/tools/nv.sh" /usr/bin/nv
    chmod +x /usr/bin/nv
  fi
}

cmupdatecheck() {
  if [ ! -h /usr/bin/cmupdate ]; then
    rm -rf /usr/bin/cmupdate
    ln -s "${SCRIPT_DIR}/tools/cmupdate.sh" /usr/bin/cmupdate
    chmod +x /usr/bin/cmupdate
  fi
}

dmotdcheck() {
  # correct /etc/profile.d/dmotd.sh to only run motd and logging on interactive ssh sessions
  # skip for non-interactive sessions like remote ssh logs that are scripted
  if [[ -f /etc/profile.d/dmotd.sh && -z "$(grep 'tty' /etc/profile.d/dmotd.sh)" ]]; then
    echo '#!/bin/bash' > /etc/profile.d/dmotd.sh
    # echo "if [[ \"\$(id -u)\" = '0' && -f /usr/local/bin/dmotd ]]; then  /usr/local/bin/dmotd; fi" >> /etc/profile.d/dmotd.sh
    echo "if tty -s; then type='interactive'; if [[ \"\$(id -u)\" = '0' && -f /usr/local/bin/dmotd ]]; then /usr/local/bin/dmotd; fi; else type='non-interactive'; fi;" >> /etc/profile.d/dmotd.sh
    echo "if tty -s; then type='interactive'; if [[ \"\$(id -u)\" != '0' && \"\$(id -Gn | grep -wo 'wheel')\" = 'wheel' && -f /usr/local/bin/dmotd ]]; then sudo /usr/local/bin/dmotd; fi; else type='non-interactive'; fi;" >> /etc/profile.d/dmotd.sh
    echo "if echo \$0 | grep -e ^\\- 2>&1 > /dev/null; then login='login'; else login='non-login'; fi;" >> /etc/profile.d/dmotd.sh
    # echo "$type/$login"
  fi
}

multiphpcheck() {
  if [[ -d /usr/local/nginx/conf ]] && [[ ! -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf || ! -f /usr/local/nginx/conf/php-pool5.conf ]]; then
    # check to see if multiple php-fpm pool files exist and copy over if they don't
    # exist on existing server
    if [[ -f "${SCRIPT_DIR}/config/nginx/php-pool5.conf" ]]; then
      \cp -f ${SCRIPT_DIR}/config/nginx/php-pool* /usr/local/nginx/conf/
    fi
    if [[ -f "${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool2.conf" && ! -f /usr/local/nginx/conf/phpfpmd/phpsocket1.conf ]]; then
      \cp -f ${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool* /usr/local/nginx/conf/phpfpmd/
      if [[ "$USEEDITOR" = 'vim' ]]; then
          echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
          echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
          echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
          echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
      else
          echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
          echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
          echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
          echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
      fi # USEEDITOR      
    fi
  fi

  if [[ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf && ! -f /usr/bin/fpmconf-2 ]]; then
    if [[ "$USEEDITOR" = 'vim' ]]; then
        echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
        echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
        echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
        echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
    else
        echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
        echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
        echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
        echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
    fi # USEEDITOR  
  fi  
}

mjemalloc() {
    if [[ "$MARIADB_JEMALLOC" = [yY] && ! "$(grep libjemalloc /usr/bin/mysqld_safe)" && -f /usr/lib64/libjemalloc.so.1 ]]; then
        # echo "switching MariaDB to jemalloc malloc method"
        sed -i 's|^mysqld_ld_preload=|mysqld_ld_preload=/usr/lib64/libjemalloc.so.1|' /usr/bin/mysqld_safe
        # service mysql restart
        # pmap `pidof mysqld` | grep jemalloc
    fi    
}

fixlogrotate() {
  if [ -f /etc/logrotate.d/nginx ]; then
  if [[ -z "$(grep 'maxsize' /etc/logrotate.d/nginx)" || "$(grep -w 'su nginx' /etc/logrotate.d/nginx)" ]]; then
    # sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/usr\/local\/nginx\/logs\/nginx.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/nginx
  if [[ "$CENTOS_SEVEN" = '7' ]]; then
    VARDFSIZE=$(df --output=avail /var | tail -1)
  else
    VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
  fi
if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
else
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
fi
  fi
  fi

  if [ -f /etc/logrotate.d/php-fpm ]; then
  if [[ -z "$(grep '\$' /etc/logrotate.d/php-fpm)" || -z "$(grep 'maxsize' /etc/logrotate.d/php-fpm)" ]]; then

  if [[ "$CENTOS_SEVEN" = '7' ]]; then
    VARDFSIZE=$(df --output=avail /var | tail -1)
  else
    VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
  fi

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
else
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
fi
  fi
  fi

if [[ ! -f /etc/logrotate.d/mysql || ! -f /etc/logrotate.d/mysql-slowlog || ! -f /var/log/mysqld.log ]]; then
  funct_logmysqlrotate silent
fi

if [[ -f /etc/logrotate.d/mysql && ! "$(grep 'dateext' /etc/logrotate.d/mysql)" ]]; then
  funct_logmysqlrotate silent
fi
}

csf_distftp() {
  if [ -f /etc/csf/csf.conf ]; then
    # raise LF_DISTFTP = 40 & LF_DISTFTP_UNIQ = 40
    if [[ ! "$(grep 'LF_DISTFTP = \"40\"' /etc/csf/csf.conf)" ]]; then
      sed -i 's/LF_DISTFTP = .*/LF_DISTFTP = \"40\"/g' /etc/csf/csf.conf
    fi
    if [[ ! "$(grep 'LF_DISTFTP_UNIQ = \"40\"' /etc/csf/csf.conf)" ]]; then
      sed -i 's/LF_DISTFTP_UNIQ = .*/LF_DISTFTP_UNIQ = \"40\"/g' /etc/csf/csf.conf
      csf -ra >/dev/null 2>&1
    fi
  fi
}

pureftpdupdates() {
  if [ -f /etc/pure-ftpd/pure-ftpd.conf ]; then
    if [ "$SECOND_IP" ]; then
      CNIP="$SECOND_IP"
    else
      CNIP=$(curl -4s https://ipinfo.io/ip)
    fi
    # disable anonymous logins by default
    CHECKPFTPD_ANON=$(grep 'NoAnonymous                 no' /etc/pure-ftpd/pure-ftpd.conf)
    if [ "$CHECKPFTPD_ANON" ]; then
      sed -i 's|NoAnonymous                 no|NoAnonymous                 yes|' /etc/pure-ftpd/pure-ftpd.conf
      if [[ "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        service pure-ftpd restart >/dev/null 2>&1
      fi
      echo
      echo "disable pure-ftpd annonymous logins"
      echo
    fi
    # auto updates
    # raise max number of concurrent ftp connections from 50/2 to 20010/2
    CHECKPFTPD_PASSIVEPORTS=$(grep '^PassivePortRange    3000 3050' /etc/pure-ftpd/pure-ftpd.conf)
    CHECKPFTPD_MAXCLIENTS=$(grep '^MaxClientsNumber            1000' /etc/pure-ftpd/pure-ftpd.conf)
    if [ "$CHECKPFTPD_PASSIVEPORTS" ]; then
     sed -i 's/^PassivePortRange    3000 3050/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
    fi
    if [ ! "$CHECKPFTPD_MAXCLIENTS" ]; then
      sed -i 's|^MaxClientsNumber .*|MaxClientsNumber            1000|' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|^MaxClientsPerIP .*|MaxClientsPerIP             500|' /etc/pure-ftpd/pure-ftpd.conf
    fi
      if [[ "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        service pure-ftpd restart >/dev/null 2>&1
      fi
      # echo
      # echo "increasing pure-ftpd passive port range"
      # echo
    CHECKCSF_PASSIVEPORTS=$(grep '3000:3050' /etc/csf/csf.conf)
    if [ "$CHECKCSF_PASSIVEPORTS" ]; then
      sed -i 's/3000:3050/30001:50011/' /etc/csf/csf.conf
      csf -ra >/dev/null 2>&1
      echo
      echo "adjusting pure-ftpd passive port range for CSF Firewall"
      echo
    fi  
  fi
  if [ ! -d /etc/ssl/private ]; then
    mkdir -p /etc/ssl/private
  fi
  if [[ "$INITIALINSTALL" != [yY] && ! -f /etc/ssl/private/pure-ftpd-dhparams.pem ]]; then
    echo "pure-ftpd: one time setup /etc/ssl/private/pure-ftpd-dhparams.pem file"
    echo "may take a while to create..."
    openssl dhparam -out /etc/ssl/private/pure-ftpd-dhparams.pem 2048 >/dev/null 2>&1
    echo "pure-ftpd: /etc/ssl/private/pure-ftpd-dhparams.pem created"
    if [[ "$PUREFTPD_DISABLED" != [yY] && "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
      service pure-ftpd restart >/dev/null 2>&1
    elif [[ "$PUREFTPD_DISABLED" != [yY] && "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
      service pure-ftpd restart >/dev/null 2>&1
    fi
  fi
  if [[ "$INITIALINSTALL" != [yY] && -f /etc/pki/pure-ftpd/pure-ftpd.pem && "$(openssl x509 -in /etc/pki/pure-ftpd/pure-ftpd.pem -text -noout | grep '1024 bit')" ]]; then
    echo "pure-ftpd: update & re-create 2048bit /etc/pki/pure-ftpd/pure-ftpd.pem file for $CNIP"
    openssl req -x509 -days 7300 -sha256 -nodes -subj "/C=US/ST=California/L=Los Angeles/O=Default Company Ltd/CN=$CNIP" -newkey rsa:2048 -keyout /etc/pki/pure-ftpd/pure-ftpd.pem -out /etc/pki/pure-ftpd/pure-ftpd.pem >/dev/null 2>&1
    echo "pure-ftpd: /etc/pki/pure-ftpd/pure-ftpd.pem created"
    if [[ "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
      service pure-ftpd restart >/dev/null 2>&1
    fi
  elif [[ "$INITIALINSTALL" != [yY] && ! -f /etc/pki/pure-ftpd/pure-ftpd.pem ]]; then
    echo "pure-ftpd: create 2048bit /etc/pki/pure-ftpd/pure-ftpd.pem file for $CNIP"
    openssl req -x509 -days 7300 -sha256 -nodes -subj "/C=US/ST=California/L=Los Angeles/O=Default Company Ltd/CN=$CNIP" -newkey rsa:2048 -keyout /etc/pki/pure-ftpd/pure-ftpd.pem -out /etc/pki/pure-ftpd/pure-ftpd.pem >/dev/null 2>&1
    echo "pure-ftpd: /etc/pki/pure-ftpd/pure-ftpd.pem created"
    if [[ "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
      service pure-ftpd restart >/dev/null 2>&1
    fi
  fi
  # fix bug https://community.centminmod.com/posts/71920/
  if [[ "$INITIALINSTALL" != [yY] && -f /etc/pure-ftpd/pure-ftpd.conf ]]; then
    check_pureftpd_verboselog=$(awk '/VerboseLog/ {print $2}' /etc/pure-ftpd/pure-ftpd.conf)
    check_pureftpd_tls=$(awk '/^TLS / {print $2}' /etc/pure-ftpd/pure-ftpd.conf)
    check_pureftpd_portrange=$(awk '/PassivePortRange / {print $3}' /etc/pure-ftpd/pure-ftpd.conf)
    # echo "check_pureftpd_tls = $check_pureftpd_tls"
    # echo "check_pureftpd_verboselog = $check_pureftpd_verboselog"
    # echo "check_pureftpd_portrange = $check_pureftpd_portrange"
    
    if [[ "$check_pureftpd_tls" != '2' || "$check_pureftpd_verboselog" != 'yes' ]] && [[ "$check_pureftpd_portrange" != '50011' ]]; then
      echo "fix pure-ftpd updated configuration"
      echo "https://community.centminmod.com/posts/71920/"
      echo "backup existing config at /etc/pure-ftpd/pure-ftpd.conf.backup.$DT"
      # backup original /etc/pure-ftpd/pure-ftpd.conf
      cp -a /etc/pure-ftpd/pure-ftpd.conf "/etc/pure-ftpd/pure-ftpd.conf.backup.$DT"
      # tweak /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/# UnixAuthentication  /UnixAuthentication  /' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/VerboseLog .*/VerboseLog yes/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|# PureDB user |# PureDB-user |' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/# PureDB .*/PureDB                        \/etc\/pure-ftpd\/pureftpd.pdb/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/# CreateHomeDir .*/CreateHomeDir               yes/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/# TLS .*/TLS                      2/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/# PassivePortRange .*/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's/^PassivePortRange    3000 3050/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|MaxClientsNumber .*|MaxClientsNumber            1000|' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|MaxClientsPerIP .*|MaxClientsPerIP             500|' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|NoAnonymous .*|NoAnonymous                 yes|' /etc/pure-ftpd/pure-ftpd.conf
      # fix default file/directory permissions
      sed -i 's/Umask .*/Umask                       137:027/' /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|# TLSCipherSuite .*|TLSCipherSuite               HIGH|' /etc/pure-ftpd/pure-ftpd.conf
      sdiff -s -w200 /etc/pure-ftpd/pure-ftpd.conf "/etc/pure-ftpd/pure-ftpd.conf.backup.$DT"
      cat /etc/pure-ftpd/pure-ftpd.conf | egrep 'UnixAuthentication|VerboseLog|PureDB |CreateHomeDir|TLS|PassivePortRange|TLSCipherSuite|MaxClientsNumber|MaxClientsPerIP|NoAnonymous|Umask'
      if [[ "$(ps aufx | grep pure-ftpd | grep -v grep | grep pure-ftpd  >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        service pure-ftpd restart >/dev/null 2>&1
      fi
    fi
  fi
}

leupdates() {
  # update .well-known content-type
  if [ -f /usr/local/nginx/conf/staticfiles.conf ]; then
   CHECKCONTENTTYPE=$(grep 'application\/jose+json' /usr/local/nginx/conf/staticfiles.conf)
   if [ "$CHECKCONTENTTYPE" ]; then
      sed -i "s|application\/jose+json|text\/plain|" /usr/local/nginx/conf/staticfiles.conf
   fi
   WELLKNOWN_CHECK=$(grep '.well-known' /usr/local/nginx/conf/staticfiles.conf)
   if [ ! "$WELLKNOWN_CHECK" ]; then
      { echo -e "    # prepare for letsencrypt\n    # https://community.centminmod.com/posts/17774/\n    location ~ /.well-known { location ~ /.well-known/acme-challenge/(.*) { more_set_headers    "Content-Type: text/plain"; } }\n"; cat /usr/local/nginx/conf/staticfiles.conf; } > /usr/local/nginx/conf/staticfiles.conf.tmp
      mv -f /usr/local/nginx/conf/staticfiles.conf.tmp /usr/local/nginx/conf/staticfiles.conf
   fi
  fi
}

memcachedupdatechecks() {
  # 1.4.25 added options https://community.centminmod.com/threads/memcached-1-4-25-released.5007/
  if [[ "$INITIALINSTALL" != [yY] ]]; then
    if [ -f /etc/init.d/memcached ]; then
      if [[ -z "$(grep 'MEM_CURVER' /etc/init.d/memcached)" ]]; then
        if [[ -f "${SCRIPT_DIR}/config/memcached/memcached1425.patch" && -f /etc/init.d/memcached ]]; then
          \cp -f "${SCRIPT_DIR}/config/memcached/memcached1425.patch" /etc/init.d/memcached1425.patch
          pushd /etc/init.d/
          patch -s < memcached1425.patch >/dev/null 2>&1
          rm -rf memcached1425.patch
          popd
        fi
      fi
      if [[ -z "$(grep 'modern' /etc/init.d/memcached)" ]]; then
        if [[ -f "${SCRIPT_DIR}/config/memcached/memcached1426.patch" && -f /etc/init.d/memcached ]]; then
          \cp -f "${SCRIPT_DIR}/config/memcached/memcached1426.patch" /etc/init.d/memcached1426.patch
          pushd /etc/init.d/
          patch -s < memcached1426.patch >/dev/null 2>&1
          rm -rf memcached1426.patch
          popd
        fi
      fi
      if [ ! "$(grep -w 'memcached' /etc/passwd)" ]; then
        echo "Update memcached server setup"
        echo "Adding memcached user/group and adding to nginx group"
        adduser -s /sbin/nologin -M memcached >/dev/null
        usermod -G nginx memcached >/dev/null
        id memcached
        SOCKETPATCH=y
      elif [[ "$(grep -w 'memcached' /etc/passwd)" && "$(grep -w 'nginx' /etc/passwd)" ]]; then
        if [[ "$(id memcached | grep -o nginx)" != 'nginx' ]]; then
          usermod -G nginx memcached >/dev/null
          id memcached
          SOCKETPATCH=y
        fi
      fi
      if [[ "$(grep -w "USER=nobody" /etc/init.d/memcached)" && "$SOCKETPATCH" = [Yy] ]]; then
          echo "Changing memcached server user from nobody to memcached"
          sed -i 's|USER=nobody|USER=memcached|' /etc/init.d/memcached
          grep -w "USER=memcached" /etc/init.d/memcached
          echo "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch"
          if [[ -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" && -f /etc/init.d/memcached ]]; then
              \cp -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" /etc/init.d/memcached-socketfix.patch
              pushd /etc/init.d/
              patch -s < memcached-socketfix.patch >/dev/null 2>&1
              rm -rf memcached-socketfix.patch
              if [[ "$CENTOS_SEVEN" = '7' ]]; then
                systemctl daemon-reload
              fi
              if [ "$(ps -C memcached | grep -w memcached)" ]; then
                  /usr/bin/memcachedstop >/dev/null
                  /usr/bin/memcachedstart >/dev/null
              fi
              popd
          fi
      elif [[ "$SOCKETPATCH" = [Yy] ]]; then
          echo "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch"
          if [[ -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" && -f /etc/init.d/memcached ]]; then
              \cp -f "${SCRIPT_DIR}/config/memcached/memcached-socketfix.patch" /etc/init.d/memcached-socketfix.patch
              pushd /etc/init.d/
              patch -s < memcached-socketfix.patch >/dev/null 2>&1
              rm -rf memcached-socketfix.patch
              if [[ "$CENTOS_SEVEN" = '7' ]]; then
                systemctl daemon-reload
              fi
              if [ "$(ps -C memcached | grep -w memcached)" ]; then
                  /usr/bin/memcachedstop >/dev/null
                  /usr/bin/memcachedstart >/dev/null
              fi
              popd
          fi
      fi
      if [[ ! "$(grep -w '\-a 0766' /etc/init.d/memcached)" ]]; then
        sed -i 's|0775|0766|g' /etc/init.d/memcached
      fi # 766 fix
    fi # init.d check
  fi
}

cronpathadjust() {
  # if [[ "$INITIALINSTALL" = [yY] ]]; then
    # append /usr/local/bin to /etc/crontab PATH variable for /usr/local/bin/php to work
    if [ -f /etc/crontab ]; then
      if [[ -z "$(grep '\/usr\/local\/bin' /etc/crontab)" ]]; then
        # echo "adjust /etc/crontab \$PATH for /usr/local/bin support"
        sed -i 's|PATH=\/sbin:\/bin:\/usr\/sbin:\/usr\/bin|PATH=\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/usr\/local\/bin|' /etc/crontab
        cmservice crond restart >/dev/null 2>&1
      fi
    fi
  # fi
}

crontabdisable_r() {
  if [[ ! "$(grep -w 'crontab \(\)' /root/.bashrc)" ]]; then
    # disable crontab -r commmand https://community.centminmod.com/threads/strange-cancellation-of-the-cron.14059/#post-59929
    echo 'crontab () { [[ $@ =~ -[iel]*r ]] && echo '"r" not allowed' || command crontab "$@" ;}' >> /root/.bashrc
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ ! "$(grep -w 'crontab \(\)' $HOME/.bashrc)" ]]; then
      # disable crontab -r commmand https://community.centminmod.com/threads/strange-cancellation-of-the-cron.14059/#post-59929
      echo 'crontab () { [[ $@ =~ -[iel]*r ]] && echo '"r" not allowed' || command crontab "$@" ;}' >> $HOME/.bashrc
    fi
  fi
}

xpowerby() {
  # add x-powered-by header check
  if [ -f /usr/local/nginx/conf/nginx.conf ]; then
  XPB_CHECKA=$(grep 'more_set_headers \"Server: nginx\";' /usr/local/nginx/conf/nginx.conf)
  XPB_CHECKB=$(grep 'more_set_headers \"X-Powered-By: centminmod\";' /usr/local/nginx/conf/nginx.conf)
  XPB_CHECKC=$(grep 'more_set_headers \"Server: nginx centminmod\";' /usr/local/nginx/conf/nginx.conf)
  if [[ "$XPB_CHECKA" ]]; then
    sed -i "s|^more_set_headers \"Server: nginx\";|more_set_headers \"Server: nginx\";\nmore_set_headers \"X-Powered-By: centminmod\";|" /usr/local/nginx/conf/nginx.conf
  elif [[ -z "$XPB_CHECKB" ]]; then
    if [[ "$XPB_CHECKC" ]]; then
      sed -i "s|^more_set_headers \"Server: nginx centminmod\";|more_set_headers \"Server: nginx centminmod\";\nmore_set_headers \"X-Powered-By: centminmod\";|" /usr/local/nginx/conf/nginx.conf
    else
      sed -i "s|include \/usr\/local\/nginx\/conf\/vts_http.conf;|more_set_headers \"X-Powered-By: centminmod\";\n\ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;|" /usr/local/nginx/conf/nginx.conf
    fi
  fi
  fi
}

pathfixes() {
  if [[ -z "$(grep '\$PATH:' /root/.bashrc)" ]]; then
    # sed -i "s|export PATH=\"\/usr\/lib64\/ccache|export PATH=\"\$PATH:\/usr\/lib64\/ccache|" /root/.bashrc
    sed -i 's|export PATH=.*|export PATH="\$PATH:\/usr\/lib64\/ccache:\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin"|' /root/.bashrc
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ -z "$(grep '\$PATH:' $HOME/.bashrc)" ]]; then
      # sed -i "s|export PATH=\"\/usr\/lib64\/ccache|export PATH=\"\$PATH:\/usr\/lib64\/ccache|" $HOME/.bashrc
      sed -i 's|export PATH=.*|export PATH="\$PATH:\/usr\/lib64\/ccache:\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin"|' $HOME/.bashrc
    fi
  fi
  # for sudo
  if [ -d /etc/sudoers.d ]; then
    if [ ! -f /etc/sudoers.d/addpaths ]; then
      touch /etc/sudoers.d/addpaths
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [[ "$(grep '\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin' /etc/sudoers.d/addpaths >/dev/null; echo $?)" != '0' ]]; then
          echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
        fi
      else
        if [[ "$(grep '\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin' /etc/sudoers.d/addpaths >/dev/null; echo $?)" != '0' ]]; then
          echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
        fi
      fi
    fi
    if [ -f /etc/sudoers.d/addpaths ]; then
      chmod 0440 /etc/sudoers.d/addpaths
      # visudo -c -q
    fi
  fi
}

sshdfixes() {
  # forgot line break in sshd_config for centos 6.x systems
  # centos 6.x openssh 5.3p1 backported ecdsa keys support but
  # not configured out of the box https://community.centminmod.com/posts/19702/
  # this sets up ecdh-sha2-* key exchange support but setup without a line break
  # this fixes it
  if [[ "$(grep 'UseDNS noKexAlgorithms' /etc/ssh/sshd_config)" ]]; then
    sed -i 's|UseDNS noKexAlgorithms|UseDNS no\nKexAlgorithms|' /etc/ssh/sshd_config
  fi
  if [[ "$CENTOS_SEVEN" -eq '7' && "$(egrep '^PrintMotd noKexAlgorithms' /etc/ssh/sshd_config)" ]]; then
    # fix https://community.centminmod.com/posts/75607/
    sed -i 's|^PrintMotd noKexAlgorithms|PrintMotd no\nKexAlgorithms|' /etc/ssh/sshd_config
  fi
}

auto_gitupdate() {
  # if centmin mod code install directory has been setup
  # with git environment via centmin.sh menu option 23
  # submenu option 1, then allow centmin.sh to auto update
  # the centmin mod code at /usr/local/src/centminmod
  # silently in background
  if [[ -d "${SCRIPT_DIR}/.git" && "$AUTO_GITUPDATE" = [yY] ]]; then
    cd ${SCRIPT_DIR}
    # totally silence the output
    git stash -q &> /dev/null
    git fetch -q &> /dev/null
    git pull -q &> /dev/null
  fi
}

figletcheck() {
    if [[ ! -f "$(rpm -ql figlet | grep 'bin\/figlet')" && -f /etc/yum.repos.d/epel.repo ]]; then
        time $YUMDNFBIN -y -q install figlet${DISABLEREPO_DNF}
    fi
}

maintenance_confcheck() {
  if [ ! -f /usr/local/nginx/conf/sitestatus.conf ]; then
    if [[ -f "${SCRIPT_DIR}/config/nginx/sitestatus.conf" && -d /usr/local/nginx/conf ]]; then
      \cp -f "${SCRIPT_DIR}/config/nginx/sitestatus.conf" /usr/local/nginx/conf/sitestatus.conf
    fi
  fi
  if [ ! -f /usr/local/nginx/conf/maintenance.conf ]; then
    if [[ -f "${SCRIPT_DIR}/config/nginx/maintenance.conf" && -d /usr/local/nginx/conf ]]; then
      \cp -f "${SCRIPT_DIR}/config/nginx/maintenance.conf" /usr/local/nginx/conf/maintenance.conf
    fi
  fi
  if [ ! -f /usr/local/nginx/conf/503include-main.conf ]; then
    if [[ -f "${SCRIPT_DIR}/config/nginx/503include-main.conf" && -d /usr/local/nginx/conf ]]; then
      \cp -f "${SCRIPT_DIR}/config/nginx/503include-main.conf" /usr/local/nginx/conf/503include-main.conf
    fi
  fi
  if [ ! -f /usr/local/nginx/conf/503include-only.conf ]; then
    if [[ -f "${SCRIPT_DIR}/config/nginx/503include-only.conf" && -d /usr/local/nginx/conf ]]; then
      \cp -f "${SCRIPT_DIR}/config/nginx/503include-only.conf" /usr/local/nginx/conf/503include-only.conf
    fi
  fi
  if [ ! -f /usr/bin/sitestatus ]; then
    if [[ -f "${SCRIPT_DIR}/tools/sitestatus.sh" && -d /usr/local/nginx/conf ]]; then
      \cp -f "${SCRIPT_DIR}/tools/sitestatus.sh" /usr/bin/sitestatus
      chmod +x /usr/bin/sitestatus
      MCONF_CHECK=$(grep 'maintenance.conf' /usr/local/nginx/conf/nginx.conf)
      if [[ -z "$MCONF_CHECK" ]]; then
            sed -i 's/include \/usr\/local\/nginx\/conf\/geoip.conf;/include \/usr\/local\/nginx\/conf\/geoip.conf;\ninclude \/usr\/local\/nginx\/conf\/maintenance.conf;\n/g' /usr/local/nginx/conf/nginx.conf
      fi
    fi
  fi
}

ngxmodule_cleanups() {
  # clean ups
  if [ -f "$DIR_TMP/nginx-develkit_v0.2.19.tar.gz" ]; then
    rm -rf "$DIR_TMP/nginx-develkit_v0.2.19.tar.gz"
  fi
  if [ -d "$DIR_TMP/ngx_devel_kit-0.2.19" ]; then
    rm -rf "$DIR_TMP/ngx_devel_kit-0.2.19"
  fi
}

wpcompfix() {
  # for wordpress auto installs via centmin.sh menu option 22, the wpsecure include
  # file needs an added deny all location context to prevent compressed files in
  # wordpress plugins that save to wp-content subdirectory from public access
  if [ -d /usr/local/nginx/conf ]; then
    WPSECUREDIRS=$(find /usr/local/nginx/conf/ -type f -name "wpsecure_*.conf")
    if [[ "$WPSECUREDIRS" ]]; then
      for wsf in ${WPSECUREDIRS[@]}; do
        if [[ -z "$(grep 'zip|gz|tar|bzip2|7z|txt' $wsf)" ]]; then
          echo "location ~* /(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z|txt)\$ { deny all; }" >> "$wsf";
        fi
      done
    fi
  fi
}

entropy_centosseven() {
if [[ -f /usr/lib/systemd/system/haveged.service ]] && [[ "$CENTOS_SEVEN" = '7' ]]; then
  if [[ ! -d /etc/systemd/system/haveged.service.d || ! -f /etc/systemd/system/haveged.service.d/haveged.conf ]]; then
    mkdir -p /etc/systemd/system/haveged.service.d
cat > "/etc/systemd/system/haveged.service.d/haveged.conf" <<EFF
[Service]
ExecStart=
ExecStart=/usr/sbin/haveged -w 4067 -v 1 --Foreground
EFF
    systemctl daemon-reload >/dev/null 2>&1
    systemctl restart haveged >/dev/null 2>&1
    # cat /etc/systemd/system/haveged.service.d/haveged.conf
  fi
fi
}

ovhkernelcheck() {
  if [[ "$INITIALINSTALL" = [yY] ]]; then
    # check for new installs only whether server is OVH based 
    # and whether a custom kernel is used with exception for
    # Intel Xeon D-15xx Broadwell-DE processors which need
    # OVH custom kernel for the newer Intel network NIC support
    # Intel Corporation Ethernet Connection X552/X557-AT 10GBASE-T
    OVHASNCHECK=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/org | grep -o 'OVH' >/dev/null 2>&1; echo $?)
    OVHKERNEL_CHECK=$(uname -r | grep '\-grs' >/dev/null 2>&1; echo $?)
    BROADWELLDE_CHECK=$(grep 'Xeon(R) CPU D-15' /proc/cpuinfo >/dev/null 2>&1; echo $?)
    # more sure way to check for OVH custom kernel if /root/.ovhrc file exists
    # can check value of variable DISTRIBKERNEL=1 for distro kernel or 
    # DISTRIBKERNEL=0 for custom OVH kernel and let it override ASN check too
    if [ -f /root/.ovhrc ]; then
      OVHRC_CHECK=$(awk -F "\"" '/DISTRIBKERNEL/ {print $2}' /root/.ovhrc)
      # check for both ovh dedicated and ovh vps which has empty OVHRC_CHECK values
      # if /root/.ovhrc exists = ovh based
      # if DISTRIBKERNEL value = 1 = distro kernel and not empty
      # if OVHRC_CHECK is empty = ovh based non-dedicated/vps
      if [[ "$OVHRC_CHECK" = '1' ]]; then
        # ovh dedicated with distro kernel
        OVHASNCHECK='1'
        OVHKERNEL_CHECK='1'
      elif [[ "$OVHRC_CHECK" != '1' && -z "$OVHRC_CHECK" ]]; then
        # ovh vps
        OVHASNCHECK='1'
        OVHKERNEL_CHECK='1'
      elif [[ "$OVHRC_CHECK" != '1' && ! -z "$OVHRC_CHECK" ]]; then
        OVHASNCHECK='0'
        OVHKERNEL_CHECK='0'
      fi    
    fi
    if [[ "$OVHASNCHECK" = '0' && "$OVHKERNEL_CHECK" = '0' && "$BROADWELLDE_CHECK" != '0' ]]; then
      echo
      echo "Detected OVH based server with custom OVH Linux kernel"
      echo "It's best to use CentOS distro default Linux Kernels"
      echo "aborting script..."
      exit
    fi
  fi
}

tcpcheck_centos() {
  # lnstat -c -1 -f nf_conntrack
  TCPMEMTOTAL=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  # mem usage ~ NF_CTMAX x 288 bytes
  # mem usage ~ NF_CTHASHSIZE x 8 bytes
  if [ "$TCPMEMTOTAL" -le '385000' ]; then
    NF_CTMAX='16384'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '385000' && "$TCPMEMTOTAL" -le '770000' ]]; then
    NF_CTMAX='65536'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '770000' && "$TCPMEMTOTAL" -le '1049000' ]]; then
    NF_CTMAX='131072'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '1049000' && "$TCPMEMTOTAL" -le '2098000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '2098000' && "$TCPMEMTOTAL" -le '3147000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '3147000' && "$TCPMEMTOTAL" -le '4196000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '4196000' && "$TCPMEMTOTAL" -le '8392000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '8392000' && "$TCPMEMTOTAL" -le '16784000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '16784000' && "$TCPMEMTOTAL" -le '33568000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '33568000' && "$TCPMEMTOTAL" -le '67136000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '67136000' && "$TCPMEMTOTAL" -le '134272000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '134272000' && "$TCPMEMTOTAL" -le '268544000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [ "$TCPMEMTOTAL" -gt '268544000' ]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  fi

if [[ ! -f /proc/user_beancounters ]]; then
    # create /etc/sysctl.d/101-sysctl.conf if doesn't exist on centos 7
    if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/sysctl.d/101-sysctl.conf ]]; then
        if [ -d /etc/sysctl.d ]; then
            if [[ ! -z "$NF_CTHASHSIZE" && -f /sys/module/nf_conntrack/parameters/hashsize ]]; then
              echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
            fi
            if [[ ! -z "$NF_CTHASHSIZE" && "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
                echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi

            # centos 7
            touch /etc/sysctl.d/101-sysctl.conf
            if [[ "$(grep 'centminmod added' /etc/sysctl.d/101-sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
cat >> "/etc/sysctl.d/101-sysctl.conf" <<EOF
# centminmod added
kernel.pid_max=65536
kernel.printk=4 1 1 7
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=65536
net.core.somaxconn=65535
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=65536
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 0
net.ipv4.tcp_tw_reuse = 0
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_limit_output_bytes=65536
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.netfilter.nf_conntrack_max = $NF_CTMAX
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024
net.unix.max_dgram_qlen = 4096
EOF
        /sbin/sysctl --system >/dev/null 2>&1
            fi           
        fi
    else
        # centos 6 check for missing new tcp settings backported to existing installs
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" = '0' ]]; then
            if [[ ! -z "$NF_CTHASHSIZE" && -f /sys/module/nf_conntrack/parameters/hashsize ]]; then
              # raise hashsize for conntrack entries
              echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
              if [[ ! -z "$NF_CTHASHSIZE" && "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
                echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
              fi
            fi
            if [[ "$(grep 'net.ipv4.tcp_slow_start_after_idle=0' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then  
              echo "net.ipv4.tcp_slow_start_after_idle=0" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.ipv4.tcp_limit_output_bytes=65536' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then 
              echo "net.ipv4.tcp_limit_output_bytes=65536" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.ipv4.tcp_rfc1337=1' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then  
              echo "net.ipv4.tcp_rfc1337=1" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_helper=0' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then 
              echo "net.netfilter.nf_conntrack_helper=0" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_max = 524288' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "net.netfilter.nf_conntrack_max = 524288" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_tcp_timeout_established = 28800' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "net.netfilter.nf_conntrack_tcp_timeout_established = 28800" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_generic_timeout = 60' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "net.netfilter.nf_conntrack_generic_timeout = 60" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.ipv4.tcp_mtu_probing = 1' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "net.ipv4.tcp_mtu_probing = 1" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.ipv4.tcp_base_mss = 1024' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "net.ipv4.tcp_base_mss = 1024" >> /etc/sysctl.conf
            fi
            sysctl -p >/dev/null 2>&1
        fi
    fi
fi
if [[ ! -f /proc/user_beancounters ]]; then
  # fix for https://community.centminmod.com/posts/34527/
  # CVE-2016-5696
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ -z "$(grep 'tcp_challenge_ack_limit' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_challenge_ack_limit = 999999999" >> /etc/sysctl.d/101-sysctl.conf
      /sbin/sysctl --system >/dev/null 2>&1
    fi
    # raise centos 7 default net.unix.max_dgram_qlen from 512 to 4096
    if [[ -z "$(grep 'net.unix.max_dgram_qlen' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo "net.unix.max_dgram_qlen = 4096" >> /etc/sysctl.d/101-sysctl.conf
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SIX" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ -z "$(grep 'tcp_challenge_ack_limit' /etc/sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_challenge_ack_limit = 999999999" >> /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
fi
if [[ ! -f /proc/user_beancounters ]]; then
  # https://blog.cloudflare.com/path-mtu-discovery-in-practice/
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ -z "$(grep 'net.ipv4.tcp_mtu_probing' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_mtu_probing = 1" >> /etc/sysctl.d/101-sysctl.conf
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SIX" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ -z "$(grep 'net.ipv4.tcp_mtu_probing' /etc/sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_mtu_probing = 1" >> /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
fi
if [[ ! -f /proc/user_beancounters ]]; then
  # https://blog.cloudflare.com/path-mtu-discovery-in-practice/
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ -z "$(grep 'net.ipv4.tcp_base_mss' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_base_mss = 1024" >> /etc/sysctl.d/101-sysctl.conf
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SIX" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ -z "$(grep 'net.ipv4.tcp_base_mss' /etc/sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_base_mss = 1024" >> /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
fi
if [[ ! -f /proc/user_beancounters ]]; then
  # fix for https://community.centminmod.com/posts/35910/
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ "$(sysctl -a 2>&1 | awk '/net.ipv4.tcp_tw_recycle/ {print $3}')" = '1' ]]; then
      sed -i 's|net.ipv4.tcp_tw_recycle.*|net.ipv4.tcp_tw_recycle = 0|' /etc/sysctl.d/101-sysctl.conf
      if [ -f /etc/sysctl.conf ]; then
        sed -i 's|net.ipv4.tcp_tw_recycle.*|net.ipv4.tcp_tw_recycle = 0|' /etc/sysctl.conf
      fi
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SIX" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ "$(sysctl -a 2>&1 | awk '/net.ipv4.tcp_tw_recycle/ {print $3}')" = '1' ]]; then
      sed -i 's|net.ipv4.tcp_tw_recycle.*|net.ipv4.tcp_tw_recycle = 0|' /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
fi
if [[ ! -f /proc/user_beancounters ]]; then
  # fix for https://community.centminmod.com/posts/35922/
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ "$(sysctl -a 2>&1 | awk '/net.ipv4.tcp_tw_reuse/ {print $3}')" = '1' ]]; then
      sed -i 's|net.ipv4.tcp_tw_reuse.*|net.ipv4.tcp_tw_reuse = 0|' /etc/sysctl.d/101-sysctl.conf
      if [ -f /etc/sysctl.conf ]; then
        sed -i 's|net.ipv4.tcp_tw_reuse.*|net.ipv4.tcp_tw_reuse = 0|' /etc/sysctl.conf
      fi
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SIX" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ "$(sysctl -a 2>&1 | awk '/net.ipv4.tcp_tw_reuse/ {print $3}')" = '1' ]]; then
      sed -i 's|net.ipv4.tcp_tw_reuse.*|net.ipv4.tcp_tw_reuse = 0|' /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
  fi
fi
}

remi_tweaks() {
  if [ -f /etc/yum.repos.d/remi-safe.repo ]; then
    sed -i 's|enabled=1|enabled=0|' /etc/yum.repos.d/remi-safe.repo
  fi
}

imagick_fixes() {
  if [[ -f /etc/ImageMagick/policy.xml || -f /etc/ImageMagick-6/policy.xml ]]; then
    if [ -f "${SCRIPT_DIR}/tools/imagemagick-fix.sh" ]; then
      "${SCRIPT_DIR}/tools/imagemagick-fix.sh" >/dev/null 2>&1
    fi
  fi
}

apache_protect() {
  if [[ -z "$(crontab -l 2>&1 | grep '\/usr\/local\/src\/centminmod\/tools\/autoprotect.sh')" && -f "/usr/local/src/centminmod/tools/autoprotect.sh" ]]; then
      crontab -l > cronjoblist
      sed -i '/autoprotect.sh/d' cronjoblist
      echo "13 23 * * * /usr/local/src/centminmod/tools/autoprotect.sh >/dev/null 2>&1" >> cronjoblist
      crontab cronjoblist
      rm -rf cronjoblist
  elif [[ "$(crontab -l | grep -q '11 \*\/23'; echo $?)" -eq '0' && -f "/usr/local/src/centminmod/tools/autoprotect.sh" ]]; then
      crontab -l > cronjoblist
      sed -i '/autoprotect.sh/d' cronjoblist
      echo "13 23 * * * /usr/local/src/centminmod/tools/autoprotect.sh >/dev/null 2>&1" >> cronjoblist
      crontab cronjoblist
      rm -rf cronjoblist 
  fi
}

centossix_crsetup() {
  # centos <=6.7 openssl fix https://community.centminmod.com/posts/31166/
  if [[ ! -f /etc/yum.repos.d/CentOS-CR.repo && "$CENTOS_SIX" = '6' && "$(awk '{ print $3 }' /etc/redhat-release | sed -e 's|\.||')" -lt '68' ]]; then
    time $YUMDNFBIN -y -q install centos-release-cr${DISABLEREPO_DNF}
    sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/CentOS-CR.repo
    echo "priority=1" >> /etc/yum.repos.d/CentOS-CR.repo
    time $YUMDNFBIN -y -q update openssl openssl-devel --disableplugin=priorities --enablerepo=cr >/dev/null 2>&1
  fi
}

fileperm_fixes() {
  if [ -f /usr/lib/udev/rules.d/60-net.rules ]; then
    if [[ "$(lsattr /usr/lib/udev/rules.d/60-net.rules | cut -c5)" = 'i' ]]; then
      # fix for some centos 7 vps templates on vps hosts setting chattr +i on
      # /usr/lib/udev/rules.d/60-net.rules preventing yum updates for initscripts
      # yum packages
      chattr -i /usr/lib/udev/rules.d/60-net.rules
    fi
  fi
}

yumfastmirror_check() {
  # disable yum fastmirror plugin if not enough detected system memory available
  # for yum fastmirror operation
  if [ -f /etc/yum/pluginconf.d/fastestmirror.conf ]; then
    if [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -lt '1018000' && "$CENTOS_SEVEN" = '7' ]]; then
      sed -i 's|enabled = 1|enabled = 0|' /etc/yum/pluginconf.d/fastestmirror.conf
    elif [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -lt '263000' ]]; then
      sed -i 's|enabled = 1|enabled = 0|' /etc/yum/pluginconf.d/fastestmirror.conf
    fi
  fi
}

mariadb_errorlogfix() {
  # disable custom mariadb log-error path due to changes in 10.0.29 & 10.1.21
  # https://jira.mariadb.org/browse/MDEV-11841
  if [ -f /etc/my.cnf ]; then
    MDB_LOGERRORCHECK=$(grep '^log-error=\/var\/log\/mysqld.log' /etc/my.cnf)
    if [ "$MDB_LOGERRORCHECK" ]; then
      sed -i 's|^log-error=\/var\/log\/mysqld.log|#log-error=\/var\/log\/mysqld.log|g' /etc/my.cnf
    fi
  fi
}

expand_cmdprompt() {
  if [[ -z "$(grep 'PS1' /root/.bashrc)" ]]; then
    if [ "$(id -u)" -eq 0 ]; then 
      # you are root
      ORIG=$PS1
      # PS1="\e[40;1;37m[\A][\u@\H \w]\\$ \e[m " 
      echo 'export PS1="[\A][\u@\H \W]\\$ "' >> /root/.bashrc
      export PS1="[\A][\u@\H \W]\\$ "
    else
      ORIG=$PS1
      echo 'export PS1="[\A][\u@\H \W]\\$ "' >> /root/.bashrc
      export PS1="[\A][\u@\H \W]\\$ "
    fi
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ -z "$(grep 'PS1' $HOME/.bashrc)" ]]; then
      if [ "$(id -u)" -eq 0 ]; then 
        # you are root
        ORIG=$PS1
        # PS1="\e[40;1;37m[\A][\u@\H \w]\\$ \e[m " 
        echo 'export PS1="[\A][\u@\H \W]\\$ "' >> $HOME/.bashrc
        export PS1="[\A][\u@\H \W]\\$ "
      else
        ORIG=$PS1
        echo 'export PS1="[\A][\u@\H \W]\\$ "' >> $HOME/.bashrc
        export PS1="[\A][\u@\H \W]\\$ "
      fi
    fi
  fi
}

setup_pwdh() {
  if [[ -z "$(grep -w 'pwdh' /root/.bashrc)" ]]; then
    echo "alias pwdh='echo -n \"\$HOSTNAME\"; echo \" \$PWD\"'" >> /root/.bashrc
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ -z "$(grep -w 'pwdh' $HOME/.bashrc)" ]]; then
      echo "alias pwdh='echo -n \"\$HOSTNAME\"; echo \" \$PWD\"'" >> $HOME/.bashrc
    fi
  fi
}

cpcheck() {
  if [[ ! -f /proc/user_beancounters && -f /usr/bin/systemd-detect-virt && "$(/usr/bin/systemd-detect-virt)" = 'lxc' ]] || [[ ! -f /proc/user_beancounters && -f $(which virt-what) && $(virt-what | xargs | grep -o lxc) = 'lxc' ]]; then
    CHECK_LXD='y'
    if [[ -d /etc/profile.d && ! -f /etc/profile.d/locale.sh ]]; then
      echo "export LANG=en_US.UTF-8" >> /etc/profile.d/locale.sh
      echo "export LANGUAGE=en_US.UTF-8" >> /etc/profile.d/locale.sh
      source /etc/profile.d/locale.sh
    fi
  fi
  FIRSTINSTALL=$1
  if [[ "$FIRSTINSTALL" = 'initialinstall' ]]; then
    INITIALINSTALL='y'
  else
    INITIALINSTALL='n'
  fi
  
if [ -f /var/cpanel/cpanel.config ]; then
echo "WHM/Cpanel detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/psa/.psa.shadow ]; then
echo "Plesk detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/init.d/directadmin ]; then
echo "DirectAdmin detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
  # yum -y -q install epel-release
  if [[ "$DNF_COPR" = [yY] ]]; then
cat > "/etc/yum.repos.d/dnf-centos.repo" <<EOF
[dnf-centos]
name=Copr repo for dnf-centos owned by @rpm-software-management
baseurl=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/pubkey.gpg
enabled=1
enabled_metadata=1
EOF
  fi
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
  fi
  if [ ! "$(grep -w 'exclude' /etc/dnf/dnf.conf)" ]; then
    echo "excludepkgs=*.i386 *.i586 *.i686" >> /etc/dnf/dnf.conf
  fi
  if [ ! "$(grep -w 'fastestmirror=true' /etc/dnf/dnf.conf)" ]; then
    echo "fastestmirror=true" >> /etc/dnf/dnf.conf
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
fi

march_hostcheck
yumfastmirror_check
ovhkernelcheck
fileperm_fixes
cronpathadjust
crontabdisable_r
rclocalchecks
checkovzkernels
kernelchecks
pathfixes
apache_protect
setup_pwdh
cmupdatecheck
pzcat_install
csf_portflood
python_centossix_sclfix
disable_avahi
qrencode_setup
initial_pip_updates initial
jetpack_ip_whitelist_cronsetup

if [[ "$INITIALINSTALL" != [yY] ]]; then
  march_hostcheck
  expand_cmdprompt
  setup_pwdh
  c7mariadb_tmpdir
  mariadb_openfilesfix
  mariadb_errorlogfix
  yumfastmirror_check
  axelcheck
  fileperm_fixes
  selinxcheck
  setupemailcheck
  checkovzkernels
  rclocalchecks
  kernelchecks
  centaltoff
  csfipsetcheck
  csf_portflood
  checkaliases
  checkcmdircmd
  dmotdcheck
  nvcheck
  cmupdatecheck
  multiphpcheck
  mjemalloc
  blockeditorcheck
  rpcnfsports
  fixlogrotate
  csf_distftp
  pureftpdupdates
  leupdates
  memcachedupdatechecks
  xpowerby
  pathfixes
  sshdfixes
  auto_gitupdate
  figletcheck
  maintenance_confcheck
  axivo_remove
  forgefix
  ngxmodule_cleanups
  wpcompfix
  entropy_centosseven
  tcpcheck_centos
  remi_tweaks
  imagick_fixes
  apache_protect
  centossix_crsetup
  cfauthorigin_cronsetup
  if [[ "$LOWMEM_INSTALL" != [yY] ]]; then
    wgetver_check
  fi
  check_jemstatsfile
  nginx_mutexoff
  #fixphpfpm_includes
  if [[ -z "$SKIP_FIXPHPFPM_HTTPPROXY" || "$SKIP_FIXPHPFPM_HTTPPROXY" = 'n' ]]; then
    # skip fixphpfpm_httpproxy
    fixphpfpm_httpproxy
  fi
  fixlibmysqlclient_symlink
  fixnodejs_epel
  fixnginx_epel
  fixsclutils_epel
  fixlshw_rpmforge
  fixpercona_epel
  fixnoarch_percona
  fixsysbench_percona
  #fixclamav_epel
  nano_highlight
  checkwoff
  if [[ -z "$SKIP_CHECKIPVSIX" || "$SKIP_CHECKIPVSIX" = 'n' ]]; then
    # skip checkipvsix    
    checkipvsix
  fi
  if [[ -z "$SKIP_FIXWP_UPDATER" || "$SKIP_FIXWP_UPDATER" = 'n' ]]; then
    # skip fixwp_updater    
    fixwp_updater
  fi
  update_nginxconf
  update_cmdshortcuts
  update_initphpfpm
  update_phpfpmconfg
  cityfan_fix
  if [[ -z "$SKIP_LIBC_FIX" || "$SKIP_LIBC_FIX" = 'n' ]]; then
    # skip libc_fix    
    libc_fix
  fi
  varnishfour_setup
  disablelogs
  nginxlargefile_fix
  fix_phperrorlogperm
  crontabdisable_r
  if [[ -z "$SKIP_PIP_UPDATES" || "$SKIP_PIP_UPDATES" = 'n' ]]; then
    # skip pip_updates    
    pip_updates
  fi
  ngx_gzipbuffersfix
  ngx_brotlibuffersfix
  ngxmaster_openfiles
  check_memcachedflush
  csf_smtpports
  csf_loadalert
  disable_varnishrepo
  pzcat_install
  redetect_tlsonethree
  dft_phpupstream
  python_centossix_sclfix
  disable_avahi
  qrencode_setup
  less_console_verbose
  opt_sslcache
  add_versionlocks
  symlink_setup_emailnotify
  symlink_setup_mariadb
  pureftpd_symlink_fix
  cfauthorigin_cronsetup
  if [[ -z "$SKIP_CSF_MAXMIND_REGO" || "$SKIP_CSF_MAXMIND_REGO" = 'n' ]]; then
    # skip csf_maxmind_rego    
    csf_maxmind_rego
  fi
  update_webp_inc
  phpmyadmin_composer_fix
  phpmyadmin_updater_fix
  imagemagick_heif
  fix_nasm
  update_cache_enabler_schemefix
  update_wpce_map
  jetpack_ip_whitelist_cronsetup
fi

}

histformat() {
  if [[ -z "$(grep HISTTIMEFORMAT /root/.bashrc)" ]]; then
    echo "HISTTIMEFORMAT=\"[%d.%m.%y] %T   \"" >> /root/.bashrc
    echo "export HISTSIZE=10000" >> /root/.bashrc
    echo "export HISTTIMEFORMAT" >> /root/.bashrc
  fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ -z "$(grep HISTTIMEFORMAT $HOME/.bashrc)" ]]; then
      echo "HISTTIMEFORMAT=\"[%d.%m.%y] %T   \"" >> $HOME/.bashrc
      echo "export HISTSIZE=10000" >> $HOME/.bashrc
      echo "export HISTTIMEFORMAT" >> $HOME/.bashrc
    fi
  fi
}

setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
wget -q --no-check-certificate -O /usr/bin/cminfo https://raw.githubusercontent.com/centminmod/centminmod/master/tools/cminfo.sh
chmod 0700 /usr/bin/cminfo
EOF

chmod 0700 /usr/bin/cminfo_updater

# insert itself into cronjob for auto updates
if [ ! -f /usr/bin/crontab ]; then
  time $YUMDNFBIN -y -q install cronie${DISABLEREPO_DNF}
    if [[ "$CENTOS_SEVEN" != '7' ]]; then
      service crond restart
      chkconfig crond on
    else
      systemctl restart crond.service
      systemctl enable crond.service
    fi  
fi

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
    if [[ "$CENTOS_SEVEN" != '7' ]]; then
      service sysstat restart
      chkconfig sysstat on
    else
      systemctl restart sysstat.service
      systemctl enable sysstat.service
    fi    
fi

if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    mkdir -p /etc/centminmod/cronjobs
    cp cronjoblist /etc/centminmod/cronjobs/cronjoblist-before-setupdate-cminfo-updater-setup-${DT}.txt
    echo "0 */4 * * * /usr/bin/cminfo_updater 2>/dev/null" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi
}