customrpms() {
	echo
	echo "custom RPM installs"
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