#!/bin/bash
######################################################
# written by George Liu (eva2000) centminmod.com
# custom curl yum repo addon installer
# use at own risk as it can break the system
# info at http://nervion.us.es/city-fan/yum-repo/
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

# custom curl/libcurl RPM for 7.47 and higher
# enable with CUSTOM_CURLRPM=y
# use at own risk as it can break the system
# info at http://nervion.us.es/city-fan/yum-repo/
CUSTOM_CURLRPM=y

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

if [ ! -d "$DIR_TMP" ]; then
	mkdir -p "$DIR_TMP"
  chmod 0750 "$DIR_TMP"
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
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
	if [ -f "/usr/local/src/centminmod/downloads/curlrpms.zip" ]; then
    /usr/bin/unzip -qo "/usr/local/src/centminmod/downloads/curlrpms.zip" -d "$DIR_TMP"/
	fi
	###############################################################
	if [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then
	#############################
	# el6 32bit
	curl -sI --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/i386/city-fan.org-release-1-13.rhel6.noarch.rpm
	CURL_NOARCHRPMCHECK=$?
	if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
		rpm -Uvh http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/i386/city-fan.org-release-1-13.rhel6.noarch.rpm
	else
		if [ -f "$DIR_TMP/city-fan.org-release-1-13.rhel6.noarch.rpm" ]; then
			rpm -Uvh "$DIR_TMP/city-fan.org-release-1-13.rhel6.noarch.rpm"
		fi
	fi
	sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
	if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
		cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
		if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
    	#echo priorities already set for city-fan.org.repo
			PRIOREXISTS=1
		else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99|' /etc/yum.repos.d/city-fan.org.repo
		fi
	fi # repo file check
	yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
	echo
	curl -V
	echo
	cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
	cecho "complete new curl version setup on your system" $boldyellow
	###############################################################
	elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
	###############################################################
	# el6 64bit
	curl -sI --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/x86_64/city-fan.org-release-1-13.rhel6.noarch.rpm
	CURL_NOARCHRPMCHECK=$?
	if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
		rpm -Uvh http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/x86_64/city-fan.org-release-1-13.rhel6.noarch.rpm
	else
		if [ -f "$DIR_TMP/city-fan.org-release-1-13.rhel6.noarch.rpm" ]; then
			rpm -Uvh "$DIR_TMP/city-fan.org-release-1-13.rhel6.noarch.rpm"
		fi
	fi
	
	sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
	if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
		cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
		if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
    	#echo priorities already set for city-fan.org.repo
			PRIOREXISTS=1
  	else
      echo "setting yum priorities for city-fan.org.repo"
			sed -i 's|^gpgkey=.*|&\npriority=99|' /etc/yum.repos.d/city-fan.org.repo
		fi
	fi # repo file check
	yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
	echo
	curl -V
	echo
	cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
	cecho "complete new curl version setup on your system" $boldyellow
	###############################################################
	elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" = 'x86_64' ]]; then
	###############################################################
	# el7 64bit
	curl -sI --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel7/x86_64/city-fan.org-release-1-13.rhel7.noarch.rpm
	CURL_NOARCHRPMCHECK=$?
	if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
		rpm -Uvh http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel7/x86_64/city-fan.org-release-1-13.rhel7.noarch.rpm
	else
		if [ -f "$DIR_TMP/city-fan.org-release-1-13.rhel7.noarch.rpm" ]; then
			rpm -Uvh "$DIR_TMP/city-fan.org-release-1-13.rhel7.noarch.rpm"
		fi
	fi
	
	sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
	if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
		cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
		if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
    	#echo priorities already set for city-fan.org.repo
			PRIOREXISTS=1
  	else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99|' /etc/yum.repos.d/city-fan.org.repo
		fi
	fi # repo file check
	yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
	echo
	curl -V
	echo
	cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
	cecho "complete new curl version setup on your system" $boldyellow
	fi
	###############################################################
fi # CUSTOM_CURLRPM=y
}
##############################################################
starttime=$(date +%s.%N)
{
curlrpm

echo
cecho "custom curl RPMs installed..." $boldyellow
cecho "you can now use yum update to update curl" $boldyellow
echo
echo " yum update --enablerepo=city-fan.org --disableplugin=priorities"
echo
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"
echo "Total Custom Curl RPMs Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"