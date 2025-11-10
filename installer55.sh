#!/bin/bash
#######################################################
# centminmod.com cli installer
#
#######################################################
# some OS image templates are missing some locales that need to be installed
check_install_locale() {
  if [[ ! "$(locale -a | grep -qi "en_US.UTF8")" ]]; then
    local os_version=$(rpm -qa yum | grep -o 'el[0-9]*')
    case "$os_version" in
      el7)
          yum install -y glibc-common
          ;;
      el8|el9)
          yum install -y glibc-langpack-en
          ;;
    esac
  fi
}

check_install_locale
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
DT=$(date +"%d%m%y-%H%M%S")
exec > >(tee -a installer_${DT}.log) 2>&1
if [[ "$ARCH_CHECK" = 'aarch64' ]]; then echo; echo -e "Centmin Mod supports x86_64 CPUs only.\nARM based aarch64 CPUs not supported yet."; echo; exit 1; fi
#######################################################
# check if Centmin Mod already installed
FIRSTYUM_FILE=""

# Only run the find command if the directory exists
if [[ -d /root/centminlogs/ ]]; then
  FIRSTYUM_FILE=$(find /root/centminlogs/ -maxdepth 1 -type f -name "firstyum_installtime_*.log" | head -n 1)
fi

if [[ -f "$FIRSTYUM_FILE" ]] || [[ -f /usr/local/src/centminmod/centmin.sh && -f /usr/local/bin/php && -f /usr/local/sbin/nginx ]]; then
  echo
  echo "error: Detected that Centmin Mod has already been installed on this system"
  echo "       You are only meant to run initial installer once"
  echo "       If you want to reinstall Centmin Mod, you need to reinstall"
  echo "       your operating system first."
  echo
  exit
fi
mkdir -p /etc/centminmod
touch /etc/centminmod/custom_config.inc
#if [ ! "$(grep 'CENTOS_ALPHATEST' /etc/centminmod/custom_config.inc)" ]; then
#  echo "CENTOS_ALPHATEST='y'" >> /etc/centminmod/custom_config.inc
#fi
CENTOS_ALPHATEST='y'
#######################################################
DNF_ENABLE='n'
DNF_COPR='y'
branchname='141.00beta01'
DOWNLOAD="${branchname}.zip"
LOCALCENTMINMOD_MIRROR='https://parts.centminmod.com'
CPUS=$(nproc)

FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
INSTALLDIR='/usr/local/src'
DIR_TMP='/svr-setup'
#CUR_DIR="/usr/local/src/centminmod-${branchname}"
#CM_INSTALLDIR=$CUR_DIR
#SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
#####################################################
# Centmin Mod Git Repo URL - primary repo
# https://github.com/centminmod/centminmod
GITINSTALLED='y'
CMGIT='https://github.com/centminmod/centminmod.git'
# Gitlab backup repo 
# https://gitlab.com/centminmod/centminmod
#CMGIT='https://gitlab.com/centminmod/centminmod.git'
#####################################################
# wget renamed github
AXEL='n'
AXEL_VER='2.6'
AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

#######################################################
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

OS_PRETTY_NAME=$(cat /etc/os-release | awk -F '=' '/PRETTY_NAME/ {print $2}' | sed -e 's| (| |g' -e 's|)| |g' -e 's| Core ||g' -e 's|"||g')
CPUSPEED=$(awk -F: '/cpu MHz/{print $2}' /proc/cpuinfo | sort | uniq -c | sed -e s'|      ||g' | xargs); 
CPUMODEL=$(awk -F: '/model name/{print $2}' /proc/cpuinfo | sort | uniq -c | xargs);
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

if [ "$(id -u)" != 0 ]; then
  echo "script needs to be run as root user" >&2
  if [ "$(id -Gn | grep -o wheel)" ]; then
    echo "if using a sudo user, switch to full root first:" >&2
    echo >&2
    echo "sudo -i" >&2
  fi
  exit 1
fi

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
CENTMINLOGDIR='/root/centminlogs'
if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p $CENTMINLOGDIR
fi

lets_dst_root_ca_fix() {
  if [ "$(/usr/bin/openssl version | grep '1.0.2')" ] && [ ! -z "$(grep -i 'DST Root CA X3' /etc/pki/tls/certs/ca-bundle.crt)" ]; then
    echo
    echo "Update workaround to blacklist expiring Letsencrypt DST Root CA X3 certificate..."
    echo "https://community.centminmod.com/threads/21965/"
    echo
    mkdir -p /root/tools/backup-ca-certs
    if [ -f /etc/pki/tls/certs/ca-bundle.crt ]; then
      \cp -f /etc/pki/tls/certs/ca-bundle.crt /root/tools/backup-ca-certs/ca-bundle.crt-backup
    fi
    if [[ ! -f /usr/bin/trust || ! -f /usr/bin/update-ca-trust ]]; then
      yum -q -y install ca-certificates p11-kit-trust
    fi
    if [[ -f /usr/bin/trust && -f /usr/bin/update-ca-trust ]]; then
      trust dump --filter "pkcs11:id=%c4%a7%b1%a4%7b%2c%71%fa%db%e1%4b%90%75%ff%c4%15%60%85%89%10" | openssl x509 > /etc/pki/ca-trust/source/blacklist/DST-Root-CA-X3.pem
      update-ca-trust extract
      diff /root/tools/backup-ca-certs/ca-bundle.crt-backup /etc/pki/tls/certs/ca-bundle.crt > /root/tools/backup-ca-certs/diff-ca-bundle.crt.diff
      echo "Diff check file at /root/tools/backup-ca-certs/diff-ca-bundle.crt.diff"
      echo
      echo "Check to see if DST Root CA X3 is blacklisted"
      echo "trust list | grep -C3 'DST Root CA X3' | grep -B1 'blacklisted'"
      echo
      trust list | grep -C3 'DST Root CA X3' | grep -B1 'blacklisted'
      echo
      echo "Update ca-certificates YUM package for permanent fix"
      yum -q -y update ca-certificates
      echo "Updated ca-certificates"
      yum -q history list ca-certificates
    fi
  fi
}
lets_dst_root_ca_fix

# sudo adjustment
  if [ -d /etc/sudoers.d ]; then
    if [ ! -f /etc/sudoers.d/addpaths ]; then
      touch /etc/sudoers.d/addpaths
     if [[ "$(uname -m)" = 'x86_64' ]]; then
      if ! grep -q '/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin' /etc/sudoers.d/addpaths 2>/dev/null; then
        echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
      fi
    else
      if ! grep -q '/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin' /etc/sudoers.d/addpaths 2>/dev/null; then
        echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
      fi
    fi

    fi
    if [ -f /etc/sudoers.d/addpaths ]; then
      chmod 0440 /etc/sudoers.d/addpaths
      # visudo -c -q
    fi
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

# Almalinux 8.8 changed GPG keys breaking YUM for <=8.7
# https://almalinux.org/blog/2023-12-20-almalinux-8-key-update/
if [[ "$ALMALINUXVER" -ge '80000' && "$ALMALINUXVER" -le '80007' ]]; then
  rpm --import https://repo.almalinux.org/almalinux/RPM-GPG-KEY-AlmaLinux
fi

# If set to yes, will abort centmin mod installation if the memory requirements are not met
# you can override this setting by setting ABORTINSTALL='n' in which case centmin mod
# install may either install successfully but very very slowly or crap out
# and fail to successfully install.
ABORTINSTALL='y'

#############################################################
TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALMEM_T=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALMEM_SWAP=$(awk '/SwapFree/ {print $2}' /proc/meminfo)
TOTALMEM_PHP=$(($TOTALMEM_T+$TOTALMEM_SWAP))

