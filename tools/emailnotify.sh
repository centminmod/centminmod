#!/bin/bash
######################################################
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

symlink_setup_emailnotify() {
  if [[ -f "${SCRIPT_DIR}/emailnotify.sh" && ! -h /usr/local/bin/emailnotify ]]; then
    ln -s "${SCRIPT_DIR}/emailnotify.sh" /usr/local/bin/emailnotify >/dev/null 2>&1
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
        echo "-----------------------------------------------------------------------------"
        echo -e "pass:\n$serverhostname reverse PTR lookup IPv4 address = $serverhostname_ipv4"
      else
        echo
        echo "server IPv4 IP: $serverip_ipv4"
        echo "$serverhostname checked DNS IPv4 record: $serverhostname_ipv4"
        echo
        echo "-----------------------------------------------------------------------------"
        echo "fail: PTR IPv4 DNS record setup"
        echo "-----------------------------------------------------------------------------"
        echo "$serverhostname reverse PTR lookup IPv4 not found"
        echo "$serverhostname requires a working PTR DNS record to ensure"
        echo "server outbound sent emails are properly delivered to your"
        echo "previously setup server notification email addresses otherwise"
        echo "server sent emails end up in destination receipient's spam/junk"
        echo "mail box. Full instructions to remedy this can be read at"
        echo "https://community.centminmod.com/threads/6999/"
        echo
        echo "Cloudflare users should DISABLE 'orange cloud' Proxy on"
        echo "server's main $serverhostname DNS record as enabling"
        echo "Cloudflare on main $serverhostname DNS record will negatively"
        echo "impact proper email delivery from your server as destination"
        echo "mail servers can not do a proper reverse DNS PTR lookup to"
        echo "verify and match your main hostname $serverhostname and it's"
        echo "resolving server IP address which currently points to:"
        echo "$serverhostname_ipv4"
        echo "-----------------------------------------------------------------------------"
        echo
      fi
      if [[ "$serverip_ipv6" ]] && [[ "$serverhostname_ipv6" = "$serverip_ipv6" ]]; then
        checkptr_ipv6=y
        echo "-----------------------------------------------------------------------------"
        echo -e "pass:\n$serverhostname reverse PTR lookup IPv6 address = $serverhostname_ipv6"
      elif [[ "$serverip_ipv6" ]] && [[ "$serverhostname_ipv6" != "$serverip_ipv6" ]]; then
        echo
        echo "server IPv6 IP: $serverip_ipv6"
        echo "$serverhostname checked DNS IPv6 record: $serverhostname_ipv6"
        echo
        echo
        echo "-----------------------------------------------------------------------------"
        echo "fail: PTR IPv6 DNS record setup"
        echo "-----------------------------------------------------------------------------"
        echo "$serverhostname reverse PTR lookup IPv6 not found"
        echo "$serverhostname requires a working PTR DNS record to ensure"
        echo "server outbound sent emails are properly delivered to your"
        echo "previously setup server notification email addresses otherwise"
        echo "server sent emails end up in destination receipient's spam/junk"
        echo "mail box. Full instructions to remedy this can be read at"
        echo "https://community.centminmod.com/threads/6999/"
        echo
        echo "Cloudflare users should DISABLE 'orange cloud' Proxy on"
        echo "server's main $serverhostname DNS record as enabling"
        echo "Cloudflare on main $serverhostname DNS record will negatively"
        echo "impact proper email delivery from your server as destination"
        echo "mail servers can not do a proper reverse DNS PTR lookup to"
        echo "verify and match your main hostname $serverhostname and it's"
        echo "resolving server IP address which currently points to:"
        echo "$serverhostname_ipv6"
        echo "-----------------------------------------------------------------------------"
        echo
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
    symlink_setup_emailnotify
    postfix_update
    checks
    email_date=$(date)
    primary_email=$(head -n1 /etc/centminmod/email-primary.ini)
    secondary_email=$(head -n1 /etc/centminmod/email-secondary.ini)
    body=$1
    subject=$2
    if [ -f "$body" ]; then
      echo "cat \"$body\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $primary_email"
      cat "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $primary_email
      get_mailid
      check_sent
    else
      echo "echo \"$body\" | mail -s \" ${subject} - $(hostname) ${email_date}\" $primary_email"
      echo "$body" | mail -s " ${subject} - $(hostname) ${email_date}" $primary_email
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
  echo
}

######################################################
case "$1" in
  send )
    if [[ -z "$2" || -z "$3" ]]; then
      echo "error: incorrect paramters"
      usage
    else
      send_mail $2 $3
    fi
    ;;
  * )
    usage
    ;;
esac