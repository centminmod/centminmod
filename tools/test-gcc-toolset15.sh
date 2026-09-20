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
# A present but failing activation script must not select a compiler.
install_fake
printf 'return 9\n' > "$scratch/15-env.source"
if enable_gcc_toolset15; then echo 'FAIL: activation source failure accepted'; exit 1; fi
printf ':\n' > "$scratch/15-env.source"
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
# GCC11 fallback applies to the requested PHP version, never an older installed one.
if [[ "$branch" = 141.00beta01 ]]; then
  fallback=$(awk '/^# Keep an explicitly requested conservative PHP PGO compiler/ {copy=1} copy && /^####/ {exit} copy {print}' "$repo/inc/php_configure.inc")
  [[ -n "$fallback" ]]
  select_php_compiler() { eval "$fallback"; }
  for os in 8 9 10; do
    CENTOS_EIGHT=0 CENTOS_NINE=0 CENTOS_TEN=0
    case "$os" in
      8) CENTOS_EIGHT=8 CENTOSVER_NUMERIC=810;;
      9) CENTOS_NINE=9 CENTOSVER_NUMERIC=98;;
      10) CENTOS_TEN=10 CENTOSVER_NUMERIC=102;;
    esac
    for PHPMUVER in 7.0 7.3 7.4 8.0 8.1 8.2 8.3 8.4 8.5; do
      PHPMVER=8.1
      for PHP_PGO in y n; do
        for PHP_PGO_FALLBACK_GCC11 in y n; do
          for PHP_PGO_FALLBACK_GCC in y n; do
            DEVTOOLSETFIFTTEEN=y DEVTOOLSETELEVEN=n DEVTOOLSETTWELVE=n DEVTOOLSETTHIRTEEN=n DEVTOOLSETFOURTEEN=n
            select_php_compiler
            expected=15
            if [[ "$PHP_PGO" = y && "$PHP_PGO_FALLBACK_GCC11" = y ]]; then
              case "$PHPMUVER" in
                7.4|8.0|8.1) expected=11;;
                7.0|7.3) [[ "$os" != 8 ]] || expected=11;;
              esac
            fi
            if [[ "$expected" = 15 ]]; then
              [[ "$DEVTOOLSETFIFTTEEN" = y && "$DEVTOOLSETELEVEN" = n && "$DEVTOOLSETTWELVE" = n && "$DEVTOOLSETTHIRTEEN" = n && "$DEVTOOLSETFOURTEEN" = n ]] || { echo "FAIL: PHP $PHPMUVER EL$os PGO=$PHP_PGO fallback11=$PHP_PGO_FALLBACK_GCC11 fallback12=$PHP_PGO_FALLBACK_GCC lost GCC15"; exit 1; }
            else
              [[ "$DEVTOOLSETELEVEN" = y && "$DEVTOOLSETFIFTTEEN" = n ]] || exit 1
            fi
          done
        done
      done
    done
  done
  PHPMUVER= PHPMVER=8.3 PHP_PGO=y PHP_PGO_FALLBACK_GCC11=y DEVTOOLSETFIFTTEEN=y
  select_php_compiler
  [[ "$DEVTOOLSETFIFTTEEN" = y ]]
  PHPMUVER=8.3 PHP_PGO_FALLBACK_GCC11=n PHP_PGO_FALLBACK_GCC=y DEVTOOLSETFIFTTEEN=n
  select_php_compiler
  [[ "$DEVTOOLSETTWELVE" = y && "$DEVTOOLSETFIFTTEEN" = n ]]
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

