#!/bin/bash
######################################################
# ntpd setup/resetup script to ensure ntp is working
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'

######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

setup_ntpd() {

  if [ ! -f /usr/sbin/ntpd ]; then
    yum -y install ntp
    chkconfig --levels 235 ntpd on
  fi

if [ -f /etc/ntp.conf ]; then
  if [[ -z "$(grep 'logfile' /etc/ntp.conf)" ]]; then
    echo "logfile /var/log/ntpd.log" >> /etc/ntp.conf
    ls -lahrt /var/log | grep 'ntpd.log'
  fi
  echo "current ntp servers"
  NTPSERVERS=$(awk '/server / {print $2}' /etc/ntp.conf | grep ntp.org | sort -r)
  for s in $NTPSERVERS; do
    echo -ne "\n$s test connectivity: "
    if [[ "$(echo | nc -u -w1 $s 53 >/dev/null 2>&1 ;echo $?)" = '0' ]]; then
      echo " ok"
     else
      echo " error"
    fi
    ntpdate -q $s | tail -1
    if [[ -f /etc/ntp/step-tickers && -z "$(grep $s /etc/ntp/step-tickers )" ]]; then
      echo "$s" >> /etc/ntp/step-tickers
    fi
  done
  if [ -f /etc/ntp/step-tickers ]; then
    echo -e "\nsetup /etc/ntp/step-tickers server list\n"
    cat /etc/ntp/step-tickers
  fi
  service ntpd restart >/dev/null 2>&1
  echo -e "\ncheck ntpd peers list"
  ntpdc -p
fi
}

######################################################
if [ ! -f /proc/user_beancounters ]; then
  starttime=$(TZ=UTC date +%s.%N)
  {
  setup_ntpd
  } 2>&1 | tee "${CENTMINLOGDIR}/tools_ntpdsh-${DT}.log"

  endtime=$(TZ=UTC date +%s.%N)
  INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
  echo "" >> "${CENTMINLOGDIR}/tools_ntpdsh-${DT}.log"
  echo "tools/ntpd.sh Run Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/tools_ntpdsh-${DT}.log"
else
  echo "OpenVZ system detected, ntp not used"
fi