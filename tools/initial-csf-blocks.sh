#!/bin/bash
######################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
######################################################
# standalone initial CSF Firewall setup IP block script
# allow re-running these blocks once CSF Firewall limits
# are reached for DENY_IP_LIMIT & DENY_TEMP_IP_LIMIT
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
DIR_TMP=/svr-setup
CENTMINLOGDIR='/root/centminlogs'
CSF_PERMABAN_LISTDIR='/etc/centminmod/csf'
GET_DENY_IP_LIMIT=$(egrep '^DENY_IP_LIMIT' /etc/csf/csf.conf | awk -F' = ' '{print $2}' | tr -d '"')
THRESHOLD_DENY_IP_LIMIT=$(echo "$GET_DENY_IP_LIMIT - 100" | bc)
GET_DENY_TEMP_IP_LIMIT=$(egrep '^DENY_TEMP_IP_LIMIT' /etc/csf/csf.conf | awk -F' = ' '{print $2}' | tr -d '"')
THRESHOLD_DENY_TEMP_IP_LIMIT=$(echo "$GET_DENY_TEMP_IP_LIMIT - 100" | bc)

if [ ! -d "$CSF_PERMABAN_LISTDIR" ]; then
  mkdir -p "$CSF_PERMABAN_LISTDIR"
fi

if [ -f "${CSF_PERMABAN_LISTDIR}/csf-permaban.conf" ]; then
  CSF_PERMABAN_LIST=$(cat "${CSF_PERMABAN_LISTDIR}/csf-permaban.conf")
fi

echo
echo "Check /etc/csf/csf.conf"
echo "DENY_IP_LIMIT: $GET_DENY_IP_LIMIT"
echo "DENY_TEMP_IP_LIMIT: $GET_DENY_TEMP_IP_LIMIT"
if [ -f /etc/csf/csf.deny ]; then
  CSF_COUNT_IPS=$(wc -l < /etc/csf/csf.deny)
else
  CSF_COUNT_IPS='0'
fi
echo "Number of CSF Firewall Blocked IPs: $CSF_COUNT_IPS"

initial_csf_blocks() {
  if [[ "$(egrep 'censys|shodan' -c /etc/csf/csf.deny)" -ne '85' ]] || [[ ! "$(grep 'perma' /etc/csf/csf.deny)" && "$(wc -l < ${CSF_PERMABAN_LISTDIR}/csf-permaban.conf)" -gt '1' ]] || [[ ! "$(grep 'shodan' /etc/csf/csf.deny)" ]] || [[ ! "$(grep 'censys' /etc/csf/csf.deny)" ]] || [[ "$CSF_COUNT_IPS" -ge "$GET_DENY_IP_LIMIT" && "$(systemctl is-enabled csf)" = 'enabled' && -f /etc/csf/csf.deny ]]; then
        echo
        echo "Re-apply initial CSF Firewall set of IP blocks once CSF Firewall"
        echo "DENY_IP_LIMIT & DENY_TEMP_IP_LIMIT IP limits are reached"
        if [ -f "${CSF_PERMABAN_LISTDIR}/csf-permaban.conf" ]; then
          echo
          # Get the epoch time for 24 hours ago
          TIME_24_HOURS_AGO=$(($(date +%s) - 86400))
          # Get the epoch time of the last modification of the last backup file
          LAST_BACKUP_FILE=$(ls -1t /var/lib/csf/backup/*_cmm_b4_permaban_block_tool | head -n 1)
          LAST_BACKUP_TIME=$(stat -c %Y "$LAST_BACKUP_FILE")
      
          # Check if the last backup time is less than the epoch time for 24 hours ago
          if (( LAST_BACKUP_TIME <= TIME_24_HOURS_AGO )); then
            echo
            if [ "$(wc -l < ${CSF_PERMABAN_LISTDIR}/csf-permaban.conf)" -gt '1' ]; then
              csf --profile backup cmm-b4-permaban-block-tool
            fi
          else
            echo "Backup file is not older than 24 hours, skipping csf --profile backup cmm-b4-permaban-block-tool command."
          fi
          for ip in $CSF_PERMABAN_LIST; 
          do
            if [[ "$(ipcalc -c "$ip" >/dev/null 2>&1; echo $?)" -eq '0' ]] && [[ ! "$(grep "$ip" /etc/csf/csf.deny)" ]]; then
              csf -d $ip permaban
            fi
          done
        fi
    if [[ "$(egrep 'censys|shodan' -c /etc/csf/csf.deny)" -ne '85' ]] || [[ ! "$(grep 'shodan' /etc/csf/csf.deny)" ]] || [[ ! "$(grep 'censys' /etc/csf/csf.deny)" ]] || [[ "$CSF_COUNT_IPS" -ge "$GET_DENY_IP_LIMIT" && "$(systemctl is-enabled csf)" = 'enabled' && -f /etc/csf/csf.deny ]]; then
        csf --profile backup cmm-b4-censys-block-tool
        # block censys.io scans
        # https://support.censys.io/getting-started/frequently-asked-questions-faq
        csf -d 141.212.121.0/24 censys
        csf -d 141.212.122.0/24 censys
        # https://whois.arin.net/rest/org/CENSY/nets
        csf -d 198.108.66.0/23 censys
        csf -d 198.108.204.216/29 censys
        csf -d 162.142.125.0/24 censys
        csf -d 167.248.133.0/24 censys
        csf -d 167.94.138.0/24 censys
        csf -d 167.94.145.0/24 censys
        csf -d 167.94.146.0/24 censys
        csf -d 192.35.168.0/23 censys
        csf -d 2602:80D:1000::/44 censys
        csf -d 2620:96:E000::/48 censys
        csf -d 74.120.14.0/24 censys

        csf --profile backup cmm-b4-shodan-block-tool
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
        csf -d 188.138.9.50 atlantic481.serverprofi24.com
    fi
  else
    echo
    echo "DENY_IP_LIMIT & DENY_TEMP_IP_LIMIT IP limits not reached"
    echo "nothing to do"
    exit
  fi
}

######################################################
{
	initial_csf_blocks
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_initial_csf_blocks_${DT}.log