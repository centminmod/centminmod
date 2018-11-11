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
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"120000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"130000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '32500001' && "$CSFTOTALMEM" -le '65000000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"60000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"80000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '16250001' && "$CSFTOTALMEM" -le '32500000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"30000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"45000\"/' /etc/csf/csf.conf
  elif [[ "$CSFTOTALMEM" -gt '8125001' && "$CSFTOTALMEM" -le '16250000' ]]; then
    sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"16000\"/' /etc/csf/csf.conf
    sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"20000\"/' /etc/csf/csf.conf
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
#RBN|86400|0|https://rules.emergingthreats.net/fwrules/emerging-Block-IPs.txt
EOF
  fi
}

additional_blocks() {
  csf --profile backup cmm-b4-additional-censys-block
  # block censys.io scans
  # https://support.censys.io/getting-started/frequently-asked-questions-faq
  csf -d 141.212.121.0/24 censys
  csf -d 141.212.122.0/24 censys
  csf --profile backup cmm-b4-additional-shodan-block
  # block shodan scans
  # https://wiki.ipfire.org/configuration/firewall/blockshodan
  # http://romcheckfail.com/blocking-shodan-keeping-shodan-io-in-the-dark-from-scanning/
  # https://isc.sans.edu/api/threatlist/shodan/
  # https://isc.sans.edu/api/threatlist/shodan/?json
  # curl -s https://isc.sans.edu/api/threatlist/shodan/?json > isc-shodan.txt
  # cat isc-shodan.txt  | jq -r '.[] .ipv4'
  csf -d 104.131.0.69 hello.data.shodan.io
  csf -d 104.236.198.48 blog.shodan.io
  csf -d 185.163.109.66 goldfish.census.shodan.io
  csf -d 185.181.102.18 turtle.census.shodan.io
  csf -d 188.138.9.50 atlantic.census.shodan.io
  csf -d 198.20.69.72 census1.shodan.io
  csf -d 198.20.69.73 census1.shodan.io
  csf -d 198.20.69.74 census1.shodan.io
  csf -d 198.20.69.75 census1.shodan.io
  csf -d 198.20.69.76 census1.shodan.io
  csf -d 198.20.69.77 census1.shodan.io
  csf -d 198.20.69.78 census1.shodan.io
  csf -d 198.20.69.79 census1.shodan.io
  csf -d 198.20.69.96 census2.shodan.io
  csf -d 198.20.69.97 census2.shodan.io
  csf -d 198.20.69.98 census2.shodan.io
  csf -d 198.20.69.99 census2.shodan.io
  csf -d 198.20.69.100 census2.shodan.io
  csf -d 198.20.69.101 census2.shodan.io
  csf -d 198.20.69.102 census2.shodan.io
  csf -d 198.20.69.103 census2.shodan.io
  csf -d 198.20.70.111 census3.shodan.io
  csf -d 198.20.70.112 census3.shodan.io
  csf -d 198.20.70.113 census3.shodan.io
  csf -d 198.20.70.114 census3.shodan.io
  csf -d 198.20.70.115 census3.shodan.io
  csf -d 198.20.70.116 census3.shodan.io
  csf -d 198.20.70.117 census3.shodan.io
  csf -d 198.20.70.118 census3.shodan.io
  csf -d 198.20.70.119 census3.shodan.io
  csf -d 198.20.99.128 census4.shodan.io
  csf -d 198.20.99.129 census4.shodan.io
  csf -d 198.20.99.130 census4.shodan.io
  csf -d 198.20.99.131 census4.shodan.io
  csf -d 198.20.99.132 census4.shodan.io
  csf -d 198.20.99.133 census4.shodan.io
  csf -d 198.20.99.134 census4.shodan.io
  csf -d 198.20.99.135 census4.shodan.io
  csf -d 93.120.27.62 census5.shodan.io
  csf -d 66.240.236.119 census6.shodan.io
  csf -d 71.6.135.131 census7.shodan.io
  csf -d 66.240.192.138 census8.shodan.io
  csf -d 71.6.167.142 census9.shodan.io
  csf -d 82.221.105.6 census10.shodan.io
  csf -d 82.221.105.7 census11.shodan.io
  csf -d 71.6.165.200 census12.shodan.io
  csf -d 216.117.2.180 census13.shodan.io
  csf -d 198.20.87.98 border.census.shodan.io
  csf -d 208.180.20.97 shodan.io
  csf -d 209.126.110.38 atlantic.dns.shodan.io
  csf -d 66.240.219.146 burger.census.shodan.io
  csf -d 71.6.146.185 pirate.census.shodan.io
  csf -d 71.6.158.166 ninja.census.shodan.io
  csf -d 85.25.103.50 pacific.census.shodan.io
  csf -d 71.6.146.186 inspire.census.shodan.io
  csf -d 85.25.43.94 rim.census.shodan.io
  csf -d 89.248.167.131 mason.census.shodan.io
  csf -d 89.248.172.16 house.census.shodan.io
  csf -d 93.174.95.106 battery.census.shodan.io
  csf -d 198.20.87.96 border.census.shodan.io
  csf -d 198.20.87.97 border.census.shodan.io
  csf -d 198.20.87.98 border.census.shodan.io
  csf -d 198.20.87.99 border.census.shodan.io
  csf -d 198.20.87.100 border.census.shodan.io
  csf -d 198.20.87.101 border.census.shodan.io
  csf -d 198.20.87.102 border.census.shodan.io
  csf -d 198.20.87.103 border.census.shodan.io
  csf -d 94.102.49.190 flower.census.shodan.io
  csf -d 94.102.49.193 cloud.census.shodan.io
  csf -d 71.6.146.130 refrigerator.census.shodan.io
  csf -d 159.203.176.62 private.shodan.io
  csf -d 188.138.1.119 atlantic249.serverprofi24.com
  csf -d 80.82.77.33 sky.census.shodan.io
  csf -d 80.82.77.139 dojo.census.shodan.io
  csf -d 66.240.205.34 malware-hunter.census.shodan.io
  csf -d 93.120.27.62 lavender.mrtaggy.com
  csf -d 188.138.9.50 atlantic481.serverprofi24.com
  csf -d 85.25.43.94 atlantic756.dedicatedpanel.com
  csf -d 85.25.103.50 atlantic836.serverprofi24.com
  # whitelisting IPs for downloads/services Centmin Mod relies on
  csf --profile backup cmm-b4-additional-whitelist
  # whitelist CSF Firewall's download url otherwise unable to download CSF Firewall updates
  dig +short A download.configserver.com | while read i; do csf -a $i csf-download.configserver.com; done
  # whitelist centminmod.com IPs which Centmin Mod LEMP stack relies on for some downloaded 
  # dependencies and file download updates
  dig +short A centminmod.com | while read i; do csf -a $i centminmod.com; done
  # whitelist nginx.org download IPs
  dig +short A nginx.org | while read i; do csf -a $i nginx.org; done
  csf --profile backup cmm-after-additional-whitelist
  csf --profile list
}

denyiplimits
blocklistdeb_extended
blocklistdeb_extendedb
additional_blocks