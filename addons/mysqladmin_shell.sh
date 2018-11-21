#!/bin/bash
VER=0.1.0
###############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
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

CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
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

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

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
	_thedbfile="$1"
	cecho "----------------------------------------------------------------------------" $boldyellow
	cecho "Create Multiple MySQL Databases, User & Pass From specified filepath/name" $boldgreen
	cecho "i.e. /home/nginx/domains/domain.com/dbfile.txt" $boldgreen
	cecho "One entry per line in dbfile.txt in format of:" $boldgreen
	cecho "databasename databaseuser databasepass" $boldgreen
	cecho "----------------------------------------------------------------------------" $boldyellow

  if [[ -z "$_thedbfile" || ! -f "$_thedbfile" ]]; then
		echo
		read -ep " Enter full path to db list file i.e. /home/nginx/domains/domain.com/dbfile.txt (to exit type = x): " dbfile
		echo
	else
		dbfile="$_thedbfile"
  fi

if [[ "$dbfile" = [xX] || -z "$dbfile" ]]; then
	exit
fi

if [[ "$rootset" = [yY] && -f "$dbfile" ]]; then
	sort -k2 $dbfile | while read -r db u p; do
		echo "CREATE DATABASE \`$db\`;" | mysql ${MYSQLOPTS} >/dev/null 2>&1
		DBCHECK=$?
		if [[ "$DBCHECK" = '0' ]]; then
			if [ -f /tmp/mysqladminshell_userpass.txt ]; then
				PREV_USER=$(awk '{print $1}' /tmp/mysqladminshell_userpass.txt)
				PREV_PASS=$(awk '{print $2}' /tmp/mysqladminshell_userpass.txt)
			fi
			if [[ "$PREV_USER" != "$u" && "$PREV_PASS" != "$p" ]]; then
				# if PREV_USER not equal to $u AND PREV_PASS not equal to $p
				# then it's not the same mysql username and pass so create the
				# mysql username
				mysql ${MYSQLOPTS} -e "CREATE USER '$u'@'$MYSQLHOSTNAME' IDENTIFIED BY '$p';" >/dev/null 2>&1
				USERCHECK=$?
			elif [[ "$PREV_USER" != "$u" && "$PREV_PASS" = "$p" ]]; then
				# if PREV_USER not equal to $u AND PREV_PASS equal to $p
				# then it's not the same mysql username and pass so create the
				# mysql username
				mysql ${MYSQLOPTS} -e "CREATE USER '$u'@'$MYSQLHOSTNAME' IDENTIFIED BY '$p';" >/dev/null 2>&1
				USERCHECK=$?
			elif [[ "$PREV_USER" = "$u" && "$PREV_PASS" = "$p" ]]; then
				# if PREV_USER equal to $u AND PREV_PASS equal to $p
				# then it's same mysql username and pass so skip
				# mysql user creation
				USERCHECK=0
			elif [[ -z "$u" && -z "$p" ]]; then
				# if mysql username and password empty
				# skip mysql user creation
				USERCHECK=0
			fi
		else
			cecho "Error: unable to create DATABASE = $db" $boldgreen
			USERCHECK=1
		fi
		if [[ "$USERCHECK" = '0' ]]; then
			if [ -f /tmp/mysqladminshell_userpass.txt ]; then
				PREV_USER=$(awk '{print $1}' /tmp/mysqladminshell_userpass.txt)
				PREV_PASS=$(awk '{print $2}' /tmp/mysqladminshell_userpass.txt)
			fi
			if [[ "$PREV_USER" = "$u" && "$PREV_PASS" = "$p" ]]; then
				# if PREV_USER equal to $u AND PREV_PASS equal to $p
				# then it's same mysql username and pass so add database
				# to existing mysql user and pass
				echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS} >/dev/null 2>&1
			elif [[ "$PREV_USER" != "$u" && "$PREV_PASS" = "$p" ]]; then
				# if PREV_USER not equal to $u AND PREV_PASS equal to $p
				echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS} >/dev/null 2>&1
				echo "$u $p" > /tmp/mysqladminshell_userpass.txt
			else
				# if PREV_USER not equal to $u AND PREV_PASS not equal to $p
				echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS} >/dev/null 2>&1
				echo "$u $p" > /tmp/mysqladminshell_userpass.txt
			fi
		elif [[ "$DBCHECK" = '0' && "$USERCHECK" != '0' ]]; then
			cecho "Error: unable to create MySQL USER = $u with PASSWORD = $p" $boldgreen
			USERCHECK=1
		fi

		ERROR=$(echo "$DBCHECK+$USERCHECK"|bc)
		if [[ "$ERROR" != '0' ]]; then
			# echo ""
			cecho "Error: $0 multidb run was unsuccessful" $boldgreen
			echo
		else 
			echo ""
			if [[ -z "$u" && -z "$p" ]]; then
				cecho "---------------------------------" $boldgreen
				cecho "Ok: MySQL user: skipped MySQL database: $db created successfully" $boldyellow
			else
				cecho "---------------------------------" $boldgreen
				cecho "Ok: MySQL user: $u MySQL database: $db created successfully" $boldyellow
			fi
			echo
		fi
	done
	rm -rf /tmp/mysqladminshell_userpass.txt
fi
}

createuserglobal() {
	echo
	cecho "Create a MySQL Username that has access to all Databases" $boldyellow
	cecho "But without SUPER ADMIN privileges" $boldyellow
	echo
	read -ep " Enter new MySQL username you want to create: " globalnewmysqluser
	read -ep " Enter new MySQL username's password: " globalnewmysqluserpass

	mysql ${MYSQLOPTS} -e "CREATE USER '$globalnewmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$globalnewmysqluserpass';" >/dev/null 2>&1
	GLOBALUSERCHECK=$?
	if [[ "$GLOBALUSERCHECK" = '0' ]]; then
		mysql ${MYSQLOPTS} -e "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON *.* TO '$globalnewmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$globalnewmysqluser'@'$MYSQLHOSTNAME';"
		echo ""
		cecho "Ok: MySQL global user: $globalnewmysqluser created successfully" $boldyellow
		echo
	else
			cecho "Error: unable to create MySQL USER = $u with PASSWORD = $p" $boldgreen
			# GLOBALUSERCHECK=1
	fi
}

createuserdb() {
	UNATTENDED=$1
	cinput_dbname=$2
	cinput_dbuser=$3
	cinput_dbpass=$4

	if [[ "$UNATTENDED" = 'unattended' ]]; then
		MYSQLOPTS="--defaults-extra-file=${MYSQLEXTRA_FILE}"
		rootset=y
		createnewuser=y
		newmysqluser=$cinput_dbuser
		newmysqluserpass=$cinput_dbpass
		newdbname=$cinput_dbname
	fi

	if [[ "$UNATTENDED" != 'unattended' ]]; then
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
	fi # if UNATTENDED != unattended
		
		if [[ "$rootset" = [yY] && "$createnewuser" = [yY] ]]; then
	echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
		
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
	echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
		
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
	echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$existingmysqluser'@'$		MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
		
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
	echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$existingmysqluser'@'$		MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
		
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

echo "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;" | mysql ${MYSQLOPTS}

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

echo "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;" | mysql

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

echo "SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}

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

echo "SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME';" | mysql

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

cleanup() {
	rm -rf /tmp/mysqladminshell_userpass.txt
}

trap cleanup SIGHUP SIGINT SIGTERM
###############################################################
case "$1" in
	multidb)
		mysqlperm
		if [[ -f "$2" ]]; then
			multicreatedb $2
		else
			multicreatedb
		fi
		;;	
	setglobaluser)
		mysqlperm
		createuserglobal
		;;
	setuserdb)
		mysqlperm
		createuserdb
		;;
	createuserdb)
		input_dbname=$2
		input_dbuser=$3
		input_dbpass=$4
		mysqlperm
		createuserdb unattended $input_dbname $input_dbuser $input_dbpass
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
		cecho "$0 {multidb|setglobaluser|setuserdb|setpass|createuserdb|deluser|showgrants}" $boldyellow
		echo ""
cecho "multidb - multiple mysql databse/user creation mode passing a file name containing db, user, pass 3 column entries
setglobaluser - create a mysql username with access to all databases on server without SUPER ADMIN privileges (non-root)
setuserdb - create individual mysql username and databases or assign a new database to an existing mysql username
setpass - change mysql username password
createuserdb - unattended create individual mysql username & databases fields required are dbname dbuser dbpass
deluser - delete a mysql usernames
showgrants - show existing mysql username granted privileges" $boldyellow
		;;
esac
exit