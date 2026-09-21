#!/bin/bash
# Centmin Mod menu 21. A generation is usable only after COMPLETE is written.
set -Eeuo pipefail
umask 077
BACKUP_FORMAT=2
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
source "$SCRIPT_DIR/logging.sh"
datam_log_init backup "$@"
BACKUP_DIR_PARENT=/home/mysqlbackup
FILES_BACKUP_ROOT=/home/databackup
MY_CNF=/root/.my.cnf
DBHOST=localhost
COMPRESSION_METHOD=zstd
COMPRESSION_LEVEL_ZSTD=4
COMPRESSION_LEVEL_GZIP=4
FASTCOMPRESS_ZSTD=y
COMPRESS_THREADS=2
FILES_TARBALL_CREATION=n
CHECKSUMS=n
BUFFER_PERCENT=10
# Full file checksums require another disk read. Transfers hash the compressed stream.
DIRECTORIES_TO_BACKUP=(/etc/centminmod /usr/local/nginx/conf /root/tools /usr/local/nginx/html
  /root/.acme.sh /root/.aws /root/.my.cnf /etc/my.cnf /etc/my.cnf.d
  /usr/local/etc/php-fpm.conf /usr/local/etc/php-fpm.d /usr/local/lib/php.ini
  /etc/sudoers.d /etc/pure-ftpd /etc/redis /etc/keydb /etc/supervisord
  /etc/supervisord.d /etc/supervisord.conf /etc/elasticsearch /etc/systemd/system
  /var/spool/cron /etc/cron.d /etc/crontab /etc/cron.hourly /etc/cron.daily /etc/cron.weekly /etc/cron.monthly)
DOMAINS_ROOT=/home/nginx/domains
# Configuration is trusted root-owned shell. Detect obsolete overrides before work.
set +u
for config in /etc/centminmod/binlog-backups.ini /etc/centminmod/backups.ini; do
  # shellcheck disable=SC1090
  [[ ! -f $config ]] || source "$config"
done
# shellcheck disable=SC1090
[[ -z ${BACKUP_CONFIG:-} ]] || source "$BACKUP_CONFIG"
set -u
fail() { echo "ERROR: $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null || fail "Install required command: $1"; }
client() { command -v "$1" || command -v "$2"; }
usage() {
  echo "Usage: $0 {backup-all-mariabackup|backup-files|backup-mariabackup} [comp]"
  echo "       $0 {backup-all|backup-mysql|backup-binlogs} [comp|pigz|none]"
  echo "       $0 {flush-logs|flush-binlogs|purge-binlogs}"
  echo "COMPLETE marks success; failed generations are retained. No automatic retention deletion."
}
mode=${1:-help}
case $mode in
  backup-all-mariabackup|backup-files|backup-mariabackup|backup-all|backup-mysql|backup-binlogs|flush-logs|flush-binlogs|purge-binlogs) ;;
  help|-h|--help) usage; exit 0 ;;
  *) usage; exit 1 ;;
esac
case $mode in
  backup-files|backup-all-mariabackup|backup-mariabackup)
    [[ -z ${2:-} || ${2:-} == comp ]] || fail 'File/physical backups accept only the optional comp argument.'
    if [[ ${2:-} == comp || $FILES_TARBALL_CREATION == [yY] ]]; then need tar; need zstd; fi
    ;;
esac
for obsolete in BASE_DIR MYSQL_BACKUP_DIR BACKUP_DIR BACKUP_NAME MARIADB_TMP_DIR DIRECTORIES_TO_BACKUP_NOCOMPRESS MYSQLDUMP_OPTS MYSQLIMPORT_OPTS NICEOPT IONICEOPT COMPRESS_RSYNCABLE; do
  [[ ! ${!obsolete+set} ]] || fail "Obsolete configuration: $obsolete. Migrate this setting using datamanagement/backups.sh.md before running a backup."
