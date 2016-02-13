#!/bin/bash
######################################################
# written by George Liu (eva2000) vbtechsupport.com
# custom curl RPMs addon installer
# use at own risk as it can break the system
# info at http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring/
######################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

# custom curl/libcurl RPM for 7.44 and higher
# enable with CUSTOM_CURLRPM=y
# use at own risk as it can break the system
# info at http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring/
CUSTOM_CURLRPM=y
CUSTOM_CURLRPMVER='7.47.1-2.0'             # custom curl/libcurl version
CUSTOM_CURLLIBSSHVER='1.6.0-4.0'     # libssh2 version
CUSTOM_CURLRPMCARESVER='1.10.0-6.0'  # c-ares version
CUSTOM_CURLRPMSYSURL='http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring'
CUSTOM_CURLRPMLIBURL='http://mirror.city-fan.org/ftp/contrib/libraries'
###############################################################
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
# functions
#############

curlrpm() {
	if [[ "$CUSTOM_CURLRPM" = [yY] ]]; then
if [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then
	#############################
	# el6 32bit
	yum -y install libmetalink libssh2-devel nss-devel c-ares
	cd ${DIR_TMP}
	if [[ ! -f "curl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/curl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm
	fi	
	if [[ ! -f "c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.i686.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libcurl7155-7.15.5-17.cf.rhel6.i686.rpm" ]]; then
		wget ${CUSTOM_CURLRPMSYSURL}/libcurl7155-7.15.5-17.cf.rhel6.i686.rpm
	fi
	if [[ ! -f "libcurl7155-7.15.5-17.cf.rhel6.i686.rpm" ]]; then
		wget ${CUSTOM_CURLRPMSYSURL}/libcurl7112-7.11.2-25.cf.rhel6.i686.rpm
	fi

	# only process with custom curl rpm update if the rpm files exist
	if [[ -f "curl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" && -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" && -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm" && -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm" && -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm" && -f "c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.i686.rpm" && -f "libcurl7155-7.15.5-17.cf.rhel6.i686.rpm" && -f "libcurl7155-7.15.5-17.cf.rhel6.i686.rpm" ]]; then
		
	rpm --nodeps -e curl
	rpm --nodeps -e libcurl
	rpm --nodeps -e libcurl-devel
	rpm --nodeps -e libssh2
	rpm --nodeps -e libssh2-devel
	rpm --nodeps -e c-ares
	
	rpm -Uvh c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.i686.rpm
	rpm -Uvh libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm
	rpm -Uvh libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.i686.rpm
	rpm -Uvh libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	rpm -Uvh libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	rpm -Uvh curl-${CUSTOM_CURLRPMVER}.cf.rhel6.i686.rpm
	rpm -Uvh libcurl7155-7.15.5-17.cf.rhel6.i686.rpm
	rpm -Uvh libcurl7112-7.11.2-25.cf.rhel6.i686.rpm
	
	rpm -qa curl libcurl libcurl-devel libssh2 libssh2-devel libcurl7155 libcurl7112 c-ares
	else
		echo "Error: expected curl related named rpm files are not found"
		echo "could be their source names have changed etc..."
	fi

elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
	#############################
	# el6 64bit
	yum -y install libmetalink libssh2-devel nss-devel c-ares
	cd ${DIR_TMP}
	if [[ ! -f "c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMLIBURL}/libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm
	fi	
	if [[ ! -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "curl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/curl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm
	fi

	# only process with custom curl rpm update if the rpm files exist
	if [[ -f "c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.x86_64.rpm" && -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm" && -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm" && -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" && -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" && -f "curl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm" && -f "libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm" && -f "libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm" ]]; then

	rpm --nodeps -e curl
	rpm --nodeps -e libcurl
	rpm --nodeps -e libcurl-devel
	rpm --nodeps -e libssh2
	rpm --nodeps -e libssh2-devel
	rpm --nodeps -e c-ares
	
	rpm -Uvh c-ares-${CUSTOM_CURLRPMCARESVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh libcurl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh curl-${CUSTOM_CURLRPMVER}.cf.rhel6.x86_64.rpm
	rpm -Uvh libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm
	rpm -Uvh libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm
	
	rpm -qa curl libcurl libcurl-devel libssh2 libssh2-devel libcurl7155 libcurl7112 c-ares
	else
		echo "Error: expected curl related named rpm files are not found"
		echo "could be their source names have changed etc..."
	fi	

elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" = 'x86_64' ]]; then
	#############################
	# el7 64bit
	yum -y install libmetalink libssh2-devel nss-devel c-ares psl
	cd ${DIR_TMP}
	# if [[ ! -f "libmetalink-0.1.2-4.el7.x86_64.rpm" ]]; then
	# 	wget -cnv ftp://ftp.sunet.se/pub/Linux/distributions/fedora/epel/7/x86_64/libmetalink-0.1.2-4.el7.x86_64.rpm
	# fi
	if [[ ! -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/curl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	fi
	if [[ ! -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	fi
	if [[ ! -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	fi
	if [[ ! -f "libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm" ]]; then
		wget -cnv ${CUSTOM_CURLRPMSYSURL}/libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm
	fi
	if [[ ! -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm" ]]; then
		wget ${CUSTOM_CURLRPMLIBURL}/libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm
	fi	
	if [[ ! -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm" ]]; then
		wget ${CUSTOM_CURLRPMLIBURL}/libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm
	fi

	# only process with custom curl rpm update if the rpm files exist
	if [[ -f "libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" && -f "libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" && -f "curl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm" && -f "libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm" && -f "libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm" && -f "libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm" && -f "libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm" ]]; then

	rpm --nodeps -e curl
	rpm --nodeps -e libcurl
	rpm --nodeps -e libcurl-devel
	rpm --nodeps -e libssh2
	rpm --nodeps -e libssh2-devel
	
	# rpm -ivh libmetalink-0.1.2-4.el7.x86_64.rpm
	rpm -Uvh libssh2-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh libssh2-devel-${CUSTOM_CURLLIBSSHVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh --nodeps libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh curl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh libcurl7155-7.15.5-17.cf.rhel6.x86_64.rpm
	rpm -Uvh libcurl7112-7.11.2-25.cf.rhel6.x86_64.rpm
	
	rpm -qa curl libcurl libcurl-devel libssh2 libssh2-devel libcurl7155 libcurl7112 c-ares
	else
		echo "Error: expected curl related named rpm files are not found"
		echo "could be their source names have changed etc..."
	fi		
fi
	fi # CUSTOM_CURLRPM=y
}
##############################################################
starttime=$(date +%s.%N)
{
curlrpm

echo
cecho "custom curl RPMs installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log
echo "Total Custom Curl RPMs Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log

