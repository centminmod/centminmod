#!/bin/bash
######################################################
# pip initial updates done manually instead of using
# centmin.sh menu auto update
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
DEBUG='n'
YUMDNFBIN='yum'

DT=$(date +"%d%m%y-%H%M%S")
######################################################
# functions
#############

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

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
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

pip_updates() {
  initial=$1
  mkdir -p /home/piptmp
  chmod 1777 /home/piptmp
  export TMPDIR=/home/piptmp
  if [ -f /etc/centminmod-release ]; then
    if [[ "$inital" = 'initial' ]]; then
      echo "pip updates..."
    fi
    # for glances and psutil as glances is installed via outdated EPEL
    # yum repo but there's a new version available
    if [[ ! -f /usr/bin/python-config ]]; then
      $YUMDNFBIN -q -y install python-devel
    fi
    if [[ -d /usr/local/lib/python3.6/site-packages/pip && "$(pip --version | grep -o 'python3.6')" && ! "$(rpm -qa python3-devel | grep -o 'python3-devel')" ]]; then
      yum -q -y install python3-devel
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && ! -d /usr/lib/python2.7/site-packages/urllib3/packages/ssl_match_hostname ]]; then
        # $YUMDNFBIN -q -y install python-urllib3 >/dev/null 2>&1
        # yum -q -y versionlock python-urllib3 >/dev/null 2>&1
        if [[ ! "$(grep 'python-urllib3' /etc/yum.conf)" ]]; then
          NEW_PYTHONURLLIB_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-urllib3")
          sed -i "s|^exclude=.*|$NEW_PYTHONURLLIB_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade urllib3
    elif [[ "$CENTOS_SEVEN" -eq '7' && -d /usr/lib/python2.7/site-packages/urllib3/packages/ssl_match_hostname ]]; then
        if [ ! "$initial" ]; then
          CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list -o --format columns)
        fi
        CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list --format columns)
        CHECK_PIPURLLIBUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o urllib3)
        if [[ "$(rpm -qa python-urllib3 | grep -o python-urllib3)" = 'python-urllib3' ]]; then
          #yum -q -y remove python-urllib3 >/dev/null 2>&1
          #yum versionlock delete python-urllib3 >/dev/null 2>&1
          yum versionlock python-urllib3 cloud-init python-requests >/dev/null 2>&1
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq urllib3
        fi
        if [[ ! "$(grep 'python-urllib3' /etc/yum.conf)" ]]; then
          NEW_PYTHONURLLIB_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-urllib3")
          sed -i "s|^exclude=.*|$NEW_PYTHONURLLIB_EXCLUDES|" /etc/yum.conf
        fi
        if [[ ! "$(grep 'cloud-init' /etc/yum.conf)" ]]; then
          CLOUDINIT_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) cloud-init")
          sed -i "s|^exclude=.*|$CLOUDINIT_EXCLUDES|" /etc/yum.conf
        fi
        if [[ ! "$(grep 'python-requests' /etc/yum.conf)" ]]; then
          PYTHONREQUESTS_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-requests")
          sed -i "s|^exclude=.*|$PYTHONREQUESTS_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade urllib3
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && ! -f /usr/bin/pip ]] || [[ "$CENTOS_SIX" -eq '6' &&  ! -f /usr/bin/pip2.7 ]]; then
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        $YUMDNFBIN -q -y install python2-pip >/dev/null 2>&1
        yum -q -y versionlock python2-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip==20.3.4
      else
        $YUMDNFBIN -q -y install python-pip >/dev/null 2>&1
        yum -q -y versionlock python-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip==20.3.4
        if [[ "$CENTOS_SIX" -eq '6' && -f "${SCRIPT_DIR}/addons/python27_install.sh" && ! -f /usr/bin/pip2.7 ]]; then
          "${SCRIPT_DIR}/addons/python27_install.sh" install
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade setuptools
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade psutil
          PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
          echo
          echo "CentOS 6 python 2.7 compatibility updates completed"
          echo
        fi
      fi
    elif [[ "$CENTOS_SIX" -eq '6' && -f /usr/bin/pip2.7 ]]; then
      CHECK_PIPVER=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip show pip 2>&1 | awk '/^Version: / {print $2}' | sed -e 's|\.|0|g')
      if [[ "$CHECK_PIPVER" -lt '1801' ]]; then
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
      fi
    elif [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/bin/pip ]]; then
      CHECK_PIPVER=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip show pip 2>&1 | awk '/^Version: / {print $2}' | sed -e 's|\.||g')
      if [[ "$CHECK_PIPVER" -lt '901' ]]; then
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip==20.3.4
      fi
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && -f /usr/bin/pip && -f /usr/bin/python-config ]]; then
      if [ ! "$initial" ]; then
        CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list -o --format columns)
      fi
      CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip list --format columns)
      if [ ! "$initial" ]; then
        CHECK_PIPUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o pip)
        CHECK_PSUTILUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o psutil)
        CHECK_GLANCESUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -io glances)
      fi
      CHECK_PSUTILINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -o psutil)
      CHECK_GLANCEINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -io glances)
      if [[ "$(yum versionlock list | grep 'python2-pip')" ]]; then
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PIPUPDATE" = 'pip' || ! "$initial" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade pip==20.3.4
        yum -q -y versionlock python2-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python2-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PSUTILUPDATE" = 'psutil' || ! "$CHECK_PSUTILINSTALL" ]]; then
        export CC='gcc'
        if [[ "$(rpm -qa python2-psutil | grep -o python2-psutil)" = 'python2-psutil' ]]; then
          yum -q -y remove python2-psutil >/dev/null 2>&1
          # yum -q -y versionlock python2-psutil >/dev/null 2>&1
          if [[ ! "$(grep 'python2-psutil' /etc/yum.conf)" ]]; then
            NEW_PYTHONPSUTIL_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python2-psutil")
            sed -i "s|^exclude=.*|$NEW_PYTHONPSUTIL_EXCLUDES|" /etc/yum.conf
          fi
        fi
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade setuptools
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade psutil
      fi
      if [[ "$CHECK_GLANCESUPDATE" = 'Glances' || ! "$CHECK_GLANCEINSTALL" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
      fi
    fi
    if [[ "$CENTOS_SIX" -eq '6' && -f /usr/bin/pip2.7 && -f /usr/bin/python2.7-config ]]; then
      CHECK_PIPALL_UPDATES=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 list -o --format columns)
      CHECK_PIPALL_INSTALLED=$(PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 list --format columns)
      if [ ! "$initial" ]; then
        CHECK_PIPUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o pip)
        CHECK_PSUTILUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -o psutil)
        CHECK_GLANCESUPDATE=$(echo "$CHECK_PIPALL_UPDATES" | grep -io glances)
      fi
      CHECK_PSUTILINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -o psutil)
      CHECK_GLANCEINSTALL=$(echo "$CHECK_PIPALL_INSTALLED" | grep -io glances)
      if [[ "$(yum versionlock list | grep 'python-pip')" ]]; then
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PIPUPDATE" = 'pip' || ! "$initial" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade pip
        yum -q -y versionlock python-pip >/dev/null 2>&1
        if [[ ! "$(grep 'python-pip' /etc/yum.conf)" ]]; then
          NEW_PYTHONPIP_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-pip")
          sed -i "s|^exclude=.*|$NEW_PYTHONPIP_EXCLUDES|" /etc/yum.conf
        fi
      fi
      if [[ "$CHECK_PSUTILUPDATE" = 'psutil' || ! "$CHECK_PSUTILINSTALL" ]]; then
        export CC='gcc'
        if [[ "$(rpm -qa python-psutil | grep -o python-psutil)" = 'python-psutil' ]]; then
          yum -q -y remove python-psutil >/dev/null 2>&1
          # yum -q -y versionlock python-psutil >/dev/null 2>&1
          if [[ ! "$(grep 'python-psutil' /etc/yum.conf)" ]]; then
            NEW_PYTHONPSUTIL_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) python-psutil")
            sed -i "s|^exclude=.*|$NEW_PYTHONPSUTIL_EXCLUDES|" /etc/yum.conf
          fi
        fi
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade setuptools
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade psutil
      fi
      if [[ "$CHECK_GLANCESUPDATE" = 'Glances' || ! "$CHECK_GLANCEINSTALL" ]]; then
        export CC='gcc'
        PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install -qqq --upgrade 'glances[cpuinfo,ip,raid]'
      fi
    fi
  fi
  unset TMPDIR
  rm -rf /home/piptmp
}

