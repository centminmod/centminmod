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
  # only query when MariaDB is running, otherwise the client prints ERROR 2002
  # to the terminal on every centmin.sh launch when the service is stopped
  if [[ "$(systemctl is-active mariadb 2>/dev/null)" != 'active' ]]; then
    return 0
  fi
  # Check for anonymous users
  ANON_USER_EXISTS=$(${ALIAS_MYSQL} -BNe "SELECT User, Host FROM mysql.user WHERE User=''" 2>/dev/null | wc -l)
  if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ "$ANON_USER_EXISTS" -gt '1' ]]; then
    ${ALIAS_MYSQL} -e "DROP USER ''@'localhost';" >/dev/null 2>&1
    ${ALIAS_MYSQL} -e "DROP USER ''@'`hostname`';" >/dev/null 2>&1
  fi
}

securemysql() {
  # if [[ "$(systemctl is-active mariadb)" != 'active' ]]; then
  #   systemctl start mariadb
  # fi
  if ! is_mariadb_active; then
      start_mariadb
  fi
THEMYSQLPASS=$(/usr/bin/openssl rand -base64 26 | tr -dc 'a-zA-Z0-9')

# ${ALIAS_MYSQL} -e "DELETE FROM mysql.user WHERE User=""; FLUSH PRIVILEGES;"
# ${ALIAS_MYSQLADMIN} -u root password $THEMYSQLPASS

CHECK_FOR_MARIADB_TENFOUR_ABOVE=$(rpm -qa | grep -i MariaDB-server | grep -v 'utils' | head -n1 | cut -d '-' -f3 | cut -d . -f1,2)

# any MariaDB >= 10.4 (incl 11.x/12.x) uses mysql.global_priv + ALTER USER
MARIADB_TENFOUR_ABOVE_SECURED='n'
if [[ -n "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" && "$(printf '%s\n' 10.4 "$CHECK_FOR_MARIADB_TENFOUR_ABOVE" | sort -V | head -n1)" = '10.4' ]]; then
  MARIADB_TENFOUR_ABOVE_SECURED='y'
  echo
  ${ALIAS_MYSQL} -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-before.log"
  echo
  ${ALIAS_MYSQL} -e "ALTER USER root@localhost IDENTIFIED VIA mysql_native_password USING PASSWORD('$THEMYSQLPASS')"  >/dev/null 2>&1
  echo
else
  remove_anonymous_mysqlusers
  ${ALIAS_MYSQL} -e "DROP DATABASE test;" >/dev/null 2>&1
  ${ALIAS_MYSQL} -e "UPDATE mysql.user SET Password = PASSWORD('$THEMYSQLPASS') WHERE User = 'root'; FLUSH PRIVILEGES;" >/dev/null 2>&1
fi

# check
# ${ALIAS_MYSQL} -e "SELECT User, Host, Password FROM mysql.user;"

# credentials files are root-only (0600) before any secret is written
touch /root/.my.cnf "${CENTMINLOGDIR}/set-dot-my.cnf.log"
chmod 600 /root/.my.cnf "${CENTMINLOGDIR}/set-dot-my.cnf.log"
cat > "/root/.my.cnf" <<EOF
[client]
user=root
password=$THEMYSQLPASS
EOF

# root now authenticates by password only, so the post-ALTER check
# can only run once /root/.my.cnf holds the new password
if [[ "$MARIADB_TENFOUR_ABOVE_SECURED" = 'y' ]]; then
  # global_priv output includes the root password hash
  touch "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-after.log"
  chmod 600 "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-after.log"
  ${ALIAS_MYSQL} -e "select concat(user, '@', host, ' => ', json_detailed(priv)) from mysql.global_priv;" > "${CENTMINLOGDIR}/mariadb10.4-or-above-check-users-after.log"
  echo
  remove_anonymous_mysqlusers
fi

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