#!/usr/bin/env bash
#
# cache.sh — state-based cache for cmsec
#
# Sourced by tools/cmm-security/cmsec.sh. Provides:
#
#   cmsec_cache_dir            -> echoes the cache directory, creates if missing
#   cmsec_cache_state_key      -> echoes a digest of all state inputs that
#                                 should invalidate the cache (uname -r, kernel
#                                 cmdline mitigation flag, livepatch state,
#                                 check-script SHA, baseline file SHA)
#   cmsec_cache_read FILE      -> if FILE exists and its embedded state key
#                                 matches the current state and TTL hasn't
#                                 expired, echoes the cached payload and
#                                 returns 0; otherwise returns 1
#   cmsec_cache_read_stale FILE -> echoes any existing cached payload regardless
#                                 of state-key/TTL match (used for stale
#                                 fallback under flock contention)
#   cmsec_cache_write FILE PAYLOAD -> writes PAYLOAD with current state key
#                                 header under flock; safe under concurrent
#                                 dmotd login stampedes
#
# Cache file format:
#   #cmsec-cache-v1
#   STATE_KEY=<sha256>
#   WRITTEN_TS=<epoch>
#   ---
#   <payload bytes>
#
# Layout:
#   /var/cache/centminmod/cmsec/        (root: 0700)
#     <CVE-id>.cache                    (file: 0644)
#     <CVE-id>.lock                     (flock target)
#

CMSEC_CACHE_BASE="${CMSEC_CACHE_BASE:-/var/cache/centminmod/cmsec}"
CMSEC_CACHE_TTL_MIN="${CMSEC_CACHE_TTL_MIN:-1440}"   # 24h fallback safety net

cmsec_cache_dir() {
  if [ ! -d "$CMSEC_CACHE_BASE" ]; then
    if [ "$(id -u 2>/dev/null || echo 1)" = "0" ]; then
      mkdir -p "$CMSEC_CACHE_BASE" 2>/dev/null && chmod 0700 "$CMSEC_CACHE_BASE" 2>/dev/null
    else
      # non-root caller (e.g. cmsec CLI invoked by ordinary user) —
      # fall back to per-user cache dir; no host-level cache write.
      CMSEC_CACHE_BASE="${HOME:-/tmp}/.cache/centminmod/cmsec"
      mkdir -p "$CMSEC_CACHE_BASE" 2>/dev/null && chmod 0700 "$CMSEC_CACHE_BASE" 2>/dev/null
    fi
  fi
  printf '%s' "$CMSEC_CACHE_BASE"
}

