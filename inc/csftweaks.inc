reduceports() {
    PORTS_TOCHECK='3000 3334 8080 8888 9312 6081 6082 30865'
    # reduce number of whitelisted open ports setup by csf install routine
    # double check if any services are listening on these ports before removal
    for p in $PORTS_TOCHECK; do
        if [[ -z "$(netstat -plnt | awk '{print $4}' | grep "$p")" ]]; then
            # grep --color ",$p," /etc/csf/csf.conf
            sed -i "s|,$p,|,|g" /etc/csf/csf.conf
            # sed -e "s|,$p,|,|g" /etc/csf/csf.conf |egrep --color '^TCP_|^TCP6_|^UDP_|^UDP6_'
        fi
    done
    # csf -ra >/dev/null 2>&1
}

rpcnfsports() {
    # remove RPC/portmapper and NFS ports 111 and 2049 
    # from CSF Firewall whitelist by default so that folks who 
    # only need it can open it up to specific server IP addresses
    # see advance CSF Firewall rules at https://community.centminmod.com/posts/3731/
    # examples for allowing 111, 2049 TCP/UDP ports for only 
    # source/destination IP = 11.22.33.44 to be added to /etc/csf/csf.allow
    # restart of CSF service is required
    # 
    # tcp|in|d=111|s=11.22.33.44
    # tcp|in|d=2049|s=11.22.33.44
    # tcp|out|d=111|d=11.22.33.44
    # tcp|out|d=2049|d=11.22.33.44
    # udp|in|d=111|s=11.22.33.44
    # udp|in|d=2049|s=11.22.33.44
    # udp|out|d=111|d=11.22.33.44
    # udp|out|d=2049|d=11.22.33.44
    
    # only remove ports 111, 2049 from TCP/UDP whitelist if detected NFS 
    # package not installed
    if [[ ! -z "$(rpm -ql nfs-utils | grep 'not installed')" && -f /etc/csf/csf.conf ]]; then
        if [[ "$INITIALINSTALL" = [yY] ]]; then 
            echo
            echo "Before RPC/NFS port tweak"
            egrep '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
            egrep --color ',111,|,2049,' /etc/csf/csf.conf
        fi
        if [[ "$INITIALINSTALL" = [yY] ]]; then 
            sed -e 's|,111,|,|g' /etc/csf/csf.conf | egrep --color ',111,|,2049,'
            sed -e 's|,2049,|,|g' /etc/csf/csf.conf | egrep --color ',111,|,2049,'
        fi
        
        sed -i 's|,111,|,|g' /etc/csf/csf.conf
        sed -i 's|,2049,|,|g' /etc/csf/csf.conf
        
        if [[ "$INITIALINSTALL" = [yY] ]]; then 
            echo
            echo "After RPC/NFS port tweak"
            egrep --color ',111,|,2049,' /etc/csf/csf.conf
            egrep '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
        fi
    fi
}

setiplimits(){
    # only applies to non-openvz systems which have ipset detected support otherwise
    # DENY_IP_LIMIT & DENY_TEMP_IP_LIMIT are both set to 200 for openvz systems or
    # not-openvz systems without a linux kernel to support ipset hashed ip sets
  CSFTOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
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
}

