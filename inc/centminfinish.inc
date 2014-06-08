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
	cecho "Check YUM Repo List" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
yum repolist

echo ""
	fi # unattended

if [[ "$NSD_DISABLED" = [yY] ]]; then

	echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Stopping NSD service:" $boldyellow
	service nsd stop
	cecho "Disabling NSD service:" $boldyellow
	chkconfig nsd off
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
	service memcached stop
	cecho "Disabling Memcached server service:" $boldyellow
	chkconfig memcached stop
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
	service php-fpm stop
	cecho "Disabling php-fpm service:" $boldyellow
	chkconfig php-fpm off
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
	service mysql stop
	cecho "Disabling mysql service:" $boldyellow
	chkconfig mysql off
	cecho "--------------------------------------------------------" $boldyellow
	cecho "In future to re-enable mysql service type these 2 commands:" $boldyellow
	echo "service mysql start"
	echo "chkconfig mysql on"
	cecho "--------------------------------------------------------" $boldyellow
fi

}

centminfinish() {

installchecks
DATENOW=$(date +"%b %d")
grep -h $DATENOW /var/log/yum.log | tee -a ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yum-log.log


    echo "*************************************************"
    cecho "* Centmin Mod install completed...." $boldgreen
    echo "*************************************************"

}