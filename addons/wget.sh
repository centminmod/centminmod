#!/bin/bash
###########################################################
# set locale temporarily to english
# for wget compile due to some non-english
# locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
###########################################################
# wget source installer to /usr/local/bin/wget path for
# centminmod.com LEMP stacks
# installs newer wget version than available via centos RPM
# repos but does not interfere with YUM installed wget as it
# is just an alias wget command setup
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
DNF_ENABLE='n'
DNF_COPR='y'
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
LOCALCENTMINMOD_MIRROR='https://parts.centminmod.com'

ALTPCRE_VERSION='8.45'
ALTPCRELINKFILE="pcre-${ALTPCRE_VERSION}.tar.gz"
ALTPCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${ALTPCRELINKFILE}"

WGET_VERSION='1.20.3'
WGET_VERSION_SEVEN='1.20.3'
WGET_VERSION_EIGHT='1.21.4'
WGET_VERSION_NINE='1.21.4'
WGET_VERSION_TEN='1.25.0'
WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
WGET_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
WGET_OPENSSL='n'
WGET_STRACE='n'
CENTOS_ALPHATEST='y'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###########################################################
shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

ALTPCRELINKFILE="pcre-${ALTPCRE_VERSION}.tar.gz"
ALTPCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${ALTPCRELINKFILE}"

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
    # Try $4 first (Kitten format), check if it's a valid version number
    CENTOSVER_TEST=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1)
    if [[ "$CENTOSVER_TEST" =~ ^[0-9]+$ ]]; then
      # $4 contains version (Kitten: "AlmaLinux release 10.0")
      CENTOSVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2)
      ALMALINUXVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
    else
      # $4 is not numeric (Purple Lion: "AlmaLinux release 10.0 (Purple Lion)"), use $3
      CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
      ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
    fi
  else
    # EL8/EL9 continue using $3
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

