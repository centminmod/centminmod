#!/usr/bin/env bash
#
# cmsec.sh — Centmin Mod security check dispatcher
#
# Thin orchestrator over per-CVE detection scripts in checks/ and per-CVE
# active probes in probes/. Stays out of verdict logic — each check declares
# its own NOT_AFFECTED / VULNERABLE / INDETERMINATE based on its kernel-range
# rules. cmsec normalizes exit codes, manages caching, applies an
# informational distro-EOL footnote, and emits compact dmotd output.
#
# Usage:
#   cmsec                                 run all checks, summary
#   cmsec list                            list available checks
#   cmsec check cve-2026-31431            run a specific check
#   cmsec check cve-2026-31431 --json     JSON output (machine readable)
#   cmsec probe cve-2026-31431 --yes      run active probe (PoC algorithm)
#   cmsec --dmotd                         compact one-line dmotd output
#   cmsec --no-cache                      bypass cache, always re-run
#
# Exit codes (aggregate):
#   0 = all checks PATCHED or NOT_AFFECTED
#   1 = at least one VULNERABLE
#   2 = usage error
#   3 = at least one INDETERMINATE (and no VULNERABLE)
#
set -uo pipefail

CMSEC_VERSION="0.1.0"
CMSEC_DIR="$(cd "$(dirname "$(readlink -f "$0" 2>/dev/null || echo "$0")")" && pwd)"
CHECKS_DIR="$CMSEC_DIR/checks"
PROBES_DIR="$CMSEC_DIR/probes"
LIB_DIR="$CMSEC_DIR/lib"

# Source helpers (osdetect populates CMSEC_OS_*, CMSEC_KERNEL*, etc.)
# shellcheck source=lib/osdetect.sh
. "$LIB_DIR/osdetect.sh"
# shellcheck source=lib/cache.sh
. "$LIB_DIR/cache.sh"

# ---------- color helpers ----------
if [ -t 1 ]; then
  C_RED='\033[1;31m'; C_GREEN='\033[1;32m'; C_YELLOW='\033[1;33m'; C_BLUE='\033[1;34m'; C_RESET='\033[0m'
else
  C_RED=''; C_GREEN=''; C_YELLOW=''; C_BLUE=''; C_RESET=''
fi
say()    { printf '%b\n' "$*"; }
header() { printf '\n%b%s%b\n' "$C_BLUE" "$*" "$C_RESET"; }

# Minimal JSON string escaper for synthesized JSON docs. Used only for the
# few cmsec-internal printf paths (suppress / container-skip / JSON-empty
# fallback) where a defensive escape protects against future check IDs that
# could contain quotes, backslashes, or control characters. Returns the
# value WITHOUT enclosing quotes so callers can embed it inside printf '%s'.
cmsec_json_escape_inner() {
  printf '%s' "$1" | python3 -c 'import sys,json; sys.stdout.write(json.dumps(sys.stdin.read())[1:-1])' 2>/dev/null && return 0
  # Bash fallback: backslash and quote, then strip ASCII controls (lossy but
  # produces valid JSON). Mirrors check-cve-2026-31431.sh json_escape().
  printf '%s' "$1" | sed \
    -e 's/\\/\\\\/g' \
    -e 's/"/\\"/g' \
    -e 's/\t/\\t/g' \
    -e 's/\r/\\r/g' \
    -e ':a' -e 'N' -e '$!ba' -e 's/\n/\\n/g' \
    | tr -d '\000-\010\013\014\016-\037'
}

# ---------- runtime state ----------
SUBCMD="run"
CVE_ID=""
JSON_OUT=0
NO_CACHE=0
DMOTD_MODE=0
PROBE_YES=0
QUIET=0
SUPPRESS_LIST="${DMOTD_CVECHECK_SUPPRESS:-}"

