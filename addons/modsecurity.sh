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
	echo "/opt/pcre/lib" > /etc/ld.so.conf.d/modsec_pcre.conf
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
	else
		if [ ! -f /usr/local/nginx/conf/modsecurity.conf ]; then
			cp -a modsecurity.conf-recommended /usr/local/nginx/conf/modsecurity.conf
		fi
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