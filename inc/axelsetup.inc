install_axel() {
	cd $DIR_TMP

        cecho "Download $AXEK_LINKFILE ..." $boldyellow
    if [ -s $AXEK_LINKFILE ]; then
        cecho "Axel ${AXEL_VER} Archive found, skipping download..." $boldgreen 
    else
        wget -O $AXEK_LINKFILE $AXEK_LINK
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $AXEK_LINKFILE download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download $AXEK_LINKFILE done." $boldyellow
#echo ""
    fi
    fi

tar xzf $AXEK_LINKFILE
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $AXEK_LINKFILE extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "$AXEK_LINKFILE valid file." $boldyellow
echo ""
    fi

	cd axel-${AXEL_VER}
	./configure
	make${MAKETHREADS}
	make install
	which axel
}

axelsetup() {

	if [[ "$INITIALINSTALL" = [yY] ]]; then
		# remove any yum installed axel packages
		# in favour of source compiled package
		if [[ -z "$(rpm -ql axel | grep 'not installed')" && -f /usr/bin/axel ]]; then
			yum -q -y remove axel
		fi
	elif [[ "$INITIALINSTALL" != [yY] ]]; then
		# remove any yum installed axel packages
		# in favour of source compiled package
		if [[ -z "$(rpm -ql axel | grep 'not installed')" && -f /usr/bin/axel ]]; then
			yum -q -y remove axel
		fi
	fi

	if [ ! -f /usr/bin/axel ]; then
		AXELRPMCHECK=0
		# echo
		# echo "Detected that axel download accelerator"
		# echo "was not installed via rpmforge repository"
		# echo
		# echo "Trying alternate mirror for required RPMs"
		# echo

	if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' || "$CENTOSVER" == '5.11' || "$CENTOSVER" == '5.12' || "$CENTOSVER" == '5.13' || "$CENTOSVER" == '5.14' ]]; then

    	cd $DIR_TMP

		if [[ "$RPMFORGEREPO_DISABLE" = [nN] ]]; then
			if [ -f "${DIR_TMP}/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm" ]; then
  			echo "rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm [found]"
  			else
  			echo "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm not found !!! Downloading now......"
  			wget -cnv http://centminmod.com/centminmodparts/v123/04rpms/${ARCH}/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm --tries=3 
			ERROR=$?
				if [[ "$ERROR" != '0' ]]; then
				cecho "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
			FAILRPMFORGE='y'
			checklogdetails
				# exit $ERROR
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
		fi # RPMFORGEREPO_DISABLE = n

		if [ -f "${DIR_TMP}/axel-2.4-1.el5.rf.${ARCH}.rpm" ]; then
  		echo "axel-2.4-1.el5.rf.${ARCH}.rpm [found]"
  		else
  		echo "Error: axel-2.4-1.el5.rf.${ARCH}.rpm not found !!! Downloading now......"
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
	fi # centos 5.x ver

	###
	if [[ "$CENTOS_SIX" = '6' ]]; then
		install_axel
	fi
	###

	if [[ "$CENTOS_SEVEN" = '7' ]]; then
		# axel download accelerator works with CentOS 7.x and epel repo
		# unlike axel provided by rpmforge on CentOS 6.x repo
		# yum -y install axel
		install_axel
	fi
	fi # check for /usr/bin/axel
}