csftweaks() {
if [ -f /etc/csf/csf.conf ]; then
  # auto detect which SSHD port is default and auto update it for base
  # csf firewall template
  CSFSSHD_PORT='22'
  DETECTED_PORT=$(awk '/^Port / {print $2}' /etc/ssh/sshd_config)
  if [[ "$DETECTED_PORT" != '22' && -z "$(netstat -plant | grep sshd | grep ':22')" ]] && [[ -z "$(netstat -plant | grep sshd | grep ":$DETECTED_PORT")" ]]; then
    echo "switching csf.conf SSHD port default from $CSFSSHD_PORT to detected SSHD port $DETECTED_PORT"
    sed -i "s/,${CSFSSHD_PORT},/,${DETECTED_PORT},/" /etc/csf/csf.conf
  fi
  if [[ "$(cat /etc/csf/csf.conf | grep TCP_IN | grep ',,')" ]] && [[ "$(netstat -plant | grep sshd | grep ":${CSFSSHD_PORT}")" ]]; then
    echo "correct bug that removed $CSFSSHD_PORT in CSF firewall TCP_IN entry"
    echo "https://community.centminmod.com/posts/34444/"
    sed -i "s/\,\,/,${CSFSSHD_PORT},/" /etc/csf/csf.conf
  fi

  # echo "Custom configure CSF settings...set"
  rpcnfsports
  reduceports
  if [ ! -z "$EMAIL" ]; then
    sed -i "s/LF_ALERT_TO = \"\"/LF_ALERT_TO = \"$EMAIL\"/g" /etc/csf/csf.conf
  fi
    #sed -i 's/LF_TRIGGER = "0"/LF_TRIGGER = "1"/g' /etc/csf/csf.conf
    sed -i 's|USE_CONNTRACK = "1"|USE_CONNTRACK = "0"|g' /etc/csf/csf.conf
    sed -i 's/LF_IPSET = "0"/LF_IPSET = "1"/g' /etc/csf/csf.conf
    sed -i 's/LF_DSHIELD = "0"/LF_DSHIELD = "86400"/g' /etc/csf/csf.conf
    sed -i 's/LF_SPAMHAUS = "0"/LF_SPAMHAUS = "86400"/g' /etc/csf/csf.conf
    sed -i 's/LF_EXPLOIT = "300"/LF_EXPLOIT = "86400"/g' /etc/csf/csf.conf
    sed -i 's/LF_DIRWATCH = "300"/LF_DIRWATCH = "86400"/g' /etc/csf/csf.conf
    sed -i 's/LF_INTEGRITY = "3600"/LF_INTEGRITY = "0"/g' /etc/csf/csf.conf
    sed -i 's/LF_PARSE = "5"/LF_PARSE = "20"/g' /etc/csf/csf.conf
    sed -i 's/LF_PARSE = "600"/LF_PARSE = "20"/g' /etc/csf/csf.conf
    sed -i 's/PS_LIMIT = "10"/PS_LIMIT = "15"/g' /etc/csf/csf.conf
    sed -i 's/PT_LIMIT = "60"/PT_LIMIT = "0"/g' /etc/csf/csf.conf
    sed -i 's/PT_USERPROC = "10"/PT_USERPROC = "0"/g' /etc/csf/csf.conf
    sed -i 's/PT_USERMEM = "200"/PT_USERMEM = "0"/g' /etc/csf/csf.conf
    sed -i 's/PT_USERTIME = "1800"/PT_USERTIME = "0"/g' /etc/csf/csf.conf
    sed -i 's/PT_LOAD = "30"/PT_LOAD = "600"/g' /etc/csf/csf.conf
    sed -i 's/PT_LOAD_AVG = "5"/PT_LOAD_AVG = "15"/g' /etc/csf/csf.conf
    sed -i "s/^PT_LOAD_LEVEL .*/PT_LOAD_LEVEL = \"$(nproc)\"/g" /etc/csf/csf.conf
    sed -i 's/LF_FTPD = "10"/LF_FTPD = "3"/g' /etc/csf/csf.conf

    sed -i 's/LF_DISTATTACK = "0"/LF_DISTATTACK = "1"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP = "0"/LF_DISTFTP = "40"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP_UNIQ = "3"/LF_DISTFTP_UNIQ = "40"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP_PERM = "3600"/LF_DISTFTP_PERM = "6000"/g' /etc/csf/csf.conf

    # enable CSF support of dynamic DNS
    # add your dynamic dns hostnames to /etc/csf/csf.dyndns and restart CSF
    # https://community.centminmod.com/threads/csf-firewall-info.25/page-2#post-10687
    sed -i 's/DYNDNS = \"0\"/DYNDNS = \"300\"/' /etc/csf/csf.conf
    sed -i 's/DYNDNS_IGNORE = \"0\"/DYNDNS_IGNORE = \"1\"/' /etc/csf/csf.conf

    csf_loadalert

    if [[ ! -f /proc/user_beancounters ]] && [[ "$(uname -r | grep linode)" || "$(find /lib/modules/`uname -r` -name 'ipset')" ]]; then
        if [[ ! -f /usr/sbin/ipset ]]; then
            # CSF now has ipset support to offload large IP address numbers 
            # from iptables so uses less server resources to handle many IPs
            # does not work with OpenVZ VPS so only implement for non-OpenVZ
            yum -q -y install ipset ipset-devel
            sed -i 's/LF_IPSET = \"0\"/LF_IPSET = \"1\"/' /etc/csf/csf.conf
            setiplimits
        elif [[ -f /usr/sbin/ipset ]]; then
            sed -i 's/LF_IPSET = \"0\"/LF_IPSET = \"1\"/' /etc/csf/csf.conf
            setiplimits
        fi
    else
        sed -i 's/LF_IPSET = \"1\"/LF_IPSET = \"0\"/' /etc/csf/csf.conf
        sed -i 's/^DENY_IP_LIMIT = .*/DENY_IP_LIMIT = \"200\"/' /etc/csf/csf.conf
        sed -i 's/^DENY_TEMP_IP_LIMIT = .*/DENY_TEMP_IP_LIMIT = \"200\"/' /etc/csf/csf.conf
    fi

    sed -i 's/UDPFLOOD = \"0\"/UDPFLOOD = \"1\"/g' /etc/csf/csf.conf
    sed -i 's/UDPFLOOD_ALLOWUSER = \"named\"/UDPFLOOD_ALLOWUSER = \"named nsd\"/g' /etc/csf/csf.conf

    # whitelist the SSH client IP from initial installation to prevent some
    # instances of end user IP being blocked from CSF Firewall
    if [[ "$INITIALINSTALL" = [yY] ]]; then
        CMUSER_SSHCLIENTIP=$(echo $SSH_CLIENT | awk '{print $1}' | head -n1)
        csf -a $CMUSER_SSHCLIENTIP # initialinstall_userip
        echo "$CMUSER_SSHCLIENTIP" >> /etc/csf/csf.ignore
    fi

    if [[ "$INITIALINSTALL" = [yY] ]]; then
        cp -a /etc/csf/csf.conf /etc/csf/csf.conf-tweakbak
        csf --profile backup cmm-default-tweaked
        csf --profile backup cmm-b4-censys-block
        # block censys.io scans
        # https://support.censys.io/getting-started/frequently-asked-questions-faq
        csf -d 141.212.121.0/24 censys
        csf -d 141.212.122.0/24 censys
        csf --profile backup cmm-b4-shodan-block
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
        # https://isc.sans.edu/api/threatcategory/research/?json
        # cat other-nov2018.txt  | jq -r '.[] | "\(.ipv4) \(.type)"' | grep -v shodan| sort
        # csf --profile backup cmm-b4-other-scanners-block
        # csf -d 107.150.52.82 blindferret
        # csf -d 107.150.52.84 blindferret
        # csf -d 107.150.52.85 blindferret
        # csf -d 107.150.52.86 blindferret
        # csf -d 141.212.121.0 univmichigan
        # csf -d 141.212.121.100 univmichigan
        # csf -d 141.212.121.101 univmichigan
        # csf -d 141.212.121.102 univmichigan
        # csf -d 141.212.121.103 univmichigan
        # csf -d 141.212.121.104 univmichigan
        # csf -d 141.212.121.105 univmichigan
        # csf -d 141.212.121.106 univmichigan
        # csf -d 141.212.121.107 univmichigan
        # csf -d 141.212.121.108 univmichigan
        # csf -d 141.212.121.109 univmichigan
        # csf -d 141.212.121.10 univmichigan
        # csf -d 141.212.121.110 univmichigan
        # csf -d 141.212.121.111 univmichigan
        # csf -d 141.212.121.112 univmichigan
        # csf -d 141.212.121.113 univmichigan
        # csf -d 141.212.121.114 univmichigan
        # csf -d 141.212.121.115 univmichigan
        # csf -d 141.212.121.116 univmichigan
        # csf -d 141.212.121.117 univmichigan
        # csf -d 141.212.121.118 univmichigan
        # csf -d 141.212.121.119 univmichigan
        # csf -d 141.212.121.11 univmichigan
        # csf -d 141.212.121.120 univmichigan
        # csf -d 141.212.121.121 univmichigan
        # csf -d 141.212.121.122 univmichigan
        # csf -d 141.212.121.123 univmichigan
        # csf -d 141.212.121.124 univmichigan
        # csf -d 141.212.121.125 univmichigan
        # csf -d 141.212.121.126 univmichigan
        # csf -d 141.212.121.127 univmichigan
        # csf -d 141.212.121.128 univmichigan
        # csf -d 141.212.121.129 univmichigan
        # csf -d 141.212.121.12 univmichigan
        # csf -d 141.212.121.130 univmichigan
        # csf -d 141.212.121.131 univmichigan
        # csf -d 141.212.121.132 univmichigan
        # csf -d 141.212.121.133 univmichigan
        # csf -d 141.212.121.134 univmichigan
        # csf -d 141.212.121.135 univmichigan
        # csf -d 141.212.121.136 univmichigan
        # csf -d 141.212.121.137 univmichigan
        # csf -d 141.212.121.138 univmichigan
        # csf -d 141.212.121.139 univmichigan
        # csf -d 141.212.121.13 univmichigan
        # csf -d 141.212.121.140 univmichigan
        # csf -d 141.212.121.141 univmichigan
        # csf -d 141.212.121.142 univmichigan
        # csf -d 141.212.121.143 univmichigan
        # csf -d 141.212.121.144 univmichigan
        # csf -d 141.212.121.145 univmichigan
        # csf -d 141.212.121.146 univmichigan
        # csf -d 141.212.121.147 univmichigan
        # csf -d 141.212.121.148 univmichigan
        # csf -d 141.212.121.149 univmichigan
        # csf -d 141.212.121.14 univmichigan
        # csf -d 141.212.121.150 univmichigan
        # csf -d 141.212.121.151 univmichigan
        # csf -d 141.212.121.152 univmichigan
        # csf -d 141.212.121.153 univmichigan
        # csf -d 141.212.121.154 univmichigan
        # csf -d 141.212.121.155 univmichigan
        # csf -d 141.212.121.156 univmichigan
        # csf -d 141.212.121.157 univmichigan
        # csf -d 141.212.121.158 univmichigan
        # csf -d 141.212.121.159 univmichigan
        # csf -d 141.212.121.15 univmichigan
        # csf -d 141.212.121.160 univmichigan
        # csf -d 141.212.121.161 univmichigan
        # csf -d 141.212.121.162 univmichigan
        # csf -d 141.212.121.163 univmichigan
        # csf -d 141.212.121.164 univmichigan
        # csf -d 141.212.121.165 univmichigan
        # csf -d 141.212.121.166 univmichigan
        # csf -d 141.212.121.167 univmichigan
        # csf -d 141.212.121.168 univmichigan
        # csf -d 141.212.121.169 univmichigan
        # csf -d 141.212.121.16 univmichigan
        # csf -d 141.212.121.170 univmichigan
        # csf -d 141.212.121.171 univmichigan
        # csf -d 141.212.121.172 univmichigan
        # csf -d 141.212.121.173 univmichigan
        # csf -d 141.212.121.174 univmichigan
        # csf -d 141.212.121.175 univmichigan
        # csf -d 141.212.121.176 univmichigan
        # csf -d 141.212.121.177 univmichigan
        # csf -d 141.212.121.178 univmichigan
        # csf -d 141.212.121.179 univmichigan
        # csf -d 141.212.121.17 univmichigan
        # csf -d 141.212.121.180 univmichigan
        # csf -d 141.212.121.181 univmichigan
        # csf -d 141.212.121.182 univmichigan
        # csf -d 141.212.121.183 univmichigan
        # csf -d 141.212.121.184 univmichigan
        # csf -d 141.212.121.185 univmichigan
        # csf -d 141.212.121.186 univmichigan
        # csf -d 141.212.121.187 univmichigan
        # csf -d 141.212.121.188 univmichigan
        # csf -d 141.212.121.189 univmichigan
        # csf -d 141.212.121.18 univmichigan
        # csf -d 141.212.121.190 univmichigan
        # csf -d 141.212.121.191 univmichigan
        # csf -d 141.212.121.192 univmichigan
        # csf -d 141.212.121.193 univmichigan
        # csf -d 141.212.121.194 univmichigan
        # csf -d 141.212.121.195 univmichigan
        # csf -d 141.212.121.196 univmichigan
        # csf -d 141.212.121.197 univmichigan
        # csf -d 141.212.121.198 univmichigan
        # csf -d 141.212.121.199 univmichigan
        # csf -d 141.212.121.19 univmichigan
        # csf -d 141.212.121.1 univmichigan
        # csf -d 141.212.121.200 univmichigan
        # csf -d 141.212.121.201 univmichigan
        # csf -d 141.212.121.202 univmichigan
        # csf -d 141.212.121.203 univmichigan
        # csf -d 141.212.121.204 univmichigan
        # csf -d 141.212.121.205 univmichigan
        # csf -d 141.212.121.206 univmichigan
        # csf -d 141.212.121.207 univmichigan
        # csf -d 141.212.121.208 univmichigan
        # csf -d 141.212.121.209 univmichigan
        # csf -d 141.212.121.20 univmichigan
        # csf -d 141.212.121.210 univmichigan
        # csf -d 141.212.121.211 univmichigan
        # csf -d 141.212.121.212 univmichigan
        # csf -d 141.212.121.213 univmichigan
        # csf -d 141.212.121.214 univmichigan
        # csf -d 141.212.121.215 univmichigan
        # csf -d 141.212.121.216 univmichigan
        # csf -d 141.212.121.217 univmichigan
        # csf -d 141.212.121.218 univmichigan
        # csf -d 141.212.121.219 univmichigan
        # csf -d 141.212.121.21 univmichigan
        # csf -d 141.212.121.220 univmichigan
        # csf -d 141.212.121.221 univmichigan
        # csf -d 141.212.121.222 univmichigan
        # csf -d 141.212.121.223 univmichigan
        # csf -d 141.212.121.224 univmichigan
        # csf -d 141.212.121.225 univmichigan
        # csf -d 141.212.121.226 univmichigan
        # csf -d 141.212.121.227 univmichigan
        # csf -d 141.212.121.228 univmichigan
        # csf -d 141.212.121.229 univmichigan
        # csf -d 141.212.121.22 univmichigan
        # csf -d 141.212.121.230 univmichigan
        # csf -d 141.212.121.231 univmichigan
        # csf -d 141.212.121.232 univmichigan
        # csf -d 141.212.121.233 univmichigan
        # csf -d 141.212.121.234 univmichigan
        # csf -d 141.212.121.235 univmichigan
        # csf -d 141.212.121.236 univmichigan
        # csf -d 141.212.121.237 univmichigan
        # csf -d 141.212.121.238 univmichigan
        # csf -d 141.212.121.239 univmichigan
        # csf -d 141.212.121.23 univmichigan
        # csf -d 141.212.121.240 univmichigan
        # csf -d 141.212.121.241 univmichigan
        # csf -d 141.212.121.242 univmichigan
        # csf -d 141.212.121.243 univmichigan
        # csf -d 141.212.121.244 univmichigan
        # csf -d 141.212.121.245 univmichigan
        # csf -d 141.212.121.246 univmichigan
        # csf -d 141.212.121.247 univmichigan
        # csf -d 141.212.121.248 univmichigan
        # csf -d 141.212.121.249 univmichigan
        # csf -d 141.212.121.24 univmichigan
        # csf -d 141.212.121.250 univmichigan
        # csf -d 141.212.121.251 univmichigan
        # csf -d 141.212.121.252 univmichigan
        # csf -d 141.212.121.253 univmichigan
        # csf -d 141.212.121.254 univmichigan
        # csf -d 141.212.121.255 univmichigan
        # csf -d 141.212.121.25 univmichigan
        # csf -d 141.212.121.26 univmichigan
        # csf -d 141.212.121.27 univmichigan
        # csf -d 141.212.121.28 univmichigan
        # csf -d 141.212.121.29 univmichigan
        # csf -d 141.212.121.2 univmichigan
        # csf -d 141.212.121.30 univmichigan
        # csf -d 141.212.121.31 univmichigan
        # csf -d 141.212.121.32 univmichigan
        # csf -d 141.212.121.33 univmichigan
        # csf -d 141.212.121.34 univmichigan
        # csf -d 141.212.121.35 univmichigan
        # csf -d 141.212.121.36 univmichigan
        # csf -d 141.212.121.37 univmichigan
        # csf -d 141.212.121.38 univmichigan
        # csf -d 141.212.121.39 univmichigan
        # csf -d 141.212.121.3 univmichigan
        # csf -d 141.212.121.40 univmichigan
        # csf -d 141.212.121.41 univmichigan
        # csf -d 141.212.121.42 univmichigan
        # csf -d 141.212.121.43 univmichigan
        # csf -d 141.212.121.44 univmichigan
        # csf -d 141.212.121.45 univmichigan
        # csf -d 141.212.121.46 univmichigan
        # csf -d 141.212.121.47 univmichigan
        # csf -d 141.212.121.48 univmichigan
        # csf -d 141.212.121.49 univmichigan
        # csf -d 141.212.121.4 univmichigan
        # csf -d 141.212.121.50 univmichigan
        # csf -d 141.212.121.51 univmichigan
        # csf -d 141.212.121.52 univmichigan
        # csf -d 141.212.121.53 univmichigan
        # csf -d 141.212.121.54 univmichigan
        # csf -d 141.212.121.55 univmichigan
        # csf -d 141.212.121.56 univmichigan
        # csf -d 141.212.121.57 univmichigan
        # csf -d 141.212.121.58 univmichigan
        # csf -d 141.212.121.59 univmichigan
        # csf -d 141.212.121.5 univmichigan
        # csf -d 141.212.121.60 univmichigan
        # csf -d 141.212.121.61 univmichigan
        # csf -d 141.212.121.62 univmichigan
        # csf -d 141.212.121.63 univmichigan
        # csf -d 141.212.121.64 univmichigan
        # csf -d 141.212.121.65 univmichigan
        # csf -d 141.212.121.66 univmichigan
        # csf -d 141.212.121.67 univmichigan
        # csf -d 141.212.121.68 univmichigan
        # csf -d 141.212.121.69 univmichigan
        # csf -d 141.212.121.6 univmichigan
        # csf -d 141.212.121.70 univmichigan
        # csf -d 141.212.121.71 univmichigan
        # csf -d 141.212.121.72 univmichigan
        # csf -d 141.212.121.73 univmichigan
        # csf -d 141.212.121.74 univmichigan
        # csf -d 141.212.121.75 univmichigan
        # csf -d 141.212.121.76 univmichigan
        # csf -d 141.212.121.77 univmichigan
        # csf -d 141.212.121.78 univmichigan
        # csf -d 141.212.121.79 univmichigan
        # csf -d 141.212.121.7 univmichigan
        # csf -d 141.212.121.80 univmichigan
        # csf -d 141.212.121.81 univmichigan
        # csf -d 141.212.121.82 univmichigan
        # csf -d 141.212.121.83 univmichigan
        # csf -d 141.212.121.84 univmichigan
        # csf -d 141.212.121.85 univmichigan
        # csf -d 141.212.121.86 univmichigan
        # csf -d 141.212.121.87 univmichigan
        # csf -d 141.212.121.88 univmichigan
        # csf -d 141.212.121.89 univmichigan
        # csf -d 141.212.121.8 univmichigan
        # csf -d 141.212.121.90 univmichigan
        # csf -d 141.212.121.91 univmichigan
        # csf -d 141.212.121.92 univmichigan
        # csf -d 141.212.121.93 univmichigan
        # csf -d 141.212.121.94 univmichigan
        # csf -d 141.212.121.95 univmichigan
        # csf -d 141.212.121.96 univmichigan
        # csf -d 141.212.121.97 univmichigan
        # csf -d 141.212.121.98 univmichigan
        # csf -d 141.212.121.99 univmichigan
        # csf -d 141.212.121.9 univmichigan
        # csf -d 184.105.139.100 shadowserver
        # csf -d 184.105.139.101 shadowserver
        # csf -d 184.105.139.102 shadowserver
        # csf -d 184.105.139.105 shadowserver
        # csf -d 184.105.139.126 shadowserver
        # csf -d 184.105.139.66 shadowserver
        # csf -d 184.105.139.67 shadowserver
        # csf -d 184.105.139.68 shadowserver
        # csf -d 184.105.139.69 shadowserver
        # csf -d 184.105.139.70 shadowserver
        # csf -d 184.105.139.71 shadowserver
        # csf -d 184.105.139.72 shadowserver
        # csf -d 184.105.139.73 shadowserver
        # csf -d 184.105.139.74 shadowserver
        # csf -d 184.105.139.75 shadowserver
        # csf -d 184.105.139.76 shadowserver
        # csf -d 184.105.139.77 shadowserver
        # csf -d 184.105.139.78 shadowserver
        # csf -d 184.105.139.79 shadowserver
        # csf -d 184.105.139.80 shadowserver
        # csf -d 184.105.139.81 shadowserver
        # csf -d 184.105.139.82 shadowserver
        # csf -d 184.105.139.83 shadowserver
        # csf -d 184.105.139.84 shadowserver
        # csf -d 184.105.139.85 shadowserver
        # csf -d 184.105.139.86 shadowserver
        # csf -d 184.105.139.87 shadowserver
        # csf -d 184.105.139.88 shadowserver
        # csf -d 184.105.139.89 shadowserver
        # csf -d 184.105.139.90 shadowserver
        # csf -d 184.105.139.91 shadowserver
        # csf -d 184.105.139.92 shadowserver
        # csf -d 184.105.139.93 shadowserver
        # csf -d 184.105.139.94 shadowserver
        # csf -d 184.105.139.95 shadowserver
        # csf -d 184.105.139.96 shadowserver
        # csf -d 184.105.139.97 shadowserver
        # csf -d 184.105.139.98 shadowserver
        # csf -d 184.105.247.194 shadowserver
        # csf -d 184.105.247.195 shadowserver
        # csf -d 184.105.247.196 shadowserver
        # csf -d 184.105.247.197 shadowserver
        # csf -d 184.105.247.198 shadowserver
        # csf -d 184.105.247.199 shadowserver
        # csf -d 184.105.247.200 shadowserver
        # csf -d 184.105.247.201 shadowserver
        # csf -d 184.105.247.202 shadowserver
        # csf -d 184.105.247.203 shadowserver
        # csf -d 184.105.247.204 shadowserver
        # csf -d 184.105.247.205 shadowserver
        # csf -d 184.105.247.206 shadowserver
        # csf -d 184.105.247.207 shadowserver
        # csf -d 184.105.247.208 shadowserver
        # csf -d 184.105.247.209 shadowserver
        # csf -d 184.105.247.210 shadowserver
        # csf -d 184.105.247.211 shadowserver
        # csf -d 184.105.247.212 shadowserver
        # csf -d 184.105.247.213 shadowserver
        # csf -d 184.105.247.214 shadowserver
        # csf -d 184.105.247.215 shadowserver
        # csf -d 184.105.247.216 shadowserver
        # csf -d 184.105.247.217 shadowserver
        # csf -d 184.105.247.218 shadowserver
        # csf -d 184.105.247.219 shadowserver
        # csf -d 184.105.247.220 shadowserver
        # csf -d 184.105.247.221 shadowserver
        # csf -d 184.105.247.222 shadowserver
        # csf -d 184.105.247.223 shadowserver
        # csf -d 184.105.247.224 shadowserver
        # csf -d 184.105.247.225 shadowserver
        # csf -d 184.105.247.226 shadowserver
        # csf -d 184.105.247.227 shadowserver
        # csf -d 184.105.247.228 shadowserver
        # csf -d 184.105.247.229 shadowserver
        # csf -d 209.126.230.71 erratasec
        # csf -d 209.126.230.72 erratasec
        # csf -d 209.126.230.73 erratasec
        # csf -d 209.126.230.74 erratasec
        # csf -d 209.126.230.75 erratasec
        # csf -d 209.126.230.76 erratasec
        # csf -d 216.218.206.100 shadowserver
        # csf -d 216.218.206.101 shadowserver
        # csf -d 216.218.206.66 shadowserver
        # csf -d 216.218.206.67 shadowserver
        # csf -d 216.218.206.68 shadowserver
        # csf -d 216.218.206.69 shadowserver
        # csf -d 216.218.206.70 shadowserver
        # csf -d 216.218.206.71 shadowserver
        # csf -d 216.218.206.72 shadowserver
        # csf -d 216.218.206.73 shadowserver
        # csf -d 216.218.206.74 shadowserver
        # csf -d 216.218.206.75 shadowserver
        # csf -d 216.218.206.76 shadowserver
        # csf -d 216.218.206.77 shadowserver
        # csf -d 216.218.206.78 shadowserver
        # csf -d 216.218.206.79 shadowserver
        # csf -d 216.218.206.80 shadowserver
        # csf -d 216.218.206.81 shadowserver
        # csf -d 216.218.206.82 shadowserver
        # csf -d 216.218.206.83 shadowserver
        # csf -d 216.218.206.84 shadowserver
        # csf -d 216.218.206.85 shadowserver
        # csf -d 216.218.206.86 shadowserver
        # csf -d 216.218.206.87 shadowserver
        # csf -d 216.218.206.88 shadowserver
        # csf -d 216.218.206.89 shadowserver
        # csf -d 216.218.206.90 shadowserver
        # csf -d 216.218.206.91 shadowserver
        # csf -d 216.218.206.92 shadowserver
        # csf -d 216.218.206.93 shadowserver
        # csf -d 216.218.206.94 shadowserver
        # csf -d 216.218.206.95 shadowserver
        # csf -d 216.218.206.96 shadowserver
        # csf -d 216.218.206.97 shadowserver
        # csf -d 216.218.206.98 shadowserver
        # csf -d 216.218.206.99 shadowserver
        # csf -d 52.211.122.16 cybergreen
        # csf -d 52.73.169.169 cybergreen
        # csf -d 54.249.206.188 cybergreen
        # csf -d 65.49.20.100 shadowserver
        # csf -d 65.49.20.101 shadowserver
        # csf -d 65.49.20.66 shadowserver
        # csf -d 65.49.20.67 shadowserver
        # csf -d 65.49.20.68 shadowserver
        # csf -d 65.49.20.69 shadowserver
        # csf -d 65.49.20.70 shadowserver
        # csf -d 65.49.20.71 shadowserver
        # csf -d 65.49.20.72 shadowserver
        # csf -d 65.49.20.73 shadowserver
        # csf -d 65.49.20.74 shadowserver
        # csf -d 65.49.20.75 shadowserver
        # csf -d 65.49.20.76 shadowserver
        # csf -d 65.49.20.77 shadowserver
        # csf -d 65.49.20.78 shadowserver
        # csf -d 65.49.20.79 shadowserver
        # csf -d 65.49.20.80 shadowserver
        # csf -d 65.49.20.81 shadowserver
        # csf -d 65.49.20.82 shadowserver
        # csf -d 65.49.20.83 shadowserver
        # csf -d 65.49.20.84 shadowserver
        # csf -d 65.49.20.85 shadowserver
        # csf -d 65.49.20.86 shadowserver
        # csf -d 65.49.20.87 shadowserver
        # csf -d 65.49.20.88 shadowserver
        # csf -d 65.49.20.89 shadowserver
        # csf -d 65.49.20.90 shadowserver
        # csf -d 65.49.20.91 shadowserver
        # csf -d 65.49.20.92 shadowserver
        # csf -d 65.49.20.93 shadowserver
        # csf -d 65.49.20.94 shadowserver
        # csf -d 65.49.20.95 shadowserver
        # csf -d 65.49.20.96 shadowserver
        # csf -d 65.49.20.97 shadowserver
        # csf -d 65.49.20.98 shadowserver
        # csf -d 65.49.20.99 shadowserver
        # csf -d 71.6.216.32 rapid7sonar
        # csf -d 71.6.216.33 rapid7sonar
        # csf -d 71.6.216.34 rapid7sonar
        # csf -d 71.6.216.35 rapid7sonar
        # csf -d 71.6.216.36 rapid7sonar
        # csf -d 71.6.216.37 rapid7sonar
        # csf -d 71.6.216.38 rapid7sonar
        # csf -d 71.6.216.39 rapid7sonar
        # csf -d 71.6.216.40 rapid7sonar
        # csf -d 71.6.216.41 rapid7sonar
        # csf -d 71.6.216.42 rapid7sonar
        # csf -d 71.6.216.43 rapid7sonar
        # csf -d 71.6.216.44 rapid7sonar
        # csf -d 71.6.216.45 rapid7sonar
        # csf -d 71.6.216.46 rapid7sonar
        # csf -d 71.6.216.47 rapid7sonar
        # csf -d 71.6.216.48 rapid7sonar
        # csf -d 71.6.216.49 rapid7sonar
        # csf -d 71.6.216.50 rapid7sonar
        # csf -d 71.6.216.51 rapid7sonar
        # csf -d 71.6.216.52 rapid7sonar
        # csf -d 71.6.216.53 rapid7sonar
        # csf -d 71.6.216.54 rapid7sonar
        # csf -d 71.6.216.55 rapid7sonar
        # csf -d 71.6.216.56 rapid7sonar
        # csf -d 71.6.216.57 rapid7sonar
        # csf -d 71.6.216.58 rapid7sonar
        # csf -d 71.6.216.59 rapid7sonar
        # csf -d 71.6.216.60 rapid7sonar
        # csf -d 71.6.216.61 rapid7sonar
        # csf -d 71.6.216.62 rapid7sonar
        # csf -d 71.6.216.63 rapid7sonar
        # csf -d 74.82.47.10 shadowserver
        # csf -d 74.82.47.11 shadowserver
        # csf -d 74.82.47.12 shadowserver
        # csf -d 74.82.47.13 shadowserver
        # csf -d 74.82.47.14 shadowserver
        # csf -d 74.82.47.15 shadowserver
        # csf -d 74.82.47.16 shadowserver
        # csf -d 74.82.47.17 shadowserver
        # csf -d 74.82.47.18 shadowserver
        # csf -d 74.82.47.19 shadowserver
        # csf -d 74.82.47.20 shadowserver
        # csf -d 74.82.47.21 shadowserver
        # csf -d 74.82.47.22 shadowserver
        # csf -d 74.82.47.23 shadowserver
        # csf -d 74.82.47.24 shadowserver
        # csf -d 74.82.47.25 shadowserver
        # csf -d 74.82.47.26 shadowserver
        # csf -d 74.82.47.27 shadowserver
        # csf -d 74.82.47.28 shadowserver
        # csf -d 74.82.47.29 shadowserver
        # csf -d 74.82.47.2 shadowserver
        # csf -d 74.82.47.30 shadowserver
        # csf -d 74.82.47.31 shadowserver
        # csf -d 74.82.47.32 shadowserver
        # csf -d 74.82.47.33 shadowserver
        # csf -d 74.82.47.34 shadowserver
        # csf -d 74.82.47.35 shadowserver
        # csf -d 74.82.47.36 shadowserver
        # csf -d 74.82.47.37 shadowserver
        # csf -d 74.82.47.3 shadowserver
        # csf -d 74.82.47.41 shadowserver
        # csf -d 74.82.47.4 shadowserver
        # csf -d 74.82.47.5 shadowserver
        # csf -d 74.82.47.6 shadowserver
        # csf -d 74.82.47.7 shadowserver
        # csf -d 74.82.47.8 shadowserver
        # csf -d 74.82.47.9 shadowserver
        # whitelisting IPs for downloads/services Centmin Mod relies on
        csf --profile backup cmm-b4-whitelist
        # whitelist CSF Firewall's download url otherwise unable to download CSF Firewall updates
        dig +short A download.configserver.com | while read i; do csf -a $i csf-download.configserver.com; done
        # whitelist centminmod.com IPs which Centmin Mod LEMP stack relies on for some downloaded 
        # dependencies and file download updates
        dig +short A centminmod.com | while read i; do csf -a $i centminmod.com; done
        # whitelist nginx.org download IPs
        dig +short A nginx.org | while read i; do csf -a $i nginx.org; done
        csf --profile backup cmm-after-whitelist
        csf --profile list
    fi

fi
}

