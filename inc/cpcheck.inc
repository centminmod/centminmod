set_logdate() {
	DT=$(date +"%d%m%y-%H%M%S")
}

ngxver_checker() {
	if [[ "$(which nginx >/dev/null 2>&1; echo $?)" = '0' ]]; then
		LASTEST_NGINXVERS=$(curl -sL https://github.com/nginx/nginx/tags 2>&1 | awk -F '[<|>]' '/tag-name/ { print $3}' | sed 's|release-||' | head -n1 2>&1)
		CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk -F '/' '{print $2}')
		if [[ "$CURRENT_NGINXVERS" != "$LASTEST_NGINXVERS" ]]; then
			echo
			echo "###############################################################"
			echo "* Current Nginx Version: $CURRENT_NGINXVERS"
			echo "* Latest Nginx Available: $LASTEST_NGINXVERS"
			echo "###############################################################"
			echo
		fi
	fi
}

yumupdatechecker() {
	if [[ "$INITIALINSTALL" != [yY] ]]; then
		# Setup Colours
		black='\E[30;40m'
		red='\E[31;40m'
		green='\E[32;40m'
		yellow='\E[33;40m'
		blue='\E[34;40m'
		magenta='\E[35;40m'
		cyan='\E[36;40m'
		white='\E[37;40m'
		
		boldblack='\E[1;30;40m'
		boldred='\E[1;31;40m'
		boldgreen='\E[1;32;40m'
		boldyellow='\E[1;33;40m'
		boldblue='\E[1;34;40m'
		boldmagenta='\E[1;35;40m'
		boldcyan='\E[1;36;40m'
		boldwhite='\E[1;37;40m'
		
		Reset="tput sgr0"      #  Reset text attributes to normal
                       		#+ without clearing screen.
		
		cecho ()                     # Coloured-echo.
                             		# Argument $1 = message
                             		# Argument $2 = color
		{
		message=$1
		color=$2
		echo -e "$color$message" ; $Reset
		return
		}

		if [ -f /etc/yum.repos.d/remi.repo ]; then
			YENABLEREPOS='remi'
		else
			YENABLEREPOS=""
		fi
		YEXCLUDE='nginx* php* mysql*'
		YYUM_PARAMETER='--disableplugin=priorities'
		if [ -f /etc/yum.repos.d/remi.repo ]; then
			UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --enablerepo=$YENABLEREPOS --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
		else
			UPDATE_CHECK=$(/usr/bin/yum $YYUM_PARAMETER -e 0 -d 0 check-update --exclude="$YEXCLUDE" 1> /dev/null 2>&1; echo $?)
		fi
		if [[ "$UPDATE_CHECK" = '100' ]]; then
			echo
			cecho "-------------------------------------------------------------" $boldgreen
			cecho "  New YUM Updates available for host $(hostname)" $boldyellow
			cecho "-------------------------------------------------------------" $boldgreen
			cecho "  To list available YUM Updates type: " $boldyellow
			cecho "-------------------------------------------------------------" $boldgreen
			if [ -f /etc/yum.repos.d/remi.repo ]; then
				echo "  yum list updates $YYUM_PARAMETER --enablerepo=$YENABLEREPOS"
			else
				echo "  yum list updates $YYUM_PARAMETER"
			fi
			# cecho "-------------------------------------------------------------" $boldgreen
			echo
			cecho "-------------------------------------------------------------" $boldgreen
			cecho "  To updates type: " $boldyellow
			cecho "-------------------------------------------------------------" $boldgreen
			if [ -f /etc/yum.repos.d/remi.repo ]; then
				echo "  yum update $YYUM_PARAMETER --enablerepo=$YENABLEREPOS"
			else
				echo "  yum update $YYUM_PARAMETER"
			fi
			echo
		fi
	fi
}

