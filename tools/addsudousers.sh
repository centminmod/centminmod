#!/bin/bash
#################################################################
# add sudo user script as discussed at
# http://stackoverflow.com/questions/8784761/adding-users-to-sudoers-through-shell-script?lq=1
# http://www.liquidweb.com/kb/how-to-add-a-user-and-grant-root-privileges-on-centos-7/
# http://www.liquidweb.com/kb/how-to-add-a-user-and-grant-root-privileges-on-centos-6-5/
#################################################################
# usage:
# 
# ./addsudousers.sh username
# 
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)

if [ -d /etc/sudoers.d ]; then
  while [[ -n $1 ]]; do
  echo
  echo "Creating a sudo user & setting password for $1"
  echo
  # read -ep "Enter the username for sudo user you want to create: " sudo_username
  sudo_username=$1
  useradd $sudo_username
  # read -ep "Enter the password for $sudo_username: " sudo_userpass
  passwd $sudo_username
  echo
  # echo "${sudo_username}:${sudo_userpass}" | chpasswd
  echo "$sudo_username with password: $sudo_userpass created"
  echo "sudo setup for $sudo_username"
  echo "$1    ALL=(ALL:ALL) ALL" > /etc/sudoers.d/sudo.$1;
  chmod 0440 /etc/sudoers.d/sudo.$1
  visudo -c -q -f /etc/sudoers.d/sudo.$1
  shift # shift all parameters;
  echo
  echo "$1 sudo user setup at /etc/sudoers.d/sudo.$1"
  done
 fi

exit