#!/bin/bash
###############################
# written by George Liu (eva2000)
# centminmod.com
###############################
CFIPLOG='/root/cfips.txt'
CFIP6LOG='/root/cfips6.txt'
CFIPNGINXLOG='/root/cfnginxlog.log'
CFIPCSFLOG='/root/csf_log.log'
CFINCLUDEFILE='/usr/local/nginx/conf/cloudflare.conf'
CURL_TIMEOUTS='--max-time 20 --connect-timeout 20'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###############################
if [ ! -f /usr/bin/curl ]; then
	echo "Installing curl please wait..."
	yum -y -q install curl
fi
###############################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi

ipv4get() {
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v4/ > $CFIPLOG
	
	CFIPS=$(cat $CFIPLOG)
	
	echo "--------------------------------------------"
	echo " Downloading Cloudflare IP list"
	echo " from: https://www.cloudflare.com/ips-v4/"
	echo "--------------------------------------------"
	echo ""
	echo "--------------------------------------------"
	echo " Format for Centminmod.com Nginx Installer"
	echo "  1). add to nginx.conf"
	echo "  2). add to /etc/csf/csf.allow"
	echo "--------------------------------------------"
	
	for ip in $CFIPS; 
	do
		if [[ "$(ipcalc -c "$ip" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
			echo "set_real_ip_from $ip;" >> $CFIPNGINXLOG
			echo "csf -a $ip cloudflare" >> $CFIPCSFLOG
		fi
	done
	echo "real_ip_header CF-Connecting-IP;" >> $CFIPNGINXLOG
	
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
ipv6get() {
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v6/ > $CFIP6LOG
	
	CFIPS=$(cat $CFIP6LOG)
	
	echo "--------------------------------------------"
	echo " Downloading Cloudflare IP list"
	echo " from: https://www.cloudflare.com/ips-v6/"
	echo "--------------------------------------------"
	echo ""
	echo "--------------------------------------------"
	echo " Format for Centminmod.com Nginx Installer"
	echo "  1). add to nginx.conf"
	echo "  2). add to /etc/csf/csf.allow"
	echo "--------------------------------------------"
	
	for ip in $CFIPS; 
	do
		if [[ "$(ipcalc -c "$ip" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
			echo "set_real_ip_from $ip;" >> $CFIPNGINXLOG
			echo "csf -a $ip cloudflare" >> $CFIPCSFLOG
		fi
	done
	echo "real_ip_header CF-Connecting-IP;" >> $CFIPNGINXLOG
	
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
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v4/ > $CFIPLOG
	/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v6/ > $CFIP6LOG
	
	CFIPS=$(cat $CFIPLOG)
	CFIP6S=$(cat $CFIP6LOG)
	
	echo "--------------------------------------------"
	echo " Add Cloudflare IP list to CSF"
	echo " from: https://www.cloudflare.com/ips-v4/"
	echo " from: https://www.cloudflare.com/ips-v6/"
	echo "--------------------------------------------"
	echo ""


	echo "--------------------------------------------"
	echo "  Add to /etc/csf/csf.allow"
	echo "--------------------------------------------"

	for ip in $CFIPS; 
	do
		if [[ "$(grep "$ip" /etc/csf/csf.allow >/dev/null 2>&1; echo $?)" = '1' ]] || [[ "$(grep "$ip" /etc/csf/csf.ignore >/dev/null 2>&1; echo $?)" = '1' ]]; then
			if [[ "$(ipcalc -c "$ip" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
				csf -a "$ip" cloudflare
				echo "$ip" >> /etc/csf/csf.ignore
			fi
		fi
	done

	if [[ "$(awk -F '= ' '/^IPV6 =/ {print $2}' /etc/csf/csf.conf | sed -e 's|\"||g')" = '1' ]]; then
	for ip in $CFIP6S; 
	do
		if [[ "$(grep "$ip" /etc/csf/csf.allow >/dev/null 2>&1; echo $?)" = '1' ]] || [[ "$(grep "$ip" /etc/csf/csf.ignore >/dev/null 2>&1; echo $?)" = '1' ]]; then
			if [[ "$(ipcalc -c "$ip" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
				csf -a "$ip" cloudflare
				echo "$ip" >> /etc/csf/csf.ignore
			fi
		fi
	done
	fi

	# auto fix previous bug
	# https://community.centminmod.com/posts/45907/
	sed -i '/^ip/d' /etc/csf/csf.ignore
}

###############################
nginxsetup() {
	echo
	# echo "create $CFINCLUDEFILE include file"
	if [ -f "$CFINCLUDEFILE" ]; then
		\cp -af "$CFINCLUDEFILE" "${CFINCLUDEFILE}.bak"
	fi
	echo > $CFINCLUDEFILE
	cflista=$(/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v4/)
	cflistb=$(/usr/bin/curl -${ipv_forceopt}s ${CURL_TIMEOUTS} https://www.cloudflare.com/ips-v6/)
	if [ ! -f /usr/local/nginx/conf/cloudflare_customips.conf ]; then
		touch /usr/local/nginx/conf/cloudflare_customips.conf
	fi
	echo "include /usr/local/nginx/conf/cloudflare_customips.conf;" >> $CFINCLUDEFILE
	for i in $cflista; do
      if [[ "$(ipcalc -c "$i" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        	echo "set_real_ip_from $i;" >> $CFINCLUDEFILE
      fi
	done
	if [[ -f /etc/sysconfig/network && "$(awk -F "=" '/NETWORKING_IPV6/ {print $2}' /etc/sysconfig/network | grep 'yes' >/dev/null 2>&1; echo $?)" = '0' ]]; then
		for i in $cflistb; do
      if [[ "$(ipcalc -c "$i" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        		echo "set_real_ip_from $i;" >> $CFINCLUDEFILE
      fi
		done
	else
		for i in $cflistb; do
      if [[ "$(ipcalc -c "$i" >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        		echo "#set_real_ip_from $i;" >> $CFINCLUDEFILE
      fi
		done
	fi
	echo "real_ip_header CF-Connecting-IP;" >> $CFINCLUDEFILE
	if [[ "$(diff -u "${CFINCLUDEFILE}.bak" "$CFINCLUDEFILE" >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
		service nginx reload >/dev/null 2>&1
	fi
	rm -rf "${CFINCLUDEFILE}.bak"
	echo "created $CFINCLUDEFILE include file"
}

###############################
case "$1" in
ipv4)
	ipv4get
;;
ipv6)
	ipv6get
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
echo "$0 {ipv4|ipv6|csf|nginx|auto}"
;;
esac
exit