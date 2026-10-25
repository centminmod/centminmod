#!/bin/bash
#######################################################################
# php-fpm brotli override, a brotli CVE-2026-111497 stopgap
#
# The distro brotli packages on EL8/EL9/EL10 (1.0.6/1.0.9/1.1.0) lack the
# decoder out-of-bounds write fix, and php-fpm loads their libbrotlidec
# through libcurl, freetype and libjxl. libcurl decodes brotli that remote
# servers send, and on EL8/EL9 it decodes Transfer-Encoding: br even when
# the request never asked for decoding.
#
# on      build brotli 1.2.1 into /opt/brotli_php/<version>-<timestamp> and
#         point php-fpm.service at it with a systemd LD_LIBRARY_PATH drop-in
# off     remove the drop-in and go back to the distro brotli
# status  show what php-fpm loads and whether the distro package is fixed
# prepare menu 5, before its php-fpm restart: refresh or retire the override
# verify  menu 5, after its php-fpm restart: confirm it or fall back
#
# The override retires itself (on, prepare) once the distro brotli package
# includes the fix. prepare and verify never fail the calling menu.
#######################################################################
BROTLI_PHP_VER='1.2.1'
BROTLI_PHP_SHA256='98d35d11a78dc453aebb7d10c3748f3f73ff78c8267c0dc697b8631f67bf7e8f'
BROTLI_PHP_URL="https://github.com/google/brotli/archive/refs/tags/v${BROTLI_PHP_VER}.tar.gz"
BROTLI_PHP_MIRROR="https://parts.centminmod.com/centminmodparts/brotli/brotli-${BROTLI_PHP_VER}.tar.gz"
BROTLI_PHP_BASE='/opt/brotli_php'
DROPIN_DIR='/etc/systemd/system/php-fpm.service.d'
DROPIN="${DROPIN_DIR}/brotli-source.conf"
PHPFPM_BIN='/usr/local/sbin/php-fpm'
DIR_TMP='/svr-setup'
CUSTOM_CONFIG='/etc/centminmod/custom_config.inc'
CM_DIR=$(readlink -f "$(dirname "${BASH_SOURCE[0]}")/..")

# brotli_fixed_version and brotli_version_check
source "${CM_DIR}/inc/brotli.inc" || exit 1

unit_prop() {
  # systemd 219 (EL7) has no systemctl show --value
  systemctl show -p "$1" php-fpm 2>/dev/null | cut -d= -f2-
}

require_systemd() {
  if [[ ! -d /run/systemd/system ]] || ! systemctl cat php-fpm >/dev/null 2>&1; then
    echo "php-fpm.service not found: the override needs a systemd managed php-fpm"
    return 1
  fi
  if [ ! -x "$PHPFPM_BIN" ]; then
    echo "$PHPFPM_BIN not found"
    return 1
  fi
}

# Build directory the drop-in points php-fpm at, empty without a drop-in
active_dir() {
  [ -f "$DROPIN" ] || return 0
  sed -n 's|^Environment="LD_LIBRARY_PATH=\(.*\)/lib64"$|\1|p' "$DROPIN"
}

# All three libraries present and the CLI built from the pinned, fixed version
dir_valid() {
  local d=$1 lib
  [[ -n "$d" && -d "$d" ]] || return 1
  for lib in libbrotlicommon libbrotlidec libbrotlienc; do
    [ -e "${d}/lib64/${lib}.so.1" ] || return 1
  done
  [[ "$("${d}/bin/brotli" -V 2>/dev/null | awk '{print $2}')" = "$BROTLI_PHP_VER" ]] || return 1
  brotli_version_check "${d}/bin/brotli" >/dev/null
}

# Package owning the system libbrotlidec php-fpm loads without the override
distro_brotli_pkg() {
  local lib pkg
  lib=$(readlink -f /usr/lib64/libbrotlidec.so.1 2>/dev/null)
  [ -f "$lib" ] || return 1
  # rpm prints "not owned by any package" on stdout, so go by its exit status
  pkg=$(rpm -qf --qf '%{NAME}-%{VERSION}-%{RELEASE}\n' "$lib" 2>/dev/null) || return 1
  echo "${pkg%%$'\n'*}"
}

# AlmaLinux/Rocky backports keep the version number, so check the changelog too
distro_brotli_fixed() {
  local pkg
  pkg=$(distro_brotli_pkg) || return 1
  rpm -q --changelog "$pkg" 2>/dev/null | grep -q 'CVE-2026-111497' && return 0
  brotli_fixed_version "$(rpm -q --qf '%{VERSION}' "$pkg")"
}