usage() {
  cat <<EOF
cmsec $CMSEC_VERSION — Centmin Mod security check dispatcher

USAGE:
  cmsec [run]                       Run all available CVE checks (default)
  cmsec list                        List available checks
  cmsec check <cve-id>              Run a specific check (e.g. cve-2026-31431)
  cmsec probe <cve-id> --yes        Run an active probe (requires --yes)

OPTIONS:
  --json                            Machine-readable output: one JSON document
                                    per check, emitted back-to-back (NOT a JSON
                                    array). Use \`cmsec check <cve-id> --json\`
                                    for a single parseable document.
  --no-cache                        Bypass cache, always re-run
  --dmotd                           Compact one-line login output
  --quiet                           Suppress narrative
  --version                         Print version
  -h, --help                        This help

EXIT CODES:
  0 = all checks patched / not_affected
  1 = at least one vulnerable
  2 = usage error
  3 = at least one indeterminate (no vulnerable)

CONFIG (custom_config.inc):
  DMOTD_CVECHECK='y'                enable cmsec line in dmotd login banner
  DMOTD_CVECHECK_SUPPRESS='CVE-X,CVE-Y'   per-CVE suppression (comma-sep)
EOF
}

# ---------- argument parsing ----------
# First positional may be subcommand or option.
if [ "$#" -gt 0 ]; then
  case "$1" in
    run|list|check|probe)
      SUBCMD="$1"; shift ;;
    --help|-h)
      usage; exit 0 ;;
    --version)
      printf 'cmsec %s\n' "$CMSEC_VERSION"; exit 0 ;;
    --*)
      : ;;
    *)
      printf 'cmsec: unknown subcommand: %s\n' "$1" >&2
      usage >&2; exit 2 ;;
  esac
fi

# Optional CVE id for check/probe subcommands.
if [ "$SUBCMD" = "check" ] || [ "$SUBCMD" = "probe" ]; then
  if [ "$#" -gt 0 ] && [ "${1:0:2}" != "--" ]; then
    CVE_ID="$1"; shift
  fi
fi

while [ "$#" -gt 0 ]; do
  case "$1" in
    --json)      JSON_OUT=1 ;;
    --no-cache)  NO_CACHE=1 ;;
    --dmotd)     DMOTD_MODE=1; QUIET=1 ;;
    --quiet)     QUIET=1 ;;
    --yes)       PROBE_YES=1 ;;
    --help|-h)   usage; exit 0 ;;
    --version)   printf 'cmsec %s\n' "$CMSEC_VERSION"; exit 0 ;;
    *)           printf 'cmsec: unknown option: %s\n' "$1" >&2; exit 2 ;;
  esac
  shift
done

# ---------- check discovery ----------
list_checks() {
  local f
  [ -d "$CHECKS_DIR" ] || return 0
  for f in "$CHECKS_DIR"/check-*.sh; do
    [ -e "$f" ] || continue
    local base id
    base="$(basename "$f" .sh)"
    id="${base#check-}"
    printf '%s\n' "$id"
  done
}

resolve_check_path() {
  local id="$1"
  # accept "cve-2026-31431", "CVE-2026-31431"
  id="$(printf '%s' "$id" | tr '[:upper:]' '[:lower:]')"
  local p="$CHECKS_DIR/check-$id.sh"
  [ -x "$p" ] || return 1
  printf '%s' "$p"
}

resolve_probe_path() {
  local id="$1"
  id="$(printf '%s' "$id" | tr '[:upper:]' '[:lower:]')"
  local p="$PROBES_DIR/probe-$id.sh"
  [ -x "$p" ] || return 1
  printf '%s' "$p"
}

# ---------- suppression ----------
is_suppressed() {
  local cve_id="$1"
  [ -z "$SUPPRESS_LIST" ] && return 1
  local IFS=','
  for s in $SUPPRESS_LIST; do
    s="$(printf '%s' "$s" | tr -d '[:space:]')"
    [ "$(printf '%s' "$s" | tr '[:upper:]' '[:lower:]')" = \
      "$(printf '%s' "$cve_id" | tr '[:upper:]' '[:lower:]')" ] && return 0
  done
  return 1
}

# ---------- per-check execution ----------
# Runs a single check script, captures stdout + exit code, writes cache.
# Echoes JSON line: {"id":"...","status":"...","exit":N,"json":{...}}
run_check_cached() {
  local check_id="$1"
  local check_path="$2"
  local cache_dir cache_file payload exit_rc
  cache_dir="$(cmsec_cache_dir)"
  cache_file="$cache_dir/$check_id.cache"

  local state_key
  state_key="$(cmsec_cache_state_key "$check_path" "")"

  if [ "$NO_CACHE" -ne 1 ] && [ "$DMOTD_MODE" -eq 1 ]; then
    payload="$(cmsec_cache_read "$cache_file" "$state_key" 2>/dev/null || true)"
    if [ -n "$payload" ]; then
      printf '%s' "$payload"
      return 0
    fi
    # cache miss in dmotd mode: try stale fallback first, refresh in background.
    # Pass state_key so stale fallback only fires when the system state still
    # matches (TTL expired only). On state changes (kernel reboot, OS conversion,
    # livepatch applied), stale will return nothing — forcing a synchronous fresh
    # run rather than serving the now-incorrect prior verdict.
    payload="$(cmsec_cache_read_stale "$cache_file" "$state_key" 2>/dev/null || true)"
    if [ -n "$payload" ]; then
      printf '%s' "$payload"
      ( run_check_fresh "$check_id" "$check_path" "$state_key" "$cache_file" >/dev/null 2>&1 & ) >/dev/null 2>&1
      return 0
    fi
    # no usable stale either — must run synchronously
  fi

  if [ "$NO_CACHE" -ne 1 ]; then
    payload="$(cmsec_cache_read "$cache_file" "$state_key" 2>/dev/null || true)"
    if [ -n "$payload" ]; then
      printf '%s' "$payload"
      return 0
    fi
  fi

  run_check_fresh "$check_id" "$check_path" "$state_key" "$cache_file"
}

run_check_fresh() {
  local check_id="$1" check_path="$2" state_key="$3" cache_file="$4"
  local stdout_capture exit_rc
  stdout_capture="$("$check_path" --json --quiet 2>/dev/null)"
  exit_rc=$?
  local payload
  payload="$(printf '%s\n%s\n' "EXIT_RC=$exit_rc" "$stdout_capture")"
  cmsec_cache_write "$cache_file" "$state_key" "$payload"
  printf '%s' "$payload"
  return $exit_rc
}

# Parse cached payload back into status / exit_rc / json fields.
# Sets globals: PARSE_EXIT, PARSE_STATUS, PARSE_KERNEL, PARSE_JSON
parse_payload() {
  local payload="$1"
  PARSE_EXIT="$(printf '%s\n' "$payload" | awk -F= '/^EXIT_RC=/{print $2; exit}')"
  PARSE_JSON="$(printf '%s\n' "$payload" | awk 'NR>1')"
  PARSE_STATUS="$(printf '%s' "$PARSE_JSON" | awk -F'"' '/"final_status"/ {print $4; exit}')"
  PARSE_KERNEL="$(printf '%s' "$PARSE_JSON" | awk -F'"' '/"running_kernel_full"/ {print $4; exit}')"
  [ -z "$PARSE_STATUS" ] && case "${PARSE_EXIT:-3}" in
    0) PARSE_STATUS="patched" ;;
    1) PARSE_STATUS="vulnerable" ;;
    3) PARSE_STATUS="indeterminate" ;;
    *) PARSE_STATUS="indeterminate" ;;
  esac
}

# ---------- output formatters ----------
status_color() {
  case "$1" in
    patched|not_affected) printf '%s' "$C_GREEN" ;;
    vulnerable)           printf '%s' "$C_RED" ;;
    indeterminate)        printf '%s' "$C_YELLOW" ;;
    *)                    printf '%s' "$C_YELLOW" ;;
  esac
}

# Emit a single dmotd line for one check.
emit_dmotd_line() {
  local cve_id="$1" status="$2" kernel_track="$3" extra="${4:-}"
  local cve_label color
  cve_label="$(printf '%s' "$cve_id" | tr '[:lower:]' '[:upper:]')"
  color="$(status_color "$status")"
  case "$status" in
    patched)
      printf '%b * %s (%s kernel): PATCHED%b\n' "$color" "$cve_label" "$kernel_track" "$C_RESET"
      ;;
    not_affected)
      printf '%b * %s (%s kernel): NOT AFFECTED%b\n' "$color" "$cve_label" "$kernel_track" "$C_RESET"
      ;;
    vulnerable)
      printf '%b * %s (%s kernel): VULNERABLE — run "cmsec check %s" for details%b\n' \
        "$color" "$cve_label" "$kernel_track" "$cve_id" "$C_RESET"
      ;;
    *)
      printf '%b * %s (%s kernel): status indeterminate — run "cmsec check %s" for details%b\n' \
        "$color" "$cve_label" "$kernel_track" "$cve_id" "$C_RESET"
      ;;
  esac
  [ -n "$extra" ] && printf '   %s\n' "$extra"
}

# Compose distro-EOL footnote (informational only, never modifies verdict).
eol_footnote() {
  [ "$CMSEC_OS_EOL" = "true" ] || return 0
  local note
  case "$CMSEC_OS_ID:$CMSEC_OS_MAJOR" in
    centos:6)
      note="CentOS 6 reached EOL $CMSEC_OS_EOL_DATE — no vendor patches."
      ;;
    centos:7)
      note="CentOS 7 stock kernel reached EOL $CMSEC_OS_EOL_DATE. CloudLinux 7h / ELS extended support may still ship backports — verify with vendor."
      ;;
  esac
  [ -n "$note" ] && printf '%s' "$note"
}

# ---------- subcommand: list ----------
do_list() {
  header "Available cmsec checks:"
  local id
  for id in $(list_checks); do
    say " * $id"
  done
  header "Available cmsec probes:"
  local f base pid
  for f in "$PROBES_DIR"/probe-*.sh; do
    [ -e "$f" ] || continue
    base="$(basename "$f" .sh)"; pid="${base#probe-}"
    say " * $pid (requires --yes)"
  done
}

# ---------- subcommand: probe ----------
do_probe() {
  local pid="$CVE_ID"
  if [ -z "$pid" ]; then
    printf 'cmsec probe: missing <cve-id>\n' >&2
    usage >&2; exit 2
  fi
  local probe_path
  probe_path="$(resolve_probe_path "$pid")" || {
    printf 'cmsec: no probe registered for "%s"\n' "$pid" >&2
    exit 2
  }
  if [ "$PROBE_YES" -ne 1 ]; then
    "$probe_path"   # probe enforces its own --yes guard and prints help
    exit 2
  fi
  exec "$probe_path" --yes
}

# ---------- subcommand: check / run ----------
# Print a per-check section (full cmsec, not dmotd). Returns exit_rc of the check.
do_one_check_full() {
  local cve_id="$1"
  local check_path
  check_path="$(resolve_check_path "$cve_id")" || {
    printf 'cmsec: no check registered for "%s"\n' "$cve_id" >&2
    return 2
  }
  local cve_id_esc
  cve_id_esc="$(cmsec_json_escape_inner "$cve_id")"
  if is_suppressed "$cve_id"; then
    if [ "$JSON_OUT" -eq 1 ]; then
      printf '{"cve":"%s","final_status":"skipped","reason":"suppressed via DMOTD_CVECHECK_SUPPRESS","exit_rc":0}\n' "$cve_id_esc"
    elif [ "$QUIET" -ne 1 ]; then
      say "$cve_id: suppressed via DMOTD_CVECHECK_SUPPRESS"
    fi
    return 0
  fi
  if [ "$CMSEC_IN_CONTAINER" = "true" ]; then
    if [ "$JSON_OUT" -eq 1 ]; then
      printf '{"cve":"%s","final_status":"skipped","reason":"running inside container/chroot — host-kernel verdict not derivable","exit_rc":3}\n' "$cve_id_esc"
    elif [ "$QUIET" -ne 1 ]; then
      say "$cve_id: skipped — running inside container/chroot (host-kernel verdict not derivable)"
    fi
    return 3
  fi

  if [ "$JSON_OUT" -eq 1 ]; then
    # Route JSON through the cache so dmotd's --json call doesn't re-run the
    # check after --dmotd just populated the cache. Cache hit → instant; miss →
    # one fresh run that populates cache for any subsequent consumer.
    local payload
    payload="$(run_check_cached "$cve_id" "$check_path")"
    parse_payload "$payload"
    if [ -n "$PARSE_JSON" ]; then
      printf '%s\n' "$PARSE_JSON"
    else
      # Check script failed to emit JSON — synthesize a minimal document so
      # consumers don't get empty stdout for this CVE.
      printf '{"cve":"%s","final_status":"indeterminate","reason":"check script produced no JSON output","exit_rc":%s}\n' \
        "$cve_id_esc" "${PARSE_EXIT:-3}"
    fi
    return "${PARSE_EXIT:-3}"
  fi

  "$check_path"
  return $?
}

# Single-check dmotd output.
do_one_check_dmotd() {
  local cve_id="$1"
  local check_path payload
  check_path="$(resolve_check_path "$cve_id")" || return 2
  if is_suppressed "$cve_id"; then return 0; fi
  if [ "$CMSEC_IN_CONTAINER" = "true" ]; then return 0; fi

  payload="$(run_check_cached "$cve_id" "$check_path")"
  parse_payload "$payload"

  local extra=""
  case "$CMSEC_KERNEL_TRACK" in
    linode)
      [ "$PARSE_STATUS" = "vulnerable" ] && extra="Linode boot kernel — update via Linode Cloud Manager → Configuration Profile → Kernel, then reboot. dnf upgrade alone will not fix the running kernel."
      ;;
  esac
  if [ -z "$extra" ]; then
    local foot
    foot="$(eol_footnote)"
    [ -n "$foot" ] && extra="$foot"
  fi

  emit_dmotd_line "$cve_id" "$PARSE_STATUS" "$CMSEC_KERNEL_TRACK" "$extra"
  return "${PARSE_EXIT:-3}"
}

do_run() {
  local exit_aggregate=0  saw_indeterminate=0  saw_any=0  cve_id rc
  local checks
  checks="$(list_checks)"
  [ -z "$checks" ] && {
    [ "$DMOTD_MODE" -ne 1 ] && say "cmsec: no checks installed in $CHECKS_DIR"
    exit 0
  }

  if [ "$DMOTD_MODE" -eq 1 ]; then
    for cve_id in $checks; do
      do_one_check_dmotd "$cve_id" || true
    done
    return 0
  fi

  for cve_id in $checks; do
    saw_any=1
    if [ "$JSON_OUT" -ne 1 ]; then
      header "===== cmsec: $cve_id ====="
    fi
    do_one_check_full "$cve_id"
    rc=$?
    case "$rc" in
      1) exit_aggregate=1 ;;
      # rc=2 is a usage / resolve failure (e.g. check-*.sh present in checks/
      # but not executable). Treat as indeterminate at the aggregate level so
      # missing-but-listed checks don't silently report "all clear".
      2|3) [ "$exit_aggregate" -eq 0 ] && saw_indeterminate=1 ;;
    esac
  done

  if [ "$exit_aggregate" -eq 1 ]; then
    return 1
  elif [ "$saw_indeterminate" -eq 1 ]; then
    return 3
  fi
  return 0
}

do_check() {
  local cve_id="$CVE_ID"
  if [ -z "$cve_id" ]; then
    printf 'cmsec check: missing <cve-id>\n' >&2
    usage >&2; exit 2
  fi
  if [ "$DMOTD_MODE" -eq 1 ]; then
    do_one_check_dmotd "$cve_id"
    return $?
  fi
  do_one_check_full "$cve_id"
}

# ---------- main ----------
case "$SUBCMD" in
  list)   do_list ;;
  probe)  do_probe ;;
  check)  do_check; exit $? ;;
  run)    do_run; exit $? ;;
  *)      usage; exit 2 ;;
esac
