#!/bin/bash
######################################################
# sets up email admin setting for server notifications
# and other future planned features which require an
# email address of the centmin mod user
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")

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
setupemails() {
	echo
	cecho "--------------------------------------------------------------------" $boldgreen
	cecho "Setup Server Administration Email" $boldyellow
	cecho "Emails will be used for future notification alert features" $boldyellow
	cecho "--------------------------------------------------------------------" $boldgreen
	cecho "Hit Enter To Skip..." $boldyellow
	cecho "Will be prompted everytime run centmin.sh if both emails not entered" $boldyellow
	cecho "--------------------------------------------------------------------" $boldgreen
	read -ep "enter primary email: " mainemail
	read -ep "enter secondary email: " secondemail
	cecho "--------------------------------------------------------------------" $boldgreen

	if [ -z "$mainemail" ]; then
		mainemail=""
		rm -rf /etc/centminmod/email-primary.ini
		cecho "primary email setup skipped..." $boldyellow
	else
		echo
		cecho "Primary: $mainemail" $boldyellow
		echo "$mainemail" > /etc/centminmod/email-primary.ini
		cecho "setup at /etc/centminmod/email-primary.ini" $boldyellow
		echo
		echo -n "  "
		cat /etc/centminmod/email-primary.ini
		echo
	fi
	if [ -z "$secondemail" ]; then
		secondemail=$mainemail
		rm -rf /etc/centminmod/email-secondary.ini
		cecho "secondary email setup skipped..." $boldyellow
	else
		cecho "Secondary: $secondemail" $boldyellow
		echo "$secondemail" > /etc/centminmod/email-secondary.ini
		cecho "setup at /etc/centminmod/email-secondary.ini" $boldyellow
		echo
		echo -n "  "
		cat /etc/centminmod/email-secondary.ini
		echo
	fi
	echo
}

######################################################
setupemails