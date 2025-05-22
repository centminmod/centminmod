#!/bin/bash
###############################################################
# reinstall CSF Firewall and backup and restore data
# instead of just saving csf conf file backups, this
# script dynamically gathers all whitelist, ban ips
# and restores to a fresh CSF Firewall installation
# configured to replicate Centmin Mod initial install
# of CSF Firewall.
# 
# this allows for updates and new settings to be 
# properly accounted for in csf conf files in
# subsequent CSF version updates.
# 
# if you just backed up the conf files and restored
# you may miss new conf file options and settings 
# introduced into CSF Firewall conf files.
# 
# written by George Liu (eva2000) centminmod.com
###############################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
RESTORE_DENYIPS='y'
RESTORE_ALLOWIPS='y'
FIREWALLD_DISABLE='y'
CMSDEBUG='n'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f ../inc/csfinstall.inc ]; then
  . ../inc/csfinstall.inc
# elif [ -f /usr/local/src/centminmod/inc/csfinstall.inc ]; then
#   . /usr/local/src/centminmod/inc/csfinstall.inc
else
    echo "can not find ../inc/csfinstall.inc"
    echo "$0 needs to be ran from"
    echo "/usr/local/src/centminmod/tools"
    exit
fi
if [ -f ../inc/csftweaks.inc ]; then
  . ../inc/csftweaks.inc
# elif [ -f /usr/local/src/centminmod/inc/csftweaks.inc ]; then
#   . /usr/local/src/centminmod/inc/csftweaks.inc
else
    echo "can not find ../inc/csftweaks.inc"
    echo "$0 needs to be ran from"
    echo "/usr/local/src/centminmod/tools"
    exit
fi
NOTICE='y'
###############################################################
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

###############################################################
# functions
#############

if [[ "$NOTICE" = [yY] ]]; then
  echo
  echo "-------------------------------------------------"
  echo "$0 is in beta testing phase"
  echo "only run on test servers right now"
  echo "-------------------------------------------------"
  echo
  read -ep "continue [y/n] ? " _proceed
  if [[ "$_proceed" != [yY] ]]; then
    echo
    echo "aborting..."
    echo
    exit
  fi
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
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
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  if [[ "$EL_VERID" -eq 10 ]]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2)
    ALMALINUXVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  else
    CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
    ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  fi
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ALMALINUX_TEN='10'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ROCKYLINUX_TEN='10'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ORACLELINUX_TEN='10'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    VZLINUX_TEN='10'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    CIRCLELINUX_TEN='10'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    NAVYLINUX_TEN='10'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    EUROLINUX_TEN='10'
  fi
