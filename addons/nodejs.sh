#!/bin/bash
VER='0.0.7'
######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
######################################################
# node.js installer
# for Centminmod.com
# written by George Liu (eva2000) centminmod.com
######################################################
# switch to nodesource yum repo instead of source compile
# specify version branch so set NODEJSVER to 4, 5, 6, 7 or 8
NODEJSVER='8'
NODEJS_SOURCEINSTALL='y'
NODEJS_REINSTALL='y'

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

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
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

scl_install() {
	# if gcc version is less than 4.7 (407) install scl collection yum repo
	if [[ "$CENTOS_SIX" = '6' ]]; then
		if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
			cecho "install centos-release-scl for newer gcc and g++ versions" $boldgreen
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        yum -y -q install centos-release-scl
      else
        yum -y -q install centos-release-scl --disablerepo=rpmforge
      fi
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          yum -y -q install devtoolset-4-gcc devtoolset-4-gcc-c++ devtoolset-4-binutils
        else
          yum -y -q install devtoolset-4-gcc devtoolset-4-gcc-c++ devtoolset-4-binutils --disablerepo=rpmforge
        fi

			CCTOOLSET=' --gcc-toolchain=/opt/rh/devtoolset-4/root/usr/'
			unset CC
			unset CXX
			# export CC="/opt/rh/devtoolset-4/root/usr/bin/gcc ${CCTOOLSET}"
			# export CXX="/opt/rh/devtoolset-4/root/usr/bin/g++"
			CLANG_CCOPT=""
			export CC="ccache /usr/bin/clang ${CCTOOLSET}${CLANG_CCOPT}"
			export CXX="ccache /usr/bin/clang++ ${CCTOOLSET}${CLANG_CCOPT}"
			export CCACHE_CPP2=yes
			echo ""
		else
			CCTOOLSET=""
		fi
	fi # centos 6 only needed
}

installnodejs_new() {
  if [[ "$(which node >/dev/null 2>&1; echo $?)" != '0' ]]; then
      cd $DIR_TMP
      curl --silent -4 --location https://rpm.nodesource.com/setup_${NODEJSVER}.x | bash -
      yum -y install nodejs --disableplugin=priorities --disablerepo=epel
      time npm install npm@latest -g
  
    echo
    cecho "---------------------------" $boldyellow
    cecho -n "Node.js Version: " $boldgreen
    node -v
    cecho "---------------------------" $boldyellow
    cecho -n "npm Version: " $boldgreen
    npm --version
    cecho "---------------------------" $boldyellow
    echo
    cecho "node.js YUM install completed" $boldgreen
  else
    echo
    cecho "node.js install already detected" $boldgreen
  fi
}

