function funct_sendmailmc {

# Configure sendmail to send and receive email outside
# of local network

if [ -f /etc/mail/sendmail.mc ]; then

cd /etc/mail

mv /etc/mail/sendmail.mc /etc/mail/sendmail.mc-backup

cp $CUR_DIR/config/sendmail/sendmail.mc /etc/mail/sendmail.mc

m4 sendmail.mc > /etc/mail/sendmail.cf

sed -i 's/Sendmail \$v\/\$Z; \$b/Sendmail \$b/g' /etc/mail/sendmail.cf

echo "sendmail: ALL" >> /etc/hosts.allow

service sendmail restart

fi

}
