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
DT=`date +"%d%m%y-%H%M%S"`
DIR_TMP='/svr-setup'


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
########################################################################
install() {
	if [[ "$(uname -m)" = 'x86_64' ]]; then
		echo "installing devtoolset-3 from softwarecollections.org"
		if [[ "$CENTOS_SIX" = '6' ]]; then
			cd $DIR_TMP
			wget https://www.softwarecollections.org/en/scls/rhscl/devtoolset-3/epel-6-x86_64/download/rhscl-devtoolset-3-epel-6-x86_64.noarch.rpm
			wget https://www.softwarecollections.org/en/scls/rhscl/rh-java-common/epel-6-x86_64/download/rhscl-rh-java-common-epel-6-x86_64.noarch.rpm
			rpm -ivh rhscl-devtoolset-3-epel-6-x86_64.noarch.rpm
			rpm -ivh rhscl-rh-java-common-epel-6-x86_64.noarch.rpm
			yum -y install devtoolset-3
			echo
			/opt/rh/devtoolset-3/root/usr/bin/gcc --version
			/opt/rh/devtoolset-3/root/usr/bin/g++ --version
		else
			cd $DIR_TMP
			wget https://www.softwarecollections.org/en/scls/rhscl/devtoolset-3/epel-7-x86_64/download/rhscl-devtoolset-3-epel-7-x86_64.noarch.rpm
			wget https://www.softwarecollections.org/en/scls/rhscl/rh-java-common/epel-7-x86_64/download/rhscl-rh-java-common-epel-7-x86_64.noarch.rpm
			rpm -ivh rhscl-devtoolset-3-epel-7-x86_64.noarch.rpm
			rpm -ivh rhscl-rh-java-common-epel-7-x86_64.noarch.rpm
			yum -y install devtoolset-3
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