checklogdetails() {
	echo " "
	cecho "----------------------------------------------------------------------------------" $boldyellow
	echo "check Centmin Mod log for details at /root/centminlogs/"
	echo "Aborting script..."
	cecho "----------------------------------------------------------------------------------" $boldyellow
	#exit #$ERROR
}

###############################################
yumcentosfivedownload() {

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then
{
    cd $DIR_TMP

if [ -s epel-release-5-4.noarch.rpm ]; then
  echo "epel-release-5-4.noarch.rpm [found]"
  else
  echo "Error: epel-release-5-4.noarch.rpm not found !!! Download now......"
  wget -cnv http://mirror.us.leaseweb.net/epel/5/${ARCH}/epel-release-5-4.noarch.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: epel-release-5-4.noarch.rpm download failed." $boldgreen
FAILEPEL='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm -i epel-release-5-4.noarch.rpm
	yum -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCEPELCHECK="$ERR"
		if [ "$CCEPELCHECK" == '0' ]; then
			yumpriorepel
			echo ""
		else
			cecho "Error: problem with epel-release-5-4.noarch.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [[ -f /etc/yum.repos.d/rpmforge.repo || -f /etc/yum.repos.d/rpmforge.OLD ]]; then
	rm -rf /etc/yum.repos.d/rpmforge.repo
	rm -rf /etc/yum.repos.d/rpmforge.OLD
	rm -rf ${DIR_TMP}/rpmforge-release*

	FORGERPM=$(rpm -qa | grep rpmforge-release)
	rpm -e $FORGERPM
fi

if [ -s rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm ]; then
  echo "rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm [found]"
  else
  echo "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm not found !!! Download now......"
  wget -cnv http://pkgs.repoforge.org/rpmforge-release/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	#exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm
	yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm install." $boldgreen
			#exit $ERR
		fi
	fi

fi


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s centalt-release-5-3.noarch.rpm ]; then
  echo "centalt-release-5-3.noarch.rpm [found]"
  else
  echo "Error: centalt-release-5-3.noarch.rpm not found !!! Download now......"
  wget -cnv http://centos.alt.ru/repository/centos/5/${RPMFARCH}/centalt-release-5-3.noarch.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: centalt-release-5-3.noarch.rpm download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm - import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -i centalt-release-5-3.noarch.rpm
yumpriorcentalt
echo ""
	fi

fi
fi # CENTALTREPO_DISABLE

if [ -s percona-release-0.0-1.${ARCH}.rpm ]; then
  echo "percona-release-0.0-1.${ARCH}.rpm [found]"
  else
  echo "Error: percona-release-0.0-1.${ARCH}.rpm not found !!! Download now......"
  wget -cnv http://www.percona.com/downloads/percona-release/percona-release-0.0-1.${ARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: percona-release-0.0-1.${ARCH}.rpm download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -i percona-release-0.0-1.${ARCH}.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCPERCONACHECK="$ERR"
		if [ "$CCPERCONACHECK" == '0' ]; then
			yumpriorpercona
			echo ""
		else
			cecho "Error: problem with percona-release-0.0-1.${ARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s varnish-release-3.0-1.el5.centos.noarch.rpm ]; then
  echo "varnish-release-3.0-1.el5.centos.noarch.rpm [found]"
  else
  echo "Error: varnish-release-3.0-1.el5.centos.noarch.rpm not found !!! Download now......"
  wget -cnv http://repo.varnish-cache.org/redhat/varnish-3.0/el5/noarch/varnish-release/varnish-release-3.0-1.el5.centos.noarch.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: varnish-release-3.0-1.el5.centos.noarch.rpm download failed." $boldgreen
FAILVARNISH='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow

rpm --nosignature -i varnish-release-3.0-1.el5.centos.noarch.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCVARNISHCHECK="$ERR"
		if [ "$CCVARNISHCHECK" == '0' ]; then
			yumpriorvarnish
			echo ""
		else
			cecho "Error: problem with varnish-release-3.0-1.el5.centos.noarch.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

####

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos5.log
fi # CENTOS 5.x

}

