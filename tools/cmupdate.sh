#!/bin/bash
######################################################
# centmin mod updater for .08 beta and higher
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`

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
######################################################
# functions
#############
updatecm() {
    echo "*************************************************"
    cecho "* Update Centmin Mod Source Code Only" $boldgreen
    echo "*************************************************"
    echo
	branchname=123.08centos7beta02
	rm -rf /usr/local/src/${branchname}.zip
	rm -rf /usr/local/src/centminmod-${branchname}
    sed -i "s|\/usr\/local\/src\/centmin-v1.2.3mod|\/usr\/local\/src\/centminmod-${branchname}|g" /root/.bashrc
    sed -i "s|\/usr\/local\/src\/centminmod-123.08centos7beta01|\/usr\/local\/src\/centminmod-${branchname}|g" /root/.bashrc
    sed -i "s|\/usr\/local\/src\/centminmod-123.08centos7beta02|\/usr\/local\/src\/centminmod-${branchname}|g" /root/.bashrc
	wget -cnv --no-check-certificate -O /usr/local/src/${branchname}.zip https://github.com/centminmod/centminmod/archive/${branchname}.zip
	cd /usr/local/src
	unzip ${branchname}.zip
	cd centminmod-${branchname}
    chmod +x addons/*.sh
    chmod +x tools/*.sh    
	chmod +x centmin.sh
    echo
    echo "Exit SSH session - re-login to SSH to complete update"
    sleep 3
    exit
	echo
    echo "*************************************************"
    cecho "* Centmin Mod Source Code Only Updated" $boldgreen
    echo "*************************************************"
}

######################################################

updatecm