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
        csf -d 104.131.0.69 hello.data.shodan.io
        csf -d 104.236.198.48 blog.shodan.io
        csf -d 188.138.9.50 atlantic.census.shodan.io
        csf -d 198.20.69.74 census1.shodan.io
        csf -d 198.20.69.98 census2.shodan.io
        csf -d 198.20.70.114 census3.shodan.io
        csf -d 198.20.87.98 border.census.shodan.io
        csf -d 198.20.99.130 census4.shodan.io
        csf -d 208.180.20.97 shodan.io
        csf -d 209.126.110.38 atlantic.dns.shodan.io
        csf -d 66.240.192.138 census8.shodan.io
        csf -d 66.240.219.146 burger.census.shodan.io
        csf -d 66.240.236.119 census6.shodan.io
        csf -d 71.6.135.131 census7.shodan.io
        csf -d 71.6.146.185 pirate.census.shodan.io
        csf -d 71.6.158.166 ninja.census.shodan.io
        csf -d 71.6.165.200 census12.shodan.io
        csf -d 71.6.167.142 census9.shodan.io
        csf -d 82.221.105.6 census10.shodan.io
        csf -d 82.221.105.7 census11.shodan.io
        csf -d 85.25.103.50 pacific.census.shodan.io
        csf -d 85.25.43.94 rim.census.shodan.io
        csf -d 93.120.27.62 census5.shodan.io
        csf --profile list
    fi

fi
}

