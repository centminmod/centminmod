#!/bin/bash
#######################################################
# centminmod.com updater
# https://community.centminmod.com/threads/3398/
# 
# setup cron job i.e. every 6 hrs
# 0 */6 * * * /usr/local/src/centminmod/updatecm.sh 2>/dev/null
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

update() {
  if [ -d /usr/local/src/centminmod/.git ]; then
  echo
  echo "-------------------------------------"
  echo "Updating Centmin Mod code"
  echo "-------------------------------------"
  echo
    cd /usr/local/src/centminmod
    git branch
    git stash
    git pull
    git log -1 | sed -e 's|Author: George Liu <.*>|Author: George Liu <snipped>|g'
  fi
  echo
  echo "-------------------------------------"
  echo "Updated Centmin Mod code"
  echo "-------------------------------------"
  echo
}

starttime=$(TZ=UTC date +%s.%N)
{
update
} 2>&1 | tee "${CENTMINLOGDIR}/updatecm_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/updatecm_${DT}.log"
echo "Total updatecm.sh Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/updatecm_${DT}.log"