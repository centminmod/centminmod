#!/bin/bash
########################################################################
# install devtoolset-3 package for centos 6 or 7
# 64bit only
# https://www.softwarecollections.org/en/scls/rhscl/devtoolset-3/
# https://www.softwarecollections.org/en/scls/rhscl/rh-java-common/
# written by George Liu (eva2000) centminmod.com
# 
# requires at least an extra 1GB of disk free space for install
# install to obtain side install of gcc and g++ 4.9.x
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

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi
########################################################################
install() {
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    echo "installing devtoolset-3 from softwarecollections.org"
    if [[ "$CENTOS_SIX" = '6' && ! -f /etc/yum.repos.d/slc6-scl.repo ]]; then
      cd $DIR_TMP
      # wget https://www.softwarecollections.org/en/scls/rhscl/devtoolset-3/epel-6-x86_64/download/rhscl-devtoolset-3-epel-6-x86_64.noarch.rpm
      # wget https://www.softwarecollections.org/en/scls/rhscl/rh-java-common/epel-6-x86_64/download/rhscl-rh-java-common-epel-6-x86_64.noarch.rpm
      # rpm -ivh rhscl-devtoolset-3-epel-6-x86_64.noarch.rpm
      # rpm -ivh rhscl-rh-java-common-epel-6-x86_64.noarch.rpm
      yum clean all
      #yum install centos-release-scl-rh --disableplugin=fastmirror
      wget -O /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo https://copr.fedorainfracloud.org/coprs/rhscl/devtoolset-3/repo/epel-6/rhscl-devtoolset-3-epel-6.repo
      yum -y install devtoolset-3-gcc devtoolset-3-gcc-c++ devtoolset-3-binutils
      echo
      /opt/rh/devtoolset-3/root/usr/bin/gcc --version
      /opt/rh/devtoolset-3/root/usr/bin/g++ --version
		elif [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/slc6-scl.repo ]]; then
      yum -y remove $(yum -q list installed --disableplugin=priorities | awk '/slc6-scl/ {print $1}')
      rm -rf /etc/yum.repos.d/slc6-scl.repo
      yum clean all
      #yum install centos-release-scl-rh --disableplugin=fastmirror
      wget -O /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo https://copr.fedorainfracloud.org/coprs/rhscl/devtoolset-3/repo/epel-6/rhscl-devtoolset-3-epel-6.repo
      yum -y install devtoolset-3-gcc devtoolset-3-gcc-c++ devtoolset-3-binutils
      echo
      /opt/rh/devtoolset-3/root/usr/bin/gcc --version
      /opt/rh/devtoolset-3/root/usr/bin/g++ --version
		elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      cd $DIR_TMP
      # wget https://www.softwarecollections.org/en/scls/rhscl/devtoolset-3/epel-7-x86_64/download/rhscl-devtoolset-3-epel-7-x86_64.noarch.rpm
      # wget https://www.softwarecollections.org/en/scls/rhscl/rh-java-common/epel-7-x86_64/download/rhscl-rh-java-common-epel-7-x86_64.noarch.rpm
      # rpm -ivh rhscl-devtoolset-3-epel-7-x86_64.noarch.rpm
      # rpm -ivh rhscl-rh-java-common-epel-7-x86_64.noarch.rpm
      yum clean all
      #yum install centos-release-scl-rh --disableplugin=fastmirror
      wget -O /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo https://copr.fedorainfracloud.org/coprs/rhscl/devtoolset-3-el7/repo/epel-7/rhscl-devtoolset-3-el7-epel-7.repo
      yum -y install devtoolset-3-gcc devtoolset-3-gcc-c++ devtoolset-3-binutils
      echo
      /opt/rh/devtoolset-3/root/usr/bin/gcc --version
      /opt/rh/devtoolset-3/root/usr/bin/g++ --version
		fi
  else
    echo "64bit install only, detected 32bit OS aborting..."
    exit
  fi
}

########################################################################
starttime=$(date +%s.%N)
{
install

echo
cecho "devtoolset-3 installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_devtoolset-3_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_devtoolset-3_install_${DT}.log
echo "Total devtoolset-3 Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_devtoolset-3_install_${DT}.log