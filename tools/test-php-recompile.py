#!/usr/bin/env python3
"""Isolated checks of the actual PHP build bodies. No services or packages touched."""
import os
from pathlib import Path
import subprocess
import shutil
import time
import tempfile
from unittest.mock import patch

REPO = Path(__file__).resolve().parents[1]
UPGRADE = (REPO / 'inc/php_upgrade.inc').read_text()
CONFIGURE = (REPO / 'inc/php_configure.inc').read_text()
BINARY_STRIP = 'strip_php_binary() {' + CONFIGURE.split('strip_php_binary() {', 1)[1].split('\n}', 1)[0] + '\n}\n'
STRIP = CONFIGURE.split('strip_php_extensions() {', 1)[1].split('\n}', 1)[0]
INITIAL = CONFIGURE.split('  # only initial install needs', 1)[1].split('  if [ -f config.log ]; then', 1)[0]
INITIAL = INITIAL[INITIAL.index('  if [[ "$FPM_PHPFPM_RPM"'):]

MOCKS = r'''
event() { printf '%s\n' "$*" >> "$EVENTS"; }
cecho() { :; }; figlet() { :; }; axelsetup() { :; }; funct_mktempfile() { :; }
gethtpasswdsh() { :; }; phpng_download() { :; }; cmm_php_fatal_pending() { return 1; }
cmm_php_fatal() { event source-failure; return 1; }
cmm_archive_valid() { event validate; [[ "$FAILPHASE" != source ]]; }
cmm_php_mirror_list() { echo https://mock.invalid; }; fake_download() { return 7; }
phpgeolocation() { PHPEXTSION=gz; PHPTAR_FLAGS=xzf; }
tar() { event extract; [[ "$FAILPHASE" != extract ]]; }
autodetectextensions() { event detect; }; zopcacheupgrade() { event opcache; }
funct_centos6check() { :; }; rpm() { echo php-cli; }; yum() { event yum; }
git() { return 1; }; wget() { return 1; }; curl() { return 1; }
rm() { :; }; sleep() { :; }; find() { event find; [[ "$FAILPHASE" != strip ]] || return 7; }
pgrep() { [[ "$RUNNING" = y ]]; }; id() { echo 0; }
lscpu() { :; }; free() { :; }; df() { :; }; tail() { :; }; journalctl() { :; }
sar_call() { :; }; php_patches() { event patches; [[ "$FAILPHASE" != patches ]] || return 7; }; check_devtoolset_php() { :; }
enable_devtoolset() { :; }; max_spawn_rate_check() { :; }
phpsededit() { event ini-tune; }; phptuning() { event fpm-tune; }
zendopcacheextfix() { :; }; phpiadmin() { :; }; strip_php_extensions() { event strip; [[ "$FAILPHASE" != strip ]] || return 7; }
fileinfo_standalone() { event fileinfo; [[ "$FAILPHASE" != fileinfo ]] || return 7; }; phptimezonedb_install() { event timezonedb; [[ "$FAILPHASE" != timezonedb ]] || return 7; }; run_after_php_upgrade() { :; }
funct_phpconfigure() { event configure; [[ "$PWD" = "$DIR_TMP/php-8.3.35/fpm-build" && "$FAILPHASE" != configure ]] || return 7; }
cmservice() { event restart; [[ "$FAILPHASE" != restart ]] || return 7; }
funct_igbinaryinstall() { event igbinary; [[ "$FAILPHASE" != igbinary ]] || return 7; }
strip_php_binary() { strip -s "$1"; }
strip() { event "binary-strip:$*"; [[ "$FAILPHASE" != binary-strip && "$FAILPHASE" != stage-strip ]] || return 7; }
php() { [[ "$FAILPHASE" != ig-load ]] || return 7; [[ "$*" != '--ri igbinary' ]] || echo 'igbinary version => 3.2.16'; }
php-config() { "$FAKE_PHP_CONFIG" "$@"; }
grep() { if [[ "$*" = *'/proc/cpuinfo'* ]]; then echo 2; else command grep "$@"; fi; }
cp() { event copy; [[ "$FAILPHASE" != backup ]] || return 7; command cp "$@"; }
make() {
  case "$*" in
    clean|distclean) return 0;;
    *INSTALL_ROOT*) event stage; [[ "$FAILPHASE" != stage ]] || return 7;;
    install) event install; [[ "$FAILPHASE" != install ]] || return 7;;
    *) event make; [[ "$FAILPHASE" != make ]] || return 7;;
  esac
}
'''

