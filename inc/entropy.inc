entropyup() {

  if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
      if [[ ! -f /usr/bin/dnf ]]; then
        yum -y -q install dnf
      fi
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
          DISABLEREPO_DNF=' --disablerepo=rpmforge'
          YUMDNFBIN="dnf${DISABLEREPO_DNF}"
      else
          DISABLEREPO_DNF=""
          YUMDNFBIN='dnf'
      fi
  else
      YUMDNFBIN='yum'
  fi

if [[ ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] ]]; then
  if [[ ! -f /etc/rc.d/init.d/haveged ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    echo "*************************************************"
    cecho "* Increase system Entropy pool availability" $boldgreen
    echo "*************************************************"
    # cat /proc/sys/kernel/random/entropy_avail
    if [[ $(rpm -q haveged >/dev/null 2>&1; echo $?) != '0' ]]; then
      ${YUMDNFBIN}${CACHESKIP} -y -q install haveged rng-tools
      sar_call
    fi

    if [ -f /etc/rc.d/init.d/haveged ]; then
      sed -i 's/-w 1024/-w 4067/' /etc/rc.d/init.d/haveged
      cmchkconfig haveged on
      cmservice haveged restart
    fi
    
  elif [[ ! -f /usr/lib/systemd/system/haveged.service ]] && [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo "*************************************************"
    cecho "* Increase system Entropy pool availability" $boldgreen
    echo "*************************************************"
    # cat /proc/sys/kernel/random/entropy_avail
    if [[ $(rpm -q haveged >/dev/null 2>&1; echo $?) != '0' ]]; then
      ${YUMDNFBIN}${CACHESKIP} -y -q install haveged rng-tools
      sar_call
    fi

      if [[ ! -d /etc/systemd/system/haveged.service.d || ! -f /etc/systemd/system/haveged.service.d/haveged.conf ]]; then
        mkdir -p /etc/systemd/system/haveged.service.d
cat > "/etc/systemd/system/haveged.service.d/haveged.conf" <<EFF
[Service]
ExecStart=
ExecStart=/usr/sbin/haveged -w 4067 -v 1 --Foreground
EFF
        systemctl daemon-reload
        systemctl enable haveged
        systemctl restart haveged
        cat /etc/systemd/system/haveged.service.d/haveged.conf
      fi
  fi
fi
}