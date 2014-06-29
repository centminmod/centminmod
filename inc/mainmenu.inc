cminstall() {

starttime=$(date +%s.%N)

lowmemcheck
centminlog
diskalert
{

CHECKCENTMINMODINSTALL=`ls /etc/init.d | grep -E '(csf|lfd|nginx|php-fpm|^nsd)'`
if [ ! -z "$CHECKCENTMINMODINSTALL" ]; then
echo ""
echo "Centmin Mod previous installation detected. "
echo ""
echo "If you are upgrading a server which already previously had Centmin Mod installed"
echo "you DO NOT need to run option #1, instead run option #4 & then #5 for upgrading"
echo "Nginx web server and upgrading PHP."
echo ""
echo "exiting script"
exit
fi

alldownloads
funct_centmininstall
echo "$SCRIPT_VERSION" > /etc/centminmod-release
#echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log
echo "Total Centmin Mod Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_install.log

exit 0

} #cminstall

cmainmenu() {
while :
do
	# clear
        # display menu
	cecho "--------------------------------------------------------" $boldyellow
	cecho "Centmin Mod $SCRIPT_VERSION - $SCRIPT_URL" $boldgreen
	#cecho "Menu/Mods Author: $SCRIPT_MODIFICATION_AUTHOR" $boldgreen
	#cecho "Centmin Original Author: $SCRIPT_AUTHOR" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "                   Centmin Mod Menu                   " $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "1).  Centmin Install" $boldgreen
	cecho "2).  Add Nginx vhost domain" $boldgreen
	cecho "3).  NSD setup domain name DNS" $boldgreen
	cecho "4).  Nginx Upgrade / Downgrade" $boldgreen
	cecho "5).  PHP Upgrade / Downgrade" $boldgreen
	cecho "6).  XCache Re-install" $boldgreen
	cecho "7).  APC Cache Re-install" $boldgreen
	cecho "8).  XCache Install" $boldgreen
	cecho "9).  APC Cache Install" $boldgreen
	cecho "10). Memcached Server Re-install" $boldgreen
	cecho "11). MariaDB 5.2, 5.5, 10 Upgrade Sub-Menu" $boldgreen
	cecho "12). Zend OpCache Install/Re-install" $boldgreen
	cecho "13). Install ioping.sh vbtechsupport.com/1239/" $boldgreen
	cecho "14). SELinux disable" $boldgreen
	cecho "15). Install/Re-install imagick PHP Extension" $boldgreen
	cecho "16). Change SSHD Port Number" $boldgreen
	cecho "17). Multi-thread compression: pigz,pbzip2,lbzip2,p7zip etc" $boldgreen
	cecho "18). Suhosin PHP Extension install" $boldgreen
	cecho "19). Install FFMPEG and FFMPEG PHP Extension" $boldgreen
	cecho "20). NSD Re-install" $boldgreen
	cecho "21). Exit" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow

	read -ep "Enter option [ 1 - 21 ] " option
	cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$option" in
1|install)

cminstall

;;
2|addvhost)

centminlog
{
funct_nginxaddvhost
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log

;;
3|nsdsetup)

centminlog
{
funct_nsdsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_setup.log

;;
4|nginxupgrade)

starttime=$(date +%s.%N)

centminlog
diskalert
csftweaks

{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

yumskipinstall
if [[ "$yuminstallrun" == [yY] ]]; then
yuminstall
fi
funct_nginxupgrade
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log
echo "Total Nginx Upgrade Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_upgrade.log

;;
5|phpupgrade)

starttime=$(date +%s.%N)

centminlog
diskalert
csftweaks

{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

yumskipinstall
if [[ "$yuminstallrun" == [yY] ]]; then
yuminstall
fi
funct_phpupgrade
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log
echo "Total PHP Upgrade Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_php_upgrade.log

;;
6|xcachereinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_xcachereinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log
echo "Total Xcache Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_reinstall.log

;;
7|apcreinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_apcreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log
echo "Total APC Cache Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_reinstall.log

;;
8|installxcache)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

MANXCACHEINSTALL='y'

funct_installxcache
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log
echo "Total Xcache Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_xcache_install.log

;;
9|installapc)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_installapc
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log
echo "Total APC Cache Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_apc_install.log

;;
10|memcachedreinstall)

starttime=$(date +%s.%N)

centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

funct_memcachedreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log
	fi
fi

endtime=$(date +%s.%N)
INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log
echo "Total Memcached Re-Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_memcached_reinstall.log

;;
11|mariadbsubmenu)

mariadbsubmenu

;;
12|zendoptcachesubmenu)

zendoptcachesubmenu

;;
13|iopinginstall)

funct_installiopingcentmin

;;
14|selinux)

funct_selinux

;;
15|imagick)

imagickinstall

;;
16|sshdport)

funct_sshd

;;
17|multithreadcomp)

funct_pigzinstall
funct_pbzip2install
funct_lbzip2install
funct_lzipinstall
funct_plzipinstall
funct_p7zipinstall

;;
18|suhosininstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

suhosinsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_suhosin_install.log
	fi
fi

;;
19|ffmpeginstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

ffmpegsetup
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_ffmpeg_install.log
	fi
fi

;;
20|nsdreinstall)
centminlog
{

if [ "$CCACHEINSTALL" == 'y' ]; then
ccacheinstall
fi

nsdreinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log

if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ -f /usr/bin/ccache ]; then
{ echo ""; source ~/.bashrc; echo "ccache stats:"; ccache -s; echo ""; } 2>&1 | tee -a  ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log
	fi
fi

;;
21|exit)

bookmark

exit 0

;;

esac

done
} #cmainmenu