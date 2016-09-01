fixphpfpm_includes() {
  # in line with https://www.nginx.com/resources/wiki/start/topics/examples/phpfcgi/
  if [ -d /usr/local/nginx/conf ]; then
  {
  DT=$(date +"%d%m%y-%H%M%S")
  for pifinc in $(find /usr/local/nginx/conf -type f -name "*.conf" | grep php); do
    if [[ "$(grep 'shave 200+ ms' $pifinc)" && "$(grep 'try_files' $pifinc)" ]]; then
      echo "updating php-fpm config files syntax"
      echo $pifinc
      sed -i 's|location ~ \\.php$ {|location ~ [^\/]\\.php(\/\|$) {|' $pifinc
      sed -i 's|fastcgi_split_path_info ^(.+\\.php)(/.+)$;|fastcgi_split_path_info ^(.+\?\\.php)(/.*)$;|' $pifinc
      sed -i 's|    *fastcgi_param  SCRIPT_FILENAME    $request_filename;|    #fastcgi_param  SCRIPT_FILENAME    $request_filename;|' $pifinc
      sed -i 's|    *#fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;|    fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;|' $pifinc
      sed -i 's|try_files $uri =404;|if (!-f $document_root$fastcgi_script_name) { return 404; }|' $pifinc
      egrep 'location|fastcgi_split_path_info|fastcgi_param  SCRIPT_FILENAME|404' $pifinc
      echo "y" > /tmp/phpinc-restart-check
    fi
  done
    if [ -f /tmp/phpinc-restart-check ]; then
      rm -rf /tmp/phpinc-restart-check
      /etc/init.d/nginx restart
      /etc/init.d/php-fpm restart
    fi
  } 2>&1 | tee "/root/centminlogs/fixphpfpm_includes_${DT}.log"
  fi
}

fixphpfpm_httpproxy() {
  # CVE-2016-5385
  # https://www.nginx.com/blog/mitigating-the-httpoxy-vulnerability-with-nginx/
  if [ -d /usr/local/nginx/conf ]; then
  {
  DT=$(date +"%d%m%y-%H%M%S")
  for pif in $(find /usr/local/nginx/conf -type f -name "*.conf" -o -name "fastcgi_params" | egrep 'php|fastcgi_params'); do
    if [[ "$(grep 'fastcgi_param  HTTPS' $pif)" && ! "$(grep 'HTTP_PROXY' $pif)" ]]; then
      echo "updating php-fpm config files to block Proxy header / HTTP_PROXY"
      echo $pif
      #sed -i '/HTTP_PROXY/d' $pif
      #sed -e 's|fastcgi_param  HTTPS              $https if_not_empty;|fastcgi_param  HTTPS              $https if_not_empty;\nfastcgi_param  HTTP_PROXY         "";|' $pif | grep HTTP_PROXY
      sed -i 's|fastcgi_param  HTTPS              $https if_not_empty;|fastcgi_param  HTTPS              $https if_not_empty;\nfastcgi_param  HTTP_PROXY         "";|' $pif
      grep 'HTTP_PROXY' $pif
      echo "y" > /tmp/phprestart-check
    fi
  done
    if [ -f /tmp/phprestart-check ]; then
      rm -rf /tmp/phprestart-check
      /etc/init.d/nginx restart
      /etc/init.d/php-fpm restart
    fi
  } 2>&1 | tee "/root/centminlogs/fixphpfpm_httpproxy_${DT}.log"
  fi
}

