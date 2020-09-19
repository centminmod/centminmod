#!/bin/bash
######################################################
# tools/emailnotify.sh
# https://community.centminmod.com/threads/18184/
# utilise tools/email.sh setup primary & secondary
# email addresses populated by user at initial install
# time to send centminmod related email notifications
# https://community.centminmod.com/threads/6117/
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

# Amazon SES smtp based email sending settings
# add these variables to persistent config file
# /etc/centminmod/custom_config.inc
# for them to persistent. Using below variables will 
# be overridden on centminmod updates so persistent
# config file method is highly recommended
# Amazon SES emails are sent via native openssl s_client
# https://www.openssl.org/docs/man1.1.1/man1/s_client.html
EMAILNOTIFY_SES='n'
EMAILNOTIFY_SES_FROM_EMAIL='your_ses_verified_from_email_address'
EMAILNOTIFY_SES_TO_EMAIL='your_desired_email_to_receive_server_emails'
# CC and BCC options are optional. If not need, set and leave 
# empty without a value
EMAILNOTIFY_SES_CC_EMAIL='your_desired_cc_email_to_receive_server_emails'
EMAILNOTIFY_SES_BCC_EMAIL='your_desired_bcc_email_to_receive_server_emails'
EMAILNOTIFY_SES_SMTP_USERNAME='your_ses_smtp_username'
EMAILNOTIFY_SES_SMTP_PASSWORD='your_ses_smtp_password'
EMAILNOTIFY_SES_SMTP_SERVER='your_ses_smtp_server'
EMAILNOTIFY_SES_SMTP_PORT='587'
######################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}
######################################################
# functions
#############
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

symlink_setup_emailnotify() {
  if [[ -f "${SCRIPT_DIR}/emailnotify.sh" && ! -h /usr/local/bin/emailnotify ]]; then
    ln -s "${SCRIPT_DIR}/emailnotify.sh" /usr/local/bin/emailnotify >/dev/null 2>&1
  fi
}

send_ses_credential_encode() {
  SES_SMTP_USER=$(echo -n "$EMAILNOTIFY_SES_SMTP_USERNAME" | openssl enc -base64)
  SES_SMTP_PASS=$(echo -n "$EMAILNOTIFY_SES_SMTP_PASSWORD" | openssl enc -base64)
}

send_ses_mail() {
  # send email using native openssl client
  bodyget=$1
  subject=$2
  toemail=$3
  send_ses_credential_encode
  if [ "$toemail" ]; then
    EMAILNOTIFY_SES_TO_EMAIL="$toemail"
  else
    EMAILNOTIFY_SES_TO_EMAIL="$EMAILNOTIFY_SES_TO_EMAIL"
  fi
  if [ -f "$bodyget" ]; then
    body=$(cat "$bodyget")
  else
    body="$bodyget"
  fi
  email_date=$(date)
  SES_FROM_DOMAIN=$(echo "$EMAILNOTIFY_SES_FROM_EMAIL" | awk -F '@' '{print $2}')
  if [ "$EMAILNOTIFY_SES_CC_EMAIL" ]; then
    ses_cc="\nCc: $EMAILNOTIFY_SES_CC_EMAIL"
  else
    ses_cc=
  fi
  if [ "$EMAILNOTIFY_SES_BCC_EMAIL" ]; then
    ses_bcc="\nBcc: $EMAILNOTIFY_SES_BCC_EMAIL"
  else
    ses_bcc=
  fi
cat > /tmp/email_body_template.txt <<EOF
EHLO $SES_FROM_DOMAIN
AUTH LOGIN
$SES_SMTP_USER
$SES_SMTP_PASS
MAIL FROM: ${EMAILNOTIFY_SES_FROM_EMAIL}
RCPT TO: ${EMAILNOTIFY_SES_TO_EMAIL}
DATA
From: $EMAILNOTIFY_SES_FROM_EMAIL <${EMAILNOTIFY_SES_FROM_EMAIL}>
To: ${EMAILNOTIFY_SES_TO_EMAIL}${ses_cc}${ses_bcc}
Subject: $subject $(hostname) $email_date

$body
.
QUIT
EOF
  if [ -f /usr/bin/systemctl ]; then
    # centos 7 and higher openssl supported option
    brief_opt=' -brief'
  else
    brief_opt=
  fi
  if [[ "$EMAILNOTIFY_SES_SMTP_PORT" = '587' ]]; then
    echo "----------------------------------------------------"
    echo "Send email via SES using openssl client"
    echo "Success email send will end with line:"
    echo "451 4.4.2 Timeout waiting for data from client"
    echo "and return to command prompt after 10 seconds"
    echo "----------------------------------------------------"
    echo
    openssl s_client -crlf -quiet${brief_opt} -starttls smtp -connect ${EMAILNOTIFY_SES_SMTP_SERVER}:${EMAILNOTIFY_SES_SMTP_PORT} < /tmp/email_body_template.txt
    # rm -f /tmp/email_body_template.txt
  elif [[ "$EMAILNOTIFY_SES_SMTP_PORT" = '465' ]]; then
    echo "----------------------------------------------------"
    echo "Send email via SES using openssl client"
    echo "Success email send will end with line:"
    echo "451 4.4.2 Timeout waiting for data from client"
    echo "and return to command prompt after 10 seconds"
    echo "----------------------------------------------------"
    echo
    openssl s_client -crlf -quiet${brief_opt} -connect ${EMAILNOTIFY_SES_SMTP_SERVER}:${EMAILNOTIFY_SES_SMTP_PORT} < /tmp/email_body_template.txt
    # rm -f /tmp/email_body_template.txt
  fi
}

