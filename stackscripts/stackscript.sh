#!/bin/bash
######################################################
# centos 7 linode stackscript for centmin mod install
# written by George Liu (eva2000) centminmod.com
######################################################
# stackscript installer for latest Centmin Mod LEMP 
# beta + redis installation for both remi server
######################################################
# variables
#############
#DT=$(date +"%d%m%y-%H%M%S")

#<UDF name="hostname" label="Enter main hostname for the new Linode server.">
# HOSTNAME=
#
#<UDF name="fqdn" label="Enter Server's Fully Qualified Domain Name (same as main hostname)">
# FQDN=
# 
#<UDF name="mainemail" label="Enter primary email address to associate with server">
# MAINEMAIL=
# 
#<UDF name="secondemail" label="Enter secondary backup email address to associate with server">
# SECONDEMAIL=
# 
#<UDF name="loginalert" Label="Enable sshd login email alerts (community.centminmod.com/posts/40191/) ?" oneOf="yes,no" default="no" />
# LOGINALERT=
# 
#<UDF name="loginalertemail" Label="Enter sshd login alert email address" default="none" />
# LOGINALERTEMAIL=
# 
#<UDF name="letsencrypt" Label="Enable Letsencrypt SSL Integration Support (centminmod.com/acmetool) ?" oneOf="yes,no" default="no" />
# LETSENCRYPT=
# 
#<UDF name="auditd" Label="Enable Auditd Support (community.centminmod.com/posts/37733/) ?" oneOf="yes,no" default="no" />
# AUDITD=
# 
#<UDF name="csfblocklist" Label="Enable CSF Firewall Advance Blocklist Support (community.centminmod.com/posts/50058/) ?" oneOf="yes,no" default="no" />
# CSFBLOCKLIST=
# 
#<UDF name="redis" Label="Install & Configure Redis Server from REMI YUM repo ?" oneOf="yes,no" default="yes" />
# REDIS=
# 
#<UDF name="pureftp" Label="Stop & Disable Pure-FTPD Server ?" oneOf="yes,no" default="no" />
# PUREFTP=
#  
#<UDF name="targetnative" Label="Build Nginx & PHP-FPM with march native ?" oneOf="yes,no" default="no" />
# TARGETNATIVE=
# 
#<UDF name="compiler" Label="Build Nginx with GCC or Clang Compiler ?" oneOf="clang,gcc" default="gcc" />
# COMPILER=
# 
#<UDF name="openssl" Label="Build Nginx against LibreSSL 2.8+ or OpenSSL 1.1.1+ ?" oneOf="libressl,openssl" default="openssl" />
# OPENSSL=
# 
#<UDF name="zstdlogrotate" Label="Enable zstd compression for Nginx & PHP-FPM Log Rotation (https://community.centminmod.com/threads/16374/) ?" oneOf="yes,no" default="no" />
# ZSTDLOGROTATE=
# 
#<UDF name="hpack" Label="Enable Cloudflare HTTP/2 HPACK Full Encoding Patch (community.centminmod.com/posts/51082/) ?" oneOf="yes,no" default="no" />
# HPACK=
# 
#<UDF name="cloudflarezlib" Label="Enable Nginx install with Cloudflare Zlib Performance Fork (https://github.com/cloudflare/zlib) ?" oneOf="yes,no" default="yes" />
# CLOUDFLAREZLIB=
# 
#<UDF name="brotli" Label="Enable ngx_brotli nginx module (community.centminmod.com/posts/45818/) ?" oneOf="yes,no" default="no" />
# BROTLI=
# 
#<UDF name="pagespeed" Label="Build Nginx with ngx_pagespeed module enabled ?" oneOf="yes,no" default="no" />
# PAGESPEED=
# 
#<UDF name="lua" Label="Enable OpenResty Lua Nginx mdoule support ? (auto fallback to OpenSSL 1.0.2+ for Lua Nginx compatibility)" oneOf="yes,no" default="no" />
# LUA=
# 
#<UDF name="php" Label="Install Latest PHP 5.6 or 7.0 or 7.1 or 7.2 or 7.3 Version ?" oneOf="5.6,7.0,7.1,7.2,7.3" default="5.6" />
# PHP=
# 
#<UDF name="docker" Label="Install Docker ?" oneOf="yes,no" default="no" />
# DOCKER=
# 
#<UDF name="pushover" Label="Pushover.net Mobile Email Notification on StackScript completion ?" oneOf="yes,no" default="no" />
# PUSHOVER=
# 
#<UDF name="pushoveremail" Label="Enter Pushover.net Email Address" default="none" />
# PUSHOVEREMAIL=
# 
#<UDF name="whitelist" Label="Whitelist custom IP addresses for CSF Firewall i.e. ISP IP address, remote web or mysql servers or VPN IP etc ?" oneOf="yes,no" default="no" />
# WHITELIST=
# 
#<UDF name="csfipa" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPA=
# 
#<UDF name="csfipb" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPB=
# 
#<UDF name="csfipc" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPC=
# 
#<UDF name="csfipd" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPD=
# 
#<UDF name="csfipe" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPE=
# 
#<UDF name="csfipf" label="Enter IP address to add to CSF Firewall Whitelisting." default="none" />
# CSFIPF=
# 
#<UDF name="sshpublickey" Label="Add SSH Public Key for root user (rsa,ecdsa,ed25519)" example="ssh-rsa ..." default="none" />
# SSHPUBLICKEY=
# 
#<UDF name="nginxvhosta" Label="Create Nginx Vhost Domain/Subdomain without http:// i.e. domain.com or sub.domain.com : " default="none" />
# NGINXVHOSTA=
# 
#<UDF name="ftpusernamea" Label="Enter Desired Pure-FTPD Virtual FTP Username : " default="none" />
# FTPUSERNAMEA=
# 
#<UDF name="mysqldbnamea" Label="Create Desired MySQL Database Named: " default="none" />
# MYSQLDBNAMEA=
# 
#<UDF name="mysqlusera" Label="Create Desired MySQL Username Named: " default="none" />
# MYSQLUSERA=
# 
#<UDF name="mysqlpassa" Label="Create Desired MySQL User's Password: " default="none" />
# MYSQLPASSA=


