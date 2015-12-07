opcachehugepages() {
	# https://www.kernel.org/doc/Documentation/vm/transhuge.txt
	# only enable PHP zend opcache opcache.huge_code_pages=1 support if on CentOS 7.x and kernel
	# supports transparent hugepages. Otherwise, disable it in PHP zend opcache
	if [[ -f /sys/kernel/mm/redhat_transparent_hugepage/enabled && "$PHPMUVER" = '7.0' ]]; then
		# cat /sys/kernel/mm/redhat_transparent_hugepage/enabled
		HP_CHECK=$(cat /sys/kernel/mm/redhat_transparent_hugepage/enabled | grep -o '\[.*\]')
		if [[ "$CENTOS_SIX" = '6' ]]; then
			if [ -f /etc/centminmod/php.d/zendopcache.ini ]; then
				OPCACHEHUGEPAGES_OPT=''
				echo $OPCACHEHUGEPAGES_OPT
				if [[ "$(grep 'opcache.huge_code_pages' /etc/centminmod/php.d/zendopcache.ini)" ]]; then
					sed -i 's|^opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
					sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
				else
					echo "opcache.huge_code_pages=0" >> /etc/centminmod/php.d/zendopcache.ini
				fi
			fi			
		elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[always]' ]]; then
			if [ -f /etc/centminmod/php.d/zendopcache.ini ]; then
				OPCACHEHUGEPAGES_OPT=' --enable-huge-code-pages'
				echo $OPCACHEHUGEPAGES_OPT
				if [[ "$(grep 'opcache.huge_code_pages' /etc/centminmod/php.d/zendopcache.ini)" ]]; then
					sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=1|' /etc/centminmod/php.d/zendopcache.ini
				else
					echo "opcache.huge_code_pages=1" >> /etc/centminmod/php.d/zendopcache.ini
				fi
			fi
		elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[never]' ]]; then
			if [ -f /etc/centminmod/php.d/zendopcache.ini ]; then
				OPCACHEHUGEPAGES_OPT=''
				echo $OPCACHEHUGEPAGES_OPT
				if [[ "$(grep 'opcache.huge_code_pages' /etc/centminmod/php.d/zendopcache.ini)" ]]; then
					sed -i 's|^opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
					sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
				else
					echo "opcache.huge_code_pages=0" >> /etc/centminmod/php.d/zendopcache.ini
				fi
			fi				
		fi
	else
		if [ -f /etc/centminmod/php.d/zendopcache.ini ]; then
			if [[ "$(grep 'opcache.huge_code_pages' /etc/centminmod/php.d/zendopcache.ini)" ]]; then
				sed -i 's|^opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
				sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=0|' /etc/centminmod/php.d/zendopcache.ini
			else
				echo "opcache.huge_code_pages=0" >> /etc/centminmod/php.d/zendopcache.ini
			fi		
		fi
	fi
}