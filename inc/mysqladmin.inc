# http://dev.mysql.com/doc/refman/5.1/en/account-management-sql.html
# http://dev.mysql.com/doc/refman/5.1/en/create-user.html
# http://dev.mysql.com/doc/refman/5.1/en/drop-user.html
# http://dev.mysql.com/doc/refman/5.1/en/grant.html
# http://dev.mysql.com/doc/refman/5.1/en/revoke.html
# http://dev.mysql.com/doc/refman/5.1/en/set-password.html

function funct_mysqladmin {

MYSQLHOSTNAME='localhost'

echo "*************************************************"
cecho "Basic MySQL Admin - create mysql user & databases " $boldgreen
echo "*************************************************"

read -ep "Do you have mysql root user password set ? [y/n]: " rootset

if [[ "$rootset" = [yY] ]]; then

read -ep "Enter your mysql root username i.e. root: " myrootuser
read -ep "Enter your mysql root password: " myrootpass

else

rootset='n'

fi

echo "*************************************************"

}

function funct_createuserdb {

read -ep "Do you want to create a new MySQL username (y) or Add a new database name to existing MySQL username (n) ? " createnewuser

if [[ "$createnewuser" = [yY] ]]; then

echo "---------------------------------"
echo "Create MySQL username:"
echo "---------------------------------"

read -ep "Enter new MySQL username you want to create: " newmysqluser
read -ep "Enter new MySQL username's password: " newmysqluserpass

else

createnewuser='n'

echo "-------------------------------------------------------------------------"
echo "Add new database name to existing MySQL username:"
echo "-------------------------------------------------------------------------"

read -ep "Enter existing MySQL username you want to add new database name to: " existingmysqluser

fi

echo "---------------------------------"
echo "Create MySQL database:"
echo "---------------------------------"

read -ep "Enter new MySQL database name: " newdbname

echo

if [[ "$rootset" = [yY] && "$createnewuser" = [yY] ]]; then

mysql -u$myrootuser -p$myrootpass -e "CREATE DATABASE $newdbname; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';"

elif [[ "$rootset" = [nN] && "$createnewuser" = [yY] ]]; then

mysql -e "CREATE DATABASE $newdbname; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';"

elif [[ "$rootset" = [nN] && "$createnewuser" = [nN] ]]; then

mysql -e "CREATE DATABASE $newdbname; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';"

elif [[ "$rootset" = [yY] && "$createnewuser" = [nN] ]]; then

mysql -u$myrootuser -p$myrootpass -e "CREATE DATABASE $newdbname; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';"

fi

}

function funct_changeuserpass {

echo "--------------------------------------------------------------"
echo "Change Existing MySQL username's password:"
echo "--------------------------------------------------------------"

read -ep "Enter MySQL username you want to change password for: " changemysqluserpass
read -ep "Enter MySQL username's new password to change to: " changenewmysqlpass

if [[ "$rootset" = [yY] ]]; then

mysql -u$myrootuser -p$myrootpass -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"

else

mysql -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"

fi

}

function funct_delusername {

echo "---------------------------------"
echo "Delete MySQL username:"
echo "---------------------------------"

read -ep "Enter MySQL username you want to delete: " delmysqluser

if [[ "$rootset" = [yY] ]]; then

mysql -u$myrootuser -p$myrootpass -e "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;"

else

mysql -e "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;"

fi

}