def run(phase='', body=None, setup='', running='n'):
    with tempfile.TemporaryDirectory(prefix='cmm-php-test-') as tmp:
        root = Path(tmp)
        def write(name, text, executable=False):
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
            if executable:
                path.chmod(0o700)
            return path
        events = write('events', '')
        ext = root / 'extensions'
        ext.mkdir()
        config = root / 'ini'
        config.mkdir()
        source = root / 'src/php-8.3.35'
        (source / 'fpm-build').mkdir(parents=True)
        write('src/php-8.3.35/main/php_version.h', '#define PHP_VERSION_ID 80335\n')
        write('src/php-8.3.35/php.ini-production', 'new php.ini\n')
        write('usr/local/lib/php.ini', 'old php.ini\n')
        write('usr/local/bin/php', '#!/bin/bash\nexit 0\n', True)
        write('root/.bashrc', 'fpm-errlog fpm-phperrlog fpm-slowlog\n')
        php_config = write('usr/local/bin/php-config', "#!/bin/bash\nextension_dir='" + str(ext) + "'\ncase \"$1\" in --extension-dir) echo \"$extension_dir\";; --version) echo 8.3.35;; esac\n", True)
        write('usr/local/sbin/php-fpm', '#!/bin/bash\necho configtest >> "$EVENTS"\n[[ "$FAILPHASE" != configtest ]]\n', True)
        write('usr/bin/sitestatus', '#!/bin/bash\necho maintenance-$1 >> "$EVENTS"\n[[ "$FAILPHASE" != maintenance-$1 ]] || exit 7\n', True)
        write('repo/tools/php-libs-versionlock.sh', '#!/bin/bash\necho libs-$1 >> "$EVENTS"\n[[ "$FAILPHASE" != libs-$1 ]] || exit 7\n[[ "$FAILPHASE" != libs-refresh || "$1" != lock || $(grep -c "^libs-lock$" "$EVENTS") -lt 2 ]] || exit 7\n', True)
        write('src/php-8.3.35/buildconf', '#!/bin/bash\nexit 0\n', True)
        body = UPGRADE + MOCKS + '\nfunct_phpupgrade 8.3.35\n' if body is None else UPGRADE + MOCKS + body
        for old in ('/usr/local', '/usr/bin/sitestatus', '/root/.bashrc'):
            body = body.replace(old, str(root) + old)
        prelude = f'''
DIR_TMP='{root}/src'; SCRIPT_DIR='{root}/repo'; CONFIGSCANDIR='{config}'
CENTMINLOGDIR='{root}'; FAKE_PHP_CONFIG='{php_config}'; DT=test
DOWNLOADAPP_PHP=fake_download; PHPNG_YES=n; PHP_PGO=n; PHP_PGO_ALWAYS=n; STRIPPHP=n
PHPMAKETEST=n; UALL=n; AUTODETECPHP_OVERRIDE=n; PHP_MCRYPTPECL=n; PHPIONCUBE=n
PHP_SECURED=n; PHP_LDMOLD=n; PHPREDIS=n; PHPMSSQL=n; PHPMONGODB=n
PHPIMAGICK_ALWAYS=n; PHPGEOIP_ALWAYS=n; PHPZOPFLI_ALWAYS=n; YUMDNFBIN=yum
CENTOS_SIX=; CENTOS_SEVEN=; CENTOS_EIGHT=; CENTOS_NINE=; CENTOS_TEN=
PHP_UPDATEMAINTENANCE=y; PHP_LIBS_VERSIONLOCK=y; FPM_PHPFPM_INSTALLDIR_ENABLE=y
FPM_PHPFPM_INSTALLDIR='{root}/stage'; INITIALINSTALL=n; IGBINARY_INSTALL=y
'''
        env = {key: os.environ[key] for key in ('PATH', 'TMPDIR') if key in os.environ}
        env.update(LC_ALL='C', EVENTS=str(events), FAILPHASE=phase, RUNNING=running)
        # Inject setup after mocks, before the tested call.
        marker = '\nfunct_phpupgrade 8.3.35\n'
        if marker in body:
            body = body.replace(marker, '\n' + setup + marker)
        else:
            body = body.replace(MOCKS, MOCKS + '\n' + setup + '\n')
        script = write('test-script.sh', prelude + body)
        result = subprocess.run(['bash', str(script)], stdin=subprocess.DEVNULL, cwd=root, env=env, capture_output=True, text=True, timeout=10)
        return result, events.read_text().splitlines(), (root / 'usr/local/lib/php.ini').read_text()

# Post-build lock failures warn: the new PHP is already installed and serving.
for phase in ('libs-lock', 'libs-refresh'):
    result, events, ini = run(phase, setup='cecho() { echo "$1"; }; touch "$CONFIGSCANDIR/igbinary.ini" "$(php-config --extension-dir)/igbinary.so"')
    assert result.returncode == 0, (phase, events, result.stdout, result.stderr)
    assert 'version locks were not updated' in result.stdout + result.stderr, (phase, result.stdout)
    assert ini == 'new php.ini\n' and 'restart' in events and 'maintenance-on' in events, (phase, events)
    if phase == 'libs-refresh':
        assert events.index('maintenance-on') < len(events) - 1 - events[::-1].index('libs-lock'), events
    print('PASS:', phase, 'failure warns and keeps the installed PHP online')

for phase in ('source', 'extract', 'patches', 'libs-update', 'configure', 'make', 'stage', 'install', 'backup', 'configtest', 'restart', 'strip', 'maintenance-off', 'igbinary'):
    result, events, ini = run(phase, setup='touch "$CONFIGSCANDIR/igbinary.ini"' if phase == 'igbinary' else '')
    assert result.returncode != 0, (phase, events, result.stdout, result.stderr)
    if phase in ('configure', 'make', 'stage', 'install', 'backup', 'restart', 'strip'):
        assert result.returncode == 7, (phase, result.returncode, events)
    if phase in ('source', 'extract'):
        assert not set(events) & {'detect', 'opcache', 'yum', 'configure', 'copy', 'maintenance-off'}
    if phase in ('source', 'extract', 'patches', 'libs-update', 'configure', 'make', 'stage'):
        assert 'install' not in events and ini == 'old php.ini\n'
        assert 'maintenance-off' not in events, (phase, events)
    if phase == 'maintenance-off':
        assert 'install' not in events
    if phase == 'install':
        assert 'copy' not in events and 'restart' not in events and ini == 'old php.ini\n'
    if phase == 'configtest':
        assert 'restart' not in events
    assert 'maintenance-on' not in events, (phase, events)
    print('PASS:', phase, 'failure stops later changes')
result, events, ini = run(setup='touch "$CONFIGSCANDIR/igbinary.ini" "$(php-config --extension-dir)/igbinary.so"')
assert result.returncode == 0, (events, result.stdout, result.stderr)
assert events.index('extract') < events.index('detect') < events.index('libs-update') < events.index('configure') < events.index('make') < events.index('stage') < events.index('maintenance-off') < events.index('install') < events.index('libs-lock') < events.index('copy') < events.index('configtest') < events.index('restart') < events.index('maintenance-on')
assert ini == 'new php.ini\n' and 'igbinary' not in events and events.count('maintenance-off') == 1
print('PASS: source/build/install/config/restart ordering and healthy igbinary preservation')

# Run the actual initial-install body with failed ordinary make and staging.
for phase in ('make', 'stage'):
    result, events, _ = run(phase, '\ninitial() {\n' + INITIAL + '\n}\nINITIALINSTALL=y; initial\n')
    assert result.returncode != 0 and 'install' not in events, (phase, events, result.stderr)
print('PASS: initial make and staging failures')

# Old extension directories may be unavailable or unchanged when OPcache is disabled.
for old in ('', 'same', 'missing-new'):
    result, events, _ = run(body='\nautodetectinstallextensions\n', setup='PHPEXTDIRDOLD="' + ('$(php-config --extension-dir)' if old else '') + '"; ' + ('cat() { :; }; ' if old == 'missing-new' else '') + 'touch "$CONFIGSCANDIR/test.ini"; sed() { event sed-edit; return 7; }')
    assert result.returncode == 0 and 'sed-edit' not in events, (old, events, result.stderr)
