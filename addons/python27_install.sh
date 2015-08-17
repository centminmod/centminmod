#!/bin/bash
###########################################################
# installs IUS Community YUM Repository
# https://iuscommunity.org/pages/Repos.html
# for access to Python 2.7, 3.2, 3.3 and 3.4 on CentOS as default
# is Python 2.6
#
# i.e. python 2.7
# yum -y install python27 python27-devel python27-pip python27-setuptools python27-tools python27-virtualenv --enablerepo=ius
# rpm -ql python27 python27-devel python27-pip python27-setuptools python27-tools tkinter27 python27-virtualenv
# 
# rpm -ql python27 python27-pip python27-virtualenv | grep bin
# /usr/bin/pydoc27
# /usr/bin/python2.7
# /usr/bin/pip2.7
# /usr/bin/virtualenv-2.7
# 
# i.e. python 3.4
# yum -y install python34u python34u-devel python34u-pip python34u-setuptools python34u-tools --enablerepo=ius
# rpm -ql python34u python34u-devel python34u-pip python34u-setuptools python34u-tools python34u-tkinter
# 
# rpm -ql python34u python34u-pip | grep bin
# /usr/bin/pydoc3
# /usr/bin/pydoc3.4
# /usr/bin/python3
# /usr/bin/python3.4
# /usr/bin/python3.4m
# /usr/bin/pyvenv
# /usr/bin/pyvenv-3.4
# /usr/bin/pip3
# /usr/bin/pip3.4
# 
# https://docs.python.org/3/library/venv.html
###########################################################
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

###########################################################
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

if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo
    echo "detected CentOS 7.x OS, python 2.7 is already"
    echo "the default version for CentOS 7.x"
    echo "aborting install..."
    echo
    exit
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
starttime=$(date +%s.%N)
{

if [[ "$CENTOS_SIX" = '6' ]]; then
	yum -y install https://dl.iuscommunity.org/pub/ius/stable/CentOS/6/x86_64/ius-release-1.0-14.ius.centos6.noarch.rpm
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
	yum -y install https://dl.iuscommunity.org/pub/ius/stable/CentOS/7/x86_64/ius-release-1.0-14.ius.centos7.noarch.rpm
fi

# disable by default the ius.repo
sed -i 's/enabled=1/enabled=0/' /etc/yum.repos.d/ius.repo

if [ -f /etc/yum.repos.d/ius.repo ]; then
cp -p /etc/yum.repos.d/ius.repo /etc/yum.repos.d/ius.OLD
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
cecho "Installing Python 2.7" $boldgreen
cecho "*************************************************" $boldgreen

# install Python 2.7 besides system default Python 2.6
yum -y install python27 python27-devel python27-pip python27-setuptools python27-tools python27-virtualenv --enablerepo=ius

rpm -ql python27 python27-devel python27-pip python27-setuptools python27-tools tkinter27 python27-virtualenv | grep bin

} 2>&1 | tee ${CENTMINLOGDIR}/python27-install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/python27-install_${DT}.log
echo "Python 2.7 Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/python27-install_${DT}.log