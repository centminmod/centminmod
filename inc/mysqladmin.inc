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
    cecho "10). Return to Main Menu" $boldgreen
    cecho "----------------------------------------------------------------" $boldyellow
    read -p "Enter option [ 1 - 10 ]: " mysql_admin_option

    case $mysql_admin_option in
      1)
        confirm_and_execute mysqlperm create_mysql_user_and_db
        # break
        ;;
      2)
        confirm_and_execute mysqlperm add_db_to_existing_mysql_user
        # break
        ;;
      3)
        confirm_and_execute mysqlperm add_existing_db_to_existing_mysql_user
        # break
        ;;
      4)
        confirm_and_execute mysqlperm add_existing_db_to_new_mysql_user
        # break
        ;;
      5)
        confirm_and_execute mysqlperm changeuserpass
        # break
        ;;
      6)
        confirm_and_execute mysqlperm delusername
        # break
        ;;
      7)
        confirm_and_execute mysqlperm listgrants
        # break
        ;;
      8)
        confirm_and_execute mysqlperm createuserglobal
        # break
        ;;
      9)
        confirm_and_execute mysqlperm multicreatedb
        # break
        ;;
      10)
        return
        ;;
      *)
        echo "Invalid option, please try again."
        ;;
    esac
    # read -p "Press [Enter] key to continue..."
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
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
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
    echo "CREATE DATABASE \`$newdbname\`; USE \`$newdbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$newdbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
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
    echo "USE \`$existingmysqldbname\`; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$existingmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$existingmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
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
    echo "USE \`$existingmysqldbname\`; CREATE USER '$newmysqluser'@'$MYSQLHOSTNAME' IDENTIFIED BY '$newmysqluserpass'; GRANT index, select, insert, delete, update, create, drop, alter, create temporary tables, execute, lock tables, create view, show view, create routine, alter routine, trigger ON \`$existingmysqldbname\`.* TO '$newmysqluser'@'$MYSQLHOSTNAME'; flush privileges; show grants for '$newmysqluser'@'$MYSQLHOSTNAME';" | mysql ${MYSQLOPTS}
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