print('PASS: unknown/unchanged extension directory skips INI rewrite')

# Extensions are stripped through private copies, so a running PHP-FPM keeps
# its loaded mappings and is never restarted or config-tested here.
for phase in ('', 'strip', 'binary-strip'):
    result, events, _ = run(phase, '\nstrip_php_extensions() {' + STRIP + '\n}\nstrip_php_extensions\n',
                            setup='find() { event find; [[ "$FAILPHASE" != strip ]] || return 7; command find "$@"; }; touch "$(php-config --extension-dir)/a.so" "$(php-config --extension-dir)/b.so"', running='y')
    assert 'restart' not in events and 'configtest' not in events, (phase, events)
    if phase == '':
        assert result.returncode == 0 and sum(e.startswith('binary-strip:-s ') for e in events) == 2, (events, result.stderr)
    else:
        assert result.returncode == 7 and 'extension stripping failed' in result.stderr, (phase, result.returncode, result.stderr)
print('PASS: extension stripping replaces files without restarting PHP-FPM and preserves failure status')

retry = UPGRADE.split('    PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)\n    echo\n    # validate', 1)[1]
retry = '    # validate' + retry.split('    if [[ ("$CENTOS_NINE"', 1)[0]
with tempfile.TemporaryDirectory(prefix='cmm-php-retry-') as tmp:
    script = Path(tmp) / 'retry.sh'
    script.write_text('phpver=invalid; read() { phpver=8.3.35; };\n' + retry + '\n[[ "$PHPMUVER" = 8.3 ]]')
    result = subprocess.run(['bash', str(script)], stdin=subprocess.DEVNULL, env=dict(LC_ALL='C', **{key: os.environ[key] for key in ('PATH', 'TMPDIR') if key in os.environ}), capture_output=True, text=True, timeout=10)
assert result.returncode == 0, result.stderr
print('PASS: retried target recalculates PHPMUVER')

# Hugepage calculations must probe memory themselves after php.ini tuning moves.
huge = (REPO / 'inc/zendopcache_tweaks.inc').read_text()
huge = huge.replace('/usr/bin/numactl', '$FAKE_PHP_CONFIG')
for original, name in (('/proc/meminfo', 'meminfo'), ('/etc/security/limits.conf', 'limits.conf'), ('/etc/sysctl.conf', 'sysctl.conf'), ('/sys/kernel/mm/transparent_hugepage/enabled', 'enabled'), ('/usr/bin/redis-cli', 'absent-redis-cli')):
    huge = huge.replace(original, '"${CONFIGSCANDIR}/' + name + '"')
huge_call = next(line for line in CONFIGURE.splitlines() if line.strip().startswith('opcachehugepages'))
for release in (9, 10):
    for mode, value, expected in (('available', '4194304', 4194304), ('available', '', None), ('available', '0', None), ('available', 'bad', None), ('numa', '2048', 2097452), ('numa', '', None), ('numa', '0', None), ('numa', 'bad', None)):
        for previous in (('unset FREEMEM', 'FREEMEM=1/0') if expected is not None else ('FREEMEM=123456',)):
            setup = f'''
CENTOS_NINE={'9' if release == 9 else ''}; CENTOS_TEN={'10' if release == 10 else ''}
CHECK_LXD=y; PHP_HUGEPAGES=y; OPCACHEHUGEPAGES_OPT=; {previous}
printf 'MemAvailable: %s kB\\nBuffers: 100 kB\\nCached: 200 kB\\n' '{value if mode == 'available' else '4194304'}' > "$CONFIGSCANDIR/meminfo"
printf '[never]\\n' > "$CONFIGSCANDIR/enabled"
touch "$CONFIGSCANDIR/limits.conf" "$CONFIGSCANDIR/sysctl.conf" "$CONFIGSCANDIR/zendopcache.ini"
numactl() {{ printf 'available: {2 if mode == 'numa' else 1} nodes\\nnode 0 free: {value if mode == 'numa' else '2048'} MB\\n'; }}
sysctl() {{ event hugepage-policy; return 7; }}
'''
            body = huge + '\nmemory_check() {\n' + huge_call + '\n}\nmemory_check || exit $?\n[[ -z "$OPCACHEHUGEPAGES_OPT" && ! -s "$CONFIGSCANDIR/sysctl.conf" ]] || exit 8\nevent "memory:$FREEMEM"\n'
            result, events, _ = run(body=body, setup=setup)
            assert 'hugepage-policy' not in events, (release, mode, value, events)
            if expected is None:
                assert result.returncode == 1 and 'invalid' in result.stderr, (release, mode, value, result.returncode, result.stderr)
            else:
                assert result.returncode == 0 and f'memory:{expected}' in events, (release, mode, value, events, result.stderr)
print('PASS: EL9/EL10 memory probes ignore stale values, validate available/NUMA memory and retain hugepage policy')

with patch.dict(os.environ, BASH_ENV='/nonexistent/inherited-startup.sh', CMM_TEST_INHERITED='yes'):
    result, _, _ = run(body='\n[[ -z ${BASH_ENV+x} && -z ${CMM_TEST_INHERITED+x} ]]\n')
    assert result.returncode == 0, result.stderr
print('PASS: isolated subprocess environment drops BASH_ENV and unrelated inherited variables')

strip_body = '\nstrip_php_extensions() {' + STRIP + '\n}\nstrip_php_extensions\n'
for mode in ('missing', 'empty', 'relative', 'root', 'file', 'probe'):
    setup = {
        'missing': 'command rmdir "$(php-config --extension-dir)"',
        'empty': 'php-config() { echo; }',
        'relative': 'php-config() { echo extensions; }',
        'root': 'php-config() { echo /; }',
        'file': 'command rmdir "$(php-config --extension-dir)"; touch "$(php-config --extension-dir)"',
        'probe': 'php-config() { return 7; }',
    }[mode]
    result, events, _ = run(body=strip_body, setup=setup, running='y')
    assert (result.returncode == 0) == (mode == 'missing'), (mode, events, result.stderr)
    assert 'find' not in events and 'restart' not in events, (mode, events)
