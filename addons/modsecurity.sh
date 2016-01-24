#!/bin/bash
###############################################################################
# modsecurity installer for centminmod.com LEMP stack
# required to be installed before you can enable modsecurity nginx module as
# outlined at https://community.centminmod.com/posts/24344/ using persistent
# config file at /etc/centminmod/custom_config.inc add variable:
# 
# NGINX_MODSECURITY=y
# 
# then run centmin.sh menu option 4 to recompile nginx and if addons/modsecurity.sh
# ran and installed modsecurity, then nginx configuration routine in centmin.sh
# menu option 4 will auto detect and configure modsecurity ngin module
# 
# actual configuration of modsecurity on Nginx will be left to end user without 
# any support from George (eva2000). I'll provide the install but configuration 
# and tuning of modsecurity for nginx will be left to end user.
###############################################################################
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
MODSEC_PCREVER='8.38'
APR_VER='1.5.2'
APRU_VER='1.5.4'
MODSEC_VER='2.9.0'
OPENSSL_VER='1.0.2e'
PREFIX='/opt'
PCRE_PREFIX='/opt/pcre/lib'
TMP_DIR='/svr-setup'
TMP_SSL='/opt'
PCRELINKFILE="pcre-${MODSEC_PCREVER}.tar.gz"
PCRELINK="http://centminmod.com/centminmodparts/pcre/${PCRELINKFILE}"
###############################################################################

