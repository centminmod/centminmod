#!/bin/bash
##########################################################################
# check centminmod download domain's dns records and whois info
# keep a log of changes and in future alert to changes as a security 
# measure in case a download's domain name dns is compromised or expiry
# lapses and domain is picked up by a rogue or malicious person
##########################################################################
WHOIS_TIMEOUT='4'

WHOISBIN='whois'
WHOISOPT=' -n'

DEBUG='n'
##########################################################################
check_domains() {
if [ ! -f /usr/bin/whois ]; then
  yum -y -q install whois
fi
if [ ! -f /usr/bin/jwhois ]; then
  yum -y -q install jwhois
fi
if [[ ! "$(grep -w '43' /etc/csf/csf.conf)" ]]; then
  sed -i "s/TCP_OUT = \"/TCP_OUT = \"43,/g" /etc/csf/csf.conf
  sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"43,/g" /etc/csf/csf.conf
  egrep '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
  csf -r >/dev/null 2>&1
fi

if [[ "$DEBUG" != [yY] ]]; then
LISTDOMAINS=$(egrep -rn 'http:|https:' /usr/local/src/centminmod/ | egrep -v 'http://ftp.osuosl.org|\${HTUSER}|\$request_uri|\$vhostname|\${vhostname}|rpm.axivo.com|foo.bar|master.ourdelta.org|newdomain1.com|apt.sw.be|medium.com|href=|my.incapsula.com|#|echo|cecho|<li>|<li class|centos.alt.ru|<|>|\(|\[|\)|\]|<html|<!DOCTYPE|nginx.org|centminmod.com|centmin.com|centmin.sh' | sed -e "s|<||g" -e "s|'||g" -e "s|\| bash -s stable||g" | grep -Eo '(http|https|ftp)://[^/"]+' | sed -e "s|http:\/\/||g" -e "s|https:\/\/||g" | sort | uniq -c | sort -rn | awk '{print $2}')
fi

OTHERDOMAINS='nginx.org centminmod.com centmin.com centmin.sh'

for d in ${OTHERDOMAINS[@]}; do
  echo "----------"
  if [[ "$WHOISBIN" = 'jwhois' ]]; then
    echo -n "$d "
    toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
    whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar URL:/ {print $2}')
    whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
    whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
    #echo "$whoisdate $whoisurl"; echo
    echo -n "$whoisdate"; echo
    echo -n "$d "
    echo "$whoisns" | tr '\n\r' ' '
    echo
    rm -rf ${toplevel}.txt
  elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
    echo -n "$d "
    toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
    whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar URL:/ {print $2}')
    whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
    whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
    #echo "$whoisdate $whoisurl"; echo
    echo -n "$whoisdate"; echo
    echo -n "$d "
    echo "$whoisns" | tr '\n\r' ' '
    echo
    rm -rf ${toplevel}.txt
  elif [[ "$WHOISBIN" = 'whois' ]]; then
    echo -n "$d "
    toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
    whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar:/ {print $2}')
    whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
    whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
    #echo "$whoisdate $whoisurl"; echo
    echo -n "$whoisdate"; echo
    echo -n "$d "
    echo "$whoisns" | tr '\n\r' ' '
    echo
    rm -rf ${toplevel}.txt
  fi
  echo -n "$d "
  echo -n $(dig -4 @8.8.8.8 +short A $d)
  echo
done

if [[ "$DEBUG" != [yY] ]]; then
  for d in ${LISTDOMAINS[@]}; do
    echo "----------"
    if [[ "$WHOISBIN" = 'jwhois' ]]; then
      echo -n "$d "
      toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
      whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar URL:/ {print $2}')
      whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
      whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
      #echo "$whoisdate $whoisurl"; echo
      echo -n "$whoisdate"; echo
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' '
      echo
      rm -rf ${toplevel}.txt
    elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
      echo -n "$d "
      toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
      whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar URL:/ {print $2}')
      whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
      whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
      #echo "$whoisdate $whoisurl"; echo
      echo -n "$whoisdate"; echo
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' '
      echo
      rm -rf ${toplevel}.txt
    elif [[ "$WHOISBIN" = 'whois' ]]; then
      echo -n "$d "
      toplevel="$(echo $d |grep -o '[^.]*\.[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${toplevel}.txt"
      whoisurl=$(cat ${toplevel}.txt | awk  -F ": " '/Registrar:/ {print $2}')
      whoisdate=$(cat ${toplevel}.txt | awk  -F ": " '/Expiry Date:/ {print $2}')
      whoisns=$(cat ${toplevel}.txt | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]')
      #echo "$whoisdate $whoisurl"; echo
      echo -n "$whoisdate"; echo
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' '
      echo
      rm -rf ${toplevel}.txt
    fi
    echo -n "$d "
    echo -n $(timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$(echo $d |grep -o '[^.]*\.[^.]*$')" | awk  -F ": " '/Name Server:/ {print $2}' | tr '[:upper:]' '[:lower:]'); echo
    echo -n "$d "
    echo -n $(dig -4 @8.8.8.8 +short A $d)
    echo
  done
fi
}

case "$1" in
  check )
    check_domains
    ;;
  pattern )
    ;;
  * )
    echo "Usage:"
    echo
    echo "$0 {check}"
    echo
    ;;
esac