securemysql() {
THEMYSQLPASS=$(openssl rand 21 -base64 | tr -dc 'a-zA-Z0-9')

# mysql -e "DELETE FROM mysql.user WHERE User=""; FLUSH PRIVILEGES;"
# mysqladmin -u root password $THEMYSQLPASS

mysql -e "DROP USER ''@'localhost';" >/dev/null 2>&1
mysql -e "DROP USER ''@'`hostname`';" >/dev/null 2>&1
mysql -e "DROP DATABASE test;" >/dev/null 2>&1
mysql -e "UPDATE mysql.user SET Password = PASSWORD('$THEMYSQLPASS') WHERE User = 'root'; FLUSH PRIVILEGES;" >/dev/null 2>&1

# check
# mysql -e "SELECT User, Host, Password FROM mysql.user;"

cat > "/root/.my.cnf" <<EOF
[client]
user=root
password=$THEMYSQLPASS
EOF

echo ""
cecho "-------------------------------------------------------" $boldyellow
cecho "MySQL root user password set" $boldgreen
cecho "MySQL root user added to /root/.my.cnf" $boldgreen
cecho "-------------------------------------------------------" $boldyellow
cecho "MySQL root password: $THEMYSQLPASS" $boldgreen
cecho "-------------------------------------------------------" $boldyellow
}