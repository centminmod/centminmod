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
USER=$(whoami)
HOSTNAME=$(uname -n)
RELEASE=$(cat /etc/redhat-release | sed -e 's| (Core)||' -e 's| release||')
PSA=$(ps -Afl | wc -l)
CURRENTUSER=$(users | wc -w)
CMSCRIPT_GITDIR='/usr/local/src/centminmod'
CONFIGSCANBASE='/etc/centminmod'
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
    source "${CONFIGSCANBASE}/custom_config.inc"
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
 - Hostname......: $HOSTNAME on $RELEASE
 - Users.........: Currently $CURRENTUSER user(s) logged on (includes: $USER)
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
! This server maybe running CSF Firewall !  
  DO NOT run the below command or you  will lock yourself out of the server: 

  iptables -F 
"
fi
if [[ "$ENABLEMOTD_LINKSMSG" != [nN] ]]; then
echo "
===============================================================================
* Getting Started Guide - http://centminmod.com/getstarted.html
* Centmin Mod FAQ - http://centminmod.com/faq.html
* Change Log - http://centminmod.com/changelog.html
* Community Forums https://community.centminmod.com  [ << Register ]
===============================================================================
"
fi
}


ngxver_checker() {
  if [[ "$(which nginx >/dev/null 2>&1; echo $?)" = '0' ]]; then
    LASTEST_NGINXVERS=$(curl -sL https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n 1 2>&1)
    CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk -F '/' '{print $2}')
    if [[ "$CURRENT_NGINXVERS" != "$LASTEST_NGINXVERS" ]]; then
      echo
      cecho "===============================================================================" $boldgreen
      cecho "* Nginx Update May Be Available via centmin.sh menu option 4" $boldyellow
      cecho "* see https://centminmod.com/nginx.html#nginxupgrade" $boldyellow
      cecho "===============================================================================" $boldgreen
      cecho "* Current Nginx Version: $CURRENT_NGINXVERS" $boldyellow
      cecho "* Latest Nginx Available: $LASTEST_NGINXVERS" $boldyellow
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
      CURL_GITURL=$(curl -s https://raw.githubusercontent.com/centminmod/centminmod/$(awk -F "=" '/branchname=/ {print $2}' ${CMSCRIPT_GITDIR}/centmin.sh | sed -e "s|'||g" )/giturl.txt)
      # if git version >1.8 use supported ls-remote --get-url flag otherwise use alternative
      if [[ -d "${CMSCRIPT_GITDIR}" ]]; then
        if [[ "$GET_GITVER" -ge '18' ]]; then
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git ls-remote --get-url)
        else
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git remote -v | awk '/\(fetch/ {print $2}' | head -n1)
        fi
        if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod remote branch has changed" $boldyellow
          cecho " from $GET_GITREMOTEURL" $boldyellow
          cecho " to $CURL_GITURL" $boldyellow
          cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          cecho "===============================================================================" $boldgreen
        fi
      fi
      pushd "${CMSCRIPT_GITDIR}" >/dev/null 2>&1
      git fetch >/dev/null 2>&1
      popd >/dev/null 2>&1
      if [[ "$(cd ${CMSCRIPT_GITDIR}; git rev-parse HEAD)" != "$(cd ${CMSCRIPT_GITDIR}; git rev-parse @{u})" ]]; then
          # if remote branch commits don't match local commit, then there are new updates need
          # pulling
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod code updates available for ${CMSCRIPT_GITDIR}" $boldyellow
          if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
            cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          else
            cecho " to update re-run centmin.sh menu option 23 submenu option 2" $boldyellow
          fi
          cecho "===============================================================================" $boldgreen
        else
          # no new commits/updates available
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod local code is up to date at ${CMSCRIPT_GITDIR}" $boldyellow
          cecho " no available updates at this time..." $boldyellow
          cecho "===============================================================================" $boldgreen
      fi
    fi
}

motd_output
ngxver_checker
gitenv_askupdate