check_dns() {
  vhostname_dns="$1"
    # if CHECKIDN = 0 then internationalized domain name which not supported by letsencrypt
    CHECKIDN=$(echo $vhostname_dns | grep '^xn--' >/dev/null 2>&1; echo $?)
    if [[ "$CHECKIDN" = '0' ]]; then
      TOPLEVELCHECK=$(dig soa @8.8.8.8 $vhostname_dns | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//' | idn)
    else
      TOPLEVELCHECK=$(dig soa @8.8.8.8 $vhostname_dns | grep -v ^\; | grep SOA | awk '{print $1}' | sed 's/\.$//')
    fi
    if [[ "$TOPLEVELCHECK" = "$vhostname_dns" ]]; then
      # top level domain
      TOPLEVEL=y
    elif [[ -z "$TOPLEVELCHECK" ]]; then
      # vhost dns not setup
      TOPLEVEL=z
      # if [[ "$(echo $vhostname_dns | grep -o "\." | wc -l)" -le '1' ]]; then
      #   TOPLEVEL=y
      # else
      #   TOPLEVEL=n
      # fi
    else
      # subdomain or non top level domain
      TOPLEVEL=n
    fi
    echo
    if [[ "$TOPLEVEL" = [yY] ]]; then
      VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $vhostname_dns)
      VHOST_AWWWCHECK=$(dig -t A +short @8.8.8.8 www.$vhostname_dns | grep -v $vhostname_dns)
    else
      VHOST_ACHECK=$(dig -t A +short @8.8.8.8 $vhostname_dns)
    fi
    echo
    if [[ "$TOPLEVEL" = [yY] ]]; then
      cecho "$vhostname_dns is a top level domain" $boldyellow  
      if [ "$VHOST_ACHECK" ]; then
        cecho "your server IP address: $CNIP" $boldyellow
        cecho "current DNS A record IP address for $vhostname_dns is: $VHOST_ACHECK" $boldyellow
        if [[ "$(ifconfig | grep 'inet' | grep $VHOST_ACHECK >/dev/null 2>&1; echo $?)" != '0' ]]; then
          echo
          cecho "!! Error: DNS A record IP doesn't match any found on this server" $boldyellow
          echo
          read -ep "Abort this Nginx vhost domain setup to setup updated DNS A record(s) first? [y/n]: " letabort
          if [[ "$letabort" = [yY] ]]; then
            exit
          fi
        fi
      else
        cecho "your server IP address: $CNIP" $boldyellow
        cecho "current DNS A record IP address for $vhostname_dns is: $VHOST_ACHECK" $boldyellow
        cecho "!! Error: missing DNS A record for $vhostname_dns" $boldyellow
        echo
        read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
        if [[ "$letabort" = [yY] ]]; then
          exit
        fi
      fi
      if [ "$VHOST_AWWWCHECK" ]; then
        cecho "current DNS A record IP address for www.$vhostname_dns is: $VHOST_AWWWCHECK" $boldyellow
        if [[ "$(ifconfig | grep 'inet' | grep $VHOST_AWWWCHECK >/dev/null 2>&1; echo $?)" != '0' ]]; then
          echo
          cecho "!! Error: DNS A record IP doesn't match any found on this server" $boldyellow
          echo
          read -ep "Abort this Nginx vhost domain setup to setup updated DNS A record(s) first? [y/n]: " letabort
          if [[ "$letabort" = [yY] ]]; then
            exit
          fi
        fi
      else
        cecho "current DNS A record IP address for www.$vhostname_dns is: $VHOST_AWWWCHECK" $boldyellow
        cecho "!! Error: missing DNS A record for www.$vhostname_dns" $boldyellow
        echo
        read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
        if [[ "$letabort" = [yY] ]]; then
          exit
        fi
      fi
    elif [[ "$TOPLEVEL" = 'z' ]]; then
      cecho "!! Error: $vhostname_dns DNS records not found or setup properly yet or $vhostname_dns invalid" $boldyellow
      echo
      read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
      if [[ "$letabort" = [yY] ]]; then
        exit
      fi
    else
      cecho "$vhostname_dns is not a top level domain" $boldyellow
      if [ "$VHOST_ACHECK" ]; then
        cecho "your server IP address: $CNIP" $boldyellow
        cecho "current DNS A record IP address for $vhostname_dns is: $VHOST_ACHECK" $boldyellow
        if [[ "$(ifconfig | grep 'inet' | grep $VHOST_ACHECK >/dev/null 2>&1; echo $?)" != '0' ]]; then
          echo
          cecho "!! Error: DNS A record IP doesn't match any found on this server" $boldyellow
          echo
          read -ep "Abort this Nginx vhost domain setup to setup updated DNS A record(s) first? [y/n]: " letabort
          if [[ "$letabort" = [yY] ]]; then
            exit
          fi
        fi
      else
        cecho "current DNS A record IP address for $vhostname_dns is: $VHOST_ACHECK" $boldyellow
        cecho "!! Error: missing DNS A record for $vhostname_dns" $boldyellow
        echo
        read -ep "Abort this Nginx vhost domain setup to setup proper DNS A record(s) first? [y/n]: " letabort
        if [[ "$letabort" = [yY] ]]; then
          exit
        fi
      fi
    fi
    echo
    read -ep "Do you want to continue [y/n]: " let_continue
    if [[ "$let_continue" != [yY] ]]; then
      exit
    fi
}