# php-fpm -t loads the extensions too, so this covers libjxl via imagick
phpfpm_loads_brotli() {
  env -u LD_LIBRARY_PATH LD_DEBUG=libs "$PHPFPM_BIN" -t 2>&1 | grep -q 'calling init: .*/libbrotlidec\.so'
}

# LD_LIBRARY_PATH for php-fpm set anywhere other than our drop-in
foreign_ld_path() {
  local f
  for f in $(unit_prop FragmentPath) $(unit_prop DropInPaths); do
    [[ "$f" = "$DROPIN" ]] && continue
    if grep -qs 'LD_LIBRARY_PATH' "$f"; then
      echo "$f"
      return 0
    fi
  done
  for f in $(unit_prop EnvironmentFiles | awk '{print $1}'); do
    if grep -qs 'LD_LIBRARY_PATH' "${f#-}"; then
      echo "${f#-}"
      return 0
    fi
  done
  return 1
}

cmake_cmd() {
  local c v
  for c in cmake cmake3; do
    command -v "$c" >/dev/null 2>&1 || continue
    v=$("$c" --version 2>/dev/null | awk 'NR==1 {print $3}')
    if [[ -n "$v" && "$(printf '3.15\n%s\n' "$v" | sort -V | head -1)" = '3.15' ]]; then
      echo "$c"
      return 0
    fi
  done
  return 1
}

# Builds into a new directory every time, so libraries a running php-fpm
# has mapped are never overwritten. Sets BUILD_DIR.
build_brotli() {
  local d
  d="${BROTLI_PHP_BASE}/${BROTLI_PHP_VER}-$(date +%Y%m%d%H%M%S)"
  local src="${DIR_TMP}/brotli-php-${BROTLI_PHP_VER}"
  local tarball="${DIR_TMP}/brotli-${BROTLI_PHP_VER}.tar.gz"
  local log="${DIR_TMP}/brotli-php-build.log"
  local cmake
  if ! cmake=$(cmake_cmd); then
    if command -v dnf >/dev/null 2>&1; then
      dnf -y -q install cmake
    else
      yum -y -q install cmake3
    fi
    cmake=$(cmake_cmd) || { echo "brotli ${BROTLI_PHP_VER} needs cmake 3.15 or newer"; return 1; }
  fi
  mkdir -p "$DIR_TMP" "$BROTLI_PHP_BASE" || return 1
  chmod 755 "$BROTLI_PHP_BASE"
  if ! echo "${BROTLI_PHP_SHA256}  ${tarball}" | sha256sum -c --quiet - >/dev/null 2>&1; then
    rm -f "$tarball"
    echo "downloading brotli ${BROTLI_PHP_VER}"
    if ! curl -fsSL --connect-timeout 30 --max-time 300 -o "$tarball" "$BROTLI_PHP_URL"; then
      curl -fsSL --connect-timeout 30 --max-time 300 -o "$tarball" "$BROTLI_PHP_MIRROR" || { rm -f "$tarball"; echo "brotli ${BROTLI_PHP_VER} download failed"; return 1; }
    fi
    if ! echo "${BROTLI_PHP_SHA256}  ${tarball}" | sha256sum -c --quiet -; then
      rm -f "$tarball"
      echo "brotli ${BROTLI_PHP_VER} tarball does not match its pinned sha256"
      return 1
    fi
  fi
  rm -rf "$src"
  mkdir -p "$src" && tar -xzf "$tarball" -C "$src" --strip-components=1 || return 1
  echo "building brotli ${BROTLI_PHP_VER} into ${d}"
  # Plain -O2 with hardening, not the nginx-dep -Ofast/LTO flags, and no
  # compiler or flags inherited from the caller
  if ! ( cd "$src" &&
         env -u CC -u CXX -u CFLAGS -u CXXFLAGS -u CPPFLAGS -u LDFLAGS -u LD_LIBRARY_PATH \
           "$cmake" -S . -B out -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBROTLI_DISABLE_TESTS=ON \
           -DCMAKE_INSTALL_PREFIX="$d" -DCMAKE_INSTALL_LIBDIR=lib64 -DCMAKE_INSTALL_RPATH="${d}/lib64" \
           -DCMAKE_C_FLAGS="-fPIC -fstack-protector-strong -D_FORTIFY_SOURCE=2" -DCMAKE_C_FLAGS_RELEASE="-O2 -DNDEBUG" \
           -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-z,relro,-z,now" -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,relro,-z,now" &&
         env -u CC -u CXX -u CFLAGS -u CXXFLAGS -u CPPFLAGS -u LDFLAGS -u LD_LIBRARY_PATH \
           "$cmake" --build out -j"$(nproc)" &&
         "$cmake" --install out ) > "$log" 2>&1; then
    tail -20 "$log"
    rm -rf "$d" "$src"
    echo "brotli ${BROTLI_PHP_VER} build failed, log: ${log}"
    return 1
  fi
  seq 1 20000 > "${src}/roundtrip.txt"
  if ! "${d}/bin/brotli" -c "${src}/roundtrip.txt" | "${d}/bin/brotli" -dc | cmp -s - "${src}/roundtrip.txt" || ! dir_valid "$d"; then
    rm -rf "$d" "$src"
    echo "brotli ${BROTLI_PHP_VER} build in ${d} failed its checks"
    return 1
  fi
  rm -rf "$src"
  BUILD_DIR=$d
}

