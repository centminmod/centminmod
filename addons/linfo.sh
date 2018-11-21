#/bin/bash
#################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
#################################################
# linfo installer written by George Liu (eva2000) vbtechsupport.com
# latest version at http://linfo.sourceforge.net/
#################################################
DIR_TMP='/svr-setup'
LINFO_VER='2.0.3'
LINFOBASE='/usr/local/nginx/html'	# DO NOT CHANGE
LINFODIR='cinfo'
LINFOPATH="${LINFOBASE}/${LINFODIR}"
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
#################################################
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

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

updatever() {
	read -ep "Enter linfo version number: " LINFO_VER
}

deletecinfo() {
	rm -rf ${LINFOPATH}
	rm -rf /usr/local/nginx/conf/cinfo_htpasswd
	cecho "-------------------------------------------------------" $boldyellow
	cecho "rm -rf ${LINFOPATH}" $boldgreen
	cecho "rm -rf /usr/local/nginx/conf/cinfo_htpasswd" $boldgreen
	cecho "-------------------------------------------------------" $boldyellow
}

linfosetup() {

cd $DIR_TMP

        cecho "Download linfo-${LINFO_VER}.tar.gz ..." $boldyellow
    if [ -s linfo-${LINFO_VER}.tar.gz ]; then
        cecho "linfo-${LINFO_VER}.tar.gz found, skipping download..." $boldgreen
    else
       wget -q -c http://downloads.sourceforge.net/project/linfo/Linfo%20Stable%20Releases/linfo-${LINFO_VER}.tar.gz --tries=3
       # wget -q -c http://sourceforge.net/projects/linfo/files/Linfo%20Stable%20Releases/linfo-${LINFO_VER}.tar.gz/download --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: linfo-${LINFO_VER}.tar.gz download failed." $boldgreen
	exit $ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

mkdir -p linfo-${LINFO_VER}
tar -xzf linfo-${LINFO_VER}.tar.gz -C linfo-${LINFO_VER}
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: linfo-${LINFO_VER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit $ERROR
else 
         cecho "linfo-${LINFO_VER}.tar.gz valid file." $boldyellow
echo ""
	fi

rm -rf ${LINFOPATH}
\cp -aRf linfo-${LINFO_VER}/* ${LINFOPATH}

chown nginx:nginx ${LINFOPATH}
chown -R nginx:nginx ${LINFOPATH}

rm -rf ${LINFOPATH}/config.inc.php
/bin/cp -af ${LINFOPATH}/sample.config.inc.php ${LINFOPATH}/config.inc.php

}

###########################################
passp() {

CSALT=$(openssl rand 6 -base64 | tr -dc 'a-zA-Z0-9')
CUSER=$(echo "admin${CSALT}")
CPASS=$(openssl rand 19 -base64 | tr -dc 'a-zA-Z0-9')

    if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
      hname=$(hostname)
    else
      hname=$(hostname -f)
    fi

echo ""
cecho "Creating cinfo_htpasswd user/pass..." $boldyellow
echo "python /usr/local/nginx/conf/htpasswd.py -b /usr/local/nginx/conf/cinfo_htpasswd $CUSER $CPASS"

if [ ! -f /usr/local/nginx/conf/cinfo_htpasswd ]; then
	touch /usr/local/nginx/conf/cinfo_htpasswd
fi

python /usr/local/nginx/conf/htpasswd.py -b /usr/local/nginx/conf/cinfo_htpasswd $CUSER $CPASS

echo "setup /usr/local/nginx/conf/phpallowed.conf"
\cp -af /usr/local/nginx/conf/php.conf /usr/local/nginx/conf/phpallowed.conf
sed -i 's/fastcgi_param PHP_ADMIN_VALUE open_basedir/#fastcgi_param PHP_ADMIN_VALUE open_basedir/' /usr/local/nginx/conf/phpallowed.conf

echo ""
cecho "cinfo_htpasswd user/pass created..." $boldyellow
cat /usr/local/nginx/conf/cinfo_htpasswd

echo ""
cecho "Create /usr/local/nginx/conf/cinfo.conf" $boldyellow
echo ""
cat > "/usr/local/nginx/conf/cinfo.conf" <<EOF
            location /${LINFODIR}/ {
	auth_basic "Private";
	auth_basic_user_file /usr/local/nginx/conf/cinfo_htpasswd;
	include /usr/local/nginx/conf/phpallowed.conf;
            }
EOF

cat /usr/local/nginx/conf/cinfo.conf

echo ""
cecho "Setup virtual.conf" $boldyellow
cecho "Adding /usr/local/nginx/conf/cinfo.conf include entry" $boldyellow

CHECKCINFO=$(grep cinfo.conf /usr/local/nginx/conf/conf.d/virtual.conf)

if [[ -z "$CHECKCINFO" ]]; then

sed -i '/include \/usr\/local\/nginx\/conf\/staticfiles.conf;/a \include \/usr\/local\/nginx\/conf\/cinfo.conf;' /usr/local/nginx/conf/conf.d/virtual.conf

fi

echo ""
nprestart

echo ""
cecho "-------------------------------------------------------" $boldyellow
cecho "Password protected ${hname}/${LINFODIR}" $boldgreen
cecho "at path ${LINFOPATH}" $boldgreen
cecho "-------------------------------------------------------" $boldyellow
cecho "Username: $CUSER" $boldgreen
cecho "Password: $CPASS" $boldgreen
cecho "-------------------------------------------------------" $boldyellow

}
###########################################

case "$1" in
install)
if [[ ! -d ${LINFOPATH} ]]; then
	linfosetup
	passp
else
	cecho "--------------------------------------------------------------" $boldgreen
	cecho " linfo install detected" $boldyellow
	cecho " run update instead:" $boldyellow
	cecho " latest version number at http://linfo.sourceforge.net/" $boldyellow
	cecho " $0 update" $boldyellow
	cecho "--------------------------------------------------------------" $boldgreen
fi
;;
update)
	deletecinfo
	updatever
	linfosetup
	passp
;;
delete)
	deletecinfo
;;
*)
	echo "$0 install"
	echo "$0 update"
	echo "$0 delete"
;;
esac
exit