installchecks() {

	if [ "$UNATTENDED" == 'y' ]; then

waits() {
sleep 2
}

    echo "*************************************************"
    cecho "* Post-Install Check List...." $boldgreen
    echo "*************************************************"
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check ccache Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	ccache -V
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Nginx Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	nginx -V
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check PHP-FPM Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	php -v
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check MariaDB installed RPM Versions:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
	rpm -qa | grep -i MariaDB
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Memcached Server Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow	
	/usr/local/bin/memcached -h | head -n1
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check CSF Firewall Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
csf -v
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check Siege Benchmark Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
siege -V
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check ngx_pagespeed Control Script:" $boldyellow
	cecho "http://centminmod.com/nginx_ngx_pagespeed.html" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
pscontrol
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check TwemPerf Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
mcperf -V
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check mysqlreport version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
/root/mysqlreport --help | head -n1
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check NSD Bind Version:" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
nsd -v
waits
echo ""
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Check YUM Repo List" $boldyellow
	cecho "--------------------------------------------------------" $boldyellow
yum repolist

echo ""
	fi # unattended
}

centminfinish() {

installchecks


    echo "*************************************************"
    cecho "* Centmin Mod install completed...." $boldgreen
    echo "*************************************************"

}