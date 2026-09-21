#!/bin/bash
# Run as root on an EL test VM. Only a private-socket disposable server is changed.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d /var/tmp/menu21-dependencies.XXXXXX)
chmod 755 "$work"
export CENTMINLOGDIR="$work/logs"
pid=''
cleanup() {
  rc=$?
  if [[ -n $pid ]]; then kill "$pid" 2>/dev/null || :; wait "$pid" 2>/dev/null || :; fi
  rm -rf -- "$work"
  exit "$rc"
}
trap cleanup EXIT
trap 'exit 130' INT TERM
server=$(command -v mariadbd || command -v mysqld)
installer=$(command -v mariadb-install-db || command -v mysql_install_db)
mysql=$(command -v mariadb || command -v mysql)
mkdir "$work/db" "$work/run"
chown mysql:mysql "$work/db" "$work/run"
"$installer" --no-defaults --datadir="$work/db" --user=mysql > "$work/install.log" 2>&1
"$server" --no-defaults --datadir="$work/db" --socket="$work/run/mysql.sock" --pid-file="$work/run/mysql.pid" --skip-networking --event-scheduler=OFF --user=mysql --log-error="$work/run/error.log" & pid=$!
printf '[client]\nuser=root\nsocket=%s/run/mysql.sock\n' "$work" > "$work/client.cnf"
SQL=("$mysql" "--defaults-extra-file=$work/client.cnf")
for ((i=0;i<100;i++)); do
  if "${SQL[@]}" -NBe 'SELECT 1' >/dev/null 2>&1; then break; fi
  sleep .2
