#!/bin/bash
######################################################
# written by George Liu (eva2000) vbtechsupport.com
# GeoIP2 version also exists have yet to test
# https://github.com/leev/ngx_http_geoip2_module
# http://dev.maxmind.com/geoip/geoip2/geolite2/
######################################################
# variables
#############
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR='/svr-setup'

# To enable GeoIP Update client requires an active 
# GeoIP subscription http://dev.maxmind.com/geoip/geoipupdate/
GEOIPUPDATE='n'
######################################################
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

###########################################
# functions
#############

geoipinstall() {

cecho "GeoIP database and library install..." $boldyellow

yum -y install GeoIP GeoIP-devel --disablerepo=rpmforge
rpm -ql GeoIP-devel GeoIP

	cd $DIR
# 	# wget http://geolite.maxmind.com/download/geoip/api/c/GeoIP.tar.gz
# 	# tar -zxvf GeoIP.tar.gz

#         cecho "Download GeoIP.tar.gz ..." $boldyellow
#     if [ -s GeoIP.tar.gz ]; then
#         cecho "GeoIP.tar.gz found, skipping download..." $boldgreen
#     else
#         wget -vnc http://geolite.maxmind.com/download/geoip/api/c/GeoIP.tar.gz --tries=3
# ERROR=$?
# 	if [[ "$ERROR" != '0' ]]; then
# 	cecho "Error: GeoIP.tar.gz download failed." $boldgreen
# 	exit #$ERROR
# else 
#          cecho "Download done." $boldyellow
# #echo ""
# 	fi
#     fi

# tar xzf GeoIP.tar.gz 
# ERROR=$?
# 	if [[ "$ERROR" != '0' ]]; then
# 	cecho "Error: GeoIP.tar.gz extraction failed." $boldgreen
# 	exit #$ERROR
# else 
#          cecho "GeoIP.tar.gz valid file." $boldyellow
# echo ""
# 	fi

# 	cd GeoIP-1.4.8
# 	./configure
# 	make
# 	make install
	
# 	echo '/usr/local/lib' > /etc/ld.so.conf.d/geoip.conf
# 	ldconfig
# 	ldconfig -v | grep GeoIP

cecho "GeoLiteCity database download ..." $boldyellow
	wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz -O /usr/share/GeoIP/GeoLiteCity.dat.gz
	# gzip -d /usr/local/share/GeoIP/GeoLiteCity.dat.gz
	gzip -d -f /usr/share/GeoIP/GeoLiteCity.dat.gz
	cp -a /usr/share/GeoIP/GeoLiteCity.dat /usr/share/GeoIP/GeoIPCity.dat
	
	# cp -a /usr/share/GeoIP/GeoIP.dat /usr/share/GeoIP/GeoIP.dat-backup
	# wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz -O /usr/share/GeoIP/GeoIP.dat.gz
	# gzip -df /usr/share/GeoIP/GeoIP.dat.gz
	
# if [[ "$GEOIPUPDATE" == [yY] ]]; then

# 	cecho "GeoIP Update client install..." $boldyellow
	
# 		cd $DIR
# 		# https://github.com/maxmind/geoipupdate/archive/master.zip -O GeoIPupdate.zip
# 		# unzip GeoIPupdate.zip
	
#         	cecho "Download GeoIPupdate.zip ..." $boldyellow
#     	if [ -s GeoIPupdate.zip ]; then
#         	cecho "GeoIPupdate.zip found, skipping download..." $boldgreen
#     	else
#         	wget -vnc --no-check-certificate https://github.com/maxmind/geoipupdate/archive/master.zip -O GeoIPupdate.zip --tries=3
# 	ERROR=$?
# 		if [[ "$ERROR" != '0' ]]; then
# 		cecho "Error: GeoIPupdate.zip download failed." $boldgreen
# 		exit #$ERROR
# 	else 
#          	cecho "Download done." $boldyellow
# 	#echo ""
# 		fi
#     	fi
	
# 	unzip GeoIPupdate.zip 
# 	ERROR=$?
# 		if [[ "$ERROR" != '0' ]]; then
# 		cecho "Error: GeoIPupdate.zip extraction failed." $boldgreen
# 		exit #$ERROR
# 	else 
#          	cecho "GeoIPupdate.zip valid file." $boldyellow
# 	echo ""
# 		fi
	
# 		cd geoipupdate-master
# 		./configure
# 		make
# 		make install
# 	fi

}

######################################################

geoinccheck() {

cecho "geoip.conf include check..." $boldyellow

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
geoip_city /usr/share/GeoIP/GeoLiteCity.dat;
EOF

echo
echo "cat /usr/local/nginx/conf/geoip.conf"
cat /usr/local/nginx/conf/geoip.conf
echo

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

	cecho "Test geoip.php file located at: " $boldyellow
	cecho "/usr/local/nginx/html/geoip.php" $boldyellow

}
##############################################################
starttime=$(date +%s.%N)
{
geoipinstall
geoinccheck
geoipphp

echo
cecho "GeoIP database and library installed..." $boldyellow
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_geoipdb_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_geoipdb_install_${DT}.log
echo "Total GeoIP database and libraries Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_geoipdb_install_${DT}.log

