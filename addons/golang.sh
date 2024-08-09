#!/bin/bash
VER='0.1.1'
######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
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
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
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

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  fi
fi

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

go_install() {
	cd $DIR_TMP
  GO_VERSION=$(curl -${ipv_forceopt}s https://go.dev/dl/ | egrep -o "go[0-9.]+\.linux\-amd64\.tar[.a-z]*" | head -n1 | sed -e 's|.linux-amd64.tar.gz||' -e 's|go||')
		
  cecho "Download go${GO_VERSION}.linux-${GOARCH}.tar.gz ..." $boldyellow
  if [ -s go${GO_VERSION}.linux-${GOARCH}.tar.gz ]; then
  	cecho "go${GO_VERSION}.linux-${GOARCH}.tar.gz Archive found, skipping download..." $boldgreen
  else
  	wget --progress=bar https://dl.google.com/go/go${GO_VERSION}.linux-${GOARCH}.tar.gz --tries=3 
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
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ ! -d $HOME/golang/packages || ! "$(grep 'GOPATH' $HOME/.bashrc)" ]] && [ -f /usr/local/go/bin/go ]; then
      cecho "---------------------------" $boldyellow
      cecho "$HOME/.bashrc before update: " $boldwhite
      cat $HOME/.bashrc
      cecho "---------------------------" $boldyellow
      mkdir -p $HOME/golang/packages
      export GOPATH=$HOME/golang/packages
      export PATH=$PATH:/usr/local/go/bin
      export PATH=$GOPATH/bin:$PATH
      if [[ ! "$(grep 'golang' $HOME/.bashrc)" ]]; then
        echo "export PATH=\$PATH:/usr/local/go/bin" >> $HOME/.bashrc
        echo "export GOPATH=~/golang/packages" >> $HOME/.bashrc
        echo "export PATH=\$GOPATH/bin:\$PATH" >> $HOME/.bashrc
        . $HOME/.bashrc
        cecho "---------------------------" $boldyellow
        cecho "$HOME/.bashrc after update: " $boldwhite
        cat $HOME/.bashrc
        cecho "---------------------------" $boldyellow
      fi
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