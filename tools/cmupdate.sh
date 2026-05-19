#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
######################################################
# cmupdate
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
MAINDIR='/etc/centminmod'
CM_INSTALLDIR='/usr/local/src/centminmod'
#############
# variables
#############
cmupdate_branchname='141.00beta01'
cmupdate_branchname_new=$cmupdate_branchname
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
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

if [ -f "${MAINDIR}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    source "${MAINDIR}/custom_config.inc"
fi

if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

# Rebase-aware git sync wrapper. Prefers the canonical inc/git_sync.inc
# helper. If the helper is missing (first bootstrap after an upstream
# rebase), falls through to a 6-line panic stub that does the bare
# minimum to unstick. Full safety logic lives in inc/git_sync.inc only —
# do not add features to the panic stub.
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

fupdate() {
  branch_opt=$1
  if [[ "$branch_opt" = 'beta' ]]; then
    cmupdate_branchname=140.00beta01
    cmupdate_branchname_new=$cmupdate_branchname
  elif [[ "$branch_opt" = 'stable' ]]; then
    cmupdate_branchname=132.00stable
    cmupdate_branchname_new=$cmupdate_branchname
  fi

  CMUPDATE_GITCURLSTATUS=$(curl -${ipv_forceopt}sL --connect-timeout 20 -sI https://raw.githubusercontent.com/centminmod/centminmod/${cmupdate_branchname}/gitclean.txt | grep 'HTTP/' | awk '/200/ {print $2}')
    if [[ "$CMUPDATE_GITCURLSTATUS" = '200' ]]; then
      CHECK_GITCLEAN=$(curl -${ipv_forceopt}sLk --connect-timeout 20 https://raw.githubusercontent.com/centminmod/centminmod/${cmupdate_branchname}/gitclean.txt)
    else
      echo
      echo "Error: unable to connect to Github.com repo right now"
      echo "try again later"
      echo
      echo "if issue persists, report it on official Centmin Mod community"
      echo "https://community.centminmod.com/forums/install-upgrades-or-pre-install-questions.8/"
      exit 1
    fi
  if [[ -d "${CM_INSTALLDIR}/.git" ]]; then
    if [[ "$branch_opt" = 'beta' || "$branch_opt" = 'stable' ]]; then
      echo "Switching local code branch to $cmupdate_branchname_new"
      echo
      cd /usr/local/src
      mv centminmod centminmod-automoved-cmupdate
      git clone -b ${cmupdate_branchname_new} --depth=1 https://github.com/centminmod/centminmod.git centminmod
      if [[ "$?" -eq '0' ]]; then
        rm -rf centminmod-automoved-cmupdate
        echo
        echo "Completed. Fresh ${CM_INSTALLDIR} code base in place"
        echo "To run centmin.sh again, you need to change into directory: ${CM_INSTALLDIR}"
        echo "cd ${CM_INSTALLDIR}"
        echo
      else
        mv centminmod-automoved-cmupdate centminmod
        echo
        echo "Error: wasn't able to successfully update ${CM_INSTALLDIR} code base"
        echo "       restoring previous copy of ${CM_INSTALLDIR} code base"
      fi
    elif [[ "$CHECK_GITCLEAN" = 'no' ]]; then
      echo
      echo "-------------------------------------"
      echo "Updating Centmin Mod code"
      echo "-------------------------------------"
      echo
      cd "${CM_INSTALLDIR}"
      git branch
      _cmm_git_sync "$cmupdate_branchname_new" "${CM_INSTALLDIR}"
      git log -1 | sed -e 's|Author: George Liu <.*>|Author: George Liu <snipped>|g'
      echo
      echo "-------------------------------------"
      echo "Updated Centmin Mod code"
      echo "-------------------------------------"
      echo
    else
      echo
      echo "Detected Centmin Mod Github Remote Repo Changes"
      echo "setting up fresh ${CM_INSTALLDIR} code base to match"
      echo
      cd /usr/local/src
      mv centminmod centminmod-automoved-cmupdate
      git clone -b ${cmupdate_branchname_new} --depth=1 https://github.com/centminmod/centminmod.git centminmod
      if [[ "$?" -eq '0' ]]; then
        rm -rf centminmod-automoved-cmupdate
        echo
        echo "Completed. Fresh ${CM_INSTALLDIR} code base in place"
        echo "To run centmin.sh again, you need to change into directory: ${CM_INSTALLDIR}"
        echo "cd ${CM_INSTALLDIR}"
        echo
      else
        mv centminmod-automoved-cmupdate centminmod
        echo
        echo "Error: wasn't able to successfully update ${CM_INSTALLDIR} code base"
        echo "       restoring previous copy of ${CM_INSTALLDIR} code base"
      fi
    fi
  fi
  if [ -x "${CM_INSTALLDIR}/tools/cmm-security/cmsec.sh" ]; then
    cmsec_target="${CM_INSTALLDIR}/tools/cmm-security/cmsec.sh"
    cmsec_replace=1
    if [ -L /usr/bin/cmsec ]; then
      cmsec_current="$(readlink /usr/bin/cmsec 2>/dev/null)"
      if [ -e /usr/bin/cmsec ] && [ "$cmsec_current" = "$cmsec_target" ]; then
        cmsec_replace=0
      fi
    fi
    if [ "$cmsec_replace" -eq 1 ]; then
      rm -f /usr/bin/cmsec
      ln -s "$cmsec_target" /usr/bin/cmsec
      chmod +x /usr/bin/cmsec
    fi
    unset cmsec_target cmsec_replace cmsec_current
  fi
}

######################################################
case "$1" in
  update )
    fupdate
    ;;
  update-stable )
    fupdate stable
    ;;
  update-beta )
    fupdate beta
    ;;
  * )
    fupdate
    exit
    ;;
esac

exit