addmysql_legacy() {
  if [[ "$PHPMUVER" > 7.0 || "$PHPMUVER" = 7.0 ]]; then
    # legacy mysql extension addition
    # https://community.centminmod.com/posts/48931/
    echo
    echo "PHP 7+ detected which uses newer mysqlnd"
    echo "or PDO MySQL extensions and removed the"
    echo "legacy mysql extension. You can optionally"
    echo "re-add the removed legacy mysql extension"
    echo "to PHP 7+ by answering yes to next question"
    echo "Only answer yes if you know for sure you"
    echo "have very old web scripts which need mysql"
    echo "legacy extension re-added. Otherwise answer"
    echo "no which is recommended for best stability"
    echo
    read -ep "Re-add legacy mysql extension to PHP 7+ [y/n] ? " add_legacymysql
    echo ""
  fi
  if [[ "$add_legacymysql" != [yY] ]]; then
  	if [ -d "${DIR_TMP}/php-${phpver}/ext" ]; then
    	pushd "${DIR_TMP}/php-${phpver}/ext"
    	rm -rf mysql
    	popd
  	fi
  fi
}

addmysql_legacyinstall() {
  if [[ "$add_legacymysql" = [yY] ]]; then
    # http://php.net/manual/en/mysql.php
    # http://php.net/manual/en/mysql.installation.php
    # http://php.net/manual/en/mysqlinfo.api.choosing.php
    # https://ckon.wordpress.com/2015/08/06/put-mysql-functions-back-into-php-7/
    # https://github.com/php/pecl-database-mysql
    if [ -d "${DIR_TMP}/php-${phpver}/ext" ]; then
    	pushd "${DIR_TMP}/php-${phpver}/ext"
    	rm -rf mysql
    	git clone https://github.com/php/pecl-database-mysql mysql --recursive
    	cd "${DIR_TMP}/php-${phpver}/ext/mysql"
    	phpize
    	./configure --with-php-config=/usr/local/bin/php-config
    	make${MAKETHREADS}
    	make install
    	# echo "extension=mysql.so" > /etc/centminmod/php.d/mysql.ini
    	popd
  	fi
  fi
}