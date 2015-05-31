#!/bin/bash
######################################################
# script to switch centmin mod locally installed code
# base to github version for easier updates
# https://community.centminmod.com/threads/working-with-git-command-line-for-updating-centmin-mod-local-copies.2150/
######################################################
branchname='123.08beta03'

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

echo
cecho "setup Centmin Mod git sourced install..." $boldyellow
cd /usr/local/src
rm -rf centminmod-${branchname}

echo  
cecho "download github.com centmin mod ${branchname} branch repo" $boldyellow
git clone https://github.com/centminmod/centminmod.git centminmod-${branchname}
cd centminmod-${branchname}
git checkout -f ${branchname}
chmod +x centmin.sh

echo
cecho "list all available local branches" $boldyellow
cecho "	git branch -a" $boldgreen
git branch -a

echo
cecho "list git log last commit" $boldyellow
cecho "	git log -a" $boldgreen
git log -1

echo
cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
cecho "	cd /usr/local/src/centminmod-${branchname}" $boldgreen
cecho "	git stash" $boldgreen
cecho "	git pull" $boldgreen
cecho "	chmod +x centmin.sh" $boldgreen

exit