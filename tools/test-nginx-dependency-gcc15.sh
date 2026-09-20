#!/usr/bin/env bash
# Host-independent dispatch/failure/flag checks. No packages or system paths modified.
set -e
root=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
export root work
python3 - <<'PY'
import os
from pathlib import Path
root, work = Path(os.environ['root']), Path(os.environ['work'])
for name in ('gcc', 'zlib', 'brotli'):
    s=(root/'inc'/f'{name}.inc').read_text()
    s=s.replace('/opt/rh/',str(work)+'/opt/rh/').replace('/usr/local/',str(work)+'/local/')
    s=s.replace('/usr/bin/mold',str(work)+'/mold')
    (work/f'{name}.inc').write_text(s)
# Simulate installed older toolsets alongside requested 15. Sourcing them is a failure.
for v in (9,10,11):
    d=work/f'opt/rh/gcc-toolset-{v}'
    (d/'root/usr/bin').mkdir(parents=True)
    for tool in ('gcc','g++'): (d/'root/usr/bin'/tool).touch()
    (d/'enable').write_text('echo DOWNGRADE >> "$work/trace"; return 99\n')
PY
source "$work/zlib.inc"
source "$work/brotli.inc"
uname() { echo x86_64; }
fail() { echo "FAIL: $*" >&2; exit 1; }
fakecc() { cat >/dev/null; printf '%s\n' "$*" >> "$work/cc"; }
export -f fakecc
enable_gcc_toolset15() { echo activate >> "$work/trace"; CC=fakecc; CXX=fakecc; }
CENTOS_NINE=9 CLANG=n NGINX_DEVTOOLSETGCC=y DEVTOOLSETFIFTTEEN=y INITIALINSTALL=y
CFLAGS=-DC_KEEP CXXFLAGS=-DCXX_KEEP
nginx_dependency_compiler
[[ "$NGX_DEP_GCC15" = y && "$CC" = fakecc && "$LDFLAGS" = *-fuse-ld=bfd* && "$CXX" = "$CC" ]] || fail selection
[[ "$CFLAGS" = *-std=gnu17* && "$CXXFLAGS" = -DCXX_KEEP ]] || fail language-flags
[[ $(grep -c activate "$work/trace") = 1 ]] || fail absent-toolset-dispatch
nginx_dependency_compiler
[[ "$CFLAGS" != *'-std=gnu17 -std=gnu17'* ]] || fail repeated-flags
for mode in clang optout nginx-optout unsupported; do
 (
  CC=sentinel CXX=sentinel
  case $mode in clang) CLANG=y;; optout) DEVTOOLSETFIFTTEEN=n;; nginx-optout) NGINX_DEVTOOLSETGCC=n;; unsupported) CENTOS_NINE=0;; esac
  nginx_dependency_compiler
  [[ "$CC" = sentinel && "$CXX" = sentinel && "$NGX_DEP_GCC15" = n ]] || fail "$mode"
 )
