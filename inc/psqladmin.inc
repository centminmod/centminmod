#!/bin/bash
# shellcheck source=/dev/null
# shellcheck disable=SC2154
#######################################################################################
# PostgreSQL Admin Sub Menu
# File: /inc/psqladmin.inc
# Modeled on /inc/mysqladmin.inc pattern
#######################################################################################

# Future-use variables for extended connection options
export PGHOST='localhost'
export PGEXTRA_FILE='/var/lib/pgsql/.pgpass'
export PGSQL_SUPERUSER='postgres'

pgsql_confirm_and_execute() {
  local perm_func=$1
  local action_func=$2

  while true; do
    read -rp "Do you want to proceed with this option? [y/n]: " confirm
    if [[ "$confirm" =~ ^[Yy]$ ]]; then
      $perm_func
      $action_func
      return
    elif [[ "$confirm" =~ ^[Nn]$ ]]; then
      return
    else
      echo "Invalid input. Please enter 'y' or 'n'."
    fi
  done
}

pgsql_admin_menu() {
  while true; do
    clear
    cecho "----------------------------------------------------------------" "$boldyellow"
    echo " PostgreSQL User Database Management Sub Menu"
    cecho "----------------------------------------------------------------" "$boldyellow"
    cecho "1). Create PostgreSQL Role and Database" "$boldgreen"
    cecho "2). Add Database to Existing Role" "$boldgreen"
    cecho "3). Change PostgreSQL Role Password" "$boldgreen"
    cecho "4). Delete PostgreSQL Role" "$boldgreen"
    cecho "5). Show Grants for PostgreSQL Role" "$boldgreen"
    cecho "6). List All Databases and Roles" "$boldgreen"
    cecho "7). Delete PostgreSQL Database" "$boldgreen"
    cecho "8). Return to Main Menu" "$boldgreen"
    cecho "----------------------------------------------------------------" "$boldyellow"
    read -rp "Enter option [ 1 - 8 ]: " pgsql_admin_option

    case $pgsql_admin_option in
      1)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm create_pgsql_role_and_db
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_create_role_db.log"
        ;;
      2)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm add_db_to_existing_role
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_add_db.log"
        ;;
      3)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm change_role_password
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_change_password.log"
        ;;
      4)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm delete_role
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_delete_role.log"
        ;;
      5)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm list_role_grants
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_list_grants.log"
        ;;
      6)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm list_databases_and_roles
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_list_all.log"
        ;;
      7)
        centminlog
        {
          pgsql_confirm_and_execute pgsqlperm delete_database
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_delete_db.log"
        ;;
      8) return ;;
      *) echo "Invalid option, please try again." ;;
    esac
  done
}

# Input validation: PostgreSQL identifier (role name, database name)
# Must start with letter/underscore, contain only alphanumeric/underscore, max 63 chars
validate_pg_identifier() {
  local name="$1"
  if [[ ! "$name" =~ ^[a-zA-Z_][a-zA-Z0-9_]{0,62}$ ]]; then
    return 1
  fi
  return 0
}

# Escape single quotes for SQL string literals (prevents SQL injection)
escape_sql_string() {
  echo "${1//\'/\'\'}"
}

pgsqlperm() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "PostgreSQL Admin" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  # Check for postgres superuser access via peer authentication
  if sudo -u postgres psql -c '\q' 2>/dev/null; then
    PGSQL_AUTH="peer"
    echo "Using peer authentication as postgres user"
  else
    # Use -sp for silent password input (security best practice)
    read -rsp " Enter PostgreSQL superuser password: " pgpass
    echo  # newline after silent input
    export PGPASSWORD="$pgpass"
    PGSQL_AUTH="password"
  fi

  cecho "--------------------------------------------------------------" "$boldyellow"
  echo ""
}

# Clear PGPASSWORD after operations (call at end of each function)
pgsql_cleanup() {
  if [[ -n "$PGPASSWORD" ]]; then
    unset PGPASSWORD
  fi
}

