########################################################################
phpng_download() {
	if [[ "$PHPMUVER" = 'NG' || "$PHPMUVER" = '5.7' ]]; then
		if [[ "$PHPMUVER" = 'NG' ]]; then
			PHPNGVER='NG'
		elif [[ "$PHPMUVER" = '5.7' ]]; then
			PHPNGVER='5.7.0'
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
	fi
}