function funct_nsdsetup {

echo ""
cecho "---------------------------------------------" $boldyellow
echo ""
cecho "New to NSD DNS setup ? Be sure to read NSD setup guide:" $boldgreen
cecho "http://centminmod.com/nginx_domain_dns_setup.html#dns" $boldgreen
echo ""

read -ep "Enter domain name you want to add to NSD (without www. prefix): " vhostdomainname

echo ""

read -ep "Enter IP address you want to assign to domain name (your A record): " vhostdomainip

cecho "---------------------------------------------------------" $boldyellow
cecho "You entered domain name: $vhostdomainname" $boldgreen
cecho "You entered domain IP address (A record): $vhostdomainip" $boldgreen
cecho "---------------------------------------------------------" $boldyellow

echo ""
read -ep "Are the domain name and IP address (A record) entered correctly ? [y/n]: " domaininfocorrect

if [[ "$domaininfocorrect" = [yY] ]]; then

vhostdomainname="$vhostdomainname"
vhostdomainip="$vhostdomainip"

echo ""
cecho "---------------------------" $boldyellow
cecho "Nameserver ns1/ns2 setup:" $boldgreen
cecho "---------------------------" $boldyellow

echo ""
cecho "Note #1:" $boldyellow
echo "nameserver ns1/ns2 IP addresses must already exist and be assigned to this"
echo "server by your web host. If unsure, ask your web host the exact IP addresses"
echo "assigned to your server."
echo ""

echo ""
cecho "Note #2:" $boldyellow
echo "For vanity or custom name servers using your own domain name, ensure you"
echo "have created them first with your domain registrar. You can see tutorial guides"
echo "for Namecheap and Godaddy domain registrars for creating domain names'"
echo "private name servers on web site Part #1 at "
cecho "http://centminmod.com/nginx_domain_dns_setup.html#dns" $boldyellow
echo ""

echo ""
read -ep "Want to abort NSD setup to check with web host and/or domain registrar first ? [y/n]: " abortnsdsetup

if [[ "$abortnsdsetup" = [yY] ]]; then
	echo ""
	cecho "Aborting NSD domain DNS setup..." $boldyellow
	echo ""
	exit
fi

read -ep "* Enter IP address for ns1 nameserver: " nameserverip1

echo ""

read -ep "* Enter IP address for ns2 nameserver: " nameserverip2

cecho "--------------------------------------------------------------" $boldyellow
cecho "You entered ns1.$vhostdomainname IP address: $nameserverip1" $boldgreen
cecho "You entered ns2.$vhostdomainname IP address: $nameserverip2" $boldgreen
cecho "--------------------------------------------------------------" $boldyellow

echo ""
read -ep "Are the ns1/ns2 name server IP address entered correct ? [y/n]: " nscorrect

if [[ "$nscorrect" = [yY] ]]; then

echo ""

##########################################
# check if zone already exists 

NSDCONFDOMAINCHECK=`grep -E '("${vhostdomainname}"|"master/${vhostdomainname}.zone")' /etc/nsd/nsd.conf`

cecho "---------------------------------------------" $boldyellow
echo "checking to see if entry for ${vhostdomainname} already exists in /etc/nsd/nsd.conf"
cecho "---------------------------------------------" $boldyellow
echo ""

if [[ -z $NSDCONFDOMAINCHECK ]]; then

cecho "---------------------------------------------" $boldyellow
echo "no entry for ${vhostdomainname} found in /etc/nsd/nsd.conf"
echo "creating entry for ${vhostdomainname} ..."
cecho "---------------------------------------------" $boldyellow
echo ""

# Setting up nsd.conf entry
cat >> "/etc/nsd/nsd.conf" <<END

zone:
        name: "$vhostdomainname"
        zonefile: "master/$vhostdomainname.zone"
END

cat /etc/nsd/nsd.conf

else

cecho "---------------------------------------------" $boldyellow
echo "entry for ${vhostdomainname} already exists in /etc/nsd/nsd.conf"
echo "$NSDCONFDOMAINCHECK"
cecho "---------------------------------------------" $boldyellow
echo ""

fi

if [ ! -f /etc/nsd/master/${vhostdomainname}.zone ]; then

touch /etc/nsd/master/${vhostdomainname}.zone

cat > "/etc/nsd/master/${vhostdomainname}.zone" <<END
\$TTL 14400
@       IN      SOA     ns1.$vhostdomainname.      hostmaster.$vhostdomainname. (
                                                2010091500
                                                14400
                                                3600
                                                1209600
                                                86400 )

; Nameservers
$vhostdomainname.       14400   IN      NS      ns1.$vhostdomainname.
$vhostdomainname.       14400   IN      NS      ns2.$vhostdomainname.

; A Records
$vhostdomainname.       14400   IN      A       $vhostdomainip
ftp                     14400   IN      A       $vhostdomainip
localhost       14400   IN      A       127.0.0.1
mail            14400   IN      A       $vhostdomainip
ns1                     14400   IN      A       $nameserverip1
ns2                     14400   IN      A       $nameserverip2
pop                     14400   IN      A       $vhostdomainip
smtp            14400   IN      A       $vhostdomainip
www                     14400   IN      A       $vhostdomainip

; MX Record
$vhostdomainname.       14400   IN      MX      10 mail

; TXT Record (for SPF)
$vhostdomainname.       14400   IN      TXT     "v=spf1 a mx ip4:$vhostdomainip ~all"
END

cecho "---------------------------------------------" $boldyellow
service nsd restart

echo 
cecho "---------------------------------------------" $boldyellow
cecho "Creating zone file at /etc/nsd/master/${vhostdomainname}.zone" $boldwhite
cecho "---------------------------------------------" $boldyellow

cat /etc/nsd/master/${vhostdomainname}.zone

echo
cecho "---------------------------------------------" $boldyellow
cecho "Current zone files listing at: /etc/nsd/master/" $boldwhite

ls -Alhrt /etc/nsd/master/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

cecho "---------------------------------------------" $boldyellow

echo 
cecho "---------------------------------------------" $boldyellow
cecho "NSD entry for $vhostdomainname created successfully in /etc/nsd/nsd.conf" $boldwhite
cecho "NSD zone created at /etc/nsd/master/${vhostdomainname}.zone" $boldwhite
echo 
cecho "---------------------------------------------" $boldyellow
echo "Remember to check your domain name's DNS is properly configured"
echo "at both your domain registrar & web server end (NSD) by running" 
echo "domain name through these 3 dns test sites"
echo "* http://www.dnsvalidation.com/"
echo "* http://www.intodns.com/"
echo "* http://dnscheck.pingdom.com/"
cecho "---------------------------------------------" $boldyellow

exit

else

echo ""
cecho "---------------------------------------------" $boldyellow
#cecho "NSD entry for $vhostdomainname already exists in /etc/nsd/nsd.conf" $boldwhite
cecho "/etc/nsd/master/${vhostdomainname}.zone already exists" $boldwhite
cecho "---------------------------------------------" $boldyellow
echo ""
echo ""

exit

fi # check if zone already exists 

else

echo "-------------------------------"
echo "Aborting NSD setup... try again"
echo "-------------------------------"
exit

fi # check if nameserver ip addresses correct

else

echo "-------------------------------"
echo "Aborting NSD setup... try again"
echo "-------------------------------"
exit


fi # check if domain name and ip address are correct

}
