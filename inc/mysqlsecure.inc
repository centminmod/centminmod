securemysql() {
THEMYSQLPASS=$(openssl rand 21 -base64 | tr -dc 'a-zA-Z0-9')

# mysql -e "DELETE FROM mysql.user WHERE User=""; FLUSH PRIVILEGES;"
# mysqladmin -u root password $THEMYSQLPASS

CHECK_FOR_MARIADB_TENFOUR=$(rpm -qa | grep -i MariaDB-server | head -n1 | cut -d '-' -f3 | cut -d . -f1,2)

if [[ "$CHECK_FOR_MARIADB_TENFOUR" = '10.4' ]]; then
  echo
  mysql -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-check-users-before.log"
  echo
  mysql -e "ALTER USER root@localhost IDENTIFIED VIA mysql_native_password USING PASSWORD('$THEMYSQLPASS')"  >/dev/null 2>&1
  echo
  mysql -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-check-users-after.log"
  echo
else
  mysql -e "DROP USER ''@'localhost';" >/dev/null 2>&1
  mysql -e "DROP USER ''@'`hostname`';" >/dev/null 2>&1
  mysql -e "DROP DATABASE test;" >/dev/null 2>&1
  mysql -e "UPDATE mysql.user SET Password = PASSWORD('$THEMYSQLPASS') WHERE User = 'root'; FLUSH PRIVILEGES;" >/dev/null 2>&1
fi

# check
# mysql -e "SELECT User, Host, Password FROM mysql.user;"

cat > "/root/.my.cnf" <<EOF
[client]
user=root
password=$THEMYSQLPASS
EOF

{
echo ""
cecho "-------------------------------------------------------" $boldyellow
cecho "MySQL root user password set" $boldgreen
cecho "MySQL root user added to /root/.my.cnf" $boldgreen
cecho "-------------------------------------------------------" $boldyellow
cecho "MySQL root password: $THEMYSQLPASS" $boldgreen
cecho "-------------------------------------------------------" $boldyellow
} 2>&1 | tee "${CENTMINLOGDIR}/set-dot-my.cnf.log"
}


show_mysqlpass() {
  cat "${CENTMINLOGDIR}/set-dot-my.cnf.log"
}