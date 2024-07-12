#!/bin/bash
# Set the target directory to the directory containing the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MY_CNF="/root/.my.cnf"
DBHOST="localhost"
MARIABACKUP_VERBOSE='n'

# Function to get MariaDB version
get_mariadb_version() {
    local version=$(mysql -V 2>&1 | awk '{print $5}' | awk -F. '{print $1"."$2}')
    echo $version
}

# Function to set client command variables based on MariaDB version
set_mariadb_client_commands() {
    local version=$(get_mariadb_version)
    
    if (( $(echo "$version <= 10.11" | bc -l) )); then
        ALIAS_MYSQLACCESS="mysqlaccess"
        ALIAS_MYSQLADMIN="mysqladmin"
        ALIAS_MYSQLBINLOG="mysqlbinlog"
        ALIAS_MYSQLCHECK="mysqlcheck"
        ALIAS_MYSQLDUMP="mysqldump"
        ALIAS_MYSQLDUMPSLOW="mysqldumpslow"
        ALIAS_MYSQLHOTCOPY="mysqlhotcopy"
        ALIAS_MYSQLIMPORT="mysqlimport"
        ALIAS_MYSQLREPORT="mysqlreport"
        ALIAS_MYSQLSHOW="mysqlshow"
        ALIAS_MYSQLSLAP="mysqlslap"
        ALIAS_MYSQL_CONVERT_TABLE_FORMAT="mysql_convert_table_format"
        ALIAS_MYSQL_EMBEDDED="mysql_embedded"
        ALIAS_MYSQL_FIND_ROWS="mysql_find_rows"
        ALIAS_MYSQL_FIX_EXTENSIONS="mysql_fix_extensions"
        ALIAS_MYSQL_INSTALL_DB="mysql_install_db"
        ALIAS_MYSQL_PLUGIN="mysql_plugin"
        ALIAS_MYSQL_SECURE_INSTALLATION="mysql_secure_installation"
        ALIAS_MYSQL_SETPERMISSION="mysql_setpermission"
        ALIAS_MYSQL_TZINFO_TO_SQL="mysql_tzinfo_to_sql"
        ALIAS_MYSQL_UPGRADE="mysql_upgrade"
        ALIAS_MYSQL_WAITPID="mysql_waitpid"
        ALIAS_MYSQL="mysql"
    else
        ALIAS_MYSQLACCESS="mariadb-access"
        ALIAS_MYSQLADMIN="mariadb-admin"
        ALIAS_MYSQLBINLOG="mariadb-binlog"
        ALIAS_MYSQLCHECK="mariadb-check"
        ALIAS_MYSQLDUMP="mariadb-dump"
        ALIAS_MYSQLDUMPSLOW="mariadb-dumpslow"
        ALIAS_MYSQLHOTCOPY="mariadb-hotcopy"
        ALIAS_MYSQLIMPORT="mariadb-import"
        ALIAS_MYSQLREPORT="mariadb-report"
        ALIAS_MYSQLSHOW="mariadb-show"
        ALIAS_MYSQLSLAP="mariadb-slap"
        ALIAS_MYSQL_CONVERT_TABLE_FORMAT="mariadb-convert-table-format"
        ALIAS_MYSQL_EMBEDDED="mariadb-embedded"
        ALIAS_MYSQL_FIND_ROWS="mariadb-find-rows"
        ALIAS_MYSQL_FIX_EXTENSIONS="mariadb-fix-extensions"
        ALIAS_MYSQL_INSTALL_DB="mariadb-install-db"
        ALIAS_MYSQL_PLUGIN="mariadb-plugin"
        ALIAS_MYSQL_SECURE_INSTALLATION="mariadb-secure-installation"
        ALIAS_MYSQL_SETPERMISSION="mariadb-setpermission"
        ALIAS_MYSQL_TZINFO_TO_SQL="mariadb-tzinfo-to-sql"
        ALIAS_MYSQL_UPGRADE="mariadb-upgrade"
        ALIAS_MYSQL_WAITPID="mariadb-waitpid"
        ALIAS_MYSQL="mariadb"
    fi
}
set_mariadb_client_commands

