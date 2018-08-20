perdbiinstall() {
    # PERLDBICHECK=$(rpm -qa perl-DBD-MySQL)
    if [[ $(rpm -q perl-DBD-MySQL >/dev/null 2>&1; echo $?) != '0' ]]; then
        yum${CACHESKIP} -q -y install perl-DBD-MySQL
    fi
}

mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb103_yuminstall() {

echo "MariaDB 10.3 Repo setup..."
echo ""

MARIADB55ARCH=`uname -m`

echo "setup /var/log/mysqld.log"
touch /var/log/mysqld.log
chown mysql:mysql /var/log/mysqld.log
chmod 600 /var/log/mysqld.log
ls -lah /var/log/mysqld.log
echo

MDB_COUNTRY=$(curl -s https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | awk '/country: / {print $2}')

# MariaDB yum mirrors may have connectivity issues for servers in China (CN)
# so can use alternative mirrors for better speed and connectivity
if [[ "$MDB_COUNTRY" = 'CN' ]]; then
    echo "rpm --import https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB"
    rpm --import https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
else
    echo "rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB"
    rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
fi

###############################################################################
if [[ "$CENTOS_SEVEN" = '7' ]]; then

#     if [ "$(uname -m)" == 'aarch64' ]; then

# if [[ "$MDB_COUNTRY" = 'CN' ]]; then
# cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# [mariadb]
# name = MariaDB
# baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos7-amd64
# gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
# gpgcheck=1
# EOF
# else
# cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# [mariadb]
# name = MariaDB
# baseurl = http://yum.mariadb.org/10.1/centos7-amd64
# gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# gpgcheck=1
# EOF
# fi

    if [ "$(uname -m)" == 'aarch64' ]; then

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos7-aarch64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos7-aarch64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi
    fi

    if [ "$(uname -m)" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos7-amd64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos7-amd64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    fi # centos 7 uname -m

fi

###############################################################################
if [[ "$CENTOS_SIX" = '6' ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos6-amd64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos6-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.3/centos6-x86
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos6-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    fi # arch check
fi # centos6 check

if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos5-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

    else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos5-x86
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
        # echo "rpm -e --nodeps $OLDMYSQL_LIBS"
        # rpm -e --nodeps $OLDMYSQL_LIBS
        echo "yum -y remove mariadb-libs"
        yum -y remove mariadb-libs
    fi
    echo ""
    # Should not exist on CentOS 7 systems
    echo "Check for existing MySQL-shared-compat"
    OLDMYSQL_SHAREDCOMPAT=`rpm -qa | grep 'MySQL-shared-compat' | head -n1`
    if [[ ! -z "$OLDMYSQL_SHAREDCOMPAT" ]]; then
        echo "yum -y remove MySQL-shared-compat"
        yum -y remove MySQL-shared-compat
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

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.3 Yum Install"
    fi

echo "MariaDB 10.3 YUM install..."

if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEOTHERREPO=' --disablerepo=epel --disablerepo=rpmforge --disableplugin=priorities'
else
    DISABLEOTHERREPO=' --disablerepo=epel --disableplugin=priorities'
fi

# set /etc/my.cnf templates
setmycnf

sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
sed -i 's|ignore-db-dir|ignore_db_dirs|g' /etc/my.cnf
sed -i 's|^innodb_thread_concurrency|#innodb_thread_concurrency|g' /etc/my.cnf
sed -i 's|^skip-federated|#skip-federated|g' /etc/my.cnf
sed -i 's|^skip-pbxt|#skip-pbxt|g' /etc/my.cnf
sed -i 's|^skip-pbxt_statistics|#skip-pbxt_statistics|g' /etc/my.cnf
sed -i 's|^skip-archive|#skip-archive|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_dump_at_shutdown|#innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_load_at_startup|#innodb_buffer_pool_load_at_startup|g' /etc/my.cnf
# deprecated in 10.3
# https://mariadb.com/kb/en/library/mariadb-1031-release-notes/
sed -i '/^innodb_large_prefix/d' /etc/my.cnf
sed -i '/^innodb_file_format/d' /etc/my.cnf
sed -i '/^innodb_support_xa/d' /etc/my.cnf

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo ""
    echo "*************************************************"
    cecho "MariaDB 10.3.x YUM install..." $boldgreen
    cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}" $boldgreen
    echo "*************************************************"
    echo ""
    time yum -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}

    cp -a /etc/my.cnf /etc/my.cnf-newold
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    # run for CentOS 7.x
    echo "time ${YUMDNFBIN}${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install perl-DBI"
    time ${YUMDNFBIN}${CACHESKIP} -q -y --disableplugin=fastestmirror,downloadonly${DISABLECENTALTREPO}${DISABLEOTHERREPO} install perl-DBI

    echo ""
    echo "*************************************************"
    cecho "MariaDB 10.3.x YUM install..." $boldgreen
    cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}" $boldgreen
    echo "*************************************************"
    echo ""
    time time $YUMDNFBIN -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}

    cp -a /etc/my.cnf /etc/my.cnf-newold
fi # CENTOS_SEVEN != 7

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "/etc/my.cnf compat checks"
    fi

###############################################################################
echo ""

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.3 Specific /etc/my.cnf"
    fi

