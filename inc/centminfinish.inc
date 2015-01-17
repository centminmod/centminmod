installchecks() {

	if [ "$UNATTENDED" == 'y' ]; then

letswait() {
sleep 0.3
}

entropyup

    echo "*************************************************"
    cecho "* Post-Install Check List...." $boldgreen
    echo "*************************************************"
letswait

echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check ccache Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	ccache -V
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Nginx Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	nginx -V
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check PHP-FPM Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	php -v
letswait

echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check MariaDB installed RPM Versions:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	rpm -qa | grep -i MariaDB
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Memcached Server Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow	
	/usr/local/bin/memcached -h | head -n1
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check CSF Firewall Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
csf -v
letswait

echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Siege Benchmark Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
siege -V
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check ngx_pagespeed Control Script:" $boldyellow
	cecho "http://centminmod.com/nginx_ngx_pagespeed.html" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
pscontrol
letswait
# echo ""
# 	cecho "--------------------------------------------------------" $boldyellow
# 	cecho "Check TwemPerf Version:" $boldyellow
# 	cecho "--------------------------------------------------------" $boldyellow
# mcperf -V
# letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check mysqlreport version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
/root/mysqlreport --help | head -n1
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check NSD Bind Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
nsd -v
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check pure-ftpd Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
pure-ftpd -h | head -n1 | awk '{print $1,$2}'
letswait
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check YUM Repo List" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
yum repolist

echo ""
	fi # unattended

if [[ "$NSD_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping NSD service:" $boldyellow
	cmservice nsd stop
	cecho "Disabling NSD service:" $boldyellow
	cmchkconfig nsd off
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable NSD service type these 2 commands:" $boldyellow
	echo "service nsd start"
	echo "chkconfig nsd on"
	cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$MEMCACHED_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping Memcached server service:" $boldyellow
	cmservice memcached stop
	cecho "Disabling Memcached server service:" $boldyellow
	cmchkconfig memcached stop
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable Memcached server service type these 2 commands:" $boldyellow
	echo "service memcached start"
	echo "chkconfig memcached on"
	cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$PHP_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping php-fpm service:" $boldyellow
	cmservice php-fpm stop
	cecho "Disabling php-fpm service:" $boldyellow
	cmchkconfig php-fpm off
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable php-fpm service type these 2 commands:" $boldyellow
	echo "service php-fpm start"
	echo "chkconfig php-fpm on"
	cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$MYSQLSERVICE_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping mysql service:" $boldyellow
	cmservice mysql stop
	cecho "Disabling mysql service:" $boldyellow
	cmchkconfig mysql off
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable mysql service type these 2 commands:" $boldyellow
	echo "service mysql start"
	echo "chkconfig mysql on"
	cecho "--------------------------------------------------------" $boldyellow
fi

if [[ "$PUREFTPD_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping pure-ftpd service:" $boldyellow
	cmservice pure-ftpd stop
	cecho "Disabling pure-ftpd service:" $boldyellow
	cmchkconfig pure-ftpd off
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable pure-ftpd service type these 2 commands:" $boldyellow
	echo "service pure-ftpd start"
	echo "chkconfig pure-ftpd on"
	cecho "--------------------------------------------------------" $boldyellow
fi

}

centminfinish() {

if [[ ! $(ps aufx | grep rsyslog | grep -v grep) ]]; then
	cmservice rsyslog start
	cmchkconfig rsyslog on
fi

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' ]]; then
	axivosixyum
fi

if [[ "$CENTOSVER" = '7.0' ]]; then
	axivoseven
fi

installchecks
DATENOW=$(date +"%b %d")
YUMLOG=$(grep -h $DATENOW /var/log/yum.log)
echo "$YUMLOG" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yum-log.log


    echo "*************************************************"
    cecho "* Centmin Mod install completed...." $boldgreen
    echo "*************************************************"

}