if [[ "$CENTOS_TEN" -eq '10' ]]; then
  if [[ "$ISMINMEM_OVERRIDE" = [yY] ]]; then
    ISMINMEM='1500000'  # 1.43GB in bytes
  else
    ISMINMEM='1730000'  # 1.7GB in bytes
  fi
  if [[ "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
    ISMINSWAP='2097152'  # 2.0GB in bytes
  else
    ISMINSWAP='3774873'  # 3.6GB in bytes
  fi
elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  if [[ "$ISMINMEM_OVERRIDE" = [yY] ]]; then
    ISMINMEM='1500000'  # 1.43GB in bytes
  else
    ISMINMEM='1730000'  # 1.7GB in bytes
  fi
  if [[ "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
    ISMINSWAP='2097152'  # 2.0GB in bytes
  else
    ISMINSWAP='3774873'  # 3.6GB in bytes
  fi
elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  ISMINMEM='922624'  # 900MB in bytes
  ISMINSWAP='2097152'  # 2GB in bytes
else
  ISMINMEM='262144'  # 256MB in bytes
  ISMINSWAP='524288'  # 512MB in bytes
fi

#############################################################
# Formulas
if [[ "$(rpm -qa bc | grep -o 'bc')" != 'bc' ]]; then
  yum -y -q install bc
fi
if [[ ! -f /.dockerenv ]]; then
  if [[ ! -f /usr/bin/expr ]]; then
    yum -y -q install coreutils
  fi
else
  if rpm -q coreutils-single >/dev/null 2>&1; then
    echo "coreutils-single package is already installed. expr command should be available."
  elif rpm -q coreutils >/dev/null 2>&1; then
    echo "coreutils package is already installed. expr command should be available."
  else
    if [[ ! -f /usr/bin/expr ]]; then
      if yum -y -q install coreutils-single >/dev/null 2>&1; then
        echo "coreutils-single package installed successfully."
      else
        echo "Failed to install coreutils-single package. Attempting to install coreutils package..."
        if yum -y -q install coreutils >/dev/null 2>&1; then
          echo "coreutils package installed successfully."
        else
          echo "Failed to install coreutils package."
          exit 1
        fi
      fi
    fi
  fi
fi
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISMINMEMMB=`echo "scale=0;$ISMINMEM/1024" | bc`
ISMINSWAPMB=`echo "scale=0;$ISMINSWAP/1024" | bc`
CHECKMINMEM=`expr $TOTALMEM_T \< $ISMINMEM`

#############################################################
lowmemcheck() {
  # Check memory and swap threshold
  if [ "$CHECKMINMEM" == "1" ]; then
    if [ "$TOTALMEM_SWAP" -lt "$ISMINSWAP" ]; then
      CPUS='1'
      MAKETHREADS=" -j$CPUS"
      echo ""
      if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
        echo "For EL8 and EL9 operating system the minimum and recommended memory requirements have increased"
        echo "Minimum: 2GB memory with 4GB swap disk"
        echo "Recommended: 4GB memory with 4GB swap disk"
      fi
      echo -e "Warning: physically installed memory and swap too low for Centmin Mod\\nInstallation [Installed: $TOTALMEMMB MB < $ISMINMEMMB MB memory and $ISMINSWAPMB MB < $ISMINSWAPMB MB swap (recommended minimum)]\\n"
      if [ "$ABORTINSTALL" == 'y' ]; then
        echo "aborting install..."
        sleep 20
        exit
      fi
    else
      echo ""
      echo -e "Ok: swap is sufficient for Centmin Mod installation despite low memory\\nInstallation [Installed: $TOTALMEMMB MB < $ISMINMEMMB MB memory, but $TOTALMEM_SWAP MB >= $ISMINSWAPMB MB swap]\\n"
    fi
  else
    echo ""
    echo -e "Ok: physically installed memory is sufficient for Centmin Mod\\nInstallation [Installed: $TOTALMEMMB MB >= $ISMINMEMMB MB memory]\\n"
  fi
}

swap_setup() {
# swap file detection and setup routine add a 4GB swap file
# to servers without swap setup and non-openvz based as a
# precaution for low memory vps systems <2GB which require
# memory intensive initial install and running i.e. php fileinfo
# extension when enabled via PHPFINFO='y' need more memory ~2GB
# on <2GB systems this can be a problem without a swap file as
# an additional memory buffer

FINDSWAPSIZE=$(free -m | awk '/Swap: / {print $2}' | head -n1)

# if free -m output swap size = 0, create a 4GB swap file for
# non-openvz systems or if less than 2GB of memory and swap
# smaller than 4GB on non-openvz systems, create a 4GB additional
# swap file
if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  if [[ "$ISMINSWAP_OVERRIDE" = [yY] && "$FINDSWAPSIZE" -eq '0' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && ! -f /swapfile ]] || [[ "$ISMINSWAP_OVERRIDE" = [yY] && "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -le '2097152' && "$FINDSWAPSIZE" -le '2047' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && ! -f /swapfile ]]; then
    {
      echo
      free -m
      echo
      if [[ "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
        echo "create 2GB swap file";
        dd_size=2048
        fallocate_size=2
      else
        echo "create 4GB swap file";
        dd_size=4096
        fallocate_size=4
      fi
      if [[ "$(df -hT | grep -w xfs)" || "$(virt-what | grep -o lxc)" = 'lxc' ]]; then
        dd if=/dev/zero of=/swapfile bs=$dd_size count=1048576;
      else
        fallocate -l ${fallocate_size}G /swapfile
      fi
      ls -lah /swapfile;
      mkswap /swapfile;
      swapon /swapfile;
      chown root:root /swapfile;
      chmod 0600 /swapfile;
      swapon -s;
      echo "/swapfile swap swap defaults 0 0" >> /etc/fstab;
      mount -a;
      free -m
      echo
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_swapsetup_installer_${DT}.log"
  elif [[ "$FINDSWAPSIZE" -eq '0' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && ! -f /swapfile ]] || [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -le '2097152' && "$FINDSWAPSIZE" -le '4096' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && ! -f /swapfile ]]; then
    {
      echo
      free -m
      echo
      if [[ "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
        echo "create 2GB swap file";
        dd_size=2048
        fallocate_size=2
      else
        echo "create 4GB swap file";
        dd_size=4096
        fallocate_size=4
      fi
      if [[ "$(df -hT | grep -w xfs)" || "$(virt-what | grep -o lxc)" = 'lxc' ]]; then
        dd if=/dev/zero of=/swapfile bs=$dd_size count=1048576;
      else
        fallocate -l ${fallocate_size}G /swapfile
      fi
      ls -lah /swapfile;
      mkswap /swapfile;
      swapon /swapfile;
      chown root:root /swapfile;
      chmod 0600 /swapfile;
      swapon -s;
      echo "/swapfile swap swap defaults 0 0" >> /etc/fstab;
      mount -a;
      free -m
      echo
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_swapsetup_installer_${DT}.log"
  elif [[ "$FINDSWAPSIZE" -eq '0' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && -f /swapfile && "$(grep '/swapfile' /etc/fstab)" ]] || [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -le '2097152' && "$FINDSWAPSIZE" -le '4096' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && -f /swapfile && "$(grep '/swapfile' /etc/fstab)" ]]; then
    {
      echo
      free -mlt
      echo
      echo "re-create 4GB swap file";
      swapoff -a
      if [[ "$(df -hT | grep -w xfs)" || "$(virt-what | grep -o lxc)" = 'lxc' ]]; then
        dd if=/dev/zero of=/swapfile bs=4096 count=1048576;
      else
        fallocate -l 4G /swapfile
      fi
      ls -lah /swapfile;
      mkswap /swapfile;
      swapon /swapfile;
      chown root:root /swapfile;
      chmod 0600 /swapfile;
      swapon -s;
      # echo "/swapfile swap swap defaults 0 0" >> /etc/fstab;
      mount -a;
      free -mlt
      echo
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_swapsetup_installer_${DT}.log"
  fi
elif [[ "$FINDSWAPSIZE" -eq '0' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && -f /swapfile && "$(grep '/swapfile' /etc/fstab)" ]] || [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -le '1048576' && "$FINDSWAPSIZE" -le '512' && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] && -f /swapfile && "$(grep '/swapfile' /etc/fstab)" ]]; then
    {
    echo
    free -mlt
    echo
    echo "re-create 1GB swap file";
    swapoff -a
    if [[ "$(df -hT | grep -w xfs)" || "$(virt-what | grep -o lxc)" = 'lxc' ]]; then
        dd if=/dev/zero of=/swapfile bs=4096 count=1024k;
    else
        fallocate -l 4G /swapfile
    fi
    ls -lah /swapfile;
    mkswap /swapfile;
    swapon /swapfile;
    chown root:root /swapfile;
    chmod 0600 /swapfile;
    swapon -s;
    # echo "/swapfile swap swap defaults 0 0" >> /etc/fstab;
    mount -a;
    free -mlt
    echo
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_swapsetup_installer_${DT}.log"
fi
# Recalculate swap after potential swap file creation
TOTALMEM_SWAP=$(awk '/SwapFree/ {print $2}' /proc/meminfo)
}

swap_setup
lowmemcheck

if [ ! -f /usr/sbin/virt-what ]; then
  yum -q -y install virt-what
fi

if [[ ! -f /proc/user_beancounters ]]; then
    if [[ -f /usr/bin/systemd-detect-virt && "$(/usr/bin/systemd-detect-virt)" = 'lxc' ]]; then
        CHECK_LXD='y'
    elif [[ -f $(which virt-what) ]]; then
        VIRT_WHAT_OUTPUT=$(virt-what | xargs)
        if [[ $VIRT_WHAT_OUTPUT == *'openvz'* ]]; then
            CHECK_LXD='n'
        elif [[ $VIRT_WHAT_OUTPUT == *'lxc'* ]]; then
            CHECK_LXD='y'
        fi
    fi
fi

# check for Docker environment to skip grub routines
if [[ ! -f /.dockerenv && "$CHECK_LXD" != 'y' ]]; then
  # earlier selinux check for el9 systems
  SELINUX_STATUS=$(getenforce)
  if [ -f /etc/default/grub ]; then
    SELINUX_STATUS_GRUB=$(grep 'selinux=0' /etc/default/grub)
  else
    SELINUX_STATUS_GRUB=""
  fi
  if [[ "$CENTOS_NINE" -eq '9' ]] && [[ -z "$SELINUX_STATUS_GRUB" ]]; then
    echo "Detected SELinux NOT disabled for EL9"
    echo "Adding selinux=0 to Kernel GRUB_CMDLINE_LINUX line in /etc/default/grub"
    echo
    # https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/9/html/using_selinux/changing-selinux-states-and-modes_using-selinux#Enabling_and_Disabling_SELinux-Disabling_SELinux_changing-selinux-states-and-modes
    if [ ! "$(rpm -qa grubby | grep grubby)" ]; then
      yum -y install grubby
    fi
    echo "grubby --update-kernel ALL --args selinux=0"
    grubby --update-kernel ALL --args selinux=0
    echo
    grep '^GRUB_CMDLINE_LINUX=' /etc/default/grub
    echo
    echo "Added selinux=0 to Kernel GRUB_CMDLINE_LINUX line in /etc/default/grub to disable SELinux"
    echo "This is the right way to disable SELinux in future as other run-time methods deprecated"
    echo "If you intend to use own custom Linux Kernels i.e. ELRepo, ensure you have selinux=0 set"
    echo "Please reboot system to disable SELinux then install Centmin Mod"
    exit
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' ]] && [[ -z "$SELINUX_STATUS_GRUB" ]]; then
    echo "Detected SELinux NOT disabled for EL8"
    echo "Adding selinux=0 to Kernel GRUB_CMDLINE_LINUX line in /etc/default/grub"
    if [ -f /etc/default/grub ]; then
      sed -i '/^GRUB_CMDLINE_LINUX=/ s/"$/ selinux=0"/' /etc/default/grub
      grep '^GRUB_CMDLINE_LINUX=' /etc/default/grub
    fi
    echo "Regenerating GRUB2 configuration"
    if [ ! -f /usr/sbin/grub2-mkconfig ]; then
      echo "/usr/sbin/grub2-mkconfig not found"
      echo "installing grub2-tools"
      yum -y install grub2-tools
    fi
    if [ -d /sys/firmware/efi ]; then
      # UEFI-based systems
      if [ -f /etc/almalinux-release ] && [ -f /boot/efi/EFI/almalinux/grub.cfg ]; then
        # AlmaLinux OS
        echo "grub2-mkconfig -o /boot/efi/EFI/almalinux/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/almalinux/grub.cfg
      elif [ -f /etc/rocky-release ] && [ -f /boot/efi/EFI/rocky/grub.cfg ]; then
        # Rocky Linux
        echo "grub2-mkconfig -o /boot/efi/EFI/rocky/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/rocky/grub.cfg
      elif [ -f /etc/oracle-release ] && [ -f /boot/efi/EFI/oracle/grub.cfg ]; then
        # Oracle Linux
        echo "grub2-mkconfig -o /boot/efi/EFI/oracle/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/oracle/grub.cfg
      elif [ -f /etc/vzlinux-release ] && [ -f /boot/efi/EFI/vzlinux/grub.cfg ]; then
        # VzLinux
        echo "grub2-mkconfig -o /boot/efi/EFI/vzlinux/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/vzlinux/grub.cfg
      elif [ -f /etc/circle-release ] && [ -f /boot/efi/EFI/circle/grub.cfg ]; then
        # Circle Linux
        echo "grub2-mkconfig -o /boot/efi/EFI/circle/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/circle/grub.cfg
      elif [ -f /etc/navylinux-release ] && [ -f /boot/efi/EFI/navylinux/grub.cfg ]; then
        # Navy Linux
        echo "grub2-mkconfig -o /boot/efi/EFI/navylinux/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/navylinux/grub.cfg
      elif [ -f /boot/efi/EFI/centos/grub.cfg ]; then
        # CentOS Stream
        echo "grub2-mkconfig -o /boot/efi/EFI/centos/grub.cfg"
        grub2-mkconfig -o /boot/efi/EFI/centos/grub.cfg
      else
      echo "GRUB2 configuration file not found for your distribution. Please check the file paths and update the  script accordingly."
        exit 1
      fi
    else
      # BIOS-based systems
      if [ -f /boot/grub2/grub.cfg ]; then
        echo "grub2-mkconfig -o /boot/grub2/grub.cfg"
        grub2-mkconfig -o /boot/grub2/grub.cfg
      else
      echo "GRUB2 configuration file not found for your distribution. Please check the file paths and update the  script accordingly."
        exit 1
      fi
    fi
    echo "Added selinux=0 to Kernel GRUB_CMDLINE_LINUX line in /etc/default/grub to disable SELinux"
    echo "This is the right way to disable SELinux in future as other run-time methods deprecated"
    echo "If you intend to use own custom Linux Kernels i.e. ELRepo, ensure you have selinux=0 set"
    echo "Please reboot system to disable SELinux then install Centmin Mod"
    exit
  fi
fi

# set el9 to utf8mb4 charset for MariaDB 10.6
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  #echo "DEVTOOLSETTEN='n'" >> /etc/centminmod/custom_config.inc
  #echo "DEVTOOLSETELEVEN='n'" >> /etc/centminmod/custom_config.inc
  #echo "DEVTOOLSETTWELVE='y'" >> /etc/centminmod/custom_config.inc
  echo "SET_DEFAULT_MYSQLCHARSET='utf8mb4'" >> /etc/centminmod/custom_config.inc
  echo "SELFSIGNEDSSL_ECDSA='y'" >> /etc/centminmod/custom_config.inc
  if [[ "$ISMINMEM_OVERRIDE" = [yY] && "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
    echo "PHPFINFO='n'" >> /etc/centminmod/custom_config.inc
  else
    echo "PHPFINFO='y'" >> /etc/centminmod/custom_config.inc
  fi
  echo "PHP_OVERWRITECONF='n'" >> /etc/centminmod/custom_config.inc
  echo "PYTHON_INSTALL_ALTERNATIVES='y'" >> /etc/centminmod/custom_config.inc
fi
# set el8 defaults
if [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  #echo "DEVTOOLSETTEN='n'" >> /etc/centminmod/custom_config.inc
  #echo "DEVTOOLSETELEVEN='n'" >> /etc/centminmod/custom_config.inc
  #echo "DEVTOOLSETTWELVE='y'" >> /etc/centminmod/custom_config.inc
  echo "SELFSIGNEDSSL_ECDSA='y'" >> /etc/centminmod/custom_config.inc
  if [[ "$ISMINMEM_OVERRIDE" = [yY] && "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
    echo "PHPFINFO='n'" >> /etc/centminmod/custom_config.inc
  else
    echo "PHPFINFO='y'" >> /etc/centminmod/custom_config.inc
  fi
  echo "PHP_OVERWRITECONF='n'" >> /etc/centminmod/custom_config.inc
  echo "PYTHON_INSTALL_ALTERNATIVES='y'" >> /etc/centminmod/custom_config.inc
fi
# set el7 defaults
if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  echo "DEVTOOLSETTEN='n'" >> /etc/centminmod/custom_config.inc
  echo "DEVTOOLSETELEVEN='y'" >> /etc/centminmod/custom_config.inc
  echo "SELFSIGNEDSSL_ECDSA='y'" >> /etc/centminmod/custom_config.inc
  echo "PHP_OVERWRITECONF='n'" >> /etc/centminmod/custom_config.inc
fi

# el8+ dnf/yum speed tweaks
if [[ -f /etc/dnf/dnf.conf && "$CPUS" -ge '2' ]]; then
  echo "Optimizing /etc/dnf/dnf.conf settings"
  if [[ "$CPUS" -eq '2' ]]; then
    max_dnf_downloads=4
  elif [[ "$CPUS" -eq '3' ]]; then
    max_dnf_downloads=4
  elif [[ "$CPUS" -eq '4' ]]; then
    max_dnf_downloads=6
  elif [[ "$CPUS" -eq '5' ]]; then
    max_dnf_downloads=6
  elif [[ "$CPUS" -eq '6' ]]; then
    max_dnf_downloads=6
  elif [[ "$CPUS" -eq '7' ]]; then
    max_dnf_downloads=7
  elif [[ "$CPUS" -eq '8' ]]; then
    max_dnf_downloads=8
  elif [[ "$CPUS" -eq '9' ]]; then
    max_dnf_downloads=9
  elif [[ "$CPUS" -ge '10' ]]; then
    max_dnf_downloads=10
  fi
  if [[ ! "$(grep 'max_parallel_downloads' /etc/dnf/dnf.conf)" ]]; then
    echo "max_parallel_downloads=$max_dnf_downloads" >> /etc/dnf/dnf.conf
  elif [[ "$(grep 'max_parallel_downloads' /etc/dnf/dnf.conf)" ]]; then
    sed -i "s|max_parallel_downloads=.*|max_parallel_downloads=$max_dnf_downloads|" /etc/dnf/dnf.conf
  fi
  if [[ ! "$(grep 'fastestmirror=' /etc/dnf/dnf.conf)" ]]; then
    echo "fastestmirror=True" >> /etc/dnf/dnf.conf
  elif [[ "$(grep 'fastestmirror=' /etc/dnf/dnf.conf)" ]]; then
    sed -i "s|fastestmirror=.*|fastestmirror=True|" /etc/dnf/dnf.conf
  fi
  dnf -y update --refresh
elif [[ -f /etc/dnf/dnf.conf && "$CPUS" -eq '1' ]]; then
  echo "Optimizing /etc/dnf/dnf.conf settings"
  if [[ ! "$(grep 'fastestmirror=' /etc/dnf/dnf.conf)" ]]; then
    echo "fastestmirror=True" >> /etc/dnf/dnf.conf
  elif [[ "$(grep 'fastestmirror=' /etc/dnf/dnf.conf)" ]]; then
    sed -i "s|fastestmirror=.*|fastestmirror=True|" /etc/dnf/dnf.conf
  fi
  dnf -y update --refresh
fi

if [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_NINE" -eq '9' ]] || [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_EIGHT" -eq '8' ]] || [[ "$CENTOS_ALPHATEST" != [yY] && "$CENTOS_NINE" -eq '9' ]]; then
  if [[ "$ORACLELINUX_NINE" -eq '9' ]]; then
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

  # enable CentOS 8 PowerTools repo for -devel packages
  if [ "$(yum repolist powertools | grep -ow 'powertools')" ]; then
    reponame_powertools=powertools
  elif [ "$(yum repolist all | grep -ow 'ol8_codeready_builder')" ]; then
    reponame_powertools=ol8_codeready_builder
  elif [ "$(yum repolist all | grep -ow 'ol9_codeready_builder')" ]; then
    reponame_powertools=ol9_codeready_builder
  else
    reponame_powertools=PowerTools
  fi
  if [ ! -f /usr/bin/yum-config-manager ]; then
    yum -q -y install yum-utils tar
    yum-config-manager --enable $reponame_powertools
  elif [ -f /usr/bin/yum-config-manager ]; then
    yum-config-manager --enable $reponame_powertools
  fi

  # disable native CentOS 8 AppStream repo based nginx, php & oracle mysql packages
  yum -q -y module disable nginx mariadb mysql php redis:5

  # install missing dependencies specific to CentOS 8
  # for csf firewall installs
  # if [ ! -f /usr/share/perl5/vendor_perl/Math/BigInt.pm ]; then
  #   echo "EL8 CSF Firewall dependency"
  #   yum -q -y install perl-Math-BigInt
  # fi
fi
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  echo "EL${label_os_ver} Install Dependencies Start..."
  WGET_VERSION=$WGET_VERSION_NINE
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"

  if [ "$(yum repolist all | grep -ow 'ol9_codeready_builder')" ]; then
    # oracle linux 9
    reponame_powertools=ol9_codeready_builder
  else
    # enable CentOS 9 crb repo for -devel packages
    reponame_powertools=crb
  fi

  if [ ! -f /usr/bin/yum-config-manager ]; then
    yum -q -y install yum-utils tar
    yum-config-manager --enable $reponame_powertools
  elif [ -f /usr/bin/yum-config-manager ]; then
    yum-config-manager --enable $reponame_powertools
  fi

  # disable native CentOS 9 AppStream repo based nginx, php & oracle mysql packages
  # yum -q -y module disable nginx mariadb mysql php redis:6

  # install missing dependencies specific to CentOS 9
  # for csf firewall installs
  # if [ ! -f /usr/share/perl5/vendor_perl/Math/BigInt.pm ]; then
  #   echo "EL9 CSF Firewall dependency"
  #   yum -q -y install perl-Math-BigInt
  # fi
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

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  AXEL_VER='2.16.1'
  AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
  AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
  AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

if [[ "$(uname -m)" = 'x86_64' ]]; then
  if [[ "$CENTOS_SIX" = '6' || "$CENTOS_SEVEN" = '7' ]] && [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/plugins=1/
:a
exclude=*.i386 *.i586 *.i686
.
:w
:q
EOF
  elif [[ "$CENTOS_EIGHT" = '8' ]] && [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/best=True/
:a
exclude=*.i686
.
:w
:q
EOF
  elif [[ "$CENTOS_NINE" = '9' ]] && [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/best=True/
:a
exclude=*.i686
.
:w
:q
EOF
  fi
fi

# some centos images don't even install tar by default !
if [[ "$CENTOS_NINE" -eq '9' && ! -f /usr/bin/tar ]]; then
  yum -y -q install tar
elif [[ "$CENTOS_EIGHT" -eq '8' && ! -f /usr/bin/tar ]]; then
  yum -y -q install tar
elif [[ "$CENTOS_SEVEN" -eq '7' && ! -f /usr/bin/tar ]]; then
  yum -y -q install tar
elif [[ "$CENTOS_SIX" -eq '6' && ! -f /bin/tar ]]; then
  yum -y -q install tar
fi

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$DNF_ENABLE" = [yY] ]]; then
  if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
    yum -y -q install epel-release
    yum clean all
  fi

  if [[ "$DNF_COPR" = [yY] ]]; then
cat > "/etc/yum.repos.d/dnf-centos.repo" <<EOF
[dnf-centos]
name=Copr repo for dnf-centos owned by @rpm-software-management
baseurl=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/pubkey.gpg
enabled=1
enabled_metadata=1
EOF
  fi
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
    dnf clean all
  fi
  if [ ! "$(grep -w 'exclude' /etc/dnf/dnf.conf)" ]; then
    echo "excludepkgs=*.i386 *.i586 *.i686" >> /etc/dnf/dnf.conf
  fi
  if [ ! "$(grep -w 'fastestmirror=true' /etc/dnf/dnf.conf)" ]; then
    echo "fastestmirror=true" >> /etc/dnf/dnf.conf
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

if [ -f /proc/user_beancounters ]; then
    echo "OpenVZ system detected, NTP not installed"
elif [[ "$CHECK_LXD" = [yY] ]]; then
    echo "LXC/LXD container system detected, NTP not installed"
else
  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
      echo
      echo "*************************************************"
      echo "* Installing chronyd and syncing time"
      echo "*************************************************"
      time $YUMDNFBIN -y install chrony
      systemctl start chronyd
      systemctl enable chronyd
      systemctl status chronyd --no-pager
      echo "current chrony ntp servers"
      chronyc sources
  else
    if [ ! -f /usr/sbin/ntpd ]; then
      echo "*************************************************"
      echo "* Installing NTP and syncing time"
      echo "*************************************************"
      echo "The date/time before was:"
      date
      echo
      time $YUMDNFBIN -y install ntp
      chkconfig ntpd on
      if [ -f /etc/ntp.conf ]; then
        if [[ -z "$(grep 'logfile' /etc/ntp.conf)" ]]; then
            echo "logfile /var/log/ntpd.log" >> /etc/ntp.conf
            ls -lahrt /var/log | grep 'ntpd.log'
        fi
        echo "current ntp servers"
        NTPSERVERS=$(awk '/server / {print $2}' /etc/ntp.conf | grep ntp.org | sort -r)
        for s in $NTPSERVERS; do
          if [ -f /usr/bin/nc ]; then
            echo -ne "\n$s test connectivity: "
            if [[ "$(echo | nc -u -w1 $s 53 >/dev/null 2>&1 ;echo $?)" = '0' ]]; then
            echo " ok"
            else
            echo " error"
            fi
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
    fi
  fi
  echo "The date/time is now:"
  date
fi

# only run for CentOS 6.x
if [[ "$CENTOS_SIX" = '6' ]]; then
    echo ""
    echo "Check for existing mysql-server packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mysql-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo ""
    echo "Check for existing mariadb packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mariadb-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
    echo ""
    echo "Check for existing mariadb-libs package"
    OLDMYSQL_LIBS=`rpm -qa | grep 'mariadb-libs' | head -n1`
    if [[ ! -z "$OLDMYSQL_LIBS" ]]; then
        # echo "rpm -e --nodeps $OLDMYSQL_LIBS"
        # rpm -e --nodeps $OLDMYSQL_LIBS
        echo "yum -y remove mariadb-libs"
        yum -y remove mariadb-libs
    fi
    echo ""
    # Should not exist on CentOS 7 systems
    echo "Check for existing MySQL-shared-compat"
    OLDMYSQL_SHAREDCOMPAT=`rpm -qa | grep 'MySQL-shared-compat' | head -n1`
    if [[ ! -z "$OLDMYSQL_SHAREDCOMPAT" ]]; then
        echo "yum -y remove MySQL-shared-compat"
        yum -y remove MySQL-shared-compat
    fi
fi # CENTOS_SEVEN != 7

sar_call() {
  $SARCALL 1 1
}

double_check_wget_bc() {
  if [ ! -f /usr/bin/wget ]; then
    yum -y -q install wget
  fi
  if [ ! -f /usr/bin/bc ]; then
    yum -y -q install bc
  fi
  if [ ! -f /usr/bin/nano ]; then
    yum -y -q install nano
  fi
}

systemstats() {
  if [ -d /root/centminlogs ]; then
    sar -u > /root/centminlogs/sar-u-installstats.log
    sar -q > /root/centminlogs/sar-q-installstats.log
    sar -r > /root/centminlogs/sar-r-installstats.log
    if [ ! -f /proc/user_beancounters ]; then
    sar -d > /root/centminlogs/sar-d-installstats.log
    fi
    sar -b > /root/centminlogs/sar-b-installstats.log
    if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
      SERVERHOSTNAME=$(hostname)
    else
      SERVERHOSTNAME=$(hostname -f)
    fi
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-u-installstats.log
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-q-installstats.log
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-r-installstats.log
    if [ ! -f /proc/user_beancounters ]; then
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-d-installstats.log
    fi
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-b-installstats.log
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/x86_64/7/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/x86_64/7/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      fi
    else
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      else
        if [ -f /var/cache/yum/i386/6/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/i386/6/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      fi
    fi
  fi
  if [ -f /etc/cron.d/cmsar ]; then
    rm -rf /etc/cron.d/cmsar
  fi
}

scl_install() {
  # if gcc version is less than 4.7 (407) install scl collection yum repo
  if [[ "$CENTOS_SIX" = '6' ]]; then
    # if devtoolset exists, enable it first before checking gcc versions
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
      fi
    else
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
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
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
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
      if [[ "$DEVTOOLSETNINE" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-binutils
          time $YUMDNFBIN -y -q install devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-binutils
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-binutils --disablerepo=rpmforge
          time $YUMDNFBIN -y -q install devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-binutils --disablerepo=rpmforge
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-9/root/usr/bin/gcc --version
        /opt/rh/devtoolset-9/root/usr/bin/g++ --version
      elif [[ "$DEVTOOLSETEIGHT" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-binutils
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-binutils --disablerepo=rpmforge
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-8/root/usr/bin/gcc --version
        /opt/rh/devtoolset-8/root/usr/bin/g++ --version
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
  fi # centos 6 only needed
}

gccdevtools() {
  if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ || ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    scl_install
    unset CC
    unset CXX
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
    else
      export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
    fi
  elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
  elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-4/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-4/root/usr/bin/g++" 
  fi
}

source_pcreinstall() {
  if [[ "$(/usr/local/bin/pcre-config --version 2>&1 | grep -q ${ALTPCRE_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
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
  CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
  sar_call
  make${MAKETHREADS}
  sar_call
  make install
  sar_call
  /usr/local/bin/pcre-config --version
  fi
}

source_wgetinstall() {
  if [[ "$(/usr/local/bin/wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
  cd "$DIR_TMP"
  cecho "Download $WGET_FILENAME ..." $boldyellow
  if [ -s "$WGET_FILENAME" ]; then
    cecho "$WGET_FILENAME Archive found, skipping download..." $boldgreen
  else
    wget --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3 
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
  make clean
  if [[ "$(uname -m)" = 'x86_64' ]]; then
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
    if [ -f /root/.wgetrc ]; then
      \cp -fp /root/.wgetrc /root/.wgetrc-bak
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
    else
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
    fi
  fi
  # ./configure --with-ssl=openssl PCRE_CFLAGS="-I /usr/local/include" PCRE_LIBS="-L /usr/local/lib -lpcre"
  ./configure --with-ssl=openssl
  sar_call
  make${MAKETHREADS}
  sar_call
  make install
  sar_call
  echo "/usr/local/lib/" > /etc/ld.so.conf.d/wget.conf
  ldconfig
  if [[ ! "$(grep '^alias wget' /root/.bashrc)" ]]; then
    echo "alias wget='/usr/local/bin/wget'" >> /root/.bashrc
  fi
  . /root/.bashrc
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ ! "$(grep '^alias wget' $HOME/.bashrc)" ]]; then
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
  cecho "wget ${WGET_VERSION} installed at /usr/local/bin/wget" $boldyellow
  cecho "--------------------------------------------------------" $boldgreen
  if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
    unset CFLAGS
  fi
  echo
  fi
}

fileperm_fixes() {
  if [ -f /usr/lib/udev/rules.d/60-net.rules ]; then
    if [[ "$(lsattr /usr/lib/udev/rules.d/60-net.rules | cut -c5)" = 'i' ]]; then
      # fix for some centos 7 vps templates on vps hosts setting chattr +i on
      # /usr/lib/udev/rules.d/60-net.rules preventing yum updates for initscripts
      # yum packages
      chattr -i /usr/lib/udev/rules.d/60-net.rules
    fi
  fi
}

libc_fix() {
  # https://community.centminmod.com/posts/52555/
  if [[ "$CENTOS_NINE" -eq '9' ]]; then
    # yum -y -q install python3-dnf-plugin-versionlock
    yum -y install libc-client uw-imap-devel
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
  elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
    # yum -y -q install python3-dnf-plugin-versionlock
    yum -y install libc-client uw-imap-devel
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
  elif [[ "$CENTOS_NINE" -eq '9' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" = 'libc-client-2007f-30.el9.remi.x86_64' ]]; then
    yum -y -q install python3-dnf-plugin-versionlock
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
  elif [[ "$CENTOS_EIGHT" -eq '8' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" = 'libc-client-2007f-24.el8.x86_64' ]]; then
    yum -y -q install python3-dnf-plugin-versionlock
    yum versionlock libc-client uw-imap-devel -q >/dev/null 2>&1
  elif [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" = 'libc-client-2007f-16.el7.x86_64' ]]; then
    yum -y install yum-plugin-versionlock uw-imap-devel
    yum versionlock libc-client uw-imap-devel
  elif [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget ${LOCALCENTMINMOD_MIRROR}/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget ${LOCALCENTMINMOD_MIRROR}/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y remove libc-client
    yum -y localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y install yum-plugin-versionlock
    yum versionlock libc-client uw-imap-devel uw-imap-devel
    cd "$INIT_DIR"
   elif [[ "$CENTOS_SEVEN" -eq '7' && -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget ${LOCALCENTMINMOD_MIRROR}/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget ${LOCALCENTMINMOD_MIRROR}/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock delete libc-client uw-imap-devel
    yum -y remove libc-client
    yum -y localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock libc-client uw-imap-devel uw-imap-devel
    cd "$INIT_DIR" 
  fi
}

opt_tcp() {
#######################################################
# check if custom open file descriptor limits already exist
    LIMITSCONFCHECK=`grep '* hard nofile 524288' /etc/security/limits.conf`
    if [[ -z $LIMITSCONFCHECK ]]; then
        # Set VPS hard/soft limits
        echo "* soft nofile 524288" >>/etc/security/limits.conf
        echo "* hard nofile 524288" >>/etc/security/limits.conf
# https://community.centminmod.com/posts/52406/
if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [ ! -f /etc/rc.d/rc.local ]; then


cat > /usr/lib/systemd/system/rc-local.service <<EOF
# This unit gets pulled automatically into multi-user.target by
# systemd-rc-local-generator if /etc/rc.d/rc.local is executable.
[Unit]
Description=/etc/rc.d/rc.local Compatibility
ConditionFileIsExecutable=/etc/rc.d/rc.local
After=network.target

[Service]
Type=forking
ExecStart=/etc/rc.d/rc.local start
TimeoutSec=0
RemainAfterExit=yes
EOF

cat > /etc/rc.d/rc.local <<EOF
#!/bin/bash
# THIS FILE IS ADDED FOR COMPATIBILITY PURPOSES
#
# It is highly advisable to create own systemd services or udev rules
# to run scripts during boot instead of using this file.
#
# In contrast to previous versions due to parallel execution during boot
# this script will NOT be run after all other services.
#
# Please note that you must run 'chmod +x /etc/rc.d/rc.local' to ensure
# that this script will be executed during boot.

touch /var/lock/subsys/local
EOF

# remove non-standard centos 7 service file if detected
if [ -f /etc/systemd/system/rc-local.service ]; then
  echo "cat /etc/systemd/system/rc-local.service"
  cat /etc/systemd/system/rc-local.service
  echo
  rm -rf /etc/systemd/system/rc-local.service
  rm -rf /var/lock/subsys/local
  systemctl daemon-reload
  systemctl stop rc-local.service
fi

  chmod +x /etc/rc.d/rc.local
  pushd /etc; ln -s rc.d/rc.local /etc/rc.local; popd
  systemctl daemon-reload
  systemctl start rc-local.service
  systemctl status rc-local.service --no-pager
fi
        ulimit -n 524288
        echo "ulimit -n 524288" >> /etc/rc.local
        if [[ ! "$(grep '/var/run/php-fpm' /etc/rc.local)" ]]; then
          echo 'if [ ! -d /var/run/php-fpm/ ]; then mkdir -p /var/run/php-fpm/; fi' >> /etc/rc.local
        fi
    fi # check if custom open file descriptor limits already exist

    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
        # centos 8
        if [[ -f /etc/security/limits.d/20-nproc.conf ]]; then
cat > "/etc/security/limits.d/20-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
      fi
    elif [[ "$CENTOS_SEVEN" = '7' ]]; then
        # centos 7
        if [[ -f /etc/security/limits.d/20-nproc.conf ]]; then
cat > "/etc/security/limits.d/20-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
      fi
    else
        # centos 6
        if [[ -f /etc/security/limits.d/90-nproc.conf ]]; then
cat > "/etc/security/limits.d/90-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
        fi # raise user process limits
    fi

if [[ ! -f /proc/user_beancounters ]]; then
    if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
        TCPMEMTOTAL=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
        if [ "$TCPMEMTOTAL" -le '3000000' ]; then
          TCP_OPTMEM_MAX='8192'
        else
          TCP_OPTMEM_MAX='81920'
        fi
        if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
          TCP_PID_MAX='4194300'
          TCP_BACKLOG='524280'
        elif [[ "$CENTOS_SEVEN" = '7' ]]; then
          TCP_PID_MAX='65535'
          TCP_BACKLOG='65535'
        fi
        if [[ ! -d /etc/sysctl.d || ! -f /usr/sbin/sysctl ]]; then
          # ensure sysctl is installed
          yum -y install procps-ng
        fi
        if [ -d /etc/sysctl.d ]; then
            # centos 7
            touch /etc/sysctl.d/101-sysctl.conf
            if [[ "$(grep 'centminmod added' /etc/sysctl.d/101-sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
cat >> "/etc/sysctl.d/101-sysctl.conf" <<EOF
# centminmod added
kernel.pid_max=$TCP_PID_MAX
kernel.printk=4 1 1 7
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=65536
net.core.somaxconn=$TCP_BACKLOG
net.core.optmem_max=$TCP_OPTMEM_MAX
net.ipv4.tcp_fin_timeout=30
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=$TCP_BACKLOG
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 0
net.ipv4.tcp_tw_reuse = 0
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_limit_output_bytes=65536
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.nf_conntrack_max = 524288
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024
net.unix.max_dgram_qlen = 4096
EOF
        if [[ "$(grep -o 'AMD EPYC' /proc/cpuinfo | sort -u)" = 'AMD EPYC' ]]; then
          echo "kernel.watchdog_thresh = 20" >> /etc/sysctl.d/101-sysctl.conf
        fi
        /sbin/sysctl --system
            fi           
        fi
    else
        # centos 6
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
cat >> "/etc/sysctl.conf" <<EOF
# centminmod added
kernel.pid_max=65536
kernel.printk=4 1 1 7
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=65536
net.core.somaxconn=65535
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=30
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=65536
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 0
net.ipv4.tcp_tw_reuse = 0
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.nf_conntrack_max = 524288
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024
EOF
sysctl -p
        fi
    fi # centos 6 or 7
fi
}

if [ ! -d "$DIR_TMP" ]; then
  mkdir -p $DIR_TMP
fi

DEF=${1:-novalue}

yum clean all
opt_tcp

if [[ ! -f /usr/bin/git || ! -f /usr/bin/bc || ! -f /usr/bin/wget || ! -f /bin/nano || ! -f /usr/bin/unzip || ! -f /usr/bin/applydeltarpm ]]; then
  firstyuminstallstarttime=$(TZ=UTC date +%s.%N)
  echo
  echo "installing yum packages..."
  echo

  # do not install yum fastmirror plugin if not enough detected system memory available
  # for yum fastmirror operation
  if [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '1018000' && "$CENTOS_SEVEN" = '7' ]]; then
    time $YUMDNFBIN -y install yum-plugin-fastestmirror yum-plugin-security
    sar_call
  elif [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '263000' && "$CENTOS_SIX" = '6' ]]; then
    time $YUMDNFBIN -y install yum-plugin-fastestmirror yum-plugin-security
    sar_call
  fi

  if [[ -f /etc/machine-info && "$(grep -qi 'OVH bhs' /etc/machine-info; echo $?)" -eq '0' ]]; then
    # detected OVH BHS based server so disable slower babylon network mirror
    # https://community.centminmod.com/posts/47320/
    if [[ "$CENTOS_SEVEN" = '7' && -f /etc/yum/pluginconf.d/fastestmirror.conf ]]; then
      echo "exclude=ca.mirror.babylon.network" >> /etc/yum/pluginconf.d/fastestmirror.conf
      cat /etc/yum/pluginconf.d/fastestmirror.conf
    fi
    # if [[ -f /etc/dnf/dnf.conf && "$(grep -qw 'exclude' /etc/dnf/dnf.conf; echo $?)" -eq '0' ]]; then
    #   echo "exclude=ca.mirror.babylon.network" >> /etc/dnf/dnf.conf
    # fi
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/x86_64/7/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000115046005249|' /var/cache/yum/x86_64/7/timedhosts.txt
        fi
      fi
    else
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000115046005249|' /var/cache/yum/timedhosts.txt
        fi
      else
        if [ -f /var/cache/yum/i386/6/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000110046005249|' /var/cache/yum/i386/6/timedhosts.txt
        fi
      fi
    fi
  fi

  if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
    if [[ $(rpm -q nmap-ncat >/dev/null 2>&1; echo $?) != '0' ]]; then
      time $YUMDNFBIN -y install nmap-ncat${DISABLEREPO_DNF}
      sar_call
    fi
  else
    if [[ $(rpm -q nc >/dev/null 2>&1; echo $?) != '0' ]]; then
      time $YUMDNFBIN -y install nc libgcj
      sar_call
    fi
  fi

  # ensure ipset doesn't get caught in autoremove list
  # https://community.centminmod.com/posts/48144/
  KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')
  if [ -f /proc/user_beancounters ]; then
    USER_PKGS=""
  elif [[ -f /proc/user_beancounters && "$KERNEL_NUMERICVER" -ge '3000000000' ]]; then
    USER_PKGS=" ipset ipset-devel"  
  else
    USER_PKGS=" ipset ipset-devel"
  fi

if [[ "$CENTOS_NINE" -eq '9' ]]; then
  time $YUMDNFBIN -y install perl-FindBin perl-diagnostics libc-client libc-client-devel systemd-devel systemd-libs open-sans-fonts libidn2-devel libpsl-devel gpgme-devel gnutls-devel virt-what acl libacl-devel attr libattr-devel lz4-devel gawk unzip libuuid-devel sqlite-devel bc wget lynx screen ca-certificates yum-utils bash mlocate subversion rsyslog dos2unix boost-program-options net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables-nft iptables-nft-services iptables-libs iptables-utils rrdtool diffutils which perl-Math-BigInt perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop libvpx libvpx-devel t1lib t1lib-devel expect readline readline-devel libedit libedit-devel libxslt libxslt-devel openssl openssl-devel curl curl-devel openldap openldap-devel zlib zlib-devel gd gd-devel pcre pcre-devel gettext gettext-devel libidn libidn-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel e2fsprogs e2fsprogs-devel libc-client libc-client-devel cyrus-sasl cyrus-sasl-devel pam pam-devel libaio libaio-devel libevent libevent-devel recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel enchant enchant-devel lua lua-devel s-nail perl-LWP-Protocol-https OpenEXR-devel OpenEXR-libs atk cups-libs fftw-libs-double fribidi gdk-pixbuf2 ghostscript-devel gl-manpages graphviz gtk2 hicolor-icon-theme ilmbase ilmbase-devel jasper-devel jasper-libs jbigkit-devel jbigkit-libs lcms2 lcms2-devel libICE-devel libSM-devel libXaw libXcomposite libXcursor libXdamage-devel libXfixes-devel libXi libXinerama libXmu libXrandr libXt-devel libXxf86vm-devel libdrm-devel libfontenc librsvg2 libtiff libtiff-devel libwebp libwebp-devel libwmf-lite mesa-libGL-devel mesa-libGLU mesa-libGLU-devel poppler-data urw-fonts xorg-x11-font-utils${USER_PKGS}${DISABLEREPO_DNF} --skip-broken
elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  time $YUMDNFBIN -y install perl-FindBin libc-client libc-client-devel systemd-devel systemd-libs open-sans-fonts libidn2-devel libpsl-devel gpgme-devel gnutls-devel virt-what acl libacl-devel attr libattr-devel lz4-devel gawk unzip libuuid-devel sqlite-devel bc wget lynx screen ca-certificates yum-utils bash mlocate subversion rsyslog dos2unix boost-program-options net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Math-BigInt perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop libvpx libvpx-devel t1lib t1lib-devel expect readline readline-devel libedit libedit-devel libxslt libxslt-devel openssl openssl-devel curl curl-devel openldap openldap-devel zlib zlib-devel gd gd-devel pcre pcre-devel gettext gettext-devel libidn libidn-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel e2fsprogs e2fsprogs-devel libc-client libc-client-devel cyrus-sasl cyrus-sasl-devel pam pam-devel libaio libaio-devel libevent libevent-devel recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel enchant enchant-devel lua lua-devel mailx perl-LWP-Protocol-https OpenEXR-devel OpenEXR-libs atk cups-libs fftw-libs-double fribidi gdk-pixbuf2 ghostscript-devel gl-manpages graphviz gtk2 hicolor-icon-theme ilmbase ilmbase-devel jasper-devel jasper-libs jbigkit-devel jbigkit-libs lcms2 lcms2-devel libICE-devel libSM-devel libXaw libXcomposite libXcursor libXdamage-devel libXfixes-devel libXi libXinerama libXmu libXrandr libXt-devel libXxf86vm-devel libdrm-devel libfontenc librsvg2 libtiff libtiff-devel libwebp libwebp-devel libwmf-lite mesa-libGL-devel mesa-libGLU mesa-libGLU-devel poppler-data urw-fonts xorg-x11-font-utils${USER_PKGS}${DISABLEREPO_DNF} --skip-broken
else
  time $YUMDNFBIN -y install systemd-libs open-sans-fonts virt-what acl libacl-devel attr libattr-devel lz4-devel python-devel gawk unzip pyOpenSSL python-dateutil libuuid-devel sqlite-devel bc wget lynx screen deltarpm ca-certificates yum-utils bash mlocate subversion rsyslog dos2unix boost-program-options net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop libvpx libvpx-devel t1lib t1lib-devel expect expect-devel readline readline-devel libedit libedit-devel libxslt libxslt-devel openssl openssl-devel curl curl-devel openldap openldap-devel zlib zlib-devel gd gd-devel pcre pcre-devel gettext gettext-devel libidn libidn-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel e2fsprogs e2fsprogs-devel libc-client libc-client-devel cyrus-sasl cyrus-sasl-devel pam pam-devel libaio libaio-devel libevent libevent-devel recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel enchant enchant-devel lua lua-devel mailx perl-LWP-Protocol-https OpenEXR-devel OpenEXR-libs atk cups-libs fftw-libs-double fribidi gdk-pixbuf2 ghostscript-devel ghostscript-fonts gl-manpages graphviz gtk2 hicolor-icon-theme ilmbase ilmbase-devel jasper-devel jasper-libs jbigkit-devel jbigkit-libs lcms2 lcms2-devel libICE-devel libSM-devel libXaw libXcomposite libXcursor libXdamage-devel libXfixes-devel libXfont libXi libXinerama libXmu libXrandr libXt-devel libXxf86vm-devel libdrm-devel libfontenc librsvg2 libtiff libtiff-devel libwebp libwebp-devel libwmf-lite mesa-libGL-devel mesa-libGLU mesa-libGLU-devel poppler-data urw-fonts xorg-x11-font-utils${USER_PKGS}${DISABLEREPO_DNF}
fi
  sar_call
  # allows curl install to skip checking for already installed yum packages 
  # later on in initial curl installations
  touch /tmp/curlinstaller-yum
  time $YUMDNFBIN -y install epel-release${DISABLEREPO_DNF}
  # $YUMDNFBIN makecache fast
  sar_call
  if [[ "$CENTOS_NINE" = '9' ]]; then
    time $YUMDNFBIN -y install checksec systemd-libs xxhash-devel libzstd xxhash libzstd-devel datamash qrencode jq clang clang-devel jemalloc jemalloc-devel zstd python2-pip libmcrypt libmcrypt-devel libraqm oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 bash-completion mlocate re2c kernel-headers kernel-devel${DISABLEREPO_DNF} --enablerepo=epel,epel-testing,remi --skip-broken --allowerasing
    libc_fix
    if [ -f /usr/bin/pip ]; then
      PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install --upgrade pip
    fi
    sar_call
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    time $YUMDNFBIN -y install checksec systemd-libs xxhash-devel libzstd xxhash libzstd-devel datamash qrencode jq clang clang-devel jemalloc jemalloc-devel zstd python2-pip libmcrypt libmcrypt-devel libraqm oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 bash-completion mlocate re2c kernel-headers kernel-devel${DISABLEREPO_DNF} --enablerepo=epel,epel-testing,remi --skip-broken --allowerasing
    libc_fix
    if [ -f /usr/bin/pip ]; then
      PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install --upgrade pip
    fi
    sar_call
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    time $YUMDNFBIN -y install checksec systemd-libs xxhash-devel libzstd xxhash libzstd-devel datamash qrencode jq clang clang-devel jemalloc jemalloc-devel zstd python2-pip libmcrypt libmcrypt-devel libraqm oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 bash-completion bash-completion-extras mlocate re2c kernel-headers kernel-devel${DISABLEREPO_DNF} --enablerepo=epel
    libc_fix
    if [ -f /usr/bin/pip ]; then
      PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install --upgrade pip==20.3.4
    fi
    sar_call
  else
    time $YUMDNFBIN -y install datamash qrencode jq clang clang-devel jemalloc jemalloc-devel zstd python-pip libmcrypt libmcrypt-devel libraqm oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 libJudy bash-completion bash-completion-extras mlocate re2c kernel-headers kernel-devel cmake28 uw-imap-devel${DISABLEREPO_DNF} --enablerepo=epel
    if [ -f /usr/bin/pip ]; then
      PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install --upgrade pip
    fi
    sar_call
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    time $YUMDNFBIN -y install GeoIP GeoIP-devel --disablerepo=rpmforge
    sar_call
  else
    time $YUMDNFBIN -y install GeoIP GeoIP-devel
    sar_call
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    time $YUMDNFBIN -y install centos-release-cr
    sar_call
    sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/CentOS-CR.repo
    # echo "priority=1" >> /etc/yum.repos.d/CentOS-CR.repo
  fi
  touch ${INSTALLDIR}/curlinstall_yum.txt
  firstyuminstallendtime=$(TZ=UTC date +%s.%N)
fi

if [ -f /etc/selinux/config ]; then
  sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
  sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
fi

yumupdater() {
  yum clean all
  time $YUMDNFBIN -y update
  #time $YUMDNFBIN -y install expect imake bind-utils readline readline-devel libedit libedit-devel libxslt libxslt-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip patch sysstat zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel glib2 glib2-devel bzip2 bzip2-devel vim-minimal nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-ExtUtils-MakeMaker perl-Time-HiRes cyrus-sasl cyrus-sasl-devel strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel git
}

install_axel() {
  cd $DIR_TMP
  echo "Download $AXEL_LINKFILE ..."
  if [ -s $AXEL_LINKFILE ]; then
    echo "Axel ${AXEL_VER} Archive found, skipping download..." 
  else
    wget -O $AXEL_LINKFILE $AXEL_LINK
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
     echo "Error: $AXEL_LINKFILE download failed."
      exit #$ERROR
    else 
      echo "Download $AXEL_LINKFILE done."
    fi
  fi
  if [[ "$(tar -tzf axel-${AXEL_VER}.tar.gz >/dev/null; echo $?)" != '0' ]]; then
    rm -rf /svr-setup/axel-${AXEL_VER}.*
    echo "re-try download form local mirror..."
    wget -O $AXEL_LINKFILE $AXEL_LINKLOCAL
  fi
  tar xzf $AXEL_LINKFILE
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    echo "Error: $AXEL_LINKFILE extraction failed."
    exit #$ERROR
  else 
    echo "$AXEL_LINKFILE valid file."
    echo ""
  fi

  cd axel-${AXEL_VER}
  if [ -f autogen.sh ]; then
    if [ ! -f /usr/bin/autoreconf ]; then
      yum -y -q install autoconf
    fi
  ./autogen.sh
  fi
  ./configure
  make
  make install
  which axel
}

cminstall() {

    if command -v figlet >/dev/null 2>&1; then
        figlet -ckf standard "Centmin Mod Install"
    fi

cd $INSTALLDIR
  if [[ "$GITINSTALLED" = [yY] ]]; then
    if [[ ! -f "${INSTALLDIR}/centminmod" ]]; then
      getcmstarttime=$(TZ=UTC date +%s.%N)
      echo "git clone Centmin Mod repo..."
      time git clone -b ${branchname} --depth=5 ${CMGIT} centminmod
      getcmendtime=$(TZ=UTC date +%s.%N)
      sar_call
      if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
        git config --global pull.rebase false
      fi
      cd centminmod
      chmod +x centmin.sh
    fi
  else
    if [[ ! -f "${DOWNLOAD}" ]]; then
    getcmstarttime=$(TZ=UTC date +%s.%N)
    echo "downloading Centmin Mod..."
    if [[ -f /usr/local/bin/axel && $AXEL = [yY] ]]; then
      /usr/bin/axel https://github.com/centminmod/centminmod/archive/${DOWNLOAD}
    else
      wget --no-check-certificate https://github.com/centminmod/centminmod/archive/${DOWNLOAD} --tries=3
    fi
    getcmendtime=$(TZ=UTC date +%s.%N)
    rm -rf centminmod-*
    unzip ${DOWNLOAD}
    fi
    #export CUR_DIR
    #export CM_INSTALLDIR
    mv centminmod-${branchname} centminmod
    cd centminmod
    chmod +x centmin.sh
  fi
  GETCMTIME=$(echo "$getcmendtime - $getcmstarttime" | bc)
  echo "$GETCMTIME" > "/root/centminlogs/getcmtime_installtime_${DT}.log"
  GETCMTIME=$(printf "%0.4f\n" $GETCMTIME)
  echo "$GETCMTIME" >> "/root/centminlogs/getcmtime_installtime_${DT}.log"

# disable nginx lua and luajit by uncommenting these 2 lines
#sed -i "s|LUAJIT_GITINSTALL='y'|LUAJIT_GITINSTALL='n'|" centmin.sh
#sed -i "s|ORESTY_LUANGINX='y'|ORESTY_LUANGINX='n'|" centmin.sh

# disable nginx pagespeed module by uncommenting this line
#sed -i "s|NGINX_PAGESPEED=y|NGINX_PAGESPEED=n|" centmin.sh

# disable nginx geoip module by uncommenting this line
#sed -i "s|NGINX_GEOIP=y|NGINX_GEOIP=n|" centmin.sh

# disable nginx vhost traffic stats module by uncommenting this line
#sed -i "s|NGINX_VHOSTSTATS=y|NGINX_VHOSTSTATS=n|" centmin.sh

# disable nginx webdav modules by uncommenting this line
#sed -i "s|NGINX_WEBDAV=y|NGINX_WEBDAV=n|" centmin.sh

# disable openresty additional nginx modules by uncommenting this line
#sed -i "s|NGINX_OPENRESTY='y'|NGINX_OPENRESTY='n'|" centmin.sh

# switch back to OpenSSL instead of LibreSSL for Nginx
#sed -i "s|LIBRESSL_SWITCH='y'|LIBRESSL_SWITCH='n'|" centmin.sh

# siwtch back to Libmemcached source compile instead of YUM repo install
#sed -i "s|LIBMEMCACHED_YUM='y'|LIBMEMCACHED_YUM='n'|" centmin.sh

# disable PHP redis extension
#sed -i "s|PHPREDIS='y'|PHPREDIS='n'|" centmin.sh

# switch from PHP 5.4.41 to 5.6.9 default with Zend Opcache
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  PHPVERLATEST=$(curl -${ipv_forceopt}sL https://www.php.net/downloads.php?source=Y| grep -E -o "php-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "php-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | uniq | grep '7.4' | head -n1)
elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  PHPVERLATEST=$(curl -${ipv_forceopt}sL https://www.php.net/downloads.php?source=Y| grep -E -o "php-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "php-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | uniq | grep '7.4' | head -n1)
else
  PHPVERLATEST=$(curl -${ipv_forceopt}sL https://www.php.net/downloads.php?source=Y| grep -E -o "php-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "php-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | uniq | grep '7.4' | head -n1)
fi
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  PHPVERLATEST=${PHPVERLATEST:-"7.4.33"}
elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  PHPVERLATEST=${PHPVERLATEST:-"7.2.34"}
else
  PHPVERLATEST=${PHPVERLATEST:-"5.5.38"}
fi
sed -i "s|^PHP_VERSION='.*'|PHP_VERSION='$PHPVERLATEST'|" centmin.sh
sed -i "s|ZOPCACHEDFT='n'|ZOPCACHEDFT='y'|" centmin.sh

# disable axivo yum repo
#sed -i "s|AXIVOREPO_DISABLE=n|AXIVOREPO_DISABLE=y|" centmin.sh

# bypass initial setup email prompt
mkdir -p /etc/centminmod/
if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
  echo "LOWMEM_INSTALL='y'" >> /etc/centminmod/custom_config.inc
fi
echo "1" > /etc/centminmod/email-primary.ini
echo "2" > /etc/centminmod/email-secondary.ini
echo "${INSTALLDIR}/centminmod"
cd "${INSTALLDIR}/centminmod"
sed -i 's|TESTEDCENTOSVER='9.3'|TESTEDCENTOSVER='9.3'|' centmin.sh
./centmin.sh install
sar_call
echo "./centmin.sh install completion"
rm -rf /etc/centminmod/email-primary.ini
rm -rf /etc/centminmod/email-secondary.ini

    # setup command shortcut aliases 
    # given the known download location
    # updated method for cmdir and centmin shorcuts
    echo
    echo "/root/.bashrc modifications"
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    if [ -f /usr/bin/cmdir ]; then
      rm -rf /usr/bin/cmdir
    fi
    echo "alias command setup"
    alias cmdir="pushd /usr/local/src/centminmod"
    echo "alias cmdir='pushd /usr/local/src/centminmod'" >> /root/.bashrc
    echo -e "pushd /usr/local/src/centminmod; bash centmin.sh" > /usr/bin/centmin
    if [[ "$(id -u)" -ne '0' ]]; then
      sed -i '/cmdir=/d' $HOME/.bashrc
      sed -i '/centmin=/d' $HOME/.bashrc
      if [ -f /usr/bin/cmdir ]; then
        rm -rf /usr/bin/cmdir
      fi
      alias cmdir="pushd /usr/local/src/centminmod"
      echo "alias cmdir='pushd /usr/local/src/centminmod'" >> $HOME/.bashrc
      echo -e "pushd /usr/local/src/centminmod; bash centmin.sh" > /usr/bin/centmin
    fi
    chmod 0700 /usr/bin/centmin
  echo
  echo "Created command shortcuts:"
  echo "* type cmdir to change to Centmin Mod install directory"
  echo "  at /usr/local/src/centminmod"
  echo "* type centmin call and run centmin.sh"
  echo "  at /usr/local/src/centminmod/centmin.sh"
}

if [[ "$DEF" = 'novalue' ]]; then
  {
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    if [[ "$CHECK_LXD" = [yY] || ! -f /usr/bin/gcc ]]; then
      # lxd containers have minimal default yum packages installed
      $YUMDNFBIN -y install yum-utils cmake which e2fsprogs e2fsprogs-devel bc libuuid libuuid-devel openssl openssl-devel zlib zlib-devel gd gd-devel net-tools bzip2-devel gmp-devel libXext-devel libidn-devel libtool-ltdl-devel openldap-devel bluez-libs-devel gcc gcc-c++ automake libtool make
      $YUMDNFBIN -y install libcurl libcurl-devel
      yum -y reinstall bzip2 bzip2-devel
      yum -y groupinstall "Development tools"
      if [ -f /etc/yum.repos.d/jsynacek-systemd-centos-7.repo ]; then
        SYSTEMD_FACEBOOKRPM='y'
      fi
    fi
    double_check_wget_bc
    source_pcreinstall
    source_wgetinstall
  fi
  install_axel
  fileperm_fixes
  cminstall
} 2>&1 | tee "/root/centminlogs/installer_cmm_${DT}.log"
  echo
  FIRSTYUMINSTALLTIME=$(echo "$firstyuminstallendtime - $firstyuminstallstarttime" | bc)
  FIRSTYUMINSTALLTIME=$(printf "%0.4f\n" $FIRSTYUMINSTALLTIME)

  #touch ${CENTMINLOGDIR}/firstyum_installtime_${DT}.log
  echo "" > "/root/centminlogs/firstyum_installtime_${DT}.log"
  {
echo "---------------------------------------------------------------------------"
  echo "Total Curl Installer YUM or DNF Time: $FIRSTYUMINSTALLTIME seconds" >> "/root/centminlogs/firstyum_installtime_${DT}.log"
  tail -1 /root/centminlogs/firstyum_installtime_*.log
  tail -1 /root/centminlogs/centminmod_yumtimes_*.log
  DTIME=$(tail -1 /root/centminlogs/centminmod_downloadtimes_*.log)
  DTIME_SEC=$(echo "$DTIME" |awk '{print $7}')
  NTIME=$(tail -1 /root/centminlogs/centminmod_ngxinstalltime_*.log)
  NTIME_SEC=$(echo "$NTIME" |awk '{print $7}')
  CM_INSTALL_TIME_LOG=$(find /root/centminlogs/ -type f -name "*_install.log" | grep -v pcre)
  if [ -f /root/centminlogs/centminmod_phpinstalltime_*.log ]; then
    PTIME=$(tail -1 /root/centminlogs/centminmod_phpinstalltime_*.log)
    PTIME_SEC=$(echo "$PTIME" |awk '{print $7}')
  else
    PTIME_SEC='0'
  fi
  CMTIME=$(tail -1 ${CM_INSTALL_TIME_LOG})
  CMTIME_SEC=$(echo "$CMTIME" |awk '{print $6}')
  CMTIME_SEC=$(printf "%0.4f\n" $CMTIME_SEC)
if [[ "$DNF_ENABLE" = [yY] ]]; then
  CURLT=$(awk '{print $8}' /root/centminlogs/firstyum_installtime_*.log | tail -1)
else
  CURLT=$(awk '{print $8}' /root/centminlogs/firstyum_installtime_*.log | tail -1)
fi
  FPM_CHECK_PGO=$(/usr/local/bin/php -v | grep -o PGO | head -n1)
  if [[ "$FPM_CHECK_PGO" = 'PGO' ]]; then
    DESC_PGO='PGO'
  else
    DESC_PGO=''
  fi
  CT=$(awk '{print $6}' ${CM_INSTALL_TIME_LOG} | tail -1)
  GETCMTIME=$(tail -1 /root/centminlogs/getcmtime_installtime_${DT}.log)
  TT=$(echo "$CURLT + $CT + $GETCMTIME" | bc)
  TT=$(printf "%0.4f\n" $TT)
  ST=$(echo "$CT - ($DTIME_SEC + $NTIME_SEC + $PTIME_SEC)" | bc)
  ST=$(printf "%0.4f\n" $ST)
  echo "Total YUM or DNF + Source Download Time: $(printf "%0.4f\n" $DTIME_SEC)"
  echo "Total Nginx First Time Install Time: $(printf "%0.4f\n" $NTIME_SEC)"
  echo "Total PHP First Time Install Time: $(printf "%0.4f\n" $PTIME_SEC)"
  echo "Download From Github Time: $GETCMTIME"
  echo "Total Time Other eg. source compiles: $ST"
  echo "Total Centmin Mod Install Time: $CMTIME_SEC"
echo "---------------------------------------------------------------------------"
  echo "Total Install Time for curl yum + cm install + zip download: ${TT} seconds"    
echo "---------------------------------------------------------------------------"
  echo "$OS_PRETTY_NAME $(uname -r)"
  echo "$CPUMODEL"; echo "$CPUSPEED"
  echo "PHP VERSION: $(php-config --version) $DESC_PGO"
  if [[ "$CENTOS_NINE" -eq '9' ]]; then
    echo "EL9 OS minimum supported PHP version is 7.4"
  elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
    echo "EL9 OS minimum supported PHP version is 7.2"
  fi
echo "---------------------------------------------------------------------------"
  echo "Centmin Mod Version: $(cat /etc/centminmod-release)"
  echo "Install Summary Logs: /root/centminlogs/installer_summary_links.log"
echo "---------------------------------------------------------------------------"
} 2>&1 | tee "/root/centminlogs/install_time_stats_${DT}.log"
  cat "/root/centminlogs/install_time_stats_${DT}.log" >> "installer_${DT}.log"
  grep -E -v '\*{6}|shell-init:|csf: |Flushing chain  CC |and iptables DROP|The set with the given name does not exist|csf: IPSET adding|\.{9}|DOPENSSL_PIC|/opt/openssl/share/|fpm-build/libtool|checking for |checking whether |make -f |make\[1\]|make\[2\]|make\[3\]|make\[4\]|make\[5\]|--noexecstack -O3 -m64 -march=native -Wimplicit-fallthrough=0 |install ./include/openssl' "installer_${DT}.log" > "/root/centminlogs/installer_${DT}_minimal.log"
  cp -a "installer_${DT}.log" "/root/centminlogs/installer_${DT}.log"
  echo "Full initial install log: /root/centminlogs/installer_${DT}.log" > /root/centminlogs/installer_summary_links.log
  echo "Minimal initial install log: /root/centminlogs/installer_${DT}_minimal.log" >> /root/centminlogs/installer_summary_links.log
  echo "Initial install time stats: /root/centminlogs/install_time_stats_${DT}.log" >> /root/centminlogs/installer_summary_links.log
  echo "Initial install nginx configure options: /root/centminlogs/$(ls -t /root/centminlogs/ | grep 'nginx-configure-' | tail -1)" >> /root/centminlogs/installer_summary_links.log
  systemstats
  echo -e "Initial install sar stats: \n$(ls -t /root/centminlogs/ | awk '/^sar-/ {print "/root/centminlogs/"$0}')" >> /root/centminlogs/installer_summary_links.log
fi

if [ -f "${INSTALLDIR}/curlinstall_yum.txt" ]; then
  rm -rf "${INSTALLDIR}/curlinstall_yum.txt"
fi

case "$1" in
  install)
    # devtoolset SCL repo only supports 64bit OSes
    if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
      source_pcreinstall
      source_wgetinstall
    fi
    install_axel
    fileperm_fixes
    cminstall
    ;;
  yumupdate)
    yumupdater
    install_axel
    cminstall
    ;;
  *)
    if [[ "$DEF" = 'novalue' ]]; then
      echo
    else
      echo "./$0 {install|yumupdate}"
    fi
    ;;
esac