# This sets the variable $IPADDR to the IPv4 address the new Linode receives.
IPADDR=$(hostname -I | cut -f1 -d' ')

# This sets the variable $IPADDR6 to the IPv6 address the new Linode receives.
IPADDR6=$(hostname -I | cut -f2 -d' ')

# YOURIP=""

######################################################
# Redirect output of this script to our logfile
exec &> /root/stackscript.log

# This section sets the hostname.
hostnamectl set-hostname $HOSTNAME

# This section sets the Fully Qualified Domain Name (FQDN) in the hosts file.
echo $IPADDR $FQDN $HOSTNAME >> /etc/hosts
echo $IPADDR6 $FQDN $HOSTNAME >> /etc/hosts

# Centmin Mod 123.09beta01
mkdir -p /etc/centminmod
touch /etc/centminmod/custom_config.inc

# Build Nginx with GCC or Clang compiler
if [[ "$COMPILER" = 'gcc' ]]; then
echo
echo "Set CLANG='n'"
echo "Set DEVTOOLSETSEVEN='y'"
echo "Set NGINX_DEVTOOLSETGCC='y'"
echo "CLANG='n'" >> /etc/centminmod/custom_config.inc
echo "DEVTOOLSETSEVEN='y'" >> /etc/centminmod/custom_config.inc
echo "NGINX_DEVTOOLSETGCC='y'" >> /etc/centminmod/custom_config.inc
echo
fi


