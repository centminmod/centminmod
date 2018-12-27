#!/bin/bash
###########################################################
# set locale temporarily to english
# for wget compile due to some non-english
# locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
###########################################################
# wget source installer to /usr/local/bin/wget path for
# centminmod.com LEMP stacks
# installs newer wget version than available via centos RPM
# repos but does not interfere with YUM installed wget as it
# is just an alias wget command setup
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
DNF_ENABLE='n'
DNF_COPR='y'
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
LOCALCENTMINMOD_MIRROR='https://centminmod.com'

ALTPCRE_VERSION='8.42'
ALTPCRELINKFILE="pcre-${ALTPCRE_VERSION}.tar.gz"
ALTPCRELINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/pcre/${ALTPCRELINKFILE}"

WGET_VERSION='1.20.1'
WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
WGET_LINK="https://centminmod.com/centminmodparts/wget/${WGET_FILENAME}"
WGET_LINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/wget/${WGET_FILENAME}"
WGET_STRACE='n'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###########################################################
shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

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

if [ -f /usr/local/lib/libssl.a ]; then
    # echo "clean up old /usr/local/lib/libssl.a"
    rm -rf /usr/local/lib/libssl.a
    ldconfig
fi
if [ -f /usr/local/lib/libcrypto.a ]; then
    # echo "clean up old /usr/local/lib/libcrypto.a"
    rm -rf /usr/local/lib/libcrypto.a
    ldconfig
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

if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
  # yum -y -q install epel-release
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
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
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
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
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
fi
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
sar_call() {
  $SARCALL 1 1
}

