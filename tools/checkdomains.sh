#!/bin/bash
##########################################################################
# check centminmod download domain's dns records and whois info
# keep a log of changes and in future alert to changes as a security 
# measure in case a download's domain name dns is compromised or expiry
# lapses and domain is picked up by a rogue or malicious person
##########################################################################
WHOIS_TIMEOUT='4'

DIGOPTS='+nocomments'
WHOISBIN='whois'
WHOISOPT=' -n'
WHOIS_SHOWNS='y'
WHOIS_SHOWREGISTRAR='y'
WHOIS_NAMESERVER='8.8.8.8'

DEBUG='n'
CTMPDIR=/home/checkdomainstmp
##########################################################################
if [[ ! -d "$CTMPDIR" ]]; then
  mkdir -p "$CTMPDIR"
fi
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
LISTDOMAINS=$(egrep -rn 'http:|https:' /usr/local/src/centminmod/ | egrep -v 'http://ftp.osuosl.org|\${HTUSER}|\$request_uri|\$vhostname|\${vhostname}|rpm.axivo.com|foo.bar|master.ourdelta.org|newdomain1.com|apt.sw.be|medium.com|href=|my.incapsula.com|#|echo|cecho|<li>|<li class|centos.alt.ru|<|>|\(|\[|\)|\]|<html|<!DOCTYPE|nginx.org|centminmod.com|centmin.com|centmin.sh|github.com|php.net|yum.mariadb.org' | sed -e "s|<||g" -e "s|'||g" -e "s|\| bash -s stable||g" | grep -Eo '(http|https|ftp)://[^/"]+' | sed -e "s|http:\/\/||g" -e "s|https:\/\/||g" | sort | uniq -c | sort -rn | awk '{print $2}')
fi

OTHERDOMAINS='nginx.org centminmod.com centmin.com centmin.sh github.com php.net yum.mariadb.org'

for d in ${OTHERDOMAINS[@]}; do
  echo "----------"
  if [[ "$WHOISBIN" = 'jwhois' ]]; then
    echo -n "$d "
    toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    echo
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    rm -rf "${CTMPDIR}/${toplevel}.txt"
  elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
    echo -n "$d "
    toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    echo
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    rm -rf "${CTMPDIR}/${toplevel}.txt"
  elif [[ "$WHOISBIN" = 'whois' ]]; then
    echo -n "$d "
    toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
    whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    echo
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      echo -n "$d "
      echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
    rm -rf "${CTMPDIR}/${toplevel}.txt"
  fi
  echo -n "$d "
  echo -n $(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
  echo
done

if [[ "$DEBUG" != [yY] ]]; then
  for d in ${LISTDOMAINS[@]}; do
    echo "----------"
    if [[ "$WHOISBIN" = 'jwhois' ]]; then
      echo -n "$d "
      toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      echo
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        echo -n "$d "
        echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      rm -rf "${CTMPDIR}/${toplevel}.txt"
    elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
      echo -n "$d "
      toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      echo
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        echo -n "$d "
        echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      rm -rf "${CTMPDIR}/${toplevel}.txt"
    elif [[ "$WHOISBIN" = 'whois' ]]; then
      echo -n "$d "
      toplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$toplevel" > "${CTMPDIR}/${toplevel}.txt"
      whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${toplevel}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${toplevel}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      echo
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        echo -n "$d "
        echo "$whoisns" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      rm -rf "${CTMPDIR}/${toplevel}.txt"
    fi
    echo -n "$d "
    echo -n $(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
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