lemsgdns() {
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "To get Letsencrypt SSL certificate, you must already have updated intended" $boldgreen
  cecho "domain vhost name's DNS A record to this server's IP addresss." $boldgreen
  cecho "If top level domain, DNS A record is needed also for www. version of domain" $boldgreen
  cecho "otherwise, Letsencrypt domain name validation will fail." $boldgreen
  cecho "---------------------------------------------------------------" $boldyellow
  read -ep "continue [y/n] ? " _lemsgdnsproceed
  if [[ "$_lemsgdnsproceed" != [yY] ]]; then
    echo
    echo "aborting..."
    echo
    exit
  fi
}

info_notice() {
cecho "---------------------------------------------------------------" $boldyellow
cecho "Important Information" $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo
echo "You are about to create an Nginx vhost site account with/without"
echo "HTTPS/SSL support. Details of this process are outlined on site"
echo "at centminmod.com/nginx_domain_dns_setup.html. Also read the"
echo "continually updated Getting Started Guide for Centmin Mod usage"
echo "at centminmod.com/getstarted.html which covers the pure-ftpd"
echo "ftp username that is auto generated with the Nginx vhost site."
cecho "---------------------------------------------------------------" $boldyellow
echo "403 Permission denied message handling"
echo "if after vhost site setup you encounter 403 permission denied errors,"
echo "check https://community.centminmod.com/threads/7308/ to see if your"
echo "site needs tools/autoprotect.sh tweaking/whitelisting"
cecho "---------------------------------------------------------------" $boldyellow
if [[ "$LETSENCRYPT_DETECT" != [yY] ]]; then
echo "[ LETSENCRYPT_DETECT is not enabled ]"
echo "Ignore this message if you do not want HTTPS based web site otherwise"
echo "read below carefully."
echo
echo "Free letsencrypt SSL certificates integration is in beta testing if"
echo "you want to obtain free letsencrypt SSL certificate for HTTPS site,"
echo "you will need to manually enable LETSENCRYPT_DETECT='y' outlined"
echo "at https://centminmod.com/acmetool so exit this vhost routine first"
echo "set LETSENCRYPT_DETECT='y' and update domain DNS A record first"
echo "then re-run vhost site creation menu option"
cecho "---------------------------------------------------------------" $boldyellow
fi
echo
read -ep "Do you want to continue with Nginx vhost site creation ? [y/n] " dovhost_continue
echo

if [[ "$dovhost_continue" != [yY] ]]; then
  echo "aborting Nginx vhost setup..."
  exit
fi
}

sslvhost() {

cecho "---------------------------------------------------------------" $boldyellow
cecho "SSL Vhost Setup..." $boldgreen
cecho "---------------------------------------------------------------" $boldyellow
echo ""

if [ ! -f /usr/local/nginx/conf/ssl ]; then
  mkdir -p /usr/local/nginx/conf/ssl
fi

if [ ! -d /usr/local/nginx/conf/ssl/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/${vhostname}
fi

# cloudflare authenticated origin pull cert
# setup https://community.centminmod.com/threads/13847/
if [ ! -d /usr/local/nginx/conf/ssl/cloudflare/${vhostname} ]; then
  mkdir -p /usr/local/nginx/conf/ssl/cloudflare/${vhostname}
  wget $CLOUDFLARE_AUTHORIGINPULLCERT -O origin.crt
fi

if [ ! -f /usr/local/nginx/conf/ssl_include.conf ]; then
cat > "/usr/local/nginx/conf/ssl_include.conf"<<EVS
ssl_session_cache      shared:SSL:10m;
ssl_session_timeout    60m;
ssl_protocols  TLSv1 TLSv1.1 TLSv1.2;
EVS
fi

cd /usr/local/nginx/conf/ssl/${vhostname}

cecho "---------------------------------------------------------------" $boldyellow
cecho "Generating self signed SSL certificate..." $boldgreen
cecho "CSR file can also be used to be submitted for paid SSL certificates" $boldgreen
cecho "If using for paid SSL certificates be sure to keep both private key and CSR safe" $boldgreen
cecho "creating CSR File: ${vhostname}.csr" $boldgreen
cecho "creating private key: ${vhostname}.key" $boldgreen
cecho "creating self-signed SSL certificate: ${vhostname}.crt" $boldgreen
sleep 9

if [[ -z "$SELFSIGNEDSSL_O" ]]; then
  SELFSIGNEDSSL_O="$vhostname"
else
  SELFSIGNEDSSL_O="$SELFSIGNEDSSL_O"
fi

if [[ -z "$SELFSIGNEDSSL_OU" ]]; then
  SELFSIGNEDSSL_OU="$vhostname"
else
  SELFSIGNEDSSL_OU="$SELFSIGNEDSSL_OU"
fi

openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}.csr -keyout ${vhostname}.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"
openssl x509 -req -days 36500 -sha256 -in ${vhostname}.csr -signkey ${vhostname}.key -out ${vhostname}.crt

# echo
# cecho "---------------------------------------------------------------" $boldyellow
# cecho "Generating backup CSR and private key for HTTP Public Key Pinning..." $boldgreen
# cecho "creating CSR File: ${vhostname}-backup.csr" $boldgreen
# cecho "creating private key: ${vhostname}-backup.key" $boldgreen
# sleep 5

# openssl req -new -newkey rsa:2048 -sha256 -nodes -out ${vhostname}-backup.csr -keyout ${vhostname}-backup.key -subj "/C=${SELFSIGNEDSSL_C}/ST=${SELFSIGNEDSSL_ST}/L=${SELFSIGNEDSSL_L}/O=${SELFSIGNEDSSL_O}/OU=${SELFSIGNEDSSL_OU}/CN=${vhostname}"

# echo
# cecho "---------------------------------------------------------------" $boldyellow
# cecho "Extracting Base64 encoded information for primary and secondary" $boldgreen
# cecho "private key's SPKI - Subject Public Key Information" $boldgreen
# cecho "Primary private key - ${vhostname}.key" $boldgreen
# cecho "Backup private key - ${vhostname}-backup.key" $boldgreen
# cecho "For HPKP - HTTP Public Key Pinning hash generation..." $boldgreen
# sleep 5

# echo
# cecho "extracting SPKI Base64 encoded hash for primary private key = ${vhostname}.key ..." $boldgreen

# openssl rsa -in ${vhostname}.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt

# echo
# cecho "extracting SPKI Base64 encoded hash for backup private key = ${vhostname}-backup.key ..." $boldgreen

# openssl rsa -in ${vhostname}-backup.key -outform der -pubout | openssl dgst -sha256 -binary | openssl enc -base64 | tee -a /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt

# echo
# cecho "HTTP Public Key Pinning Header for Nginx" $boldgreen

# echo
# cecho "for 7 days max-age including subdomains" $boldgreen
# echo
# echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400; includeSubDomains';"

# echo
# cecho "for 7 days max-age excluding subdomains" $boldgreen
# echo
# echo "add_header Public-Key-Pins 'pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-primary-pin.txt)\"; pin-sha256=\"$(cat /usr/local/nginx/conf/ssl/${vhostname}/hpkp-info-secondary-pin.txt)\"; max-age=86400';"

if [[ ! -f "$(find /usr/local/nginx/conf/ssl -type f -name "dhparam.pem" | head -n1)" ]]; then
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "Generating dhparam.pem file - can take a few minutes..." $boldgreen 
  dhparamstarttime=$(TZ=UTC date +%s.%N) 
  openssl dhparam -out dhparam.pem 2048
  dhparamendtime=$(TZ=UTC date +%s.%N)
  DHPARAMTIME=$(echo "$dhparamendtime-$dhparamstarttime"|bc)
  cecho "dhparam file generation time: $DHPARAMTIME" $boldyellow
else
  echo
  cecho "---------------------------------------------------------------" $boldyellow
  cecho "Copy/setup dhparam.pem file..." $boldgreen
  cp -a "$(find /usr/local/nginx/conf/ssl -type f -name "dhparam.pem" | head -n1)" .
fi
}