setupemailcheck() {
	if [[ "$INITIALINSTALL" != [yY] ]]; then
		if [ -f /etc/centminmod/email-primary.ini ]; then
			if [ ! -s /etc/centminmod/email-primary.ini ]; then
				PEMAIL_NOSET='y'
			fi
		else
			PEMAIL_NOSET='y'
		fi
		if [ -f /etc/centminmod/email-secondary.ini ]; then
			if [ ! -s /etc/centminmod/email-secondary.ini ]; then
				SEMAIL_NOSET='y'
			fi
		else
			SEMAIL_NOSET='y'
		fi
		if [[ "$PEMAIL_NOSET" = 'y' || "$SEMAIL_NOSET" = 'y' ]] && [[ "$INITIALINSTALL" != [yY] ]]; then
			if [ -f "${SCRIPT_DIR}/tools/email.sh" ]; then
				"${SCRIPT_DIR}/tools/email.sh"
			fi
		fi
	fi
}

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

reuseportchecks() {
	if [[ "$INITIALINSTALL" = [yY] ]]; then
		CHECKREUSEPORT=$(grep --color -Ro SO_REUSEPORT /usr/src/kernels/* | head -n1 | awk -F ":" '{print $2}')
		if [[ "$CHECKREUSEPORT" = 'SO_REUSEPORT' ]]; then
			SUPPORT_REUSEPORT=y
			if [ -f /usr/local/nginx/conf/nginx.conf ]; then
				sed -i 's|accept_mutex .*|accept_mutex off;|' /usr/local/nginx/conf/nginx.conf
			fi
		else
			SUPPORT_REUSEPORT=n
			if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
				sed -i 's| reuseport||' /usr/local/nginx/conf/conf.d/virtual.conf
			fi
		fi
	fi
}

rclocalchecks() {
	if [ ! -x /etc/rc.d/rc.local ]; then
		# centos 7 doesn't give rc.local executable permissions
		# so /etc/rc.local isn't run on reboots like on centos 6
		if [[ "$INITIALINSTALL" = [yY] ]]; then
			ls -lah /etc/rc.d/rc.local
		fi
		chmod +x /etc/rc.d/rc.local
		if [[ "$INITIALINSTALL" = [yY] ]]; then
			ls -lah /etc/rc.d/rc.local
		fi
	fi
}

kernelchecks() {
	if [[ "$(uname -r | cut -d . -f1)" = '4' ]]; then
		if [[ "$(cat /proc/sys/net/netfilter/nf_conntrack_helper)" != '0' ]]; then
			echo "linux 4.x kernel related adjustments"
			echo "cat /proc/sys/net/netfilter/nf_conntrack_helper"
			cat /proc/sys/net/netfilter/nf_conntrack_helper
			echo 0 > /proc/sys/net/netfilter/nf_conntrack_helper
			if [[ -z "$(grep 'nf_conntrack_helper' /etc/sysctl.conf)" ]]; then
				echo "net.netfilter.nf_conntrack_helper=0" >> /etc/sysctl.conf
				sysctl -p
			fi
			if [[ -z "$(grep 'nf_conntrack_helper' /etc/rc.local)" ]]; then
				echo "echo 0 > /proc/sys/net/netfilter/nf_conntrack_helper" >> /etc/rc.local
			fi		
			echo "cat /proc/sys/net/netfilter/nf_conntrack_helper"
			cat /proc/sys/net/netfilter/nf_conntrack_helper
		fi	
	fi
}

blockeditorcheck() {
	if [ -f /usr/local/nginx/conf/drop.conf ]; then
	if [[ -z "$(grep 17234 /usr/local/nginx/conf/drop.conf)" ]]; then
		if [ -f "${SCRIPT_DIR}/config/nginx/drop.conf" ]; then
		rm -rf /usr/local/nginx/conf/drop.conf
		\cp -f "${SCRIPT_DIR}/config/nginx/drop.conf" /usr/local/nginx/conf/drop.conf
		fi
	fi
	fi
}

centaltoff() {
	if [[ -f /etc/yum.repos.d/centalt.repo ]]; then
		sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/centalt.repo
		yum clean all -q
	fi
}

axivo_remove() {
	if [ -f /etc/yum.repos.d/axivo.repo ]; then
		# remove Axivo outdated yum repo
		# if detected as being installed
		# yum list installed --disablerepo=* --enablerepo=axivo --disableplugin=priorities | grep axivo
		yum -y -q remove axivo-release >/dev/null 2>&1
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

checkovzkernels() {
	if [[ -f /proc/user_beancounters ]]; then
		sed -i 's|installonly_limit=.*|installonly_limit=3|' /etc/yum.conf
	fi
}

checkaliases() {
	if [[ "$(grep 'centminmod-123.08centos7beta01' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta01/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta02' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta02/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta03/centminmod/' /root/.bashrc
	fi
	if [[ "$(grep 'centminmod-123.08beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08beta03/centminmod/' /root/.bashrc        
    fi
}

checkcmdircmd() {
	if [[ "$(grep 'cmdir=' /root/.bashrc)" || "$(grep 'centmin=' /root/.bashrc)" ]]; then
		sed -i '/cmdir=/d' /root/.bashrc
		sed -i '/centmin=/d' /root/.bashrc
		rm -rf /usr/bin/cmdir
		alias cmdir="pushd ${SCRIPT_DIR}"
		echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd "$SCRIPT_DIR"; bash centmin.sh
EOF
		chmod 0700 /usr/bin/centmin
	fi
}

nvcheck() {
	if [[ -f "${SCRIPT_DIR}/tools/nv.sh" && -f /usr/bin/nv ]]; then
		# auto update /usr/bin/nv if different from centmin mod tools/nv.sh copy
		# if [[ "$(diff -qr ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv; echo $?)" != '0' ]]; then
		\cp -f "${SCRIPT_DIR}/tools/nv.sh" /usr/bin/nv
		chmod +x /usr/bin/nv
		# fi
	else
		\cp -f "${SCRIPT_DIR}/tools/nv.sh" /usr/bin/nv
		chmod +x /usr/bin/nv
	fi		
}

multiphpcheck() {
	if [[ -d /usr/local/nginx/conf ]] && [[ ! -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf || ! -f /usr/local/nginx/conf/php-pool5.conf ]]; then
		# check to see if multiple php-fpm pool files exist and copy over if they don't
		# exist on existing server
		if [[ -f "${SCRIPT_DIR}/config/nginx/php-pool5.conf" ]]; then
			\cp -f ${SCRIPT_DIR}/config/nginx/php-pool* /usr/local/nginx/conf/
		fi
		if [[ -f "${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool2.conf" && ! -f /usr/local/nginx/conf/phpfpmd/phpsocket1.conf ]]; then
			\cp -f ${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool* /usr/local/nginx/conf/phpfpmd/
			if [[ "$USEEDITOR" = 'vim' ]]; then
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
			else
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
			fi # USEEDITOR			
		fi
	fi

	if [[ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf && ! -f /usr/bin/fpmconf-2 ]]; then
		if [[ "$USEEDITOR" = 'vim' ]]; then
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
		else
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
		fi # USEEDITOR	
	fi	
}

mjemalloc() {
    if [[ "$MARIADB_JEMALLOC" = [yY] && ! "$(grep libjemalloc /usr/bin/mysqld_safe)" && -f /usr/lib64/libjemalloc.so.1 ]]; then
        # echo "switching MariaDB to jemalloc malloc method"
        sed -i 's|^mysqld_ld_preload=|mysqld_ld_preload=/usr/lib64/libjemalloc.so.1|' /usr/bin/mysqld_safe
        # service mysql restart
        # pmap `pidof mysqld` | grep jemalloc
    fi    
}

fixlogrotate() {
	if [ -f /etc/logrotate.d/nginx ]; then
	if [[ -z "$(grep '\$' /etc/logrotate.d/nginx)" || -z "$(grep 'maxsize' /etc/logrotate.d/nginx)" ]]; then
		# sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/usr\/local\/nginx\/logs\/nginx.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/nginx
	if [[ "$CENTOS_SEVEN" = '7' ]]; then
		VARDFSIZE=$(df --output=avail /var | tail -1)
	else
		VARDFSIZE=$(df /var | tail -1 | awk '{print $4}')
	fi
if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
else
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
fi
	fi
	fi

	if [ -f /etc/logrotate.d/php-fpm ]; then
	if [[ -z "$(grep '\$' /etc/logrotate.d/php-fpm)" || -z "$(grep 'maxsize' /etc/logrotate.d/php-fpm)" ]]; then

	if [[ "$CENTOS_SEVEN" = '7' ]]; then
		VARDFSIZE=$(df --output=avail /var | tail -1)
	else
		VARDFSIZE=$(df /var | tail -1 | awk '{print $4}')
	fi

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
else
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
fi
	fi
	fi
}

pureftpdupdates() {
	# auto updates
	# raise max number of concurrent ftp connections from 50/2 to 20010/2
	if [ -f /etc/pure-ftpd/pure-ftpd.conf ]; then
		CHECKPFTPD_PASSIVEPORTS=$(grep '^PassivePortRange    3000 3050' /etc/pure-ftpd/pure-ftpd.conf)
		if [ "$CHECKPFTPD_PASSIVEPORTS" ]; then
			sed -i 's/^PassivePortRange    3000 3050/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
			sed -i 's|MaxClientsNumber            50|MaxClientsNumber            500|' /etc/pure-ftpd/pure-ftpd.conf
			sed -i 's|MaxClientsPerIP             8|MaxClientsPerIP             200|' /etc/pure-ftpd/pure-ftpd.conf
			if [[ "$(ps aufx | grep pure-ftpd | grep -v grep)" ]]; then
				service pure-ftpd restart
			fi
			echo
			echo "increasing pure-ftpd passive port range"
			echo
		fi
		CHECKCSF_PASSIVEPORTS=$(grep '3000:3050' /etc/csf/csf.conf)
		if [ "$CHECKCSF_PASSIVEPORTS" ]; then
			sed -i 's/3000:3050/30001:50011/' /etc/csf/csf.conf
			csf -r
			echo
			echo "adjusting pure-ftpd passive port range for CSF Firewall"
			echo
		fi	
	fi
}

leupdates() {
	# update .well-known content-type
	if [ -f /usr/local/nginx/conf/staticfiles.conf ]; then
	CHECKCONTENTTYPE=$(grep 'application\/jose+json' /usr/local/nginx/conf/staticfiles.conf)
	if [ "$CHECKCONTENTTYPE" ]; then
		sed -i "s|application\/jose+json|text\/plain|" /usr/local/nginx/conf/staticfiles.conf
	fi
	fi
}

memcachedupdatechecks() {
	# 1.4.25 added options https://community.centminmod.com/threads/memcached-1-4-25-released.5007/
	if [[ "$INITIALINSTALL" != [yY] ]]; then
		if [ -f /etc/init.d/memcached ]; then
		if [[ -z "$(grep 'MEM_CURVER' /etc/init.d/memcached)" ]]; then
			if [[ -f "${SCRIPT_DIR}/config/memcached/memcached1425.patch" && -f /etc/init.d/memcached ]]; then
				\cp -f "${SCRIPT_DIR}/config/memcached/memcached1425.patch" /etc/init.d/memcached1425.patch
				pushd /etc/init.d/
				patch -s < memcached1425.patch >/dev/null 2>&1
				rm -rf memcached1425.patch
				popd
			fi
		fi
		fi
	fi
}

cronpathadjust() {
	# if [[ "$INITIALINSTALL" = [yY] ]]; then
		# append /usr/local/bin to /etc/crontab PATH variable for /usr/local/bin/php to work
		if [ -f /etc/crontab ]; then
			if [[ -z "$(grep '\/usr\/local\/bin' /etc/crontab)" ]]; then
				# echo "adjust /etc/crontab \$PATH for /usr/local/bin support"
				sed -i 's|PATH=\/sbin:\/bin:\/usr\/sbin:\/usr\/bin|PATH=\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/usr\/local\/bin|' /etc/crontab
				cmservice crond restart >/dev/null 2>&1
			fi
		fi
	# fi
}

xpowerby() {
	# add x-powered-by header check
	if [ -f /usr/local/nginx/conf/nginx.conf ]; then
	XPB_CHECKA=$(grep 'more_set_headers \"Server: nginx\";' /usr/local/nginx/conf/nginx.conf)
	XPB_CHECKB=$(grep 'more_set_headers \"X-Powered-By: centminmod\";' /usr/local/nginx/conf/nginx.conf)
	XPB_CHECKC=$(grep 'more_set_headers \"Server: nginx centminmod\";' /usr/local/nginx/conf/nginx.conf)
	if [[ "$XPB_CHECKA" ]]; then
		sed -i "s|^more_set_headers \"Server: nginx\";|more_set_headers \"Server: nginx\";\nmore_set_headers \"X-Powered-By: centminmod\";|" /usr/local/nginx/conf/nginx.conf
	elif [[ -z "$XPB_CHECKB" ]]; then
		if [[ "$XPB_CHECKC" ]]; then
			sed -i "s|^more_set_headers \"Server: nginx centminmod\";|more_set_headers \"Server: nginx centminmod\";\nmore_set_headers \"X-Powered-By: centminmod\";|" /usr/local/nginx/conf/nginx.conf
		else
			sed -i "s|include \/usr\/local\/nginx\/conf\/vts_http.conf;|more_set_headers \"X-Powered-By: centminmod\";\n\ninclude \/usr\/local\/nginx\/conf\/vts_http.conf;|" /usr/local/nginx/conf/nginx.conf
		fi
	fi
	fi
}

pathfixes() {
	if [[ -z "$(grep '\$PATH:' /root/.bashrc)" ]]; then
		# sed -i "s|export PATH=\"\/usr\/lib64\/ccache|export PATH=\"\$PATH:\/usr\/lib64\/ccache|" /root/.bashrc
		sed -i 's|export PATH=.*|export PATH="\$PATH:\/usr\/lib64\/ccache:\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin"|' /root/.bashrc
	fi
	# for sudo
	if [ -d /etc/sudoers.d ]; then
		if [ ! -f /etc/sudoers.d/addpaths ]; then
			touch /etc/sudoers.d/addpaths
			if [[ "$(uname -m)" = 'x86_64' ]]; then
				if [[ "$(grep '\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin' /etc/sudoers.d/addpaths >/dev/null; echo $?)" != '0' ]]; then
					echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
				fi
			else
				if [[ "$(grep '\/usr\/local\/sbin:\/usr\/local\/bin:\/sbin:\/bin:\/usr\/sbin:\/usr\/bin:\/root\/bin' /etc/sudoers.d/addpaths >/dev/null; echo $?)" != '0' ]]; then
					echo "Defaults secure_path = /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin" > /etc/sudoers.d/addpaths
				fi
			fi
		fi
	fi
}

sshdfixes() {
	# forgot line break in sshd_config for centos 6.x systems
	# centos 6.x openssh 5.3p1 backported ecdsa keys support but
	# not configured out of the box https://community.centminmod.com/posts/19702/
	# this sets up ecdh-sha2-* key exchange support but setup without a line break
	# this fixes it
	if [[ "$(grep 'UseDNS noKexAlgorithms' /etc/ssh/sshd_config)" ]]; then
		sed -i 's|UseDNS noKexAlgorithms|UseDNS no\nKexAlgorithms|' /etc/ssh/sshd_config
	fi
}

auto_gitupdate() {
	# if centmin mod code install directory has been setup
	# with git environment via centmin.sh menu option 23
	# submenu option 1, then allow centmin.sh to auto update
	# the centmin mod code at /usr/local/src/centminmod
	# silently in background
	if [[ -d "${SCRIPT_DIR}/.git" && "$AUTO_GITUPDATE" = [yY] ]]; then
		cd ${SCRIPT_DIR}
		# totally silence the output
		git stash -q &> /dev/null
		git fetch -q &> /dev/null
		git pull -q &> /dev/null
	fi
}

figletcheck() {
    if [ ! -f "$(rpm -ql figlet | grep 'bin\/figlet')" ]; then
        yum -y -q install figlet
    fi
}

maintenance_confcheck() {
	if [ ! -f /usr/local/nginx/conf/sitestatus.conf ]; then
		if [[ -f "${SCRIPT_DIR}/config/nginx/sitestatus.conf" && -d /usr/local/nginx/conf ]]; then
			\cp -f "${SCRIPT_DIR}/config/nginx/sitestatus.conf" /usr/local/nginx/conf/sitestatus.conf
		fi
	fi
	if [ ! -f /usr/local/nginx/conf/maintenance.conf ]; then
		if [[ -f "${SCRIPT_DIR}/config/nginx/maintenance.conf" && -d /usr/local/nginx/conf ]]; then
			\cp -f "${SCRIPT_DIR}/config/nginx/maintenance.conf" /usr/local/nginx/conf/maintenance.conf
		fi
	fi
	if [ ! -f /usr/local/nginx/conf/503include-main.conf ]; then
		if [[ -f "${SCRIPT_DIR}/config/nginx/503include-main.conf" && -d /usr/local/nginx/conf ]]; then
			\cp -f "${SCRIPT_DIR}/config/nginx/503include-main.conf" /usr/local/nginx/conf/503include-main.conf
		fi
	fi
	if [ ! -f /usr/local/nginx/conf/503include-only.conf ]; then
		if [[ -f "${SCRIPT_DIR}/config/nginx/503include-only.conf" && -d /usr/local/nginx/conf ]]; then
			\cp -f "${SCRIPT_DIR}/config/nginx/503include-only.conf" /usr/local/nginx/conf/503include-only.conf
		fi
	fi
	if [ ! -f /usr/bin/sitestatus ]; then
		if [[ -f "${SCRIPT_DIR}/tools/sitestatus.sh" && -d /usr/local/nginx/conf ]]; then
			\cp -f "${SCRIPT_DIR}/tools/sitestatus.sh" /usr/bin/sitestatus
			chmod +x /usr/bin/sitestatus
     	MCONF_CHECK=$(grep 'maintenance.conf' /usr/local/nginx/conf/nginx.conf)
     	if [[ -z "$MCONF_CHECK" ]]; then
          	sed -i 's/include \/usr\/local\/nginx\/conf\/geoip.conf;/include \/usr\/local\/nginx\/conf\/geoip.conf;\ninclude \/usr\/local\/nginx\/conf\/maintenance.conf;\n/g' /usr/local/nginx/conf/nginx.conf
     	fi
		fi
	fi
}

ngxmodule_cleanups() {
	# clean ups
	if [ -f "$DIR_TMP/nginx-develkit_v0.2.19.tar.gz" ]; then
		rm -rf "$DIR_TMP/nginx-develkit_v0.2.19.tar.gz"
	fi
	if [ -d "$DIR_TMP/ngx_devel_kit-0.2.19" ]; then
		rm -rf "$DIR_TMP/ngx_devel_kit-0.2.19"
	fi
}

wpcompfix() {
	# for wordpress auto installs via centmin.sh menu option 22, the wpsecure include
	# file needs an added deny all location context to prevent compressed files in
	# wordpress plugins that save to wp-content subdirectory from public access
	if [ -d /usr/local/nginx/conf ]; then
		WPSECUREDIRS=$(find /usr/local/nginx/conf/ -type f -name "wpsecure_*.conf")
		if [[ "$WPSECUREDIRS" ]]; then
			for wsf in "$WPSECUREDIRS"; do
				if [[ -z "$(grep 'zip|gz|tar|bzip2|7z|txt' $wsf)" ]]; then
					echo "location ~* /(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z|txt)\$ { deny all; }" >> "$wsf";
				fi
			done
		fi
	fi
}

entropy_centosseven() {
if [[ -f /usr/lib/systemd/system/haveged.service ]] && [[ "$CENTOS_SEVEN" = '7' ]]; then
	if [[ ! -d /etc/systemd/system/haveged.service.d || ! -f /etc/systemd/system/haveged.service.d/haveged.conf ]]; then
		mkdir -p /etc/systemd/system/haveged.service.d
cat > "/etc/systemd/system/haveged.service.d/haveged.conf" <<EFF
[Service]
ExecStart=
ExecStart=/usr/sbin/haveged -w 4067 -v 1 --Foreground
EFF
		systemctl daemon-reload >/dev/null 2>&1
		systemctl restart haveged >/dev/null 2>&1
		# cat /etc/systemd/system/haveged.service.d/haveged.conf
	fi
fi
}

ovhkernelcheck() {
	if [[ "$INITIALINSTALL" = [yY] ]]; then
		# check for new installs only whether server is OVH based 
		# and whether a custom kernel is used with exception for
		# Intel Xeon D-15xx Broadwell-DE processors which need
		# OVH custom kernel for the newer Intel network NIC support
		# Intel Corporation Ethernet Connection X552/X557-AT 10GBASE-T
		OVHASNCHECK=$(curl -s${CURL_TIMEOUTS} ipinfo.io/org | grep -o 'OVH' >/dev/null 2>&1; echo $?)
		OVHKERNEL_CHECK=$(uname -r | grep '\-grs' >/dev/null 2>&1; echo $?)
		BROADWELLDE_CHECK=$(grep 'Xeon(R) CPU D-15' /proc/cpuinfo >/dev/null 2>&1; echo $?)
		# more sure way to check for OVH custom kernel if /root/.ovhrc file exists
		# can check value of variable DISTRIBKERNEL=1 for distro kernel or 
		# DISTRIBKERNEL=0 for custom OVH kernel and let it override ASN check too
		if [ -f /root/.ovhrc ]; then
			OVHRC_CHECK=$(awk -F "\"" '/DISTRIBKERNEL/ {print $2}' /root/.ovhrc)
			if [[ "$OVHRC_CHECK" != '1' ]]; then
				OVHASNCHECK='0'
				OVHKERNEL_CHECK='0'
			fi		
		fi
		if [[ "$OVHASNCHECK" = '0' && "$OVHKERNEL_CHECK" = '0' && "$BROADWELLDE_CHECK" != '0' ]]; then
			echo
			echo "Detected OVH based server with custom OVH Linux kernel"
			echo "It's best to use CentOS distro default Linux Kernels"
			echo "aborting script..."
			exit
		fi
	fi
}

tcpcheck_centos() {
if [[ ! -f /proc/user_beancounters ]]; then
		# create /etc/sysctl.d/101-sysctl.conf if doesn't exist on centos 7
    if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/sysctl.d/101-sysctl.conf ]]; then
        if [ -d /etc/sysctl.d ]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
            	echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
            # centos 7
            touch /etc/sysctl.d/101-sysctl.conf
            if [[ "$(grep 'centminmod added' /etc/sysctl.d/101-sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
cat >> "/etc/sysctl.d/101-sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8151
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_limit_output_bytes=65536
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.netfilter.nf_conntrack_max = 524288
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
EOF
        /sbin/sysctl --system >/dev/null 2>&1
            fi           
        fi
		else
        # centos 6 check for missing new tcp settings backported to existing installs
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" = '0' ]]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
            	echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
          	fi
            if [[ "$(grep 'net.ipv4.tcp_slow_start_after_idle=0' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then	
							echo "net.ipv4.tcp_slow_start_after_idle=0" >> /etc/sysctl.conf
						fi
            if [[ "$(grep 'net.ipv4.tcp_limit_output_bytes=65536' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then	
							echo "net.ipv4.tcp_limit_output_bytes=65536" >> /etc/sysctl.conf
						fi
            if [[ "$(grep 'net.ipv4.tcp_rfc1337=1' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then	
							echo "net.ipv4.tcp_rfc1337=1" >> /etc/sysctl.conf
						fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_helper=0' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then	
							echo "net.netfilter.nf_conntrack_helper=0" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_max = 524288' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
							echo "net.netfilter.nf_conntrack_max = 524288" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_tcp_timeout_established = 28800' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
							echo "net.netfilter.nf_conntrack_tcp_timeout_established = 28800" >> /etc/sysctl.conf
            fi
            if [[ "$(grep 'net.netfilter.nf_conntrack_generic_timeout = 60' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
							echo "net.netfilter.nf_conntrack_generic_timeout = 60" >> /etc/sysctl.conf
            fi
            sysctl -p >/dev/null 2>&1
        fi
		fi
fi
}

remi_tweaks() {
	if [ -f /etc/yum.repos.d/remi-safe.repo ]; then
		sed -i 's|enabled=1|enabled=0|' /etc/yum.repos.d/remi-safe.repo
	fi
}

imagick_fixes() {
	if [[ -f /etc/ImageMagick/policy.xml || -f /etc/ImageMagick-last/ImageMagick-6/policy.xml ]]; then
		if [ -f "${SCRIPT_DIR}/tools/imagemagick-fix.sh" ]; then
			"${SCRIPT_DIR}/tools/imagemagick-fix.sh" >/dev/null 2>&1
		fi
	fi
}

apache_protect() {
	if [[ -z "$(crontab -l 2>&1 | grep 'autoprotect.sh')" && -f "${SCRIPT_DIR}/tools/autoprotect.sh" ]]; then
    	crontab -l > cronjoblist
    	echo "11 */23 * * * ${SCRIPT_DIR}/tools/autoprotect.sh" >> cronjoblist
    	crontab cronjoblist
    	rm -rf cronjoblist
    	crontab -l
	fi
}

centossix_crsetup() {
	# centos <=6.7 openssl fix https://community.centminmod.com/posts/31166/
	if [[ ! -f /etc/yum.repos.d/CentOS-CR.repo && "$CENTOS_SIX" = '6' && "$(awk '{ print $3 }' /etc/redhat-release | sed -e 's|\.||')" -lt '68' ]]; then
		yum -y -q install centos-release-cr
		sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/CentOS-CR.repo
		echo "priority=1" >> /etc/yum.repos.d/CentOS-CR.repo
		yum -y -q update openssl openssl-devel --disableplugin=priorities --enablerepo=cr >/dev/null 2>&1
	fi
}

fileperm_fixes() {
	if [ -f /usr/lib/udev/rules.d/60-net.rules ]; then
		if [[ "$(lsattr /usr/lib/udev/rules.d/60-net.rules | cut -c5)" = 'i' ]]; then
			# fix for some centos 7 vps templates on vps hosts setting chattr +i on
			# /usr/lib/udev/rules.d/60-net.rules preventing yum updates for initscripts
			# yum packages
			chattr -i /usr/lib/udev/rules.d/60-net.rules
		fi
	fi
}

yumfastmirror_check() {
	# disable yum fastmirror plugin if not enough detected system memory available
	# for yum fastmirror operation
	if [ -f /etc/yum/pluginconf.d/fastestmirror.conf ]; then
		if [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -lt '1018000' && "$CENTOS_SEVEN" = '7' ]]; then
			sed -i 's|enabled = 1|enabled = 0|' /etc/yum/pluginconf.d/fastestmirror.conf
		elif [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -lt '263000' ]]; then
			sed -i 's|enabled = 1|enabled = 0|' /etc/yum/pluginconf.d/fastestmirror.conf
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

