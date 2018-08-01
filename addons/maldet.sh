#!/bin/bash
#########################################################
# Install Linux Malware Detect (maldet) + ClamAV AntiVirus scanner
# Official Centmin Mod Addon for centminmod.com LEMP web stack
# written by George Liu vbtechsupport.com
# https://www.rfxn.com/projects/linux-malware-detect/
# http://www.clamav.net/lang/en/
#########################################################
DT=$(date +"%d%m%y-%H%M%S")
TMP_DIR='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

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
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CPUS=$(cat "/proc/cpuinfo" | grep "processor"|wc -l)

if [ ! -d "$CENTMINLOGDIR" ]; then
	mkdir -p "$CENTMINLOGDIR"
fi

if [[ "$CPUS" = '1' ]]; then
    MAXTHREADS=1
else
    MAXTHREADS=$(echo $CPUS/2 | bc)
fi

if [[ ! -f /etc/redhat-release ]] ; then
	cecho "No CentOS / RHEL system detected" $boldyellow
	cecho "Please only install on CentOS / RHEL systems" $boldyellow
	cecho "aborting install..." $boldyellow
	exit
else
	cecho "CentOS / RHEL system detected" $boldyellow
fi

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

# if [[ ! "$(grep -w 'enabled = 1' /etc/yum.repos.d/rpmforge.repo)" ]]; then
#   echo "rpmforge repo is disabled"
#   echo "aborting maldet.sh install due to clamav rpmforge requirements"
#   echo "check forums for any updates to this issue at"
#   echo "https://community.centminmod.com/forums/add-ons.10/"
#   exit
# fi

if [[ ! -f /usr/bin/wget ]] ; then
	yum -y -q install wget
fi

if [[ -z "$ALERTEMAIL" ]]; then
	echo
	cecho "ALERTEMAIL variable detected as empty" $boldyellow
	cecho "add to persistent config file created at" $boldyellow
  cecho "/etc/centminmod/custom_config.inc and set an" $boldyellow
  cecho "email address for variable: " $boldyellow
  echo
  cecho " ALERTEMAIL='youremail@domain.com'" $boldyellow
  echo
	cecho "Then re-run the script $0" $boldyellow
	exit
fi
#########################################################
# functions

setemailalert() {
	if [[ ! -z "$ALERT_POEMAIL" ]]; then
		sed -i 's/email_alert=0/email_alert=1/g' /usr/local/maldetect/conf.maldet
		sed -i 's/email_alert=\"0\"/email_alert=\"1\"/g' /usr/local/maldetect/conf.maldet
		sed -i "s/email_addr=\"you@domain.com\"/email_addr=\"${ALERTEMAIL},${ALERT_POEMAIL}\"/g" /usr/local/maldetect/conf.maldet
	else
		sed -i 's/email_alert=0/email_alert=1/g' /usr/local/maldetect/conf.maldet
		sed -i 's/email_alert=\"0\"/email_alert=\"1\"/g' /usr/local/maldetect/conf.maldet
		sed -i "s/email_addr=\"you@domain.com\"/email_addr=\"${ALERTEMAIL}\"/g" /usr/local/maldetect/conf.maldet
	fi
}

maldetinstall() {
if [ ! -f /usr/local/sbin/maldet ]; then
	# install maldet
	cecho "Installing maldet..."  $boldyellow
	cd $TMP_DIR

	        cecho "Download maldetect-current.tar.gz ..." $boldyellow
    if [ -s maldetect-current.tar.gz ]; then
        cecho "maldetect-current.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -${ipv_forceopt}cnv https://www.rfxn.com/downloads/maldetect-current.tar.gz --tries=3 
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
	# sed -i 's/quar_hits=0/quar_hits=1/g' /usr/local/maldetect/conf.maldet
	# sed -i 's/quar_hits=\"0\"/quar_hits=\"1\"/g' /usr/local/maldetect/conf.maldet
	# sed -i 's/quarantine_hits=0/quarantine_hits=1/g' /usr/local/maldetect/conf.maldet
	# sed -i 's/quarantine_hits=\"0\"/quarantine_hits=\"1\"/g' /usr/local/maldetect/conf.maldet	

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
fi
}

