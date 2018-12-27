#!/bin/bash
#######################################################
# centminmod.com cli installer
#
#######################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
#######################################################
DT=$(date +"%d%m%y-%H%M%S")
DNF_ENABLE='n'
DNF_COPR='y'
branchname=123.09beta01
DOWNLOAD="${branchname}.zip"
LOCALCENTMINMOD_MIRROR='https://centminmod.com'

FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
INSTALLDIR='/usr/local/src'
DIR_TMP='/svr-setup'
#CUR_DIR="/usr/local/src/centminmod-${branchname}"
#CM_INSTALLDIR=$CUR_DIR
#SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
#####################################################
# Centmin Mod Git Repo URL - primary repo
# https://github.com/centminmod/centminmod
GITINSTALLED='y'
CMGIT='https://github.com/centminmod/centminmod.git'
# Gitlab backup repo 
# https://gitlab.com/centminmod/centminmod
#CMGIT='https://gitlab.com/centminmod/centminmod.git'
#####################################################
# wget renamed github
AXEL='n'
AXEL_VER='2.6'
AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

#######################################################
ALTPCRE_VERSION='8.42'
ALTPCRELINKFILE="pcre-${ALTPCRE_VERSION}.tar.gz"
ALTPCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${ALTPCRELINKFILE}"

WGET_VERSION='1.20.1'
WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
WGET_LINK="https://centminmod.com/centminmodparts/wget/${WGET_FILENAME}"

CPUSPEED=$(awk -F: '/cpu MHz/{print $2}' /proc/cpuinfo | sort | uniq -c | sed -e s'|      ||g' | xargs); 
CPUMODEL=$(awk -F: '/model name/{print $2}' /proc/cpuinfo | sort | uniq -c | xargs);
###########################################################
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

###########################################################

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
CENTMINLOGDIR='/root/centminlogs'
if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p $CENTMINLOGDIR
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

if [[ "$CENTOS_SEVEN" = '7' ]]; then
  AXEL_VER='2.16.1'
  AXEL_LINKFILE="axel-${AXEL_VER}.tar.gz"
  AXEL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/axel/v${AXEL_VER}.tar.gz"
  AXEL_LINKLOCAL="https://github.com/axel-download-accelerator/axel/archive/v${AXEL_VER}.tar.gz"

fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi

if [[ -f /usr/bin/systemd-detect-virt && "$(/usr/bin/systemd-detect-virt)" = 'lxc' ]] || [[ -f $(which virt-what) && $(virt-what | head -n1) = 'lxc' ]]; then
  CHECK_LXD='y'
fi

if [[ "$(uname -m)" = 'x86_64' ]]; then
  if [ ! "$(grep -w 'exclude' /etc/yum.conf)" ]; then
ex -s /etc/yum.conf << EOF
:/plugins=1/
:a
exclude=*.i386 *.i586 *.i686
.
:w
:q
EOF
  fi
fi

# some centos images don't even install tar by default !
if [[ "$CENTOS_SEVEN" = '7' && ! -f /usr/bin/tar ]]; then
  yum -y -q install tar
elif [[ "$CENTOS_SIX" = '6' && ! -f /bin/tar ]]; then
  yum -y -q install tar
fi

if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
  if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
    yum -y -q install epel-release
    yum clean all
  fi

  if [[ "$DNF_COPR" = [yY] ]]; then
cat > "/etc/yum.repos.d/dnf-centos.repo" <<EOF
[dnf-centos]
name=Copr repo for dnf-centos owned by @rpm-software-management
baseurl=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/pubkey.gpg
enabled=1
enabled_metadata=1
EOF
  fi
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
    dnf clean all
  fi
  if [ ! "$(grep -w 'exclude' /etc/dnf/dnf.conf)" ]; then
    echo "excludepkgs=*.i386 *.i586 *.i686" >> /etc/dnf/dnf.conf
  fi
  if [ ! "$(grep -w 'fastestmirror=true' /etc/dnf/dnf.conf)" ]; then
    echo "fastestmirror=true" >> /etc/dnf/dnf.conf
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEREPO_DNF=' --disablerepo=rpmforge'
  else
    DISABLEREPO_DNF=""
  fi
fi

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
elif [ -f /usr/bin/sar ]; then
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
fi

if [ -f /proc/user_beancounters ]; then
    echo "OpenVZ system detected, NTP not installed"
elif [[ "$CHECK_LXD" = [yY] ]]; then
    echo "LXC/LXD container system detected, NTP not installed"
