#!/bin/bash
# nc/socat carry the bulk stream; SSH authenticates control and verifies its digest.
set -Eeuo pipefail
umask 077
fail() { echo "ERROR: $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null || fail "Missing command: $1"; }
# These internal operations are sent over authenticated SSH, never exposed as a daemon.
if [[ ${1:-} == --receive ]]; then
  shift
  stage=$1 method=$2 port=$3 bind=$4 peer=$5 buffer=$6
  exec 3>&2
  exec 2>>"$stage/receiver.log"
  cd -- "$stage"
  receiver_phase=initialization hash_pid=''
  receiver_log() { printf '%s receiver phase=%s %s\n' "$(date -u +%FT%TZ)" "$receiver_phase" "$*" >&2; }
  receiver_cleanup() {
    local rc=$?
    [[ -z $hash_pid ]] || kill "$hash_pid" 2>/dev/null || :
    rm -f digest.pipe || :
    receiver_log "exit_status=$rc"
    if (( rc )); then printf 'Receiver failed: phase=%s status=%s; diagnostics: %s/receiver.log\n' "$receiver_phase" "$rc" "$stage" >&3; fi
  }
  trap receiver_cleanup EXIT
  receiver_log "method=$method"
  printf '%s\n' "$$" > "$stage/receiver.pid"
  [[ -d data && ! -e stream.sha256 && ! -e RECEIVED ]] || fail 'Invalid receive state.'
  mkfifo digest.pipe
  sha256sum < digest.pipe > stream.sha256 & hash_pid=$!
  receiver() {
    case $method in
      nc) nc -4 -l "$bind" "$port" --recv-only --allow "$peer" --idle-timeout 60 ;;
      socat) socat -4 -u -T60 -b "$buffer" "TCP-LISTEN:$port,bind=$bind,reuseaddr,range=$peer/32,rcvbuf=$buffer" - ;;
      ssh) cat ;;
    esac
  }
  receiver_phase=receive-extract
  receiver_log 'started'
  if receiver | tee digest.pipe | zstd -qd | tar --numeric-owner --acls --xattrs --xattrs-include='*' --sparse -xpf - -C data; then
    receiver_statuses=("${PIPESTATUS[@]}")
  else
    receiver_statuses=("${PIPESTATUS[@]}")
  fi
  receiver_log "pipeline_status receiver=${receiver_statuses[0]} tee=${receiver_statuses[1]} zstd=${receiver_statuses[2]} tar=${receiver_statuses[3]}"
  receiver_rc=0
  for status in "${receiver_statuses[@]}"; do [[ $status == 0 ]] || receiver_rc=$status; done
  (( receiver_rc == 0 )) || exit "$receiver_rc"
  receiver_phase=receiver-digest
  if wait "$hash_pid"; then receiver_rc=0; else receiver_rc=$?; fi
  hash_pid=''
  receiver_log "sha256sum_status=$receiver_rc"
  (( receiver_rc == 0 )) || exit "$receiver_rc"
  touch RECEIVED
  receiver_phase=received
  receiver_log 'stream extracted and digest saved; publication pending'
  exit
fi
# Receiver scripts are embedded over SSH and do not have this adjacent helper.
# shellcheck source=logging.sh
source "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)/logging.sh"
datam_log_init transfer "$@"
usage() {
  echo "Usage: $0 -h HOST -s SOURCE -r DEST_ROOT [-p SSH_PORT] [-u USER] [-k KEY]"
  echo '  [-m nc|socat|ssh] [-l DATA_PORT] [-b SOCAT_BUFFER]'
  echo 'nc is the default. Raw data is unencrypted: use a trusted private network or -m ssh.'
  echo 'A new DEST_ROOT/source-id/run-id is published only after stream verification.'
  echo 'REMOTE_SUDO=y opts into passwordless sudo. No firewall or sshd policy is changed.'
}
remote_port=22 remote_user=root remote_server='' method=nc buffer_size=131072 listen_port=12345
backupdir='' remote_backupdir='' private_key=''
while getopts 'p:u:h:m:b:l:s:r:k:' opt; do
  case $opt in
    p) remote_port=$OPTARG ;; u) remote_user=$OPTARG ;; h) remote_server=$OPTARG ;;
    m) method=$OPTARG ;; b) buffer_size=$OPTARG ;; l) listen_port=$OPTARG ;;
    s) backupdir=$OPTARG ;; r) remote_backupdir=$OPTARG ;; k) private_key=$OPTARG ;;
    *) usage; exit 1 ;;
  esac
