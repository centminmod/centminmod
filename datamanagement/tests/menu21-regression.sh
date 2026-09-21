#!/bin/bash
# Run as root on an EL test VM. Creates a private-socket MariaDB, never stops installed services.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d /var/tmp/menu21-test.XXXXXX)
export CENTMINLOGDIR="$work/job-logs"
chmod 755 "$work"
server=$(command -v mariadbd || command -v mysqld)
mysql=$(command -v mariadb || command -v mysql)
installer=$(command -v mariadb-install-db || command -v mysql_install_db)
pid=''; test_service=''; mounted=''
cleanup() {
 rc=$?
 if [[ -n $mounted ]]; then umount "$work/mounted-db" 2>/dev/null || :; fi
 if [[ -n $test_service ]]; then
   systemctl stop "$test_service" || :
   rm -f "/run/systemd/system/$test_service.service"
   systemctl daemon-reload
 fi
 if [[ -n $pid ]]; then kill "$pid" 2>/dev/null || :; wait "$pid" 2>/dev/null || :; fi
 rm -rf -- "$work"
 exit "$rc"
}
trap cleanup EXIT
trap 'exit 130' INT TERM
mkdir "$work/db"; chown mysql:mysql "$work/db"
"$installer" --no-defaults --datadir="$work/db" --user=mysql > "$work/install.log" 2>&1
mkdir "$work/run"; chown mysql:mysql "$work/run"
"$server" --no-defaults --datadir="$work/db" --socket="$work/run/mysql.sock" --pid-file="$work/run/mysql.pid" --skip-networking --user=mysql --log-bin="$work/db/mysql-bin" --server-id=721 --log-error="$work/run/error.log" & pid=$!
for (( i=0; i<60; i++ )); do [[ ! -S $work/run/mysql.sock ]] || break; sleep .5; done
cat > "$work/client.cnf" <<CNF
[client]
user=root
socket=$work/run/mysql.sock
[mariabackup]
socket=$work/run/mysql.sock
CNF
SQL=("$mysql" "--defaults-extra-file=$work/client.cnf" --default-character-set=utf8mb4)
"${SQL[@]}" -e 'SELECT 1' >/dev/null
"${SQL[@]}" <<'SQL'
DROP DATABASE IF EXISTS test;
CREATE DATABASE `menu21-fixture` CHARACTER SET utf8mb4;
USE `menu21-fixture`;
CREATE TABLE items(id INT PRIMARY KEY, amount INT, txt TEXT, binary_data BLOB, optional INT NULL);
CREATE TRIGGER increment_amount BEFORE INSERT ON items FOR EACH ROW SET NEW.amount=NEW.amount+1;
INSERT INTO items VALUES(1,10,'hello 🍉',UNHEX('0001FF'),NULL),(2,20,'world',UNHEX('0A0D'),3);
CREATE TABLE computed(a INT, b INT GENERATED ALWAYS AS (a*2) STORED);
INSERT INTO computed(a) VALUES(7);
CREATE TABLE legacy(txt VARCHAR(8) CHARACTER SET latin1);
INSERT INTO legacy VALUES(UNHEX('C4F1FF'));
CREATE TABLE child(id INT PRIMARY KEY, item INT, FOREIGN KEY(item) REFERENCES items(id));
INSERT INTO child VALUES(1,1);
CREATE VIEW totals AS SELECT SUM(amount) AS amount FROM items;
CREATE PROCEDURE sample() SELECT COUNT(*) FROM items;
CREATE EVENT future_event ON SCHEDULE EVERY 1 DAY DISABLE DO INSERT INTO items VALUES(99,99,'event',NULL,NULL);
SQL
cat > "$work/config" <<CFG
MY_CNF='$work/client.cnf'
BACKUP_DIR_PARENT='$work/backups'
FILES_BACKUP_ROOT='$work/files-backups'
SOURCE_ID=fixture
DIRECTORIES_TO_BACKUP=('$work/source/config file' '$work/source/config-dir')
DOMAINS_ROOT='$work/source/domains'
CFG
mkdir -p "$work/source/config-dir" "$work/source/domains/site"
printf 'synthetic secret\n' > "$work/source/config file"
printf 'dotfile\n' > "$work/source/domains/site/.hidden"
ln -s ../config-dir "$work/source/domains/link"
runbackup() {
 BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" "$@" > "$work/backup.out" 2>&1 || { cat "$work/backup.out"; return 1; }
 result=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$work/backup.out")
 [[ -f $result/COMPLETE && ! -e $result/INCOMPLETE ]]
}
# Sparse allocation and hardlinks spanning separate configured source roots.
truncate -s 67108864 "$work/source/config-dir/sparse"
printf tail | dd of="$work/source/config-dir/sparse" bs=1 seek=67108860 conv=notrunc status=none
ln "$work/source/config file" "$work/source/domains/site/cross-root-link"
for reserved in SHA256SUMS COMPLETE INCOMPLETE inventory.tsv; do
  printf 'payload %s\n' "$reserved" > "$work/source/config-dir/$reserved"
done
printf 'CHECKSUMS=y\n' >> "$work/config"
runbackup backup-files
copy="$result/files$work/source"
[[ $(stat -c %i "$copy/config file") == $(stat -c %i "$copy/domains/site/cross-root-link") ]]
[[ $(stat -c %b "$copy/config-dir/sparse") -lt 1024 ]]
cmp "$work/source/config-dir/sparse" "$copy/config-dir/sparse"
for reserved in SHA256SUMS COMPLETE INCOMPLETE inventory.tsv; do
  grep -Fq "files$work/source/config-dir/$reserved" "$result/SHA256SUMS"
  grep -Fq "files$work/source/config-dir/$reserved" "$result/inventory.tsv"
done
(cd "$result"; sha256sum --check --quiet SHA256SUMS)
echo 'PASS sparse allocation, cross-root hardlinks and reserved-basename payload checksums'
# Root-owned parents, legacy mysql-owned namespace, and malicious namespace entries.
mkdir "$work/path-root" "$work/path-victim"
chmod 755 "$work/path-victim"
printf sentinel > "$work/path-victim/kept"
cp "$work/config" "$work/path-config"
printf "FILES_BACKUP_ROOT='%s/path-root'\n" "$work" >> "$work/path-config"
for target in "$work/path-victim" "$work/missing-victim"; do
  ln -s "$target" "$work/path-root/fixture"
  if BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log" 2>&1; then exit 1; fi
  grep -q 'Refusing symlinked source directory' "$work/path.log"
  [[ $(stat -c %a "$work/path-victim") == 755 && ! -e $work/missing-victim ]]
  [[ $(cat "$work/path-victim/kept") == sentinel ]]
  rm "$work/path-root/fixture"
