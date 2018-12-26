#!/bin/bash
VER='0.1.0'
#####################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
######################################################
# ruby, rubygem, rails and passenger installer
# for Centminmod.com
# written by George Liu (eva2000) centminmod.com
######################################################
RUBYVER='2.6.0'
RUBYBUILD=''

# switch to nodesource yum repo instead of source compile
NODEJSVER='8'

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

installnodejs() {

# nodesource yum only works on CentOS 7 right now
# https://github.com/nodesource/distributions/issues/128
if [[ "$CENTOS_SEVEN" = '7' ]]; then
  if [[ "$(which node >/dev/null 2>&1; echo $?)" != '0' ]]; then
  
      cd $DIR_TMP
      curl --silent -4 --location https://rpm.nodesource.com/setup_4.x | bash -
      yum -y install nodejs --disableplugin=priorities --disablerepo=epel
      npm install npm@latest -g
  
  # npm install forever -g
  # https://github.com/Unitech/pm2/issues/232
  # https://github.com/arunoda/node-usage/issues/19
  # npm install pm2@latest -g --unsafe-perm
  
  echo -n "Node.js Version: "
  node -v
  echo -n "npm Version: "
  npm --version
  # echo -n "forver Version: "
  # forever -v
  # echo -n "pm2 Version: "
  # pm2 -V
  else
    echo "node.js install already detected"
  fi
elif [[ "$CENTOS_SIX" = '6' ]]; then
  echo
  echo "CentOS 6.x detected... "
  echo "addons/nodejs.sh currently only works on CentOS 7.x systems"
  # exit
fi
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

installruby() {

if [[ -z $(which ruby >/dev/null 2>&1) || -z $(which rvm >/dev/null 2>&1) || -z $(which gem >/dev/null 2>&1) ]]; then

  groupadd rvm
  usermod -a -G rvm root
  
  echo "curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -"
  curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -
  echo "curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -"
  curl -sSL https://rvm.io/pkuczynski.asc | gpg2 --import -
  \curl -L https://get.rvm.io | bash -s stable
  # \curl -L https://get.rvm.io | bash -s stable --ruby
  # \curl -L https://get.rvm.io | bash -s stable --rails
  
  source /etc/profile.d/rvm.sh

  # export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin:/usr/local/rvm/bin"

  # export PATH="$PATH:/usr/local/rvm/bin"
  
  # echo '[[ -s "/etc/profile.d/rvm.sh" ]] && source "/etc/profile.d/rvm.sh"  # This loads RVM into a shell session.' >> ~/.bash_profile

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
  echo "gem install rake rails sqlite3 mysql bundler --no-ri --no-rdoc"
  echo "gem install passenger --no-ri --no-rdoc"
  
  echo "--------------------------------"
  # RUBYVER=$(rvm list | awk -F " " '/^\=\*/ {print $2}' | awk -F "-" '{print $2}')
  rvm install ${RUBYVER}
  echo "--------------------------------"
  rvm use ${RUBYVER} --default
  echo "--------------------------------"

  echo "PATH echo..."
  # sed -i 's/export PATH/#export PATH/' ~/.bashrc

  # PATH=$(echo $PATH | tr ':' '\n' | sort | uniq | tr '\n' ':')

  # echo "export PATH=\"$PATH:/usr/local/rvm/gems/ruby-${RUBYVER}/bin:/usr/local/rvm/gems/ruby-${RUBYVER}@global/bin:/usr/local/rvm/rubies/ruby-${RUBYVER}/bin\"" >> ~/.bashrc
  # export PATH="$PATH:/usr/local/rvm/gems/ruby-${RUBYVER}/bin:/usr/local/rvm/gems/ruby-${RUBYVER}@global/bin:/usr/local/rvm/rubies/ruby-${RUBYVER}/bin"

  # echo "export PATH="$PATH"" >> ~/.bashrc
  # export PATH="$PATH"

  echo "--------------------------------"
  rvm rubygems current
  echo "--------------------------------"
  gem env
  echo "--------------------------------"
  gem install rake rails sqlite3 mysql --no-ri --no-rdoc
  gem install passenger --no-ri --no-rdoc
  echo "--------------------------------"
  
  echo "more checks..."
  echo "--------------------------------"
  ruby -v
  echo "--------------------------------"
  rails --version
  echo "--------------------------------"
  passenger -v | head -n1
  echo "--------------------------------"
  # passenger-memory-stats
  passenger-memory-stats | sed -r "s/\x1B\[([0-9]{1,3}((;[0-9]{1,3})*)?)?[m|K]//g"
  echo "--------------------------------"
  # passenger-status
  echo "--------------------------------"
  gem list
  echo "--------------------------------"
else
  echo "ruby or rvm or gem install already detected"
fi

} # installruby

nginxruby() {

if [[ -z $(which passenger-config >/dev/null 2>&1) || -f /usr/local/nginx/conf/passenger.conf ]]; then

  PASSENGERROOT=$(passenger-config --root | head -n1)

  echo "-------------------------------------------"
  echo "Setup /usr/local/nginx/conf/passenger.conf"
  echo "-------------------------------------------"

  echo "Passenger root located at: $PASSENGERROOT"

cat > "/usr/local/nginx/conf/passenger.conf" <<END
#passenger_root $PASSENGERROOT;
#passenger_ruby /usr/local/rvm/bin/ruby;
#passenger_max_pool_size 4;
END

  # Check that passenger.conf is included in nginx.con if not detected
  PASSENGERCHECK=$(grep '/usr/local/nginx/conf/passenger.conf' /usr/local/nginx/conf/nginx.conf)

  if [[ -z "$PASSENGERCHECK" ]]; then
    sed -i 's/http {/http { \n#include \/usr\/local\/nginx\/conf\/passenger.conf;/g' /usr/local/nginx/conf/nginx.conf
  fi
  cecho "-------------------------------------------" $boldgreen
  cecho "Setup completed..." $boldyellow
  cecho "-------------------------------------------" $boldgreen
  echo ""
  echo "Log out and log back into your SSH session"
  echo "to complete the next setup steps bellow"
  echo "" 
  echo "Instructions also at https://community.centminmod.com/threads/3282"

  echo ""
  echo "Uncomment lines in /usr/local/nginx/conf/passenger.conf to enable passenger"
  echo "Nginx needs to have passenger nginx module compiled for it to work"
  echo ""
  echo " 1. set NGINX_PASSENGER=y in persistent config file /etc/centminmod/custom_config.inc (create file if missing)"
  echo " 2. run centmin.sh menu option 4 to recompile Nginx"
  echo " 3. uncomment/enable /usr/local/nginx/conf/passenger.conf include file in nginx.conf"
  echo " 4. then check that passenger module is in list of nginx modules via command: "
  echo ""
  echo " nginx -V"
  echo ""
  # sed -i 's/#passenger_/passenger_/g' /usr/local/nginx/conf/passenger.conf
  echo ""
  echo "This script only installs passenger, node.js, ruby, rails, rubygem and is provided as is."
  echo "See Phusion Passenger documentation at for deployment and configuration at:"
  echo "* http://www.modrails.com/documentation/Users%20guide%20Nginx.html"
  echo "* https://github.com/phusion/passenger/wiki/Phusion-Passenger%3A-Node.js-tutorial"
  echo "* https://github.com/phusion/passenger/wiki"

  echo ""
  echo "Log out and log back into your SSH session"
  echo "to complete the next setup steps above"
  echo "" 
  echo "Instructions also at https://community.centminmod.com/threads/3282" 
  echo ""   
else
  echo "Passenger install already detected"
fi
}

###########################################################################
case $1 in
  install)
starttime=$(TZ=UTC date +%s.%N)
{
    preyum
    installnodejs_new
    installruby
    nginxruby
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_passenger_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_passenger_install_${DT}.log
echo "Total Phusion Passenger Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_passenger_install_${DT}.log
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit