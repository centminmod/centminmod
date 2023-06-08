#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
###################################################
# tool to update fix DST Root CA X3 expiry by
# blacklisting expired cert
# https://community.centminmod.com/threads/21965/
###################################################
DT=$(date +"%d%m%y-%H%M%S")

lets_dst_root_ca_fix() {
  if [ "$(/usr/bin/openssl version | grep '1.0.2')" ] && [ ! -z "$(grep -i 'DST Root CA X3' /etc/pki/tls/certs/ca-bundle.crt)" ]; then
    echo
    echo "Update workaround to blacklist expiring Letsencrypt DST Root CA X3 certificate..."
    echo "https://community.centminmod.com/threads/21965/"
    echo
    mkdir -p /root/tools/backup-ca-certs
    if [ -f /etc/pki/tls/certs/ca-bundle.crt ]; then
      \cp -f /etc/pki/tls/certs/ca-bundle.crt /root/tools/backup-ca-certs/ca-bundle.crt-backup
    fi
    if [[ ! -f /usr/bin/trust || ! -f /usr/bin/update-ca-trust ]]; then
      yum -q -y install ca-certificates p11-kit-trust
    fi
    if [[ -f /usr/bin/trust && -f /usr/bin/update-ca-trust ]]; then
      trust dump --filter "pkcs11:id=%c4%a7%b1%a4%7b%2c%71%fa%db%e1%4b%90%75%ff%c4%15%60%85%89%10" | openssl x509 > /etc/pki/ca-trust/source/blacklist/DST-Root-CA-X3.pem
      update-ca-trust extract
      diff /root/tools/backup-ca-certs/ca-bundle.crt-backup /etc/pki/tls/certs/ca-bundle.crt > /root/tools/backup-ca-certs/diff-ca-bundle.crt.diff
      echo "Diff check file at /root/tools/backup-ca-certs/diff-ca-bundle.crt.diff"
      echo
      echo "Check to see if DST Root CA X3 is blacklisted"
      echo "trust list | grep -C3 'DST Root CA X3' | grep -B1 'blacklisted'"
      echo
      trust list | grep -C3 'DST Root CA X3' | grep -B1 'blacklisted'
      echo
      echo "Update ca-certificates YUM package for permanent fix"
      yum -q -y update ca-certificates
      echo "Updated ca-certificates"
      yum -q history list ca-certificates
    fi
  elif [ "$(/usr/bin/openssl version | grep '1.0.2')" ] && [ -z "$(grep -i 'DST Root CA X3' /etc/pki/tls/certs/ca-bundle.crt)" ]; then
      echo
      echo "Expiring DST Root CA X3 certificate not detected in /etc/pki/tls/certs/ca-bundle.crt"
      echo "System is good to go :)"
  fi
}
lets_dst_root_ca_fix