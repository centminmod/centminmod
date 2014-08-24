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

if [ -s "${CENTOSFIVEEPELFILE}" ]; then
  echo "${CENTOSFIVEEPELFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEEPELFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEEPEL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEEPELFILE} download failed." $boldgreen
FAILEPEL='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm --import http://epel.mirror.constant.com/RPM-GPG-KEY-EPEL-5
	rpm -i ${CENTOSFIVEEPELFILE}
	yum${CACHESKIP} -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCEPELCHECK="$ERR"
		if [ "$CCEPELCHECK" == '0' ]; then
			yumpriorepel
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEEPELFILE} install." $boldgreen
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

if [ -s "${CENTOSFIVERPMFORGEFILE}" ]; then
  echo "${CENTOSFIVERPMFORGEFILE} [found]"
  else
  echo "Error: ${CENTOSFIVERPMFORGEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVERPMFORGE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVERPMFORGEFILE} download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	#exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i ${CENTOSFIVERPMFORGEFILE}
	yum${CACHESKIP} -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVERPMFORGEFILE} install." $boldgreen
			#exit $ERR
		fi
	fi

fi


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s ${CENTOSFIVECENTALTFILE} ]; then
  echo "${CENTOSFIVECENTALTFILE} [found]"
  else
  echo "Error: ${CENTOSFIVECENTALTFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVECENTALT} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVECENTALTFILE} download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -i ${CENTOSFIVECENTALTFILE}
yumpriorcentalt
echo ""
	fi

fi
fi # CENTALTREPO_DISABLE

if [ -s "${CENTOSFIVEPERCONARELEASEFILE}" ]; then
  echo "${CENTOSFIVEPERCONARELEASEFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEPERCONARELEASEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEPERCONARELEASE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEPERCONARELEASEFILE} download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -i ${CENTOSFIVEPERCONARELEASEFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCPERCONACHECK="$ERR"
		if [ "$CCPERCONACHECK" == '0' ]; then
			yumpriorpercona
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEPERCONARELEASEFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s "${CENTOSFIVEVARNISHFILE}" ]; then
  echo "${CENTOSFIVEVARNISHFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEVARNISHFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEVARNISH} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEVARNISHFILE} download failed." $boldgreen
FAILVARNISH='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow

rpm --nosignature -i ${CENTOSFIVEVARNISHFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCVARNISHCHECK="$ERR"
		if [ "$CCVARNISHCHECK" == '0' ]; then
			yumpriorvarnish
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEVARNISHFILE} install." $boldgreen
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
cecho "* CentOS 6.x detected installing EPEL Repo" $boldgreen
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

if [ -s "${CENTOSSIXEPELFILE}" ]; then
  echo "${CENTOSSIXEPELFILE} [found]"
  else
  echo "Error: ${CENTOSSIXEPELFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXEPEL} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXEPELLOCAL} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXEPELFILE} download failed." $boldgreen
FAILEPEL='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm --import http://epel.mirror.constant.com/RPM-GPG-KEY-EPEL-6
	rpm -i ${CENTOSSIXEPELFILE}
	yum${CACHESKIP} -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCEPELCHECK="$ERR"
		if [ "$CCEPELCHECK" == '0' ]; then
			yumpriorepel
			echo ""
		else
			cecho "Error: problem with ${CENTOSSIXEPELFILE} install." $boldgreen
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

if [ -s "${CENTOSSIXRPMFORGEFILE}" ]; then
  echo "${CENTOSSIXRPMFORGEFILE} [found]"
  else
  echo "Error: ${CENTOSSIXRPMFORGEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXRPMFORGE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXRPMFORGEFILE} download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	#exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i ${CENTOSSIXRPMFORGEFILE}
	yum${CACHESKIP} -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with ${CENTOSSIXRPMFORGEFILE} install." $boldgreen
			#exit $ERR
		fi
	fi

fi


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s "${CENTOSSIXCENTALTFILE}" ]; then
  echo "${CENTOSSIXCENTALTFILE} [found]"
  else
  echo "Error: ${CENTOSSIXCENTALTFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXCENTALT} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXCENTALTLOCAL} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXCENTALTFILE} download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -i ${CENTOSSIXCENTALTFILE}