print('PASS: fresh PHP 8.5 missing extension directory skips stripping; invalid paths and probes fail')

for initial in (False, True):
    body = '\ninitial() {\n' + INITIAL + '\n}\ninitial\n' if initial else None
    for phase in ('stage-strip', 'binary-strip', 'binary-backup'):
        setup = 'STRIPPHP=y; PHP_LIBS_VERSIONLOCK=n; ' + ('INITIALINSTALL=y; ' if initial else '')
        if phase != 'stage-strip':
            setup += 'FPM_PHPFPM_INSTALLDIR_ENABLE=n; '
        if phase == 'binary-backup':
            setup += 'cp() { event copy; [[ "$*" != *-b4strip* ]] || return 7; command cp "$@"; }; '
        result, events, _ = run(phase, body, setup)
        assert result.returncode == 7 and 'maintenance-on' not in events and 'restart' not in events, (initial, phase, events, result.stderr)
        if phase == 'stage-strip':
            assert 'install' not in events and 'maintenance-off' not in events
    # CGI can intentionally be absent; required CLI and FPM binaries are still stripped.
    result, events, _ = run(body=body, setup='STRIPPHP=y; ' + ('INITIALINSTALL=y; ' if initial else ''))
    assert result.returncode == 0 and not any('php-cgi' in event for event in events if event.startswith('binary-strip:')), (initial, events, result.stderr)
print('PASS: initial/upgrade staged and live strip/backup errors stop promotion; optional absent CGI succeeds')

PATCHES = (REPO / 'inc/php_patch.inc').read_text()
for phase in ('patch-copy', 'patch-convert', 'patch-apply', 'patch-tee', 'applied', 'success'):
    setup = '''
CUR_DIR="$DIR_TMP/patchrepo"; PHP_PATCH=y; PHPMUVER=5.4; PHPVER_ID=50445
mkdir -p "$CUR_DIR/patches/php"; echo fixture > "$CUR_DIR/patches/php/php54-81719.patch"
cd "$DIR_TMP"
cp() { event patch-copy; [[ "$FAILPHASE" != patch-copy ]] || return 7; command cp "$@"; }
dos2unix() { event patch-convert; [[ "$FAILPHASE" != patch-convert ]] || return 7; }
patch() { event patch-apply; [[ "$FAILPHASE" != patch-apply ]] || return 7; }
rm() { command rm "$@"; }
tee() { [[ "$FAILPHASE" != patch-tee ]] || { command cat >/dev/null; return 7; }; command tee "$@"; }
[[ "$FAILPHASE" != applied ]] || cp "$CUR_DIR/patches/php/php54-81719.patch" php54-81719.patch
'''
    body = PATCHES + '\nphp_patches; status=$?; [[ $status -eq 0 || ! -f php54-81719.patch || "$FAILPHASE" = patch-tee ]] || exit 9; exit "$status"\n'
    result, events, _ = run(phase, body, setup)
    assert result.returncode == (7 if phase.startswith('patch-') else 0), (phase, events, result.stderr)
    if phase == 'applied':
        assert 'patch-apply' not in events
print('PASS: actual PHP patch copy/conversion/application/tee failures propagate and failed markers clear')

# Exercise an existing restoration branch without running a real patch or GNU grep.
setup = '''
CUR_DIR="$DIR_TMP/patchrepo"; PHP_PATCH=y; PHPMUVER=7.0; PHPVER_ID=70033
mkdir -p "$CUR_DIR/patches/php"; echo fixture > "$CUR_DIR/patches/php/php7033-78862.patch"
cd "$DIR_TMP"; patch() { event patch-apply; return 7; }; dos2unix() { :; }
grep() { [[ "$1" != -oP ]] || return 0; command grep "$@"; }; rm() { command rm "$@"; }
'''
result, events, _ = run(body=PATCHES + '\nphp_patches; status=$?; [[ ! -f php7033-78862.patch ]] || exit 9; exit "$status"\n', setup=setup)
assert result.returncode != 0 and 'patch-apply' in events, (events, result.stderr)
print('PASS: actual PHP patch restoration path returns failure and clears its marker')

IGBINARY = (REPO / 'inc/igbinary.inc').read_text()
ig_setup = r'''
PHP_INSTALL=y; IGBINARY_INSTALL=y; IGBINARYGIT=n; IGBINARY_VERSION=3.2.16; INITIALINSTALL=y
mkdir -p "$DIR_TMP/igbinary-3.2.16"
printf '#!/bin/bash\necho ig-configure >> "$EVENTS"\n[[ "$FAILPHASE" != ig-configure ]] || exit 7\n' > "$DIR_TMP/igbinary-3.2.16/configure"
chmod +x "$DIR_TMP/igbinary-3.2.16/configure"
printf '#!/bin/bash\necho phpize >> "$EVENTS"\n[[ "$FAILPHASE" != phpize ]] || exit 7\n' > "${FAKE_PHP_CONFIG%php-config}phpize"
chmod +x "${FAKE_PHP_CONFIG%php-config}phpize"
echo module > "$(php-config --extension-dir)/igbinary.so"
[[ "$FAILPHASE" = ig-fetch ]] || echo archive > "$DIR_TMP/igbinary-3.2.16.tgz"
download_cmd() { event ig-fetch; return 7; }; autoconf() { :; }
[[ "$FAILPHASE" != ig-module ]] || command rm "$(php-config --extension-dir)/igbinary.so"
[[ "$FAILPHASE" != ig-ini ]] || { command rmdir "$CONFIGSCANDIR"; }
'''
for phase in ('ig-fetch', 'extract', 'phpize', 'ig-configure', 'make', 'install', 'ig-module', 'ig-ini', 'ig-load', 'configtest', 'restart', 'success'):
    result, events, _ = run(phase, IGBINARY + '\nfunct_igbinaryinstall\n', ig_setup)
    assert (result.returncode == 0) == (phase == 'success'), (phase, events, result.stdout, result.stderr)
    if phase not in ('restart', 'success'):
        assert 'restart' not in events, (phase, events)
    if phase in ('ig-fetch', 'extract', 'phpize', 'ig-configure', 'make'):
        assert 'install' not in events
print('PASS: actual igbinary fetch/extract/phpize/configure/build/install/module/INI/configtest/reload failures propagate')

