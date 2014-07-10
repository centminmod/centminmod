mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb55install() {

echo "MariaDB 5.5 Repo setup..."
echo ""

# CENTOSVER=$(cat /etc/redhat-release | awk '{ print $3 }')

# if [ "$CENTOSVER" == 'release' ]; then
#     CENTOSVER=$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1,2)
# fi

# if [ "$CENTOSVER" == 'Enterprise' ]; then
#     CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
#     OLS='y'
# fi

MARIADB55ARCH=`uname -m`

echo "rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB"
rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB

###############################################################################
if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '7.0' ]]; then

	if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos6-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

	else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos6-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

	fi # arch check
fi # centos6 check
###############################################################################

###############################################################################
if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' ]]; then

	if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos5-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

	else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos5-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

	fi # arch check
fi # centos5 check
###############################################################################

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo ""
    echo "Check for existing mysql-server packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mysql-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e -nodeps $OLDMYSQLSERVER"
        rpm -e -nodeps $OLDMYSQLSERVER
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo ""
    echo "Check for existing mariadb packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mariadb' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e -nodeps $OLDMYSQLSERVER"
        rpm -e -nodeps $OLDMYSQLSERVER
    fi
fi # CENTOS_SEVEN != 7

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
    DISABLECENTALTREPO=" --disablerepo=CentALT"
else
    DISABLECENTALTREPO=""
fi # CENTALTREPO_DISABLE

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    # for CentOS 7.x and excluding default mariadb 
    # opting for mariadb official yum repo instead
    if [[ ! `grep exclude /etc/yum.conf` ]]; then
        cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for mariadb*" $boldgreen
        echo "exclude=mariadb*">> /etc/yum.conf
    fi
fi # CENTOS_SEVEN = 7

###############################################################################
echo ""
echo "MariaDB 5.5 YUM install..."

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo "yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly$DISABLECENTALTREPO install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared"
    time yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly$DISABLECENTALTREPO install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    # run for CentOS 7.x
    echo "yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly$DISABLECENTALTREPO --disablerepo=* --enablerepo=mariadb install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared"
    time yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly$DISABLECENTALTREPO --disablerepo=* --enablerepo=mariadb install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared
fi # CENTOS_SEVEN != 7

#cp -a /etc/my.cnf /etc/my.cnf-newold
#cp -f /etc/my.cnf.rpmorig /etc/my.cnf
sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
###############################################################################
echo ""

mysqltmpdir

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    mysqlinitfix
fi # CENTOS_SEVEN != 7

cmservice mysql restart
cmservice mysql stop
# remove default innodb tablespace and log files that are incompatible with
# MariaDB 5.5 included my.cnf used
gzip /var/lib/mysql/{ibdata1,ib_logfile0,ib_logfile1}
cmservice mysql start

	if [ "$UNATTENDED" == 'nn' ]; then
echo ""
echo "------------------------------------------------"
echo "Updating MySQL database tables"
echo "------------------------------------------------"
read -ep "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
echo ""

if [[ "$mysqlrootset" == [yY] ]]; then
echo "Enter MySQL root password when prompted"
echo "mysql_upgrade -u root -p"
mysql_upgrade -u root -p
else
echo "mysql_upgrade"
mysql_upgrade
fi
	fi #unattended
echo ""

yum${CACHESKIP} -q -y install perl-DBD-MySQL

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
# make sure MariaDB 5.5 is updated from only one repo
echo ""
if [[ -z "$(grep 'exclude=mariadb' /etc/yum.repos.d/centalt.repo)" ]]; then
echo "exclude=mariadb*" >> /etc/yum.repos.d/centalt.repo
fi
fi # CENTALTREPO_DISABLE

# exclude MariaDB Galera Server
if [[ -z "$(grep 'exclude=MariaDB-Galera-server' /etc/yum.repos.d/mariadb.repo)" ]]; then
echo "exclude=MariaDB-Galera-server" >> /etc/yum.repos.d/mariadb.repo
fi

}

