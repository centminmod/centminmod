#!/bin/bash
######################################################
# when csf lfd high cpu alert is triggered send an 
# additional email with cminfo top and netstats output
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
if [ -f /usr/bin/cminfo ]; then
>/etc/csf/csf.report
/usr/bin/cminfo top >> /etc/csf/csf.report
/usr/bin/cminfo netstat >> /etc/csf/csf.report
CHECK_LFEMAIL=$(awk -F '=' '/LF_ALERT_TO/ {print $2}' /etc/csf/csf.conf | sed -e 's|\"||g' -e 's|\s||')
if [[ "$CHECK_LFEMAIL" ]]; then
  mail -s "lfd on $(hostname) Centmin Mod Extended Load Report $(date)" "$CHECK_LFEMAIL" -r "$CHECK_LFEMAIL" < /etc/csf/csf.report
elif [[ -z "$CHECK_LFEMAIL" ]]; then
  mail -s "lfd on $(hostname) Centmin Mod Extended Load Report $(date)" root < /etc/csf/csf.report
fi
fi