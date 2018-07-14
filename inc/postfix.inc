postfix_presetup() {

  if [ ! -f /bin/mailx ]; then
    time $YUMDNFBIN -y install mailx${DISABLEREPO_DNF}
  fi

if [[ "$CENTOS_SIX" = '6' ]]; then
  if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [nN] ]]; then
    if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
      echo "*************************************************"
      cecho "* Installing sendmail" $boldgreen
      echo "*************************************************"
      $YUMDNFBIN${CACHESKIP} -y -q install sendmail sendmail-cf${DISABLEREPO_DNF}
            sar_call
      chkconfig --levels 235 sendmail on
      funct_sendmailmc
      echo "*************************************************"
      cecho "* sendmail installed" $boldgreen
      echo "*************************************************"
    elif [[ -f /etc/init.d/postfix ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, sendmail install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /etc/init.d/sendmail ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          chkconfig --levels 235 sendmail on
          funct_sendmailmc
          echo "*************************************************"
          cecho "sendmail already detected, sendmail install aborted" $boldgreen
          echo "*************************************************"
    fi
  fi
  
  if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [yY] ]]; then
      if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
          echo "*************************************************"
          cecho "* Installing postfix" $boldgreen
          echo "*************************************************"
          $YUMDNFBIN${CACHESKIP} -y -q install postfix${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      elif [[ -f /etc/init.d/postfix ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, postfix install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /etc/init.d/sendmail ]]; then
          $YUMDNFBIN${CACHESKIP} -y -q remove sendmail sendmail-cf${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      fi
  fi
  
  
  if [[ "$POSTFIX_INSTALL" = [yY] && "$SENDMAIL_INSTALL" = [nN] ]]; then
      if [[ ! -f /etc/init.d/postfix && ! -f /etc/init.d/sendmail ]]; then
          echo "*************************************************"
          cecho "* Installing postfix" $boldgreen
          echo "*************************************************"
          $YUMDNFBIN${CACHESKIP} -y -q install postfix${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      elif [[ -f /etc/init.d/postfix ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, postfix install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /etc/init.d/sendmail ]]; then
          if [ -f /usr/share/sendmail-cf ]; then
              $YUMDNFBIN${CACHESKIP} -y -q remove sendmail sendmail-cf${DISABLEREPO_DNF}
                sar_call
          else
            $YUMDNFBIN${CACHESKIP} -y -q remove sendmail${DISABLEREPO_DNF}
            sar_call
          fi
          $YUMDNFBIN${CACHESKIP} -y -q install postfix${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      fi
  fi
fi # centos6

if [[ "$CENTOS_SEVEN" = '7' ]]; then
  if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [nN] ]]; then
    if [[ ! -f /usr/lib/systemd/system/postfix.service && ! -f /usr/lib/systemd/system/sendmail.service ]]; then
      echo "*************************************************"
      cecho "* Installing sendmail" $boldgreen
      echo "*************************************************"
      $YUMDNFBIN${CACHESKIP} -y -q install sendmail sendmail-cf${DISABLEREPO_DNF}
            sar_call
      chkconfig --levels 235 sendmail on
      funct_sendmailmc
      echo "*************************************************"
      cecho "* sendmail installed" $boldgreen
      echo "*************************************************"
    elif [[ -f /usr/lib/systemd/system/postfix.service ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, sendmail install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /usr/lib/systemd/system/sendmail.service ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          chkconfig --levels 235 sendmail on
          funct_sendmailmc
          echo "*************************************************"
          cecho "sendmail already detected, sendmail install aborted" $boldgreen
          echo "*************************************************"
    fi
  fi
  
  if [[ "$SENDMAIL_INSTALL" = [yY] && "$POSTFIX_INSTALL" = [yY] ]]; then
      if [[ ! -f /usr/lib/systemd/system/postfix.service && ! -f /usr/lib/systemd/system/sendmail.service ]]; then
          echo "*************************************************"
          cecho "* Installing postfix" $boldgreen
          echo "*************************************************"
          $YUMDNFBIN${CACHESKIP} -y -q install postfix mailx${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      elif [[ -f /usr/lib/systemd/system/postfix.service ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, postfix install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /usr/lib/systemd/system/sendmail.service ]]; then
          $YUMDNFBIN${CACHESKIP} -y -q remove sendmail sendmail-cf${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      fi
  fi
  
  
  if [[ "$POSTFIX_INSTALL" = [yY] && "$SENDMAIL_INSTALL" = [nN] ]]; then
      if [[ ! -f /usr/lib/systemd/system/postfix.service && ! -f /usr/lib/systemd/system/sendmail.service ]]; then
          echo "*************************************************"
          cecho "* Installing postfix" $boldgreen
          echo "*************************************************"
          $YUMDNFBIN${CACHESKIP} -y -q install postfix${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      elif [[ -f /usr/lib/systemd/system/postfix.service ]]; then
          if [ ! -f /bin/mailx ]; then
              $YUMDNFBIN${CACHESKIP} -y -q install mailx${DISABLEREPO_DNF}
                sar_call
          fi
          postfixsetup
          echo "*************************************************"
          cecho "Postfix already detected, postfix install aborted" $boldgreen
          echo "*************************************************"
      elif [[ -f /usr/lib/systemd/system/sendmail.service ]]; then
          if [ -f /usr/share/sendmail-cf ]; then
              $YUMDNFBIN${CACHESKIP} -y -q remove sendmail sendmail-cf${DISABLEREPO_DNF}
                sar_call
          else
            $YUMDNFBIN${CACHESKIP} -y -q remove sendmail${DISABLEREPO_DNF}
            sar_call
          fi
          $YUMDNFBIN${CACHESKIP} -y -q install postfix${DISABLEREPO_DNF}
            sar_call
          postfixsetup
          echo "*************************************************"
          cecho "* postfix installed" $boldgreen
          echo "*************************************************"
      fi
  fi
fi # centos7
}

postfixsetup() {

  if [ ! -f /usr/sbin/pflogsumm ]; then
    time $YUMDNFBIN -q -y install postfix-perl-scripts${DISABLEREPO_DNF}
  fi
  time $YUMDNFBIN -q -y update postfix --disableplugin=priorities${DISABLEREPO_DNF}

  CHECKSENDMAILPATH=$(grep sendmail_path /usr/local/lib/php.ini)
  if [ "$CHECKSENDMAILPATH" == ';sendmail_path =' ]; then
    sed -i "s/;sendmail_path =/sendmail_path = \/usr\/lib\/sendmail.postfix -t -i/g" /usr/local/lib/php.ini
  fi

# tweak mailbox_size_limit from default 50MB to 1000MB

if [ ! -d /var/mail/root ]; then
    MBOXSIZE='0'
else    
    MBOXSIZE=$(du -sh /var/mail/root | head -n1 | awk '{print $1}' | sed 's/M//')
fi
MBOXSIZEBYTES=$(echo "$MBOXSIZE*1024*1024" | bc)
MBOXSIZELIMIT=$(postconf -d | grep mailbox_size_limit | head -n1 | awk '{print $3}')
INCLIMIT=$(echo "$MBOXSIZELIMIT*20" | bc)
CHECKMBOXSIZELIMIT=$(grep mailbox_size_limit /etc/postfix/main.cf)

# check if /var/spool/postfix/public/pickup exists
if [[ ! -S /var/spool/postfix/public/pickup ]]; then
  mkfifo /var/spool/postfix/public/pickup
  chown postfix:postfix /var/spool/postfix/public/pickup
fi

postconf -e default_destination_concurrency_limit=100
postconf -e default_destination_recipient_limit=100
postconf -e default_process_limit=200
postconf -e mailbox_size_limit=$INCLIMIT
postconf -e maximal_backoff_time=1000s
postconf -e minimal_backoff_time=300s
postconf -e qmgr_message_active_limit=40000
postconf -e qmgr_message_recipient_limit=40000
postconf -e smtp_connect_timeout=20s
postconf -e smtp_destination_concurrency_limit=200
postconf -e smtp_helo_timeout=200s
postconf -e smtp_mx_session_limit=100
postconf -e smtpd_client_connection_count_limit=50
#postconf -e smtpd_client_connection_rate_limit=60
postconf -e 'recipient_delimiter = +'
postconf -e smtpd_recipient_limit=2000
postconf -e smtpd_recipient_overshoot_limit=2000
postconf -e 'smtpd_banner = $myhostname ESMTP'

postconf -d | egrep 'smtpd_tls_mandatory_protocols|smtpd_tls_protocols'
postconf -e 'smtpd_tls_mandatory_protocols = !SSLv2, !SSLv3'
postconf -e 'smtpd_tls_protocols = !SSLv2 !SSLv3'
postconf -n | egrep 'smtpd_tls_mandatory_protocols|smtpd_tls_protocols|smtpd_banner'

postconf -d | egrep 'smtp_tls_mandatory_protocols|smtp_tls_protocols'
postconf -e 'smtp_tls_mandatory_protocols = !SSLv2 !SSLv3'
postconf -e 'smtp_tls_protocols = !SSLv2 !SSLv3'
postconf -n | egrep 'smtp_tls_mandatory_protocols|smtp_tls_protocols'

postconf -d | egrep 'lmtp_tls_protocols|lmtp_tls_mandatory_protocols'
postconf -e 'lmtp_tls_mandatory_protocols = !SSLv2 !SSLv3'
postconf -e 'lmtp_tls_protocols = !SSLv2 !SSLv3'
postconf -n | egrep 'lmtp_tls_protocols|lmtp_tls_mandatory_protocols'

postconf -d | grep '^smtpd_tls_mandatory_exclude_ciphers'
postconf -e 'smtpd_tls_mandatory_exclude_ciphers = aNULL, eNULL, EXPORT, DES, RC4, MD5, PSK, aECDH, EDH-DSS-DES-CBC3-SHA, EDH-RSA-DES-CDC3-SHA, KRB5-DE5, CBC3-SHA'
postconf -n | grep '^smtpd_tls_mandatory_exclude_ciphers'

# openssl dhparam -out /path/to/dhparams.pem 2048
# postconf -d | grep '^smtpd_tls_dh1024_param_file'
# postconf -e 'smtpd_tls_dh1024_param_file = /path/to/dhparams.pem'
# postconf -n | grep '^smtpd_tls_dh1024_param_file'

# setup Postfix Opportunistic TLS connections for outgoing emails from server
# http://www.postfix.org/TLS_README.html#client_tls_may
# With opportunistic TLS, mail delivery continues even if the server certificate is untrusted or bears the wrong name. 
# When the TLS handshake fails for an opportunistic TLS session, rather than give up on mail delivery, the Postfix 
# SMTP client retries the transaction with TLS disabled. Trying an unencrypted connection makes it possible to 
# deliver mail to sites with non-interoperable server TLS implementations.
postconf -d smtp_tls_CAfile smtp_tls_security_level smtp_tls_loglevel smtp_tls_session_cache_database
postconf -e 'smtp_tls_CAfile=/etc/pki/tls/certs/ca-bundle.crt'
postconf -e 'smtp_tls_security_level = may'
postconf -e 'smtp_tls_loglevel = 1'
postconf -e 'smtp_tls_session_cache_database = btree:/var/lib/postfix/smtp_scache'
service postfix reload
postconf -n smtp_tls_CAfile smtp_tls_security_level smtp_tls_loglevel smtp_tls_session_cache_database

if [[ "$DISABLE_IPVSIX" = [yY] ]]; then
    postconf -n inet_protocols
    postconf -e 'inet_protocols = ipv4'
    postconf -n inet_protocols
fi

cmservice postfix restart
postconf -n | grep mailbox_size_limit | head -n1 | awk '{print $3}'

  cd $DIR_TMP
  wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/pflogsumm/pflogsumm-1.1.5.tar.gz --tries=3
  tar -xzf pflogsumm-1.1.5.tar.gz
  cd pflogsumm-1.1.5
  cp pflogsumm.pl /usr/bin/pflogsumm
  chmod 0700 /usr/bin/pflogsumm
  alias postfixlog='pflogsumm -d today --verbose_msg_detail /var/log/maillog'
  echo "alias postfixlog='pflogsumm -d today --verbose_msg_detail /var/log/maillog'" >> /root/.bashrc
}
