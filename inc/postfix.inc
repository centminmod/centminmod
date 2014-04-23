postfixsetup() {

	yum -q -y install postfix-perl-scripts

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

#if [[ -z "$CHECKMBOXSIZELIMIT" ]]; then
#	echo "mailbox_size_limit = $INCLIMIT" >> /etc/postfix/main.cf
#else
#	sed -i "s/mailbox_size_limit = 51200000/mailbox_size_limit = $INCLIMIT/g" /etc/postfix/main.cf
#fi

postconf -e mailbox_size_limit=$INCLIMIT
/etc/init.d/postfix reload
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
