#!/bin/bash
######################################################
# cmupdate
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
MAINDIR='/etc/centminmod'
CM_INSTALLDIR='/usr/local/src/centminmod'
#############
if [ -f "${MAINDIR}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    source "${MAINDIR}/custom_config.inc"
fi

# variables
#############
branchname=123.09beta01
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

fupdate() {
  if [[ -d "${CM_INSTALLDIR}/.git" ]]; then
    if [[ "$(curl -sL https://github.com/centminmod/centminmod/raw/${branchname}/gitclean.txt)" = 'no' ]]; then
      cd "${CM_INSTALLDIR}"
      git stash
      git pull
    else
      echo
      echo "Detected Centmin Mod Github Remote Repo Changes"
      echo "setting up fresh /usr/local/src/centminmod code base to match"
      echo
      rm -rf "${CM_INSTALLDIR}"
      cd /usr/local/src
      git clone -b ${branchname} --depth=1 https://github.com/centminmod/centminmod.git centminmod
      echo
      echo "Completed. Fresh /usr/local/src/centminmod code base in place"
      echo "To run centmin.sh again, you need to change into directory: ${CM_INSTALLDIR}"
      echo "cd ${CM_INSTALLDIR}"
      echo
    fi
  fi
}

######################################################
fupdate

exit


