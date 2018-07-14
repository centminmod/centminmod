#!/bin/bash
##########################################################################
# check centminmod download domain's dns records and whois info
# keep a log of changes and in future alert to changes as a security 
# measure in case a download's domain name dns is compromised or expiry
# lapses and domain is picked up by a rogue or malicious person
##########################################################################
DT=$(date +"%d%m%y-%H%M%S")

DIGOPTS='+nocomments'
DIG_DNSSEC='y'

WHOIS_TIMEOUT='4'
WHOISBIN='whois'
WHOISOPT=' -n'
WHOIS_SHOWNS='y'
WHOIS_SHOWREGISTRAR='y'
WHOIS_SHOWREGISTRANT='n'
WHOIS_NAMESERVER='8.8.8.8'

CHECKDOMAINS_DEBUG='n'
DELETE_TMPLOGS='y'
CTMPDIR=/home/checkdomainstmp
CENTMINLOGDIR='/root/centminlogs'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
##########################################################################
if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi
if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
  WHOISOPT=''
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
  if [[ "$CHECKDOMAINS_DEBUG" = [yY] ]]; then
    sed -i "s/TCP_OUT = \"/TCP_OUT = \"43,/g" /etc/csf/csf.conf
    sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"43,/g" /etc/csf/csf.conf
    egrep '^TCP_|^TCP6_|^UDP_|^UDP6_' /etc/csf/csf.conf
  else
    sed -i "s/TCP_OUT = \"/TCP_OUT = \"43,/g" /etc/csf/csf.conf
    sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"43,/g" /etc/csf/csf.conf
  fi
  csf -ra >/dev/null 2>&1
fi

if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
LISTDOMAINS=$(egrep -rn 'http:|https:|baseurl=|mirrorlist=' /usr/local/src/centminmod/ /etc/yum.repos.d/ | egrep -v 'http://ftp.osuosl.org|\${HTUSER}|\$request_uri|\$vhostname|\${vhostname}|rpm.axivo.com|foo.bar|master.ourdelta.org|newdomain1.com|apt.sw.be|medium.com|href=|my.incapsula.com|#|echo|cecho|<li>|<li class|centos.alt.ru|<|>|\(|\[|\)|\]|<html|<!DOCTYPE|nginx.org|centminmod.com|centmin.com|centmin.sh|github.com|php.net|yum.mariadb.org|apache.mirror.uber.com.au' | sed -e "s|<||g" -e "s|'||g" -e "s|\| bash -s stable||g" | grep -Eo '(http|https|ftp)://[^/"]+' | sed -e "s|http:\/\/||g" -e "s|https:\/\/||g" | sort | uniq -c | sort -rn | awk '{print $2}')
fi

OTHERDOMAINS='nginx.org centminmod.com centmin.com centmin.sh github.com php.net yum.mariadb.org'

