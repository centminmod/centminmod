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
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
#######################################################
DT=$(date +"%d%m%y-%H%M%S")
CM_INSTALLDIR='/usr/local/src/centminmod'

# Rebase-aware git sync wrapper. Prefers the canonical inc/git_sync.inc
# helper. If the helper is missing (first bootstrap after an upstream
# rebase), falls through to a 6-line panic stub. Full safety logic lives
# in inc/git_sync.inc only — do not add features to the panic stub.
_cmm_git_sync() {
  local branch="$1"
  local workdir="${2:-${CM_INSTALLDIR}}"
  if [ -r "${CM_INSTALLDIR}/inc/git_sync.inc" ]; then
    . "${CM_INSTALLDIR}/inc/git_sync.inc"
    _cmm_git_sync_impl "$branch" "$workdir"
    return $?
  fi
  cd "$workdir" || return 2
  git fetch --prune origin "$branch" || return 2
  git reset --hard "origin/$branch" || return 2
  chmod +x centmin.sh
  echo "RESULT: SUCCESS (panic-stub) — Centmin Mod synced. Run cmupdate again."
  return 0
}

update() {
  if [ -d /usr/local/src/centminmod/.git ]; then
  echo
  echo "-------------------------------------"
  echo "Updating Centmin Mod code"
  echo "-------------------------------------"
  echo
    cd /usr/local/src/centminmod
    git branch
    # Detect the active branch; refuse to operate on a detached HEAD or
    # an unknown branch — both can corrupt unattended cron updates.
    branchname=$(git symbolic-ref --short HEAD 2>/dev/null)
    if [ -z "$branchname" ] || [ "$branchname" = "HEAD" ]; then
      echo "cmupdate-status: branch=UNKNOWN action=skip reason=detached-or-no-branch" >&2
      return 1
    fi
    _cmm_git_sync "$branchname" /usr/local/src/centminmod
    _rc=$?
    echo "cmupdate-status: branch=$branchname rc=$_rc"
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