create_pgsql_role_and_db() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Create PostgreSQL Role and Database" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  read -rep "Enter new role name: " newrole
  read -rsp "Enter password for role: " rolepass
  echo  # newline after silent input
  read -rep "Enter database name: " dbname

  if [[ -z "$newrole" || -z "$rolepass" || -z "$dbname" ]]; then
    cecho "Error: All fields are required. Aborting." "$boldred"
    pgsql_cleanup
    return
  fi

  # Validate identifiers to prevent SQL injection
  if ! validate_pg_identifier "$newrole"; then
    cecho "Error: Invalid role name (use letters, numbers, underscore; start with letter/underscore)" "$boldred"
    pgsql_cleanup
    return
  fi

  if ! validate_pg_identifier "$dbname"; then
    cecho "Error: Invalid database name (use letters, numbers, underscore; start with letter/underscore)" "$boldred"
    pgsql_cleanup
    return
  fi

  # Escape password to prevent SQL injection
  rolepass_escaped=$(escape_sql_string "$rolepass")

  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql <<EOF
CREATE ROLE "$newrole" WITH LOGIN PASSWORD '${rolepass_escaped}';
CREATE DATABASE "$dbname" OWNER "$newrole";
GRANT ALL PRIVILEGES ON DATABASE "$dbname" TO "$newrole";
\c "$dbname"
GRANT ALL ON SCHEMA public TO "$newrole";
EOF
  else
    psql -h localhost -U postgres <<EOF
CREATE ROLE "$newrole" WITH LOGIN PASSWORD '${rolepass_escaped}';
CREATE DATABASE "$dbname" OWNER "$newrole";
GRANT ALL PRIVILEGES ON DATABASE "$dbname" TO "$newrole";
\c "$dbname"
GRANT ALL ON SCHEMA public TO "$newrole";
EOF
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: Creating role/database was unsuccessful" "$boldred"
  else
    cecho "Ok: Role '$newrole' and database '$dbname' created successfully" "$boldgreen"
    echo ""
    echo "Connection string:"
    echo "postgresql://${newrole}:<password>@localhost:5432/${dbname}"
    echo "(Password hidden for security)"
  fi

  pgsql_cleanup
  echo
  read -rep "Press [Enter] to continue..."
}

list_databases_and_roles() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "List PostgreSQL Databases and Roles" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  echo ""
  echo "=== PostgreSQL Databases ==="
  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "\l"
  else
    psql -h localhost -U postgres -c "\l"
  fi

  echo ""
  echo "=== PostgreSQL Roles ==="
  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "\du"
  else
    psql -h localhost -U postgres -c "\du"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}

change_role_password() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Change PostgreSQL Role Password" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  read -rep "Enter role name: " rolename
  read -rsp "Enter new password: " newpass
  echo  # newline after silent input

  if [[ -z "$rolename" || -z "$newpass" ]]; then
    cecho "Error: Role name and password are required. Aborting." "$boldred"
    pgsql_cleanup
    return
  fi

  # Validate role name
  if ! validate_pg_identifier "$rolename"; then
    cecho "Error: Invalid role name" "$boldred"
    pgsql_cleanup
    return
  fi

  # Escape password to prevent SQL injection
  newpass_escaped=$(escape_sql_string "$newpass")

  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "ALTER ROLE \"$rolename\" WITH PASSWORD '${newpass_escaped}';"
  else
    psql -h localhost -U postgres -c "ALTER ROLE \"$rolename\" WITH PASSWORD '${newpass_escaped}';"
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: Changing password was unsuccessful" "$boldred"
  else
    cecho "Ok: Password for role '$rolename' changed successfully" "$boldgreen"
  fi

  pgsql_cleanup
  echo
  read -rep "Press [Enter] to continue..."
}

