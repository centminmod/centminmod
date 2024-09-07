MYSQLHOSTNAME='localhost'
MYSQLEXTRA_FILE='/root/.my.cnf'

confirm_and_execute() {
  local perm_func=$1
  local action_func=$2

  while true; do
    read -p "Do you want to proceed with this option? [y/n]: " confirm
    if [[ "$confirm" =~ ^[Yy]$ ]]; then
      $perm_func
      $action_func
      return
    elif [[ "$confirm" =~ ^[Nn]$ ]]; then
      # echo "Returning to MySQL Admin Menu..."
      # sleep 1
      return
    else
      echo "Invalid input. Please enter 'y' or 'n'."
    fi
  done
}

# Define the new submenu for option 6
mysql_admin_menu() {
  while true; do
    clear
    cecho "----------------------------------------------------------------" $boldyellow
    echo " MySQL User Database Management Sub Menu"
    cecho "----------------------------------------------------------------" $boldyellow
    echo " Command Line Version community.centminmod.com/threads/543/"
    cecho "----------------------------------------------------------------" $boldyellow
    cecho "1). Create MySQL User and Database" $boldgreen
    cecho "2). Add Database to Existing MySQL User" $boldgreen
    cecho "3). Add Existing Database to Existing MySQL User" $boldgreen
    cecho "4). Add Existing Database to New MySQL User" $boldgreen
    cecho "5). Change MySQL User Password" $boldgreen
    cecho "6). Delete MySQL User" $boldgreen
    cecho "7). Show Grants for MySQL User" $boldgreen
    cecho "8). Create Global MySQL User (access to all databases)" $boldgreen
    cecho "9). Create Multiple MySQL Databases and Users from File" $boldgreen
    cecho "10). Delete MySQL Database" $boldgreen
    cecho "11). List All MySQL Databases and Users" $boldgreen
    cecho "12). Return to Main Menu" $boldgreen
    cecho "----------------------------------------------------------------" $boldyellow
    read -p "Enter option [ 1 - 12 ]: " mysql_admin_option

    case $mysql_admin_option in
      1)
        confirm_and_execute mysqlperm create_mysql_user_and_db
        ;;
      2)
        confirm_and_execute mysqlperm add_db_to_existing_mysql_user
        ;;
      3)
        confirm_and_execute mysqlperm add_existing_db_to_existing_mysql_user
        ;;
      4)
        confirm_and_execute mysqlperm add_existing_db_to_new_mysql_user
        ;;
      5)
        confirm_and_execute mysqlperm changeuserpass
        ;;
      6)
        confirm_and_execute mysqlperm delusername
        ;;
      7)
        confirm_and_execute mysqlperm listgrants
        ;;
      8)
        confirm_and_execute mysqlperm createuserglobal
        ;;
      9)
        confirm_and_execute mysqlperm multicreatedb
        ;;
      10)
        confirm_and_execute mysqlperm delete_mysql_database
        ;;
      11)
        confirm_and_execute mysqlperm list_mysql_databases_and_users
        ;;
      12)
        return
        ;;
      *)
        echo "Invalid option, please try again."
        ;;
    esac
  done
}

