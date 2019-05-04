#!/bin/bash
######################################################
# written by George Liu (eva2000) centminmod.com
######################################################
# Create at /usr/local/nginx/conf/htpasswd
# ./htpasswd.sh create /usr/local/nginx/conf/htpasswd user1 pass1  
######################################################
# Append at /usr/local/nginx/conf/htpasswd
# ./htpasswd.sh append /usr/local/nginx/conf/htpasswd user2 pass2 
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")

######################################################
# functions
#############

genpassa() {
  if [[ -f "$file" && "$user" && "$pass" ]]; then
  printf "${user}:$(openssl passwd -apr1 ${pass})\n" >> $file
  echo ""
  echo "$file contents:"
  cat $file
  fi
}

genpassc() {
  if [[ -f "$file" && "$user" && "$pass" ]]; then
  printf "${user}:$(openssl passwd -apr1 ${pass})\n" > $file
  echo ""
  echo "$file contents:"
  cat $file
  fi
}

######################################################
file=$2
user=$3
pass=$4

case "$1" in
  create)
    touch $file
    genpassc
    ;;
  append)
    genpassa
    ;;
  *)
    echo ""
    echo "$0 {create|append} /usr/local/nginx/conf/htpasswd user1 pass1"
    ;;
esac