cat >>/etc/my.cnf<<FFF


[mariadb-10.3]
#innodb_file_format = Barracuda
innodb_default_row_format = dynamic
innodb_file_per_table = 1

## wsrep specific
# wsrep_on=OFF
# wsrep_provider
# wsrep_cluster_address
# binlog_format=ROW
# default_storage_engine=InnoDB
# innodb_autoinc_lock_mode=2
# innodb_doublewrite=1
# query_cache_size=0

# 2 variables needed to switch from XtraDB to InnoDB plugins
#plugin-load=ha_innodb
#ignore_builtin_innodb

## MariaDB 10 only save and restore buffer pool pages
## warm up InnoDB buffer pool on server restarts
innodb_buffer_pool_dump_at_shutdown=1
innodb_buffer_pool_load_at_startup=1
## Disabled settings
performance_schema=OFF
innodb_stats_on_metadata=OFF
innodb_sort_buffer_size=2M
innodb_online_alter_log_max_size=128M
query_cache_strip_comments=0
log_slow_filter =admin,filesort,filesort_on_disk,full_join,full_scan,query_cache,query_cache_miss,tmp_table,tmp_table_on_disk

# Defragmenting unused space on InnoDB tablespace
innodb_defragment=1
innodb_defragment_n_pages=7
innodb_defragment_stats_accuracy=0
innodb_defragment_fill_factor_n_recs=20
innodb_defragment_fill_factor=0.9
innodb_defragment_frequency=40

# page compression
# 10.1+ https://mariadb.com/kb/en/mariadb/compression/
#innodb_compression_algorithm=zlib
#innodb_use_fallocate=0
#innodb_use_mtflush=0
#innodb_mtflush_threads=4
FFF

echo ""

mysqltmpdir

echo "setup /var/log/mysqld.log"
touch /var/log/mysqld.log
chown mysql:mysql /var/log/mysqld.log
chmod 600 /var/log/mysqld.log
ls -lah /var/log/mysqld.log
echo

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    mysqlinitfix
fi # CENTOS_SEVEN != 7

if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/systemd/system/mariadb.service.d/openfileslimit.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for LimitNOFILE=524288"
cat > "/etc/systemd/system/mariadb.service.d/openfileslimit.conf" <<TDG
[Service]
LimitNOFILE=524288
TDG
  systemctl daemon-reload
fi

cmservice mysql restart

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10 Plugins"
    fi

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

perdbiinstall
# reinstall postfix removed from mariadb-libs removale
if [[ $(rpm -q postfix >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install postfix${DISABLEOTHERREPO} 
fi
if [[ $(rpm -q net-snmp >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install net-snmp${DISABLEOTHERREPO} 
fi
if [[ $(rpm -q mytop >/dev/null 2>&1; echo $?) != '0' ]]; then
    yum -y install mytop
fi

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

# setup mariadb mysql log rotation
funct_logmysqlrotate

}

mariadbtenthree_installfunct() {

MDB_DEBUG='n'

if [[ "$MDB_INSTALL" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Installing MariaDB " $boldgreen
    echo "*************************************************"

cmservice mysqld stop

${YUMDNFBIN}${CACHESKIP} -y remove mysql mysql-libs

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

perdbiinstall

cmservice mysqld stop

#yum -y remove mysql mysql-libs

rpm -e --nodeps $(rpm -qa mysql-libs) >/dev/null 2>&1

echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessmem

#echo
#echo

setmycnf

#download mariadb rpms

echo "rpm -Uvh MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -Uvh MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm
echo "rpm -Uvh MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -Uvh MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm
echo "rpm -Uvh MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -Uvh MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm

#killall mysqld_safe
#killall mysqld
gzip /var/lib/mysql/{ibdata1,ib_logfile0,ib_logfile1}

echo "rpm -Uvh MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm"
    rpm -Uvh MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm

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

#echo "rpm -Uvh MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm"
    #rpm -Uvh MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm

    yum ${CACHESKIP} -y install perl-DBD-MySQL
    yum ${CACHESKIP} -q clean all

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

if [[ "$MDB_YUMREPOINSTALL" = [yY] && "$MDB_INSTALL" = [nN] ]]; then

    echo "*************************************************"
    cecho "* Installing MariaDB via CentOS Yum Repo " $boldgreen
    echo "*************************************************"

#yum -y remove mysql mysql-libs
if [ -f /etc/my.cnf ]; then
    cp /etc/my.cnf /etc/my.cnf-original
    rm -rf /etc/my.cnf
fi

mariadb103_yuminstall

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "New Initial /etc/my.cnf"
    fi

echo "new initial /etc/my.cnf"
echo "================================================="
cat /etc/my.cnf
echo "================================================="
echo

# if [[ "$lessmem" = [yY] ]]; then
#     echo $lessmem
#     echo -e "\nCopying MariaDB my-mdb10-min.cnf file to /etc/my.cnf\n"
#     cp $CUR_DIR/config/mysql/my-mdb10-min.cnf /etc/my.cnf
# else
#     echo $lessmem
#     echo -e "\nCopying MariaDB my-mdb10.cnf file to /etc/my.cnf\n"
#     cp $CUR_DIR/config/mysql/my-mdb10.cnf /etc/my.cnf
# fi

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