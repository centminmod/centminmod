#!/bin/bash
###########################################################
# rlone client installer http://rclone.org
# written by George Liu centminmod.com
# http://rclone.org/install/
# http://rclone.org/docs/
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
DEBUG='y'

CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
RCLONE_BASEURL='https://downloads.rclone.org'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###########################################################
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

  if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
    HOSTDOMAIN=$(hostname)
  else
    HOSTDOMAIN=$(hostname -f)
  fi

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

if [[ "$RCLONE_ENABLE" != [yY] ]]; then
  echo
  echo "RCLONE_ENABLE='y' not set"
  echo "during beta testing of addons/rclone.sh you need to"
  echo "manually enable addons/rclone.sh via persistent config"
  echo "file at /etc/centminmod/custom_config.inc and add to it"
  echo
  echo "to edit persistent config, type command: customconfig"
  echo
  echo "add to it and hit CTRL+X to save file"
  echo
  echo "RCLONE_ENABLE='y'"
  echo
  exit
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
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

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

###########################################################
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

###########################################################
csf_port() {
  SSH_CLIENTIPADDR=$(echo "${SSH_CLIENT%% *}")
  echo
  if [[ -f /etc/csf/csf.allow && -z "$(grep "tcp|in|d=53682|s=$SSH_CLIENTIPADDR" /etc/csf/csf.allow)" ]]; then
    echo "open port 53682 for rclone web server"
    echo "tcp|in|d=53682|s=$SSH_CLIENTIPADDR" >> /etc/csf/csf.allow
    csf -ra >/dev/null 2>&1
    tail -1 /etc/csf/csf.allow
  fi
}

rclone_config() {
  echo
  echo "------------------------------------------------"
  echo "You need to now manually configure your storage provider"
  echo "from instructions at http://rclone.org/docs/"
  echo "------------------------------------------------"
  echo
  sleep 3
  if [ -f /usr/sbin/rclone ]; then
    # csf_port
    rclone config
  fi
}

rclone_install() {
  if [[ "$(uname -m)" = 'x86_64' ]]; then
      echo "------------------------------------------------"
      echo "Install Rclone 64bit"
      echo "------------------------------------------------"
      cd "$DIR_TMP"
      rm -rf rclone-*
      wget -${ipv_forceopt}cnv -O rclone-current-linux-amd64.zip "${RCLONE_BASEURL}/rclone-current-linux-amd64.zip"
      unzip rclone-current-linux-amd64.zip
      cd rclone-*-linux-amd64
      \cp -f rclone /usr/sbin/
      if [ -f /usr/sbin/rclone ]; then
        chown root:root /usr/sbin/rclone
        chmod 755 /usr/sbin/rclone
        mkdir -p /usr/local/share/man/man1
        \cp -f rclone.1 /usr/local/share/man/man1/
        mandb >/dev/null 2>&1
        ls -lah /usr/sbin/rclone
        echo
        echo "------------------------------------------------"
        echo "rclone binary at /usr/sbin/rclone"
        echo "------------------------------------------------"
      fi
  else
      echo "------------------------------------------------"
      echo "Install Rclone 32bit"
      echo "------------------------------------------------"
      cd "$DIR_TMP"
      rm -rf rclone-*
      wget -${ipv_forceopt}cnv -O rclone-current-linux-386.zip "${RCLONE_BASEURL}/rclone-current-linux-386.zip"
      unzip rclone-current-linux-386.zip
      cd rclone-*-linux-386
      \cp -f rclone /usr/sbin/
      if [ -f /usr/sbin/rclone ]; then
        chown root:root /usr/sbin/rclone
        chmod 755 /usr/sbin/rclone
        mkdir -p /usr/local/share/man/man1
        \cp -f rclone.1 /usr/local/share/man/man1/
        mandb >/dev/null 2>&1
        ls -lah /usr/sbin/rclone
        echo
        echo "------------------------------------------------"
        echo "rclone binary at /usr/sbin/rclone"
        echo "------------------------------------------------"
        # echo
      fi
  fi # centos 6 only needed
}

rclone_copy() {
  remote=$1
  if [[ -z "$remote" ]]; then
    echo
    echo "incorrect syntax"
    echo "use the following syntax where remote_name"
    echo "is the remote you configured with rclone"
    echo
    echo "$0 copy remote_name"
    exit
  fi
  echo
  echo "copy /root/centminlogs to cloud storage remote $remote"
  echo "https://community.centminmod.com/posts/39071/"
  echo
  echo "rclone copy /root/centminlogs ${remote}:${HOSTDOMAIN}/copy-centminlogs --exclude "rclone_copy_${DT}.log""
  rclone copy /root/centminlogs ${remote}:${HOSTDOMAIN}/copy-centminlogs --exclude "rclone_copy_${DT}.log"
  echo
  echo "copy /usr/local/nginx/conf to cloud storage remote $remote"
  echo
  echo "rclone copy /usr/local/nginx/conf ${remote}:${HOSTDOMAIN}/copy-nginxconf"
  rclone copy /usr/local/nginx/conf ${remote}:${HOSTDOMAIN}/copy-nginxconf
  echo
}