else
  if [ ! -f /usr/sbin/ntpd ]; then
    echo "*************************************************"
    echo "* Installing NTP (and syncing time)"
    echo "*************************************************"
    echo "The date/time before was:"
    date
    echo
    time $YUMDNFBIN -y install ntp
    chkconfig ntpd on
    if [ -f /etc/ntp.conf ]; then
    if [[ -z "$(grep 'logfile' /etc/ntp.conf)" ]]; then
        echo "logfile /var/log/ntpd.log" >> /etc/ntp.conf
        ls -lahrt /var/log | grep 'ntpd.log'
    fi
    echo "current ntp servers"
    NTPSERVERS=$(awk '/server / {print $2}' /etc/ntp.conf | grep ntp.org | sort -r)
    for s in $NTPSERVERS; do
      if [ -f /usr/bin/nc ]; then
        echo -ne "\n$s test connectivity: "
        if [[ "$(echo | nc -u -w1 $s 53 >/dev/null 2>&1 ;echo $?)" = '0' ]]; then
        echo " ok"
        else
        echo " error"
        fi
      fi
        ntpdate -q $s | tail -1
        if [[ -f /etc/ntp/step-tickers && -z "$(grep $s /etc/ntp/step-tickers )" ]]; then
        echo "$s" >> /etc/ntp/step-tickers
        fi
    done
    if [ -f /etc/ntp/step-tickers ]; then
        echo -e "\nsetup /etc/ntp/step-tickers server list\n"
        cat /etc/ntp/step-tickers
    fi
    service ntpd restart >/dev/null 2>&1
    echo -e "\ncheck ntpd peers list"
    ntpdc -p
    fi
    echo "The date/time is now:"
    date
  fi
fi

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo ""
    echo "Check for existing mysql-server packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mysql-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo ""
    echo "Check for existing mariadb packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mariadb-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
    echo ""
    echo "Check for existing mariadb-libs package"
    OLDMYSQL_LIBS=`rpm -qa | grep 'mariadb-libs' | head -n1`
    if [[ ! -z "$OLDMYSQL_LIBS" ]]; then
        # echo "rpm -e --nodeps $OLDMYSQL_LIBS"
        # rpm -e --nodeps $OLDMYSQL_LIBS
        echo "yum -y remove mariadb-libs"
        yum -y remove mariadb-libs
    fi
    echo ""
    # Should not exist on CentOS 7 systems
    echo "Check for existing MySQL-shared-compat"
    OLDMYSQL_SHAREDCOMPAT=`rpm -qa | grep 'MySQL-shared-compat' | head -n1`
    if [[ ! -z "$OLDMYSQL_SHAREDCOMPAT" ]]; then
        echo "yum -y remove MySQL-shared-compat"
        yum -y remove MySQL-shared-compat
    fi
fi # CENTOS_SEVEN != 7

sar_call() {
  $SARCALL 1 1
}

systemstats() {
  if [ -d /root/centminlogs ]; then
    sar -u > /root/centminlogs/sar-u-installstats.log
    sar -q > /root/centminlogs/sar-q-installstats.log
    sar -r > /root/centminlogs/sar-r-installstats.log
    if [ ! -f /proc/user_beancounters ]; then
    sar -d > /root/centminlogs/sar-d-installstats.log
    fi
    sar -b > /root/centminlogs/sar-b-installstats.log
    if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
      SERVERHOSTNAME=$(hostname)
    else
      SERVERHOSTNAME=$(hostname -f)
    fi
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-u-installstats.log
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-q-installstats.log
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-r-installstats.log
    if [ ! -f /proc/user_beancounters ]; then
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-d-installstats.log
    fi
    sed -i "s|$SERVERHOSTNAME|hostname|" /root/centminlogs/sar-b-installstats.log
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/x86_64/7/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/x86_64/7/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      fi
    else
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      else
        if [ -f /var/cache/yum/i386/6/timedhosts.txt ]; then
          sort -k2 /var/cache/yum/i386/6/timedhosts.txt > /root/centminlogs/yum-timedhosts.txt
        fi
      fi
    fi
  fi
  if [ -f /etc/cron.d/cmsar ]; then
    rm -rf /etc/cron.d/cmsar
  fi
}

