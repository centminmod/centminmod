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
# rpm-revert  downgrade the patched brotli rpm to the distro release
# reconcile   cmupdate: replace a patched build Centmin Mod has withdrawn
# notice      centmin.sh start: rpm-only check that warns about a withdrawn build
#
# With PHPFPM_BROTLI_RPM='y', on and prepare first install a Centmin Mod
# patched build of the distro brotli packages when one matches the installed
# release exactly. That fixes the system library for php-fpm, PHP CLI and
# everything else, so the override is not needed. A build listed in
# BROTLI_RPM_REVOKED is replaced by the distro release plus the override on
# the next cmupdate, on or menu 5 prepare.
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

# Patched distro brotli builds: the distro source rpm rebuilt with the fix,
# mirror directory, package and sha256. Same version and release plus ^cmm1:
# the caret sorts above the distro release but below any later distro rebuild
# (.1, .alma.1, a release bump), so the distro's own fix still replaces them.
BROTLI_RPM_SUFFIX='^cmm1'
BROTLI_RPM_MIRROR='https://parts.centminmod.com/centminmodparts/brotli/rpm'
BROTLI_RPMS='
brotli-1.0.6-4.el8_10 el8 brotli fe861ba4f1cc8f6eaf81d3c074986060343886cd906cfc20bcc952a39395cfb4
brotli-1.0.6-4.el8_10 el8 brotli-devel 840237fcb04ad73bc443edae1227765258cd9ac43b5380ae80927aa185c3b918
brotli-1.0.6-4.el8_10 el8 python3-brotli e30ea19b55639ca23c07f207f7775a105b3e43ab303e410fa38be3eac3498dcf
brotli-1.0.9-10.el9_8 el9 brotli b7d7a912a15a6e750adbfaa5c29ae87e1dfe238653745d1d121eec577d6d072b
brotli-1.0.9-10.el9_8 el9 brotli-devel f75c890a9cf7eef1c1f7ab918454878136261763ae1f3c7e85f38fbfcb5542eb
brotli-1.0.9-10.el9_8 el9 libbrotli 03319fb666388f0b6a0ccc1aefa553e4f2b5d56806985c1e3662a81aa10de261
brotli-1.0.9-10.el9_8 el9 python3-brotli f285aed342c5092623eea60ce1db6fbc561974615639497fe904e5f1e802557b
brotli-1.1.0-8.el10_2 el10 brotli 17dd2c31fa5426214bcb4fc66894e30b9fa007ba3d60868b78707676ded64bdd
brotli-1.1.0-8.el10_2 el10 brotli-devel 9bdb5169e1fed94581341ae876ae7b7a1c24557f6b420fe18e5816bd978e9406
brotli-1.1.0-8.el10_2 el10 libbrotli 01a2b1057f8bc4c98a19e75165c48ae691700d730fb29ef9f3928a8d6603c8b6
brotli-1.1.0-8.el10_2 el10 python3-brotli 8ab7f5572a9370bdd2df3a9206b78d6ab655c619bbe32ff7e4c0b731dfd39035
'
# Patched builds Centmin Mod has withdrawn, one source rpm per line without
# .src.rpm, e.g. brotli-1.0.9-10.el9_8^cmm1. Never installed, and replaced by
# the distro release plus the override where installed. Only ever add lines,
# so an older checkout can't withdraw more than a newer one.
BROTLI_RPM_REVOKED='
'

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

# rpm query (rpm -qf options) on the package owning the system libbrotlidec
# php-fpm loads without the override. By file, because EL8's rpm 4.14.3 can't
# look up a release with a caret (^cmm1) by name.
distro_brotli_rpmq() {
  local lib
  lib=$(readlink -f /usr/lib64/libbrotlidec.so.1 2>/dev/null)
  [ -f "$lib" ] || return 1
  # rpm prints "not owned by any package" on stdout, so go by its exit status
  rpm -qf "$@" "$lib" 2>/dev/null
}

distro_brotli_pkg() {
  local pkg
  pkg=$(distro_brotli_rpmq --qf '%{NAME}-%{VERSION}-%{RELEASE}\n') || return 1
  echo "${pkg%%$'\n'*}"
}

# AlmaLinux/Rocky backports keep the version number, so check the changelog too.
# A withdrawn patched build doesn't count as fixed.
distro_brotli_fixed() {
  distro_brotli_pkg >/dev/null || return 1
  rpm_revoked "$(distro_brotli_srpm)" && return 1
  distro_brotli_rpmq --changelog | grep -q 'CVE-2026-111497' && return 0
  brotli_fixed_version "$(distro_brotli_rpmq --qf '%{VERSION}\n' | head -1)"
}