fi

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

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
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7272' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7272' ]]; then
            # 7272 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7272
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7282' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7282' ]]; then
            # 7282 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7282
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7302' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7302' ]]; then
            # 7302 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7302
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7352' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7352' ]]; then
            # 7352 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7352
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7402' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7402' ]]; then
            # 7402 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7402
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7452' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7452' ]]; then
            # 7452 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7452
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7502' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7502' ]]; then
            # 7502 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7502
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7532' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7532' ]]; then
            # 7532 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7532
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7542' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7542' ]]; then
            # 7542 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7542
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7552' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7552' ]]; then
            # 7552 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7552
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7642' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7642' ]]; then
            # 7642 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7642
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7662' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7662' ]]; then
            # 7662 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7662
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7702' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7702' ]]; then
            # 7702 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7702
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7742' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7742' ]]; then
            # 7742 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7742
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7H12' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7H12' ]]; then
            # 7H12 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7H12
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F52' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F52' ]]; then
            # 7F52 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F52
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F72' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F72' ]]; then
            # 7F72 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F72
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7313' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7313' ]]; then
            # 7313 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7313
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7413' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7413' ]]; then
            # 7413 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7413
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7443' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7443' ]]; then
            # 7443 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7443
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7453' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7453' ]]; then
            # 7453 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7453
            CPUS=14
        elif [[ "$(grep -o 'AMD EPYC 7513' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7513' ]]; then
            # 7513 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7513
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7543' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7543' ]]; then
            # 7543 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7543
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7643' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7643' ]]; then
            # 7643 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7643
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7663' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7663' ]]; then
            # 7663 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7663
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7713' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7713' ]]; then
            # 7713 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7713
            CPUS=32
        elif [[ "$(grep -o 'AMD EPYC 73F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 73F3' ]]; then
            # 73F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/73F3
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 74F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 74F3' ]]; then
            # 74F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/74F3
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 75F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 75F3' ]]; then
            # 75F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/75F3
            CPUS=32
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
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7272' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7272' ]]; then
            # 7272 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7272
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7282' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7282' ]]; then
            # 7282 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7282
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7302' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7302' ]]; then
            # 7302 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7302
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7352' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7352' ]]; then
            # 7352 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7352
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7402' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7402' ]]; then
            # 7402 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7402
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7452' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7452' ]]; then
            # 7452 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7452
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7502' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7502' ]]; then
            # 7502 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7502
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7532' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7532' ]]; then
            # 7532 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7532
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7542' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7542' ]]; then
            # 7542 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7542
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7552' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7552' ]]; then
            # 7552 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7552
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7642' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7642' ]]; then
            # 7642 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7642
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7662' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7662' ]]; then
            # 7662 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7662
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7702' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7702' ]]; then
            # 7702 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7702
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7742' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7742' ]]; then
            # 7742 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7742
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7H12' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7H12' ]]; then
            # 7H12 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7H12
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F52' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F52' ]]; then
            # 7F52 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F52
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F72' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F72' ]]; then
            # 7F72 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F72
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7313' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7313' ]]; then
            # 7313 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7313
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7413' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7413' ]]; then
            # 7413 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7413
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7443' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7443' ]]; then
            # 7443 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7443
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7453' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7453' ]]; then
            # 7453 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7453
            CPUS=14
        elif [[ "$(grep -o 'AMD EPYC 7513' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7513' ]]; then
            # 7513 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7513
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7543' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7543' ]]; then
            # 7543 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7543
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7643' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7643' ]]; then
            # 7643 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7643
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7663' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7663' ]]; then
            # 7663 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7663
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7713' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7713' ]]; then
            # 7713 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7713
            CPUS=32
        elif [[ "$(grep -o 'AMD EPYC 73F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 73F3' ]]; then
            # 73F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/73F3
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 74F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 74F3' ]]; then
            # 74F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/74F3
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 75F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 75F3' ]]; then
            # 75F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/75F3
            CPUS=32
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

cmservice() {
  servicename=$1
  action=$2
  if [[ "$CENTOS_SIX" = '6' ]] && [[ "${servicename}" = 'haveged' || "${servicename}" = 'pure-ftpd' || "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
    echo "service ${servicename} $action"
    if [[ "$CMSDEBUG" = [nN] ]]; then
      service "${servicename}" "$action"
    fi
  else
    if [[ "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' ]]; then
      echo "service ${servicename} $action"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        service "${servicename}" "$action"
      fi
    elif [[ "${servicename}" = 'mysql' || "${servicename}" = 'mysqld' ]]; then
      servicename='mariadb'
      echo "systemctl $action ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        systemctl "$action" "${servicename}.service"
      fi
    fi
  fi
}

cmchkconfig() {
  servicename=$1
  status=$2
  if [[ "$CENTOS_SIX" = '6' ]] && [[ "${servicename}" = 'haveged' || "${servicename}" = 'pure-ftpd' || "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
    echo "chkconfig ${servicename} $status"
    if [[ "$CMSDEBUG" = [nN] ]]; then
      chkconfig "${servicename}" "$status"
    fi
  else
    if [[ "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' ]]; then
      echo "chkconfig ${servicename} $status"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        chkconfig "${servicename}" "$status"
      fi
    else
      if [ "$status" = 'on' ]; then
        status=enable
      fi
      if [ "$status" = 'off' ]; then
        status=disable
      fi
      echo "systemctl $status ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        systemctl "$status" "${servicename}.service"
      fi
    fi
  fi
}

backupdata() {
    # backup ip allow and ban lists to reapply on reinstalled CSF
    echo
    echo "---------------------------------------------------------------------"
    echo "create /etc/csf-backups directory"
    mkdir -p /etc/csf-backups
    echo "cp -a /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}"
    cp -a /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}
    echo "cp -a /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}"
    cp -a /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}
    echo "cp -a /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}"
    cp -a /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}
    echo "cp -a /etc/csf/csf.ignore /etc/csf-backups/csf.ignore-${DT}"
    cp -a /etc/csf/csf.ignore /etc/csf-backups/csf.ignore-${DT}
    echo "cp -a /etc/csf/csf.fignore /etc/csf-backups/csf.fignore-${DT}"
    cp -a /etc/csf/csf.fignore /etc/csf-backups/csf.fignore-${DT}
    echo "cp -a /etc/csf/csf.mignore /etc/csf-backups/csf.mignore-${DT}"
    cp -a /etc/csf/csf.mignore /etc/csf-backups/csf.mignore-${DT}
    echo "cp -a /etc/csf/csf.rignore /etc/csf-backups/csf.rignore-${DT}"
    cp -a /etc/csf/csf.rignore /etc/csf-backups/csf.rignore-${DT}
    echo "cp -a /etc/csf/csf.signore /etc/csf-backups/csf.signore-${DT}"
    cp -a /etc/csf/csf.signore /etc/csf-backups/csf.signore-${DT}
    echo "cp -a /etc/csf/csf.suignore /etc/csf-backups/csf.suignore-${DT}"
    cp -a /etc/csf/csf.suignore /etc/csf-backups/csf.suignore-${DT}
    echo "cp -a /usr/local/csf/tpl /etc/csf-backups/tpl-${DT}"
    cp -a /usr/local/csf/tpl /etc/csf-backups/tpl-${DT}
    echo "cp -a /usr/local/csf/profiles /etc/csf-backups/profiles-${DT}"
    cp -a /usr/local/csf/profiles/ /etc/csf-backups/profiles-${DT}

    echo
    echo "file backups at /etc/csf-backups"
    ls -lahrt /etc/csf-backups | grep "$DT"

    # backup temp ip bans
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup temp ips ban to /tmp/csf-tempips.txt"
    csf -t | awk '/DENY  / {print $2}' > /tmp/csf-tempips.txt

    # backup allowed ip whitelist
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup whitelisted ips to /tmp/csf-allowips.txt"
    awk '/\ #\ / {print $1}' /etc/csf/csf.allow > /tmp/csf-allowips.txt

    # backup denied ip ban list
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup denied ips to /tmp/csf-denyips.txt"
    awk '/\ #\ / {print $1}' /etc/csf/csf.deny > /tmp/csf-denyips.txt

    echo
    echo "---------------------------------------------------------------------"
    echo "Existing Port Whitelist Profile"
    grep -E '^TCP_|^TCP6_|^UDP_|^UDP6' /etc/csf/csf.conf
    TCP_INBACKUP=$(awk '/^TCP_IN/ {print $3}' /etc/csf/csf.conf)
    TCP_OUTBACKUP=$(awk '/^TCP_OUT/ {print $3}' /etc/csf/csf.conf)
    TCP6_INBACKUP=$(awk '/^TCP6_IN/ {print $3}' /etc/csf/csf.conf)
    TCP6_OUTBACKUP=$(awk '/^TCP6_OUT/ {print $3}' /etc/csf/csf.conf)
    UDP_INBACKUP=$(awk '/^UDP_IN/ {print $3}' /etc/csf/csf.conf)
    UDP_OUTBACKUP=$(awk '/^UDP_OUT/ {print $3}' /etc/csf/csf.conf)
    UDP6_INBACKUP=$(awk '/^UDP6_IN/ {print $3}' /etc/csf/csf.conf)
    UDP6_OUTBACKUP=$(awk '/^UDP6_OUT/ {print $3}' /etc/csf/csf.conf)
}

delcsf() {
    if [ -f /etc/csf/uninstall.sh ]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "Uninstall CSF Firewall"
    echo "---------------------------------------------------------------------"
      /etc/csf/uninstall.sh
    fi
}

restoredata() {
    # echo
    echo "---------------------------------------------------------------------"
    echo "Restoring CSF Firewall data"
    echo "---------------------------------------------------------------------"
    sed -i "s|^TCP_IN = .*|TCP_IN = $TCP_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP_OUT = .*|TCP_OUT = $TCP_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP6_IN = .*|TCP6_IN = $TCP6_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP6_OUT = .*|TCP6_OUT = $TCP6_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP_IN = .*|UDP_IN = $UDP_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP_OUT = .*|UDP_OUT = $UDP_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP6_IN = .*|UDP6_IN = $UDP6_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP6_OUT = .*|UDP6_OUT = $UDP6_OUTBACKUP|" /etc/csf/csf.conf
    if [[ "$RESTORE_DENYIPS" = [yY] ]]; then
        cat /tmp/csf-denyips.txt >> /etc/csf/csf.deny
    fi
    if [[ "$RESTORE_ALLOWIPS" = [yY] ]]; then
        cat /tmp/csf-allowips.txt >> /etc/csf/csf.allow
    fi

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}"
    sdiff -w 120 -s /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}"
    sdiff -w 120 -s /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}"
    sdiff -w 120 -s /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    csf -ra >/dev/null 2>&1

    echo "---------------------------------------------------------------------"
    echo "Check Whitelist Profile"
    echo "---------------------------------------------------------------------"

    grep -E '^TCP_|^TCP6_|^UDP_|^UDP6' /etc/csf/csf.conf

    echo
    echo "---------------------------------------------------------------------"
    echo "CSF Firewall data restored"
    echo "---------------------------------------------------------------------"
    if [[ "$RESTORE_DENYIPS" != [yY] ]]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "RESTORE_DENYIPS is disabled skipped restore of deny ips"
    echo "---------------------------------------------------------------------"
    fi
    if [[ "$RESTORE_ALLOWIPS" != [yY] ]]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "RESTORE_ALLOWPS is disabled skipped restore of allow ips"
    echo "---------------------------------------------------------------------"
    fi
    echo
}

additional_blocks() {
  csf --profile backup cmm-b4-censys-block
  # block censys.io scans
  # https://support.censys.io/getting-started/frequently-asked-questions-faq
  csf -d 141.212.121.0/24 censys
  csf -d 141.212.122.0/24 censys
  # https://whois.arin.net/rest/org/CENSY/nets
  csf -d 198.108.66.0/23 censys
  csf -d 198.108.204.216/29 censys
  
  csf --profile backup cmm-b4-shodan-block
  # block shodan scans
  # https://wiki.ipfire.org/configuration/firewall/blockshodan
  # http://romcheckfail.com/blocking-shodan-keeping-shodan-io-in-the-dark-from-scanning/
  # https://isc.sans.edu/api/threatlist/shodan/
  # https://isc.sans.edu/api/threatlist/shodan/?json
  # curl -s https://isc.sans.edu/api/threatlist/shodan/?json > isc-shodan.txt
  # cat isc-shodan.txt  | jq -r '.[] .ipv4'
  csf -d 104.131.0.69 hello.data.shodan.io
  csf -d 104.236.198.48 blog.shodan.io
  csf -d 185.163.109.66 goldfish.census.shodan.io
  csf -d 185.181.102.18 turtle.census.shodan.io
  csf -d 188.138.9.50 atlantic.census.shodan.io
  csf -d 198.20.69.72 census1.shodan.io
  csf -d 198.20.69.73 census1.shodan.io
  csf -d 198.20.69.74 census1.shodan.io
  csf -d 198.20.69.75 census1.shodan.io
  csf -d 198.20.69.76 census1.shodan.io
  csf -d 198.20.69.77 census1.shodan.io
  csf -d 198.20.69.78 census1.shodan.io
  csf -d 198.20.69.79 census1.shodan.io
  csf -d 198.20.69.96 census2.shodan.io
  csf -d 198.20.69.97 census2.shodan.io
  csf -d 198.20.69.98 census2.shodan.io
  csf -d 198.20.69.99 census2.shodan.io
  csf -d 198.20.69.100 census2.shodan.io
  csf -d 198.20.69.101 census2.shodan.io
  csf -d 198.20.69.102 census2.shodan.io
  csf -d 198.20.69.103 census2.shodan.io
  csf -d 198.20.70.111 census3.shodan.io
  csf -d 198.20.70.112 census3.shodan.io
  csf -d 198.20.70.113 census3.shodan.io
  csf -d 198.20.70.114 census3.shodan.io
  csf -d 198.20.70.115 census3.shodan.io
  csf -d 198.20.70.116 census3.shodan.io
  csf -d 198.20.70.117 census3.shodan.io
  csf -d 198.20.70.118 census3.shodan.io
  csf -d 198.20.70.119 census3.shodan.io
  csf -d 198.20.99.128 census4.shodan.io
  csf -d 198.20.99.129 census4.shodan.io
  csf -d 198.20.99.130 census4.shodan.io
  csf -d 198.20.99.131 census4.shodan.io
  csf -d 198.20.99.132 census4.shodan.io
  csf -d 198.20.99.133 census4.shodan.io
  csf -d 198.20.99.134 census4.shodan.io
  csf -d 198.20.99.135 census4.shodan.io
  csf -d 93.120.27.62 census5.shodan.io
  csf -d 66.240.236.119 census6.shodan.io
  csf -d 71.6.135.131 census7.shodan.io
  csf -d 66.240.192.138 census8.shodan.io
  csf -d 71.6.167.142 census9.shodan.io
  csf -d 82.221.105.6 census10.shodan.io
  csf -d 82.221.105.7 census11.shodan.io
  csf -d 71.6.165.200 census12.shodan.io
  csf -d 216.117.2.180 census13.shodan.io
  csf -d 198.20.87.98 border.census.shodan.io
  csf -d 208.180.20.97 shodan.io
  csf -d 209.126.110.38 atlantic.dns.shodan.io
  csf -d 66.240.219.146 burger.census.shodan.io
  csf -d 71.6.146.185 pirate.census.shodan.io
  csf -d 71.6.158.166 ninja.census.shodan.io
  csf -d 85.25.103.50 pacific.census.shodan.io
  csf -d 71.6.146.186 inspire.census.shodan.io
  csf -d 85.25.43.94 rim.census.shodan.io
  csf -d 89.248.167.131 mason.census.shodan.io
  csf -d 89.248.172.16 house.census.shodan.io
  csf -d 93.174.95.106 battery.census.shodan.io
  csf -d 198.20.87.96 border.census.shodan.io
  csf -d 198.20.87.97 border.census.shodan.io
  csf -d 198.20.87.99 border.census.shodan.io
  csf -d 198.20.87.100 border.census.shodan.io
  csf -d 198.20.87.101 border.census.shodan.io
  csf -d 198.20.87.102 border.census.shodan.io
  csf -d 198.20.87.103 border.census.shodan.io
  csf -d 94.102.49.190 flower.census.shodan.io
  csf -d 94.102.49.193 cloud.census.shodan.io
  csf -d 71.6.146.130 refrigerator.census.shodan.io
  csf -d 159.203.176.62 private.shodan.io
  csf -d 188.138.1.119 atlantic249.serverprofi24.com
  csf -d 80.82.77.33 sky.census.shodan.io
  csf -d 80.82.77.139 dojo.census.shodan.io
  csf -d 66.240.205.34 malware-hunter.census.shodan.io
  csf -d 188.138.9.50 atlantic481.serverprofi24.com
  # whitelisting IPs for downloads/services Centmin Mod relies on
  csf --profile backup cmm-b4-whitelist
  # whitelist CSF Firewall's download url otherwise unable to download CSF Firewall updates
  dig +short A download.configserver.com | while read i; do csf -a $i csf-download.configserver.com; done
  # whitelist centminmod.com IPs which Centmin Mod LEMP stack relies on for some downloaded 
  # dependencies and file download updates
  dig +short A centminmod.com | while read i; do csf -a $i centminmod.com; done
  # whitelist nginx.org download IPs
  dig +short A nginx.org | while read i; do csf -a $i nginx.org; done
  csf --profile backup cmm-after-whitelist
  csf --profile list
}

cleanup() {
    echo "cleaning up temp files"
    rm -rf /tmp/csf-tempips.txt
    rm -rf /tmp/csf-allowips.txt
    rm -rf /tmp/csf-denyips.txt
}

trap cleanup SIGHUP SIGINT SIGTERM
######################################################
{
backupdata
delcsf
csfinstalls
    echo
    echo "---------------------------------------------------------------------"
    echo "CSF Firewall reinstalled"
    echo "---------------------------------------------------------------------"
    echo
restoredata
additional_blocks
cleanup
} 2>&1 | tee "${CENTMINLOGDIR}/csf-reinstall_${DT}.log"
echo
echo "---------------------------------------------------------------------"
echo "saved log: ${CENTMINLOGDIR}/csf-reinstall_${DT}.log"
echo "---------------------------------------------------------------------"