yumpriorcentalt
echo ""
	fi

fi
fi # CENTALTREPO_DISABLE


if [ -s "${CENTOSSIXPERCONALRELEASEFILE}" ]; then
  echo "${CENTOSSIXPERCONALRELEASEFILE} [found]"
  else
  echo "Error: ${CENTOSSIXPERCONALRELEASEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXPERCONALRELEASE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXPERCONALRELEASEFILE} download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -i ${CENTOSSIXPERCONALRELEASEFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCPERCONACHECK="$ERR"
		if [ "$CCPERCONACHECK" == '0' ]; then
			yumpriorpercona
			echo ""
		else
			cecho "Error: problem with ${CENTOSSIXPERCONALRELEASEFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s "${CENTOSSIXVARNISHFILE}" ]; then
  echo "${CENTOSSIXVARNISHFILE} [found]"
  else
  echo "Error: ${CENTOSSIXVARNISHFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXVARNISH} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXVARNISHLOCAL} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXVARNISHFILE} download failed." $boldgreen
FAILVARNISH='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow

rpm --nosignature -i ${CENTOSSIXVARNISHFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCVARNISHCHECK="$ERR"
		if [ "$CCVARNISHCHECK" == '0' ]; then
			yumpriorvarnish
			echo ""
		else
			cecho "Error: problem with ${CENTOSSIXVARNISHFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

####
# axivo centos 6.x 64bit only

if [[ "$AXIVOREPO_DISABLE" = [nN] ]]; then

if [ ${MACHINE_TYPE} == 'x86_64' ]; then

if [ -s "${CENTOSSIXAXIVOFILE}" ]; then
  echo "${CENTOSSIXAXIVOFILE} [found]"
  else
  echo "Error: ${CENTOSSIXAXIVOFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSSIXAXIVO} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXAXIVOLOCAL} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSSIXAXIVOFILE} download failed." $boldgreen
FAILAXIVO='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm -ivh --nosignature ${CENTOSSIXAXIVOFILE}
	yum${CACHESKIP} -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCAXIVOCHECK="$ERR"
		if [ "$CCAXIVOCHECK" == '0' ]; then
			yumprioraxivo
			echo ""
		else
			cecho "Error: problem with ${CENTOSSIXAXIVOFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

fi

fi # AXIVOREPO_DISABLE

####


# CentOS 6 repo mirrors aren't fully updated so need to specify actual kernel-headers version
PACKAGEEXTRA='libJudy axel glances bash-completion mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake28'

declare -a arrays
arrays=(${PACKAGEEXTRA})

for pkgs in ${arrays[@]}
do
echo Processing package: $pkgs
YUMINSTALLRPMCHECKS=$(rpm -qa $pkgs)
#echo $YUMINSTALLRPMCHECKS
        if [ "$YUMINSTALLRPMCHECKS" ]; then
echo "----------------------------------------------------------------------------------"
        # yum${CACHESKIP} -q -y update $pkgs
        # yum${CACHESKIP} -q list $pkgs
        echo "$pkg already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgs" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum${CACHESKIP} -q -y install $pkgs
        # yum${CACHESKIP} -q list $pkgs
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
        $DOWNLOADAPP ${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.gz/from/this/mirror $AXELPHPTARGZ $WGETRETRY
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

        cecho "Download ${PCRELINKFILE} ..." $boldyellow
    if [ -s ${PCRELINKFILE} ]; then
        cecho "${PCRELINKFILE} Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar ${PCRELINK} --tries=3 
        # wget -c --progress=bar ${PCRELINKLOCAL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PCRELINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${PCRELINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PCRELINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PCRELINKFILE} valid file." $boldyellow
echo ""
	fi
}

nginxwebdavtarball() {

    cd $DIR_TMP

        cecho "Download ${NGX_WEBDAVLINKFILE} ..." $boldyellow
    if [ -s ${NGX_WEBDAVLINKFILE} ]; then
        cecho "${NGX_WEBDAVLINKFILE} Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate ${NGX_WEBDAVLINK} -O ${NGX_WEBDAVLINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_WEBDAVLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_WEBDAVLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_WEBDAVLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_WEBDAVLINKFILE} valid file." $boldyellow
echo ""
	fi
}

nginxpgspeedtarball() {

	if [[ "$NGINX_PAGESPEED" = [yY] ]]; then

    cd $DIR_TMP

    # official github master zip renamed
    if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then

            cecho "Download ${NGX_PAGESPEEDGITLINKFILE} ..." $boldyellow
    		if [ -s ${NGX_PAGESPEEDGITLINKFILE} ]; then
        	cecho "${NGX_PAGESPEEDGITLINKFILE} Archive found, skipping download..." $boldgreen
    else
    	    wget -c --progress=bar --no-check-certificate ${NGX_PAGESPEEDGITLINK} -O ${NGX_PAGESPEEDGITLINKFILE} --tries=3 
			ERROR=$?
				if [[ "$ERROR" != '0' ]]; then
					cecho "Error: ${NGX_PAGESPEEDGITLINKFILE} download failed." $boldgreen
					checklogdetails
					exit #$ERROR
				else 
         			cecho "Download done." $boldyellow
					#echo ""
				fi
    fi

	fi # NGINX_PAGESPEEDGITMASTER

        cecho "Download ${NGX_PAGESPEEDLINKFILE} ..." $boldyellow
    if [ -s ${NGX_PAGESPEEDLINKFILE} ]; then
        cecho "${NGX_PAGESPEEDLINKFILE} Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate ${NGX_PAGESPEEDLINK} -O ${NGX_PAGESPEEDLINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_PAGESPEEDLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_PAGESPEEDLINKFILE}

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_PAGESPEEDLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_PAGESPEEDLINKFILE} valid file." $boldyellow
echo ""
	fi

if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
	\cp -Rf ngx_pagespeed-master ngx_pagespeed-release-${NGXPGSPEED_VER}
	rm -rf ngx_pagespeed-master
	rm -rf ${NGX_PAGESPEEDGITLINKFILE}
fi

        cecho "Download ${NGX_PAGESPEEDPSOLINKLFILE} ..." $boldyellow
    if [ -s ${NGX_PAGESPEEDPSOLINKLFILE} ]; then
        cecho "${NGX_PAGESPEEDPSOLINKLFILE} Archive found, skipping download..." $boldgreen
    else
    cd $DIR_TMP/ngx_pagespeed-release-${NGXPGSPEED_VER}
        wget -c --progress=bar --no-check-certificate ${NGX_PAGESPEEDPSOLINKL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_PAGESPEEDPSOLINKLFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_PAGESPEEDPSOLINKLFILE}
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_PAGESPEEDPSOLINKLFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_PAGESPEEDPSOLINKLFILE} valid file." $boldyellow
    cd $DIR_TMP
