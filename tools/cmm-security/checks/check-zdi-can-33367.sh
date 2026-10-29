#!/usr/bin/env bash
#
# check-zdi-can-33367.sh
#
# Read-only ZDI-CAN-33367 skb_zerocopy() kernel LPE checker for AlmaLinux,
# Rocky Linux, CloudLinux, Oracle Linux, RHEL-family VPS, dedicated
# servers, and Linode/Akamai Cloud instances.
#
# Tool version: 0.1
# Written 2026-10-29. Based on the Fragnesia check (CVE-2026-46300), which
# covers the same bug class.
#
# What this issue is:
#   ZDI-CAN-33367 (CVSS 7.8, no CVE assigned yet) is a Linux kernel local
#   privilege escalation. skb_zerocopy(), used by NFQUEUE and Open vSwitch,
#   moves page-cache-backed fragments into a new socket buffer but drops the
#   SKBFL_SHARED_FRAG marker that the Dirty Frag fix relies on. IPsec ESP
#   (esp4/esp6) then decrypts in place over the cached file pages, letting
#   an unprivileged user (via an unprivileged user namespace) change the
#   in-memory copy of read-only files such as setuid binaries and gain root.
#   It bypasses the Dirty Frag (CVE-2026-43284) and Fragnesia
#   (CVE-2026-46300) fixes: a kernel patched for those can still be
#   vulnerable. rxrpc is not involved. Blocking esp4 and esp6 via
#   modprobe.d is AlmaLinux's published workaround.
#
# What this script does (read-only):
#    1. Prints OS release information.
#    2. Prints the currently running kernel.
#    3. Lists installed kernel/kernel-core RPM packages.
#    4. Shows the default boot kernel selected by grubby, where available.
#    5. Maps the running kernel back to an installed RPM package.
#    6. Reports kernel updates available / up-to-date / dnf error distinctly.
#       (dnf updateinfo is not queried: there is no CVE ID to search for.)
#    7. Detects esp4 / esp6 module state via /lib/modules/.../modules.builtin,
#       /sys/module/<name>, modinfo, and kernel config
#       (CONFIG_INET_ESP / CONFIG_INET6_ESP).
#    8. Detects whether the modprobe.d stopgap (install esp4 /bin/false and
#       install esp6 /bin/false, or blacklist) covers BOTH modules, in any
#       file. An existing Dirty Frag dirtyfrag.conf counts.
#    9. Detects whether the host is an IPsec gateway via xfrm policy/state,
#       strongswan/libreswan service state, /etc/ipsec.conf etc., and
#       NetworkManager VPN profiles. Used to gate the modprobe.d
#       recommendation, which would BREAK IPsec on such hosts.
#   10. Detects livepatch tooling (KernelCare, kpatch) and tries to confirm
#       ZDI-CAN-33367 coverage with kcarectl. Dirty Frag / Fragnesia
#       livepatches do NOT count as coverage.
#   11. Detects containers and chroots; downgrades kernel verdicts to
#       indeterminate inside them.
#   12. Marks stock CentOS/RHEL 6 and 7 as not_affected (medium confidence)
#       and stock CloudLinux 7 (non-h) as not_affected per CloudLinux.
#       CloudLinux 7h (hybrid EL8-based kernel) IS affected and is
#       disambiguated via the .lve[.N].el7h kernel suffix.
#   13. Checks the running kernel's RPM changelog for the upstream fix
#       subject (FIX_MARKER below). This is the primary signal on every
#       distro. [Divergence from 46300] Version numbers are NOT unique
#       across vendors for this fix: AlmaLinux's fixed
#       kernel-5.14.0-687.60.1.el9_8 carries the patch, while Rocky
#       Linux's kernel with the identical NVR (and its 687.61.1) does not
#       (verified on live AL9 / Rocky 9.8 hosts 2026-10-29). Comparing a
#       Rocky kernel against AlmaLinux NVRs would report a false PATCHED.
#   14. Falls back to the vendor's own fixed-NVR baseline (AlmaLinux 8/9/10,
#       CloudLinux 7h/8) only when the changelog cannot be read.
#   15. Detects Linode-provided kernels and emits a Linode-specific advisory
#       (no fixed Linode baseline known at script ship date).
#
# What this script does NOT do:
#   - Install updates, reboot, modify GRUB, change mitigations, unload
#     modules, run exploit code, or call provider APIs.
#   - Write the modprobe.d stopgap file. The "Suggested next actions"
#     block tells the user the exact command, but executing it is their
#     decision.
#   - Check the user.max_user_namespaces=0 alternative mitigation.
#
# Recommended fix:
#   Update to a vendor-patched kernel, reboot, then re-run this script.
#
# Vendor fixed-kernel baselines (verified 2026-10-29). Lowest fixed NVR per
# vendor; AlmaLinux's testing kernels (2026-10-23) carry the fix, production
# kernels followed on 2026-10-28:
#   AlmaLinux 8:         kernel-4.18.0-553.177.2.el8_10 or newer (production: 553.178.1)
#   AlmaLinux 9:         kernel-5.14.0-687.59.2.el9_8 or newer   (production: 687.60.1)
#   AlmaLinux 10:        kernel-6.12.0-211.68.2.el10_2 or newer  (production: 211.69.1)
#     https://almalinux.org/blog/2026-10-23-zdi-can-33367/
#   CloudLinux 7h:       kernel-4.18.0-553.177.1.lve.1.el7h or newer (beta, rolling out)
#   CloudLinux 8:        kernel-4.18.0-553.177.1.lve.1.el8 or newer  (beta, rolling out)
#     https://blog.cloudlinux.com/zdi-can-33367
#   AlmaLinux Kitten 10, Rocky Linux 8/9/10, RHEL 8/9/10, Oracle Linux
#   8/9/10 RHCK, CloudLinux 9/10: no vendor NVR published; changelog only.
#
# Stock CentOS/RHEL 7 and 6:
#   NOT_AFFECTED. TuxCare: the EL7 3.10 ESP code always copies nonlinear
#   packets before decrypting, and the EL6 2.6.32 kernel predates
#   skb_zerocopy(). Verdict confidence: medium (not Red Hat confirmed).
#     https://tuxcare.com/blog/zdi-ksmbd-bugs-cve/
#
# Exit codes:
#   0 = patched / not_affected
#   1 = vulnerable (fix not in the running kernel AND no confirmed
#                   livepatch coverage)
#   2 = usage / argument error
#   3 = indeterminate (unknown vendor, container/chroot, custom/UEK kernel,
#                      or no changelog and no comparable baseline)
#
# Safe usage:
#   chmod +x check-zdi-can-33367.sh
#   ./check-zdi-can-33367.sh
#   ./check-zdi-can-33367.sh --json
#   ./check-zdi-can-33367.sh --debug --log-file /tmp/cve-check.log
#   ./check-zdi-can-33367.sh --quiet --json
#
# Help:
#   ./check-zdi-can-33367.sh --help
#
set -euo pipefail

# No CVE yet. The variable keeps its name (and the JSON "cve" key its
# value) so cmsec, dmotd and check-for-updates consumers work unchanged.
# Rename this file to check-cve-YYYY-NNNNN.sh once a CVE is assigned.
CVE="ZDI-CAN-33367"
CVE_LABEL="skb_zerocopy shared-frag LPE"
TOOL_VERSION="0.1-cmsec"
BASELINE_VERIFIED_DATE="2026-10-29"
# Upstream fix subject, as listed in AlmaLinux 8/9/10 kernel changelogs:
#   - net: skbuff: propagate shared-frag marker in skb_zerocopy()
# https://lore.kernel.org/all/20261022104217.1852191-1-pointerliu@126.com/
FIX_MARKER="propagate shared-frag marker in skb_zerocopy"
# Whether Rocky Linux's emergency `security` repo ships a fix for this
# issue. No Rocky fix was announced as of BASELINE_VERIFIED_DATE.
# Reference: https://rockylinux.org/news/2026-05-14-introducing-security-repository
ROCKY_SECURITY_REPO_HAS_FIX="false"

