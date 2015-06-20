customrpms() {
	if [[ -f "${DIR_TMP}/rpms/re2c-0.14.3-1.el7.x86_64.rpm" && "$CENTOS_SEVEN" ='7' ]]; then
		rpm -Uvh ${DIR_TMP}/rpms/re2c-0.14.3-1.el7.x86_64.rpm
	fi
}