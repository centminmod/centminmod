#!/bin/bash
# Set the target directory to the directory containing the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MY_CNF="/root/.my.cnf"
DBHOST="localhost"
MARIABACKUP_VERBOSE='n'
MYSQLBACKUP_CMD_PREFIX="mariabackup --defaults-extra-file=$MY_CNF"
MYSQLADMIN_CMD_PREFIX="mysqladmin --defaults-extra-file=$MY_CNF -h $DBHOST"
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
