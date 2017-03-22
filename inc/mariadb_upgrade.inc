checklogdetails() {
	echo "check Centmin Mod log for details at /root/centminlogs/"
	echo "Aborting script..."
}

function funct_mariadbupgrade {

    echo "*************************************************"
    cecho "* Upgrading MariaDB ${MDB_VERONLY}-${MDB_BUILD} " $boldgreen
    echo "*************************************************"

# for mariadb 5.2.13 removed pbxt
sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
sed -i 's/skip-pbxt_statistics/#skip-pbxt_statistics/g' /etc/my.cnf

echo "Where do you want the downloads stored ? Enter path to download directory (i.e. /usr/local/src) "
read downloadpath

    cd $downloadpath

    # The MariaDB mirror uses x86 and amd64 prefixes for rpm folders instead of i386/x84_64, so compensate for that...
    if [ ${ARCH} == 'x86_64' ];
    then
        MDB_ARCH='amd64'     
    else
        MDB_ARCH='x86'
        ARCH='i386'
    fi

    if [ -s MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY && status=ok || status=fail
	if [ "$status" == 'fail' ]; then
	cecho "Error: MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

    if [ -s MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY && status=ok || status=fail
	if [ "$status" == 'fail' ]; then
	cecho "Error: MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

    if [ -s MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY && status=ok || status=fail
	if [ "$status" == 'fail' ]; then
	cecho "Error: MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

    if [ -s MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY && status=ok || status=fail
	if [ "$status" == 'fail' ]; then
	cecho "Error: MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

    if [ -s MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm ]; then
        cecho "MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms/MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm $WGETRETRY && status=ok || status=fail
	if [ "$status" == 'fail' ]; then
	cecho "Error: MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm download failed." $boldgreen
checklogdetails
	exit
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
    fi

yum -y remove mysql mysql-libs

OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`

rpm -Uvh MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm
rpm -Uvh MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm
rpm -Uvh MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm
#rpm --nodeps -ev MariaDB-server-${MDB_VERSION}.el5
rpm -e $OLDMARIADBSERVER
rpm -Uvh MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm
rpm -Uvh MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm

funct_timestamp

}
