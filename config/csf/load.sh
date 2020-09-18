#!/bin/bash
######################################################
# when csf lfd high cpu alert is triggered send an 
# additional email with cminfo top and netstats output
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

######################################################
# functions
#############
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

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

if [ -f /usr/bin/cminfo ]; then
>/etc/csf/csf.report
/usr/bin/cminfo top >> /etc/csf/csf.report
/usr/bin/cminfo netstat >> /etc/csf/csf.report
CHECK_LFEMAIL=$(awk -F '=' '/LF_ALERT_TO/ {print $2}' /etc/csf/csf.conf | sed -e 's|\"||g' -e 's|\s||')

if [[ "$CHECK_LFEMAIL" && "$EMAILNOTIFY_SES" = [yY] && "$EMAILNOTIFY_SES_FROM_EMAIL" && "$EMAILNOTIFY_SES_TO_EMAIL" && -f /usr/local/src/centminmod/tools/emailnotify.sh ]]; then
  # use tools/emailnotify.sh AWS SES supported tool
  # https://community.centminmod.com/threads/20407/
  /usr/local/src/centminmod/tools/emailnotify.sh send /etc/csf/csf.report "lfd on $(hostname) Centmin Mod Extended Load Report $(date)"
elif [[ "$CHECK_LFEMAIL" ]]; then
  mail -s "lfd on $(hostname) Centmin Mod Extended Load Report $(date)" "$CHECK_LFEMAIL" -r "$CHECK_LFEMAIL" < /etc/csf/csf.report
elif [[ -z "$CHECK_LFEMAIL" ]]; then
  mail -s "lfd on $(hostname) Centmin Mod Extended Load Report $(date)" root < /etc/csf/csf.report
fi
fi