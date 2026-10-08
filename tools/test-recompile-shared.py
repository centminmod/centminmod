#!/usr/bin/env python3
"""Run actual shell functions with temporary archives, locklists and command mocks."""
import io
import os
import pathlib
import re
import shlex
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time

REPO = pathlib.Path(__file__).resolve().parents[1]


def run(code, work, expected=0):
    env = {key: os.environ[key] for key in ('PATH', 'TMPDIR') if key in os.environ}
    env['LC_ALL'] = 'C'
    result = subprocess.run(['bash', '-c', code], cwd=work, env=env, text=True, capture_output=True)
    assert result.returncode == expected, (result.returncode, result.stdout, result.stderr)
    return result


def function(source, name):
    start = source.index(name + '() {')
    end = re.search(r'^}(?:[ \t]*#[^\n]*)?$', source[start:], re.M)
    return source[start:start + end.end()]


with tempfile.TemporaryDirectory(prefix='centmin-shared-') as directory:
    work = pathlib.Path(directory)
    startup = work / 'bash-env.sh'
    marker = work / 'startup-marker'
    startup.write_text('touch ' + shlex.quote(str(marker)) + '\n')
    inherited = os.environ.get('BASH_ENV')
    try:
        os.environ['BASH_ENV'] = str(startup)
        run('[[ -z ${BASH_ENV+x} ]]', work)
        assert not marker.exists()
    finally:
        if inherited is None:
            os.environ.pop('BASH_ENV', None)
        else:
            os.environ['BASH_ENV'] = inherited
    src = work / 'src'
    nginx = src / 'nginx-1.31.6'
    (nginx / 'src/core').mkdir(parents=True)
    (nginx / 'src/http').mkdir(parents=True)
    (nginx / 'src/core/nginx.h').write_text('#define nginx_version  1031006\n')
    (nginx / 'src/http/ngx_http_script.h').write_text('ngx_http_script_complex_value_end_code\n')
    patchdir = work / 'patches/ngx-devel-kit'
    patchdir.mkdir(parents=True)
    (patchdir / 'ndk-nginx-complex-value-end.patch').touch()
    with tarfile.open(src / 'ndk.tar.gz', 'w:gz') as archive:
        member = tarfile.TarInfo('ndk-0.3.4/')
        member.type = tarfile.DIRTYPE
        member.mode = 0o755
        archive.addfile(member)
        data = b'unpatched ndk\n'
        member = tarfile.TarInfo('ndk-0.3.4/src/ndk_rewrite.c')
        member.size = len(data)
        archive.addfile(member, io.BytesIO(data))
    nginx_source = (REPO / 'inc/nginx_patch.inc').read_text()
    # Relocate historical helpers, but test NDK's actual DIR_TMP handling intact.
    nginx_functions = nginx_source.replace('/svr-setup', str(src)) + '\n' + function(nginx_source, 'ngx_develkit_patch')
    helpers = ['ngx_renegotiate_patch', 'ngx_maxprotocol_patch', 'ngx_upstreamcheck_patch',
               'ngx_gzip_multi_status_patch', 'ngx_hpack_patch', 'iouring_patch',
               'ocsp_ttl_override', 'ngx_srcache_patch', 'ngx_headers_more_patch',
               'aws_lc_patch', 'http2_shutdown_fix_patch']
    setup = nginx_functions + '\n' + '\n'.join(name + '() { :; }' for name in helpers) + f'''
DIR_TMP={shlex.quote(str(src))}
CUR_DIR={shlex.quote(str(work))}
CENTMINLOGDIR={shlex.quote(str(work))}
DT=test ngver=1.31.6 NGX_DEVELKITLINKFILE=ndk.tar.gz NGINXPATCH_DELAY=0
NGINX_HTTP2=n NGINX_SPDYPATCHED=n NGINX_DYNAMICTLS=n OPENSSL_VERSION=3.5.0 ipv_forceopt=4
cecho() {{ :; }}
'''
    sentinel = src / 'keep-cache'
    sentinel.touch()
    run(setup + '\nrm() { echo UNEXPECTED_DELETE; return 99; }; tar() { return 2; }; ngx_develkit_patch', work, 2)
    assert sentinel.exists()
    for unsafe in ('', './', '../ndk/', '/ndk/', 'ndk'):
        result = run(setup + '\nrm() { echo UNEXPECTED_DELETE; return 99; }; '
                     'tar() { printf "%s\\n" ' + shlex.quote(unsafe) + '; }; ngx_develkit_patch', work, 1)
        assert 'UNEXPECTED_DELETE' not in result.stdout
    run(setup + '\ntar() { if [[ "$1" = -tzf ]]; then command tar "$@"; else return 73; fi; }; ngx_develkit_patch', work, 73)
    result = run(setup + '''
patch() { echo PATCH_FAILURE; return 72; }
aws_lc_patch() { echo UNEXPECTED_CONTINUATION; }
patchnginx
''', work, 72)
    assert 'PATCH_FAILURE' in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
    run(setup + '\npatch() { return 0; }; patchnginx', work)
    run(setup + '\npatch() { return 0; }; tee() { cat >/dev/null; return 74; }; patchnginx', work, 74)
    for root_cache in ('/', '/private/tmp/../..'):
        result = run(setup + '\nDIR_TMP=' + shlex.quote(root_cache)
                     + '; rm() { echo UNEXPECTED_DELETE; return 99; }; ngx_develkit_patch', work, 1)
        assert 'UNEXPECTED_DELETE' not in result.stdout
    header = nginx / 'src/http/ngx_http_script.h'
    header.unlink()
    result = run(setup + '\nrm() { echo UNEXPECTED_DELETE; return 99; }; ngx_develkit_patch', work, 1)
    assert 'UNEXPECTED_DELETE' not in result.stdout and 'readable nginx source headers' in result.stdout
    # The same source probe disables patching on older nginx.
    (nginx / 'src/http/ngx_http_script.h').write_text('old nginx\n')
    run(setup + '\npatch() { return 72; }; patchnginx', work)
    free = src / 'freenginx-1.30.1'
    (free / 'src/core').mkdir(parents=True)
    (free / 'src/http').mkdir(parents=True)
    (free / 'src/core/nginx.h').write_text('#define nginx_version  1030001\n')
    (free / 'src/http/ngx_http_script.h').write_text('old freenginx\n')
    run(setup + '\nngver=; FREENGINX_INSTALL=y; FREENGINX_VERSION=1.30.1; NGINX_VERSION=1.31.6; patch() { return 72; }; ngx_develkit_patch', work)
    # Exercise both existing download gates with wget recording its arguments.
    (nginx / 'src/core/nginx.h').write_text('#define nginx_version  1011004\n')
    for spdy, tls in (('y', 'n'), ('n', 'y')):
        result = run(setup + f'''
NGINX_SPDYPATCHED={spdy} NGINX_DYNAMICTLS={tls}
ngx_dynamic_tls_message() {{ :; }}
curl() {{ echo 'HTTP/1.1 200'; }}
wget() {{ echo WGET "$@"; }}
patchnginx
''', work)
        assert 'WGET -4v https://' in result.stdout and '--no-check-certificate' not in result.stdout
    print('PASS: NDK listing/path/extraction guards, patch and tee failures, feature probe and TLS gates')

    lock_functions = (REPO / 'tools/php-libs-versionlock.sh').read_text().split('if [[ "$1" != \'check\'')[0]
    lockfile = work / 'locks.list'
    lockconf = work / 'versionlock.conf'
    lockconf.write_text('locklist=' + str(lockfile) + '\n')
    original = ('# foreign comment\nuser-package-0:9.0-1.*\n'
                '# centminmod php-libs-versionlock begin\n'
                'libavif-0:0.11-1.*\nlibavif-devel-0:0.11-1.*\n'
                '# centminmod php-libs-versionlock end\n')
    # The script is Linux-only; use GNU cp installed as gcp on macOS.
    cp = shutil.which('gcp') or shutil.which('cp')
    lock_setup = lock_functions + f'''
LOCK_CONF={shlex.quote(str(lockconf))}
cp() {{ command {shlex.quote(cp)} "$@"; }}
rpm() {{ shift 3; for name in "$@"; do printf '%s-0:%s-1.*\\n' "$name" "$INSTALLED_VERSION"; done; }}
INSTALLED_VERSION=0.11
dnf_plugin_paths() {{ echo /etc/dnf/plugins; }}
'''
    for status in (0, 42):
        for version in ('0.11', '1.1'):
            lockfile.write_text(original)
            lockfile.chmod(0o640)
            run(lock_setup + f'\ndnf() {{ INSTALLED_VERSION={version}; return {status}; }}; do_update', work, status)
            locked = lockfile.read_text()
            assert locked.startswith('# foreign comment\nuser-package-0:9.0-1.*\n')
            assert f'libavif-0:{version}-1.*\n' in locked and f'libavif-devel-0:{version}-1.*\n' in locked
            assert lockfile.stat().st_mode & 0o777 == 0o640
    for command in ('mktemp', 'awk', 'rpm', 'cp', 'mv'):
        lockfile.write_text(original)
        modified = lockfile.stat().st_mtime_ns
        mock = f'{command}() {{ return 76; }}'
        if command == 'awk':
            mock = 'awk() { if [[ "$1" = -F= ]]; then command awk "$@"; else return 76; fi; }'
        run(lock_setup + '\n' + mock + '; write_block libavif', work, 76)
        assert lockfile.read_text() == original, command
        assert lockfile.stat().st_mtime_ns == modified, command
        assert not list(work.glob('locks.list.??????')), command
    lockfile.unlink()
    run(lock_setup + '\ntouch() { return 76; }; write_block libavif', work, 76)
    assert not lockfile.exists()
    lockfile.write_text(original)
    run(lock_setup + '\necho() { [[ "$1" = "$LOCK_END" ]] && return 76; builtin echo "$@"; }; write_block libavif', work, 76)
    assert lockfile.read_text() == original
    for action in ('do_lock', 'do_unlock', 'do_update'):
        lockfile.write_text(original)
        result = run(lock_setup + '''
ensure_plugin() { return 0; }; do_check() { return 0; }; collect_packages() { echo libavif; }
write_block() { return 76; }; dnf() { echo UNEXPECTED_UPDATE; }
''' + action, work, 76)
        assert 'UNEXPECTED_UPDATE' not in result.stdout
    for status, expected in ((0, 76), (42, 42)):
        lockfile.write_text(original)
        calls = work / 'rpm-query-count'
        calls.write_text('0')
        result = run(lock_setup + f'''
dnf() {{ return {status}; }}
rpm() {{ count=$(cat {shlex.quote(str(calls))}); count=$((count + 1)); echo "$count" > {shlex.quote(str(calls))}; [[ "$count" -gt 1 ]] && return 76; shift 3; for name in "$@"; do printf '%s-0:%s-1.*\\n' "$name" "$INSTALLED_VERSION"; done; }}
do_update
''', work, expected)
        assert 'failed to restore PHP library version locks' in result.stderr
        assert lockfile.read_text().startswith('# foreign comment\nuser-package-0:9.0-1.*\n')
        assert 'libavif-0:0.11-1.*\n' in lockfile.read_text()
    target = work / 'lock-target'
    lockfile.unlink()
    target.write_text(original)
    target.chmod(0o640)
    lockfile.symlink_to(target)
    run(lock_setup + '\nwrite_block libavif', work)
    assert lockfile.is_symlink() and target.stat().st_mode & 0o777 == 0o640
    assert 'libavif-0:0.11-1.*\n' in target.read_text()
    print('PASS: successful/failed/partial DNF relock, foreign locks, writer errors and symlink/mode preservation')

    ledger = pathlib.Path(str(lockfile) + '.centminmod-php-libs-owned')
    foreign = 'user-package-0:9.0-1.*\nImageMagick-0:7-1.*\n'
    records = lambda version: f'libavif-0:{version}-1.*\nlibavif-devel-0:{version}-1.*\n'
    collector_setup = lock_setup + '''
do_check() { return 0; }
php_libs() { echo /test/libavif.so; }
base_repos() { echo baseos; }
rpm() {
  case "$1" in
    -qf) echo libavif.src.rpm ;;
    -qa) printf 'libavif.src.rpm\tlibavif\nlibavif.src.rpm\tlibavif-devel\n' ;;
    -q) shift 3; for name in "$@"; do printf '%s-0:%s-1.*\\n' "$name" "$INSTALLED_VERSION"; done ;;
  esac
}
dnf() {
  [[ "$1" = -y ]] && { INSTALLED_VERSION=1.1; return 0; }
  printf 'libavif\tepel\nlibavif-devel\tepel\n'
}
'''

    def seed_locks():
        ledger.unlink(missing_ok=True)
        lockfile.write_text(foreign + original[original.index('# centminmod php-libs-versionlock begin'):])
        run(collector_setup + '\nwrite_block libavif libavif-devel', work)

    def strip_comments():
        # DNF's delete command retains parsed entries and drops every comment.
        lockfile.write_text(''.join(line + '\n' for line in lockfile.read_text().splitlines() if line and not line.startswith('#')))

    seed_locks()
    assert ledger.read_text() == records('0.11') and not pathlib.Path(str(target) + '.centminmod-php-libs-owned').exists()
    strip_comments()
    run(collector_setup + '\ndo_lock; do_update', work)
    assert ledger.read_text() == records('1.1') and lockfile.read_text().startswith(foreign)
    strip_comments()
    result = run(collector_setup + '\nblock_names; do_unlock', work)
    assert 'libavif\nlibavif-devel\n' in result.stdout and lockfile.read_text() == foreign

    seed_locks()
    strip_comments()
    different = 'libavif-0:9-1.*\n'
    lockfile.write_text(lockfile.read_text() + different)
    run(collector_setup + '\nwrite_block libavif libavif-devel; do_unlock', work)
    assert lockfile.read_text() == foreign + different

    seed_locks()
    collision = 'libavif-0:1.1-1.*\n'
    lockfile.write_text(lockfile.read_text() + collision)
    run(collector_setup + '\ndo_update', work)
    assert collision not in ledger.read_text() and lockfile.read_text().count(collision) == 1
    strip_comments()
    run(collector_setup + '\ndo_unlock', work)
    assert lockfile.read_text() == foreign + collision

    ledger.unlink()
    lockfile.write_text(foreign + records('0.11'))
    run(collector_setup + '\ndo_lock; do_unlock', work)
    assert lockfile.read_text() == foreign + records('0.11') and ledger.read_text() == ''
    for action in ('do_lock', 'do_list'):
        seed_locks()
        before = lockfile.read_bytes(), ledger.read_bytes()
        run(collector_setup + '\ndnf() { return 73; }; ' + action, work, 73)
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
        run(collector_setup + '\nrpm() { [[ "$1" = -qa ]] && return 73; [[ "$1" = -qf ]] && echo libavif.src.rpm; }; ' + action, work, 73)
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    seed_locks()
    run(collector_setup + '\nphp_libs() { return 0; }; do_lock', work)
    assert lockfile.read_text() == foreign and ledger.read_text() == ''
    seed_locks()
    run(collector_setup + '\nrpm() { [[ "$1" = -qf ]] && echo libavif.src.rpm; return 0; }; do_lock', work)
    assert lockfile.read_text() == foreign and ledger.read_text() == ''
    seed_locks()
    before = lockfile.read_bytes(), ledger.read_bytes()
    queries = work / 'repoquery-count'
    queries.write_text('0')
    run(collector_setup + f'''
dnf() {{ count=$(cat {shlex.quote(str(queries))}); count=$((count + 1)); echo "$count" > {shlex.quote(str(queries))}; [[ "$count" -gt 1 ]] && return 73; printf 'libavif\\tepel\\nlibavif-devel\\tepel\\n'; }}
do_list
''', work, 73)
    assert (lockfile.read_bytes(), ledger.read_bytes()) == before

    for commit in (1, 2, 3):
        seed_locks()
        strip_comments()
        before = lockfile.read_bytes(), ledger.read_bytes()
        run(collector_setup + f'''
INSTALLED_VERSION=1.1 MOVE_COUNT=0
mv() {{ MOVE_COUNT=$((MOVE_COUNT + 1)); [[ "$MOVE_COUNT" -eq {commit} ]] && return 76; command mv "$@"; }}
write_block libavif libavif-devel
''', work, 76)
        if commit < 3:
            assert lockfile.read_bytes() == before[0]
        if commit == 1:
            assert ledger.read_bytes() == before[1]
        strip_comments()
        live_version = '0.11' if commit < 3 else '1.1'
        result = run(collector_setup + '\nowned_entries', work)
        assert result.stdout == records(live_version)
        run(collector_setup + '\nINSTALLED_VERSION=1.1; write_block libavif libavif-devel', work)
        assert ledger.read_text() == records('1.1')
        strip_comments()
        run(collector_setup + '\ndo_unlock', work)
        assert lockfile.read_text() == foreign
        assert not list(work.glob('*.centminmod-php-libs-owned.??????'))
    seed_locks()
    before = lockfile.read_bytes(), ledger.read_bytes()
    run(collector_setup + '\nawk() { [[ "$1" = *\'foreign[$0]\'* ]] && return 76; command awk "$@"; }; write_block libavif', work, 76)
    assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    print('PASS: exact ownership survives DNF comment rewrite, preserves foreign pins, and recovers all publication faults; failed queries preserve locks')

    # DNF's real EL10 header contains both whitespace and an interpreter flag.
    bindir = work / 'dnf-bin'
    bindir.mkdir()
    interpreter = bindir / 'python-fixture'
    interpreter.write_text('#!/bin/bash\n[[ "$1" = -s && "$2" = -c ]] || exit 75\nprintf "/etc/dnf/plugins,/admin/plugins\\n"\n')
    interpreter.chmod(0o755)
    (bindir / 'dnf').write_text('#! ' + str(interpreter) + ' -s\n')
    (bindir / 'dnf').chmod(0o755)
    result = run(lock_functions + '\nPATH=' + shlex.quote(str(bindir))
                 + ':$PATH; [[ $(command -v dnf) = ' + shlex.quote(str(bindir / 'dnf'))
                 + ' ]] || exit 75; dnf_plugin_paths', work)
    assert result.stdout == '/etc/dnf/plugins,/admin/plugins\n'
    interpreter.write_text('#!/bin/bash\nexit 73\n')
    run(lock_functions + '\nPATH=' + shlex.quote(str(bindir))
        + ':$PATH; dnf_plugin_paths', work, 73)

    # Execute the actual Python configuration/canonical-path gate. The local
    # DNF API fixture reads real private config files through ConfigParser.
    admin_plugins = work / 'admin-plugins'
    admin_plugins.mkdir()
    (work / 'dnf.py').write_text(f'''
import configparser
class Conf:
    pluginconfpath = [{str(admin_plugins)!r}]
    def read(self): pass
class Base:
    def __init__(self): self.conf = Conf()
class Plugin:
    @classmethod
    def read_config(cls, conf):
        parser = configparser.ConfigParser()
        parser.read([path + '/' + cls.name + '.conf' for path in conf.pluginconfpath])
        return parser
''')
    update_marker = work / 'native-dnf-update'
    fake_dnf = bindir / 'dnf'
    fake_dnf.write_text(f'#! {sys.executable} -s\nimport pathlib\npathlib.Path({str(update_marker)!r}).touch()\n')
    fake_dnf.chmod(0o755)
    active_foreign = work / 'active-foreign.list'
    active_foreign.write_text('active-user-pin-0:3-1.*\n')
    active_config = admin_plugins / 'versionlock.conf'
    active_config.write_text('[main]\nlocklist=' + str(active_foreign) + '\n')
    real_paths = function(lock_functions, 'dnf_plugin_paths')
    native_setup = lock_setup + '\nPATH=' + shlex.quote(str(bindir)) + ':$PATH\n' + real_paths
    seed_locks()
    before = lockfile.read_bytes(), ledger.read_bytes(), active_foreign.read_bytes()
    result = run(native_setup + '\ndo_update', work, 1)
    assert 'different versionlock locklist' in result.stderr and not update_marker.exists()
    assert (lockfile.read_bytes(), ledger.read_bytes(), active_foreign.read_bytes()) == before
    active_config.write_text('[main]\nenabled=1\n')
    result = run(native_setup + '\ndo_update', work, 1)
    assert 'no configured versionlock locklist' in result.stderr and not update_marker.exists()
    assert (lockfile.read_bytes(), ledger.read_bytes(), active_foreign.read_bytes()) == before
    # Configured symlink and canonical target identify the same active list.
    active_config.write_text('[main]\nlocklist=' + str(target) + '\n')
    run(native_setup + '\ndo_update', work)
    assert update_marker.exists() and lockfile.is_symlink()
    (work / 'dnf.py').unlink()

    seed_locks()
    negative = '!other-package-0:2-1.*\n'
    lockfile.write_text(lockfile.read_text() + negative)
    result = run(collector_setup + '''
dnf() {
  [[ "$1" = -y && "$2" = --setopt=pluginconfpath=/etc/dnf/plugins,* && "$3" = update ]] || return 75
  overlay=${2##*,}
  [[ $(cat "$overlay/locks.list") = $'user-package-0:9.0-1.*\\nImageMagick-0:7-1.*\\n!other-package-0:2-1.*' ]] || return 75
  [[ $(owned_entries) = $'libavif-0:0.11-1.*\\nlibavif-devel-0:0.11-1.*' ]] || return 75
  [[ $(cat "$overlay/versionlock.conf") = $'[main]\\nlocklist='"$overlay/locks.list" ]] || return 75
  INSTALLED_VERSION=1.1
}
do_update
''', work)
    assert negative in lockfile.read_text() and ledger.read_text() == records('1.1')
    seed_locks()
    forbidden_version = '!libavif-0:1.1-1.*\n'
    lockfile.write_text(lockfile.read_text() + forbidden_version)
    run(collector_setup + '\ndo_lock', work)
    assert forbidden_version in lockfile.read_text() and ledger.read_text() == records('0.11')
    assert records('0.11') in lockfile.read_text()
    seed_locks()
    before = lockfile.read_bytes(), ledger.read_bytes()
    run(collector_setup + '\ndnf_plugin_paths() { return 73; }; do_update', work, 73)
    assert (lockfile.read_bytes(), ledger.read_bytes()) == before

    # A real TERM during the actual update function leaves ownership recoverable.
    seed_locks()
    overlay_setup = f'''
mktemp() {{ if [[ "$1" = -d ]]; then command mktemp -d {shlex.quote(str(work / 'overlay.XXXXXX'))}; else command mktemp "$@"; fi; }}
'''
    run(collector_setup + overlay_setup + '\ndnf() { builtin kill -TERM "$BASHPID"; }; do_update', work, 143)
    assert ledger.read_text() == records('0.11') and records('0.11') in lockfile.read_text()
    assert not list(work.glob('overlay.??????'))
    strip_comments()
    result = run(collector_setup + '\nblock_names; do_update', work)
    assert 'libavif\nlibavif-devel\n' in result.stdout and ledger.read_text() == records('1.1')

    # Use the host's native flock syscall on the actual inherited fd9; this
    # remains a real kernel/process concurrency check on macOS and Linux.
    mutex = work / 'helper-mutex'
    flock_setup = f'''
LOCK_MUTEX={shlex.quote(str(mutex))}
flock() {{ {shlex.quote(sys.executable)} -c 'import fcntl, sys; fcntl.flock(int(sys.argv[1]), fcntl.LOCK_EX)' "${{@: -1}}"; }}
'''
    ready, release, waiting, done = (work / name for name in ('update-ready', 'update-release', 'unlock-waiting', 'unlock-done'))
    isolated_env = {key: os.environ[key] for key in ('PATH', 'TMPDIR') if key in os.environ}
    isolated_env['LC_ALL'] = 'C'

    def wait_for(path, process):
        deadline = time.monotonic() + 5
        while not path.exists() and time.monotonic() < deadline:
            assert process.poll() is None, process.communicate()
            time.sleep(0.02)
        assert path.exists(), path

    seed_locks()
    first = subprocess.Popen(['bash', '-c', collector_setup + flock_setup + overlay_setup + f'''
dnf() {{ touch {shlex.quote(str(ready))}; while [[ ! -f {shlex.quote(str(release))} ]]; do sleep 0.02; done; INSTALLED_VERSION=1.1; }}
mutate do_update
'''], cwd=work, env=isolated_env, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    second = None
    try:
        wait_for(ready, first)
        second = subprocess.Popen(['bash', '-c', collector_setup + flock_setup + f'''
flock() {{ touch {shlex.quote(str(waiting))}; {shlex.quote(sys.executable)} -c 'import fcntl, sys; fcntl.flock(int(sys.argv[1]), fcntl.LOCK_EX)' "${{@: -1}}"; }}
mutate do_unlock && touch {shlex.quote(str(done))}
'''], cwd=work, env=isolated_env, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        wait_for(waiting, second)
        time.sleep(0.1)
        assert second.poll() is None and not done.exists()
        assert ledger.read_text() == records('0.11') and records('0.11') in lockfile.read_text()
        release.touch()
        output = first.communicate(timeout=5)
        assert first.returncode == 0, output
        output = second.communicate(timeout=5)
        assert second.returncode == 0 and done.exists(), output
        assert lockfile.read_text() == foreign and ledger.read_text() == ''
    finally:
        release.touch()
        for process in (first, second):
            if process is not None and process.poll() is None:
                process.terminate()
                process.communicate(timeout=5)
    assert not list(work.glob('overlay.??????'))
    seed_locks()
    before = lockfile.read_bytes(), ledger.read_bytes()
    run(collector_setup + flock_setup + '\nflock() { return 73; }; mutate do_unlock', work, 73)
    assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    print('PASS: per-invocation DNF overlay retains foreign pins/exclusions; real TERM recovery and kernel-serialized concurrent mutations')

    # Discovery failures pass through actual php_targets/php_libs rather than
    # their collector mocks, using only owned executable fixture paths.
    phpconfig = work / 'php-config'
    phpconfig.write_text('#!/bin/bash\nexit 73\n')
    phpconfig.chmod(0o755)
    phpbin = work / 'php-fixture'
    phpbin.touch()
    phpbin.chmod(0o755)
    discovery_setup = lock_setup.replace('/usr/local/bin/php-config', str(phpconfig)) + f'''
PHP_BINARIES={shlex.quote(str(phpbin))}
ensure_plugin() {{ return 0; }}
'''
    for action in ('do_check', 'do_lock', 'do_list'):
        seed_locks()
        before = lockfile.read_bytes(), ledger.read_bytes()
        run(discovery_setup + '\n' + action, work, 73)
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    phpconfig.write_text('#!/bin/bash\nprintf "' + str(work / 'absent-extensions') + '\\n"\n')
    for action in ('do_check', 'do_lock', 'do_list'):
        seed_locks()
        before = lockfile.read_bytes(), ledger.read_bytes()
        run(discovery_setup + '\nldd() { return 73; }; ' + action, work, 73)
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    phpconfig.unlink()
    run(discovery_setup + '\nphp_targets', work, 127)
    run(discovery_setup + '\nPHP_BINARIES=""; php_targets', work)
    for mock in (
            'sort() { return 73; }',
            'sed() { return 73; }',
            'base_repos() { return 73; }',
            'awk() { [[ "$*" = *\'foreign[$1]\'* ]] && return 73; command awk "$@"; }',
            'rpm() { return 73; }'):
        seed_locks()
        before = lockfile.read_bytes(), ledger.read_bytes()
        run(collector_setup + '\n' + mock + '; do_lock', work, 73)
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    run(lock_setup + '\nrpm() { echo "file $4 is not owned by any package"; return 1; }; rpm_file_record "%{SOURCERPM}" /custom/lib.so', work)
    run(lock_setup + '\nrpm() { return 1; }; rpm_file_record "%{SOURCERPM}" /custom/lib.so', work, 1)
    release_file = work / 'redhat-release'
    repodir = work / 'repos'
    repodir.mkdir()
    (repodir / 'vendor.repo').write_text('[baseos]\n')
    base_setup = function(lock_functions, 'base_repos').replace('/etc/redhat-release', str(release_file)).replace('/etc/yum.repos.d', str(repodir))
    run(lock_setup + '\n' + base_setup + '\nrpm() { return 73; }; base_repos', work, 73)
    result = run(lock_setup + '\n' + base_setup + '\nrpm() { echo AlmaLinux; }; base_repos', work)
    assert 'baseos\n' in result.stdout
    run(lock_setup + '\n' + base_setup + '\nrpm() { echo AlmaLinux; }; awk() { return 73; }; base_repos', work, 73)
    seed_locks()
    run(collector_setup + '''
rpm() { [[ "$1" = -qf ]] && { echo "file $4 is not owned by any package"; return 1; }; return 0; }
do_lock
''', work)
    assert lockfile.read_text() == foreign and ledger.read_text() == ''

    seed_locks()
    status_setup = collector_setup + '''
dnf() { [[ "$2" = list ]] && { echo 'libavif.x86_64 1.1-1 epel'; return 0; }; echo 'libavif.so.16()(64bit)'; }
rpm() { [[ "$2" = --provides ]] && { echo 'libavif.so.15()(64bit)'; return 0; }; echo 0.11-1; }
'''
    for mock in (
            'block_names() { return 73; }',
            'dnf() { return 73; }',
            "dnf() { [[ \"$2\" = list ]] && { echo 'libavif.x86_64 1.1-1 epel'; return 0; }; return 73; }",
            'rpm() { return 73; }',
            "rpm() { [[ \"$2\" = --provides ]] && { echo 'libavif.so.15()(64bit)'; return 0; }; return 73; }",
            'sonames() { return 73; }',
            'comm() { return 73; }'):
        before = lockfile.read_bytes(), ledger.read_bytes()
        result = run(status_setup + '\n' + mock + '; do_status', work, 73)
        assert 'SONAME CHANGE' not in result.stdout and 'no PHP library locks' not in result.stdout
        assert (lockfile.read_bytes(), ledger.read_bytes()) == before
    result = run(status_setup + '\ndo_status', work)
    assert 'SONAME CHANGE removes: libavif.so.15' in result.stdout
    run(status_setup + '\ndnf() { return 0; }; do_status', work)
    full_lock_script = (REPO / 'tools/php-libs-versionlock.sh').read_text()
    dispatch = full_lock_script[full_lock_script.index('if [[ "$1" != \'check\''):]
    run(lock_setup + '\nrpm() { return 73; }; set -- lock\n' + dispatch, work, 73)
    run(lock_setup + '\nrpm() { echo unknown; }; set -- lock\n' + dispatch, work, 1)
    result = run(lock_setup + '\nrpm() { echo 7; }; set -- lock\n' + dispatch, work)
    assert 'EL7 repos are frozen' in result.stdout
    print('PASS: actual discovery/filter/status failures preserve locks; genuine unowned libraries and empty queries remain valid')

    # These are actual source fragments, with earlier installer setup omitted.
    install = (REPO / 'inc/nginx_install.inc').read_text()
    call = re.search(r'^\s+patchnginx[^\n]*$', install, re.M).group()
    run('patchnginx() { return 72; };\n' + call + '\nexit 99', work, 72)
    for script in ('centmin.sh', 'centmin-cli.sh'):
        code = (REPO / script).read_text()
        fragment = re.search(r'\{\s*\nngxinstallmain\n\} 2>&1 \| tee[^\n]*\n[^\n]*PIPESTATUS[^\n]*', code).group()
        run(f'CENTMINLOGDIR={shlex.quote(str(work))}; DT=test; ngxinstallmain() {{ return 37; }};\n'
            + fragment + '\nexit 99', work, 1)
        outer_installs = re.findall(r'\} 2>&1 \| tee[^\n]*_install\.log"\nif \[\[.*?\nfi', code, re.S)
        assert len(outer_installs) == 2, (script, len(outer_installs))
        for outer in outer_installs:
            for producer, logging in ((0, 0), (37, 0), (0, 74), (37, 74)):
                run(f'''
CENTMINLOGDIR={shlex.quote(str(work))} SCRIPT_VERSION=test DT=test
funct_centmininstall() {{ return {producer}; }}
tee() {{ cat >/dev/null; return {logging}; }}
{{ funct_centmininstall || exit $?
''' + outer, work, 0 if producer == logging == 0 else 1)
        finish_call = re.search(r'^centminfinish[^\n]*$', code, re.M).group()
        run('centminfinish() { return 77; };\n' + finish_call + '\nexit 99', work, 77)
        wrappers = re.findall(r'\{[^{}]*?funct_(?:nginx|php)upgrade[^{}]*?\n\s*\} 2>&1 \| tee[^\n]*\n\s*(?:NGINX|PHP)_UPGRADE_STATUS[^\n]*\n\s*if \[\[.*?\n\s*fi', code, re.S)
        assert len(wrappers) == (2 if script == 'centmin.sh' else 5), (script, len(wrappers))
        for wrapper in wrappers:
            for producer, logging, expected in ((0, 0, 0), (37, 0, 37), (0, 74, 74), (37, 74, 37)):
                run(f'''
CENTMINLOGDIR={shlex.quote(str(work))} SCRIPT_VERSION=test DT=test
funct_nginxupgrade() {{ return {producer}; }}; funct_phpupgrade() {{ return {producer}; }}
tee() {{ cat >/dev/null; return {logging}; }}
''' + wrapper, work, expected)
    finish = function((REPO / 'inc/centminfinish.inc').read_text(), 'centminfinish')
    finish += '\n' + function((REPO / 'inc/php_upgrade.inc').read_text(), 'php_libs_lock_warn')
    result = run(finish + f'''
SCRIPT_DIR={shlex.quote(str(REPO))} CENTMINLOGDIR={shlex.quote(str(work))} DT=test
PHP_LIBS_VERSIONLOCK=y YUMLOG_FILE=/dev/null
ps() {{ :; }}; cmservice() {{ :; }}; cmchkconfig() {{ :; }}; installchecks() {{ :; }}; nvcheck() {{ :; }}
cecho() {{ echo "$1"; }}
bash() {{ return 77; }}
centminfinish
''', work, 0)
    # The install is complete; a failed library lock is reported, not fatal.
    assert 'install completed' in result.stdout and 'version locks were not updated' in result.stdout
    print('PASS: initial nginx install and both entry pipelines stop on failure')

    downloads = (REPO / 'inc/downloads.inc').read_text()
    zlib = function(downloads, 'nginxzlibtarball')
    all_downloads = function(downloads, 'alldownloads')
    other_downloaders = ['phpsrc_preflight', 'yuminstall', 'questions', 'ccachetarball',
                         'ccacheinstall', 'axelsetup', 'nginxtarball', 'freenginxtarball',
                         'csftarball', 'pythontarball', 'mailparsephpexttarball', 'phptarball',
                         'xcachetarball', 'apctarball', 'zopcachetarball', 'memcachetarball',
                         'imagickphpexttarball', 'redisphptarball', 'mongodbphptarball',
                         'swoolephptarball', 'gperftools', 'openssldownload', 'libressldownload',
                         'pcretarball', 'siegetarball', 'mysqltools', 'nsdtarball', 'mariadbrpms']
    download_setup = zlib + '\n' + all_downloads + '\n' + '\n'.join(
        name + '() { :; }' for name in other_downloaders) + f'''
DIR_TMP={shlex.quote(str(src))}
CM_INSTALLDIR={shlex.quote(str(work / 'missing-install-dir'))}
NGX_ZLIBLINKFILE=zlib.tar.gz NGX_ZLIBLINK=https://primary.invalid/zlib NGX_ZLIBLINKLOCAL=https://mirror.invalid/zlib
INITIALINSTALL=y PHP_VERSION=8.3.0 UNATTENDED=y
cecho() {{ :; }}; checklogdetails() {{ :; }}; cmm_php_fatal_pending() {{ return 1; }}
grep() {{ if [[ "$*" = '-c processor /proc/cpuinfo' ]]; then echo 2; else command grep "$@"; fi; }}
phptarball() {{ echo UNEXPECTED_CONTINUATION; }}
'''
    scenarios = [
        'wget() { echo DOWNLOAD; printf invalid > "$NGX_ZLIBLINKFILE"; }; tar() { echo UNEXPECTED_EXTRACTION; return 2; }',
        'wget() { echo UNEXPECTED_DOWNLOAD; return 99; }; tar() { [[ "$1" = -tzf ]] && return 0; return 84; }',
    ]
    for scenario in scenarios:
        for mode in ('n', 'y'):
            (src / 'zlib.tar.gz').write_text('cached archive')
            result = run(download_setup + '\nPARALLEL_MODE=' + mode + '\n' + scenario + '\nalldownloads', work, 1)
            assert 'UNEXPECTED_CONTINUATION' not in result.stdout and 'UNEXPECTED_DOWNLOAD' not in result.stdout
    (src / 'zlib.tar.gz').write_text('cached archive')
    run(download_setup + '\ntar() { return 0; }; wget() { return 99; }; nginxzlibtarball', work)
    run(download_setup + '\nDIR_TMP=/missing-centmin-test-directory; nginxzlibtarball', work, 1)
    print('PASS: zlib invalid mirrors and cached extraction failures cross serial/background dispatch; cache success and cd guard')

    fatal_helpers = ['ngxmoduletarball', 'openssldownload', 'libressldownload',
                     'openrestytarball', 'nginxpcretarball', 'nginxpcretwotarball',
                     'nginxwebdavtarball', 'nginxpgspeedtarball']
    fatal_functions = '\n'.join(function(downloads, name) for name in ['checklogdetails'] + fatal_helpers)
    check_modules = function((REPO / 'inc/nginx_upgrade.inc').read_text(), 'checknginxmodules')
    fatal_setup = fatal_functions + '\n' + check_modules + f'''
DIR_TMP={shlex.quote(str(src))}
CM_INSTALLDIR={shlex.quote(str(work / 'missing-install-dir'))}
CONFIGSCANBASE={shlex.quote(str(work / 'missing-config-dir'))}
NGX_FANCYINDEXLINKFILE=failure.tar.gz NGX_MEMCLINKFILE=failure.tar.gz
NGX_WEBDAVLINKFILE=failure.tar.gz NGX_PAGESPEEDGITLINKFILE=failure.tar.gz
NGINX_PAGESPEEDGITMASTER=y
OPENSSL_LINKFILE=failure.tar.gz LIBRESSL_LINKFILE=failure.tar.gz PCRELINKFILE=failure.tar.gz PCRETWOLINKFILE=failure.tar.gz
MASTER_OPENSSL_LINKFILE={shlex.quote(str(work / 'missing-master' / 'openssl.tar.gz'))}
MASTER_PCRETWOLINKFILE={shlex.quote(str(work / 'missing-master' / 'pcre2.tar.gz'))}
MASTER_PCRELINKFILE={shlex.quote(str(work / 'missing-master' / 'pcre.tar.gz'))}
NGX_LUANGINXLINKFILE=unused-lua.tar.gz
NGINX_OPENRESTY=y ORESTY_LUANGINX=n NGINX_PAGESPEED=n
CENTOSVER=7 CENTOS_SIX=6 OPENSSL_VERSION=3.5.0 NGINX_VERSION=1.31.6 LIBRESSL_SWITCH=y
cecho() {{ echo "$1"; }}; libresslgeolocation() {{ :; }}; axelsetup() {{ :; }}
curl() {{ echo 'HTTP/1.1 200'; }}; ping() {{ :; }}; dig() {{ :; }}
download_cmd() {{ wget "$@"; }}
grep() {{ if [[ "$*" = '-c processor /proc/cpuinfo' ]]; then echo 2; else command grep "$@"; fi; }}
'''
    for helper in fatal_helpers:
        for failure in ('download', 'extract'):
            for parallel in (False, True):
                archive = src / 'failure.tar.gz'
                archive.unlink(missing_ok=True)
                if failure == 'extract':
                    archive.write_text('cached archive')
                mocks = ('wget() { echo DOWNLOAD_FAILURE; return 73; }; '
                         'tar() { [[ "$1" = -tzf ]] && return 0; echo TAR_FAILURE; return 74; }')
                dispatch = (helper + ' & download_pid=$!; wait "$download_pid" || exit $?;'
                            if parallel else helper + ' || exit $?;')
                flags = '\nNGINX_PAGESPEED=y\n' if helper == 'nginxpgspeedtarball' else '\n'
                result = run(fatal_setup + flags + mocks + '\n' + dispatch + '\necho UNEXPECTED_CONTINUATION', src, 1)
                assert 'Aborting script...' in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
                assert ('DOWNLOAD_FAILURE' if failure == 'download' else 'TAR_FAILURE') in result.stdout, (helper, failure, parallel, result.stdout, result.stderr)
    for failure in ('download', 'extract'):
        for parallel in ('n', 'y'):
            archive = src / 'failure.tar.gz'
            archive.unlink(missing_ok=True)
            if failure == 'extract':
                archive.write_text('cached archive')
            result = run(fatal_setup + f'''
PARALLEL_MODE={parallel} NGINX_OPENRESTY=n
libressldownload() {{ :; }}; nginxzlibtarball() {{ :; }}
wget() {{ echo DOWNLOAD_FAILURE; return 73; }}
tar() {{ [[ "$1" = -tzf ]] && return 0; echo TAR_FAILURE; return 74; }}
checknginxmodules || exit $?
echo UNEXPECTED_CONTINUATION
''', work, 1)
            assert 'Aborting script...' in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
    psol_scripts = src / 'pagespeed/scripts'
    psol_scripts.mkdir(parents=True)
    formatter = psol_scripts / 'format_binary_url.sh'
    formatter.write_text('#!/bin/sh\nprintf "https://test.invalid/psol.tar.gz\\n"\n')
    formatter.chmod(0o755)
    for version in ('1.12.34', '1.13.35'):
        for failure in ('download', 'extract'):
            for parallel in (False, True):
                (src / 'failure.tar.gz').write_text('cached archive')
                dispatch = ('nginxpgspeedtarball & child=$!; wait "$child" || exit $?;'
                            if parallel else 'nginxpgspeedtarball || exit $?;')
                result = run(fatal_setup + f'''
NGINX_PAGESPEED=y NGXPGSPEED_VER={version} NGX_PAGESPEEDLINKFILE=failure.tar.gz
FAILURE={failure}
wget() {{ [[ "$FAILURE" = download ]] && {{ echo DOWNLOAD_FAILURE; return 73; }}; return 0; }}
tar() {{ [[ "$1" = -tzf ]] && {{ echo pagespeed/; return 0; }}; [[ "$1" = xzf && "$FAILURE" = extract ]] && {{ echo TAR_FAILURE; return 74; }}; return 0; }}
''' + dispatch + '\necho UNEXPECTED_CONTINUATION', src, 1)
                assert 'Aborting script...' in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
    print('PASS: actual checklogdetails and selected download/extraction failures return nonzero through serial/background checks')

    initial_functions = '\n'.join(function(downloads, name) for name in ('nginxtarball', 'freenginxtarball'))
    initial_setup = download_setup + '\n' + fatal_setup + '\n' + initial_functions + '''
NGINX_INSTALL=y FREENGINX_INSTALL=y
NGX_LINKFILE=initial.tar.gz FREENGX_LINKFILE=initial.tar.gz
NGX_FANCYINDEXLINKFILE=missing-fancy.tar.gz NGX_MEMCLINKFILE=missing-memc.tar.gz
LIBRESSL_SWITCH=n
wget() { echo DOWNLOAD_FAILURE; return 73; }
'''
    archive = src / 'initial.tar.gz'

    def initial_archive():
        with tarfile.open(archive, 'w:gz') as cached:
            entry = tarfile.TarInfo('initial-source/')
            entry.type = tarfile.DIRTYPE
            entry.mode = 0o755
            cached.addfile(entry)

    for leaf in ('nginxtarball', 'freenginxtarball'):
        sibling = 'freenginxtarball' if leaf == 'nginxtarball' else 'nginxtarball'
        for parallel in ('n', 'y'):
            for failure in ('download', 'extract', 'modules', 'openresty'):
                archive.unlink(missing_ok=True)
                if failure != 'download':
                    initial_archive()
                mocks = f'\nPARALLEL_MODE={parallel}; {sibling}() {{ :; }}\n'
                if failure == 'extract':
                    mocks += 'tar() { [[ "$1" = xzf ]] && return 74; command tar "$@"; };\n'
                if failure == 'openresty':
                    mocks += 'ngxmoduletarball() { :; }; NGINX_OPENRESTY=y;\n'
                else:
                    mocks += 'openrestytarball() { :; };\n'
                result = run(initial_setup + mocks + '\nalldownloads', src, 1)
                assert 'Downloads complete.' not in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
            initial_archive()
            run(initial_setup + '\nPARALLEL_MODE=' + parallel
                + '; ngxmoduletarball() { :; }; openrestytarball() { :; }; wget() { return 99; }; ' + leaf, src)
    for leaf in ('openssldownload', 'libressldownload'):
        sibling = 'libressldownload' if leaf == 'openssldownload' else 'openssldownload'
        for parallel in ('n', 'y'):
            (src / 'failure.tar.gz').unlink(missing_ok=True)
            result = run(initial_setup + f'''
PARALLEL_MODE={parallel}
nginxtarball() {{ :; }}; freenginxtarball() {{ :; }}; {sibling}() {{ :; }}
phptarball() {{ :; }}
alldownloads
''', src, 1)
            assert 'DOWNLOAD_FAILURE' in result.stdout and 'Downloads complete.' not in result.stdout
    print('PASS: actual initial nginx/FreeNginx/module/OpenResty/OpenSSL/LibreSSL failures cross serial/background dispatch; valid caches succeed')

    site = (REPO / 'tools/sitestatus.sh').read_text()
    site = site[site.index('checkstatus() {'):site.index('case "$1" in')].replace('/usr/local/nginx/conf', str(work))
    maintenance = work / 'sitestatus.conf'
    for action, value in (('son', 0), ('soff', 1)):
        maintenance.write_text(f'default {value};\n')
        result = run(site + '\nCHECK=n; sed() { return 73; }; service() { echo UNEXPECTED_RELOAD; }; ' + action, work, 73)
        assert 'UNEXPECTED_RELOAD' not in result.stdout and maintenance.read_text() == f'default {value};\n'
    print('PASS: actual maintenance edit failures stop before reload')

    pcre = (REPO / 'inc/pcre.inc').read_text()
    wrapper_setup = fatal_setup + '\n' + '\n'.join(
        function(pcre, name) for name in ('pcredir_check', 'pcre_two_dir_check')) + f'''
CENTMINLOGDIR={shlex.quote(str(work))} DT=test
'''
    for wrapper in ('pcredir_check', 'pcre_two_dir_check'):
        for branch in ('missing-directory', 'cached-directory'):
            extracted = src / 'pcre-fixture'
            shutil.rmtree(extracted, ignore_errors=True)
            if branch == 'cached-directory':
                extracted.mkdir()
            for failure in ('download', 'extract'):
                archive = src / 'failure.tar.gz'
                archive.unlink(missing_ok=True)
                if branch == 'cached-directory' or failure == 'extract':
                    archive.write_text('cached archive')
                result = run(wrapper_setup + f'''
FAILURE={failure}
wget() {{ printf archive > "$DIR_TMP/failure.tar.gz"; [[ "$FAILURE" = download ]] && {{ echo DOWNLOAD_FAILURE; return 73; }}; return 0; }}
tar() {{ [[ "$1" = -tzf ]] && {{ echo pcre-fixture/; return {1 if branch == 'cached-directory' else 0}; }}; echo TAR_FAILURE; return 74; }}
{wrapper} || exit $?
echo UNEXPECTED_CONTINUATION
''', src, 1)
                assert 'Aborting script...' in result.stdout and 'UNEXPECTED_CONTINUATION' not in result.stdout
            archive.write_text('cached archive')
            run(wrapper_setup + '\ntar() { [[ "$1" = -tzf ]] && { echo pcre-fixture/; return 1; }; return 0; }; wget() { return 0; }; '
                'tee() { cat >/dev/null; return 74; }; ' + wrapper, src, 1)
        with tarfile.open(archive, 'w:gz') as cached:
            entry = tarfile.TarInfo('pcre-fixture/')
            entry.type = tarfile.DIRTYPE
            entry.mode = 0o755
            cached.addfile(entry)
        before = archive.read_bytes()
        run(wrapper_setup + '\nnginxpcretarball() { return 99; }; nginxpcretwotarball() { return 99; }; ' + wrapper, src)
        assert archive.read_bytes() == before
    print('PASS: actual PCRE1/PCRE2 wrappers retain downloader and tee failure across all four branches')

    compile_helpers = ('source_pcre_two_install', 'nginx_pcreinstall', 'source_pcreinstall')
    compile_setup = '\n'.join(function(pcre, name) for name in compile_helpers) + f'''
DIR_TMP={shlex.quote(str(src))} CENTMINLOGDIR={shlex.quote(str(work))} DT=test
DETECT_NGXVER=1031006 NGINX_PCRE_TWO=y NGINX_PCRE=y PCRE_SOURCEINSTALL=y
NGINX_PCRETWOVER=10.45 PCRE_VERSION=8.45 DROPBOX_SEND=n
export PCRE_TRACE={shlex.quote(str(work / 'pcre-trace'))} CONFIGURE_STATUS=0
cecho() {{ :; }}
find() {{ :; }}
make() {{
  printf '%s\\n' "${{1:-make}}" >> "$PCRE_TRACE"
  [[ "$FAILURE" = make && $# -eq 0 ]] && return 74
  [[ "$FAILURE" = install && "$1" = install ]] && return 75
  return 0
}}
'''
    for tree in ('pcre2-10.45', 'pcre-8.45'):
        directory = src / tree
        directory.mkdir()
        configure = directory / 'configure'
        configure.write_text('#!/bin/sh\nprintf "configure\\n" >> "$PCRE_TRACE"\nexit "$CONFIGURE_STATUS"\n')
        configure.chmod(0o755)
    for helper in compile_helpers:
        for failure in ('configure', 'make', 'install', 'tee'):
            trace = work / 'pcre-trace'
            trace.write_text('')
            mocks = ('\ntee() { cat >/dev/null; return 76; };' if failure == 'tee' else '')
            status = 73 if failure == 'configure' else 0
            run(compile_setup + f'\nFAILURE={failure}; export CONFIGURE_STATUS={status};' + mocks + '\n'
                + helper + ' || exit $?; echo UNEXPECTED_CONTINUATION', src, 1)
            phases = trace.read_text().splitlines()
            if failure == 'configure':
                assert 'make' not in phases and 'install' not in phases
            elif failure == 'make':
                assert 'install' not in phases
    print('PASS: actual PCRE compile helpers stop on configure/make/install/tee failure')

# A failed download that aborts must not report success: a bare `exit` after
# checklogdetails returned cecho's status 0, so menu 5 looked successful.
for name in ('inc/downloads.inc', 'inc/zendopcache_reinstall.inc'):
    source = (REPO / name).read_text()
    assert not re.search(r'^\s*exit #\$ERROR', source, re.M), name
assert not re.search(r'^\s*exit\s*$', (REPO / 'inc/php_upgrade.inc').read_text(), re.M)
print('PASS: download and PHP upgrade aborts exit non-zero')

# Addons that menu 5 runs as separate scripts honour the deferred php-fpm
# restart instead of restarting php-fpm onto a half-updated PHP build.
for name in ('addons/php72-mcrypt.sh', 'addons/php73-mcrypt.sh', 'addons/php74-mcrypt.sh', 'addons/php80-mcrypt.sh', 'addons/ioncube.sh'):
    lines = [line for line in (REPO / name).read_text().splitlines() if 'restart php-fpm' in line and not line.lstrip().startswith('#')]
    assert lines and all('CMM_PHPFPM_RESTART_DEFERRED' in line for line in lines), (name, lines)
assert 'export CMM_PHPFPM_RESTART_DEFERRED=y' in (REPO / 'inc/php_upgrade.inc').read_text()
print('PASS: menu 5 addons defer their php-fpm restart')
