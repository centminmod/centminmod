#######################################################################################
# MariaDB MySQL Upgrade sub menu
#######################################################################################
ENABLE_MARIADBTENUPGRADE='y'
ENABLE_MARIADBTENTWOUPGRADE='n'
ENABLE_MARIADBTENTHREEUPGRADE='n'
ENABLE_MARIADBTENFOURUPGRADE='y'
ENABLE_MARIADBTENFIVEUPGRADE='y'
ENABLE_MARIADBTENSIXUPGRADE='y'
ENABLE_MARIADBTENELEVENUPGRADE='n'
ENABLE_MARIADBELEVENFOURUPGRADE='n'
SKIP_INNODBFASTSHUTDOWNDISABLE='y'   # no longer needed as of MariaDB 10.2.5 https://mariadb.com/kb/en/library/upgrading-from-mariadb-101-to-mariadb-102/

# Function to check InnoDB status
check_innodb_status() {
    STATUS=$(${ALIAS_MYSQL} -e "SHOW ENGINE INNODB STATUS\G" 2>&1 | grep "sleeping")
    if [[ -n "$STATUS" ]]; then
        return 0
    else
        return 1
    fi
}

mariadb_upgrade_check_versionlock() {
  # Function for centmin.sh menu option 11 based major MariaDB server version
  # upgrades to check if MariaDB yum packages are yum version locked by system
  # administrator. If yum versionlock is in place, it will prevent centmin.sh
  # menu option 11 based MariaDB major version upgrades as yum will not be able
  # to find the MariaDB yum packages to update to the new version.
  # This function checks if MariaDB and Galera yum packages are version locked
  # and if so, version unlock them to allow for upgrades. This function will not
  # re-lock them after update though due to the updated yum packages maybe having
  # a different yum package name which may not properly lock the package or only
  # lock some of the updated yum packages - leading to some packages being locked
  # and others being unlocked which will make yum update command probably fail
  # otherwise. So leave it to system admin to determine what yum packages they
  # want to re-lock after updates via centmin.sh menu option 11
  # check for version locked MariaDB & Galera yum packages

  # only run if yum versionlock plugin is installed
  if [[ "$(yum --help | grep -co versionlock)" -ge '1' ]]; then
    check_mdb_versionlock_pkgs=$(yum versionlock list | egrep -i 'mariadb|galera' | awk -F '0:' '{print $2}' | sed -e 's|\.\*||g')
    if [[ ! -z "$check_mdb_versionlock_pkgs" ]]; then
      echo "*************************************************"
      echo -e "\ndetected the following MariaDB server yum package yum versionlocks:\n"
      echo "*************************************************"
      echo "$check_mdb_versionlock_pkgs"
      echo "*************************************************"
      echo -e "\nremoving MariaDB yum versionlocks for updates...\n"
      echo "*************************************************"
      yum versionlock delete $check_mdb_versionlock_pkgs
    else
      echo -e "\n no MariaDB / Galera yum packages version locked packages detected\n"
    fi
  fi
}

mariadbsubmenu() {
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-autobackup-$DT"
  fi

while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "     MariaDB MySQL Upgrade & Management Sub-Menu        " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "0). Regenerate New MySQL Root User Password (local server)" $boldgreen
  cecho "1). MariaDB 5.2.x Branch Upgrade Only" $boldgreen
  cecho "2). MariaDB 5.2.x to MariaDB 5.5 YUM upgrade" $boldgreen
  cecho "3). MariaDB 5.5 to MariaDB 10.x YUM upgrade" $boldgreen
  cecho "4). MariaDB 5.5 to MariaDB 10.1.x YUM upgrade" $boldgreen
  cecho "5). MariaDB 10.x to MariaDB 10.1.x YUM upgrade" $boldgreen
  cecho "6). MariaDB 10.1.x to MariaDB 10.2.x YUM upgrade" $boldgreen
  cecho "7). MariaDB 10.2.x to MariaDB 10.3.x YUM upgrade" $boldgreen
  cecho "8). MariaDB 10.3.x to MariaDB 10.4.x YUM upgrade" $boldgreen
  cecho "9). MariaDB 10.4.x to MariaDB 10.5.x YUM upgrade" $boldgreen
  cecho "10). MariaDB 10.5.x to MariaDB 10.6.x LTS YUM upgrade" $boldgreen
  cecho "11). MariaDB 10.6.x to MariaDB 10.11.x YUM upgrade" $boldgreen
  cecho "12). MariaDB 10.6.x to MariaDB 11.4.x LTS YUM upgrade" $boldgreen
  cecho "13). MariaDB 10.11.x to MariaDB 11.4.x LTS YUM upgrade" $boldgreen
  cecho "14). Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 0 - 14 ] " mmoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$mmoption" in
0)

