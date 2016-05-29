#!/bin/bash
############################################################################
# Official centminmod.com addon: wpcli.sh installer
# written by George Liu (eva2000) centminmod.com
############################################################################
# http://wp-cli.org/
# WP-CLI is a set of command-line tools for managing WordPress 
# installations. You can update plugins, set up multisite installs, 
# create posts etc
############################################################################
# install instructions
# chmod +x /usr/local/src/centminmod/addons/wpcli.sh
# cd /usr/local/src/centminmod/addons
# ./wpcli.sh install
############################################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
WPCLIDIR='/root/wpcli'
WPCLILINK='https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar'

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

# fallback mirror if official wp-cli download http status is not 200, use local
# centminmod.com mirror download instead
curl -sI --connect-timeout 5 --max-time 5 $WPCLILINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
WPCLI_CURLCHECK=$?
if [[ "$WPCLI_CURLCHECK" != '0' ]]; then
	WPCLILINK='https://centminmod.com/centminmodparts/wp-cli/wp-cli.phar'
fi

# functions
updatewpcli() {
	if [ -f /usr/bin/wp ]; then
		echo "update wp-cli"
		rm -rf /usr/bin/wp
		wget -cnv --no-check-certificate $WPCLILINK -O /usr/bin/wp --tries=3
		chmod 0700 /usr/bin/wp
		/usr/bin/wp --info --allow-root	
		echo ""
		echo "-------------------------------------------------------------"
		echo "wp-cli update completed"
		echo "Read http://wp-cli.org/ for full usage info"
		echo "-------------------------------------------------------------"
		echo
	fi	
}

installwpcli() {

mkdir -p $WPCLIDIR

if [ ! -f /usr/bin/git ]; then
	yum -q -y install git
fi

if [[ ! -f /usr/bin/wp ]]; then

echo ""
if [ -s /usr/bin/wp ]; then
  echo "/usr/bin/wp [found]"
  else
  echo "Error: /usr/bin/wp not found !!! Downloading now......"
  wget -cnv --no-check-certificate $WPCLILINK -O /usr/bin/wp --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	echo "Error: /usr/bin/wp download failed."
	exit $ERROR
else 
         echo "Download done."
	fi
fi

chmod 0700 /usr/bin/wp

echo ""
if [ -s "${WPCLIDIR}/wp-completion.bash" ]; then
  echo "${WPCLIDIR}/wp-completion.bash [found]"
  else
  echo "Error: ${WPCLIDIR}/wp-completion.bash not found !!! Downloading now......"
  wget -cnv --no-check-certificate https://github.com/wp-cli/wp-cli/raw/master/utils/wp-completion.bash -O ${WPCLIDIR}/wp-completion.bash --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	echo "Error: ${WPCLIDIR}/wp-completion.bash download failed."
	exit $ERROR
else 
         echo "Download done."
	fi
fi

echo ""

WPCLICHECK=$(grep 'WP-CLI' /root/.bash_profile)

if [[ -z "$WPCLICHECK" ]]; then
	echo ""
	echo "" >> /root/.bash_profile
	#echo "# Composer scripts" >> /root/.bash_profile
	#echo "PATH=$HOME/.wp-cli/bin:$PATH" >> /root/.bash_profile
	#echo "" >> /root/.bash_profile
	echo "# WP-CLI completions" >> /root/.bash_profile
	echo "source ${WPCLIDIR}/wp-completion.bash" >> /root/.bash_profile
fi

echo "-------------------------------------------------------------"
/usr/bin/wp --info --allow-root
echo "-------------------------------------------------------------"

echo ""
echo "-------------------------------------------------------------"
echo "wp-cli install completed"
echo "Read http://wp-cli.org/ for full usage info"

echo ""
echo "-------------------------------------------------------------"
echo "Please log out of SSH session and log back in"
echo "You can then call wp-cli via command: wp"
echo "i.e. wp --info --allow-root"
echo "-------------------------------------------------------------"

fi

}

############################################################################

case "$1" in
install)
starttime=$(date +%s.%N)
{
echo "installing..."
installwpcli
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log
echo "Total WPCLI Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log
;;
update)
ustarttime=$(date +%s.%N)
{
echo "updating..."
updatewpcli
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log

uendtime=$(date +%s.%N)

UINSTALLTIME=$(echo "scale=2;$uendtime - $ustarttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log
echo "Total WPCLI Upgrade Time: $UINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log
;;
*)
echo "$0 {install|update}"
;;
esac

exit