installnodejs() {

# nodesource yum only works on CentOS 7 right now
# https://github.com/nodesource/distributions/issues/128
# https://github.com/nodesource/distributions/blob/master/OLDER_DISTROS.md
if [[ "$CENTOS_SEVEN" = '7' ]]; then
	if [[ "$(which node >/dev/null 2>&1; echo $?)" != '0' ]]; then
    	cd $DIR_TMP
    	curl --silent -4 --location https://rpm.nodesource.com/setup_8.x | bash -
    	yum -y install nodejs --disableplugin=priorities --disablerepo=epel
    	npm install npm@latest -g
	
		echo
		cecho "---------------------------" $boldyellow
		cecho -n "Node.js Version: " $boldgreen
		node -v
		cecho "---------------------------" $boldyellow
		cecho -n "npm Version: " $boldgreen
		npm --version
		cecho "---------------------------" $boldyellow
		echo
		cecho "node.js source install completed" $boldgreen
	else
		echo
		cecho "node.js install already detected" $boldgreen
	fi
elif [[ "$CENTOS_SIX" = '6' ]]; then
	echo
	cecho "--------------------------------------------------------------------" $boldyellow
	cecho "CentOS 6.x detected... " $boldgreen
	cecho "nodesource YUM install currently only works on CentOS 7.x systems" $boldgreen
	cecho "alternative is to compile node.js from source instead" $boldgreen
	cecho "due to devtoolset-4 & source compilation method it may" $boldgreen
	cecho "take between 10-45 minutes to compile depending on system" $boldgreen
	cecho "--------------------------------------------------------------------" $boldyellow
	echo
	read -ep "Do you want to continue with node.js source install ? [y/n]: " nodecontinue
	echo
	if [[ "$nodecontinue" = [yY] && "$NODEJS_SOURCEINSTALL" = [yY] ]]; then
		if [[ "$(which node >/dev/null 2>&1; echo $?)" != '0' || "$NODEJS_REINSTALL" = [yY] ]]; then
	
			if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
				scl_install
			elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
				CCTOOLSET=' --gcc-toolchain=/opt/rh/devtoolset-6/root/usr/'
				unset CC
				unset CXX
				CLANG_CCOPT=""
				export CC="ccache /usr/bin/clang ${CCTOOLSET}${CLANG_CCOPT}"
				export CXX="ccache /usr/bin/clang++ ${CCTOOLSET}${CLANG_CCOPT}"
				export CCACHE_CPP2=yes
				echo ""
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        CCTOOLSET=' --gcc-toolchain=/opt/rh/devtoolset-4/root/usr/'
        unset CC
        unset CXX
        CLANG_CCOPT=""
        export CC="ccache /usr/bin/clang ${CCTOOLSET}${CLANG_CCOPT}"
        export CXX="ccache /usr/bin/clang++ ${CCTOOLSET}${CLANG_CCOPT}"
        export CCACHE_CPP2=yes
        echo ""
			fi
		
    		cd $DIR_TMP
		
        		cecho "Download node-v${NODEJSVER}.tar.gz ..." $boldyellow
    		if [ -s node-v${NODEJSVER}.tar.gz ]; then
        		cecho "node-v${NODEJSVER}.tar.gz Archive found, skipping download..." $boldgreen
    		else
        		wget -c${ipv_forceopt} --progress=bar https://nodejs.org/dist/v${NODEJSVER}/node-v${NODEJSVER}.tar.gz --tries=3 
		ERROR=$?
			if [[ "$ERROR" != '0' ]]; then
			cecho "Error: node-v${NODEJSVER}.tar.gz download failed." $boldgreen
		checklogdetails
			exit #$ERROR
		else 
         		cecho "Download done." $boldyellow
		#echo ""
			fi
    		fi
		
			tar xzf node-v${NODEJSVER}.tar.gz 
			ERROR=$?
			if [[ "$ERROR" != '0' ]]; then
			cecho "Error: node-v${NODEJSVER}.tar.gz extraction failed." $boldgreen
		checklogdetails
			exit #$ERROR
		else 
         		cecho "node-v${NODEJSVER}.tar.gz valid file." $boldyellow
				echo ""
			fi
		
			cd node-v${NODEJSVER}
			make clean
			./configure
			make${MAKETHREADS}
			make install
			make doc
    	npm install npm@latest -g
		
			echo
			cecho "---------------------------" $boldyellow
			cecho -n "Node.js Version: " $boldgreen
			node -v
			cecho "---------------------------" $boldyellow
			cecho -n "npm Version: " $boldgreen
			npm --version
			cecho "---------------------------" $boldyellow
			echo
			cecho "node.js source install completed" $boldgreen
		else
			echo
			cecho "node.js install already detected" $boldgreen
		fi
	else
		if [[ "$NODEJS_SOURCEINSTALL" != [yY] ]]; then
			echo
			cecho "NODEJS_SOURCEINSTALL=n is set" $boldgreen
			cecho "exiting..." $boldgreen
			exit			
		else	
			echo
			cecho "exiting..." $boldgreen
			exit
		fi
	fi # nodecontinue
fi

}

###########################################################################
case $1 in
	install)
starttime=$(TZ=UTC date +%s.%N)
{
		# preyum
		installnodejs_new
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log
echo "Total Node.js Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_nodejs_install_${DT}.log
	;;
	*)
		echo "$0 install"
	;;
esac
exit