done
printf file > "$work/path-root/fixture"
if BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log" 2>&1; then exit 1; fi
rm "$work/path-root/fixture"
mkdir "$work/path-root/fixture"
chown mysql:mysql "$work/path-root" "$work/path-root/fixture"
BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log"
[[ $(stat -c '%u:%a' "$work/path-root") == 0:700 && $(stat -c '%u:%a' "$work/path-root/fixture") == 0:700 ]]
ln -s "$work/path-root" "$work/trusted-alias"
printf "FILES_BACKUP_ROOT='%s/trusted-alias'\n" "$work" >> "$work/path-config"
BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log"
mkdir "$work/untrusted-parent"
chown mysql:mysql "$work/untrusted-parent"
printf "FILES_BACKUP_ROOT='%s/untrusted-parent/backup'\n" "$work" >> "$work/path-config"
if BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log" 2>&1; then exit 1; fi
[[ ! -e $work/untrusted-parent/backup ]]
# Additional parent-boundary controls: writable nonsticky, missing, and owned links.
mkdir "$work/writable-parent"
chmod 777 "$work/writable-parent"
for denied in "$work/writable-parent/backup" "$work/missing-parent/backup"; do
  printf "FILES_BACKUP_ROOT='%s'\n" "$denied" >> "$work/path-config"
  if BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log" 2>&1; then exit 1; fi
  [[ ! -e $denied ]]
done
ln -s "$work/path-victim" "$work/untrusted-link"
chown -h mysql:mysql "$work/untrusted-link"
printf "FILES_BACKUP_ROOT='%s/untrusted-link///'\n" "$work" >> "$work/path-config"
if BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log" 2>&1; then exit 1; fi
[[ $(stat -c %a "$work/path-victim") == 755 ]]
chmod 1777 "$work/writable-parent"
printf "FILES_BACKUP_ROOT='%s/writable-parent/backup'\n" "$work" >> "$work/path-config"
BACKUP_CONFIG="$work/path-config" bash "$ROOT/backups.sh" backup-files > "$work/path.log"
echo 'PASS source/parent link and permission refusals, sticky parent, trusted alias and legacy migration'
for mode in backup-files backup-mariabackup backup-all-mariabackup; do
  for compression in pigz none; do
    if BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" "$mode" "$compression" > "$work/encoding.log" 2>&1; then exit 1; fi
    grep -q 'accept only the optional comp argument' "$work/encoding.log"
  done