mariadbinstallfunct() {

MDB_DEBUG='n'

if [[ "$MDB_INSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Installing MariaDB " $boldgreen
    echo "*************************************************"

cmservice mysqld stop

yum${CACHESKIP} -y remove mysql mysql-libs

if [ ! -d /home/mysqltmp ]; then
mkdir /home/mysqltmp
chmod 1777 /home/mysqltmp
CHOWNMYSQL=y
fi

if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original
fi
#echo -e "\nCopying MariaDB my-mdb.cnf file to /etc/my.cnf\n"
#    cp $CUR_DIR/config/mysql/my-mdb.cnf /etc/my.cnf

    # The MariaDB mirror uses x86 and amd64 prefixes for rpm folders instead of i386/x84_64, so compensate for that...
    if [ ${ARCH} == 'x86_64' ];
    then
        MDB_ARCH='amd64'
    else
        MDB_ARCH='x86'
        ARCH='i386'
    fi

    cd $DIR_TMP

if [[ "$MDB_DEBUG" = [yY] ]]; then
exit
fi

yum${CACHESKIP} -y install perl-DBD-MySQL

cmservice mysqld stop

#yum -y remove mysql mysql-libs

rpm -e --nodeps $(rpm -qa mysql-libs)

echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessmem

#echo
#echo

if [[ "$lessmem" = [yY] ]]; then

echo $lessmem

echo -e "\nCopying MariaDB my-mdb-min.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb-min.cnf /etc/my.cnf

else

echo $lessmem

echo -e "\nCopying MariaDB my-mdb.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb.cnf /etc/my.cnf

fi

#download mariadb rpms

echo "rpm -i MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -i MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm
echo "rpm -i MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -i MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm
echo "rpm -i MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -i MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm

#killall mysqld_safe
#killall mysqld
gzip /var/lib/mysql/{ibdata1,ib_logfile0,ib_logfile1}

echo "rpm -i MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -i MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm

# check MariaDB server installed properly
cmservice mysql status

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	echo ""
	echo "MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm not installed properly"
	exit
	echo ""
	fi

cecho "Ensure mysql system directory has correct permissions" $boldgreen
cecho " chown -R mysql:mysql /var/lib/mysql/mysql/" $boldgreen
chown -R mysql:mysql /var/lib/mysql/mysql/

#echo "rpm -i MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm"
    #rpm -i MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm

    yum${CACHESKIP} -y install perl-DBD-MySQL
    yum${CACHESKIP} -q clean all

#download mysqltuner mysqlreport

    cd $DIR_TMP

   # chkconfig --levels 235 mysql on
   # cmservice mysql restart

if [[ ! `grep exclude /etc/yum.conf` ]]; then

cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for mysql*" $boldgreen

echo "exclude=mysql*">> /etc/yum.conf

else

cecho "exclude line exists... adding exclude line for mysql*" $boldgreen

sed -i "s/exclude=\*.i386 \*.i586 \*.i686/exclude=\*.i386 \*.i586 \*.i686 mysql\*/" /etc/yum.conf

fi

    echo "*************************************************"
    cecho "* Starting MariaDB Server Installation" $boldgreen
    echo "*************************************************"

mysql_upgrade --force --verbose

mysqlproclimit

cmservice mysql restart
#mysql_secure_installation
    
    echo "*************************************************"
    cecho "* MariaDB installed" $boldgreen
    echo "*************************************************"
    
    /etc/init.d/mysql stop
fi

if [[ "$MDB_YUMREPOINSTALL" = [yY] ]]; 
then

    echo "*************************************************"
    cecho "* Installing MariaDB via CentOS Yum Repo " $boldgreen
    echo "*************************************************"

#yum -y remove mysql mysql-libs
if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original
rm rf /etc/my.cnf
fi

mariadb55install

if [[ "$lessmem" = [yY] ]]; then

echo $lessmem

echo -e "\nCopying MariaDB my-mdb55-min.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb55-min.cnf /etc/my.cnf

else

echo $lessmem

echo -e "\nCopying MariaDB my-mdb55.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb55.cnf /etc/my.cnf

fi

if [[ ! `grep exclude /etc/yum.conf` ]]; then

cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for mysql*" $boldgreen

echo "exclude=mysql*">> /etc/yum.conf

else

cecho "exclude line exists... adding exclude line for mysql*" $boldgreen

sed -i "s/exclude=\*.i386 \*.i586 \*.i686/exclude=\*.i386 \*.i586 \*.i686 mysql\*/" /etc/yum.conf

fi

    echo "*************************************************"
    cecho "* Starting MariaDB Secure Installation" $boldgreen
    echo "*************************************************"

mysql_upgrade --force --verbose
cmservice mysql restart
#mysql_secure_installation
  
    echo "*************************************************"
    cecho "* MariaDB installed" $boldgreen
    echo "*************************************************"
    
    cmservice mysql stop
fi

}