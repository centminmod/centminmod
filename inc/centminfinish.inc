disk_cleanups() {
    # tidy up to reduce disk space usage
    find /svr-setup -maxdepth 1 -type d ! -wholename "/svr-setup" -exec rm -rf {} \;
    rm -f /svr-setup/1
    rm -f /svr-setup/axel-2.14.1.tar.gz
    rm -f /svr-setup/axel-2.4-1.el5.rf.i386.rpm
    rm -f /svr-setup/axel-2.4-1.el5.rf.x86_64.rpm
    rm -f /svr-setup/axel-2.4-1.el6.rf.i686.rpm
    rm -f /svr-setup/axel-2.4-1.el6.rf.x86_64.rpm
    rm -f /svr-setup/axel-2.5.tar.gz
    rm -f /svr-setup/axel-2.6.tar.gz
    rm -f /svr-setup/ccache-3.4.1.tar.gz
    rm -f /svr-setup/city-fan.org-release-1-13.rhel6.noarch.rpm
    rm -f /svr-setup/city-fan.org-release-1-13.rhel7.noarch.rpm
    rm -f /svr-setup/csf.tgz-local
    rm -f /svr-setup/epel-release*
    rm -f /svr-setup/help-dummy.o
    rm -f /svr-setup/mongodb-1.4.0.tgz
    rm -f /svr-setup/ngx-fancyindex-0.3.1.tar.gz
    rm -f /svr-setup/ngx_cache_purge-2.4.2.tar.gz
    rm -f /svr-setup/package.xml
    rm -f /svr-setup/redis2-nginx-module_v0.14.tar.gz
    rm -f /svr-setup/remi-release-5.rpm
    rm -f /svr-setup/remi-release-6.rpm
    rm -f /svr-setup/rpmforge-release-0.5.3-1.el5*
    rm -f /svr-setup/rpmforge-release-0.5.3-1.el6*
    rm -f /svr-setup/set-misc-nginx-module_v0.31*
    rm -f /svr-setup/varnish-release-3.0-1.el6.noarch.rpm
    rm -f /svr-setup/wget-1.19.4.tar.gz
}

