#!/bin/bash
VER=0.0.2
###############################################################
# mysqladmin shell Centmin Mod Addon for centminmod.com users
# create new mysql username and assign standard
# permissions to specified database name
# written by George Liu (eva2000) centminmod.com
#
# How to Use
# https://community.centminmod.com/threads/543
###############################################################
# http://dev.mysql.com/doc/refman/5.1/en/account-management-sql.html
# http://dev.mysql.com/doc/refman/5.1/en/create-user.html
# http://dev.mysql.com/doc/refman/5.1/en/drop-user.html
# http://dev.mysql.com/doc/refman/5.1/en/grant.html
# http://dev.mysql.com/doc/refman/5.1/en/revoke.html
# http://dev.mysql.com/doc/refman/5.1/en/set-password.html
###############################################################
# variables
#############
MYSQLHOSTNAME='localhost'

MYSQLEXTRA_FILE='/root/.my.cnf'
###############################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}
###############################################################
# functions
#############

mysqlperm() {

cecho "--------------------------------------------------------------" $boldyellow
cecho "Basic MySQL Admin - create mysql user & databases " $boldgreen
cecho "--------------------------------------------------------------" $boldyellow

if [ ! -f "$MYSQLEXTRA_FILE" ]; then
	read -ep " Do you have mysql root user password set ? [y/n]: " rootset
	
	if [[ "$rootset" = [yY] ]]; then
		read -ep " Enter your mysql root username i.e. root: " myrootuser
		read -ep " Enter your mysql root password: " myrootpass
		MYSQLOPTS="-u$myrootuser -p$myrootpass"
	else
		rootset='n'
	fi
else
	MYSQLOPTS="--defaults-extra-file=${MYSQLEXTRA_FILE}"
	rootset=y
fi

cecho "--------------------------------------------------------------" $boldyellow
echo ""

}

multicreatedb() {
	cecho "----------------------------------------------------------------------------" $boldyellow
	cecho "Create Multiple MySQL Databases, User & Pass From specified filepath/name" $boldgreen
	cecho "i.e. /home/nginx/domains/domain.com/dbfile.txt" $boldgreen
	cecho "One entry per line in dbfile.txt in format of:" $boldgreen
	cecho "databasename databaseuser databasepass" $boldgreen
	cecho "----------------------------------------------------------------------------" $boldyellow

	echo
	read -ep " Enter full path to db list file i.e. /home/nginx/domains/domain.com/dbfile.txt (to exit type = x): " dbfile
	echo

if [[ "$dbfile" = [xX] || -z "$dbfile" ]]; then
	exit
fi

if [[ "$rootset" = [yY] && -f "$dbfile" ]]; then
	while read -r db u p; do
		mysql ${MYSQLOPTS} -e "CREATE DATABASE $db; CREATE USER '$u'@'$MYSQLHOSTNAME' IDENTIFIED BY '$p'; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $db.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';"

		ERROR=$?
		if [[ "$ERROR" != '0' ]]; then
			echo ""
			cecho "Error: command was unsuccessful" $boldgreen
			echo
		else 
			echo ""
			cecho "Ok: MySQL user: $newmysqluser MySQL database: $db created successfully" $boldyellow
			echo
		fi
	done < $dbfile
fi
}