# Parent callers must stop before later install/configuration/service changes.
(
  set +e
  for file in centmin.sh centmin-cli.sh inc/php_upgrade.inc inc/cpcheck.inc inc/memcheck.inc; do
    calls=$(grep -E '^[[:space:]]*(incmemcachedinstall|funct_memcachedreinstall|redisinstall|imagickinstall|mongodbinstall|swooleinstall|mailparseinstall|geoiptwolite_phpext_install|geoipphpext|zopfliinstall|php_ext_brotli|php_ext_lzfour|php_ext_lzf|php_ext_zstd|autodetectinstallextensions|enable_devtoolset|funct_centmininstall|funct_phpupgrade|compressmenu_notice|libheif_install|imagemagick_heif|cpcheck|lowmemcheck)([[:space:]]|$)' "$repo/$file" | grep -v '> >(')
    [[ -n "$calls" ]] || exit 1
    while IFS= read -r call; do
      name=$(printf '%s\n' "$call" | awk '{print $1}')
      failed_parent() {
        eval "$name() { return 7; }"
        eval "$call"
        echo continued
      }
      if output=$(failed_parent); then
        echo "FAIL: $file ignored child failure: $call" >&2; exit 1
      fi
      [[ -z "$output" ]] || exit 1
    done <<< "$calls"
  done
  # Execute real nested timestamp/tee call sites for both timing selections.
  for file in centmin.sh centmin-cli.sh; do
    for timing in TIME_MEMCACHED TIME_IMAGEMAGICK; do
      block=$(awk -v timing="$timing" 'index($0, "if [[") && index($0, timing) {copy=1} copy {print} copy && /PIPESTATUS/ {exit}' "$repo/$file")
      [[ "$block" = *PIPESTATUS* ]] || exit 1
      for enabled in y n; do
        failed_logged_parent() {
          CENTMINLOGDIR="$scratch"
          funct_memcachedreinstall() { return 7; }
          imagickinstall() { return 7; }
          printf -v "$timing" '%s' "$enabled"
          eval "{ $block"
          echo continued
        }
        if output=$(failed_logged_parent); then
          echo "FAIL: $file $timing=$enabled lost failure through logging" >&2; exit 1
        fi
        [[ -z "$output" ]] || exit 1
      done
    done
  done
  # Redis must not restart services or continue its menu after activation failure.
  redis_menu=$(cat "$repo/inc/redis_submenu.inc")
  redis_menu=${redis_menu//\/usr\/bin\/nprestart/unexpected_restart}
  for enabled in y n; do
    for option in 1 2; do
      failed_redis_parent() {
        CENTMINLOGDIR="$scratch" TIME_REDIS="$enabled"
        cecho() { :; }; centminlog() { :; }; checkredis_server_install() { :; }
        read() { phpredisoption="$option"; }
        redisinstall() { return 7; }
        unexpected_restart() { touch "$scratch/unexpected-restart"; }
        eval "$redis_menu"
        phpredis_submenu
      }
      if failed_redis_parent >/dev/null; then
        echo "FAIL: Redis menu option $option TIME_REDIS=$enabled ignored failure" >&2; exit 1
      fi
      [[ ! -e "$scratch/unexpected-restart" ]] || exit 1
    done
  done
) || exit 1
echo 'PASS: parent activation failures stop before later work, including timestamp/tee pipelines'

# Shared callers retain C compatibility without replacing package optimization.
(
  set +e
  source "$repo/inc/gcc.inc"
  CENTMINLOGDIR="$scratch" DT=test INITIALINSTALL=y NGX_LDMOLD=n
  GENERAL_DEVTOOLSETGCC=y DEVTOOLSETFIFTTEEN=y
  CENTOS_SIX=0 CENTOS_SEVEN=0 CENTOS_EIGHT=8 CENTOS_NINE=0 CENTOS_TEN=0
  CPUS=2 MARCH_TARGET=x86-64 march_flag=x86-64 MARCH_TARGETNATIVE=y
  gcc() { printf '  -march x86-64\n  -msse [enabled]\n'; }
  cat() { if [[ "$1" = /proc/cpuinfo ]]; then echo 'vendor_id : AuthenticAMD'; else command cat "$@"; fi; }
  enable_gcc_toolset15() { echo selected >> "$scratch/selected"; }
  enable_devtoolset >/dev/null 2>&1 || exit 1
  [[ "$COMPILER_CFLAGS" = *-std=gnu17* && "$CXXFLAGS" != *-std=gnu17* ]] || exit 1
  before=$(wc -l < "$scratch/selected")
  enable_devtoolset >/dev/null 2>&1 || exit 1
  [[ "$COMPILER_CFLAGS" != *'-std=gnu17'*'-std=gnu17'* ]] || exit 1
  # Re-selecting Clang must clear GCC-only dialect flags, without GCC activation.
  CENTOS_EIGHT=0
  for selection in y Y; do
    COMPILER_CFLAGS=-std=gnu17
    enable_devtoolset "$selection" >/dev/null 2>&1 || exit 1
    [[ "$CC" = *clang* && "$COMPILER_CFLAGS" != *-std=gnu17* ]] || exit 1
  done
  [[ $(wc -l < "$scratch/selected") -eq $((before+1)) ]] || exit 1
  # A GCC15 opt-out on EL9 retains its existing system-compiler path.
  CENTOS_NINE=9 DEVTOOLSETFIFTTEEN=n
  enable_devtoolset >/dev/null 2>&1 || exit 1
  [[ "$COMPILER_CFLAGS" != *-std=gnu17* ]] || exit 1
  # Unsupported OS/general opt-out must not retain any prior selection.
  CENTOS_NINE=0 GENERAL_DEVTOOLSETGCC=n COMPILER_CFLAGS=-std=gnu17
  enable_devtoolset >/dev/null 2>&1 || exit 1
  [[ -z "$COMPILER_CFLAGS" ]] || exit 1
  for selection in n N; do
    COMPILER_CFLAGS=-std=gnu17
    enable_devtoolset "$selection" >/dev/null 2>&1 || exit 1
    [[ "$CC" = gcc && -z "$COMPILER_CFLAGS" ]] || exit 1
  done
  GENERAL_DEVTOOLSETGCC=y DEVTOOLSETFIFTTEEN=y CENTOS_EIGHT=8
  enable_gcc_toolset15() { return 9; }
  COMPILER_CFLAGS=-std=gnu17
  if enable_devtoolset N; then exit 1; else [[ $? -eq 1 && -z "$COMPILER_CFLAGS" ]] || exit 1; fi
  enable_clang() { return 7; }
  COMPILER_CFLAGS=-std=gnu17
  if enable_devtoolset Y; then exit 1; else [[ $? -eq 7 && -z "$COMPILER_CFLAGS" ]] || exit 1; fi
) || { echo 'FAIL: compiler repeat/Clang/opt-out compatibility flags'; exit 1; }

# Every selected compression OS uses GCC15 once, even when older toolsets exist.
(
  source "$repo/inc/compress.inc"
  enable_gcc_toolset15() { echo selected >> "$scratch/compress-selected"; }
  gcc() { echo '  -march x86-64'; }
  for old in seven eight nine ten eleven twelve thirteen fourteen; do
    eval "compgcc_$old() { echo legacy >> \"$scratch/compress-legacy\"; }"
  done
  for os in 7 8 9 10; do
    for selection in y n; do
      rm -f "$scratch/compress-selected" "$scratch/compress-legacy"
      CENTOS_EIGHT=0 CENTOS_NINE=0 CENTOS_TEN=0
      case "$os" in 8) CENTOS_EIGHT=8;; 9) CENTOS_NINE=9;; 10) CENTOS_TEN=10;; esac
      DEVTOOLSETFIFTTEEN=$selection COMPILER_CFLAGS=-std=gnu17
      compressmenu_notice >/dev/null
      [[ -z "$COMPILER_CFLAGS" ]]
      if [[ "$selection" = y ]] && { [[ "$os" = 8 || "$os" = 9 ]] || [[ "$os" = 10 && "$branch" = 141.00beta01 ]]; }; then
        [[ $(wc -l < "$scratch/compress-selected") -eq 1 && ! -e "$scratch/compress-legacy" ]]
      else
        [[ ! -e "$scratch/compress-selected" && -s "$scratch/compress-legacy" ]]
      fi
    done
  done
  CENTOS_EIGHT=8 DEVTOOLSETFIFTTEEN=y
  enable_gcc_toolset15() { return 7; }
  if compressmenu_notice >/dev/null; then echo 'FAIL: compression fell back after GCC15 failure'; exit 1; fi
)

