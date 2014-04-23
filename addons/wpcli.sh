#!/bin/bash
###############################################
# Official centminmod.com addon: wpcli.sh installer
# written by George Liu (eva2000) vbtechsupport.com
###############################################
# http://wp-cli.org/
# WP-CLI is a set of command-line tools for managing WordPress 
# installations. You can update plugins, set up multisite installs, 
# create posts etc
###############################################
# install instructions
# chmod +x /usr/local/src/centmin-v1.2.3mod/addons/wpcli.sh
# cd /usr/local/src/centmin-v1.2.3mod/addons
# ./wpcli.sh install
###############################################
DT=`date +"%d%m%y-%H%M%S"`
WPCLIDIR='/root/wpcli'

# functions

installwpcli() {

mkdir -p $WPCLIDIR

yum -q -y install git

if [[ ! -f /usr/bin/wp ]]; then

echo ""
if [ -s /usr/bin/wp ]; then
  echo "/usr/bin/wp [found]"
  else
  echo "Error: /usr/bin/wp not found !!! Download now......"
  wget -cnv --no-check-certificate https://raw.github.com/wp-cli/builds/gh-pages/phar/wp-cli.phar -O /usr/bin/wp --tries=3 
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
  echo "Error: ${WPCLIDIR}/wp-completion.bash not found !!! Download now......"
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

###############################################

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
*)
echo "$0 install"
;;
esac

exit