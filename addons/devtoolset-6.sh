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
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi
########################################################################
install() {
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    echo "installing devtoolset-6 from softwarecollections.org"
    if [[ "$CENTOS_SIX" = '6' && ! -f /etc/yum.repos.d/CentOS-SCLo-scl.repo ]]; then
      cd $DIR_TMP
      yum clean all
      yum -y -q install centos-release-scl --disablerepo=rpmforge
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
      yum -y -q install centos-release-scl --disablerepo=rpmforge
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
      yum -y -q install centos-release-scl --disablerepo=rpmforge
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
      yum -y -q install centos-release-scl --disablerepo=rpmforge
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
      yum -y -q install centos-release-scl --disablerepo=rpmforge
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