postfixsetup() {

	yum -q -y install postfix-perl-scripts
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
postconf -e default_destination_recipient_limit=10000
/etc/init.d/postfix restart
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
