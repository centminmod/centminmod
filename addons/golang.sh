#!/bin/bash
VER='0.1.1'
######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
######################################################
# golang binary installer
# for Centminmod.com
# written by George Liu (eva2000) centminmod.com
######################################################
GO_VERSION='1.10.3'

DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
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

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

if [[ "$(uname -m)" = 'x86_64' ]]; then
  GOARCH='amd64'
else
  GOARCH='386'
fi

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi

go_install() {
	cd $DIR_TMP
  GO_VERSION=$(curl -${ipv_forceopt}s https://golang.org/dl/ | egrep -o "go[0-9.]+\.linux\-amd64\.tar[.a-z]*" | head -n1 | sed -e 's|.linux-amd64.tar.gz||' -e 's|go||')
		
  cecho "Download go${GO_VERSION}.linux-${GOARCH}.tar.gz ..." $boldyellow
  if [ -s go${GO_VERSION}.linux-${GOARCH}.tar.gz ]; then
  	cecho "go${GO_VERSION}.linux-${GOARCH}.tar.gz Archive found, skipping download..." $boldgreen
  else
  	wget -c${ipv_forceopt} --progress=bar https://dl.google.com/go/go${GO_VERSION}.linux-${GOARCH}.tar.gz --tries=3 
	ERROR=$?
		if [[ "$ERROR" != '0' ]]; then
			cecho "Error: go${GO_VERSION}.linux-${GOARCH}.tar.gz download failed." $boldgreen
			checklogdetails
			exit #$ERROR
		else 
  	cecho "Download done." $boldyellow
		fi
  fi
		
  rm -rf /usr/local/go
  tar -C /usr/local -xzf go${GO_VERSION}.linux-${GOARCH}.tar.gz
	ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		cecho "Error: go${GO_VERSION}.linux-${GOARCH}.tar.gz extraction failed." $boldgreen
		checklogdetails
		exit #$ERROR
	else
		echo "ls -lah /usr/local/go/"
		ls -lah /usr/local/go/
    cecho "go${GO_VERSION}.linux-${GOARCH}.tar.gz valid file." $boldyellow
		echo ""
	fi
		
	if [[ ! -d /root/golang/packages || ! "$(grep 'GOPATH' /root/.bashrc)" ]] && [ -f /usr/local/go/bin/go ]; then
		cecho "---------------------------" $boldyellow
		cecho "/root/.bashrc before update: " $boldwhite
		cat /root/.bashrc
		cecho "---------------------------" $boldyellow
		mkdir -p /root/golang/packages
		export GOPATH=/root/golang/packages
		export PATH=$PATH:/usr/local/go/bin
		export PATH=$GOPATH/bin:$PATH
		if [[ ! "$(grep 'golang' /root/.bashrc)" ]]; then
			echo "export PATH=\$PATH:/usr/local/go/bin" >> /root/.bashrc
			echo "export GOPATH=~/golang/packages" >> /root/.bashrc
			echo "export PATH=\$GOPATH/bin:\$PATH" >> /root/.bashrc
			. /root/.bashrc
			cecho "---------------------------" $boldyellow
			cecho "/root/.bashrc after update: " $boldwhite
			cat /root/.bashrc
			cecho "---------------------------" $boldyellow
		fi
	fi
	echo
	cecho "---------------------------" $boldyellow
	cecho -n "golang Version: " $boldgreen
	go version
	cecho "---------------------------" $boldyellow
}

###########################################################################
case $1 in
	install)
starttime=$(TZ=UTC date +%s.%N)
{
		go_install
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_goinstall_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_goinstall_${DT}.log
echo "Total golang Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_goinstall_${DT}.log
	;;
	*)
		echo "$0 install"
	;;
esac
exit