postfixsetup() {

	if [ ! -f /usr/sbin/pflogsumm ]; then
		yum -q -y install postfix-perl-scripts
	fi
	yum -q -y update postfix --disableplugin=priorities

	CHECKSENDMAILPATH=$(grep sendmail_path /usr/local/lib/php.ini)
	if [ "$CHECKSENDMAILPATH" == ';sendmail_path =' ]; then
		sed -i "s/;sendmail_path =/sendmail_path = \/usr\/lib\/sendmail.postfix -t -i/g" /usr/local/lib/php.ini
	fi

# tweak mailbox_size_limit from default 50MB to 1000MB

MBOXSIZE=$(du -sh /var/mail/root | head -n1 | awk '{print $1}' | sed 's/M//')
MBOXSIZEBYTES=$(echo "$MBOXSIZE*1024*1024" | bc)
MBOXSIZELIMIT=$(postconf -d | grep mailbox_size_limit | head -n1 | awk '{print $3}')
INCLIMIT=$(echo "$MBOXSIZELIMIT*20" | bc)
CHECKMBOXSIZELIMIT=$(grep mailbox_size_limit /etc/postfix/main.cf)

# check if /var/spool/postfix/public/pickup exists
if [[ ! -f /var/spool/postfix/public/pickup ]]; then
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

cmservice postfix restart
postconf -n | grep mailbox_size_limit | head -n1 | awk '{print $3}'

	cd $DIR_TMP
	wget -cnv http://linxnet.com/downloads/pflogsumm-1.1.5.tar.gz --tries=3
	tar -xzf pflogsumm-1.1.5.tar.gz
	cd pflogsumm-1.1.5
	cp pflogsumm.pl /usr/bin/pflogsumm
	chmod 0700 /usr/bin/pflogsumm
	alias postfixlog='pflogsumm -d today --verbose_msg_detail /var/log/maillog'
	echo "alias postfixlog='pflogsumm -d today --verbose_msg_detail /var/log/maillog'" >> /root/.bashrc
}
