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
#   php-libs-versionlock.sh update  unlock and update the locked packages
#   php-libs-versionlock.sh status  show locked packages with held updates
#   php-libs-versionlock.sh check   report missing PHP shared libraries
#   php-libs-versionlock.sh list    show the packages that would be locked
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
LOCK_BEGIN='# centminmod php-libs-versionlock begin'
LOCK_END='# centminmod php-libs-versionlock end'
LOCK_CONF='/etc/dnf/plugins/versionlock.conf'
PHP_BINARIES='/usr/local/sbin/php-fpm /usr/local/bin/php'
# PHP extensions whose libraries other menu options version lock
# (imagick.so: ImageMagick locks managed by centmin.sh menu option 15)
SKIP_EXTENSIONS='imagick.so'

lock_file() {
  local f
  f=$(awk -F= '/^[[:space:]]*locklist[[:space:]]*=/ {gsub(/[[:space:]]/, "", $2); print $2; exit}' "$LOCK_CONF" 2>/dev/null)
  echo "${f:-/etc/dnf/plugins/versionlock.list}"
}

ensure_plugin() {
  if [[ ! -f "$LOCK_CONF" ]]; then
    dnf -y -q install python3-dnf-plugin-versionlock >/dev/null 2>&1
  fi
  [[ -f "$LOCK_CONF" ]]
}

php_targets() {
  local f extdir
  for f in $PHP_BINARIES; do
    [[ -x "$f" ]] && echo "$f"
  done
  extdir=$(/usr/local/bin/php-config --extension-dir 2>/dev/null)
  if [[ -d "$extdir" ]]; then
    for f in "$extdir"/*.so; do
      [[ -f "$f" ]] || continue
      [[ " $SKIP_EXTENSIONS " = *" ${f##*/} "* ]] && continue
      echo "$f"
    done
  fi
}

php_libs() {
  local f
  for f in $(php_targets); do
    ldd "$f" 2>/dev/null | awk '$2 == "=>" && $3 ~ /^\// {print $3}'
  done | sort -u
}

# lock list entries outside this script's block (other menu options, user)
other_locks() {
  local lf
  lf=$(lock_file)
  [[ -f "$lf" ]] || return 0
  awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '$0 == b {s = 1; next} $0 == e {s = 0; next} !s && NF && !/^[#!]/' "$lf"
}

# lock entries are name-epoch:version-release.*
entry_name() {
  sed -E 's/-[0-9]+:[^:]+$//'
}

block_names() {
  local lf
  lf=$(lock_file)
  [[ -f "$lf" ]] || return 0
  awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '$0 == b {s = 1; next} $0 == e {s = 0; next} s && NF && !/^#/' "$lf" | entry_name | sort -u
}

# repo ids from repo files shipped by the OS vendor (baseos, appstream, crb
# etc) plus pseudo repos for the OS install and local rpm installs.
# Vendor of individual packages is unreliable (AlmaLinux 8 base packages
# carry vendor CloudLinux), so classify by the repo a package came from.
base_repos() {
  local osvendor f
  osvendor=$(rpm -qf --qf '%{VENDOR}\n' /etc/redhat-release 2>/dev/null | head -1)
  printf '%s\n' anaconda System @System commandline @commandline
  for f in /etc/yum.repos.d/*.repo; do
    [[ -f "$f" ]] || continue
    if [[ "${f##*/}" = 'redhat.repo' ]] || [[ -n "$osvendor" && "$(rpm -qf --qf '%{VENDOR}' "$f" 2>/dev/null)" = "$osvendor" ]]; then
      awk -F'[][]' '/^\[/ {print $2}' "$f"
    fi
  done | sort -u
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
  local libs srpms names
  libs=$(php_libs)
  [[ -z "$libs" ]] && return 0
  srpms=$(rpm -qf --qf '%{SOURCERPM}\n' $libs 2>/dev/null | grep -v ' is not owned ' | sort -u)
  names=$(rpm -qa --qf '%{SOURCERPM}\t%{NAME}\n' | awk -F'\t' 'NR == FNR {s[$1] = 1; next} ($1 in s) {print $2}' <(echo "$srpms") - | sort -u)
  [[ -z "$names" ]] && return 0
  from_repo $names | awk -F'\t' 'NR == FNR {r[$1] = 1; next} $2 != "" && !($2 in r) {print $1}' <(base_repos) - | sort -u | grep -vxF -f <(other_locks | entry_name)
  return 0
}