glances_aliascheck() {
  if [ -f /usr/bin/glances ]; then
    if [ ! -f /etc/glances/glances.conf ]; then
      mkdir -p /etc/glances
      # wget -O /etc/glances/glances.conf https://raw.githubusercontent.com/nicolargo/glances/master/conf/glances.conf
      wget -O /etc/glances/glances.conf https://github.com/centminmod/centminmod/raw/123.09beta01/config/glances/glances.conf
      sed -i 's|^disable=True|disable=False|g' /etc/glances/glances.conf
    fi
    if [[ ! "$(grep -w 'glances' /root/.bashrc)" ]]; then
        echo "alias top2=\"glances\"" >> /root/.bashrc
        alias top2="glances"
        echo "top2 alias configured"
        echo "to use exit SSH session & relogin"
    fi
    if [[ "$(id -u)" -ne '0' ]]; then
        if [[ ! "$(grep -w 'glances' $HOME/.bashrc)" ]]; then
            echo "alias top2=\"glances\"" >> $HOME/.bashrc
            alias top2="glances"
            echo "top2 alias configured"
            echo "to use exit SSH session & relogin"
        fi
    fi
  fi
}

######################################################
case "$1" in
  update )
    initial=$2
    pip_updates "$initial"
    glances_aliascheck
    ;;
  * )
    echo "$0 update"
    ;;
esac