for d in ${OTHERDOMAINS[@]}; do
  echo "----------"
  if [[ "$WHOISBIN" = 'jwhois' ]]; then
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    if [[ -z "$DOMAINIPS" ]]; then
      WHOIS_NAMESERVER='4.2.2.2'
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    fi
    whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='no'
    elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='yes'
    else
      dnssec=''
    fi
    echo -n "$d dnssec: ${dnssec} "
    if [[ "$tld" = 'edu' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      fi
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    else
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
      echo
      whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
      echo "$whois_registrant"
    else
      echo
    fi
    
    for ip in ${DOMAINIPS[@]}; do
      if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
        ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        echo -n "$d "
        echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
        echo
        if [[ "$DELETE_TMPLOGS" = [yY] ]]; then        
          rm -rf "${CTMPDIR}/${d}-ip.txt"
        fi
      fi
    done
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      for ns in ${whoisns[@]}; do
        ns=$(echo "$ns" | tr '\n\r' ' ')
        nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
        nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
        echo "$nsiplist" | xargs -n2 | while read nspair; do
          echo "$nspair"
        done
      done
    fi
    if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    if [[ -z "$DOMAINIPS" ]]; then
      WHOIS_NAMESERVER='4.2.2.2'
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    fi
    whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
    whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='no'
    elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='yes'
    else
      dnssec=''
    fi
    echo -n "$d dnssec: ${dnssec} "
    if [[ "$tld" = 'edu' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      fi
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    else
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
      echo
      whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
      echo "$whois_registrant"
    else
      echo
    fi
    
    for ip in ${DOMAINIPS[@]}; do
      if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
        ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        echo -n "$d "
        echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
        echo
        if [[ "$DELETE_TMPLOGS" = [yY] ]]; then        
          rm -rf "${CTMPDIR}/${d}-ip.txt"
        fi
      fi
    done
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      for ns in ${whoisns[@]}; do
        ns=$(echo "$ns" | tr '\n\r' ' ')
        nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
        nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
        echo "$nsiplist" | xargs -n2 | while read nspair; do
          echo "$nspair"
        done
      done
    fi
    if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  elif [[ "$WHOISBIN" = 'whois' ]]; then
    ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
    tld="$(echo "$d" |grep -o '[^.]*$')"
    timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
    DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    if [[ -z "$DOMAINIPS" ]]; then
      WHOIS_NAMESERVER='4.2.2.2'
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
    fi
    whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
    whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${d}.txt")
    if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='no'
    elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
      dnssec='yes'
    else
      dnssec=''
    fi
    echo -n "$d dnssec: ${dnssec} "
    if [[ "$tld" = 'edu' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      else
        whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
      fi
    elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    else
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      else
        whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
      fi
    fi
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
      whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
    fi
    if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
      echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
      echo
    fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
    echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
    if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
      echo
      whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
      echo "$whois_registrant"
    else
      echo
    fi
    
    for ip in ${DOMAINIPS[@]}; do
      if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
        ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
        echo -n "$d "
        echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
        echo
        if [[ "$DELETE_TMPLOGS" = [yY] ]]; then        
          rm -rf "${CTMPDIR}/${d}-ip.txt"
        fi
      fi
    done
    if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
      for ns in ${whoisns[@]}; do
        ns=$(echo "$ns" | tr '\n\r' ' ')
        nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
        nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
        echo "$nsiplist" | xargs -n2 | while read nspair; do
          echo "$nspair"
        done
      done
    fi
    if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
      rm -rf "${CTMPDIR}/${d}.txt"
    fi
  fi
  echo
done

if [[ "$CHECKDOMAINS_DEBUG" != [yY] ]]; then
  for d in ${LISTDOMAINS[@]}; do
    echo "----------"
    if [[ "$WHOISBIN" = 'jwhois' ]]; then
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      if [[ -z "$DOMAINIPS" ]]; then
        WHOIS_NAMESERVER='4.2.2.2'
        DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      fi
      whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='no'
      elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='yes'
      else
        dnssec=''
      fi
      echo -n "$d dnssec: ${dnssec} "
      if [[ "$tld" = 'edu' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        else
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        fi
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      else
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        echo
        whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
        echo "$whois_registrant"
      else
        echo
      fi
      
      for ip in ${DOMAINIPS[@]}; do
        if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
          curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
          ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          echo -n "$d "
          echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
          echo
          if [[ "$DELETE_TMPLOGS" = [yY] ]]; then        
            rm -rf "${CTMPDIR}/${d}-ip.txt"
          fi
        fi
      done
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        for ns in ${whoisns[@]}; do
          ns=$(echo "$ns" | tr '\n\r' ' ')
          nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
          nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
          echo "$nsiplist" | xargs -n2 | while read nspair; do
            echo "$nspair"
          done
        done
      fi
      if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    elif [[ "$WHOISBIN" = 'whois' && "$WHOISOPT" = ' -n' ]]; then
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      if [[ -z "$DOMAINIPS" ]]; then
        WHOIS_NAMESERVER='4.2.2.2'
        DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      fi
      whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
      whoisurl=$(awk  -F ": " '/Registrar URL:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='no'
      elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='yes'
      else
        dnssec=''
      fi
      echo -n "$d dnssec: ${dnssec} "
      if [[ "$tld" = 'edu' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        else
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        fi
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      else
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        echo
        whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
        echo "$whois_registrant"
      else
        echo
      fi
      
      for ip in ${DOMAINIPS[@]}; do
        if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
          curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
          ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          echo -n "$d "
          echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
          echo
          if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
            rm -rf "${CTMPDIR}/${d}-ip.txt"
          fi
        fi
      done
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        for ns in ${whoisns[@]}; do
          ns=$(echo "$ns" | tr '\n\r' ' ')
          nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
          nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
          echo "$nsiplist" | xargs -n2 | while read nspair; do
            echo "$nspair"
          done
        done
      fi
      if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    elif [[ "$WHOISBIN" = 'whois' ]]; then
      ctoplevel="$(echo "$d" |grep -o '[^.]*\.[^.]*$')"
      tld="$(echo "$d" |grep -o '[^.]*$')"
      timeout ${WHOIS_TIMEOUT}s ${WHOISBIN}${WHOISOPT} "$ctoplevel" > "${CTMPDIR}/${d}.txt"
      DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      if [[ -z "$DOMAINIPS" ]]; then
        WHOIS_NAMESERVER='4.2.2.2'
        DOMAINIPS=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $d)
      fi
      whoisdnssec=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} ${d} +dnssec +nocomments dnskey +short)
      whoisurl=$(awk  -F ": " '/Registrar:/ {print $2}' "${CTMPDIR}/${d}.txt")
      if [[ -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='no'
      elif [[ ! -z "$whoisdnssec" && "$DIG_DNSSEC" = [yY] ]]; then
        dnssec='yes'
      else
        dnssec=''
      fi
      echo -n "$d dnssec: ${dnssec} "
      if [[ "$tld" = 'edu' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        else
          whoisdate=$(awk  -F ": " '/Domain expires:/ {print $2}' "${CTMPDIR}/${d}.txt" | tr -s ' ')
        fi
      elif [[ "$tld" = 'au' && "$ctoplevel" = 'com.au' ]]; then
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Last Modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      else
        if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
          whoisdate=$(awk  -F ": " '/Expiration Date:|Expiry Date:|expires at:|expires:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        else
          whoisdate=$(awk  -F ": " '/Expiry Date:/ {print $2}' "${CTMPDIR}/${d}.txt")
          whoisupdate=$(awk  -F ": " '/Updated Date:|modified:/ {print $2}' "${CTMPDIR}/${d}.txt")
        fi
      fi
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        whoisnsa=$(awk  -F ": " '/Name Server:|nserver:|Nserver:/ {print $2}' "${CTMPDIR}/${d}.txt")
        whoisns=$(echo "$whoisnsa" | tr '[:upper:]' '[:lower:]')
      fi
      if [[ "$WHOIS_SHOWREGISTRAR" = [yY] ]]; then
        echo "registrar: $whoisurl" | tr '\n\r' ' ' | tr -s ' '
        echo
      fi
      if [ -z "$whoisdate" ]; then
        whoisdate=' - '
      else
        whoisdate=$(date -d "$whoisdate" "+%b %d %Y")
      fi
      if [ -z "$whoisupdate" ]; then
        whoisupdate=' - '
      else
        whoisupdate=$(date -d "$whoisupdate" "+%b %d %Y")
      fi  
      echo "expiry: $whoisdate updated: $whoisupdate" | tr '\n\r' ' ' | tr -s ' '
      if [[ "$WHOIS_SHOWREGISTRANT" = [yY] ]]; then
        echo
        whois_registrant=$(egrep -i 'registrant name:|registrant email:|admin name:|admin email:' "${CTMPDIR}/${d}.txt")
        echo "$whois_registrant"
      else
        echo
      fi
      
      for ip in ${DOMAINIPS[@]}; do
        if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
          curl -${ipv_forceopt}s https://ipinfo.io/$ip 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' > "${CTMPDIR}/${d}-ip.txt"
          ipaddr=$(awk -F ": " '/ip:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          country=$(awk -F ": " '/country:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          org=$(awk -F ": " '/org:/ {print $2}' "${CTMPDIR}/${d}-ip.txt")
          echo -n "$d "
          echo "$ipaddr $country $org" | tr '\n\r' ' ' | tr -s ' '
          echo
          if [[ "$DELETE_TMPLOGS" = [yY] ]]; then        
            rm -rf "${CTMPDIR}/${d}-ip.txt"
          fi
        fi
      done
      if [[ "$WHOIS_SHOWNS" = [yY] ]]; then
        for ns in ${whoisns[@]}; do
          ns=$(echo "$ns" | tr '\n\r' ' ')
          nsip=$(dig -4 ${DIGOPTS} @${WHOIS_NAMESERVER} +short A $ns)
          nsiplist=$(echo "$ns $nsip" | tr '\n\r' ' ' | tr -s ' ')
          echo "$nsiplist" | xargs -n2 | while read nspair; do
            echo "$nspair"
          done
        done
      fi
      if [[ "$DELETE_TMPLOGS" = [yY] ]]; then
        rm -rf "${CTMPDIR}/${d}.txt"
      fi
    fi
    echo
  done
fi
}

case "$1" in
  check )
    {
    check_domains
    } 2>&1 | tee ${CENTMINLOGDIR}/check-domains-dns_${DT}.log
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