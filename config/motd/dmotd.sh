#!/bin/bash

# Installation:
#
# echo "PrintMotd no" >> /etc/ssh/sshd_config
# echo "# session optional pam_motd.so" >> /etc/pam.d/login
# echo "/usr/local/bin/dmotd" >> /etc/profile
# chmod +x /usr/local/bin/dmotd
# 

USER=$(whoami)
HOSTNAME=$(uname -n)
RELEASE=$(cat /etc/redhat-release | sed -e 's| (Core)||' -e 's| release||')
PSA=$(ps -Afl | wc -l)
CURRENTUSER=$(users | wc -w)

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
===============================================================================
! This server maybe running CSF Firewall !  
  DO NOT run the below command or you  will lock yourself out of the server: 

  iptables -F 

===============================================================================
* Getting Started Guide - http://centminmod.com/getstarted.html
* Centmin Mod FAQ - http://centminmod.com/faq.html
* Change Log - http://centminmod.com/changelog.html
* Community Forums https://community.centminmod.com  [ << Register ]
===============================================================================
"