mysqlperm() {
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "MySQL Admin" $boldgreen
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

list_mysql_databases_and_users() {
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "List MySQL Databases, Creation Dates, and Associated Users" $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow

  if [[ "$rootset" = [yY] ]]; then
    # Get MySQL data directory
    datadir=$(${ALIAS_MYSQL} ${MYSQLOPTS} -Nse "SELECT @@datadir")

    # Get list of databases
    databases=$(${ALIAS_MYSQL} ${MYSQLOPTS} -Nse "SHOW DATABASES;" | grep -vE '^(information_schema|mysql|performance_schema|sys)$' | sort -V)
    
    for db in $databases; do
      # Use directory modification time as creation date
      if [[ -d "${datadir}${db}" ]]; then
        created_date=$(stat -c %y "${datadir}${db}" 2>/dev/null | cut -d. -f1)
      else
        created_date="N/A"
      fi

      # Get users with privileges on this database
      users=$(${ALIAS_MYSQL} ${MYSQLOPTS} -Nse "SELECT DISTINCT GRANTEE FROM information_schema.SCHEMA_PRIVILEGES WHERE TABLE_SCHEMA='$db'" | sed "s/'//g" | cut -d'@' -f1 | sort -V | uniq | tr '\n' ',' | sed 's/,/, /g' | sed 's/, $//')
      
      if [[ -z "$users" ]]; then
        echo "db: $db created: $created_date [users: none]"
      else
        echo "db: $db created: $created_date [users: $users]"
      fi
    done
  else
    cecho "Error: Root access is required to list databases and users." $boldred
  fi

  echo
  read -ep "Press [Enter] to continue..."
}

delete_mysql_database() {
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "Delete MySQL Database" $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow
  echo "WARNING: Ensure you have a MySQL database backup before deletion!"
  echo

  read -ep "Enter the name of the database you want to delete: " db_to_delete

  if [[ -z "$db_to_delete" ]]; then
    cecho "Error: No database name entered. Aborting." $boldred
    return
  fi

  # Check if the database exists
  if [[ "$rootset" = [yY] ]]; then
    db_exists=$(${ALIAS_MYSQL} ${MYSQLOPTS} -Nse "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '$db_to_delete'")
  else
    db_exists=$(mysql -Nse "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '$db_to_delete'")
  fi

  if [[ -z "$db_exists" ]]; then
    cecho "Error: Database '$db_to_delete' does not exist. Aborting deletion." $boldred
    read -ep "Press [Enter] to continue..."
    return
  fi

  cecho "Are you absolutely sure you want to delete the database '$db_to_delete'?" $boldred
  read -ep "Type the database name again to confirm deletion: " confirm_db_name

  if [[ "$db_to_delete" != "$confirm_db_name" ]]; then
    cecho "Error: Database names do not match. Aborting deletion." $boldred
    read -ep "Press [Enter] to continue..."
    return
  fi

  if [[ "$rootset" = [yY] ]]; then
    echo "DROP DATABASE \`$db_to_delete\`;" | ${ALIAS_MYSQL} ${MYSQLOPTS}
    ERROR=$?
  else
    echo "DROP DATABASE \`$db_to_delete\`;" | mysql
    ERROR=$?
  fi

  if [[ "$ERROR" != '0' ]]; then
    echo ""
    cecho "Error: Deleting database '$db_to_delete' was unsuccessful" $boldred
    echo
  else 
    echo ""
    cecho "Ok: Database '$db_to_delete' was successfully deleted" $boldgreen
    echo
  fi

  read -ep "Press [Enter] to continue..."
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
    echo "CREATE DATABASE \`$db\`;" | ${ALIAS_MYSQL} ${MYSQLOPTS} >/dev/null 2>&1
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
        ${ALIAS_MYSQL} ${MYSQLOPTS} -e "CREATE USER '$u'@'$MYSQLHOSTNAME' IDENTIFIED BY '$p';" >/dev/null 2>&1
        USERCHECK=$?
      elif [[ "$PREV_USER" != "$u" && "$PREV_PASS" = "$p" ]]; then
        # if PREV_USER not equal to $u AND PREV_PASS equal to $p
        # then it's not the same mysql username and pass so create the
        # mysql username
        ${ALIAS_MYSQL} ${MYSQLOPTS} -e "CREATE USER '$u'@'$MYSQLHOSTNAME' IDENTIFIED BY '$p';" >/dev/null 2>&1
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
        echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS} >/dev/null 2>&1
      elif [[ "$PREV_USER" != "$u" && "$PREV_PASS" = "$p" ]]; then
        # if PREV_USER not equal to $u AND PREV_PASS equal to $p
        echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS} >/dev/null 2>&1
        echo "$u $p" > /tmp/mysqladminshell_userpass.txt
      else
        # if PREV_USER not equal to $u AND PREV_PASS not equal to $p
        echo "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$db\`.* TO '$u'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$u'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS} >/dev/null 2>&1
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

  ${ALIAS_MYSQL} ${MYSQLOPTS} -e "CREATE USER '$globalnewmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$globalnewmysqluserpass';" >/dev/null 2>&1
  GLOBALUSERCHECK=$?
  if [[ "$GLOBALUSERCHECK" = '0' ]]; then
    ${ALIAS_MYSQL} ${MYSQLOPTS} -e "GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON *.* TO '$globalnewmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$globalnewmysqluser'@'$MYSQLHOSTNAME';"
    echo ""
    cecho "Ok: MySQL global user: $globalnewmysqluser created successfully" $boldyellow
    echo
  else
      cecho "Error: unable to create MySQL USER = $u with PASSWORD = $p" $boldgreen
      # GLOBALUSERCHECK=1
  fi
}

create_mysql_user_and_db() {
  cecho "---------------------------------" $boldyellow
  cecho "Create MySQL username:" $boldgreen
  cecho "---------------------------------" $boldyellow

  read -ep " Enter new MySQL username you want to create: " newmysqluser
  read -ep " Enter new MySQL username's password: " newmysqluserpass

  cecho "---------------------------------" $boldyellow
  cecho "Create MySQL database:" $boldgreen
  cecho "---------------------------------" $boldyellow
  read -ep " Enter new MySQL database name: " newdbname
  echo

  if [[ "$rootset" = [yY] ]]; then
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS}
  else
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql
  fi

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
}


