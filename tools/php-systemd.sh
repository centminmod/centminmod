#!/bin/bash
# centminmod.com experimental systemd php-fpm service setup
# switch php-fpm service from init.d based to systemd for centos 7 systems

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  fi
fi

if [[ "$CENTOS_NINE" -eq '9' ]]; then
  PHP_PID_PATH='/run/php-fpm/php-fpm.pid'
  PHP_PID_PATHDIR='/run/php-fpm/'
elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  PHP_PID_PATH='/run/php-fpm/php-fpm.pid'
  PHP_PID_PATHDIR='/run/php-fpm/'
else
  PHP_PID_PATH='/var/run/php-fpm/php-fpm.pid'
  PHP_PID_PATHDIR='/var/run/php-fpm/'
fi

adjust_phpfpm_unix_socket_path() {
  if [[ "$CENTOS_NINE" -eq '9' ]]; then
    PHP_PID_PATHDIR='/run/php-fpm/' 
    if [ -f /usr/local/nginx/conf/php-geoip2.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php-geoip2.conf
    fi
    if [ -f /usr/local/nginx/conf/php.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php.conf
    fi
    if [ -f /usr/local/nginx/conf/php_laravael.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php_laravael.conf
    fi
    if [ -f /usr/local/nginx/conf/phpalt.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpalt.conf
    fi
    if [ -f /usr/local/nginx/conf/phpssl.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpssl.conf
    fi
    if [ -f /usr/local/nginx/conf/phpstatus.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpstatus.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool1_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool1_uds.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool2_uds.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool3_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool3_uds.conf
    fi
  elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
    PHP_PID_PATHDIR='/run/php-fpm/'
    if [ -f /usr/local/nginx/conf/php-geoip2.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php-geoip2.conf
    fi
    if [ -f /usr/local/nginx/conf/php.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php.conf
    fi
    if [ -f /usr/local/nginx/conf/php_laravael.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/php_laravael.conf
    fi
    if [ -f /usr/local/nginx/conf/phpalt.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpalt.conf
    fi
    if [ -f /usr/local/nginx/conf/phpssl.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpssl.conf
    fi
    if [ -f /usr/local/nginx/conf/phpstatus.conf ]; then
      sed -i "s|unix:/var/run/php-fpm/|unix:$PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpstatus.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool1_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool1_uds.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool2_uds.conf
    fi
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool3_uds.conf ]; then
      sed -i "s|listen = /var/run/php-fpm/|listen = $PHP_PID_PATHDIR|g" /usr/local/nginx/conf/phpfpmd/phpfpm_pool3_uds.conf
    fi
  fi
}

phpfpm_setup_systemd() {
  fpm_systemd=$1
  if [[ -d /etc/systemd/system ]]; then
    if [ -f /etc/init.d/php-fpm ]; then 
      service php-fpm stop
      rm -rf /etc/init.d/php-fpm
    fi
  mkdir -p /etc/systemd/system/php-fpm.service.d
  echo "d      $PHP_PID_PATHDIR         0755 root root" > /etc/tmpfiles.d/php-fpm.conf
  adjust_phpfpm_unix_socket_path
  if [ ! -d "$PHP_PID_PATHDIR" ]; then mkdir -p "$PHP_PID_PATHDIR"; fi
  if [[ ! "$(grep "$PHP_PID_PATHDIR" /etc/rc.local)" ]]; then
    echo "if [ ! -d $PHP_PID_PATHDIR ]; then mkdir -p $PHP_PID_PATHDIR; fi" >> /etc/rc.local
  fi

  if [[ -f /proc/user_beancounters || "$(virt-what | grep -o lxc )" = 'lxc' ]]; then
cat > /etc/systemd/system/php-fpm.service.d/limit.conf <<EOF
[Service]
LimitNOFILE=262144
LimitNPROC=16384
LimitSTACK=2097152
#LimitNICE=-15
# disable Nice for bug workaround
# https://community.centminmod.com/threads/17045/
# Nice=-10
StartLimitBurst=50
#CPUShares=1500
#CPUSchedulingPolicy=fifo
#CPUSchedulingPriority=99
EOF
else
cat > /etc/systemd/system/php-fpm.service.d/limit.conf <<EOF
[Service]
LimitNOFILE=262144
LimitNPROC=16384
#LimitNICE=-15
Nice=-10
StartLimitBurst=50
#CPUShares=1500
#CPUSchedulingPolicy=fifo
#CPUSchedulingPriority=99
EOF
  fi
    if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
        if [ ! -f /etc/systemd/system/php-fpm.service.d/failure-restart.conf ]; then
cat > "/etc/systemd/system/php-fpm.service.d/failure-restart.conf" <<TDG
[Unit]
StartLimitIntervalSec=30
StartLimitBurst=2

[Service]
Restart=on-failure
RestartSec=5s
TDG
        fi
    elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        if [ ! -f /etc/systemd/system/php-fpm.service.d/failure-restart.conf ] || [[ "$CENTOS_SEVEN" = '7' && "$(grep -o Unit /etc/systemd/system/php-fpm.service.d/failure-restart.conf)" = 'Unit' ]]; then
cat > "/etc/systemd/system/php-fpm.service.d/failure-restart.conf" <<TDG
[Service]
StartLimitInterval=30
StartLimitBurst=2
Restart=on-failure
RestartSec=5s
TDG
        fi
    fi

CHECK_FPMSYSTEMD=$(php-config --configure-options | grep -o with-fpm-systemd)

if [[ "$fpm_systemd" = 'yes' && "$CHECK_FPMSYSTEMD" = 'with-fpm-systemd' ]]; then
cat > /usr/lib/systemd/system/php-fpm.service <<EOF
[Unit]
Description=PHP FastCGI Process Manager
After=syslog.target network.target

[Service]
Type=forking
PIDFile=$PHP_PID_PATH
ExecStart=/usr/local/sbin/php-fpm --daemonize --fpm-config /usr/local/etc/php-fpm.conf --pid $PHP_PID_PATH
ExecReload=/bin/kill -USR2 \$MAINPID
ExecStop=/bin/kill -s QUIT \$MAINPID
#Restart=on-failure
PrivateTmp=true
#RestartSec=5
#TimeoutSec=2
#WatchdogSec=30
#NotifyAccess=all


[Install]
WantedBy=multi-user.target
EOF
else
cat > /usr/lib/systemd/system/php-fpm.service <<EOF
[Unit]
Description=PHP FastCGI Process Manager
After=syslog.target network.target

[Service]
Type=forking
PIDFile=$PHP_PID_PATH
ExecStart=/usr/local/sbin/php-fpm --daemonize --fpm-config /usr/local/etc/php-fpm.conf --pid $PHP_PID_PATH
ExecReload=/bin/kill -USR2 \$MAINPID
ExecStop=/bin/kill -s QUIT \$MAINPID
#Restart=on-failure
PrivateTmp=true
#RestartSec=5
#TimeoutSec=2
#WatchdogSec=30
#NotifyAccess=all

[Install]
WantedBy=multi-user.target
EOF
fi

  # update cmd shorts
  echo "systemctl daemon-reload; systemctl stop php-fpm; echo \"Stopping php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
  echo "systemctl daemon-reload; systemctl start php-fpm; echo \"Starting php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
  echo "systemctl daemon-reload; systemctl restart php-fpm; echo \"Restarting php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
  echo "systemctl daemon-reload; systemctl reload php-fpm; echo \"Reloading php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
  rm -rf /usr/bin/fpmconfigtest
  echo "systemctl daemon-reload; systemctl status php-fpm" >/usr/bin/fpmstatus ; chmod 700 /usr/bin/fpmstatus
  if [[ "$fpm_systemd" = 'yes' && "$CHECK_FPMSYSTEMD" = 'with-fpm-systemd' ]]; then
cat > /usr/bin/fpmstats <<EOF
systemctl daemon-reload;
systemctl show php-fpm -p StatusText --no-pager | awk -F '=' '{print \$2}';
phpstatuscheck=\$(curl -sI localhost/phpstatus 2>&1 | head -n1 | grep -o 200);
phpstatusuds1check=\$(curl -sI localhost/phpstatus-pool1-uds 2>&1 | head -n1 | grep -o 200);
phpstatusuds2check=\$(curl -sI localhost/phpstatus-pool2-uds 2>&1 | head -n1 | grep -o 200);
phpstatusuds3check=\$(curl -sI localhost/phpstatus-pool3-uds 2>&1 | head -n1 | grep -o 200);
if [[ "\$phpstatuscheck" -eq '200' ]]; then
  curl -s localhost/phpstatus;
fi
if [[ "\$phpstatusuds1check" -eq '200' ]]; then
  echo
  curl -s localhost/phpstatus-pool1-uds;
fi
if [[ "\$phpstatusuds2check" -eq '200' ]]; then
  echo
  curl -s localhost/phpstatus-pool2-uds;
fi
if [[ "\$phpstatusuds3check" -eq '200' ]]; then
  echo
  curl -s localhost/phpstatus-pool3-uds;
fi
EOF
    chmod 700 /usr/bin/fpmstats
  fi

  echo "systemctl daemon-reload; service nginx stop; systemctl stop php-fpm; echo \"Stopping php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
  echo "systemctl daemon-reload; service nginx start; systemctl start php-fpm; echo \"Starting php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
  echo "systemctl daemon-reload; service nginx restart; systemctl restart php-fpm; echo \"Restarting php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
  echo "systemctl daemon-reload; service nginx reload; systemctl reload php-fpm; echo \"Reloading php-fpm (via systemctl) [  OK  ]\"" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload

  echo "systemctl daemon-reload"
  systemctl daemon-reload
  echo
  echo "systemctl restart php-fpm"
  systemctl restart php-fpm
  echo
  echo "systemctl enable php-fpm"
  systemctl enable php-fpm
  echo
  echo "systemctl status php-fpm"
  systemctl status php-fpm
  echo
  if [[ "$fpm_systemd" = 'yes' && "$CHECK_FPMSYSTEMD" = 'with-fpm-systemd' ]]; then
    echo "php-fpm systemd service setup"
    echo "with --with-fpm-systemd integration"
  else
    echo "php-fpm systemd service setup"
  fi
    if [[ "$CENTOS_NINE" -eq '9' ]]; then
      sed -i 's|\/var\/run\/php-fpm\/php-fpm.pid|\/run\/php-fpm\/php-fpm.pid|' /usr/local/etc/php-fpm.conf
    fi
  fi
}

restore_initd() {
  if [[ -d /etc/systemd/system ]]; then
    if [ -f /usr/lib/systemd/system/php-fpm.service ]; then 
      systemctl stop php-fpm
      rm -rf /etc/systemd/system/php-fpm.service.d
      rm -rf /usr/lib/systemd/system/php-fpm.service
      rm -rf /etc/tmpfiles.d/php-fpm.conf
    fi
    if [ -f /usr/local/src/centminmod/init/php-fpm ]; then
      cp "/usr/local/src/centminmod/init/php-fpm" /etc/init.d/php-fpm
      dos2unix /etc/init.d/php-fpm >/dev/null 2>&1
      chmod +x /etc/init.d/php-fpm
      mkdir -p /var/run/php-fpm
      chmod 755 /var/run/php-fpm
      touch $PHP_PID_PATH
      chown nginx:nginx /var/run/php-fpm
      chown root:root $PHP_PID_PATH
  
      mkdir -p /var/log/php-fpm/
      touch /var/log/php-fpm/www-error.log
      touch /var/log/php-fpm/www-php.error.log
      chmod 0666 /var/log/php-fpm/www-error.log
      chmod 0666 /var/log/php-fpm/www-php.error.log
    fi

    echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
    echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
    echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
    echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
    echo "/etc/init.d/php-fpm configtest" >/usr/bin/fpmconfigtest ; chmod 700 /usr/bin/fpmconfigtest
    echo "/etc/init.d/php-fpm status" >/usr/bin/fpmstatus ; chmod 700 /usr/bin/fpmstatus
    rm -rf /usr/bin/fpmstats

    echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
    echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
    echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
    echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload

    echo "systemctl daemon-reload"
    systemctl daemon-reload
    echo
    echo "service php-fpm start"
    service php-fpm start
    echo
    echo "chkconfig on"
    chkconfig on
    echo
    echo "service php-fpm status"
    service php-fpm status
    echo
    echo "php-fpm init.d service restored"
    echo 
  fi
}

if [[ "$1" = 'restore' ]]; then
  restore_initd
elif [[ "$1" = 'fpm-systemd' ]]; then
  phpfpm_setup_systemd yes
else
  phpfpm_setup_systemd
fi