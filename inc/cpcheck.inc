selinxcheck() {
	# check if selinux is disabled, if not abort and prompt to reboot
	if [[ -z $(sestatus | grep disabled) && -f /etc/selinux/config ]]; then
		echo "---------------------------------------------------------------"
		echo "SELINUX linux detected"
		echo "---------------------------------------------------------------"
		echo "disabling SELINUX..."
		echo "will require a server reboot before running initial install"
		sleep 3
		sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
		echo "SELINUX disabled, please reboot server and rerun install"
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

}