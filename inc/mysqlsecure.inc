securemysql() {
THEMYSQLPASS=$(openssl rand 12 -base64)

# mysql -e "DELETE FROM mysql.user WHERE User=""; FLUSH PRIVILEGES;"
# mysqladmin -u root password $THEMYSQLPASS

mysql -e "DROP USER ''@'localhost';"
mysql -e "DROP USER ''@'`hostname`';"
mysql -e "DROP DATABASE test;"
mysql -e "UPDATE mysql.user SET Password = PASSWORD('$THEMYSQLPASS') WHERE User = 'root'; FLUSH PRIVILEGES;"

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