wgetver_check() {
  if [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    if [[ -f "${SCRIPT_DIR}/addons/wget.sh" && -f /usr/local/sbin/nginx ]]; then
      echo "update wget to ${WGET_VERSION} version... one time task"
      sleep 6
      "${SCRIPT_DIR}/addons/wget.sh" install
    fi
  elif [[ -f "${SCRIPT_DIR}/addons/wget.sh" && -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No UTF-8 support' >/dev/null 2>&1; echo $?)" = '0' ]]; then
    echo "fix pcre install for missing UTF8 support... one time task"
    sleep 6
    "${SCRIPT_DIR}/addons/wget.sh" pcre
  fi
  if [[ "$(uname -m)" != 'x86_64' ]]; then
    if [ -f /root/.wgetrc ]; then
      \cp -fp /root/.wgetrc /root/.wgetrc-bak
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
    else
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
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

blockeditorcheck() {
	if [ -f /usr/local/nginx/conf/drop.conf ]; then
	if [[ -z "$(grep 17234 /usr/local/nginx/conf/drop.conf)" ]]; then
		if [ -f "${SCRIPT_DIR}/config/nginx/drop.conf" ]; then
		rm -rf /usr/local/nginx/conf/drop.conf
		\cp -f ${SCRIPT_DIR}/config/nginx/drop.conf /usr/local/nginx/conf/drop.conf
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
		yum -y -q remove axivo-release
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
pushd $SCRIPT_DIR; bash centmin.sh
EOF
		chmod 0700 /usr/bin/centmin
	fi
}

nvcheck() {
	if [[ -f "${SCRIPT_DIR}/tools/nv.sh" && -f /usr/bin/nv ]]; then
		# auto update /usr/bin/nv if different from centmin mod tools/nv.sh copy
		# if [[ "$(diff -qr ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv; echo $?)" != '0' ]]; then
		\cp -f ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv
		chmod +x /usr/bin/nv
		# fi
	else
		\cp -f ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv
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
	if [ -z "$(grep '\$' /etc/logrotate.d/nginx)" ]; then
		# sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/usr\/local\/nginx\/logs\/nginx.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/nginx
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        size=500M
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

	if [ -f /etc/logrotate.d/php-fpm ]; then
	if [ -z "$(grep '\$' /etc/logrotate.d/php-fpm)" ]; then
		# sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/var\/run\/php-fpm\/php-fpm.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/php-fpm
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        size=500M
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
}

pureftpdupdates() {
  if [ -f /etc/pure-ftpd/pure-ftpd.conf ]; then
    # disable anonymous logins by default
    CHECKPFTPD_ANON=$(grep 'NoAnonymous                 no' /etc/pure-ftpd/pure-ftpd.conf)
    if [ "$CHECKPFTPD_ANON" ]; then
      sed -i 's|NoAnonymous                 no|NoAnonymous                 yes|' /etc/pure-ftpd/pure-ftpd.conf
      if [[ "$(ps aufx | grep pure-ftpd | grep -v grep)" ]]; then
        service pure-ftpd restart
      fi
      echo
      echo "disable pure-ftpd annonymous logins"
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
      if [[ -z "$(grep 'modern' /etc/init.d/memcached)" ]]; then
        if [[ -f "${SCRIPT_DIR}/config/memcached/memcached1426.patch" && -f /etc/init.d/memcached ]]; then
          \cp -f "${SCRIPT_DIR}/config/memcached/memcached1426.patch" /etc/init.d/memcached1426.patch
          pushd /etc/init.d/
          patch -s < memcached1426.patch >/dev/null 2>&1
          rm -rf memcached1426.patch
          popd
        fi
      fi
    fi
  fi
}

imagick_fixes() {
	if [[ -f /etc/ImageMagick/policy.xml || -f /etc/ImageMagick-last/ImageMagick-6/policy.xml ]]; then
		if [ -f "${SCRIPT_DIR}/tools/imagemagick-fix.sh" ]; then
			"${SCRIPT_DIR}/tools/imagemagick-fix.sh" >/dev/null 2>&1
		fi
	fi
}

tcpcheck_centos() {
  # lnstat -c -1 -f nf_conntrack
  TCPMEMTOTAL=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  # mem usage ~ NF_CTMAX x 288 bytes
  # mem usage ~ NF_CTHASHSIZE x 8 bytes
  if [ "$TCPMEMTOTAL" -le '385000' ]; then
    NF_CTMAX='16384'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '385000' && "$TCPMEMTOTAL" -le '770000' ]]; then
    NF_CTMAX='65536'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '770000' && "$TCPMEMTOTAL" -le '1049000' ]]; then
    NF_CTMAX='131072'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '1049000' && "$TCPMEMTOTAL" -le '2098000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '2098000' && "$TCPMEMTOTAL" -le '3147000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '3147000' && "$TCPMEMTOTAL" -le '4196000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '4196000' && "$TCPMEMTOTAL" -le '8392000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '8392000' && "$TCPMEMTOTAL" -le '16784000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '16784000' && "$TCPMEMTOTAL" -le '33568000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '33568000' && "$TCPMEMTOTAL" -le '67136000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '67136000' && "$TCPMEMTOTAL" -le '134272000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '134272000' && "$TCPMEMTOTAL" -le '268544000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [ "$TCPMEMTOTAL" -gt '268544000' ]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  fi
if [[ ! -f /proc/user_beancounters ]]; then
    # create /etc/sysctl.d/101-sysctl.conf if doesn't exist on centos 7
    if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/sysctl.d/101-sysctl.conf ]]; then
        if [ -d /etc/sysctl.d ]; then
          if [ -f /sys/module/nf_conntrack/parameters/hashsize ]; then
            # raise hashsize for conntrack entries
            echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
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
net.netfilter.nf_conntrack_max = $NF_CTMAX
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
EOF
        /sbin/sysctl --system >/dev/null 2>&1
            fi           
        fi
    else
        # centos 6 check for missing new tcp settings backported to existing installs
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" = '0' ]]; then
            if [ -f /sys/module/nf_conntrack/parameters/hashsize ]; then
              # raise hashsize for conntrack entries
              echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
              if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
                echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
              fi
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
if [[ ! -f /proc/user_beancounters ]]; then
  # fix for https://community.centminmod.com/posts/34527/
  # CVE-2016-5696
  if [[ "$CENTOS_SEVEN" = '7' && -f /etc/sysctl.d/101-sysctl.conf ]]; then
    if [[ -z "$(grep 'tcp_challenge_ack_limit' /etc/sysctl.d/101-sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_challenge_ack_limit = 999999999" >> /etc/sysctl.d/101-sysctl.conf
      /sbin/sysctl --system >/dev/null 2>&1
    fi
  fi
  if [[ "$CENTOS_SEVEN" = '6' && -f /etc/sysctl.conf ]]; then
    if [[ -z "$(grep 'tcp_challenge_ack_limit' /etc/sysctl.conf)" ]]; then
      echo "net.ipv4.tcp_challenge_ack_limit = 999999999" >> /etc/sysctl.conf
      sysctl -p >/dev/null 2>&1
    fi
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

if [[ "$INITIALINSTALL" != [yY] ]]; then
  selinxcheck
  tcpcheck_centos
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
  axivo_remove
  imagick_fixes
  wgetver_check
  fixphpfpm_includes
  fixphpfpm_httpproxy
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