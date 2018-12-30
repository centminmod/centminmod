#!/bin/bash
VER='0.0.2'
#####################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
######################################################
# ruby, rubygem, rails installer
# for Centminmod.com
# written by George Liu (eva2000) centminmod.com
# https://rvm.io/
######################################################
RUBYVER='2.6.0'
RUBYBUILD=''

DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
CONFIGSCANBASE='/etc/centminmod'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
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

if [ -f "${SCRIPT_DIR}/inc/custom_config.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "inc/custom_config.inc"
  fi
    source "inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
  fi
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

preyum() {
  if [[ ! -d /svr-setup ]]; then
    yum -y install gcc-c++ patch readline readline-devel zlib zlib-devel libyaml-devel libffi-devel make bzip2 autoconf automake libtool bison iconv-devel sqlite-devel openssl-devel
  elif [[ -z "$(rpm -ql libffi-devel)" || -z "$(rpm -ql libyaml-devel)" || -z "$(rpm -ql sqlite-devel)" ]]; then
    yum -y install libffi-devel libyaml-devel sqlite-devel
  fi

  if [[ "$(rpm -ql ruby | grep -v 'not installed')" || "$(rpm -ql ruby-libs | grep -v 'not installed')" || "$(rpm -ql rubygems | grep -v 'not installed')" ]]; then
    yum -y erase ruby ruby-libs ruby-mode rubygems
  fi

  mkdir -p /home/.ccache/tmp
}

installruby() {

if [[ -z $(which ruby >/dev/null 2>&1) || -z $(which rvm >/dev/null 2>&1) || -z $(which gem >/dev/null 2>&1) ]]; then

  groupadd rvm
  usermod -a -G rvm root
  
  echo "curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -"
  curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -
  echo "curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -"
  curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -
  \curl -L https://get.rvm.io | bash -s stable
  
  source /etc/profile.d/rvm.sh

  echo '[[ -s "/etc/profile.d/rvm.sh" ]] && source "/etc/profile.d/rvm.sh"  # This loads RVM into a shell session.' >> ~/.bashrc
  
  echo "checks..."
  echo "--------------------------------"
  echo "export PATH="$PATH""
  export PATH="$PATH"
  echo
  echo $PATH
  echo "--------------------------------"
  rvm requirements
  echo "--------------------------------"
  rvm list
  echo
  rvm list | awk -F " " '/^\=\*/ {print $2}'
  echo "--------------------------------"
  type rvm | head -1
  echo "--------------------------------"
  
  echo "rvm install ${RUBYVER}"
  echo "rvm use ${RUBYVER} --default"
  echo "rvm rubygems current"
  echo "--------------------------------" 
  echo $GEM_HOME
  echo $GEM_PATH
  echo "--------------------------------"
  echo "gem install rake rails sqlite3"
    
  echo "--------------------------------"
  # RUBYVER=$(rvm list | awk -F " " '/^\=\*/ {print $2}' | awk -F "-" '{print $2}')
  rvm install ${RUBYVER}
  echo "--------------------------------"
  rvm use ${RUBYVER} --default
  echo "--------------------------------"

  echo "PATH echo..."
  echo "--------------------------------"
  rvm rubygems current
  echo "--------------------------------"
  gem env
  echo "--------------------------------"
  gem install rake rails sqlite3
  echo "--------------------------------"
  
  echo "more checks..."
  echo "--------------------------------"
  ruby -v
  echo "--------------------------------"
  rails --version
  echo "--------------------------------"
  gem list
  echo "--------------------------------"
else
  echo "ruby or rvm or gem install already detected"
fi

} # installruby

###########################################################################
case $1 in
  install)
starttime=$(TZ=UTC date +%s.%N)
{
    preyum
    installruby
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log
echo "Total Ruby Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_ruby_install_${DT}.log
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit