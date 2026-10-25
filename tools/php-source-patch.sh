#!/bin/bash
###########################################################
# Centmin Mod PHP source patcher: apply Centmin Mod's PHP
# security backport patches (patches/php) to any PHP source
# tree, built by Centmin Mod or not.
#
# It selects exactly the patches, order and patch options
# that php_patches() in inc/php_patch.inc would use for the
# tree's PHP version, OS and options, applies them and writes
# a per-patch outcome record.
#
# usage:
#   tools/php-source-patch.sh --src /path/to/php-8.1.34
#   tools/php-source-patch.sh --src DIR --dry-run
#   tools/php-source-patch.sh --list --version 7.4.33 --os el8
#   tools/php-source-patch.sh --src php-8.1.34.tar.xz --tarball /srv/dist/
#   options:
#     --src DIR|TARBALL  PHP source tree, or a php.net style .tar.gz/.tar.xz/
#                        .tar.bz2 (patched in a scratch copy, so it needs
#                        --tarball, --list or --dry-run); main/php_version.h
#                        gives the version
#     --tarball OUT      patch a copy of the source and pack it like php.net:
#                        top directory php-X.Y.Z, OUT.sha256 beside it and the
#                        record inside as php-X.Y.Z/.php-source-patch/record.tsv.
#                        OUT ends in .tar.gz, .tgz or .tar.xz; a directory gets
#                        php-X.Y.Z-centmin-<commit>.tar.xz. No tarball when a
#                        patch fails (the record goes to OUT.record.tsv); the
#                        source given to --src is not changed. Left out: .git,
#                        Centmin Mod's copied top-level *.patch files and the
#                        backups patching made; owners 0/0, nothing group or
#                        world writable. Refused: a configured tree, symlinks
#                        that point outside the tree, OUT in or as the input
#     --version X.Y.Z    PHP version (must match the tree when --src is given)
#     --patch-dir DIR    patches/php of a Centmin Mod checkout or release bundle
#                        (default: the checkout or bundle this script is in)
#     --inc FILE         php_patch.inc (default: inc/php_patch.inc beside patches/)
#     --os OS            other, el7, el8, el9 or el10 (default: detected)
#     --set NAME=y|n     PHP_PATCH, PHP_MYSQLND_PATCH_FIX, FPM_WAKEUP_PATCH or
#                        PHP_PATCH_OPENSSL_THREE. Defaults are Centmin Mod's,
#                        then /etc/centminmod/custom_config.inc when this runs
#                        from a Centmin Mod checkout
#     --dry-run          trial-apply on a scratch copy, change nothing
#     --list             show the selection only (needs --src or --version)
#     --record FILE      outcome record (default SRC/.php-source-patch/record.tsv)
#     --verbose          show patch output
#     --manifest [--versions "X.Y.Z ..."]
#                        print the selection manifest (TSV) for the final
#                        release of each end-of-life PHP branch, every OS
#     --check-manifest FILE
#                        regenerate FILE and fail (exit 3) when it differs
#     --make-bundle DIR  write a release bundle from a Centmin Mod checkout
#     --self-test        run the built-in tests
#   environment: PATCH=gpatch selects the GNU patch binary, TAR=gtar the GNU
#   tar binary (--tarball needs GNU tar). Patches with git
#   renames need GNU patch 2.7 or newer; CentOS 6 ships 2.6, which stops at
#   php80-cve-2025-1219 and leaves renamed 7.2/7.4 test files at old names
#
# exit: 0 done, 1 a patch failed (earlier patches stay applied, the failed
# one changed nothing), 2 usage or setup error, 3 manifest drift
#
# Selection: php_patches() itself decides. The script runs it with every
# external command stubbed (cp, patch, grep, ...) under an empty PATH. The
# stubs record each patch call: file, options, directory and php_patch.inc
# line. Unknown commands, writes outside the scratch dir and unset variables
# make the selection fail instead of guessing; recorded patch options must be
# known patch flags and paths must stay inside the tree. The sandbox catches
# php_patch.inc changes the script does not model; it is not a jail, so
# php_patch.inc is trusted code, from the same checkout or checksummed bundle
# as the patches. Content checks (CentOS 7 ffi and OpenSSL 1.0.2 shims) read
# the real tree through a symlinked shadow. The NEWS-only tolerance of each
# call site is probed the same way.
#
# Applying: patches already in the tree are found first by reversing the
# selection last to first on a scratch copy (never by a copied patch file,
# which php_patch.inc treats as "already applied"). Each other patch is
# trial-run on a scratch copy of the files it touches. Applied: the trial was
# clean, so it runs on the tree. NEWS-only: as php_patch_news_only_reject(),
# only at call sites that use it. Any other result stops with exit 1, leaving
# the tree unchanged by that patch, so a rerun after a fix resumes there.
# The record lists each patch: bugid, file, sha256, php_patch.inc line and
# status (applied, news-only, already-applied, skipped-missing-file,
# skipped-duplicate, skipped-gate, failed, not-run).
###########################################################
export LC_ALL=C
PSP_VERSION=1.0.0
PSP_OPTS='PHP_PATCH PHP_MYSQLND_PATCH_FIX FPM_WAKEUP_PATCH PHP_PATCH_OPENSSL_THREE'
# Centmin Mod defaults (centmin.sh); centmin.sh or a bundle's BUNDLE file override them
PSP_DEFAULTS='PHP_PATCH=y PHP_MYSQLND_PATCH_FIX=y FPM_WAKEUP_PATCH=n PHP_PATCH_OPENSSL_THREE=y'
PSP_OSES='other el7 el8 el9 el10'
# final release of each end-of-life branch, the --manifest default
PSP_EOL_VERSIONS='5.6.40 7.0.33 7.1.33 7.2.34 7.3.33 7.4.33 8.0.30 8.1.34'
# grep -oP '(?<=\+\+\+ )[^\s]+' without grep -P; strip=1 adds the sed 's/b\///' step
PSP_AWK_PLUS='{
  s = $0; p = 1
  while (1) {
    b = (p > 4) ? p - 4 : 1
    i = index(substr(s, b), "+++ ")
    if (!i) break
    j = b + i + 3
    r = substr(s, j)
    if (match(r, /^[^ \t\r\f\v]+/)) {
      n = substr(r, 1, RLENGTH)
      if (strip) sub(/b\//, "", n)
      print n
      p = j + RLENGTH
    } else p = j + 1
  }
}'

###########################################################
# selection sandbox: runs in a fresh "env -i" bash (see run_trace)
trace_driver() {
  __psp_inc=$1 CUR_DIR=$2 __psp_t=$3 PHPVER_ID=$4 PHP_VERSION=$5 __psp_os=$6 __psp_opts=$7 __psp_probe=$8
  __psp_ev=$__psp_t/ev __psp_src=- __psp_dst=- __psp_cwd=.
  for __psp_c in awk grep sed rm mv find wc; do
    eval "__psp_$__psp_c=\$(command -v $__psp_c)" && eval "[ -n \"\$__psp_$__psp_c\" ]" || exit 3
  done
  PATH=/nonexistent/php-source-patch

  __psp_e() { local IFS=$'\t'; printf '%s\n' "$*" >> "$__psp_ev"; }
  __psp_bad() { __psp_e X "${BASH_LINENO[1]}" "$*"; }
  # paths a stub may write: plain names in the shadow (whose subdirs link into the
  # source tree) or anything under the scratch dir
  __psp_safe() {
    case $1 in
      *..*) return 1 ;;
      "$__psp_t"/*) return 0 ;;
      /*|*/*) return 1 ;;
    esac
    [ ! -h "$1" ]
  }
  cp() {
    local a s='' d='' n=0
    for a in "$@"; do
      case $a in -a|-p|-f) ;; *) n=$((n + 1)); if [ $n = 1 ]; then s=$a; else d=$a; fi ;; esac
    done
    { [ $n = 2 ] && __psp_safe "$d"; } || { __psp_bad cp "$@"; return 1; }
    [ -f "$s" ] || return 1
    __psp_src=$s __psp_dst=$d
    : > "$d"
  }
  patch() {
    local line=${BASH_LINENO[0]} args="$*"
    if [ "$line" = "$__psp_probe" ] && [ ! -e "$__psp_t/probed" ]; then
      # pretend the code hunks applied and only NEWS was rejected
      : > "$__psp_t/probed"
      { __psp_safe NEWS.rej && __psp_safe NEWS.orig && __psp_safe "$__psp_dst"; } || { __psp_bad probe; return 1; }
      printf -- '--- a/NEWS\n+++ b/NEWS\n' > "$__psp_dst"
      : > NEWS.rej
      : > NEWS.orig
      __psp_e P "$line" "$__psp_cwd" "$__psp_src" "${bugid:--}" "${args:--}"
      printf 'patching file NEWS\nHunk #1 FAILED at 1.\n1 out of 1 hunk FAILED -- saving rejects to file NEWS.rej\n'
      return 1
    fi
    __psp_e P "$line" "$__psp_cwd" "$__psp_src" "${bugid:--}" "${args:--}"
  }
  dos2unix() { return 0; }
  ls() { return 0; }
  mktemp() { printf '%s\n' "$__psp_t/tmp.$RANDOM"; }
  tee() { local l; while IFS= read -r l || [ -n "$l" ]; do printf '%s\n' "$l"; done; }
  touch() { local args="$*"; __psp_e T "${BASH_LINENO[0]}" "$__psp_cwd" "${args:--}"; }
  pushd() { [ "$__psp_cwd" = . ] || { __psp_bad pushd "$@"; return 1; }; __psp_cwd=$1; }
  popd() { __psp_cwd=.; }
  grep() {
    if [ "${1-}" = -oP ]; then
      { [ $# = 3 ] && [ "$2" = '(?<=\+\+\+ )[^\s]+' ]; } || { __psp_bad grep "$@"; return 2; }
      "$__psp_awk" -v strip=0 "$PSP_AWK_PLUS" "$3"
      return 0
    fi
    "$__psp_grep" "$@"
  }
  sed() {
    local a
    for a in "$@"; do case $a in -i*|--in-place*) __psp_bad sed "$@"; return 1 ;; esac; done
    "$__psp_sed" "$@"
  }
  rm() {
    local a
    for a in "$@"; do case $a in -*) ;; *) __psp_safe "$a" || { __psp_bad rm "$@"; return 1; } ;; esac; done
    "$__psp_rm" "$@"
  }
  mv() {
    local a last=
    for a in "$@"; do
      case $a in -*) ;; *) last=$a; __psp_safe "$a" || [ "$a" = NEWS ] || { __psp_bad mv "$@"; return 1; } ;; esac
    done
    [ -d "$last" ] && { __psp_bad mv "$@"; return 1; }
    "$__psp_mv" "$@"
  }
  find() {
    local a
    for a in "$@"; do case $a in -delete|-exec*|-ok*|-fprint*|-fls) __psp_bad find "$@"; return 1 ;; esac; done
    "$__psp_find" "$@"
  }
  wc() { "$__psp_wc" "$@"; }
  echo() {
    builtin echo "$@"
    case "$*" in *' patch file already applied') __psp_e M "${BASH_LINENO[0]}" "$__psp_cwd" "${1%% *}" ;; esac
  }
  command_not_found_handle() { __psp_e X "${BASH_LINENO[0]}" "$*"; return 127; }

  PHP_PATCH=n PHP_MYSQLND_PATCH_FIX=n FPM_WAKEUP_PATCH=n PHP_PATCH_OPENSSL_THREE=n
  for __psp_kv in ${__psp_opts//,/ }; do
    case ${__psp_kv%%=*} in PHP_PATCH|PHP_MYSQLND_PATCH_FIX|FPM_WAKEUP_PATCH|PHP_PATCH_OPENSSL_THREE) ;; *) exit 3 ;; esac
    case ${__psp_kv#*=} in y|n) printf -v "${__psp_kv%%=*}" '%s' "${__psp_kv#*=}" ;; *) exit 3 ;; esac
  done
  CENTOS_SEVEN=0 CENTOS_EIGHT=0 CENTOS_NINE=0 CENTOS_TEN=0
  case $__psp_os in
    el7) CENTOS_SEVEN=7 ;; el8) CENTOS_EIGHT=8 ;; el9) CENTOS_NINE=9 ;; el10) CENTOS_TEN=10 ;; other) ;; *) exit 3 ;;
  esac
  # no default PHP version here: PHPMVER is the target branch, like PHPMUVER
  phpver=$PHP_VERSION PHPMUVER=${PHP_VERSION%.*} PHPMVER=${PHP_VERSION%.*}
  DIR_TMP=$__psp_t CENTMINLOGDIR=$__psp_t DT=trace FPM_MAX_SPAWN_RATE=0
  builtin cd "$__psp_t/php-$PHP_VERSION" || exit 3
  set -u
  . "$__psp_inc" || exit 3
  php_patches
  __psp_e R "$?"
}

###########################################################
# helpers
die() { echo "php-source-patch: $*" >&2; exit 2; }

# EXIT trap: tarball temp files beside OUT, then the scratch dir (chmod first: a
# tree with read-only directories would otherwise survive rm -rf)
cleanup() {
  [ -z "${TB_TMP:-}" ] || rm -f "$TB_TMP"
  [ -z "${TB_SHA_TMP:-}" ] || rm -f "$TB_SHA_TMP"
  [ -n "${PSP_KEEP_TMP:-}" ] || [ -z "${W:-}" ] || { chmod -R u+rwX "$W" 2>/dev/null; rm -rf "$W"; }
}

sha256_of() {  # FILE... -> "sha  file" lines
  if command -v sha256sum >/dev/null 2>&1; then sha256sum -- "$@"
  elif command -v shasum >/dev/null 2>&1; then shasum -a 256 -- "$@"
  else
    local f
    for f in "$@"; do
      case $f in -*) f=./$f ;; esac
      printf '%s  %s\n' "$(openssl dgst -sha256 -r "$f" | awk '{print $1}')" "${f#./}"
    done
  fi
}

# values from the trace that reach a command line: patch options, directories and
# touched files. Only known patch flags; paths must be plain and stay in the tree
args_ok() {
  local a
  for a in $1; do
    case $a in
      -) ;;
      -p[0-9]|-p[0-9][0-9]|--strip=[0-9]|-N|--forward|-l|--ignore-whitespace|-F[0-9]|--fuzz=[0-9]|-E|--remove-empty-files|-s|--silent|--quiet|--binary) ;;
      *) return 1 ;;
    esac
  done
}
path_ok() {
  case $1 in
    .) return 0 ;;
    ''|/*|-*|..|../*|*/..|*/../*|*[!A-Za-z0-9._/+-]*) return 1 ;;
  esac
}

plus_names() { awk -v strip="$2" "$PSP_AWK_PLUS" "$1"; }

# php742-cve-2026-14355.patch -> cve-2026-14355, intel/x.patch -> x
bugid_of() {
  local b=${1##*/}
  b=${b%.patch}
  [[ $b =~ ^php[0-9]+-(.+)$ ]] && b=${BASH_REMATCH[1]}
  echo "$b"
}

declare -A PSP_SHA
sha_of_patch() {  # file relative to PDIR, cached
  [ -n "${PSP_SHA[$1]:-}" ] || PSP_SHA[$1]=$(sha256_of "$PDIR/$1" | awk '{ print $1 }')
  echo "${PSP_SHA[$1]}"
}

# file names a patch may read or write, after -pN (strip -1: no -p, so
# GNU patch may use the whole name or its basename); over-listing is harmless
patch_paths() {
  awk -v strip="$2" '
  function nm(p,   k, a, n) {
    sub(/[ \t].*$/, "", p)
    if (p == "" || p == "/dev/null") return
    if (strip < 0) { print p; n = split(p, a, "/"); print a[n]; return }
    for (k = 0; k < strip; k++) { if (!index(p, "/")) return; p = substr(p, index(p, "/") + 1) }
    print p
  }
  /^--- / || /^\+\+\+ / { nm(substr($0, 5)) }
  /^diff --git / { nm($3); nm($4) }
  /^rename from / || /^copy from / { s = $0; sub(/^(rename|copy) from /, "", s); nm2 = s; print nm2 }
  ' "$1" | awk '!seen[$0]++'
}

strip_of() {
  local a prev=
  for a in "$@"; do
    case $a in -p[0-9]*) echo "${a#-p}"; return ;; --strip=*) echo "${a#--strip=}"; return ;; esac
    case $prev in -p|--strip) echo "$a"; return ;; esac
    prev=$a
  done
  echo -1
}

# dos2unix without dos2unix: CRLF -> LF, lone CR kept
normalize() {
  if grep -q "$(printf '\r')" "$1"; then awk '{ sub(/\r$/, ""); print }' "$1" > "$2"; else cp "$1" "$2"; fi
}

# copy the listed paths (stdin, relative to cwd $3) that exist in tree $1 to $2
mirror() {
  local from=$1 to=$2 cwd=$3 q list=$2.list
  rm -rf "$to"
  mkdir -p "$to/$cwd" || return 2
  : > "$list"
  while IFS= read -r q; do
    case $q in /*|..|../*|*/../*|*/..) continue ;; esac
    [ "$cwd" = . ] || q=$cwd/$q
    [ -f "$from/$q" ] && printf '%s\n' "$q" >> "$list"
  done
  [ -s "$list" ] || return 0
  # "./" keeps GNU tar 1.29+ from reading a listed name that starts with "-" as an
  # option (--checkpoint-action=exec=... runs a command)
  sed 's|^|./|' "$list" > "$list.t"
  (cd "$from" && tar chf - -T "$list.t") | (cd "$to" && tar xf -)
}

has_rej() {  # dir cwd candfile
  local q
  while IFS= read -r q; do [ -e "$1/$2/$q.rej" ] && return 0; done < "$3"
  return 1
}

# php_patch_news_only_reject(): patch exited 1, NEWS.rej is the only reject
# and nothing was skipped, ignored or reversed
news_only() {
  local p=$1 rc=$2 out=$3 dir=$4 n news=n
  [ "$rc" = 1 ] || return 1
  printf '%s\n' "$out" | grep -qiE "can't find file to patch|Skipping patch|hunks? ignored|Reversed" && return 1
  while IFS= read -r n; do
    if [ -f "$dir/$n.rej" ]; then [ "$n" = NEWS ] || return 1; news=y; fi
  done < <(plus_names "$p" 1)
  [ $news = y ]
}

detect_os() {
  local id='' like='' ver='' major
  if [ -r /etc/os-release ]; then
    id=$(awk -F= '$1 == "ID" { gsub(/"/, "", $2); print $2 }' /etc/os-release)
    like=$(awk -F= '$1 == "ID_LIKE" { gsub(/"/, "", $2); print $2 }' /etc/os-release)
    ver=$(awk -F= '$1 == "VERSION_ID" { gsub(/"/, "", $2); print $2 }' /etc/os-release)
  elif [ -r /etc/redhat-release ]; then
    id=rhel
    ver=$(sed -n 's/.*release \([0-9][0-9]*\).*/\1/p' /etc/redhat-release)
  fi
  case " $id $like " in
    *' rhel '*|*' centos '*|*' ol '*) major=${ver%%.*} ;;
    *) echo other; return ;;
  esac
  case $major in 7|8|9|10) echo "el$major" ;; *) echo other ;; esac
}

opt_get() { local kv; for kv in $OPTSTR; do [ "${kv%%=*}" = "$1" ] && { echo "${kv#*=}"; return; }; done; }
opt_set() {  # NAME VALUE SOURCE
  local kv out=
  case " $PSP_OPTS " in *" $1 "*) ;; *) die "unknown option $1 (known: $PSP_OPTS)" ;; esac
  case $2 in [yY]) set -- "$1" y "$3" ;; [nN]) set -- "$1" n "$3" ;; *) die "$1 must be y or n" ;; esac
  for kv in $OPTSTR; do [ "${kv%%=*}" = "$1" ] || out="$out $kv"; done
  OPTSTR="${out# } $1=$2"
  OPTSTR=${OPTSTR# }
  [ -n "$3" ] && OPTSRC="$OPTSRC $1($3)"
}
opt_csv() {  # options in PSP_OPTS order, comma separated
  local o out=
  for o in $PSP_OPTS; do out="$out,$o=$(opt_get "$o")"; done
  echo "${out#,}"
}
# last NAME='y|n' assignment in a bash config file, without sourcing it
opt_from_file() {
  awk -v n="$1" '
    { sub(/^[ \t]+/, "") }
    index($0, n "=") == 1 { v = substr($0, length(n) + 2); gsub(/["'"'"']/, "", v); v = substr(v, 1, 1); if (v ~ /^[yYnN]$/) last = v }
    END { if (last != "") print last }' "$2"
}

###########################################################
# selection
run_trace() {  # dir id ver os opts probe-line shadow(tree|fake|empty)
  local d=$1 ver=$3 mode=$7 e sh
  local -a links=()
  rm -rf "$d"
  sh=$d/php-$ver
  mkdir -p "$sh" || return 2
  case $mode in
    tree)
      for e in "$SRC"/*; do
        case ${e##*/} in *.patch|*.orig|*.rej) continue ;; esac
        links[${#links[@]}]=$e
      done
      [ ${#links[@]} -gt 0 ] && ln -s "${links[@]}" "$sh/"
      ;;
    fake)
      # content a content-gated block looks for, so --list without a tree shows it
      mkdir -p "$sh/ext/ffi" "$sh/ext/openssl" "$sh/Zend"
      printf '/* ffi */\n' > "$sh/ext/ffi/ffi.c"
      printf 'GEN_IPADD\nx = ASN1_STRING_get0_data(y);\n' > "$sh/ext/openssl/xp_ssl.c"
      : > "$sh/Zend/zend_vm_gen.php"
      ;;
  esac
  env -i HOME=/ LC_ALL=C TMPDIR="$d" PATH="$PATH" "$BASH" "$SELF" --trace-driver \
    "$INC" "$CUR_DIR" "$d" "$2" "$ver" "$4" "$5" "$6" > "$d/out" 2>&1
  : >> "$d/ev"
}

trace_error() {  # dir: why a trace is unusable, empty when it is fine
  local d=$1 rc
  rc=$(awk -F'\t' '$1 == "R" { print $2 }' "$d/ev")
  if grep -q '^X' "$d/ev"; then
    awk -F'\t' '$1 == "X" { printf "php_patch.inc:%s: unsupported in the sandbox: %s\n", $2, $3 }' "$d/ev" | sort -u
  elif [ -z "$rc" ]; then
    echo "php_patches() did not finish:"; tail -5 "$d/out"
  elif grep -q 'unbound variable' "$d/out"; then
    echo "php_patches() reads a variable this script does not set:"; grep 'unbound variable' "$d/out" | sort -u
  elif [ "$rc" != 0 ]; then
    echo "php_patches() exits $rc for this version even when every patch applies:"; tail -5 "$d/out"
  fi
}

# P events as "file|args|cwd" keys, one per line
trace_keys() { awk -F'\t' -v pre="$CUR_DIR/patches/php/" '$1 == "P" { f = $4; if (index(f, pre) == 1) f = substr(f, length(pre) + 1); print f "|" $6 "|" $3 }' "$1/ev"; }

# fills SEL_* for one configuration; mode tree|fake. Sets SEL_N
select_patches() {  # id ver os opts mode
  local id=$1 ver=$2 os=$3 opts=$4 mode=$5 err k line cwd src bug args pre=$CUR_DIR/patches/php/ i n a
  local d=$W/trace
  run_trace "$d" "$id" "$ver" "$os" "$opts" '' "$mode"
  err=$(trace_error "$d")
  [ -z "$err" ] || { echo "$err" >&2; return 2; }
  local -A seen=()
  SEL_N=0
  while IFS=$'\t' read -r k line cwd src bug args; do
    case $k in P|T|M) ;; *) continue ;; esac
    n=$SEL_N
    SEL_TYPE[n]=$k SEL_LINE[n]=$line SEL_CWD[n]=$cwd SEL_NEWS[n]=- SEL_CONTENT[n]=n
    path_ok "$cwd" || { echo "php_patch.inc:$line: unsupported directory: $cwd" >&2; return 2; }
    case $k in
      P)
        case $src in "$pre"*) src=${src#"$pre"} ;; *) echo "php_patch.inc:$line: patch source outside patches/php: $src" >&2; return 2 ;; esac
        path_ok "$src" || { echo "php_patch.inc:$line: unsupported patch file name: $src" >&2; return 2; }
        args_ok "$args" || { echo "php_patch.inc:$line: patch options this script does not support: $args" >&2; return 2; }
        # $bugid is the loop's id only if it names this file; outside loops it is stale
        case ${src##*/} in *-"$bug".patch|"$bug".patch) ;; *) bug=$(bugid_of "$src") ;; esac
        SEL_FILE[n]=$src SEL_BUGID[n]=$bug SEL_ARGS[n]=$args
        # the same patch called twice (140/132 apply pcre1038 twice): one application
        [ -n "${seen[$src|$args|$cwd]:-}" ] && SEL_TYPE[n]=D
        seen[$src|$args|$cwd]=1
        ;;
      T)
        for a in $src; do path_ok "$a" || { echo "php_patch.inc:$line: unsupported touch argument: $a" >&2; return 2; }; done
        SEL_FILE[n]=- SEL_BUGID[n]=- SEL_ARGS[n]=$src
        ;;
      M)
        # a name php_patch.inc logs as "already applied": a copied marker cannot
        # exist in the sandbox, so it is a list entry without its patch file,
        # unless the same name was already applied earlier in this run
        [ -f "$pre$src" ] && continue
        SEL_FILE[n]=$src SEL_BUGID[n]=$(bugid_of "$src") SEL_ARGS[n]=-
        ;;
    esac
    SEL_N=$((n + 1))
  done < "$d/ev"
  trace_keys "$d" > "$W/base.keys"
  # NEWS-only tolerance per call site: pretend its first patch left only NEWS.rej
  local lines l pk
  lines=$(awk -F'\t' '$1 == "P" && !s[$2]++ { print $2 }' "$d/ev")
  for l in $lines; do
    run_trace "$W/probe" "$id" "$ver" "$os" "$opts" "$l" "$mode"
    trace_keys "$W/probe" > "$W/probe.keys"
    pk=$(awk -F'\t' -v l="$l" '$1 == "P" { n++ } $1 == "P" && $2 == l { print n; exit }' "$d/ev")
    if [ "$(awk -F'\t' '$1 == "R" { print $2 }' "$W/probe/ev")" = 0 ] && cmp -s "$W/base.keys" "$W/probe.keys"; then
      k=y
    elif head -n "$pk" "$W/base.keys" | cmp -s - "$W/probe.keys"; then
      k=n
    else
      echo "php_patch.inc:$l: NEWS-only probe gave an unexpected result" >&2; return 2
    fi
    for ((i = 0; i < SEL_N; i++)); do [ "${SEL_TYPE[i]}" = P ] && [ "${SEL_LINE[i]}" = "$l" ] && SEL_NEWS[i]=$k; done
  done
  # without a tree: mark entries that need tree content (absent from an empty tree)
  if [ "$mode" = fake ]; then
    run_trace "$W/empty" "$id" "$ver" "$os" "$opts" '' empty
    trace_keys "$W/empty" > "$W/empty.keys"
    for ((i = 0; i < SEL_N; i++)); do
      [ "${SEL_TYPE[i]}" = P ] || continue
      grep -qxF "${SEL_FILE[i]}|${SEL_ARGS[i]}|${SEL_CWD[i]}" "$W/empty.keys" || SEL_CONTENT[i]=y
    done
  fi
  return 0
}