echo ""
	fi

	fi # NGINX_PAGESPEED=y

}

ngxmoduletarball() {
### nginx modules ###

    cd $DIR_TMP

        cecho "Download ${NGX_FANCYINDEXLINKFILE} ..." $boldyellow
    if [ -s ${NGX_FANCYINDEXLINKFILE} ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar ${NGX_FANCYINDEXLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_FANCYINDEXLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_FANCYINDEXLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_FANCYINDEXLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_FANCYINDEXLINKFILE} valid file." $boldyellow
echo ""
	fi

if [ ! -d ${DIR_TMP}/ngx-fancyindex-ngx-fancyindex ]; then
        cecho "Download ${NGX_FANCYINDEXLINKFILE} ..." $boldyellow
rm -rf ${NGX_FANCYINDEXLINKFILE}

    if [ -s ${NGX_FANCYINDEXLINKFILE} ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar ${NGX_FANCYINDEXLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_FANCYINDEXLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi
fi #fancyindex recheck

tar xzf ${NGX_FANCYINDEXLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_FANCYINDEXLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_FANCYINDEXLINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${NGX_CACHEPURGEFILE} ..." $boldyellow
    if [ -s ${NGX_CACHEPURGEFILE} ]; then
        cecho "ngx_cache_purge 2.1 Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${NGX_CACHEPURGE} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_CACHEPURGEFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_CACHEPURGEFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_CACHEPURGEFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_CACHEPURGEFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${NGX_ACCESSKEYLINKFILE} ..." $boldyellow
    if [ -s ${NGX_ACCESSKEYLINKFILE} ]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        wget -c ${NGX_ACCESSKEYLINK} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_ACCESSKEYLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_ACCESSKEYLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_ACCESSKEYLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_ACCESSKEYLINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${NGX_CONCATLINKFILE} ..." $boldyellow
    if [ -s ${NGX_CONCATLINKFILE} ]; then
        cecho "${NGX_CONCATLINKFILE} Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate ${NGX_CONCATLINK} -O ${NGX_CONCATLINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_CONCATLINKFILE} download failed." $boldgreen

checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_CONCATLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_CONCATLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_CONCATLINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${NGX_HEADERSMORELINKFILE} ..." $boldyellow
    if [ -s ${NGX_HEADERSMORELINKFILE} ]; then
        cecho "${NGX_HEADERSMORELINKFILE} Archive found, skipping download..." $boldgreen
    else
        wget -c --progress=bar --no-check-certificate ${NGX_HEADERSMORELINK} -O ${NGX_HEADERSMORELINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_HEADERSMORELINKFILE} download failed." $boldgreen

checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_HEADERSMORELINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_HEADERSMORELINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_HEADERSMORELINKFILE} valid file." $boldyellow
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

        cecho "Download ${NGX_LINKFILE} ..." $boldyellow
    if [ -s ${NGX_LINKFILE} ]; then
        cecho "${NGX_LINKFILE} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP "${NGX_LINK}" $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_LINKFILE} valid file." $boldyellow
echo ""
	fi

ngxmoduletarball

if [ "$NGINX_OPENRESTY" == 'y' ]; then

        cecho "Download ${NGX_MEMCLINKFILE} ..." $boldyellow
if [ -s ${NGX_MEMCLINKFILE} ]; then
  cecho "${NGX_MEMCLINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_MEMCLINKFILE} not found!!!download now......"
        wget -c --progress=bar --no-check-certificate ${NGX_MEMCLINK} -O ${NGX_MEMCLINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_MEMCLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_MEMCLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_MEMCLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_MEMCLINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${NGX_SRCACHELINKFILE} ..." $boldyellow
if [ -s ${NGX_SRCACHELINKFILE} ]; then
  cecho "${NGX_SRCACHELINKFILE} Archive found, skipping download..." $boldgreen
  else
  cecho "Error: ${NGX_SRCACHELINKFILE} not found!!!download now......"
        wget -c --progress=bar --no-check-certificate ${NGX_SRCACHELINK} -O ${NGX_SRCACHELINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_SRCACHELINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NGX_SRCACHELINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NGX_SRCACHELINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NGX_SRCACHELINKFILE} valid file." $boldyellow
echo ""
	fi
	
fi # openresty
fi # NGINX INSTALL

}

###############################################
libeventtarball() {

    cd $DIR_TMP

        cecho "Download ${LIBEVENTLINKFILE} ..." $boldyellow
if [ -s ${LIBEVENTLINKFILE} ]; then
  cecho "${LIBEVENTLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${LIBEVENTLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar --no-check-certificate ${LIBEVENTLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${LIBEVENTLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${LIBEVENTLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${LIBEVENTLINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${LIBEVENTLINKFILE} valid file." $boldyellow
echo ""
	fi

}

###############################################
imagickphpextlocaltarball() {

    cd $DIR_TMP
    rm -rf imagick*

        cecho "Download local ${PHPEXT_IMAGICKLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_IMAGICKLINKFILE} ]; then
  cecho "${PHPEXT_IMAGICKLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_IMAGICKLINKFILE} local not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_IMAGICKLINKLOCAL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_IMAGICKLINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_IMAGICKLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_IMAGICKLINKFILE} local extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PHPEXT_IMAGICKLINKFILE} local valid file." $boldyellow
echo ""
	fi
}

###############################################
imagickphpexttarball() {

    cd $DIR_TMP

        cecho "Download ${PHPEXT_IMAGICKLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_IMAGICKLINKFILE} ]; then
  cecho "${PHPEXT_IMAGICKLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_IMAGICKLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_IMAGICKLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_IMAGICKLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_IMAGICKLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_IMAGICKLINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	imagickphpextlocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${PHPEXT_IMAGICKLINKFILE} valid file." $boldyellow
echo ""
	fi
}

###############################################
memcacheextlocaltarball() {

    cd $DIR_TMP
    rm -rf memcache*

        cecho "Download local ${PHPEXT_MEMCACHELINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_MEMCACHELINKFILE} ]; then
  cecho "${PHPEXT_MEMCACHELINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHELINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHELINKLOCAL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHELINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_MEMCACHELINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHELINKFILE} local extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHELINKFILE} local valid file." $boldyellow
echo ""
	fi
}

###############################################
memcacheexttarball() {

    cd $DIR_TMP

        cecho "Download ${PHPEXT_MEMCACHELINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_MEMCACHELINKFILE} ]; then
  cecho "${PHPEXT_MEMCACHELINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHELINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHELINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHELINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_MEMCACHELINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHELINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	memcacheextlocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHELINKFILE} valid file." $boldyellow
echo ""
	fi
}

###############################################
memcachedphplocaltarball() {

    cd $DIR_TMP
    rm -rf memcached*

        cecho "Download local ${PHPEXT_MEMCACHEDLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_MEMCACHEDLINKFILE} ]; then
  cecho "${PHPEXT_MEMCACHEDLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHEDLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHEDLINKLOCAL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_MEMCACHEDLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} local extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHEDLINKFILE} local valid file." $boldyellow
echo ""
	fi
}

###############################################
memcachedphptarball() {

    cd $DIR_TMP

        cecho "Download ${PHPEXT_MEMCACHEDLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_MEMCACHEDLINKFILE} ]; then
  cecho "${PHPEXT_MEMCACHEDLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_MEMCACHEDLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_MEMCACHEDLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_MEMCACHEDLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_MEMCACHEDLINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	memcachedphplocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${PHPEXT_MEMCACHEDLINKFILE} valid file." $boldyellow
echo ""
	fi
}

###############################################
libmemcachedlocaltarball() {

if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then

    cd $DIR_TMP

        cecho "Download local ${PHPEXT_LIBMEMCACHEDLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_LIBMEMCACHEDLINKFILE} ]; then
  cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} local found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_LIBMEMCACHEDLINKLOCAL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_LIBMEMCACHEDLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} local extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} local valid file." $boldyellow
echo ""
	fi
fi #LIBMEMCACHED_YUM
}

###############################################
libmemcachedtarball() {

if [[ "$LIBMEMCACHED_YUM" = [nN] ]]; then

    cd $DIR_TMP

        cecho "Download ${PHPEXT_LIBMEMCACHEDLINKFILE} ..." $boldyellow
if [ -s ${PHPEXT_LIBMEMCACHEDLINKFILE} ]; then
  cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} not found !!! Download now......"
        wget ${WGETOPT} --progress=bar ${PHPEXT_LIBMEMCACHEDLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PHPEXT_LIBMEMCACHEDLINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PHPEXT_LIBMEMCACHEDLINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	libmemcachedlocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${PHPEXT_LIBMEMCACHEDLINKFILE} valid file." $boldyellow
echo ""
	fi
fi #LIBMEMCACHED_YUM
}

###############################################
twemperftarball() {

    cd $DIR_TMP

        cecho "Download ${TWEMPERF_LINKFILE} ..." $boldyellow
if [ -s ${TWEMPERF_LINKFILE} ]; then
  cecho "${TWEMPERF_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${TWEMPERF_LINKFILE} not found !!! Download now......"
        
     wget ${WGETOPT} --progress=bar --no-check-certificate ${TWEMPERF_LINK} -O ${TWEMPERF_LINKFILE} --tries=3 

	# wget ${WGETOPT} --progress=bar ${TWEMPERF_LINK} --tries=3

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${TWEMPERF_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

if [ ! -d "twemperf-${TWEMPERF_VER}" ]; then
tar xzf ${TWEMPERF_LINKFILE}
fi

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${TWEMPERF_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${TWEMPERF_LINKFILE} valid file." $boldyellow
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
        cecho "Download ${MEMCACHEDSERVER_LINKFILE} ..." $boldyellow
if [ -s ${MEMCACHEDSERVER_LINKFILE} ]; then
  cecho "${MEMCACHEDSERVER_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${MEMCACHEDSERVER_LINKFILE} not found !!! Download now......"
  		wget ${WGETOPT} --progress=bar ${MEMCACHEDSERVER_LINK} --tries=3
        # wget ${WGETOPT} --progress=bar http://memcached.googlecode.com/files/${MEMCACHEDSERVER_LINKFILE} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${MEMCACHEDSERVER_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${MEMCACHEDSERVER_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${MEMCACHEDSERVER_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${MEMCACHEDSERVER_LINKFILE} valid file." $boldyellow
echo ""
	fi

#########################################################
memcacheexttarball
memcachedphptarball
libmemcachedtarball
# twemperftarball
#########################################################

fi

}

###############################################
mysqltools() {

    cd /root

        cecho "Download ${MYSQLREPORT_LINKFILE} ..." $boldyellow
    if [ -s ${MYSQLREPORT_LINKFILE} ]; then
        cecho "${MYSQLREPORT_LINKFILE} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${MYSQLREPORT_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${MYSQLREPORT_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${MYSQLREPORT_LINKFILE} done." $boldyellow
#echo ""
	fi
    fi

        cecho "Download ${MYSQLTUNER_LINKFILE} ..." $boldyellow
    if [ -s ${MYSQLTUNER_LINKFILE} ]; then
        cecho "${MYSQLTUNER_LINKFILE} found, skipping download..." $boldgreen
    else
        wget ${WGETOPT} --progress=bar ${MYSQLTUNER_LINK} -O ${MYSQLTUNER_LINKFILE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${MYSQLTUNER_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${MYSQLTUNER_LINKFILE} done." $boldyellow
#echo ""
	fi
    fi

    chmod +x mysqlreport ${MYSQLTUNER_LINKFILE}

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

        cecho "Download ${OPENSSL_LINKFILE} ..." $boldyellow
    if [ -s ${OPENSSL_LINKFILE} ]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${OPENSSL_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${OPENSSL_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${OPENSSL_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${OPENSSL_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${OPENSSL_LINKFILE} valid file." $boldyellow
echo ""
	fi

fi # for nginx openssl

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
    # Install OpenSSL
    cd $DIR_TMP

        cecho "Download ${OPENSSL_LINKFILE} ..." $boldyellow
    if [ -s ${OPENSSL_LINKFILE} ]; then
        cecho "openssl ${OPENSSL_VERSION} found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${OPENSSL_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${OPENSSL_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${OPENSSL_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${OPENSSL_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${OPENSSL_LINKFILE} valid file." $boldyellow
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

        cecho "Download ${SIEGE_LINKFILE} ..." $boldyellow
    if [ -s ${SIEGE_LINKFILE} ]; then
        cecho "${SIEGE_LINKFILE} found, skipping download..." $boldgreen
    else
        # $DOWNLOADAPP ${SIEGE_LINK} $WGETRETRY
        $DOWNLOADAPP ${SIEGE_LINKLOCAL} $WGETRETRY
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${SIEGE_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${SIEGE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${SIEGE_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${SIEGE_LINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${SPROXY_LINKFILE} ..." $boldyellow
    if [ -s ${SPROXY_LINKFILE} ]; then
        cecho "sproxy-latest found, skipping download..." $boldgreen
    else
        # $DOWNLOADAPP ${SPROXY_LINK} $WGETRETRY
        $DOWNLOADAPP http://centminmod.com/centminmodparts/siege/${SPROXY_LINKFILE} $WGETRETRY
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${SPROXY_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""

	fi
    fi

tar xzf ${SPROXY_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${SPROXY_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${SPROXY_LINKFILE} valid file." $boldyellow
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

        cecho "Download ${PYTHON_LINKFILE} ..." $boldyellow
if [ -s ${PYTHON_LINKFILE} ]; then
  cecho "${PYTHON_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_LINKFILE} not found !!! Download now......"
        $DOWNLOADAPP http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "${PYTHON_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PYTHON_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PYTHON_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PYTHON_LINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} ..." $boldyellow
if [ -s ${PYTHON_SETUPTOOLSLINKFILE} ]; then
  cecho "${PYTHON_SETUPTOOLSLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_SETUPTOOLSLINKFILE} not found !!! Download now......"
        wget -c --no-check-certificate ${PYTHON_SETUPTOOLSLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "${PYTHON_SETUPTOOLSLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} done." $boldyellow
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

        cecho "Download ${XCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${XCACHE_LINKFILE} ]; then
        cecho "xcache ${XCACHE_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${XCACHE_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${XCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${XCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${XCACHE_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${XCACHE_LINKFILE} valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
apclocaltarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
	else
	askapcinstall='y'
	fi #unattended
if [[ "$askapcinstall" = [yY] ]]; then
APCINSTALL='y'
    cd $DIR_TMP

    rm -rf APC*

        cecho "Download local ${APCCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${APCCACHE_LINKFILE} ]; then
        cecho "${APCCACHE_LINKFILE} Archive (local) found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${APCCACHE_LINKLOCAL} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${APCCACHE_LINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${APCCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${APCCACHE_LINKFILE} (local) extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${APCCACHE_LINKFILE} (local) valid file." $boldyellow
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

        cecho "Download ${APCCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${APCCACHE_LINKFILE} ]; then
        cecho "${APCCACHE_LINKFILE} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${APCCACHE_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${APCCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${APCCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${APCCACHE_LINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	apclocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${APCCACHE_LINKFILE} valid file." $boldyellow
echo ""
	fi

fi

}

###############################################
zopcachelocaltarball() {

echo ""
	if [ "$UNATTENDED" == 'n' ]; then
read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
	else
	askzopcacheinstall='y'
	fi #unattended
if [[ "$askzopcacheinstall" = [yY] ]]; then
ZOPCACHEINSTALL='y'
    cd $DIR_TMP
    rm -rf zendopcache*

        cecho "Download local ${ZENDOPCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${ZENDOPCACHE_LINKFILE} ]; then
        cecho " ${ZENDOPCACHE_LINKFILE} local Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${ZENDOPCACHE_LINKLOCAL} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} local download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download local done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${ZENDOPCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} local extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${ZENDOPCACHE_LINKFILE} local valid file." $boldyellow
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

        cecho "Download ${ZENDOPCACHE_LINKFILE} ..." $boldyellow
    if [ -s ${ZENDOPCACHE_LINKFILE} ]; then
        cecho " ${ZENDOPCACHE_LINKFILE} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${ZENDOPCACHE_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${ZENDOPCACHE_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${ZENDOPCACHE_LINKFILE} extraction failed." $boldgreen
	cecho "trying local mirror download..." $boldgreen
	zopcachelocaltarball
# checklogdetails
# 	exit #$ERROR
else 
         cecho "${ZENDOPCACHE_LINKFILE} valid file." $boldyellow
echo ""
	fi

fi

}

###############################################

nsdtarball() {
if [[ "$NSD_INSTALL" = [yY] ]]; 
then

    cd $DIR_TMP

        cecho "Download ${NSD_LINKFILE} ..." $boldyellow
    if [ -s ${NSD_LINKFILE} ]; then
        cecho "NSD ${NSD_VERSION} Archive found, skipping download..." $boldgreen 
    else
        # $DOWNLOADAPP ${NSD_LINKLOCAL} $WGETRETRY
        $DOWNLOADAPP ${NSD_LINK} $WGETRETRY
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NSD_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${NSD_LINKFILE} done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${NSD_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${NSD_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${NSD_LINKFILE} valid file." $boldyellow
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

        cecho "Download ${CSF_LINKFILE} ..." $boldyellow
    if [ -s ${CSF_LINKFILE} ]; then
        cecho "csf Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ${CSF_LINK} $WGETRETRY 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CSF_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${CSF_LINKFILE} done." $boldyellow
#echo ""
	fi
    fi

tar xzf ${CSF_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CSF_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${CSF_LINKFILE} valid file." $boldyellow
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

phptarball

xcachetarball

apctarball

zopcachetarball

memcachetarball

imagickphpexttarball

gperftools

openssldownload

pcretarball

mysqltools

nsdtarball

mariadbrpms

#mariadbyumrepo

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