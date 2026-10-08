#!/usr/bin/env python3
"""Run the actual upgrade body with isolated paths and mocked build/service commands."""
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile
from unittest.mock import patch

root = Path(__file__).resolve().parents[1]
bash = os.environ.get("BASH_BIN", shutil.which("bash"))
upgrade = (root / "inc/nginx_upgrade.inc").read_text()
backup = (root / "inc/nginx_backup.inc").read_text()
mocks = r'''
for fn in cecho funct_mktempfile perl_ipc_cmd_install gethtpasswdsh pagespeeduptasks install_gperftools installopenssl pcre_two_dir_check pcredir_check luajitinstall funct_nginxmodules check_requestscheme sar_call systemctl run_after_nginx_upgrade nginx_deprecate_httptwo_param clear_ps geoinccheck geoipphp mimefix pcre_check_nginx detect_tlsonethree gcc; do eval "$fn() { :; }"; done
checkmap() { echo map >> "$work/trace"; }
checkgeoip() { :; }
nginxbackup() { echo backup >> "$work/trace"; [[ "$mode" != backupfail ]]; }
checknginxmodules() { [[ "$mode" != modulesfail ]]; }
nginx_dependency_compiler() { [[ "$mode" != compilerfail ]]; }
luajitinstall() { [[ "$mode" != luajitfail ]] || return 23; }
nginxzlib_install() { [[ "$mode" != zlibfail ]]; }
nginx_maintenance_on() { echo maintenance_on >> "$work/trace"; [[ "$mode" != maintenanceonfail ]]; }
nginx_maintenance_off() { echo maintenance_off >> "$work/trace"; [[ "$mode" != maintenanceofffail ]]; }
funct_nginxconfigure() { [[ "$mode" != configurefail ]]; }
patchnginx() { [[ "$mode" != patchfail ]]; }
nginx() { echo 'nginx version: nginx/1.31.6'; }
bc() { command cat >/dev/null; echo 0; }
tar() {
  echo "tar $*" >> "$work/trace"
  if [[ "$mode" = *downloadfail ]] || [[ "$mode" = invalidcache && ! -f "$work/downloaded" ]]; then return 2; fi
  [[ "$mode" != extractfail || "$1" != xvfz ]]
}
download_cmd() { echo "download $*" >> "$work/trace"; if [[ "$mode" = invalidcache ]]; then touch "$work/downloaded"; else return 8; fi; }
wget() { echo "wget $*" >> "$work/trace"; return 8; }
make() {
  echo "make $*" >> "$work/trace"
  [[ "$mode" != stagefail || "$*" != *DESTDIR=* ]] || return 7
  [[ "$mode" != makefail || "$1" = clean || "$1" = install ]] || return 1
  [[ "$mode" != installfail || "$1" != install ]]
}
service() { echo "service $*" >> "$work/trace"; [[ "$mode" != restartfail || "$2" != restart ]]; }
sleep() { SECONDS=$((SECONDS + 31)); }
ps() {
  local title=nginx generation=""
  if [[ "$mode" = angie* ]]; then title=angie generation=' #3'; fi
  if [[ "$1" = -p ]]; then echo "$title: master process v1.11.4$generation [nginx]"; return; fi
  if [[ ! -f "$work/switched" ]]; then echo "21 $title: worker process$generation"; return; fi
  if [[ "$mode" = newstopping || "$mode" = angiestopping ]]; then echo "42 $title: worker process is shutting down$generation";
  elif [[ "$mode" != noworkers ]]; then echo "42 $title: worker process$generation"; fi
  if [[ ! -f "$work/draining" || "$mode" = drainhang || "$mode" = angiedrainhang ]]; then echo "21 $title: worker process$generation"; fi
}
kill() {
  if [[ "$1" = -0 ]]; then
    [[ "$2" = 21 || "$2" = 42 ]] || return 1
    [[ "$mode" != newdeath || "$2" != 42 || ! -f "$work/draining" ]]
    return
  fi
  echo "kill $*" >> "$work/trace"
  case "$1" in
    -USR2)
      [[ "$mode" != usr2fail ]] || return 1
      touch "$work/switched"
      case "$mode" in badnewpid) echo 0;; samepid) echo 21;; *) echo 42;; esac > "$work/local/nginx/logs/nginx.pid"
      if [[ "$mode" = badoldbin ]]; then echo 99; else echo 21; fi > "$work/local/nginx/logs/nginx.pid.oldbin"
      ;;
    -WINCH) touch "$work/draining";;
  esac
}
DIR_TMP="$work/src" SCRIPT_DIR="$work/absent" CENTMINLOGDIR="$work/logs"
NGINX_INSTALL=y NGINXBACKUP=y INITIALINSTALL=n CENTOS_SIX=0
NGINX_HTTP2=y NGINX_DYNAMICTLS=y NGINX_SPDYPATCHED=n NGINX_PAGESPEED=n DYNAMIC_SUPPORT=n NGINXPATCH=y
OPENSSL_VERSION=3.5.8 NGINX_ZERODT=n
case "$mode" in stage*) FPM_NGINX_INSTALLDIR_ENABLE=y; FPM_NGINX_INSTALLDIR="$work/stage";; esac
[[ "$mode" != stageempty ]] || FPM_NGINX_INSTALLDIR=""
case "$mode" in stagerelative) FPM_NGINX_INSTALLDIR=relative-stage;; stageroot) FPM_NGINX_INSTALLDIR=/;; stageslashes) FPM_NGINX_INSTALLDIR=///;; esac
rm() {
  case "$mode" in stageempty|stagerelative|stageroot|stageslashes) [[ "${@: -1}" != "$FPM_NGINX_INSTALLDIR" ]] || return 99;; esac
  [[ "$mode" != stagecleanfail || "${@: -1}" != "$work/stage" ]] || return 13
  command rm "$@"
}
mkdir() { [[ "$mode" != stagedirfail || "${@: -1}" != "$work/stage" ]] || return 14; command mkdir "$@"; }
case "$mode" in live|angielive|angiestopping|angiedrainhang|usr2fail|badnewpid|samepid|badoldbin|noworkers|newstopping|newdeath|drainhang|badoldpid) NGINX_ZERODT=y;; esac
[[ "$mode" != freenginx && "$mode" != freedownloadfail ]] || FREENGINX_INSTALL=y
funct_nginxupgrade "${target:-1.31.6}"
'''


