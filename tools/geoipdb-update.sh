#!/bin/bash
######################################################
# to update geoip country and city databases
######################################################
branchname='123.09beta01'

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
######################################################

echo
cecho "Updating GeoIP databases..." $boldyellow

\cp -f /usr/share/GeoIP/GeoIP.dat /usr/share/GeoIP/GeoIP.dat-backup
rm -rf /usr/share/GeoIP/GeoIP.dat
wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz -O /usr/share/GeoIP/GeoIP.dat.gz
gzip -df /usr/share/GeoIP/GeoIP.dat.gz

rm -rf /usr/share/GeoIP/GeoIPCity.dat
wget -cnv http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz -O /usr/share/GeoIP/GeoLiteCity.dat.gz
gzip -df /usr/share/GeoIP/GeoLiteCity.dat.gz
\cp -af /usr/share/GeoIP/GeoLiteCity.dat /usr/share/GeoIP/GeoIPCity.dat

nprestart

echo
cecho "Updated GeoIP databases" $boldyellow

exit