done
# Validate every provider before creating media or making any remote request.
s3_args=(); s3_bucket=''; providers=()
for provider in AWS BACKBLAZE DIGITALOCEAN LINODE CFR2 UPCLOUD; do
  flag=${provider}_UPLOAD; [[ $provider != AWS ]] || flag=AWSUPLOAD
  [[ ${!flag:-n} != [yY] ]] || providers+=("$provider")
done
(( ${#providers[@]} <= 1 )) || fail 'Enable only one S3 provider per job.'
if (( ${#providers[@]} )); then
  provider=${providers[0]}
  bucket=${provider}_BUCKETNAME; profile=${provider}_PROFILE; endpoint=${provider}_ENDPOINT
  s3_bucket=${!bucket:-}; profile=${!profile:-}; endpoint=${!endpoint:-}
  case $provider in
    AWS) profile=${profile:-default} ;;
    BACKBLAZE) profile=${profile:-b2}; endpoint=${endpoint:-https://s3.us-west-001.backblazeb2.com} ;;
    DIGITALOCEAN) profile=${profile:-do}; endpoint=${endpoint:-https://sfo2.digitaloceanspaces.com} ;;
    LINODE) profile=${profile:-linode}; endpoint=${endpoint:-https://us-east-1.linodeobjects.com} ;;
    CFR2) profile=${profile:-r2}; [[ -n $endpoint || -z ${CFR2_ACCOUNTID:-} ]] || endpoint="https://${CFR2_ACCOUNTID}.r2.cloudflarestorage.com" ;;
    UPCLOUD) profile=${profile:-upcloud} ;;
  esac
  endpoint=${endpoint# }; endpoint=${endpoint#--endpoint-url=}
  [[ -n $s3_bucket && $s3_bucket != YOUR_BUCKETNAME && $s3_bucket != */* ]] || fail 'Configure a valid S3 bucket.'
  [[ $provider == AWS || -n $endpoint ]] || fail "Configure ${provider}_ENDPOINT explicitly."
  [[ -z $endpoint || ( $endpoint =~ ^https?://[^/[:space:]]+(/[^[:space:]]*)?$ ) ]] || fail 'Invalid S3 endpoint URL.'
  need aws
  s3_args=(--profile "$profile"); [[ -z $endpoint ]] || s3_args+=(--endpoint-url "$endpoint")
fi
# Object storage cannot represent a filesystem tree's links and metadata.
if [[ -n $s3_bucket ]]; then
  case $mode in
    backup-files|backup-all-mariabackup|backup-mariabackup)
      FILES_TARBALL_CREATION=y
      need tar; need zstd
      echo 'S3 file/physical backup: creating a metadata-preserving tar.zst archive.'
      ;;
  esac
fi
[[ $EUID == 0 ]] || fail 'Run as root.'
[[ $COMPRESS_THREADS =~ ^[1-9][0-9]*$ && $BUFFER_PERCENT =~ ^[0-9]+$ && $BUFFER_PERCENT -lt 100 ]] || fail 'Invalid resource limits.'
[[ $COMPRESSION_LEVEL_ZSTD =~ ^[1-9][0-9]*$ && $COMPRESSION_LEVEL_GZIP =~ ^[1-9]$ ]] || fail 'Invalid compression level.'
need flock
# ponytail: one backup at a time per server; use per-source locks if concurrency is needed.
exec 9>/run/lock/centminmod-backup.lock
flock -n 9 || fail 'Another menu 21 backup is running.'
mysql_setup() {
  datam_log_phase database-preflight
  [[ -r $MY_CNF ]] || fail "Credential file required: $MY_CNF"
  MYSQL=$(client mariadb mysql) || fail 'MariaDB client missing.'
  SQL=("$MYSQL" "--defaults-extra-file=$MY_CNF" -h "$DBHOST")
  SERVER_VERSION=$("${SQL[@]}" -NBe 'SELECT VERSION()')
  datam_log_event INFO "server_version=$SERVER_VERSION"
  DUMP_CHARSET=$("${SQL[@]}" -NBe "SELECT IF(COUNT(*),'utf8mb4','utf8') FROM information_schema.CHARACTER_SETS WHERE CHARACTER_SET_NAME='utf8mb4'")
}
if [[ $mode != backup-files ]]; then mysql_setup; fi
case $mode in
  flush-logs) "${SQL[@]}" -e 'FLUSH LOGS'; exit ;;
  flush-binlogs) "${SQL[@]}" -e 'FLUSH BINARY LOGS'; exit ;;
  purge-binlogs) fail 'Automatic purge removed: select a verified recovery point and purge its unneeded binlogs explicitly.' ;;
esac
SOURCE_ID=${SOURCE_ID:-$(cat /etc/machine-id)}
[[ $SOURCE_ID =~ ^[a-zA-Z0-9._-]+$ && $SOURCE_ID != . && $SOURCE_ID != .. ]] || fail 'Invalid SOURCE_ID.'
RUN_ID=$(date -u +%Y%m%dT%H%M%SZ)-$$-${RANDOM}
case $mode in
  backup-files|backup-all-mariabackup|backup-mariabackup) root=$FILES_BACKUP_ROOT ;;
  *) root=$BACKUP_DIR_PARENT ;;
esac
while [[ $root == */ && $root != / ]]; do root=${root%/}; done
[[ $root == /* && $root != / ]] || fail 'Backup root must be an absolute non-root directory.'
# A legacy backup root may belong to mysql. Its ancestors must already be trusted.
# Require existing parents rather than following a newly inserted mkdir -p component.
backup_parent_check() {
  local parent=$1 permissions
  while :; do
    [[ -d $parent && $(stat -c %u -- "$parent") == 0 ]] || fail "Backup parent must exist and be root-owned: $parent"
    if [[ ! -L $parent ]]; then
      permissions=$(stat -c %a -- "$parent")
      (( (8#$permissions & 0022) == 0 || (8#$permissions & 01000) != 0 )) || fail "Backup parent is writable by other users: $parent"
    fi
    [[ $parent != / ]] || break
    parent=$(dirname -- "$parent")
  done
}
backup_parent_check "$(dirname -- "$root")"
[[ ! -L $root || $(stat -c %u -- "$root") == 0 ]] || fail 'Refusing an untrusted backup-root symlink.'
root=$(realpath -m -- "$root")
backup_parent_check "$(dirname -- "$root")"
case $root in /|/home|/root|/etc|/usr|/var|/var/lib|/mnt|/media) fail 'Use a dedicated backup directory, not a shared system parent.' ;; esac
if [[ ! -e $root && ! -L $root ]]; then mkdir -- "$root"; fi
[[ -d $root && ! -L $root ]] || fail 'Backup root must be a real directory.'
# -h never changes a substituted symlink target. Root ownership also protects a
# legacy mysql-owned entry against rename beneath a root-owned sticky parent.
chown -h root:root -- "$root"
[[ -d $root && ! -L $root ]] || fail 'Backup root changed during preflight.'
chmod 700 -- "$root"
source_root="$root/$SOURCE_ID"
[[ ! -L $source_root ]] || fail "Refusing symlinked source directory: $source_root"
if [[ ! -e $source_root ]]; then mkdir -- "$source_root"; fi
[[ -d $source_root && ! -L $source_root ]] || fail 'Source namespace must be a real directory.'
chown -h root:root -- "$source_root"
chmod 700 -- "$source_root"
BASE_DIR="$root/$SOURCE_ID/$RUN_ID"
case "$MENU21_LOG_FILE" in "$BASE_DIR/"*) fail "Operational log is inside backup generation." ;; esac
mkdir -- "$BASE_DIR"
datam_log_event INFO "generation=$BASE_DIR mode=$mode"
printf 'format=%s\nsource=%s\nhost=%s\nrun=%s\nmode=%s\nserver_version=%s\n' "$BACKUP_FORMAT" "$SOURCE_ID" "$(hostname)" "$RUN_ID" "$mode" "${SERVER_VERSION:-none}" > "$BASE_DIR/backup.info"
touch "$BASE_DIR/INCOMPLETE"
# shellcheck disable=SC2154
trap 'rc=$?; if (( rc )); then echo "Backup failed ($rc). Preserved: $BASE_DIR" >&2; fi' EXIT
space_check() {
  local required=$1 avail
  avail=$(df -Pk "$BASE_DIR" | awk 'END {print $4}')
  (( required <= avail * (100 - BUFFER_PERCENT) / 100 )) || fail "Insufficient space: need $required KiB plus reserve; available $avail KiB."
}
compress() {
  case $COMPRESSION_METHOD in
    zstd) need zstd; if [[ $FASTCOMPRESS_ZSTD == [yY] ]]; then
        zstd -q -T"$COMPRESS_THREADS" --fast="$COMPRESSION_LEVEL_ZSTD"
      else zstd -q -T"$COMPRESS_THREADS" -"$COMPRESSION_LEVEL_ZSTD"; fi ;;
    pigz) need pigz; pigz -p "$COMPRESS_THREADS" -"$COMPRESSION_LEVEL_GZIP" ;;
    none) cat ;;
    *) fail 'COMPRESSION_METHOD must be zstd, pigz or none.' ;;
  esac
}
case ${2:-} in comp) COMPRESSION_METHOD=zstd ;; pigz|none) COMPRESSION_METHOD=$2 ;; '') ;; *) fail 'Invalid compression option.' ;; esac
case $COMPRESSION_METHOD in zstd) suffix=.zst ;; pigz) suffix=.gz ;; none) suffix='' ;; *) fail 'Invalid COMPRESSION_METHOD.' ;; esac
# SQL/binlog encoders are checked before any dump or binlog rotation; file/physical modes use their own tar path.
case $mode in
  backup-all|backup-mysql|backup-binlogs) case $COMPRESSION_METHOD in zstd) need zstd ;; pigz) need pigz ;; esac ;;
esac
# Capacity estimates are advisory. A file vanishing under du must not abort a run; ENOSPC still fails it.
estimate_kib() {
  local listing
  listing=$(du -sk -- "$@" 2>/dev/null) || datam_log_event WARN "partial-estimate paths=$*"
  printf '%s\n' "$listing" | awk '{s+=$1} END {print s+0}'
}
files_backup() {
  datam_log_phase files-preflight
  need rsync
  local path canonical required=0 size
  local -a sources=()
  # Equivalent coverage in compressed and uncompressed generations, paths relative to /.
  for path in "${DIRECTORIES_TO_BACKUP[@]}" "$DOMAINS_ROOT"; do
    if [[ -e $path || -L $path ]]; then
      [[ $path == /* && $path != / ]] || fail "Invalid source: $path"
      canonical=$(realpath -m -- "$path")
      [[ $canonical != / ]] || fail "Invalid root source alias: $path"
      DATAM_LOG_EXCLUDE_DIR="$canonical" datam_log_check_location "$MENU21_LOG_FILE" || fail "Set CENTMINLOGDIR outside source: $path"
      case "$BASE_DIR/" in "$canonical/"*) fail "Backup destination is inside source: $path" ;; esac
      size=$(estimate_kib "$path")
      required=$((required + size))
      sources+=("$path")
    else printf 'missing\t%s\n' "$path" >> "$BASE_DIR/coverage.tsv"; fi
  done
  (( ${#sources[@]} )) || fail 'No configured file source exists; check DIRECTORIES_TO_BACKUP and DOMAINS_ROOT.'
  [[ ${2:-} != comp && $FILES_TARBALL_CREATION != [yY] ]] || required=$((required * 2))
  space_check "$required"
  mkdir "$BASE_DIR/files"
  datam_log_phase file-copy "sources=${#sources[@]} details=$BASE_DIR/files.log"
  # One invocation preserves hardlinks across configured roots; -S preserves holes.
  rsync -aHAXSR --numeric-ids -- "${sources[@]}" "$BASE_DIR/files/" >> "$BASE_DIR/files.log" 2>&1
  printf 'included\t%s\n' "${sources[@]}" >> "$BASE_DIR/coverage.tsv"
  printf '%s\n' 'Live files are not an application snapshot. Freeze writes before the final backup.' \
    'Redis/KeyDB/PostgreSQL/Elasticsearch data requires a separate native backup.' \
    'Review identity, IP addresses, credentials, PHP and systemd settings before activation.' >> "$BASE_DIR/coverage.tsv"
}
physical_backup() {
  datam_log_phase physical-preflight
  local tool datadir required
  [[ $DBHOST == localhost || $DBHOST == 127.0.0.1 ]] || fail 'Physical backup requires a local MariaDB server.'
  tool=$(client mariadb-backup mariabackup) || fail 'Install the MariaDB-backup package matching the source server.'
  datadir=$("${SQL[@]}" -NBe 'SELECT @@datadir')
  required=$(estimate_kib "$datadir")
  [[ ${2:-} != comp && $FILES_TARBALL_CREATION != [yY] ]] || required=$((required * 2))
  space_check "$required"
  mkdir "$BASE_DIR/mariadb_tmp"
  datam_log_phase physical-backup "details=$BASE_DIR/physical.log"
  "$tool" "--defaults-extra-file=$MY_CNF" --host="$DBHOST" --backup --target-dir="$BASE_DIR/mariadb_tmp" > "$BASE_DIR/physical.log" 2>&1
  datam_log_phase physical-prepare "details=$BASE_DIR/physical.log"
  "$tool" "--defaults-extra-file=$MY_CNF" --prepare --target-dir="$BASE_DIR/mariadb_tmp" >> "$BASE_DIR/physical.log" 2>&1
  cp -- "$SCRIPT_DIR/logging.sh" "$SCRIPT_DIR/mariabackup-restore.sh" "$BASE_DIR/mariadb_tmp/"
}
logical_backup() {
  datam_log_phase logical-preflight
  local dump db hex datadir
  local -a databases=()
  dump=$(client mariadb-dump mysqldump) || fail 'MariaDB dump client missing.'
  datadir=$("${SQL[@]}" -NBe 'SELECT @@datadir')
  # SQL expansion can exceed on-disk data; reserve twice the full datadir size.
  space_check "$(( $(estimate_kib "$datadir") * 2 ))"
  mkdir "$BASE_DIR/mysql"
  "${SQL[@]}" -NBe 'SELECT @@lower_case_table_names' > "$BASE_DIR/mysql/lower_case_table_names"
  grep -qxE '[012]' "$BASE_DIR/mysql/lower_case_table_names" || fail 'Invalid source lower_case_table_names.'
  "${SQL[@]}" -NBe "SELECT SCHEMA_NAME FROM information_schema.SCHEMATA WHERE SCHEMA_NAME NOT IN ('information_schema','performance_schema','sys','mysql') ORDER BY SCHEMA_NAME" > "$BASE_DIR/mysql/databases.txt"
  # Default locks cover nontransactional tables too. Each database is a separate snapshot.
  # Native SQL installs triggers after their table data, unlike --tab + mysqlimport.
  while IFS= read -r db; do
    [[ -n $db && $db != *$'\t'* && $db != *$'\r'* && $db != *\\* ]] || fail 'Unsupported control character in database name.'
    databases+=("$db")
    hex=$(printf %s "$db" | sha256sum | cut -d' ' -f1)
    datam_log_phase logical-dump "database=$db file=$hex.sql$suffix"
    "$dump" "--defaults-extra-file=$MY_CNF" -h "$DBHOST" --default-character-set="$DUMP_CHARSET" --max-allowed-packet=1024M --opt --routines --events --triggers --hex-blob --databases -- "$db" \
      | compress > "$BASE_DIR/mysql/$hex.sql$suffix"
    printf '%s\t%s.sql%s\n' "$db" "$hex" "$suffix" >> "$BASE_DIR/mysql/index.tsv"
  done < "$BASE_DIR/mysql/databases.txt"
  touch "$BASE_DIR/mysql/index.tsv"
  if (( ${#databases[@]} )); then
    # Native multi-database dumping creates all base objects before final views.
    # Only schema is repeated; full per-database dumps retain selective restore.
    datam_log_phase schema-bootstrap-export
    "$dump" "--defaults-extra-file=$MY_CNF" -h "$DBHOST" --default-character-set="$DUMP_CHARSET" --max-allowed-packet=1024M --opt --no-data --skip-triggers --routines --skip-events --databases -- "${databases[@]}" \
      | compress > "$BASE_DIR/mysql/all-schema.sql$suffix"
  fi
  # System tables are reference media, never automatically imported across releases.
  datam_log_phase account-reference
  "$dump" "--defaults-extra-file=$MY_CNF" -h "$DBHOST" --default-character-set="$DUMP_CHARSET" --max-allowed-packet=1024M --opt --routines --events --triggers --hex-blob --databases mysql \
    | compress > "$BASE_DIR/mysql/system-reference.sql$suffix"
  if "$dump" --help 2>/dev/null | grep -- '--system' >/dev/null; then
    "$dump" "--defaults-extra-file=$MY_CNF" -h "$DBHOST" --default-character-set="$DUMP_CHARSET" --system=users > "$BASE_DIR/mysql/accounts-review.sql"
  fi
  cp -- "$SCRIPT_DIR/mysql-restore.sh" "$BASE_DIR/mysql/restore.sh"
  cp -- "$SCRIPT_DIR/logging.sh" "$BASE_DIR/mysql/"
  (cd "$BASE_DIR/mysql"; find . -maxdepth 1 -type f ! -name SHA256SUMS -print0 | sort -z | xargs -0 -r sha256sum > SHA256SUMS)
}
backup_binlogs() {
  datam_log_phase binlogs
  local tool file active size enabled
  enabled=$("${SQL[@]}" -NBe 'SELECT @@log_bin')
  case $enabled in
    0) echo 'Binlogs disabled; no PITR coverage.' > "$BASE_DIR/binlogs-unavailable.txt"; return ;;
    1) ;;
    *) fail 'Could not determine whether binary logging is enabled.' ;;
  esac
  tool=$(client mariadb-binlog mysqlbinlog) || fail 'MariaDB binlog client missing.'
  # Validate names and budget capacity from the pre-rotation listing so a refusal leaves the
  # server's binlog set unchanged. The active log can still grow, and concurrent rotation can
  # add a closed log, before the flush; the per-file recheck below and ENOSPC cover that gap.
  local listing
  listing=$("${SQL[@]}" -NBe 'SHOW BINARY LOGS')
  while IFS=$'\t' read -r file _; do
    [[ -z $file || ( $file =~ ^[a-zA-Z0-9._-]+$ && $file != . && $file != .. ) ]] || fail 'Unsafe binlog name.'
  done <<< "$listing"
  space_check "$(printf '%s\n' "$listing" | awk '{s+=$2} END {printf "%.0f\n", s/1024+1}')"
  mkdir "$BASE_DIR/binlog"
  "${SQL[@]}" -e 'FLUSH BINARY LOGS'
  "${SQL[@]}" -NBe 'SHOW BINARY LOGS' > "$BASE_DIR/binlog/index.tsv"
  active=$(tail -n 1 "$BASE_DIR/binlog/index.tsv" | cut -f1)
  while IFS=$'\t' read -r file size _; do
    [[ $file != "$active" ]] || continue
    [[ $file =~ ^[a-zA-Z0-9._-]+$ && $file != . && $file != .. ]] || fail 'Unsafe binlog name.'
    (cd "$BASE_DIR/binlog"; "$tool" "--defaults-extra-file=$MY_CNF" -h "$DBHOST" --read-from-remote-server --raw "$file")
    [[ $(stat -c %s "$BASE_DIR/binlog/$file") == "$size" ]] || fail "Incomplete binlog: $file"
    if [[ -n $suffix ]]; then
      # The raw file still occupies space; allow output expansion before compression.
      # 1% plus 64 KiB covers zstd/gzip framing for incompressible input.
      datam_log_phase binlog-compression "file=$file"
      space_check "$(( (size + size / 100 + 65536) / 1024 + 1 ))"
      compress < "$BASE_DIR/binlog/$file" > "$BASE_DIR/binlog/$file$suffix"
      rm -- "$BASE_DIR/binlog/$file"
    fi
  done < "$BASE_DIR/binlog/index.tsv"
  printf '%s\n' 'Closed logs only. Per-database logical dumps do not define one global PITR coordinate.' > "$BASE_DIR/binlog/README.txt"
}
s3_upload() {
  [[ -n $s3_bucket ]] || return 0
  local dest="s3://$s3_bucket/$SOURCE_ID/$RUN_ID"
  datam_log_phase s3-upload "provider=$provider destination=$dest"
  # File/physical trees are now inside their archive; remaining media must be regular.
  [[ -z $(find "$BASE_DIR" ! -type f ! -type d -print -quit) ]] || fail 'Unexpected non-regular S3 media; upload an archive instead.'
  aws "${s3_args[@]}" s3 sync "$BASE_DIR/" "$dest/" --no-follow-symlinks --exclude COMPLETE --exclude INCOMPLETE --only-show-errors
  datam_log_phase s3-publish
  aws "${s3_args[@]}" s3 cp "$BASE_DIR/COMPLETE" "$dest/COMPLETE" --only-show-errors
}
case $mode in
  backup-files) files_backup "$@" ;;
  backup-all-mariabackup) files_backup "$@"; physical_backup "$@" ;;
  backup-mariabackup) physical_backup "$@" ;;
  backup-mysql) logical_backup ;;
  backup-all) logical_backup; backup_binlogs ;;
  backup-binlogs) backup_binlogs ;;
esac
if [[ $mode == backup-files || $mode == backup-all-mariabackup || $mode == backup-mariabackup ]]; then
  if [[ ${2:-} == comp || $FILES_TARBALL_CREATION == [yY] ]]; then
    need zstd
    parts=(); [[ ! -d $BASE_DIR/files ]] || parts+=(files)
    [[ ! -d $BASE_DIR/mariadb_tmp ]] || parts+=(mariadb_tmp)
    space_check "$(estimate_kib "${parts[@]/#/$BASE_DIR/}")"
    datam_log_phase archive "file=$BASE_DIR/centminmod_backup.tar.zst"
    tar --numeric-owner --acls --xattrs --sparse -C "$BASE_DIR" -cpf - "${parts[@]}" | (COMPRESSION_METHOD=zstd; compress) > "$BASE_DIR/centminmod_backup.tar.zst"
    # Keep staging on any archive failure. On success avoid a second full-data read.
    for part in "${parts[@]}"; do rm -rf -- "${BASE_DIR:?}/$part"; done
  fi
fi
datam_log_phase inventory
(cd "$BASE_DIR"; find . -type f ! -path ./inventory.tsv ! -path ./INCOMPLETE ! -path ./COMPLETE ! -path ./SHA256SUMS -printf '%s\t%P\n' > inventory.tsv)
if [[ $CHECKSUMS == [yY] ]]; then
  (cd "$BASE_DIR"; find . -type f ! -path ./SHA256SUMS ! -path ./INCOMPLETE -print0 | sort -z | xargs -0 -r sha256sum > SHA256SUMS)
fi
printf 'Completed UTC %s\n' "$(date -u +%FT%TZ)" > "$BASE_DIR/COMPLETE"
rm -- "$BASE_DIR/INCOMPLETE"
s3_upload
# Machine-readable result: exactly one record, emitted only after all requested stages.
printf 'BACKUP_RESULT\t%s\n' "$BASE_DIR"
echo "Backup saved to $BASE_DIR"
