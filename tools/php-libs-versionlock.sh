#!/bin/bash
###########################################################
# php-libs-versionlock.sh
#
# Centmin Mod compiles PHP from source, so the RPM database does not
# know which shared libraries PHP was linked against. When a non-base
# repo (EPEL, Remi etc) ships an update that changes a library soname,
# a plain dnf update removes the old .so file and php-fpm fails on its
# next restart or reboot. Example: EPEL 9 libavif 0.11.1 -> 1.1.1
# replaced libavif.so.15 with libavif.so.16.
#
# This script versionlocks the non-base (not from the OS vendor)
# packages owning the libraries php-fpm, php and PHP extensions link
# against, plus their sibling subpackages (-devel etc) so dnf updates
# leave them alone. centmin.sh menu option 5 runs 'update' before
# rebuilding PHP and 'lock' after a successful rebuild.
#
# EL8+ only: EPEL and Remi EL7 repos are frozen, so EL7 has nothing
# to lock and only 'check' does anything there.
#
# usage:
#   php-libs-versionlock.sh lock    lock the currently installed versions
#   php-libs-versionlock.sh unlock  remove this script's locks
#   php-libs-versionlock.sh update  update this script's locked packages
#   php-libs-versionlock.sh status  show locked packages with held updates
#   php-libs-versionlock.sh check   report missing PHP shared libraries
#   php-libs-versionlock.sh list    show the packages that would be locked
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
LOCK_BEGIN='# centminmod php-libs-versionlock begin'
LOCK_END='# centminmod php-libs-versionlock end'
LOCK_CONF='/etc/dnf/plugins/versionlock.conf'
LOCK_MUTEX='/var/lock/centminmod-php-libs-versionlock.lock'
PHP_BINARIES='/usr/local/sbin/php-fpm /usr/local/bin/php'
# PHP extensions whose libraries other menu options version lock
# (imagick.so: ImageMagick locks managed by centmin.sh menu option 15)
SKIP_EXTENSIONS='imagick.so'

lock_file() {
  local f
  if [[ -f "$LOCK_CONF" ]]; then
    f=$(awk -F= '/^[[:space:]]*locklist[[:space:]]*=/ {gsub(/[[:space:]]/, "", $2); print $2; exit}' "$LOCK_CONF" 2>/dev/null) || return $?
  fi
  echo "${f:-/etc/dnf/plugins/versionlock.list}"
}

ensure_plugin() {
  if [[ ! -f "$LOCK_CONF" ]]; then
    dnf -y -q install python3-dnf-plugin-versionlock >/dev/null 2>&1
  fi
  [[ -f "$LOCK_CONF" ]]
}

php_targets() {
  local f extdir installed
  for f in $PHP_BINARIES; do
    if [[ -x "$f" ]]; then
      echo "$f"
      installed=y
    fi
  done
  if [[ -x /usr/local/bin/php-config ]]; then
    extdir=$(/usr/local/bin/php-config --extension-dir 2>/dev/null) || return $?
  elif [[ "$installed" == y ]]; then
    echo "error: installed PHP has no executable php-config; cannot discover its extensions" >&2
    return 127
  fi
  if [[ -d "$extdir" ]]; then
    for f in "$extdir"/*.so; do
      [[ -f "$f" ]] || continue
      [[ " $SKIP_EXTENSIONS " = *" ${f##*/} "* ]] && continue
      echo "$f"
    done
  fi
}

php_libs() {
  (
    set -o pipefail
    local f targets dependencies
    targets=$(php_targets) || exit $?
    for f in $targets; do
      dependencies=$(ldd "$f" 2>/dev/null) || exit $?
      awk '$2 == "=>" && $3 ~ /^\// {print $3}' <<< "$dependencies" || exit $?
    done | sort -u
  )
}

# Custom-built libraries and locally created repo files need not belong to RPMs.
# Other RPM query failures must not look like a successful empty discovery.
rpm_file_record() {
  local records status
  records=$(rpm -qf --qf "$1" "$2" 2>/dev/null)
  status=$?
  if [[ "$status" -ne 0 ]]; then
    [[ "$records" == "file $2 is not owned by any package" ]] && return 0
    return "$status"
  fi
  printf '%s\n' "$records"
}

# DNF versionlock delete drops comments, so retain exact ownership separately.
# Without a ledger or markers, existing entries remain foreign: their owner is unknown.
owned_entries() {
  local lf ledger
  lf=$(lock_file) || return $?
  [[ -f "$lf" ]] || return 0
  ledger="${lf}.centminmod-php-libs-owned"
  [[ -f "$ledger" ]] || ledger=/dev/null
  awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '
    FILENAME == ARGV[1] {if (NF) owned[$0] = 1; next}
    $0 == b {s = 1; next} $0 == e {s = 0; next}
    NF && !/^#/ && (s || ($0 in owned)) {print}
  ' "$ledger" "$lf"
}