def run(source, script, mode, work):
    env = {key: os.environ[key] for key in ("PATH", "TMPDIR") if key in os.environ}
    env.update(LC_ALL="C", work=str(work), mode=mode)
    return subprocess.run([bash], input=source + "\n" + script, text=True,
                          env=env, cwd=work, capture_output=True, timeout=10)


with tempfile.TemporaryDirectory(prefix="nginx-recompile-") as tmp:
    with patch.dict(os.environ, FPM_NGINX_INSTALLDIR_ENABLE="y",
                    FPM_NGINX_INSTALLDIR=str(Path(tmp) / "inherited-stage"),
                    BASH_ENV=str(Path(tmp) / "inherited-startup.sh")):
        result = run("", '[[ -z ${FPM_NGINX_INSTALLDIR_ENABLE+x} && -z ${FPM_NGINX_INSTALLDIR+x} && -z ${BASH_ENV+x} ]]', "isolation", Path(tmp))
        assert result.returncode == 0, (result.stdout, result.stderr)

    for mode in ("backupfail", "modulesfail", "compilerfail", "luajitfail", "zlibfail", "downloadfail",
                 "stagefail", "stagecleanfail", "stagedirfail", "stageempty", "stagerelative", "stageroot", "stageslashes", "stagesuccess",
                 "invalidcache", "freedownloadfail", "extractfail", "configurefail", "makefail", "patchfail", "installfail",
                 "configtestfail", "restartfail", "usr2fail", "badnewpid", "samepid",
                 "badoldbin", "noworkers", "newstopping", "newdeath", "drainhang", "badoldpid",
                 "success", "live", "angie", "angielive", "angiestopping", "angiedrainhang", "freenginx", "invalidversion", "maintenanceonfail", "maintenanceofffail"):
        work = Path(tmp) / mode
        for directory in ("logs", "src/nginx-1.31.6", "src/freenginx-1.31.6", "local/sbin", "local/nginx/logs"):
            (work / directory).mkdir(parents=True, exist_ok=True)
        (work / "local/nginx/logs/nginx.pid").write_text("0\n" if mode == "badoldpid" else "21\n")
        binary = work / "local/sbin/nginx"
        binary.write_text('#!/bin/bash\necho "nginx $*" >> "$work/trace"\n[[ "$mode" != configtestfail ]]\n')
        binary.chmod(0o700)
        source = upgrade.replace("/usr/local/", str(work / "local") + "/")
        script = mocks if mode != "invalidversion" else 'target="1.31.6 /tmp/unsafe"\n' + mocks
        result = run(source, script, mode, work)
        trace = (work / "trace").read_text() if (work / "trace").exists() else ""
        expected_success = mode in ("success", "live", "angie", "angielive", "freenginx", "invalidcache", "stagesuccess")
        assert (result.returncode == 0) == expected_success, (mode, result.returncode, result.stderr, result.stdout)
        assert "nginx -s stop" not in trace, mode
        assert "backup\nmap\n" in trace or mode in ("backupfail", "invalidversion"), (mode, trace)
        if mode == "backupfail":
            assert "map\n" not in trace, mode
        if mode in ("backupfail", "modulesfail", "compilerfail", "luajitfail", "zlibfail", "downloadfail", "freedownloadfail",
                    "extractfail", "configurefail", "makefail", "patchfail", "invalidversion"):
            assert "maintenance_" not in trace and "make install" not in trace, (mode, trace)
        if mode in ("stagefail", "stagecleanfail", "stagedirfail", "stageempty", "stagerelative", "stageroot", "stageslashes"):
            assert "maintenance_" not in trace and "make install\n" not in trace, (mode, trace)
            assert "service nginx restart" not in trace and "kill -USR2" not in trace, (mode, trace)
            assert result.returncode == {"stagefail": 7, "stagecleanfail": 13, "stagedirfail": 14}.get(mode, 1), (mode, result.returncode)
        if mode == "stagesuccess":
            assert trace.index("make install DESTDIR=") < trace.index("maintenance_on") < trace.index("make install\n"), trace
        if mode in ("maintenanceonfail", "installfail", "configtestfail"):
            assert "service nginx restart" not in trace and "kill -USR2" not in trace, (mode, trace)
        if mode in ("maintenanceonfail", "installfail", "configtestfail", "restartfail", "usr2fail", "badnewpid",
                    "samepid", "badoldbin", "noworkers", "newstopping", "angiestopping", "newdeath", "drainhang", "angiedrainhang"):
            assert "maintenance_off" not in trace, (mode, trace)
        if mode in ("usr2fail", "badnewpid", "samepid", "badoldbin", "noworkers", "newstopping", "angiestopping", "badoldpid"):
            assert "kill -WINCH" not in trace and "kill -QUIT" not in trace, (mode, trace)
        if mode in ("newdeath", "drainhang", "angiedrainhang"):
            assert "kill -WINCH 21" in trace and "kill -QUIT" not in trace, (mode, trace)
        if expected_success:
            assert trace.index("make install") < trace.index("nginx -t"), (mode, trace)
        if mode in ("live", "angielive"):
            assert trace.index("nginx -t") < trace.index("kill -USR2 21") < trace.index("kill -WINCH 21") < trace.index("kill -QUIT 21"), trace
        if mode == "freenginx":
            assert "tar -tzf freenginx-1.31.6.tar.gz" in trace and "tar xvfz freenginx-1.31.6.tar.gz" in trace, trace
        if mode == "downloadfail":
            assert "wget https://nginx.org/download/nginx-1.31.6.tar.gz -O nginx-1.31.6.tar.gz" in trace, trace
        if mode == "freedownloadfail":
            assert "wget https://freenginx.org/download/freenginx-1.31.6.tar.gz -O freenginx-1.31.6.tar.gz" in trace, trace
            assert "https://nginx.org/" not in trace and "https://centminmod.com/" not in trace, trace
        if mode == "invalidcache":
            assert "download https://nginx.org/download/nginx-1.31.6.tar.gz" in trace and "wget " not in trace, trace

    # Real PCRE caches survive an offline run unchanged, for both versions.
    work = Path(tmp) / "pcre-cache"
    (work / "src/pcre-fixture").mkdir(parents=True)
    (work / "logs").mkdir()
    (work / "src/pcre-fixture/configure").write_text("cached source\n")
    archive = work / "src/pcre.tar.gz"
    with tarfile.open(archive, "w:gz") as cached:
        cached.add(work / "src/pcre-fixture", arcname="pcre-fixture")
    original = archive.read_bytes()
    pcre = (root / "inc/pcre.inc").read_text()
    for action in ("pcredir_check", "pcre_two_dir_check"):
        result = run(pcre, r'''
DIR_TMP="$work/src" CENTMINLOGDIR="$work/logs" DT=test
PCRELINKFILE=pcre.tar.gz PCRETWOLINKFILE=pcre.tar.gz
nginxpcretarball() { echo UNEXPECTED_DOWNLOAD; return 73; }
nginxpcretwotarball() { echo UNEXPECTED_DOWNLOAD; return 73; }
''' + action + '\n', action, work)
        assert result.returncode == 0 and "UNEXPECTED_DOWNLOAD" not in result.stdout, (action, result.stdout, result.stderr)
        assert archive.read_bytes() == original and (work / "src/pcre-fixture/configure").read_text() == "cached source\n"

    # The actual LuaJIT helper retains the first failed Git/build/install status.
    luajit = (root / "inc/luajit.inc").read_text()
    lua_mocks = r'''
DIR_TMP="$work/src" ORESTY_LUANGINX=y LUAJIT_GITINSTALL=y LUAJIT_GITINSTALLVER=2.1 INITIALINSTALL=n
cecho() { :; }; sar_call() { :; }
uname() { echo "$arch"; }
git() {
  echo "git $*" >> "$work/trace"
  case "$mode:$1" in stash:stash) return 21;; pull:pull) return 22;; clone:clone) return 23;; esac
}
make() {
  echo "make $*" >> "$work/trace"
  case "$mode:${1:-build}" in clean:clean) return 31;; build:build|build:XCFLAGS=*|build:PREFIX=*) return 32;; install:install) return 33;; esac
}
[[ "$mode" != disabled ]] || ORESTY_LUANGINX=n
[[ "$mode" != cd ]] || DIR_TMP="$work/missing"
luajitinstall
'''
    for arch in ("x86_64", "aarch64"):
        for mode, status in (("stash", 21), ("pull", 22), ("clone", 23), ("cd", 1), ("clean", 31),
                             ("build", 32), ("install", 33), ("version", 34), ("emptyversion", 1), ("success", 0), ("disabled", 0)):
            work = Path(tmp) / ("lua-" + arch + "-" + mode)
            (work / "src").mkdir(parents=True)
            if mode != "clone":
                tree = work / "src/LuaJIT-2.1"
                (tree / ".git").mkdir(parents=True)
                (tree / "src").mkdir()
                binary = tree / "src/luajit"
                binary.write_text('#!/bin/bash\n[[ "$mode" != version ]] || exit 34\n[[ "$mode" != emptyversion ]] || exit 0\necho "LuaJIT 2.1"\n')
                binary.chmod(0o700)
            result = run(luajit.replace("/usr/local/", str(work / "local") + "/"),
                         "arch=" + arch + "\n" + lua_mocks, mode, work)
            trace = (work / "trace").read_text() if (work / "trace").exists() else ""
            assert result.returncode == status, (arch, mode, result.returncode, result.stdout, result.stderr)
            if mode in ("stash", "pull", "clone", "cd", "clean", "build", "disabled"):
                assert "make install" not in trace, (mode, trace)
            if mode in ("stash", "pull", "clone", "cd", "disabled"):
                assert "make " not in trace, (mode, trace)
            if mode == "success":
                assert ("XCFLAGS=-DLUAJIT_ENABLE_GC64" in trace) == (arch == "x86_64"), trace

    # The real initial-install caller stops when LuaJIT rejects its build.
    work = Path(tmp) / "initial-lua"
    (work / "src").mkdir(parents=True)
    initial = (root / "inc/nginx_install.inc").read_text()
    initial = initial[initial.index("ngxinstallmain() {"):].replace("/usr/local/", str(work / "local") + "/")
    result = run(initial, r'''
DIR_TMP="$work/src" NGINX_INSTALL=y NGINX_VERSION=1.31.6 CENTOS_SEVEN=7
NGINX_PCRE_TWO=n INITIALINSTALL=n OPENSSL_SYSTEM_USE=y
for fn in cecho opt_tcp nginx_dependency_compiler install_gperftools pcredir_check perl_ipc_cmd_install; do eval "$fn() { :; }"; done
rpm() { return 1; }; bc() { cat >/dev/null; echo 0; }
luajitinstall() { return 23; }
nginxzlib_install() { echo UNEXPECTED_ZLIB; }
ngxinstallmain
''', "initial-lua", work)
    assert result.returncode == 23 and "UNEXPECTED_ZLIB" not in result.stdout, (result.returncode, result.stdout, result.stderr)

    # Optional finish reporting cannot mask or manufacture an install failure.
    finish = (root / "inc/centminfinish.inc").read_text()
    finish = finish[finish.index("centminfinish() {"):]
    for mode in ("finish-success", "finish-lockfail"):
        work = Path(tmp) / mode
        (work / "repo/tools").mkdir(parents=True)
        (work / "yum.log").touch()
        lock = work / "repo/tools/php-libs-versionlock.sh"
        lock.write_text('#!/bin/bash\necho libs-lock >> "$work/trace"\n[[ "$mode" != finish-lockfail ]] || exit 73\n')
        result = run(finish, r'''
CENTMINLOGDIR="$work" SCRIPT_DIR="$work/repo" YUMLOG_FILE="$work/yum.log"
SCRIPT_VERSION=test DT=test PHP_LIBS_VERSIONLOCK=y TS_INSTALL=y
installchecks() { :; }; nvcheck() { :; }; ps() { echo rsyslog; }
cecho() { echo "$*" >> "$work/trace"; }
curl() { echo optional-curl >> "$work/trace"; return 73; }
centminfinish
''', mode, work)
        trace = (work / "trace").read_text()
        assert result.returncode == (73 if mode == "finish-lockfail" else 0), (mode, result.returncode, result.stdout, result.stderr)
        assert "libs-lock" in trace, trace
        assert ("optional-curl" in trace) == (mode == "finish-success"), trace
        assert ("Centmin Mod install completed" in trace) == (mode == "finish-success"), trace

    # The real module checker must propagate serial and named background failures.
    work = Path(tmp) / "modules"
    (work / "src").mkdir(parents=True)
    checker = upgrade[upgrade.index("checknginxmodules() {"):upgrade.index("\nclear_ps() {")]
    for parallel in ("n", "y"):
        script = r'''
DIR_TMP="$work/src" CONFIGSCANBASE="$work/absent" CM_INSTALLDIR="$work/absent"
NGX_LUANGINXLINKFILE=lua.tar.gz LIBRESSL_LINKFILE=libressl.tar.gz NGX_ZLIBLINKFILE=zlib.tar.gz
LIBRESSL_SWITCH=n NGINX_PAGESPEED=n NGINX_OPENRESTY=n
axelsetup() { :; }; cecho() { :; }; tar() { :; }
libressldownload() { :; }; nginxzlibtarball() { :; }
ngxmoduletarball() { return 23; }; openssldownload() { :; }
grep() { if [[ "$*" = '-c processor /proc/cpuinfo' ]]; then echo 2; else command grep "$@"; fi; }
checknginxmodules
'''
        result = run(checker, "PARALLEL_MODE=" + parallel + "\n" + script, parallel, work)
        assert result.returncode != 0, "module failure swallowed, parallel=" + parallel

    # Real backup helper repairs missing children, preserves cwd and rejects failed tar.
    work = Path(tmp) / "backup"
    (work / "nginx/conf").mkdir(parents=True)
    (work / "nginx/conf/nginx.conf").write_text("events {}\n")
    (work / "backup").mkdir()
    script = 'NGINXDIR="$work/nginx" NGINXBACKUPDIR="$work/backup"\nbefore=$PWD\nnginxbackup || exit 1\n[[ "$PWD" = "$before" ]]\n'
    result = run(backup, script, "backup", work)
    assert result.returncode == 0, result.stderr
    result = run(backup, 'tar() { return 28; }\n' + script, "backupfail", work)
    assert result.returncode != 0, "backup tar failure swallowed"

    # Check the actual maintenance functions, with service mocked and config isolated.
    work = Path(tmp) / "maintenance"
    (work / "local/nginx/conf").mkdir(parents=True)
    (work / "local/nginx/conf/sitestatus.conf").write_text("default 0;\n")
    site = (root / "tools/sitestatus.sh").read_text()
    site = site[site.index("checkstatus() {"):site.index('case "$1" in')]
    site = site.replace("/usr/local/", str(work / "local") + "/")
    for action in ("son", "soff"):
        result = run(site, f'CHECK=n\nsed() {{ :; }}\nservice() {{ [[ "$*" = "nginx reload" ]] || exit 99; return 7; }}\n{action}\n', action, work)
        assert result.returncode == 7, (action, result.returncode, result.stderr)
        result = run(site, f'CHECK=n\nsed() {{ return 9; }}\nservice() {{ echo UNEXPECTED_RELOAD; }}\n{action}\n', action, work)
        assert result.returncode == 9 and "UNEXPECTED_RELOAD" not in result.stdout, (action, result.returncode, result.stdout, result.stderr)

print("PASS: Nginx source, backup, build/staging failures, maintenance/promotion guards, PCRE caches and LuaJIT propagation")
