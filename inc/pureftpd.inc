pureftpinstall() {

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$DNF_ENABLE" = [yY] ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEREPO_DNF=' --disablerepo=rpmforge'
  else
    DISABLEREPO_DNF=""
  fi
fi

  # detect if vsftpd and proftpd already installed and remove it
  if [[ -f /usr/sbin/vsftpd || "$(rpm -qa vsftpd | grep vsftpd)" ]]; then
    yum -y remove vsftpd${DISABLEREPO_DNF}
  fi
  if [[ -f /usr/sbin/proftpd || "$(rpm -qa proftpd | grep proftpd)" ]]; then
    yum -y remove proftpd${DISABLEREPO_DNF}
  fi
  
  if [ ! -f /usr/bin/pure-pw ]; then
    echo "pure-ftpd not installed"
    echo "installing pure-ftpd"
    if [ "$SECOND_IP" ]; then
      CNIP="$SECOND_IP"
    else
          if [ "$SECOND_IP" ]; then
      CNIP="$SECOND_IP"
    else
      CNIP=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://geoip.centminmod.com/v3 | jq -r '.ip')
    fi
    fi

    time $YUMDNFBIN -q -y install pure-ftpd${DISABLEREPO_DNF}
    cmchkconfig pure-ftpd on
    sed -i 's/LF_FTPD = "10"/LF_FTPD = "3"/g' /etc/csf/csf.conf
    sed -i 's/PORTFLOOD = \"\"/PORTFLOOD = \"21;tcp;20;300\"/g' /etc/csf/csf.conf

    echo "configuring pure-ftpd for virtual user support"
    # backup original /etc/pure-ftpd/pure-ftpd.conf
    cp -a /etc/pure-ftpd/pure-ftpd.conf /etc/pure-ftpd/pure-ftpd.conf.backup
    # tweak /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/# UnixAuthentication  /UnixAuthentication  /' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/VerboseLog .*/VerboseLog yes/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/# PureDB                        \/etc\/pure-ftpd\/pureftpd.pdb/PureDB                        \/etc\/pure-ftpd\/pureftpd.pdb/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/# CreateHomeDir .*/CreateHomeDir               yes/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/# TLS .*/TLS                      2/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/# PassivePortRange .*/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's/^PassivePortRange    3000 3050/PassivePortRange    30001 50011/' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's|MaxClientsNumber .*|MaxClientsNumber            1000|' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's|MaxClientsPerIP .*|MaxClientsPerIP             500|' /etc/pure-ftpd/pure-ftpd.conf
    sed -i 's|NoAnonymous .*|NoAnonymous                 yes|' /etc/pure-ftpd/pure-ftpd.conf


    # fix default file/directory permissions
    sed -i 's/Umask .*/Umask                       137:027/' /etc/pure-ftpd/pure-ftpd.conf

    # ensure TLS Cipher preference protects against poodle attacks
    # sed -i 's/# TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:+SSLv3/TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:!SSLv3/' /etc/pure-ftpd/pure-ftpd.conf

    if [[ "$(grep 'TLSCipherSuite' /etc/pure-ftpd/pure-ftpd.conf | grep -o HIGH)" != 'HIGH' ]]; then
      # echo 'TLSCipherSuite           HIGH:MEDIUM:+TLSv1:!SSLv2:!SSLv3' >> /etc/pure-ftpd/pure-ftpd.conf
      sed -i 's|# TLSCipherSuite .*|TLSCipherSuite               HIGH|' /etc/pure-ftpd/pure-ftpd.conf
    fi

    # check if /etc/pure-ftpd/pureftpd.passwd exists
    if [ ! -f /etc/pure-ftpd/pureftpd.passwd ]; then
      touch /etc/pure-ftpd/pureftpd.passwd
      chmod 0600 /etc/pure-ftpd/pureftpd.passwd
      pure-pw mkdb
    fi

    # generate /etc/pure-ftpd/pureftpd.pdb
    if [ ! -f /etc/pure-ftpd/pureftpd.pdb ]; then
      pure-pw mkdb
    fi

    # check tweaks were made
    echo
    sdiff -s -w200 /etc/pure-ftpd/pure-ftpd.conf /etc/pure-ftpd/pure-ftpd.conf.backup
    cat /etc/pure-ftpd/pure-ftpd.conf | egrep 'UnixAuthentication|VerboseLog|PureDB |CreateHomeDir|TLS|PassivePortRange|TLSCipherSuite|MaxClientsNumber|MaxClientsPerIP|NoAnonymous|Umask'

    echo
    echo "generating self-signed ssl certificate..."
    echo "FTP client needs to use FTP (explicit SSL) mode"
    echo "to connect to server's main ip address on port 21"
    sleep 4
    # echo "just hit enter at each prompt until complete"
    # setup self-signed ssl certs
    mkdir -p /etc/ssl/private
    # time openssl dhparam -out /etc/ssl/private/pure-ftpd-dhparams.pem 2048
    openssl req -x509 -days 7300 -sha256 -nodes -subj "/C=US/ST=California/L=Los Angeles/O=Default Company Ltd/CN=$CNIP" -newkey rsa:2048 -keyout /etc/pki/pure-ftpd/pure-ftpd.pem -out /etc/pki/pure-ftpd/pure-ftpd.pem
    chmod 600 /etc/pki/pure-ftpd/*.pem
    openssl x509 -in /etc/pki/pure-ftpd/pure-ftpd.pem -text -noout
    echo 
    # ls -lah /etc/ssl/private/
    ls -lah /etc/pki/pure-ftpd
    echo
    echo "self-signed ssl cert generated"

    pureftpd_symlink_fix
      
    echo "pure-ftpd installed"
    cmservice pure-ftpd restart
    csf -r

    echo
    echo "check /etc/pure-ftpd/pureftpd.passwd"
    ls -lah /etc/pure-ftpd/pureftpd.passwd

    echo
    echo "check /etc/pure-ftpd/pureftpd.pdb"
    ls -lah /etc/pure-ftpd/pureftpd.pdb

    echo
  fi
}