postfix_update() {
  if [[ -f /usr/sbin/postconf && "$(postconf -n inet_protocols | awk -F ' = ' '{print $2}')" != 'ipv4' ]]; then
    # force postfix to use IPv4 address
    echo "updating postfix inet_protocols = ipv4"
    postconf -e 'inet_protocols = ipv4'
    if [[ "$(ps -ef | grep postfix | grep -v grep)" ]]; then
      # only restart postfix is detected that it's currently running
      service postfix restart >/dev/null 2>&1
    fi
  fi
}

cloudflare_note() {
  if [[ "$(curl -s4Ik $serverhostname | awk '/Server:/ {print $2}')" = 'cloudflare' ]]; then
  echo "
  Cloudflare users should DISABLE 'orange cloud' Proxy on
  server's main $serverhostname DNS record as enabling
  Cloudflare on main $serverhostname DNS record will negatively
  impact proper email delivery from your server as destination
  mail servers can not do a proper reverse DNS PTR lookup to
  verify and match your main hostname $serverhostname and it's
  resolving server IP address which currently points to:
  $pointed_hostname_ip
  "
  fi
}

check_fail_msg() {
  echo "
  server ${text_label} IP: $pointed_ip
  $serverhostname checked DNS ${text_label} record: $pointed_hostname_ip
  
  -----------------------------------------------------------------------------
  fail: PTR ${text_label} DNS record setup
  -----------------------------------------------------------------------------
  Centmin Mod main hostname $serverhostname reverse PTR 
  DNS ${text_label} record not detected

  $serverhostname requires a working PTR DNS record to ensure
  server outbound sent emails are properly delivered to your
  previously setup server notification email addresses otherwise
  server sent emails end up in destination receipient's spam/junk
  mail box. Full instructions to remedy this can be read at
  https://community.centminmod.com/threads/6999/
  $cloudflare_note
  -----------------------------------------------------------------------------

  " | tee /tmp/check_fail_msg.log
}

check_pass_msg() {
  echo "-----------------------------------------------------------------------------"
  echo -e "pass: ok\n$serverhostname reverse PTR lookup ${text_label} address = $pointed_hostname_ip"
}

