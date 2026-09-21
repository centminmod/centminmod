#!/bin/bash
# Synthetic status/secret fixtures; no real credentials, services or remote writes.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d /var/tmp/menu21-logging.XXXXXX)
cancel_pid=''; cancel_group=''
cleanup() {
  local rc=$?
  [[ -z $cancel_group ]] || kill -KILL -- "-$cancel_group" 2>/dev/null || :
  [[ -z $cancel_pid ]] || { kill -KILL -- "-$cancel_pid" 2>/dev/null || :; wait "$cancel_pid" 2>/dev/null || :; }
  rm -rf -- "$work"
  exit "$rc"
}
trap cleanup EXIT
export CENTMINLOGDIR="$work/logs"
cp "$ROOT/logging.sh" "$work/"
cat > "$work/action.sh" <<'SCRIPT'
#!/bin/bash
set -Eeuo pipefail
source "$(dirname "$0")/logging.sh"
datam_log_init fixture "$@"
datam_log_phase producer
printf 'normal output\n'
printf 'native stderr\n' >&2
[[ -z ${TEST_MUTATION:-} ]] || touch "$TEST_MUTATION"
if [[ ${1:-} == fail ]]; then (exit 17) | cat; fi
printf 'FIXTURE_RESULT\tok\n'
SCRIPT
bash "$work/action.sh" unused-sensitive-argument > "$work/success" 2>&1
log=$(sed -n 's/^Log: //p' "$work/success")
[[ -f $log && $(stat -c %a "$log") == 600 ]]
grep -q '^FIXTURE_RESULT' "$work/success"
grep -q 'phase=producer' "$log"
grep -q 'native stderr' "$log"
grep -q 'operation_exit=0 logger_exit=0 exit=0' "$log"
! grep -q 'unused-sensitive-argument' "$log" || exit 1
if bash "$work/action.sh" fail > "$work/failure" 2>&1; then exit 1; else rc=$?; fi
[[ $rc == 17 ]]
failed=$(sed -n 's/^Log: //p' "$work/failure")
[[ $log != "$failed" ]]
grep -q 'pipeline_status=\[17 0\]' "$failed"
grep -q 'operation_exit=17 logger_exit=0 exit=17' "$failed"
echo 'PASS private unique logs, stdout/stderr capture, phases, pipeline status, secret-safe arguments'
printf occupied > "$work/not-directory"
if CENTMINLOGDIR="$work/not-directory" TEST_MUTATION="$work/mutation" bash "$work/action.sh" > "$work/init-failure" 2>&1; then exit 1; fi
[[ ! -e $work/mutation ]]
echo 'PASS failed log initialization prevents operation'
mkdir "$work/bin"
cat > "$work/bin/tee" <<'STUB'
#!/bin/bash
cat
exit 9
STUB
chmod +x "$work/bin/tee"
if PATH="$work/bin:$PATH" bash "$work/action.sh" > "$work/logger-failure" 2>&1; then exit 1; else rc=$?; fi
[[ $rc == 74 ]]
grep -q 'operation_exit=0 logger_exit=9 exit=74' "$work/logger-failure"
if PATH="$work/bin:$PATH" bash "$work/action.sh" fail > "$work/both-failure" 2>&1; then exit 1; else rc=$?; fi
[[ $rc == 17 ]]
grep -q 'operation_exit=17 logger_exit=9 exit=17' "$work/both-failure"
echo 'PASS logger failure is explicit and preserves operation failure status'
if DATAM_LOG_EXCLUDE_DIR="$work/media" CENTMINLOGDIR="$work/media/logs" bash "$work/action.sh" > "$work/excluded" 2>&1; then exit 1; fi
[[ ! -e $work/media ]]
echo 'PASS log path inside backup media refused before writes'
# Plain deliberate exits need a final record even without an ERR trap event.
cat > "$work/explicit.sh" <<'SCRIPT'
#!/bin/bash
set -Eeuo pipefail
source "$(dirname "$0")/logging.sh"
datam_log_init explicit "$@"
exit 23
SCRIPT
if bash "$work/explicit.sh" > "$work/explicit" 2>&1; then exit 1; else rc=$?; fi
[[ $rc == 23 ]]
grep -q 'operation_exit=23 logger_exit=0 exit=23' "$work/explicit"
echo 'PASS explicit failure has a persistent terminal status'
# Delay setup and the final child launch independently. The launch adapter
# deliberately forks the real entrypoint after the signal broadcast, proving
# that a prelaunch flag check alone cannot satisfy this regression.
mkdir "$work/startup-bin"
export START_REAL_DATE START_REAL_BASH
START_REAL_DATE=$(command -v date)
START_REAL_BASH=$(command -v bash)
cat > "$work/startup-bin/date" <<'STUB'
#!/bin/bash
if [[ ! -e $START_READY && ( ( $START_STAGE == pre-header && $* == '-u +%Y%m%dT%H%M%SZ' ) || ( $START_STAGE == header && $* == '-u +%FT%TZ' ) ) ]]; then
  perl -e 'print getpgrp(0), "\n"' > "$START_GROUP"
  touch "$START_READY"
  while [[ ! -e $START_RELEASE ]]; do sleep .05; done
