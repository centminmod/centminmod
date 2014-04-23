axelsetup() {

if [ ! -f /usr/bin/axel ]; then
	AXELRPMCHECK=0
	echo
	echo "Detected that axel download accelerator"
	echo "was not installed via rpmforge repository"
	echo
	echo "Trying alternate mirror for required RPMs"
	echo

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then

    cd $DIR_TMP

if [ -s rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm ]; then
  echo "rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm [found]"
  else
  echo "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm not found !!! Download now......"
  wget -cnv http://centminmod.com/centminmodparts/v123/04rpms/${ARCH}/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	exit $ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s axel-2.4-1.el5.rf.${ARCH}.rpm ]; then
  echo "axel-2.4-1.el5.rf.${ARCH}.rpm [found]"
  else
  echo "Error: axel-2.4-1.el5.rf.${ARCH}.rpm not found !!! Download now......"
  wget -cnv http://centminmod.com/centminmodparts/v123/04rpms/${ARCH}/axel-2.4-1.el5.rf.${ARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: axel-2.4-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
FAILEDAXELRPM='y'
checklogdetails
	exit $ERROR
else 
         cecho "Download done." $boldyellow
rpm -i axel-2.4-1.el5.rf.${ARCH}.rpm
	ERR=$?
	AXELFORGECHECK="$ERR"
		if [ "$AXELFORGECHECK" == '0' ]; then
			#yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with axel-2.4-1.el5.rf.${ARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

fi # centos ver

###

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        AARCH='x86_64'
       ARPMFARCH='x86_64'
    else
        AARCH='i686'
        ARPMFARCH='i386'
    fi

    cd $DIR_TMP

if [ -s rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm ]; then
  echo "rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm [found]"
  else
  echo "Error: rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm not found !!! Download now......"
  wget -cnv http://centminmod.com/centminmodparts/v123/04rpms/${ARPMFARCH}/rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	exit $ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with rpmforge-release-0.5.3-1.el6.rf.${AARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s axel-2.4-1.el6.rf.${AARCH}.rpm ]; then
  echo "axel-2.4-1.el6.rf.${AARCH}.rpm [found]"
  else
  echo "Error: axel-2.4-1.el6.rf.${AARCH}.rpm not found !!! Download now......"
  wget -cnv http://centminmod.com/centminmodparts/v123/04rpms/${ARPMFARCH}/axel-2.4-1.el6.rf.${AARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: axel-2.4-1.el6.rf.${AARCH}.rpm download failed." $boldgreen
FAILEDAXELRPM='y'
checklogdetails
	exit $ERROR
else 
         cecho "Download done." $boldyellow
rpm -i axel-2.4-1.el6.rf.${AARCH}.rpm
	ERR=$?
	AXELFORGECHECK="$ERR"
		if [ "$AXELFORGECHECK" == '0' ]; then
			#yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with axel-2.4-1.el6.rf.${AARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

fi # centos vercheck

fi
}