###############################################
yumcentossixdownload() {

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
{
echo "*************************************************"
cecho "* CentOS 6 / 6.1 / 6.2 detected installing EPEL Repo" $boldgreen
echo "*************************************************"

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
       RPMFARCH='x86_64'
    else
        ARCH='i686'
        RPMFARCH='i386'
    fi

    cd $DIR_TMP

if [ -s epel-release-6-8.noarch.rpm ]; then
  echo "epel-release-6-8.noarch.rpm [found]"
  else
  echo "Error: epel-release-6-8.noarch.rpm not found !!! Download now......"
  wget -cnv http://mirror.us.leaseweb.net/epel/6/${RPMFARCH}/epel-release-6-8.noarch.rpm --tries=3
  # wget -cnv http://centminmod.com/centminmodparts/repos/epel-release-6-8.noarch.rpm --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: epel-release-6-8.noarch.rpm download failed." $boldgreen
FAILEPEL='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm -i epel-release-6-8.noarch.rpm
	yum -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCEPELCHECK="$ERR"
		if [ "$CCEPELCHECK" == '0' ]; then
			yumpriorepel
			echo ""
		else
			cecho "Error: problem with epel-release-6-8.noarch.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [[ -f /etc/yum.repos.d/rpmforge.repo || -f /etc/yum.repos.d/rpmforge.OLD ]]; then
	rm -rf /etc/yum.repos.d/rpmforge.repo
	rm -rf /etc/yum.repos.d/rpmforge.OLD
	rm -rf ${DIR_TMP}/rpmforge-release*

	FORGERPM=$(rpm -qa | grep rpmforge-release)
	rpm -e $FORGERPM
fi

if [ -s rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm ]; then
  echo "rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm [found]"
  else
  echo "Error: rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm not found !!! Download now......"
  wget -cnv http://pkgs.repoforge.org/rpmforge-release/rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	#exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm
	yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with rpmforge-release-0.5.3-1.el6.rf.${ARCH}.rpm install." $boldgreen
			#exit $ERR
		fi
	fi

fi


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s centalt-release-6-1.noarch.rpm ]; then
  echo "centalt-release-6-1.noarch.rpm [found]"
  else
  echo "Error: centalt-release-6-1.noarch.rpm not found !!! Download now......"
  wget -cnv http://centos.alt.ru/repository/centos/6/${RPMFARCH}/centalt-release-6-1.noarch.rpm --tries=3
  # wget -cnv http://centminmod.com/centminmodparts/repos/centalt-release-6-1.noarch.rpm --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: centalt-release-6-1.noarch.rpm download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm - import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -i centalt-release-6-1.noarch.rpm
yumpriorcentalt
echo ""
	fi

fi
fi # CENTALTREPO_DISABLE


if [ -s percona-release-0.0-1.${RPMFARCH}.rpm ]; then
  echo "percona-release-0.0-1.${RPMFARCH}.rpm [found]"
  else
  echo "Error: percona-release-0.0-1.${RPMFARCH}.rpm not found !!! Download now......"
  wget -cnv http://www.percona.com/downloads/percona-release/percona-release-0.0-1.${RPMFARCH}.rpm --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: percona-release-0.0-1.${RPMFARCH}.rpm download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -i percona-release-0.0-1.${RPMFARCH}.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCPERCONACHECK="$ERR"
		if [ "$CCPERCONACHECK" == '0' ]; then
			yumpriorpercona
			echo ""
		else
			cecho "Error: problem with percona-release-0.0-1.${RPMFARCH}.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s varnish-release-3.0-1.el6.noarch.rpm ]; then
  echo "varnish-release-3.0-1.el6.noarch.rpm [found]"
  else
  echo "Error: varnish-release-3.0-1.el6.noarch.rpm not found !!! Download now......"
  wget -cnv http://repo.varnish-cache.org/redhat/varnish-3.0/el6/noarch/varnish-release/varnish-release-3.0-1.el6.noarch.rpm --tries=3
  # wget -cnv http://centminmod.com/centminmodparts/repos/varnish-release-3.0-1.el6.noarch.rpm --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: varnish-release-3.0-1.el6.noarch.rpm download failed." $boldgreen
FAILVARNISH='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow

rpm --nosignature -i varnish-release-3.0-1.el6.noarch.rpm
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCVARNISHCHECK="$ERR"
		if [ "$CCVARNISHCHECK" == '0' ]; then
			yumpriorvarnish
			echo ""
		else
			cecho "Error: problem with varnish-release-3.0-1.el6.noarch.rpm install." $boldgreen
			exit $ERR
		fi
	fi

fi

####



# CentOS 6 repo mirrors aren't fully updated so need to specify actual kernel-headers version
PACKAGEEXTRA='axel mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake28 rssh'

declare -a arrays
arrays=(${PACKAGEEXTRA})

for pkgs in ${arrays[@]}
do
echo Processing package: $pkgs
YUMINSTALLRPMCHECKS=`yum -q list installed $pkgs | awk '/Installed/ {print $1}' &>/dev/null`
#echo $YUMINSTALLRPMCHECKS
        if [ "$YUMINSTALLRPMCHECKS" == 'Installed' ]; then
echo "----------------------------------------------------------------------------------"
        yum -q -y update $pkgs
        yum -q list $pkgs
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgs" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum -q -y install $pkgs
        yum -q list $pkgs
echo "----------------------------------------------------------------------------------"
        fi
done

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos6.log
fi

}

###############################################

