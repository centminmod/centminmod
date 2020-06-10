#!/bin/bash
######################################################
# systemd journald logging configuration for
# CentOS 7+ 64bit systems only
# written by George Liu (centminmod.com)
# https://www.freedesktop.org/software/systemd/man/journald.conf.html
######################################################
MEMBASED='y'

######################################################
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

journald_usedisk() {
  dryrun_usedisk=$1
  # switch from journald default logging to
  # memory based tmpfs system at /run/log/journal
  # to disk based logging at /var/log/journal
  if [[ -f /usr/lib/systemd/system/systemd-journald.service ]]; then
    journald_diskusage
    echo
    echo "change journald logging from memory based to disk based at /var/log/journal/"
    if [[ ! "$dryrun_usedisk" ]]; then
      if [[ "$MEMBASED" = [nN] ]]; then
        mkdir -p /var/log/journal
        chmod 2755 /var/log/journal
        echo "mkdir -p /var/log/journal"
        echo "chmod 2755 /var/log/journal"
        echo
      elif [[ "$MEMBASED" = [yY] && -d /var/log/journal ]]; then
        rm -rf /var/log/journal
        echo "rm -rf /var/log/journal"
        echo
      fi
    elif [[ "$dryrun_usedisk" = 'dryrun' ]]; then
      echo
      echo "dry run"
      if [[ "$MEMBASED" = [nN] ]]; then
        echo "mkdir -p /var/log/journal"
        echo "chmod 2755 /var/log/journal"
        echo
      elif [[ "$MEMBASED" = [yY] && -d /var/log/journal ]]; then
        echo "rm -rf /var/log/journal"
        echo
      fi
    fi
  fi
}

journald_diskusage() {
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    echo
    echo "journalctl --disk-usage"
    journalctl --disk-usage
  fi
}

journald_status() {
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    systemctl status systemd-journald | sed -e "s|$(hostname)|hostname|g"
    journald_diskusage
  fi
}

journald_restart() {
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    systemctl restart systemd-journald
    sleep 2
    journald_status
  fi
}

journald_vacuum() {
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    journald_diskusage
    echo
    echo "journalctl --vacuum-time=1d"
    journalctl --vacuum-time=1d
    journald_diskusage
  fi
}

journald_size() {
    dryrun_size=$1
    var_disksize=$(df -mP /var | grep -v 'Used' | awk '{print $4}')
    var_disksize_threshold=$((($var_disksize*5)/100))
    free=$(free -m -w | awk '/Mem:/ {print $8}')
    if [[ "$free" -gt '3500' ]]; then
      limit=$((($free * 12)/100))
    else
      limit=$((($free * 6)/100))
    fi
    if [[ "$MEMBASED" = [yY] ]]; then
      limit=$((($limit * 90)/100))
    elif [[ "$MEMBASED" = [nN] ]]; then
      limit="$var_disksize_threshold"
    fi
    if [[ ! "$dryrun_size" ]]; then
      sed -i "s|^#SystemMaxUse=.*|SystemMaxUse=${limit}M|" /etc/systemd/journald.conf
      sed -i "s|^SystemMaxUse=.*|SystemMaxUse=${limit}M|" /etc/systemd/journald.conf
      sed -i "s|^#RuntimeMaxUse=.*|RuntimeMaxUse=${limit}M|" /etc/systemd/journald.conf
      sed -i "s|^RuntimeMaxUse=.*|RuntimeMaxUse=${limit}M|" /etc/systemd/journald.conf
      echo "set SystemMaxUse=${limit}M in /etc/systemd/journald.conf"
      echo "set RuntimeMaxUse=${limit}M in /etc/systemd/journald.conf"
      echo
    elif [[ "$dryrun_size" = 'dryrun' ]]; then
      echo "set SystemMaxUse=${limit}M in /etc/systemd/journald.conf"
      echo "set RuntimeMaxUse=${limit}M in /etc/systemd/journald.conf"
      echo
    fi
}

journald_config() {
  mode=$1
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    journald_diskusage
    journald_size "$mode"
    journald_usedisk "$mode"
    journald_restart
  fi
}

journald_check() {
  if [ -f /usr/lib/systemd/system/systemd-journald.service ]; then
    journald_status    
  fi
}


case "$1" in
  config )
    journald_config
    ;;
  config-dryrun )
    journald_config dryrun
    ;;
  check )
    journald_check
    ;;
  * )
    echo
    echo "Usage:"
    echo
    echo "$0 {config|config-dryrun|check}"
    echo
    ;;
esac

exit