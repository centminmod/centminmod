##############################################
# functions to disable or uninstall centmin mod installed software
# experimental functions not 100% tested yet
##############################################

nginxoff() {
service nginx stop
chkconfig nginx off
}

phpfpmoff() {
service php-fpm stop
chkconfig php-fpm off
}

mariadboff() {
service mysql stop
chkconfig mysql off
}

mysqloff() {
service mysqld stop
chkconfig mysqld off
}

csfoff() {
service csf stop
chkconfig csf off

service lfd stop
chkconfig lfd off
}

memcachedoff() {
service memcached stop
chkconfig memcached off
}

nsdoff() {
service nsd stop
chkconfig nsd off
}

read -ep "Do you want to disable or uninstall Centmin Mod installed software ? Type either [disable or uninstall] " offoruninstall

if [[ "$offoruninstall" = 'disable' ]]; then




else

echo "Uninstall software..."

fi