done
"${SQL[@]}" <<'SQL'
DROP DATABASE IF EXISTS test;
CREATE DATABASE z_data;
CREATE DATABASE m_middle;
CREATE DATABASE a_view;
CREATE DATABASE solo;
CREATE TABLE z_data.items(id INT PRIMARY KEY, amount INT);
CREATE TRIGGER z_data.increment_amount BEFORE INSERT ON z_data.items FOR EACH ROW SET NEW.amount=NEW.amount+1;
INSERT INTO z_data.items VALUES(1,10);
CREATE FUNCTION z_data.bump(x INT) RETURNS INT DETERMINISTIC RETURN x+1;
CREATE VIEW m_middle.v AS SELECT id,amount FROM z_data.items;
CREATE VIEW a_view.direct_v AS SELECT id,amount FROM z_data.items;
CREATE VIEW a_view.chain_v AS SELECT id,amount,z_data.bump(amount) AS answer FROM m_middle.v;
CREATE TABLE a_view.child(id INT PRIMARY KEY, parent_id INT, FOREIGN KEY(parent_id) REFERENCES z_data.items(id));
INSERT INTO a_view.child VALUES(1,1);
CREATE PROCEDURE a_view.sample() SELECT amount FROM z_data.items WHERE id=1;
CREATE EVENT z_data.future_event ON SCHEDULE EVERY 1 DAY DISABLE DO INSERT INTO z_data.items VALUES(99,99);
CREATE TABLE solo.items(id INT PRIMARY KEY);
INSERT INTO solo.items VALUES(7);
SQL
cat > "$work/config" <<CFG
MY_CNF='$work/client.cnf'
BACKUP_DIR_PARENT='$work/backups'
SOURCE_ID=dependencies
CFG
BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-mysql none > "$work/backup.log" 2>&1 || { cat "$work/backup.log"; exit 1; }
result=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$work/backup.log")
media="$result/mysql"
[[ -f $result/COMPLETE && -s $media/all-schema.sql ]]
cp "$media/all-schema.sql" "$work/schema.saved"
drop_apps() { "${SQL[@]}" -e 'SET FOREIGN_KEY_CHECKS=0; DROP DATABASE IF EXISTS a_view; DROP DATABASE IF EXISTS m_middle; DROP DATABASE IF EXISTS z_data; DROP DATABASE IF EXISTS solo'; }
empty_apps() { [[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME IN ('a_view','m_middle','z_data','solo')") == 0 ]]; }
restore() { MY_CNF="$work/client.cnf" bash "$media/restore.sh" "$@"; }
# shellcheck disable=SC2094 # The output manifest is explicitly excluded from input.
manifest() { (cd "$1"; find . -maxdepth 1 -type f ! -name SHA256SUMS -print0 | sort -z | xargs -0 -r sha256sum > SHA256SUMS); }
drop_apps
printf '\ncorrupt\n' >> "$media/all-schema.sql"
if restore all > "$work/corrupt.log" 2>&1; then echo 'FAIL corrupt bootstrap accepted'; exit 1; fi
empty_apps
cp "$work/schema.saved" "$media/all-schema.sql"
echo 'PASS corrupt bootstrap refused before schema mutation'
restore all > "$work/restore.log" 2>&1 || { cat "$work/restore.log"; exit 1; }
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM z_data.items WHERE id=1') == 11 ]]
[[ $("${SQL[@]}" -NBe 'SELECT amount FROM a_view.direct_v WHERE id=1') == 11 ]]
[[ $("${SQL[@]}" -NBe 'SELECT answer FROM a_view.chain_v WHERE id=1') == 12 ]]
[[ $("${SQL[@]}" -NBe 'CALL a_view.sample()') == 11 ]]
[[ $("${SQL[@]}" -NBe 'SELECT parent_id FROM a_view.child') == 1 ]]
[[ $("${SQL[@]}" -NBe "SELECT STATUS FROM information_schema.EVENTS WHERE EVENT_SCHEMA='z_data' AND EVENT_NAME='future_event'") == DISABLED ]]
echo 'PASS forward views, cross-database view chain/function/FK, routines, events and trigger-after-data'
"${SQL[@]}" -e 'DROP DATABASE solo'
restore --database solo > "$work/selective.log" 2>&1 || { cat "$work/selective.log"; exit 1; }
[[ $("${SQL[@]}" -NBe 'SELECT id FROM solo.items') == 7 ]]
[[ $("${SQL[@]}" -NBe 'SELECT answer FROM a_view.chain_v WHERE id=1') == 12 ]]
if grep -q 'phase=schema-bootstrap-import' "$work/selective.log"; then echo 'FAIL selective restore imported global schema'; exit 1; fi
echo 'PASS selective restore leaves other databases unchanged and skips global bootstrap'
cp -a "$media" "$work/legacy"
rm "$work/legacy/all-schema.sql"
awk -F '\t' '$1=="solo"' "$work/legacy/index.tsv" > "$work/solo-index"
cp "$work/solo-index" "$work/legacy/index.tsv"
manifest "$work/legacy"
"${SQL[@]}" -e 'DROP DATABASE solo'
MY_CNF="$work/client.cnf" LOGICAL_BACKUP_DIR="$work/legacy" bash "$ROOT/mysql-restore.sh" all > "$work/legacy.log" 2>&1 || { cat "$work/legacy.log"; exit 1; }
[[ $("${SQL[@]}" -NBe 'SELECT id FROM solo.items') == 7 ]]
grep -q 'Legacy media has no schema bootstrap' "$work/legacy.log"
echo 'PASS legacy independent-database media remains restorable with current external helper'
drop_apps
cp "$media/SHA256SUMS" "$work/manifest.saved"
sed '/all-schema\.sql$/d' "$work/manifest.saved" > "$media/SHA256SUMS"
if restore all > "$work/unlisted.log" 2>&1; then echo 'FAIL unchecksummed bootstrap accepted'; exit 1; fi
grep -q 'Schema bootstrap is not checksummed' "$work/unlisted.log"
empty_apps
cp "$work/manifest.saved" "$media/SHA256SUMS"
echo 'PASS unchecksummed bootstrap refused before schema mutation'
printf 'CREATE DATABASE a_view;\nINVALID SQL;\n' > "$media/all-schema.sql"
manifest "$media"
if restore all > "$work/schema-failure.log" 2>&1; then echo 'FAIL invalid bootstrap accepted'; exit 1; fi
grep -q 'Schema creation started across the selected databases' "$work/schema-failure.log"
[[ $("${SQL[@]}" -NBe "SELECT COUNT(*) FROM information_schema.SCHEMATA WHERE SCHEMA_NAME='a_view'") == 1 ]]
echo 'PASS partial schema failure retains media and explains recovery scope'
echo 'ALL PASS'
