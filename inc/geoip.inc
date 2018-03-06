ngxgeoipmemtweak() {
	# on low memory systems <=768MB or when NGINX_GEOIPMEM=n
	# optimise nginx memory usage when nginx geoip module is 
	# enabled (by default) by changing from in memory caching
	# of geoip country, city databases to on disk caching 
	if [[ "$TOTALMEM" -le '759000' || "$NGINX_GEOIPMEM" = [nN] ]]; then
		sed -i "s|_MEMORY_CACHE|_STANDARD|g" src/http/modules/ngx_http_geoip_module.c
		grep -n _STANDARD src/http/modules/ngx_http_geoip_module.c
	elif [[ "$NGINX_GEOIPMEM" = [yY] ]]; then
		sed -i "s|_STANDARD|_MEMORY_CACHE|g" src/http/modules/ngx_http_geoip_module.c
		grep -n _MEMORY_CACHE src/http/modules/ngx_http_geoip_module.c
	fi
}

geoipphpext() {
    if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ -d /usr/share/GeoIP ]]; then
	if [ ! -f /usr/bin/svn ]; then
		echo "installing subversion..."
		yum -y -q install subversion
		sar_call
	fi
	
	echo "compiling geoip PHP extension..."
	cd $DIR_TMP
	# fallback mirror if official php svn repo is down, use local centmin mod mirror
	# each php recompile will check again so if php svn repo is backup, the source
	# code will switch back to official php svn repo again
  curl -4Is --connect-timeout 5 --max-time 5 http://svn.php.net/repository/pecl/geoip/trunk/ | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
  SVNGEOIP_CURLCHECK=$?
  if [[ "$SVNGEOIP_CURLCHECK" != '0' ]]; then
  	if [ -d geoip ]; then
  		\cp -Rpf geoip geoip-backup
  		rm -rf geoip
  	fi
  	git clone https://github.com/centminmod/geoip geoip
  	sar_call
  else
  	if [ -d geoip ]; then
  		\cp -Rpf geoip geoip-backup
  		rm -rf geoip
  	fi
		svn checkout http://svn.php.net/repository/pecl/geoip/trunk/ geoip
		sar_call
  fi
		cd geoip
		if [[ "$INITIALINSTALL" != [yY] ]]; then
			make clean
		fi
		/usr/local/bin/phpize
		./configure --with-php-config=/usr/local/bin/php-config
		sar_call
		make$MAKETHREADS
		sar_call
		make install
		sar_call

cat > "${CONFIGSCANDIR}/geoip.ini" <<EOF
extension=geoip.so
geoip.custom_directory=/usr/share/GeoIP
EOF
	else
		echo "Error: geoip php extension not installed. Could not find ${DIR_TMP}/geoip directory" > ${CENTMINLOGDIR}/centminmod_errors_install_${DT}.log
fi
    fi # PHP_INSTALL=y
}

geoipinstall() {

cecho "GeoIP database and library install..." $boldyellow
if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    # GEOIPYUMCHECK=$(rpm -qa GeoIP)
    # GEOIPYUMCHECKB=$(rpm -qa GeoIP-devel)
    if [[ $(rpm -q GeoIP >/dev/null 2>&1; echo $?) != '0' ]] || [[ $(rpm -q GeoIP-devel >/dev/null 2>&1; echo $?) != '0' ]]; then
        yum -y -q install GeoIP GeoIP-devel --disablerepo=rpmforge
        sar_call
        rpm -ql GeoIP-devel GeoIP
    fi
else
		# GEOIPYUMCHECK=$(rpm -qa GeoIP)
		# GEOIPYUMCHECKB=$(rpm -qa GeoIP-devel)
    if [[ $(rpm -q GeoIP >/dev/null 2>&1; echo $?) != '0' ]] || [[ $(rpm -q GeoIP-devel >/dev/null 2>&1; echo $?) != '0' ]]; then
        yum -y -q install GeoIP GeoIP-devel
        sar_call
        rpm -ql GeoIP-devel GeoIP
    fi
fi

	cd $DIR_TMP

cecho "GeoLiteCity database download ..." $boldyellow
	wget -4 -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz -O /usr/share/GeoIP/GeoLiteCity.dat.gz
	gzip -d -f /usr/share/GeoIP/GeoLiteCity.dat.gz
	cp -a /usr/share/GeoIP/GeoLiteCity.dat /usr/share/GeoIP/GeoIPCity.dat
	
	# wget -4 -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz -O /usr/share/GeoIP/GeoIP.dat.gz
	# gzip -df /usr/share/GeoIP/GeoIP.dat.gz
}

######################################################

