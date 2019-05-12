rerpm() {
	if [[ -f "${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm" && "$CENTOS_SEVEN" = '7' ]]; then
		echo "skip re2c custom rpm for CentOS 7"
		# echo "rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm"
		# rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm
	elif [[ -f "${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm" && "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
		# re2c 0.13.5 too old http://re2c.org/ on CentOS 6
		echo
		echo "rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm"
		rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm
	fi	
}

curlrpm() {
if [[ "$CUSTOM_CURLRPM" = [yY] ]]; then
	if [ -f "/usr/local/src/centminmod/downloads/curlrpms.zip" ]; then
    /usr/bin/unzip -qo "/usr/local/src/centminmod/downloads/curlrpms.zip" -d "$DIR_TMP"/
	fi
	###############################################################
	if [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then
	#############################
	# el6 32bit
	curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/i386/city-fan.org-release-1-13.rhel6.noarch.rpm
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
	curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel6/x86_64/city-fan.org-release-1-13.rhel6.noarch.rpm
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
	curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 http://mirror.city-fan.org/ftp/contrib/yum-repo/rhel7/x86_64/city-fan.org-release-1-13.rhel7.noarch.rpm
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

customrpms() {
	echo
	echo "custom RPM installs"
	rerpm
	curlrpm
}