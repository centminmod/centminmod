#!/bin/bash
########################################################################
# install devtoolset-6 package for centos 6 or 7
# 64bit only
# https://www.softwarecollections.org/en/scls/rhscl/devtoolset-6/
# written by George Liu (eva2000) centminmod.com
# 
# requires at least an extra 1GB of disk free space for install
# install to obtain side install of gcc and g++ 6.2.x
########################################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
DIR_TMP='/svr-setup'

CENTMINLOGDIR='/root/centminlogs'
########################################################################
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
########################################################################
# functions
#############
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

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

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
  CENTOSVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
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
  ROCKYLINUXVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
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
  CENTOSVER=$(awk '{ print $4 }' /etc/el-release | cut -d . -f1,2)
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

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
  echo "$0 only for CentOS 7"
  echo "aborted..."
  exit 1
fi

########################################################################
install() {
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    echo "installing devtoolset-6 from softwarecollections.org"
    if [[ "$CENTOS_SIX" = '6' && ! -f /etc/yum.repos.d/CentOS-SCLo-scl.repo ]]; then
      cd $DIR_TMP
      yum clean all
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
      else
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
      fi
      echo
      /opt/rh/devtoolset-6/root/usr/bin/gcc --version
      /opt/rh/devtoolset-6/root/usr/bin/g++ --version
    elif [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo ]]; then
      if [ -f /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo ]; then
        yum -y remove rhscl-devtoolset-3
        rm -rf /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo
      fi
      yum clean all
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
      else
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
      fi
      echo
      /opt/rh/devtoolset-6/root/usr/bin/gcc --version
      /opt/rh/devtoolset-6/root/usr/bin/g++ --version
		elif [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/slc6-scl.repo ]]; then
      yum -y remove $(yum -q list installed --disableplugin=priorities | awk '/slc6-scl/ {print $1}')
      rm -rf /etc/yum.repos.d/slc6-scl.repo
      yum clean all
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
      else
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
      fi
      echo
      /opt/rh/devtoolset-6/root/usr/bin/gcc --version
      /opt/rh/devtoolset-6/root/usr/bin/g++ --version
    elif [[ "$CENTOS_SEVEN" = '7' && -f /etc/yum.repos.d/rhscl-devtoolset-3-el7-epel-7.repo ]]; then
      if [ -f /etc/yum.repos.d/rhscl-devtoolset-3-el7-epel-7.repo ]; then
        yum -y remove rhscl-devtoolset-3-el7
        rm -rf /etc/yum.repos.d/rhscl-devtoolset-3-el7-epel-7.repo
      fi
      yum clean all
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
      else
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
      fi
      echo
      /opt/rh/devtoolset-6/root/usr/bin/gcc --version
      /opt/rh/devtoolset-6/root/usr/bin/g++ --version
		elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      cd $DIR_TMP
      yum clean all
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
      else
        yum -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
      fi
      echo
      /opt/rh/devtoolset-6/root/usr/bin/gcc --version
      /opt/rh/devtoolset-6/root/usr/bin/g++ --version
		fi
  else
    echo "64bit install only, detected 32bit OS aborting..."
    exit
  fi
}

########################################################################
starttime=$(TZ=UTC date +%s.%N)
{
install

echo
cecho "devtoolset-6 installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_devtoolset-6_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_devtoolset-6_install_${DT}.log
echo "Total devtoolset-6 Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_devtoolset-6_install_${DT}.log