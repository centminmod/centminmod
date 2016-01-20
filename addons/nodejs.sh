#!/bin/bash
VER='0.0.1'
######################################################
# node.js installer
# for Centminmod.com
# written by George Liu (eva2000) vbtechsupport.com
######################################################
# switch to nodesource yum repo instead of source compile
NODEJSVER='4.2.4'

DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
######################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}

###########################################

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=`grep "processor" /proc/cpuinfo |wc -l`
    CPUS=$(echo $CPUS+1 | bc)
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=`grep "processor" /proc/cpuinfo |wc -l`
    CPUS=$(echo $CPUS+1 | bc)
    MAKETHREADS=" -j$CPUS"
fi

preyum() {
	if [[ ! -d /svr-setup ]]; then
		yum -y install gcc-c++ patch readline readline-devel zlib zlib-devel libyaml-devel libffi-devel make bzip2 autoconf automake libtool bison iconv-devel sqlite-devel openssl-devel
	elif [[ -z "$(rpm -ql libffi-devel)" || -z "$(rpm -ql libyaml-devel)" || -z "$(rpm -ql sqlite-devel)" ]]; then
		yum -y install libffi-devel libyaml-devel sqlite-devel
	fi

	mkdir -p /home/.ccache/tmp
}

installnodejs() {

if [[ "$(which node >/dev/null 2>&1; echo $?)" != '0' ]]; then
    cd $DIR_TMP
    curl --silent --location https://rpm.nodesource.com/setup_4.x | bash -
    yum -y install nodejs --disableplugin=priorities
    npm install npm@latest -g

	echo -n "Node.js Version: "
	node -v
	echo -n "npm Version: "
	npm --version
else
	echo "node.js install already detected"
fi

}

###########################################################################
case $1 in
	install)
starttime=$(date +%s.%N)
{
		# preyum
		installnodejs
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log
echo "Total Node.js Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log
	;;
	*)
		echo "$0 install"
	;;
esac
exit