if [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_EIGHT" -eq '8' ]] || [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_NINE" -eq '9' ]] || [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_TEN" -eq '10' ]]; then
  if [[ "$ORACLELINUX_TEN" -eq '10' ]]; then
    label_os=OracleLinux
    label_os_ver=10
    label_prefix='https://community.centminmod.com/forums/31/'
  elif [[ "$ROCKYLINUX_TEN" -eq '10' ]]; then
    label_os=RockyLinux
    label_os_ver=10
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=84'
  elif [[ "$ALMALINUX_TEN" -eq '10' ]]; then
    label_os=AlmaLinux
    label_os_ver=10
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=83'
  elif [[ "$ORACLELINUX_NINE" -eq '9' ]]; then
    label_os=OracleLinux
    label_os_ver=9
    label_prefix='https://community.centminmod.com/forums/31/'
  elif [[ "$ROCKYLINUX_NINE" -eq '9' ]]; then
    label_os=RockyLinux
    label_os_ver=9
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=84'
  elif [[ "$ALMALINUX_NINE" -eq '9' ]]; then
    label_os=AlmaLinux
    label_os_ver=9
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=83'
  elif [[ "$ORACLELINUX_EIGHT" -eq '8' ]]; then
    label_os=OracleLinux
    label_os_ver=8
    label_prefix='https://community.centminmod.com/forums/31/'
  elif [[ "$ROCKYLINUX_EIGHT" -eq '8' ]]; then
    label_os=RockyLinux
    label_os_ver=8
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=84'
  elif [[ "$ALMALINUX_EIGHT" -eq '8' ]]; then
    label_os=AlmaLinux
    label_os_ver=8
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=83'
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    label_os_ver=10
    label_os=CentOS
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=81'
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    label_os_ver=9
    label_os=CentOS
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=81'
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    label_os_ver=8
    label_os=CentOS
    label_prefix='https://community.centminmod.com/forums/31/?prefix_id=81'
  fi
  echo
  echo "$label_os ${label_os_ver} is currently not supported by Centmin Mod, please use CentOS 7.9+"
  echo "To follow EL${label_os_ver} compatibility for CentOS ${label_os_ver} / AlmaLinux ${label_os_ver} read thread at:"
  echo "https://community.centminmod.com/threads/18372/"
  echo "You can read CentOS 8 specific discussions via prefix tag link at:"
  echo "$label_prefix"
  exit 1
  echo
fi

if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  WGET_VERSION=$WGET_VERSION_SEVEN
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
fi
if [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  echo "EL${label_os_ver} Install Dependencies Start..."
  WGET_VERSION=$WGET_VERSION_EIGHT
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
fi
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  echo "EL${label_os_ver} Install Dependencies Start..."
  WGET_VERSION=$WGET_VERSION_NINE
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
fi
if [[ "$CENTOS_TEN" -eq '10' ]]; then
  echo "EL${label_os_ver} Install Dependencies Start..."
  WGET_VERSION=$WGET_VERSION_TEN
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
fi

if [ -f /usr/local/lib/libssl.a ]; then
    # echo "clean up old /usr/local/lib/libssl.a"
    rm -rf /usr/local/lib/libssl.a
    ldconfig
fi
if [ -f /usr/local/lib/libcrypto.a ]; then
    # echo "clean up old /usr/local/lib/libcrypto.a"
    rm -rf /usr/local/lib/libcrypto.a
    ldconfig
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

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$DNF_ENABLE" = [yY] ]]; then
  # yum -y -q install epel-release
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEREPO_DNF=' --disablerepo=rpmforge'
  else
    DISABLEREPO_DNF=""
  fi
fi

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
elif [ -f /usr/bin/sar ]; then
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
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
sar_call() {
  $SARCALL 1 1
}

libmetalink_install() {
  echo
  pushd "$DIR_TMP"
  rm -rf libmetalink
  git clone https://github.com/metalink-dev/libmetalink
  cd libmetalink
  ./buildconf
  ./configure
  make -j$(nproc)
  make install
  echo
  popd
}

patch_wget() {
  if [[ "$WGET_VERSION" = '1.20.2' && -f /usr/local/src/centminmod/patches/wget/x509_v_flag_partial_chain.patch ]]; then
    if [ ! -f x509_v_flag_partial_chain.patch ]; then
      cp -a /usr/local/src/centminmod/patches/wget/x509_v_flag_partial_chain.patch x509_v_flag_partial_chain.patch
      patch -p1 < x509_v_flag_partial_chain.patch
    fi
  fi
}

scl_install() {
  # if gcc version is less than 4.7 (407) install scl collection yum repo
  if [[ "$CENTOS_SIX" = '6' ]]; then
    # if devtoolset exists, enable it first before checking gcc versions
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
      fi
    else
      if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
      fi
    fi
    if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      echo "install centos-release-scl for newer gcc and g++ versions"
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl
        fi
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      fi
    fi
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
      if [[ "$DEVTOOLSETNINE" = [yY] ]]; then
        if [[ "$(rpm -ql gcc-toolset-9-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql gcc-toolset-9-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql gcc-toolset-9-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install gcc-toolset-9-gcc gcc-toolset-9-gcc-c++ gcc-toolset-9-binutils
        fi
        sar_call
        echo
        /opt/rh/gcc-toolset-9/root/usr/bin/gcc --version
        /opt/rh/gcc-toolset-9/root/usr/bin/g++ --version
      elif [[ "$DEVTOOLSETTEN" = [yY] ]]; then
        if [[ "$(rpm -ql gcc-toolset-10-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql gcc-toolset-10-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql gcc-toolset-10-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install gcc-toolset-10-gcc gcc-toolset-10-gcc-c++ gcc-toolset-10-binutils
        fi
        sar_call
        echo
        /opt/rh/gcc-toolset-10/root/usr/bin/gcc --version
        /opt/rh/gcc-toolset-10/root/usr/bin/g++ --version
      fi
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl
        fi
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      fi
  fi # centos 6 only needed
}

gccdevtools() {
  if [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    scl_install
    unset CC
    unset CXX
    if [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
      export CFLAGS="-Wimplicit-fallthrough=0"
      export CXXFLAGS="${CFLAGS}"
    else
      export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
      export CFLAGS="-Wimplicit-fallthrough=0"
      export CXXFLAGS="${CFLAGS}"
    fi
  elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  elif [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++"
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  elif [[ -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/gcc-toolset-9/root/usr/bin/gcc"
    export CXX="/opt/rh/gcc-toolset-9/root/usr/bin/g++"
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  elif [[ -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/gcc-toolset-10/root/usr/bin/gcc"
    export CXX="/opt/rh/gcc-toolset-10/root/usr/bin/g++"
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  elif [ "$CENTOS_EIGHT" = '8' ]; then
    unset CC
    unset CXX
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  fi
}

source_pcreinstall() {
  if [[ "$CENTOS_SEVEN" -eq '7' ]] && [[ "$(/usr/local/bin/pcre-config --version 2>&1 | grep -q ${ALTPCRE_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]] || [[ -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No UTF-8 support' >/dev/null 2>&1; echo $?)" = '0' ]] || [[ -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No just-in-time compiler support' >/dev/null 2>&1; echo $?)" = '0' ]] || [[ -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C >/dev/null 2>&1; echo $?)" != '0' ]]; then
  cd "$DIR_TMP"
  cecho "Download $ALTPCRELINKFILE ..." $boldyellow
  if [ -s "$ALTPCRELINKFILE" ]; then
    cecho "$ALTPCRELINKFILE Archive found, skipping download..." $boldgreen
  else
    wget --progress=bar "$ALTPCRELINK" --tries=3 
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: $ALTPCRELINKFILE download failed." $boldgreen
      exit #$ERROR
    else 
      cecho "Download done." $boldyellow
    fi
  fi
  
  tar xzf "$ALTPCRELINKFILE"
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $ALTPCRELINKFILE extraction failed." $boldgreen
    exit #$ERROR
  else 
    cecho "$ALTPCRELINKFILE valid file." $boldyellow
    echo ""
  fi
  cd "pcre-${ALTPCRE_VERSION}"
  make clean >/dev/null 2>&1
  CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_pcre_make_$DT.log" -f -s256 -tt -T -q make${MAKETHREADS}
  else
    make${MAKETHREADS}
  fi
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_pcre_make_install_$DT.log" -f -s256 -tt -T -q make install
  else  
    make install
  fi
  sar_call
  /usr/local/bin/pcre-config --version
  fi
}

source_wgetinstall() {
  if [[ "$WGET_REBUILD_ALWAYS" = [yY] || "$(/usr/local/bin/wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    libmetalink_install
    export METALINK_CFLAGS='-I/usr/local/include'
    export METALINK_LIBS='-L/usr/local/lib -lmetalink'
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [ ! -f /usr/include/idn2.h ]; then
    yum -q -y install libidn2-devel libidn2
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [ ! -f /usr/include/libpsl.h ]; then
    yum -q -y install libpsl libpsl-devel
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [ ! -f /usr/include/gpgme.h ]; then
    yum -q -y install gpgme gpgme-devel
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [ ! -f /usr/include/gnutls/gnutls.h ]; then
    yum -q -y install gnutls gnutls-devel
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [ ! -f /usr/include/libassuan2/assuan.h ]; then
    yum -q -y install libassuan-devel
  fi
  cd "$DIR_TMP"
  cecho "Download $WGET_FILENAME ..." $boldyellow
  if [ -s "$WGET_FILENAME" ]; then
    cecho "$WGET_FILENAME Archive found, skipping download..." $boldgreen
  else

    curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 "$WGET_LINK" | grep 'HTTP/' | grep '200'
    WGET_CURLCHECK=$?
    if [[ "$WGET_CURLCHECK" = '0' ]]; then
      wget --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3
    else
      WGET_LINK="$WGET_LINKLOCAL"
      echo "wget --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3"
      wget --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3
    fi
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: $WGET_FILENAME download failed." $boldgreen
      exit #$ERROR
    else 
      cecho "Download done." $boldyellow
    fi
  fi
  
  tar xzf "$WGET_FILENAME"
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $WGET_FILENAME extraction failed." $boldgreen
    exit #$ERROR
  else 
    cecho "$WGET_FILENAME valid file." $boldyellow
    echo ""
  fi
  cd "wget-${WGET_VERSION}"
  if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
    gccdevtools
  fi
  if [ -f config.status ]; then
    make clean
  fi
  patch_wget
  if [[ "$CENTOS_TEN" = '10' && "$(uname -m)" = 'x86_64' ]]; then
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -Wp,-D_GLIBCXX_ASSERTIONS -fexceptions -fstack-protector-strong -fasynchronous-unwind-tables -fstack-clash-protection -fcf-protection -grecord-gcc-switches -m64 -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    # ensure wget.sh installer utilises system openssl
    export OPENSSL_CFLAGS="-I /usr/include"
    export OPENSSL_LIBS="-L /usr/lib64 -lssl -lcrypto"
  elif [[ "$CENTOS_NINE" = '9' && "$(uname -m)" = 'x86_64' ]]; then
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -Wp,-D_GLIBCXX_ASSERTIONS -fexceptions -fstack-protector-strong -fasynchronous-unwind-tables -fstack-clash-protection -fcf-protection -grecord-gcc-switches -m64 -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    # ensure wget.sh installer utilises system openssl
    export OPENSSL_CFLAGS="-I /usr/include"
    export OPENSSL_LIBS="-L /usr/lib64 -lssl -lcrypto"
  elif [[ "$CENTOS_EIGHT" = '8' && "$(uname -m)" = 'x86_64' ]]; then
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -Wp,-D_GLIBCXX_ASSERTIONS -fexceptions -fstack-protector-strong -fasynchronous-unwind-tables -fstack-clash-protection -fcf-protection -grecord-gcc-switches -m64 -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    # ensure wget.sh installer utilises system openssl
    export OPENSSL_CFLAGS="-I /usr/include"
    export OPENSSL_LIBS="-L /usr/lib64 -lssl -lcrypto"
  elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" = 'x86_64' ]]; then
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    # ensure wget.sh installer utilises system openssl
    export OPENSSL_CFLAGS="-I /usr/include"
    export OPENSSL_LIBS="-L /usr/lib64 -lssl -lcrypto"
  else
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    if [[ "$CENTOS_TEN" -eq '10' ]]; then
      CACERT_BUNDLE_PATH='ca_certificate=/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem'
    else
      CACERT_BUNDLE_PATH='ca_certificate=/etc/pki/tls/certs/ca-bundle.crt'
    fi
    if [ -f /root/.wgetrc ]; then
      \cp -fp /root/.wgetrc /root/.wgetrc-bak
      echo "$CACERT_BUNDLE_PATH" > /root/.wgetrc
    else
      echo "$CACERT_BUNDLE_PATH" > /root/.wgetrc
    fi    
  fi
  # ./configure --with-ssl=openssl PCRE_CFLAGS="-I /usr/local/include" PCRE_LIBS="-L /usr/local/lib -lpcre"
  if [[ "$CENTOS_TEN" = '10' && "$WGET_OPENSSL" = [yY] ]]; then
    ./configure --with-ssl=openssl --with-metalink
  elif [[ "$CENTOS_NINE" = '9' && "$WGET_OPENSSL" = [yY] ]]; then
    ./configure --with-ssl=openssl --with-metalink
  elif [[ "$CENTOS_EIGHT" = '8' && "$WGET_OPENSSL" = [yY] ]]; then
    ./configure --with-ssl=openssl --with-metalink
  elif [[ "$CENTOS_EIGHT" = '8' && "$WGET_OPENSSL" != [yY] ]]; then
    ./configure --with-ssl=gnutls --with-metalink
  else
    ./configure --with-ssl=openssl
  fi
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    make check
    make distcheck
    strace -o "${CENTMINLOGDIR}/strace_wget_make_$DT.log" -f -s256 -tt -T -q make${MAKETHREADS}
  else
    make${MAKETHREADS}
  fi
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" -f -s256 -tt -T -q make install
  else
    make install
  fi
  sar_call
  echo "/usr/local/lib/" > /etc/ld.so.conf.d/wget.conf
  ldconfig
  if [[ ! "$(grep '^alias wget' /root/.bashrc)" ]] && [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" = '0' ]]; then
    echo "alias wget='/usr/local/bin/wget'" >> /root/.bashrc
  fi
  . /root/.bashrc
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ ! "$(grep '^alias wget' $HOME/.bashrc)" ]] && [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" = '0' ]]; then
      echo -e "\nalias wget='/usr/local/bin/wget'" >> $HOME/.bashrc
    fi
    . $HOME/.bashrc
  fi

  echo
  cecho "--------------------------------------------------------" $boldgreen
  echo "ldconfig -p | grep libpcre.so.1"
  ldconfig -p | grep libpcre.so.1
  echo
  echo "ldd $(which wget)"
  ldd $(which wget)
  cecho "--------------------------------------------------------" $boldgreen
  cecho "wget -V" $boldyellow
  wget -V
  cecho "--------------------------------------------------------" $boldgreen
  if [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" = '0' ]]; then
    cecho "wget ${WGET_VERSION} installed at /usr/local/bin/wget" $boldyellow
    cecho "https://community.centminmod.com/tags/wget/" $boldyellow
    if [[ "$WGET_STRACE" = [yY] ]]; then
      # ls -lah ${CENTMINLOGDIR} | grep $DT
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_$DT.log"
        cecho "strace make log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_$DT.log.gz" $boldyellow
      fi
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log"
        cecho "strace make install log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_install_$DT.log.gz" $boldyellow
      fi
    fi
  else
    cecho "wget ${WGET_VERSION} failed to update, still using system wget" $boldyellow
    cecho "https://community.centminmod.com/tags/wget/" $boldyellow
    cecho "install log: ${CENTMINLOGDIR}/wget_source_install_${DT}.log" $boldyellow
    if [[ "$WGET_STRACE" = [yY] ]]; then
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_$DT.log"
        cecho "strace make log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_$DT.log.gz" $boldyellow
      fi
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log"
        cecho "strace make install log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_install_$DT.log.gz" $boldyellow
      fi
    fi
  fi
  # clean up strace logs older than 14 days
  find "${CENTMINLOGDIR}" -type f -mtime +14 \( -name 'strace_wget_make*' ! -name "strace_pcre_make*" \) -print
  find "${CENTMINLOGDIR}" -type f -mtime +14 \( -name 'strace_wget_make*' ! -name "strace_pcre_make*" \) -exec rm -rf {} \;
  cecho "--------------------------------------------------------" $boldgreen
  echo
  fi
}

###########################################################################
case $1 in
  install)
starttime=$(TZ=UTC date +%s.%N)
{
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    source_pcreinstall
    source_wgetinstall
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/wget_source_install_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
echo "Total wget Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
tail -1 "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
  ;;
  pcre)
starttime=$(TZ=UTC date +%s.%N)
{
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    source_pcreinstall
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
echo "Total wget pcre Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
tail -1 "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit