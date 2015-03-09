#!/bin/bash
#########################################################
# Install Linux Malware Detect (maldet) + ClamAV AntiVirus scanner
# Official Centmin Mod Addon for centminmod.com LEMP web stack
# written by George Liu vbtechsupport.com
# https://www.rfxn.com/projects/linux-malware-detect/
# http://www.clamav.net/lang/en/
#########################################################
DT=`date +"%d%m%y-%H%M%S"`
TMP_DIR='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'

# enter email address you want alerts sent to
# i.e. your@domain.com
ALERTEMAIL=''

# enter your pushover.net email you want alerts sent to 
# i.e. youruserkey+devicename+p1@api.pushover.net
ALERT_POEMAIL=''
#########################################################
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
#########################################################
if [[ ! -f /etc/redhat-release ]] ; then
	cecho "No CentOS / RHEL system detected" $boldyellow
	cecho "Please only install on CentOS / RHEL systems" $boldyellow
	cecho "aborting install..." $boldyellow
	exit
else
	cecho "CentOS / RHEL system detected" $boldyellow
fi

if [[ ! -f /usr/bin/wget ]] ; then
	yum -y -q install wget
fi

if [[ -z "$ALERTEMAIL" ]]; then
	echo
	cecho "ALERTEMAIL variable detected as empty" $boldyellow
	cecho "edit $0 and set an email address for ALERTEMAIL" $boldyellow
	cecho "Then re-run the script $0" $boldyellow
	exit
fi
#########################################################
# functions

setemailalert() {
	if [[ ! -z "$ALERT_POEMAIL" ]]; then
		sed -i 's/email_alert=0/email_alert=1/g' /usr/local/maldetect/conf.maldet
		sed -i "s/email_addr=\"you@domain.com\"/email_addr=\"${ALERTEMAIL},${ALERT_POEMAIL}\"/g" /usr/local/maldetect/conf.maldet
	else
		sed -i 's/email_alert=0/email_alert=1/g' /usr/local/maldetect/conf.maldet
		sed -i "s/email_addr=\"you@domain.com\"/email_addr=\"${ALERTEMAIL}\"/g" /usr/local/maldetect/conf.maldet
	fi
}

maldetinstall() {
	# install maldet
	cecho "Installing maldet..."  $boldyellow
	cd $TMP_DIR

	        cecho "Download maldetect-current.tar.gz ..." $boldyellow
    if [ -s maldetect-current.tar.gz ]; then
        cecho "maldetect-current.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -cnv http://www.rfxn.com/downloads/maldetect-current.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: maldetect-current.tar.gz download failed." $boldgreen
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf maldetect-current.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: maldetect-current.tar.gz extraction failed." $boldgreen
	exit #$ERROR
else 
         cecho "maldetect-current.tar.gz valid file." $boldyellow
echo ""
	fi

	cd maldetect-*
	./install.sh

	# set email alerts
	setemailalert
	
	# enable auto quarantine of malware hits
	sed -i 's/quar_hits=0/quar_hits=1/g' /usr/local/maldetect/conf.maldet

	# append Centmin Mod specific scan paths into cron.daily/malet
	sed -i '/\/home?\/?\/public_html/ a\                \/usr\/local\/maldetect\/maldet -b -r \/home\/nginx\/domains\/?\/public 2 >> \/dev\/null 2>&1' /etc/cron.daily/maldet
	sed -i '/\/home?\/?\/public_html/ a\                \/usr\/local\/maldetect\/maldet -b -r \/var\/www\/html 2 >> \/dev\/null 2>&1' /etc/cron.daily/maldet
	sed -i '/\/home?\/?\/public_html/ a\                \/usr\/local\/maldetect\/maldet -b -r \/usr\/local\/nginx\/html 2 >> \/dev\/null 2>&1' /etc/cron.daily/maldet
 
 	# extend scan to other system directories where malware and viruses can be placed
	echo "" >> /etc/cron.daily/maldet
	echo "# extend maldet scans to other areas" >> /etc/cron.daily/maldet
	echo "/usr/local/maldetect/maldet -b -r /boot 2 >> /dev/null 2>&1" >> /etc/cron.daily/maldet
	echo "/usr/local/maldetect/maldet -b -r /etc 2 >> /dev/null 2>&1" >> /etc/cron.daily/maldet
	echo "/usr/local/maldetect/maldet -b -r /usr 2 >> /dev/null 2>&1" >> /etc/cron.daily/maldet

}

clamavinstall() {
	# install clamav and clamd
	echo
	cecho "Installing clamav..."  $boldyellow
	yum clean all -q
	yum makecache fast -q
	yum -y install clamav clamd --disablerepo=epel
	if [[ -z "$(grep clam /etc/yum.repos.d/epel.repo)" ]]; then
		sed -i 's/exclude=varnish/exclude=varnish clamd clamav clamav-db/' /etc/yum.repos.d/epel.repo
	fi
	/etc/init.d/clamd start
	chkconfig clamd on
	time freshclam
}
#########################################################
starttime=$(date +%s.%N)
{
maldetinstall
clamavinstall

echo
cecho "maldet + clamav installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log
echo "Total maldet + clamav Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log