done
(
 CENTOS_NINE=0 CENTOS_TEN=10
 nginx_dependency_compiler
 if grep -q 'CENTOS_TEN' "$root/inc/zlib.inc"; then [[ "$NGX_DEP_GCC15" = y ]]; else [[ "$NGX_DEP_GCC15" = n ]]; fi
) || fail el10-scope
(
 enable_gcc_toolset15() { return 42; }
 for fn in nginx_dependency_compiler nginxzlib_install install_stdzlib install_cfzlib zlibng_install ngxbrotli_download; do
  NGINX_LIBBROTLI=y
  if "$fn"; then fail "activation failure masked by $fn"; fi
 done
)
(
 fakecc() { return 1; }
 if nginx_dependency_compiler; then fail linker-failure; fi
)
(
 NGX_LDMOLD=y
 if nginx_dependency_compiler 2>/dev/null; then fail missing-mold; fi
 ln -s /bin/sh "$work/mold"
 nginx_dependency_compiler
 [[ "$LDFLAGS" = *-fuse-ld=mold ]] || fail requested-mold
)
# The real shared helper must request installation when the toolset is absent.
(
 source "$work/gcc.inc"
 yum() { printf '%s\n' "$*" > "$work/packages"; return 1; }
 if nginx_dependency_compiler 2>/dev/null; then fail missing-packages; fi
 grep -q 'gcc-toolset-15-gcc gcc-toolset-15-gcc-c++ gcc-toolset-15-binutils gcc-toolset-15-runtime' "$work/packages" || fail package-request
)
# Dispatch all variants without touching their install paths.
(
 gcc() { echo ' -mpclmul [enabled]'; }
 cat() { if [[ "$1" = /proc/cpuinfo ]]; then echo sse4_2; else command cat "$@"; fi; }
 install_cfzlib() { echo cf > "$work/dispatch"; }
 zlibng_install() { echo ng > "$work/dispatch"; }
 install_stdzlib() { echo std > "$work/dispatch"; }
 NGINX_ZLIBCUSTOM=y
 for variant in cf ng std; do
  CLOUDFLARE_ZLIB=n NGINX_ZLIBNG=n
  case $variant in cf) CLOUDFLARE_ZLIB=y;; ng) NGINX_ZLIBNG=y;; esac
  nginxzlib_install
  [[ $(<"$work/dispatch") = "$variant" ]] || fail "dispatch $variant"
 done
 install_cfzlib() { return 1; }
 CLOUDFLARE_ZLIB=y
 if nginxzlib_install; then fail build-failure; fi
)
# Exercise the actual Brotli body with a fake checkout/build, including the CMake argv.
(
 DIR_TMP="$work/src" NGINX_LIBBROTLI=y NGINX_BROTLI_NEW_METHOD=y
 DEVTOOLSETNINE=y DEVTOOLSETTEN=y DEVTOOLSETELEVEN=y
 mkdir -p "$DIR_TMP" "$work/local/nginx/conf"
 touch "$work/local/nginx/conf/nginx.conf"
 cmake_alternatives() { :; }
 curl() { echo 'HTTP/2 200'; }
 git() { if [[ "$1" = clone ]]; then mkdir -p ngx_brotli/deps/brotli/out; touch ngx_brotli/deps/brotli/out/stale; fi; }
 brdep_update() { :; }
 cmake() { [[ ! -f stale ]] || fail stale-cmake; printf '%s\n' "$@" >> "$work/cmake"; }
 ldd() { :; }
 sed() { :; }
 ngxbrotli_download >/dev/null 2>&1 || fail brotli-build
 [[ "$CC" = fakecc && "$LDFLAGS" = *-fuse-ld=bfd* ]] || fail brotli-compiler
 ! grep -q DOWNGRADE "$work/trace" || fail downgrade
 grep '^\-DCMAKE_C_FLAGS=' "$work/cmake" | grep -q -- '-DC_KEEP -std=gnu17' || fail cmake-c
 grep '^\-DCMAKE_CXX_FLAGS=' "$work/cmake" | grep -q -- '-DCXX_KEEP' || fail cmake-cxx
 ! grep '^\-DCMAKE_CXX_FLAGS=' "$work/cmake" | grep -q -- '-std=gnu17' || fail cxx-gnu17
)
# A failed dependency must survive tee and stop the surrounding caller.
(
 nginxzlib_install() { return 73; }
 { nginxzlib_install; } 2>&1 | tee "$work/pipeline.log"
 if [[ "${PIPESTATUS[*]}" = "0 0" ]]; then fail pipeline-masked; fi
)
# Verify pipeline status is checked immediately at each production call site.
python3 - <<'PY'
import os, subprocess
from pathlib import Path
root=Path(os.environ['root'])
for file, marker in [('nginx_install','nginx-install-zlib_${DT}.log'),('nginx_upgrade','nginx-upgrade-zlib_${DT}.log'),('nginx_configure','_nginx_brotli.log')]:
    lines=(root/'inc'/f'{file}.inc').read_text().splitlines()
    i=next(i for i,l in enumerate(lines) if marker in l)
    assert 'PIPESTATUS[*]' in lines[i+1], file
s=(root/'inc/nginx_configure.inc').read_text()
guard=next(l for l in s.splitlines() if l.strip().startswith('geoiptwolite_install'))
result=subprocess.run(['bash','-c','geoiptwolite_install() { return 42; }; caller() { '+guard+'\necho UNREACHED; }; caller'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,universal_newlines=True)
assert result.returncode != 0 and 'UNREACHED' not in result.stdout, 'GeoIP failure swallowed'
PY
echo 'PASS: dependency GCC15 dispatch, flags, downgrade guards and failure propagation'
