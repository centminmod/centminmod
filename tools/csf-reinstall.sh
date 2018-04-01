#!/bin/bash
###############################################################
# reinstall CSF Firewall and backup and restore data
# instead of just saving csf conf file backups, this
# script dynamically gathers all whitelist, ban ips
# and restores to a fresh CSF Firewall installation
# configured to replicate Centmin Mod initial install
# of CSF Firewall.
# 
# this allows for updates and new settings to be 
# properly accounted for in csf conf files in
# subsequent CSF version updates.
# 
# if you just backed up the conf files and restored
# you may miss new conf file options and settings 
# introduced into CSF Firewall conf files.
# 
# written by George Liu (eva2000) centminmod.com
###############################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
RESTORE_DENYIPS='y'
RESTORE_ALLOWIPS='y'
FIREWALLD_DISABLE='y'

###############################################################
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

if [ -f ../inc/csfinstall.inc ]; then
  . ../inc/csfinstall.inc
# elif [ -f /usr/local/src/centminmod/inc/csfinstall.inc ]; then
#   . /usr/local/src/centminmod/inc/csfinstall.inc
else
    echo "can not find ../inc/csfinstall.inc"
    echo "$0 needs to be ran from"
    echo "/usr/local/src/centminmod/tools"
    exit
fi
if [ -f ../inc/csftweaks.inc ]; then
  . ../inc/csftweaks.inc
# elif [ -f /usr/local/src/centminmod/inc/csftweaks.inc ]; then
#   . /usr/local/src/centminmod/inc/csftweaks.inc
else
    echo "can not find ../inc/csftweaks.inc"
    echo "$0 needs to be ran from"
    echo "/usr/local/src/centminmod/tools"
    exit
fi
NOTICE='y'
###############################################################
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

###############################################################
# functions
#############

if [[ "$NOTICE" = [yY] ]]; then
  echo
  echo "-------------------------------------------------"
  echo "$0 is in beta testing phase"
  echo "only run on test servers right now"
  echo "-------------------------------------------------"
  echo
  read -ep "continue [y/n] ? " _proceed
  if [[ "$_proceed" != [yY] ]]; then
    echo
    echo "aborting..."
    echo
    exit
  fi
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

cmservice() {
        servicename=$1
        action=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "service ${servicename} $action"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                service "${servicename}" "$action"
        fi
        else
        echo "systemctl $action ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl "$action" "${servicename}.service"
        fi
        fi
}

cmchkconfig() {
        servicename=$1
        status=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "chkconfig ${servicename} $status"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                chkconfig "${servicename}" "$status"
        fi
        else
                if [ "$status" = 'on' ]; then
                        status=enable
                fi
                if [ "$status" = 'off' ]; then
                        status=disable
                fi
        echo "systemctl $status ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl "$status" "${servicename}.service"
        fi
        fi
}

backupdata() {
    # backup ip allow and ban lists to reapply on reinstalled CSF
    echo
    echo "---------------------------------------------------------------------"
    echo "create /etc/csf-backups directory"
    mkdir -p /etc/csf-backups
    echo "cp -a /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}"
    cp -a /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}
    echo "cp -a /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}"
    cp -a /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}
    echo "cp -a /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}"
    cp -a /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}
    echo "cp -a /etc/csf/csf.ignore /etc/csf-backups/csf.ignore-${DT}"
    cp -a /etc/csf/csf.ignore /etc/csf-backups/csf.ignore-${DT}
    echo "cp -a /etc/csf/csf.fignore /etc/csf-backups/csf.fignore-${DT}"
    cp -a /etc/csf/csf.fignore /etc/csf-backups/csf.fignore-${DT}
    echo "cp -a /etc/csf/csf.mignore /etc/csf-backups/csf.mignore-${DT}"
    cp -a /etc/csf/csf.mignore /etc/csf-backups/csf.mignore-${DT}
    echo "cp -a /etc/csf/csf.rignore /etc/csf-backups/csf.rignore-${DT}"
    cp -a /etc/csf/csf.rignore /etc/csf-backups/csf.rignore-${DT}
    echo "cp -a /etc/csf/csf.signore /etc/csf-backups/csf.signore-${DT}"
    cp -a /etc/csf/csf.signore /etc/csf-backups/csf.signore-${DT}
    echo "cp -a /etc/csf/csf.suignore /etc/csf-backups/csf.suignore-${DT}"
    cp -a /etc/csf/csf.suignore /etc/csf-backups/csf.suignore-${DT}
    echo "cp -a /usr/local/csf/tpl /etc/csf-backups/tpl-${DT}"
    cp -a /usr/local/csf/tpl /etc/csf-backups/tpl-${DT}
    echo "cp -a /usr/local/csf/profiles /etc/csf-backups/profiles-${DT}"
    cp -a /usr/local/csf/profiles/ /etc/csf-backups/profiles-${DT}

    echo
    echo "file backups at /etc/csf-backups"
    ls -lahrt /etc/csf-backups | grep "$DT"

    # backup temp ip bans
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup temp ips ban to /tmp/csf-tempips.txt"
    csf -t | awk '/DENY  / {print $2}' > /tmp/csf-tempips.txt

    # backup allowed ip whitelist
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup whitelisted ips to /tmp/csf-allowips.txt"
    awk '/\ #\ / {print $1}' /etc/csf/csf.allow > /tmp/csf-allowips.txt

    # backup denied ip ban list
    # echo
    # echo "---------------------------------------------------------------------"
    # echo "backup denied ips to /tmp/csf-denyips.txt"
    awk '/\ #\ / {print $1}' /etc/csf/csf.deny > /tmp/csf-denyips.txt

    echo
    echo "---------------------------------------------------------------------"
    echo "Existing Port Whitelist Profile"
    egrep '^TCP_|^TCP6_|^UDP_|^UDP6' /etc/csf/csf.conf
    TCP_INBACKUP=$(awk '/^TCP_IN/ {print $3}' /etc/csf/csf.conf)
    TCP_OUTBACKUP=$(awk '/^TCP_OUT/ {print $3}' /etc/csf/csf.conf)
    TCP6_INBACKUP=$(awk '/^TCP6_IN/ {print $3}' /etc/csf/csf.conf)
    TCP6_OUTBACKUP=$(awk '/^TCP6_OUT/ {print $3}' /etc/csf/csf.conf)
    UDP_INBACKUP=$(awk '/^UDP_IN/ {print $3}' /etc/csf/csf.conf)
    UDP_OUTBACKUP=$(awk '/^UDP_OUT/ {print $3}' /etc/csf/csf.conf)
    UDP6_INBACKUP=$(awk '/^UDP6_IN/ {print $3}' /etc/csf/csf.conf)
    UDP6_OUTBACKUP=$(awk '/^UDP6_OUT/ {print $3}' /etc/csf/csf.conf)
}

