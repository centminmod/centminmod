entropyup() {

  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]  && [[ "$DNF_ENABLE" = [yY] ]]; then
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

  # haveged is only needed on EL7/8 (kernels 3.10/4.18)
  # EL9+ (kernel 5.14+) has modern RNG - haveged is obsolete per kernel 5.4+ changes
  # Reference: https://github.com/jirka-h/haveged/blob/master/README.md
  elif [[ ! -f /usr/lib/systemd/system/haveged.service ]] && [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' ]]; then
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