DEBUG=0
JSON=0
QUIET=0
LOG_FILE=""

FINAL_STATUS="indeterminate"
FINAL_CONFIDENCE="low"
declare -a REASONS=()
declare -a RECOMMENDED_ACTIONS=()

# JSON-collected fields
J_OS_ID=""
J_OS_VERSION=""
J_OS_PRETTY=""
J_RUNNING_KERNEL=""
J_RUNNING_KERNEL_FULL=""
J_KERNEL_RPM=""
J_VENDOR_BASELINE=""
J_VENDOR_BASELINE_DETECTED=""
J_COMPARISON_RESULT=""
J_CHANGELOG_CONFIRMS_FIX="unknown"
J_PENDING_VENDOR_NVR="false"
J_MITIGATION_ACTIVE="false"
J_MODPROBE_BLACKLIST_PATH=""
J_ESP4_STATE="unknown"
J_ESP6_STATE="unknown"
J_IPSEC_HOST_DETECTED="false"
J_IPSEC_DETECTION_REASONS=""
J_LINODE_KERNEL=""
J_LIVEPATCH_TOOL=""
J_LIVEPATCH_CVE_COVERED="unknown"
J_CONTAINER_DETECTED="false"
J_CHROOT_DETECTED="false"
J_UEK_DETECTED="false"

show_help() {
  cat <<'EOF'
check-zdi-can-33367.sh (v0.1)

Read-only checker for ZDI-CAN-33367 (no CVE assigned yet) — a kernel local
privilege escalation where skb_zerocopy() drops SKBFL_SHARED_FRAG, letting
IPsec ESP (esp4/esp6) decrypt in place over page-cache pages. It bypasses
the Dirty Frag and Fragnesia fixes. Blocking esp4 and esp6 via modprobe.d
is the published workaround; an existing Dirty Frag dirtyfrag.conf covers it.

Usage:
  ./check-zdi-can-33367.sh [options]

Options:
  --json              Emit a single JSON document instead of human-readable
                      output. Pairs well with structured exit codes.
  --quiet             Suppress narrative output. Compatible with --json.
                      Final status line still printed unless --json.
  --debug             Verbose decision audit to stderr (or --log-file).
  --log-file PATH     Tee --debug output to a file (and stderr).
  --help, -h          Show this help.

Exit codes:
  0 = patched / not_affected
  1 = vulnerable
  2 = usage error
  3 = indeterminate

Related CVEs:
  CVE-2026-43284 (Dirty Frag / ESP half)
  CVE-2026-46300 (Fragnesia)
  Same bug class, but their kernel fixes do NOT fix ZDI-CAN-33367.
EOF
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --help|-h)       show_help; exit 0 ;;
    --json)          JSON=1; QUIET=1 ;;
    --quiet)         QUIET=1 ;;
    --debug)         DEBUG=1 ;;
    --log-file)      shift; LOG_FILE="${1:-}"; [ -z "$LOG_FILE" ] && { echo "--log-file requires a path" >&2; exit 2; } ;;
    --log-file=*)    LOG_FILE="${1#--log-file=}" ;;
    *)               echo "Unknown option: $1" >&2; echo "Use --help for usage." >&2; exit 2 ;;
  esac
  shift
done

if [ -n "$LOG_FILE" ]; then
  : >"$LOG_FILE" 2>/dev/null || { echo "Cannot write log file: $LOG_FILE" >&2; exit 2; }
fi

dbg() {
  [ "$DEBUG" -eq 1 ] || return 0
  local msg="[DEBUG] $*"
  printf '%s\n' "$msg" >&2
  [ -n "$LOG_FILE" ] && printf '%s\n' "$msg" >>"$LOG_FILE" 2>/dev/null || true
}

say() {
  [ "$QUIET" -eq 1 ] && return 0
  printf '%s\n' "$*"
}

section() {
  [ "$QUIET" -eq 1 ] && return 0
  printf '\n%s\n' "$*"
}

record_reason()  { REASONS+=("$1"); dbg "reason: $1"; }
record_action()  {
  local a
  for a in "${RECOMMENDED_ACTIONS[@]+"${RECOMMENDED_ACTIONS[@]}"}"; do
    [ "$a" = "$1" ] && return 0
  done
  RECOMMENDED_ACTIONS+=("$1")
}

upgrade_status() {
  local new="$1" conf="${2:-medium}"
  case "$FINAL_STATUS:$new" in
    vulnerable:*)            return 0 ;;
    *:vulnerable)            FINAL_STATUS="$new"; FINAL_CONFIDENCE="$conf" ;;
    *)                       FINAL_STATUS="$new"; FINAL_CONFIDENCE="$conf" ;;
  esac
  dbg "upgrade_status -> $FINAL_STATUS ($FINAL_CONFIDENCE)"
}

has_cmd()         { command -v "$1" >/dev/null 2>&1; }
print_cmd_status() {
  if has_cmd "$1"; then say "$1: found"; else say "$1: not found"; fi
}

strip_arch() {
  printf '%s\n' "$1" | sed -E 's/\.x86_64$//'
}

# Compares with the trailing dist tag (.el8_10, .el7h, .el9_7_ciq, ...) removed.
# sort -V puts a letter after a digit, so it ranks a GA release such as
# 4.18.0-553.el8_10 above the z-stream 4.18.0-553.124.3.el8_10, the reverse of
# rpm order. Vendor callers run dist_tag_compatible() first, so the tag adds no
# ordering; Linode semver strings carry no tag.
ver_ge() {
  local a b
  local first
  a="$(printf '%s' "$1" | sed -E 's/\.el[0-9].*$//')"
  b="$(printf '%s' "$2" | sed -E 's/\.el[0-9].*$//')"
  first="$(printf '%s\n%s\n' "$b" "$a" | sort -V | head -n1)"
  if [ "$first" = "$b" ]; then
    dbg "ver_ge: '$a' >= '$b' -> true"
    return 0
  else
    dbg "ver_ge: '$a' >= '$b' -> false (sort -V first=$first)"
    return 1
  fi
}

dist_tag_compatible() {
  local running="$1" baseline="$2"
  local r_tag b_tag
  r_tag="$(printf '%s' "$running"  | grep -oE '(\.lve)?\.el[0-9]+(_[0-9]+)?(h)?(_ciq)?$' || true)"
  b_tag="$(printf '%s' "$baseline" | grep -oE '(\.lve)?\.el[0-9]+(_[0-9]+)?(h)?(_ciq)?$' || true)"
  dbg "dist_tag_compatible: running_tag='$r_tag' baseline_tag='$b_tag'"
  if [ -z "$r_tag" ] || [ -z "$b_tag" ]; then
    return 1
  fi
  local r_norm b_norm
  r_norm="$(printf '%s' "$r_tag" | sed -E 's/_ciq$//')"
  b_norm="$(printf '%s' "$b_tag" | sed -E 's/_ciq$//')"
  [ "$r_norm" = "$b_norm" ] && return 0
  # Same-EL-major cross-minor: .el9_8 vs .el9_7, .el10_2 vs .el10_1, etc.
  # Assumes CVE errata land simultaneously across active minor streams (RHEL
  # standard policy); ver_ge() handles direction via sort -V. The (h)? in the
  # regex preserves the CL7h suffix, keeping .lve.el7h incompatible with .lve.el7.
  local r_major b_major
  r_major="$(printf '%s' "$r_norm" | grep -oE '^(\.lve)?\.el[0-9]+(h)?' || true)"
  b_major="$(printf '%s' "$b_norm" | grep -oE '^(\.lve)?\.el[0-9]+(h)?' || true)"
  [ -n "$r_major" ] && [ -n "$b_major" ] && [ "$r_major" = "$b_major" ]
}

