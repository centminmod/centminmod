#!/bin/bash
# Copied beside native SQL dumps by backups.sh. Restore to an empty database namespace.
set -Eeuo pipefail
umask 077
SCRIPT_DIR=$(dirname -- "$(readlink -f -- "$0")")
DATAM_LOG_EXCLUDE_DIR=$(realpath -m -- "${LOGICAL_BACKUP_DIR:-$SCRIPT_DIR}")
# Exported so the logging supervisor refuses a log location inside the media before creating it.
export DATAM_LOG_EXCLUDE_DIR
# logging.sh is bundled beside this helper when a backup is created.
source "$SCRIPT_DIR/logging.sh"
datam_log_init logical-restore "$@"
current_db=''; current_file=''; import_started=n; schema_started=n; completed_count=0
logical_restore_exit() {
  local rc=$? context
  if (( rc )); then
    printf -v context 'database=%q file=%q' "$current_db" "$current_file"
    datam_log_event ERROR "Logical restore failed: phase=$DATAM_PHASE exit=$rc completed_databases=$completed_count $context"
    if [[ $import_started == y ]]; then
      echo 'The current database may be partially restored. Keep traffic and jobs disabled. Inspect it before retrying; the helper refuses existing databases and does not roll back SQL.' >&2
      [[ $schema_started != y ]] || echo 'The schema bootstrap also created objects across the selected databases; inspect them before retrying.' >&2
    elif (( completed_count )); then
      echo 'Previously completed databases remain restored. Review the completed-database log entries before retrying.' >&2
    elif [[ $schema_started == y ]]; then
      echo 'Schema creation started across the selected databases and may be incomplete. Inspect all selected databases before retrying; no automatic rollback is performed.' >&2
    else
      echo 'No database import was started. Backup media retained.' >&2
    fi
  fi
  return "$rc"
}
trap logical_restore_exit EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
cd -- "$DATAM_LOG_EXCLUDE_DIR"
fail() { echo "ERROR: $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null || fail "Install required command: $1"; }
[[ ( $# == 1 || ( $# == 2 && $1 == --database ) ) && -f index.tsv && -f SHA256SUMS ]] || fail 'Usage: restore.sh all, or restore.sh --database NAME'
MYSQL=$(command -v mariadb || command -v mysql) || fail 'MariaDB client missing.'
MY_CNF=${MY_CNF:-/root/.my.cnf}
[[ -r $MY_CNF ]] || fail "Credentials required: $MY_CNF"
SQL=("$MYSQL" "--defaults-extra-file=$MY_CNF" --max-allowed-packet=1024M -h "${DBHOST:-localhost}")
need flock; need sha256sum
exec 9>/run/lock/centminmod-logical-restore.lock
flock -n 9 || fail 'Another logical restore is running.'
# Check media before ANY SQL mutation; retain all compressed originals.
datam_log_phase checksum-validation
sha256sum --check --strict --quiet SHA256SUMS
has_checksum() { grep -Eq "^[a-f0-9]{64} [ *](\\./)?${1//./\\.}$" SHA256SUMS; }
has_checksum index.tsv || fail 'Dump index is not checksummed.'
datam_log_phase target-preflight
if [[ -f lower_case_table_names ]]; then
  has_checksum lower_case_table_names || fail 'Source name-case metadata is not checksummed.'
  read -r source_case < lower_case_table_names
  [[ -z ${SOURCE_LOWER_CASE_TABLE_NAMES:-} || $SOURCE_LOWER_CASE_TABLE_NAMES == "$source_case" ]] || fail 'Source name-case override contradicts backup metadata.'
else
  source_case=${SOURCE_LOWER_CASE_TABLE_NAMES:-}
  [[ $source_case =~ ^[012]$ ]] || fail 'Legacy media needs a verified SOURCE_LOWER_CASE_TABLE_NAMES=0, 1 or 2; never guess this value.'
  echo 'Using operator-verified legacy source lower_case_table_names.' >&2
fi
target_case=$("${SQL[@]}" -NBe 'SELECT @@lower_case_table_names')
[[ $source_case =~ ^[012]$ && $source_case == "$target_case" ]] || fail 'Source and target lower_case_table_names must match. Use a staging server with the source setting.'
unpack() {
  case $1 in *.zst) zstd -qdc -- "$1" ;; *.gz) gzip -dc -- "$1" ;; *.sql) cat -- "$1" ;; *) fail 'Unknown dump encoding.' ;; esac
}
name_expression=SCHEMA_NAME
[[ $target_case != 0 ]] || name_expression="BINARY SCHEMA_NAME"
selected=(); names=(); declare -A seen=()
while IFS=$'\t' read -r db file || [[ -n $db ]]; do
  [[ $1 == all || ${2:-$1} == "$db" ]] || continue
  current_db=$db; current_file=$file
  printf -v context 'database=%q file=%q' "$db" "$file"
  datam_log_phase database-preflight "$context"
  [[ $file =~ ^[0-9a-f]+\.sql(\.zst|\.gz)?$ && -f $file ]] || fail 'Invalid dump index.'
  has_checksum "$file" || fail "Selected dump is not checksummed: $file"
  [[ $db != mysql && $db != sys && $db != information_schema && $db != performance_schema ]] || fail 'System databases require a separate reviewed procedure.'
  hex=$(printf %s "$db" | od -An -tx1 | tr -d ' \n')
  file_id=$(printf %s "$db" | sha256sum | cut -d' ' -f1)
  [[ ! ${seen[$file_id]+set} ]] || fail 'Duplicate database in dump index.'
  seen[$file_id]=1
  [[ $file == "$file_id.sql"* ]] || fail 'Database/index mismatch.'
  # A case-insensitive metadata comparison also protects lower_case_table_names targets.
  existing=$("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE $name_expression=CONVERT(UNHEX('$hex') USING utf8)")
  [[ $existing == 0 ]] || fail "Database already exists: $db. Restore on an empty staging server."
  datam_log_phase dump-validation "$context"
  unpack "$file" >/dev/null
  selected+=("$file"); names+=("$db")
done < index.tsv
(( ${#selected[@]} )) || fail 'No matching application databases.'
bootstrap=''
if [[ $1 == all ]]; then
  for candidate in all-schema.sql all-schema.sql.zst all-schema.sql.gz; do
    [[ -e $candidate || -L $candidate ]] || continue
    [[ -z $bootstrap && -f $candidate && ! -L $candidate ]] || fail 'Ambiguous or invalid schema bootstrap.'
    bootstrap=$candidate
    has_checksum "$candidate" || fail 'Schema bootstrap is not checksummed.'
    datam_log_phase schema-bootstrap-validation "file=$bootstrap"
    unpack "$bootstrap" >/dev/null
  done
  [[ -n $bootstrap ]] || echo 'WARNING: Legacy media has no schema bootstrap; cross-database dependencies may require separate recovery.' >&2
fi
datam_log_phase destination-readiness
scheduler=$("${SQL[@]}" -NBe 'SELECT @@event_scheduler')
case $scheduler in
  OFF|DISABLED) ;;
  ON) fail 'Disable event_scheduler on the destination before restoring.' ;;
  *) fail 'Could not establish a safe event_scheduler state.' ;;
esac
if [[ -n $bootstrap ]]; then
  for db in "${names[@]}"; do
    hex=$(printf %s "$db" | od -An -tx1 | tr -d ' \n')
    [[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE $name_expression=CONVERT(UNHEX('$hex') USING utf8)") == 0 ]] || fail "Database appeared after preflight: $db"
  done
  current_db='all selected databases'; current_file=$bootstrap
  datam_log_phase schema-bootstrap-import "file=$bootstrap"
  schema_started=y
  unpack "$bootstrap" | "${SQL[@]}"
fi
for i in "${!selected[@]}"; do
  current_db=${names[$i]}; current_file=${selected[$i]}
  printf -v context 'database=%q file=%q' "$current_db" "$current_file"
  datam_log_phase database-preflight "$context"
  if [[ $schema_started != y ]]; then
    hex=$(printf %s "${names[$i]}" | od -An -tx1 | tr -d ' \n')
    [[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE $name_expression=CONVERT(UNHEX('$hex') USING utf8)") == 0 ]] || fail "Database appeared after preflight: ${names[$i]}"
  fi
  datam_log_phase database-import "$context"
  import_started=y
  unpack "${selected[$i]}" | "${SQL[@]}"
  import_started=n
  completed_count=$((completed_count + 1))
  datam_log_event INFO "Database restore completed: $context"
done
datam_log_phase complete "completed_databases=$completed_count"
echo 'Logical restore completed. Backup media retained. Review accounts-review.sql and application configuration before enabling traffic or jobs.'
