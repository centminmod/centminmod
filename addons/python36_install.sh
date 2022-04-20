#!/bin/bash
###########################################################
# installs IUS Community YUM Repository
# https://iuscommunity.org/pages/Repos.html
# for access to Python 3.6 on CentOS as default
# is Python 2.6 for CentOS 6
#
# i.e. python 3.6
# yum -y install python36u python36u-devel python36u-pip python36u-setuptools python36u-tools --enablerepo=ius
# rpm -ql python36u python36u-devel python36u-pip python36u-setuptools python36u-tools python36u-tkinter
# 
# rpm -ql python36u python36u-pip | grep bin
# /usr/bin/pydoc3.6
# /usr/bin/python3.6
# /usr/bin/python3.6m
# /usr/bin/pyvenv-3.6
# /usr/bin/pip3.6
# 
# https://docs.python.org/3/library/venv.html
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###########################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

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
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
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
if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  echo "$0 only for CentOS 7"
  echo "aborted..."
  exit 1
fi

starttime=$(TZ=UTC date +%s.%N)
{

check_pythonthree_six() {
  if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  # if [[ "$(echo "$CENTOSVER" | sed -e 's|\.||g')" -ge '77' ]]; then
    # CentOS 7.7+ already have native python 3.6 yum packages
    # via python3 and python3-libs so no longer require EPEL python36 packages
    if [[ "$CENTOS_SEVEN" -eq '7' && -z "$(rpm -qa python3)" ]]; then
      yum -q -y install python3
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && -z "$(rpm -qa python3-libs)" ]]; then
      yum -q -y install python3-libs
    fi
  fi
}

if [[ "$CENTOS_SIX" = '6' ]]; then
    rpm --import https://repo.ius.io/RPM-GPG-KEY-IUS-6
    yum -y install https://repo.ius.io/ius-release-el6.rpm
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    rpm --import https://repo.ius.io/RPM-GPG-KEY-IUS-7
    yum -y install https://repo.ius.io/ius-release-el7.rpm
fi

# disable by default the ius.repo
sed -i 's/enabled=1/enabled=0/' /etc/yum.repos.d/ius.repo

if [ -f /etc/yum.repos.d/ius.repo ]; then
\cp -pf /etc/yum.repos.d/ius.repo /etc/yum.repos.d/ius.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/ius.repo)" ]
        then
                #echo priorities already set for ius.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for ius.repo"
                ex -s /etc/yum.repos.d/ius.repo << EOF
:/\[ius/ , /gpgkey/
:a
priority=98
.
:w
:/\[ius-debuginfo/ , /gpgkey/
:a
priority=98
.
:w
:/\[ius-source/ , /gpgkey/
:a
priority=98
.
:w
:q
EOF

cecho "*************************************************" $boldgreen
cecho "Fixing ius.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
echo "cat /etc/yum.repos.d/ius.repo"
cat /etc/yum.repos.d/ius.repo
echo ""
fi
fi # repo file check

cecho "*************************************************" $boldgreen
cecho "Installing Python 3.6" $boldgreen
cecho "*************************************************" $boldgreen

if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
# if [[ "$(echo "$CENTOSVER" | sed -e 's|\.||g')" -ge '77' ]]; then
  # prefer CentOS 7.7+ native python3 packages for python 3.6
    if [[ -f /bin/systemctl && "$(rpm -qa python36u)" ]]; then
      # remove ius community python36u
      yum -y remove python36u python36u-devel python36u-pip python36u-setuptools python36u-tools python36u-libs python36u-tkinter
      if [[ "$CENTOS_SEVEN" = '7' ]] && [[ ! "$(rpm -qa cmake3)" || ! "$(rpm -qa cmake3-data)" ]]; then
        check_pythonthree_six
        # reinstall removed dependencies from above removed ius community packages
        yum -y install cmake3 cmake3-data
      fi
    fi
    if [[ -f /bin/systemctl && "$(rpm -qa python36)" ]]; then
      # remove epel python36
      yum -y remove python36 python36-devel python36-pip python36-setuptools python36-tools python36-libs python36-tkinter
      if [[ "$CENTOS_SEVEN" = '7' ]] && [[ ! "$(rpm -qa cmake3)" || ! "$(rpm -qa cmake3-data)" ]]; then
        check_pythonthree_six
        # reinstall removed dependencies from above removed ius community packages
        yum -y install cmake3 cmake3-data
      fi
    fi
    yum -y install python3 python3-devel python3-pip python3-setuptools python3-tools python3-libs python3-tkinter
    rpm -ql python3 python3-devel python3-pip python3-setuptools python3-tools python3-tkinter | grep bin
    if [[ "$CENTOS_SEVEN" = '7' ]] && [[ ! "$(rpm -qa cmake3)" || ! "$(rpm -qa cmake3-data)" ]]; then
      # reinstall removed dependencies from above removed ius community packages
      yum -y install cmake3 cmake3-data
    fi
# elif [[ "$(echo "$CENTOSVER" | sed -e 's|\.||g')" -lt '77' ]]; then
#   # install Python 3.6 besides system default Python 2.6
#   if [[ -f /bin/systemctl && -z "$(rpm -qa python36u)" ]] && [[ -z "$(rpm -qa python36)" ]]; then
#     # only install python36u if python36 isn't installed
#     yum -y install python36u python36u-devel python36u-pip python36u-setuptools python36u-tools --enablerepo=ius
#     rpm -ql python36u python36u-devel python36u-pip python36u-setuptools python36u-tools python36u-tkinter | grep bin
#   elif [[ -f /bin/systemctl && "$(rpm -qa python36)" && -z "$(rpm -qa python36-tools)" ]]; then
#     # install epel python36
#     yum -y install python36 python36-devel python36-pip python36-setuptools python36-tools python36-libs python36-tkinter
#     rpm -ql python36 python36-devel python36-pip python36-setuptools python36-tools python36-tkinter | grep bin
#   elif [[ -f /bin/systemctl && -z "$(rpm -qa python36)" ]]; then
#     # install epel python36
#     yum -y install python36 python36-devel python36-pip python36-setuptools python36-tools python36-libs python36-tkinter
#     rpm -ql python36 python36-devel python36-pip python36-setuptools python36-tools python36-tkinter | grep bin
#   fi
  
#   # switch in favour of epel python36 version
#   # only apply to centos 7 as centos 6 epel doesn't have python36
#   if [[ -f /bin/systemctl && "$(rpm -qa python36u)" ]]; then
#     # remove ius community python36u
#     yum -y remove python36u python36u-devel python36u-pip python36u-setuptools python36u-tools python36u-libs python36u-tkinter
#     # install epel python36
#     yum -y install python36 python36-devel python36-pip python36-setuptools python36-tools python36-libs python36-tkinter
#     rpm -ql python36 python36-devel python36-pip python36-setuptools python36-tools python36-tkinter | grep bin
#   fi
#   if [[ "$CENTOS_SEVEN" = '7' ]] && [[ ! "$(rpm -qa cmake3)" || ! "$(rpm -qa cmake3-data)" ]]; then
#     check_pythonthree_six
#     # reinstall removed dependencies from above removed ius community packages
#     yum -y install cmake3 cmake3-data
#   fi
fi

} 2>&1 | tee ${CENTMINLOGDIR}/python36-install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/python36-install_${DT}.log
echo "Python 3.6 Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/python36-install_${DT}.log