###############################################
phptarball() {

if [[ "$PHP_INSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download php-${PHP_VERSION}.tar.gz ..." $boldyellow
    if [ -s php-${PHP_VERSION}.tar.gz ]; then
        cecho "php-${PHP_VERSION}.tar.gz found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.gz/from/$(echo $PHP_MIRRORURL | sed -e 's/http:\/\///')/mirror $AXELPHPTARGZ $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: php-${PHP_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf php-${PHP_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: php-${PHP_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "php-${PHP_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi

}


###############################################
# when adding additional modules, make sure to update inc/nginx_upgrade.inc
# checknginxmodules function needs updating as well

nginxpcretarball() {

    cd $DIR_TMP

        cecho "Download pcre-${NGINX_PCREVER}.tar.gz ..." $boldyellow
    if [ -s pcre-${NGINX_PCREVER}.tar.gz ]; then
        cecho "pcre-${NGINX_PCREVER}.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://downloads.sourceforge.net/project/pcre/pcre/${NGINX_PCREVER}/pcre-${NGINX_PCREVER}.tar.gz --tries=3 
        # wget -c --progress=bar http://centminmod.com/centminmodparts/pcre/pcre-${NGINX_PCREVER}.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: pcre-${NGINX_PCREVER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf pcre-${NGINX_PCREVER}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: pcre-${NGINX_PCREVER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "pcre-${NGINX_PCREVER}.tar.gz valid file." $boldyellow
echo ""
	fi
}

nginxwebdavtarball() {

    cd $DIR_TMP

        cecho "Download nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz ..." $boldyellow
    if [ -s nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz ]; then
        cecho "nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate https://github.com/arut/nginx-dav-ext-module/archive/v${NGINX_EXTWEBDAVVER}.tar.gz -O nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz valid file." $boldyellow
echo ""
	fi
}

nginxpgspeedtarball() {

	if [[ "$NGINX_PAGESPEED" = [yY] ]]; then

    cd $DIR_TMP

    # official github master zip renamed
    if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then

            cecho "Download release-${NGXPGSPEED_VER}.tar.gz ..." $boldyellow
    		if [ -s release-${NGXPGSPEED_VER}.tar.gz ]; then
        	cecho "release-${NGXPGSPEED_VER}.tar.gz Archive found, skipping download..." $boldgreen
    else
    	    wget -c --progress=bar --no-check-certificate https://github.com/pagespeed/ngx_pagespeed/archive/master.tar.gz -O release-${NGXPGSPEED_VER}.tar.gz --tries=3 
			ERROR=$?
				if [[ "$ERROR" != '0' ]]; then
					cecho "Error: release-${NGXPGSPEED_VER}.tar.gz download failed." $boldgreen
					checklogdetails
					exit #$ERROR
				else 
         			cecho "Download done." $boldyellow
					#echo ""
				fi
    fi

	fi # NGINX_PAGESPEEDGITMASTER

        cecho "Download release-${NGXPGSPEED_VER}.tar.gz ..." $boldyellow
    if [ -s release-${NGXPGSPEED_VER}.tar.gz ]; then
        cecho "release-${NGXPGSPEED_VER}.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate https://github.com/pagespeed/ngx_pagespeed/archive/release-${NGXPGSPEED_VER}.tar.gz -O release-${NGXPGSPEED_VER}.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: release-${NGXPGSPEED_VER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf release-${NGXPGSPEED_VER}.tar.gz

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: release-${NGXPGSPEED_VER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "release-${NGXPGSPEED_VER}.tar.gz valid file." $boldyellow
echo ""
	fi

if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
	\cp -Rf ngx_pagespeed-master ngx_pagespeed-release-${NGXPGSPEED_VER}
	rm -rf ngx_pagespeed-master
	rm -rf release-${NGXPGSPEED_VER}.tar.gz
fi

        cecho "Download ${NGINX_PAGESPEEDPSOL_VER}.tar.gz ..." $boldyellow
    if [ -s ${NGINX_PAGESPEEDPSOL_VER}.tar.gz ]; then
        cecho "${NGINX_PAGESPEEDPSOL_VER}.tar.gz Archive found, skipping download..." $boldgreen
    else
    cd $DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}
        wget -c --progress=bar --no-check-certificate https://dl.google.com/dl/page-speed/psol/${NGINX_PAGESPEEDPSOL_VER}.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGINX_PAGESPEEDPSOL_VER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGINX_PAGESPEEDPSOL_VER}.tar.gz
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGINX_PAGESPEEDPSOL_VER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGINX_PAGESPEEDPSOL_VER}.tar.gz valid file." $boldyellow
    cd $DIR_TMP
echo ""
	fi

	fi # NGINX_PAGESPEED=y

}

ngxmoduletarball() {
### nginx modules ###

    cd $DIR_TMP

        cecho "Download ngx-fancyindex-0.3.1.tar.gz ..." $boldyellow
    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://centminmod.com/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx-fancyindex-0.3.1.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ngx-fancyindex-0.3.1.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx-fancyindex-0.3.1.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "ngx-fancyindex-0.3.1.tar.gz valid file." $boldyellow
echo ""
	fi

if [ ! -d /svr-setup/ngx-fancyindex-ngx-fancyindex ]; then
        cecho "Download ngx-fancyindex-0.3.1.tar.gz ..." $boldyellow
rm -rf ngx-fancyindex-0.3.1.tar.gz

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar http://centminmod.com/centminmodparts/nginx/modules/ngx-fancyindex-0.3.1.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx-fancyindex-0.3.1.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi
fi #fancyindex recheck

tar xzf ngx-fancyindex-0.3.1.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx-fancyindex-0.3.1.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "ngx-fancyindex-0.3.1.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download ngx_cache_purge-2.1.tar.gz ..." $boldyellow
    if [ -s ngx_cache_purge-2.1.tar.gz ]; then
        cecho "ngx_cache_purge 2.1 Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://centminmod.com/centminmodparts/nginx/modules/ngx_cache_purge-2.1.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx_cache_purge-2.1.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ngx_cache_purge-2.1.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ngx_cache_purge-2.1.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "ngx_cache_purge-2.1.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download Nginx-accesskey-2.0.3.tar.gz ..." $boldyellow
    if [ -s Nginx-accesskey-2.0.3.tar.gz ]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        wget -c http://wiki.nginx.org/images/5/51/Nginx-accesskey-2.0.3.tar.gz --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: Nginx-accesskey-2.0.3.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf Nginx-accesskey-2.0.3.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: Nginx-accesskey-2.0.3.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Nginx-accesskey-2.0.3.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download nginx-http-concat-master.tar.gz ..." $boldyellow
    if [ -s nginx-http-concat-master.tar.gz ]; then
        cecho "nginx-http-concat-master.tar.gz Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate https://github.com/alibaba/nginx-http-concat/archive/master.tar.gz -O nginx-http-concat-master.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-http-concat-master.tar.gz download failed." $boldgreen

checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf nginx-http-concat-master.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-http-concat-master.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "nginx-http-concat-master.tar.gz valid file." $boldyellow
echo ""
	fi

nginxpcretarball
nginxwebdavtarball
nginxpgspeedtarball

### nginx modules ###
} # ngxmoduletarball


###############################################
nginxtarball() {

if [[ "$NGINX_INSTALL" = [yY] ]]; 
then

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
    read -ep "Would you like to compile nginx with IPv6 support? [y/n] " asknginxipv
	else
	asknginxipv="$NGINX_IPV"
	fi #unattended

    cd $DIR_TMP

        cecho "Download nginx-${NGINX_VERSION}.tar.gz ..." $boldyellow
    if [ -s nginx-${NGINX_VERSION}.tar.gz ]; then
        cecho "nginx-${NGINX_VERSION}.tar.gz found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP "http://nginx.org/download/nginx-${NGINX_VERSION}.tar.gz" $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-${NGINX_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf nginx-${NGINX_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nginx-${NGINX_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "nginx-${NGINX_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

ngxmoduletarball

if [ "$NGINX_OPENRESTY" == 'y' ]; then

        cecho "Download memc-nginx-module_v0.13rc3.tar.gz ..." $boldyellow
if [ -s memc-nginx-module_v0.13rc3.tar.gz ]; then
  cecho "memc-nginx-module_v0.13rc3.tar.gz Archive found, skipping download..." $boldgreen
  else
  cecho "Error: memc-nginx-module_v0.13rc3.tar.gz not found!!!download now......"
        wget -c --progress=bar --no-check-certificate https://github.com/agentzh/memc-nginx-module/tarball/v0.13rc3 -O memc-nginx-module_v0.13rc3.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memc-nginx-module_v0.13rc3.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf memc-nginx-module_v0.13rc3.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memc-nginx-module_v0.13rc3.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "memc-nginx-module_v0.13rc3.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download srcache-nginx-module_v0.16.tar.gz ..." $boldyellow
if [ -s srcache-nginx-module_v0.16.tar.gz ]; then
  cecho "srcache-nginx-module_v0.16.tar.gz Archive found, skipping download..." $boldgreen
  else
  cecho "Error: srcache-nginx-module_v0.16.tar.gz not found!!!download now......"
        wget -c --progress=bar --no-check-certificate https://github.com/agentzh/srcache-nginx-module/tarball/v0.16 -O srcache-nginx-module_v0.16.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: srcache-nginx-module_v0.16.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf srcache-nginx-module_v0.16.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: srcache-nginx-module_v0.16.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "srcache-nginx-module_v0.16.tar.gz valid file." $boldyellow
echo ""
	fi
	
fi # openresty
fi # NGINX INSTALL

}

###############################################
libeventtarball() {

    cd $DIR_TMP

        cecho "Download release-${LIBEVENT_VERSION}-stable.tar.gz ..." $boldyellow
if [ -s release-${LIBEVENT_VERSION}-stable.tar.gz ]; then
  cecho "release-${LIBEVENT_VERSION}-stable.tar.gz found, skipping download..." $boldgreen
  else
  echo "Error: release-${LIBEVENT_VERSION}-stable.tar.gz not found !!! Download now......"
        wget -cnv --progress=bar --no-check-certificate https://github.com/libevent/libevent/archive/release-${LIBEVENT_VERSION}-stable.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: release-${LIBEVENT_VERSION}-stable.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf release-${LIBEVENT_VERSION}-stable.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: release-${LIBEVENT_VERSION}-stable.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "release-${LIBEVENT_VERSION}-stable.tar.gz valid file." $boldyellow
echo ""
	fi

}

###############################################
imagickphpexttarball() {

    cd $DIR_TMP

        cecho "Download imagick-${IMAGICKPHP_VER}.tgz ..." $boldyellow
if [ -s imagick-${IMAGICKPHP_VER}.tgz ]; then
  cecho "imagick-${IMAGICKPHP_VER}.tgz found, skipping download..." $boldgreen
  else
  echo "Error: imagick-${IMAGICKPHP_VER}.tgz not found !!! Download now......"
        wget -cnv --progress=bar http://pecl.php.net/get/imagick-${IMAGICKPHP_VER}.tgz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: imagick-${IMAGICKPHP_VER}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf imagick-${IMAGICKPHP_VER}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: imagick-${IMAGICKPHP_VER}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "imagick-${IMAGICKPHP_VER}.tgz valid file." $boldyellow
echo ""
	fi
}

###############################################
memcacheexttarball() {

    cd $DIR_TMP

        cecho "Download memcache-${MEMCACHE_VERSION}.tgz ..." $boldyellow
if [ -s memcache-${MEMCACHE_VERSION}.tgz ]; then
  cecho "memcache-${MEMCACHE_VERSION}.tgz found, skipping download..." $boldgreen
  else
  echo "Error: memcache-${MEMCACHE_VERSION}.tgz not found !!! Download now......"
        wget -cnv --progress=bar http://pecl.php.net/get/memcache-${MEMCACHE_VERSION}.tgz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcache-${MEMCACHE_VERSION}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf memcache-${MEMCACHE_VERSION}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcache-${MEMCACHE_VERSION}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "memcache-${MEMCACHE_VERSION}.tgz valid file." $boldyellow
echo ""
	fi
}

###############################################
memcachedphptarball() {

    cd $DIR_TMP

        cecho "Download memcached-${MEMCACHEDPHP_VER}.tgz ..." $boldyellow
if [ -s memcached-${MEMCACHEDPHP_VER}.tgz ]; then
  cecho "memcached-${MEMCACHEDPHP_VER}.tgz found, skipping download..." $boldgreen
  else
  echo "Error: memcached-${MEMCACHEDPHP_VER}.tgz not found !!! Download now......"
        wget -cnv --progress=bar http://pecl.php.net/get/memcached-${MEMCACHEDPHP_VER}.tgz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcached-${MEMCACHEDPHP_VER}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf memcached-${MEMCACHEDPHP_VER}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcached-${MEMCACHEDPHP_VER}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "memcached-${MEMCACHEDPHP_VER}.tgz valid file." $boldyellow
echo ""
	fi
}

###############################################
libmemcachedtarball() {

    cd $DIR_TMP

        cecho "Download libmemcached-${LIBMEMCACHED_VER}.tar.gz ..." $boldyellow
if [ -s libmemcached-${LIBMEMCACHED_VER}.tar.gz ]; then
  cecho "libmemcached-${LIBMEMCACHED_VER}.tar.gz found, skipping download..." $boldgreen
  else
  echo "Error: libmemcached-${LIBMEMCACHED_VER}.tar.gz not found !!! Download now......"
        wget -cnv --progress=bar https://launchpad.net/libmemcached/1.0/${LIBMEMCACHED_VER}/+download/libmemcached-${LIBMEMCACHED_VER}.tar.gz --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: libmemcached-${LIBMEMCACHED_VER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf libmemcached-${LIBMEMCACHED_VER}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: libmemcached-${LIBMEMCACHED_VER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "libmemcached-${LIBMEMCACHED_VER}.tar.gz valid file." $boldyellow
echo ""
	fi
}

###############################################
twemperftarball() {

    cd $DIR_TMP

        cecho "Download mcperf-${TWEMPERF_VER}.tar.gz ..." $boldyellow
if [ -s mcperf-${TWEMPERF_VER}.tar.gz ]; then
  cecho "mcperf-${TWEMPERF_VER}.tar.gz found, skipping download..." $boldgreen
  else
  echo "Error: mcperf-${TWEMPERF_VER}.tar.gz not found !!! Download now......"
        #wget -cnv --progress=bar --no-check-certificate https://github.com/twitter/twemperf/archive/v${TWEMPERF_VER}.tar.gz -O mcperf-${TWEMPERF_VER}.tar.gz --tries=3 

wget -cnv --progress=bar https://twemperf.googlecode.com/files/mcperf-${TWEMPERF_VER}.tar.gz --tries=3

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: mcperf-${TWEMPERF_VER}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

if [ ! -d "twemperf-${TWEMPERF_VER}" ]; then
tar xzf mcperf-${TWEMPERF_VER}.tar.gz
fi

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: mcperf-${TWEMPERF_VER}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "mcperf-${TWEMPERF_VER}.tar.gz valid file." $boldyellow
echo ""
	fi

}

###############################################
memcachetarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Memcached Server? (default uses 16MB RAM each) [y/n] " askmemcinstall
	else
	askmemcinstall='y'
	fi #unattended
if [[ "$askmemcinstall" = [yY] ]];
then
MEMCINSTALL='y'

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install how many Memcached servers ? 1 or 2 ?: " nomemcachedservers
echo "will install $nomemcachedservers Memcached servers"
echo ""
	else
	nomemcachedservers='1'
	fi #unattended

libeventtarball

#########################################################
        cecho "Download memcached-${MEMCACHED_VERSION}.tar.gz ..." $boldyellow
if [ -s memcached-${MEMCACHED_VERSION}.tar.gz ]; then
  cecho "memcached-${MEMCACHED_VERSION}.tar.gz found, skipping download..." $boldgreen
  else
  echo "Error: memcached-${MEMCACHED_VERSION}.tar.gz not found !!! Download now......"
  		# wget -cnv --progress=bar http://www.memcached.org/files/memcached-${MEMCACHED_VERSION}.tar.gz --tries=3
        wget -cnv --progress=bar http://centminmod.com/centminmodparts/memcached/memcached-${MEMCACHED_VERSION}.tar.gz --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcached-${MEMCACHED_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf memcached-${MEMCACHED_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: memcached-${MEMCACHED_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "memcached-${MEMCACHED_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

#########################################################
memcacheexttarball
memcachedphptarball
libmemcachedtarball
twemperftarball
#########################################################

fi

}

###############################################
mysqltools() {

    cd /root

        cecho "Download mysqlreport ..." $boldyellow
    if [ -s mysqlreport ]; then
        cecho "mysqlreport found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://centminmod.com/centminmodparts/mysqlreport/mysqlreport $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: mysqlreport download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download mysqltuner.pl ..." $boldyellow
    if [ -s mysqltuner.pl ]; then
        cecho "mysqltuner.pl found, skipping download..." $boldgreen
    else
        wget -cnv --progress=bar http://centminmod.com/centminmodparts/mysqltuner/mysqltuner.txt -O mysqltuner.pl --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: mysqltuner.pl download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

    chmod +x mysqlreport mysqltuner.pl

}

###############################################
mariadbrpms() {


if [[ "$MDB_INSTALL" = [yY] ]]; 
then

    # The MariaDB mirror uses x86 and amd64 prefixes for rpm folders instead of i386/x84_64, so compensate for that...
    if [ ${ARCH} == 'x86_64' ];
    then
        MDB_ARCH='amd64'
    else
        MDB_ARCH='x86'
        ARCH='i386'
    fi

    cd $DIR_TMP

        cecho "Download MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download  MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm ..." $boldyellow
    if [ -s MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

fi

}

###############################################
mariadbyumrepo() {

if [[ "$MDB_YUMREPOINSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download ourdelta-release-5-1.noarch.rpm ..." $boldyellow
    if [ -s ourdelta-release-5-1.noarch.rpm ]; then
        cecho "ourdelta-release-5-1.noarch.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://master.ourdelta.org/yum/CentOS-MariaDB52/ourdelta-release-5-1.noarch.rpm $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ourdelta-release-5-1.noarch.rpm download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

fi

}


###############################################
gperftools() {

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download libunwind-${LIBUNWIND_VERSION}.tar.gz ..." $boldyellow
    if [ -s libunwind-${LIBUNWIND_VERSION}.tar.gz ]; then
        cecho "libunwind ${LIBUNWIND_VERSION} found, skipping download..." $boldgreen 
    else
        $DOWNLOADAPP http://download.savannah.gnu.org/releases/libunwind/libunwind-${LIBUNWIND_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: libunwind-${LIBUNWIND_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf libunwind-${LIBUNWIND_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: libunwind-${LIBUNWIND_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "libunwind-${LIBUNWIND_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download google-perftools-${GPERFTOOLS_VERSION}.tar.gz ..." $boldyellow
    if [ -s google-perftools-${GPERFTOOLS_VERSION}.tar.gz ]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://google-perftools.googlecode.com/files/google-perftools-${GPERFTOOLS_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: google-perftools-${GPERFTOOLS_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf google-perftools-${GPERFTOOLS_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: google-perftools-${GPERFTOOLS_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "google-perftools-${GPERFTOOLS_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
openssldownload() {

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then
    cd $DIR_TMP

        cecho "Download openssl-${OPENSSL_VERSION}.tar.gz ..." $boldyellow
    if [ -s openssl-${OPENSSL_VERSION}.tar.gz ]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: openssl-${OPENSSL_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf openssl-${OPENSSL_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: openssl-${OPENSSL_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "openssl-${OPENSSL_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi # for nginx openssl

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
    # Install OpenSSL
    cd $DIR_TMP

        cecho "Download openssl-${OPENSSL_VERSION}.tar.gz ..." $boldyellow
    if [ -s openssl-${OPENSSL_VERSION}.tar.gz ]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: openssl-${OPENSSL_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf openssl-${OPENSSL_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: openssl-${OPENSSL_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "openssl-${OPENSSL_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi # openssl centos 6

}

###############################################
pcretarball() {

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download pcre-${PCRE_VERSION}.tar.gz ..." $boldyellow
    if [ -s pcre-${PCRE_VERSION}.tar.gz ]; then
        cecho "pcre ${PCRE_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-${PCRE_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: pcre-${PCRE_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf pcre-${PCRE_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: pcre-${PCRE_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "pcre-${PCRE_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
siegetarball() {
#SIEGE_VERSION

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Siege Benchmark script ? [y/n] " asksiegeinstall
	else
	asksiegeinstall='y'
	fi #unattended
if [[ "$asksiegeinstall" = [yY] ]];
then
SIEGEINSTALL='y'

    cd $DIR_TMP

        cecho "Download siege-${SIEGE_VERSION}.tar.gz ..." $boldyellow
    if [ -s siege-${SIEGE_VERSION}.tar.gz ]; then
        cecho "siege-${SIEGE_VERSION}.tar.gz found, skipping download..." $boldgreen
    else
        # $DOWNLOADAPP http://www.joedog.org/pub/siege/siege-${SIEGE_VERSION}.tar.gz $WGETRETRY
        $DOWNLOADAPP http://centminmod.com/centminmodparts/siege/siege-${SIEGE_VERSION}.tar.gz $WGETRETRY
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: siege-${SIEGE_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf siege-${SIEGE_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: siege-${SIEGE_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "siege-${SIEGE_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

        cecho "Download sproxy-latest.tar.gz ..." $boldyellow
    if [ -s sproxy-latest.tar.gz ]; then
        cecho "sproxy-latest found, skipping download..." $boldgreen
    else
        # $DOWNLOADAPP http://www.joedog.org/pub/sproxy/sproxy-latest.tar.gz $WGETRETRY
        $DOWNLOADAPP http://centminmod.com/centminmodparts/siege/sproxy-latest.tar.gz $WGETRETRY
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: sproxy-latest.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""

	fi
    fi

tar xzf sproxy-latest.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: sproxy-latest.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "sproxy-latest.tar.gz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
pythontarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Python Update ? [y/n] " askpythonupdate
	else
	askpythonupdate='n'
	fi #unattended
if [[ "$askpythonupdate" = [yY] ]];
then
PYTHONUPDATE='y'

    cd $DIR_TMP

        cecho "Download Python-${PYTHON_VERSION}.tgz ..." $boldyellow
if [ -s Python-${PYTHON_VERSION}.tgz ]; then
  cecho "Python-${PYTHON_VERSION}.tgz found, skipping download..." $boldgreen
  else
  echo "Error: Python-${PYTHON_VERSION}.tgz not found !!! Download now......"
        $DOWNLOADAPP http://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Python-${PYTHON_VERSION}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf Python-${PYTHON_VERSION}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: Python-${PYTHON_VERSION}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Python-${PYTHON_VERSION}.tgz valid file." $boldyellow
echo ""
	fi

        cecho "Download setuptools-0.6c11-py2.7.egg ..." $boldyellow
if [ -s setuptools-0.6c11-py2.7.egg ]; then
  cecho "setuptools-0.6c11-py2.7.egg found, skipping download..." $boldgreen
  else
  echo "Error: setuptools-0.6c11-py2.7.egg not found !!! Download now......"
        wget -c --no-check-certificate http://pypi.python.org/packages/2.7/s/setuptools/setuptools-0.6c11-py2.7.egg --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "setuptools-0.6c11-py2.7.egg download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
echo ""
	fi
fi

#test egg file validity check

fi

}

###############################################
xcachetarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install XCache? (By default uses 32MB RAM) If XCache installed DO NOT install APC [y/n] " askxcacheinstall
	else
	askxcacheinstall='n'
	fi #unattended

# menu #8 install xcache
if [[ "$MANXCACHEINSTALL" = 'y' ]]; then
	askxcacheinstall='y'
fi

if [[ "$askxcacheinstall" = [yY] ]];then
XCACHESRCINSTALL='y'

    cd $DIR_TMP

        cecho "Download xcache-${XCACHE_VERSION}.tar.gz ..." $boldyellow
    if [ -s xcache-${XCACHE_VERSION}.tar.gz ]; then
        cecho "xcache ${XCACHE_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://xcache.lighttpd.net/pub/Releases/${XCACHE_VERSION}/xcache-${XCACHE_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: xcache-${XCACHE_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf xcache-${XCACHE_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: xcache-${XCACHE_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "xcache-${XCACHE_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
apctarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
	else
	askapcinstall='y'
	fi #unattended
if [[ "$askapcinstall" = [yY] ]]; then
APCINSTALL='y'
    cd $DIR_TMP

        cecho "Download APC-${APCCACHE_VERSION}.tgz ..." $boldyellow
    if [ -s APC-${APCCACHE_VERSION}.tgz ]; then
        cecho "APC-${APCCACHE_VERSION}.tgz Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://pecl.php.net/get/APC-${APCCACHE_VERSION}.tgz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: APC-${APCCACHE_VERSION}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf APC-${APCCACHE_VERSION}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: APC-${APCCACHE_VERSION}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "APC-${APCCACHE_VERSION}.tgz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
zopcachetarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
	else
	askzopcacheinstall='y'
	fi #unattended
if [[ "$askzopcacheinstall" = [yY] ]]; then
ZOPCACHEINSTALL='y'
    cd $DIR_TMP

        cecho "Download zendopcache-${ZOPCACHECACHE_VERSION}.tgz ..." $boldyellow
    if [ -s zendopcache-${ZOPCACHECACHE_VERSION}.tgz ]; then
        cecho " zendopcache-${ZOPCACHECACHE_VERSION}.tgz Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://pecl.php.net/get/zendopcache-${ZOPCACHECACHE_VERSION}.tgz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: zendopcache-${ZOPCACHECACHE_VERSION}.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf zendopcache-${ZOPCACHECACHE_VERSION}.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: zendopcache-${ZOPCACHECACHE_VERSION}.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "zendopcache-${ZOPCACHECACHE_VERSION}.tgz valid file." $boldyellow
echo ""
	fi

fi

}

###############################################

nsdtarball() {
if [[ "$NSD_INSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download nsd-${NSD_VERSION}.tar.gz ..." $boldyellow
    if [ -s nsd-${NSD_VERSION}.tar.gz ]; then
        cecho "NSD ${NSD_VERSION} Archive found, skipping download..." $boldgreen 
    else
        $DOWNLOADAPP http://centminmod.com/centminmodparts/nsd/nsd-${NSD_VERSION}.tar.gz  $WGETRETRY
        #$DOWNLOADAPP http://www.nlnetlabs.nl/downloads/nsd/nsd-${NSD_VERSION}.tar.gz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nsd-${NSD_VERSION}.tar.gz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf nsd-${NSD_VERSION}.tar.gz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: nsd-${NSD_VERSION}.tar.gz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "nsd-${NSD_VERSION}.tar.gz valid file." $boldyellow
echo ""
	fi

fi
}


###############################################
csftarball() {

	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install CSF firewall script ? [y/n] " askcsfinstall
	else
	askcsfinstall='y'
	fi #unattended
if [[ "$askcsfinstall" = [yY] ]];
then

CSFINSTALLOK='y'

    cd $DIR_TMP

        cecho "Download csf.tgz ..." $boldyellow
    if [ -s csf.tgz ]; then
        cecho "csf Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://www.configserver.com/free/csf.tgz $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: csf.tgz download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf csf.tgz 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: csf.tgz extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "csf.tgz valid file." $boldyellow
echo ""
	fi

fi

}
###############################################

alldownloads() {
echo "*****************************************************"
cecho "Downloading all required software for install routines." $boldgreen
echo "*****************************************************"

yuminstall

questions

# yum double check
if [[ "$FAILEPEL" != 'y' || "$FAILRPMFORGE" != 'y' || "$FAILCENTALT" != 'y' ]]; then

ccacheinstall

csftarball

siegetarball

pythontarball

nginxtarball

xcachetarball

apctarball

zopcachetarball

memcachetarball

imagickphpexttarball

gperftools

openssldownload

pcretarball

phptarball

mariadbrpms

#mariadbyumrepo

mysqltools

nsdtarball

echo "*****************************************************"
cecho "Downloads complete." $boldgreen
echo "*****************************************************"

	if [ "$UNATTENDED" == 'n' ]; then
echo ""
read -ep "You you want to continue installation ?  [y/n] " downloadcontinstall

if [[ "$downloadcontinstall" = [nN] ]]; then
exit
fi
	fi #unattended

else
echo ""
echo "Error installing or downloading YUM repositories..."
checklogdetails
exit

fi # yum double check

}