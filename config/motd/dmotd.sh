#!/bin/bash
###########################################################
# Installation:
#
# echo "PrintMotd no" >> /etc/ssh/sshd_config
# echo "# session optional pam_motd.so" >> /etc/pam.d/login
# echo "/usr/local/bin/dmotd" >> /etc/profile
# chmod +x /usr/local/bin/dmotd
# 
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
DMOTD_USER=$(whoami)
DMOTD_HOSTNAME=$(uname -n)
DMOTD_RELEASE=$(cat /etc/redhat-release | sed -e 's| (Core)||' -e 's| release||')
PSA=$(ps -Afl | wc -l)
DMOTD_CURRENTUSER=$(users | wc -w)
CMSCRIPT_GITDIR='/usr/local/src/centminmod'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
SSHLOGIN_KERNELCHECK='n'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
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
if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    source "${CONFIGSCANBASE}/custom_config.inc"
fi
if [[ "$(id -u)" -eq '0' && ! -d "$CENTMINLOGDIR" ]]; then
  mkdir -p $CENTMINLOGDIR
fi

# time of day
HOUR=$(date +"%H")
if [ $HOUR -lt 12  -a $HOUR -ge 0 ]
then    TIME="morning"
elif [ $HOUR -lt 17 -a $HOUR -ge 12 ] 
then    TIME="afternoon"
else 
    TIME="evening"
fi

#System uptime
uptime=$(cat /proc/uptime | cut -f1 -d.)
upDays=$((uptime/60/60/24))
upHours=$((uptime/60/60%24))
upMins=$((uptime/60%60))
upSecs=$((uptime%60))

#System load
LOAD1=$(cat /proc/loadavg | awk {'print $1'})
LOAD5=$(cat /proc/loadavg | awk {'print $2'})
LOAD15=$(cat /proc/loadavg | awk {'print $3'})

#System Info
MEM=$(free -m)
DF=$(df -hT)

motd_output() {
echo "
===============================================================================
 - Hostname......: $DMOTD_HOSTNAME on $DMOTD_RELEASE
 - Users.........: Currently $DMOTD_CURRENTUSER user(s) logged on (includes: $DMOTD_USER)
===============================================================================
 - CPU usage.....: $LOAD1, $LOAD5, $LOAD15 (1, 5, 15 min)
 - Processes.....: $PSA running
 - System uptime.: $upDays days $upHours hours $upMins minutes $upSecs seconds
===============================================================================
$MEM
===============================================================================
$DF
"
if [[ "$ENABLEMOTD_CSFMSG" != [nN] ]]; then
echo "===============================================================================
# ! This server maybe running CSF Firewall !  
#   DO NOT run the below command or you  will lock yourself out of the server: 
# 
#   iptables -F 
"
fi
if [[ "$ENABLEMOTD_LINKSMSG" != [nN] ]]; then
echo "
===============================================================================
* Getting Started Guide - https://centminmod.com/getstarted.html
* Centmin Mod FAQ - https://centminmod.com/faq.html
* Change Log - https://centminmod.com/changelog.html
* Community Forums https://community.centminmod.com  [ << Register ]
===============================================================================
"
fi
}


ngxver_checker() {
  if [[ "$(which nginx >/dev/null 2>&1; echo $?)" = '0' ]]; then
    LASTEST_NGINXVERS=$(curl -${ipv_forceopt}sL https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n1 2>&1)
    LATEST_NGINXSTABLEVER=$(curl -${ipv_forceopt}sL https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n2 | tail -1)
    CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk '{print $3}' | awk -F '/' '{print $2}')
    if [[ "$CURRENT_NGINXVERS" != "$LASTEST_NGINXVERS" ]]; then
      echo
      cecho "===============================================================================" $boldgreen
      cecho "* Nginx Update May Be Available via centmin.sh menu option 4" $boldyellow
      cecho "* see https://centminmod.com/nginx.html#nginxupgrade" $boldyellow
      cecho "===============================================================================" $boldgreen
      cecho "* Current Nginx Version:           $CURRENT_NGINXVERS" $boldyellow
      cecho "* Latest Nginx Mainline Available: $LASTEST_NGINXVERS (centminmod.com/nginxnews)" $boldyellow
      # cecho "* Latest Nginx Stable Available:   $LATEST_NGINXSTABLEVER" $boldyellow
      cecho "===============================================================================" $boldgreen
      echo
    fi
  fi
}

