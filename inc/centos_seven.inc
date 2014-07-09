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
        echo "service ${servicename} $action"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                service ${servicename} $action
        fi
        else
        echo "systemctl $action ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl $action ${servicename}.service
        fi
        fi
}

cmchkconfig() {
        servicename=$1
        status=$2
        if [[ "$CENTOS_SEVEN" != '7' ]]; then
        echo "chkconfig ${servicename} $status"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                chkconfig ${servicename} $status
        fi
        else
                if [ "$status" = 'on' ]; then
                        status=enable
                fi
                if [ "$status" = 'off' ]; then
                        status=disable
                fi
        echo "systemctl $status ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl $status ${servicename}.service
        fi
        fi
}

}