MYSQLBACKUP_CMD_PREFIX="mariabackup --defaults-extra-file=$MY_CNF"
MYSQLADMIN_CMD_PREFIX="${ALIAS_MYSQLADMIN} --defaults-extra-file=$MY_CNF -h $DBHOST"
DATADIR=$($MYSQLADMIN_CMD_PREFIX var | grep datadir | awk '{ print $4}'| sed 's:/$::')
SKIP_MARIABACKUP_VER_CHECK='y'

if [[ "$MARIABACKUP_VERBOSE" = [yY] ]]; then
  MDB_VERBOSE_OPT=' --verbose'
else
  MDB_VERBOSE_OPT=""
fi

check_command_exists() {
  command -v "$1" >/dev/null 2>&1 || {
    local package_name="$2"
    if [ "$1" = "mariabackup" ]; then
      grep -q "AlmaLinux" /etc/os-release && package_name="mariadb-backup"
      grep -q "CentOS Linux 7" /etc/os-release && package_name="MariaDB-backup"
    fi
    echo "[$(date)] Command '$1' not found. Installing package '$package_name'..."
    yum install -y "$package_name"
  }
}
check_command_exists mariabackup mariadb-backup

check_backup_info() {
  TARGET_DIR=$1
  XTRABACKUP_INFO="${TARGET_DIR}/xtrabackup_info"
  MARIABACKUP_DATA_VERSION="$(cat "$XTRABACKUP_INFO" | awk -F '= ' '/ibbackup_version / {print $2}')"
  MARIABACKUP_DATA_VERSION_SHORT="$(echo "$MARIABACKUP_DATA_VERSION" | cut -d . -f1-2)"
  MARIABACKUP_DATA_VERSION_LONG="$(echo "$MARIABACKUP_DATA_VERSION" | cut -d . -f1-3 | sed -e 's|-MariaDB||g')"
  DETECT_VERSION_LONG=$($MYSQLADMIN_CMD_PREFIX var | grep '^| version ' | tr -s ' ' | awk -F "| " '{print $4}' | sed -e 's|-MariaDB-log||g' -e 's|-MariaDB||g')
  DETECT_VERSION_SHORT=$($MYSQLADMIN_CMD_PREFIX var | grep '^| version ' | tr -s ' ' | awk -F "| " '{print $4}' | sed -e 's|-MariaDB-log||g' -e 's|-MariaDB||g' | cut -d . -f1-2)
  echo "[$(date)] MariaBackup source data version used: ${MARIABACKUP_DATA_VERSION_LONG}"
  echo "[$(date)] This system MariaDB Server version: ${DETECT_VERSION_LONG}"
  # if [[ "${MARIABACKUP_DATA_VERSION_LONG}" = "$DETECT_VERSION_LONG" ]]; then
  #   echo "[$(date)] Minor versions match: ${MARIABACKUP_DATA_VERSION_LONG} = $DETECT_VERSION_LONG"
  # else
  #   echo "[$(date)] Minor versions do not match: ${MARIABACKUP_DATA_VERSION_LONG} = $DETECT_VERSION_LONG"
  #   echo "[$(date)] aborting restore ..."
  #   exit 1
  # fi
  if [[ "${MARIABACKUP_DATA_VERSION_SHORT}" = "$DETECT_VERSION_SHORT" ]]; then
    echo "[$(date)] Major versions match: ${MARIABACKUP_DATA_VERSION_SHORT} = $DETECT_VERSION_SHORT"
  elif [[ "$SKIP_MARIABACKUP_VER_CHECK" != [yY] ]]; then
    echo "[$(date)] Major versions do not match: ${MARIABACKUP_DATA_VERSION_SHORT} = $DETECT_VERSION_SHORT"
    echo "[$(date)] aborting restore ..."
    exit 1
  fi
}