done
echo 'PASS unsupported file/physical compression arguments rejected'
# Hide only tar from prerequisite discovery; failure must precede any generation.
cat > "$work/no-tar-env" <<'STUB'
command() {
  if [[ ${1:-} == -v && ${2:-} == tar ]]; then return 1; fi
  builtin command "$@"
}
STUB
cp "$work/config" "$work/no-tar-config"
printf "FILES_BACKUP_ROOT='%s/no-tar-backups'\n" "$work" >> "$work/no-tar-config"
if BASH_ENV="$work/no-tar-env" BACKUP_CONFIG="$work/no-tar-config" bash "$ROOT/backups.sh" backup-files comp > "$work/no-tar.log" 2>&1; then exit 1; fi
grep -q 'Install required command: tar' "$work/no-tar.log"
[[ ! -e $work/no-tar-backups ]]
echo 'PASS missing tar refused before archive staging'
runbackup backup-files
cmp "$work/source/config file" "$result/files$work/source/config file"
[[ -f $result/files$work/source/domains/site/.hidden ]]
[[ $(stat -c %a "$result") == 700 ]]
[[ -L $result/files$work/source/domains/link && $(readlink "$result/files$work/source/domains/link") == ../config-dir ]]
! grep -Eq $'\t(INCOMPLETE|COMPLETE)$' "$result/inventory.tsv" || exit 1
echo 'PASS regular-file, hidden-file, symlink, inventory and private generation'
runbackup backup-files comp
mkdir "$work/extract"
zstd -qdc "$result/centminmod_backup.tar.zst" | tar -xf - -C "$work/extract"
cmp "$work/source/config file" "$work/extract/files$work/source/config file"
echo 'PASS archive coverage parity'
runbackup backup-all
[[ -s $result/binlog/mysql-bin.000001.zst ]]
zstd -qt "$result/binlog/mysql-bin.000001.zst"
echo "PASS real closed-binlog capture, size gate and compression"
# A missing encoder must be refused before the production binlog is rotated.
cat > "$work/no-pigz-env" <<'STUB'
command() {
  if [[ ${1:-} == -v && ${2:-} == pigz ]]; then return 1; fi
  builtin command "$@"
}
STUB
binlogs_before=$("${SQL[@]}" -NBe 'SHOW BINARY LOGS' | wc -l)
if BASH_ENV="$work/no-pigz-env" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-binlogs pigz > "$work/no-pigz.log" 2>&1; then exit 1; fi
grep -q 'Install required command: pigz' "$work/no-pigz.log"
[[ $("${SQL[@]}" -NBe 'SHOW BINARY LOGS' | wc -l) == "$binlogs_before" ]]
! grep -q '^BACKUP_RESULT' "$work/no-pigz.log" || exit 1
echo 'PASS missing binlog encoder refused before binary log rotation'
logical="$result/mysql"
# Corrupt data must fail before creating the database, and remain available for diagnosis.
"${SQL[@]}" -e 'DROP DATABASE `menu21-fixture`'
file=$(cut -f2 "$logical/index.tsv")
cp "$logical/$file" "$work/saved.zst"
printf broken > "$logical/$file"
if MY_CNF="$work/client.cnf" bash "$logical/restore.sh" all > "$work/restore.log" 2>&1; then echo 'FAIL corrupt restore accepted'; exit 1; fi
[[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME='menu21-fixture'") == 0 ]]
[[ -f $logical/$file ]]
grep -Fq "$file: FAILED" "$work/restore.log"
grep -q "phase=checksum-validation" "$work/restore.log"
cp "$work/saved.zst" "$logical/$file"
cp "$logical/SHA256SUMS" "$work/complete-manifest"
for missing in index.tsv "$file"; do
  grep -vF "$missing" "$work/complete-manifest" > "$logical/SHA256SUMS"
  for selection in all selective; do
    args=(all); [[ $selection != selective ]] || args=(--database menu21-fixture)
    if MY_CNF="$work/client.cnf" bash "$logical/restore.sh" "${args[@]}" > "$work/missing-checksum.log" 2>&1; then
      echo "FAIL unlisted $missing accepted for $selection"; exit 1
    fi
    grep -q 'not checksummed' "$work/missing-checksum.log"
    [[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME='menu21-fixture'") == 0 ]]
  done
done
cp "$work/complete-manifest" "$logical/SHA256SUMS"
echo 'PASS missing index/dump manifest records refuse all/selective restore before creation'
# Delegate every real SQL operation except the one deliberately controlled query.
mkdir "$work/query-bin"
cat > "$work/query-bin/mariadb" <<'STUB'
#!/bin/bash
if [[ ${*: -1} == "$STATUS_QUERY" ]]; then
  case $STATUS_RESPONSE in
    fail) echo 'Fixture status query failed' >&2; exit 9 ;;
    empty) exit 0 ;;
    *) printf '%s\n' "$STATUS_RESPONSE"; exit 0 ;;
  esac
fi
exec "$REAL_SQL" "$@"
STUB
chmod +x "$work/query-bin/mariadb"
for state in fail empty unexpected ON; do
  for selection in all selective; do
    args=(all); [[ $selection != selective ]] || args=(--database menu21-fixture)
    rc=0
    PATH="$work/query-bin:$PATH" REAL_SQL="$mysql" STATUS_QUERY='SELECT @@event_scheduler' STATUS_RESPONSE="$state" \
      MY_CNF="$work/client.cnf" bash "$logical/restore.sh" "${args[@]}" > "$work/scheduler.log" 2>&1 || rc=$?
    expected=1; [[ $state != fail ]] || expected=9
    [[ $rc == "$expected" ]]
    grep -q 'No database import was started' "$work/scheduler.log"
    [[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME='menu21-fixture'") == 0 ]]
  done
done
echo 'PASS scheduler query errors, empty/unknown values and ON block all/selective mutations'
for state in OFF DISABLED; do
  for selection in all selective; do
    args=(all); [[ $selection != selective ]] || args=(--database menu21-fixture)
    PATH="$work/query-bin:$PATH" REAL_SQL="$mysql" STATUS_QUERY='SELECT @@event_scheduler' STATUS_RESPONSE="$state" \
      MY_CNF="$work/client.cnf" bash "$logical/restore.sh" "${args[@]}" > "$work/scheduler.log" 2>&1
    [[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1') == 11 ]]
    "${SQL[@]}" -e 'DROP DATABASE `menu21-fixture`'
  done
done
echo 'PASS explicitly OFF/DISABLED scheduler states allow all/selective recovery'
for state in fail empty unexpected; do
  cp "$work/config" "$work/binlog-query-config"
  printf "BACKUP_DIR_PARENT='%s/binlog-query-%s'\n" "$work" "$state" >> "$work/binlog-query-config"
  rc=0
  PATH="$work/query-bin:$PATH" REAL_SQL="$mysql" STATUS_QUERY='SELECT @@log_bin' STATUS_RESPONSE="$state" \
    BACKUP_CONFIG="$work/binlog-query-config" bash "$ROOT/backups.sh" backup-binlogs > "$work/binlog-query.log" 2>&1 || rc=$?
  expected=1; [[ $state != fail ]] || expected=9
  [[ $rc == "$expected" ]]
  [[ -n $(find "$work/binlog-query-$state" -name INCOMPLETE -print -quit) ]]
  [[ -z $(find "$work/binlog-query-$state" \( -name COMPLETE -o -name binlogs-unavailable.txt \) -print -quit) ]]
  ! grep -q '^BACKUP_RESULT' "$work/binlog-query.log" || exit 1
done
PATH="$work/query-bin:$PATH" REAL_SQL="$mysql" STATUS_QUERY='SELECT @@log_bin' STATUS_RESPONSE=0 \
  BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-binlogs > "$work/binlog-disabled.log" 2>&1
disabled=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$work/binlog-disabled.log")
[[ -f $disabled/COMPLETE && ! -e $disabled/INCOMPLETE ]]
grep -q 'Binlogs disabled' "$disabled/binlogs-unavailable.txt"
echo 'PASS binlog status failures prevent completion; explicit disabled state remains documented'
MY_CNF="$work/client.cnf" bash "$logical/restore.sh" all
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1') == 11 ]]
[[ $("${SQL[@]}" -NBe 'SELECT HEX(binary_data) FROM `menu21-fixture`.items WHERE id=1') == 0001FF ]]
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.totals') == 32 ]]
[[ $("${SQL[@]}" -NBe 'CALL `menu21-fixture`.sample()') == 2 ]]
[[ $("${SQL[@]}" -NBe 'SELECT COUNT(*) FROM `menu21-fixture`.child') == 1 ]]
if MY_CNF="$work/client.cnf" bash "$logical/restore.sh" all > "$work/existing.log" 2>&1; then echo 'FAIL existing DB overwritten'; exit 1; fi
[[ $("${SQL[@]}" -NBe 'SELECT optional IS NULL FROM `menu21-fixture`.items WHERE id=1') == 1 ]]
[[ $("${SQL[@]}" -NBe 'SELECT HEX(txt) FROM `menu21-fixture`.items WHERE id=1') == 68656C6C6F20F09F8D89 ]]
[[ $("${SQL[@]}" -NBe 'SELECT b FROM `menu21-fixture`.computed') == 14 ]]
[[ $("${SQL[@]}" -NBe 'SELECT HEX(txt) FROM `menu21-fixture`.legacy') == C4F1FF ]]
[[ $("${SQL[@]}" -NBe "SELECT STATUS FROM information_schema.EVENTS WHERE EVENT_SCHEMA='menu21-fixture' AND EVENT_NAME='future_event'") == DISABLED ]]
echo 'PASS emoji, NULL, generated column, event state and latin1 values'
echo 'PASS native SQL triggers, binary, NULL, FK, views, routines, events, hyphenated names, corruption and existing-database refusal'
# Producer failure must not publish a result or COMPLETE.
mkdir "$work/bin"
cat > "$work/bin/rsync" <<'STUB'
#!/bin/bash
exit 9
STUB
chmod +x "$work/bin/rsync"
if PATH="$work/bin:$PATH" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files > "$work/failure.log" 2>&1; then exit 1; fi
! grep -q '^BACKUP_RESULT' "$work/failure.log" || exit 1
echo 'PASS producer failure propagation'
rm "$work/bin/rsync"
# A file vanishing under du is an estimate problem, not a backup failure; ENOSPC still fails.
real_du=$(command -v du)
cat > "$work/bin/du" <<'STUB'
#!/bin/bash
"$REAL_DU" "$@"
exit 1
STUB
chmod +x "$work/bin/du"
PATH="$work/bin:$PATH" REAL_DU="$real_du" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files > "$work/du.log" 2>&1 || { cat "$work/du.log"; exit 1; }
grep -q '^BACKUP_RESULT' "$work/du.log"
grep -q 'partial-estimate' "$work/du.log"
rm "$work/bin/du"
# Every configured source missing is a configuration error, never an empty COMPLETE generation.
cp "$work/config" "$work/no-source-config"
printf "DIRECTORIES_TO_BACKUP=('%s/absent-a' '%s/absent-b')\nDOMAINS_ROOT='%s/absent-domains'\nFILES_BACKUP_ROOT='%s/no-source-backups'\n" "$work" "$work" "$work" "$work" >> "$work/no-source-config"
if BACKUP_CONFIG="$work/no-source-config" bash "$ROOT/backups.sh" backup-files > "$work/no-source.log" 2>&1; then exit 1; fi
grep -q 'No configured file source exists' "$work/no-source.log"
! grep -q '^BACKUP_RESULT' "$work/no-source.log" || exit 1
[[ -z $(find "$work/no-source-backups" -name COMPLETE -print -quit) ]]
echo 'PASS advisory capacity estimates and missing-source refusal'
cat > "$work/bin/zstd" <<'STUB'
#!/bin/bash
cat >/dev/null
exit 9
STUB
chmod +x "$work/bin/zstd"
if PATH="$work/bin:$PATH" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files comp > "$work/compression.log" 2>&1; then exit 1; fi
! grep -q '^BACKUP_RESULT' "$work/compression.log" || exit 1
find "$work/files-backups" -name INCOMPLETE | grep . >/dev/null
rm "$work/bin/zstd"
echo 'PASS archive compressor failure retains incomplete generation'

# Capture actual archive compression arguments, including the configured fast/level choice.
real_zstd=$(command -v zstd)
cat > "$work/bin/zstd" <<'STUB'
#!/bin/bash
printf '%s\n' "$@" >> "$ZSTD_ARGS_LOG"
exec "$REAL_ZSTD" "$@"
STUB
chmod +x "$work/bin/zstd"
cp "$work/config" "$work/archive-config"
printf 'FASTCOMPRESS_ZSTD=n\nCOMPRESSION_LEVEL_ZSTD=7\n' >> "$work/archive-config"
PATH="$work/bin:$PATH" REAL_ZSTD="$real_zstd" ZSTD_ARGS_LOG="$work/zstd-args" BACKUP_CONFIG="$work/archive-config" bash "$ROOT/backups.sh" backup-files comp > "$work/archive.log"
grep -qx -- '-7' "$work/zstd-args"
: > "$work/zstd-args"
printf 'FASTCOMPRESS_ZSTD=y\nCOMPRESSION_LEVEL_ZSTD=4\n' >> "$work/archive-config"
PATH="$work/bin:$PATH" REAL_ZSTD="$real_zstd" ZSTD_ARGS_LOG="$work/zstd-args" BACKUP_CONFIG="$work/archive-config" bash "$ROOT/backups.sh" backup-files comp > "$work/archive.log"
grep -qx -- '--fast=4' "$work/zstd-args"
rm "$work/bin/zstd"
# First space check passes; a fresh post-staging check must catch exhausted disk.
cat > "$work/bin/df" <<'STUB'
#!/bin/bash
if [[ -e $SPACE_COUNT ]]; then avail=1; else avail=999999999; touch "$SPACE_COUNT"; fi
printf 'Filesystem 1024-blocks Used Available Capacity Mounted\nfixture 999999999 0 %s 0%% /\n' "$avail"
STUB
chmod +x "$work/bin/df"
cp "$work/config" "$work/space-config"
printf "FILES_BACKUP_ROOT='%s/space-backups'\n" "$work" >> "$work/space-config"
if PATH="$work/bin:$PATH" SPACE_COUNT="$work/space-count" BACKUP_CONFIG="$work/space-config" bash "$ROOT/backups.sh" backup-files comp > "$work/space.log" 2>&1; then exit 1; fi
grep -q 'Insufficient space' "$work/space.log"
find "$work/space-backups" -name INCOMPLETE | grep . >/dev/null
find "$work/space-backups" -type d -name files | grep . >/dev/null
rm "$work/bin/df"
echo 'PASS archive tuning and post-staging disk-space refusal retain staged files'

# Real binlog capture, but simulated exhaustion after the initial capacity check.
cat > "$work/bin/df" <<'STUB'
#!/bin/bash
if [[ -e $SPACE_COUNT ]]; then avail=1; else avail=999999999; touch "$SPACE_COUNT"; fi
printf 'Filesystem 1024-blocks Used Available Capacity Mounted\nfixture 999999999 0 %s 0%% /\n' "$avail"
STUB
chmod +x "$work/bin/df"
cp "$work/config" "$work/binlog-space-config"
printf "BACKUP_DIR_PARENT='%s/binlog-space'\n" "$work" >> "$work/binlog-space-config"
if PATH="$work/bin:$PATH" SPACE_COUNT="$work/binlog-space-count" BACKUP_CONFIG="$work/binlog-space-config" bash "$ROOT/backups.sh" backup-binlogs > "$work/binlog-space.log" 2>&1; then exit 1; fi
grep -q 'Insufficient space' "$work/binlog-space.log"
grep -q 'phase=binlog-compression' "$work/binlog-space.log"
find "$work/binlog-space" -name INCOMPLETE | grep . >/dev/null
[[ -n $(find "$work/binlog-space" -name 'mysql-bin.[0-9]*' ! -name '*.zst' -print -quit) ]]
[[ -z $(find "$work/binlog-space" -name '*.zst' -o -name COMPLETE) ]]
rm "$work/bin/df"
echo 'PASS compressed binlog output-space refusal preserves raw staging before compression'

# Fail extraction after consuming the stream. The receiver must not publish RECEIVED.
cat > "$work/bin/tar" <<'STUB'
#!/bin/bash
cat >/dev/null
exit 9
STUB
chmod +x "$work/bin/tar"
mkdir -p "$work/receiver/data"
if tar -C "$work/source" -cpf - . | zstd -q | PATH="$work/bin:$PATH" bash "$ROOT/tunnel-transfers.sh" --receive "$work/receiver" ssh 12345 127.0.0.1 127.0.0.1 131072; then exit 1; fi
[[ ! -e $work/receiver/RECEIVED && $(stat -c %a "$work/receiver/receiver.log") == 600 ]]
grep -q "tar=9" "$work/receiver/receiver.log"
rm "$work/bin/tar"
echo 'PASS receiver extraction failure propagation'

# Exercise the combined file+physical S3 packaging path before standalone restore tests.
cp "$work/config" "$work/combined-config"
printf 'AWSUPLOAD=y\nAWS_PROFILE=fixture\nAWS_BUCKETNAME=fixture\n' >> "$work/combined-config"
cat > "$work/bin/aws" <<'STUB'
#!/bin/bash
exit 0
STUB
chmod +x "$work/bin/aws"
PATH="$work/bin:$PATH" BACKUP_CONFIG="$work/combined-config" bash "$ROOT/backups.sh" backup-all-mariabackup > "$work/combined.log" 2>&1 || { cat "$work/combined.log"; exit 1; }
combined=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$work/combined.log")
[[ -f $combined/centminmod_backup.tar.zst && ! -d $combined/files && ! -d $combined/mariadb_tmp ]]
mkdir "$work/combined-restored"
zstd -qdc "$combined/centminmod_backup.tar.zst" | tar --numeric-owner --acls --xattrs -xpf - -C "$work/combined-restored"
cmp "$work/source/config file" "$work/combined-restored/files$work/source/config file"
DATADIR="$work/combined-target" bash "$ROOT/mariabackup-restore.sh" check "$work/combined-restored/mariadb_tmp" > "$work/combined-check.log"
rm "$work/bin/aws"
echo 'PASS automatic combined file/physical S3 archive and extracted physical preflight'
runbackup backup-mariabackup
physical="$result/mariadb_tmp"
DATADIR="$work/restore-db" bash "$ROOT/mariabackup-restore.sh" check "$physical"
# A datadir that is its own mount point cannot be preserved by rename; refuse before any service change.
mkdir "$work/mounted-db" "$work/bind-source"
for mount_kind in tmpfs bind; do
  if [[ $mount_kind == tmpfs ]]; then mount -t tmpfs -o size=4m none "$work/mounted-db" 2>/dev/null || { echo 'SKIP mount-point datadir: tmpfs mount unavailable'; continue; }
  else mount --bind "$work/bind-source" "$work/mounted-db" 2>/dev/null || { echo 'SKIP mount-point datadir: bind mount unavailable'; continue; }; fi
  mounted=y
  if DATADIR="$work/mounted-db" bash "$ROOT/mariabackup-restore.sh" check "$physical" > "$work/mount.log" 2>&1; then exit 1; fi
  grep -q 'separate mount point' "$work/mount.log"
  ! grep -q 'Preflight passed' "$work/mount.log" || exit 1
  umount "$work/mounted-db"; mounted=''
  echo "PASS $mount_kind mount-point datadir refused during preflight"
done
# Ask the real server to format a spaced datadir through its help output.
mkdir "$work/mysql data"
cat > "$work/bin/mariadbd" <<'STUB'
#!/bin/bash
exec "$REAL_SERVER" --no-defaults "--datadir=$SPACED_DATADIR" "$@"
STUB
chmod +x "$work/bin/mariadbd"
(
  unset DATADIR
  PATH="$work/bin:$PATH" REAL_SERVER="$server" SPACED_DATADIR="$work/mysql data" \
    bash "$ROOT/mariabackup-restore.sh" check "$physical" > "$work/spaced-datadir.log" 2>&1
)
grep -Fxq "Preflight passed for $work/mysql data" "$work/spaced-datadir.log"
DATADIR="$work/mysql data" bash "$ROOT/mariabackup-restore.sh" check "$physical" > "$work/explicit-datadir.log" 2>&1
grep -Fxq "Preflight passed for $work/mysql data" "$work/explicit-datadir.log"
rm "$work/bin/mariadbd"
echo 'PASS real server help preserves spaced datadir with automatic and explicit discovery'
# Current modern filename aliases, then deliberate conflicting metadata.
cp "$physical/xtrabackup_info" "$physical/mariadb_backup_info"
cp "$physical/xtrabackup_checkpoints" "$physical/mariadb_backup_checkpoints"
DATADIR="$work/restore-db" bash "$ROOT/mariabackup-restore.sh" check "$physical"
mv "$physical/xtrabackup_info" "$work/legacy_info"
mv "$physical/xtrabackup_checkpoints" "$work/legacy_checkpoints"
DATADIR="$work/restore-db" bash "$ROOT/mariabackup-restore.sh" check "$physical"
mv "$work/legacy_info" "$physical/xtrabackup_info"
mv "$work/legacy_checkpoints" "$physical/xtrabackup_checkpoints"
echo conflict >> "$physical/mariadb_backup_info"
if DATADIR="$work/restore-db" bash "$ROOT/mariabackup-restore.sh" check "$physical" >/dev/null 2>&1; then exit 1; fi
rm "$physical/mariadb_backup_info" "$physical/mariadb_backup_checkpoints"
mkdir "$work/restore-db"; echo original > "$work/restore-db/original"
cat > "$work/bin/systemctl" <<'STUB'
#!/bin/bash
[[ $* != 'show mariadb -p ActiveState --value' ]] || { echo inactive; exit; }
exit 9
STUB
chmod +x "$work/bin/systemctl"
if PATH="$work/bin:$PATH" DATADIR="$work/restore-db" bash "$ROOT/mariabackup-restore.sh" copy-back "$physical" > "$work/stop.log" 2>&1; then exit 1; fi
[[ -f $work/restore-db/original ]]
grep -q 'MariaDB stop failed' "$work/stop.log"
echo 'PASS real physical backup/prepare, offline metadata check, dual-name conflicts, failed stop preserves datadir'
# Actual copy-back into another private directory, boot and query the restored data.
kill "$pid"; wait "$pid"; pid=''
mkdir "$work/copied"
rm "$work/bin/systemctl"
test_service="menu21-regression-$$"
cat > "/run/systemd/system/$test_service.service" <<UNIT
[Unit]
Description=Disposable menu21 restore regression
[Service]
Type=notify
ExecStart=$server --no-defaults --datadir=$work/copied --socket=$work/run/mysql.sock --pid-file=$work/run/mysql.pid --skip-networking --user=mysql --log-error=$work/run/copy-error.log
TimeoutStartSec=30
TimeoutStopSec=30
UNIT
systemctl daemon-reload
MARIADB_SERVICE="$test_service" DATADIR="$work/copied" bash "$ROOT/mariabackup-restore.sh" copy-back "$physical" > "$work/copy.log" 2>&1 || { cat "$work/copy.log"; exit 1; }
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1') == 11 ]]
# The tool copies unrecognized backup files; the bundled helpers must not remain in the live datadir.
[[ ! -e $work/copied/logging.sh && ! -e $work/copied/mariabackup-restore.sh ]]
[[ -f $physical/logging.sh && -f $physical/mariabackup-restore.sh ]]
echo 'PASS real isolated systemd restore, main-process/datadir identity and helper-free datadir'
main_pid=$(systemctl show "$test_service" -p MainPID --value)
if MARIADB_SERVICE="$test_service" DATADIR="$work/wrong-db" bash "$ROOT/mariabackup-restore.sh" copy-back "$physical" > "$work/wrong.log" 2>&1; then exit 1; fi
[[ $(systemctl show "$test_service" -p MainPID --value) == "$main_pid" ]]
grep -q 'nothing stopped' "$work/wrong.log"
cat > "$work/bin/mv" <<'STUB'
#!/bin/bash
exit 9
STUB
chmod +x "$work/bin/mv"
if PATH="$work/bin:$PATH" MARIADB_SERVICE="$test_service" DATADIR="$work/copied" bash "$ROOT/mariabackup-restore.sh" copy-back "$physical" > "$work/rename.log" 2>&1; then exit 1; fi
rm "$work/bin/mv"
grep -q "original-service start command succeeded" "$work/rename.log"
grep -q "initial=active final=active" "$work/rename.log"
systemctl is-active --quiet "$test_service"
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1') == 11 ]]
echo 'PASS wrong active datadir refused, failed rename restarts unchanged original'
# Source case-sensitive databases and tables must never merge on a folding target.
"${SQL[@]}" -e 'CREATE DATABASE Foo; CREATE DATABASE foo; CREATE TABLE Foo.items(v INT); INSERT INTO Foo.items VALUES(111); CREATE TABLE foo.items(v INT); INSERT INTO foo.items VALUES(222); CREATE TABLE Foo.ITEMS(v INT); INSERT INTO Foo.ITEMS VALUES(333)'
runbackup backup-mysql
case_logical="$result/mysql"
"${SQL[@]}" -e 'DROP DATABASE Foo; DROP DATABASE foo; DROP DATABASE `menu21-fixture`'
MY_CNF="$work/client.cnf" bash "$case_logical/restore.sh" all
[[ $("${SQL[@]}" -NBe 'SELECT v FROM Foo.items') == 111 && $("${SQL[@]}" -NBe 'SELECT v FROM foo.items') == 222 && $("${SQL[@]}" -NBe 'SELECT v FROM Foo.ITEMS') == 333 ]]
echo 'PASS same-mode distinct database and table names round trip'
# Run the current helper against legacy media without replacing its embedded helper.
cp -a "$logical" "$work/legacy-logical"
rm "$work/legacy-logical/lower_case_table_names"
(cd "$work/legacy-logical"; find . -maxdepth 1 -type f ! -name SHA256SUMS -print0 | sort -z | xargs -0 sha256sum > SHA256SUMS)
"${SQL[@]}" -e 'DROP DATABASE `menu21-fixture`'
if MY_CNF="$work/client.cnf" LOGICAL_BACKUP_DIR="$work/legacy-logical" bash "$ROOT/mysql-restore.sh" all > "$work/legacy.log" 2>&1; then exit 1; fi
grep -q 'Legacy media needs' "$work/legacy.log"
if MY_CNF="$work/client.cnf" SOURCE_LOWER_CASE_TABLE_NAMES=1 bash "$logical/restore.sh" all > "$work/contradiction.log" 2>&1; then exit 1; fi
grep -q 'contradicts' "$work/contradiction.log"
MY_CNF="$work/client.cnf" LOGICAL_BACKUP_DIR="$work/legacy-logical" SOURCE_LOWER_CASE_TABLE_NAMES=0 bash "$ROOT/mysql-restore.sh" all
(cd "$work/legacy-logical"; sha256sum -c SHA256SUMS >/dev/null)
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1') == 11 ]]
echo 'PASS legacy external helper, required source assertion, contradictory override and unchanged media checksums'
# An inactive service configured for another datadir must not report a successful restore.
systemctl stop "$test_service"
if MARIADB_SERVICE="$test_service" DATADIR="$work/offline-wrong" bash "$ROOT/mariabackup-restore.sh" copy-back "$physical" > "$work/offline.log" 2>&1; then exit 1; fi
grep -q 'not using the restored datadir' "$work/offline.log"
! systemctl is-active --quiet "$test_service" || exit 1
! grep -q '^Restore completed' "$work/offline.log" || exit 1
echo 'PASS offline service/datadir mismatch stopped and reported as failure'

source "$ROOT/../inc/datamanager.inc"
printf '[one]\na=1\n[two]\nb=2\n[default]\nc=3\n' > "$work/ini"
datam_profile_section "$work/ini" one remove > "$work/remaining"
grep -q '\[two\]' "$work/remaining"; grep -q '\[default\]' "$work/remaining"
[[ $(datam_profile_section "$work/ini" two export | wc -l) == 2 ]]
echo 'PASS adjacent INI section boundaries'
mkdir -p "$work/menu-stub/datamanagement" "$work/menu-tmp"
printf '#!/bin/bash\nexit 9\n' > "$work/menu-stub/datamanagement/backups.sh"
chmod +x "$work/menu-stub/datamanagement/backups.sh"
if SCRIPT_DIR="$work/menu-stub" TMPDIR="$work/menu-tmp" bash -c 'set -e; source "$1/../inc/datamanager.inc"; datam_backup backup-files' -- "$ROOT"; then exit 1; fi
[[ -z $(ls -A "$work/menu-tmp") ]]
echo 'PASS menu backup failure and temporary-result cleanup'

cat > "$work/config" <<CFG
FILES_BACKUP_ROOT='$work/s3-backups'
SOURCE_ID=fixture
DIRECTORIES_TO_BACKUP=('$work/source')
DOMAINS_ROOT='$work/nonexistent'
AWSUPLOAD=y
AWS_PROFILE=fixture-profile
AWS_BUCKETNAME=fixture-bucket
AWS_ENDPOINT=' --endpoint-url=https://fixture.invalid'
CFG
cat > "$work/bin/aws" <<'STUB'
#!/bin/bash
printf '%s\t' "$@" >> "$AWS_TEST_LOG"
printf '\n' >> "$AWS_TEST_LOG"
[[ ${AWS_TEST_FAIL:-n} != y ]]
STUB
chmod +x "$work/bin/aws"
PATH="$work/bin:$PATH" AWS_TEST_LOG="$work/aws.log" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files > "$work/ok.log"
grep -q '^BACKUP_RESULT' "$work/ok.log"
s3_generation=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$work/ok.log")
[[ -f $s3_generation/centminmod_backup.tar.zst && ! -d $s3_generation/files ]]
mkdir "$work/s3-restored"
zstd -qdc "$s3_generation/centminmod_backup.tar.zst" | tar --numeric-owner --acls --xattrs -xpf - -C "$work/s3-restored"
cmp "$work/source/config file" "$work/s3-restored/files$work/source/config file"
[[ -L $work/s3-restored/files$work/source/domains/link ]]
grep -q -- '--no-follow-symlinks' "$work/aws.log"
echo 'PASS automatic S3 file archive retains file contents and symlinks'
[[ $(wc -l < "$work/aws.log") == 2 ]]
sed -n '1p' "$work/aws.log" | grep -q $'s3\tsync'
sed -n '2p' "$work/aws.log" | grep -q '/COMPLETE'
grep -q -- '--profile.fixture-profile' "$work/aws.log"
grep -q -- '--endpoint-url.https://fixture.invalid' "$work/aws.log"
grep -q 's3://fixture-bucket/fixture/' "$work/aws.log"
if PATH="$work/bin:$PATH" AWS_TEST_FAIL=y AWS_TEST_LOG="$work/aws.log" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files > "$work/fail.log" 2>&1; then exit 1; fi
! grep -q '^BACKUP_RESULT' "$work/fail.log" || exit 1
# The local generation completed before the upload; an upload failure must preserve it as complete.
s3_failed=$(sed -n 's/^Backup failed ([0-9]*)\. Preserved: //p' "$work/fail.log")
[[ -n $s3_failed && $s3_failed != "$s3_generation" ]]
[[ -f $s3_failed/COMPLETE && ! -e $s3_failed/INCOMPLETE && -f $s3_failed/centminmod_backup.tar.zst ]]
echo 'PASS S3 profile/endpoint/bucket/namespace, marker-last publication and failure status with API adapter'

cp "$work/config" "$work/aws-config"
for provider in BACKBLAZE DIGITALOCEAN LINODE CFR2 UPCLOUD; do
  sed '/^AWS/d' "$work/aws-config" > "$work/provider-config"
  printf '%s_UPLOAD=y\n%s_BUCKETNAME=fixture-bucket\n' "$provider" "$provider" >> "$work/provider-config"
  case $provider in
    BACKBLAZE) expected=b2; endpoint=backblazeb2.com ;;
    DIGITALOCEAN) expected='do'; endpoint=digitaloceanspaces.com ;;
    LINODE) expected=linode; endpoint=linodeobjects.com ;;
    CFR2) expected=r2; endpoint=account.r2.cloudflarestorage.com; echo CFR2_ACCOUNTID=account >> "$work/provider-config" ;;
    UPCLOUD) expected=upcloud; endpoint=fixture.upcloudobjects.com; echo UPCLOUD_ENDPOINT=https://fixture.upcloudobjects.com >> "$work/provider-config" ;;
  esac
  : > "$work/aws.log"
  PATH="$work/bin:$PATH" AWS_TEST_LOG="$work/aws.log" BACKUP_CONFIG="$work/provider-config" bash "$ROOT/backups.sh" backup-files > "$work/provider.log"
  grep -q -- "--profile.$expected" "$work/aws.log"
  grep -q -- "$endpoint" "$work/aws.log"
