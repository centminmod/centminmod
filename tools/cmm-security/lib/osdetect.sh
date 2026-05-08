#!/usr/bin/env bash
#
# osdetect.sh — shared OS detection helpers for cmsec
#
# Sourced by tools/cmm-security/cmsec.sh. Exports:
#
#   CMSEC_OS_ID         lowercase distro id (almalinux, rocky, centos, cloudlinux, ol, rhel, ...)
#   CMSEC_OS_VERSION    VERSION_ID from /etc/os-release (e.g. "7", "8.10", "9.7", "10")
#   CMSEC_OS_MAJOR      first numeric component of CMSEC_OS_VERSION (e.g. "8", "9", "10")
#   CMSEC_OS_PRETTY     PRETTY_NAME from /etc/os-release
#   CMSEC_KERNEL        running kernel string (uname -r)
#   CMSEC_KERNEL_TRACK  one of: linode, almalinux, rocky, cloudlinux, oracle-rhck, oracle-uek,
#                       cloudlinux-7h, custom, unknown
#   CMSEC_OS_EOL        true if distro major is past EOL (EL6 currently); else false
#   CMSEC_OS_EOL_DATE   ISO date when EOL took effect (empty if not EOL)
#   CMSEC_IN_CONTAINER  true if running inside a container or chroot, else false
#
# Returns 0 on success. Never exits — must be safe to source from any caller.
#

# ----- /etc/os-release -----
CMSEC_OS_ID=""
CMSEC_OS_VERSION=""
CMSEC_OS_MAJOR=""
CMSEC_OS_PRETTY=""
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release
  CMSEC_OS_ID="${ID:-}"
  CMSEC_OS_VERSION="${VERSION_ID:-}"
  CMSEC_OS_PRETTY="${PRETTY_NAME:-}"
fi
CMSEC_OS_MAJOR="$(printf '%s' "$CMSEC_OS_VERSION" | awk -F. '{print $1}')"

# ----- running kernel -----
CMSEC_KERNEL="$(uname -r 2>/dev/null || echo unknown)"

# ----- kernel track classification -----
# Determines which vendor baseline path applies. Per-CVE check scripts use
# their own internal logic for the verdict; this is purely a label for
# display and JSON emission.
cmsec_classify_kernel_track() {
  local k="$CMSEC_KERNEL"
  case "$k" in
    *-linode*)            CMSEC_KERNEL_TRACK="linode" ;;
    *.lve.el7h*)          CMSEC_KERNEL_TRACK="cloudlinux-7h" ;;
    *.lve.el8*|*.lve.el9*|*.lve.el10*) CMSEC_KERNEL_TRACK="cloudlinux" ;;
    *.uek.*|*uek*)        CMSEC_KERNEL_TRACK="oracle-uek" ;;
    *)
      case "$CMSEC_OS_ID" in
        almalinux)        CMSEC_KERNEL_TRACK="almalinux" ;;
        rocky)            CMSEC_KERNEL_TRACK="rocky" ;;
        cloudlinux)       CMSEC_KERNEL_TRACK="cloudlinux" ;;
        ol|oracle*)       CMSEC_KERNEL_TRACK="oracle-rhck" ;;
        centos|rhel)      CMSEC_KERNEL_TRACK="$CMSEC_OS_ID" ;;
        *)                CMSEC_KERNEL_TRACK="unknown" ;;
      esac
      ;;
  esac
}
cmsec_classify_kernel_track

# ----- distro EOL flagging (informational — does NOT modify per-CVE verdict) -----
# Only flags major versions known to be past upstream EOL on 2026-05-08. Per-CVE
# scripts decide whether their kernel range applies; this annotation is shown
# alongside the verdict.
CMSEC_OS_EOL="false"
CMSEC_OS_EOL_DATE=""
case "$CMSEC_OS_ID:$CMSEC_OS_MAJOR" in
  centos:6)        CMSEC_OS_EOL="true"; CMSEC_OS_EOL_DATE="2020-11-30" ;;
  centos:7)        CMSEC_OS_EOL="true"; CMSEC_OS_EOL_DATE="2024-06-30" ;;
  # Note: stock CentOS 7 EOL but CloudLinux 7h, ELS, and other extended-support
  # variants may still receive backported kernel patches — caller should display
  # informational-only.
esac

# ----- container / chroot detection -----
CMSEC_IN_CONTAINER="false"
if [ -f /.dockerenv ]; then
  CMSEC_IN_CONTAINER="true"
elif [ -f /run/.containerenv ]; then
  CMSEC_IN_CONTAINER="true"
elif [ -r /proc/1/cgroup ] && grep -qE '/(docker|lxc|containerd|kubepods|podman)' /proc/1/cgroup 2>/dev/null; then
  CMSEC_IN_CONTAINER="true"
elif command -v systemd-detect-virt >/dev/null 2>&1; then
  v="$(systemd-detect-virt 2>/dev/null || true)"
  case "$v" in
    docker|lxc|lxc-libvirt|systemd-nspawn|podman|rkt|wsl|kubernetes|container-other) CMSEC_IN_CONTAINER="true" ;;
  esac
fi
# chroot detection: process-1 root inode != current root inode -> chroot.
# Must use stat -L to dereference the /proc/1/root symlink — without -L, GNU
# stat reports the symlink's procfs synthetic inode (always different from /'s
# inode on a normal host), causing a false-positive container/chroot verdict
# on every Linux system.
# Both inodes must be non-zero (real-FS) AND different to flag chroot. The "0"
# fallback denotes a stat failure (perm-denied, hardened kernel, /proc weird
# state) — treating it as a "different inode" would false-positive on hosts
# where /proc/1/root is unreadable but / is fine.
if [ -r /proc/1/root ] && [ -d / ]; then
  _cmsec_root_inode="$(stat -L -c %i / 2>/dev/null || echo 0)"
  _cmsec_init_root_inode="$(stat -L -c %i /proc/1/root 2>/dev/null || echo 0)"
  if [ "$_cmsec_root_inode" != "0" ] \
     && [ "$_cmsec_init_root_inode" != "0" ] \
     && [ "$_cmsec_init_root_inode" != "$_cmsec_root_inode" ]; then
    CMSEC_IN_CONTAINER="true"
  fi
  unset _cmsec_root_inode _cmsec_init_root_inode
fi

export CMSEC_OS_ID CMSEC_OS_VERSION CMSEC_OS_MAJOR CMSEC_OS_PRETTY \
       CMSEC_KERNEL CMSEC_KERNEL_TRACK CMSEC_OS_EOL CMSEC_OS_EOL_DATE \
       CMSEC_IN_CONTAINER
