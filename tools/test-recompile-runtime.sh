#!/usr/bin/env bash
# Isolated Linux runtime checks. Only temporary processes, configs and Unix sockets change.
set -euo pipefail
repo=$(cd "${1:-$(dirname "$0")/..}" && pwd)
[[ $(id -u) = 0 && $(uname -s) = Linux ]] || { echo 'Run as root on an authorized Linux test VM.' >&2; exit 1; }
# Starts real nginx and php-fpm masters: never on a production server by accident.
[[ "${CMM_RUNTIME_TEST:-}" = yes ]] || { echo 'Set CMM_RUNTIME_TEST=yes to run on a disposable test VM.' >&2; exit 1; }
for binary in /usr/local/sbin/nginx /usr/local/sbin/php-fpm; do
  [[ -x "$binary" ]] || { echo "Missing $binary" >&2; exit 1; }
done
# Centmin Mod mounts /tmp noexec; private /root paths also keep socket access isolated.
work=$(mktemp -d /root/cmm-runtime.XXXXXXXX)
mkdir "$work/logs"
main_before=$(systemctl show nginx php-fpm mariadb -p MainPID --value)
repo_before=$(git -C /usr/local/src/centminmod diff --binary | sha256sum)
cleanup() {
  local status=$? pid
  if ((status)); then
    for log in "$work/nginx.error.log" "$work/php.error.log"; do
      [[ ! -f "$log" ]] || tail -20 "$log" >&2
    done
  fi
  # USR2 briefly renames the pidfile before its replacement appears. Use owned argv.
  for ((n=0; n<50; n++)); do
    while read -r pid; do
      [[ "$pid" =~ ^[1-9][0-9]*$ && "$pid" -gt 1 ]] || continue
      builtin kill -QUIT "$pid" 2>/dev/null || true
    done < <(ps -eo pid=,args= | awk -v dir="$work/" 'index($0,dir) && ($2 == "nginx:" || $2 == "php-fpm:") && $3 == "master" {print $1}')
    sleep .1
    if ((n > 2)) && ! ps -eo args= | grep -F "$work/" | grep -Eq '^(nginx: master|php-fpm: master)'; then break; fi
  done
  if ps -eo args= | grep -F "$work/" | grep -Eq '^(nginx: master|php-fpm: master)'; then
    echo "Temporary master remains; preserve $work for recovery." >&2
  else
    rm -rf "$work"
  fi
}
trap cleanup EXIT
export work repo
python3 - <<'PY'
import os, re
from pathlib import Path
root, work = Path(os.environ['repo']), Path(os.environ['work'])
nginx = (root/'inc/nginx_upgrade.inc').read_text()
ready = re.search(r'^nginx_master_ready\(\) \{.*?^}', nginx, re.M | re.S)
assert ready, 'missing production Nginx readiness helper'
start = nginx.index('       /usr/local/sbin/nginx -t || return 1')
end = nginx.index('        # cecho "Checking OpenSSL version', start)
promotion = nginx[start:end]
assert all(x in promotion for x in ('kill -USR2', 'kill -WINCH', 'kill -QUIT', 'nginx_master_ready'))
promotion = promotion.replace('/usr/local/sbin/nginx -t', '"$work/nginx" -t -p "$work/" -c "$work/nginx.conf"')
promotion = promotion.replace('/usr/local/nginx/logs/nginx.pid', '$work/nginx.pid')
recover = re.search(r'^nginx_upgrade_recover\(\) \{.*?^}', nginx, re.M | re.S)
assert recover, 'missing production Nginx recovery helper'
recover = recover.group().replace('/usr/local/nginx/logs/nginx.pid', '$work/nginx.pid')
php = (root/'inc/php_upgrade.inc').read_text()
gate = re.search(r'^    /usr/local/sbin/php-fpm -t \|\| return \$\?\n    cmservice php-fpm restart \|\| return \$\?', php, re.M)
assert gate, 'missing production PHP config/restart gates'
gate = gate.group().replace('/usr/local/sbin/php-fpm -t', 'PHP_INI_SCAN_DIR= /usr/local/sbin/php-fpm -n -R -t -y "$work/php.conf"')
(work/'gates.sh').write_text(ready.group()+'\nnginx_promote() {\n'+promotion+'\n}\nphp_restart_gate() {\n'+gate+'\n}\n'+recover+'\n')
PY
source "$work/gates.sh"
echo 'SOURCE_HASHES'
sha256sum "$repo/inc/nginx_upgrade.inc" "$repo/inc/php_upgrade.inc" "$repo/tools/test-recompile-runtime.sh"