done
for invalid in multiple endpoint malformed legacy; do
  cp "$work/aws-config" "$work/invalid-config"
  case $invalid in
    multiple) echo BACKBLAZE_UPLOAD=y >> "$work/invalid-config" ;;
    endpoint) sed -i '/^AWS/d' "$work/invalid-config"; printf 'UPCLOUD_UPLOAD=y\nUPCLOUD_BUCKETNAME=fixture\n' >> "$work/invalid-config" ;;
    malformed) echo AWS_ENDPOINT=https:// >> "$work/invalid-config" ;;
    legacy) echo 'BASE_DIR="$MOST_FREE_SPACE_MOUNT/databackup/$DT"' >> "$work/invalid-config" ;;
  esac
  : > "$work/aws.log"
  if PATH="$work/bin:$PATH" AWS_TEST_LOG="$work/aws.log" BACKUP_CONFIG="$work/invalid-config" bash "$ROOT/backups.sh" backup-files > "$work/invalid.log" 2>&1; then exit 1; fi
  [[ ! -s $work/aws.log ]]
done
echo 'PASS all provider defaults, R2 account expansion, pre-upload rejection and obsolete config diagnostics'
# Profile deletion must reject either symlink before editing either file.
printf '[fixture]\nx=1\n' > "$work/credentials"
printf '[profile fixture]\nregion=test\n' > "$work/profile-config"
ln -s "$work/credentials" "$work/credentials-link"
if printf 'fixture\ny\n' | PATH="$work/bin:$PATH" AWS_CONFIG_FILE="$work/profile-config" AWS_SHARED_CREDENTIALS_FILE="$work/credentials-link" datam_profile_action 5 > "$work/profile.log" 2>&1; then exit 1; fi
grep -q '\[profile fixture\]' "$work/profile-config"
grep -q '\[fixture\]' "$work/credentials"
echo 'PASS profile symlink refusal before mutation'
# Two actions in the same menu process always get distinct private logs.
(
  datam_action() { echo "action-$1"; }
  printf '13\n13\n14\n' | CENTMINLOGDIR="$work/menu-logs" datamanager_menu >/dev/null
)
[[ $(find "$work/menu-logs" -type f | wc -l) == 2 ]]
[[ $(find "$work/menu-logs" -type f ! -perm 600 | wc -l) == 0 ]]
echo 'PASS unique private menu action logs'