# Source rpm of that package, e.g. brotli-1.0.9-10.el9_8.src.rpm
distro_brotli_srpm() {
  distro_brotli_rpmq --qf '%{SOURCERPM}\n' | head -1
}

# Version-release of that package, e.g. 1.0.9-10.el9_8^cmm1
distro_brotli_vr() {
  distro_brotli_rpmq --qf '%{VERSION}-%{RELEASE}\n' | head -1
}

# Installed packages built from source rpm $1, as name.arch
srpm_pkgs() {
  rpm -qa --qf '%{NAME}.%{ARCH} %{SOURCERPM}\n' | awk -v s="$1" '$2 == s {print $1}' | sort
}

# Any Centmin Mod patched build, current (^cmmN) or older (.cmmN)
patched_rpm_installed() {
  [[ "$(distro_brotli_pkg)" =~ [.^]cmm[0-9]+$ ]]
}

# Source rpm $1 (with or without .src.rpm) is a withdrawn patched build
rpm_revoked() {
  [ -n "$1" ] && grep -qxF "${1%.src.rpm}" <<< "$BROTLI_RPM_REVOKED"
}

# Mirror directory and sha256 of the patched build of package $2 from source rpm $1
patched_rpm_entry() {
  awk -v s="${1%.src.rpm}" -v n="$2" '$1 == s && $3 == n {print $2, $4}' <<< "$BROTLI_RPMS"
}

