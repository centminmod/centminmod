#!/bin/bash
##################################################################
# letsencrypt client standalone installer for centminmod.com
# just installs the letsencrypt client itself for initial setup
##################################################################
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

##################################################################
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi
##################################################################
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

##################################################################

lememstats() {
	echo
	cecho "----------------------------------------------------" $boldyellow
	cecho "system memory profile: " $boldgreen
	cecho "----------------------------------------------------" $boldyellow
	free -ml
}

getuseragent() {
	# build Centmin Mod's identifying letsencrypt user agent
	# --user-agent=
	if [[ "$CENTOS_SIX" = '6' ]]; then
		LE_OSVER=centos6
	elif [[ "$CENTOS_SEVEN" = '7' ]]; then
		LE_OSVER=centos7
	fi
	LE_USERAGENT="centminmod-$LE_OSVER-webroot"
}

python_setup() {
	if [ -f /usr/local/src/centminmod/addons/python27_install.sh ]; then
		if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/python2.7 ]]; then
			/usr/local/src/centminmod/addons/python27_install.sh install
		fi
	fi
}

leclientsetup() {
	# build letsencrypt version timestamp
	# find last github commit date to compare with current client version number
	if [ -d /root/tools/letsencrypt ]; then
		LECOMMIT_DATE=$(cd /root/tools/letsencrypt; date -d @$(git log -n1 --format="%at") +%Y%m%d)
	fi
	# setup letsencrypt client and virtualenv
	# https://community.centminmod.com/posts/19914/
	lememstats
	echo
	cecho "installing or updating letsencrypt client" $boldgreen
	echo
	python_setup
	echo
	mkdir -p /root/tools
	cd /root/tools
	if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
		# compare current letsencrypt version timestamp with last github commit date YYMMDD
		LE_CLIENTVER=$(/root/.local/share/letsencrypt/bin/letsencrypt --version 2>&1 | awk '{print $2}')
		LE_CLIENTCOMPARE=$(echo $LE_CLIENTVER | grep $LECOMMIT_DATE)
		if [[ "$LE_CLIENTCOMPARE" ]]; then
			cd letsencrypt
			git pull
		else
			rm -rf /root/tools/letsencrypt
			git clone https://github.com/letsencrypt/letsencrypt
			cd letsencrypt
		fi
	elif [ ! -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
		git clone https://github.com/letsencrypt/letsencrypt
		cd letsencrypt
	fi
		
	if [[ "$CENTOS_SIX" = '6' && -f /usr/bin/python2.7 ]]; then
		sed -i "s|--python python2|--python python2.7|" letsencrypt-auto
	fi
	# staging endpoint
	# LE_SERVER='https://acme-staging.api.letsencrypt.org/directory'
	# live and beta invitee trusted cert endpoint
	LE_SERVER='https://acme-v01.api.letsencrypt.org/directory'
	if [ -f ./letsencrypt-auto ]; then
		./letsencrypt-auto --server $LE_SERVER
	else
		cecho "./letsencrypt-auto not found" $boldgreen
	fi

	if [ ! -f /etc/letsencrypt/webroot.ini ]; then
	cecho "setup general /etc/letsencrypt/webroot.ini letsencrypt config file" $boldgreen
	touch /etc/letsencrypt/webroot.ini
cat > "/etc/letsencrypt/webroot.ini" <<EOF
# webroot.ini general config ini

rsa-key-size = 2048

# Always use the staging/testing server
#server = https://acme-staging.api.letsencrypt.org/directory

# for beta invitees
server = https://acme-v01.api.letsencrypt.org/directory

# Uncomment and update to register with the specified e-mail address
email = foo@example.com

# Uncomment to use a text interface instead of ncurses
text = True
agree-tos = True
#agree-dev-preview = True
renew-by-default = True

authenticator = webroot
EOF
	fi

	if [[ "$(grep 'foo@example.com' /etc/letsencrypt/webroot.ini)" ]]; then
		echo
		cecho "Registering an account with Letsencrypt" $boldgreen
		echo "You only do this once, so that Letsencrypt can notify &"
		echo "contact you via email regarding your SSL certificates"
		read -ep "Enter your email address to setup Letsencrypt account: " letemail

		if [ -z "$letemail" ]; then
			echo
			echo "!! Error: email address is empty"
		else
			echo
			echo "You are registering $letemail address for Letsencrypt"
		fi

		# check email domain has MX records which letsencrypt client checks for
		CHECKLE_MXEMAIL=$(echo "$letemail" | awk -F '@' '{print $2}')
		while [[ -z "$(dig -t MX +short @8.8.8.8 $CHECKLE_MXEMAIL)" || -z "$letemail" ]]; do
			echo
			if [[ -z "$(dig -t MX +short @8.8.8.8 $CHECKLE_MXEMAIL)" ]]; then
				echo "!! Error: $letemail does not have a DNS MX record !!"
			fi
			if [ -z "$letemail" ]; then
				echo "!! Error: email address is empty"
			fi
			echo
			read -ep "Re-Enter your email address to setup Letsencrypt account: " letemail
			if [ -z "$letemail" ]; then
				echo
				echo "!! Error: email address is empty"
			else
				echo
				echo "You are registering $letemail address for Letsencrypt"
			fi
			CHECKLE_MXEMAIL=$(echo "$letemail" | awk -F '@' '{print $2}')
		done

		sed -i "s|foo@example.com|$letemail|" /etc/letsencrypt/webroot.ini
		echo
	fi

if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
	lememstats
	echo
	cecho "----------------------------------------------------" $boldyellow
	cecho "letsencrypt client is installed at:" $boldgreen
	cecho "/root/.local/share/letsencrypt/bin/letsencrypt" $boldgreen
	cecho "----------------------------------------------------" $boldyellow	
	echo
fi

}

##################################################################
starttime=$(date +%s.%N)
{
leclientsetup
} 2>&1 | tee ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log
echo "Letsencrypt Addon Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/letsencrypt-addon-install_${DT}.log