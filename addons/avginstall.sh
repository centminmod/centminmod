#!/bin/bash
#########################################################
# Install AVG Anti-Virus Free edition for Linux on CentOS/RHEL systems
# default scans for /lib and /lib64 directories only
# in response to http://www.webhostingtalk.com/showpost.php?p=8569541&postcount=1171
# Summary at http://www.webhostingtalk.com/showthread.php?t=1235797
# written by George Liu vbtechsupport.com
# http://centminmod.com/avg_antivirus_free.html
#########################################################

if [[ ! -f /etc/redhat-release ]] ; then
	echo "No CentOS / RHEL system detected"
	echo "Please only install on CentOS / RHEL systems"
	echo "aborting install..."
	exit
else
	echo "CentOS / RHEL system detected"
fi

if [[ ! -f /usr/bin/lynx ]] ; then
	yum -y -q install lynx
fi

if [[ ! -f /usr/bin/wget ]] ; then
	yum -y -q install wget
fi

if [[ ! -f /bin/grep ]] ; then
	yum -y -q install grep
fi

if [[ ! -f /bin/awk ]] ; then
	yum -y -q install awk
fi
#########################################################

SRCDIR='/usr/local/src'
AVGDOWNLOADRPM=$(lynx -dump http://free.avg.com/us-en/download-free-all-product | grep 'download.avgfree.com' | grep 'rpm' | awk -F " " '{print $2}')
AVGRPMNAME=$(echo ${AVGDOWNLOADRPM##*/})

#########################################################
# functions

glibccheck() {

echo ""
echo "============================================"
echo "Check for 32bit glibc package"
echo "============================================"
yum -q list installed --disableexcludes=main glibc.i686 | grep -v Installed | grep glibc.i686

ERR=$?

if [ $ERR == '1' ]; then
	yum -y -q install --disableexcludes=main glibc.i686
fi

}

avginstall() {

echo "============================================"
echo "Downloading $AVGRPM ... "
echo "============================================"

cd $SRCDIR

if [ -s $AVGRPMNAME ]; then
  echo "$AVGRPMNAME [found]"
  else
  echo "Error: $AVGRPMNAME not found !!! Download now......"
  wget -c $AVGDOWNLOADRPM --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	echo "Error: $AVGRPMNAME download failed."
	exit $ERROR
else 
         echo "Download done."

echo ""
echo "rpm -Uvh $AVGRPMNAME"
rpm -Uvh $AVGRPMNAME

	ERR=$?
	CCAVGCHECK="$ERR"
		if [ "$CCAVGCHECK" == '0' ]; then
			echo "Ok: $AVGRPMNAME install."
			echo ""
		else
			echo "Error: problem with $AVGRPMNAME install."
			exit $ERR
		fi
	fi

fi

if [ -f /proc/user_beancounters ]; then
	sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/avgd

	service avgd restart
fi

}

avgupgrade() {

	echo "============================================"
	echo "Update AVG Free Anti-Virus for Linux... "
	echo "============================================"
	avgupdate

}

avglibscan() {

if [ -f /usr/bin/avgupdate ]; then

	echo ""
	service avgd status
	echo ""
	service avgd restart

avgupgrade

	echo ""
	echo "---------------------------------------------------------------"
	echo "Scan /lib"
	echo "---------------------------------------------------------------"
	avgscan /lib

	echo ""
	echo "---------------------------------------------------------------"
	echo "Scan /lib64"
	echo "---------------------------------------------------------------"
	avgscan /lib64
else
	echo ""
	echo "---------------------------------------------------------------"
	echo "AVG not installed"
	echo "---------------------------------------------------------------"
fi

}

#########################################################

if [ "$1" == 'libscan' ]; then
	avglibscan
else
	glibccheck
	avginstall
	avglibscan
fi
exit