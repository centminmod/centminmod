#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
VER='0.0.8'
#####################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
######################################################
# ruby, rubygem, rails installer
# for Centminmod.com
# written by George Liu (eva2000) centminmod.com
# https://rvm.io/
######################################################
# https://rvm_io.global.ssl.fastly.net/binaries/centos/7/x86_64/
RUBYVER='2.6.10'
RUBYBUILD=''

DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
CONFIGSCANBASE='/etc/centminmod'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
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

###########################################
shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

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
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
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

if [ -f "${SCRIPT_DIR}/inc/custom_config.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "inc/custom_config.inc"
  fi
    source "inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
  fi
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

preyum() {
  if [[ ! -d /svr-setup ]]; then
    yum -y install gcc-c++ patch readline readline-devel zlib zlib-devel libyaml-devel libffi-devel make bzip2 autoconf automake libtool bison iconv-devel sqlite-devel openssl-devel
  elif [[ -z "$(rpm -ql libffi-devel)" || -z "$(rpm -ql libyaml-devel)" || -z "$(rpm -ql sqlite-devel)" ]]; then
    yum -y install libffi-devel libyaml-devel sqlite-devel
  fi

  if [[ "$(rpm -ql ruby | grep -v 'not installed')" || "$(rpm -ql ruby-libs | grep -v 'not installed')" || "$(rpm -ql rubygems | grep -v 'not installed')" ]]; then
    yum -y remove ruby ruby-libs rubygems --skip-broken --disableplugin=versionlock
    yum versionlock ruby ruby-libs rubygems
  fi

  mkdir -p /home/.ccache/tmp
}

installruby() {

if [[ -z $(which ruby >/dev/null 2>&1) || -z $(which rvm >/dev/null 2>&1) || -z $(which gem >/dev/null 2>&1) ]]; then

  groupadd rvm
  usermod -a -G rvm root
  
  echo "curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -"
  curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -
  echo "curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -"
  curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -
  \curl -L https://get.rvm.io | bash -s stable
  
  # clear duplicates previously added
  sed -i '/rvm.sh/d' $HOME/.bashrc
  if [[ ! "$(grep 'rvm.sh' $HOME/.bashrc)" ]]; then
    echo '[[ -s "/etc/profile.d/rvm.sh" ]] && source "/etc/profile.d/rvm.sh"  # This loads RVM into a shell session.' >> /root/.bashrc
  fi
  if [[ "$(id -u)" -ne '0' && ! "$(grep 'rvm.sh' $HOME/.bashrc)" ]]; then
    echo '[[ -s "/etc/profile.d/rvm.sh" ]] && source "/etc/profile.d/rvm.sh"  # This loads RVM into a shell session.' >> $HOME/.bashrc
  fi
  
  echo "checks..."
  if [ ! -f ~/.rvmrc ]; then
    touch ~/.rvmrc
    echo "rvm_silence_path_mismatch_check_flag=1" >> ~/.rvmrc
  fi
  source /etc/profile.d/rvm.sh
  echo "--------------------------------"
  echo "export PATH="$PATH""
  export PATH="$PATH"
  echo
  echo $PATH
  echo "--------------------------------"
  rvm requirements
  echo "--------------------------------"
  rvm list
  echo
  rvm list | awk -F " " '/^\=\*/ {print $2}'
  echo "--------------------------------"
  type rvm | head -1
  echo "--------------------------------"
  
  echo "rvm install ${RUBYVER}"
  echo "rvm use ${RUBYVER} --default"
  echo "rvm rubygems current --force"
  echo "--------------------------------" 
  echo $GEM_HOME
  echo $GEM_PATH
  echo "--------------------------------"
  echo "gem install rake"
  echo "gem install rails"
  echo "gem install sqlite3"
    
  echo "--------------------------------"
  # RUBYVER=$(rvm list | awk -F " " '/^\=\*/ {print $2}' | awk -F "-" '{print $2}')
  rvm install ${RUBYVER} 
  echo "--------------------------------"
  rvm use ${RUBYVER} --default
  echo "--------------------------------"

  echo "PATH echo..."
  echo "--------------------------------"
  rvm rubygems current --force
  echo "--------------------------------"
  gem env
  echo "--------------------------------"
  # find /usr/local/rvm/gems -name "concurrent-ruby-*.gem" -delete
  # find /usr/local/rvm/gems -name "rake-*.gem" -delete
  # find /usr/local/rvm/gems -name "rails-*.gem" -delete
  find /usr/local/rvm/gems/ruby-*/cache -type f -name "*.gem"
  gem install rake
  gem install rails
  gem install sqlite3
  echo "--------------------------------"
  
  echo "more checks..."
  echo "--------------------------------"
  ruby -v
  echo "--------------------------------"
  rails --version
  echo "--------------------------------"
  gem list
  echo "--------------------------------"
else
  echo "ruby or rvm or gem install already detected"
fi

} # installruby

###########################################################################
case $1 in
  install)
starttime=$(TZ=UTC date +%s.%N)
{
    if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        echo "EL7 install"
        preyum
        installruby
    elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        echo "EL8 module install"
        dnf module list ruby
        dnf module -y reset ruby
        dnf module -y enable ruby:3.0
        dnf module -y update ruby:3.0
    elif [[ "$CENTOS_NINE" -eq '9' ]]; then
        echo "EL9 install"
        yum -y install ruby rubygem rubygem-bundler ruby-libs
    fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log
echo "Total Ruby Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit