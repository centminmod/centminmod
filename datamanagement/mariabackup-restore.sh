#!/bin/bash
# Standalone physical restore, also copied into each physical backup.
set -Eeuo pipefail
umask 077
SCRIPT_DIR=$(dirname -- "$(readlink -f -- "$0")")
DATAM_LOG_EXCLUDE_DIR=''
if [[ -n ${2:-} && -d $2 ]]; then DATAM_LOG_EXCLUDE_DIR=$(realpath -e -- "$2"); fi
export DATAM_LOG_EXCLUDE_DIR
# logging.sh is bundled beside this helper when a backup is created.
source "$SCRIPT_DIR/logging.sh"
datam_log_init physical-restore "$@"
original=none; restore_started=n; initial_state=unknown; original_identity=''; recovery_armed=n
recover() {
  local rc=$? state=not-inspected context original_location=${DATADIR:-not-selected}
  if (( rc )); then
    if [[ $recovery_armed == y ]]; then
      state=$(systemctl show "$service" -p ActiveState --value) || state=unknown
      if [[ $initial_state == active && $restore_started == n && -n $original_identity && $(stat -Lc '%d:%i' -- "$DATADIR" 2>/dev/null) == "$original_identity" ]]; then
        if [[ $state == inactive || $state == failed ]]; then
          if ! fuser "$DATADIR" "$DATADIR/ibdata1" "$DATADIR/aria_log_control" >/dev/null 2>&1; then
            datam_log_event INFO 'Recovery: restarting the unchanged original service.'
            if systemctl start "$service"; then
              datam_log_event INFO 'Recovery: original-service start command succeeded.'
            else
              datam_log_event ERROR 'Recovery: could not restart the unchanged original service.'
            fi
          else
            datam_log_event WARN 'Recovery: original datadir is in use; automatic restart skipped.'
          fi
        else
          datam_log_event INFO "Recovery: no restart attempted; service state=$state."
        fi
      else
        datam_log_event INFO 'Recovery: automatic restart skipped because the original service was not active, its directory changed, or restore copying started.'
      fi
      state=$(systemctl show "$service" -p ActiveState --value) || state=unknown
      datam_log_event INFO "Recovery service state: service=$service initial=$initial_state final=$state"
    fi
    [[ $original == none || ! -d $original ]] || original_location=$original
    printf -v context 'original_data=%q destination=%q backup=%q' "$original_location" "${DATADIR:-not-selected}" "${TARGET_DIR:-not-selected}"
    datam_log_event ERROR "Physical restore failed: phase=$DATAM_PHASE exit=$rc restore_started=$restore_started $context"
    echo 'Preserve all recovery data. After service failures, inspect the selected unit with systemctl status and journalctl before retrying.' >&2
  fi
  return "$rc"
}
trap recover EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
fail() { echo "ERROR: $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null || fail "Install required command: $1"; }
client() { command -v "$1" || command -v "$2"; }
# No live server or package installation is needed just to inspect the backup.
metadata_file() {
  local dir=$1 old=$2 new=$3
  if [[ -f $dir/$old && -f $dir/$new ]]; then
    cmp -s -- "$dir/$old" "$dir/$new" || fail "Conflicting metadata: $old / $new"
  fi
  if [[ -f $dir/$new && ! -L $dir/$new ]]; then printf '%s\n' "$dir/$new"
  elif [[ -f $dir/$old && ! -L $dir/$old ]]; then printf '%s\n' "$dir/$old"
  else fail "Missing metadata: $old or $new"; fi
}
field() {
  awk -F= -v key="$2" '{k=$1; gsub(/^[ \t]+|[ \t]+$/, "", k); if(k==key) {v=substr($0,index($0,"=")+1); gsub(/^[ \t]+|[ \t\r]+$/, "", v); print v}}' "$1"
}
version() { printf '%s\n' "$1" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1; }
# Prove that this systemd main process has a database file from the selected tree open.
# This works after restoring source accounts without requiring their SQL password.
service_owns_datadir() {
  local pid exe file identity fd
  pid=$(systemctl show "$service" -p MainPID --value) || return 1
  [[ $pid =~ ^[1-9][0-9]*$ ]] || { datam_log_event WARN 'Service identity check found no running main process.'; return 1; }
  exe=$(readlink -e "/proc/$pid/exe") || return 1
  [[ $exe == "$(readlink -e "$server")" ]] || { datam_log_event WARN "Service identity check found a different executable for PID $pid."; return 1; }
  for file in "$DATADIR/aria_log_control" "$DATADIR/ibdata1"; do
    [[ -f $file && ! -L $file ]] || continue
    identity=$(stat -Lc '%d:%i' -- "$file") || return 1
    for fd in /proc/"$pid"/fd/*; do
      [[ $(stat -Lc '%d:%i' -- "$fd" 2>/dev/null) != "$identity" ]] || return 0
    done
  done
  datam_log_event WARN "Service identity check found no matching open database file for PID $pid."
  return 1
}
[[ $# == 2 ]] || fail 'Usage: mariabackup-restore.sh {check|copy-back|move-back} /backup/directory'
case $1 in check|copy-back|move-back) action=$1 ;; *) fail 'Unknown action.' ;; esac
[[ -d $2 && ! -L $2 ]] || fail 'Backup must be a real directory.'
TARGET_DIR=$(realpath -e -- "$2")
[[ $TARGET_DIR != / ]] || fail 'Invalid backup directory.'
printf -v context 'backup=%q action=%q' "$TARGET_DIR" "$action"
datam_log_phase metadata-validation "$context"
info=$(metadata_file "$TARGET_DIR" xtrabackup_info mariadb_backup_info)
checkpoints=$(metadata_file "$TARGET_DIR" xtrabackup_checkpoints mariadb_backup_checkpoints)
[[ -f $TARGET_DIR/backup-my.cnf && ! -L $TARGET_DIR/backup-my.cnf ]] || fail 'Missing backup-my.cnf.'
# Optional metadata is not mandatory for a standalone nonreplication restore.
for name in binlog_info slave_info galera_info; do
  if [[ -e $TARGET_DIR/xtrabackup_$name || -e $TARGET_DIR/mariadb_backup_$name ]]; then
    metadata_file "$TARGET_DIR" "xtrabackup_$name" "mariadb_backup_$name" >/dev/null
  fi
done
[[ $(field "$checkpoints" backup_type) == log-applied ]] || fail 'Backup is not prepared. Prepare a working copy with the matching producer first.'
source_version=$(version "$(field "$info" server_version)") || fail 'Missing source server_version.'
producer_version=$(version "$(field "$info" ibbackup_version)") || fail 'Missing producer ibbackup_version.'
server=$(client mariadbd mysqld) || fail 'MariaDB server binary missing.'
tool=$(client mariadb-backup mariabackup) || fail 'Install the matching MariaDB-backup package.'
datam_log_phase version-validation
target_version=$(version "$("$server" --version)") || fail 'Could not determine installed MariaDB server version.'
tool_version=$(version "$("$tool" --version 2>&1)") || fail 'Could not determine installed MariaDB backup-tool version.'
printf 'Source server=%s; backup producer=%s; target server=%s; restore tool=%s\n' "$source_version" "$producer_version" "$target_version" "$tool_version"
# Exact patch is deliberately conservative; family agreement alone does not prove compatibility.
if [[ $source_version != "$target_version" || $producer_version != "$tool_version" || ${source_version%.*} != "${producer_version%.*}" ]]; then
  [[ ${SKIP_MARIABACKUP_VER_CHECK:-n} == [yY] ]] || fail 'Physical version mismatch. Use matching packages or logical migration. Expert override: SKIP_MARIABACKUP_VER_CHECK=y'
  echo 'WARNING: explicit version override, compatibility is unverified.' >&2
fi
# Let the installed server parse its own includes and defaults while it is offline.
datam_log_phase datadir-discovery
DATADIR=${DATADIR:-$("$server" --verbose --help | awk '$1=="datadir" {sub(/^[ \t]*datadir[ \t]+/, ""); dir=$0} END {print dir}')} || fail 'Could not read the configured MariaDB datadir.'
[[ -n $DATADIR && $DATADIR == /* && ! -L $DATADIR ]] || fail 'Invalid datadir; set DATADIR explicitly.'
DATADIR=$(realpath -m -- "$DATADIR")
case $DATADIR in /|/var|/var/lib|/home|/root|/etc|/usr) fail 'Unsafe datadir.' ;; esac
# Preservation renames the datadir; a directory that is itself a mount point cannot be renamed.
# findmnt consults the mount table directly: stat %m reports the parent mount for bind mounts.
need findmnt
if [[ -d $DATADIR ]] && findmnt -n --mountpoint "$DATADIR" >/dev/null; then
  fail 'Datadir is a separate mount point and cannot be preserved by rename. Mount the volume one level above the datadir, or preserve its contents with a separately verified manual procedure.'
fi
case "$TARGET_DIR/" in "$DATADIR/"*) fail 'Backup is inside datadir.' ;; esac
case "$DATADIR/" in "$TARGET_DIR/"*) fail 'Datadir is inside backup.' ;; esac
printf -v context 'destination=%q' "$DATADIR"
datam_log_event INFO "$context"
if [[ $action == check ]]; then datam_log_phase complete; echo "Preflight passed for $DATADIR"; exit; fi
[[ $EUID == 0 ]] || fail 'Run restore as root.'
service=${MARIADB_SERVICE:-mariadb}
[[ $service =~ ^[a-zA-Z0-9_.@-]+$ && $service != -* ]] || fail 'Invalid MariaDB service name.'
[[ $action != move-back || ${ALLOW_MOVE_BACK:-n} == y ]] || fail 'move-back consumes backup media. Set ALLOW_MOVE_BACK=y or use copy-back.'
need flock
exec 9>/run/lock/centminmod-physical-restore.lock
flock -n 9 || fail 'Another physical restore is running.'
datam_log_phase capacity-check
parent=$(dirname -- "$DATADIR")
[[ -d $parent ]] || fail 'Datadir parent is missing.'
required=$(du -sk -- "$TARGET_DIR" | awk '{print $1}')
available=$(df -Pk "$parent" | awk 'END {print $4}')
(( available > required + required / 10 )) || fail 'Insufficient free space for restoration plus 10% reserve.'
command -v fuser >/dev/null || fail 'Install psmisc for the datadir-in-use check.'
datam_log_phase service-preflight "service=$service"
initial_state=$(systemctl show "$service" -p ActiveState --value)
datam_log_event INFO "Initial service state: service=$service state=$initial_state"
case $initial_state in active) service_owns_datadir || fail 'Selected datadir does not belong to the running MariaDB service; nothing stopped.' ;; inactive|failed) ;; *) fail "Service is transitioning: $initial_state" ;; esac
original_identity=$(stat -Lc '%d:%i' -- "$DATADIR" 2>/dev/null || :)
recovery_armed=y
datam_log_phase service-stop "service=$service"
systemctl stop "$service" || fail 'MariaDB stop failed; original data untouched.'
state=$(systemctl show "$service" -p ActiveState --value)
[[ $state == inactive || $state == failed ]] || fail "MariaDB has not stopped: $state"
[[ $(systemctl show "$service" -p MainPID --value) == 0 ]] || fail 'MariaDB still has a main process.'
# Refuse independently started server processes using this datadir as well.
if fuser "$DATADIR" "$DATADIR/ibdata1" "$DATADIR/aria_log_control" >/dev/null 2>&1; then fail 'Datadir is still in use.'; fi
datam_log_phase preserve-original
original="${DATADIR}-copy-$(date -u +%Y%m%dT%H%M%SZ)-$$"
[[ ! -e $original ]] || fail 'Recovery directory already exists.'
if [[ -e $DATADIR ]]; then mv -- "$DATADIR" "$original"; else original=none; fi
mkdir -- "$DATADIR"
restore_started=y
datam_log_phase physical-copy "action=$action"
"$tool" "--$action" --target-dir="$TARGET_DIR" --datadir="$DATADIR"
# The tool copies every unrecognized file in the backup; keep the bundled helpers out of the datadir.
rm -f -- "$DATADIR/logging.sh" "$DATADIR/mariabackup-restore.sh"
datam_log_phase ownership
chown -R mysql:mysql -- "$DATADIR"
datam_log_phase service-start "service=$service"
systemctl start "$service"
systemctl is-active --quiet "$service"
datam_log_phase service-identity "service=$service"
if ! service_owns_datadir; then
  systemctl stop "$service" || echo 'Failed to stop the mismatched service; check it immediately.' >&2
  fail 'Started service is not using the restored datadir. Check service configuration before retrying.'
fi
datam_log_phase complete "service=$service final_state=active"
echo 'The source database accounts are now active. Use matching source credentials and validate application queries before enabling traffic.'
printf 'Restore completed. Previous datadir retained at %s\n' "$original"