write_block() {
  local lf tmp
  lf=$(lock_file)
  touch "$lf"
  tmp=$(mktemp)
  awk -v b="$LOCK_BEGIN" -v e="$LOCK_END" '$0 == b {s = 1; next} $0 == e {s = 0; next} !s' "$lf" > "$tmp"
  if [[ $# -gt 0 ]]; then
    {
      echo "$LOCK_BEGIN"
      echo "# $(date -u +%F' '%T) UTC packages linked by source compiled PHP, centmin.sh menu 5 updates them"
      rpm -q --qf '%{NAME}-%{EPOCHNUM}:%{VERSION}-%{RELEASE}.*\n' "$@"
      echo "$LOCK_END"
    } >> "$tmp"
  fi
  cat "$tmp" > "$lf"
  rm -f "$tmp"
}

do_lock() {
  local pkgs
  ensure_plugin || { echo "error: python3-dnf-plugin-versionlock not installed, PHP libraries not locked" >&2; return 1; }
  do_check >/dev/null || echo "warning: some PHP shared libraries are missing, locking the rest" >&2
  pkgs=$(collect_packages)
  write_block $pkgs
  echo "php-libs-versionlock: locked $(echo $pkgs | wc -w) packages in $(lock_file)"
  [[ -n "$pkgs" ]] && echo "  $(echo $pkgs)"
  return 0
}

do_unlock() {
  write_block
  echo "php-libs-versionlock: removed locks from $(lock_file)"
}

do_update() {
  local names
  names=$(block_names)
  do_unlock
  [[ -z "$names" ]] && return 0
  echo "dnf -y update $(echo $names)"
  dnf -y update $names
}

sonames() {
  grep -oE '^[^ (]+\.so[^ (]*' | sort -u
}

do_status() {
  local names name newver gone
  names=$(block_names)
  if [[ -z "$names" ]]; then
    echo "php-libs-versionlock: no PHP library locks in $(lock_file)"
    return 0
  fi
  echo "php-libs-versionlock: $(echo $names | wc -w) locked packages: $(echo $names)"
  dnf -q list updates --disableplugin=versionlock $names 2>/dev/null | awk 'NF == 3 && $2 ~ /^[0-9]/ {print $1, $2}' | while read -r name newver; do
    name=${name%.*}
    gone=$(comm -23 <(rpm -q --provides "$name" | sonames) <(dnf -q repoquery --disableplugin=versionlock --provides "${name}-${newver}" 2>/dev/null | sonames) | tr '\n' ' ')
    if [[ -n "$gone" ]]; then
      echo "  held: $name $(rpm -q --qf '%{VERSION}-%{RELEASE}' "$name") -> $newver SONAME CHANGE removes: $gone"
    else
      echo "  held: $name $(rpm -q --qf '%{VERSION}-%{RELEASE}' "$name") -> $newver"
    fi
  done
  echo "  run centmin.sh menu option 5 to update held packages and rebuild PHP against them"
}

do_check() {
  local f missing
  missing=$(for f in $(php_targets); do
    ldd "$f" 2>/dev/null | awk -v f="$f" '$2 == "=>" && $3 == "not" {print "  " f ": " $1}'
  done | sort -u)
  if [[ -n "$missing" ]]; then
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
  local pkgs
  pkgs=$(collect_packages)
  [[ -z "$pkgs" ]] && { echo "no non-base packages linked by PHP"; return 0; }
  from_repo $pkgs | while IFS=$'\t' read -r name repo; do
    printf '%-32s %-36s %s\n' "$name" "$(rpm -q --qf '%{VERSION}-%{RELEASE}' "$name")" "$repo"
  done
}

if [[ "$1" != 'check' && "$(rpm -E '%{rhel}' 2>/dev/null)" -lt 8 ]]; then
  echo "php-libs-versionlock: EL7 repos are frozen, nothing to lock"
  exit 0
fi

case "$1" in
  lock) do_lock ;;
  unlock) do_unlock ;;
  update) do_update ;;
  status) do_status ;;
  check) do_check ;;
  list) do_list ;;
  *) echo "usage: $0 {lock|unlock|update|status|check|list}"; exit 1 ;;
esac