add_db_to_existing_mysql_user() {
  cecho "-------------------------------------------------------------------------" $boldyellow
  cecho "Add new database name to existing MySQL username:" $boldgreen
  cecho "-------------------------------------------------------------------------" $boldyellow
  read -ep " Enter existing MySQL username you want to add new database name to: " existingmysqluser

  cecho "---------------------------------" $boldyellow
  cecho "Create MySQL database:" $boldgreen
  cecho "---------------------------------" $boldyellow
  read -ep " Enter new MySQL database name: " newdbname
  echo

  if [[ "$rootset" = [yY] ]]; then
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS}
  else
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql
  fi

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
}

add_existing_db_to_existing_mysql_user() {
  cecho "-------------------------------------------------------------------------" $boldyellow
  cecho "Add existing database name to existing MySQL username:" $boldgreen
  cecho "-------------------------------------------------------------------------" $boldyellow
  read -ep " Enter existing MySQL username you want to add existing database name to: " existingmysqluser
  read -ep " Enter existing MySQL database name to attach to MySQL user $existingmysqluser: " existingmysqldbname

  if [[ "$rootset" = [yY] ]]; then
    echo "USE \`$existingmysqldbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS}
  else
    echo "USE \`$existingmysqldbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    echo ""
    cecho "Error: command was unsuccessful" $boldgreen
    echo
  else 
    echo ""
    cecho "Ok: existing MySQL database: $existingmysqldbname assigned to existing MySQL user: $existingmysqluser" $boldyellow
    echo
  fi
}

add_existing_db_to_new_mysql_user() {
  cecho "-------------------------------------------------------------------------" $boldyellow
  cecho "Add existing database name to new MySQL username:" $boldgreen
  cecho "-------------------------------------------------------------------------" $boldyellow
  read -ep " Enter new MySQL username you want to create: " newmysqluser
  read -ep " Enter new MySQL username's password: " newmysqluserpass
  read -ep " Enter existing MySQL database name to attach to new MySQL user $newmysqluser: " existingmysqldbname

  if [[ "$rootset" = [yY] ]]; then
    echo "USE \`$existingmysqldbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS}
  else
    echo "USE \`$existingmysqldbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    echo ""
    cecho "Error: command was unsuccessful" $boldgreen
    echo
  else 
    echo ""
    cecho "Ok: MySQL user: $newmysqluser MySQL database: $existingmysqldbname created successfully" $boldyellow
    echo
  fi
}

changeuserpass() {
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "Change Existing MySQL username's password:" $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow
  echo
  echo "This is for non-root MySQL user password changes only"
  echo "For MySQL root user password changes, use centmin.sh"
  echo "menu option 11, submenu 0"
  echo

  read -ep " Enter MySQL username you want to change password for: " changemysqluserpass
  read -ep " Enter MySQL username's new password to change to: " changenewmysqlpass

  if [[ "$changemysqluserpass" = 'root' ]]; then
    echo
    echo "This is for non-root MySQL user password changes only"
    echo "For MySQL root user password changes, use centmin.sh"
    echo "menu option 11, submenu 0"
    echo
    exit
  fi

  if [[ "$rootset" = [yY] ]]; then
    ${ALIAS_MYSQL} ${MYSQLOPTS} -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"
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
    ${ALIAS_MYSQL} -e "set password for '$changemysqluserpass'@'$MYSQLHOSTNAME' = password('$changenewmysqlpass');"
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

  read -t 30 -ep " Enter MySQL username you want to delete (type exit to abort): " delmysqluser

  # Check if read command was interrupted by a signal
  if [[ $? -eq 128 ]]; then
      cecho "Aborted: User interrupted the input" $boldred
      return
  fi

  # Check if the delmysqluser variable is empty and return from the function if it is
  if [[ -z "$delmysqluser" || "$delmysqluser" = 'exit' || "$delmysqluser" = 'EXIT' ]]; then
      cecho " Aborted: No MySQL username entered" $boldred
      return
  fi

  if [[ "$rootset" = [yY] ]]; then
    echo "drop user '$delmysqluser'@'$MYSQLHOSTNAME'; flush privileges;" | ${ALIAS_MYSQL} ${MYSQLOPTS}
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
    echo "SHOW GRANTS for '$showmysqluser'@'$MYSQLHOSTNAME';" | ${ALIAS_MYSQL} ${MYSQLOPTS}
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