# [Divergence 1] Tri-state changelog probe, the primary verdict signal.
# Echoes "fix" when the running kernel's RPM changelog lists the upstream fix
# subject (or the ZDI ID), "nofix" when the changelog was read and lacks it,
# and "unavailable" when there is no rpm, no RPM owns the running kernel, or
# the changelog is empty. Uses the package found by the RPM mapping section
# (kernel-core on EL8+). grep reads all input (no -q) so there is no SIGPIPE
# race under pipefail.
running_kernel_changelog_state() {
  has_cmd rpm || { printf 'unavailable'; return 0; }
  local pkg log hits
  pkg="${J_KERNEL_RPM%%$'\n'*}"
  [ -n "$pkg" ] || { printf 'unavailable'; return 0; }
  if ! log="$(rpm -q --changelog "$pkg" 2>/dev/null)" || [ -z "$log" ]; then
    printf 'unavailable'; return 0
  fi
  hits="$(printf '%s\n' "$log" | grep -iF -e "$FIX_MARKER" -e "$CVE" || true)"
  dbg "changelog hits for $pkg: ${hits:-none}"
  if [ -n "$hits" ]; then printf 'fix'; else printf 'nofix'; fi
}

detect_container() {
  if has_cmd systemd-detect-virt; then
    local v
    v="$(systemd-detect-virt --container 2>/dev/null || true)"
    if [ -n "$v" ] && [ "$v" != "none" ]; then
      dbg "container: systemd-detect-virt=$v"
      printf '%s' "$v"; return 0
    fi
  fi
  [ -f /.dockerenv ]               && { dbg "container: /.dockerenv";       printf 'docker';   return 0; }
  [ -f /run/.containerenv ]        && { dbg "container: /run/.containerenv"; printf 'podman';   return 0; }
  if [ -r /proc/1/cgroup ] && grep -qE '/(docker|kubepods|containerd|lxc|libpod|crio)' /proc/1/cgroup 2>/dev/null; then
    dbg "container: cgroup hint"
    printf 'cgroup-detected'; return 0
  fi
  printf ''
}

detect_chroot() {
  if [ -r /proc/1/root ]; then
    local root_inode init_root_inode
    root_inode="$(stat -L -c '%i' / 2>/dev/null || echo 0)"
    init_root_inode="$(stat -L -c '%i' /proc/1/root 2>/dev/null || echo 0)"
    if [ "$root_inode" != "0" ] && [ "$init_root_inode" != "0" ] && [ "$root_inode" != "$init_root_inode" ]; then
      dbg "chroot: / inode=$root_inode pid1 root inode=$init_root_inode"
      printf 'true'; return 0
    fi
  fi
  printf 'false'
}

detect_uek() {
  case "$1" in
    *uek*|*UEK*) printf 'true' ;;
    *)           printf 'false' ;;
  esac
}

detect_esp_module_state() {
  local mod="$1"
  local kver
  kver="$(uname -r)"
  if [ -r "/lib/modules/$kver/modules.builtin" ] && \
     grep -qE "(^|/)${mod}\\.ko(\$|\\.)" "/lib/modules/$kver/modules.builtin" 2>/dev/null; then
    printf 'builtin'
    return 0
  fi
  if [ -d "/sys/module/$mod" ]; then
    if [ -r "/sys/module/$mod/initstate" ] && \
       [ "$(cat "/sys/module/$mod/initstate" 2>/dev/null || echo)" = "live" ]; then
      printf 'loaded'
      return 0
    fi
    printf 'present'
    return 0
  fi
  if has_cmd modinfo && modinfo "$mod" >/dev/null 2>&1; then
    printf 'module-available'
    return 0
  fi
  local cfg_key
  [ "$mod" = "esp4" ] && cfg_key="CONFIG_INET_ESP" || cfg_key="CONFIG_INET6_ESP"
  if [ -r "/boot/config-$kver" ]; then
    case "$(grep -E "^${cfg_key}=" "/boot/config-$kver" 2>/dev/null || true)" in
      *=y) printf 'builtin-from-config'; return 0 ;;
      *=m) printf 'module-from-config';  return 0 ;;
    esac
  fi
  if [ -r /proc/config.gz ] && has_cmd zcat; then
    case "$(zcat /proc/config.gz 2>/dev/null | grep -E "^${cfg_key}=" || true)" in
      *=y) printf 'builtin-from-config'; return 0 ;;
      *=m) printf 'module-from-config';  return 0 ;;
    esac
  fi
  printf 'absent-or-unknown'
}

# [Divergence 2] Per-module scan (esp4 / esp6 only; rxrpc is not involved).
# Echoes the first modprobe.d file that blocks module $1 with
# `install <mod> /bin/false` or `blacklist <mod>`, or nothing. Each module is
# looked up across all files, so esp4 and esp6 may be blocked in different
# files. The trailing ([[:space:]]|$) stops `blacklist esp4_offload` matching.
modprobe_block_file() {
  local mod="$1" d f
  for d in /etc/modprobe.d /usr/lib/modprobe.d /run/modprobe.d; do
    [ -d "$d" ] || continue
    f="$(grep -lE "^[[:space:]]*(install[[:space:]]+${mod}[[:space:]]+/bin/false|blacklist[[:space:]]+${mod})([[:space:]]|\$)" \
      "$d"/*.conf 2>/dev/null | head -n 1 || true)"
    [ -n "$f" ] && { printf '%s' "$f"; return 0; }
  done
  printf ''
}