funct_nginxaddvhost() {
PUREUSER=nginx
PUREGROUP=nginx
    if [ "$SECOND_IP" ]; then
      CNIP="$SECOND_IP"
    else
      CNIP=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
    fi
# CURRENTIP=$(echo $SSH_CLIENT | awk '{print $1}')
# CURRENTCOUNTRY=$(curl -s https://ipinfo.io/$CURRENTIP/country)
pureftpinstall

# Support secondary dedicated IP configuration for centmin mod
# nginx vhost generator, so out of the box, new nginx vhosts 
# generated will use the defined SECOND_IP=111.222.333.444 where
# the IP is a secondary IP addressed added to the server.
# You define SECOND_IP variable is centmin mod persistent config
# file outlined at http://centminmod.com/upgrade.html#persistent
# you manually creat the file at /etc/centminmod/custom_config.inc
# and add SECOND_IP=yoursecondary_IPaddress variable to it which
# will be registered with nginx vhost generator routine so that 
# any new nginx vhosts created via centmin.sh menu option 2 or
# /usr/bin/nv or centmin.sh menu option 22, will have pre-defined
# SECOND_IP ip address set in the nginx vhost's listen directive
if [[ -z "$SECOND_IP" ]]; then
  DEDI_IP=""
  DEDI_LISTEN=""
elif [[ "$SECOND_IP" ]]; then
  DEDI_IP=$(echo $(echo ${SECOND_IP}:))
  DEDI_LISTEN="listen   ${DEDI_IP}80;"
fi

echo ""
cecho "---------------------------------------------" $boldyellow
echo

info_notice

read -ep "Enter vhost domain name to add (without www. prefix): " vhostname

if [ -d "/home/nginx/domains/$vhostname" ]; then
  echo ""
  cecho "-------------------------------------------------------------" $boldyellow
  cecho "vhost for $vhostname already exists" $boldwhite
  cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
  cecho "if you need to delete the domain read the guide at: " $boldwhite
  cecho "centminmod.com/nginx_domain_dns_setup.html#deletevhost" $boldwhite
  cecho "-------------------------------------------------------------" $boldyellow
  echo ""
  exit 1
fi

# check to make sure you don't add a domain name vhost that matches
# your server main hostname setup in server_name within main hostname
# nginx vhost at /usr/local/nginx/conf/conf.d/virtual.conf
if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
  CHECK_MAINHOSTNAME=$(awk '/server_name/ {print $2}' /usr/local/nginx/conf/conf.d/virtual.conf | sed -e 's|;||')
  if [[ "${CHECK_MAINHOSTNAME}" = "${vhostname}" ]]; then
    echo
    echo " Error: $vhostname is already setup for server main hostname"
    echo " at /usr/local/nginx/conf/conf.d/virtual.conf"
    echo " It is important that main server hostname be setup correctly"
    echo
    echo " As per Getting Started Guide Step 1 centminmod.com/getstarted.html"
    echo " The server main hostname needs to be unique. So please setup"
    echo " the main server name vhost properly first as per Step 1 of guide."
    echo
    echo " Aborting nginx vhost creation..."
    echo
    exit 1
  fi
fi

if [[ "$NGINX_VHOSTSSL" = [yY] ]]; then
  echo
  read -ep "Create a self-signed SSL certificate Nginx vhost? [y/n]: " vhostssl
  if [[ -f "${SCRIPT_DIR}/addons/acmetool.sh" && "$LETSENCRYPT_DETECT" = [yY] ]]; then
    read -ep "Get Letsencrypt SSL certificate Nginx vhost? [y/n]: " vhostssl_le
    if [[ "$vhostssl_le" = [yY] ]]; then
      lemsgdns
      check_dns $vhostname
      # echo
      # cecho "------------------------------------------------" $boldyellow
      # echo "Important Note For HTTPS Default Options"
      # cecho "------------------------------------------------" $boldyellow
      # echo "For HTTPS default options note you can not automatically"
      # echo "switch back to non-HTTPS mode, it will be a manual process"
      # echo "you have to do yourself. Re-running HTTP + HTTPS mode after"
      # echo "running HTTPS default will break your non-HTTPS site until"
      # echo "you manually fix it by recreating the non-HTTPS nginx vhost."
      # echo "There maybe a previous backup copy of non-HTTPS vhost at"
      # echo "/usr/local/nginx/conf/acmevhostbackup created when you first"
      # echo "switched to HTTPS default which you need to copy back to"
      # echo "/usr/local/nginx/conf/conf.d/yourdomain.com.conf"
      # cecho "------------------------------------------------" $boldyellow
      echo
      cecho "------------------------------------------------" $boldyellow
      echo "You have 4 options: "
      cecho "------------------------------------------------" $boldyellow
      echo "1. issue staging test cert with HTTP + HTTPS (untrusted)"
      echo "2. issue staging test cert with HTTPS default (untrusted)"
      echo "3. issue live cert with HTTP + HTTPS (trusted)"
      echo "4. issue live cert with HTTPS default (trusted)"
      read -ep "Enter option number 1-4: " vhostssl_opt
      if [[ "$vhostssl_opt" = '1' ]]; then
        vhostssl='le'
      elif [[ "$vhostssl_opt" = '2' ]]; then
        vhostssl='led'
      elif [[ "$vhostssl_opt" = '3' ]]; then
        vhostssl='lelive'
      elif [[ "$vhostssl_opt" = '4' ]]; then
        vhostssl='lelived'
      fi
      echo
    fi
  fi
fi

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  if [ ! -f /usr/sbin/cracklib-check ]; then
    yum -y -q install cracklib
  fi
  if [ ! -f /usr/bin/pwgen ]; then
    yum -y -q install pwgen
  fi
  echo
  read -ep "Create FTP username for vhost domain (enter username): " ftpuser
  read -ep "Auto generate FTP password (recommended) [y/n]: " autogenpass
  # echo

  if [[ "$autogenpass" = [yY] ]]; then
    ftppass=$(pwgen -1cnys 21)
  else
    read -ep "Create FTP password for $ftpuser (enter password): " ftppass
  
    # simple password strength check
    # utilise http://cracklib.sourceforge.net/ too
    CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
    okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    while [[ "$okay" != "OK" ]]; do
      echo "!! password strength not strong enough !! "
      echo "!! do not use common dictionary words !! "
      echo "!! do not use short passwords !! "
      echo "!! do not use simplistic passwords !! "
      echo
      read -ep "re-enter FTP password for $ftpuser (enter password): " ftppass
      CHECKPASSWD="$(cracklib-check <<<"$ftppass")"
      okay="$(awk -F': ' '{ print $2}' <<<"$CHECKPASSWD")"
    done
  fi # autogenpass
  echo
  echo "FTP username you entered: $ftpuser"
  if [[ "$autogenpass" = [yY] ]]; then
    echo "FTP password auto generated: $ftppass"
  else
    echo "FTP password you entered: $ftppass"    
  fi
fi

echo ""

if [ ! -d "/home/nginx/domains/$vhostname" ]; then

# Checking Permissions, making directories, example index.html
umask 027
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}
ngx_logformats

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  ( echo "${ftppass}" ; echo "${ftppass}" ) | pure-pw useradd "$ftpuser" -u $PUREUSER -g $PUREGROUP -d "/home/nginx/domains/$vhostname"
  pure-pw mkdb