# Compose a state digest. Input changes here force re-evaluation.
cmsec_cache_state_key() {
  local check_script="${1:-}"
  local baseline_file="${2:-}"
  local kernel="$(uname -r 2>/dev/null || echo unknown)"
  # OS identity participates in cache key — verdict baselines branch on
  # ${os_id}:${os_version} (e.g., almalinux:8 vs rocky:8 vs centos:8). Same
  # kernel string after an in-place OS conversion (migrate2alma, etc.) must
  # not reuse the prior OS's verdict.
  local os_identity="unknown"
  if [ -r /etc/os-release ]; then
    os_identity="$(awk -F= '
      /^ID=/{gsub(/"/,"",$2); id=$2}
      /^VERSION_ID=/{gsub(/"/,"",$2); ver=$2}
      END{print id":"ver}' /etc/os-release 2>/dev/null)"
    [ -z "$os_identity" ] || [ "$os_identity" = ":" ] && os_identity="unknown"
  fi
  local cmdline_mitig=""
  if [ -r /proc/cmdline ]; then
    cmdline_mitig="$(grep -oE 'initcall_blacklist=algif_aead_init' /proc/cmdline 2>/dev/null || true)"
  fi
  local sysctl_ptrace_scope=""
  if [ -r /proc/sys/kernel/yama/ptrace_scope ]; then
    sysctl_ptrace_scope="$(cat /proc/sys/kernel/yama/ptrace_scope 2>/dev/null || echo unknown)"
  fi
  local livepatch_digest=""
  if command -v kcarectl >/dev/null 2>&1; then
    # Wrap with `timeout` to prevent hanging the dmotd login banner when the
    # KernelCare daemon is unresponsive. 5 seconds is generous for a local
    # query; bail to empty digest on timeout (state-key still varies on the
    # other inputs). Use the same `--patch-info || --info` fallback chain
    # the verdict logic uses (check-cve-2026-31431.sh kcarectl_covers_cve)
    # so the state key reflects whichever output the verdict actually saw.
    local kc_raw=""
    if command -v timeout >/dev/null 2>&1; then
      kc_raw="$(timeout 5 kcarectl --patch-info 2>/dev/null || timeout 5 kcarectl --info 2>/dev/null || true)"
    else
      kc_raw="$(kcarectl --patch-info 2>/dev/null || kcarectl --info 2>/dev/null || true)"
    fi
    livepatch_digest="$(printf '%s' "$kc_raw" | sha256sum 2>/dev/null | awk '{print $1}' | head -c 16)"
  fi
  local script_sha=""
  [ -n "$check_script" ] && [ -r "$check_script" ] && \
    script_sha="$(sha256sum "$check_script" 2>/dev/null | awk '{print $1}' | head -c 16)"
  local baseline_sha=""
  [ -n "$baseline_file" ] && [ -r "$baseline_file" ] && \
    baseline_sha="$(sha256sum "$baseline_file" 2>/dev/null | awk '{print $1}' | head -c 16)"
  # Dirty Frag (CVE-2026-43284 / CVE-2026-43500) modprobe-blacklist digest.
  # Toggling /etc/modprobe.d/dirtyfrag.conf (or any conf blacklisting esp4 /
  # esp6 / rxrpc) invalidates the cached verdict. Search /etc, /usr/lib, and
  # /run modprobe.d roots so vendor-shipped or systemd-tmpfile-installed
  # blacklist files also feed the key. Per-conf-file grep + single sha over
  # the matched-files list runs in <10ms even with hundreds of conf files;
  # safe in the dmotd hot path. See plan §"4. cache.sh — extend state key".
  local modprobe_dirtyfrag_digest=""
  local _mp_dir _mp_files
  _mp_files=""
  for _mp_dir in /etc/modprobe.d /usr/lib/modprobe.d /run/modprobe.d; do
    [ -d "$_mp_dir" ] || continue
    _mp_files="$_mp_files
$(grep -lE '^[[:space:]]*(install[[:space:]]+(esp4|esp6|rxrpc)[[:space:]]+/bin/false|blacklist[[:space:]]+(esp4|esp6|rxrpc))' \
      "$_mp_dir"/*.conf 2>/dev/null || true)"
  done
  modprobe_dirtyfrag_digest="$(printf '%s' "$_mp_files" \
    | sed '/^$/d' | sort -u \
    | xargs -r sha256sum 2>/dev/null \
    | sha256sum | awk '{print $1}' | head -c 16 || true)"
  # Fold lsmod state for the three modules into the digest, so a manual
  # rmmod / modprobe between sessions invalidates the cache even when the
  # modprobe.d files don't change.
  local _mod_loaded=""
  if [ -r /proc/modules ]; then
    _mod_loaded="$(awk '$1=="esp4"||$1=="esp6"||$1=="rxrpc" {print $1":1"}' /proc/modules \
                  | sort | tr '\n' ',')"
  fi
  modprobe_dirtyfrag_digest="$(printf '%s|%s' "$modprobe_dirtyfrag_digest" "$_mod_loaded" \
                                | sha256sum | awk '{print $1}' | head -c 16)"
  printf '%s|%s|%s|%s|%s|%s|%s|%s' \
    "$kernel" "$os_identity" "$cmdline_mitig" "$sysctl_ptrace_scope" "$livepatch_digest" "$script_sha" "$baseline_sha" "$modprobe_dirtyfrag_digest" \
    | sha256sum | awk '{print $1}'
}

cmsec_cache_read() {
  local file="$1"
  local current_key="$2"
  [ -r "$file" ] || return 1

  local stored_key written_ts age_min
  stored_key="$(awk -F= '/^STATE_KEY=/{print $2; exit}' "$file" 2>/dev/null)"
  [ "$stored_key" = "$current_key" ] || return 1

  written_ts="$(awk -F= '/^WRITTEN_TS=/{print $2; exit}' "$file" 2>/dev/null)"
  if [ -n "$written_ts" ] && [ "$written_ts" -gt 0 ] 2>/dev/null; then
    age_min=$(( ( $(date +%s) - written_ts ) / 60 ))
    [ "$age_min" -gt "$CMSEC_CACHE_TTL_MIN" ] && return 1
  fi

  awk 'flag{print; next} /^---$/{flag=1}' "$file"
}

cmsec_cache_read_stale() {
  # Stale = TTL expired but state key (kernel/OS/baseline) still matches.
  # If the state key differs (e.g., user rebooted into a new kernel), the
  # cached payload reflects a system state that no longer exists and MUST
  # NOT be returned — that would cause dmotd to keep showing "VULNERABLE"
  # on a freshly-patched system until the async refresh completed. The
  # state-key argument is optional (omitted = legacy unconditional read,
  # not used by current callers) so this stays additive.
  local file="$1"
  local current_key="${2:-}"
  [ -r "$file" ] || return 1
  if [ -n "$current_key" ]; then
    local stored_key
    stored_key="$(awk -F= '/^STATE_KEY=/{print $2; exit}' "$file" 2>/dev/null)"
    [ "$stored_key" = "$current_key" ] || return 1
  fi
  awk 'flag{print; next} /^---$/{flag=1}' "$file"
}

cmsec_cache_write() {
  local file="$1" current_key="$2"
  shift 2
  local payload="$*"
  local lock="${file%.cache}.lock"

  # Ensure parent dir exists with proper mode.
  cmsec_cache_dir >/dev/null

  # Use flock with non-blocking attempt; if contended, just bail (another
  # process is already refreshing) — caller can fall back to stale cache.
  if command -v flock >/dev/null 2>&1; then
    (
      exec 9>"$lock" 2>/dev/null || exit 0
      flock -n 9 || exit 0
      umask 022
      {
        printf '#cmsec-cache-v1\n'
        printf 'STATE_KEY=%s\n' "$current_key"
        printf 'WRITTEN_TS=%s\n' "$(date +%s)"
        printf -- '---\n'
        printf '%s' "$payload"
      } >"$file.tmp" 2>/dev/null && mv -f "$file.tmp" "$file" 2>/dev/null
      chmod 0644 "$file" 2>/dev/null || true
    )
  else
    # No flock — best effort write (rare; flock present on EL7+).
    umask 022
    {
      printf '#cmsec-cache-v1\n'
      printf 'STATE_KEY=%s\n' "$current_key"
      printf 'WRITTEN_TS=%s\n' "$(date +%s)"
      printf -- '---\n'
      printf '%s' "$payload"
    } >"$file.tmp" 2>/dev/null && mv -f "$file.tmp" "$file" 2>/dev/null
    chmod 0644 "$file" 2>/dev/null || true
  fi
}
