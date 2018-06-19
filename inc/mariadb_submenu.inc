#######################################################################################
# MariaDB MySQL Upgrade sub menu
#######################################################################################
ENABLE_MARIADBTENUPGRADE='y'
ENABLE_MARIADBTENTWOUPGRADE='n'
ENABLE_MARIADBTENTHREEUPGRADE='n'
SKIP_INNODBFASTSHUTDOWNDISABLE='y'   # no longer needed as of MariaDB 10.2.5 https://mariadb.com/kb/en/library/upgrading-from-mariadb-101-to-mariadb-102/

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
  cecho "8). Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 0 - 8 ] " mmoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$mmoption" in
0)

centminlog
{
STOPMYSQL='n'
echo
if [[ "$STOPMYSQL" = [yY] ]]; then
  cecho "This routine will regenerate a new MySQL root user password &" $boldyellow
  cecho "properly update the /root/.my.cnf file as well. To do this MySQL" $boldyellow
  cecho "server needs to be temporarily shutdown and restarted. To prevent" $boldyellow
  cecho "visitor shown errors, this routine will also temp shutdown Nginx." $boldyellow
  cecho "So you will want to do this at a off peak site/server time ideally." $boldyellow
else
  cecho "This routine will reset the MySQL root user password" $boldyellow
  cecho "and properly update the /root/.my.cnf file as well." $boldyellow
fi
echo
cecho "This routine is only for local MariaDB MySQL servers not remote ones" $boldyellow
echo
read -ep "Do you want to continue? [y/n]: " mysqlroot_reset
echo
if [[ "$mysqlroot_reset" = [yY] ]]; then
  echo "Process begins in 10 seconds..."
  echo
  if [ -f /root/.my.cnf ]; then
    echo "Previous MySQL root password:"
    echo
    cat /root/.my.cnf
    echo
    OLDMYSQLROOTPASS=$(awk -F '=' '/password/ {print $2}' /root/.my.cnf)
  else
    echo "/root/.my.cnf does not exist"
    echo "can not continue as this routine uses /root/.my.cnf"
    echo
    exit
  fi
  if [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    mysqladmin flush-tables >/dev/null 2>&1
    sleep 10
    service nginx stop
    service mariadb stop
    mysqld --skip-grant-tables --skip-networking --user=mysql &
    mysql -e "FLUSH PRIVILEGES;"
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    mysqladmin flush-tables >/dev/null 2>&1
    sleep 10
    service nginx stop
    service mysql stop
    mysqld --skip-grant-tables --skip-networking --user=mysql &
    mysql -e "FLUSH PRIVILEGES;"
  fi
  NEWMYSQLROOTPASS=$(openssl rand 21 -base64 | tr -dc 'a-zA-Z0-9')
  echo "mysqladmin -u root -p${OLDMYSQLROOTPASS} password $NEWMYSQLROOTPASS"
  mysqladmin -u root -p${OLDMYSQLROOTPASS} password $NEWMYSQLROOTPASS
  echo
  echo "New MySQL root user password: $NEWMYSQLROOTPASS"
  echo
  if [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    mysqladmin -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    MYSQL_PID=$(pidof mysqld)
    kill -9 $MYSQL_PID >/dev/null 2>&1
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    mysqladmin -u root -p${NEWMYSQLROOTPASS} flush-tables >/dev/null 2>&1
    sleep 3
    MYSQL_PID=$(pidof mysqld)
    kill -9 $MYSQL_PID >/dev/null 2>&1
  fi
  if [[ "$STOPMYSQL" = [yY] && "$CENTOS_SEVEN" = '7' ]]; then
    service mariadb start
    service nginx start
  elif [[ "$STOPMYSQL" = [yY] && "$CENTOS_SIX" = '6' ]]; then
    service mysql start
    service nginx start
  fi
  if [[ "$STOPMYSQL" = [yY] && "$?" = '0' ]]; then
    echo "MySQL root user password updated: $NEWMYSQLROOTPASS"
    echo
  fi
  if [ -f /root/.my.cnf ]; then
    sed -i "s|password=.*|password=$NEWMYSQLROOTPASS|" /root/.my.cnf
    echo "/root/.my.cnf updated"
    echo
    cat /root/.my.cnf
    echo
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
      # echo "Place holder for now until MariaDB 10.2.x is ready to be added"
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
  break
;;
esac
done

}

