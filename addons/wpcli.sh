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
# set WPCLI_EXTRAPACKAGES='y' in /etc/centminmod/custom_config.inc if you
# want to install wp-sec and wp-check 3rdparty wp-cli packages
WPCLI_EXTRAPACKAGES='n'
CENTMINLOGDIR='/root/centminlogs'
WPCLIDIR='/root/wpcli'
WPCLILINK='https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

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

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

# fallback mirror if official wp-cli download http status is not 200, use local
# centminmod.com mirror download instead
curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $WPCLILINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
WPCLI_CURLCHECK=$?
if [[ "$WPCLI_CURLCHECK" != '0' ]]; then
	WPCLILINK='https://centminmod.com/centminmodparts/wp-cli/wp-cli.phar'
fi

# functions
updatewpcli() {
  TOTALMEM_T=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  TOTALMEM_SWAP=$(awk '/SwapFree/ {print $2}' /proc/meminfo)
  TOTALMEM_PHP=$(($TOTALMEM_T+$TOTALMEM_SWAP))
	if [ -f /usr/bin/wp ]; then

  WPALIASCHECK=$(grep 'allow-root' /root/.bashrc)
  
  if [[ -z "$WPALIASCHECK" ]]; then
    echo "alias wp='wp --allow-root'" >> /root/.bashrc
  fi
  
  if [[ "$WPCLI_EXTRAPACKAGES" = [yY] && "$TOTALMEM_PHP" -gt '2000000' ]]; then
    echo "current memory_limit: $(php -r "echo ini_get('memory_limit').PHP_EOL;")"
    if [[ "$(wp package list --allow-root | grep -q 'eriktorsner\/wp-checksum'; echo $?)" -ne '0' ]]; then
      echo "-------------------------------------------------------------"
      echo "install wp-cli https://github.com/eriktorsner/wp-checksum"
      /usr/local/bin/php -d memory_limit=512M /usr/bin/wp package install eriktorsner/wp-checksum --allow-root
    fi
    if [[ "$(wp package list --allow-root | grep -q 'markri\/wp-sec'; echo $?)" -ne '0' ]]; then
      echo "-------------------------------------------------------------"
      echo "install wp-cli https://github.com/markri/wp-sec"
      /usr/local/bin/php -d memory_limit=512M /usr/bin/wp package install markri/wp-sec --allow-root
    fi
  fi
  echo "-------------------------------------------------------------"
  echo "update wp-cli packages"
  /usr/bin/wp package update --allow-root
  echo "-------------------------------------------------------------"

		echo "update wp-cli"
		rm -rf /usr/bin/wp
		wget -${ipv_forceopt}cnv --no-check-certificate $WPCLILINK -O /usr/bin/wp --tries=3
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
  wget -${ipv_forceopt}cnv --no-check-certificate $WPCLILINK -O /usr/bin/wp --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	echo "Error: /usr/bin/wp download failed."
	exit $ERROR
else 
         echo "Download done."
	fi
fi

if [ -f /usr/bin/wp ]; then
  chmod 0700 /usr/bin/wp
fi

echo ""
if [ -s "${WPCLIDIR}/wp-completion.bash" ]; then
  echo "${WPCLIDIR}/wp-completion.bash [found]"
  else
  echo "Error: ${WPCLIDIR}/wp-completion.bash not found !!! Downloading now......"
  wget -${ipv_forceopt}cnv --no-check-certificate https://github.com/wp-cli/wp-cli/raw/master/utils/wp-completion.bash -O ${WPCLIDIR}/wp-completion.bash --tries=3 
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

WPALIASCHECK=$(grep 'allow-root' /root/.bashrc)

if [[ -z "$WPALIASCHECK" ]]; then
  echo "alias wp='wp --allow-root'" >> /root/.bashrc
fi

if [[ "$(wp package list --allow-root | grep -q 'eriktorsner\/wp-checksum'; echo $?)" -ne '0' ]]; then
  echo "-------------------------------------------------------------"
  echo "install wp-cli https://github.com/eriktorsner/wp-checksum"
  /usr/bin/wp package install eriktorsner/wp-checksum --allow-root
fi
if [[ "$(wp package list --allow-root | grep -q 'markri\/wp-sec'; echo $?)" -ne '0' ]]; then
  echo "-------------------------------------------------------------"
  echo "install wp-cli https://github.com/markri/wp-sec"
  /usr/bin/wp package install markri/wp-sec --allow-root
fi
echo "-------------------------------------------------------------"
echo "update wp-cli packages"
/usr/bin/wp package update --allow-root
echo "-------------------------------------------------------------"
echo "wp-cli info"
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
starttime=$(TZ=UTC date +%s.%N)
{
echo "installing..."
installwpcli
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log
echo "Total WPCLI Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_wpcli_install_${DT}.log
;;
update)
ustarttime=$(TZ=UTC date +%s.%N)
{
echo "updating..."
updatewpcli
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log

uendtime=$(TZ=UTC date +%s.%N)

UINSTALLTIME=$(echo "scale=2;$uendtime - $ustarttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log
echo "Total WPCLI Upgrade Time: $UINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_wpcli_update_${DT}.log
;;
*)
echo "$0 {install|update}"
;;
esac

exit