setupmotd() {

ENABLEMOTD='y'

MOTDCHECK=$(cat /etc/motd)
CSFWCHECK=$(grep 'CSF' /etc/motd)
IPTABLEWCHECK=$(grep 'iptables -F' /etc/motd)
CMWCHECK=$(grep 'centminmod.com' /etc/motd)

	if [[ "$ENABLEMOTD" = [yY] ]]; then

if [[ -z "$MOTDCHECK" ]]; then

#echo "Backup /etc/motd"
#\cp -af /etc/motd /etc/motd-backup_$DT

cat > "/etc/motd" <<EOF
################################################################
# ! This server maybe running CSF Firewall !  
#   DO NOT run the below command or you 
#   will lock yourself out of the server: 
#
#   iptables -F 
#
################################################################
# This message was setup by Centmin Mod 
# * Getting Started Guide - http://centminmod.com/getstarted.html
# * Latest Centmin Mod version - http://centminmod.com       
# * Centmin Mod FAQ - http://centminmod.com/faq.html
# * Change Log - http://centminmod.com/changelog.html
# * Google+ Page latest news http://centminmod.com/gpage
# * Google+ Community Forum http://centminmod.com/gcom
# * Centmin Mod Twitter https://twitter.com/centminmod
###############################################################
# Cheap VPS Hosting at Digitalocean 
# https://www.digitalocean.com/?refcode=c1cb367108e8
###############################################################
# recommend to keep the above lines and remove and 
# add your own messages below this point
###############################################################
# To edit this message,
# 1). edit /etc/motd
# 2). edit inc/motd.inc and set to no ENABLEMOTD='n' 
###############################################################

EOF

elif  [[ ! -z "$MOTDCHECK" && -z "$CSFWCHECK" || -z "$IPTABLEWCHECK" || -z "$CMWCHECK" ]]; then

#echo "Backup /etc/motd"
\cp -af /etc/motd /etc/motd-backup_$DT

cat >> "/etc/motd" <<EOF
################################################################
# ! This server maybe running CSF Firewall !  
#   DO NOT run the below command or you 
#   will lock yourself out of the server: 
#
#   iptables -F 
#
################################################################
# This message was setup by Centmin Mod 
# * Getting Started Guide - http://centminmod.com/getstarted.html
# * Latest Centmin Mod version - http://centminmod.com       
# * Centmin Mod FAQ - http://centminmod.com/faq.html
# * Change Log - http://centminmod.com/changelog.html
# * Google+ Page latest news http://centminmod.com/gpage
# * Google+ Community Forum http://centminmod.com/gcom
# * Centmin Mod Twitter https://twitter.com/centminmod
###############################################################
# Cheap VPS Hosting at Digitalocean 
# https://www.digitalocean.com/?refcode=c1cb367108e8
###############################################################
# recommend to keep the above lines and remove and 
# add your own messages below this point
###############################################################
# To edit this message,
# 1). edit /etc/motd
# 2). edit inc/motd.inc and set to no ENABLEMOTD='n' 
###############################################################

EOF

fi
	fi #ENABLEMOTD

}