# Build Nginx with OpenResty Nginx Lua module support
if [[ "$LUA" = 'yes' ]]; then
echo
echo "Set ORESTY_LUANGINX='y'"
echo "Set NGXDYNAMIC_LUA='y'"
echo "Set NGXDYNAMIC_DEVELKIT='y'"
echo "ORESTY_LUANGINX='y'" >> /etc/centminmod/custom_config.inc
echo "NGXDYNAMIC_LUA='y'" >> /etc/centminmod/custom_config.inc
echo "NGXDYNAMIC_DEVELKIT='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Enable Cloudflare HPACK patch 
# https://community.centminmod.com/posts/51082/
if [[ "$HPACK" = 'yes' ]]; then
echo
echo "Set NGINX_HPACK='y'"
echo "NGINX_HPACK='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Enable zstd compression for Nginx & PHP-FPM log rotation
# https://community.centminmod.com/threads/16374/
if [[ "$ZSTDLOGROTATE" = 'yes' ]]; then
echo
echo "Set ZSTD_LOGROTATE_NGINX='y'"
echo "Set ZSTD_LOGROTATE_PHPFPM='y'"
echo "ZSTD_LOGROTATE_NGINX='y'" >> /etc/centminmod/custom_config.inc
echo "ZSTD_LOGROTATE_PHPFPM='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Enable Cloudflare zlib library install for Nginx server 
# https://community.centminmod.com/threads/13521/
# https://community.centminmod.com/threads/13498/
if [[ "$CLOUDFLAREZLIB" = 'yes' ]]; then
echo
echo "Set CLOUDFLARE_ZLIB='y'"
echo "CLOUDFLARE_ZLIB='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Linode host vps nodes can use different intel based processor
# models and march=native optimises performance for the specific
# cpu model family only. If you migrate linode vps to a different
# host node with different cpu model, you need to recompile nginx
# & php-fpm for the new cpu if you have the usual march=native 
# compile flag. Setting MARCH_TARGETNATIVE='n' disables march=native
# so you do not need to recompile after migrating to different
# linode host node with different cpu model
if [[ "$TARGETNATIVE" = 'no' ]]; then
echo
echo "Disable march=native"
echo "MARCH_TARGETNATIVE='n'" >> /etc/centminmod/custom_config.inc
fi

# Build Nginx with LibreSSL or OpenSSL
if [[ "$OPENSSL" = 'openssl' ]]; then
echo
echo "Set LIBRESSL_SWITCH='n'"
echo "LIBRESSL_SWITCH='n'" >> /etc/centminmod/custom_config.inc
echo
fi

# Enable ngx_brolti nginx module
# https://community.centminmod.com/posts/45818/
if [[ "BROTLI" = 'yes' ]]; then
echo
echo "Set NGXDYNAMIC_BROTLI='y'"
echo "Set NGINX_LIBBROTLI='y'"
echo "NGXDYNAMIC_BROTLI='y'" >> /etc/centminmod/custom_config.inc
echo "NGINX_LIBBROTLI='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Build Nginx with ngx_pagespeed
if [[ "$PAGESPEED" = 'yes' ]]; then
echo
echo "Set NGXDYNAMIC_NGXPAGESPEED='y'"
echo "Set NGINX_PAGESPEED='y'"
echo "NGXDYNAMIC_NGXPAGESPEED='y'" >> /etc/centminmod/custom_config.inc
echo "NGINX_PAGESPEED='y'" >> /etc/centminmod/custom_config.inc
echo
fi

# Build PHP version
if [[ "$PHP" = '7.3' ]]; then
echo
yum -y update
echo
curl -O https://centminmod.com/betainstaller73.sh && chmod 0700 betainstaller73.sh && bash betainstaller73.sh
echo
fi

# Build PHP version
if [[ "$PHP" = '7.2' ]]; then
echo
yum -y update
echo
curl -O https://centminmod.com/betainstaller72.sh && chmod 0700 betainstaller72.sh && bash betainstaller72.sh
echo
fi

# Build PHP version
if [[ "$PHP" = '7.1' ]]; then
echo
yum -y update
echo
curl -O https://centminmod.com/betainstaller71.sh && chmod 0700 betainstaller71.sh && bash betainstaller71.sh
echo
fi

if [[ "$PHP" = '7.0' ]]; then
echo
yum -y update
echo
curl -O https://centminmod.com/betainstaller7.sh && chmod 0700 betainstaller7.sh && bash betainstaller7.sh
echo
fi

if [[ "$PHP" = '5.6' ]]; then
echo
yum -y update
echo
curl -O https://centminmod.com/betainstaller.sh && chmod 0700 betainstaller.sh && bash betainstaller.sh
echo
fi

