#!/usr/bin/env python3
"""Isolated addon GCC15 selector checks; no packages, root access or services."""
from pathlib import Path
import subprocess
import shutil
import tempfile

ROOT = Path(__file__).resolve().parents[1]
EL10 = 'CENTOS_TEN' in (ROOT / 'addons/ffmpeg.sh').read_text()


def run(script, cwd):
    return subprocess.run(['bash', '-c', script], cwd=cwd, text=True,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE)


with tempfile.TemporaryDirectory(prefix='addon-gcc15-') as work:
    temp = Path(work)
    (temp / 'addons').mkdir()
    (temp / 'inc').mkdir()
    toolset = temp / 'toolset'
    helper = (ROOT / 'inc/gcc.inc').read_text().replace('/opt/rh/gcc-toolset-15', str(toolset))
    helper = helper.replace('/usr/lib/gcc-toolset/15-env.source', str(temp / 'missing-env'))
    (temp / 'inc/gcc.inc').write_text(helper)
    setup = f'''
export PATH=/usr/bin:/bin
export LD_LIBRARY_PATH=/existing/lib
GCC_FIFTTEEN=y DEVTOOLSETFIFTTEEN=y
OPT_LEVEL=-O3 MARCH_TARGET=x86-64 INITIALINSTALL=n
install_fixture() {{
  mkdir -p '{toolset}/root/usr/bin'
  for compiler in gcc g++ ld; do
    printf '#!/bin/sh\\necho 15.2.1\\n' > '{toolset}/root/usr/bin/'"$compiler"
    chmod +x '{toolset}/root/usr/bin/'"$compiler"
  done
  printf 'export PATH="{toolset}/root/usr/bin:$PATH"\\nexport LD_LIBRARY_PATH="{toolset}/root/usr/lib64:$LD_LIBRARY_PATH"\\n' > '{toolset}/enable'
}}
yum() {{ echo PACKAGE_INSTALL; [[ "$FAIL_INSTALL" != y ]] && install_fixture; }}
uname() {{ echo x86_64; }}
'''
    checks = 0
    # The direct-compiler override must not leak into the calling addon.
    for initial in ('unset', 'y', 'n'):
        for failed in (False, True):
            if toolset.exists():
                shutil.rmtree(toolset)
            prefix = setup + ('unset INITIALINSTALL\n' if initial == 'unset' else f'INITIALINSTALL={initial}\n')
            result = run(prefix + f'source "{temp}/inc/gcc.inc"\n' +
                         ('FAIL_INSTALL=y\n' if failed else '') +
                         'INITIALINSTALL=y enable_gcc_toolset15; status=$?\n' +
                         'printf "STATE:%s|%s|%s\\n" "${INITIALINSTALL-unset}" "$status" "$CC"', temp)
            state = result.stdout.split('STATE:')[1].strip().split('|')
            assert state[0] == initial, state
            assert state[1] == ('1' if failed else '0'), state
            if not failed:
                assert state[2] == f'{toolset}/root/usr/bin/gcc', state
            checks += 1
    names = ['redis-server-install.sh', 'ffmpeg.sh']
    if (ROOT / 'addons/php-libheif.sh').exists():
        names.append('php-libheif.sh')
    for name in names:
        text = (ROOT / 'addons' / name).read_text()
        subprocess.run(['bash', '-n', str(ROOT / 'addons' / name)], check=True)
        if name == 'redis-server-install.sh':
            body = text[text.index('redisinstall_source() {'):]
            body = body[:body.index('  cd "$SRCDIR"')] + '\n}\nredisinstall_source || exit 1\n'
            assert 'enable_gcc_toolset15' not in text[:text.index('redisinstall_source() {')]
            assert 'redisinstall_source || exit 1' in text
            assert 'make${MAKETHREADS} CC="${CC:-gcc}"' in text
        else:
            start = text.index('# Preserve explicit Clang selections.')
            end = text.index('do_continue() {', start) if name == 'ffmpeg.sh' else text.index('# Install required build tools', start)
            body = text[start:end]
            assert "GCC_FIFTTEEN='n'" in text
            assert '/opt/gcc-custom/gcc15' not in text
            if name == 'php-libheif.sh':
                gate_start = text.index('if [[ "$CUSTOM_LIBHEIF_INSTALL" != [yY]')
                assert gate_start < start
                disabled = temp / 'addons/disabled-heif.sh'
                disabled.write_text(text[gate_start:start] + body + '\necho UNEXPECTED_BUILD\n')
                result = run(setup + f'CUSTOM_LIBHEIF_INSTALL=n\nsource "{disabled}"', temp)
                assert result.returncode == 0 and 'experimental script not enabled' in result.stdout
                assert 'UNEXPECTED_BUILD' not in result.stdout and 'PACKAGE_INSTALL' not in result.stdout
                checks += 1
        body += '\nprintf "RESULT:%s|%s|%s|%s|%s|%s|%s\\n" "$CC" "$CXX" "$CFLAGS" "$CXXFLAGS" "$PATH" "$LD_LIBRARY_PATH" "$INITIALINSTALL"\n'
        selector = temp / 'addons' / name
        selector.write_text(body)
        for osver in (7, 8, 9, 10):
            for mode in ('absent', 'present', 'failure', 'optout', 'clang', 'options'):
                if toolset.exists():
                    shutil.rmtree(toolset)
                prefix = setup + f'\nEL_VERID={osver}\n'
                if mode == 'present':
                    prefix += 'install_fixture\n'
                if mode == 'failure':
                    prefix += 'FAIL_INSTALL=y\n'
                if mode == 'optout':
                    prefix += 'GCC_FIFTTEEN=n DEVTOOLSETFIFTTEEN=n\n'
                if mode == 'options':
                    prefix += 'HOIST=y FLTO=y GOLDLINKER=y DWARF=y\n'
                if mode == 'clang':
                    prefix += 'CC=clang CXX=clang++\n'
                result = run(prefix + f'source "{selector}"', temp)
                active = osver in ((8, 9, 10) if EL10 else (8, 9)) and mode not in ('optout', 'clang')
                label = (name, osver, mode, result.stdout, result.stderr)
                if active and mode == 'failure':
                    assert result.returncode != 0 and 'RESULT:' not in result.stdout, label
                else:
                    assert result.returncode == 0, label
                    fields = result.stdout.split('RESULT:')[1].strip().split('|')
                    if active:
                        assert fields[0] == f'{toolset}/root/usr/bin/gcc', label
                        assert fields[1] == f'{toolset}/root/usr/bin/g++', label
                        assert '-std=gnu17' in fields[2] and '-std=gnu17' not in fields[3], label
                        assert fields[4].startswith(str(toolset)), label
                        assert fields[5].endswith(':/existing/lib'), label
                        assert fields[6] == 'n', label
                        if name == 'redis-server-install.sh' and mode == 'options':
                            for flag in ('-fcode-hoisting', '-flto', '-ffat-lto-objects', '-fuse-ld=gold', '-gsplit-dwarf'):
                                assert flag in fields[2] and flag in fields[3], label
                    else:
                        assert 'PACKAGE_INSTALL' not in result.stdout, label
                        if mode == 'clang':
                            assert fields[:2] == ['clang', 'clang++'], label
                if mode == 'present':
                    assert 'PACKAGE_INSTALL' not in result.stdout, label
                checks += 1
        if name == 'ffmpeg.sh':
            lines = [line.strip() for line in text.splitlines() if './configure' in line and '--enable-libx264' in line]
            assert len(lines) == 4, 'Both install/update and nasm paths must be checked'
            (temp / 'configure').write_text('#!/bin/bash\nprintf "%s\\n" "$@"\nprintf "LIBPATH:%s\\n" "$LD_LIBRARY_PATH"\n')
            (temp / 'configure').chmod(0o755)
            for line in lines:
                result = run(setup + f'install_fixture\nEL_VERID=9\nsource "{selector}"\nOPT={temp}\nMOLD_OPT=" -fuse-ld=mold"\n' + line, temp)
                assert result.returncode == 0, result.stderr
                assert f'--cc={toolset}/root/usr/bin/gcc' in result.stdout
                assert f'--ld={toolset}/root/usr/bin/gcc' in result.stdout
                assert '-std=gnu17' in result.stdout and '-fuse-ld=mold' in result.stdout
                assert f'LIBPATH:{temp}/ffmpeg/lib:{toolset}/root/usr/lib64:/existing/lib' in result.stdout
                checks += 1
            cmake_lines = [line for line in text.splitlines() if not line.lstrip().startswith('#') and '../../source' in line]
            assert len(cmake_lines) == 2
            assert all('-DCMAKE_C_COMPILER="${CC:-gcc}" -DCMAKE_CXX_COMPILER="${CXX:-g++}"' in line for line in cmake_lines)
    print(f'PASS: {checks} isolated addon checks; EL10 enabled={EL10}')
