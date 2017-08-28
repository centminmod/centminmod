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

CHECKDOMAINS_DEBUG='n'
CTMPDIR=/home/checkdomainstmp
##########################################################################
if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  . "/etc/centminmod/custom_config.inc"
fi

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

if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
LISTDOMAINS=$(egrep -rn 'http:|https:' /usr/local/src/centminmod/ | egrep -v 'http://ftp.osuosl.org|\${HTUSER}|\$request_uri|\$vhostname|\${vhostname}|rpm.axivo.com|foo.bar|master.ourdelta.org|newdomain1.com|apt.sw.be|medium.com|href=|my.incapsula.com|#|echo|cecho|<li>|<li class|centos.alt.ru|<|>|\(|\[|\)|\]|<html|<!DOCTYPE|nginx.org|centminmod.com|centmin.com|centmin.sh|github.com|php.net|yum.mariadb.org|apache.mirror.uber.com.au' | sed -e "s|<||g" -e "s|'||g" -e "s|\| bash -s stable||g" | grep -Eo '(http|https|ftp)://[^/"]+' | sed -e "s|http:\/\/||g" -e "s|https:\/\/||g" | sort | uniq -c | sort -rn | awk '{print $2}')
fi

OTHERDOMAINS='nginx.org centminmod.com centmin.com centmin.sh github.com php.net yum.mariadb.org apache.mirror.uber.com.au'

for d in ${OTHERDOMAINS[@]}; do
  echo "----------"
  if [[ "$WHOISBIN" = 'jwhois' ]]; then
    echo -n "$d "
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
    if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
    echo -n "$d "
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
    if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  elif [[ "$WHOISBIN" = 'whois' ]]; then
    echo -n "$d "
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ "$tld" = 'edu' ]]; then
      whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
    else
      whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
    if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  fi
  DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
  for ip in ${DOMAINIPS[@]}; do
    if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
      curl -4s ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
      ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
      country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
      org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
      echo -n "$d "
      echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
      echo
      rm -rf "${CTMPDIR}/${d}-ip.txt"
    fi
  done
  echo
done

if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
  for d in ${LISTDOMAINS[@]}; do
    echo "----------"
    if [[ "$WHOISBIN" = 'jwhois' ]]; then
      echo -n "$d "
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
      if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
      echo -n "$d "
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
      if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    elif [[ "$WHOISBIN" = 'whois' ]]; then
      echo -n "$d "
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ "$tld" = 'edu' ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:/ {print $2}' "${CTMPDIR}/${d}.txt")
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
      if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    fi
    DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    for ip in ${DOMAINIPS[@]}; do
      if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        curl -4s ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
        ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        echo -n "$d "
        echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
        echo
        rm -rf "${CTMPDIR}/${d}-ip.txt"
      fi
    done
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