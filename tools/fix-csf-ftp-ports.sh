#!/bin/bash
#################################################################
# fix csf v14.18 adding DoT DNS over TLS port 853 which broke
# Centmin Mod initial install CSF port list configuration
# and didn't add pure-ftpd passive port range to whitelist
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

fix_csf_ftp_ports() {
  # Check if the range 30001:50011 is absent in TCP_IN and TCP6_IN
  if ! grep -qE 'TCP_IN.*30001:50011' /etc/csf/csf.conf || ! grep -qE 'TCP6_IN.*30001:50011' /etc/csf/csf.conf; then
    # check CSF ports current config
    grep -E '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
    # backup existing CSF Firewall profile config at /var/lib/csf/backup/
    csf --profile backup fix-ftp
    # modify /etc/csf/csf.conf and save modifications /etc/csf/csf.conf.tmp
    awk -v additional="1110,1186,1194,2049,81,9418,30001:50011" '/^TCP_IN =/ {gsub("\"$", "," additional "\""); print; next} /^TCP6_IN =/ {gsub("\"$", "," additional "\""); print; next} {print}' /etc/csf/csf.conf > /etc/csf/csf.conf.tmp
    # check CSF ports current config
    grep -E '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
    # compare /etc/csf/csf.conf vs /etc/csf/csf.conf.tmp via diff
    diff -u /etc/csf/csf.conf /etc/csf/csf.conf.tmp
    # overwrite existing csf.conf with modified
    mv /etc/csf/csf.conf.tmp /etc/csf/csf.conf
    # reduce ports list if nfs isn't installed
    [[ ! -z "$(rpm -ql nfs-utils | grep 'not installed')" && -f /etc/csf/csf.conf ]] && sed -i.bak -E -e 's|,111,|,|g' -e 's|,2049,|,|g' /etc/csf/csf.conf
    # check CSF ports current config
    grep -E '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
    # restart CSF firewall and lfd
    csf -ra
  else
    echo "Port range 30001:50011 is already present. No changes needed."
  fi
}

{
  fix_csf_ftp_ports
} 2>&1 | tee "${CENTMINLOGDIR}/fix-csf-ftp-ports-${DT}.log"