# Reuse the active or current build when it is valid. Sets BUILD_DIR.
ensure_build() {
  local d
  for d in "$(active_dir)" "$(readlink -f "${BROTLI_PHP_BASE}/current" 2>/dev/null)"; do
    if dir_valid "$d"; then
      BUILD_DIR=$d
      return 0
    fi
  done
  build_brotli
}

# php-fpm -t under the override must load the new brotli and no system copy
preflight() {
  local d=$1 out rc
  out=$(env LD_DEBUG=libs LD_LIBRARY_PATH="${d}/lib64" "$PHPFPM_BIN" -t 2>&1)
  rc=$?
  if [ "$rc" -ne 0 ]; then
    echo "preflight: php-fpm -t failed (exit $rc) with LD_LIBRARY_PATH=${d}/lib64"
    return 1
  fi
  if ! grep -q "calling init: ${d}/lib64/libbrotlidec\.so" <<< "$out"; then
    echo "preflight: php-fpm did not load ${d}/lib64/libbrotlidec.so.1"
    return 1
  fi
  if grep -qE 'calling init: /(usr/)?lib64/libbrotli' <<< "$out"; then
    echo "preflight: php-fpm still loads a distro libbrotli"
    return 1
  fi
  echo "preflight: php-fpm -t loads brotli from ${d}/lib64"
}

write_dropin() {
  local d=$1 tmp
  mkdir -p "$DROPIN_DIR" || return 1
  tmp=$(mktemp "${DROPIN_DIR}/.brotli-source.XXXXXX") || return 1
  cat > "$tmp" <<EOF
# Centmin Mod brotli CVE-2026-111497 stopgap, managed by tools/phpfpm-brotli.sh
# php-fpm loads brotli ${BROTLI_PHP_VER} from ${d}/lib64 instead of the distro copy
# remove with: ${CM_DIR}/tools/phpfpm-brotli.sh off
[Service]
Environment="LD_LIBRARY_PATH=${d}/lib64"
EOF
  chmod 644 "$tmp" && mv -f "$tmp" "$DROPIN" && systemctl daemon-reload
}

remove_dropin() {
  rm -f "$DROPIN"
  systemctl daemon-reload
}

# php-fpm master and worker pids, from the service cgroup (v2, then v1)
fpm_pids() {
  local cg base
  cg=$(unit_prop ControlGroup)
  if [ -n "$cg" ]; then
    for base in /sys/fs/cgroup /sys/fs/cgroup/systemd; do
      if [ -f "${base}${cg}/cgroup.procs" ]; then
        cat "${base}${cg}/cgroup.procs"
        return 0
      fi
    done
  fi
  unit_prop MainPID | grep -v '^0$'
}

fpm_brotli_maps() {
  local pid
  for pid in $(fpm_pids); do
    awk '$6 ~ /libbrotli/ {print $6}' "/proc/${pid}/maps" 2>/dev/null
  done | sort -u
}

# Running php-fpm maps the override's libbrotlidec and no distro libbrotli
verify_active() {
  local d=$1 maps
  systemctl is-active --quiet php-fpm || return 1
  maps=$(fpm_brotli_maps)
  grep -q "^${d}/lib64/libbrotlidec\.so" <<< "$maps" || return 1
  ! grep -qE '^/(usr/)?lib64/libbrotli' <<< "$maps"
}