rclone_sync() {
  remote=$1
  if [[ -z "$remote" ]]; then
    echo
    echo "incorrect syntax"
    echo "use the following syntax where remote_name"
    echo "is the remote you configured with rclone"
    echo
    echo "$0 sync remote_name"
    exit
  fi
  echo
  echo "sync /root/centminlogs to cloud storage remote $remote"
  echo "https://community.centminmod.com/posts/39071/"
  echo
  echo "rclone sync /root/centminlogs ${remote}:${HOSTDOMAIN}/sync-centminlogs --exclude "rclone_sync_${DT}.log""
  rclone sync /root/centminlogs ${remote}:${HOSTDOMAIN}/sync-centminlogs --exclude "rclone_sync_${DT}.log"
  echo
  echo "sync /usr/local/nginx/conf to cloud storage remote $remote"
  echo
  echo "rclone sync /usr/local/nginx/conf ${remote}:${HOSTDOMAIN}/sync-nginxconf"
  rclone sync /usr/local/nginx/conf ${remote}:${HOSTDOMAIN}/sync-nginxconf
  echo
}

rclone_copyssl() {
  remote=$1
  if [[ -z "$remote" ]]; then
    echo
    echo "incorrect syntax"
    echo "use the following syntax where remote_name"
    echo "is the remote you configured with rclone"
    echo
    echo "$0 copyssl remote_name"
    exit
  fi
  echo
  echo "copy /usr/local/nginx/conf/ssl to cloud storage remote $remote"
  echo "https://community.centminmod.com/posts/39071/"
  echo
  echo "rclone copy /usr/local/nginx/conf/ssl ${remote}:${HOSTDOMAIN}/copy-nginxconf-ssl"
  rclone copy /usr/local/nginx/conf/ssl ${remote}:${HOSTDOMAIN}/copy-nginxconf-ssl
  echo
}

rclone_syncssl() {
  remote=$1
  if [[ -z "$remote" ]]; then
    echo
    echo "incorrect syntax"
    echo "use the following syntax where remote_name"
    echo "is the remote you configured with rclone"
    echo
    echo "$0 syncssl remote_name"
    exit
  fi
  echo
  echo "sync /usr/local/nginx/conf/ssl to cloud storage remote $remote"
  echo "https://community.centminmod.com/posts/39071/"
  echo
  echo "rclone sync /usr/local/nginx/conf/ssl ${remote}:${HOSTDOMAIN}/sync-nginxconf-ssl"
  rclone sync /usr/local/nginx/conf/ssl ${remote}:${HOSTDOMAIN}/sync-nginxconf-ssl
  echo
}

###########################################################################
case $1 in
  config)
starttime=$(TZ=UTC date +%s.%N)
{
  rclone_config
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_config_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_config_${DT}.log"
echo "Total Rclone Config Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_config_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_config_${DT}.log"
  ;;
  install)
starttime=$(TZ=UTC date +%s.%N)
{
  rclone_install
  if [ ! -f /root/.rclone.conf ]; then
    rclone_config
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_installer_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_installer_${DT}.log"
echo "Total Rclone Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_installer_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_installer_${DT}.log"
  ;;
  update)
starttime=$(TZ=UTC date +%s.%N)
{
  rclone_install
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_update_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_update_${DT}.log"
echo "Total Rclone Update Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_update_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_update_${DT}.log"
  ;;
  copy)
starttime=$(TZ=UTC date +%s.%N)
{
  remote=$2
  if [[ "$DEBUG" = [yY] ]]; then
    echo "remote = $remote"
  fi
  rclone_copy $remote
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_copy_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_copy_${DT}.log"
echo "Total Rclone Copy Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_copy_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_copy_${DT}.log"
  ;;
  sync)
starttime=$(TZ=UTC date +%s.%N)
{
  remote=$2
  if [[ "$DEBUG" = [yY] ]]; then
    echo "remote = $remote"
  fi
  rclone_sync $remote
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_sync_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_sync_${DT}.log"
echo "Total Rclone Sync Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_sync_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_sync_${DT}.log"
  ;;
  copyssl)
starttime=$(TZ=UTC date +%s.%N)
{
  remote=$2
  if [[ "$DEBUG" = [yY] ]]; then
    echo "remote = $remote"
  fi
  rclone_copyssl $remote
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_copyssl_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_copyssl_${DT}.log"
echo "Total Rclone Copy SSL Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_copyssl_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_copyssl_${DT}.log"
  ;;
  syncssl)
starttime=$(TZ=UTC date +%s.%N)
{
  remote=$2
  if [[ "$DEBUG" = [yY] ]]; then
    echo "remote = $remote"
  fi
  rclone_syncssl $remote
} 2>&1 | tee "${CENTMINLOGDIR}/rclone_syncssl_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/rclone_syncssl_${DT}.log"
echo "Total Rclone Sync SSL Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/rclone_syncssl_${DT}.log"
echo
tail -1 "${CENTMINLOGDIR}/rclone_syncssl_${DT}.log"
  ;;
  *)
    echo "$0 {config|install|update|copy|sync|copyssl|syncssl}"
  ;;
esac
exit