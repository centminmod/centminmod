# example usage
# https://community.centminmod.com/threads/work-log-for-dealing-with-centos-7-0-changes-systemd.738/

seven_function() {

CMSDEBUG=y

if [[ "$CMSDEBUG" = [yY] ]]; then
	if [[ "$CENTOS_SEVEN" != '7' ]]; then
        		echo "Detected 6.5"
	else
        		echo "Detected 7.0"
	fi
fi

cmservice() {
	servicename=$1
	action=$2
	if [[ "$CENTOS_SEVEN" != '7' ]]; then
        echo "service ${1} $2"
        if [[ "$CMSDEBUG" = [nN] ]]; then
        	service ${1} $2
        fi
	else
        echo "systemctl $2 ${1}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
        	systemctl $2 ${1}.service
        fi
	fi
}

}