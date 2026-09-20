#!/bin/bash
# Optional isolated checks for the real shared toolset helper. No packages installed.
set -e
repo=$(cd "$(dirname "$0")/.." && pwd)
scratch=$(mktemp -d "$repo/.gcc15-test.XXXXXX")
trap 'rm -rf "$scratch"' EXIT
helper=$(sed -n '/^enable_gcc_toolset15() {/,/^}/p' "$repo/inc/gcc.inc")
[[ -n "$helper" ]]
helper=${helper//\/opt\/rh\/gcc-toolset-15/$scratch/toolset}
helper=${helper//\/usr\/lib\/gcc-toolset\/15-env.source/$scratch/15-env.source}
eval "$helper"
mkdir -p "$scratch/toolset/root/usr/bin"
printf ':\n' > "$scratch/toolset/enable"
printf '#!/bin/bash\necho "${GCC_TEST_VERSION:-15.2.1}"\n' > "$scratch/compiler"
install_fake() {
  cp "$scratch/compiler" "$scratch/toolset/root/usr/bin/gcc"
  cp "$scratch/compiler" "$scratch/toolset/root/usr/bin/g++"
  cp "$scratch/compiler" "$scratch/toolset/root/usr/bin/ld"
  chmod +x "$scratch/toolset/root/usr/bin/"*
}
yum() {
  [[ "$*" = '-y install gcc-toolset-15-gcc gcc-toolset-15-gcc-c++ gcc-toolset-15-binutils gcc-toolset-15-runtime' ]] || return 1
  case "$install_result" in
    fail) return 1;;
    incomplete) return 0;;
    *) install_fake;;
  esac
}
install_result=success INITIALINSTALL=y
# Missing packages: installer is called, then the actual files are verified.
enable_gcc_toolset15
[[ "$CC" = "$scratch/toolset/root/usr/bin/gcc" ]]
mv "$scratch/toolset/enable" "$scratch/15-env.source"
INITIALINSTALL=n install_result=fail
# Already installed: no package transaction is needed.
enable_gcc_toolset15
[[ "$CC" = "ccache $scratch/toolset/root/usr/bin/gcc" && "$CCACHE_CPP2" = yes ]]
export GCC_TEST_VERSION=14.2.1
if enable_gcc_toolset15; then echo 'FAIL: wrong compiler accepted'; exit 1; fi
unset GCC_TEST_VERSION
rm "$scratch/toolset/root/usr/bin/g++"
if enable_gcc_toolset15; then echo 'FAIL: failed install accepted'; exit 1; fi
install_result=incomplete
if enable_gcc_toolset15; then echo 'FAIL: incomplete install accepted'; exit 1; fi
( enable_gcc_toolset15 ) 2>&1 | cat >/dev/null
[[ "${PIPESTATUS[0]}" = 1 ]]
# Exercise the actual branch-specific OS defaults, without sourcing the main script.
defaults=$(sed -n '/^# Official GCC Toolset 15 on supported EL releases\./,/^fi$/p' "$repo/centmin.sh")
[[ -n "$defaults" ]]
[[ "$(sed -n '/^# Official GCC Toolset 15 on supported EL releases\./,/^fi$/p' "$repo/centmin-cli.sh")" = "$defaults" ]]
branch=$(sed -n "s/^branchname='\(.*\)'/\1/p" "$repo/centmin.sh")
for os in 7 8 9 10; do
  CENTOS_EIGHT= CENTOS_NINE= CENTOS_TEN= DEVTOOLSETFIFTTEEN=n
  case "$os" in
    8) CENTOS_EIGHT=8;;
    9) CENTOS_NINE=9;;
    10) CENTOS_TEN=10;;
  esac
  eval "$defaults"
  expected=n
  if [[ "$os" = 8 || "$os" = 9 ]] || { [[ "$os" = 10 ]] && [[ "$branch" = 141.00beta01 ]]; }; then expected=y; fi
  [[ "$DEVTOOLSETFIFTTEEN" = "$expected" ]]