for phase in ('ig-clone', 'ig-version', 'ig-sed', 'success'):
    setup = ig_setup + r'''
IGBINARYGIT=y; PHPMUVER=8.3; PHPVER_ID=80335
sed() { event ig-sed; [[ "$FAILPHASE" != ig-sed ]] || return 7; }
[[ "$FAILPHASE" != ig-version ]] || php-config() { return 7; }
git() { event ig-clone; [[ "$FAILPHASE" != ig-clone ]] || return 7; mkdir -p igbinary-php; command cp "$DIR_TMP/igbinary-3.2.16/configure" igbinary-php/configure; }
'''
    result, events, _ = run(phase, IGBINARY + '\nfunct_igbinaryinstall\n', setup)
    assert (result.returncode == 0) == (phase == 'success'), (phase, events, result.stderr)
    if phase != 'success':
        assert 'restart' not in events and (phase == 'ig-sed' or 'install' not in events)
setup = ig_setup + r'''
IGBINARYGIT=y; PHPMUVER=5.6; PHPVER_ID=50640
php-config() { [[ "$1" != --version ]] || { echo 5.6.40; return; }; "$FAKE_PHP_CONFIG" "$@"; }
wget() { event legacy-fetch; return 7; }
'''
result, events, _ = run(body=IGBINARY + '\nfunct_igbinaryinstall\n', setup=setup)
assert result.returncode == 7 and 'legacy-fetch' in events and 'install' not in events and 'restart' not in events, (events, result.stderr)
print('PASS: actual modern igbinary clone/version failure and legacy archive fetch failure stop enabling/reloading')

for include in ('inc/memcached_install.inc', 'inc/redis.inc'):
    source = (REPO / include).read_text()
    for mode in ('missing', 'version', 'healthy'):
        setup = 'IGBINARY_VERSION=3.2.16; '
        if mode != 'missing':
            setup += 'touch "$CONFIGSCANDIR/igbinary.ini" "$(php-config --extension-dir)/igbinary.so"; '
        if mode == 'version':
            setup += 'php() { echo "igbinary version => 1.2.3"; }; '
        result, events, _ = run('igbinary', source + '\ncheckigbinary\n', setup)
        assert result.returncode == (0 if mode == 'healthy' else 7), (include, mode, events, result.stderr)
        assert ('igbinary' in events) == (mode != 'healthy')
print('PASS: both sibling igbinary checks propagate missing/version repair failures and preserve healthy modules')

REDIS = (REPO / 'inc/redis.inc').read_text()
result, events, _ = run('igbinary', REDIS + '\nredisinstall\n', 'PHP_INSTALL=y; INITIALINSTALL=n')
assert result.returncode == 7 and 'igbinary' in events and 'install' not in events and 'restart' not in events, (events, result.stderr)
result, events, _ = run('igbinary', REDIS + '\nfunct_phpupgrade 8.3.35\n', 'PHP_INSTALL=y; PHPREDIS=y; INITIALINSTALL=n')
assert result.returncode == 7 and 'igbinary' in events and 'maintenance-on' not in events and 'restart' not in events, (events, result.stderr)
print('PASS: redis install and actual full PHP upgrade propagate nested igbinary failure')

for include, function in (('inc/apcinstall.inc', 'funct_apcsourceinstall_disabled'), ('inc/apcreinstall.inc', 'funct_apcreinstall_disabled')):
    setup = r'''
PHP_INSTALL=y; INITIALINSTALL=y; ENABLE_MENU=y; APCCACHE_VERSION=3.1.13; APCINSTALL=y
mkdir -p "$DIR_TMP/APC-3.1.13"; printf '#!/bin/bash\nexit 0\n' > "$DIR_TMP/APC-3.1.13/configure"; chmod +x "$DIR_TMP/APC-3.1.13/configure"
printf '#!/bin/bash\nexit 0\n' > "${FAKE_PHP_CONFIG%php-config}phpize"; chmod +x "${FAKE_PHP_CONFIG%php-config}phpize"
php-config() { echo 5.4.45; }; chown() { :; }; od() { echo test; }; cp() { :; }
read() { case "${@: -1}" in apckeyd) apckeyd=y;; apcver) apcver=3.1.13;; resetapcini) resetapcini=n;; esac; }
'''
    result, events, _ = run('igbinary', (REPO / include).read_text() + '\n' + function + '\n', setup)
    assert result.returncode == 7 and 'igbinary' in events, (include, events, result.stderr)
# The live APC entry points remain intentional deprecation no-ops.
for include, function in (('inc/apcinstall.inc', 'funct_apcsourceinstall'), ('inc/apcreinstall.inc', 'funct_apcreinstall')):
    result, events, _ = run('igbinary', (REPO / include).read_text() + '\n' + function + '\n')
    assert result.returncode == 0 and 'igbinary' not in events, (include, events)
print('PASS: historical APC installer bodies propagate failures; active deprecated APC remains unchanged')

for setup in (ig_setup + '\nIGBINARY_INSTALL=n', ig_setup + '\nPHP_INSTALL=n'):
    result, events, _ = run(body=IGBINARY + '\nfunct_igbinaryinstall\n', setup=setup)
    assert result.returncode == 0 and not set(events) & {'install', 'make', 'ig-fetch'}, (events, result.stderr)
    if 'PHP_INSTALL=n' in setup:
        assert 'restart' not in events and 'configtest' not in events
print('PASS: disabled igbinary skips building and PHP_INSTALL=n skips its FPM branch')

for initial in (False, True):
    body = '\ninitial() {\n' + INITIAL + '\n}\ninitial\n' if initial else None
    for phase in ('stage-clean', 'stage-dir', 'stage-empty', 'stage-relative', 'stage-root'):
        setup = 'PHP_LIBS_VERSIONLOCK=n; ' + ('INITIALINSTALL=y; ' if initial else '') + r'''
rm() { event stage-clean; [[ "$FAILPHASE" != stage-clean || "${@: -1}" != "$FPM_PHPFPM_INSTALLDIR" ]] || return 13; }
mkdir() { [[ "$FAILPHASE" != stage-dir || "${@: -1}" != "$FPM_PHPFPM_INSTALLDIR" ]] || return 14; command mkdir "$@"; }
case "$FAILPHASE" in stage-empty) FPM_PHPFPM_INSTALLDIR='';; stage-relative) FPM_PHPFPM_INSTALLDIR=relative;; stage-root) FPM_PHPFPM_INSTALLDIR=/;; esac
'''
        result, events, _ = run(phase, body, setup)
        assert result.returncode == {'stage-clean': 13, 'stage-dir': 14}.get(phase, 1), (initial, phase, events, result.stderr)
        assert not set(events) & {'stage', 'install', 'restart', 'maintenance-off', 'maintenance-on'}, (initial, phase, events)