# lock list entries owned by other menu options or the user
other_locks() {
  local lf owned
  lf=$(lock_file) || return $?
  [[ -f "$lf" ]] || return 0
  owned=$(owned_entries) || return $?
  awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '
    FILENAME == ARGV[1] {if (NF) owned[$0] = 1; next}
    $0 == b {s = 1; next} $0 == e {s = 0; next}
    !s && NF && !/^#/ && !($0 in owned) {print}
  ' <(printf '%s\n' "$owned") "$lf"
}

# lock entries are name-epoch:version-release.*
entry_name() {
  sed -E 's/-[0-9]+:[^:]+$//'
}

block_names() {
  local status
  local -a read_status
  owned_entries | entry_name | sort -u
  read_status=("${PIPESTATUS[@]}")
  for status in "${read_status[@]}"; do
    [[ "$status" -eq 0 ]] || return "$status"
  done
}

# repo ids from repo files shipped by the OS vendor (baseos, appstream, crb
# etc) plus pseudo repos for the OS install and local rpm installs.
# Vendor of individual packages is unreliable (AlmaLinux 8 base packages
# carry vendor CloudLinux), so classify by the repo a package came from.
base_repos() {
  (
    set -o pipefail
    local osvendor vendor f
    osvendor=$(rpm_file_record '%{VENDOR}\n' /etc/redhat-release) || exit $?
    osvendor=${osvendor%%$'\n'*}
    printf '%s\n' anaconda System @System commandline @commandline
    for f in /etc/yum.repos.d/*.repo; do
      [[ -f "$f" ]] || continue
      vendor=$(rpm_file_record '%{VENDOR}' "$f") || exit $?
      if [[ "${f##*/}" = 'redhat.repo' ]] || [[ -n "$osvendor" && "$vendor" = "$osvendor" ]]; then
        awk -F'[][]' '/^\[/ {print $2}' "$f" || exit $?
      fi
    done | sort -u
  )
}