done
[[ -n $remote_server && -d $backupdir && $remote_backupdir == /* && $remote_backupdir != / ]] || { usage; exit 1; }
[[ $remote_server =~ ^[a-zA-Z0-9][a-zA-Z0-9.-]*$ && $remote_user =~ ^[a-zA-Z_][a-zA-Z0-9_-]*$ ]] || fail 'Use an IPv4 address or DNS hostname and a valid SSH username.'
[[ $remote_backupdir != *$'\n'* && $backupdir != *$'\n'* ]] || fail 'Newlines in paths are not supported.'
for port in "$remote_port" "$listen_port"; do
  [[ $port =~ ^[1-9][0-9]{0,4}$ ]] && (( port <= 65535 )) || fail 'Invalid port.'
done
[[ $buffer_size =~ ^[1-9][0-9]{0,8}$ ]] || fail 'Invalid buffer size.'
case $method in nc|socat|ssh) ;; *) fail 'Method must be nc, socat or ssh.' ;; esac
datam_log_event INFO "transfer_method=$method source=$backupdir remote=$remote_user@$remote_server destination_root=$remote_backupdir"
for cmd in tar zstd ssh sha256sum mkfifo; do need "$cmd"; done
[[ $method == ssh ]] || need "$method"
if [[ $method == nc ]]; then nc --version 2>&1 | grep -i ncat >/dev/null || fail 'nc must be Nmap Ncat.'; fi
backupdir=$(realpath -e -- "$backupdir")
DATAM_LOG_EXCLUDE_DIR="$backupdir" datam_log_check_location "$MENU21_LOG_FILE" || fail 'Set CENTMINLOGDIR outside the transfer source.'
[[ ! -e $backupdir/INCOMPLETE ]] || fail 'Source is an incomplete backup.'
ssh_cmd=(ssh -4 -T -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -o BatchMode=yes -o StrictHostKeyChecking=yes -o ConnectTimeout=10 -o ServerAliveInterval=15 -o ServerAliveCountMax=4 -p "$remote_port")
[[ -z $private_key ]] || { [[ -f $private_key ]] || fail 'Key not found.'; ssh_cmd+=(-i "$private_key" -o IdentitiesOnly=yes); }
remote() {
  local script=$1 arg command
  shift
  printf -v command 'bash -c %q --' "$script"
  for arg in "$@"; do printf -v command '%s %q' "$command" "$arg"; done
  [[ ${1:-} != --receive ]] || command="setsid --wait $command"
  [[ ${REMOTE_SUDO:-n} != y ]] || command="sudo -n $command"
  if [[ ${1:-} == --receive && ${3:-} == ssh ]]; then
    "${ssh_cmd[@]}" "$remote_user@$remote_server" "$command"
  else
    "${ssh_cmd[@]}" -n "$remote_user@$remote_server" "$command"
  fi
}
datam_log_phase ssh-preflight
connection=$("${ssh_cmd[@]}" -n "$remote_user@$remote_server" 'printf "%s\n" "$SSH_CONNECTION"')
read -r peer _ bind _ <<< "$connection"
[[ $peer =~ ^[0-9.]+$ && $bind =~ ^[0-9.]+$ ]] || fail 'This transfer requires IPv4 SSH connectivity.'
source_id=${SOURCE_ID:-}
if [[ -z $source_id && -f $backupdir/backup.info ]]; then
  source_id=$(awk -F= '$1=="source" {print $2}' "$backupdir/backup.info")
fi
source_id=${source_id:-$(cat /etc/machine-id)}
[[ $source_id =~ ^[a-zA-Z0-9._-]+$ && $source_id != . && $source_id != .. ]] || fail 'Invalid SOURCE_ID.'
run_id=$(date -u +%Y%m%dT%H%M%SZ)-$$-$RANDOM
# Source identity is for collision avoidance, not a tenant authorization boundary.
remote_root=$(remote 'set -eu; mkdir -p -- "$1"; realpath -e -- "$1"' "$remote_backupdir")
[[ $remote_root == /* && $remote_root != / && $remote_root != *$'\n'* && $remote_root != *$'\r'* ]] || fail 'Invalid remote root response; remote shell startup must be quiet.'
stage="$remote_root/.incoming/$source_id/$run_id"
final="$remote_root/$source_id/$run_id"
size=$(du -sk -- "$backupdir" | awk '{print $1}')
datam_log_phase receiver-preflight "stage=$stage estimated_source_kib=$size"
remote 'set -euo pipefail; umask 077
for c in tar zstd sha256sum mkfifo ss; do command -v "$c" >/dev/null; done
if [[ $3 != ssh ]]; then command -v "$3" >/dev/null; fi
if [[ $3 == nc ]]; then nc --version 2>&1 | grep -i ncat >/dev/null; fi
avail=$(df -Pk "$1" | awk "END {print \$4}")
(( avail > $4 + $4 / 10 )) || { echo "Insufficient receiver disk space" >&2; exit 1; }
mkdir -p -- "$(dirname -- "$2")"
mkdir -- "$2"
mkdir -- "$2/data"
if [[ $3 != ssh ]] && ss -ltnH "sport = :$5" | grep . >/dev/null; then echo "Data port is busy" >&2; exit 1; fi
' "$remote_root" "$stage" "$method" "$size" "$listen_port"
work=$(mktemp -d)
ssh_pid='' hash_pid=''
cleanup() {
  rc=$?
  [[ -z $ssh_pid ]] || kill "$ssh_pid" 2>/dev/null || :
  [[ -z $hash_pid ]] || kill "$hash_pid" 2>/dev/null || :
  rm -rf -- "$work"
  if (( rc )); then
    remote 'if [[ -f $1/receiver.pid ]]; then read -r pid < "$1/receiver.pid"; [[ $pid =~ ^[1-9][0-9]*$ ]] && kill -- "-$pid" 2>/dev/null; fi' "$stage" >/dev/null 2>&1 || :
  fi
  if (( rc )); then
    echo "Transfer failed. Source retained; incomplete receiver data: $stage" >&2
    datam_log_event ERROR "receiver_diagnostics=$stage/receiver.log (present once receiver starts)"
  fi
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
mkfifo "$work/digest.pipe"
sha256sum < "$work/digest.pipe" > "$work/stream.sha256" & hash_pid=$!
script=$(cat -- "$0")
threads=${TRANSFER_THREADS:-2}
[[ $threads =~ ^[1-9][0-9]*$ ]] || fail 'Invalid TRANSFER_THREADS.'
stream() {
  local statuses rc=0 status
  if tar --acls --xattrs --sparse -C "$backupdir" -cpf - . | zstd -q -T"$threads" --fast=8 | tee "$work/digest.pipe"; then
    statuses=("${PIPESTATUS[@]}")
  else
    statuses=("${PIPESTATUS[@]}")
  fi
  datam_log_event INFO "sender_pipeline_status tar=${statuses[0]} zstd=${statuses[1]} tee=${statuses[2]}"
  for status in "${statuses[@]}"; do [[ $status == 0 ]] || rc=$status; done
  return "$rc"
}
echo "Transferring with $method; SSH control; destination $final"
datam_log_phase receiver-start "receiver_diagnostics=$stage/receiver.log"
if [[ $method != ssh ]]; then
  # The raw listener is restricted to the source address seen by SSH.
  remote "$script" --receive "$stage" "$method" "$listen_port" "$bind" "$peer" "$buffer_size" < /dev/null & ssh_pid=$!
  ready=n
  for (( attempt=0; attempt<20; attempt++ )); do
    kill -0 "$ssh_pid" 2>/dev/null || fail 'Receiver exited before listening.'
    if remote 'ss -ltnH "sport = :$1" | grep . >/dev/null' "$listen_port"; then ready=y; break; fi
    sleep .25
  done
  [[ $ready == y ]] || fail 'Receiver did not become ready.'
fi
send_data() {
  case $method in
    ssh) remote "$script" --receive "$stage" ssh "$listen_port" "$bind" "$peer" "$buffer_size" ;;
    nc) nc -4 --send-only --wait 10 --idle-timeout 60 "$remote_server" "$listen_port" ;;
    socat) socat -4 -u -T60 -b "$buffer_size" - "TCP:$remote_server:$listen_port,sndbuf=$buffer_size" ;;
  esac
}
datam_log_phase streaming "method=$method compression_threads=$threads"
if stream | send_data; then
  transport_statuses=("${PIPESTATUS[@]}")
else
  transport_statuses=("${PIPESTATUS[@]}")
fi
datam_log_event INFO "transport_pipeline_status sender=${transport_statuses[0]} transport=${transport_statuses[1]} method=$method"
transport_rc=0
for status in "${transport_statuses[@]}"; do [[ $status == 0 ]] || transport_rc=$status; done
(( transport_rc == 0 )) || exit "$transport_rc"
datam_log_phase receiver-completion
if [[ -n $ssh_pid ]]; then
  if wait "$ssh_pid"; then receiver_rc=0; else receiver_rc=$?; fi
  ssh_pid=''
  datam_log_event INFO "receiver_ssh_status=$receiver_rc receiver_diagnostics=$stage/receiver.log"
  (( receiver_rc == 0 )) || exit "$receiver_rc"
fi
datam_log_phase sender-digest
if wait "$hash_pid"; then hash_rc=0; else hash_rc=$?; fi
hash_pid=''
datam_log_event INFO "sender_sha256sum_status=$hash_rc"
(( hash_rc == 0 )) || exit "$hash_rc"
read -r digest _ < "$work/stream.sha256"
[[ $digest =~ ^[a-f0-9]{64}$ ]] || fail 'Sender checksum unavailable.'
datam_log_phase verify-publish "destination=$final"
remote 'set -euo pipefail; umask 077
[[ -f $1/RECEIVED ]]
read -r actual _ < "$1/stream.sha256"
[[ $actual == "$3" ]] || { echo "Stream checksum mismatch" >&2; exit 1; }
mkdir -p -- "$(dirname -- "$2")"
[[ ! -e $2 && ! -L $2 ]]
# The receipt belongs to the protocol, never to the transferred file namespace.
(set -o noclobber; printf "%s\n" "$3" > "$2.transfer.sha256")
# -T refuses accidental nesting. A unique generation never overwrites accepted data.
mv -T -- "$1/data" "$2"
if ! rm -- "$1/stream.sha256" "$1/RECEIVED" "$1/receiver.pid" "$1/receiver.log" || ! rmdir -- "$1"; then
  echo "WARNING: Transfer published successfully at $2; staging cleanup needed: $1" >&2
fi
' "$stage" "$final" "$digest"
datam_log_phase published "destination=$final receipt=$final.transfer.sha256"
printf 'TRANSFER_RESULT\t%s\n' "$final"
echo 'Transfer completed successfully; sender and receiver stream checksums match.'
