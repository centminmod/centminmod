# reference for centos 7 systemd
# LimitCPU=
# LimitFSIZE=
# LimitDATA=
# LimitSTACK=
# LimitCORE=
# LimitRSS=
# LimitNOFILE=
# LimitAS=
# LimitNPROC=
# LimitMEMLOCK=
# LimitLOCKS=
# LimitSIGPENDING=
# LimitMSGQUEUE=
# LimitNICE=
# LimitRTPRIO=
# LimitRTTIME=

function funct_openvz_stacksize {
  if [ -f /proc/user_beancounters ]; then
    echo ""
    echo "********************************************************************"
    cecho "* Correct service's stack size for OpenVZ systems. Please wait...." $boldgreen
    echo "********************************************************************"
    if [[ "$CENTOS_SEVEN" != '7' ]]; then
      if [ -f /etc/init.d/rsyslog ]; then
        sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 128\nfi\n/g' /etc/init.d/rsyslog
      fi
      if [ -f /etc/init.d/nsd ]; then
        sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 128\nfi\n/g' /etc/init.d/nsd
      fi
      if [ -f /etc/init.d/nginx ]; then
        sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/nginx
      fi
      if [ -f /etc/init.d/lfd ]; then
        sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/lfd
      fi
      if [ -f /etc/init.d/mysql ]; then
        sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/mysql
      fi
      if [ -f /etc/init.d/pure-ftpd ]; then
        sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/pure-ftpd
      fi
      
      echo "checking stack size ulimit -s set properly: "
      if [ -f /etc/init.d/rsyslog ]; then
        head -n 5 /etc/init.d/rsyslog
      fi
      if [ -f /etc/init.d/nsd ]; then
        head -n 5 /etc/init.d/nsd
      fi
      if [ -f /etc/init.d/nginx ]; then
        head -n 5 /etc/init.d/nginx
      fi
      if [ -f /etc/init.d/lfd ]; then
        head -n 5 /etc/init.d/lfd
      fi
      if [ -f /etc/init.d/mysql ]; then
        head -n 5 /etc/init.d/mysql
      fi
      if [ -f /etc/init.d/pure-ftpd ]; then
        head -n 5 /etc/init.d/pure-ftpd
      fi
    else
      if [[ ! -d /etc/systemd/system/rsyslog.service.d || ! -f /etc/systemd/system/rsyslog.service.d/rsyslog.conf ]]; then
        mkdir -p /etc/systemd/system/rsyslog.service.d
cat > "/etc/systemd/system/rsyslog.service.d/rsyslog.conf" <<EFF
[Service]
LimitSTACK=524288
EFF
        systemctl daemon-reload
        systemctl restart rsyslog
        cat /etc/systemd/system/rsyslog.service.d/rsyslog.conf
      fi
      if [ -f /etc/init.d/nsd ]; then
        sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 128\nfi\n/g' /etc/init.d/nsd
      fi
      if [ -f /etc/init.d/nginx ]; then
        sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/nginx
      fi
      if [[ ! -d /etc/systemd/system/lfd.service.d || ! -f /etc/systemd/system/lfd.service.d/lfd.conf ]]; then
        mkdir -p /etc/systemd/system/lfd.service.d
cat > "/etc/systemd/system/lfd.service.d/lfd.conf" <<EFF
[Service]
LimitSTACK=1048576
EFF
        systemctl daemon-reload
        systemctl restart lfd
        cat /etc/systemd/system/lfd.service.d/lfd.conf
      fi
      if [ -f /etc/init.d/mysql ]; then
        sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/mysql
      fi
      if [[ ! -d /etc/systemd/system/pure-ftpd.service.d || ! -f /etc/systemd/system/pure-ftpd.service.d/pure-ftpd.conf ]]; then
        mkdir -p /etc/systemd/system/pure-ftpd.service.d
cat > "/etc/systemd/system/pure-ftpd.service.d/pure-ftpd.conf" <<EFF
[Service]
RestartSec=1
LimitSTACK=524288
EFF
        systemctl daemon-reload
        systemctl restart pure-ftpd
        cat /etc/systemd/system/pure-ftpd.service.d/pure-ftpd.conf
      fi
    fi # centos 6 or centos 7

    if [[ "$CENTOS_SEVEN" != '7' ]]; then
      RSYSLOGRUNNING=$(service rsyslog status | awk '{ print $5 }' | sed 's/running.\.\./running/g')
      if [[ "$RSYSLOGRUNNING" = 'running' ]]; then
        service rsyslog restart
      fi
    else
      RSYSLOGRUNNING=$(systemctl status rsyslog 2>&1 | grep 'Active:' | grep 'running' >/dev/null; echo $?)
      if [[ "$RSYSLOGRUNNING" = '0' ]]; then
        systemctl daemon-reload
        systemctl restart rsyslog
      fi
    fi

        echo "********************************************************************"
  fi
}