# patches this branch of PHP has, which a gate skipped here: rerun with each other
# OS, each option flipped and each version boundary php_patch.inc tests in the branch
select_skipped() {  # id ver os opts mode -> SKIP_FILE/SKIP_LINE/SKIP_WHY
  local id=$1 ver=$2 os=$3 opts=$4 mode=$5 o kv flip c lo hi nid key why line allon tab
  local -a cfg=()
  tab=$(printf '\t')
  allon=$(echo "$opts" | sed 's/=n/=y/g')
  for o in $PSP_OSES; do
    [ "$o" = "$os" ] && continue
    cfg[${#cfg[@]}]="$id $ver $o $opts os=$o"
    [ "$allon" = "$opts" ] || cfg[${#cfg[@]}]="$id $ver $o $allon os=$o+all-options-y"
  done
  for kv in ${opts//,/ }; do
    case ${kv#*=} in y) flip=${kv%=*}=n ;; *) flip=${kv%=*}=y ;; esac
    cfg[${#cfg[@]}]="$id $ver $os $(echo ",$opts," | sed "s/,$kv,/,$flip,/; s/^,//; s/,\$//") $flip"
  done
  lo=$((id / 100 * 100)); hi=$((lo + 99))
  for c in $( { echo "$lo"; echo "$hi"; grep -oE '"\$PHPVER_ID" (-ge|-gt|-le|-lt|-eq|-ne|=|==|!=) '"'"'?[0-9]+' "$INC" | grep -oE '[0-9]+$' | awk '{ print $1 - 1; print $1; print $1 + 1 }'; } | sort -un); do
    [ "$c" -ge "$lo" ] && [ "$c" -le "$hi" ] && [ "$c" != "$id" ] || continue
    nid="${ver%.*}.$((c % 100))"
    cfg[${#cfg[@]}]="$c $nid $os $opts php $nid"
  done
  trace_keys "$W/trace" > "$W/sel.keys"
  : > "$W/skip.tsv"
  for c in "${cfg[@]}"; do
    set -- $c
    run_trace "$W/nb" "$1" "$2" "$3" "$4" '' "$mode"
    [ -z "$(trace_error "$W/nb")" ] || continue
    why=${c#* * * * }
    awk -F'\t' -v pre="$CUR_DIR/patches/php/" -v why="$why" '$1 == "P" { f = $4; if (index(f, pre) == 1) f = substr(f, length(pre) + 1); print f "|" $6 "|" $3 "\t" $2 "\t" why }' "$W/nb/ev" >> "$W/skip.tsv"
  done
  SKIP_N=0
  while IFS=$'\t' read -r key line why; do
    SKIP_KEY[SKIP_N]=$key SKIP_LINE[SKIP_N]=$line SKIP_WHY[SKIP_N]=$why
    SKIP_N=$((SKIP_N + 1))
  # reasons: single changes beat "os=X+all-options-y"; sampled versions become a range
  done < <(awk -F'\t' '
    NR == FNR { sel[$0]; next }
    ($1 in sel) { next }
    {
      k = $1
      if (!(k in line)) { order[++n] = k; line[k] = $2 }
      if (!index(SUBSEP all[k] SUBSEP, SUBSEP $3 SUBSEP)) all[k] = all[k] SUBSEP $3
    }
    END {
      for (i = 1; i <= n; i++) {
        k = order[i]; m = split(substr(all[k], 2), w, SUBSEP); simple = 0; np = 0; out = ""
        for (j = 1; j <= m; j++) if (w[j] !~ /\+all-options-y$/) simple = 1
        for (j = 1; j <= m; j++) {
          if (simple && w[j] ~ /\+all-options-y$/) continue
          if (w[j] ~ /^php /) { if (!np++) pf = w[j]; pl = w[j]; continue }
          out = out (out == "" ? "" : ", ") w[j]
        }
        if (np) out = out (out == "" ? "" : ", ") (np > 2 ? pf "-" substr(pl, 5) : (np == 2 ? pf ", " pl : pf))
        print k "\t" line[k] "\t" out
      }
    }
  ' "$W/sel.keys" "$W/skip.tsv" | sort -s -t"$tab" -k2,2n)
}

###########################################################
# applying

# one file per file section of a patch: $2.1 .. $2.N; prints N. Hunk line
# counts keep removed "-- x" lines from looking like a new "--- " header
split_sections() {
  awk -v pre="$2." '
  function sec() { if (out != "") close(out); out = pre (++n); hdr = 1 }
  function emit(l) { if (out != "") print l > out }
  inh {
    c = substr($0, 1, 1)
    if (c == "\\") { emit($0); next }
    if (c == " " || $0 == "") { o--; w-- } else if (c == "-") o--; else if (c == "+") w--; else inh = 0
    if (inh) { emit($0); if (o <= 0 && w <= 0) inh = 0; next }
  }
  pend != "" {
    l = pend; pend = ""
    if ($0 ~ /^\+\+\+ /) { if (!hdr) sec(); emit(l); emit($0); next }
    emit(l)
  }
  /^diff --git / { sec(); emit($0); next }
  /^--- / { pend = $0; next }
  /^@@ -[0-9]/ {
    h = $0; sub(/^@@ -/, "", h); split(h, a, " ")
    o = (split(a[1], b, ",") > 1) ? b[2] : 1
    t = a[2]; sub(/^\+/, "", t); w = (split(t, b, ",") > 1) ? b[2] : 1
    inh = (o > 0 || w > 0); hdr = 0; emit($0); next
  }
  { emit($0) }
  END { if (pend != "") emit(pend); print n + 0 }
  ' "$1"
}

# put a patch's files in tree $2 back from backup $1 (made by mirror with .orig names)
restore_from() {  # backup tree cwd cand
  local q x
  while IFS= read -r q; do
    case $q in /*|..|../*|*/../*|*/..) continue ;; esac
    for x in "$q" "$q.orig"; do
      if [ -f "$1/$3/$x" ]; then
        # patch -R removes the empty directories of files it deletes
        case $x in */*) [ -d "$2/$3/${x%/*}" ] || mkdir -p "$2/$3/${x%/*}" ;; esac
        cp -p "$1/$3/$x" "$2/$3/$x"
      else
        rm -f "$2/$3/$x"
      fi
    done
    rm -f "$2/$3/$q.rej"
  done < "$4"
}

# Which selected patches the tree already has: on a scratch copy of every file the
# selection touches, reverse them last to first, so later patches that changed an
# earlier one's context are undone first. A patch that does not reverse whole is
# retried one file section at a time, last first (multi-commit patches touch a
# file twice); a NEWS section may fail where php_patch.inc allows NEWS-only rejects.
detect_applied() {
  local i k n rc args cwd ok news strip out c=$W/chain
  for ((i = 0; i < SEL_N; i++)); do
    [ "${SEL_TYPE[i]}" = P ] || continue
    awk -v c="${SEL_CWD[i]}" '{ f = (c == ".") ? $0 : c "/" $0; print f; print f ".orig" }' "$W/cand.$i"
  done | mirror "$TT" "$c" . || return 2
  for ((i = SEL_N - 1; i >= 0; i--)); do
    [ "${SEL_TYPE[i]}" = P ] || continue
    SEL_APPLIED[i]=n
    cwd=${SEL_CWD[i]} args=${SEL_ARGS[i]}
    [ "$args" = - ] && args=
    mkdir -p "$c/$cwd"
    awk '{ print; print $0 ".orig" }' "$W/cand.$i" | mirror "$c" "$W/cb" "$cwd" || return 2
    (cd "$c/$cwd" && $PATCH $args -R -N -t < "$W/p.$i" > /dev/null 2>&1); rc=$?
    if [ "$rc" = 0 ] && ! has_rej "$c" "$cwd" "$W/cand.$i"; then SEL_APPLIED[i]=y; continue; fi
    restore_from "$W/cb" "$c" "$cwd" "$W/cand.$i"
    n=$(split_sections "$W/p.$i" "$W/sec")
    strip=$(strip_of $args)
    ok=y news=n
    for ((k = n; k >= 1; k--)); do
      out=$(cd "$c/$cwd" && $PATCH $args -R -N -t < "$W/sec.$k" 2>&1); rc=$?
      [ "$rc" = 0 ] && ! has_rej "$c" "$cwd" "$W/cand.$i" && continue
      [ "$VERBOSE" = y ] && printf 'chain: %s section %s of %s does not reverse:\n%s\n' "${SEL_FILE[i]}" "$k" "$n" "$out" | sed 's/^/    /'
      if [ "${SEL_NEWS[i]}" = y ] && [ "$(patch_paths "$W/sec.$k" "$strip" | sort -u)" = NEWS ]; then
        news=y
        echo NEWS > "$W/news.cand"
        restore_from "$W/cb" "$c" "$cwd" "$W/news.cand"
        continue
      fi
      ok=n
      break
    done
    if [ $ok = y ]; then
      SEL_APPLIED[i]=y
      [ $news = y ] && SEL_APPLIED[i]=news
    else
      restore_from "$W/cb" "$c" "$cwd" "$W/cand.$i"
    fi
  done
  return 0
}

apply_one() {  # index; target tree TT; sets ST and DT
  local i=$1 args=${SEL_ARGS[$1]} cwd=${SEL_CWD[$1]} news=${SEL_NEWS[$1]}
  local p=$W/p.$1 cand=$W/cand.$1 out rc
  [ "$args" = - ] && args=
  ST='' DT=-
  case ${SEL_APPLIED[$i]:-n} in
    y) ST=already-applied; return 0 ;;
    news) ST=already-applied DT=news-only; return 0 ;;
  esac
  mirror "$TT" "$W/m" "$cwd" < "$cand" || { ST=failed DT="scratch copy failed"; return 1; }
  out=$(cd "$W/m/$cwd" && $PATCH $args -N -t < "$p" 2>&1); rc=$?
  log_patch "$i" trial "$rc" "$out"
  if [ "$rc" = 0 ] && ! has_rej "$W/m" "$cwd" "$cand"; then
    real_apply "$i" clean && { ST=applied; return 0; }
    return 1
  fi
  if [ "$news" = y ] && news_only "$p" "$rc" "$out" "$W/m/$cwd"; then
    real_apply "$i" news && { ST=news-only; return 0; }
    return 1
  fi
  ST=failed
  DT=$(printf '%s\n' "$out" | grep -iE 'FAILED|can.t find|Skipping|Reversed|ignored|malformed|garbage|No such' | head -3 | tr '\n\t' '  ')
  [ -n "$DT" ] || DT="patch exited $rc"
  [ "$news" = n ] && news_only "$p" "$rc" "$out" "$W/m/$cwd" && DT="NEWS-only reject at a call site without the NEWS-only rule (php_patch.inc:${SEL_LINE[$i]})"
  # GNU patch < 2.7 (CentOS 6 has 2.6) ignores git renames, so later hunks for a renamed file fail
  if grep -q '^rename from ' "$p" && echo "${PATCH_DESC##* }" | awk -F. '{ exit !($1 < 2 || ($1 == 2 && $2 < 7)) }'; then
    DT="$PATCH_DESC cannot apply git renames in this patch; use GNU patch 2.7 or newer (PATCH=/path/to/patch). $DT"
  fi
  [ "$VERBOSE" = y ] || printf '%s\n' "$out" | sed 's/^/    /'
  return 1
}

real_apply() {  # index clean|news
  local i=$1 args=${SEL_ARGS[$1]} cwd=${SEL_CWD[$1]} p=$W/p.$1 cand=$W/cand.$1 out rc
  [ "$args" = - ] && args=
  awk '{ print; print $0 ".orig" }' "$cand" | mirror "$TT" "$W/b" "$cwd" || { ST=failed DT="backup failed"; return 1; }
  out=$(cd "$TT/$cwd" && $PATCH $args -N -t < "$p" 2>&1); rc=$?
  log_patch "$i" apply "$rc" "$out"
  [ "$VERBOSE" = y ] && printf '%s\n' "$out" | sed 's/^/    /'
  if [ "$2" = clean ] && [ "$rc" = 0 ] && ! has_rej "$TT" "$cwd" "$cand"; then return 0; fi
  if [ "$2" = news ] && news_only "$p" "$rc" "$out" "$TT/$cwd"; then
    [ -f "$TT/$cwd/NEWS.orig" ] && mv -f "$TT/$cwd/NEWS.orig" "$TT/$cwd/NEWS"
    rm -f "$TT/$cwd/NEWS.rej"
    return 0
  fi
  # differs from its trial run: put back every file the patch could touch
  restore_from "$W/b" "$TT" "$cwd" "$cand"
  ST=failed DT="result differs from the trial run; files restored"
  printf '%s\n' "$out" | sed 's/^/    /'
  return 1
}

log_patch() {  # index step rc output
  [ -n "$LOG" ] || return 0
  printf '### %s %s rc=%s %s\n%s\n' "${SEL_FILE[$1]}" "$2" "$3" "$(date -u +%Y-%m-%dT%H:%M:%SZ)" "$4" >> "$LOG"
}

###########################################################
# output
print_selection() {
  local i n=0 tag
  printf 'PHP %s (%s)  os=%s  %s\n' "$VER" "$ID" "$OS" "${OPTSTR}"
  printf 'data: %s\n' "$DATA_DESC"
  printf '%4s %5s %4s  %-44s %s\n' seq line news file options
  for ((i = 0; i < SEL_N; i++)); do
    case ${SEL_TYPE[i]} in
      P) n=$((n + 1)); tag=; [ "${SEL_CONTENT[i]}" = y ] && tag='  (only when the tree content needs it)'
         printf '%4s %5s %4s  %-44s %s%s\n' "$n" "${SEL_LINE[i]}" "${SEL_NEWS[i]}" "${SEL_FILE[i]}" "${SEL_ARGS[i]}" "$tag"
         [ "${SEL_CWD[i]}" = . ] || printf '%17s(runs in %s)\n' '' "${SEL_CWD[i]}" ;;
      M) printf '%4s %5s %4s  %-44s %s\n' - "${SEL_LINE[i]}" - "${SEL_FILE[i]}" 'listed, no patch file: skipped' ;;
      D) printf '%4s %5s %4s  %-44s %s\n' - "${SEL_LINE[i]}" - "${SEL_FILE[i]}" 'called again: skipped' ;;
      T) printf '%4s %5s %4s  %-44s %s\n' - "${SEL_LINE[i]}" - "touch ${SEL_ARGS[i]}" 'after patching' ;;
    esac
  done
  if [ "${SKIP_N:-0}" -gt 0 ]; then
    echo 'skipped by a gate here (selected with):'
    for ((i = 0; i < SKIP_N; i++)); do printf '%4s %5s %4s  %-44s %s\n' - "${SKIP_LINE[i]}" - "${SKIP_KEY[i]%%|*}" "${SKIP_WHY[i]}"; done
  fi
  echo "$n patches selected"
}

write_record() {  # result
  local i f sha
  [ -n "$RECORD" ] || return 0
  mkdir -p "$(dirname "$RECORD")" || return 0
  {
    printf '# php-source-patch record 1\n'
    printf '# tool\tphp-source-patch.sh %s\n# mode\t%s\n# started\t%s\n# finished\t%s\n# result\t%s\n' \
      "$PSP_VERSION" "$RUNMODE" "$STARTED" "$(date -u +%Y-%m-%dT%H:%M:%SZ)" "$1"
    if [ -n "$TB_OUT" ]; then
      # this record ships inside the tarball: names only, no local paths
      printf '# src\tphp-%s\n# input\t%s\n# tarball\t%s\n' "$TBVER" "${INPUT##*/}" "${TB_OUT##*/}"
    else
      printf '# src\t%s\n' "$SRC"
    fi
    printf '# php_version\t%s\n# php_version_id\t%s\n# os\t%s\n# options\t%s\n' "$VER" "$ID" "$OS" "$OPTSTR"
    printf '# data\t%s\n# inc_sha256\t%s\n# patch\t%s\n' "$DATA_DESC" "$INC_SHA" "$PATCH_DESC"
    [ -n "$TOUCHED" ] && printf '# touched\t%s\n' "$TOUCHED"
    printf 'seq\tstatus\tbugid\tfile\tsha256\tline\tdetail\n'
    for ((i = 0; i < SEL_N; i++)); do
      [ "${SEL_TYPE[i]}" = T ] && continue
      sha=${SEL_SHA[i]:--}
      printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$((i + 1))" "${SEL_STATUS[i]:-not-run}" "${SEL_BUGID[i]}" "${SEL_FILE[i]}" "$sha" "${SEL_LINE[i]}" "${SEL_DETAIL[i]:--}"
    done
    for ((i = 0; i < SKIP_N; i++)); do
      f=${SKIP_KEY[i]%%|*}
      printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' - skipped-gate "$(bugid_of "$f")" "$f" "$(sha_of_patch "$f")" "${SKIP_LINE[i]}" "selected with ${SKIP_WHY[i]}"
    done
  } > "$RECORD.tmp" && mv -f "$RECORD.tmp" "$RECORD"
}

###########################################################
# manifest: the selection for each version x OS with the default options
manifest_rows() {  # versions
  local v id o i n sha
  for v in $1; do
    id=$(echo "$v" | awk -F. '{ printf "%d", $1 * 10000 + $2 * 100 + $3 }')
    for o in $PSP_OSES; do
      select_patches "$id" "$v" "$o" "$(opt_csv)" fake || return 2
      n=0
      for ((i = 0; i < SEL_N; i++)); do
        n=$((n + 1))
        sha=-
        [ "${SEL_TYPE[i]}" = P ] && sha=$(sha_of_patch "${SEL_FILE[i]}")
        printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$v" "$o" "$n" "${SEL_TYPE[i]}" "${SEL_FILE[i]}" "$sha" \
          "${SEL_BUGID[i]}" "${SEL_CWD[i]}" "${SEL_ARGS[i]}" "${SEL_NEWS[i]}" "${SEL_CONTENT[i]}" "${SEL_LINE[i]}"
      done
    done
  done
}

manifest() {  # versions
  printf '# php-source-patch manifest 1\n# versions\t%s\n# oses\t%s\n# options\t%s\n# inc_sha256\t%s\n' "$1" "$PSP_OSES" "$(opt_csv)" "$INC_SHA"
  printf 'version\tos\tseq\ttype\tfile\tsha256\tbugid\tcwd\targs\tnews\tcontent\tline\n'
  manifest_rows "$1"
}

check_manifest() {  # file
  local f=$1 vers opts
  [ -f "$f" ] || die "no manifest $f"
  vers=$(awk -F'\t' '$1 == "# versions" { print $2 }' "$f")
  opts=$(awk -F'\t' '$1 == "# options" { print $2 }' "$f")
  [ -n "$vers" ] && [ -n "$opts" ] || die "$f is not a php-source-patch manifest"
  OPTSTR=${opts//,/ }
  manifest "$vers" > "$W/manifest.now" || die "could not regenerate the manifest"
  # line numbers move with any php_patch.inc edit; compare everything else
  grep -v '^#' "$f" > "$W/m.old"
  grep -v '^#' "$W/manifest.now" > "$W/m.new"
  cut -f1-11 "$W/m.old" > "$W/m.old.cut"
  cut -f1-11 "$W/m.new" > "$W/m.new.cut"
  if diff "$W/m.old.cut" "$W/m.new.cut" > "$W/manifest.diff"; then
    echo "manifest OK: $f matches $INC and $PDIR"
    cmp -s "$W/m.old" "$W/m.new" || echo "  (php_patch.inc line numbers changed; regenerate to refresh them)"
    return 0
  fi
  echo "manifest DRIFT: $f does not match $INC and $PDIR (< manifest, > php_patch.inc now):"
  cat "$W/manifest.diff"
  return 3
}

make_bundle() {  # dir
  local out=$1 o rev=unknown branch=unknown
  [ "$DATAMODE" = centmin ] || die "--make-bundle needs a Centmin Mod checkout (centmin.sh beside inc/)"
  [ -e "$out" ] && [ -n "$(ls -A "$out" 2>/dev/null)" ] && die "$out exists and is not empty"
  mkdir -p "$out/tools" "$out/inc" "$out/patches" || die "cannot create $out"
  cp -p "$SELF" "$out/tools/php-source-patch.sh" && cp -p "$INC" "$out/inc/php_patch.inc" && cp -Rp "$PDIR" "$out/patches/php" || die "copy failed"
  find "$out" -name .DS_Store -exec rm -f {} + 2>/dev/null
  if [ -n "$(git_rev)" ]; then
    rev=$(cd "$CUR_DIR" && git rev-parse HEAD)
    branch=$(cd "$CUR_DIR" && git rev-parse --abbrev-ref HEAD)
    [ -n "$(cd "$CUR_DIR" && git status --porcelain -- inc/php_patch.inc patches/php tools/php-source-patch.sh)" ] && rev="$rev-dirty"
  fi
  {
    printf 'php-source-patch bundle 1\ntool %s\nsource centminmod %s %s\ncreated %s\n' "$PSP_VERSION" "$branch" "$rev" "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
    for o in $PSP_OPTS; do printf 'default %s=%s\n' "$o" "$(opt_get "$o")"; done
  } > "$out/BUNDLE"
  manifest "$PSP_EOL_VERSIONS" > "$out/manifest.tsv" || die "manifest failed"
  (cd "$out" && find . -type f ! -name SHA256SUMS | sed 's|^\./||' | sort > "$W/bundle.list" &&
    sha256_of $(cat "$W/bundle.list") > SHA256SUMS) || die "SHA256SUMS failed"
  echo "bundle written: $out ($(wc -l < "$out/SHA256SUMS" | tr -d ' ') files, source $branch $rev)"
  "$BASH" "$out/tools/php-source-patch.sh" --check-manifest "$out/manifest.tsv" || die "bundle self-check failed"
}

# a bundle is used only when every file matches SHA256SUMS and nothing is unlisted
verify_bundle() {
  local bad
  [ -f "$CUR_DIR/SHA256SUMS" ] || die "bundle $CUR_DIR has no SHA256SUMS"
  bad=$(cd "$CUR_DIR" && awk '{ print $2 }' SHA256SUMS | while IFS= read -r f; do [ -f "$f" ] || echo "$f"; done)
  [ -z "$bad" ] || die "bundle $CUR_DIR is missing: $bad"
  bad=$(cd "$CUR_DIR" && sha256_of $(awk '{ print $2 }' SHA256SUMS) | awk 'NR == FNR { want[$2] = $1; next } want[$2] != $1 { print $2 }' SHA256SUMS -)
  [ -z "$bad" ] || die "bundle $CUR_DIR checksum mismatch: $bad"
  bad=$(cd "$CUR_DIR" && find tools inc patches -type f | sed 's|^\./||' | awk 'NR == FNR { ok[$2]; next } !($0 in ok)' SHA256SUMS -)
  [ -z "$bad" ] || die "bundle $CUR_DIR has files not in SHA256SUMS: $bad"
}

###########################################################
# patched source tarball, packed like a php.net release
tarball_target() {  # sets TB_OUT and TB_FMT from --tarball, before any patching
  local out=$TARBALL rev='' d
  if [ -d "$out" ]; then
    case $DATAMODE in
      centmin)
        rev=$(git_rev)
        [ -n "$rev" ] && [ -n "$(cd "$CUR_DIR" && git status --porcelain -- inc/php_patch.inc patches/php tools/php-source-patch.sh 2>/dev/null)" ] && rev="$rev-dirty" ;;
      bundle) rev=$(awk '$1 == "source" { print substr($4, 1, 8) ($4 ~ /-dirty$/ ? "-dirty" : "") }' "$CUR_DIR/BUNDLE") ;;
    esac
    out=$out/php-$TBVER-centmin-${rev:-local}.tar.xz
  fi
  case $out in
    *.tar.gz|*.tgz) TB_FMT=gzip ;;
    *.tar.xz) TB_FMT=xz ;;
    *) die "--tarball must end in .tar.gz, .tgz or .tar.xz, or be a directory" ;;
  esac
  command -v "$TB_FMT" >/dev/null 2>&1 || die "--tarball $out needs $TB_FMT"
  d=$(cd "$(dirname "$out")" 2>/dev/null && pwd -P) || die "no directory for $out"
  TB_OUT=$d/${out##*/}
  # the outputs must not replace the input, land inside a source tree that is
  # copied into them, or collide with the --record copy
  [ ! -e "$TB_OUT" ] || [ ! "$TB_OUT" -ef "$INPUT" ] || die "--tarball $TB_OUT is the input"
  case $TB_OUT in "$SRC"/*) die "--tarball must be outside the source tree $SRC" ;; esac
  case $RECORD in "$TB_OUT"|"$TB_OUT.sha256") die "--record must not be the tarball or its .sha256" ;; esac
}

gnu_tar() { $TAR --version 2>/dev/null | head -1 | grep -q 'GNU tar'; }

# commit of the checkout this script runs from; cd, not git -C (git 1.8.5+, newer
# than CentOS 6 and 7 ship)
git_rev() {
  command -v git >/dev/null 2>&1 || return 0
  (cd "$CUR_DIR" && git rev-parse --short=8 HEAD 2>/dev/null) || true
}

make_tarball() {
  local top=php-$TBVER name=${TB_OUT##*/} sha mode
  # mktemp temp files beside OUT (no symlink games in a shared directory); cleanup()
  # removes them on any exit, and OUT plus its .sha256 replace the old ones last
  TB_TMP=$(mktemp "$TB_OUT.tmp.XXXXXX") && TB_SHA_TMP=$(mktemp "$TB_OUT.sha256.tmp.XXXXXX") ||
    die "cannot create a temporary file beside $TB_OUT"
  # pipefail: a tar error must not hide behind a successful compressor. Modes as a
  # release ships them: readable, not group/world writable, no setuid/setgid/sticky
  (set -o pipefail; cd "$W/out" &&
    $TAR --owner=0 --group=0 --numeric-owner --mode='u+rw,go+rX,go-w,a-st' \
      --exclude="$top/.php-source-patch/patch.log" -cf - "$top" |
    if [ "$TB_FMT" = xz ]; then xz -c; else gzip -9 -n -c; fi > "$TB_TMP") || die "could not write $TB_OUT"
  sha=$(sha256_of "$TB_TMP" | awk '{ print $1 }')
  { [ -n "$sha" ] && printf '%s  %s\n' "$sha" "$name" > "$TB_SHA_TMP"; } || die "could not write $TB_OUT.sha256"
  # mktemp makes 0600 files; give them the modes a plain redirect would have
  mode=$(printf '%o' $(( 0666 & ~0$(umask) )))
  chmod "$mode" "$TB_TMP" "$TB_SHA_TMP" &&
    mv -f "$TB_TMP" "$TB_OUT" && mv -f "$TB_SHA_TMP" "$TB_OUT.sha256" || die "could not write $TB_OUT"
  TB_TMP='' TB_SHA_TMP=''
  echo "tarball: $TB_OUT"
  echo "sha256: $sha  ($name.sha256)"
}

###########################################################
# self-test: a small php_patch.inc, patches and source tree
self_test() {
  local t=$W/selftest pass=0 fail=0 out rc r got
  mkdir -p "$t/c/inc" "$t/c/patches/php" || die "self-test setup failed"
  ok() {  # name command...
    local n=$1; shift
    if "$@"; then pass=$((pass + 1)); echo "  ok   $n"; else fail=$((fail + 1)); echo "  FAIL $n"; printf '%s\n' "$out" | sed 's/^/       | /' | tail -15; fi
  }
  has() { printf '%s\n' "$out" | grep -q "$1"; }
  st_tree() {  # dir version
    rm -rf "$1"; mkdir -p "$1/main" "$1/src" "$1/Zend"
    printf '#define PHP_VERSION "%s"\n#define PHP_VERSION_ID %s\n' "$2" "$(echo "$2" | awk -F. '{ printf "%d", $1 * 10000 + $2 * 100 + $3 }')" > "$1/main/php_version.h"
    printf 'PHP NEWS\n?? ??? 2099, PHP 9.9.2\n\n- Core:\n  . old entry\n\n- Date:\n  . old date\n' > "$1/NEWS"
    printf 'int a1 = 0;\nint a2 = 0;\nint a3 = 0;\nint a4 = 0;\nint a5 = 0;\nint a6 = 0;\nint a7 = 0;\nint a8 = 0;\n' > "$1/src/a.c"
    printf 'int b1 = 0;\nint b2 = 0;\nint b3 = 0;\n' > "$1/src/b.c"
    printf '/* caf\351 */\nint c1 = 0;\nint c2 = 0;\n' > "$1/src/c.c"
    printf 'int d1 = 0;\nint d2 = 0;\nint d3 = 0;\n' > "$1/src/d.c"
    printf 'int e1 = 0;\nint e2 = 0;\nint e3 = 0;\n' > "$1/src/e.c"
    printf 'int x1 = 0;\nint x2 = 0;\nint x3 = 0;\n' > "$1/--dash.c"
    : > "$1/Zend/zend_vm_gen.php"; : > "$1/Zend/gen.h"
  }
  statuses() { awk -F'\t' '$1 != "seq" && !/^#/ { printf "%s:%s ", $3, $2 }' "$1"; }
  # the real NEWS-only helper when this runs from a checkout, else a copy
  if [ -f "$INC" ] && grep -q '^php_patch_news_only_reject()' "$INC"; then
    awk '/^php_patch_news_only_reject\(\)/ { p = 1 } p { print } p && /^}/ { exit }' "$INC" > "$t/c/inc/php_patch.inc"
  else
    cat > "$t/c/inc/php_patch.inc" <<'EOF'
php_patch_news_only_reject() {
  local patch_file="$1" patch_rc="$2" patch_out="$3" file news_reject=n
  [[ "$patch_rc" -eq 1 ]] || return 1
  if grep -qiE "can't find file to patch|Skipping patch|hunks? ignored|Reversed" <<< "$patch_out"; then
    return 1
  fi
  while read -r file; do
    file=$(echo "$file" | sed 's/b\///')
    if [[ -f "$file.rej" ]]; then
      [[ "$file" = 'NEWS' ]] || return 1
      news_reject=y
    fi
  done < <(grep -oP '(?<=\+\+\+ )[^\s]+' "$patch_file")
  [[ "$news_reject" = 'y' ]]
}
EOF
  fi
  cat >> "$t/c/inc/php_patch.inc" <<'EOF'
php_patches() {
  {
  if [[ "$PHP_PATCH" = [yY] ]]; then
    if [[ "$PHPVER_ID" -ge '90900' && "$PHPVER_ID" -le '90999' ]]; then
      phpnine_bugids='a1 news1 crlf1 iso1 gone1 strict1 dash1'
      for bugid in $phpnine_bugids; do
        if [[ ! -f "php99-${bugid}.patch" && -f "$CUR_DIR/patches/php/php99-${bugid}.patch" ]]; then
          cp -a $CUR_DIR/patches/php/php99-${bugid}.patch php99-${bugid}.patch || exit 1
          dos2unix php99-${bugid}.patch || exit 1
          patch_out=$(patch -p1 < php99-${bugid}.patch 2>&1)
          patch_rc=$?
          printf '%s\n' "$patch_out"
          if [[ "$patch_rc" -eq 0 ]]; then
            echo "Patch applied successfully for bug #${bugid}"
          elif php_patch_news_only_reject "php99-${bugid}.patch" "$patch_rc" "$patch_out"; then
            [[ -f NEWS.orig ]] && mv -f NEWS.orig NEWS
            rm -f NEWS.rej
          else
            exit 1
          fi
        else
          echo "php99-${bugid}.patch patch file already applied"
        fi
      done
    fi
    if [[ "$PHPVER_ID" -ge '90800' && "$PHPVER_ID" -le '90899' ]]; then
      if [ ! -f php98-news1.patch ]; then
        cp -a $CUR_DIR/patches/php/php98-news1.patch php98-news1.patch || exit 1
        patch -p1 < php98-news1.patch || exit 1
      fi
    fi
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' && "$FPM_WAKEUP_PATCH" = [yY] ]]; then
    cp -a "$CUR_DIR/patches/php/el8-e1.patch" el8-e1.patch || exit 1
    patch -p1 < el8-e1.patch || exit 1
  fi
  if [[ "$PHPVER_ID" -ge '90900' && -f Zend/zend_vm_gen.php ]]; then
    touch Zend/gen.h
  fi
  } 2>&1 | tee -a "${CENTMINLOGDIR}/patch_php_${DT}.log"
  local s=("${PIPESTATUS[@]}")
  [[ "${s[0]}" -eq 0 ]] || return "${s[0]}"
  return "${s[1]}"
}
EOF
  local P=$t/c/patches/php
  printf -- '--- a/src/a.c\n+++ b/src/a.c\n@@ -1,3 +1,3 @@\n-int a1 = 0;\n+int a1 = 1;\n int a2 = 0;\n int a3 = 0;\n--- a/NEWS\n+++ b/NEWS\n@@ -2,6 +2,7 @@\n ?? ??? 2099, PHP 9.9.2\n \n - Core:\n+  . a1 fixed\n   . old entry\n \n - Date:\n' > "$P/php99-a1.patch"
  printf -- '--- a/src/b.c\n+++ b/src/b.c\n@@ -1,3 +1,3 @@\n int b1 = 0;\n-int b2 = 0;\n+int b2 = 1;\n int b3 = 0;\n--- a/NEWS\n+++ b/NEWS\n@@ -2,6 +2,7 @@\n ?? ??? 2088, PHP 8.8.8\n \n - Zip:\n+  . news1 fixed\n   . zip entry\n \n - Nope:\n' > "$P/php99-news1.patch"
  cp "$P/php99-news1.patch" "$P/php98-news1.patch"
  printf -- '--- a/src/a.c\r\n+++ b/src/a.c\r\n@@ -6,3 +6,3 @@\r\n int a6 = 0;\r\n-int a7 = 0;\r\n+int a7 = 1;\r\n int a8 = 0;\r\n' > "$P/php99-crlf1.patch"
  printf -- '--- a/src/c.c\n+++ b/src/c.c\n@@ -1,3 +1,3 @@\n /* caf\351 */\n-int c1 = 0;\n+int c1 = 1;\n int c2 = 0;\n' > "$P/php99-iso1.patch"
  printf -- '--- a/src/d.c\n+++ b/src/d.c\n@@ -1,3 +1,3 @@\n int d1 = 0;\n-int d2 = 0;\n+int d2 = 1;\n int d3 = 0;\n' > "$P/php99-strict1.patch"
  # a top-level name starting with "-": GNU tar 1.29+ reads such -T list lines as options
  printf -- '--- a/--dash.c\n+++ b/--dash.c\n@@ -1,3 +1,3 @@\n int x1 = 0;\n-int x2 = 0;\n+int x2 = 1;\n int x3 = 0;\n' > "$P/php99-dash1.patch"
  printf -- '--- a/src/e.c\n+++ b/src/e.c\n@@ -1,3 +1,3 @@\n int e1 = 0;\n-int e2 = 0;\n+int e2 = 8;\n int e3 = 0;\n' > "$P/el8-e1.patch"
  local me=("$BASH" "$SELF" --patch-dir "$P" --os other)

  echo "self-test (GNU patch: $PATCH_DESC)"
  printf 'x +++ b/NEWS\tz\n+++ +++ y\n+++ a/b/c\n' > "$t/pp"
  got=$(plus_names "$t/pp" 1 | tr '\n' ' ')
  ok "grep -oP emulation ($got)" [ "$got" = "NEWS +++ y a/c " ]
  got=$(plus_names "$P/php99-iso1.patch" 1 | tr '\n' ' ')
  ok 'ISO-8859 patch lines are read' [ "$got" = "src/c.c " ]
  st_tree "$t/s" 9.9.1
  out=$("${me[@]}" --src "$t/s" --list 2>&1); rc=$?
  ok 'list: exit 0' [ "$rc" = 0 ]
  got=$(printf '%s\n' "$out" | awk '$1 ~ /^[0-9]+$/ && $4 ~ /\.patch$/ { printf "%s:%s ", $4, $3 }')
  ok "list: order and NEWS rule ($got)" [ "$got" = "php99-a1.patch:y php99-news1.patch:y php99-crlf1.patch:y php99-iso1.patch:y php99-strict1.patch:y php99-dash1.patch:y " ]
  ok 'list: missing patch file shown' has 'php99-gone1.patch .*no patch file'
  ok 'list: touch shown' has 'touch Zend/gen.h'
  ok 'list: OS and option gate shown' has 'el8-e1.patch .*os=el8+all-options-y'
  out=$("${me[@]}" --src "$t/s" --dry-run 2>&1); rc=$?
  ok 'dry-run: exit 0' [ "$rc" = 0 ]
  ok 'dry-run: reports NEWS-only' has '\[news-only\] *php99-news1.patch'
  ok 'dry-run: tree unchanged' grep -q 'int a1 = 0' "$t/s/src/a.c"
  ok 'dry-run: no record' [ ! -d "$t/s/.php-source-patch" ]
  out=$("${me[@]}" --src "$t/s" 2>&1); rc=$?
  ok 'apply: exit 0' [ "$rc" = 0 ]
  got=$(cat "$t/s/src/a.c" "$t/s/src/b.c" "$t/s/src/c.c" "$t/s/src/d.c" | grep -c ' = 1;')
  ok "apply: all code hunks ($got of 5)" [ "$got" = 5 ]
  ok 'apply: file named --dash.c patched' grep -q 'int x2 = 1' "$t/s/--dash.c"
  ok 'apply: NEWS has a1' grep -q 'a1 fixed' "$t/s/NEWS"
  ok 'apply: NEWS lacks the rejected news1 entry' [ "$(grep -c 'news1 fixed' "$t/s/NEWS")" = 0 ]
  ok 'apply: no NEWS.rej or NEWS.orig' [ ! -e "$t/s/NEWS.rej" -a ! -e "$t/s/NEWS.orig" ]
  ok 'apply: no patch files copied into the tree' [ -z "$(cd "$t/s" && ls -- *.patch 2>/dev/null)" ]
  r=$t/s/.php-source-patch/record.tsv
  got=$(statuses "$r")
  ok "record: statuses ($got)" [ "$got" = "a1:applied news1:news-only crlf1:applied iso1:applied gone1:skipped-missing-file strict1:applied dash1:applied el8-e1:skipped-gate " ]
  got=$(awk -F'\t' '$3 == "a1" { print $5 }' "$r")
  ok 'record: sha256' [ "$got" = "$(sha256_of "$P/php99-a1.patch" | awk '{ print $1 }')" ]
  ok 'record: touched' grep -q '^# touched	Zend/gen.h' "$r"
  sha256_of "$t/s/src/a.c" "$t/s/src/b.c" "$t/s/NEWS" > "$t/before"
  out=$("${me[@]}" --src "$t/s" 2>&1); rc=$?
  got=$(awk -F'\t' '$1 ~ /^[0-9]/ && $2 != "skipped-missing-file" { print $2 }' "$r" | sort -u)
  ok 'rerun: exit 0' [ "$rc" = 0 ]
  ok "rerun: all already applied ($got)" [ "$got" = already-applied ]
  sha256_of "$t/s/src/a.c" "$t/s/src/b.c" "$t/s/NEWS" > "$t/after"
  ok 'rerun: tree unchanged' cmp -s "$t/before" "$t/after"
  st_tree "$t/f" 9.9.1
  printf 'int d1 = 0;\nint dX = 0;\nint d3 = 0;\n' > "$t/f/src/d.c"
  cp -p "$t/f/src/d.c" "$t/d.before"
  out=$("${me[@]}" --src "$t/f" 2>&1); rc=$?
  r=$t/f/.php-source-patch/record.tsv
  ok 'reject: exit 1' [ "$rc" = 1 ]
  ok 'reject: rejected file unchanged' cmp -s "$t/d.before" "$t/f/src/d.c"
  ok 'reject: no .rej or .orig left' [ -z "$(cd "$t/f" && find . -name '*.rej' -o -name '*.orig')" ]
  got=$(statuses "$r")
  ok "reject: record ($got)" [ "$got" = "a1:applied news1:news-only crlf1:applied iso1:applied gone1:skipped-missing-file strict1:failed dash1:not-run el8-e1:skipped-gate " ]
  ok 'reject: result failed' grep -q '^# result	failed' "$r"
  st_tree "$t/n" 9.8.1
  out=$("${me[@]}" --src "$t/n" 2>&1); rc=$?
  ok 'NEWS-only reject at a site without the NEWS-only rule: exit 1' [ "$rc" = 1 ]
  ok '... and says why' has 'without the NEWS-only rule'
  ok '... and b.c unchanged' grep -q 'int b2 = 0' "$t/n/src/b.c"
  st_tree "$t/o" 9.9.1
  out=$("${me[@]}" --src "$t/o" --os el8 --set FPM_WAKEUP_PATCH=y 2>&1); rc=$?
  ok 'OS and option gate: el8-e1 applied' grep -q 'int e2 = 8' "$t/o/src/e.c"
  out=$("${me[@]}" --src "$t/o" --version 9.9.2 --list 2>&1); rc=$?
  ok 'version mismatch: exit 2' [ "$rc" = 2 ]
  awk '/^  if \[\[ "\$PHP_PATCH" = \[yY\] \]\]; then/ { print "  wget http://example.invalid/x" } { print }' "$t/c/inc/php_patch.inc" > "$t/bad.inc"
  out=$("${me[@]}" --src "$t/o" --inc "$t/bad.inc" --list 2>&1); rc=$?
  ok 'unknown command in php_patches(): exit 2' [ "$rc" = 2 ]
  ok '... and names it' has 'unsupported in the sandbox: wget'
  sed 's/"\$CENTOS_EIGHT" -eq/"$NEW_PHP_OPTION" = y \&\& "$CENTOS_EIGHT" -eq/' "$t/c/inc/php_patch.inc" > "$t/bad2.inc"
  out=$("${me[@]}" --src "$t/o" --inc "$t/bad2.inc" --list 2>&1); rc=$?
  ok 'unset variable in php_patches(): exit 2' [ "$rc" = 2 ]
  ok '... and names it' has NEW_PHP_OPTION
  sed 's/patch -p1 < el8-e1.patch/patch -p1 -o out.c < el8-e1.patch/' "$t/c/inc/php_patch.inc" > "$t/bad3.inc"
  out=$("${me[@]}" --src "$t/o" --inc "$t/bad3.inc" --os el8 --set FPM_WAKEUP_PATCH=y --list 2>&1); rc=$?
  ok 'unknown patch option (-o FILE) in php_patches(): exit 2' [ "$rc" = 2 ]
  ok '... and names it' has 'patch options this script does not support: -p1 -o out.c'
  # --tarball: patched copy packed like php.net, the input left alone
  if ! gnu_tar; then
    echo "  skip tarball tests: --tarball needs GNU tar (set TAR=gtar)"
    echo "self-test: $pass passed, $fail failed"
    [ "$fail" = 0 ]
    return
  fi
  st_tree "$t/tb" 9.9.1
  : > "$t/tb/php99-a1.patch"
  sha256_of "$t/tb/src/a.c" "$t/tb/NEWS" > "$t/tb.before"
  mkdir -p "$t/dist" "$t/x"
  out=$("${me[@]}" --src "$t/tb" --tarball "$t/dist/php-9.9.1-test.tar.gz" 2>&1); rc=$?
  ok 'tarball: exit 0' [ "$rc" = 0 ]
  sha256_of "$t/tb/src/a.c" "$t/tb/NEWS" > "$t/tb.after"
  ok 'tarball: input tree unchanged' cmp -s "$t/tb.before" "$t/tb.after"
  ok 'tarball: no record in the input tree' [ ! -e "$t/tb/.php-source-patch" ]
  got=$(cd "$t/dist" && sha256_of php-9.9.1-test.tar.gz)
  ok 'tarball: .sha256 matches' [ "$got" = "$(cat "$t/dist/php-9.9.1-test.tar.gz.sha256")" ]
  got=$($TAR tzf "$t/dist/php-9.9.1-test.tar.gz" | awk '{ sub(/^\.\//, "") } !/^php-9\.9\.1(\/|$)/ { bad++ } END { print bad + 0 }')
  ok 'tarball: one top directory php-9.9.1' [ "$got" = 0 ]
  (cd "$t/x" && $TAR xzf "$t/dist/php-9.9.1-test.tar.gz")
  ok 'tarball: contents patched' grep -q 'int x2 = 1' "$t/x/php-9.9.1/--dash.c"
  ok 'tarball: record inside' grep -q '^# tarball	php-9.9.1-test.tar.gz' "$t/x/php-9.9.1/.php-source-patch/record.tsv"
  ok 'tarball: no patch.log, copied patch files or .orig inside' [ -z "$(cd "$t/x/php-9.9.1" && ls .php-source-patch/patch.log *.patch 2>/dev/null; find . -name '*.orig')" ]
  rm -rf "$t/tin"; mkdir -p "$t/tin/php-9.9.1"; st_tree "$t/tin/php-9.9.1" 9.9.1
  (cd "$t/tin" && $TAR czf "$t/php-9.9.1.tar.gz" php-9.9.1)
  out=$("${me[@]}" --src "$t/php-9.9.1.tar.gz" --tarball "$t/dist/from-tarball.tar.gz" 2>&1); rc=$?
  rm -rf "$t/x"; mkdir -p "$t/x"; (cd "$t/x" && $TAR xzf "$t/dist/from-tarball.tar.gz")
  ok 'tarball from a tarball: patched' grep -q 'int a1 = 1' "$t/x/php-9.9.1/src/a.c"
  if command -v xz >/dev/null 2>&1; then
    out=$("${me[@]}" --src "$t/php-9.9.1.tar.gz" --tarball "$t/dist" 2>&1); rc=$?
    ok 'tarball into a directory: php-9.9.1-centmin-<rev>.tar.xz' [ "$rc" = 0 -a -f "$t/dist/php-9.9.1-centmin-local.tar.xz" ]
  fi
  out=$("${me[@]}" --src "$t/php-9.9.1.tar.gz" 2>&1); rc=$?
  ok 'tarball --src without --tarball: exit 2' [ "$rc" = 2 ]
  st_tree "$t/tf" 9.9.1
  printf 'int d1 = 0;\nint dX = 0;\nint d3 = 0;\n' > "$t/tf/src/d.c"
  out=$("${me[@]}" --src "$t/tf" --tarball "$t/dist/fail.tar.gz" 2>&1); rc=$?
  ok 'tarball with a failed patch: exit 1, no tarball' [ "$rc" = 1 -a ! -e "$t/dist/fail.tar.gz" ]
  ok '... record kept beside it' grep -q '^# result	failed' "$t/dist/fail.tar.gz.record.tsv"
  : > "$t/tf/config.status"
  out=$("${me[@]}" --src "$t/tf" --tarball "$t/dist/conf.tar.gz" 2>&1); rc=$?
  ok 'tarball of a configured tree: exit 2' [ "$rc" = 2 ]
  # input tricks: a symlinked record dir, .git, world-writable files
  st_tree "$t/tv" 9.9.1
  mkdir -p "$t/victim" "$t/tv/.git"; ln -s "$t/victim" "$t/tv/.php-source-patch"
  : > "$t/tv/.git/config"; chmod 0666 "$t/tv/src/e.c"
  out=$("${me[@]}" --src "$t/tv" --tarball "$t/dist/tv.tar.gz" --record "$t/new/dir/tv.tsv" 2>&1); rc=$?
  ok 'tarball: exit 0 with a symlinked .php-source-patch' [ "$rc" = 0 ]
  ok '... which gets no writes' [ -z "$(ls -A "$t/victim")" ]
  ok 'tarball: --record in a new directory' grep -q '^# result	ok' "$t/new/dir/tv.tsv"
  got=$($TAR tvzf "$t/dist/tv.tar.gz")
  ok 'tarball: no .git inside' [ -z "$(printf '%s\n' "$got" | grep '/\.git')" ]
  ok 'tarball: nothing group or world writable' [ -z "$(printf '%s\n' "$got" | awk '$1 ~ /^.....w/ || $1 ~ /^........w/')" ]
  out=$(cd "$t/dist" && "${me[@]}" --src "$t/tb" --tarball ./rel.tar.gz --record ./rel.tar.gz.record.tsv 2>&1); rc=$?
  ok 'tarball: a relative --record naming OUT.record.tsv is kept' [ "$rc" = 0 -a -s "$t/dist/rel.tar.gz.record.tsv" ]
  # outputs that would overwrite or land in the input
  cp -p "$t/php-9.9.1.tar.gz" "$t/same.tar.gz"
  out=$("${me[@]}" --src "$t/same.tar.gz" --tarball "$t/same.tar.gz" 2>&1); rc=$?
  ok 'tarball: OUT that is the input: exit 2' [ "$rc" = 2 ]
  ok '... input unchanged' cmp -s "$t/php-9.9.1.tar.gz" "$t/same.tar.gz"
  out=$("${me[@]}" --src "$t/tb" --tarball "$t/tb/inside.tar.gz" 2>&1); rc=$?
  ok 'tarball: OUT inside a directory input: exit 2' [ "$rc" = 2 -a ! -e "$t/tb/inside.tar.gz" ]
  st_tree "$t/tl" 9.9.1; ln -s /etc "$t/tl/src/etc"
  out=$("${me[@]}" --src "$t/tl" --tarball "$t/dist/tl.tar.gz" 2>&1); rc=$?
  ok 'tarball: a symlink out of the tree: exit 2' [ "$rc" = 2 ]
  ok '... and names it' has 'points outside the tree: src/etc'
  out=$("${me[@]}" --src "$t/tb" --tarball "$t/dist/l.tar.gz" --list 2>&1); rc=$?
  ok 'tarball with --list: exit 2' [ "$rc" = 2 ]
  echo "self-test: $pass passed, $fail failed"
  [ "$fail" = 0 ]
}

###########################################################
main() {
  case ${1:-} in --trace-driver) shift; trace_driver "$@"; exit $? ;; esac
  local a sv='' vers=$PSP_EOL_VERSIONS action=apply cm='' bundle='' i n o
  SELF=$(cd "$(dirname "$0")" && pwd -P)/$(basename "$0")
  SRC='' PDIR='' INC='' OS='' RECORD='' LOG='' VERBOSE=n OPTSTR='' OPTSRC='' SETS='' SEL_N=0 SKIP_N=0 TOUCHED=''
  TARBALL='' TB_OUT='' TB_FMT='' INPUT='' TBVER=''
  while [ $# -gt 0 ]; do
    case $1 in
      --src) SRC=${2:?--src needs a directory or tarball}; shift 2 ;;
      --tarball) TARBALL=${2:?--tarball needs FILE.tar.gz, FILE.tar.xz or a directory}; shift 2 ;;
      --version) sv=${2:?--version needs X.Y.Z}; shift 2 ;;
      --patch-dir) PDIR=${2:?--patch-dir needs a directory}; shift 2 ;;
      --inc) INC=${2:?--inc needs a file}; shift 2 ;;
      --os) OS=${2:?--os needs a value}; shift 2 ;;
      --set) SETS="$SETS ${2:?--set needs NAME=y|n}"; shift 2 ;;
      --record) RECORD=${2:?--record needs a file}; shift 2 ;;
      --dry-run) action=dry-run; shift ;;
      --list) action=list; shift ;;
      --verbose|-v) VERBOSE=y; shift ;;
      --manifest) action=manifest; shift ;;
      --versions) vers=${2:?--versions needs a list}; shift 2 ;;
      --check-manifest) action=check-manifest; cm=${2:?--check-manifest needs a file}; shift 2 ;;
      --make-bundle) action=make-bundle; bundle=${2:?--make-bundle needs a directory}; shift 2 ;;
      --self-test) action=self-test; shift ;;
      -h|--help) awk 'NR > 2 && /^#/ { if ($0 ~ /^####/) exit; sub(/^# ?/, ""); print; next } NR > 2 { exit }' "$0"; exit 0 ;;
      *) die "unknown option $1 (see --help)" ;;
    esac
  done
  [ "${BASH_VERSINFO[0]}" -ge 4 ] || die "needs bash 4.1 or newer"
  [ -z "$TARBALL" ] || [ "$action" = apply ] || die "--tarball cannot be combined with --$action"

  W=$(mktemp -d "${TMPDIR:-/tmp}/php-source-patch.XXXXXX") || die "mktemp failed"
  case $W in *$'\n'*) die "TMPDIR must not contain a newline" ;; esac
  TB_TMP='' TB_SHA_TMP=''
  trap cleanup EXIT
  trap 'exit 129' HUP; trap 'exit 130' INT; trap 'exit 143' TERM

  PATCH=${PATCH:-}
  if [ -z "$PATCH" ]; then command -v gpatch >/dev/null 2>&1 && PATCH='gpatch' || PATCH='patch'; fi
  TAR=${TAR:-}
  if [ -z "$TAR" ]; then command -v gtar >/dev/null 2>&1 && TAR='gtar' || TAR='tar'; fi
  # GNU tar keeps names as bytes; macOS bsdtar writes decomposed (NFD) pax names
  # for non-ASCII files, which then no longer match the names PHP's tests use
  [ -z "$TARBALL" ] || gnu_tar || die "--tarball needs GNU tar (gtar or tar); found: $($TAR --version 2>/dev/null | head -1)"
  case $RECORD in ''|/*) ;; *) RECORD=$PWD/$RECORD ;; esac
  PATCH_DESC=$($PATCH --version 2>/dev/null | head -1)
  case $PATCH_DESC in 'GNU patch'*|'patch 2.'*) ;; *) die "GNU patch is required (found: ${PATCH_DESC:-no $PATCH}); set PATCH=" ;; esac

  # data: patches/php and php_patch.inc
  if [ -n "$PDIR" ]; then
    PDIR=$(cd "$PDIR" 2>/dev/null && pwd -P) || die "no patch dir"
    case $PDIR in
      */patches/php) CUR_DIR=${PDIR%/patches/php} ;;
      *) CUR_DIR=$W/cur; mkdir -p "$CUR_DIR/patches"; ln -s "$PDIR" "$CUR_DIR/patches/php" ;;
    esac
  else
    CUR_DIR=$(cd "$(dirname "$SELF")/.." && pwd -P)
    PDIR=$CUR_DIR/patches/php
    [ -d "$PDIR" ] || [ "$action" = self-test ] || die "no patches/php beside this script; use --patch-dir"
  fi
  [ -n "$INC" ] || INC=$CUR_DIR/inc/php_patch.inc
  if [ "$action" != self-test ] || [ -f "$INC" ]; then
    INC=$(cd "$(dirname "$INC")" 2>/dev/null && pwd -P)/$(basename "$INC")
  fi
  if [ -f "$CUR_DIR/BUNDLE" ]; then DATAMODE=bundle
  elif [ -f "$CUR_DIR/centmin.sh" ]; then DATAMODE=centmin
  else DATAMODE=dir
  fi

  if [ "$action" = self-test ]; then self_test; exit $?; fi
  [ -f "$INC" ] || die "no php_patch.inc at $INC; use --inc"
  [ "$DATAMODE" = bundle ] && verify_bundle
  INC_SHA=$(sha256_of "$INC" | awk '{ print $1 }')
  case $DATAMODE in
    centmin) DATA_DESC="centmin $CUR_DIR"
             a=$(git_rev); [ -n "$a" ] && DATA_DESC="$DATA_DESC (git $a)" ;;
    bundle) DATA_DESC="bundle $CUR_DIR ($(awk '$1 == "source" { print $2, $3, $4 }' "$CUR_DIR/BUNDLE"))" ;;
    *) DATA_DESC="patches $PDIR, inc $INC" ;;
  esac

  # options: Centmin Mod defaults < centmin.sh or BUNDLE < custom_config.inc < --set
  for a in $PSP_DEFAULTS; do opt_set "${a%%=*}" "${a#*=}" ''; done
  OPTSRC=
  if [ "$DATAMODE" = centmin ]; then
    for o in $PSP_OPTS; do a=$(opt_from_file "$o" "$CUR_DIR/centmin.sh"); [ -n "$a" ] && opt_set "$o" "$a" ''; done
    # a server's persistent config; never for manifests, which describe the defaults
    if [ "$action" != manifest ] && [ "$action" != check-manifest ] && [ "$action" != make-bundle ] && [ -f /etc/centminmod/custom_config.inc ]; then
      for o in $PSP_OPTS; do a=$(opt_from_file "$o" /etc/centminmod/custom_config.inc); [ -n "$a" ] && opt_set "$o" "$a" custom_config.inc; done
    fi
  elif [ "$DATAMODE" = bundle ]; then
    for a in $(awk '$1 == "default" { print $2 }' "$CUR_DIR/BUNDLE"); do opt_set "${a%%=*}" "${a#*=}" ''; done
  fi
  for a in $SETS; do opt_set "${a%%=*}" "${a#*=}" --set; done
  OPTSTR=$(opt_csv | tr , ' ')

  case $action in
    manifest) manifest "$vers"; exit $? ;;
    check-manifest) check_manifest "$cm"; exit $? ;;
    make-bundle) make_bundle "$bundle"; exit $? ;;
  esac

  # what to patch
  local mode=fake user_record='' from_tarball=n
  local -a top=()
  if [ -n "$SRC" ]; then
    if [ -f "$SRC" ]; then
      # a php.net style tarball: extract it into scratch and work on that copy
      [ "$action" != apply ] || [ -n "$TARBALL" ] || die "a tarball --src needs --tarball OUT, --list or --dry-run"
      INPUT=$(cd "$(dirname "$SRC")" 2>/dev/null && pwd -P)/$(basename "$SRC")
      mkdir -p "$W/in" && (cd "$W/in" && $TAR --no-same-owner -xf "$INPUT") || die "cannot extract $INPUT"
      shopt -s dotglob nullglob; top=("$W/in"/*); shopt -u dotglob nullglob
      [ ${#top[@]} -eq 1 ] && [ -d "${top[0]}" ] && [ ! -h "${top[0]}" ] || die "$INPUT does not hold exactly one top-level directory"
      SRC=${top[0]} from_tarball=y
    fi
    SRC=$(cd "$SRC" 2>/dev/null && pwd -P) || die "no source tree"
    [ -n "$INPUT" ] || INPUT=$SRC
    [ -f "$SRC/main/php_version.h" ] || die "$INPUT has no main/php_version.h"
    VER=$(awk '$1 == "#define" && $2 == "PHP_VERSION" { gsub(/"/, "", $3); print $3; exit }' "$SRC/main/php_version.h")
    # php.net names RC and dev trees by the full version: php-8.4.0RC1
    TBVER=$VER
    case $TBVER in ''|*[!A-Za-z0-9.+~-]*) TBVER=${VER%%[!0-9.]*} ;; esac
    VER=${VER%%[!0-9.]*}
    ID=$(awk '/PHP_VERSION_ID/ { print $3; exit }' "$SRC/main/php_version.h")
    [ -n "$ID" ] || ID=$(echo "$VER" | awk -F. '{ printf "%d", $1 * 10000 + $2 * 100 + $3 }')
    [ -z "$sv" ] || [ "$sv" = "$VER" ] || die "--version $sv does not match $INPUT (PHP $VER)"
    mode=tree
    if [ -n "$TARBALL" ]; then
      case $VER in [0-9]*.[0-9]*.[0-9]*) ;; *) die "cannot read a PHP version X.Y.Z (got '$VER')" ;; esac
      tarball_target
      # a configured or built tree would put objects and Makefiles in the "source" tarball
      [ ! -e "$SRC/config.status" ] || die "$INPUT has been configured (config.status); use the php.net tarball or a clean tree"
      mkdir -p "$W/out" || die "cannot create $W/out"
      if [ "$from_tarball" = y ]; then
        mv "$SRC" "$W/out/php-$TBVER" || die "cannot move the extracted tree"
      else
        cp -Rp "$SRC" "$W/out/php-$TBVER" || die "cannot copy $SRC"
      fi
      SRC=$W/out/php-$TBVER
      # not part of a release: the patch files Centmin Mod copies into the top of its
      # build trees, a git checkout's .git (its config can hold credentials) and an
      # earlier run's record dir, which as a symlink would carry this run's writes
      for a in "$SRC"/*.patch; do [ -f "$a" ] && rm -f "$a" && echo "note: left out ${a##*/} (a copied Centmin Mod patch file)"; done
      [ ! -e "$SRC/.git" ] || { rm -rf "$SRC/.git"; echo "note: left out .git"; }
      rm -rf "$SRC/.php-source-patch"
      # a symlink that leaves the tree would let patching or packing reach outside it
      a=$(find "$SRC" -type l | while IFS= read -r a; do
            case $(readlink "$a") in /*|..|../*|*/..|*/../*) printf '%s\n' "${a#"$SRC"/}"; break ;; esac
          done)
      [ -z "$a" ] || die "$INPUT has a symlink that points outside the tree: $a"
      [ -f "$SRC/configure" ] || echo "note: no configure script; a git checkout needs ./buildconf before ./configure"
      find "$SRC" \( -name '*.orig' -o -name '*.rej' \) -type f | LC_ALL=C sort > "$W/out.before"
      user_record=$RECORD RECORD=''
    fi
  elif [ "$action" = list ] && [ -n "$sv" ]; then
    VER=$sv
    ID=$(echo "$VER" | awk -F. '{ printf "%d", $1 * 10000 + $2 * 100 + $3 }')
  else
    die "--src is required (or --list --version X.Y.Z)"
  fi
  case $VER in [0-9]*.[0-9]*.[0-9]*) ;; *) die "cannot read a PHP version X.Y.Z (got '$VER')" ;; esac
  [ -n "$OS" ] || OS=$(detect_os)
  case " $PSP_OSES " in *" $OS "*) ;; *) die "--os must be one of: $PSP_OSES" ;; esac

  select_patches "$ID" "$VER" "$OS" "$(opt_csv)" "$mode" || exit 2
  select_skipped "$ID" "$VER" "$OS" "$(opt_csv)" "$mode"
  [ -n "$OPTSRC" ] && OPTSTR="$OPTSTR  [from:$OPTSRC]"
  if [ "$action" = list ]; then print_selection; exit 0; fi

  # prepare: checksums, CRLF-normalised copies, touched paths
  STARTED=$(date -u +%Y-%m-%dT%H:%M:%SZ) RUNMODE=$action
  for ((i = 0; i < SEL_N; i++)); do
    [ "${SEL_TYPE[i]}" = P ] || continue
    [ -f "$PDIR/${SEL_FILE[i]}" ] || die "missing patch file $PDIR/${SEL_FILE[i]}"
    SEL_SHA[i]=$(sha256_of "$PDIR/${SEL_FILE[i]}" | awk '{ print $1 }')
    normalize "$PDIR/${SEL_FILE[i]}" "$W/p.$i"
    a=${SEL_ARGS[i]}; [ "$a" = - ] && a=
    patch_paths "$W/p.$i" "$(strip_of $a)" > "$W/cand.$i"
  done
  if [ "$action" = dry-run ]; then
    TT=$W/tree
    for ((i = 0; i < SEL_N; i++)); do
      [ "${SEL_TYPE[i]}" = P ] || continue
      awk -v c="${SEL_CWD[i]}" '{ print (c == ".") ? $0 : c "/" $0 }' "$W/cand.$i"
    done | mirror "$SRC" "$TT" . || die "scratch copy failed"
    for ((i = 0; i < SEL_N; i++)); do mkdir -p "$TT/${SEL_CWD[i]}"; done
  else
    TT=$SRC
    [ -n "$RECORD" ] || RECORD=$SRC/.php-source-patch/record.tsv
    LOG=$(dirname "$RECORD")/patch.log
    mkdir -p "$(dirname "$RECORD")" && : > "$LOG" || die "cannot write $RECORD"
  fi

  echo "php-source-patch $PSP_VERSION: PHP $VER ($ID) in ${INPUT:-$SRC}${TB_OUT:+ -> ${TB_OUT##*/}}  os=$OS  $OPTSTR"
  echo "data: $DATA_DESC"
  local result=ok counts gap=n
  detect_applied || die "scratch copy failed"
  n=0
  for ((i = 0; i < SEL_N; i++)); do
    [ "${SEL_TYPE[i]}" = P ] || continue
    n=$((n + 1))
    [ "${SEL_APPLIED[i]}" = n ] && gap=y
    [ "${SEL_APPLIED[i]}" != n ] && [ $gap = y ] && echo "note: ${SEL_FILE[i]} is in the tree but an earlier selected patch is not"
  done
  for ((i = 0; i < SEL_N; i++)); do
    case ${SEL_TYPE[i]} in
      M) SEL_STATUS[i]=skipped-missing-file SEL_DETAIL[i]="listed in php_patch.inc:${SEL_LINE[i]}, no patch file"
         printf '  %-22s %s\n' "[missing-file]" "${SEL_FILE[i]}" ;;
      D) SEL_STATUS[i]=skipped-duplicate SEL_DETAIL[i]="php_patch.inc:${SEL_LINE[i]} calls it again"
         SEL_SHA[i]=$(sha_of_patch "${SEL_FILE[i]}") ;;
      T) # arguments were checked by path_ok; split them on spaces, never glob them
         if [ "$action" = apply ]; then (set -f; cd "$SRC/${SEL_CWD[i]}" && touch -- ${SEL_ARGS[i]}) && TOUCHED="${SEL_ARGS[i]}"; fi
         printf '  %-22s %s\n' "[touch]" "${SEL_ARGS[i]}" ;;
      P) if apply_one "$i"; then
           SEL_STATUS[i]=$ST SEL_DETAIL[i]=$DT
           printf '  %-22s %s\n' "[$ST$([ "$DT" = - ] || echo " $DT")]" "${SEL_FILE[i]}"
         else
           SEL_STATUS[i]=failed SEL_DETAIL[i]=$DT
           printf '  %-22s %s: %s\n' "[FAILED]" "${SEL_FILE[i]}" "$DT"
           result=failed
           break
         fi ;;
    esac
  done
  write_record "$result"
  counts=$(for ((i = 0; i < SEL_N; i++)); do [ "${SEL_TYPE[i]}" = T ] || echo "${SEL_STATUS[i]:-not-run}"; done | sort | uniq -c | awk '{ printf "%s%s %s", sep, $1, $2; sep = ", " }')
  echo "$([ "$action" = dry-run ] && echo 'dry-run: ')$n patches selected: ${counts:-none}"
  if [ -n "$TB_OUT" ]; then
    [ -s "$RECORD" ] || die "the record was not written ($RECORD)"
    if [ "$result" != ok ]; then
      # the scratch copy goes away, so keep its record beside the tarball it would have made
      a=${user_record:-$TB_OUT.record.tsv}
      { mkdir -p "$(dirname "$a")" && cp -p "$RECORD" "$a"; } || { echo "no tarball written: a patch failed; could not save the record to $a" >&2; exit 1; }
      echo "no tarball written: a patch failed; record: $a"
      [ ! -e "$TB_OUT" ] || echo "note: $TB_OUT is from an earlier run and was left as it is"
      exit 1
    fi
    if [ -n "$user_record" ]; then
      { mkdir -p "$(dirname "$user_record")" && cp -p "$RECORD" "$user_record"; } || die "could not write $user_record"
      echo "record: $user_record"
    fi
    # an earlier failed run's record, unless --record names that same file
    [ -n "$user_record" ] && [ "$user_record" -ef "$TB_OUT.record.tsv" ] || rm -f "$TB_OUT.record.tsv"
    # backups GNU patch made of fuzzed files are not part of the source
    find "$SRC" \( -name '*.orig' -o -name '*.rej' \) -type f | LC_ALL=C sort | comm -13 "$W/out.before" - | while IFS= read -r a; do rm -f "$a"; done
    make_tarball
    echo "record: php-$TBVER/.php-source-patch/record.tsv inside the tarball"
    exit 0
  fi
  [ -n "$RECORD" ] && echo "record: $RECORD"
  [ "$result" = ok ] || exit 1
  exit 0
}

main "$@"