scl_install() {
  # if gcc version is less than 4.7 (407) install scl collection yum repo
  if [[ "$CENTOS_SIX" = '6' ]]; then
    # if devtoolset exists, enable it first before checking gcc versions
    if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
      if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
      fi
    else
      if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-7/enable
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
          if [[ "$(rpm -ql devtoolset-7-gcc >/de6/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ 6/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils6>/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/de6/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ 6/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils6>/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
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
          if [[ "$(rpm -ql devtoolset-7-gcc >/de6/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ 6/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils6>/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/de6/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ 6/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils6>/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      else
        if [[ -z "$(rpm -qa | grep rpmforge)" ]]; then
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
          fi
          sar_call
        else
          if [[ "$(rpm -ql devtoolset-7-gcc >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-gcc-c++ >/dev/null 2>&1; echo $?)" -ne '0' ]] || [[ "$(rpm -ql devtoolset-7-binutils >/dev/null 2>&1; echo $?)" -ne '0' ]]; then
            time $YUMDNFBIN -y -q install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils --disablerepo=rpmforge
          fi
          sar_call
        fi
        echo
        /opt/rh/devtoolset-7/root/usr/bin/gcc --version
        /opt/rh/devtoolset-7/root/usr/bin/g++ --version
      fi
  fi # centos 6 only needed
}

gccdevtools() {
  if [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$CENTOS_SIX" = '6' ]]; then
    scl_install
    unset CC
    unset CXX
    if [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
      export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
      export CFLAGS="-Wimplicit-fallthrough=0"
      export CXXFLAGS="${CFLAGS}"
    else
      export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
      export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
      export CFLAGS="-Wimplicit-fallthrough=0"
      export CXXFLAGS="${CFLAGS}"
    fi
  elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++" 
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  elif [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    unset CC
    unset CXX
    export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
    export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++"
    export CFLAGS="-Wimplicit-fallthrough=0"
    export CXXFLAGS="${CFLAGS}"
  fi
}

source_pcreinstall() {
  if [[ "$(/usr/local/bin/pcre-config --version 2>&1 | grep -q ${ALTPCRE_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]] || [[ -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No UTF-8 support' >/dev/null 2>&1; echo $?)" = '0' ]] || [[ -f /usr/local/bin/pcretest && "$(/usr/local/bin/pcretest -C | grep 'No just-in-time compiler support' >/dev/null 2>&1; echo $?)" = '0' ]]; then
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
  make clean >/dev/null 2>&1
  ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_pcre_make_$DT.log" -f -s256 -tt -T -q make${MAKETHREADS}
  else
    make${MAKETHREADS}
  fi
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_pcre_make_install_$DT.log" -f -s256 -tt -T -q make install
  else  
    make install
  fi
  sar_call
  /usr/local/bin/pcre-config --version
  fi
}

source_wgetinstall() {
  if [[ "$(/usr/local/bin/wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" != '0' ]]; then
  WGET_FILENAME="wget-${WGET_VERSION}.tar.gz"
  WGET_LINK="https://centminmod.com/centminmodparts/wget/${WGET_FILENAME}"
  cd "$DIR_TMP"
  cecho "Download $WGET_FILENAME ..." $boldyellow
  if [ -s "$WGET_FILENAME" ]; then
    cecho "$WGET_FILENAME Archive found, skipping download..." $boldgreen
  else

    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$WGET_LINK" | grep 'HTTP\/' | grep '200'
    WGET_CURLCHECK=$?
    if [[ "$WGET_CURLCHECK" = '0' ]]; then
      wget -c${ipv_forceopt} --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3
    else
      WGET_LINK="$WGET_LINKLOCAL"
      echo "wget -c${ipv_forceopt} --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3"
      wget -c${ipv_forceopt} --progress=bar "$WGET_LINK" -O "$WGET_FILENAME" --tries=3
    fi
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
  if [ -f config.status ]; then
    make clean
  fi
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
  if [[ "$WGET_STRACE" = [yY] ]]; then
    make check
    make distcheck
    strace -o "${CENTMINLOGDIR}/strace_wget_make_$DT.log" -f -s256 -tt -T -q make${MAKETHREADS}
  else
    make${MAKETHREADS}
  fi
  sar_call
  if [[ "$WGET_STRACE" = [yY] ]]; then
    strace -o "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" -f -s256 -tt -T -q make install
  else
    make install
  fi
  sar_call
  echo "/usr/local/lib/" > /etc/ld.so.conf.d/wget.conf
  ldconfig
  if [[ ! "$(grep '^alias wget' /root/.bashrc)" ]] && [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" = '0' ]]; then
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
  if [[ "$(wget -V | head -n1 | awk '{print $3}' | grep -q ${WGET_VERSION} >/dev/null 2>&1; echo $?)" = '0' ]]; then
    cecho "wget ${WGET_VERSION} installed at /usr/local/bin/wget" $boldyellow
    cecho "https://community.centminmod.com/tags/wget/" $boldyellow
    if [[ "$WGET_STRACE" = [yY] ]]; then
      # ls -lah ${CENTMINLOGDIR} | grep $DT
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_$DT.log"
        cecho "strace make log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_$DT.log.gz" $boldyellow
      fi
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log"
        cecho "strace make install log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_install_$DT.log.gz" $boldyellow
      fi
    fi
  else
    cecho "wget ${WGET_VERSION} failed to update, still using system wget" $boldyellow
    cecho "https://community.centminmod.com/tags/wget/" $boldyellow
    cecho "install log: ${CENTMINLOGDIR}/wget_source_install_${DT}.log" $boldyellow
    if [[ "$WGET_STRACE" = [yY] ]]; then
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_$DT.log"
        cecho "strace make log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_$DT.log.gz" $boldyellow
      fi
      if [ -f "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log" ]; then
        gzip -6 "${CENTMINLOGDIR}/strace_wget_make_install_$DT.log"
        cecho "strace make install log (gzip compressed): ${CENTMINLOGDIR}/strace_wget_make_install_$DT.log.gz" $boldyellow
      fi
    fi
  fi
  # clean up strace logs older than 14 days
  find "${CENTMINLOGDIR}" -type f -mtime +14 \( -name 'strace_wget_make*' ! -name "strace_pcre_make*" \) -print
  find "${CENTMINLOGDIR}" -type f -mtime +14 \( -name 'strace_wget_make*' ! -name "strace_pcre_make*" \) -exec rm -rf {} \;
  cecho "--------------------------------------------------------" $boldgreen
  echo
  fi
}

###########################################################################
case $1 in
  install)
starttime=$(TZ=UTC date +%s.%N)
{
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    source_pcreinstall
    source_wgetinstall
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/wget_source_install_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
echo "Total wget Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
tail -1 "${CENTMINLOGDIR}/wget_source_install_${DT}.log"
  ;;
  pcre)
starttime=$(TZ=UTC date +%s.%N)
{
  # devtoolset SCL repo only supports 64bit OSes
  if [[ "$LOWMEM_INSTALL" != [yY] && "$(uname -m)" = 'x86_64' ]]; then
    source_pcreinstall
  fi
} 2>&1 | tee "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
echo "Total wget pcre Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
tail -1 "${CENTMINLOGDIR}/wget_source_install_pcre_${DT}.log"
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit