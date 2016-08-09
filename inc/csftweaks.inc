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

csftweaks() {
if [ -f /etc/csf/csf.conf ]; then
  # auto detect which SSHD port is default and auto update it for base
  # csf firewall template
  CSFSSHD_PORT='22'
  DETECTED_PORT=$(awk '/^Port / {print $2}' /etc/ssh/sshd_config)
  if [[ "$DETECTED_PORT" != '22' && -z "$(netstat -plant | grep sshd | grep ':22')" ]]; then
    echo "switching csf.conf SSHD port default from $CSFSSHD_PORT to detected SSHD port $DETECTED_PORT"
    sed -i "s/,${CSFSSHD_PORT},/,${DETECTED_PORT},/" /etc/csf/csf.conf
  fi
  if [[ "$(cat /etc/csf/csf.conf | grep TCP_IN | grep ,,)" ]] && [[ -z "$DETECTED_PORT" && "$(netstat -plant | grep sshd | grep ":${CSFSSHD_PORT}")" ]]; then
    echo "correct bug that removed $CSFSSHD_PORT in CSF firewall TCP_IN entry"
    echo "https://community.centminmod.com/posts/34444/"
    sed -i "s/,,/,${CSFSSHD_PORT},/" /etc/csf/csf.conf
  fi

  # echo "Custom configure CSF settings...set"
  rpcnfsports
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
    sed -i 's/PT_LOAD_LEVEL = "6"/PT_LOAD_LEVEL = "8"/g' /etc/csf/csf.conf
    sed -i 's/LF_FTPD = "10"/LF_FTPD = "3"/g' /etc/csf/csf.conf

    sed -i 's/LF_DISTATTACK = "0"/LF_DISTATTACK = "1"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP = "0"/LF_DISTFTP = "1"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP_UNIQ = "3"/LF_DISTFTP_UNIQ = "6"/g' /etc/csf/csf.conf
    sed -i 's/LF_DISTFTP_PERM = "3600"/LF_DISTFTP_PERM = "6000"/g' /etc/csf/csf.conf

    # enable CSF support of dynamic DNS
    # add your dynamic dns hostnames to /etc/csf/csf.dyndns and restart CSF
    # https://community.centminmod.com/threads/csf-firewall-info.25/page-2#post-10687
    sed -i 's/DYNDNS = \"0\"/DYNDNS = \"300\"/' /etc/csf/csf.conf
    sed -i 's/DYNDNS_IGNORE = \"0\"/DYNDNS_IGNORE = \"1\"/' /etc/csf/csf.conf

    if [[ ! -f /proc/user_beancounters ]] && [[ "$(uname -r | grep linode)" || "$(find /lib/modules/`uname -r` -name 'ipset')" ]]; then
        if [[ ! -f /usr/sbin/ipset ]]; then
            # CSF now has ipset support to offload large IP address numbers 
            # from iptables so uses less server resources to handle many IPs
            # does not work with OpenVZ VPS so only implement for non-OpenVZ
            yum -q -y install ipset ipset-devel
            sed -i 's/LF_IPSET = \"0\"/LF_IPSET = \"1\"/' /etc/csf/csf.conf
            sed -i 's/DENY_IP_LIMIT = \"100\"/DENY_IP_LIMIT = \"3000\"/' /etc/csf/csf.conf
            sed -i 's/DENY_TEMP_IP_LIMIT = \"100\"/DENY_TEMP_IP_LIMIT = \"3000\"/' /etc/csf/csf.conf
        elif [[ -f /usr/sbin/ipset ]]; then
            sed -i 's/LF_IPSET = \"0\"/LF_IPSET = \"1\"/' /etc/csf/csf.conf
            sed -i 's/DENY_IP_LIMIT = \"100\"/DENY_IP_LIMIT = \"3000\"/' /etc/csf/csf.conf
            sed -i 's/DENY_TEMP_IP_LIMIT = \"100\"/DENY_TEMP_IP_LIMIT = \"3000\"/' /etc/csf/csf.conf
        fi
    else
        sed -i 's/LF_IPSET = \"1\"/LF_IPSET = \"0\"/' /etc/csf/csf.conf
        sed -i 's/DENY_IP_LIMIT = \"100\"/DENY_IP_LIMIT = \"200\"/' /etc/csf/csf.conf
        sed -i 's/DENY_TEMP_IP_LIMIT = \"100\"/DENY_TEMP_IP_LIMIT = \"200\"/' /etc/csf/csf.conf
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

fi
}

