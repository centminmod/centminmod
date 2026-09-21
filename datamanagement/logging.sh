# shellcheck shell=bash
# Operational logs stay outside backup media; never trace arguments or credentials.
datam_log_event() {
  local level=$1 message=$2
  message=${message//$'\n'/\\n}; message=${message//$'\r'/\\r}
  # Diagnostics must never prevent recovery/cleanup when stderr is unavailable.
  TZ=UTC printf '%(%FT%TZ)T [%s] job=%s component=%s phase=%s %s\n' -1 "$level" "${MENU21_JOB_ID:-unassigned}" "${DATAM_COMPONENT:-menu}" "${DATAM_PHASE:-preflight}" "$message" >&2 || :
}
datam_log_phase() {
  if [[ -n ${DATAM_PHASE_STARTED:-} ]]; then datam_log_event INFO "phase-end elapsed_seconds=$((SECONDS-DATAM_PHASE_STARTED))"; fi
  DATAM_PHASE=$1 DATAM_PHASE_STARTED=$SECONDS
  datam_log_event INFO "phase-start ${2:-}"
}
datam_log_error() {
  local rc=$1 line=$2 pipeline=$3
  datam_log_event ERROR "exit=$rc line=$line pipeline_status=[$pipeline]"
  return "$rc"
}
datam_log_begin() {
  DATAM_COMPONENT=$1 DATAM_PHASE=preflight DATAM_PHASE_STARTED=$SECONDS
  trap 'datam_log_error "$?" "$LINENO" "${PIPESTATUS[*]}"' ERR
  datam_log_event INFO "action-start bash=$BASH_VERSION"
}
datam_log_check_location() {
  local path=$1 excluded=${DATAM_LOG_EXCLUDE_DIR:-}
  if [[ -n $excluded ]]; then
    excluded=$(realpath -m -- "$excluded") || return
    case "$path/" in "$excluded/"*) echo 'ERROR: Operational logs must be outside backup media.' >&2; return 1 ;; esac
  fi
}
datam_log_cancel_pending() {
  [[ -n ${MENU21_LOG_CANCEL_FD:-} ]] || return 1
  # Poll without consuming the byte, so a child forked after the signal can
  # still see cancellation. A missing inherited channel fails closed.
  if [[ ! $MENU21_LOG_CANCEL_FD =~ ^[0-9]+$ ]] || ! { : <&"$MENU21_LOG_CANCEL_FD"; } 2>/dev/null; then
    echo 'ERROR: Logging cancellation channel is unavailable.' >&2
    return 0
  fi
  read -r -t 0 -u "$MENU21_LOG_CANCEL_FD"
}
datam_logged_action() {
  # Do not put this function or the action under `if`: that disables function errexit.
  set +e
  umask 077
  cancelled=0
  log_setup=y
  cancellation_statuses=()
  # The standalone supervisor signals the whole group. Keep the logger alive
  # until the action's recovery handlers finish and tee has drained their output.
  trap 'datam_log_cancel 143 "${PIPESTATUS[@]}"' TERM
  trap 'datam_log_cancel 130 "${PIPESTATUS[@]}"' INT
  trap 'datam_log_cancel 129 "${PIPESTATUS[@]}"' HUP
  action=$1; shift
  logdir=$(realpath -m -- "${CENTMINLOGDIR:-/root/centminlogs}") || exit 1
  datam_log_check_location "$logdir" || exit 1
  mkdir -p -- "$logdir" || exit 1
  MENU21_LOG_FILE=$(mktemp "$logdir/menu21-$(date -u +%Y%m%dT%H%M%SZ)-XXXXXXXX.log") || exit 1
  MENU21_JOB_ID=${MENU21_LOG_FILE##*/}; MENU21_JOB_ID=${MENU21_JOB_ID%.log}
  MENU21_LOG_ACTIVE=1
  export MENU21_LOG_FILE MENU21_JOB_ID MENU21_LOG_ACTIVE
  started=$SECONDS
  printf 'Log: %s\n' "$MENU21_LOG_FILE" >&2
  printf 'job=%s action=%s started_utc=%s\n' "$MENU21_JOB_ID" "$action" "$(date -u +%FT%TZ)" >> "$MENU21_LOG_FILE" || exit 1
  log_setup=n
  {
    if (( cancelled )) || datam_log_cancel_pending; then
      echo 'Action cancelled before launch.' >&2
      exit 125
    fi
    "$@"
  } 2>&1 | (trap '' TERM INT HUP; exec tee --output-error=warn -a -- "$MENU21_LOG_FILE")
  statuses=("${PIPESTATUS[@]}")
  # Bash runs a pending signal trap after the foreground pipeline has finished.
  # Its commands replace PIPESTATUS, so the trap saves the original pair.
  if (( ${#cancellation_statuses[@]} == 2 )); then statuses=("${cancellation_statuses[@]}"); fi
  rc=${statuses[0]}
  if (( statuses[1] )); then
    printf 'ERROR: Log capture failed (%s); operation exit=%s. Check completed media before retrying.\n' "${statuses[1]}" "$rc" >&2
    (( rc )) || rc=74
  fi
  (( cancelled == 0 )) || rc=$cancelled
  summary="job=$MENU21_JOB_ID action=$action finished_utc=$(date -u +%FT%TZ) elapsed_seconds=$((SECONDS-started)) operation_exit=${statuses[0]} logger_exit=${statuses[1]} exit=$rc cancellation_exit=$cancelled"
  printf '%s\n' "$summary" >&2
  printf '%s\n' "$summary" >> "$MENU21_LOG_FILE" || { echo 'ERROR: Cannot write final log status.' >&2; (( rc )) || rc=74; }
  exit "$rc"
}
datam_log_cancel() {
  cancelled=$1; shift
  if [[ ${log_setup:-y} == y ]]; then
    # Never return to startup code and launch work after a setup-time signal.
    local message="Action cancelled during logging setup; exit=$cancelled"
    printf '%s\n' "$message" >&2
    [[ ! -f ${MENU21_LOG_FILE:-} ]] || printf '%s\n' "$message" >> "$MENU21_LOG_FILE"
    exit "$cancelled"
  fi
  if (( $# == 2 )); then cancellation_statuses=("$@"); fi
}
# Menu calls retain their shell state and functions; standalone calls below run
# this body directly so the supervisor waits for the actual log-draining process.
datam_run_logged() ( datam_logged_action "$@" )
datam_log_init() {
  local action=$1; shift
  if [[ ${MENU21_LOG_ACTIVE:-} != 1 ]]; then
    # Keep the public PID as a responsive supervisor. Perl/POSIX is already part
    # of Centmin Mod; an isolated group contains grandchildren without detaching
    # from the controlling terminal needed by SSH password/passphrase prompts.
    command -v perl >/dev/null || { echo 'ERROR: Perl is required for supervised logging.' >&2; exit 1; }
    exec perl -MPOSIX -MErrno=EINTR -e '
      use strict;
      use warnings;
      use Fcntl qw(F_DUPFD F_SETFD F_SETFL O_NONBLOCK);
      my ($child, $pending, $tty, $has_terminal);
      my %cancel_exit = (TERM => 143, INT => 130, HUP => 129);
      pipe(my $cancel_reader, my $cancel_writer) or die "Cannot create cancellation channel: $!\n";
      my $cancel_fd = fcntl($cancel_reader, F_DUPFD, 64);
      defined($cancel_fd) or die "Cannot reserve cancellation descriptor: $!\n";
      open(my $cancel_handle, "<&=$cancel_fd") or die "Cannot open cancellation descriptor: $!\n";
      close($cancel_reader);
      fcntl($cancel_handle, F_SETFD, 0) or die "Cannot inherit cancellation descriptor: $!\n";
      fcntl($cancel_writer, F_SETFL, O_NONBLOCK) or die "Cannot configure cancellation channel: $!\n";
      $ENV{MENU21_LOG_CANCEL_FD} = $cancel_fd;
      $has_terminal = open($tty, "+<", "/dev/tty");
      my $job_group = getpgrp(0);
      my $caller_group = getpgrp(getppid());
      my $foreground_worker = sub {
        return 1 unless $has_terminal && $child;
        return 1 unless POSIX::tcgetpgrp(fileno($tty)) == getpgrp(0);
        local $SIG{TTOU} = "IGNORE";
        return POSIX::tcsetpgrp(fileno($tty), $child) == 0;
      };
      my $restore_terminal = sub {
        return unless $has_terminal && $child;
        return unless POSIX::tcgetpgrp(fileno($tty)) == $child;
        local $SIG{TTOU} = "IGNORE";
        POSIX::tcsetpgrp(fileno($tty), getpgrp(0));
      };
      for my $signal (qw(TERM INT HUP)) {
        $SIG{$signal} = sub {
          $pending = $signal;
          # Publish before broadcasting: a not-yet-forked child cannot receive
          # the signal, but will inherit this persistent readable descriptor.
          while (!defined(syswrite($cancel_writer, "1"))) { next if $! == EINTR; last; }
          if ($child) { kill $signal, -$child; kill "CONT", -$child; }
        };
      }
      $SIG{TSTP} = sub { kill "TSTP", -$child if $child; };
      $SIG{CONT} = sub { $foreground_worker->(); kill "CONT", -$child if $child; };
      pipe(my $ready, my $release) or die "Cannot create logging supervisor pipe: $!\n";
      $child = fork();
      defined($child) or die "Cannot fork logging worker: $!\n";
      if (!$child) {
        close($release);
        close($cancel_writer);
        $SIG{$_} = "DEFAULT" for qw(TERM INT HUP TSTP CONT);
        exit $cancel_exit{$pending} if $pending;
        POSIX::setpgid(0, 0) == 0 or die "Cannot isolate logging worker: $!\n";
        my $start;
        sysread($ready, $start, 1) == 1 or exit 1;
        close($ready);
        exec @ARGV;
        die "Cannot start logging worker: $!\n";
      }
      close($ready);
      POSIX::setpgid($child, $child) == 0 or die "Cannot isolate logging process group: $!\n";
      $foreground_worker->() or die "Cannot foreground logging worker: $!\n";
      kill $pending, -$child if $pending;
      { local $SIG{PIPE} = "IGNORE"; syswrite($release, "1") unless $pending; }
      close($release);
      my $status;
      while (1) {
        if (waitpid($child, POSIX::WUNTRACED()) < 0) {
          next if $! == EINTR;
          die "Cannot wait for logging worker: $!\n";
        }
        # Perl normalizes $? and can discard the stopped-child bits.
        $status = ${^CHILD_ERROR_NATIVE};
        last unless POSIX::WIFSTOPPED($status);
        if (!$pending) {
          $restore_terminal->();
          # Stop output-pipeline siblings too, so the interactive shell sees a
          # fully stopped job. Never group-stop a shared caller shell (set +m
          # or nested noninteractive shells); those retain self-stop only.
          my $stop_target = $has_terminal && $caller_group > 0 && $caller_group != $job_group
            ? -$job_group : $$;
          kill "STOP", $stop_target;
          $foreground_worker->();
        }
        kill "CONT", -$child;
      }
      $restore_terminal->();
      exit($pending ? $cancel_exit{$pending} : (($status & 127) ? 128 + ($status & 127) : $status >> 8));
    ' bash "${BASH_SOURCE[0]}" "$action" bash "$0" "$@"
  fi
  [[ -f ${MENU21_LOG_FILE:-} && ! -L $MENU21_LOG_FILE ]] || { echo 'ERROR: Invalid parent log.' >&2; exit 1; }
  datam_log_check_location "$(realpath -m -- "$MENU21_LOG_FILE")" || exit 1
  if datam_log_cancel_pending; then
    datam_log_event WARN "Action cancelled before initialization: component=$action"
    exit 125
  fi
  datam_log_begin "$action"
  local digest
  digest=$(sha256sum -- "$0") || return
  datam_log_event INFO "script_sha256=${digest%% *}"
}

if [[ ${BASH_SOURCE[0]} == "$0" ]]; then datam_logged_action "$@"; fi
