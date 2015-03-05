selinxcheck() {
	# check if selinux is disabled, if not abort and prompt to reboot

	if [ ! -f /usr/sbin/sestatus ]; then
		yum -q -y install policycoreutils
	fi

	if [ ! -f /usr/sbin/setenforce ]; then
		yum -q -y install libselinux-utils
	fi	

	if [[ -z $(sestatus | grep disabled) && -f /etc/selinux/config ]]; then
		echo "---------------------------------------------------------------"
		echo "SELINUX linux detected"
		echo "---------------------------------------------------------------"
		echo "disabling SELINUX..."
		echo "will require a server reboot before running initial install"
		sleep 3
		sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
		sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
		echo
		echo "SELINUX disabled, please reboot server and rerun install"
		echo "on reboot you can find the centmin.sh file located at:"
		echo "echo $(pwd)"
		echo "---------------------------------------------------------------"
		exit 0
	fi
}

centaltoff() {
	if [[ -f /etc/yum.repos.d/centalt.repo ]]; then
		sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/centalt.repo
		yum clean all -q
	fi
}

csfipsetcheck() {
	if [[ ! -f /proc/user_beancounters && "$(lsmod | grep ip_set)" ]]; then
		if [[ ! -f /usr/sbin/ipset || ! -f /usr/include/libipset/data.h ]]; then
			yum -y -q install ipset ipset-devel
			if [ -f /etc/csf/csf.conf ]; then
			sed -i 's/LF_IPSET = "0"/LF_IPSET = "1"/g' /etc/csf/csf.conf
			fi
		fi
	else
		if [ -f /etc/csf/csf.conf ]; then
		sed -i 's/LF_IPSET = \"1\"/LF_IPSET = \"0\"/' /etc/csf/csf.conf
		fi
	fi
}

cpcheck() {

if [ -f /var/cpanel/cpanel.config ]; then
echo "WHM/Cpanel detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/psa/.psa.shadow ]; then
echo "Plesk detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/init.d/directadmin ]; then
echo "DirectAdmin detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

selinxcheck
centaltoff
csfipsetcheck

}

setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
wget -q --no-check-certificate -O /usr/bin/cminfo https://gist.githubusercontent.com/centminmod/828a8ae1add0397e740b/raw/cminfo.sh
chmod 0700 /usr/bin/cminfo
EOF

chmod 0700 /usr/bin/cminfo_updater

# insert itself into cronjob for auto updates
if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    echo "*/4 * * * * /usr/bin/cminfo_updater" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi
}