fi

cat > "/home/nginx/domains/$vhostname/public/index.html" <<END
<!DOCTYPE html>
<html lang="en"><head>
  <meta charset="utf-8"><meta content="width=device-width, initial-scale=1.0" name="viewport">
  <meta content="${vhostname} nginx site generated by centminmod.com" name="description">
  <title>${vhostname}</title>
  <link href="//centminmod.com/purecss/pure-min.css" rel="stylesheet"><!--[if lte IE 8]>
  <link rel="stylesheet" href="//centminmod.com/purecss/grids-responsive-old-ie-min.css">
  <![endif]-->
  <!--[if gt IE 8]><!-->
  <link href="//centminmod.com/purecss/grids-responsive-min.css" rel="stylesheet"><!--<![endif]-->
  <!--[if gt IE 8]><!-->
  <style type="text/css">
  *{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box}
  a{text-decoration:none;color:#3d92c9}
  a:hover,a:focus{text-decoration:underline}
  h3{font-weight:100}
  .pure-img-responsive{max-width:100%;height:auto}
  #layout{padding:0}
  .header{text-align:center;top:auto;margin:3em auto}
  .sidebar{background:#2e739a;color:#fff}
  .brand-title,.brand-tagline{margin:0}
  .brand-title{text-transform:uppercase}
  .brand-tagline{font-weight:300;color:#b0cadb}
  .nav-list{margin:0;padding:0;list-style:none}
  .nav-item{display:inline-block;*display:inline;zoom:1}
  .nav-item a{background:transparent;border:2px solid #b0cadb;color:#fff;margin-top:1em;letter-spacing:.05em;text-transform:uppercase;font-size:85%}
  .nav-item a:hover,.nav-item a:focus{border:2px solid #3d92c9;text-decoration:none}
  .content-subhead{text-transform:uppercase;color:#aaa;border-bottom:1px solid #eee;padding:.4em 0;font-size:80%;font-weight:500;letter-spacing:.1em}
  .content{padding:2em 1em 0}
  .post{padding-bottom:2em}
  .post-title{font-size:2em;color:#222;margin-bottom:.2em}
  .post-avatar{border-radius:50px;float:right;margin-left:1em}
  .post-description{font-family:Georgia,"Cambria",serif;color:#444;line-height:1.8em}
  .post-meta{color:#999;font-size:90%;margin:0}
  .post-category{margin:0 .1em;padding:.3em 1em;color:#fff;background:#999;font-size:80%}
  .post-category-design{background:#5aba59}
  .post-category-pure{background:#4d85d1}
  .post-category-yui{background:#8156a7}
  .post-category-js{background:#df2d4f}
  .post-images{margin:1em 0}
  .post-image-meta{margin-top:-3.5em;margin-left:1em;color:#fff;text-shadow:0 1px 1px #333}
  .footer{text-align:center;padding:1em 0}
  .footer a{color:#ccc;font-size:80%}
  .footer .pure-menu a:hover,.footer .pure-menu a:focus{background:none}
  @media (min-width: 48em) {
  .content{padding:2em 3em 0;margin-left:25%}
  .header{margin:80% 2em 0;text-align:right}
  .sidebar{position:fixed;top:0;bottom:0}
  }
  </style><!--<![endif]-->
</head>
<body>
  <div class="pure-g" id="layout">
    <div class="sidebar pure-u-1 pure-u-md-1-4">
      <div class="header">
        <h1 class="brand-title">Welcome to ${vhostname}</h1>
        <h2 class="brand-tagline">Powered by CentminMod</h2>
        <h2 class="brand-tagline">Nginx Server</h2>
        <nav class="nav">
          <ul class="nav-list">
            <li class="nav-item">
              <a class="pure-button" href="https://centminmod.com">CentminMod.com</a>
            </li>
            <li class="nav-item">
              <a class="pure-button" href="https://community.centminmod.com">CentminMod Forums</a>
            </li>
          </ul>
        </nav>
      </div>
    </div>
    <div class="content pure-u-1 pure-u-md-3-4">
      <div>
        <!-- A wrapper for all the blog posts -->
        <div class="posts">
          <h1 class="content-subhead">index.html place holder</h1><!-- A single blog post -->
          <section class="post">
            <header class="post-header">
              <h2 class="post-title">${vhostname}</h2>
            </header>
            <div class="post-description">
              <p>Welcome to ${vhostname}. This index.html page can be removed.</p>
              <p>Useful Centmin Mod info and links to bookmark.</p>
              <ul>
                <li>Getting Started Guide - <a href="https://centminmod.com/getstarted.html" target="_blank" rel="noopener">https://centminmod.com/getstarted.html</a>
                </li>
                <li>Latest Centmin Mod version - <a href="https://centminmod.com" target="_blank" rel="noopener">https://centminmod.com</a>
                </li>
                <li>Centmin Mod FAQ - <a href="https://centminmod.com/faq.html" target="_blank" rel="noopener">https://centminmod.com/faq.html</a>
                </li>
                <li>Change Log - <a href="https://centminmod.com/changelog.html" target="_blank" rel="noopener">https://centminmod.com/changelog.html</a>
                </li>
                <li>Google+ Page latest news <a href="https://plus.google.com/u/0/b/104831941868856035845/104831941868856035845" target="_blank" rel="noopener">Centmin Mod Google+</a>
                </li>
                <li>Centmin Mod Community Forum <a href="https://community.centminmod.com/" target="_blank" rel="noopener">https://community.centminmod.com/</a>
                </li>
                <li>Centmin Mod Twitter <a href="https://twitter.com/centminmod" target="_blank" rel="noopener">https://twitter.com/centminmod</a>
                </li>
                <li>Centmin Mod Facebook Page <a href="https://www.facebook.com/centminmodcom" target="_blank" rel="noopener">https://www.facebook.com/centminmodcom</a>
                </li>
                <li>Centmin Mod Medium <a href="https://medium.com/@centminmod" target="_blank" rel="noopener">https://medium.com/@centminmod</a>
                </li>
              </ul>
              <p>For Centmin Mod LEMP stack hosting check out <a href="https://www.digitalocean.com/?refcode=c1cb367108e8" target="_blank">Digitalocean</a></p>
              
              <p><b>Disclaimer</b></p>
              <p><a href="https://centminmod.com/">Centmin Mod</a> is a free open source software for CentOS Linux that can be downloaded and installed by anybody and was installed on this server by a 3rd party end user with no relation to Centmin Mod. Centmin Mod has no control over and is not responsible for the content contained on this site.</p>
            </div>
          </section>
        </div>
        <div class="footer">
          <div class="pure-menu pure-menu-horizontal">
            <ul>
              <li class="pure-menu-item">
                <a class="pure-menu-link" href="#">PureCSS Template BSD Licensed Copyright 2016 Yahoo! Inc. All rights reserved</a>
              </li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  </div>
</body>
</html>
END

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/$vhostname/public
umask 022
chown -R nginx:nginx "/home/nginx/domains/$vhostname"
find "/home/nginx/domains/$vhostname" -type d -exec chmod g+s {} \;

# Setting up Nginx mapping

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
  sslvhost
fi

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then

  if [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]; then
      # check /svr-setup/openssl-1.0.2f/crypto/chacha20poly1305/chacha20.c exists
      OPEENSSL_CFPATCHED='y'
  elif [ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]; then
      # for openssl 1.1.0 native chacha20 support
      OPEENSSL_CFPATCHED='y'
  fi

if [[ "$(nginx -V 2>&1 | grep LibreSSL | head -n1)" ]] || [[ "$OPEENSSL_CFPATCHED" = [yY] ]]; then
  if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha20poly1305/chacha20.c" ]]; then
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  elif [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/crypto/chacha/chacha_enc.c" ]]; then
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  else
    CHACHACIPHERS='ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:'
  fi
else
  CHACHACIPHERS=""
fi

DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
    # openssl 1.1.0 unsupported flag enable-tlsext
    if [[ "$(grep -w 'tls1_3' "${DIR_TMP}/openssl-${OPENSSL_VERSION}/configdata.pm")" ]]; then
        TLSONETHREEOPT=' enable-tls1_3'
        TLSONETHREE_DETECT='y'
    else
        TLSONETHREEOPT=""
        TLSONETHREE_DETECT='n'
    fi
fi

if [[ "$TLSONETHREE_DETECT" = [yY] ]]; then
  TLSONETHREE_CIPHERS='TLS13-AES-128-GCM-SHA256:TLS13-AES-256-GCM-SHA384:TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES-128-CCM-8-SHA256:TLS13-AES-128-CCM-SHA256:'
else
  TLSONETHREE_CIPHERS=""
fi

if [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]] && [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_spdy_module')" = 'with-http_spdy_module' ]]; then
  HTTPTWO=y
  LISTENOPT='ssl spdy http2'
  COMP_HEADER='spdy_headers_comp 5'
  SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
  HTTPTWO_MAXFIELDSIZE='http2_max_field_size 16k;'
  HTTPTWO_MAXHEADERSIZE='http2_max_header_size 32k;'  
elif [[ "$(nginx -V 2>&1 | grep -Eo 'with-http_v2_module')" = 'with-http_v2_module' ]]; then
  HTTPTWO=y
  if [[ "$(grep -rn listen /usr/local/nginx/conf/conf.d/ | grep -v '#' | grep 443 | grep ' ssl' | grep ' http2' | grep reuseport | awk -F ':  ' '{print $2}' | grep -o reuseport)" != 'reuseport' ]]; then
    # check if reuseport is supported for listen 443 port - only needs to be added once globally for all nginx vhosts
    NGXVHOST_CHECKREUSEPORT=$(grep --color -Ro SO_REUSEPORT /usr/src/kernels/* | head -n1 | awk -F ":" '{print $2}')
    if [[ "$NGXVHOST_CHECKREUSEPORT" = 'SO_REUSEPORT' ]]; then
      ADD_REUSEPORT=' reuseport'
    else
      ADD_REUSEPORT=""
    fi
    LISTENOPT="ssl http2${ADD_REUSEPORT}"
  else
    LISTENOPT='ssl http2'
  fi
  COMP_HEADER='#spdy_headers_comp 5'
  SPDY_HEADER='#add_header Alternate-Protocol  443:npn-spdy/3;'
  HTTPTWO_MAXFIELDSIZE='http2_max_field_size 16k;'
  HTTPTWO_MAXHEADERSIZE='http2_max_header_size 32k;'
else
  HTTPTWO=n
  LISTENOPT='ssl spdy'
  COMP_HEADER='spdy_headers_comp 5'
  SPDY_HEADER='add_header Alternate-Protocol  443:npn-spdy/3;'
fi

if [[ -f /usr/bin/php73 && -f /usr/bin/php72 && -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php73-remi.conf;
  #include /usr/local/nginx/conf/php72-remi.conf;
  #include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php72 && -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php72-remi.conf;
  #include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php71 && -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;
  #include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ -f /usr/bin/php71 && -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;
  #include /usr/local/nginx/conf/php70-remi.conf;'
elif [[ -f /usr/bin/php71 && ! -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php71-remi.conf;'
elif [[ ! -f /usr/bin/php71 && -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php70-remi.conf;'
elif [[ ! -f /usr/bin/php71 && ! -f /usr/bin/php70 && -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES='#include /usr/local/nginx/conf/php56-remi.conf;'
elif [[ ! -f /usr/bin/php71 && ! -f /usr/bin/php70 && ! -f /usr/bin/php56 ]]; then
  MULTIPHP_INCLUDES=""
fi

if [[ "$VHOST_PRESTATICINC" = [yY] ]]; then
  PRESTATIC_INCLUDES="include /usr/local/nginx/conf/pre-staticfiles-local-${vhostname}.conf;
  include /usr/local/nginx/conf/pre-staticfiles-global.conf;"
  touch "/usr/local/nginx/conf/pre-staticfiles-local-${vhostname}.conf"
  touch /usr/local/nginx/conf/pre-staticfiles-global.conf
else
  PRESTATIC_INCLUDES=""
fi

if [[ "$VHOST_CFAUTHORIGINPULL" = [yY] ]]; then
  CFAUTHORIGINPULL_INCLUDES="# cloudflare authenticated origin pull cert community.centminmod.com/threads/13847/
  #ssl_client_certificate /usr/local/nginx/conf/ssl/cloudflare/$vhostname/origin.crt;
  #ssl_verify_client on;"
else
  CFAUTHORIGINPULL_INCLUDES=""
fi

# main non-ssl vhost at yourdomain.com.conf
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<ENSS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   ${DEDI_IP}80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
  $DEDI_LISTEN
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  # prevent access to ./directories and files
  #location ~ (?:^|/)\. {
  # deny all;
  #}

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Wordpress Permalinks example
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args;

  }

  ${PRESTATIC_INCLUDES}
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  ${MULTIPHP_INCLUDES}
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
ENSS

# separate ssl vhost at yourdomain.com.ssl.conf
cat > "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"<<ESS
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html
# For HTTP/2 SSL Setup
# read http://centminmod.com/nginx_configure_https_ssl_spdy.html

# redirect from www to non-www  forced SSL
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
# server {
#       listen   ${DEDI_IP}80;
#       server_name ${vhostname} www.${vhostname};
#       return 302 https://\$server_name\$request_uri;
# }

server {
  listen ${DEDI_IP}443 $LISTENOPT;
  server_name $vhostname www.$vhostname;

  ssl_dhparam /usr/local/nginx/conf/ssl/${vhostname}/dhparam.pem;
  ssl_certificate      /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt;
  ssl_certificate_key  /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key;
  include /usr/local/nginx/conf/ssl_include.conf;

  $CFAUTHORIGINPULL_INCLUDES
  $HTTPTWO_MAXFIELDSIZE
  $HTTPTWO_MAXHEADERSIZE
  # mozilla recommended
  ssl_ciphers ${TLSONETHREE_CIPHERS}ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:${CHACHACIPHERS}DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA:ECDHE-ECDSA-DES-CBC3-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!DSS;
  ssl_prefer_server_ciphers   on;
  $SPDY_HEADER

  # before enabling HSTS line below read centminmod.com/nginx_domain_dns_setup.html#hsts
  #add_header Strict-Transport-Security "max-age=31536000; includeSubdomains;";
  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";
  $COMP_HEADER;
  ssl_buffer_size 1369;
  ssl_session_tickets on;
  
  # enable ocsp stapling
  #resolver 8.8.8.8 8.8.4.4 1.1.1.1 1.0.0.1 valid=10m;
  #resolver_timeout 10s;
  #ssl_stapling on;
  #ssl_stapling_verify on;
  #ssl_trusted_certificate /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-trusted.crt;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Wordpress Permalinks example
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args;

  }

  ${PRESTATIC_INCLUDES}
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  ${MULTIPHP_INCLUDES}
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
ESS

else

cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf"<<END
# Centmin Mod Getting Started Guide
# must read http://centminmod.com/getstarted.html

# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
# if unsure use return 302 before using return 301
#server {
#            listen   ${DEDI_IP}80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
  $DEDI_LISTEN
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  #add_header X-Frame-Options SAMEORIGIN;
  add_header X-Xss-Protection "1; mode=block" always;
  add_header X-Content-Type-Options "nosniff" always;
  #add_header Referrer-Policy "strict-origin-when-cross-origin";

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log $NGX_LOGFORMAT buffer=256k flush=5m;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  include /usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf;
  root /home/nginx/domains/$vhostname/public;
  # uncomment cloudflare.conf include if using cloudflare for
  # server and/or vhost site
  #include /usr/local/nginx/conf/cloudflare.conf;
  include /usr/local/nginx/conf/503include-main.conf;

  location / {
  include /usr/local/nginx/conf/503include-only.conf;

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Wordpress Permalinks example
  #try_files \$uri \$uri/ /index.php?q=\$uri&\$args;

  }

  ${PRESTATIC_INCLUDES}
  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  ${MULTIPHP_INCLUDES}
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
  include /usr/local/nginx/conf/vts_server.conf;
}
END

fi

# enable / uncomment try_files line
if [[ "$ENABLE_TRYFILES" = [yY] ]]; then
  if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
    sed -i 's|#try_files|try_files|'  "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
  fi
  if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.conf" ]; then
    sed -i 's|#try_files|try_files|'  "/usr/local/nginx/conf/conf.d/${vhostname}.conf"
  fi
fi

echo 
cecho "-------------------------------------------------------------" $boldyellow
if [ -f "${SCRIPT_DIR}/tools/autoprotect.sh" ]; then
  "${SCRIPT_DIR}/tools/autoprotect.sh"
fi

cmservice nginx reload

if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
  cmservice pure-ftpd restart
fi

if [[ "$LETSENCRYPT_DETECT" = [yY] ]]; then
  if [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'le' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname""
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname"
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'led' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" d"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" d
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'lelive' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" live"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" live
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  elif [ -f "${SCRIPT_DIR}/addons/acmetool.sh" ] && [[ "$vhostssl" = 'lelived' ]]; then
    echo
    cecho "-------------------------------------------------------------" $boldyellow
    echo "ok: ${SCRIPT_DIR}/addons/acmetool.sh"
    chmod +x "${SCRIPT_DIR}/addons/acmetool.sh"
    echo ""${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" lived"
    "${SCRIPT_DIR}/addons/acmetool.sh" issue "$vhostname" lived
    cecho "-------------------------------------------------------------" $boldyellow
    echo
  fi
fi

echo 
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
cecho "-------------------------------------------------------------" $boldyellow
echo "FTP hostname : $CNIP"
echo "FTP port : 21"
echo "FTP mode : FTP (explicit SSL)"
echo "FTP Passive (PASV) : ensure is checked/enabled"
echo "FTP username created for $vhostname : $ftpuser"
echo "FTP password created for $vhostname : $ftppass"
fi
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname created successfully" $boldwhite
echo
cecho "domain: http://$vhostname" $boldyellow
cecho "vhost conf file for $vhostname created: /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
if [[ "$NGINX_VHOSTSSL" = [yY] && "$vhostssl" = [yY] ]] || [[ "$NGINX_VHOSTSSL" = [yY] && "$vhostssl" = 'le' ]] || [[ "$NGINX_VHOSTSSL" = [yY] && "$vhostssl" = 'led' ]] || [[ "$NGINX_VHOSTSSL" = [yY] && "$vhostssl" = 'lelive' ]] || [[ "$NGINX_VHOSTSSL" = [yY] && "$vhostssl" = 'lelived' ]]; then
  echo
  cecho "vhost ssl for $vhostname created successfully" $boldwhite
  echo
  cecho "domain: https://$vhostname" $boldyellow
  cecho "vhost ssl conf file for $vhostname created: /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" $boldwhite
  cecho "/usr/local/nginx/conf/ssl_include.conf created" $boldwhite
  cecho "Self-signed SSL Certificate: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" $boldyellow
  cecho "SSL Private Key: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key" $boldyellow
  cecho "SSL CSR File: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr" $boldyellow
  cecho "Backup SSL Private Key: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-backup.key" $boldyellow
  cecho "Backup SSL CSR File: /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}-backup.csr" $boldyellow    
fi
echo
cecho "upload files to /home/nginx/domains/$vhostname/public" $boldwhite
cecho "vhost log files directory is /home/nginx/domains/$vhostname/log" $boldwhite

echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
echo
cecho "-------------------------------------------------------------" $boldyellow
cecho "Current vhost ssl files listing at: /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
echo
ls -Alhrt /usr/local/nginx/conf/ssl/${vhostname} | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'
fi

echo
{
cecho "-------------------------------------------------------------" $boldyellow
cecho "Commands to remove ${vhostname}" $boldwhite
echo
if [[ "$PUREFTPD_DISABLED" = [nN] ]]; then
cecho "pure-pw userdel $ftpuser" $boldwhite
fi
cecho " rm -rf /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
# if [[ "$vhostssl" = [yY] ]] || [[ "$vhostssl" = 'le' ]] || [[ "$vhostssl" = 'led' ]] || [[ "$vhostssl" = 'lelive' ]] || [[ "$vhostssl" = 'lelived' ]]; then
cecho " rm -rf /usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" $boldwhite
# fi
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.crt" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.key" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}/${vhostname}.csr" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/ssl/${vhostname}" $boldwhite
cecho " rm -rf /home/nginx/domains/$vhostname" $boldwhite
cecho " rm -rf /root/.acme.sh/$vhostname" $boldwhite
cecho " rm -rf /root/.acme.sh/${vhostname}_ecc" $boldwhite
cecho " rm -rf /usr/local/nginx/conf/pre-staticfiles-local-${vhostname}.conf" $boldwhite
cecho " service nginx restart" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname setup successfully" $boldwhite
cecho "$vhostname setup info log saved at: " $boldwhite
cecho ""${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost.log"" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""
} | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nginx_addvhost-remove-cmds-${vhostname}.log"

  # control variables after vhost creation
  # whether cloudflare.conf include file is uncommented (enabled) or commented out (disabled)
  if [[ "$VHOSTCTRL_CLOUDFLAREINC" = [yY] ]]; then
    if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
      sed -i "s|^  #include \/usr\/local\/nginx\/conf\/cloudflare.conf;|  include \/usr\/local\/nginx\/conf\/cloudflare.conf;|g" "/usr/local/nginx/conf/conf.d/$vhostname.conf"
    fi
    if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
      sed -i "s|^  #include \/usr\/local\/nginx\/conf\/cloudflare.conf;|  include \/usr\/local\/nginx\/conf\/cloudflare.conf;|g" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
    fi
  fi
  # whether autoprotect-$vhostname.conf include file is uncommented (enabled) or commented out (disabled)
  if [[ "$VHOSTCTRL_AUTOPROTECTINC" = [nN] ]]; then
    if [ -f "/usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf" ]; then
      if [ -f "/usr/local/nginx/conf/conf.d/$vhostname.conf" ]; then
        sed -i "s|^  include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|  #include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|g" "/usr/local/nginx/conf/autoprotect/$vhostname/autoprotect-$vhostname.conf"
      fi
      if [ -f "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf" ]; then
        sed -i "s|^  include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|  #include \/usr\/local\/nginx\/conf\/autoprotect\/$vhostname\/autoprotect-$vhostname.conf;|g" "/usr/local/nginx/conf/conf.d/${vhostname}.ssl.conf"
      fi
    fi
  fi

else

echo ""
cecho "-------------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "-------------------------------------------------------------" $boldyellow
echo ""

fi


}