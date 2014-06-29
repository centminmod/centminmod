backupmsg() {

echo ""
cat <<EOF
--------------------------------------------------------------------------------
 This script's MariaDB 10.x upgrade process is still in 
 beta testing stages. So please test on a test server 
 and not a live production server. But if you use on 
 live server or if MySQL databases already exist, 
 please back them up before running this upgrade 
 script via mysqldump - instructions at: 
 http://dev.mysql.com/doc/refman/5.6/en/mysqldump.html.

 For each DATABASENAME backup via mysqldump i.e.

  mysqldump --opt --routines DATABASENAME > /path/to/DATABASENAME_backup.sql
EOF

echo "--------------------------------------------------------------------------------"
echo ""
read -ep "Have you backed up your databases ? [y/n] " askbackup

if [[ "$askbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.x upgrade..." $boldyellow
exit
fi

echo ""
echo "MariaDB 10.x no longer works with mydumper"
echo "MySQL backup tool. But still works with MariaDB 5.2.x"
read -ep "Do you want to continue with upgrade to MariaDB 10.x ? [y/n] " contbackup

if [[ "$contbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.x upgrade..." $boldyellow
exit
fi

}

mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb10uninstall() {
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
# OLDMARIADBCOMPAT=`rpm -qa | grep MariaDB | awk '/MariaDB-compat/'`

yum -q -y remove perl-DBD-MySQL

echo "rpm -e $OLDMARIADBTEST"
echo "rpm -e $OLDMARIADBCLIENT"
echo "rpm -e $OLDMARIADBDEVEL"
echo "rpm -e $OLDMARIADBSERVER"
echo "rpm -e $OLDMARIADBSHARED"
echo "rpm -e $OLDMARIADBCOMMON"
# echo "rpm -e $OLDMARIADBCOMPAT"

rpm -e $OLDMARIADBTEST
rpm -e $OLDMARIADBCLIENT
rpm -e $OLDMARIADBDEVEL
rpm -e $OLDMARIADBSERVER
rpm -e $OLDMARIADBSHARED
rpm -e $OLDMARIADBCOMMON
rpm -e $OLDMARIADBCOMPAT

yum clean all

if [ -z "$OLDMARIADBVERS" ]; then
echo "MariaDB old versions successfully removed..."
fi

echo "#####################################################"

}

mariadb10install() {

    echo "*************************************************"
	cecho "MariaDB 10.x Repo setup..." $boldgreen
   echo "*************************************************"
echo ""

yum clean all

CENTOSVER=`cat /etc/redhat-release | awk '{ print $3 }'`

if [ "$CENTOSVER" == 'release' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $4 }'`
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $7 }'`
fi

MARIADB55ARCH=`uname -m`

echo "rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB"
rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

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

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' ]]; then

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
baseurl = http://yum.mariadb.org/10.0/centos5-x86
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
priority=8
EOF

    fi # arch check
fi # centos5 check

}

mariadbtenpgrade() {
#check if MariaDB 5.5 RPM already installed
if [ -z "$OLDMARIADBVERS" ]; then
backupmsg
mariadb10uninstall
mariadb10install

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
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
cp /etc/my.cnf /etc/my.cnf-original_before_mariadb55upgrade
fi
echo ""
    echo "*************************************************"
cecho "MariaDB 10.x check available packages" $boldgreen
cecho "yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
echo ""
time yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared

echo ""
    echo "*************************************************"
cecho "MariaDB 10.x YUM install..." $boldgreen
cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
echo ""
time yum -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared

cp -a /etc/my.cnf /etc/my.cnf-newold
echo ""
    echo "*************************************************"
    cecho "* Adjust old MariaDB 5.2.x /etc/my.cnf " $boldyellow
    cecho "  for MariaDB 5.5 my.cnf compatibility " $boldyellow
    echo "*************************************************"

# unalias cp
if [ -f /etc/my.cnf.rpmorig ]; then
\cp -f /etc/my.cnf.rpmsave /etc/my.cnf
fi
sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
sed -i 's/innodb_additional_mem_pool_size/#innodb_additional_mem_pool_size/g' /etc/my.cnf
sed -i 's/skip-innodb/innodb=ON/g' /etc/my.cnf

echo "" >> /etc/my.cnf
echo "" >> /etc/my.cnf
echo "[mariadb-10.0]" >> /etc/my.cnf
echo "# 2 variables needed to switch from XtraDB to InnoDB plugins" >> /etc/my.cnf
echo "#plugin-load=ha_innodb" >> /etc/my.cnf
echo "#ignore_builtin_innodb" >> /etc/my.cnf
echo "" >> /etc/my.cnf
echo "## MariaDB 10 InnoDB plugin only" >> /etc/my.cnf
echo "#innodb_buffer_pool_load_now=1" >> /etc/my.cnf
echo "#innodb_buffer_pool_dump_now=1" >> /etc/my.cnf
echo "## Disabled settings" >> /etc/my.cnf
echo "performance_schema=OFF" >> /etc/my.cnf
echo "innodb_stats_on_metadata=OFF" >> /etc/my.cnf
echo "innodb_sort_buffer_size=2M" >> /etc/my.cnf
echo "innodb_online_alter_log_max_size=128M" >> /etc/my.cnf
echo "query_cache_strip_comments=0" >> /etc/my.cnf
echo "log_slow_filter =admin,filesort,filesort_on_disk,full_join,full_scan,query_cache,query_cache_miss,tmp_table,tmp_table_on_disk" >> /etc/my.cnf

echo ""

service mysql start

echo ""
echo "------------------------------------------------"
echo "Updating MySQL database tables"
echo "------------------------------------------------"
read -p "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
echo ""

if [[ "$mysqlrootset" == [yY] ]]; then
    echo "Enter MySQL root password when prompted"
    echo "or hit enter to continue"
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

    # echo "Enter MySQL root password when prompted"
    # echo "or hit enter to continue"
    # echo "mysql_upgrade -u root -p --force"
    # mysql_upgrade -u root -p --force

    # sed -i 's/innodb=ON/innodb=OFF/g' /etc/my.cnf
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
sed -i 's/innodb_doublewrite = 1/#innodb_doublewrite = 1/g' /etc/my.cnf
sed -i 's/innodb_doublewrite=1/#innodb_doublewrite=1/g' /etc/my.cnf
service mysql restart

echo ""

yum -q -y install perl-DBD-MySQL

echo ""

echo "------------------------------------------------"
echo "MariaDB 10.x Upgrade completed"
echo "------------------------------------------------"
fi
}