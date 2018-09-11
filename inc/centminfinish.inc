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
  cecho "Ensure crond & sysstat service are running" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
    if [[ "$CENTOS_SEVEN" != '7' ]]; then
      pkill crond
      service crond restart
      service sysstat restart
      chkconfig crond on
      chkconfig sysstat on
    else
      pkill crond
      systemctl restart crond.service
      systemctl enable crond.service
      systemctl restart sysstat.service
      systemctl enable sysstat.service
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
letswait
  else
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
  echo "PHP_INSTALL='n' set"
  fi

  if [ -f /usr/bin/php73 ]; then
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check PHP-FPM 7.2 Version:" $boldyellow
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
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check Memcached Server Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow  
  /usr/local/bin/memcached -h | head -n1
letswait
echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Check CSF Firewall Version:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
$(which csf) -v
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
  cecho "http://centminmod.com/nginx_ngx_pagespeed.html" $boldyellow
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
  echo "service nsd start"
  echo "chkconfig nsd on"
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
  echo "service memcached start"
  echo "chkconfig memcached on"
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
  echo "service php-fpm start"
  echo "chkconfig php-fpm on"
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
  echo "service mysql start"
  echo "chkconfig mysql on"
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
  echo "service pure-ftpd start"
  echo "chkconfig pure-ftpd on"
  cecho "--------------------------------------------------------" $boldyellow
fi

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
YUMLOG=$(grep -h "$DATENOW" /var/log/yum.log)
echo "$YUMLOG" >> "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yum-log.log"


    echo "*************************************************"
    cecho "* Centmin Mod install completed...." $boldgreen
    echo "*************************************************"

if [[ "$TS_INSTALL" = [yY] ]]; then
  curl -s "${CURL_TIMEOUTS}" http://check.centminmod.com/utctime > "${CENTMINLOGDIR}/install.utc.time.log" 2>&1
fi

}