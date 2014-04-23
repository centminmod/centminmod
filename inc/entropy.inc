entropyup() {
if [[ ! -f /proc/user_beancounters ]]; then
	if [[ ! -f /etc/rc.d/init.d/haveged ]]; then
        echo "*************************************************"
        cecho "* Increase system Entropy pool availability" $boldgreen
        echo "*************************************************"
		# cat /proc/sys/kernel/random/entropy_avail
		yum${CACHESKIP} -y -q install haveged rng-tools
		sed -i 's/-w 1024/-w 3072/' /etc/rc.d/init.d/haveged
	
		chkconfig haveged on
		service haveged start
	
		# cat /dev/random | rngtest -c 1000
		# sleep 10
		# cat /proc/sys/kernel/random/entropy_avail
	
	fi
fi
}