delcsf() {
    if [ -f /etc/csf/uninstall.sh ]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "Uninstall CSF Firewall"
    echo "---------------------------------------------------------------------"
      /etc/csf/uninstall.sh
    fi
}

restoredata() {
    # echo
    echo "---------------------------------------------------------------------"
    echo "Restoring CSF Firewall data"
    echo "---------------------------------------------------------------------"
    sed -i "s|^TCP_IN = .*|TCP_IN = $TCP_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP_OUT = .*|TCP_OUT = $TCP_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP6_IN = .*|TCP6_IN = $TCP6_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^TCP6_OUT = .*|TCP6_OUT = $TCP6_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP_IN = .*|UDP_IN = $UDP_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP_OUT = .*|UDP_OUT = $UDP_OUTBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP6_IN = .*|UDP6_IN = $UDP6_INBACKUP|" /etc/csf/csf.conf
    sed -i "s|^UDP6_OUT = .*|UDP6_OUT = $UDP6_OUTBACKUP|" /etc/csf/csf.conf
    if [[ "$RESTORE_DENYIPS" = [yY] ]]; then
        cat /tmp/csf-denyips.txt >> /etc/csf/csf.deny
    fi
    if [[ "$RESTORE_ALLOWIPS" = [yY] ]]; then
        cat /tmp/csf-allowips.txt >> /etc/csf/csf.allow
    fi

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}"
    sdiff -w 120 -s /etc/csf/csf.conf /etc/csf-backups/csf.conf-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}"
    sdiff -w 120 -s /etc/csf/csf.allow /etc/csf-backups/csf.allow-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    echo
    echo "---------------------------------------------------------------------"
    echo "sdiff -w 120 -s /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}"
    sdiff -w 120 -s /etc/csf/csf.deny /etc/csf-backups/csf.deny-${DT}
    if [[ "$?" = '0' ]]; then
        echo "[ no changes detected ]"
    else
        echo "[ changes detected ]"
    fi
    echo "---------------------------------------------------------------------"

    csf -ra >/dev/null 2>&1

    echo "---------------------------------------------------------------------"
    echo "Check Whitelist Profile"
    echo "---------------------------------------------------------------------"

    egrep '^TCP_|^TCP6_|^UDP_|^UDP6' /etc/csf/csf.conf

    echo
    echo "---------------------------------------------------------------------"
    echo "CSF Firewall data restored"
    echo "---------------------------------------------------------------------"
    if [[ "$RESTORE_DENYIPS" != [yY] ]]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "RESTORE_DENYIPS is disabled skipped restore of deny ips"
    echo "---------------------------------------------------------------------"
    fi
    if [[ "$RESTORE_ALLOWIPS" != [yY] ]]; then
    echo
    echo "---------------------------------------------------------------------"
    echo "RESTORE_ALLOWPS is disabled skipped restore of allow ips"
    echo "---------------------------------------------------------------------"
    fi
    echo
}

cleanup() {
    echo "cleaning up temp files"
    rm -rf /tmp/csf-tempips.txt
    rm -rf /tmp/csf-allowips.txt
    rm -rf /tmp/csf-denyips.txt
}

trap cleanup SIGHUP SIGINT SIGTERM
######################################################
{
backupdata
delcsf
csfinstalls
    echo
    echo "---------------------------------------------------------------------"
    echo "CSF Firewall reinstalled"
    echo "---------------------------------------------------------------------"
    echo
restoredata
cleanup
} 2>&1 | tee "${CENTMINLOGDIR}/csf-reinstall_${DT}.log"
echo
echo "---------------------------------------------------------------------"
echo "saved log: ${CENTMINLOGDIR}/csf-reinstall_${DT}.log"
echo "---------------------------------------------------------------------"