detect_ipsec_host() {
  local hits=""
  if has_cmd ip; then
    if [ -n "$(ip xfrm policy list 2>/dev/null | head -n 1)" ]; then
      hits="${hits:+$hits,}xfrm-policy"
    fi
    if [ -n "$(ip xfrm state list 2>/dev/null | head -n 1)" ]; then
      hits="${hits:+$hits,}xfrm-state"
    fi
  fi
  if has_cmd systemctl; then
    local svc
    for svc in strongswan strongswan-starter libreswan ipsec; do
      if systemctl is-active --quiet "$svc" 2>/dev/null; then
        hits="${hits:+$hits,}svc:$svc"
      fi
    done
  fi
  local f
  for f in /etc/ipsec.conf /etc/ipsec.secrets; do
    [ -f "$f" ] && { hits="${hits:+$hits,}cfg:$(basename "$f")"; }
  done
  for f in /etc/ipsec.d /etc/swanctl /etc/strongswan; do
    if [ -d "$f" ] && [ -n "$(ls -A "$f" 2>/dev/null)" ]; then
      hits="${hits:+$hits,}dir:$(basename "$f")"
    fi
  done
  if [ -e /etc/strongswan.conf ] || [ -e /etc/strongswan.d ]; then
    hits="${hits:+$hits,}strongswan-installed"
  fi
  if has_cmd nmcli; then
    if nmcli -t -f NAME,TYPE connection 2>/dev/null \
       | grep -qE ':vpn$'; then
      hits="${hits:+$hits,}nm-vpn"
    fi
  fi
  if [ -d /etc/NetworkManager/system-connections ]; then
    if grep -lE '^vpn-type=(libreswan|strongswan|ipsec)' \
         /etc/NetworkManager/system-connections/*.nmconnection 2>/dev/null \
       | head -n 1 >/dev/null; then
      hits="${hits:+$hits,}nm-ipsec-profile"
    fi
  fi
  printf '%s' "$hits"
}

kpatch_present() {
  has_cmd rpm || return 1
  rpm -qa 'kpatch*' 2>/dev/null | grep -q '^kpatch' && return 0
  return 1
}

# [Divergence 3] Only this issue's own identifiers count. Dirty Frag and
# Fragnesia livepatches (shared-frag, xfrm-esp, esp4/esp6 keywords) do NOT
# fix ZDI-CAN-33367, so matching them would report false coverage. A bare
# "skb_zerocopy" would also match the unrelated CVE-2026-90049 fix.
kcarectl_covers_cve() {
  has_cmd kcarectl || return 2
  local out kc_runner=""
  has_cmd timeout && kc_runner="timeout 5"
  out="$($kc_runner kcarectl --patch-info 2>/dev/null || $kc_runner kcarectl --info 2>/dev/null || true)"
  if printf '%s' "$out" | grep -qiE "ZDI.?CAN.?33367|shared.?frag marker in skb.?zerocopy"; then
    return 0
  fi
  return 1
}

json_escape() {
  printf '%s' "$1" | python3 -c 'import sys,json;print(json.dumps(sys.stdin.read()))' 2>/dev/null && return 0
  printf '"%s"' "$(printf '%s' "$1" | sed \
    -e 's/\\/\\\\/g' \
    -e 's/"/\\"/g' \
    -e 's/\t/\\t/g' \
    -e 's/\r/\\r/g' \
    -e ':a' -e 'N' -e '$!ba' -e 's/\n/\\n/g' \
    | tr -d '\000-\010\013\014\016-\037')"
}

emit_json() {
  local reasons_json="[]" actions_json="[]"
  if [ "${#REASONS[@]}" -gt 0 ]; then
    reasons_json="[$(printf '%s\n' "${REASONS[@]}" | while IFS= read -r r; do printf '%s,' "$(json_escape "$r")"; done | sed 's/,$//')]"
  fi
  if [ "${#RECOMMENDED_ACTIONS[@]}" -gt 0 ]; then
    actions_json="[$(printf '%s\n' "${RECOMMENDED_ACTIONS[@]}" | while IFS= read -r r; do printf '%s,' "$(json_escape "$r")"; done | sed 's/,$//')]"
  fi
  cat <<EOF
{
  "cve": $(json_escape "$CVE"),
  "cve_label": $(json_escape "$CVE_LABEL"),
  "tool_version": $(json_escape "$TOOL_VERSION"),
  "scanned_at": $(json_escape "$(date -u +%Y-%m-%dT%H:%M:%SZ)"),
  "cvss_score": "7.8",
  "cvss_severity": "high",
  "final_status": $(json_escape "$FINAL_STATUS"),
  "confidence": $(json_escape "$FINAL_CONFIDENCE"),
  "exit_code": $1,
  "os_id": $(json_escape "$J_OS_ID"),
  "os_version": $(json_escape "$J_OS_VERSION"),
  "os_pretty": $(json_escape "$J_OS_PRETTY"),
  "running_kernel": $(json_escape "$J_RUNNING_KERNEL"),
  "running_kernel_full": $(json_escape "$J_RUNNING_KERNEL_FULL"),
  "kernel_rpm": $(json_escape "$J_KERNEL_RPM"),
  "vendor_baseline": $(json_escape "$J_VENDOR_BASELINE"),
  "vendor_baseline_detected_for": $(json_escape "$J_VENDOR_BASELINE_DETECTED"),
  "comparison_result": $(json_escape "$J_COMPARISON_RESULT"),
  "changelog_confirms_fix": $(json_escape "$J_CHANGELOG_CONFIRMS_FIX"),
  "pending_vendor_nvr": $J_PENDING_VENDOR_NVR,
  "mitigation_active": $J_MITIGATION_ACTIVE,
  "modprobe_blacklist_path": $(json_escape "$J_MODPROBE_BLACKLIST_PATH"),
  "esp4_state": $(json_escape "$J_ESP4_STATE"),
  "esp6_state": $(json_escape "$J_ESP6_STATE"),
  "ipsec_host_detected": $J_IPSEC_HOST_DETECTED,
  "ipsec_detection_reasons": $(json_escape "$J_IPSEC_DETECTION_REASONS"),
  "linode_kernel": $(json_escape "$J_LINODE_KERNEL"),
  "livepatch_tool": $(json_escape "$J_LIVEPATCH_TOOL"),
  "livepatch_cve_covered": $(json_escape "$J_LIVEPATCH_CVE_COVERED"),
  "container_detected": $J_CONTAINER_DETECTED,
  "chroot_detected": $J_CHROOT_DETECTED,
  "uek_detected": $J_UEK_DETECTED,
  "baseline_verified_date": $(json_escape "$BASELINE_VERIFIED_DATE"),
  "reasons": $reasons_json,
  "recommended_actions": $actions_json
}
EOF
}

# ----- begin checks -----

os_id=""; os_version=""; os_name=""; os_pretty=""
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release
  os_id="${ID:-}"
  os_version="${VERSION_ID:-}"
  os_name="${NAME:-}"
  os_pretty="${PRETTY_NAME:-}"
fi
J_OS_ID="$os_id"; J_OS_VERSION="$os_version"; J_OS_PRETTY="$os_pretty"

# EL6 has no /etc/os-release; key it off /etc/redhat-release so an
# unrecognised OS never reaches the changelog-only verdict path.
os_key="${os_id}:${os_version}"
if [ -z "$os_id" ] && grep -qsE 'release 6([^0-9]|$)' /etc/redhat-release; then
  os_key="el6:6"
  os_pretty="$(head -n 1 /etc/redhat-release 2>/dev/null || true)"
fi

running_kernel_full="$(uname -r)"
running_kernel="$(strip_arch "$running_kernel_full")"
J_RUNNING_KERNEL_FULL="$running_kernel_full"
J_RUNNING_KERNEL="$running_kernel"
dbg "os_key=$os_key uname=$running_kernel_full stripped=$running_kernel"

# CloudLinux rebuilds carry a .lve.N. infix (e.g. 4.18.0-553.177.1.lve.1.el7h).
kernel_track="unknown"
case "$running_kernel_full" in
  *-linode*)            kernel_track="linode" ;;
  *.lve.el7h*|*.lve.[0-9]*.el7h*) kernel_track="cloudlinux-7h" ;;
  *.lve.el8*|*.lve.el9*|*.lve.el10*|*.lve.[0-9]*.el8*|*.lve.[0-9]*.el9*|*.lve.[0-9]*.el10*) kernel_track="cloudlinux" ;;
  *.uek.*|*uek*)        kernel_track="oracle-uek" ;;
  *)
    case "$os_id" in
      almalinux)        kernel_track="almalinux" ;;
      almalinux-kitten) kernel_track="almalinux-kitten" ;;
      rocky)            kernel_track="rocky" ;;
      cloudlinux)       kernel_track="cloudlinux" ;;
      ol|oracle*)       kernel_track="oracle-rhck" ;;
      centos|rhel)      kernel_track="$os_id" ;;
    esac
    ;;
esac
dbg "kernel_track=$kernel_track"

section "== CVE =="
say "$CVE / skb_zerocopy() shared-frag kernel local privilege escalation checker"
say "Tool version: $TOOL_VERSION  Mode: read-only"
say "No CVE has been assigned yet; this check uses the ZDI tracking ID."
say "It bypasses the Dirty Frag and Fragnesia fixes: a kernel patched for those can still be vulnerable."

section "== Tool availability =="
print_cmd_status rpm
print_cmd_status dnf
print_cmd_status grubby
print_cmd_status modinfo
print_cmd_status python3
print_cmd_status systemd-detect-virt
print_cmd_status kcarectl
print_cmd_status nmcli

section "== OS =="
if [ -r /etc/os-release ]; then
  say "NAME: $os_name"
  say "PRETTY_NAME: $os_pretty"
  say "ID: $os_id"
  say "VERSION_ID: $os_version"
  say "Kernel track: $kernel_track"
elif [ "$os_key" = "el6:6" ]; then
  say "/etc/os-release not found; /etc/redhat-release: $os_pretty"
else
  say "/etc/os-release not found"
fi

section "== Container / chroot detection =="
container_kind="$(detect_container)"
chroot_state="$(detect_chroot)"
J_CONTAINER_DETECTED="$([ -n "$container_kind" ] && echo true || echo false)"
J_CHROOT_DETECTED="$chroot_state"
if [ -n "$container_kind" ]; then
  say "Container detected: $container_kind"
  say "Note: the kernel version shown below belongs to the host machine, not this container."
  record_reason "Running inside container ($container_kind); kernel verdict applies to host."
else
  say "No container marker detected."
fi
if [ "$chroot_state" = "true" ]; then
  say "Chroot detected (PID 1 root differs from /)."
  record_reason "Chroot detected; package metadata may not reflect the running kernel."
fi

section "== Running kernel =="
say "$running_kernel_full"
say ""
say "This is the kernel your system is currently running."
say "This is what determines whether your system is vulnerable or patched right now."
say "Even if a newer, fixed kernel package is installed, it does not take effect until you reboot."

section "== Installed kernels =="
if has_cmd rpm; then
  installed_kernels="$(rpm -q kernel kernel-core 2>/dev/null | sort -V || true)"
  if [ -n "$installed_kernels" ] && ! printf '%s' "$installed_kernels" | grep -q 'is not installed'; then
    say "$installed_kernels"
  else
    say "No kernel packages found. This system may use a custom or provider-managed kernel."
  fi
else
  say "rpm command not found"
fi

section "== Default boot kernel =="
if has_cmd grubby; then
  grubby --default-kernel 2>/dev/null | while IFS= read -r l; do say "$l"; done || true
else
  say "grubby command not found"
fi

section "== Running kernel RPM mapping =="
if has_cmd rpm; then
  if mapping="$(rpm -q --whatprovides "kernel-uname-r = $running_kernel_full" 2>/dev/null)"; then
    say "$mapping"
    J_KERNEL_RPM="$mapping"
  elif mapping="$(rpm -q "kernel-core-${running_kernel_full}" 2>/dev/null)"; then
    say "$mapping (matched via kernel-core package name)"
    J_KERNEL_RPM="$mapping"
  else
    say "Could not match the running kernel to any installed kernel package."
    record_reason "Running kernel does not map to an installed RPM (provider/custom/Linode kernel?)."
  fi
else
  say "rpm command not found"
fi

# On Rocky, the opt-in `security` repo (disabled by default) carries kernel
# security re-spins for CVEs upstream RHEL is delayed on. Include it in the dnf
# query below without permanently enabling it. Empty on every other distro and
# on Rocky hosts that have not opted into the repo.
dnf_security_enable=""
if [ "$os_id" = "rocky" ] && [ -f /etc/yum.repos.d/rocky-security.repo ]; then
  dnf_security_enable="--enablerepo=security"
fi

section "== Available kernel updates =="
say "(dnf updateinfo is not queried: $CVE has no CVE ID to search for yet.)"
if has_cmd dnf; then
  [ -n "$dnf_security_enable" ] && say "(including the opt-in Rocky 'security' repo)"
  set +e
  # shellcheck disable=SC2086  # single token or empty; intentional word-split
  dnf -q $dnf_security_enable --refresh check-update 'kernel*' >/dev/null 2>&1
  rc=$?
  set -e
  case "$rc" in
    0)   say "All kernel packages are up to date - no updates available." ;;
    100) say "Newer kernel packages are available in the enabled repositories."
         say "Whether $CVE requires updating is shown by the verdict below."
         ;;
    *)   say "dnf check-update encountered an error (exit code $rc)."
         record_reason "dnf check-update returned $rc; cannot confirm update availability."
         ;;
  esac
else
  say "dnf command not found"
fi

section "== ESP module state =="
J_ESP4_STATE="$(detect_esp_module_state esp4)"
J_ESP6_STATE="$(detect_esp_module_state esp6)"
say "esp4:  $J_ESP4_STATE"
say "esp6:  $J_ESP6_STATE"
say ""
say "The exploit needs IPsec ESP (esp4/esp6) to decrypt over the cached file pages."
say "rxrpc is not involved in $CVE."

# [Divergence 4] Both esp4 and esp6 must be blocked, in the same or separate files.
section "== Modprobe blacklist mitigation status =="
mp_esp4="$(modprobe_block_file esp4)"
mp_esp6="$(modprobe_block_file esp6)"
mp_path="$mp_esp4"
[ -n "$mp_esp6" ] && [ "$mp_esp6" != "$mp_esp4" ] && mp_path="${mp_path:+$mp_path,}$mp_esp6"
J_MODPROBE_BLACKLIST_PATH="$mp_path"

loaded_count=0
for s in "$J_ESP4_STATE" "$J_ESP6_STATE"; do
  case "$s" in
    loaded|builtin|builtin-from-config) loaded_count=$((loaded_count + 1)) ;;
  esac
done
blacklisted_count=0
[ -n "$mp_esp4" ] && blacklisted_count=$((blacklisted_count + 1))
[ -n "$mp_esp6" ] && blacklisted_count=$((blacklisted_count + 1))
dbg "blacklist: esp4='$mp_esp4' esp6='$mp_esp6' blacklisted=$blacklisted_count loaded=$loaded_count"

if [ "$blacklisted_count" -eq 2 ] && [ "$loaded_count" -eq 0 ]; then
  say "Mitigation IS active: $mp_path blocks esp4 and esp6, and neither is currently loaded."
  J_MITIGATION_ACTIVE="true"
elif [ "$blacklisted_count" -eq 2 ]; then
  say "Mitigation is PARTIAL: $mp_path blocks esp4 and esp6 but at least one is still loaded."
  say "rmmod the modules or reboot to fully apply the workaround."
  record_action "Reboot or run 'rmmod esp4 esp6' to fully apply $mp_path"
elif [ "$blacklisted_count" -eq 1 ]; then
  if [ -n "$mp_esp4" ]; then
    say "Mitigation is INCOMPLETE: $mp_esp4 blocks esp4 but nothing blocks esp6."
  else
    say "Mitigation is INCOMPLETE: $mp_esp6 blocks esp6 but nothing blocks esp4."
  fi
  say "$CVE requires both esp4 and esp6 to be blocked."
  record_action "Block both esp4 and esp6 in /etc/modprobe.d, then reboot or rmmod esp4 esp6."
else
  say "Mitigation NOT active: no /etc/modprobe.d, /usr/lib/modprobe.d, or /run/modprobe.d file blocks esp4 or esp6."
fi

section "== IPsec gateway detection =="
ipsec_hits="$(detect_ipsec_host)"
J_IPSEC_DETECTION_REASONS="$ipsec_hits"
if [ -n "$ipsec_hits" ]; then
  say "This host appears to USE IPsec — detected: $ipsec_hits"
  say ""
  say "WARNING: applying the modprobe blacklist stopgap on this host WILL BREAK"
  say "your IPsec tunnels. Update the kernel and reboot instead — do NOT run"
  say "the 'install esp4 /bin/false' command on a host that terminates or"
  say "transits IPsec / strongSwan / Libreswan / NetworkManager-VPN traffic."
  J_IPSEC_HOST_DETECTED="true"
  record_reason "IPsec usage detected ($ipsec_hits); modprobe.d stopgap suppressed."
else
  say "No IPsec usage detected (xfrm policies/states empty, no strongswan/libreswan service active, no /etc/ipsec.* files, no NetworkManager VPN profiles)."
  J_IPSEC_HOST_DETECTED="false"
fi

section "== Livepatch tooling status =="
livepatch_tool=""
livepatch_cve_known="unknown"
if has_cmd kcarectl; then
  say "-- KernelCare detected --"
  if has_cmd timeout; then
    timeout 5 kcarectl --info 2>/dev/null | while IFS= read -r l; do say "$l"; done || true
  else
    kcarectl --info 2>/dev/null | while IFS= read -r l; do say "$l"; done || true
  fi
  livepatch_tool="kernelcare"
  set +e
  kcarectl_covers_cve
  rc=$?
  set -e
  case "$rc" in
    0) say "kcarectl reports coverage for $CVE."
       livepatch_cve_known="true"
       ;;
    1) say "kcarectl does not mention $CVE; coverage uncertain - verify with: kcarectl --patch-info | grep -i 33367"
       say "Dirty Frag / Fragnesia livepatches do not cover $CVE."
       livepatch_cve_known="false"
       ;;
    *) livepatch_cve_known="unknown" ;;
  esac
else
  say "KernelCare kcarectl not found"
fi

if kpatch_present; then
  say "-- kpatch packages detected --"
  rpm -qa 'kpatch*' 2>/dev/null | sort -V | while IFS= read -r l; do say "$l"; done || true
  [ -z "$livepatch_tool" ] && livepatch_tool="kpatch"
else
  say "kpatch packages not detected"
fi
J_LIVEPATCH_TOOL="$livepatch_tool"
J_LIVEPATCH_CVE_COVERED="$livepatch_cve_known"

# UEK detection
uek_detected="$(detect_uek "$running_kernel_full")"
J_UEK_DETECTED="$uek_detected"

# ----- vendor routing -----
# [Divergence 5] vendor_label set = known affected distro. fixed_kernel is
# set only where the vendor itself published a fixed NVR for its own builds
# (AlmaLinux 8/9/10, CloudLinux 7h/8); everywhere else the changelog decides.

fixed_kernel=""
vendor_label=""
pending_vendor_nvr="false"

if [ -n "$container_kind" ] || [ "$chroot_state" = "true" ]; then
  dbg "Skipping vendor routing due to container/chroot."
elif [ "$uek_detected" = "true" ]; then
  dbg "UEK detected; refusing to compare against RHCK baselines."
else
  case "$os_key" in
    centos:7*|rhel:7*|centos:6*|rhel:6*|el6:*)
      # Only the stock 2.6.32 / 3.10 kernels are known unaffected. An ELRepo
      # kernel-ml/lt or self-built kernel leaves vendor_label empty, so the
      # unknown-OS path below decides from the changelog alone.
      case "$running_kernel" in
        2.6.32-*) vendor_label="EL6 (stock 2.6.32 kernel)" ;;
        3.10.0-*) vendor_label="${os_id} 7 (stock 3.10 kernel)" ;;
      esac
      ;;
    almalinux:8*)            fixed_kernel="4.18.0-553.177.2.el8_10";  vendor_label="AlmaLinux 8" ;;
    almalinux:9*)            fixed_kernel="5.14.0-687.59.2.el9_8";    vendor_label="AlmaLinux 9" ;;
    almalinux:10*)           fixed_kernel="6.12.0-211.68.2.el10_2";   vendor_label="AlmaLinux 10" ;;
    almalinux-kitten:10*)    vendor_label="AlmaLinux Kitten 10"; pending_vendor_nvr="true" ;;
    rocky:8*|rocky:9*|rocky:10*)
      vendor_label="Rocky Linux ${os_version%%.*}"; pending_vendor_nvr="true" ;;
    cloudlinux:7*)
      if [ "$kernel_track" = "cloudlinux-7h" ]; then
        fixed_kernel="4.18.0-553.177.1.lve.1.el7h"
        vendor_label="CloudLinux 7h"
      else
        vendor_label="CloudLinux 7 (not affected per CloudLinux advisory)"
      fi
      ;;
    cloudlinux:8*)           fixed_kernel="4.18.0-553.177.1.lve.1.el8"; vendor_label="CloudLinux 8" ;;
    cloudlinux:9*|cloudlinux:10*)
      vendor_label="CloudLinux ${os_version%%.*}"; pending_vendor_nvr="true" ;;
    ol:8*|ol:9*|ol:10*)
      vendor_label="Oracle Linux ${os_version%%.*} RHCK"; pending_vendor_nvr="true" ;;
    rhel:8*|rhel:9*|rhel:10*)
      vendor_label="RHEL ${os_version%%.*}"; pending_vendor_nvr="true" ;;
  esac
fi

J_VENDOR_BASELINE="$fixed_kernel"
J_VENDOR_BASELINE_DETECTED="$vendor_label"
J_PENDING_VENDOR_NVR="$pending_vendor_nvr"

section "== Vendor running-kernel assessment =="
if [ -n "$container_kind" ]; then
  say "Skipped: container detected ($container_kind)."
  upgrade_status "indeterminate" "low"
elif [ "$chroot_state" = "true" ]; then
  say "Skipped: chroot detected."
  upgrade_status "indeterminate" "low"
elif [ "$uek_detected" = "true" ]; then
  say "Detected Oracle Unbreakable Enterprise Kernel (UEK): $running_kernel_full"
  say "UEK uses different version numbering than standard RHEL/AlmaLinux/Rocky kernels."
  say "Check Oracle's ELSA security advisories instead."
  record_reason "UEK kernel detected; no UEK status known for $CVE."
  upgrade_status "indeterminate" "low"
else
  case "$os_key" in
    centos:7*|rhel:7*|centos:6*|rhel:6*|el6:*)
      if [ -n "$vendor_label" ]; then
        say "Detected OS:     ${os_pretty:-$os_key}"
        say "Vendor branch:   $vendor_label"
        say "Running kernel:  $running_kernel_full"
        say ""
        say "RESULT: stock EL6 / EL7 kernels are NOT AFFECTED by $CVE."
        say "The EL7 3.10 ESP code always copies nonlinear packets before decrypting,"
        say "and the EL6 2.6.32 kernel predates skb_zerocopy()."
        say ""
        say "Confidence: MEDIUM. This is TuxCare's analysis; Red Hat has not published"
        say "an EL6/EL7 status. If Red Hat later publishes a contradicting status,"
        say "this verdict should be downgraded to 'indeterminate' until vendor confirms."
        record_reason "EL6/EL7 NOT_AFFECTED per TuxCare code-path analysis; not vendor-confirmed by Red Hat."
        J_COMPARISON_RESULT="not_affected"
        upgrade_status "not_affected" "medium"
      fi
      ;;
    cloudlinux:7*)
      if [ "$kernel_track" != "cloudlinux-7h" ]; then
        say "Detected OS:     ${os_pretty:-CloudLinux 7}"
        say "Vendor branch:   $vendor_label"
        say "Running kernel:  $running_kernel_full"
        say ""
        say "RESULT: CloudLinux 7 is NOT AFFECTED per CloudLinux advisory."
        record_reason "CloudLinux advisory: the CL7 3.10 kernel never decrypts in place."
        J_COMPARISON_RESULT="not_affected"
        upgrade_status "not_affected" "high"
      fi
      ;;
  esac
  if [ -z "$J_COMPARISON_RESULT" ]; then
    changelog_state="$(running_kernel_changelog_state)"
    case "$changelog_state" in
      fix)   J_CHANGELOG_CONFIRMS_FIX="true" ;;
      nofix) J_CHANGELOG_CONFIRMS_FIX="false" ;;
    esac
    baseline_ok="false"
    if [ -n "$fixed_kernel" ] && dist_tag_compatible "$running_kernel" "$fixed_kernel"; then
      baseline_ok="true"
    fi
    # A missing changelog line only proves "unfixed" for a distro-built
    # kernel (.elN[_M], CL7h h, CIQ _ciq, Rocky security .0.N re-spin), not
    # for ELRepo kernel-ml/lt or self-built kernels.
    vendor_kernel="false"
    if printf '%s' "$running_kernel" | grep -qE '\.el[0-9]+(_[0-9]+)?(h)?(_ciq)?(\.0\.[0-9]+)?$'; then
      vendor_kernel="true"
    fi
    dbg "changelog_state=$changelog_state baseline_ok=$baseline_ok vendor_kernel=$vendor_kernel vendor_label=$vendor_label"
    say "Detected OS:     ${os_pretty:-${os_name:-unknown} ${os_version:-unknown}}"
    say "Vendor branch:   ${vendor_label:-unknown}"
    say "Running kernel:  $running_kernel"
    say "Fixed baseline:  ${fixed_kernel:-none published for this vendor}"
    say "Changelog:       $changelog_state (looking for '$FIX_MARKER')"
    say ""
    if [ "$changelog_state" = "fix" ]; then
      say "RESULT: the running kernel's RPM changelog lists the $CVE fix."
      say "Your system appears PATCHED for $CVE. No action needed."
      J_COMPARISON_RESULT="patched-changelog-confirmed"
      upgrade_status "patched" "high"
    elif [ -z "$vendor_label" ]; then
      say "This script does not know whether this OS and kernel are affected:"
      say "  ${os_id:-unknown} ${os_version:-unknown}, kernel $running_kernel"
      say "You can check the running kernel's changelog manually with:"
      say "  rpm -q --changelog kernel-core-\$(uname -r) | grep -i 'skb_zerocopy'"
      record_reason "Unrecognised OS or non-stock kernel; the running kernel changelog did not confirm the $CVE fix."
      J_COMPARISON_RESULT="no-baseline"
      upgrade_status "indeterminate" "low"
    elif [ "$vendor_kernel" = "false" ]; then
      say "RESULT: the running kernel is not a $vendor_label distro kernel (for example an"
      say "ELRepo kernel-ml/lt or self-built kernel), and its changelog does not list the"
      say "$CVE fix. Check that kernel's source or provider for the upstream fix:"
      say "  net: skbuff: $FIX_MARKER()"
      record_reason "Non-distro kernel ($running_kernel); changelog did not confirm the $CVE fix."
      J_COMPARISON_RESULT="custom-kernel"
      upgrade_status "indeterminate" "low"
    elif [ "$changelog_state" = "nofix" ]; then
      # Authoritative even when ver_ge says otherwise: sort -V ranks a GA
      # kernel such as 4.18.0-553.el8_10 above 4.18.0-553.177.2.el8_10.
      say "RESULT: the running kernel's RPM changelog does not list the $CVE fix."
      say "Your system appears VULNERABLE to $CVE."
      J_COMPARISON_RESULT="vulnerable-changelog"
      upgrade_status "vulnerable" "high"
    elif [ "$baseline_ok" = "true" ]; then
      say "The running kernel's changelog could not be read; comparing versions instead."
      if ver_ge "$running_kernel" "$fixed_kernel"; then
        say "RESULT: Your running kernel is at or above the fixed version for $vendor_label."
        say "Your system appears PATCHED for $CVE."
        J_COMPARISON_RESULT="patched"
        upgrade_status "patched" "medium"
      else
        say "RESULT: Your running kernel is OLDER than the fixed version for $vendor_label."
        say "Your system appears VULNERABLE to $CVE."
        J_COMPARISON_RESULT="vulnerable"
        upgrade_status "vulnerable" "high"
      fi
    else
      say "RESULT: the running kernel's changelog could not be read, and there is no"
      say "comparable $vendor_label fixed version to fall back on."
      record_reason "Changelog unavailable and no comparable vendor baseline for $vendor_label."
      J_COMPARISON_RESULT="refused-cross-vendor"
      upgrade_status "indeterminate" "low"
    fi
    if [ "$FINAL_STATUS" = "vulnerable" ]; then
      if [ "$pending_vendor_nvr" = "true" ]; then
        say "$vendor_label: no fixed kernel version was known to this script as of $BASELINE_VERIFIED_DATE."
        if [ "$J_MITIGATION_ACTIVE" = "true" ]; then
          say "The esp4/esp6 modprobe.d workaround is active; keep it until your vendor ships a fixed kernel."
          record_action "Keep the esp4/esp6 modprobe.d workaround until $vendor_label ships a fixed kernel; then dnf upgrade and reboot."
        else
          say "If dnf has nothing to install, use the modprobe.d workaround below until your vendor ships one."
          record_action "Apply the esp4/esp6 modprobe.d workaround until $vendor_label ships a fixed kernel; then dnf upgrade and reboot."
        fi
      else
        say "Update and reboot as soon as possible."
        record_action "Run: dnf clean metadata && dnf --refresh upgrade && reboot"
      fi
    fi
  fi
fi

# ----- Linode/Akamai assessment -----

section "== Linode/Akamai Cloud kernel assessment =="
case "$running_kernel_full" in
  *linode*)
    J_LINODE_KERNEL="$running_kernel_full"
    say "Detected running Linode-provided kernel: $running_kernel_full"
    say ""
    say "No fixed Linode kernel for $CVE is known to this script as of $BASELINE_VERIFIED_DATE."
    say "Running 'dnf upgrade' inside the OS will NOT change a Linode-provided kernel."
    say "Watch the Linode Cloud Manager for an updated kernel; switching to GRUB 2"
    say "lets you manage the kernel via dnf instead."
    record_reason "Linode kernel detected; no known Linode fix for $CVE."
    record_action "Watch Linode Cloud Manager > Configuration > Kernel for the $CVE fix; or switch to GRUB 2."
    upgrade_status "indeterminate" "low"
    ;;
  *)
    say "No Linode-provided kernel marker detected in uname -r."
    ;;
esac

# ----- Generic provider warning -----
section "== Provider/custom kernel warning =="
case "$running_kernel_full" in
  *linode*)
    : ;;
  *aws*|*azure*|*gcp*|*ovh*|*cloud*|*xen*|*virt*|*uek*)
    say "Your kernel name suggests it is managed by a cloud/hosting provider: $running_kernel_full"
    say "Standard dnf updates may not affect the running kernel. Check with your provider for updates."
    record_reason "Running kernel name suggests provider/custom kernel."
    [ "$FINAL_STATUS" = "patched" ] && upgrade_status "indeterminate" "medium"
    ;;
  *)
    say "No obvious provider/custom kernel marker detected in uname -r." ;;
esac

# ----- Livepatch override -----
# [Divergence 7] Assign directly: upgrade_status() keeps "vulnerable" sticky,
# which would ignore confirmed livepatch coverage.
if [ "$livepatch_cve_known" = "true" ] && [ "$FINAL_STATUS" != "patched" ] && [ "$FINAL_STATUS" != "not_affected" ]; then
  say ""
  say "Your livepatch service reports that $CVE is covered. Marking system as patched."
  FINAL_STATUS="patched"
  FINAL_CONFIDENCE="medium"
fi
if [ -n "$livepatch_tool" ] && [ "$livepatch_cve_known" = "false" ] && [ "$FINAL_STATUS" = "vulnerable" ]; then
  say ""
  say "Livepatch tools are installed but could not confirm $CVE coverage."
  FINAL_STATUS="indeterminate"
  FINAL_CONFIDENCE="low"
  record_reason "Livepatch tool $livepatch_tool present but $CVE coverage not confirmed."
fi

# ----- Suggested next actions -----
# [Divergence 6] esp4/esp6-only workaround file; recognises an existing
# dirtyfrag.conf. No AlmaLinux testing-repo hint: AlmaLinux's fix reached the
# production repos on 2026-10-28, so a vulnerable AlmaLinux host only needs
# a normal dnf upgrade.
section "== Suggested next actions =="
say "1. UPDATE AND REBOOT (recommended fix):"
say "   Install all available updates and restart the system:"
say "     dnf clean metadata"
say "     dnf --refresh upgrade"
say "     reboot"
if [ "$pending_vendor_nvr" = "true" ] && [ "$FINAL_STATUS" = "vulnerable" ]; then
say "   No fixed $vendor_label kernel version was known to this script as of"
say "   $BASELINE_VERIFIED_DATE. If dnf has nothing to install, use the workaround in"
say "   step 3 until your vendor ships one."
fi
say "   If you use CloudLinux/KernelCare/livepatch, also confirm with your vendor that"
say "   their livepatch covers $CVE (kcarectl --patch-info | grep -i 33367)."
say ""
say "2. VERIFY AFTER REBOOT:"
say "   After restarting, confirm the new kernel is running:"
say "     uname -r"
say "   Then re-run this script to verify the system now shows as patched."
say ""
if [ "$J_IPSEC_HOST_DETECTED" = "true" ]; then
say "3. TEMPORARY WORKAROUND — DO NOT APPLY ON THIS HOST:"
say "   The modprobe.d stopgap (install esp4 /bin/false, etc.) WILL BREAK YOUR"
say "   IPsec tunnels. This host appears to use IPsec (detected: $J_IPSEC_DETECTION_REASONS)."
say "   Apply the kernel update and reboot — do NOT run the modprobe blacklist."
elif [ "$J_MITIGATION_ACTIVE" = "true" ]; then
say "3. TEMPORARY WORKAROUND (already applied):"
say "   Your system already has a modprobe.d block covering esp4 and esp6"
say "   ($mp_path). No additional workaround needed."
say "   A Dirty Frag dirtyfrag.conf that blocks esp4 and esp6 also covers $CVE."
else
say "3. TEMPORARY WORKAROUND (only if you are NOT using IPsec):"
say "   Block the ESP modules the exploit needs without updating the kernel:"
say "     printf 'install esp4 /bin/false\\ninstall esp6 /bin/false\\n' \\"
say "       > /etc/modprobe.d/zdi-can-33367.conf"
say "     rmmod esp4 esp6 2>/dev/null || true"
say "     echo 3 > /proc/sys/vm/drop_caches"
say "   Note: this BREAKS IPsec on hosts that terminate or transit IPsec tunnels."
say "   WireGuard does not use these modules."
say "   Reverse with: rm /etc/modprobe.d/zdi-can-33367.conf"
fi
say ""
say "4. CLEAN UP WORKAROUND (after applying the kernel update):"
say "   Once you have rebooted into a patched kernel, remove the workaround file"
say "   you created, for example:"
say "     rm /etc/modprobe.d/zdi-can-33367.conf"
say "   The modules load again on demand when IPsec needs them."
say ""
say "5. LINODE/AKAMAI CLOUD USERS:"
say "   If your kernel name contains 'linode', 'dnf upgrade' inside the OS will NOT change the running"
say "   kernel. Linode manages the kernel separately. Watch the Linode Cloud Manager for the $CVE"
say "   fix, or switch to GRUB 2 to manage the kernel via dnf."

# Rocky Linux ships a narrower, emergency-only `security` repo (introduced
# 2026-05-14, disabled by default) for CVEs where upstream RHEL is delayed
# or has declined to fix. Gated on ROCKY_SECURITY_REPO_HAS_FIX, which stays
# "false" until the repo is confirmed to carry this fix.
if [ "$FINAL_STATUS" = "vulnerable" ] && \
   [ "$os_id" = "rocky" ] && \
   [ "$ROCKY_SECURITY_REPO_HAS_FIX" = "true" ] && \
   [ -f /etc/yum.repos.d/rocky-security.repo ]; then
  say ""
  say "6. ROCKY LINUX SECURITY REPO (if 'dnf upgrade' reports 'Nothing to do'):"
  say "   Rocky Linux ships an opt-in 'security' repo (disabled by default,"
  say "   introduced 2026-05-14) for CVEs where upstream RHEL is delayed or"
  say "   has declined to fix. $CVE is covered by this repo. To install:"
  say "     sudo dnf --enablerepo=security --refresh upgrade 'kernel*'"
  say "     reboot"
  say "   The 'security' repo remains disabled after the run — no cleanup"
  say "   needed. The fix package is explicitly versioned to be superseded"
  say "   by the next upstream Rocky release."
  say "   See https://rockylinux.org/news/2026-05-14-introducing-security-repository"
fi

# ----- Final status -----
section "== Final status =="
case "$FINAL_STATUS" in
  patched|not_affected) exit_code=0 ;;
  vulnerable)           exit_code=1 ;;
  indeterminate)        exit_code=3 ;;
  *)                    exit_code=3 ;;
esac

if [ "$JSON" -eq 0 ]; then
  printf 'STATUS: %s  CONFIDENCE: %s  EXIT: %d\n' "$FINAL_STATUS" "$FINAL_CONFIDENCE" "$exit_code"
  say ""
  case "$FINAL_STATUS" in
    patched)
      say "Your system appears to be protected against $CVE." ;;
    not_affected)
      say "Your system is NOT AFFECTED by $CVE." ;;
    vulnerable)
      say "Your system appears to be VULNERABLE to $CVE."
      say "Update the kernel and reboot as soon as possible (see suggested actions above)." ;;
    indeterminate)
      say "The script could not determine whether your system is vulnerable or patched."
      say "See the reasons below and check manually." ;;
  esac
  if [ "${#REASONS[@]}" -gt 0 ]; then
    say ""
    say "Reasons:"
    for r in "${REASONS[@]}"; do say "  - $r"; done
  fi
  if [ "${#RECOMMENDED_ACTIONS[@]}" -gt 0 ]; then
    say ""
    say "Recommended actions:"
    for a in "${RECOMMENDED_ACTIONS[@]}"; do say "  - $a"; done
  fi
fi

if [ "$JSON" -eq 1 ]; then
  emit_json "$exit_code"
fi

[ -n "$LOG_FILE" ] && dbg "Final exit code: $exit_code"
exit "$exit_code"
