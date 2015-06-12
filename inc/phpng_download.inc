########################################################################
phpng_download() {
	if [[ "$PHPMUVER" = 'NGDEBUG' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' ]] && [[ "$PHPMU_ALPHAVER" != '7.0.0alpha1' ]]; then
		if [[ "$PHPMUVER" = 'NGDEBUG' ]]; then
			PHPNGVER='NGDEBUG'
		elif [[ "$PHPMUVER" = '5.7' ]]; then
			PHPNGVER='5.7.0'
		elif [[ "$PHPMUVER" = '7.0' ]]; then
			PHPNGVER='7.0.0'
		fi
		cecho "Downloading PHP $PHPMUVER from https://git.php.net/repository/php-src.git" $boldyellow
		cd $DIR_TMP
		if [ -f php-${PHPNGVER}.tar.gz ]; then
			rm -rf php-${PHPNGVER}.tar.gz
		fi
		if [ -d php-${PHPNGVER} ]; then
			rm -rf php-${PHPNGVER}
		fi
		git clone https://git.php.net/repository/php-src.git php-${PHPNGVER}
		cecho "Download PHP $PHPMUVER completed" $boldyellow
		echo
		cecho "Creating tar archive" $boldyellow
		tar -czf php-${PHPNGVER}.tar.gz php-${PHPNGVER}
	elif [[ "$PHPMU_ALPHAVER" = '7.0.0alpha1' ]]; then
			PHPNGVER=$PHPMU_ALPHAVER
		cecho "Downloading PHP $PHPNGVER from https://downloads.php.net/~ab/" $boldyellow
		cd $DIR_TMP
		if [ -f php-${PHPNGVER}.tar.gz ]; then
			rm -rf php-${PHPNGVER}.tar.gz
		fi
		if [ -d php-${PHPNGVER} ]; then
			rm -rf php-${PHPNGVER}
		fi
####################################################################
        cecho "Download php-${PHPNGVER}.tar.gz ..." $boldyellow
    if [ -s php-${PHPNGVER}.tar.gz ]; then
        cecho "php-${PHPNGVER}.tar.gz found, skipping download..." $boldgreen
    else
        echo "wget -cnv https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3"
        wget -cnv https://downloads.php.net/~ab/php-${PHPNGVER}.tar.gz --tries=3
	ERROR=$?
		if [[ "$ERROR" != '0' ]]; then
			cecho "Error: php-${PHPNGVER}.tar.gz download failed." $boldgreen
			checklogdetails
			exit #$ERROR
		else 
        	cecho "Download done." $boldyellow
			#echo ""
		fi
    fi

	tar xzf php-${PHPNGVER}.tar.gz 
	ERROR=$?
		if [[ "$ERROR" != '0' ]]; then
			cecho "Error: php-${PHPNGVER}.tar.gz extraction failed." $boldgreen
			checklogdetails
			exit #$ERROR
		else 
        	 cecho "php-${PHPNGVER}.tar.gz valid file." $boldyellow
			echo ""
		fi		
	fi
####################################################################	
}