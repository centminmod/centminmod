geoipphpext() {
if [[ -d /usr/share/GeoIP ]]; then
	if [ ! -f /usr/bin/svn ]; then
		echo "installing subversion..."
		yum -y -q install subversion
	fi
	
	echo "compiling geoip PHP extension..."
	cd $DIR_TMP
	svn checkout http://svn.php.net/repository/pecl/geoip/trunk geoip
	cd geoip
	make clean
	/usr/local/bin/phpize
	./configure --with-php-config=/usr/local/bin/php-config
	make$MAKETHREADS
	make install

cat > "/etc/centminmod/php.d/geoip.ini" <<EOF
extension=geoip.so
EOF
fi
}

geoipinstall() {

cecho "GeoIP database and library install..." $boldyellow

yum -y -q install GeoIP GeoIP-devel --disablerepo=rpmforge
rpm -ql GeoIP-devel GeoIP

	cd $DIR_TMP

cecho "GeoLiteCity database download ..." $boldyellow
	wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz -O /usr/share/GeoIP/GeoLiteCity.dat.gz
	gzip -d -f /usr/share/GeoIP/GeoLiteCity.dat.gz
	cp -a /usr/share/GeoIP/GeoLiteCity.dat /usr/share/GeoIP/GeoIPCity.dat
	
	# wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz -O /usr/share/GeoIP/GeoIP.dat.gz
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

	if [[ -z "$GEOIPCHECK" ]]; then
		sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/geoip.conf;/g' /usr/local/nginx/conf/nginx.conf
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