gitenv_askupdate() {
  DT=$(date +"%d%m%y-%H%M%S")
    if [[ -d "${CMSCRIPT_GITDIR}/.git" ]]; then
      # if git remote repo url is not same as one defined in giturl.txt then pull a new copy of
      # centmin mod code locally using giturl.txt defined git repo name
      GET_GITVER=$(git --version | awk '{print $3}' | sed -e 's|\.||g' | cut -c1,2)
      CURL_GITURL=$(curl -s${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/$(awk -F "=" '/branchname=/ {print $2}' ${CMSCRIPT_GITDIR}/centmin.sh | sed -e "s|'||g" )/giturl.txt)
      # if git version >1.8 use supported ls-remote --get-url flag otherwise use alternative
      if [[ -d "${CMSCRIPT_GITDIR}" ]]; then
        if [[ "$GET_GITVER" -ge '18' ]]; then
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git ls-remote --get-url)
        else
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git remote -v | awk '/\(fetch/ {print $2}' | head -n1)
        fi
        if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]] && [[ ! -z "$CURL_GITURL" ]]; then
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod remote branch has changed" $boldyellow
          cecho " from $GET_GITREMOTEURL" $boldyellow
          cecho " to $CURL_GITURL" $boldyellow
          cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          cecho "===============================================================================" $boldgreen
        fi
      fi
      pushd "${CMSCRIPT_GITDIR}" >/dev/null 2>&1
      if [[ "$DMOTD_DEBUGSSHLOGIN" = [yY] ]]; then
        echo
        echo "################ DMOTD DEBUG BEGIN ################"
        echo "DMOTD DEBUG: Ping test github.com"
        ping -c4 github.com
        echo
        echo "DMOTD DEBUG: git fetch timings"
        echo "git fetch -v"
        export GIT_TRACE=1
        export GIT_TRACE_PACKET=1
        export GIT_TRACE_PERFORMANCE=1
        /usr/bin/time --format='real: %es user: %Us sys: %Ss cpu: %P maxmem: %M KB cswaits: %w' git fetch -v
        echo
        echo "################  DMOTD DEBUG END  ################"
        echo
      else
        git fetch >/dev/null 2>&1
      fi
      popd >/dev/null 2>&1
      if [[ "$(cd ${CMSCRIPT_GITDIR}; git rev-parse HEAD)" != "$(cd ${CMSCRIPT_GITDIR}; git rev-parse @{u})" ]]; then
          # if remote branch commits don't match local commit, then there are new updates need
          # pulling
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod code updates available for ${CMSCRIPT_GITDIR}" $boldyellow
          if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
            cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          else
            cecho " to update, run cmupdate command in SSH & re-run centmin.sh once & exit" $boldyellow
          fi
          cecho "===============================================================================" $boldgreen
        else
          # no new commits/updates available
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod local code is up to date at ${CMSCRIPT_GITDIR}" $boldyellow
          cecho " no available updates at this time..." $boldyellow
          cecho "===============================================================================" $boldgreen
      fi
      if [[ "$DMOTD_DEBUGSSHLOGIN" = [yY] ]]; then
        echo
        echo "DMOTD DEBUG: timings saved at:"
        echo "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"
        echo
      fi
    fi
}

kernel_checks() {
  if [[ "$SSHLOGIN_KERNELCHECK" = [yY] && -f "$CMSCRIPT_GITDIR/tools/kernelcheck.sh" ]]; then
    "$CMSCRIPT_GITDIR/tools/kernelcheck.sh"
  fi
}

if [[ "$(id -u)" = '0' ]]; then

starttime=$(TZ=UTC date +%s.%N)
{
motd_output
kernel_checks
ngxver_checker
gitenv_askupdate
} 2>&1 | tee "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"
echo "Total Git & Nginx Check Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"

  # logs older than 5 days will be removed
  if [ -d "${CENTMINLOGDIR}" ]; then
    # find "${CENTMINLOGDIR}" -type f -mtime +5 -name 'cmm-login-git-checks_*.log' -print
    find "${CENTMINLOGDIR}" -type f -mtime +5 -name 'cmm-login-git-checks_*.log' | while read f; do
      if [ -f "$f" ]; then
        # echo "removing $f"
        rm -rf $f
      fi
    done
  fi

fi