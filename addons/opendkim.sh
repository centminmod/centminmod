#!/bin/bash
###################################################################
# opendkim install and configuration for centminmod.com LEMP stack
# https://community.centminmod.com/posts/29878/
###################################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

###################################################################
# functions

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

opendkimsetup() {
if [[ "$(rpm -qa opendkim | grep opendkim >/dev/null 2>&1; echo $?)" != '0' ]]; then
	yum -y install opendkim
	cp /etc/opendkim.conf{,.orig}
fi

if [ -f /etc/opendkim.conf ]; then

if [[ -z "$(grep 'AutoRestart' /etc/opendkim.conf)" ]]; then
echo "AutoRestart             Yes" >> /etc/opendkim.conf
echo "AutoRestartRate         10/1h" >> /etc/opendkim.conf
echo "SignatureAlgorithm      rsa-sha256" >> /etc/opendkim.conf
echo "TemporaryDirectory      /var/tmp" >> /etc/opendkim.conf
sed -i "s|^Mode.*|Mode sv|" /etc/opendkim.conf
sed -i "s|^Canonicalization.*|Canonicalization        relaxed/simple|" /etc/opendkim.conf
sed -i "s|^# ExternalIgnoreList|ExternalIgnoreList|" /etc/opendkim.conf
sed -i "s|^# InternalHosts|InternalHosts|" /etc/opendkim.conf
sed -i 's|^# KeyTable|KeyTable|' /etc/opendkim.conf
sed -i "s|^# SigningTable|SigningTable|" /etc/opendkim.conf
sed -i "s|Umask.*|Umask 022|" /etc/opendkim.conf
fi

if [ ! -f "/root/centminlogs/dkim_postfix_after.txt" ]; then
postconf -d smtpd_milters non_smtpd_milters milter_default_action milter_protocol | tee "${CENTMINLOGDIR}/dkim_postfix_before_${DT}.txt"
postconf -e "smtpd_milters           = inet:127.0.0.1:8891"
postconf -e 'non_smtpd_milters       = $smtpd_milters'
postconf -e "milter_default_action   = accept"
postconf -e "milter_protocol         = 2"
postconf -n smtpd_milters non_smtpd_milters milter_default_action milter_protocol | tee "${CENTMINLOGDIR}/dkim_postfix_after.txt"
fi

# DKIM for main hostname
if [ ! -d "/etc/opendkim/keys/$(hostname)" ]; then
vhostname=$(hostname)
mkdir -p "/etc/opendkim/keys/$vhostname"
opendkim-genkey -D "/etc/opendkim/keys/$vhostname/" -d "$vhostname" -s default
chown -R opendkim: "/etc/opendkim/keys/$vhostname"
mv "/etc/opendkim/keys/$vhostname/default.private" "/etc/opendkim/keys/$vhostname/default"
if [[ -z "$(grep "$vhostname" /etc/opendkim/KeyTable)" ]]; then
	echo "default._domainkey.$vhostname $vhostname:default:/etc/opendkim/keys/$vhostname/default" >> /etc/opendkim/KeyTable
fi
if [[ -z "$(grep "$(hostname)" /etc/opendkim/SigningTable)" ]]; then
	echo "*@$vhostname default._domainkey.$vhostname" >> /etc/opendkim/SigningTable
fi
if [[ -z "$(grep "$(hostname)" /etc/opendkim/TrustedHosts)" ]]; then
	echo "$(hostname)" >> /etc/opendkim/TrustedHosts
fi
echo "---------------------------------------------------------------------------" | tee "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$(hostname) DKIM DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
cat "/etc/opendkim/keys/$vhostname/default.txt" | tr '\n' ' ' | sed -e "s| \"        \"|\" \"|" -e "s|( \"|\"|" -e "s| )  ; ----- DKIM key default for $(hostname)||" -e "s|default._domainkey|default._domainkey.$(hostname)|" -e "s|     IN      TXT   | IN TXT|" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo -e "\n------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$(hostname) SPF DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$(hostname). 14400 IN TXT \"v=spf1 a mx ~all\"" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "dig +short default._domainkey.$vhostname TXT" >> "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "DKIM & SPF TXT details saved at $CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------"
fi

# DKIM for vhost site domain names
if [[ ! -z "$vhostname" ]]; then
if [[ ! -d "/etc/opendkim/keys/$vhostname" || ! -z "$vhostname" ]]; then
echo
mkdir -p "/etc/opendkim/keys/$vhostname"
opendkim-genkey -D "/etc/opendkim/keys/$vhostname/" -d "$vhostname" -s default
chown -R opendkim: "/etc/opendkim/keys/$vhostname"
mv "/etc/opendkim/keys/$vhostname/default.private" "/etc/opendkim/keys/$vhostname/default"
if [[ -z "$(grep "$vhostname" /etc/opendkim/KeyTable)" ]]; then
	echo "default._domainkey.$vhostname $vhostname:default:/etc/opendkim/keys/$vhostname/default" >> /etc/opendkim/KeyTable
fi
if [[ -z "$(grep "$vhostname" /etc/opendkim/SigningTable)" ]]; then
	echo "*@$vhostname default._domainkey.$vhostname" >> /etc/opendkim/SigningTable
fi
if [[ -z "$(grep "$vhostname" /etc/opendkim/TrustedHosts)" ]]; then
	echo "$vhostname" >> /etc/opendkim/TrustedHosts
fi
echo "---------------------------------------------------------------------------" | tee "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$vhostname DKIM DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
cat "/etc/opendkim/keys/$vhostname/default.txt" | tr '\n' ' ' | sed -e "s| \"        \"|\" \"|" -e "s|( \"|\"|" -e "s| )  ; ----- DKIM key default for $vhostname||" -e "s|default._domainkey|default._domainkey.$vhostname|" -e "s|     IN      TXT   | IN TXT|" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo -e "\n------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$vhostname SPF DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "$vhostname. 14400 IN TXT \"v=spf1 a mx ~all\"" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "dig +short default._domainkey.$vhostname TXT" >> "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "DKIM & SPF TXT details saved at $CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
echo "---------------------------------------------------------------------------"
echo
else
	echo "---------------------------------------------------------------------------"
	echo "! Error: domain name not specified on cmd line:"
	echo "   Please use the format below: "
	echo "   $0 domain.com"
	echo "---------------------------------------------------------------------------"
fi
fi

if [[ "$(rpm -qa opendkim | grep opendkim >/dev/null 2>&1; echo $?)" = '0' ]]; then
hash -r
service opendkim restart >/dev/null 2>&1
chkconfig opendkim on >/dev/null 2>&1
fi
service postfix restart >/dev/null 2>&1

fi # if /etc/opendkim.conf exists
}
###########################################################################

starttime=$(date +%s.%N)
{
if [[ "$1" = 'clean' ]]; then
	CLEANONLY=1
	rm -rf "/etc/opendkim/keys/$(hostname)"
	if [ -f /etc/opendkim/KeyTable ]; then
		sed -in "/$(hostname)/d" /etc/opendkim/KeyTable
	fi
	if [ -f /etc/opendkim/SigningTable ]; then
		sed -in "/$(hostname)/d" /etc/opendkim/SigningTable
	fi
fi
if [[ "$1" != 'clean' && "$CLEANONLY" != '1' ]]; then
	vhostname=$1
fi
opendkimsetup
} 2>&1 | tee "${CENTMINLOGDIR}/opendkim_${DT}.log"

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/opendkim_${DT}.log"
echo "Opendkim Setup Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/opendkim_${DT}.log"