# Remove builds the drop-in does not use and no process maps, and point
# the current symlink (for PHP CLI cron jobs) at the active build
cleanup_dirs() {
  local keep d
  [ -d "$BROTLI_PHP_BASE" ] || return 0
  keep=$(active_dir)
  for d in "$BROTLI_PHP_BASE"/*; do
    [[ -d "$d" && ! -L "$d" ]] || continue
    [[ "$d" = "$keep" ]] && continue
    if grep -qsF "${d}/" /proc/[0-9]*/maps; then
      echo "kept ${d}: a running process still maps it"
      continue
    fi
    rm -rf "$d" && echo "removed unused ${d}"
  done
  if [ -n "$keep" ]; then
    ln -sfn "$keep" "${BROTLI_PHP_BASE}/current"
  else
    rm -f "${BROTLI_PHP_BASE}/current"
    rmdir "$BROTLI_PHP_BASE" 2>/dev/null
  fi
}

# centmin.sh default, overridden by custom_config.inc
effective_setting() {
  sed -n "s/^PHPFPM_BROTLI_SOURCE=['\"]\{0,1\}\([yYnN]\).*/\1/p" "${CM_DIR}/centmin.sh" "$CUSTOM_CONFIG" 2>/dev/null | tail -1
}

cli_hint() {
  echo "PHP CLI cron jobs that fetch remote URLs need the same override, for example:"
  echo "  env LD_LIBRARY_PATH=${BROTLI_PHP_BASE}/current/lib64 /usr/local/bin/php /path/to/cmd.php xf:run-jobs"
}

cmd_on() {
  local prev d foreign
  require_systemd || return 1
  if distro_brotli_fixed; then
    echo "distro $(distro_brotli_pkg) includes the CVE-2026-111497 fix, the override is not needed"
    if [ -f "$DROPIN" ]; then
      cmd_off
      return $?
    fi
    return 0
  fi
  if ! phpfpm_loads_brotli; then
    echo "php-fpm does not load libbrotlidec, nothing to override"
    return 0
  fi
  if foreign=$(foreign_ld_path); then
    echo "php-fpm already gets LD_LIBRARY_PATH from ${foreign}, not adding a second one"
    return 1
  fi
  ensure_build || return 1
  d=$BUILD_DIR
  if [[ "$d" = "$(active_dir)" ]] && verify_active "$d"; then
    echo "php-fpm already loads brotli ${BROTLI_PHP_VER} from ${d}"
    cleanup_dirs
    return 0
  fi
  preflight "$d" || return 1
  prev=$(cat "$DROPIN" 2>/dev/null)
  write_dropin "$d" || return 1
  systemctl restart php-fpm
  if ! verify_active "$d"; then
    echo "php-fpm did not come up with brotli from ${d}, rolling back"
    if [ -n "$prev" ]; then
      printf '%s\n' "$prev" > "$DROPIN"
      systemctl daemon-reload
    else
      remove_dropin
    fi
    systemctl restart php-fpm
    return 1
  fi
  cleanup_dirs
  echo "php-fpm loads brotli ${BROTLI_PHP_VER} from ${d}"
  cli_hint
}

cmd_off() {
  require_systemd || return 1
  if [ -f "$DROPIN" ]; then
    remove_dropin
    systemctl restart php-fpm
    if ! systemctl is-active --quiet php-fpm; then
      echo "php-fpm failed to restart after removing ${DROPIN}"
      return 1
    fi
    if fpm_brotli_maps | grep -q "^${BROTLI_PHP_BASE}/"; then
      echo "php-fpm still maps brotli from ${BROTLI_PHP_BASE}"
      return 1
    fi
    echo "php-fpm loads the distro brotli again"
  else
    echo "override is not on"
  fi
  cleanup_dirs
  if [[ "$(effective_setting)" = [yY] ]]; then
    echo "PHPFPM_BROTLI_SOURCE='y' is set, so the next menu 5 PHP upgrade turns the override back on"
    echo "set PHPFPM_BROTLI_SOURCE='n' in ${CUSTOM_CONFIG} to keep it off"
  fi
  if [ -d "$BROTLI_PHP_BASE" ]; then
    echo "remove the LD_LIBRARY_PATH=${BROTLI_PHP_BASE}/current/lib64 prefix from any PHP CLI cron jobs"
  fi
}