echo "Primary: $MAINEMAIL"
echo "$MAINEMAIL" > /etc/centminmod/email-primary.ini
echo "setup at /etc/centminmod/email-primary.ini"
echo "Secondary: $SECONDEMAIL"
echo "$SECONDEMAIL" > /etc/centminmod/email-secondary.ini
echo "setup at /etc/centminmod/email-secondary.ini"

# Allow your IP on YOUR.FQDN.COM/nginx-status
#sed -i "s/.*#allow youripaddress;.*/allow ${YOURIP}; \#Your IP/" /usr/local/nginx/conf/conf.d/virtual.conf

# pure-ftpd dhparam
openssl dhparam -out /etc/ssl/private/pure-ftpd-dhparams.pem 2048

# Enable addons/acmetool.sh letsencrypt integration support
# https://centminmod.com/acmetool
if [[ "$LETSENCRYPT" = 'yes' ]]; then
echo
echo "Enable addons/acmetool.sh support"
echo "LETSENCRYPT_DETECT='y'" >> /etc/centminmod/custom_config.inc
fi

# Enable tools/auditd.sh support
# https://community.centminmod.com/posts/37733/
if [[ "$AUDITD" = 'yes' ]]; then
echo
echo "Enable tools/auditd.sh support"
echo "AUDITD_ENABLE='y'" >> /etc/centminmod/custom_config.inc
/usr/local/src/centminmod/tools/auditd.sh setup
fi

# Enable CSF advance blocklist support
# https://community.centminmod.com/posts/50058/
# extending blocklists in /etc/csf/csf.blocklists
if [[ "$CSFBLOCKLIST" = 'yes' ]]; then
echo
echo "Enable CSF Firewall Advance Blocklist support"
/usr/local/src/centminmod/tools/csf-advancetweaks.sh
csf -r; service lfd restart
fi

# Disables Pure-ftpd
if [[ "$PUREFTP" = 'yes' ]]; then
echo
service pure-ftpd stop
chkconfig pure-ftpd off
fi

# Install docker
# https://docs.docker.com/install/linux/docker-ce/centos/
if [[ "$DOCKER" = 'yes' ]]; then
echo  
yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo
yum -y install yum-utils device-mapper-persistent-data lvm2
yum -y install docker-ce
mkdir -p /etc/systemd/system/docker.service.d
touch /etc/systemd/system/docker.service.d/docker.conf
mkdir -p /etc/docker
wget -O /etc/docker/daemon.json https://gist.githubusercontent.com/centminmod/e79bca8d3ef56d4d7272663f755e830d/raw/daemon.json
systemctl daemon-reload
systemctl start docker
systemctl enable docker
echo
systemctl status docker
echo
journalctl -u docker --no-pager
echo
docker info
fi

# Install redis
if [[ "$REDIS" = 'yes' ]]; then
echo
mkdir -p /root/tools
git clone https://github.com/centminmod/centminmod-redis
cd centminmod-redis
./redis-install.sh install
fi

# CSF Whitelisting
# https://centminmod.com/csf_firewall.html
if [[ "$WHITELIST" = 'yes' ]]; then
  echo
  CSFIP_ARRAY="${CSFIPA} ${CSFIPB} ${CSFIPC} ${CSFIPD} ${CSFIPE} ${CSFIPF}"
  for ip in ${CSFIP_ARRAY[@]}; do
    if [[ "$ip" != 'none' ]]; then
      csf -a $ip # stackscript-whitelisted
      echo "$ip" >> /etc/csf/csf.ignore
    fi
  done
fi

# fix ups
wget -O /root/mysqlreport https://centminmod.com/centminmodparts/mysqlreport/mysqlreport

# clean up
yum clean all
find /svr-setup -maxdepth 1 -type d ! -wholename "/svr-setup" -exec rm -rf {} \;
if [[ "$LUA" = 'yes' ]]; then
  sed -i '/OPENSSL_VERSION=/d' /etc/centminmod/custom_config.inc
fi