fi
exec "$START_REAL_DATE" "$@"
STUB
cat > "$work/startup-bin/bash" <<'STUB'
#!/bin/bash
if [[ $START_STAGE == launch && ${MENU21_LOG_ACTIVE:-} == 1 && $1 == "$START_ACTION" && ! -e $START_READY ]]; then
  trap 'touch "$START_SIGNAL_SEEN"' TERM INT
  perl -e 'print getpgrp(0), "\n"' > "$START_GROUP"
  touch "$START_READY"
  while [[ ! -e $START_RELEASE ]]; do sleep .05; done
  # This fresh child never received the group signal. It must see the pipe byte.
  "$START_REAL_BASH" "$@"
  exit "$?"
fi
exec "$START_REAL_BASH" "$@"
STUB
chmod +x "$work/startup-bin/date" "$work/startup-bin/bash"
for stage in pre-header header launch menu-header; do
  for signal in control TERM INT; do
    rm -f -- "$work/start-ready" "$work/start-mutation" "$work/start-release" "$work/start-signal-seen"
    command=("$START_REAL_BASH" "$work/action.sh")
    adapter_stage=$stage
    if [[ $stage == menu-header ]]; then
      adapter_stage=header
      # Exercise the menu logger body without the standalone Perl supervisor.
      # Reset INT as a foreground interactive shell would before exec.
      command=(perl -e '$SIG{INT}="DEFAULT"; exec @ARGV' "$START_REAL_BASH" "$work/logging.sh" menu-fixture "$START_REAL_BASH" "$work/action.sh")
    fi
    START_STAGE="$adapter_stage" START_READY="$work/start-ready" START_GROUP="$work/start-group" START_ACTION="$work/action.sh" START_RELEASE="$work/start-release" START_SIGNAL_SEEN="$work/start-signal-seen" TEST_MUTATION="$work/start-mutation" \
      PATH="$work/startup-bin:$PATH" setsid "${command[@]}" > "$work/start-$stage-$signal" 2>&1 &
    cancel_pid=$!; cancel_group=$cancel_pid
    for (( i=0; i<200; i++ )); do [[ ! -e $work/start-ready ]] || break; sleep .05; done
    [[ -e $work/start-ready ]]
    read -r cancel_group < "$work/start-group"
    [[ $cancel_group =~ ^[1-9][0-9]*$ && $cancel_group != "$(perl -e 'print getpgrp(0)')" ]]
    [[ $signal == control ]] || kill -"$signal" "$cancel_pid"
    if [[ $stage == launch && $signal != control ]]; then
      for (( i=0; i<200; i++ )); do [[ ! -e $work/start-signal-seen ]] || break; sleep .05; done
      [[ -e $work/start-signal-seen ]]
    fi
    if [[ $signal == control || $stage == menu-header || $stage == launch ]]; then touch "$work/start-release"; fi
    for (( i=0; i<200; i++ )); do kill -0 "$cancel_pid" 2>/dev/null || break; sleep .05; done
    ! kill -0 "$cancel_pid" 2>/dev/null || exit 1
    rc=0; wait "$cancel_pid" || rc=$?
    if [[ $signal == control ]]; then
      [[ $rc == 0 && -f $work/start-mutation ]]
      grep -q 'operation_exit=0 logger_exit=0 exit=0' "$work/start-$stage-$signal"
    else
      expected=143; [[ $signal != INT ]] || expected=130
      [[ $rc == "$expected" && ! -e $work/start-mutation ]]
      grep -qi 'cancelled' "$work/start-$stage-$signal"
    fi
    cancel_pid=''; cancel_group=''
  done
