set_mariadb_mysqldump_charset() {
  if [[ -f /etc/my.cnf && "$SET_DEFAULT_MYSQLCHARSET" != 'utf8' && ! -z "$SET_DEFAULT_MYSQLCHARSET" ]]; then
    if [[ "$(grep '^#default-character-set=utf8mb4' /etc/my.cnf)" = '#default-character-set=utf8mb4' ]]; then
      sed -i "s|^#default-character-set=utf8mb4|default-character-set=utf8mb4|g" /etc/my.cnf
    elif [[ "$(grep '^default-character-set=' /etc/my.cnf)" ]]; then
      sed -i "s|^default-character-set=.*|default-character-set=$SET_DEFAULT_MYSQLCHARSET|" /etc/my.cnf
    elif [[ ! "$(grep '^#default-character-set=' /etc/my.cnf)" ]]; then
      sed -i "s|\[mysqldump\]|\[mysqldump\]\ndefault-character-set=$SET_DEFAULT_MYSQLCHARSET|" /etc/my.cnf
    fi
  fi
}

remove_anonymous_mysqlusers() {
  # Check for anonymous users
  ANON_USER_EXISTS=$(mysql -BNe "SELECT User, Host FROM mysql.user WHERE User=''" | wc -l)
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ "$(systemctl is-active mariadb)" = 'active' && "$ANON_USER_EXISTS" -gt '1' ]]; then
    mysql -e "DROP USER ''@'localhost';" >/dev/null 2>&1
    mysql -e "DROP USER ''@'`hostname`';" >/dev/null 2>&1
  fi
}

securemysql() {
  if [[ "$(systemctl is-active mariadb)" != 'active' ]]; then
    systemctl start mariadb
  fi
THEMYSQLPASS=$(/usr/bin/openssl rand -base64 26 | tr -dc 'a-zA-Z0-9')

# mysql -e "DELETE FROM mysql.user WHERE User=""; FLUSH PRIVILEGES;"
# mysqladmin -u root password $THEMYSQLPASS

CHECK_FOR_MARIADB_TENFOUR_ABOVE=$(rpm -qa | grep -i MariaDB-server | head -n1 | cut -d '-' -f3 | cut -d . -f1,2)

if [[ "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.4' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.5' || "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" = '10.6' ]]; then
  echo
  mysql -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-before.log"
  echo
  mysql -e "ALTER USER root@localhost IDENTIFIED VIA mysql_native_password USING PASSWORD('$THEMYSQLPASS')"  >/dev/null 2>&1
  echo
  mysql -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-after.log"
  echo
  remove_anonymous_mysqlusers
else
  remove_anonymous_mysqlusers
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