checks() {
  serverip_ipv4=$(curl -4s https://ipinfo.io/ip)
  serverip_ipv6=$(curl -6s https://ipinfo.io/ip)
  serverhostname=$(hostname)
  serverhostname_ipv4=$(dig +short A $serverhostname)
  serverhostname_ipv6=$(dig +short AAAA $serverhostname)
  relaycheck=$(postconf -n relayhost | egrep -o 'amazonaws|sendgrid|mailgun')
  if [[ "$relaycheck" = 'amazonaws' ]]; then
    ses_detected='y'
    relayhost_detected='y'
  elif [[ "$relaycheck" = 'sendgrid' ]]; then
    sendgrid_detected='y'
    relayhost_detected='y'
  elif [[ "$relaycheck" = 'mailgun' ]]; then
    mailgun_detected='y'
    relayhost_detected='y'
  elif [[ "$relaycheck" ]]; then
    relayhost_detected='y'
  else
    ses_detected='n'
    sendgrid_detected='n'
    mailgun_detected='n'
    relayhost_detected='n'
    if [[ "$(echo "$serverhostname_ipv4"|wc -l)" -gt '1' || "$(echo "$serverhostname_ipv6"|wc -l)" -gt '1' ]]; then
      # check for cdn or anycase/cloudflare proxies
      multiip='y'
    else
      multiip='n'
      if [[ "$serverhostname_ipv4" = "$serverip_ipv4" ]]; then
        checkptr_ipv4=y
        text_label='IPv4'
        pointed_hostname_ip=$serverhostname_ipv4
        check_pass_msg
      else
        text_label='IPv4'
        pointed_ip=$serverip_ipv4
        pointed_hostname_ip=$serverhostname_ipv4
        check_fail_msg
        failipv4=y
      fi
      if [[ "$serverip_ipv6" ]] && [[ "$serverhostname_ipv6" = "$serverip_ipv6" ]]; then
        checkptr_ipv6=y
        text_label='IPv6'
        pointed_hostname_ip=$serverhostname_ipv6
        check_pass_msg
      elif [[ "$serverip_ipv6" ]] && [[ "$serverhostname_ipv6" != "$serverip_ipv6" ]]; then
        text_label='IPv6'
        pointed_ip=$serverip_ipv6
        pointed_hostname_ip=$serverhostname_ipv6
        check_fail_msg
        failipv6=y
      fi
    fi
  fi
}

check_sent() {
  senderr=$?
  if [[ "$senderr" -eq '0' ]]; then
    echo "email sent at $email_date"
  else
    echo "error detected sending email"
  fi
  if [ -f /tmp/check_fail_msg.log ]; then
    rm -f /tmp/check_fail_msg.log
  fi
}

get_mailid() {
  echo "saving postfix maillog entry $CENTMINLOGDIR/emailnotify-maillog-${DT}.log.gz"
  echo
  echo "to read use zcat:"
  echo
  echo "zcat $CENTMINLOGDIR/emailnotify-maillog-${DT}.log.gz"
  sleep 3.5
  mailid=$(tail -20 /var/log/maillog | grep "$primary_email" | tail -1 | awk '{print $6}')
  sleep 0.5
  grep "$mailid" /var/log/maillog > "$CENTMINLOGDIR/emailnotify-maillog-${DT}.log"
  gzip -1 "$CENTMINLOGDIR/emailnotify-maillog-${DT}.log"
}

send_mail() {
  if [ -f /etc/centminmod/email-primary.ini ]; then
    postfix_update
    if [[ "$EMAILNOTIFY_SES" != [yY] ]]; then
      symlink_setup_emailnotify
      checks
    fi
    email_date=$(date)
    primary_email=$(head -n1 /etc/centminmod/email-primary.ini)
    secondary_email=$(head -n1 /etc/centminmod/email-secondary.ini)
    body=$1
    subject=$2
    toemail=$3
    if [ "$toemail" ]; then
      to_email="$toemail"
    else
      to_email=$primary_email
    fi
    if [ -f "$body" ]; then
      if [[ "$failipv4" = [yY] && -f /tmp/check_fail_msg.log ]]; then
        # append check_fail_msg to body of email so user is aware of issue
        #body=$(cat "$body" /tmp/check_fail_msg.log)
        bodyecho="$body"
        check_fail_msg_note="Message to $serverhostname system administrator regarding mail deliverability improvements: "
        body=$(echo -e "$(cat "$body")\n\n${check_fail_msg_note}\n$(cat /tmp/check_fail_msg.log)")
        bodyfile=/tmp/emailnotify_temp.log
        echo "$body" > "$bodyfile"
        if [ -f /usr/sbin/sendmail ]; then
(cat - $bodyfile)<<END | sendmail -t $to_email
From: $primary_email
To: $to_email
Subject:  ${subject} - $(hostname) ${email_date}

END
          rm -f "$bodyfile"
        else
          echo "echo \"$bodyecho\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $to_email"
          echo "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $to_email
        fi
      else
        bodyfile=/tmp/emailnotify_temp.log
        cat "$body" > "$bodyfile"
        if [ -f /usr/sbin/sendmail ]; then
(cat - $bodyfile)<<END | sendmail -t $to_email
From: $primary_email
To: $to_email
Subject:  ${subject} - $(hostname) ${email_date}

END
          rm -f "$bodyfile"
        else
          echo "cat \"$body\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $to_email"
          cat "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $to_email
        fi
      fi
      get_mailid
      check_sent
    else
      if [[ "$failipv4" = [yY] && -f /tmp/check_fail_msg.log ]]; then
        # append check_fail_msg to body of email so user is aware of issue
        bodyecho="$body"
        check_fail_msg_note="Message to $serverhostname system administrator regarding mail deliverability improvements: "
        body=$(echo -e "$body\n\n${check_fail_msg_note}\n$(cat /tmp/check_fail_msg.log)")
        bodyfile=/tmp/emailnotify_temp.log
        echo "$body" > "$bodyfile"
        if [ -f /usr/sbin/sendmail ]; then
(cat - $bodyfile)<<END | sendmail -t $to_email
From: $primary_email
To: $to_email
Subject:  ${subject} - $(hostname) ${email_date}

END
          rm -f "$bodyfile"
        else
          echo "echo \"$bodyecho\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $to_email"
          echo "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $to_email
        fi
      else
        bodyfile=/tmp/emailnotify_temp.log
        echo "$body" > "$bodyfile"
        if [ -f /usr/sbin/sendmail ]; then
(cat - $bodyfile)<<END | sendmail -t $to_email
From: $primary_email
To: $to_email
Subject:  ${subject} - $(hostname) ${email_date}

END
          rm -f "$bodyfile"
        else
          echo "echo \"$body\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $to_email"
          echo "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $to_email
        fi
      fi
      get_mailid
      check_sent
    fi
  else
    echo
    echo "error: /etc/centminmod/email-primary.ini missing"
    echo
    "${SCRIPT_DIR}/email.sh"
  fi
}

usage() {
  echo "Usage:"
  echo
  echo "$0 send emailbody emailsubject"
  echo "$0 send emailbody emailsubject toemail-address"
  echo
}

######################################################
case "$1" in
  send )
    if [[ -z "$2" || -z "$3" ]]; then
      echo "error: incorrect paramters"
      usage
    else
      if [[ "$EMAILNOTIFY_SES" = [yY] ]]; then
        send_ses_mail "${2}" "${3}" "${4}"
      else
        send_mail "${2}" "${3}" "${4}"
      fi
    fi
    ;;
  * )
    usage
    ;;
esac