done
echo 'PASS setup/menu cancellation, late-forked child gate and matching no-cancel controls'
# Signal the actual public CLI PID, not the shell group or an internal child.
# setsid keeps a broken older implementation contained for fixture cleanup.
cat > "$work/cancel.sh" <<'SCRIPT'
#!/bin/bash
set -Eeuo pipefail
source "$(dirname "$0")/logging.sh"
datam_log_init cancellation "$@"
trap 'rc=$?; sleep .1; echo RECOVERY_DRAINED; exit "$rc"' EXIT
trap 'exit 143' TERM
trap 'exit 130' INT
perl -e 'print getpgrp(0)' > "$CANCEL_GROUP"
bash -c 'echo "$BASHPID" > "$CANCEL_CHILD"; touch "$CANCEL_READY"; sleep 30; touch "$CANCEL_LATE"'
echo UNEXPECTED_SUCCESS
SCRIPT
for signal in TERM INT; do
  rm -f -- "$work/cancel-ready" "$work/cancel-late"
  CANCEL_GROUP="$work/cancel-group" CANCEL_CHILD="$work/cancel-child" CANCEL_READY="$work/cancel-ready" CANCEL_LATE="$work/cancel-late" \
    setsid bash "$work/cancel.sh" > "$work/cancel-$signal" 2>&1 &
  cancel_pid=$!
  for (( i=0; i<200; i++ )); do [[ ! -e $work/cancel-ready ]] || break; sleep .05; done
  [[ -e $work/cancel-ready ]]
  read -r cancel_group < "$work/cancel-group" || [[ -n $cancel_group ]]
  [[ $cancel_group =~ ^[1-9][0-9]*$ && $cancel_group != "$(perl -e 'print getpgrp(0)')" ]]
  kill -"$signal" "$cancel_pid"
  for (( i=0; i<200; i++ )); do kill -0 "$cancel_pid" 2>/dev/null || break; sleep .05; done
  ! kill -0 "$cancel_pid" 2>/dev/null || exit 1
  rc=0; wait "$cancel_pid" || rc=$?
  expected=143; [[ $signal != INT ]] || expected=130
  [[ $rc == "$expected" && ! -e $work/cancel-late ]]
  ! kill -0 "$(cat "$work/cancel-child")" 2>/dev/null || exit 1
  cancelled_log=$(sed -n 's/^Log: //p' "$work/cancel-$signal")
  grep -q RECOVERY_DRAINED "$cancelled_log"
  grep -q "operation_exit=$expected logger_exit=0 exit=$expected" "$cancelled_log"
  ! grep -q UNEXPECTED_SUCCESS "$cancelled_log" || exit 1
  cancel_pid=''; cancel_group=''
done
echo 'PASS PID-directed TERM/INT cancels descendants, waits for recovery and drains the log'
# Exercise a real controlling terminal, including shell job control. No real
# passwords are used; the secret fixture verifies that terminal input stays out
# of the persistent output log.
cat > "$work/terminal.sh" <<'SCRIPT'
#!/bin/bash
set -Eeuo pipefail
if [[ ${MENU21_LOG_ACTIVE:-} != 1 ]]; then printf '%s\n' "$BASHPID" >> "$PTY_PUBLIC_PIDS"; fi
source "$(dirname "$0")/logging.sh"
datam_log_init terminal "$@"
perl -e 'print getpgrp(0), "\n"' >> "$PTY_GROUPS"
if [[ ${1:-} == secret ]]; then
  read -rsp 'SECRET> ' value </dev/tty
  echo SECRET_ACCEPTED