# name<tab>repo id each installed package was installed from
from_repo() {
  [[ $# -eq 0 ]] && return 0
  dnf -q --disablerepo='*' repoquery --installed --qf '%{name}\t%{from_repo}' "$@" 2>/dev/null
}

# installed packages from non-base repos owning PHP's libraries plus their
# sibling subpackages built from the same source rpm (-devel etc), minus
# packages already locked outside this script's block. Local rpm installs
# (Centmin Mod's *-custom packages) are skipped.
collect_packages() {
  (
    set -o pipefail
    local libs lib srpms names installed_records repo_records foreign repos
    libs=$(php_libs) || exit $?
    [[ -z "$libs" ]] && exit 0
    srpms=$(for lib in $libs; do rpm_file_record '%{SOURCERPM}\n' "$lib" || exit $?; done | sort -u) || exit $?
    installed_records=$(rpm -qa --qf '%{SOURCERPM}\t%{NAME}\n') || exit $?
    names=$(awk -F'\t' 'FILENAME == ARGV[1] {if (NF) s[$1] = 1; next} ($1 in s) {print $2}' <(printf '%s\n' "$srpms") <(printf '%s\n' "$installed_records") | sort -u) || exit $?
    [[ -z "$names" ]] && exit 0
    repo_records=$(from_repo $names) || exit $?
    # A foreign exclusion forbids one version; it does not pin the package.
    foreign=$(other_locks | awk '!/^!/' | entry_name) || exit $?
    repos=$(base_repos) || exit $?
    awk -F'\t' '
      FILENAME == ARGV[1] {if (NF) r[$1] = 1; next}
      FILENAME == ARGV[2] {if (NF) foreign[$1] = 1; next}
      $2 != "" && !($2 in r) && !($1 in foreign) {print $1}
    ' <(printf '%s\n' "$repos") <(printf '%s\n' "$foreign") <(printf '%s\n' "$repo_records") | sort -u
  )
}

write_block() {
  local lf ledger tmp records pending status generated foreign
  lf=$(lock_file) || return $?
  ledger="${lf}.centminmod-php-libs-owned"
  [[ -e "$lf" ]] || touch "$lf" || return $?
  # Replace the symlink target, preserving the configured locklist symlink.
  lf=$(readlink -f -- "$lf") || return $?
  tmp=$(mktemp "${lf}.XXXXXX") || return $?
  records=$(mktemp "${ledger}.XXXXXX") || { status=$?; rm -f -- "$tmp"; return "$status"; }
  pending=$(mktemp "${ledger}.XXXXXX") || { status=$?; rm -f -- "$tmp" "$records"; return "$status"; }
  (
    cp --preserve=all -- "$lf" "$tmp" || exit $?
    if [[ -f "$ledger" ]]; then
      cp --preserve=all -- "$ledger" "$records" || exit $?
      cp --preserve=all -- "$ledger" "$pending" || exit $?
    fi
    : > "$records" || exit $?
    owned_entries > "$pending" || exit $?
    awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '
      FILENAME == ARGV[1] {if (NF) owned[$0] = 1; next}
      $0 == b {s = 1; next} $0 == e {s = 0; next}
      !s && !($0 in owned) {print}
    ' "$pending" "$lf" > "$tmp" || exit $?
    if [[ $# -gt 0 ]]; then
      generated=$(rpm -q --qf '%{NAME}-%{EPOCHNUM}:%{VERSION}-%{RELEASE}.*\n' "$@") || exit $?
      foreign=$(other_locks) || exit $?
      # A saved update name may now share an exact NEVRA with a user's pin.
      # Leave that foreign record in place and do not claim its ownership.
      awk 'FILENAME == ARGV[1] {if (NF) foreign[$0] = 1; next} NF && !($0 in foreign)' \
        <(printf '%s\n' "$foreign") <(printf '%s\n' "$generated") > "$records" || exit $?
      {
        echo "$LOCK_BEGIN" &&
        echo "# $(date -u +%F' '%T) UTC packages linked by source compiled PHP, centmin.sh menu 5 updates them" &&
        cat "$records" &&
        echo "$LOCK_END"
      } >> "$tmp" || exit $?
    fi
    # Publish old+new ownership first so either locklist generation survives
    # an interrupted commit. A retry intersects this ledger with live entries.
    cat "$records" >> "$pending" || exit $?
    mv -f -- "$pending" "$ledger" || exit $?
    mv -f -- "$tmp" "$lf" || exit $?
    mv -f -- "$records" "$ledger"
  )
  status=$?
  rm -f -- "$tmp" "$records" "$pending"
  return "$status"
}

do_lock() {
  local pkgs
  ensure_plugin || { echo "error: python3-dnf-plugin-versionlock not installed, PHP libraries not locked" >&2; return 1; }
  do_check lock >/dev/null || return $?
  pkgs=$(collect_packages) || return $?
  write_block $pkgs || return $?
  echo "php-libs-versionlock: locked $(echo $pkgs | wc -w) packages in $(lock_file)"
  [[ -n "$pkgs" ]] && echo "  $(echo $pkgs)"
  return 0
}

do_unlock() {
  write_block || return $?
  echo "php-libs-versionlock: removed locks from $(lock_file)"
}

# Use DNF's own interpreter and effective configuration (including custom
# pluginconfpath settings). Its plugin reader merges directories in order.
dnf_plugin_paths() {
  local executable interpreter expected
  local -a python_command
  executable=$(command -v dnf) || return $?
  IFS= read -r interpreter < "$executable" || return $?
  interpreter=${interpreter#\#!}
  read -r -a python_command <<< "$interpreter"
  [[ "${python_command[0]}" == /* && -x "${python_command[0]}" ]] || { echo "error: cannot determine DNF's Python interpreter" >&2; return 1; }
  expected=$(lock_file) || return $?
  "${python_command[@]}" -c '
import dnf, os, sys
class VersionlockConfig(dnf.Plugin):
    name = "versionlock"
base = dnf.Base()
base.conf.read()
config = VersionlockConfig.read_config(base.conf)
if not config.has_section("main") or not config.has_option("main", "locklist"):
    sys.exit("error: DNF has no configured versionlock locklist")
active = config.get("main", "locklist")
if not active or os.path.realpath(active) != os.path.realpath(sys.argv[1]):
    sys.exit("error: DNF uses a different versionlock locklist; PHP libraries were not updated")
print(",".join(base.conf.pluginconfpath))
' "$expected"
}

do_update() {
  (
    local names paths foreign overlay lock_status status=0
    names=$(block_names) || exit $?
    [[ -z "$names" ]] && exit 0
    paths=$(dnf_plugin_paths) || exit $?
    foreign=$(other_locks) || exit $?
    overlay=$(mktemp -d) || exit $?
    trap 'rm -rf -- "$overlay"' EXIT
    trap 'exit 130' INT
    trap 'exit 143' TERM
    printf '%s\n' "$foreign" > "$overlay/locks.list" || exit $?
    printf '%s\n' '[main]' "locklist=$overlay/locks.list" > "$overlay/versionlock.conf" || exit $?
    # Establish recoverable ownership and check publication before updating.
    # The global pins remain active even if this process or DNF is killed.
    write_block $names || exit $?
    echo "dnf -y update $(echo $names)"
    # Only this invocation bypasses our own pins. Foreign pins/exclusions and
    # every other plugin's effective settings remain in force.
    dnf -y "--setopt=pluginconfpath=${paths:+$paths,}$overlay" update $names || status=$?
    # Keep the saved names protected even if PHP rebuilding aborts. A partial
    # update is locked at its installed versions rather than rolled back.
    write_block $names || {
      lock_status=$?
      echo "error: failed to restore PHP library version locks after dnf update" >&2
      [[ "$status" -ne 0 ]] && exit "$status"
      exit "$lock_status"
    }
    exit "$status"
  )
}

sonames() {
  (
    set -o pipefail
    awk 'match($0, /^[^ (]+\.so[^ (]*/) {print substr($0, RSTART, RLENGTH)}' | sort -u
  )
}

do_status() {
  local names name newver gone updates old_provides new_provides old_sonames new_sonames installed
  names=$(block_names) || return $?
  if [[ -z "$names" ]]; then
    echo "php-libs-versionlock: no PHP library locks in $(lock_file)"
    return 0
  fi
  echo "php-libs-versionlock: $(echo $names | wc -w) locked packages: $(echo $names)"
  updates=$(dnf -q list updates --disableplugin=versionlock $names 2>/dev/null) || return $?
  updates=$(awk 'NF == 3 && $2 ~ /^[0-9]/ {print $1, $2}' <<< "$updates") || return $?
  while read -r name newver; do
    [[ -n "$name" ]] || continue
    name=${name%.*}
    old_provides=$(rpm -q --provides "$name") || return $?
    new_provides=$(dnf -q repoquery --disableplugin=versionlock --provides "${name}-${newver}" 2>/dev/null) || return $?
    old_sonames=$(sonames <<< "$old_provides") || return $?
    new_sonames=$(sonames <<< "$new_provides") || return $?
    gone=$(comm -23 <(printf '%s\n' "$old_sonames") <(printf '%s\n' "$new_sonames")) || return $?
    gone=${gone//$'\n'/ }
    installed=$(rpm -q --qf '%{VERSION}-%{RELEASE}' "$name") || return $?
    if [[ -n "$gone" ]]; then
      echo "  held: $name $installed -> $newver SONAME CHANGE removes: $gone"
    else
      echo "  held: $name $installed -> $newver"
    fi
  done <<< "$updates"
  echo "  run centmin.sh menu option 5 to update held packages and rebuild PHP against them"
}

do_check() {
  local f missing targets dependencies
  targets=$(php_targets) || return $?
  missing=$(
    set -o pipefail
    for f in $targets; do
      dependencies=$(ldd "$f" 2>/dev/null) || exit $?
      awk -v f="$f" '$2 == "=>" && $3 == "not" {print "  " f ": " $1}' <<< "$dependencies" || exit $?
    done | sort -u
  ) || return $?
  if [[ -n "$missing" ]]; then
    if [[ "$1" == lock ]]; then
      echo "warning: some PHP shared libraries are missing, locking the rest" >&2
      return 0
    fi
    echo "WARNING: PHP shared libraries are missing:"
    echo "$missing"
    echo "  php-fpm will fail on its next restart or server reboot"
    echo "  rebuild PHP via centmin.sh menu option 5"
    return 1
  fi
  echo "php-libs-versionlock: all PHP shared libraries found"
  return 0
}

do_list() {
  local pkgs repo_records installed
  pkgs=$(collect_packages) || return $?
  [[ -z "$pkgs" ]] && { echo "no non-base packages linked by PHP"; return 0; }
  repo_records=$(from_repo $pkgs) || return $?
  while IFS=$'\t' read -r name repo; do
    [[ -n "$name" ]] || continue
    installed=$(rpm -q --qf '%{VERSION}-%{RELEASE}' "$name") || return $?
    printf '%-32s %-36s %s\n' "$name" "$installed" "$repo"
  done <<< "$repo_records"
}

# Never lock the replaced locklist inode: hold one stable inode for the whole
# read/discovery/DNF/publication invocation, including inherited DNF children.
mutate() {
  (
    umask 077
    exec 9>>"$LOCK_MUTEX" || exit $?
    flock -x 9 || exit $?
    "$@"
  )
}

if [[ "$1" != 'check' ]]; then
  el_version=$(rpm -E '%{rhel}' 2>/dev/null) || exit $?
  [[ "$el_version" =~ ^[0-9]+$ ]] || { echo "error: cannot determine the EL version" >&2; exit 1; }
  if [[ "$el_version" -lt 8 ]]; then
    echo "php-libs-versionlock: EL7 repos are frozen, nothing to lock"
    exit 0
  fi
fi

case "$1" in
  lock) mutate do_lock ;;
  unlock) mutate do_unlock ;;
  update) mutate do_update ;;
  status) do_status ;;
  check) do_check ;;
  list) do_list ;;
  *) echo "usage: $0 {lock|unlock|update|status|check|list}"; exit 1 ;;
esac
