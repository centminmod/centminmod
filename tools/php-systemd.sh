#!/bin/bash
# centminmod.com experimental systemd php-fpm service setup
# switch php-fpm service from init.d based to systemd for centos 7 systems

phpfpm_setup_systemd() {
  fpm_systemd=$1
  if [[ -d /etc/systemd/system ]]; then
    if [ -f /etc/init.d/php-fpm ]; then 
      service php-fpm stop
      rm -rf /etc/init.d/php-fpm
    fi
  mkdir -p /etc/systemd/system/php-fpm.service.d
  echo "d      /var/run/php-fpm/         0755 root root" > /etc/tmpfiles.d/php-fpm.conf
  if [ ! -d /var/run/php-fpm/ ]; then mkdir -p /var/run/php-fpm/; fi
  if [[ ! "$(grep '/var/run/php-fpm' /etc/rc.local)" ]]; then
    echo 'if [ ! -d /var/run/php-fpm/ ]; then mkdir -p /var/run/php-fpm/; fi' >> /etc/rc.local
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

CHECK_FPMSYSTEMD=$(php-config --configure-options | grep -o with-fpm-systemd)

if [[ "$fpm_systemd" = 'yes' && "$CHECK_FPMSYSTEMD" = 'with-fpm-systemd' ]]; then
cat > /usr/lib/systemd/system/php-fpm.service <<EOF
[Unit]
Description=PHP FastCGI Process Manager
After=syslog.target network.target

[Service]
Type=forking
#Type=notify
PIDFile=/var/run/php-fpm/php-fpm.pid
ExecStart=/usr/local/sbin/php-fpm --daemonize --fpm-config /usr/local/etc/php-fpm.conf --pid /var/run/php-fpm/php-fpm.pid
ExecReload=/bin/kill -USR2 \$MAINPID
ExecStop=/bin/kill -s QUIT \$MAINPID
#Restart=on-failure
PrivateTmp=true
#RestartSec=5
#TimeoutSec=2
#WatchdogSec=30
NotifyAccess=all


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
PIDFile=/var/run/php-fpm/php-fpm.pid
ExecStart=/usr/local/sbin/php-fpm --daemonize --fpm-config /usr/local/etc/php-fpm.conf --pid /var/run/php-fpm/php-fpm.pid
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
      touch /var/run/php-fpm/php-fpm.pid
      chown nginx:nginx /var/run/php-fpm
      chown root:root /var/run/php-fpm/php-fpm.pid
  
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