#!/bin/bash
######################################################
# script to switch centmin mod locally installed code
# base to github version for easier updates
# https://community.centminmod.com/threads/working-with-git-command-line-for-updating-centmin-mod-local-copies.2150/
######################################################
branchname='123.09beta01'

######################################################
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
######################################################
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

echo
cecho "setup Centmin Mod git sourced install..." $boldyellow
cd /usr/local/src
rm -rf centminmod
rm -rf centminmod*

echo  
cecho "download github.com centmin mod ${branchname} branch repo" $boldyellow
time git clone -b ${branchname} --depth=5 ${CMGIT} centminmod
cd centminmod
chmod +x centmin.sh

echo
cecho "list all available local branches" $boldyellow
cecho "	git branch -a" $boldgreen
git branch -a

echo
cecho "list git log last commit" $boldyellow
cecho "	git log -a" $boldgreen
git log -1 | sed -e 's|Author: George Liu <.*>|Author: George Liu <snipped>|g'

echo
cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
cecho "	cd /usr/local/src/centminmod" $boldgreen
cecho "	git stash" $boldgreen
cecho "	git pull" $boldgreen
cecho "	chmod +x centmin.sh" $boldgreen

exit