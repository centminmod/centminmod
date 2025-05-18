sshdtweaks_ten() {
  if [[ "$CENTOS_TEN" -eq '10' && "$SSHD_REENABLE_ROOT_LOGINS" = [yY] ]]; then
    echo ""
    cecho "--------------------------------------------------------" $boldyellow
    cecho "sshd config tweak to re-enable el9 support for: PermitRootLogin yes" $boldyellow
    cecho "to remove this change, delete: /etc/ssh/sshd_config.d/01-permitrootlogin.conf" $boldyellow
    # re-enable and allow root user SSH logins
    # el9 disabled them by default
    if [[ "$(sshd -T | awk '/permitrootlogin/ {print $2}')" != 'yes' ]]; then
      echo "PermitRootLogin yes" > /etc/ssh/sshd_config.d/01-permitrootlogin.conf
    fi
    cecho "--------------------------------------------------------" $boldyellow
  fi
}

sshdtweaks_nine() {
  if [[ "$CENTOS_NINE" -eq '9' && "$SSHD_REENABLE_ROOT_LOGINS" = [yY] ]]; then
    echo ""
    cecho "--------------------------------------------------------" $boldyellow
    cecho "sshd config tweak to re-enable el9 support for: PermitRootLogin yes" $boldyellow
    cecho "to remove this change, delete: /etc/ssh/sshd_config.d/01-permitrootlogin.conf" $boldyellow
    # re-enable and allow root user SSH logins
    # el9 disabled them by default
    if [[ "$(sshd -T | awk '/permitrootlogin/ {print $2}')" != 'yes' ]]; then
      echo "PermitRootLogin yes" > /etc/ssh/sshd_config.d/01-permitrootlogin.conf
    fi
    cecho "--------------------------------------------------------" $boldyellow
  fi
}