wait_pid() {
  for ((n=0; n<50; n++)); do
    [[ -s "$1" ]] && kill -0 "$(cat "$1")" 2>/dev/null && return 0
    sleep .1
  done
  return 1
}
wait_exit() {
  for ((n=0; n<50; n++)); do
    kill -0 "$1" 2>/dev/null || return 0
    sleep .1
  done
  return 1
}
cat > "$work/php.conf" <<EOF
[global]
pid = $work/php.pid
error_log = $work/php.error.log
daemonize = yes
[cmm_runtime]
user = root
group = root
listen = $work/php.sock
listen.mode = 0600
pm = static
pm.max_children = 1
catch_workers_output = yes
EOF
cp "$work/php.conf" "$work/php.valid.conf"
start_php() {
  PHP_INI_SCAN_DIR= /usr/local/sbin/php-fpm -n -R -y "$work/php.conf"
  wait_pid "$work/php.pid"
  for ((n=0; n<50; n++)); do
    [[ -S "$work/php.sock" ]] && return 0
    sleep .1
  done
  return 1
}
restart_calls=0
cmservice() {
  [[ "$*" = 'php-fpm restart' ]] || return 1
  restart_calls=$((restart_calls + 1))
  local old
  old=$(cat "$work/php.pid")
  kill -QUIT "$old" || return $?
  wait_exit "$old" || return 1
  start_php
}
start_php
old_php_pid=$(cat "$work/php.pid")
php_restart_gate
[[ $(cat "$work/php.pid") != "$old_php_pid" && "$restart_calls" = 1 ]]
old_php_pid=$(cat "$work/php.pid")
printf '\ninvalid_pool_directive = yes\n' >> "$work/php.conf"
if php_restart_gate > "$work/php.invalid.log" 2>&1; then echo 'FAIL: invalid PHP pool accepted'; exit 1; fi
[[ "$restart_calls" = 1 && $(cat "$work/php.pid") = "$old_php_pid" ]]
kill -0 "$old_php_pid"
cp "$work/php.valid.conf" "$work/php.conf"
echo 'PASS: real PHP-FPM config validation, isolated restart/socket readiness, invalid pool preserves master'

cp /usr/local/sbin/nginx "$work/nginx.good"
ln -s nginx.good "$work/nginx"
cat > "$work/slow.php" <<EOF
<?php file_put_contents('$work/slow.started', 'started'); sleep(3); echo 'slow-ok';
EOF
cat > "$work/nginx.conf" <<EOF
user root root;
worker_processes 1;
pid $work/nginx.pid;
error_log $work/nginx.error.log notice;
events { worker_connections 32; }
http {
  access_log off;
  client_body_temp_path $work/client_temp;
  fastcgi_temp_path $work/fastcgi_temp;
  server {
    listen unix:$work/nginx.sock;
    location / { return 200 'runtime-ok'; }
    location = /slow {
      fastcgi_pass unix:$work/php.sock;
      fastcgi_param SCRIPT_FILENAME $work/slow.php;
      fastcgi_param REQUEST_METHOD GET;
    }
  }
}
EOF
cp "$work/nginx.conf" "$work/nginx.valid.conf"
"$work/nginx" -p "$work/" -c "$work/nginx.conf"
wait_pid "$work/nginx.pid"
for ((n=0; n<50; n++)); do
  nginx_master_ready "$(cat "$work/nginx.pid")" && break
  sleep .1
done
old_nginx_pid=$(cat "$work/nginx.pid")
nginx_master_ready "$old_nginx_pid"
NGINX_ZERODT=y
kill() { printf '%s\n' "$*" >> "$work/signals"; builtin kill "$@"; }
: > "$work/signals"
printf '\ninvalid_nginx_directive;\n' >> "$work/nginx.conf"
if nginx_promote > "$work/nginx.invalid.log" 2>&1; then echo 'FAIL: invalid Nginx config accepted'; exit 1; fi
! grep -Eq '^-(USR2|WINCH|QUIT) ' "$work/signals"
nginx_master_ready "$old_nginx_pid"
cp "$work/nginx.valid.conf" "$work/nginx.conf"
echo 'PASS: real invalid Nginx config prevents live-upgrade signals'

