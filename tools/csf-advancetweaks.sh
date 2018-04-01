#!/bin/bash
#####################################################
# advanced tuning for CSF firewall centminmod.com 
# installations enabling extended blocklist.de
# narrower ip lists for non-openvz systems only
# that support ipset in system linux kernel
#####################################################
DT=$(date +"%d%m%y-%H%M%S")

denyiplimits() {
if [[ ! -f /proc/user_beancounters && -f /usr/sbin/ipset ]] && [[ "$(uname -r | grep linode)" || "$(find /lib/modules/`uname -r` -name 'ipset')" ]]; then
  echo
  echo "CSF Firewall dynamically optimise DENY_IP_LIMIT"
  echo "and DENY_TEMP_IP_LIMIT based on system resources"
  echo
  CSFTOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  cp -a /etc/csf/csf.conf /etc/csf/csf.conf-$DT
  if [[ "$CSFTOTALMEM" -ge '65000001' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"20000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"30000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '32500001' && "$CSFTOTALMEM" -le '65000000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"15000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"20000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '16250001' && "$CSFTOTALMEM" -le '32500000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"10000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"15000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '8125001' && "$CSFTOTALMEM" -le '16250000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"8000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"10000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '4062501' && "$CSFTOTALMEM" -le '8125000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"6000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"8000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '2045001' && "$CSFTOTALMEM" -le '4062500' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"4000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"5000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '1022501' && "$CSFTOTALMEM" -le '2045000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"1500\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"3000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -le '1022500' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"1000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"2000\"/' /etc/csf/csf.conf
  fi
  csf -ra > /dev/null 2>&1
  echo
  echo "CSF Firewall Tweaks Completed"
  echo
  blocklistde='y'
else
  echo
  echo "Detected either openvz system and/or linux kernel that doesn't support ipset"
  echo "Aborted csf tweaks for DENY_IP_LIMIT & DENY_TEMP_IP_LIMIT & blocklist.de extension"
  echo
  blocklistde='n'
fi
}

blocklistdeb_extended() {
  if [[ "$blocklistde" = [yY] && ! "$(grep 'BDESTRONGIPS' /etc/csf/csf.blocklists)" ]]; then
    echo
    echo "extend /etc/csf/csf.blocklists for blocklist.de options"
    echo "blocklist.de extended options disabled by default"
    echo "you can edit /etc/csf/csf.blocklists and enable"
    echo "blocklist.de chains BDESTRONGIPS and BDEBRUTEFORCE"
    echo "and restart csf firewall and lfd daemon with commands"
    echo "csf -r; service lfd restart"
    echo
    echo "this is a narrower list of blocked ips than CSF default"
    echo "DBEALL chain provided as outlined at"
    echo "http://www.blocklist.de/en/export.html"
    echo
    cp -a /etc/csf/csf.blocklists /etc/csf/csf.blocklists-$DT
cat >> "/etc/csf/csf.blocklists" <<EOF

# If you do not want to use Blocklist.de large IP list from second list for
# BDEALL iptables chain name list, you can use one of these listings
# for narrower set of IPs to block for specific attack types outlined
# here http://www.blocklist.de/en/export.html
# DO NOT enable second list BDEALL as well as duplicating IP blocks by
# enabling below lists. Use second list OR one if the below narrower
# lists NOT both

#IP addresses which have been reported within the last 48 hours as 
#having run attacks on the service SSH. 
#BDESSH|86400|0|https://lists.blocklist.de/lists/ssh.txt

#IP addresses which have been reported within the last 48 hours as 
#having run attacks on the service Mail, Postfix. 
#BDEMAIL|86400|0|https://lists.blocklist.de/lists/mail.txt

#IP addresses which have been reported within the last 48 hours as 
#having run attacks on the service Apache, Apache-DDOS, RFI-Attacks
#BDEAPACHE|86400|0|https://lists.blocklist.de/lists/apache.txt

#IP addresses which have been reported within the last 48 hours 
#for attacks on the Service imap, sasl, pop3
#BDEIMAP|86400|0|https://lists.blocklist.de/lists/imap.txt

#IP addresses which have been reported within the last 48 hours 
#for attacks on the Service FTP.
#BDEFTP|86400|0|https://lists.blocklist.de/lists/ftp.txt

#IP addresses that tried to login in a SIP-, VOIP- or Asterisk-Server
#and are inclueded in the IPs-List from http://www.infiltrated.net/
#BDESIP|86400|0|https://lists.blocklist.de/lists/sip.txt

#IP addresses which have been reported within the last 48 hours as
#having run attacks attacks on the RFI-Attacks, REG-Bots, IRC-Bots 
#or BadBots
#BDEBOTS|86400|0|https://lists.blocklist.de/lists/bots.txt

#IP addresses older then 2 month & have more then 5.000 attacks. 
#BDESTRONGIPS|86400|0|https://lists.blocklist.de/lists/strongips.txt

#IP addresses for ircbot 
#BDEIRCBOT|86400|0|https://lists.blocklist.de/lists/ircbot.txt

#IP addresses which attacks Joomlas, Wordpress and other Web-Logins 
#with Brute-Force Logins
#BDEBRUTEFORCE|86400|0|https://lists.blocklist.de/lists/bruteforcelogin.txt
EOF
  fi
}

blocklistdeb_extendedb() {
  if [[ "$blocklistde" = [yY] && ! "$(grep 'emergingthreats' /etc/csf/csf.blocklists)" ]]; then
    cp -a /etc/csf/csf.blocklists /etc/csf/csf.blocklists-b-$DT
cat >> "/etc/csf/csf.blocklists" <<EOF

# Emerging Threats - Russian Business Networks List
# Details: http://doc.emergingthreats.net/bin/view/Main/RussianBusinessNetwork
#RBN|86400|0|http://rules.emergingthreats.net/fwrules/emerging-Block-IPs.txt
EOF
  fi
}

denyiplimits
blocklistdeb_extended
blocklistdeb_extendedb