sshdtweaks() {
  # uncomment sshd default port if there's only one entry #Port in sshd_config
  if [[ "$(grep '^#Port' /etc/ssh/sshd_config)" && "$(egrep '^#Port|^Port' /etc/ssh/sshd_config | wc -l)" -eq '1' ]]; then
    sed -i 's/^#Port /Port /g' /etc/ssh/sshd_config
  fi
  if [[ ! "$(grep '^X11Forwarding no' /etc/ssh/sshd_config)" ]]; then
    sed -i 's|^X11Forwarding .*|X11Forwarding no|' /etc/ssh/sshd_config
  fi
  # https://access.redhat.com/security/cve/cve-2024-6387
  if [[ ! "$(grep '^LoginGraceTime' /etc/ssh/sshd_config)" && "$(grep '^#LoginGraceTime' /etc/ssh/sshd_config)" ]]; then
    sed -i 's|^#LoginGraceTime .*|LoginGraceTime 0|' /etc/ssh/sshd_config
  fi

  CHECKHOSTKEY=$(grep '#HostKey /etc/ssh/ssh_host_rsa_key' /etc/ssh/sshd_config)
  CHECKHOSTKEYB=$(grep '#HostKey /etc/ssh/ssh_host_ecdsa_key' /etc/ssh/sshd_config)
  CHECKHOSTKEYC=$(grep '#PubkeyAuthentication' /etc/ssh/sshd_config)
  CHECKHOSTKEYD=$(grep '#PermitEmptyPasswords' /etc/ssh/sshd_config)

  if [[ ! -z "$CHECKHOSTKEY" || ! -z "$CHECKHOSTKEYB" || ! -z "$CHECKHOSTKEYC" || ! -z "$CHECKHOSTKEYD" ]]; then
    sed -i 's/#HostKey \/etc\/ssh\/ssh_host_rsa_key/HostKey \/etc\/ssh\/ssh_host_rsa_key/g' /etc/ssh/sshd_config
    # sed -i 's/#HostKey \/etc\/ssh\/ssh_host_dsa_key/HostKey \/etc\/ssh\/ssh_host_dsa_key/g' /etc/ssh/sshd_config
    sed -i 's/#HostKey \/etc\/ssh\/ssh_host_ecdsa_key/HostKey \/etc\/ssh\/ssh_host_ecdsa_key/g' /etc/ssh/sshd_config
    sed -i 's/#PubkeyAuthentication/PubkeyAuthentication/g' /etc/ssh/sshd_config
    # sed -i 's/#RSAAuthentication/RSAAuthentication/g' /etc/ssh/sshd_config
    sed -i 's/#AuthorizedKeysFile/AuthorizedKeysFile/g' /etc/ssh/sshd_config
    sed -i 's/#IgnoreRhosts yes/IgnoreRhosts yes/g' /etc/ssh/sshd_config
    sed -i 's/#PermitEmptyPasswords no/PermitEmptyPasswords no/g' /etc/ssh/sshd_config
    sed -i 's/GSSAPIAuthentication yes/GSSAPIAuthentication no/g' /etc/ssh/sshd_config
    sed -i 's/GSSAPICleanupCredentials yes/GSSAPICleanupCredentials no/g' /etc/ssh/sshd_config
  fi
  
  CHECKTCPALIVE=$(grep '#TCPKeepAlive yes' /etc/ssh/sshd_config)
  
  if [[ ! -z "$CHECKTCPALIVE" ]]; then
    sed -i 's/#TCPKeepAlive yes/TCPKeepAlive yes/g' /etc/ssh/sshd_config
  fi
  
  CHECKVALUES=$(grep 'ClientAliveInterval 3600' /etc/ssh/sshd_config)
  
  if [[ -z "$CHECKVALUES" ]]; then
    echo -e "\n" >> /etc/ssh/sshd_config
    echo "ClientAliveInterval 3600" >> /etc/ssh/sshd_config
    echo "ClientAliveCountMax 120" >> /etc/ssh/sshd_config
    echo "UseDNS no" >> /etc/ssh/sshd_config
    #echo "ServerAliveInterval 120" >> /etc/ssh/sshd_config
    #echo "ServerAliveCountMax 120" >> /etc/ssh/sshd_config
  fi

  # centos 6.x openssh 5.3p1 backported ecdsa keys support but
  # not configured out of the box https://community.centminmod.com/posts/19702/
  # this sets up ecdh-sha2-* key exchange support
  if [[ "$CENTOS_SIX" -eq '6' && ! -f /etc/ssh/ssh_host_ecdsa_key ]]; then
    echo "ssh-keygen -q -N "" -t ecdsa -f /etc/ssh/ssh_host_ecdsa_key"
    ssh-keygen -q -N "" -t ecdsa -f /etc/ssh/ssh_host_ecdsa_key
  
    if [[ "$(grep ssh_host_ecdsa_key /etc/ssh/sshd_config)" ]]; then
      echo "/etc/ssh/ssh_host_ecdsa_key already setup in /etc/ssh/sshd_config"
    else
      echo "/etc/ssh/ssh_host_ecdsa_key setup in /etc/ssh/sshd_config"
      sed -i 's|^HostKey \/etc\/ssh\/ssh_host_rsa_key|HostKey \/etc\/ssh\/ssh_host_ecdsa_key\nHostKey \/etc\/ssh\/ssh_host_rsa_key|' /etc/ssh/sshd_config
      grep ssh_host_ecdsa_key /etc/ssh/sshd_config
      echo -e "\nKexAlgorithms ecdh-sha2-nistp521,ecdh-sha2-nistp384,ecdh-sha2-nistp256,diffie-hellman-group-exchange-sha256,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1,diffie-hellman-group1-sha1" >> /etc/ssh/sshd_config
    fi
  fi
  if [[ "$CENTOS_SEVEN" -eq '7' && ! "$(egrep '^KexAlgorithms|^Ciphers|^MACs' /etc/ssh/sshd_config)" ]]; then
    # improve openssh security https://infosec.mozilla.org/guidelines/openssh
cat >> /etc/ssh/sshd_config <<EOF

KexAlgorithms curve25519-sha256@libssh.org,ecdh-sha2-nistp521,ecdh-sha2-nistp384,ecdh-sha2-nistp256,diffie-hellman-group-exchange-sha256
Ciphers chacha20-poly1305@openssh.com,aes256-gcm@openssh.com,aes128-gcm@openssh.com,aes256-ctr,aes192-ctr,aes128-ctr
MACs hmac-sha2-512-etm@openssh.com,hmac-sha2-256-etm@openssh.com,umac-128-etm@openssh.com,hmac-sha2-512,hmac-sha2-256,umac-128@openssh.com
EOF
    # echo "awk '$5 >= 3071' /etc/ssh/moduli > /etc/ssh/moduli.tmp && mv -f /etc/ssh/moduli.tmp /etc/ssh/moduli"
    awk '$5 >= 3071' /etc/ssh/moduli > /etc/ssh/moduli.tmp && mv -f /etc/ssh/moduli.tmp /etc/ssh/moduli
  fi
  if [[ "$CENTOS_SEVEN" -eq '7' && "$(egrep '^PrintMotd noKexAlgorithms' /etc/ssh/sshd_config)" ]]; then
    # fix https://community.centminmod.com/posts/75607/
    sed -i 's|^PrintMotd noKexAlgorithms|PrintMotd no\nKexAlgorithms|' /etc/ssh/sshd_config
  fi
  
  if [[ ! -z "$CHECKHOSTKEY" || ! -z "$CHECKHOSTKEYB" || ! -z "$CHECKHOSTKEYC" || ! -z "$CHECKHOSTKEYD" || ! -z "$CHECKTCPALIVE" || -z "$CHECKVALUES" ]]; then
    # echo "----------------------------"
    # echo "Configure sshd_config"
    # echo "----------------------------"
    /usr/sbin/sshd -t
    service sshd restart >/dev/null 2>&1
  fi
}