# Execute leaf functions with failed activation, before downloading or installing.
(
  set +e
  cecho() { :; }; check_devtoolset_php() { :; }
  PHP_INSTALL=y PHPIMAGICK=y PHPMONGODB=y PHPSWOOLE=y PHPMAILPARSE=y
  PHPZIP=y PHPZOPFLI=y PHP_BROTLI=y PHP_LZFOUR=y PHP_LZF=y PHP_ZOPFLI=y PHP_ZSTD=y
  NGINX_ZSTD=y NGINX_GEOIPTWOLITE=y INITIALINSTALL=y DIR_TMP="$scratch"
  CLANG_MEMCACHED=n CLANG_PHP=n CLANG=n
  CENTOS_EIGHT=8 CENTOS_NINE=0 CENTOS_TEN=0 CENTOS_SEVEN=0
  enable_devtoolset() { return 7; }
  yum() { touch "$scratch/unexpected-install"; }
  for pair in 'memcached_install funct_memcachedreinstall' 'redis redisinstall' 'imagick_install imagickinstall' 'imagick_install libheif_install' 'mongodb mongodbinstall' 'swoole swooleinstall' 'mailparse mailparseinstall' 'geoip geoipphpext' 'geoip geoiptwolite_phpext_install' 'zip zip_php_install' 'zopfli zopfliinstall' 'zstd_nginx nginx_zstd_setup' 'compress_php php_ext_brotli' 'compress_php php_ext_lzfour' 'compress_php php_ext_lzf' 'compress_php php_ext_zopfli' 'compress_php php_ext_zstd'; do
    read -r file fn <<< "$pair"
    source "$repo/inc/$file.inc"
    if "$fn" >/dev/null 2>&1; then echo "FAIL: $fn ignored activation failure"; exit 1; fi
  done
  [[ ! -e "$scratch/unexpected-install" ]] || exit 1
  # Memcached's explicit Clang selector must bypass enable_devtoolset.
  prefix=$(sed -n '/^funct_memcachedreinstall() {/,/^PHPEXTDIRD=/p' "$repo/inc/memcached_install.inc" | sed '$d')
  eval "$prefix
}"
  CLANG_MEMCACHED=y
  enable_clang() { echo clang > "$scratch/memcached-clang"; }
  funct_memcachedreinstall || exit 1
  [[ -s "$scratch/memcached-clang" ]] || exit 1
) || exit 1