print('PASS: initial/upgrade invalid staging paths and cleanup/directory failures stop before either install')

for entry in ('centmin.sh', 'centmin-cli.sh'):
    source = (REPO / entry).read_text()
    for phase, needle in (('patches', 'php_patches || exit $?'), ('igbinary', 'funct_igbinaryinstall || exit $?'), ('configtest', '/usr/local/sbin/php-fpm -t || exit $?'), ('service-start', 'service php-fpm start || exit $?')):
        line = next(line for line in source.splitlines() if line.strip() == needle)
        result, events, _ = run(phase, '\n' + line + '\nevent sentinel\n', 'service() { event service-start; return 7; }')
        assert result.returncode != 0 and 'sentinel' not in events, (entry, phase, events, result.stderr)
    fpm_gate = source[source.index('    /usr/local/sbin/php-fpm -t || exit $?'):].split('    fileinfo_standalone', 1)[0]
    php_gate = next(line for line in source.splitlines() if line.strip() == 'if [[ "$PHP_INSTALL" = [yY] ]]; then')
    assert source.index(php_gate) < source.index(fpm_gate) < source.index('\nxcacheinstall_ask')
    result, events, _ = run('configtest', '\n' + php_gate + '\n' + fpm_gate + '\nfi\nevent sentinel\n', 'PHP_INSTALL=n; service() { event service-start; return 7; }')
    assert result.returncode == 0 and events == ['sentinel'], (entry, events, result.stderr)
print('PASS: both actual entry-script guards stop before later work and PHP_INSTALL=n skips initial FPM')

optional_setup = r'''
PHPFINFO_STANDALONE=y; PHPTIMEZONEDB=y; PHPTIMEZONEDB_VER=2025.1
mkdir -p "$DIR_TMP/php-8.3.35/ext/fileinfo" "$DIR_TMP/timezonedb-2025.1"
for directory in "$DIR_TMP/php-8.3.35/ext/fileinfo" "$DIR_TMP/timezonedb-2025.1"; do
    printf '#!/bin/bash\necho ext-configure >> "$EVENTS"\n[[ "$FAILPHASE" != ext-configure ]] || exit 7\n' > "$directory/configure"
    chmod +x "$directory/configure"
done
printf '#!/bin/bash\necho phpize >> "$EVENTS"\n[[ "$FAILPHASE" != phpize ]] || exit 7\n' > "${FAKE_PHP_CONFIG%php-config}phpize"
chmod +x "${FAKE_PHP_CONFIG%php-config}phpize"
phpize() { event phpize; [[ "$FAILPHASE" != phpize ]] || return 7; }
echo module > "$(php-config --extension-dir)/fileinfo.so"
echo module > "$(php-config --extension-dir)/timezonedb.so"
[[ "$FAILPHASE" = ext-fetch ]] || echo archive > "$DIR_TMP/timezonedb-2025.1.tgz"
wget() { event ext-fetch; return 7; };
php() { event module-load; [[ "$FAILPHASE" != ext-load ]] || return 7; }
case "$FAILPHASE" in
 ext-cd) command rm -rf "$DIR_TMP/php-8.3.35/ext/fileinfo" "$DIR_TMP/timezonedb-2025.1";;
 ext-probe) php-config() { [[ "$1" != --extension-dir ]] || return 7; "$FAKE_PHP_CONFIG" "$@"; };;
 ext-version) php-config() { return 7; };;
 ext-module) command rm "$(php-config --extension-dir)/fileinfo.so" "$(php-config --extension-dir)/timezonedb.so";;
 ext-ini) command rmdir "$CONFIGSCANDIR";;
 ext-disabled) PHPFINFO_STANDALONE=n; PHPTIMEZONEDB=n;;
esac
'''
for include, function, phases in (
    ('inc/fileinfo.inc', 'fileinfo_standalone', ('ext-version', 'ext-cd', 'phpize', 'ext-configure', 'make', 'install', 'ext-probe', 'ext-module', 'ext-ini', 'ext-load', 'configtest', 'restart', 'ext-disabled', 'success')),
    ('inc/timezonedb.inc', 'phptimezonedb_install', ('ext-fetch', 'extract', 'ext-cd', 'phpize', 'ext-configure', 'make', 'install', 'ext-probe', 'ext-module', 'ext-ini', 'ext-load', 'ext-disabled', 'success')),
):
    for phase in phases:
        body = (REPO / include).read_text() + '\n' + function + '; status=$?\n'
        # A failed pre-enable phase must not create either module INI file.
        if phase in ('ext-version', 'ext-fetch', 'extract', 'ext-cd', 'phpize', 'ext-configure', 'make', 'install', 'ext-probe', 'ext-module', 'ext-disabled'):
            body += '[[ ! -f "$CONFIGSCANDIR/fileinfo.ini" && ! -f "$CONFIGSCANDIR/timezonedb.ini" ]] || exit 9\n'
        body += 'exit "$status"\n'
        result, events, _ = run(phase, body, optional_setup)
        assert (result.returncode == 0) == (phase in ('ext-disabled', 'success')), (include, phase, events, result.stderr)
        if phase not in ('restart', 'success'):
            assert 'restart' not in events, (include, phase, events)
        if phase in ('ext-version', 'ext-fetch', 'extract', 'ext-cd', 'phpize', 'ext-configure', 'make', 'ext-disabled'):
            assert 'install' not in events, (include, phase, events)
print('PASS: actual fileinfo/timezonedb selected-phase failures stop enablement, module/config/service errors propagate, disabled options skip')

for phase in ('fileinfo', 'timezonedb'):
    result, events, _ = run(phase)
    assert result.returncode == 7 and 'restart' not in events and 'maintenance-on' not in events, (phase, events, result.stderr)
