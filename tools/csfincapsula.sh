#!/bin/bash
###############################
# written by George Liu (eva2000)
# centminmod.com
###############################
CFIPLOG='/root/incapsula-ips.txt'
CFIPNGINXLOG='/root/incapsula-nginxlog.log'
CFIPCSFLOG='/root/incapsula_log.log'
CFINCLUDEFILE='/usr/local/nginx/conf/incapsula.conf'
CURL_TIMEOUTS='--max-time 20 --connect-timeout 20'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###############################
if [ ! -f /usr/bin/curl ]; then
	echo "Installing curl please wait..."
	yum -y -q install curl
fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi
###############################
ipv4get() {
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} --data "resp_format=text" https://my.incapsula.com/api/integration/v1/ips  > $CFIPLOG
	
	CFIPS=$(cat $CFIPLOG)
	
	echo "--------------------------------------------"
	echo " Downloading Incapsula P list"
	echo " from: Incapsula API"
	echo "--------------------------------------------"
	echo ""
	echo "--------------------------------------------"
	echo " Format for Centminmod.com Nginx Installer"
	echo "  1). add to nginx.conf"
	echo "  2). add to /etc/csf/csf.allow"
	echo "--------------------------------------------"
	
	for ip in $CFIPS; 
	do
		if [[ "$(ipcalc -c "$ip" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
			if [[ -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" = '0' ]]; then
				if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
					echo "set_real_ip_from $ip;" >> $CFIPNGINXLOG
					echo "csf -a $ip incapsula" >> $CFIPCSFLOG
				elif [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
					echo "set_real_ip_from $ip;" >> $CFIPNGINXLOG
					echo "csf -a $ip incapsula" >> $CFIPCSFLOG
				fi
			else
        	if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
						echo "set_real_ip_from $ip;" >> $CFIPNGINXLOG
						echo "csf -a $ip incapsula" >> $CFIPCSFLOG
        	fi
			fi
		fi
	done
	echo "real_ip_header X-Forwarded-For;" >> $CFIPNGINXLOG
	
	echo "--------------------------------------------"
	echo "  1). add to nginx.conf"
	echo "--------------------------------------------"
	cat $CFIPNGINXLOG
	
	echo ""
	
	echo "--------------------------------------------"
	echo "  2). add to /etc/csf/csf.allow"
	echo "--------------------------------------------"
	cat $CFIPCSFLOG
	
	rm -rf $CFIPLOG
	rm -rf $CFIPNGINXLOG
	rm -rf $CFIPCSFLOG
	
	echo "--------------------------------------------"
}

###############################
csfadd() {
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} --data "resp_format=text" https://my.incapsula.com/api/integration/v1/ips  > $CFIPLOG
	
	CFIPS=$(cat $CFIPLOG)
	
	echo "--------------------------------------------"
	echo " Add Incapsula P list to CSF"
	echo " from: Incapsula API"
	echo "--------------------------------------------"
	echo ""


	echo "--------------------------------------------"
	echo "  Add to /etc/csf/csf.allow"
	echo "--------------------------------------------"

	for ip in $CFIPS; 
	do
		if [[ "$(grep "$ip" /etc/csf/csf.allow >/dev/null 2>&1; echo $?)" = '1' ]] || [[ "$(grep "$ip" /etc/csf/csf.ignore >/dev/null 2>&1; echo $?)" = '1' ]]; then
			if [[ "$(ipcalc -c "$ip" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
				if [[ -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" = '0' ]]; then
					if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
						csf -a "$ip" incapsula
						echo "$ip" >> /etc/csf/csf.ignore
					elif [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
						csf -a "$ip" incapsula
						echo "$ip" >> /etc/csf/csf.ignore
					fi
				else
        	if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
						csf -a "$ip" incapsula
						echo "$ip" >> /etc/csf/csf.ignore
        	fi
				fi
			fi
		fi
	done
}

###############################
nginxsetup() {
	echo
	# echo "create $CFINCLUDEFILE include file"
	if [ -f "$CFINCLUDEFILE" ]; then
		\cp -af "$CFINCLUDEFILE" "${CFINCLUDEFILE}.bak"
	fi
	echo > $CFINCLUDEFILE
	cflista=$(/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} --data "resp_format=text" https://my.incapsula.com/api/integration/v1/ips)
	if [ ! -f /usr/local/nginx/conf/incapsula_customips.conf ]; then
		touch /usr/local/nginx/conf/incapsula_customips.conf
	fi
	echo "include /usr/local/nginx/conf/incapsula_customips.conf;" >> $CFINCLUDEFILE
	for i in $cflista; do
      if [[ "$(ipcalc -c "$i" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
      	if [[ -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" = '0' ]]; then
      		if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '0' ]]; then
        		echo "set_real_ip_from $i;" >> $CFINCLUDEFILE
        	elif [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
        		echo "set_real_ip_from $i;" >> $CFINCLUDEFILE
        	fi
        else
        	if [[ "$(ipcalc -6 "$i" >/dev/vull 2>&1; echo $?)" -eq '1' ]]; then
        		echo "set_real_ip_from $i;" >> $CFINCLUDEFILE
        	fi
        fi
      fi
	done
	echo "real_ip_header X-Forwarded-For;" >> $CFINCLUDEFILE
	if [[ "$(diff -u "${CFINCLUDEFILE}.bak" "$CFINCLUDEFILE" >/dev/vull 2>&1; echo $?)" -ne '0' ]]; then
		service nginx reload >/dev/null 2>&1
	fi
	rm -rf "${CFINCLUDEFILE}.bak"
	echo "created $CFINCLUDEFILE include file"
}

###############################
case "$1" in
ips)
	ipv4get
;;
csf)
	csfadd
;;
nginx)
	nginxsetup
;;
auto)
	csfadd
	nginxsetup
;;
*)
echo "$0 {ips|csf|nginx|auto}"
;;
esac
exit