installchecks() {

  if [ "$UNATTENDED" == 'y' ]; then

letswait() {
sleep 0.3
}

entropyup

    echo "*************************************************"
    cecho "* Post-Install Check List...." $boldgreen
    echo "*************************************************"
letswait

  if [[ -z "$(ps aufx | grep 'crond' | grep -v grep)" ]]; then
echo ""

  if [[ $(rpm -q sysstat >/dev/null 2>&1; echo $?) != '0' ]] || [[ $(rpm -q cronie >/dev/null 2>&1; echo $?) != '0' ]]; then
    $(which yum) -y -q install sysstat cronie
  fi

  cecho "--------------------------------------------------------" $boldyellow
  cecho "Ensure nginx, php-fpm, redis, crond & sysstat service are running" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
    if [[ "$CENTOS_SIX" -eq '6' ]]; then
      pkill crond
      service crond restart
      service sysstat restart
      service nginx restart
      service php-fpm restart
      service redis restart
      chkconfig crond on
      chkconfig sysstat on
      chkconfig nginx on
      chkconfig php-fpm on
      chkconfig redis on
    else
      pkill crond
      systemctl restart crond.service
      systemctl enable crond.service
      systemctl restart sysstat.service
      systemctl enable sysstat.service
      systemctl restart nginx.service
      systemctl enable nginx.service
      systemctl restart php-fpm.service
      systemctl enable php-fpm.service
      systemctl restart redis.service
      systemctl enable redis.service
    fi
  fi

 grub_efi_fix

echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check ccache Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  $(which ccache) -V
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check Nginx Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  if [ -f "$(which nginx)" ]; then
    $(which nginx) -V
    if [[ "$(systemctl is-enabled nginx)" = 'disabled' ]]; then
      systemctl enable nginx >/dev/null 2>&1
    fi
    echo
    systemctl status nginx
  else
    echo "can not find nginx binary.. install failed"
  fi

  if [[ "$DYNAMIC_SUPPORT" = [yY] ]]; then
      echo
      echo "dynamic module directory at /usr/local/nginx/modules"
      ls -lah /usr/local/nginx/modules
      echo
  fi

letswait
  if [[ "$PHP_INSTALL" = [yY] ]]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  $(which php) -v
  if [[ "$(systemctl is-enabled php-fpm)" = 'disabled' ]]; then
    systemctl enable php-fpm >/dev/null 2>&1
  fi
  echo
  systemctl status php-fpm
letswait
  else
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  echo "PHP_INSTALL='n' set"
  fi

  if [ -f /usr/bin/php81 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 8.1 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php81 -v
letswait
  fi
  if [ -f /usr/bin/php80 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 8.0 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php80 -v
letswait
  fi
  if [ -f /usr/bin/php74 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.4 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php74 -v
letswait
  fi
  if [ -f /usr/bin/php73 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.3 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php73 -v
letswait
  fi
  if [ -f /usr/bin/php72 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.2 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php72 -v
letswait
  fi
  if [ -f /usr/bin/php71 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.1 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php71 -v
letswait
  fi
  if [ -f /usr/bin/php70 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.0 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php70 -v
letswait
  fi
  if [ -f /usr/bin/php56 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 5.6 Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  /usr/bin/php56 -v
letswait
  fi

echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check MariaDB installed RPM Versions:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  $(which rpm) -qa | grep -i MariaDB
  if [[ "$(systemctl is-enabled mariadb)" = 'disabled' ]]; then
    systemctl enable mariadb >/dev/null 2>&1
  fi
  echo
  systemctl status mariadb
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check Memcached Server Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow  
  /usr/local/bin/memcached -h | head -n1
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check Redis Server Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow  
  /usr/bin/redis-server -v | head -n1
  if [[ "$(systemctl is-enabled redis)" = 'disabled' ]]; then
    systemctl enable redis >/dev/null 2>&1
  fi
  echo
  systemctl status redis
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check CSF Firewall Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  $(which csf) -v
  if [[ "$(systemctl is-enabled csf)" = 'disabled' ]]; then
    systemctl enable csf >/dev/null 2>&1
  fi
  if [[ "$(systemctl is-enabled lfd)" = 'disabled' ]]; then
    systemctl enable lfd >/dev/null 2>&1
  fi
  echo
  systemctl status csf lfd
letswait

if [[ "$(uname -m)" != 'aarch64' ]]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check Siege Benchmark Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
$(which siege) -V
letswait
fi

if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check ngx_pagespeed Control Script:" $boldyellow
  cecho "https://centminmod.com/nginx_ngx_pagespeed.html" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
/usr/bin/pscontrol
letswait
fi
# echo ""
#   cecho "--------------------------------------------------------" $boldyellow
#   cecho "Check TwemPerf Version:" $boldyellow
#   cecho "--------------------------------------------------------" $boldyellow
# mcperf -V
# letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check mysqlreport version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  chmod +x /root/mysqlreport
/root/mysqlreport --help | head -n1
letswait
if [[ "$NSD_INSTALL" = [yY] ]]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check NSD Bind Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
$(which nsd) -v
letswait
fi
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check pure-ftpd Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
$(which pure-ftpd) -h | head -n1 | awk '{print $1,$2}'
  if [[ "$(systemctl is-enabled pure-ftpd)" = 'disabled' ]]; then
    systemctl enable pure-ftpd >/dev/null 2>&1
  fi
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check postfix Status:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  if [[ "$(systemctl is-enabled postfix)" = 'disabled' ]]; then
    systemctl enable postfix >/dev/null 2>&1
  fi
  systemctl status postfix --no-pager
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check YUM Repo List" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
$(which yum) repolist

# echo ""
  fi # unattended

if [[ "$NSD_DISABLED" = [yY] ]]; then
  if [[ ! -z "$(ps aux | grep nsd | grep -v grep)" ]]; then
  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping NSD service:" $boldyellow
  cmservice nsd stop
  cecho "Disabling NSD service:" $boldyellow
  cmchkconfig nsd off
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable NSD service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "service nsd start"
    echo "chkconfig nsd on"
  else
    echo "service nsd start"
    echo "chkconfig nsd on"
  fi
  cecho "--------------------------------------------------------" $boldyellow
  fi
fi

if [[ "$MEMCACHED_DISABLED" = [yY] ]]; then

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping Memcached server service:" $boldyellow
  cmservice memcached stop
  cecho "Disabling Memcached server service:" $boldyellow
  cmchkconfig memcached stop
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable Memcached server service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "service memcached start"
    echo "chkconfig memcached on"
  else
    echo "service memcached start"
    echo "chkconfig memcached on"
  fi
  cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$REDIS_DISABLED" = [yY] ]]; then

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping Redis server service:" $boldyellow
  cmservice redis stop
  cecho "Disabling Redis server service:" $boldyellow
  cmchkconfig redis stop
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable Redis server service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "systemctl start redis"
    echo "systemctl enable redis"
  else
    echo "service redis start"
    echo "chkconfig redis on"
  fi
  cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$PHP_DISABLED" = [yY] ]]; then

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping php-fpm service:" $boldyellow
  cmservice php-fpm stop
  cecho "Disabling php-fpm service:" $boldyellow
  cmchkconfig php-fpm off
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable php-fpm service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "systemctl start php-fpm"
    echo "systemctl enable php-fpm"
  else
    echo "service php-fpm start"
    echo "chkconfig php-fpm on"
  fi
  cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$MYSQLSERVICE_DISABLED" = [yY] ]]; then

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping mysql service:" $boldyellow
  cmservice mysql stop
  cecho "Disabling mysql service:" $boldyellow
  cmchkconfig mysql off
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable mysql service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "systemctl start mariadb"
    echo "systemctl enable mariadb"
  else
    if [[ "$CENTOS_SIX" = '6' ]]; then
      echo "service mysql start"
      echo "chkconfig mysql on"
    else
      echo "systemctl start mariadb"
      echo "systemctl enable mariadb"
    fi
  fi
  cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$PUREFTPD_DISABLED" = [yY] ]]; then

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Stopping pure-ftpd service:" $boldyellow
  cmservice pure-ftpd stop
  cecho "Disabling pure-ftpd service:" $boldyellow
  cmchkconfig pure-ftpd off
  cecho "--------------------------------------------------------" $boldyellow
  cecho "In future to re-enable pure-ftpd service type these 2 commands:" $boldyellow
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    echo "systemctl start pure-ftpd"
    echo "systemctl enable pure-ftpd"
  else
    echo "service pure-ftpd start"
    echo "chkconfig pure-ftpd on"
  fi
  cecho "--------------------------------------------------------" $boldyellow
fi

  sshdtweaks_nine

  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "mytimes check:" $boldyellow
  cecho "/usr/bin/mytimes" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  if [ ! -f /usr/bin/mytimes ]; then
    cp "$CUR_DIR/tools/mytimes.sh" /usr/bin/mytimes
    chmod +x /usr/bin/mytimes
  fi
  /usr/bin/mytimes
  cecho "--------------------------------------------------------" $boldyellow

if [ -f /proc/user_beancounters ]; then
  echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "OpenVZ VPS detected:" $boldyellow
  cecho "check /proc/user_beancounters counters" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  grep oomguarpages /proc/user_beancounters  | awk '{s=$2;t=$3;u=$4; {print "VPS Memory Usage\nCurrent Held: " s/204.8"MB\nMax Held: " t/204.8 "MB\nBarrier: "u/204.8"MB" }}'
  beans=`cat /proc/user_beancounters | grep priv`
  max=$(echo $beans | awk '{ print $4;}')
  use=$(echo $beans | awk '{ print $2;}')
  let "per=$use*100/$max"
  let "mb=$use/256"
  echo "privvmpages: $mb MB ($per%)"
  echo
  cat /proc/user_beancounters
  cecho "--------------------------------------------------------" $boldyellow
fi

  if [[ "$VARNISHREPO_DISABLE" = [nN] && "$INITIALINSTALL" = [yY] && "$(uname -m)" = 'x86_64' && -f "$VARNISH_REPOFILE" ]]; then
    # disable for install stage to speed up install
    yum-config-manager --enable varnishcache_varnish41 varnishcache_varnish41-source >/dev/null 2>&1
  fi

}

centminfinish() {

if [[ ! $(ps aufx | grep rsyslog | grep -v grep) ]]; then
  cmservice rsyslog start
  cmchkconfig rsyslog on
fi

if [[ "$CENTOS_SIX" = '6' ]]; then
  axivosixyum
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
  axivoseven
fi

installchecks
nvcheck
DATENOW=$(date +"%b %d")
YUMLOG=$(grep -h "$DATENOW" "$YUMLOG_FILE")
echo "$YUMLOG" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yum-log.log"


    echo "*************************************************"
    cecho "* Centmin Mod install completed...." $boldgreen
    echo "*************************************************"

if [[ "$TS_INSTALL" = [yY] ]]; then
  curl -${ipv_forceopt}s "${CURL_TIMEOUTS}" http://check.centminmod.com/utctime > "${CENTMINLOGDIR}/install.utc.time.log" 2>&1
fi

}