yumfastmirror_check
ovhkernelcheck
fileperm_fixes
cronpathadjust
rclocalchecks
checkovzkernels
kernelchecks
pathfixes
apache_protect

if [[ "$INITIALINSTALL" != [yY] ]]; then
	yumfastmirror_check
	fileperm_fixes
	selinxcheck
	setupemailcheck
	checkovzkernels
	rclocalchecks
	kernelchecks
	centaltoff
	csfipsetcheck
	checkaliases
	checkcmdircmd
	nvcheck
	multiphpcheck
	mjemalloc
	blockeditorcheck
	rpcnfsports
	fixlogrotate
	pureftpdupdates
	leupdates
	memcachedupdatechecks
	xpowerby
	pathfixes
	sshdfixes
	auto_gitupdate
	figletcheck
	maintenance_confcheck
	axivo_remove
	ngxmodule_cleanups
	wpcompfix
	entropy_centosseven
	tcpcheck_centos
	remi_tweaks
	imagick_fixes
	apache_protect
	centossix_crsetup
fi

}

histformat() {
	if [[ -z "$(grep HISTTIMEFORMAT /root/.bashrc)" ]]; then
		echo "HISTTIMEFORMAT=\"[%d.%m.%y] %T   \"" >> /root/.bashrc
		echo "export HISTSIZE=10000" >> /root/.bashrc
		echo "export HISTTIMEFORMAT" >> /root/.bashrc
	fi	
}

setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
wget -q --no-check-certificate -O /usr/bin/cminfo https://raw.githubusercontent.com/centminmod/centminmod/master/tools/cminfo.sh
chmod 0700 /usr/bin/cminfo
EOF

chmod 0700 /usr/bin/cminfo_updater

# insert itself into cronjob for auto updates
if [ ! -f /usr/bin/crontab ]; then
	yum -y -q install cronie
		if [[ "$CENTOS_SEVEN" != '7' ]]; then
			service crond restart
			chkconfig crond on
		else
			systemctl restart crond.service
			systemctl enable crond.service
		fi	
fi

if [ ! -f /usr/bin/sar ]; then
	yum -y -q install sysstat
		if [[ "$CENTOS_SEVEN" != '7' ]]; then
			service sysstat restart
			chkconfig sysstat on
		else
			systemctl restart sysstat.service
			systemctl enable sysstat.service
		fi		
fi

if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    echo "0 */4 * * * /usr/bin/cminfo_updater" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi
}