geoinccheck() {

# cecho "geoip.conf include check..." $boldyellow

	GEOIPCHECK=$(grep '/usr/local/nginx/conf/geoip.conf' /usr/local/nginx/conf/nginx.conf)

	if [[ ! -f /usr/local/nginx/conf/geoip.conf ]]; then

sed -i 's/}//' /usr/local/nginx/conf/php.conf

echo "# Set php-fpm geoip variables" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_COUNTRY_CODE \$geoip_country_code;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_COUNTRY_CODE3 \$geoip_country_code3;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_COUNTRY_NAME \$geoip_country_name;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_CITY_COUNTRY_CODE \$geoip_city_country_code;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_CITY_COUNTRY_CODE3 \$geoip_city_country_code3;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_CITY_COUNTRY_NAME \$geoip_city_country_name;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_REGION \$geoip_region;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_CITY \$geoip_city;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_POSTAL_CODE \$geoip_postal_code;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_CITY_CONTINENT_CODE \$geoip_city_continent_code;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_LATITUDE \$geoip_latitude;" >> /usr/local/nginx/conf/php.conf
echo "fastcgi_param GEOIP_LONGITUDE \$geoip_longitude;" >> /usr/local/nginx/conf/php.conf
echo "}" >> /usr/local/nginx/conf/php.conf

cat > "/usr/local/nginx/conf/geoip.conf" <<EOF
# SET the path to the .dat file used for determining the visitors country from the IP-address ###
geoip_country /usr/share/GeoIP/GeoIP.dat;

# SET the path to the .dat file used for determining the visitors country from the IP-address ###
geoip_city /usr/share/GeoIP/GeoIPCity.dat;
EOF

#echo
#echo "cat /usr/local/nginx/conf/geoip.conf"
#cat /usr/local/nginx/conf/geoip.conf
#echo

	fi

	# check if include file /usr/local/nginx/conf/geoip.conf exists in
	# /usr/local/nginx/conf/nginx.conf
	# if does not exist insert using sed a new line after http { for
	# include /usr/local/nginx/conf/geoip.conf;
	if [[ -z "$GEOIPCHECK" ]]; then
		# check if use set in centmin.sh NGINX_GEOIP=y or n, if yes
		# insert include /usr/local/nginx/conf/geoip.conf;, if no
		# inset commented out version #include /usr/local/nginx/conf/geoip.conf;
		if [[ "$NGINX_GEOIP" = [yY] ]]; then
			sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/geoip.conf;/g' /usr/local/nginx/conf/nginx.conf
			sed -i 's/#fastcgi_param GEOIP_/fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
		else
			sed -i 's/http {/http { \n#include \/usr\/local\/nginx\/conf\/geoip.conf;/g' /usr/local/nginx/conf/nginx.conf
			sed -i 's/fastcgi_param GEOIP_/#fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
		fi
	else
		# if include /usr/local/nginx/conf/geoip.conf; line already exists in nginx.conf
		# and NGINX_GEOIP=y, ensure that the line isn't commented out
		# if NGINX_GEOIP=n, then comment out the include line with hash #
		if [[ "$NGINX_GEOIP" = [yY] ]]; then
			sed -i 's/#include \/usr\/local\/nginx\/conf\/geoip.conf;/include \/usr\/local\/nginx\/conf\/geoip.conf;/g' /usr/local/nginx/conf/nginx.conf
			sed -i 's/#fastcgi_param GEOIP_/fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
		else
			sed -i 's/include \/usr\/local\/nginx\/conf\/geoip.conf;/#include \/usr\/local\/nginx\/conf\/geoip.conf;/g' /usr/local/nginx/conf/nginx.conf
			sed -i 's/fastcgi_param GEOIP_/#fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
		fi
	fi
}

######################################################

geoipphp() {
	cat > "/usr/local/nginx/html/geoip.php" <<END
<html>
<body>
<?php	
\$geoip_country_code = getenv(GEOIP_COUNTRY_CODE);
/*
\$geoip_country_code = \$_SERVER['GEOIP_COUNTRY_CODE']; // works as well
*/
\$geoip_country_code3 = getenv(GEOIP_COUNTRY_CODE3);
\$geoip_country_name = getenv(GEOIP_COUNTRY_NAME);	
\$geoip_city_country_code = getenv(GEOIP_CITY_COUNTRY_CODE);
\$geoip_city_country_code3 = getenv(GEOIP_CITY_COUNTRY_CODE3);
\$geoip_city_country_name = getenv(GEOIP_CITY_COUNTRY_NAME);
\$geoip_region = getenv(GEOIP_REGION);
\$geoip_city = getenv(GEOIP_CITY);
\$geoip_postal_code = getenv(GEOIP_POSTAL_CODE);
\$geoip_city_continent_code = getenv(GEOIP_CITY_CONTINENT_CODE);
\$geoip_latitude = getenv(GEOIP_LATITUDE);
\$geoip_longitude = getenv(GEOIP_LONGITUDE);	
echo 'country_code: '.\$geoip_country_code.'<br>';
echo 'country_code3: '.\$geoip_country_code3.'<br>';
echo 'country_name: '.\$geoip_country_name.'<br>';	
echo 'city_country_code: '.\$geoip_city_country_code.'<br>';
echo 'city_country_code3: '.\$geoip_city_country_code3.'<br>';
echo 'city_country_name: '.\$geoip_city_country_name.'<br>';
echo 'region: '.\$geoip_region.'<br>';
echo 'city: '.\$geoip_city.'<br>';
echo 'postal_code: '.\$geoip_postal_code.'<br>';
echo 'city_continent_code: '.\$geoip_city_continent_code.'<br>';
echo 'latitude: '.\$geoip_latitude.'<br>';
echo 'longitude: '.\$geoip_longitude.'<br>';	
?>
</body>
</html>
END

	#cecho "Test geoip.php file located at: " $boldyellow
	#cecho "/usr/local/nginx/html/geoip.php" $boldyellow

}