# ssh public keys
if [[ "$SSHPUBLICKEY" != 'none' ]]; then
echo Setting up ssh public keys
mkdir -p /root/.ssh
echo "$SSHPUBLICKEY" > /root/.ssh/authorized_keys
chmod -R 700 /root/.ssh
chmod 0644 /root/.ssh/authorized_keys
systemctl restart sshd
cat /root/.ssh/authorized_keys
fi

# sshd login alert centmin mod style
# https://community.centminmod.com/posts/40191/
if [[ "$LOGINALERT" = 'yes' ]]; then
  if [[ "$LOGINALERTEMAIL" != 'none' ]]; then
    echo "SSH_ALERTEMAIL=$LOGINALERTEMAIL" >> /root/.bashrc
    echo "SSH_ALERTIP=\$(echo \$SSH_CLIENT | awk '{print \$1}')" >> /root/.bashrc
    echo "SSH_ALERTGEO=\$(curl -sL https://ipinfo.io/\$SSH_ALERTIP/geo | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||')" >> /root/.bashrc
    echo "echo -e \"ALERT: \$(whoami) Shell Access \$(hostname): \$(date)\n\$SSH_ALERTGEO\" | mail -s \"Alert: \$(whoami) Shell Access \$(hostname) from \$SSH_ALERTIP\" \$SSH_ALERTEMAIL" >> /root/.bashrc
  fi
fi

# mysql db, user setup
if [[ "$MYSQLDBNAMEA" != 'none' && "$MYSQLUSERA" != 'none' && "$MYSQLPASSA" != 'none' && -f /usr/local/src/centminmod/addons/mysqladmin_shell.sh ]]; then
  /usr/local/src/centminmod/addons/mysqladmin_shell.sh createuserdb "$MYSQLDBNAMEA" "$MYSQLUSERA" "$MYSQLPASSA"
  DBINFO="DBNAME: "$MYSQLDBNAMEA" 
DBUSER: "$MYSQLUSERA" 
DBPASS: "$MYSQLPASSA""
else
  DBINFO=""
fi

# nginx vhost setup
if [[ "$NGINXVHOSTA" != 'none' && "$FTPUSERNAMEA" != 'none' && -f /usr/bin/nv ]]; then
  echo "/usr/bin/nv -d "$NGINXVHOSTA" -s n -u "$FTPUSERNAMEA""
  /usr/bin/nv -d "$NGINXVHOSTA" -s n -u "$FTPUSERNAMEA"
  echo
  NVHOSTLOG=$(ls /root/centminlogs/ | grep 'nginx_addvhost_nv.log' | tail -1)
  echo
  NVHOSTINFO=$(echo "$NGINXVHOSTA log: /root/centminlogs/$NVHOSTLOG")
  echo
  FTPINFO=$(cat "/root/centminlogs/$NVHOSTLOG" | grep --color=never -A5 'FTP hostname ')
  echo
else
  NVHOSTINFO=""
  FTPINFO=""
fi

# And we are done
echo
if [[ "$PUSHOVER" = 'yes' ]]; then
  if [[ "$PUSHOVEREMAIL" = 'none' ]]; then
    echo -e "$(date)\nStackscript Setup Complete For $HOSTNAME\nstackscript log: /root/stackscript.log\ncentmin mod logs: /root/centminlogs\n$NVHOSTINFO\n$FTPINFO\n$DBINFO"
  else
    DT=$(date)
    echo -e "${DT}\nStackscript Setup Complete For $HOSTNAME\nstackscript log: /root/stackscript.log\ncentmin mod logs: /root/centminlogs\n$NVHOSTINFO\n$FTPINFO\n$DBINFO" | mail -s "$HOSTNAME StackScript Setup Done ${DT}" -r "$PUSHOVEREMAIL" "$PUSHOVEREMAIL"
    echo -e "${DT}\nStackscript Setup Complete For $HOSTNAME\nstackscript log: /root/stackscript.log\ncentmin mod logs: /root/centminlogs\n$NVHOSTINFO\n$FTPINFO\n$DBINFO"
  fi
else
  echo -e "$(date)\nStackscript Setup Complete For $HOSTNAME\nstackscript log: /root/stackscript.log\ncentmin mod logs: /root/centminlogs\n$NVHOSTINFO\n$FTPINFO\n$DBINFO"
fi