for entry in ('centmin.sh', 'centmin-cli.sh'):
    lines = (REPO / entry).read_text().splitlines()
    for phase, needle in (('fileinfo', 'fileinfo_standalone || exit $?'), ('timezonedb', 'phptimezonedb_install || exit $?')):
        line = next(line for line in lines if line.strip() == needle)
        result, events, _ = run(phase, '\n' + line + '\nevent sentinel\n')
        assert result.returncode == 7 and 'sentinel' not in events, (entry, phase, events)
print('PASS: full PHP upgrade and both initial entry callers propagate fileinfo/timezonedb failures')

TIMEZONEDB = (REPO / 'inc/timezonedb.inc').read_text()
for phase in ('configtest', 'restart', 'success'):
    result, events, _ = run(phase, TIMEZONEDB + '\nphptimezonedb_install\n', optional_setup + '\nINITIALINSTALL=y; PHP_INSTALL=y')
    assert (result.returncode == 0) == (phase == 'success'), (phase, events, result.stderr)
    if phase == 'configtest':
        assert 'restart' not in events
result, events, _ = run(body=TIMEZONEDB + '\nphptimezonedb_install\n', setup=optional_setup + '\nINITIALINSTALL=y; PHP_INSTALL=n')
assert result.returncode == 0 and 'restart' not in events and 'configtest' not in events, (events, result.stderr)
print('PASS: initial timezonedb configuration/reload failures propagate and PHP_INSTALL=n skips FPM actions')

FILEINFO = (REPO / 'inc/fileinfo.inc').read_text()
result, events, _ = run(body=FILEINFO + '\nfileinfo_standalone || exit $?\ngrep -qx ";extension=fileinfo.so" "$CONFIGSCANDIR/fileinfo.ini"\n', setup=optional_setup + '\nphp() { event module-load; echo "Module fileinfo is already loaded"; }')
assert result.returncode == 0 and events.count('restart') == 2, (events, result.stderr)
print('PASS: fileinfo already-loaded fallback still disables duplicate INI and restarts successfully')

result, events, _ = run(body=FILEINFO + '\nfileinfo_standalone\n', setup=optional_setup + '\nfpmrestart() { event unexpected-shortcut; return 127; }; INITIALINSTALL=y; PHP_INSTALL=y')
assert result.returncode == 0 and 'restart' in events and 'unexpected-shortcut' not in events, (events, result.stderr)
print('PASS: fresh initial fileinfo uses existing cmservice before the fpmrestart shortcut is installed')

# Linux limits each argv string to 128 KiB; script files also keep prompts off source stdin.
result, events, _ = run(body='\n#' + 'x' * (128 * 1024) + '\nif read -r unexpected; then exit 9; fi\nevent large-script\n')
assert result.returncode == 0 and events == ['large-script'], (events, result.stderr)
print('PASS: scripts beyond Linux argv limits run from files with EOF stdin')

# The running PHP-FPM keeps its loaded libraries during the library update and
# build, so opt-in maintenance starts just before make install.
for mode, setup in (('enabled', ''), ('disabled', 'PHP_LIBS_VERSIONLOCK=n'), ('missing', 'command rm "$SCRIPT_DIR/tools/php-libs-versionlock.sh"')):
    result, events, _ = run(setup=setup)
    assert result.returncode == 0 and events.count('maintenance-off') == 1 and events.count('maintenance-on') == 1, (mode, events, result.stderr)
    assert events.index('stage') < events.index('maintenance-off') < events.index('install') < events.index('maintenance-on'), (mode, events)
    if mode == 'enabled':
        assert events.index('libs-update') < events.index('configure'), (mode, events)
    else:
        assert 'libs-update' not in events, (mode, events)
report = 'cecho() { echo "$1"; }'
# Failures before make install never enter maintenance and say PHP was not changed.
for phase in ('libs-update', 'configure', 'make', 'stage'):
    result, events, _ = run(phase, setup=report)
    assert result.returncode != 0 and 'maintenance-off' not in events and 'install' not in events, (phase, events, result.stderr)
    assert 'stage: prepare' in result.stdout and 'PHP files were not changed' in result.stdout, (phase, result.stdout)
    # A (possibly partial) library update is checked against the PHP binaries on disk.
    assert 'libs-check' in events, (phase, events)
# Later failures leave maintenance only while PHP-FPM is still running.
for phase in ('maintenance-off', 'install', 'configtest', 'restart'):
    for running in ('n', 'y'):
        result, events, _ = run(phase, setup=report, running=running)
        assert result.returncode != 0, (phase, running, events)
        assert ('maintenance-on' in events) == (running == 'y'), (phase, running, events)
        assert ('Maintenance mode is still ON' in result.stdout) == (running == 'n'), (phase, running, result.stdout)
        if phase != 'maintenance-off':
            assert 'stage: install' in result.stdout and 'avoid restarting php-fpm' in result.stdout, (phase, result.stdout)
# A helper that aborts with exit still gets the report and maintenance restore.
result, events, _ = run(body='\nphptimezonedb_install() { event timezonedb; exit 9; }\nfunct_phpupgrade 8.3.35\n', setup=report, running='y')
assert result.returncode == 9 and 'maintenance-on' in events and 'failed (status 9)' in result.stdout, (events, result.stdout)
result, events, _ = run(setup='PHP_UPDATEMAINTENANCE=n')
assert result.returncode == 0 and 'libs-update' in events and 'maintenance-off' not in events and 'maintenance-on' not in events, (events, result.stderr)
print('PASS: maintenance starts at make install, failures report the stage and leave maintenance only while PHP-FPM runs')