scl_install() {
  # if gcc version is less than 4.7 (407) install scl collection yum repo
  if [[ "$CENTOS_SIX" = '6' ]]; then
    # if devtoolset exists, enable it first before checking gcc versions
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
      fi
    else
      if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
      fi
    fi
    if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      echo "install centos-release-scl for newer gcc and g++ versions"
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl
        fi
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
    fi
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl
        fi
        sar_call
      else
        if [[ "$(rpm -ql centos-release-scl >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
          time $YUMDNFBIN -y -q install centos-release-scl --disablerepo=rpmforge
        fi
        sar_call
      fi
      if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils
          sar_call
        else
          time $YUMDNFBIN -y -q install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils --disablerepo=rpmforge
          sar_call
        fi
        echo
        /opt/rh/devtoolset-6/root/usr/bin/gcc --version
        /opt/rh/devtoolset-6/root/usr/bin/g++ --version
      fi
  fi # centos 6 only needed
}

gccdevtools() {
  if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ || ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    scl_install
    unset CC
    unset CXX
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
    else
      export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
    fi
  elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++" 
  elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-4/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-4/root/usr/bin/g++" 
  fi
}

source_pcreinstall() {
  if [[ "$(/usr/local/bin/pcre-config --version 2>&1 | grep -q ${ALTPCRE_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
  cd "$DIR_TMP"
  cecho "Download $ALTPCRELINKFILE ..." $boldyellow
  if [ -s "$ALTPCRELINKFILE" ]; then
    cecho "$ALTPCRELINKFILE Archive found, skipping download..." $boldgreen
  else
    wget -c${ipv_forceopt} --progress=bar "$ALTPCRELINK" --tries=3 
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: $ALTPCRELINKFILE download failed." $boldgreen
      exit #$ERROR
    else 
      cecho "Download done." $boldyellow
    fi
  fi
  
  tar xzf "$ALTPCRELINKFILE"
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $ALTPCRELINKFILE extraction failed." $boldgreen
    exit #$ERROR
  else 
    cecho "$ALTPCRELINKFILE valid file." $boldyellow
    echo ""
  fi
  cd "pcre-${ALTPCRE_VERSION}"
  ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
  sar_call
  make${MAKETHREADS}
  sar_call
  make install
  sar_call
  /usr/local/bin/pcre-config --version
  fi
}

source_wgetinstall() {
  if [[ "$(/usr/local/bin/wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
  cd "$DIR_TMP"
  cecho "Download $WGET_FILENAME ..." $boldyellow
  if [ -s "$WGET_FILENAME" ]; then
    cecho "$WGET_FILENAME Archive found, skipping download..." $boldgreen
  else
    wget -c${ipv_forceopt} --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3 
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: $WGET_FILENAME download failed." $boldgreen
      exit #$ERROR
    else 
      cecho "Download done." $boldyellow
    fi
  fi
  
  tar xzf "$WGET_FILENAME"
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $WGET_FILENAME extraction failed." $boldgreen
    exit #$ERROR
  else 
    cecho "$WGET_FILENAME valid file." $boldyellow
    echo ""
  fi
  cd "wget-${WGET_VERSION}"
  gccdevtools
  make clean
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    # ensure wget.sh installer utilises system openssl
    export OPENSSL_CFLAGS="-I /usr/include"
    export OPENSSL_LIBS="-L /usr/lib64 -lssl -lcrypto"
  else
    export CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -mtune=generic"
    export PCRE_CFLAGS="-I /usr/local/include"
    export PCRE_LIBS="-L /usr/local/lib -lpcre"
    if [ -f /root/.wgetrc ]; then
      \cp -fp /root/.wgetrc /root/.wgetrc-bak
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
    else
      echo "ca_certificate=/etc/pki/tls/certs/ca-bundle.crt" > /root/.wgetrc
    fi
  fi
  # ./configure --with-ssl=openssl PCRE_CFLAGS="-I /usr/local/include" PCRE_LIBS="-L /usr/local/lib -lpcre"
  ./configure --with-ssl=openssl
  sar_call
  make${MAKETHREADS}
  sar_call
  make install
  sar_call
  echo "/usr/local/lib/" > /etc/ld.so.conf.d/wget.conf
  ldconfig
  if [[ ! "$(grep '^alias wget' /root/.bashrc)" ]]; then
    echo "alias wget='/usr/local/bin/wget'" >> /root/.bashrc
  fi
  . /root/.bashrc

  echo
  cecho "--------------------------------------------------------" $boldgreen
  echo "ldconfig -p | grep libpcre.so.1"
  ldconfig -p | grep libpcre.so.1
  echo
  echo "ldd $(which wget)"
  ldd $(which wget)
  cecho "--------------------------------------------------------" $boldgreen
  cecho "wget -V" $boldyellow
  wget -V
  cecho "--------------------------------------------------------" $boldgreen
  cecho "wget ${WGET_VERSION} installed at /usr/local/bin/wget" $boldyellow
  cecho "--------------------------------------------------------" $boldgreen
  unset CFLAGS
  echo
  fi
}

fileperm_fixes() {
  if [ -f /usr/lib/udev/rules.d/60-net.rules ]; then
    if [[ "$(lsattr /usr/lib/udev/rules.d/60-net.rules | cut -c5)" = 'i' ]]; then
      # fix for some centos 7 vps templates on vps hosts setting chattr +i on
      # /usr/lib/udev/rules.d/60-net.rules preventing yum updates for initscripts
      # yum packages
      chattr -i /usr/lib/udev/rules.d/60-net.rules
    fi
  fi
}

libc_fix() {
  # https://community.centminmod.com/posts/52555/
  if [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" = 'libc-client-2007f-16.el7.x86_64' ]]; then
    yum -y install yum-plugin-versionlock uw-imap-devel
    yum versionlock libc-client uw-imap-devel
  elif [[ "$CENTOS_SEVEN" -eq '7' && ! -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget https://centminmod.com/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget https://centminmod.com/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y remove libc-client
    yum -y localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum -y install yum-plugin-versionlock
    yum versionlock libc-client uw-imap-devel uw-imap-devel
    cd "$INIT_DIR"
   elif [[ "$CENTOS_SEVEN" -eq '7' && -f /etc/yum/pluginconf.d/versionlock.conf && "$(rpm -qa libc-client)" != 'libc-client-2007f-16.el7.x86_64' ]]; then
    INIT_DIR=$(echo $PWD)
    cd /svr-setup
    wget https://centminmod.com/centminmodparts/uw-imap/libc-client-2007f-16.el7.x86_64.rpm
    wget https://centminmod.com/centminmodparts/uw-imap/uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock delete libc-client uw-imap-devel
    yum -y remove libc-client
    yum -y localinstall libc-client-2007f-16.el7.x86_64.rpm uw-imap-devel-2007f-16.el7.x86_64.rpm
    yum versionlock libc-client uw-imap-devel uw-imap-devel
    cd "$INIT_DIR" 
  fi
}

opt_tcp() {
#######################################################
# check if custom open file descriptor limits already exist
    LIMITSCONFCHECK=`grep '* hard nofile 524288' /etc/security/limits.conf`
    if [[ -z $LIMITSCONFCHECK ]]; then
        # Set VPS hard/soft limits
        echo "* soft nofile 524288" >>/etc/security/limits.conf
        echo "* hard nofile 524288" >>/etc/security/limits.conf
# https://community.centminmod.com/posts/52406/
if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/rc.d/rc.local ]]; then


cat > /usr/lib/systemd/system/rc-local.service <<EOF
# This unit gets pulled automatically into multi-user.target by
# systemd-rc-local-generator if /etc/rc.d/rc.local is executable.
[Unit]
Description=/etc/rc.d/rc.local Compatibility
ConditionFileIsExecutable=/etc/rc.d/rc.local
After=network.target

[Service]
Type=forking
ExecStart=/etc/rc.d/rc.local start
TimeoutSec=0
RemainAfterExit=yes
EOF

cat > /etc/rc.d/rc.local <<EOF
#!/bin/bash
# THIS FILE IS ADDED FOR COMPATIBILITY PURPOSES
#
# It is highly advisable to create own systemd services or udev rules
# to run scripts during boot instead of using this file.
#
# In contrast to previous versions due to parallel execution during boot
# this script will NOT be run after all other services.
#
# Please note that you must run 'chmod +x /etc/rc.d/rc.local' to ensure
# that this script will be executed during boot.

touch /var/lock/subsys/local
EOF

# remove non-standard centos 7 service file if detected
if [ -f /etc/systemd/system/rc-local.service ]; then
  echo "cat /etc/systemd/system/rc-local.service"
  cat /etc/systemd/system/rc-local.service
  echo
  rm -rf /etc/systemd/system/rc-local.service
  rm -rf /var/lock/subsys/local
  systemctl daemon-reload
  systemctl stop rc-local.service
fi

  chmod +x /etc/rc.d/rc.local
  pushd /etc; ln -s rc.d/rc.local /etc/rc.local; popd
  systemctl daemon-reload
  systemctl start rc-local.service
  systemctl status rc-local.service
fi
        ulimit -n 524288
        echo "ulimit -n 524288" >> /etc/rc.local
    fi # check if custom open file descriptor limits already exist

    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        # centos 7
        if [[ -f /etc/security/limits.d/20-nproc.conf ]]; then
cat > "/etc/security/limits.d/20-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
      fi
    else
        # centos 6
        if [[ -f /etc/security/limits.d/90-nproc.conf ]]; then
cat > "/etc/security/limits.d/90-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
        fi # raise user process limits
    fi

if [[ ! -f /proc/user_beancounters ]]; then
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        if [ -d /etc/sysctl.d ]; then
            # centos 7
            touch /etc/sysctl.d/101-sysctl.conf
            if [[ "$(grep 'centminmod added' /etc/sysctl.d/101-sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
cat >> "/etc/sysctl.d/101-sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8151
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 0
net.ipv4.tcp_tw_reuse = 0
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_limit_output_bytes=65536
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.nf_conntrack_max = 524288
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024
net.unix.max_dgram_qlen = 4096
EOF
        if [[ "$(grep -o 'AMD EPYC' /proc/cpuinfo | sort -u)" = 'AMD EPYC' ]]; then
          echo "kernel.watchdog_thresh = 20" >> /etc/sysctl.d/101-sysctl.conf
        fi
        /sbin/sysctl --system
            fi           
        fi
    else
        # centos 6
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
            # raise hashsize for conntrack entries
            echo 65536 > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
              echo "echo 65536 > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
cat >> "/etc/sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8151
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 0
net.ipv4.tcp_tw_reuse = 0
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.nf_conntrack_max = 524288
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
net.ipv4.tcp_challenge_ack_limit = 999999999
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024
EOF
sysctl -p
        fi
    fi # centos 6 or 7
fi
}

if [ ! -d "$DIR_TMP" ]; then
  mkdir -p $DIR_TMP
fi

DEF=${1:-novalue}

yum clean all
opt_tcp

if [[ ! -f /usr/bin/git || ! -f /usr/bin/bc || ! -f /usr/bin/wget || ! -f /bin/nano || ! -f /usr/bin/unzip || ! -f /usr/bin/applydeltarpm ]]; then
  firstyuminstallstarttime=$(TZ=UTC date +%s.%N)
  echo
  echo "installing yum packages..."
  echo

  # do not install yum fastmirror plugin if not enough detected system memory available
  # for yum fastmirror operation
  if [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '1018000' && "$CENTOS_SEVEN" = '7' ]]; then
    time $YUMDNFBIN -y install yum-plugin-fastestmirror yum-plugin-security
    sar_call
  elif [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '263000' ]]; then
    time $YUMDNFBIN -y install yum-plugin-fastestmirror yum-plugin-security
    sar_call
  fi

  if [[ -f /etc/machine-info && "$(grep -qi 'OVH bhs' /etc/machine-info; echo $?)" -eq '0' ]]; then
    # detected OVH BHS based server so disable slower babylon network mirror
    # https://community.centminmod.com/posts/47320/
    if [ -f /etc/yum/pluginconf.d/fastestmirror.conf ]; then
      echo "exclude=ca.mirror.babylon.network" >> /etc/yum/pluginconf.d/fastestmirror.conf
      cat /etc/yum/pluginconf.d/fastestmirror.conf
    fi
    # if [[ -f /etc/dnf/dnf.conf && "$(grep -qw 'exclude' /etc/dnf/dnf.conf; echo $?)" -eq '0' ]]; then
    #   echo "exclude=ca.mirror.babylon.network" >> /etc/dnf/dnf.conf
    # fi
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/x86_64/7/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000115046005249|' /var/cache/yum/x86_64/7/timedhosts.txt
        fi
      fi
    else
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        if [ -f /var/cache/yum/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000115046005249|' /var/cache/yum/timedhosts.txt
        fi
      else
        if [ -f /var/cache/yum/i386/6/timedhosts.txt ]; then
          sed -i 's|centos.bhs.mirrors.ovh.net .*|centos.bhs.mirrors.ovh.net 0.000110046005249|' /var/cache/yum/i386/6/timedhosts.txt
        fi
      fi
    fi
  fi

  if [[ "$CENTOS_SEVEN" = '7' ]]; then
    if [[ $(rpm -q nmap-ncat >/dev/null 2>&1; echo $?) != '0' ]]; then
      time $YUMDNFBIN -y install nmap-ncat${DISABLEREPO_DNF}
      sar_call
    fi
  else
    if [[ $(rpm -q nc >/dev/null 2>&1; echo $?) != '0' ]]; then
      time $YUMDNFBIN -y install nc libgcj
      sar_call
    fi
  fi

  # ensure ipset doesn't get caught in autoremove list
  # https://community.centminmod.com/posts/48144/
  if [ -f /proc/user_beancounters ]; then
    USER_PKGS=""
  else
    USER_PKGS=" ipset ipset-devel"
  fi

  time $YUMDNFBIN -y install virt-what python-devel gawk unzip pyOpenSSL python-dateutil libuuid-devel bc wget lynx screen deltarpm ca-certificates yum-utils bash mlocate subversion rsyslog dos2unix boost-program-options net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel  krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop libvpx libvpx-devel t1lib t1lib-devel expect expect-devel readline readline-devel libedit libedit-devel libxslt libxslt-devel openssl openssl-devel curl curl-devel openldap openldap-devel zlib zlib-devel gd gd-devel pcre pcre-devel gettext gettext-devel libidn libidn-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel e2fsprogs e2fsprogs-devel libc-client libc-client-devel cyrus-sasl cyrus-sasl-devel pam pam-devel libaio libaio-devel libevent libevent-devel recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel enchant enchant-devel lua lua-devel mailx perl-LWP-Protocol-https OpenEXR-devel OpenEXR-libs atk cups-libs fftw-libs-double fribidi gdk-pixbuf2 ghostscript-devel ghostscript-fonts gl-manpages graphviz gtk2 hicolor-icon-theme ilmbase ilmbase-devel jasper-devel jasper-libs jbigkit-devel jbigkit-libs lcms2 lcms2-devel libICE-devel libSM-devel libXaw libXcomposite libXcursor libXdamage-devel libXfixes-devel libXfont libXi libXinerama libXmu libXrandr libXt-devel libXxf86vm-devel libdrm-devel libfontenc librsvg2 libtiff libtiff-devel libwebp libwebp-devel libwmf-lite mesa-libGL-devel mesa-libGLU mesa-libGLU-devel poppler-data urw-fonts xorg-x11-font-utils${USER_PKGS}${DISABLEREPO_DNF}
  sar_call
  # allows curl install to skip checking for already installed yum packages 
  # later on in initial curl installations
  touch /tmp/curlinstaller-yum
  time $YUMDNFBIN -y install epel-release${DISABLEREPO_DNF}
  sar_call
  if [[ "$CENTOS_SEVEN" = '7' ]]; then
    time $YUMDNFBIN -y install clang clang-devel jemalloc jemalloc-devel zstd python2-pip libmcrypt libmcrypt-devel libraqm figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 glances bash-completion bash-completion-extras mlocate re2c kernel-headers kernel-devel${DISABLEREPO_DNF} --enablerepo=epel
    libc_fix
    if [ -f /usr/bin/pip ]; then
      pip install --upgrade pip
    fi
    sar_call
  else
    time $YUMDNFBIN -y install clang clang-devel jemalloc jemalloc-devel zstd python-pip libmcrypt libmcrypt-devel libraqm figlet moreutils nghttp2 libnghttp2 libnghttp2-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 libJudy glances bash-completion bash-completion-extras mlocate re2c kernel-headers kernel-devel cmake28 uw-imap-devel${DISABLEREPO_DNF} --enablerepo=epel
    if [ -f /usr/bin/pip ]; then
      pip install --upgrade pip
    fi
    sar_call
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    time $YUMDNFBIN -y install GeoIP GeoIP-devel --disablerepo=rpmforge
    sar_call
  else
    time $YUMDNFBIN -y install GeoIP GeoIP-devel
    sar_call
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    time $YUMDNFBIN -y install centos-release-cr
    sar_call
    sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/CentOS-CR.repo
    # echo "priority=1" >> /etc/yum.repos.d/CentOS-CR.repo
  fi
  touch ${INSTALLDIR}/curlinstall_yum.txt
  firstyuminstallendtime=$(TZ=UTC date +%s.%N)
fi

if [ -f /etc/selinux/config ]; then
  sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
  sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
fi

yumupdater() {
  yum clean all
  time $YUMDNFBIN -y update
  #time $YUMDNFBIN -y install expect imake bind-utils readline readline-devel libedit libedit-devel libxslt libxslt-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip patch sysstat zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel glib2 glib2-devel bzip2 bzip2-devel vim-minimal nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-ExtUtils-MakeMaker perl-Time-HiRes cyrus-sasl cyrus-sasl-devel strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel git
}

install_axel() {
  cd $DIR_TMP
  echo "Download $AXEL_LINKFILE ..."
  if [ -s $AXEL_LINKFILE ]; then
    echo "Axel ${AXEL_VER} Archive found, skipping download..." 
  else
    wget -O $AXEL_LINKFILE $AXEL_LINK
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
     echo "Error: $AXEL_LINKFILE download failed."
      exit #$ERROR
    else 
      echo "Download $AXEL_LINKFILE done."
    fi
  fi
  if [[ "$(tar -tzf axel-${AXEL_VER}.tar.gz >/dev/null; echo $?)" != '0' ]]; then
    rm -rf /svr-setup/axel-${AXEL_VER}.*
    echo "re-try download form local mirror..."
    wget -O $AXEL_LINKFILE $AXEL_LINKLOCAL
  fi
  tar xzf $AXEL_LINKFILE
  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    echo "Error: $AXEL_LINKFILE extraction failed."
    exit #$ERROR
  else 
    echo "$AXEL_LINKFILE valid file."
    echo ""
  fi

  cd axel-${AXEL_VER}
  if [ -f autogen.sh ]; then
    if [ ! -f /usr/bin/autoreconf ]; then
      yum -y -q install autoconf
    fi
  ./autogen.sh
  fi
  ./configure
  make
  make install
  which axel
}

cminstall() {

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Centmin Mod Install"
    fi

cd $INSTALLDIR
  if [[ "$GITINSTALLED" = [yY] ]]; then
    if [[ ! -f "${INSTALLDIR}/centminmod" ]]; then
      getcmstarttime=$(TZ=UTC date +%s.%N)
      echo "git clone Centmin Mod repo..."
      time git clone -b ${branchname} --depth=5 ${CMGIT} centminmod
      getcmendtime=$(TZ=UTC date +%s.%N)
      sar_call
      cd centminmod
      chmod +x centmin.sh
    fi
  else
    if [[ ! -f "${DOWNLOAD}" ]]; then
    getcmstarttime=$(TZ=UTC date +%s.%N)
    echo "downloading Centmin Mod..."
    if [[ -f /usr/local/bin/axel && $AXEL = [yY] ]]; then
      /usr/bin/axel https://github.com/centminmod/centminmod/archive/${DOWNLOAD}
    else
      wget -c${ipv_forceopt} --no-check-certificate https://github.com/centminmod/centminmod/archive/${DOWNLOAD} --tries=3
    fi
    getcmendtime=$(TZ=UTC date +%s.%N)
    rm -rf centminmod-*
    unzip ${DOWNLOAD}
    fi
    #export CUR_DIR
    #export CM_INSTALLDIR
    mv centminmod-${branchname} centminmod
    cd centminmod
    chmod +x centmin.sh
  fi
  GETCMTIME=$(echo "$getcmendtime - $getcmstarttime" | bc)
  echo "$GETCMTIME" > "/root/centminlogs/getcmtime_installtime_${DT}.log"
  GETCMTIME=$(printf "%0.4f\n" $GETCMTIME)
  echo "$GETCMTIME" >> "/root/centminlogs/getcmtime_installtime_${DT}.log"

# disable nginx lua and luajit by uncommenting these 2 lines
#sed -i "s|LUAJIT_GITINSTALL='y'|LUAJIT_GITINSTALL='n'|" centmin.sh
#sed -i "s|ORESTY_LUANGINX='y'|ORESTY_LUANGINX='n'|" centmin.sh

# disable nginx pagespeed module by uncommenting this line
#sed -i "s|NGINX_PAGESPEED=y|NGINX_PAGESPEED=n|" centmin.sh

# disable nginx geoip module by uncommenting this line
#sed -i "s|NGINX_GEOIP=y|NGINX_GEOIP=n|" centmin.sh

# disable nginx vhost traffic stats module by uncommenting this line
#sed -i "s|NGINX_VHOSTSTATS=y|NGINX_VHOSTSTATS=n|" centmin.sh

# disable nginx webdav modules by uncommenting this line
#sed -i "s|NGINX_WEBDAV=y|NGINX_WEBDAV=n|" centmin.sh

# disable openresty additional nginx modules by uncommenting this line
#sed -i "s|NGINX_OPENRESTY='y'|NGINX_OPENRESTY='n'|" centmin.sh

# switch back to OpenSSL instead of LibreSSL for Nginx
#sed -i "s|LIBRESSL_SWITCH='y'|LIBRESSL_SWITCH='n'|" centmin.sh

# siwtch back to Libmemcached source compile instead of YUM repo install
#sed -i "s|LIBMEMCACHED_YUM='y'|LIBMEMCACHED_YUM='n'|" centmin.sh

# disable PHP redis extension
#sed -i "s|PHPREDIS='y'|PHPREDIS='n'|" centmin.sh

# switch from PHP 5.4.41 to 5.6.9 default with Zend Opcache
sed -i "s|^PHP_VERSION='.*'|PHP_VERSION='5.5.38'|" centmin.sh
sed -i "s|ZOPCACHEDFT='n'|ZOPCACHEDFT='y'|" centmin.sh

# disable axivo yum repo
#sed -i "s|AXIVOREPO_DISABLE=n|AXIVOREPO_DISABLE=y|" centmin.sh

# bypass initial setup email prompt
mkdir -p /etc/centminmod/
if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
  echo "LOWMEM_INSTALL='y'" >> /etc/centminmod/custom_config.inc
fi
echo "1" > /etc/centminmod/email-primary.ini
echo "2" > /etc/centminmod/email-secondary.ini
cd "${INSTALLDIR}/centminmod"
./centmin.sh install
sar_call
rm -rf /etc/centminmod/email-primary.ini
rm -rf /etc/centminmod/email-secondary.ini

    # setup command shortcut aliases 
    # given the known download location
    # updated method for cmdir and centmin shorcuts
    sed -i '/cmdir=/d' /root/.bashrc
    sed -i '/centmin=/d' /root/.bashrc
    rm -rf /usr/bin/cmdir
    alias cmdir="pushd /usr/local/src/centminmod"
    echo "alias cmdir='pushd /usr/local/src/centminmod'" >> /root/.bashrc
    echo -e "pushd /usr/local/src/centminmod; bash centmin.sh" > /usr/bin/centmin
    chmod 0700 /usr/bin/centmin
  echo
  echo "Created command shortcuts:"
  echo "* type cmdir to change to Centmin Mod install directory"
  echo "  at /usr/local/src/centminmod"
  echo "* type centmin call and run centmin.sh"
  echo "  at /usr/local/src/centminmod/centmin.sh"
}

if [[ "$DEF" = 'novalue' ]]; then
  {
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    if [[ "$CHECK_LXD" = [yY] || ! -f /usr/bin/gcc ]]; then
      # lxd containers have minimal default yum packages installed
      $YUMDNFBIN -y install yum-utils cmake which e2fsprogs e2fsprogs-devel bc libuuid libuuid-devel openssl openssl-devel zlib zlib-devel gd gd-devel net-tools bzip2-devel gmp-devel libXext-devel libidn-devel libtool-ltdl-devel openldap-devel bluez-libs-devel gcc gcc-c++ automake libtool make
      $YUMDNFBIN -y install libcurl libcurl-devel
      yum -y reinstall bzip2 bzip2-devel
      yum -y groupinstall "Development tools"
      if [ -f /etc/yum.repos.d/jsynacek-systemd-centos-7.repo ]; then
        SYSTEMD_FACEBOOKRPM='y'
      fi
    fi
    source_pcreinstall
    source_wgetinstall
  fi
  install_axel
  fileperm_fixes
  cminstall
} 2>&1 | tee "/root/centminlogs/installer_${DT}.log"
  echo
  FIRSTYUMINSTALLTIME=$(echo "$firstyuminstallendtime - $firstyuminstallstarttime" | bc)
  FIRSTYUMINSTALLTIME=$(printf "%0.4f\n" $FIRSTYUMINSTALLTIME)

  #touch ${CENTMINLOGDIR}/firstyum_installtime_${DT}.log
  echo "" > "/root/centminlogs/firstyum_installtime_${DT}.log"
  {
echo "---------------------------------------------------------------------------"
  echo "Total Curl Installer YUM or DNF Time: $FIRSTYUMINSTALLTIME seconds" >> "/root/centminlogs/firstyum_installtime_${DT}.log"
  tail -1 /root/centminlogs/firstyum_installtime_*.log
  tail -1 /root/centminlogs/centminmod_yumtimes_*.log
  DTIME=$(tail -1 /root/centminlogs/centminmod_downloadtimes_*.log)
  DTIME_SEC=$(echo "$DTIME" |awk '{print $7}')
  NTIME=$(tail -1 /root/centminlogs/centminmod_ngxinstalltime_*.log)
  NTIME_SEC=$(echo "$NTIME" |awk '{print $7}')
  if [ -f /root/centminlogs/centminmod_phpinstalltime_*.log ]; then
    PTIME=$(tail -1 /root/centminlogs/centminmod_phpinstalltime_*.log)
    PTIME_SEC=$(echo "$PTIME" |awk '{print $7}')
  else
    PTIME_SEC='0'
  fi
  CMTIME=$(tail -1 /root/centminlogs/*_install.log)
  CMTIME_SEC=$(echo "$CMTIME" |awk '{print $6}')
  CMTIME_SEC=$(printf "%0.4f\n" $CMTIME_SEC)
if [[ "$DNF_ENABLE" = [yY] ]]; then
  CURLT=$(awk '{print $8}' /root/centminlogs/firstyum_installtime_*.log | tail -1)
else
  CURLT=$(awk '{print $8}' /root/centminlogs/firstyum_installtime_*.log | tail -1)
fi
  CT=$(awk '{print $6}' /root/centminlogs/*_install.log | tail -1)
  GETCMTIME=$(tail -1 /root/centminlogs/getcmtime_installtime_${DT}.log)
  TT=$(echo "$CURLT + $CT + $GETCMTIME" | bc)
  TT=$(printf "%0.4f\n" $TT)
  ST=$(echo "$CT - ($DTIME_SEC + $NTIME_SEC + $PTIME_SEC)" | bc)
  ST=$(printf "%0.4f\n" $ST)
  echo "Total YUM or DNF + Source Download Time: $(printf "%0.4f\n" $DTIME_SEC)"
  echo "Total Nginx First Time Install Time: $(printf "%0.4f\n" $NTIME_SEC)"
  echo "Total PHP First Time Install Time: $(printf "%0.4f\n" $PTIME_SEC)"
  echo "Download From Github Time: $GETCMTIME"
  echo "Total Time Other eg. source compiles: $ST"
  echo "Total Centmin Mod Install Time: $CMTIME_SEC"
echo "---------------------------------------------------------------------------"
  echo "Total Install Time (curl yum + cm install + zip download): $TT seconds"    
echo "---------------------------------------------------------------------------"
  echo "$CPUMODEL"; echo "$CPUSPEED"
echo "---------------------------------------------------------------------------"
} 2>&1 | tee "/root/centminlogs/install_time_stats_${DT}.log"
  cat "/root/centminlogs/install_time_stats_${DT}.log" >> "/root/centminlogs/installer_${DT}.log"
  cat "/root/centminlogs/installer_${DT}.log" | egrep -v 'DOPENSSL_PIC|\/opt\/openssl\/share\/|fpm-build\/libtool|checking for |checking whether |make -f |make\[1\]|make\[2\]|make\[3\]|make\[4\]|make\[5\]' > "/root/centminlogs/installer_${DT}_minimal.log"
  systemstats
fi

if [ -f "${INSTALLDIR}/curlinstall_yum.txt" ]; then
  rm -rf "${INSTALLDIR}/curlinstall_yum.txt"
fi

case "$1" in
  install)
    # devtoolset SCL repo only supports 64bit OSes
    if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
      source_pcreinstall
      source_wgetinstall
    fi
    install_axel
    fileperm_fixes
    cminstall
    ;;
  yumupdate)
    yumupdater
    install_axel
    cminstall
    ;;
  *)
    if [[ "$DEF" = 'novalue' ]]; then
      echo
    else
      echo "./$0 {install|yumupdate}"
    fi
    ;;
esac