else
  read -rp 'INPUT> ' value </dev/tty
  printf 'GOT=%s\n' "$value"
fi
SCRIPT
python3 - "$work" <<'PY'
import errno, os, pathlib, pty, select, shlex, signal, subprocess, sys, time
work = pathlib.Path(sys.argv[1])
prompt = b'MENU21_PROMPT> '
pid, fd = pty.fork()
if pid == 0:
    os.environ.update(PS1=prompt.decode(), PROMPT_COMMAND='', PTY_GROUPS=str(work/'pty-groups'),
                      PTY_PUBLIC_PIDS=str(work/'pty-public-pids'))
    os.execvp('bash', ['bash', '--noprofile', '--norc', '-i'])
buffer = b''
def expect(text, timeout=8):
    global buffer
    deadline = time.monotonic() + timeout
    while text not in buffer:
        if time.monotonic() > deadline:
            raise AssertionError((text, buffer[-1500:]))
        if select.select([fd], [], [], .1)[0]:
            buffer += os.read(fd, 65536)
    before, buffer = buffer.split(text, 1)
    return before
def send(text):
    os.write(fd, text if isinstance(text, bytes) else text.encode())
command = 'bash ' + shlex.quote(str(work/'terminal.sh'))
try:
    expect(prompt)
    send('set -b\n'); expect(prompt)
    send(command + '\n'); expect(b'INPUT> ')
    send('typed-value\n'); expect(b'GOT=typed-value'); expect(prompt)
    send(command + ' secret\n'); expect(b'SECRET> ')
    send('synthetic-unlogged-secret\n'); expect(b'SECRET_ACCEPTED'); expect(prompt)
    send(command + '\n'); expect(b'INPUT> ')
    send(b'\x1a'); expect(prompt)
    send('fg\n')
    send('resumed-value\n'); expect(b'GOT=resumed-value'); expect(prompt)
    send(command + ' &\n'); expect(prompt); expect(b'Stopped')
    send('fg\n')
    send('background-value\n'); expect(b'GOT=background-value'); expect(prompt)
    for consumer in ('cat', 'tee ' + shlex.quote(str(work/'pty-tee-output'))):
        send(command + ' | ' + consumer + '\n'); expect(b'INPUT> ')
        send(b'\x1a'); expect(prompt, timeout=3)
        send('fg\n')
        send('pipeline-value\n'); expect(b'GOT=pipeline-value'); expect(prompt)
        send("printf 'PIPELINE_SHELL_ALIVE\\n'\n"); expect(b'PIPELINE_SHELL_ALIVE\r\n'); expect(prompt)
    # Without shell job control, the caller shares the supervisor group. It
    # must stay alive; only the supervisor stops and needs explicit CONT.
    send('set +m\n'); expect(prompt)
    send(command + '\n'); expect(b'INPUT> ')
    public_pid = int((work/'pty-public-pids').read_text().splitlines()[-1])
    send(b'\x1a')
    deadline = time.monotonic() + 3
    while True:
        state = subprocess.check_output(['ps', '-p', str(public_pid), '-o', 'stat=']).decode().strip()
        if state.startswith('T'): break
        assert time.monotonic() < deadline, state
        time.sleep(.05)
    shell_state = subprocess.check_output(['ps', '-p', str(pid), '-o', 'stat=']).decode().strip()
    assert shell_state and not shell_state.startswith('T'), shell_state
    os.kill(public_pid, signal.SIGCONT)
    send('shared-group-value\n'); expect(b'GOT=shared-group-value'); expect(prompt)
    send('set -m\n'); expect(prompt)
    send("printf 'SHELL_INPUT_OK\\n'\n"); expect(b'SHELL_INPUT_OK\r\n'); expect(prompt)
    for logfile in (work/'logs').glob('*.log'):
        assert 'synthetic-unlogged-secret' not in logfile.read_text()