centminlog
{
STOPMYSQL='y'
CHECK_FOR_MARIADB_TENFOUR_ABOVE=$(rpm -qa | grep -i -E '^MariaDB-server-|^mariadb-server-' | grep -v 'server-utils' | head -n1 | awk -F- '{print $3}' | cut -d . -f1,2)
echo
if [[ "$STOPMYSQL" = [yY] ]]; then
  cecho "You may want to schedule the password reset at a off peak site time" $boldyellow
  echo
  cecho "When you answer no to - Do you know the existing MySQL root password question," $boldyellow
  cecho "this routine will regenerate a new MySQL root user password &" $boldyellow
  cecho "properly update the /root/.my.cnf file as well. To do this MySQL" $boldyellow
  cecho "server needs to be temporarily shutdown and restarted. To prevent" $boldyellow
  cecho "visitor shown errors, this routine will also temp shutdown Nginx." $boldyellow
  cecho "So you will want to do this at a off peak site/server time ideally." $boldyellow
  echo
  cecho "When you answer yes to - Do you know the existing MySQL root password question," $boldyellow
  cecho "this routine will not need to shutdown MariaDB MySQL & Nginx server" $boldyellow
else
  cecho "This routine will reset the MySQL root user password" $boldyellow
  cecho "and properly update the /root/.my.cnf file as well." $boldyellow
fi
echo
cecho "This routine is only for local MariaDB MySQL servers not remote ones" $boldyellow
echo
read -ep "Do you want to continue? [y/n]: " mysqlroot_reset
read -ep "Do you know the existing MySQL root password set in /root/.my.cnf? [y/n]: " mysqlroot_known
echo
if [[ "$mysqlroot_reset" = [yY] ]]; then
  echo "Process begins in 10 seconds..."
  echo
  if [[ "$mysqlroot_known" = [yY] && -f /root/.my.cnf ]]; then
    STOPMYSQL=n
    echo "Previous MySQL root password:"
    echo
    cat /root/.my.cnf
    echo
    OLDMYSQLROOTPASS=$(awk -F '=' '/password/ {print $2}' /root/.my.cnf)
  else
    # echo "/root/.my.cnf does not exist"
    # echo "can not continue as this routine uses /root/.my.cnf"
    # echo
    # exit
    echo "Proceeding with root password reset without known password"
  fi
  if [[ "$STOPMYSQL" = [yY] ]] && [[ "$CENTOS_NINE" = '9' ]]; then
    # mysqladmin flush-tables >/dev/null 2>&1
    # sleep 10
    service nginx stop
    systemctl stop mariadb
    ${ALIAS_MYSQLDSAFE} --skip-grant-tables > /dev/null 2>&1 &
    sleep 10
    # ${ALIAS_MYSQL} -e "FLUSH PRIVILEGES;"
  elif [[ "$STOPMYSQL" = [yY] ]] && [[ "$CENTOS_EIGHT" = '8' ]]; then
    # mysqladmin flush-tables >/dev/null 2>&1
    # sleep 10
    service nginx stop
    systemctl stop mariadb
    ${ALIAS_MYSQLDSAFE} --skip-grant-tables > /dev/null 2>&1 &
    sleep 10
    # ${ALIAS_MYSQL} -e "FLUSH PRIVILEGES;"
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    # mysqladmin flush-tables >/dev/null 2>&1
    # sleep 10
    service nginx stop
    systemctl stop mariadb
    ${ALIAS_MYSQLDSAFE} --skip-grant-tables > /dev/null 2>&1 &
    sleep 10
    # ${ALIAS_MYSQL} -e "FLUSH PRIVILEGES;"
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    # mysqladmin flush-tables >/dev/null 2>&1
    # sleep 10
    service nginx stop
    service mysql stop
    ${ALIAS_MYSQLD} --skip-grant-tables --skip-networking --user=mysql > /dev/null 2>&1 &
    sleep 10
    # ${ALIAS_MYSQL} -e "FLUSH PRIVILEGES;"
  fi
  NEWMYSQLROOTPASS=$(/usr/bin/openssl rand -base64 27 | tr -dc 'a-zA-Z0-9')
  if [[ "$mysqlroot_known" = [yY] && -f /root/.my.cnf ]]; then
    echo "${ALIAS_MYSQLADMIN} -u root -p${OLDMYSQLROOTPASS} password $NEWMYSQLROOTPASS"
    ${ALIAS_MYSQLADMIN} -u root -p${OLDMYSQLROOTPASS} password $NEWMYSQLROOTPASS
  else
    rm -rf /root/.my.cnf
    if [[ "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.2' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.3' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.4' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.5' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.6' ]]; then
      # echo "${ALIAS_MYSQL} -u root -e \"FLUSH PRIVILEGES; ALTER USER root@localhost IDENTIFIED BY mysql_native_password USING PASSWORD('$NEWMYSQLROOTPASS');FLUSH PRIVILEGES;\""
      # ${ALIAS_MYSQL} -u root -e "FLUSH PRIVILEGES; ALTER USER root@localhost IDENTIFIED BY mysql_native_password USING PASSWORD('$NEWMYSQLROOTPASS');FLUSH PRIVILEGES;"
      echo "${ALIAS_MYSQL} -u root -B -N -e \"FLUSH PRIVILEGES; ALTER USER 'root'@'localhost' IDENTIFIED BY '${NEWMYSQLROOTPASS}'; FLUSH PRIVILEGES;\""
      ${ALIAS_MYSQL} -u root -B -N -e "FLUSH PRIVILEGES; ALTER USER 'root'@'localhost' IDENTIFIED BY '${NEWMYSQLROOTPASS}'; FLUSH PRIVILEGES;"
    else
      sleep 5
      # echo "${ALIAS_MYSQL} -e \"select user, host, password, plugin, authentication_string from mysql.user where user='root';\""
      # ${ALIAS_MYSQL} -e "select user, host, password, plugin, authentication_string from mysql.user where user='root';"

      # echo "${ALIAS_MYSQL} -u root -B -N -e \"FLUSH PRIVILEGES; ALTER USER 'root'@'localhost' IDENTIFIED BY '${NEWMYSQLROOTPASS}'; FLUSH PRIVILEGES;\""
      # ${ALIAS_MYSQL} -u root -B -N -e "FLUSH PRIVILEGES; ALTER USER 'root'@'localhost' IDENTIFIED BY '${NEWMYSQLROOTPASS}'; FLUSH PRIVILEGES;"

      echo "${ALIAS_MYSQL} -u root -e \"UPDATE mysql.user SET Password = PASSWORD('$NEWMYSQLROOTPASS') WHERE User = 'root'; FLUSH PRIVILEGES;\""
      ${ALIAS_MYSQL} -u root -e "UPDATE mysql.user SET Password = PASSWORD('$NEWMYSQLROOTPASS') WHERE User = 'root'; FLUSH PRIVILEGES;"
    fi
  fi
  if [[ $? -ne 0 ]]; then
    echo "Error: Failed to reset MySQL root password."
    # exit 1
  fi
  echo
  echo "New MySQL root user password: $NEWMYSQLROOTPASS"
  echo
  if [[ "$STOPMYSQL" = [yY] ]] && [[ "$CENTOS_NINE" = '9' ]]; then
    # ${ALIAS_MYSQLADMIN} -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    # MYSQL_PID=$(pidof mysqld)
    # kill -9 $MYSQL_PID >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLDSAFE} >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLD} >/dev/null 2>&1
    killall -9 mariadbd >/dev/null 2>&1
    sleep 10
  elif [[ "$STOPMYSQL" = [yY] ]] && [[ "$CENTOS_EIGHT" = '8' ]]; then
    # ${ALIAS_MYSQLADMIN} -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    # MYSQL_PID=$(pidof mysqld)
    # kill -9 $MYSQL_PID >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLDSAFE} >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLD} >/dev/null 2>&1
    killall -9 mariadbd >/dev/null 2>&1
    sleep 10
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    # ${ALIAS_MYSQLADMIN} -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    # MYSQL_PID=$(pidof mysqld)
    # kill -9 $MYSQL_PID >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLDSAFE} >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLD} >/dev/null 2>&1
    sleep 10
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    # ${ALIAS_MYSQLADMIN} -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    # MYSQL_PID=$(pidof mysqld)
    # kill -9 $MYSQL_PID >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLDSAFE} >/dev/null 2>&1
    killall -9 ${ALIAS_MYSQLD} >/dev/null 2>&1
    sleep 10
  fi
  if [[ "$STOPMYSQL" = [yY] ]] && [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
    echo "systemctl start mariadb"
    systemctl start mariadb
    echo "service nginx start"
    service nginx start
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    echo "systemctl start mariadb"
    systemctl start mariadb
    echo "service nginx start"
    service nginx start
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    echo "service mysql start"
    service mysql start
    echo "service nginx start"
    service nginx start
  fi
  # if [[ "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.3' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.2' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.1' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.0' ]]; then
  #     sleep 5
  #     echo "${ALIAS_MYSQL} -u root -p${NEWMYSQLROOTPASS} -e \"SET PASSWORD FOR 'root'@'localhost' = PASSWORD('$NEWMYSQLROOTPASS'); FLUSH PRIVILEGES;\""
  #     ${ALIAS_MYSQL} -u root -p${NEWMYSQLROOTPASS} -e "SET PASSWORD FOR 'root'@'localhost' = PASSWORD('$NEWMYSQLROOTPASS'); FLUSH PRIVILEGES;"
  # fi
  if [[ "$STOPMYSQL" = [yY] && "$?" = '0' ]]; then
    echo "MySQL root user password updated: $NEWMYSQLROOTPASS"
    echo
  fi
  if [[ "$mysqlroot_known" = [yY] && -f /root/.my.cnf ]]; then
    sed -i "s|password=.*|password=$NEWMYSQLROOTPASS|" /root/.my.cnf
    echo "/root/.my.cnf updated"
    echo
    cat /root/.my.cnf
    echo
  else
cat > /root/.my.cnf <<EOF
[client]
user=root
password=${NEWMYSQLROOTPASS}
EOF
  fi
else
  echo "aborting routine..."
  exit
fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mysqlrootpass_regenerate.log

;;
1)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu1-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-3`

# for MariaDB 5.2 upgrade option
if [[ "$MARIADBVERCHECK" == '5.3' || "$MARIADBVERCHECK" == '5.5' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.0' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.1' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.2' ]]; then

echo ""
cecho "MariaDB 5.3.x or 5.5.x server detected" $boldyellow
cecho "downgrading to MariaDB 5.2.x is not recommended" $boldyellow
cecho "aborting MariaDB 5.2.x upgrade option..." $boldyellow
sleep 5
exit

else

funct_mariadbupgrade

fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb52_upgrade.log

;;
2)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu2-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-3`

# for MariaDB 5.5 upgrade option
if [[ "$MARIADBVERCHECK" = '5.5' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.0' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.1' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.2' ]]; then

echo ""
cecho "MariaDB 5.5.x server already detected" $boldyellow
cecho "MariaDB 5.5 uses normal YUM commands for updates as follows:" $boldyellow
echo ""
cecho " yum update --disablerepo=CentALT MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared MariaDB-test" $boldgreen
echo ""
cecho "aborting MariaDB 5.5.x upgrade option..." $boldyellow
sleep 5
exit

else

mariadbfiveupgrade

fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb55_upgrade.log

;;
3)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu3-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  if [[ "$MARIADBVERCHECK" = '10.0' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.1' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.2' ]]; then

    echo ""
    cecho "MariaDB 10.x server already detected" $boldyellow

  else

    if [[ "$ENABLE_MARIADBTENUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.x is ready to be added"
    else
      c7tmpsetup
      mariadbtenpgrade
    fi
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb10_upgrade.log
;;
4)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu4-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  if [[ "$MARIADBVERCHECK" = '10.1' || $(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4) = '10.2' ]]; then

    echo ""
    cecho "MariaDB 10.1.x server already detected" $boldyellow

  else

    if [[ "$ENABLE_MARIADBTENUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.1.x is ready to be added"
    else
      echo "Place holder for now until MariaDB 10.1.x is ready to be added"
      # mariadbfivetotenoneupgrade
    fi
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_55-to-10-1_upgrade.log
;;
5)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu5-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  if [[ "$MARIADBVERCHECK" = '10.1' ]]; then

    echo ""
    cecho "MariaDB 10.1.x server already detected" $boldyellow

  else

    if [[ "$ENABLE_MARIADBTENUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.1.x is ready to be added"
    else
      # echo "Place holder for now until MariaDB 10.1.x is ready to be added"
      c7tmpsetup
      mariadbtentotenoneupgrade
    fi
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-to-10-1_upgrade.log
;;
6)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu6-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  if [[ "$MARIADBVERCHECK" = '10.2' ]]; then

    echo ""
    cecho "MariaDB 10.2.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.1' ]; then

    if [[ "$ENABLE_MARIADBTENTWOUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.2.x is ready to be added"
    else
      # echo "Place holder for now until MariaDB 10.2.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtentotentwoupgrade
      else
        echo "currently only tested on CentOS 7 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.1' ]; then
    echo "Need to be on MariaDB 10.1 before updating to MariaDB 10.2"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-1-to-10-2_upgrade.log
;;
7)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu7-autobackup-$DT"
  fi
centminlog
{
MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  if [[ "$MARIADBVERCHECK" = '10.3' ]]; then

    echo ""
    cecho "MariaDB 10.3.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.2' ]; then

    if [[ "$ENABLE_MARIADBTENTHREEUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.3.x is ready to be added"
    else
      # echo "Place holder for now until MariaDB 10.3.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtentwototenthreeupgrade
      else
        echo "currently only tested on CentOS 7 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.2' ]; then
    echo "Need to be on MariaDB 10.2 before updating to MariaDB 10.3"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-2-to-10-3_upgrade.log
;;
8)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu8-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  fi
  if [[ "$MARIADBVERCHECK" = '10.4' ]]; then

    echo ""
    cecho "MariaDB 10.4.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.3' ]; then

    if [[ "$ENABLE_MARIADBTENFOURUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.4.x is ready to be added"
      echo "10.4 officially supported until June 2024"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-104/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-104/"
    else
      # echo "Place holder for now until MariaDB 10.4.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtenthreetotenfourupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtenthreetotenfourupgrade
      elif [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtenthreetotenfourupgrade
      else
        echo "currently only tested on CentOS 7 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.3' ]; then
    echo "Need to be on MariaDB 10.3 before updating to MariaDB 10.4"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-3-to-10-4_upgrade.log
;;
9)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu9-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  fi
  if [[ "$MARIADBVERCHECK" = '10.5' ]]; then

    echo ""
    cecho "MariaDB 10.5.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.4' ]; then

    if [[ "$ENABLE_MARIADBTENFIVEUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.5.x is ready to be added"
      echo "10.5 officially supported until June 2025"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-105/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-105/"
    else
      # echo "Place holder for now until MariaDB 10.4.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtenfourtotenfiveupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtenfourtotenfiveupgrade
      elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtenfourtotenfiveupgrade
      else
        echo "currently only tested on CentOS 7 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.4' ]; then
    echo "Need to be on MariaDB 10.4 before updating to MariaDB 10.5"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-4-to-10-5_upgrade.log
;;
10)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu10-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  fi
  if [[ "$MARIADBVERCHECK" = '10.6' ]]; then

    echo ""
    cecho "MariaDB 10.6.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.5' ]; then

    if [[ "$ENABLE_MARIADBTENSIXUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.6.x is ready to be added"
      echo "10.6 LTS release officially supported until July 2026"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-106/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-106/"
    else
      # echo "Place holder for now until MariaDB 10.5.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtenfivetotensixupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtenfivetotensixupgrade
      elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtenfivetotensixupgrade
      else
        echo "currently only tested on CentOS 7 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.5' ]; then
    echo "Need to be on MariaDB 10.5 before updating to MariaDB 10.6"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-5-to-10-6_upgrade.log
;;
11)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu11-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  fi
  if [[ "$MARIADBVERCHECK" = '10.11' ]]; then

    echo ""
    cecho "MariaDB 10.11.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.6' ]; then

    if [[ "$ENABLE_MARIADBTENELEVENUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 10.11.x is ready to be added"
      echo "10.11 LTS release officially supported until February, 2028"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-1011/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-1011/"
    else
      # echo "Place holder for now until MariaDB 10.5.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtensixtotenelevenupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtensixtotenelevenupgrade
      elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtensixtotenelevenupgrade
      else
        echo "currently only tested on EL8 or EL9 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.6' ]; then
    echo "Need to be on MariaDB 10.6 before updating to MariaDB 10.11"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10-6-to-10-11_upgrade.log
;;
12)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu12-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
  fi
  if [[ "$MARIADBVERCHECK" = '11.4' ]]; then

    echo ""
    cecho "MariaDB 11.4.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.6' ]; then

    if [[ "$ENABLE_MARIADBELEVENFOURUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 11.4.x is ready to be added"
      echo "11.4 LTS release officially supported until May 2029"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-11-4/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-114/"
    else
      # echo "Place holder for now until MariaDB 10.5.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtentoelevenupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtentoelevenupgrade
      elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtentoelevenupgrade
      else
        echo "currently only tested on EL8 or EL9 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.6' ]; then
    echo "Need to be on MariaDB 10.6 before updating to MariaDB 11.4"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_10x-to-114_upgrade.log
;;
13)
  if [ -f /etc/my.cnf ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    cp -a /etc/my.cnf "/etc/my.cnf-menu11-submenu13-autobackup-$DT"
  fi
centminlog
{
  if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    MARIADBVERCHECK=`rpm -qa | grep mariadb-server | grep -v utils | awk -F "-" '{print $3}' | cut -c1-4`
  else
    if [[ "$ENABLE_MARIADBELEVENFOURUPGRADE" = [yY] ]]; then
      MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-5`
    else
      MARIADBVERCHECK=`rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4`
    fi
  fi
  if [[ "$MARIADBVERCHECK" = '11.4' ]]; then

    echo ""
    cecho "MariaDB 11.4.x server already detected" $boldyellow

  elif [ "$MARIADBVERCHECK" == '10.11' ]; then

    if [[ "$ENABLE_MARIADBELEVENFOURUPGRADE" = [nN] ]]; then
      echo "Place holder for now until MariaDB 11.4.x is ready to be added"
      echo "11.4 LTS release officially supported until May 2029"
      echo "https://mariadb.com/kb/en/changes-improvements-in-mariadb-11-4/"
      echo
      echo "Discussions https://community.centminmod.com/tags/mariadb-114/"
    else
      # echo "Place holder for now until MariaDB 10.5.x is ready to be added"
      c7tmpsetup
      if [[ "$CENTOS_NINE" -eq '9' ]]; then
        mariadbtentoelevenupgrade
      elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
        mariadbtentoelevenupgrade
      elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        mariadbtentoelevenupgrade
      else
        echo "currently only tested on EL8 or EL9 systems"
      fi
    fi
  elif [ "$MARIADBVERCHECK" != '10.11' ]; then
    echo "Need to be on MariaDB 10.11 before updating to MariaDB 11.4"
    exit
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_mariadb_1011-to-114_upgrade.log
;;
14)
  break
;;
esac
done

}