modsec_install() {

	if [[ "$IS_UPDATE" != [yY] ]]; then
		yum -y install libuuid libuuid-devel libcurl-devel httpd-devel lua lua-devel ssdeep ssdeep-devel
		service httpd stop
		chkconfig httpd off
	fi
	
	cd $TMP_DIR/openssl-${OPENSSL_VER}
	export BPATH=$TMP_SSL
	export STATICLIBSSL="${BPATH}/staticlibssl"
	rm -rf "$STATICLIBSSL"
	mkdir -p "$STATICLIBSSL"
	make clean
	./config --prefix=$STATICLIBSSL no-shared enable-tlsext enable-ec_nistp_64_gcc_128
	make depend
	make
	make install
	
	cd $TMP_DIR
	if [[ ! -f apr-${APR_VER}.tar.gz ]]; then
		wget -cnv http://apache.mirror.uber.com.au//apr/apr-${APR_VER}.tar.gz
	fi
	tar xzf apr-${APR_VER}.tar.gz
	cd apr-${APR_VER}
	make clean
	./configure --prefix=${PREFIX}
	make -j2
	make install
	
	cd $TMP_DIR
	if [[ ! -f apr-util-${APRU_VER}.tar.gz ]]; then
		wget -cnv http://apache.mirror.uber.com.au/apr/apr-util-${APRU_VER}.tar.gz
	fi
	tar xzf apr-util-${APRU_VER}.tar.gz
	cd apr-util-${APRU_VER}
	make clean
	./configure --prefix=${PREFIX} --with-apr=${PREFIX} --with-openssl=$TMP_DIR/staticlibssl/lib --with-crypto
	make -j2
	make install
	echo "/opt/lib" > /etc/ld.so.conf.d/modsec_apr.conf
	ldconfig

	cd $TMP_DIR
	mkdir modsec_pcre
	cd modsec_pcre
	wget -cnv $PCRELINK
	tar xzf $PCRELINKFILE
	cd pcre-${MODSEC_PCREVER}
	make clean
	./configure --prefix=${PCRE_PREFIX} --enable-jit --enable-pcre16 --enable-pcre32 --enable-unicode-properties
	make -j2
	make install
	echo "/opt/pcre/lib/lib" > /etc/ld.so.conf.d/modsec_pcre.conf
	ldconfig
	
	cd $TMP_DIR
	if [[ ! -f modsecurity-${MODSEC_VER}.tar.gz ]]; then
		wget -cnv https://www.modsecurity.org/tarball/2.9.0/modsecurity-${MODSEC_VER}.tar.gz
	fi
	tar xvzf modsecurity-${MODSEC_VER}.tar.gz
	cd modsecurity-${MODSEC_VER}
	make clean
	./configure --enable-standalone-module --enable-pcre-jit --enable-lua-cache --enable-request-early --disable-mlogc --with-apu=${PREFIX} --with-apr=${PREFIX} --with-pcre=${PCRE_PREFIX}
	make -j2
	make install
	if [[ "$IS_UPDATE" != [yY] ]]; then
		cp -a modsecurity.conf-recommended /usr/local/nginx/conf/modsecurity.conf
		cp -a unicode.mapping /usr/local/nginx/conf/
		sed -i 's|SecRequestBodyLimit .*|SecRequestBodyLimit 131072000|' /usr/local/nginx/conf/modsecurity.conf
		sed -i 's|SecAuditLogType Serial|SecAuditLogType Concurrent|' /usr/local/nginx/conf/modsecurity.conf
		sed -i 's|^#SecAuditLogStorageDir \/opt\/modsecurity\/var\/audit\/|SecAuditLogStorageDir \/var\/log\/modsecurity\/audit\/|' /usr/local/nginx/conf/modsecurity.conf
		mkdir -p /var/log/modsecurity/audit/
		chown -R nginx:nginx /var/log/modsecurity/audit/
	else
		if [ ! -f /usr/local/nginx/conf/modsecurity.conf ]; then
			cp -a modsecurity.conf-recommended /usr/local/nginx/conf/modsecurity.conf
			sed -i 's|SecRequestBodyLimit .*|SecRequestBodyLimit 131072000|' /usr/local/nginx/conf/modsecurity.conf
			sed -i 's|SecAuditLogType Serial|SecAuditLogType Concurrent|' /usr/local/nginx/conf/modsecurity.conf
			sed -i 's|^#SecAuditLogStorageDir \/opt\/modsecurity\/var\/audit\/|SecAuditLogStorageDir \/var\/log\/modsecurity\/audit\/|' /usr/local/nginx/conf/modsecurity.conf
			mkdir -p /var/log/modsecurity/audit/
			chown -R nginx:nginx /var/log/modsecurity/audit/
		fi
		if [ ! -f /usr/local/nginx/conf/unicode.mapping ]; then
			cp -a unicode.mapping /usr/local/nginx/conf/
		fi
	fi
	# configure OWASP Core Rule Set
	cd $TMP_DIR
	if [ ! -d owasp-modsecurity-crs ]; then
		git clone https://github.com/SpiderLabs/owasp-modsecurity-crs.git
	else
		rm -rf owasp-modsecurity-crs
		git clone https://github.com/SpiderLabs/owasp-modsecurity-crs.git
	fi
	cd owasp-modsecurity-crs
	\cp -Rf base_rules/ /usr/local/nginx/conf/
	if [[ -z "$(grep 'base_rule' /usr/local/nginx/conf/modsecurity.conf)" ]]; then
cat >> "/usr/local/nginx/conf/modsecurity.conf" <<MDD

#DefaultAction
SecDefaultAction "log,deny,phase:1"

#If you want to load single rule /usr/loca/nginx/conf
#Include base_rules/modsecurity_crs_41_sql_injection_attacks.conf

#Load all Rule
Include base_rules/*.conf

#Disable rule by ID from error message (for wordpress)
SecRuleRemoveById 981172 981173 960032 960034 960017 960010 950117 981004 960015
MDD
	fi
}

setupmsg() {
echo "
###############################################################################
# modsecurity has been installed
# 
# modsecurity.conf located at /usr/local/nginx/conf/modsecurity.conf
# nginx.conf located at /usr/local/nginx/conf/nginx.conf
#
# You need to now enable NGINX_MODSECURITY=y in centmin mod
#
###############################################################################
# modsecurity installer for centminmod.com LEMP stack
# required to be installed before you can enable modsecurity nginx module as
# outlined at https://community.centminmod.com/posts/24344/ using persistent
# config file at /etc/centminmod/custom_config.inc add variable:
# 
# NGINX_MODSECURITY=y
# 
# then run centmin.sh menu option 4 to recompile nginx and if addons/modsecurity.sh
# ran and installed modsecurity, then nginx configuration routine in centmin.sh
# menu option 4 will auto detect and configure modsecurity ngin module
# 
# actual configuration of modsecurity on Nginx will be left to end user without 
# any support from George (eva2000). I'll provide the install but configuration 
# and tuning of modsecurity for nginx will be left to end user.
###############################################################################
"
}

updatemsg() {
	echo
	echo "modsecurity has been updated."
	echo
}

############################################################################
case "$1" in
	install )
		starttime=$(date +%s.%N)
		{
		modsec_install
		setupmsg
		} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_modsecuriy_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_modsecuriy_install_${DT}.log
echo "Total Modsecurity Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_modsecuriy_install_${DT}.log
		;;
	update )
		starttime=$(date +%s.%N)
		{
		IS_UPDATE=y
		modsec_install
		updatemsg
		} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_modsecuriy_update_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_modsecuriy_update_${DT}.log
echo "Total Modsecurity Source Compile Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_modsecuriy_update_${DT}.log
		;;		
	* )
		echo "$0 {install|update}"
		;;
esac