finally:
    # Interactive bash propagates HUP to jobs; each supervisor drains its worker.
    try: os.kill(pid, signal.SIGHUP)
    except ProcessLookupError: pass
    deadline = time.monotonic() + 2
    while time.monotonic() < deadline:
        try:
            if os.waitpid(pid, os.WNOHANG)[0]: break
        except ChildProcessError: break
        time.sleep(.05)
    else:
        os.kill(pid, signal.SIGKILL); os.waitpid(pid, 0)
    os.close(fd)
    groups = work/'pty-groups'
    if groups.exists():
        for group in set(groups.read_text().splitlines()):
            try: os.killpg(int(group), signal.SIGKILL)
            except ProcessLookupError: pass
print('PASS terminal input, hidden secret, Ctrl-Z/fg, cat/tee pipelines, background/fg and shared-caller protection')
PY
# Two real production entrypoints inherit one parent job and log. Deliberately
# invalid actions are nonmutating but still exercise each complete entrypoint.
cat > "$work/shared.sh" <<'SCRIPT'
#!/bin/bash
set -Eeuo pipefail
source "$(dirname "$0")/logging.sh"
datam_log_init shared-job "$@"
for script in keygen.sh mariabackup-restore.sh; do
  rc=0; bash "$1/$script" invalid-action || rc=$?
  [[ $rc == 1 ]]
done
echo SHARED_CHILDREN_COMPLETE
SCRIPT
CENTMINLOGDIR="$work/shared-logs" bash "$work/shared.sh" "$ROOT" > "$work/shared" 2>&1
[[ $(find "$work/shared-logs" -type f | wc -l) == 1 ]]
shared_log=$(sed -n 's/^Log: //p' "$work/shared")
grep -q 'component=ssh-key' "$shared_log"
grep -q 'component=physical-restore' "$shared_log"
[[ $(sed -n 's/.*job=\([^ ]*\).*/\1/p' "$shared_log" | sort -u | wc -l) == 1 ]]
grep -q SHARED_CHILDREN_COMPLETE "$shared_log"
echo 'PASS multiple real child commands share the parent job ID and single log'
# A diagnostic write failure cannot stop the original error's recovery handler.
cat > "$work/recovery.sh" <<'SCRIPT'
#!/bin/bash
set -Ee
source "$(dirname "$0")/logging.sh"
trap 'rc=$?; datam_log_event INFO recovery; echo RECOVERY_EXECUTED; exit "$rc"' EXIT
false
SCRIPT
if bash "$work/recovery.sh" > "$work/recovery" 2>&-; then exit 1; fi
grep -q RECOVERY_EXECUTED "$work/recovery"
echo 'PASS closed stderr cannot suppress recovery'
# A custom log directory must not be read back into a live archive/transfer.
mkdir -p "$work/source"
printf payload > "$work/source/file"
cat > "$work/config" <<CFG
FILES_BACKUP_ROOT='$work/backups'
SOURCE_ID=fixture
DIRECTORIES_TO_BACKUP=('$work/source')
DOMAINS_ROOT='$work/absent'
CFG
if CENTMINLOGDIR="$work/source/logs" BACKUP_CONFIG="$work/config" bash "$ROOT/backups.sh" backup-files > "$work/self-backup" 2>&1; then exit 1; fi
grep -q 'Set CENTMINLOGDIR outside source' "$work/self-backup"
if CENTMINLOGDIR="$work/source/logs" bash "$ROOT/tunnel-transfers.sh" -s "$work/source" -h 127.0.0.1 -r "$work/target" -m ssh > "$work/self-transfer" 2>&1; then exit 1; fi
grep -q 'Set CENTMINLOGDIR outside the transfer source' "$work/self-transfer"
cat > "$work/s3.sh" <<'SCRIPT'
#!/bin/bash
source "$1/../inc/datamanager.inc"
printf '\n\n%s\ns3://fixture/prefix\n' "$2" | datam_run_logged s3 datam_s3_action 9
SCRIPT
if CENTMINLOGDIR="$work/source/logs" bash "$work/s3.sh" "$ROOT" "$work/source" > "$work/self-s3" 2>&1; then exit 1; fi
grep -q 'Operational logs must be outside backup media' "$work/self-s3"
echo 'PASS live logs excluded from backup, transfer and S3 source trees'
echo 'ALL PASS'