done
# EL10 must reach the source OpenSSL downloader on its supported branch.
if [[ "$branch" = 141.00beta01 ]]; then
  download_gate=$(sed -n '/^openssldownload() {/,/^}/p' "$repo/inc/downloads.inc" | grep '^if \[\[ "\$CENTOSVER" > 6')
  [[ -n "$download_gate" ]]
  CENTOSVER=10.2 CENTOS_TEN=10
  download_gate=${download_gate#'if '}
  eval "${download_gate%; then}"
fi
# Compression's C dialect must not leak into C++ flags.
compression=$(sed -n '/^compgcc_fifteen() {/,/^}/p' "$repo/inc/compress.inc")
if [[ -n "$compression" ]]; then
  (
    install_fake
    compression=${compression//\/opt\/rh\/gcc-toolset-15/$scratch/toolset}
    compression=${compression//\/opt\/rh\/devtoolset-15/$scratch/absent-devtoolset}
    eval "$compression"
    enable_gcc_toolset15() { return 0; }
    gcc() { echo ' -march x86-64'; }
    compgcc_fifteen
    [[ "$label" = 'gcc15 built' && "$CFLAGS" = *' -std=gnu17'* && "$CXXFLAGS" != *' -std=gnu17'* ]] || { echo 'FAIL: compression GCC15 label or language flags'; exit 1; }
    enable_gcc_toolset15() { return 1; }
    if compgcc_fifteen; then echo 'FAIL: compression ignored activation failure'; exit 1; fi
  )
fi
# Missing Perl core modules must be installed or stop the OpenSSL build.
(
  CENTOS_EIGHT= CENTOS_NINE=9 CENTOS_TEN=
  eval "$(sed -n '/^perl_ipc_cmd_install() {/,/^}/p' "$repo/inc/openssl_install.inc")"
  perl() { return 1; }
  yum() { echo "$*" >> "$scratch/perl-packages"; }
  perl_ipc_cmd_install
  grep -q 'install perl-Time-Piece' "$scratch/perl-packages"
  yum() { return 9; }
  if perl_ipc_cmd_install; then echo 'FAIL: missing Perl dependency accepted'; exit 1; fi
)
# A failed PGO phase must stop before the next phase or installation.
for file in php_configure php_upgrade; do
  for phase in prof-gen prof-clean prof-use; do
    phase_line=$(grep -E "^[[:space:]]*time make.* $phase \\|\\|" "$repo/inc/$file.inc")
    [[ -n "$phase_line" ]]
    failed_phase() {
      make() { return 7; }
      MAKETHREADS_PHP=' -j2'
      eval "$phase_line"
      echo 'continued after failed make'
    }
    if result=$(failed_phase 2>/dev/null); then
      echo "FAIL: $file continued after $phase failure" >&2
      exit 1
    fi
    [[ -z "$result" ]]
  done
done
# Initial builds must not continue to service setup after make/install failures.
for component in nginx_install php_configure; do
  commands=$(grep -E '^[[:space:]]*time make.* \|\| (exit|return) 1$' "$repo/inc/$component.inc")
  [[ -n "$commands" ]]
  while IFS= read -r command; do
    failed_install() { make() { return 7; }; eval "$command"; echo continued; }
    if result=$(failed_install 2>/dev/null); then
      echo "FAIL: $component continued after make/install failure" >&2
      exit 1
    fi
    [[ -z "$result" ]]
  done <<< "$commands"
done
# Timestamped configure must retain environment changes and its failure status.
for component in php nginx; do
  case "$component" in
    php) timing=TIME_PHPCONFIGURE; status=PHPCONFIGURE_ERR;;
    nginx) timing=TIME_NGINX; status=NGX_CONFIGURE_ERR;;
  esac
  block=$(awk -v timing="$timing" 'index($0, "if [[ ") == 1 && index($0, timing) {copy=1} copy {print} copy && $0 == "fi" {exit}' "$repo/inc/${component}_upgrade.inc")
  [[ -n "$block" ]]
  eval "funct_${component}configure() { export GCC15_ENV_CHECK=retained; return 7; }"
  for enabled in y n; do
    unset GCC15_ENV_CHECK
    printf -v "$timing" '%s' "$enabled"
    if eval "$block"; then :; fi
    [[ "$GCC15_ENV_CHECK" = retained && "${!status}" = 7 ]]
  done
done
echo 'PASS: GCC15 install, activation, ccache, version validation and failures'