delete_role() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Delete PostgreSQL Role" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"
  echo "WARNING: This will permanently delete the role!"
  echo

  read -rep "Enter role name to delete: " rolename

  if [[ -z "$rolename" ]]; then
    cecho "Error: No role name entered. Aborting." "$boldred"
    return
  fi

  cecho "Are you sure you want to delete role '$rolename'?" "$boldred"
  read -rep "Type the role name again to confirm: " confirm_role

  if [[ "$rolename" != "$confirm_role" ]]; then
    cecho "Error: Role names do not match. Aborting." "$boldred"
    return
  fi

  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "DROP ROLE IF EXISTS \"$rolename\";"
  else
    psql -h localhost -U postgres -c "DROP ROLE IF EXISTS \"$rolename\";"
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: Deleting role was unsuccessful" "$boldred"
  else
    cecho "Ok: Role '$rolename' deleted successfully" "$boldgreen"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}

delete_database() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Delete PostgreSQL Database" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"
  echo "WARNING: Ensure you have a backup before deletion!"
  echo

  read -rep "Enter database name to delete: " dbname

  if [[ -z "$dbname" ]]; then
    cecho "Error: No database name entered. Aborting." "$boldred"
    return
  fi

  cecho "Are you sure you want to delete database '$dbname'?" "$boldred"
  read -rep "Type the database name again to confirm: " confirm_db

  if [[ "$dbname" != "$confirm_db" ]]; then
    cecho "Error: Database names do not match. Aborting." "$boldred"
    return
  fi

  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "DROP DATABASE IF EXISTS \"$dbname\";"
  else
    psql -h localhost -U postgres -c "DROP DATABASE IF EXISTS \"$dbname\";"
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: Deleting database was unsuccessful" "$boldred"
  else
    cecho "Ok: Database '$dbname' deleted successfully" "$boldgreen"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}

list_role_grants() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Show Grants for PostgreSQL Role" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  read -rep "Enter role name: " rolename

  if [[ -z "$rolename" ]]; then
    cecho "Error: No role name entered. Aborting." "$boldred"
    return
  fi

  echo ""
  echo "=== Database Privileges for '$rolename' ==="
  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql -c "SELECT datname, has_database_privilege('$rolename', datname, 'CONNECT') as connect, has_database_privilege('$rolename', datname, 'CREATE') as create FROM pg_database WHERE datistemplate = false;"
  else
    psql -h localhost -U postgres -c "SELECT datname, has_database_privilege('$rolename', datname, 'CONNECT') as connect, has_database_privilege('$rolename', datname, 'CREATE') as create FROM pg_database WHERE datistemplate = false;"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}

add_db_to_existing_role() {
  cecho "--------------------------------------------------------------" "$boldyellow"
  cecho "Add Database to Existing PostgreSQL Role" "$boldgreen"
  cecho "--------------------------------------------------------------" "$boldyellow"

  read -rep "Enter existing role name: " rolename
  read -rep "Enter new database name: " dbname

  if [[ -z "$rolename" || -z "$dbname" ]]; then
    cecho "Error: All fields are required. Aborting." "$boldred"
    return
  fi

  if [[ "$PGSQL_AUTH" = "peer" ]]; then
    sudo -u postgres psql <<EOF
CREATE DATABASE "$dbname" OWNER "$rolename";
GRANT ALL PRIVILEGES ON DATABASE "$dbname" TO "$rolename";
\c "$dbname"
GRANT ALL ON SCHEMA public TO "$rolename";
EOF
  else
    psql -h localhost -U postgres <<EOF
CREATE DATABASE "$dbname" OWNER "$rolename";
GRANT ALL PRIVILEGES ON DATABASE "$dbname" TO "$rolename";
\c "$dbname"
GRANT ALL ON SCHEMA public TO "$rolename";
EOF
  fi

  ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
    cecho "Error: Creating database was unsuccessful" "$boldred"
  else
    cecho "Ok: Database '$dbname' created and assigned to role '$rolename'" "$boldgreen"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}
