backupmsg102() {

echo ""
cat <<EOF
-------------------------------------------------------------------------------
 This script's MariaDB 10.2.x upgrade process is still in beta testing stages. 
 So please test on a test server and not a live production server. But if you use 
 on live server or if MySQL databases already exist, please back them up before 
 running this upgrade script via mysqldump - instructions at: 
 http://dev.mysql.com/doc/refman/5.6/en/mysqldump.html.

 ###################################################################
 ###          IMPORTANT MYSQLDUMP BACKUP BEFORE UPGRADE          ###
 ###        OR RISK DATA LOSS DUE TO MARIADB 10.2 Changes        ###
 ###################################################################

 For each DATABASENAME backup via mysqldump i.e.

  mysqldump --opt --routines DATABASENAME > /path/to/DATABASENAME_backup.sql
EOF
sleep 5

echo "------------------------------------------------------------------------"
echo ""
read -ep "Have you backed up your databases ? [y/n] " askbackup

if [[ "$askbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.2.x upgrade..." $boldyellow
exit
fi

echo ""
echo "MariaDB 10.2.x https://mariadb.com/kb/en/mariadb/what-is-mariadb-102/"
echo 
echo "!!  Note  !!"
echo "upgrade process can take more >5 minutes.. you may want to shutdown"
echo "nginx server to not show mysql errors to visitors on your site or" 
echo "put site(s) into maintenance mode before proceeding with mysql upgrade"
sleep 15
echo
read -ep "Do you want to continue with upgrade to MariaDB 10.2.x ? [y/n] " contbackup

if [[ "$contbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.2.x upgrade..." $boldyellow
exit
fi

    # if you set /etc/centminmod/custom_config.inc file to
    # MARIADB_UPDATEMAINTENANCE=y
    # then whenever you run mariadb upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$MARIADB_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus off
        fi
    fi

echo
echo "checking mysql root password works in /root/.my.cnf"
sleep 5
echo "mysqladmin ver"
mysqladmin ver

CHECK_MYSQLPASS=$?
if [[ "$CHECK_MYSQLPASS" != '0' ]]; then
    echo
    echo "mysql root user password in /root/.my.cnf is not correct"
    if [ ! -f /root/.my.cnf ]; then
        echo "or"
        echo "/root/.my.cnf file doesn't exist"
    fi
    cecho "Aborting MariaDB 10.2.x upgrade..." $boldyellow
    exit
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Preparing Shutdown For Upgrade"
    fi

# full purge and change buffer merge before shutting down to ensure data files are in sync 
# prior to upgrade
if [[ "$SKIP_INNODBFASTSHUTDOWNDISABLE" = [nN] ]]; then
echo
mysql -e "set global innodb_fast_shutdown=0;"
echo "continuing upgrade in ~300 seconds..."
echo "..."
sleep 20
echo "..... continue in 300s"
sleep 20
echo "....... continue in 280s"
sleep 20
echo "......... continue in 260s"
sleep 20
echo "........... continue in 240s"
sleep 20
echo "............. continue in 220s"
sleep 20
echo "............... continue in 200s"
sleep 20
echo "................. continue in 180s"
sleep 20
echo ".................. continue in 160s"
sleep 20
echo ".................... continue in 140s"
sleep 20
echo "...................... continue in 120s"
sleep 20
echo "........................ continue in 80s"
sleep 20
echo ".......................... continue in 60s"
sleep 20
echo "............................ continue in 40s"
sleep 20
echo ".............................. continue in 20s"
sleep 20
fi
}

mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb102uninstall() {
OLDMARIADBVERS=`rpm -qa | grep MariaDB`

echo "#####################################################"

# List current existing MariaDB versions for RPMs
echo "Current MariaDB versions installed:"
for mrpm in $OLDMARIADBVERS
do
echo $mrpm
done

echo "#####################################################"

echo "Proceeding to remove existing MariaDB versions:"
echo "please wait while processing...."

OLDMARIADBSHARED=`rpm -qa | grep MariaDB | awk '/MariaDB-shared/'`
OLDMARIADBCLIENT=`rpm -qa | grep MariaDB | awk '/MariaDB-client/'`
OLDMARIADBDEVEL=`rpm -qa | grep MariaDB | awk '/MariaDB-devel/'`
OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`
OLDMARIADBTEST=`rpm -qa | grep MariaDB | awk '/MariaDB-test/'`
OLDMARIADBCOMMON=`rpm -qa | grep MariaDB | awk '/MariaDB-common/'`
OLDMARIADBCOMPAT=`rpm -qa | grep MariaDB | awk '/MariaDB-compat/'`

yum -q -y remove perl-DBD-MySQL

echo "rpm -e $OLDMARIADBTEST"
echo "rpm -e $OLDMARIADBSERVER"
echo "rpm -e $OLDMARIADBCLIENT"
echo "rpm -e $OLDMARIADBDEVEL"
echo "rpm -e --nodeps $OLDMARIADBSHARED"
echo "rpm -e --nodeps $OLDMARIADBCOMPAT"
echo "rpm -e --nodeps $OLDMARIADBCOMMON"

rpm -e $OLDMARIADBTEST
rpm -e $OLDMARIADBSERVER
rpm -e $OLDMARIADBCLIENT
rpm -e $OLDMARIADBDEVEL
rpm -e --nodeps $OLDMARIADBSHARED
rpm -e --nodeps $OLDMARIADBCOMPAT
rpm -e --nodeps $OLDMARIADBCOMMON

yum clean all

if [ -z "$OLDMARIADBVERS" ]; then
echo "MariaDB old versions successfully removed..."
fi

echo "#####################################################"

}

mariadb102install() {

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.2.x Repo Setup"
    fi

    echo "*************************************************"
    cecho "MariaDB 10.2.x Repo setup..." $boldgreen
   echo "*************************************************"
echo ""

yum clean all

CENTOSVER=`cat /etc/redhat-release | awk '{ print $3 }'`

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $7 }'`
fi

MARIADB55ARCH=`uname -m`

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

    if [ "$(uname -m)" == 'aarch64' ]; then

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.2/centos7-aarch64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.2/centos7-aarch64
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
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.2/centos7-amd64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.2/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.2/centos7-amd64
gpgkey=https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum//RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.2/centos7-amd64
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
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.2/centos6-amd64
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
baseurl = http://yum.mariadb.org/10.2/centos6-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = https://mirrors.tuna.tsinghua.edu.cn/mariadb/yum/10.2/centos6-x86
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
baseurl = http://yum.mariadb.org/10.2/centos6-x86
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
baseurl = http://yum.mariadb.org/10.2/centos5-amd64
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
baseurl = http://yum.mariadb.org/10.2/centos5-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

    fi # arch check
fi # centos5 check

}

mariadbtentotentwoupgrade() {
#check if MariaDB 10.1 RPM already installed
if [ -z "$OLDMARIADBVERS" ]; then
MYSQLDATADIR=$(mysqladmin var | grep 'datadir' | awk '{ print $4}')

backupmsg102
#mariadb102uninstall

# shutdown existing mysql server
echo
echo "shut down existing MariaDB server for upgrade.."
mysql -e "set global innodb_buffer_pool_dump_at_shutdown=0;"
sleep 10
service mysql stop
echo
# move aside existing ib_logfile* files
cd $MYSQLDATADIR
ls -lah $MYSQLDATADIR
rm -rf ib_logfile0.gz ib_logfile1.gz
mv ib_logfile0 ib_logfile0.bak
mv ib_logfile1 ib_logfile1.bak
echo

OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`
rpm -e $OLDMARIADBSERVER

mariadb102install

if [[ "1$CENTALTREPO_DISABLE" = [nN] ]]; then
    DISABLECENTALTREPO=" --disablerepo=CentALT"
else
    DISABLECENTALTREPO=""
fi # CENTALTREPO_DISABLE

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

if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original_before_mariadb102upgrade
fi
echo ""
    echo "*************************************************"
cecho "MariaDB 10.2.x check available packages" $boldgreen
cecho "yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities" $boldgreen
    echo "*************************************************"
echo ""
time yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.2.x Install"
    fi

echo ""
    echo "*************************************************"
cecho "MariaDB 10.2.x YUM install..." $boldgreen
cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities" $boldgreen
    echo "*************************************************"
echo ""
time yum -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities

\cp -af /etc/my.cnf /etc/my.cnf-newold-mariadb10-2
echo ""
    echo "*************************************************"
    cecho "* Adjust old MariaDB 10.x /etc/my.cnf " $boldyellow
    cecho "  for MariaDB 10.2.x my.cnf compatibility " $boldyellow
    echo "*************************************************"

# unalias cp
if [ -f /etc/my.cnf.rpmorig ]; then
\cp -f /etc/my.cnf.rpmsave /etc/my.cnf
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.2.x /etc/my.cnf Adjustments"
    fi

cat >>/etc/my.cnf<<FFF


[mariadb-10.2]
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

# sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
# sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
# sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
# sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
# sed -i 's/innodb_additional_mem_pool_size/#innodb_additional_mem_pool_size/g' /etc/my.cnf
# sed -i 's/skip-innodb/innodb=ON/g' /etc/my.cnf
sed -i 's|ignore-db-dir|ignore_db_dirs|g' /etc/my.cnf
sed -i 's|^innodb_thread_concurrency|#innodb_thread_concurrency|g' /etc/my.cnf
sed -i 's|^skip-federated|#skip-federated|g' /etc/my.cnf
sed -i 's|^skip-pbxt|#skip-pbxt|g' /etc/my.cnf
sed -i 's|^skip-pbxt_statistics|#skip-pbxt_statistics|g' /etc/my.cnf
sed -i 's|^skip-archive|#skip-archive|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_dump_at_shutdown|#innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_load_at_startup|#innodb_buffer_pool_load_at_startup|g' /etc/my.cnf

echo ""

cmservice mysql start
sleep 15
cmservice mysql restart

if [[ "$(echo $?)" != '0' ]]; then
    echo
    echo "## !! mysql failed to start properly !! ##"
    echo "journalctl -xn"
    journalctl -xn
    echo
    echo "tail -50 /var/log/mysqld.log"
    tail -50 /var/log/mysqld.log
    echo
    echo "possible errors / warnings"
    tail -50 /var/log/mysqld.log | egrep -v 'Dumping|dump' | egrep --color '\[ERROR\]|\[Warning\]'
    echo
    echo "possible unknown or incompatible /etc/my.cnf set options below"
    echo "maybe need to comment out or remove these in /etc/my.cnf"
    UNKNOWNOPT=$(tail -50 /var/log/mysqld.log | egrep -v 'Dumping|dump' | egrep --color '\[ERROR\]|\[Warning\]' | grep 'unknown option' | awk '{print $8}' | uniq | sed -e "s|'--||g" -e "s|'||g")
    for u in $UNKNOWNOPT; do
        echo "$u"
    done
    echo
    echo "attempting auto commenting out of invalid options"
    \cp -af /etc/my.cnf /etc/my.cnf-b4autocommenting
    for u in $UNKNOWNOPT; do
        echo "auto comment out $u in /etc/my.cnf"
        sed -i "s|^${u}|#${u}|g" /etc/my.cnf
    done
    cmservice mysql restart
fi
echo
ls -lAhrt /var/lib/mysql/

sleep 3

if [ ! -f /root/.my.cnf ]; then
    echo ""
    echo "------------------------------------------------"
    echo "Updating MySQL database tables"
    echo "------------------------------------------------"
    read -p "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
    echo ""
else
    mysqlrootset=n
fi

if [[ "$mysqlrootset" == [yY] ]]; then
    echo "Enter MySQL root password when prompted"
    echo "or hit enter to continue"
    echo "mysql_upgrade -u root -p --verbose"
    mysql_upgrade -u root -p --verbose
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

    # echo "Enter MySQL root password when prompted"
    # echo "or hit enter to continue"
    # echo "mysql_upgrade -u root -p --force"
    # mysql_upgrade -u root -p --force

    # sed -i 's/innodb=ON/innodb=OFF/g' /etc/my.cnf
else
    echo "mysql_upgrade --verbose"
    mysql_upgrade --verbose
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

 #    echo "mysql_upgrade --force"
    # mysql_upgrade --force

    # sed -i 's/innodb=ON/innodb=OFF/g' /etc/my.cnf
fi

#   echo "------------------------------------------------"
#   echo "Enabling MariaDB 10 XtraDB Engine plugin"
#   echo "------------------------------------------------"
# sed -i 's/#plugin-load=ha_xtradb/plugin-load=ha_xtradb/g' /etc/my.cnf
# sed -i 's/#ignore_builtin_innodb/ignore_builtin_innodb/g' /etc/my.cnf
# sed -i 's/innodb_purge_threads=1/#innodb_purge_threads=1/g' /etc/my.cnf

# sed -i 's/innodb_doublewrite = 1/#innodb_doublewrite = 1/g' /etc/my.cnf
# sed -i 's/innodb_doublewrite=1/#innodb_doublewrite=1/g' /etc/my.cnf

# mariadb 10.1 systemd changes https://community.centminmod.com/posts/22696/
if [[ "$CENTOS_SEVEN" = '7' ]] ; then
    if [ -f /etc/systemd/system/mariadb.service.d/migrated-from-my.cnf-settings.conf ]; then
        rm -rf /etc/systemd/system/mariadb.service.d/migrated-from-my.cnf-settings.conf
cat > "/etc/systemd/system/mariadb.service.d/mdb101.conf" <<EFF
[Service]
OOMScoreAdjust=-600
LimitNOFILE=524288
LimitNPROC=16384
Nice=-5
EFF
    systemctl daemon-reload
    fi
fi

cmservice mysql restart
if [[ "$CENTOS_SEVEN" = '7' ]] ; then
    cmchkconfig mariadb on
else
    cmchkconfig mysql on
fi

sed -i 's|^#innodb_buffer_pool_dump_at_shutdown|innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
sed -i 's|^#innodb_buffer_pool_load_at_startup|innodb_buffer_pool_load_at_startup|g' /etc/my.cnf

echo ""

# yum -q -y install perl-DBD-MySQL

echo ""

mysqladmin ver

echo

    # if you set /etc/centminmod/custom_config.inc file to
    # MARIADB_UPDATEMAINTENANCE=y
    # then whenever you run mariadb upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$MARIADB_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus on
        fi
    fi

echo "------------------------------------------------"
echo "MariaDB 10.2.x Upgrade completed"
echo "------------------------------------------------"

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.2.x Upgraded"
    fi

fi
}