createuserdb() {

read -ep " Do you want to create a new MySQL username (type = y) or `echo $'\n '`Add a new database name to existing MySQL username (type = n) ? `echo $'\n '`Enter y or n: " createnewuser

if [[ "$createnewuser" = [yY] ]]; then
	cecho "---------------------------------" $boldyellow
	cecho "Create MySQL username:" $boldgreen
	cecho "---------------------------------" $boldyellow

	read -ep " Enter new MySQL username you want to create: " newmysqluser
	read -ep " Enter new MySQL username's password: " newmysqluserpass
else
	createnewuser='n'
	cecho "-------------------------------------------------------------------------" $boldyellow
	cecho "Add new database name to existing MySQL username:" $boldgreen
	cecho "-------------------------------------------------------------------------" $boldyellow
	read -ep " Enter existing MySQL username you want to add new database name to: " existingmysqluser
fi

cecho "---------------------------------" $boldyellow
cecho "Create MySQL database:" $boldgreen
cecho "---------------------------------" $boldyellow

read -ep " Enter new MySQL database name: " newdbname

echo

if [[ "$rootset" = [yY] && "$createnewuser" = [yY] ]]; then
	mysql ${MYSQLOPTS} -e "CREATE DATABASE $newdbname; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';"

	ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: command was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: MySQL user: $newmysqluser MySQL database: $newdbname created successfully" $boldyellow
		echo
	fi

elif [[ "$rootset" = [nN] && "$createnewuser" = [yY] ]]; then
	mysql -e "CREATE DATABASE $newdbname; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';"

	ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: command was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: MySQL user: $newmysqluser MySQL database: $newdbname created successfully" $boldyellow
		echo
	fi

elif [[ "$rootset" = [nN] && "$createnewuser" = [nN] ]]; then
	mysql -e "CREATE DATABASE $newdbname; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';"

	ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: command was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: New MySQL database: $newdbname assigned to existing MySQL user: $existingmysqluser" $boldyellow
		echo
	fi

elif [[ "$rootset" = [yY] && "$createnewuser" = [nN] ]]; then
	mysql ${MYSQLOPTS} -e "CREATE DATABASE $newdbname; GRANT select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables ON $newdbname.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';"

	ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: command was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: New MySQL database: $newdbname assigned to existing MySQL user: $existingmysqluser" $boldyellow
		echo
	fi

fi

}

changeuserpass() {

cecho "--------------------------------------------------------------" $boldyellow
cecho "Change Existing MySQL username's password:" $boldgreen
cecho "--------------------------------------------------------------" $boldyellow

read -ep " Enter MySQL username you want to change password for: " changemysqluserpass
read -ep " Enter MySQL username's new password to change to: " changenewmysqlpass

if [[ "$rootset" = [yY] ]]; then

mysql ${MYSQLOPTS} -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: Changing MySQL password for $changemysqluserpass was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: Changing MySQL password for $changemysqluserpass was successful" $boldyellow
		echo
	fi

else

mysql -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: Changing MySQL password for $changemysqluserpass was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: Changing MySQL password for $changemysqluserpass was successful" $boldyellow
		echo
	fi

fi

}

delusername() {

cecho "---------------------------------" $boldyellow
cecho "Delete MySQL username:" $boldgreen
cecho "---------------------------------" $boldyellow

read -ep " Enter MySQL username you want to delete: " delmysqluser

if [[ "$rootset" = [yY] ]]; then

mysql ${MYSQLOPTS} -e "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: Deleting '$delmysqluser'@'$MYSQLHOSTNAME' was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: Deleting '$delmysqluser'@'$MYSQLHOSTNAME' was successful" $boldyellow
		echo
	fi

else

mysql -e "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: Deleting '$delmysqluser'@'$MYSQLHOSTNAME' was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: Deleting '$delmysqluser'@'$MYSQLHOSTNAME' was successful" $boldyellow
		echo
	fi

fi

}

listgrants() {

cecho "---------------------------------" $boldyellow
cecho "Show Grants for MySQL username:" $boldgreen
cecho "---------------------------------" $boldyellow

read -ep " Enter MySQL username to Show Grant permissions: " showmysqluser

if [[ "$rootset" = [yY] ]]; then

mysql ${MYSQLOPTS} -e "SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME';"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME' was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME' was successful" $boldyellow
		echo
	fi

else

mysql -e "SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME';"

ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
		echo ""
		cecho "Error: SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME' was unsuccessful" $boldgreen
		echo
	else 
		echo ""
		cecho "Ok: SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME' was successful" $boldyellow
		echo
	fi

fi

}

###############################################################
case "$1" in
	multidb)
		mysqlperm
		multicreatedb
		;;	
	setuserdb)
		mysqlperm
		createuserdb
		;;
	setpass)
		mysqlperm
		changeuserpass
		;;
	deluser)
		mysqlperm
		delusername
		;;
	showgrants)
		mysqlperm
		listgrants
		;;
	*)
		echo ""
		cecho "$0 {multidb|setuserdb|setpass|deluser|showgrants}" $boldyellow
		echo ""
		;;
esac
exit