# Replace the distro brotli packages with the patched build of the same
# release. Installed from local files, so tools/php-libs-versionlock.sh
# leaves them unlocked (@commandline counts as a base repo) and a fixed
# distro release still replaces them.
install_patched_rpm() {
  local srpm vr pkgs p n dir sum file out files=()
  local d="${DIR_TMP}/brotli-rpm"
  command -v dnf >/dev/null 2>&1 || return 1
  srpm=$(distro_brotli_srpm) || return 1
  if rpm_revoked "${srpm%.src.rpm}${BROTLI_RPM_SUFFIX}"; then
    echo "the patched brotli build of ${srpm%.src.rpm} was withdrawn"
    return 1
  fi
  pkgs=$(srpm_pkgs "$srpm")
  for p in $pkgs; do
    if [[ "${p##*.}" != 'x86_64' || -z "$(patched_rpm_entry "$srpm" "${p%.*}")" ]]; then
      echo "no patched brotli build for ${p} from ${srpm}"
      return 1
    fi
  done
  vr=$(distro_brotli_vr)
  [[ -n "$pkgs" && -n "$vr" ]] || return 1
  mkdir -p "$d" || return 1
  for p in $pkgs; do
    n=${p%.*}
    read -r dir sum <<< "$(patched_rpm_entry "$srpm" "$n")"
    file="${d}/${n}-${vr}${BROTLI_RPM_SUFFIX}.x86_64.rpm"
    if ! echo "${sum}  ${file}" | sha256sum -c --quiet - >/dev/null 2>&1; then
      rm -f "$file"
      echo "downloading ${file##*/}"
      # the caret in the file name is %5E in the URL
      if ! curl -fsSL --connect-timeout 30 --max-time 300 -o "$file" "${BROTLI_RPM_MIRROR}/${dir}/$(sed 's/\^/%5E/g' <<< "${file##*/}")" ||
         ! echo "${sum}  ${file}" | sha256sum -c --quiet - >/dev/null 2>&1; then
        rm -f "$file"
        echo "${file##*/} download failed or does not match its pinned sha256"
        return 1
      fi
    fi
    files+=("$file")
  done
  # unsigned packages, checked against the pinned sha256 above instead
  if ! out=$(dnf -y -q --disablerepo='*' --setopt=localpkg_gpgcheck=0 install "${files[@]}" 2>&1); then
    printf '%s\n' "$out" | tail -5
    echo "dnf could not install the patched brotli packages"
    return 1
  fi
  rm -rf "$d"
  if ! distro_brotli_fixed; then
    echo "$(distro_brotli_pkg) is still not the patched build"
    return 1
  fi
  echo "installed $(distro_brotli_pkg), the distro brotli release with the CVE-2026-111497 fix"
  echo "PHP CLI cron jobs use it too, so they need no LD_LIBRARY_PATH prefix"
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

# php-fpm still maps a libbrotli that a package update replaced on disk
fpm_maps_replaced_brotli() {
  local pid
  for pid in $(fpm_pids); do
    awk '$6 ~ /libbrotli/ && $7 == "(deleted)" {f = 1} END {exit !f}' "/proc/${pid}/maps" 2>/dev/null && return 0
  done
  return 1
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

# centmin.sh default of setting $1, overridden by custom_config.inc
effective_setting() {
  sed -n "s/^${1}=['\"]\{0,1\}\([yYnN]\).*/\1/p" "${CM_DIR}/centmin.sh" "$CUSTOM_CONFIG" 2>/dev/null | tail -1
}

# PHPFPM_BROTLI_RPM from the caller's environment, else centmin.sh and custom_config.inc
rpm_wanted() {
  [[ "${PHPFPM_BROTLI_RPM:-$(effective_setting PHPFPM_BROTLI_RPM)}" = [yY] ]]
}

# PHPFPM_BROTLI_SOURCE the same way
source_wanted() {
  [[ "${PHPFPM_BROTLI_SOURCE:-$(effective_setting PHPFPM_BROTLI_SOURCE)}" = [yY] ]]
}

# Replace the installed patched build with the distro packages. With
# PHPFPM_BROTLI_SOURCE='y' the override goes in first, so php-fpm never runs
# the unfixed distro brotli in between. dnf distro-sync covers a distro
# release that is no longer in the repos.
revert_patched_rpm() {
  local vr p out override=n targets=() names=()
  vr=$(distro_brotli_vr | sed -E 's/[.^]cmm[0-9]+$//')
  for p in $(srpm_pkgs "$(distro_brotli_srpm)"); do
    targets+=("${p%.*}-${vr}.${p##*.}")
    names+=("${p%.*}")
  done
  # an empty list would make dnf distro-sync act on every package
  if [[ -z "$vr" || "${#names[@]}" -eq 0 ]]; then
    echo "could not work out the installed brotli packages, nothing changed"
    return 1
  fi
  if source_wanted && phpfpm_loads_brotli && ! foreign_ld_path >/dev/null; then
    if ensure_build && preflight "$BUILD_DIR" >/dev/null; then
      [[ "$BUILD_DIR" = "$(active_dir)" ]] || write_dropin "$BUILD_DIR"
      override=y
    else
      echo "WARNING the php-fpm override could not be prepared"
    fi
  fi
  if ! out=$(dnf -y -q downgrade "${targets[@]}" 2>&1) && ! out=$(dnf -y -q distro-sync "${names[@]}" 2>&1); then
    printf '%s\n' "$out" | tail -5
    echo "dnf could not replace the patched brotli packages"
  fi
  # distro-sync may have found a distro release with the fix
  if [[ "$override" = y ]] && distro_brotli_fixed; then
    remove_dropin
    override=n
  fi
  if systemctl is-active --quiet php-fpm; then
    systemctl restart php-fpm
    if [[ "$override" = y ]] && ! verify_active "$BUILD_DIR"; then
      echo "WARNING php-fpm did not come up with brotli from ${BUILD_DIR}, removing the override"
      remove_dropin
      systemctl restart php-fpm
      override=n
    fi
  fi
  cleanup_dirs
  if patched_rpm_installed; then
    echo "WARNING the patched brotli build $(distro_brotli_pkg) is still installed"
    [[ "$override" = y ]] && echo "php-fpm loads brotli ${BROTLI_PHP_VER} from ${BUILD_DIR} instead"
    return 1
  fi
  echo "replaced the patched brotli build with the distro $(distro_brotli_pkg)"
  if distro_brotli_fixed; then
    echo "the distro release includes the CVE-2026-111497 fix"
  elif [[ "$override" = y ]]; then
    echo "php-fpm loads brotli ${BROTLI_PHP_VER} from ${BUILD_DIR}"
    cli_hint
  else
    echo "WARNING the distro release lacks the CVE-2026-111497 fix and the php-fpm override is not on"
  fi
}

# Replace an installed patched build that Centmin Mod has withdrawn
rpm_reconcile() {
  patched_rpm_installed || return 0
  rpm_revoked "$(distro_brotli_srpm)" || return 0
  echo "Centmin Mod withdrew the patched brotli build $(distro_brotli_pkg), going back to the distro release"
  revert_patched_rpm
}

cli_hint() {
  echo "PHP CLI cron jobs that fetch remote URLs need the same override, for example:"
  echo "  env LD_LIBRARY_PATH=${BROTLI_PHP_BASE}/current/lib64 /usr/local/bin/php /path/to/cmd.php xf:run-jobs"
}

cmd_on() {
  local prev d foreign
  require_systemd || return 1
  rpm_reconcile
  if rpm_wanted && ! distro_brotli_fixed; then
    install_patched_rpm
  fi
  if distro_brotli_fixed; then
    echo "distro $(distro_brotli_pkg) includes the CVE-2026-111497 fix, the override is not needed"
    if [ -f "$DROPIN" ]; then
      cmd_off
      return $?
    fi
    if fpm_maps_replaced_brotli; then
      systemctl restart php-fpm
      echo "restarted php-fpm to load the updated distro brotli"
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
  if [[ "$(effective_setting PHPFPM_BROTLI_SOURCE)" = [yY] ]] && ! distro_brotli_fixed; then
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
  if patched_rpm_installed && rpm_revoked "$(distro_brotli_srpm)"; then
    echo "patched rpm:   installed but withdrawn by Centmin Mod, replace it: ${CM_DIR}/tools/phpfpm-brotli.sh reconcile"
  elif patched_rpm_installed; then
    echo "patched rpm:   installed, back to the distro release: ${CM_DIR}/tools/phpfpm-brotli.sh rpm-revert"
  elif [ -n "$pkg" ] && ! distro_brotli_fixed; then
    if rpm_revoked "$(distro_brotli_srpm | sed 's/\.src\.rpm$//')${BROTLI_RPM_SUFFIX}"; then
      echo "patched rpm:   withdrawn for this release, the override is used instead"
    elif [ -n "$(patched_rpm_entry "$(distro_brotli_srpm)" "$(distro_brotli_rpmq --qf '%{NAME}\n' | head -1)")" ]; then
      echo "patched rpm:   available for this release, on installs it when PHPFPM_BROTLI_RPM='y'"
    else
      echo "patched rpm:   none for this release"
    fi
  fi
  echo "setting:       PHPFPM_BROTLI_SOURCE='$(effective_setting PHPFPM_BROTLI_SOURCE)' PHPFPM_BROTLI_RPM='$(effective_setting PHPFPM_BROTLI_RPM)'"
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
  # before the early return below, so a withdrawn build is replaced either way
  rpm_reconcile 2>&1 | sed 's/^/php-fpm brotli override: /'
  if [[ ! -f "$DROPIN" && "${PHPFPM_BROTLI_SOURCE:-n}" != [yY] ]]; then
    return 0
  fi
  if [[ "${PHPFPM_BROTLI_SOURCE:-n}" = [yY] ]] && rpm_wanted && ! distro_brotli_fixed; then
    install_patched_rpm 2>&1 | sed 's/^/php-fpm brotli override: /'
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

# Downgrade the patched build to the distro release it was made from
cmd_rpm_revert() {
  require_systemd || return 1
  if ! patched_rpm_installed; then
    echo "the patched brotli rpm is not installed"
    return 0
  fi
  revert_patched_rpm || return 1
  if rpm_wanted && ! rpm_revoked "$(distro_brotli_srpm | sed 's/\.src\.rpm$//')${BROTLI_RPM_SUFFIX}"; then
    echo "PHPFPM_BROTLI_RPM='y' is set, so the next menu 5 PHP upgrade or on installs the patched rpm again"
    echo "set PHPFPM_BROTLI_RPM='n' in ${CUSTOM_CONFIG} to keep the distro package"
  fi
}

# cmupdate, after updating the code: replace a withdrawn patched build.
# Prints nothing when there is nothing to do.
cmd_reconcile() {
  require_systemd >/dev/null || return 0
  rpm_reconcile
  return 0
}

# centmin.sh start: rpm queries only, no dnf, no lock and no changes
cmd_notice() {
  if patched_rpm_installed && rpm_revoked "$(distro_brotli_srpm)"; then
    echo "WARNING Centmin Mod withdrew the patched brotli build $(distro_brotli_pkg)"
    echo "replace it with: ${CM_DIR}/tools/phpfpm-brotli.sh reconcile"
  fi
  return 0
}

# notice runs on every centmin.sh start, so it skips the root check and lock
if [ "$1" = notice ]; then
  cmd_notice
  exit 0
fi

if [ "$(id -u)" -ne 0 ]; then
  echo "run as root"
  exit 1
fi

# reconcile runs from cmupdate, so it doesn't wait long behind another run
lock_wait=300
[ "$1" = reconcile ] && lock_wait=10
exec 9>/run/phpfpm-brotli.lock
if ! flock -w "$lock_wait" 9; then
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
  rpm-revert )
    cmd_rpm_revert
    ;;
  reconcile )
    cmd_reconcile
    ;;
  * )
    echo "Usage: $0 {on|off|status|prepare|verify|rpm-revert|reconcile|notice}"
    exit 1
    ;;
esac