function funct_sshd {

CURRENTSSHDPORT=$(echo ${SSH_CLIENT##* })
CHECK_CURRENTPORT=$(awk '/^Port / {print $2}' /etc/ssh/sshd_config)

if [ -z "$CHECK_CURRENTPORT" ]; then
  CHECK_CURRENTPORT=$(awk '/#Port/ {print $2}' /etc/ssh/sshd_config | head -n1)
fi

if [[ "$CURRENTSSHDPORT" = "$CHECK_CURRENTPORT" ]]; then
  CURRENTSSHDPORT=$(echo ${SSH_CLIENT##* })
else
  CURRENTSSHDPORT=$CHECK_CURRENTPORT
fi

echo "*************************************************"
cecho "* Setup sshd" $boldgreen
echo "*************************************************"
echo
read -ep "Do you want to continue changing your default SSH port? [y/n]: " sshdport_changeport

if [[ "$sshdport_changeport" != [yY] ]]; then
  echo
  echo "aborting SSH port change menu..."
  exit
fi

echo "--------------------------"
echo "backup sshd_config"
echo "--------------------------"
echo "cp -a /etc/ssh/sshd_config /etc/ssh/sshd_config-backup.${DT}"

cp -a /etc/ssh/sshd_config /etc/ssh/sshd_config-backup.${DT}

echo "--------------------------"
echo "change ssh port"
echo "--------------------------"
cecho "You'll be asked what your current default SSH port is (default = 22 or $CURRENTSSHDPORT)." $boldyellow
echo
cecho "Then asked which SSH port number you want to change to." $boldyellow
cecho "New SSH port should be a number below and less than <1024." $boldyellow
sleep 15
echo

cecho "Your current default SSH port is: $CURRENTSSHDPORT" $boldyellow
cecho "or $CHECK_CURRENTPORT" $boldyellow
echo ""

read -ep "Enter existing SSH port number (default = 22 for fresh installs): " EXISTPORTNUM

# safeguard checks that existing port is entered and matches the system
# detected sshd existing port
while [[ "$CURRENTSSHDPORT" != "$EXISTPORTNUM" ]]; do
  echo
  cecho "Error: you did not enter your existing SSH port number = $CURRENTSSHDPORT !" $boldyellow
  cecho "or $CHECK_CURRENTPORT" $boldyellow
  echo
  read -ep "Enter existing SSH port number (default = 22 for fresh installs or $CURRENTSSHDPORT): " EXISTPORTNUM
done

echo ""

read -ep "Enter the SSH port number you want to change to: " PORTNUM

if [ -f /etc/csf/csf.conf ]; then
  CHECKNEWPORTNUM=$(cat /etc/csf/csf.conf | egrep '^TCP_IN|^UDP_IN|^TCP6_IN|^UDP6_IN' | grep -w $PORTNUM)
  if [[ "$CHECKNEWPORTNUM" ]]; then
    echo "the new port $PORTNUM is already in use by other software defined by CSF Firewall"
    echo "please re-run menu option 16 and try a different port"
    echo "aborting..."
    exit
  fi
fi

# safeguard checks that existing port is entered and matches the system
# detected sshd existing port
if [[ "$CURRENTSSHDPORT" = "$EXISTPORTNUM" ]]; then
  if [ "$(grep '#Port' /etc/ssh/sshd_config)" ]; then
    sed -i 's/#Port /Port /g' /etc/ssh/sshd_config
    sed -i 's/Port.*[0-9]$/Port '$PORTNUM'/gI' /etc/ssh/sshd_config   
  elif [ "$(grep '^Port' /etc/ssh/sshd_config)" ]; then
    #echo "Port $PORTNUM" >> /etc/ssh/sshd_config
    sed -i 's/Port.*[0-9]$/Port '$PORTNUM'/gI' /etc/ssh/sshd_config   
  fi
  
  echo ""
  echo "Post $PORTNUM configured in /etc/ssh/sshd_config"
  grep "Port $PORTNUM" /etc/ssh/sshd_config
  echo ""
fi

sshdtweaks

# safeguard checks that existing port is entered and matches the system
# detected sshd existing port
if [[ "$CURRENTSSHDPORT" = "$EXISTPORTNUM" ]]; then
  echo "iptables -I INPUT -p tcp --dport $EXISTPORTNUM -j REJECT"
  iptables -I INPUT -p tcp --dport $EXISTPORTNUM -j REJECT
  
  echo "iptables -I INPUT -p tcp --dport $PORTNUM -j ACCEPT"
  iptables -I INPUT -p tcp --dport $PORTNUM -j ACCEPT
  
  cmservice iptables restart
fi

# safeguard checks that existing port is entered and matches the system
# detected sshd existing port
if [[ "$CURRENTSSHDPORT" = "$EXISTPORTNUM" ]]; then
  if [ -f /etc/csf/csf.conf ]; then
  #check if portnum already exists
  EXISTPORTCHECK=$(cat /etc/csf/csf.conf | egrep '^TCP_IN' | grep ",${PORTNUM},")
  EXISTPORTCHECKED=${EXISTPORTCHECK:-novalue}
  EXISTPORTCHECK_SIX=$(cat /etc/csf/csf.conf | egrep '^TCP6_IN' | grep ",${PORTNUM},")
  EXISTPORTCHECKED_SIX=${EXISTPORTCHECK_SIX:-novalue}
    
    if [[ "$EXISTPORTCHECKED" = 'novalue' && "$EXISTPORTCHECKED_SIX" = 'novalue' ]]; then
      # IPv4
      TCPIN=$(cat /etc/csf/csf.conf | grep ^TCP_IN)
      TCPINNEW=$(cat /etc/csf/csf.conf | grep ^TCP_IN | sed -e "s/,${EXISTPORTNUM},/,${PORTNUM},/")
      sed -i "s/$TCPIN/$TCPINNEW/g" /etc/csf/csf.conf
      # IPv6
      TCPINSIX=$(cat /etc/csf/csf.conf | grep ^TCP6_IN)
      TCPINSIXNEW=$(cat /etc/csf/csf.conf | grep ^TCP6_IN | sed -e "s/,${EXISTPORTNUM},/,${PORTNUM},/")
      sed -i "s/$TCPINSIX/$TCPINSIXNEW/g" /etc/csf/csf.conf
      # don't need to change TCP_OUT or you would have problems connecting to remote ssh servers
      # on port 22
      # TCPOUT=$(cat /etc/csf/csf.conf | grep ^TCP_OUT)
      # TCPOUTNEW=$(cat /etc/csf/csf.conf | grep ^TCP_OUT | sed -e "s/,${EXISTPORTNUM},/,${PORTNUM},/")
      # sed -i "s/$TCPOUT/$TCPOUTNEW/g" /etc/csf/csf.conf
      cmservice csf restart
      csf -r
    else
      echo ""
      echo "Port Number $PORTNUM already exists, please pick another port"
      exit
    fi
  fi
  
  service sshd restart >/dev/null 2>&1
fi # safeguard line 129

if [[ "$CURRENTSSHDPORT" = "$EXISTPORTNUM" ]]; then
echo ""
echo "*************************************************"
cecho "* Setup sshd complete" $boldgreen
echo "*************************************************"
echo "To check to see if you can access your server via the new port"
echo "keep this existing SSH2 connection open, and start a new SSH2" 
echo "connection to this server connecting via the new SSH2 port"
echo "if you can connect, then it's working. "
echo ""
echo "If you can't connect, using your existing SSH2 logged in"
echo "connection, edit and check /etc/csf/csf.conf and your iptables"
echo "/etc/sysconfig/iptables and service iptables status"
echo "making sure the new port number you specified is correctly set"
echo "*************************************************"
fi

}
