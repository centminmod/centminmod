#!/bin/bash
# Set the target directory to the directory containing the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MY_CNF="/root/.my.cnf"
DBHOST="localhost"
MYSQLBACKUP_CMD_PREFIX="mariabackup --defaults-extra-file=$MY_CNF -h $DBHOST"
MYSQLADMIN_CMD_PREFIX="mysqladmin --defaults-extra-file=$MY_CNF -h $DBHOST"
DATADIR=$($MYSQLADMIN_CMD_PREFIX var | grep datadir | awk '{ print $4}')

check_backup_info() {
  TARGET_DIR=$1
  XTRABACKUP_INFO="${TARGET_DIR}/xtrabackup_info"
  MARIABACKUP_DATA_VERSION="$(cat "$XTRABACKUP_INFO" | awk -F '= ' '/ibbackup_version / {print $2}')"
  MARIABACKUP_DATA_VERSION_SHORT="$(echo "$MARIABACKUP_DATA_VERSION" | cut -d . -f1-2)"
  MARIABACKUP_DATA_VERSION_LONG="$(echo "$MARIABACKUP_DATA_VERSION" | cut -d . -f1-3 | sed -e 's|-MariaDB||g')"
  DETECT_VERSION_LONG=$($MYSQLADMIN_CMD_PREFIX var | grep '^| version ' | tr -s ' ' | awk -F "| " '{print $4}' | sed -e 's|-MariaDB-log||g')
  DETECT_VERSION_SHORT=$($MYSQLADMIN_CMD_PREFIX var | grep '^| version ' | tr -s ' ' | awk -F "| " '{print $4}' | sed -e 's|-MariaDB-log||g' | cut -d . -f1-2)
  echo "[$(date)] MariaBackup source data version used: ${MARIABACKUP_DATA_VERSION_LONG}"
  echo "[$(date)] This system MariaDB Server version: ${DETECT_VERSION_LONG}"
  if [[ "${MARIABACKUP_DATA_VERSION_LONG}" = "$DETECT_VERSION_LONG" ]]; then
    echo "[$(date)] Minor versions match: ${MARIABACKUP_DATA_VERSION_LONG} = $DETECT_VERSION_LONG"
  else
    echo "[$(date)] Minor versions do not match: ${MARIABACKUP_DATA_VERSION_LONG} = $DETECT_VERSION_LONG"
    echo "[$(date)] aborting restore ..."
    exit 1
  fi
  if [[ "${MARIABACKUP_DATA_VERSION_SHORT}" = "$DETECT_VERSION_SHORT" ]]; then
    echo "[$(date)] Major versions match: ${MARIABACKUP_DATA_VERSION_SHORT} = $DETECT_VERSION_SHORT"
  else
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

copy_back() {
  TARGET_DIR=$1
  check_dir "$TARGET_DIR"
  echo "[$(date)] Performing MariaBackup --copy-back to $TARGET_DIR ..."
  $MYSQLBACKUP_CMD_PREFIX --copy-back --target-dir="$TARGET_DIR"
}

move_back() {
  TARGET_DIR=$1
  check_dir "$TARGET_DIR"
  echo "[$(date)] Performing MariaBackup --move-back to $TARGET_DIR ..."
  $MYSQLBACKUP_CMD_PREFIX --move-back --target-dir="$TARGET_DIR"
}

# Check if the script is run with the correct number of arguments
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 [copy-back|move-back] /path/to/backup/dir"
  exit 1
fi

# Stop MariaDB server
echo "[$(date)] Stopping MariaDB server ..."
systemctl stop mariadb

# Backup and empty data directory if not empty
if [ "$(ls -A $DATADIR)" ]; then
  DT=$(date +"%d%m%y-%H%M%S")
  echo "[$(date)] Backing up existing data directory to ${DATADIR}-copy-$DT ..."
  mv $DATADIR ${DATADIR}-copy-$DT
  mkdir -p $DATADIR
fi

echo "[$(date)] Changing ownership of $DATADIR to mysql:mysql ..."
chown -R mysql:mysql $DATADIR
echo "[$(date)] Starting MariaDB server ..."
systemctl start mariadb

# Perform the copy-back or move-back operation based on the argument
case "$1" in
  copy-back)
    copy_back "$2"
    ;;
  move-back)
    move_back "$2"
    ;;
  *)
    echo "Invalid argument. Usage: $0 [copy-back|move-back] /path/to/backup/dir"
    exit 1
    ;;
esac