# Inspect the actual configure environments, not a duplicate set of flags.
(
  mkdir -p "$scratch/configure-check"
  cd "$scratch/configure-check"
  cat > configure <<'EOF'
#!/bin/bash
[[ "$CFLAGS" = *-std=gnu17* && "$CFLAGS" = *-O2* && "$CFLAGS" != *-O3* ]] || exit 1
[[ "$CXXFLAGS" = '-fPIC -O2' && "$CPPFLAGS" = '-D_FORTIFY_SOURCE=2' ]] || exit 1
[[ "$LDFLAGS" = '-Wl,-z,relro,-z,now -pie'* ]] || exit 1
EOF
  chmod +x configure
  COMPILER_CFLAGS=' -Wimplicit-fallthrough=0 -std=gnu17'
  for file in compress_php zip zopfli memcached_install redis imagick_install swoole mailparse geoip; do
    commands=$(grep -E '^[[:space:]]*(PKG_CONFIG_PATH=[^ ]+ )?CFLAGS=.*\./configure' "$repo/inc/$file.inc")
    [[ -n "$commands" ]]
    while IFS= read -r command; do
      eval "$command" || { echo "FAIL: $file final configure flags"; exit 1; }
    done <<< "$commands"
  done
)
echo 'PASS: caller failure guards, final C/C++ flags, repeat selection, Clang, opt-out and compression OS gates'

# Success paths must not become fatal merely because callers now check status.
(
  mkdir -p "$scratch/ini-check"
  CONFIGSCANDIR="$scratch/ini-check"
  printf "extension_dir='/new/extensions'\n" > "$scratch/php-config"
  autodetect=$(sed -n '/^autodetectinstallextensions() {/,/^}/p' "$repo/inc/php_upgrade.inc")
  autodetect=${autodetect//\/usr\/local\/bin\/php-config/$scratch/php-config}
  eval "$autodetect"
  cecho() { :; }; figlet() { :; }
  # Run the GNU sed edit portably on macOS too.
  sed() {
    if [[ "$1" = -i ]]; then
      command sed "$2" "$3" > "$scratch/edited-ini" && mv "$scratch/edited-ini" "$3"
    else command sed "$@"; fi
  }
  PHPEXTDIRDOLD=/old/extensions
  autodetectinstallextensions >/dev/null
  printf 'extension=/old/extensions/a.so\n' > "$CONFIGSCANDIR/a.ini"
  printf 'extension=/old/extensions/b.so\n' > "$CONFIGSCANDIR/b space.ini"
  autodetectinstallextensions >/dev/null
  grep -q '/new/extensions/a.so' "$CONFIGSCANDIR/a.ini"
  grep -q '/new/extensions/b.so' "$CONFIGSCANDIR/b space.ini"
  sed() { if [[ "$1" = -i ]]; then return 7; else command sed "$@"; fi; }
  if autodetectinstallextensions >/dev/null; then echo 'FAIL: INI edit failure ignored'; exit 1; fi
  zstd_setup=$(command sed -n '/^nginx_zstd_setup() {/,/^}/p' "$repo/inc/zstd_nginx.inc")
  zstd_setup=${zstd_setup//\/usr\/local\/nginx\/conf\/nginx.conf/$scratch/absent-nginx.conf}
  eval "$zstd_setup"
  NGINX_ZSTD=n
  nginx_zstd_setup
)
# x265/libheif must keep the compiler selected by the shared helper.
(
  libheif=$(sed -n '/^libheif_install() {/,/^}/p' "$repo/inc/imagick_install.inc")
  [[ "$libheif" != *'export CC="gcc"'* && "$libheif" != *'export CXX="g++"'* ]]
  CC='ccache /usr/bin/clang -ferror-limit=0' CXX='ccache /usr/bin/clang++ -ferror-limit=0'
  cmake() { [[ "$CC" = *clang* && "$CXX" = *clang++* && "$*" != *CMAKE_C_COMPILER=gcc* && "$*" != *CMAKE_CXX_COMPILER=g++* ]]; }
  eval "$(printf '%s\n' "$libheif" | grep '^[[:space:]]*cmake -G')"
)
echo 'PASS: empty/multiple extension INIs, disabled zstd and libheif compiler preservation'
