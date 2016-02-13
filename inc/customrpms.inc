rerpm() {
	if [[ -f "${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm" && "$CENTOS_SEVEN" = '7' ]]; then
		echo
		echo "rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm"
		rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el7.x86_64.rpm
	elif [[ -f "${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm" && "$CENTOS_SIX" = '6' ]]; then
		# re2c 0.13.5 too old http://re2c.org/ on CentOS 6
		echo
		echo "rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm"
		rpm -Uvh ${DIR_TMP}/rpms/re2c/re2c-0.14.3-1.el6.x86_64.rpm
	fi	
}

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
	rpm -Uvh libcurl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh libcurl-devel-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
	rpm -Uvh --nodeps curl-${CUSTOM_CURLRPMVER}.cf.rhel7.x86_64.rpm
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

customrpms() {
	echo
	echo "custom RPM installs"
	rerpm
	curlrpm
}