cmd_status() {
  local d pkg pid other
  require_systemd || return 1
  d=$(active_dir)
  if [ -n "$d" ]; then
    echo "override:      on, ${DROPIN}"
    echo "build:         ${d} ($("${d}/bin/brotli" -V 2>/dev/null))"
  else
    echo "override:      off"
  fi
  pkg=$(distro_brotli_pkg)
  if distro_brotli_fixed; then
    echo "distro brotli: ${pkg}, includes the CVE-2026-111497 fix$([ -n "$d" ] && echo ', run: off')"
  else
    echo "distro brotli: ${pkg:-not found}, lacks the CVE-2026-111497 fix"
  fi
  echo "setting:       PHPFPM_BROTLI_SOURCE='$(effective_setting)'"
  echo "environment:   $(unit_prop Environment)"
  echo "php-fpm ELF:   $(readelf -d "$PHPFPM_BIN" 2>/dev/null | sed -n 's/.*(\(RPATH\|RUNPATH\)).*\[\(.*\)\]/\1 \2/p')"
  echo "php-fpm maps:"
  fpm_brotli_maps | sed 's/^/  /'
  for pid in $(ps -eo pid,args | awk '/php-fpm: master/ && !/awk/ {print $1}'); do
    fpm_pids | grep -qx "$pid" || other="${other} ${pid}"
  done
  if [ -n "$other" ]; then
    echo "not covered:   php-fpm masters outside php-fpm.service:${other}"
  fi
  [ -n "$d" ] && cli_hint
  return 0
}

# menu 5, before its php-fpm restart
cmd_prepare() {
  local foreign
  require_systemd >/dev/null || return 0
  if [[ ! -f "$DROPIN" && "${PHPFPM_BROTLI_SOURCE:-n}" != [yY] ]]; then
    return 0
  fi
  if distro_brotli_fixed; then
    echo "php-fpm brotli override: distro $(distro_brotli_pkg) includes the CVE-2026-111497 fix"
    if [ -f "$DROPIN" ]; then
      echo "php-fpm brotli override: retired, php-fpm goes back to the distro brotli"
      remove_dropin
    fi
    return 0
  fi
  if ! phpfpm_loads_brotli; then
    if [ -f "$DROPIN" ]; then
      echo "php-fpm brotli override: php-fpm no longer loads libbrotlidec, removing the override"
      remove_dropin
    fi
    return 0
  fi
  if foreign=$(foreign_ld_path); then
    echo "php-fpm brotli override: WARNING php-fpm already gets LD_LIBRARY_PATH from ${foreign}, not changing it"
    return 0
  fi
  if ! ensure_build; then
    echo "php-fpm brotli override: WARNING brotli ${BROTLI_PHP_VER} build failed, php-fpm uses the distro brotli"
    [ -f "$DROPIN" ] && remove_dropin
    return 0
  fi
  if ! preflight "$BUILD_DIR"; then
    echo "php-fpm brotli override: WARNING preflight failed, php-fpm uses the distro brotli"
    [ -f "$DROPIN" ] && remove_dropin
    return 0
  fi
  [[ "$BUILD_DIR" = "$(active_dir)" ]] || write_dropin "$BUILD_DIR"
  echo "php-fpm brotli override: the php-fpm restart loads brotli ${BROTLI_PHP_VER} from ${BUILD_DIR}"
  return 0
}

# menu 5, after its php-fpm restart
cmd_verify() {
  local d
  require_systemd >/dev/null || return 0
  d=$(active_dir)
  if [ -n "$d" ]; then
    if verify_active "$d"; then
      echo "php-fpm brotli override: php-fpm loads brotli ${BROTLI_PHP_VER} from ${d}"
    else
      echo "php-fpm brotli override: WARNING php-fpm does not load ${d}, falling back to the distro brotli"
      remove_dropin
      systemctl restart php-fpm
    fi
  fi
  cleanup_dirs
  return 0
}

if [ "$(id -u)" -ne 0 ]; then
  echo "run as root"
  exit 1
fi

exec 9>/run/phpfpm-brotli.lock
if ! flock -w 300 9; then
  echo "another phpfpm-brotli.sh run holds /run/phpfpm-brotli.lock"
  exit 1
fi

case "$1" in
  on )
    cmd_on
    ;;
  off )
    cmd_off
    ;;
  status )
    cmd_status
    ;;
  prepare )
    cmd_prepare
    ;;
  verify )
    cmd_verify
    ;;
  * )
    echo "Usage: $0 {on|off|status}"
    exit 1
    ;;
esac