check_dir() {
  TARGET_DIR=$1
  # Check if the second argument is a valid MariaDB backup directory
  if [[ ! -f "${TARGET_DIR}/xtrabackup_info" && ! -f "${TARGET_DIR}/xtrabackup_checkpoints" && ! -f "${TARGET_DIR}/backup-my.cnf" ]]; then
    echo "[$(date)] Invalid MariaBackup backup directory. Does not contain valid MariaBackup data"
    exit 1
  elif [[ -f "${TARGET_DIR}/xtrabackup_info" && -f "${TARGET_DIR}/xtrabackup_checkpoints" && -f "${TARGET_DIR}/backup-my.cnf" ]]; then
    echo "[$(date)] Valid MariaBackup backup directory detected"
    check_backup_info "$TARGET_DIR"
  fi
}

backup_dir() {
  # Backup and empty data directory if not empty
  if [ "$(ls -A $DATADIR)" ]; then
    DT=$(date +"%d%m%y-%H%M%S")
    DATADIR=$($MYSQLADMIN_CMD_PREFIX var | grep datadir | awk '{ print $4}'| sed 's:/$::')
    echo "[$(date)] Stopping MariaDB server ..."
    systemctl stop mariadb
    echo
    echo "[$(date)] Backing up existing data directory to ${DATADIR}-copy-$DT ..."
    mv "$DATADIR" "${DATADIR}-copy-$DT"
    mkdir -p $DATADIR
    if [ -d "${DATADIR}-copy-$DT" ]; then
      echo "[$(date)] Backed up at ${DATADIR}-copy-$DT"
    fi
    echo "[$(date)] Check if $DATADIR is empty now"
    echo
    echo "ls -Alh $DATADIR"
    ls -Alh $DATADIR
    # echo
    # echo "[$(date)] Starting MariaDB server ..."
    # systemctl start mariadb
  fi
}

change_owner() {
  echo "[$(date)] Changing ownership of $DATADIR to mysql:mysql ..."
  chown -R mysql:mysql "$DATADIR"
}

remove_restore_script() {
  echo "[$(date)] Remove ${DATADIR}/mariabackup-restore.sh"
  rm -rf "${DATADIR}/mariabackup-restore.sh"
  echo "[$(date)] Remove ${DATADIR}/mariabackup_*.log"
  rm -rf "${DATADIR}/mariabackup_*.log"
}

copy_back() {
  TARGET_DIR=$1
  check_dir "$TARGET_DIR"
  backup_dir
  echo "[$(date)] Performing MariaBackup --copy-back from $TARGET_DIR ..."
  echo "$MYSQLBACKUP_CMD_PREFIX --copy-back --target-dir=\"$TARGET_DIR\"${MDB_VERBOSE_OPT}"
  $MYSQLBACKUP_CMD_PREFIX --copy-back --target-dir="$TARGET_DIR"${MDB_VERBOSE_OPT}
  echo
  change_owner
  remove_restore_script
  echo "[$(date)] Starting MariaDB server ..."
  systemctl start mariadb
}

move_back() {
  TARGET_DIR=$1
  check_dir "$TARGET_DIR"
  backup_dir
  echo "[$(date)] Performing MariaBackup --move-back from $TARGET_DIR ..."
  echo "$MYSQLBACKUP_CMD_PREFIX --move-back --target-dir=\s"$TARGET_DIR\s"${MDB_VERBOSE_OPT}"
  $MYSQLBACKUP_CMD_PREFIX --move-back --target-dir="$TARGET_DIR"${MDB_VERBOSE_OPT}
  echo
  change_owner
  remove_restore_script
  echo "[$(date)] Starting MariaDB server ..."
  systemctl start mariadb
}

# Check if the script is run with the correct number of arguments
if [ "$#" -ne 2 ]; then
  echo
  echo "Usage: $0 [copy-back|move-back] /path/to/backup/dir/"
  exit 1
fi

# Perform the copy-back or move-back operation based on the argument
case "$1" in
  copy-back)
    copy_back "$2"
    ;;
  move-back)
    move_back "$2"
    ;;
  *)
    echo "Invalid argument. Usage: $0 [copy-back|move-back] /path/to/backup/dir/"
    exit 1
    ;;
esac