curl --fail --silent --show-error --max-time 10 --unix-socket "$work/nginx.sock" http://localhost/slow > "$work/slow.response" &
slow_request=$!
for ((n=0; n<50; n++)); do [[ -s "$work/slow.started" ]] && break; sleep .1; done
[[ -s "$work/slow.started" ]]
if ! nginx_promote; then echo 'FAIL: real Nginx promotion gate'; exit 1; fi
wait "$slow_request"
[[ $(cat "$work/slow.response") = slow-ok ]]
wait_exit "$old_nginx_pid"
nginx_master_ready "$(cat "$work/nginx.pid")"
[[ $(curl --fail --silent --show-error --unix-socket "$work/nginx.sock" http://localhost/) = runtime-ok ]]
python3 - <<'PY'
import os
from pathlib import Path
signals=(Path(os.environ['work'])/'signals').read_text().splitlines()
actions=[x.split()[0] for x in signals if x.split()[0] in ('-USR2','-WINCH','-QUIT')]
assert actions == ['-USR2','-WINCH','-QUIT'], actions
PY
echo 'PASS: actual USR2/new master+workers/WINCH/active-request drain/QUIT promotion'

old_nginx_pid=$(cat "$work/nginx.pid")
cat > "$work/nginx.fail" <<EOF
#!/bin/sh
if [ "\$1" = -t ]; then exec "$work/nginx.good" "\$@"; fi
exit 72
EOF
chmod 700 "$work/nginx.fail"
ln -sfn nginx.fail "$work/nginx"
: > "$work/signals"
if nginx_promote > "$work/nginx.exec-failure.log" 2>&1; then echo 'FAIL: failed new master promoted'; exit 1; fi
grep -q 'New Nginx master did not start' "$work/nginx.exec-failure.log"
nginx_master_ready "$old_nginx_pid"
! grep -Eq '^-(WINCH|QUIT) ' "$work/signals"
[[ $(curl --fail --silent --show-error --unix-socket "$work/nginx.sock" http://localhost/) = runtime-ok ]]
ln -sfn nginx.good "$work/nginx"
echo 'PASS: real failed replacement exec preserves old master/workers and request service'

# Recovery after WINCH: the new master is up, the old master has no workers.
# HUP restarts the old workers without re-reading config, QUIT stops the new
# master, and the old master takes back the pid file.
old_nginx_pid=$(cat "$work/nginx.pid")
kill -USR2 "$old_nginx_pid"
for ((n=0; n<100; n++)); do
  # USR2 briefly leaves no pid file (set -e is on here)
  new_nginx_pid=$(cat "$work/nginx.pid" 2>/dev/null || true)
  [[ "$new_nginx_pid" != "$old_nginx_pid" ]] && nginx_master_ready "$new_nginx_pid" && break
  sleep .1
done
nginx_master_ready "$new_nginx_pid" && [[ "$new_nginx_pid" != "$old_nginx_pid" ]]
kill -WINCH "$old_nginx_pid"
for ((n=0; n<100; n++)); do
  ps -eo ppid=,args= | awk -v pid="$old_nginx_pid" '$1 == pid && $3 == "worker" { found=1 } END { exit found }' && break
  sleep .1
done
! nginx_master_ready "$old_nginx_pid"
nginx_upgrade_restore() { return 0; }
service() { return 1; }
nginx_old_pid=$old_nginx_pid nginx_winch_sent=y
nginx_upgrade_recover
wait_exit "$new_nginx_pid"
[[ $(cat "$work/nginx.pid") = "$old_nginx_pid" ]] && nginx_master_ready "$old_nginx_pid"
[[ ! -e "$work/nginx.pid.oldbin" ]]
[[ $(curl --fail --silent --show-error --unix-socket "$work/nginx.sock" http://localhost/) = runtime-ok ]]
echo 'PASS: real post-WINCH recovery restores the old master workers, stops the new master and returns the pid file'
[[ $(systemctl show nginx php-fpm mariadb -p MainPID --value) = "$main_before" ]]
[[ $(git -C /usr/local/src/centminmod diff --binary | sha256sum) = "$repo_before" ]]
echo 'PASS: main-service PIDs and existing tracked VM edits unchanged'