# Run the actual publishing helper against regular files; failed operations only touch its temporary copy.
copy_tool = shutil.which('gcp') if os.uname().sysname == 'Darwin' else shutil.which('cp')
readlink_tool = shutil.which('greadlink') if os.uname().sysname == 'Darwin' else shutil.which('readlink')
assert copy_tool and readlink_tool, 'GNU cp/readlink required for atomic-strip helper checks'
for phase in ('binary-copy', 'binary-temp-strip', 'binary-move', 'success', 'symlink'):
    setup = f'''
printf 'original\\n' > "$CONFIGSCANDIR/binary"; chmod 751 "$CONFIGSCANDIR/binary"
cp() {{ event binary-copy; [[ "$FAILPHASE" != binary-copy ]] || {{ printf 'partial\\n' > "${{@: -1}}"; return 7; }}; '{copy_tool}' "$@"; }}
readlink() {{ '{readlink_tool}' "$@"; }}
strip() {{ event binary-temp-strip; printf 'stripped\\n' >> "${{@: -1}}"; [[ "$FAILPHASE" != binary-temp-strip ]] || return 7; }}
mv() {{ event binary-move; [[ "$FAILPHASE" != binary-move ]] || return 7; command mv "$@"; }}
rm() {{ command rm "$@"; }}
[[ "$FAILPHASE" != symlink ]] || ln -s binary "$CONFIGSCANDIR/binary-link"
'''
    body = BINARY_STRIP + r'''
[[ "$FAILPHASE" != symlink ]] && binary="$CONFIGSCANDIR/binary" || binary="$CONFIGSCANDIR/binary-link"
strip_php_binary "$binary"; status=$?
[[ "$FAILPHASE" != symlink || -L "$CONFIGSCANDIR/binary-link" ]] || exit 8
if [[ "$status" -eq 0 ]]; then grep -qx stripped "$CONFIGSCANDIR/binary" || exit 9
else [[ "$(cat "$CONFIGSCANDIR/binary")" = original ]] || exit 9; fi
[[ "$(find "$CONFIGSCANDIR" -name 'binary.strip.*' -print)" = '' ]] || exit 9
python3 -c 'import os, sys; assert os.stat(sys.argv[1]).st_mode & 0o7777 == 0o751' "$CONFIGSCANDIR/binary" || exit 9
exit "$status"
'''
    # Check cleanup with a real find; the main harness mocks find for extension strip failures.
    result, events, _ = run(phase, body, setup + '\nfind() { command find "$@"; }')
    assert result.returncode == (0 if phase in ('success', 'symlink') else 7), (phase, events, result.stderr)
print('PASS: atomic core strip copy/strip/move failures preserve original content and clean temporary files; symlink survives')

# On Linux, exercise GNU strip with a real executable that stays mapped in a running process.
if os.uname().sysname == 'Linux':
    with tempfile.TemporaryDirectory(prefix='cmm-running-elf-') as tmp:
        root = Path(tmp)
        binary = root / 'busy-binary'
        c_source = root / 'busy.c'
        c_source.write_text('#include <unistd.h>\nint main(void) { for (;;) pause(); }\n')
        env = dict(LC_ALL='C', **{key: os.environ[key] for key in ('PATH', 'TMPDIR') if key in os.environ})
        built = subprocess.run(['cc', '-g', '-o', str(binary), str(c_source)], env=env, stdin=subprocess.DEVNULL, capture_output=True, text=True, timeout=10)
        assert built.returncode == 0, built.stderr
        binary.chmod(0o751)
        script = root / 'strip-binary.sh'
        script.write_text(BINARY_STRIP + '\nstrip_php_binary "$1"\n')
        for target in (binary, root / 'binary-link'):
            if target != binary:
                target.symlink_to(binary.name)
            old_stat = binary.stat()
            process = subprocess.Popen([str(target)], stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            try:
                time.sleep(0.05)
                assert process.poll() is None
                result = subprocess.run(['bash', str(script), str(target)], env=env, stdin=subprocess.DEVNULL, capture_output=True, text=True, timeout=10)
                assert result.returncode == 0 and process.poll() is None, (result.returncode, result.stderr)
                new_stat = binary.stat()
                assert old_stat.st_ino != new_stat.st_ino and (new_stat.st_mode & 0o7777) == 0o751
                assert os.stat(f'/proc/{process.pid}/exe').st_ino == old_stat.st_ino
                assert new_stat.st_uid == old_stat.st_uid and new_stat.st_gid == old_stat.st_gid
                assert not list(root.glob('busy-binary.strip.*'))
                if target != binary:
                    assert target.is_symlink()
            finally:
                process.terminate()
                process.wait(timeout=5)
    print('PASS: actual GNU strip atomically replaces running ELF and symlink target while retaining process, mode and owner')

# Configure must not restart the running (old) PHP-FPM: a library update may
# already have removed a SONAME it needs, and the post-build restart applies
# systemd drop-in changes anyway.
DEBUG_STEPS = CONFIGURE[CONFIGURE.index('enable_php_debug_steps() {'):CONFIGURE.index('\n}\n', CONFIGURE.index('enable_php_debug_steps() {')) + 3]
with tempfile.TemporaryDirectory(prefix='cmm-php-debugsteps-') as tmp:
    # Every system path is redirected into the temporary tree.
    steps = DEBUG_STEPS.replace('/etc/', tmp + '/etc/').replace('/proc/', tmp + '/proc/')
    for directory in ('etc/systemd/system/php-fpm.service.d', 'etc/sysctl.d', 'etc/centminmod', 'proc/sys/kernel', 'proc/sys/fs'):
        (Path(tmp) / directory).mkdir(parents=True, exist_ok=True)
    (Path(tmp) / 'proc/sys/kernel/core_pattern').write_text('core\n')
    (Path(tmp) / 'proc/sys/fs/suid_dumpable').write_text('0\n')
    for mode, dropin in (('n', False), ('n', True), ('y', False)):
        dropin_file = Path(tmp) / 'etc/systemd/system/php-fpm.service.d/10-coredump.conf'
        if dropin:
            dropin_file.write_text('[Service]\n')
        trace = Path(tmp) / 'trace'
        trace.write_text('')
        script = Path(tmp) / 'debug.sh'
        script.write_text(f'''
systemctl() {{ echo "systemctl $*" >> "{trace}"; }}
sysctl() {{ :; }}
gdb() {{ :; }}
HOME={tmp}; PHPDEBUGMODE={mode}; YUMDNFBIN=true
''' + steps + '\nenable_php_debug_steps\n')
        result = subprocess.run(['bash', str(script)], stdin=subprocess.DEVNULL, capture_output=True, text=True, timeout=10, env=dict(LC_ALL='C', PATH=os.environ['PATH']))
        calls = trace.read_text()
        assert 'restart php-fpm' not in calls, (mode, dropin, calls, result.stderr)
        assert ('daemon-reload' in calls) == (mode == 'y' or dropin), (mode, dropin, calls)
print('PASS: PHP debug-mode setup never restarts the running PHP-FPM during configure')