mkdir -p "$work/home/.ssh"
ssh-keygen -q -t ed25519 -N '' -f "$work/home/.ssh/test.key" -C old
before=$(sha256sum "$work/home/.ssh/test.key")
cat > "$work/bin/ssh" <<'STUB'
#!/bin/bash
for arg; do case $arg in *.key.new-*) exit 9 ;; esac; done
exit 0
STUB
chmod +x "$work/bin/ssh"
if HOME="$work/home" PATH="$work/bin:$PATH" bash "$ROOT/keygen.sh" rotatekeys ed25519 127.0.0.1 22 root new test > "$work/key.log" 2>&1; then exit 1; fi
[[ $(sha256sum "$work/home/.ssh/test.key") == "$before" ]]
find "$work/home/.ssh" -name 'test.key.new-*' | grep . >/dev/null
! grep -q 'BEGIN OPENSSH PRIVATE KEY' "$work/key.log" || exit 1
echo 'PASS failed replacement verification preserves old key and recovery candidate, no private key output'
systemctl stop "$test_service"
mkdir "$work/casefold-db"; chown mysql:mysql "$work/casefold-db"
"$installer" --no-defaults --user=mysql --lower-case-table-names=1 --datadir="$work/casefold-db" > "$work/install.log" 2>&1
"$server" --no-defaults --lower-case-table-names=1 --datadir="$work/casefold-db" --socket="$work/run/mysql.sock" --pid-file="$work/run/mysql.pid" --skip-networking --user=mysql --log-error="$work/run/error.log" & pid=$!
for ((i=0;i<60;i++)); do [[ ! -S $work/run/mysql.sock ]] || break; sleep .5; done
printf '[client]\nuser=root\nsocket=%s/run/mysql.sock\n' "$work" > "$work/client.cnf"
SQL=("$mysql" "--defaults-extra-file=$work/client.cnf")
if MY_CNF="$work/client.cnf" bash "$case_logical/restore.sh" all > "$work/case-collision.log" 2>&1; then exit 1; fi
grep -q 'lower_case_table_names must match' "$work/case-collision.log"
[[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME IN ('Foo','foo','menu21-fixture')") == 0 ]]
echo 'PASS cross-mode database/table collisions refused before SQL mutation'
"${SQL[@]}" -e 'CREATE DATABASE `menu21-fixture`; CREATE TABLE `menu21-fixture`.items(id INT, amount INT); INSERT INTO `menu21-fixture`.items VALUES(1,999)'
mkdir "$work/casefold-export"
cp "$ROOT/mysql-restore.sh" "$work/casefold-export/restore.sh"
cp "$ROOT/logging.sh" "$work/casefold-export/"
name=MENU21-FIXTURE
file_id=$(printf %s "$name" | sha256sum | cut -d' ' -f1)
printf '%s\t%s.sql\n' "$name" "$file_id" > "$work/casefold-export/index.tsv"
cat > "$work/casefold-export/$file_id.sql" <<'SQL'
CREATE DATABASE IF NOT EXISTS `MENU21-FIXTURE`;
USE `MENU21-FIXTURE`;
DROP TABLE IF EXISTS items;
CREATE TABLE items(id INT, amount INT);
INSERT INTO items VALUES(1,11);
SQL
echo 1 > "$work/casefold-export/lower_case_table_names"
(cd "$work/casefold-export"; sha256sum index.tsv restore.sh lower_case_table_names "$file_id.sql" > SHA256SUMS)
rc=0
MY_CNF="$work/client.cnf" bash "$work/casefold-export/restore.sh" all > "$work/result.log" 2>&1 || rc=$?
value=$("${SQL[@]}" -NBe 'SELECT amount FROM `menu21-fixture`.items WHERE id=1')
printf 'restore_exit=%s existing_value=%s\n' "$rc" "$value"
if [[ $rc == 0 || $value != 999 ]]; then echo 'FAIL case-folding overwrite'; exit 1; fi
echo 'PASS existing case-folded database preserved'

# Source only the installer function, avoiding package/network setup in this adapter test.
# shellcheck disable=SC1090
source <(sed -n '/^getcli() {/,/^}/p' "$ROOT/awscli-get.sh")
export AWSCLI_BINPATH="$work/bin"
getcli install default '' '' '' '' > "$work/aws-install-only.log"
if getcli install default fake-key '' region json > "$work/aws-partial.log" 2>&1; then exit 1; fi
if getcli install default fake-key fake-secret '' json > "$work/aws-region.log" 2>&1; then exit 1; fi
# Capture actual profile arguments without writing credentials or calling a provider.
cat > "$work/bin/aws" <<'STUB'
#!/bin/bash
printf '%s\n' "$*" >> "$AWS_PROFILE_ARGS"
[[ $2 != --profile ]] || { cat > "$AWS_PROFILE_STDIN"; exit 0; }
[[ $* != 'configure get region --profile fixture-profile' ]] || echo fixture-region
exit 0
STUB
chmod +x "$work/bin/aws"
source <(sed -n '/^r2_config() {/,/^}/p; /^configure_profile() {/,/^}/p' "$ROOT/awscli-get.sh")
export AWS_PROFILE_ARGS="$work/aws-profile-args" AWS_PROFILE_STDIN="$work/aws-profile-stdin"
getcli install fixture-profile fake-key fake-secret fixture-region json > "$work/aws-named.log"
grep -qx 'configure --profile fixture-profile' "$AWS_PROFILE_ARGS"
[[ $(cat "$AWS_PROFILE_STDIN") == $'fake-key\nfake-secret\nfixture-region\njson' ]]
! grep -q 'fake-secret' "$AWS_PROFILE_ARGS" || exit 1
grep -qx 'configure get region --profile fixture-profile' "$AWS_PROFILE_ARGS"
! grep -q default.region "$AWS_PROFILE_ARGS" || exit 1
echo 'PASS AWS binary-only setup, invalid credentials and named-profile region isolation'
# A valid-checksum dump can still fail midway through SQL application.
mkdir "$work/partial-logical"
cp "$ROOT/logging.sh" "$work/partial-logical/"
cp "$ROOT/mysql-restore.sh" "$work/partial-logical/restore.sh"
partial_db=menu21_partial_logging
partial_file=$(printf %s "$partial_db" | sha256sum | cut -d' ' -f1).sql
printf 'CREATE DATABASE `%s`;\nUSE `%s`;\nCREATE TABLE retained(v INT);\nINVALID SQL;\n' "$partial_db" "$partial_db" > "$work/partial-logical/$partial_file"
printf '%s\t%s\n' "$partial_db" "$partial_file" > "$work/partial-logical/index.tsv"
"${SQL[@]}" -NBe 'SELECT @@lower_case_table_names' > "$work/partial-logical/lower_case_table_names"
(cd "$work/partial-logical"; sha256sum ./* > SHA256SUMS)
if MY_CNF="$work/client.cnf" bash "$work/partial-logical/restore.sh" all > "$work/partial-restore.log" 2>&1; then exit 1; fi
grep -q 'phase=database-import' "$work/partial-restore.log"
grep -q 'completed_databases=0' "$work/partial-restore.log"
grep -q 'current database may be partially restored' "$work/partial-restore.log"
[[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_SCHEMA='$partial_db' AND TABLE_NAME='retained'") == 1 ]]
(cd "$work/partial-logical"; sha256sum --check --quiet SHA256SUMS)
echo 'PASS partial SQL failure context and unchanged checksummed restore media'
echo 'ALL PASS'
