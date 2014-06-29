backupmsg() {

echo ""
cat <<EOF
--------------------------------------------------------------------------------
 This script's MariaDB 5.5 upgrade process is still in 
 beta testing stages. So please test on a test server 
 and not a live production server. But if you use on 
 live server or if MySQL databases already exist, 
 please back them up before running this upgrade 
 script via mysqldump - instructions at: 
 http://dev.mysql.com/doc/refman/5.1/en/mysqldump.html.

 For each DATABASENAME backup via mysqldump i.e.

  mysqldump --opt --routines DATABASENAME > /path/to/DATABASENAME_backup.sql
EOF

echo "--------------------------------------------------------------------------------"
echo ""
read -ep "Have you backed up your databases ? [y/n] " askbackup

if [[ "$askbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 5.5 upgrade..." $boldyellow
exit
fi

echo ""
echo "MariaDB 5.5.30 no longer works with mydumper"
echo "MySQL backup tool. But still works with MariaDB 5.2.x"
read -ep "Do you want to continue with upgrade to MariaDB 5.5.x ? [y/n] " contbackup

if [[ "$contbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 5.5 upgrade..." $boldyellow
exit
fi

}

mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

# MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared MariaDB-test

mariadb52uninstall() {
OLDMARIADBVERS=`rpm -qa | grep MariaDB`

echo ""

# List current existing MariaDB versions for RPMs

    echo "*************************************************"
    cecho "* Current MariaDB versions installed: " $boldgreen
    echo "*************************************************"

for mrpm in $OLDMARIADBVERS
do
cecho "$mrpm" $boldyellow
done

echo ""

    echo "*************************************************"
    cecho "* Proceeding to remove existing MariaDB versions: " $boldgreen
    cecho "* please wait while processing... " $boldgreen
    echo "*************************************************"

OLDMARIADBSHARED=`rpm -qa | grep MariaDB | awk '/MariaDB-shared/'`
OLDMARIADBCLIENT=`rpm -qa | grep MariaDB | awk '/MariaDB-client/'`
OLDMARIADBDEVEL=`rpm -qa | grep MariaDB | awk '/MariaDB-devel/'`
OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`
OLDMARIADBTEST=`rpm -qa | grep MariaDB | awk '/MariaDB-test/'`

yum -q -y remove perl-DBD-MySQL

echo "rpm -e $OLDMARIADBTEST"
echo "rpm -e $OLDMARIADBCLIENT"
echo "rpm -e $OLDMARIADBDEVEL"
echo "rpm -e $OLDMARIADBSERVER"
echo "rpm -e $OLDMARIADBSHARED"

rpm -e $OLDMARIADBTEST
rpm -e $OLDMARIADBCLIENT
rpm -e $OLDMARIADBDEVEL
rpm -e $OLDMARIADBSERVER
rpm -e $OLDMARIADBSHARED

if [ -z "$OLDMARIADBVERS" ]; then
echo ""
    echo "*************************************************"
    cecho "* MariaDB old versions successfully removed... " $boldgreen
    echo "*************************************************"
fi

echo ""

}

mariadbupgradeprep() {

    echo "*************************************************"
    cecho "* MariaDB 5.5 Repo setup... " $boldgreen
    cecho "  at /etc/yum.repos.d/mariadb.repo" $boldgreen
    echo "*************************************************"
echo ""

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
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos6-amd64
gpgcheck=1
priority=8
EOF

	else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos6-x86
gpgcheck=1
priority=8
EOF

	fi # arch check
fi # centos6 check

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then

	if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos5-amd64
gpgcheck=1
priority=8
EOF

	else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import http://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/5.5/centos5-x86
gpgcheck=1
priority=8
EOF

	fi # arch check
fi # centos5 check

}

mariadbfiveupgrade() {

#check if MariaDB 5.5 RPM already installed

if [ -z "$OLDMARIADBVERS" ]; then

backupmsg
mariadb52uninstall
mariadbupgradeprep

# make sure MariaDB 5.5 is updated from only one repo
echo ""
if [[ -z "$(grep 'exclude=mariadb' /etc/yum.repos.d/centalt.repo)" ]]; then
echo "exclude=mariadb*" >> /etc/yum.repos.d/centalt.repo
fi
# exclude MariaDB Galera Server
if [[ -z "$(grep 'exclude=MariaDB-Galera-server' /etc/yum.repos.d/mariadb.repo)" ]]; then
echo "exclude=MariaDB-Galera-server" >> /etc/yum.repos.d/mariadb.repo
fi

if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original_before_mariadb55upgrade
fi

echo ""

    echo "*************************************************"
    cecho "* MariaDB 5.5 check available packages " $boldgreen
    cecho "  yum -q --disableplugin=fastestmirror,downloadonly --disablerepo=CentALT list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
echo ""
yum -q --disableplugin=fastestmirror,downloadonly --disablerepo=CentALT list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared
echo ""

    echo "*************************************************"
    cecho "* MariaDB 5.5 YUM install... " $boldgreen
    cecho "  yum -q -y --disableplugin=fastestmirror,downloadonly --disablerepo=CentALT install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared" $boldgreen
    echo "*************************************************"
echo ""
yum -q -y --disableplugin=fastestmirror,downloadonly --disablerepo=CentALT install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared
echo ""


echo ""
    echo "*************************************************"
    cecho "* Adjust old MariaDB 5.2.x /etc/my.cnf " $boldyellow
    cecho "  for MariaDB 5.5 my.cnf compatibility " $boldyellow
    echo "*************************************************"

unalias cp

if [ -f /etc/my.cnf.rpmorig ]; then
cp -f /etc/my.cnf.rpmorig /etc/my.cnf
fi

echo "sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf"
echo "sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf"
echo "sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf"
echo "sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf"

sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf

echo ""

mysqlinitfix
service mysql restart

echo ""
    echo "*************************************************"
    cecho "* Updating MySQL database tables " $boldgreen
    echo "*************************************************"
echo ""
read -ep "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
echo ""

if [[ "$mysqlrootset" == [yY] ]]; then
    echo "*************************************************"
    cecho "* Enter MySQL root password when prompted " $boldgreen
    echo "*************************************************"
echo ""
echo "mysql_upgrade -u root -p"
mysql_upgrade -u root -p
else
echo "mysql_upgrade"
mysql_upgrade
fi

echo ""

yum -q -y install perl-DBD-MySQL

echo ""

else

echo ""

    echo "*************************************************"
    cecho "* MariaDB 5.5 Repo already installed " $boldyellow
    echo "*************************************************"

echo ""

fi

}