mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb55install() {

echo "MariaDB 10.x Repo setup..."
echo ""

# CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

# if [ "$CENTOSVER" == 'release' ]; then
#     CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
# fi

# if [ "$CENTOSVER" == 'Enterprise' ]; then
#     CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
#     OLS='y'
# fi

MARIADB55ARCH=`uname -m`

echo "rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB"
rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB

###############################################################################
if [[ "$CENTOS_SEVEN" = '7' ]]; then

    if [ "$(uname -m)" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF

    else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF

    fi # centos 7 uname -m

fi

###############################################################################
if [[ "$CENTOS_SIX" = '6' ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos6-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF

    else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos6-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF

    fi # arch check
fi # centos6 check

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' || "$CENTOSVER" == '5.11' || "$CENTOSVER" == '5.12' || "$CENTOSVER" == '5.13' ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos5-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF

    else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.0/centos5-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
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
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo ""
    echo "Check for existing mariadb packages"
    OLDMYSQLSERVER=`rpm -qa | grep 'mariadb-server' | head -n1`
    if [[ ! -z "$OLDMYSQLSERVER" ]]; then
        echo "rpm -e --nodeps $OLDMYSQLSERVER"
        rpm -e --nodeps $OLDMYSQLSERVER
    fi
    echo ""
    echo "Check for existing mariadb-libs package"
    OLDMYSQL_LIBS=`rpm -qa | grep 'mariadb-libs' | head -n1`
    if [[ ! -z "$OLDMYSQL_LIBS" ]]; then
        echo "rpm -e --nodeps $OLDMYSQL_LIBS"
        rpm -e --nodeps $OLDMYSQL_LIBS
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
echo "MariaDB 10 YUM install..."

if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEOTHERREPO=' --disablerepo=epel,rpmforge'
else
    DISABLEOTHERREPO=' --disablerepo=epel'
fi

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo ""
    echo "*************************************************"
    cecho "MariaDB 10.x YUM install..." $boldgreen
    cecho "yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
    echo ""
    time yum${CACHESKIP} -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared

    cp -a /etc/my.cnf /etc/my.cnf-newold
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    # run for CentOS 7.x
    echo "time yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install perl-DBI"
    time yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install perl-DBI

    echo ""
    echo "*************************************************"
    cecho "MariaDB 10.x YUM install..." $boldgreen
    cecho "yum${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
    echo ""
    time yum${CACHESKIP} -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared

    cp -a /etc/my.cnf /etc/my.cnf-newold
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

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "mysql -e \"INSTALL SONAME 'metadata_lock_info';\""
    mysql -e "INSTALL SONAME 'metadata_lock_info';"
    echo "mysql -e \"INSTALL SONAME 'query_cache_info';\""
    mysql -e "INSTALL SONAME 'query_cache_info';"
    echo "mysql -e \"INSTALL SONAME 'query_response_time';\""
    mysql -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "mysql -e \"INSTALL SONAME 'ha_xtradb';\""
    # mysql -e "INSTALL SONAME 'ha_xtradb';"
    echo "mysql -t -e \"SELECT * FROM mysql.plugin;\""
    mysql -t -e "SELECT * FROM mysql.plugin;"
    echo "mysql -t -e \"SHOW PLUGINS;\""
    mysql -t -e "SHOW PLUGINS;"
    echo "mysql -t -e \"SHOW ENGINES;\""
    mysql -t -e "SHOW ENGINES;"

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

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "mysql -u root -p -e \"INSTALL SONAME 'metadata_lock_info';\""
    mysql -u root -p -e "INSTALL SONAME 'metadata_lock_info';"
    echo "mysql -u root -p -e \"INSTALL SONAME 'query_cache_info';\""
    mysql -u root -p -e "INSTALL SONAME 'query_cache_info';"
    echo "mysql -u root -p -e \"INSTALL SONAME 'query_response_time';\""
    mysql -u root -p -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "mysql -u root -p -e \"INSTALL SONAME 'ha_xtradb';\""
    # mysql -u root -p -e "INSTALL SONAME 'ha_xtradb';"

    echo "mysql -u root -p -e \"SELECT * FROM mysql.plugin;\""
    mysql -u root -p -e "SELECT * FROM mysql.plugin;"
    echo "mysql -u root -p -e \"SHOW PLUGINS;\""
    mysql -u root -p -e "SHOW PLUGINS;"
    echo "mysql -u root -p -e \"SHOW ENGINES;\""
    mysql -u root -p -e "SHOW ENGINES;"
else
    echo "mysql_upgrade"
    mysql_upgrade

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "mysql -e \"INSTALL SONAME 'metadata_lock_info';\""
    mysql -e "INSTALL SONAME 'metadata_lock_info';"
    echo "mysql -e \"INSTALL SONAME 'query_cache_info';\""
    mysql -e "INSTALL SONAME 'query_cache_info';"
    echo "mysql -e \"INSTALL SONAME 'query_response_time';\""
    mysql -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "mysql -e \"INSTALL SONAME 'ha_xtradb';\""
    # mysql -e "INSTALL SONAME 'ha_xtradb';"
    echo "mysql -t -e \"SELECT * FROM mysql.plugin;\""
    mysql -t -e "SELECT * FROM mysql.plugin;"
    echo "mysql -t -e \"SHOW PLUGINS;\""
    mysql -t -e "SHOW PLUGINS;"
    echo "mysql -t -e \"SHOW ENGINES;\""
    mysql -t -e "SHOW ENGINES;"

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

setmycnf

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

echo -e "\nCopying MariaDB my-mdb10-min.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb10-min.cnf /etc/my.cnf

else

echo $lessmem

echo -e "\nCopying MariaDB my-mdb10.cnf file to /etc/my.cnf\n"
    cp $CUR_DIR/config/mysql/my-mdb10.cnf /etc/my.cnf

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
mjemalloc
cmservice mysql restart
#mysql_secure_installation

if [[ "$CENTOS_SEVEN" != '7' ]]; then
    # for CentOS 5.x and 6.x only
    # remove default innodb tablespace and log files that are incompatible with
    # MariaDB 5.5 / 10 included my.cnf used
    # not necessary for CentOS 7.x though
    cmservice mysql stop
    gzip /var/lib/mysql/{ib_logfile0,ib_logfile1}
    cmservice mysql start
else
    # only remove log files but leave innodb tablespace
    cmservice mysql stop
    gzip /var/lib/mysql/{ib_logfile0,ib_logfile1}
    cmservice mysql start
fi

# https://community.centminmod.com/posts/25691/
if [ -f /usr/local/src/centminmod/tools/setio.sh ]; then
    /usr/local/src/centminmod/tools/setio.sh set
fi  
    echo "*************************************************"
    cecho "* MariaDB installed" $boldgreen
    echo "*************************************************"
    
    cmservice mysql stop
fi

}