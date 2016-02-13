checkmotd() {
			if [[ -z "$(grep 'PrintMotd no' /etc/ssh/sshd_config)" ]]; then
				echo "PrintMotd no" >> /etc/ssh/sshd_config
			fi
			if [[ -z "$(grep '\/usr\/local\/bin\/dmotd' /etc/profile)" ]]; then
				echo "if [ -f /usr/local/bin/dmotd ]; then 	/usr/local/bin/dmotd; fi" >> /etc/profile
			fi
}

setupmotd() {

	ENABLEMOTD='y'

	if [[ "$ENABLEMOTD" = [yY] ]]; then
		if [ -f /usr/local/bin/dmotd ]; then
			rm -rf /usr/local/bin/dmotd
			if [ -f "${SCRIPT_DIR}/config/motd/dmotd.sh" ]; then
				\cp -f "${SCRIPT_DIR}/config/motd/dmotd.sh" /usr/local/bin/dmotd
				if [ -f /usr/local/bin/dmotd ]; then
					chmod +x /usr/local/bin/dmotd
				fi
				checkmotd
			fi
		else
			if [ -f "${SCRIPT_DIR}/config/motd/dmotd.sh" ]; then
				\cp -f "${SCRIPT_DIR}/config/motd/dmotd.sh" /usr/local/bin/dmotd
				if [ -f /usr/local/bin/dmotd ]; then
					chmod +x /usr/local/bin/dmotd
				fi
				checkmotd
			fi
		fi
	fi #ENABLEMOTD

}