clamav_removeold() {
  # remove old rpmforge version of clamav
  if [[ -f /usr/bin/clamscan && "$(/usr/bin/clamscan -V | grep '0.98')" ]]; then
    yum remove clamav clamav-db clamd --disablerepo=epel --enablerepo=rpmforge --disableexclude=rpmforge --disableplugin=priorities
    OLDCLAM=y
  elif [[ -f /usr/bin/clamscan && "$(/usr/bin/clamscan -V | grep '0.99')" ]]; then
    OLDCLAM=n
  else
    OLDCLAM=none
  fi
}

clamavinstall() {
  if [[ "$OLDCLAM" = 'none' || "$OLDCLAM" = 'y' ]]; then
	 # install clamav and clamd
	 echo
	 cecho "Installing clamav..."  $boldyellow
	 yum clean all -q
	 yum makecache fast -q
	 yum -y install clamav clamav-update clamav-server --disablerepo=rpmforge --disableexclude=epel --disableplugin=priorities
    # if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
    #   yum -y install clamav-server-systemd --disablerepo=rpmforge --disableexclude=epel --disableplugin=priorities
    #   systemctl daemon-reload
    # else
    #   yum -y install clamav-server-sysvinit --disablerepo=rpmforge --disableexclude=epel --disableplugin=priorities
    # fi
	 if [[ -z "$(grep clam /etc/yum.repos.d/rpmforge.repo)" ]]; then
		  sed -i 's/exclude=.*/exclude=pure-ftpd optipng clamav* clamd/' /etc/yum.repos.d/rpmforge.repo
	 fi
    if [[ "$(grep clam /etc/yum.repos.d/epel.repo)" ]]; then
      sed -i 's/exclude=.*/exclude=varnish varnish-libs galera nodejs nginx mongodb*/' /etc/yum.repos.d/epel.repo
    fi
	 # fix for CentOS 7 on system reboot missing /var/run/clamav directory
	 if [[ -f /etc/rc.d/init.d/clamd && -z "$(grep '/var/run/clamav' /etc/rc.d/init.d/clamd)" ]]; then
		  sed -i 's|# config: \/etc\/clamav.conf|# config: \/etc\/clamav.conf\n\nif [ ! -d /var/run/clamav ]; then\n\tmkdir -p \/var\/run\/clamav\n\tchown -R clamav:clamav \/var\/run\/clamav\n\tchmod -R 700 \/var\/run\/clamav\nfi|' /etc/rc.d/init.d/clamd
	 fi
  
    if [ -f /etc/clamd.conf ]; then
	 # tweak threads to reduce cpu load - default is 50 threads !
	 # it it to half the number of cpu threads detected
      sed -i "s|^MaxThreads 50|MaxThreads $MAXTHREADS|" /etc/clamd.conf
      cat /etc/clamd.conf | grep MaxThreads
    fi
  
    if [ ! -d /var/run/clamav/ ]; then
      mkdir -p /var/run/clamav/
      chown clamav:clamav /var/run/clamav/
    fi
  
  if [[ -f /etc/rc.d/init.d/clamd && -f /proc/user_beancounters ]]; then
    echo ""
    echo "*************************************************"
    cecho "* Correct service's stack size for OpenVZ systems. Please wait...." $boldgreen
    echo "*************************************************"
    sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/rc.d/init.d/clamd
    echo "checking stack size ulimit -s set properly: "
    head -n 5  /etc/rc.d/init.d/clamd  
	 /etc/rc.d/init.d/clamd stop
	 /etc/rc.d/init.d/clamd start
	 chkconfig clamd on
  fi
	 time freshclam
fi
}
#########################################################
starttime=$(TZ=UTC date +%s.%N)
{
maldetinstall
clamav_removeold
clamavinstall

echo
cecho "maldet + clamav installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log
echo "Total maldet + clamav Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_maldet_install_${DT}.log