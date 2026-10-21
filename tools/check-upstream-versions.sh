#!/bin/bash
###########################################################
# Centmin Mod maintainer tool: compare the source compiled
# dependency versions pinned in centmin.sh, installer8x.sh
# and inc/*.inc against the latest upstream releases, and
# check that every pinned version still downloads.
#
# Not used by centmin.sh itself, safe to run anywhere with
# bash 4+, git, curl and GNU sort (-V).
#
# usage:
#   tools/check-upstream-versions.sh
#     check the working tree this script lives in
#   tools/check-upstream-versions.sh --refs "origin/132.00stable origin/140.00beta01 origin/141.00beta01"
#     check several branches side by side via git show
#     (run git fetch origin first), no checkout needed
#   options:
#     --format md|tsv   output format (default md)
#     --only-changes    only print rows needing attention
#     --no-pincheck     skip pinned version download checks
#     --jobs N          parallel network lookups (default 16)
#
# status column:
#   BROKEN  a pinned version does not download / tag missing
#   UPDATE  newer release within the same series
#   MAJOR   newer release only in a newer series (breaking)
#   OK      up to date
#   ?       upstream lookup failed
#
# exit code: 0 all OK, 1 updates available, 2 broken pins
#
# network notes: github.com archive/ and api.github.com can
# be blocked in sandboxes, so github tags are read with
# git ls-remote and archive links are verified by tag.
###########################################################
set -o pipefail
export LC_ALL=C

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
MIRROR='https://parts.centminmod.com'
FORMAT='md'
ONLY_CHANGES='n'
PINCHECK='y'
JOBS=16
REFS=''

while [ $# -gt 0 ]; do
  case "$1" in
    --refs) REFS="$2"; shift 2 ;;
    --format) FORMAT="$2"; shift 2 ;;
    --only-changes) ONLY_CHANGES='y'; shift ;;
    --no-pincheck) PINCHECK='n'; shift ;;
    --jobs) JOBS="$2"; shift 2 ;;
    -h|--help) sed -n '2,38p' "$0"; exit 0 ;;
    *) echo "unknown option: $1" >&2; exit 64 ;;
  esac
done

for bin in git curl sort sed awk xargs; do
  command -v "$bin" >/dev/null 2>&1 || { echo "missing $bin" >&2; exit 64; }
done

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

###########################################################
# dependency table
# group|software|source|upstream|tag->version sed|pin check|series|note
#
# source:   VAR                    variable in centmin.sh
#           inc/file.inc:VAR       variable (or local) in an inc file
#           installer:installer83.sh  PHPVERLATEST fallback
# upstream: gh:owner/repo:tagregex | php | pecl:pkg | nginx
#           freenginx | gnu:pkg
# pin check: tag:FMT  git tag FMT exists upstream ({V} = version)
#            url:URL  HTTP 200 for URL ({V} = version)
#            -        no check
# series:   any   compare against the newest release
#           major newest with the same X, newer X shown as MAJOR
#           minor newest with the same X.Y only (PHP branches,
#                 freenginx stable), newer X.Y is not reported
#           pin   tags are not sortable, only run the pin check
###########################################################
read -r -d '' DEPS <<'EOF'
nginx|Nginx|NGINX_VERSION|nginx||url:https://nginx.org/download/nginx-{V}.tar.gz|any|
nginx|freenginx|FREENGINX_VERSION|freenginx||url:https://freenginx.org/download/freenginx-{V}.tar.gz|minor|stable branch
nginx|njs|NGINX_NJS_VER|gh:nginx/njs:^[0-9]+\.[0-9]+\.[0-9]+$||tag:{V}|any|NGINX_NJS=n default
nginx|zlib (nginx)|NGINX_ZLIBVER|gh:madler/zlib:^v[0-9]+\.[0-9.]+$|s/^v//|url:https://www.zlib.net/zlib-{V}.tar.gz|any|zlib.net only hosts the latest release
nginx|PCRE2 (nginx)|NGINX_PCRETWOVER|gh:PCRE2Project/pcre2:^pcre2-[0-9]+\.[0-9]+$|s/^pcre2-//|url:MIRROR/centminmodparts/pcre/pcre2-{V}.tar.gz|any|NGINX_PCRE_TWO=n default, new versions need mirror upload
nginx|mimalloc (nginx)|NGINX_MIMALLOC_VERSION|gh:microsoft/mimalloc:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|major|
nginx|headers-more|NGINX_HEADERSMORE|gh:openresty/headers-more-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|set-misc|ORESTY_SETMISCVER|gh:openresty/set-misc-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|echo|ORESTY_ECHOVER|gh:openresty/echo-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|srcache|ORESTY_SRCCACHEVER|gh:openresty/srcache-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|memc|ORESTY_MEMCVER|gh:openresty/memc-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|redis2|ORESTY_REDISVER|gh:openresty/redis2-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|ngx_devel_kit|ORESTY_DEVELKITVER|gh:vision5/ngx_devel_kit:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|ngx-fancyindex|NGINX_FANCYINDEXVER|gh:aperezdc/ngx-fancyindex:^v[0-9.]+$|s/^v//|tag:v{V}|any|
nginx|ngx_cache_purge|NGINX_CACHEPURGEVER|gh:nginx-modules/ngx_cache_purge:^[0-9]+\.[0-9.]+$||tag:{V}|any|
nginx|nginx-dav-ext|NGINX_EXTWEBDAVVER|gh:arut/nginx-dav-ext-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|NGINX_WEBDAV=n default
nginx|upstream_check|NGINX_UPSTREAMCHECKVER|gh:yaoweibin/nginx_upstream_check_module:^v[0-9.]+$|s/^v//|tag:v{V}|any|NGINX_UPSTREAMCHECK=n default
nginx|zstd (nginx)|inc/zstd_nginx.inc:NGINX_ZSTD_VER|gh:facebook/zstd:^v1\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|any|
lua|lua-nginx-module|ORESTY_LUANGINXVER|gh:openresty/lua-nginx-module:^v0\.10\.[0-9]+$|s/^v//|tag:v{V}|any|must match lua-resty-core base.lua
lua|stream-lua|ORESTY_LUASTREAMVER|gh:openresty/stream-lua-nginx-module:^v0\.0\.[0-9]+$|s/^v//|tag:v{V}|any|must match lua-resty-core base.lua
lua|lua-resty-core|ORESTY_LUARESTYCOREVER|gh:openresty/lua-resty-core:^v0\.1\.[0-9]+$|s/^v//|tag:v{V}|any|pins exact lua-nginx/stream-lua versions
lua|lua-resty-lrucache|ORESTY_LUALRUCACHEVER|gh:openresty/lua-resty-lrucache:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-dns|ORESTY_LUADNSVER|gh:openresty/lua-resty-dns:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-mysql|ORESTY_LUAMYSQLVER|gh:openresty/lua-resty-mysql:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-string|ORESTY_LUASTRINGVER|gh:openresty/lua-resty-string:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-websocket|ORESTY_LUAWEBSOCKETVER|gh:openresty/lua-resty-websocket:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-redis|ORESTY_LUAREDISVER|gh:openresty/lua-resty-redis:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-memcached|ORESTY_LUAMEMCACHEDVER|gh:openresty/lua-resty-memcached:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-lock|ORESTY_LUALOCKVER|gh:openresty/lua-resty-lock:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-upload|ORESTY_LUAUPLOADVER|gh:openresty/lua-resty-upload:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-upstream-healthcheck|ORESTY_LUAUPSTREAMCHECKVER|gh:openresty/lua-resty-upstream-healthcheck:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-upstream-nginx-module|ORESTY_LUAUPSTREAMVER|gh:openresty/lua-upstream-nginx-module:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-redis-parser|ORESTY_LUAREDISPARSERVER|gh:openresty/lua-redis-parser:^v[0-9.]+$|s/^v//|tag:v{V}|any|
lua|lua-resty-logger-socket|ORESTY_LUALOGGERSOCKETVER|gh:cloudflare/lua-resty-logger-socket:^v[0-9.]+$|s/^v//|tag:v{V}|pin|tags v0.01 v0.02 v0.1 do not sort
lua|lua-cjson|LUACJSONVER|gh:openresty/lua-cjson:^2\.[0-9.]+$||tag:{V}|any|
ssl|AWS-LC|AWS_LC_VERSION|gh:aws/aws-lc:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|any|AWS_LC_SWITCH=n default
ssl|LibreSSL|LIBRESSL_VERSION|gh:libressl/portable:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-{V}.tar.gz|any|LIBRESSL_SWITCH=n default
php|PHP 8.1 fallback|installer:installer81.sh|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installer fetches latest at runtime
php|PHP 8.2 fallback|installer:installer82.sh|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installer fetches latest at runtime
php|PHP 8.3 fallback|installer:installer83.sh|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installer fetches latest at runtime
php|PHP 8.4 fallback|installer:installer84.sh|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installer fetches latest at runtime
php|PHP 8.5 fallback|installer:installer85.sh|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installer fetches latest at runtime
php|PHP_VERSION default|PHP_VERSION|php||url:https://www.php.net/distributions/php-{V}.tar.gz|minor|installers rewrite it at install
php|libzip|LIBZIP_VER|gh:nih-at/libzip:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://github.com/nih-at/libzip/releases/download/v{V}/libzip-{V}.tar.gz|any|
php|libzip (PHP 8.5)|LIBZIP_EIGHT_FIVE_VER|gh:nih-at/libzip:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://github.com/nih-at/libzip/releases/download/v{V}/libzip-{V}.tar.gz|any|
php|libsodium|LIBSODIUM_VER|gh:jedisct1/libsodium:^[0-9.]+-RELEASE$|s/-RELEASE$//|url:https://download.libsodium.org/libsodium/releases/libsodium-{V}.tar.gz|any|
php|argon2|LIBARGON_VER|gh:P-H-C/phc-winner-argon2:^[0-9]{8}$||tag:{V}|any|
php|libgd (external)|LIBGD_EXTERNAL_VER|gh:libgd/libgd:^gd-[0-9.]+$|s/^gd-//|tag:gd-{V}|any|
php|curl (PHP build)|inc/php_configure.inc:CURL_VER|gh:curl/curl:^curl-[0-9]+_[0-9]+_[0-9]+$|s/^curl-//;s/_/./g|url:https://curl.se/download/curl-{V}.tar.gz|any|
phpext|redis ext (PHP 7.4+)|REDISPHPSEVENFOUR_VER|pecl:redis||url:https://pecl.php.net/get/redis-{V}.tgz|any|
phpext|mongodb ext (PHP 8.2)|MONGODBPHP_EIGHTTWO_VER|pecl:mongodb||url:https://pecl.php.net/get/mongodb-{V}.tgz|major|2.x drops deprecated APIs
phpext|mongodb ext (PHP 8.3)|MONGODBPHP_EIGHTTHREE_VER|pecl:mongodb||url:https://pecl.php.net/get/mongodb-{V}.tgz|major|2.x drops deprecated APIs
phpext|mongodb ext (PHP 8.4)|MONGODBPHP_EIGHTFOUR_VER|pecl:mongodb||url:https://pecl.php.net/get/mongodb-{V}.tgz|major|2.x drops deprecated APIs
phpext|mongodb ext (PHP 8.5)|MONGODBPHP_EIGHTFIVE_VER|pecl:mongodb||url:https://pecl.php.net/get/mongodb-{V}.tgz|major|2.x drops deprecated APIs
phpext|swoole (PHP 8.0)|PHPSWOOLE_EIGHT_ZERO_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|major|
phpext|swoole (PHP 8.1)|PHPSWOOLE_EIGHT_ONE_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|minor|check newer minor still supports PHP 8.1
phpext|swoole (PHP 8.2)|PHPSWOOLE_EIGHT_TWO_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|major|
phpext|swoole (PHP 8.3)|PHPSWOOLE_EIGHT_THREE_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|major|
phpext|swoole (PHP 8.4)|PHPSWOOLE_EIGHT_FOUR_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|major|
phpext|swoole (PHP 8.5)|PHPSWOOLE_EIGHT_FIVE_VER|gh:swoole/swoole-src:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://pecl.php.net/get/swoole-{V}.tgz|major|
phpext|imagick ext (PHP 7+)|IMAGICKPHP_SEVEN_VER|pecl:imagick||url:https://pecl.php.net/get/imagick-{V}.tgz|any|
phpext|mailparse ext (PHP 7.4+)|MAILPARSEPHPSEVENFOUR_COMPATVER|pecl:mailparse||url:https://pecl.php.net/get/mailparse-{V}.tgz|any|
phpext|mcrypt ext|PHP_MCRYPTPECLVER|pecl:mcrypt||url:https://pecl.php.net/get/mcrypt-{V}.tgz|any|
phpext|timezonedb ext|PHPTIMEZONEDB_VER|pecl:timezonedb||url:https://pecl.php.net/get/timezonedb-{V}.tgz|any|
image|libheif|LIBHEIF_VER|gh:strukturag/libheif:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://github.com/strukturag/libheif/releases/download/v{V}/libheif-{V}.tar.gz|any|IMAGEMAGICK_HEIF=n default
image|libde265|LIBDE265_VER|gh:strukturag/libde265:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|url:https://github.com/strukturag/libde265/releases/download/v{V}/libde265-{V}.tar.gz|any|IMAGEMAGICK_HEIF=n default
other|memcached server|MEMCACHED_VERSION|gh:memcached/memcached:^1\.[0-9]+\.[0-9]+$||url:https://www.memcached.org/files/memcached-{V}.tar.gz|any|
other|libevent|LIBEVENT_VERSION|gh:libevent/libevent:^release-[0-9.]+-stable$|s/^release-//;s/-stable$//|tag:release-{V}-stable|any|
other|zstd|inc/compress.inc:ZSTD_VER|gh:facebook/zstd:^v1\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|any|
other|OWASP CRS|MODSECURITY_OWASPVER|gh:coreruleset/coreruleset:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|major|NGINX_MODSECURITY=n default
other|checksec|CHECKSEC_VERSION|gh:slimm609/checksec:^[0-9]+\.[0-9]+\.[0-9]+$||tag:{V}|any|3.x is a Go rewrite
other|mold (EL8+)|MOLD_VERSION_EL8|gh:rui314/mold:^v[0-9]+\.[0-9]+\.[0-9]+$|s/^v//|tag:v{V}|any|NGX_LDMOLD=n default
other|wget (EL8)|WGET_VERSION_EIGHT|gnu:wget||url:MIRROR/centminmodparts/wget/wget-{V}.tar.gz|any|new versions need mirror upload
other|wget (EL9)|WGET_VERSION_NINE|gnu:wget||url:MIRROR/centminmodparts/wget/wget-{V}.tar.gz|any|new versions need mirror upload
other|wget (EL10)|WGET_VERSION_TEN|gnu:wget||url:MIRROR/centminmodparts/wget/wget-{V}.tar.gz|any|new versions need mirror upload
EOF

###########################################################
# read files from the working tree or from git refs
###########################################################
if [ -n "$REFS" ]; then
  read -r -a TARGETS <<< "$REFS"
  for r in "${TARGETS[@]}"; do
    git -C "$ROOT" rev-parse -q --verify "$r^{commit}" >/dev/null \
      || { echo "unknown git ref: $r (git fetch origin first?)" >&2; exit 64; }
  done
else
  TARGETS=('worktree')
fi

read_file() {
  # $1 target, $2 repo relative path
  if [ "$1" = 'worktree' ]; then
    cat "$ROOT/$2" 2>/dev/null
  else
    git -C "$ROOT" show "$1:$2" 2>/dev/null
  fi
}

# cache each target's files once
tkey() { echo "$1" | tr '/:' '__'; }
cached_file() {
  local f="$WORK/src.$(tkey "$1").$(echo "$2" | tr '/' '_')"
  [ -f "$f" ] || read_file "$1" "$2" > "$f"
  echo "$f"
}

pinned_value() {
  # $1 target, $2 source spec
  local src="$2" file var f
  case "$src" in
    installer:*)
      f=$(cached_file "$1" "${src#installer:}")
      grep -oE 'PHPVERLATEST:-"[0-9.]+"' "$f" | head -n1 | grep -oE '[0-9][0-9.]*'
      return ;;
    *:*) file="${src%%:*}"; var="${src#*:}" ;;
    *) file='centmin.sh'; var="$src" ;;
  esac
  f=$(cached_file "$1" "$file")
  sed -nE "s/^[[:space:]]*(local[[:space:]]+)?${var}=['\"]?([^'\" #]+).*/\2/p" "$f" | head -n1 | sed 's/^v//'
}

###########################################################
# upstream lookups (run in parallel, cached as files)
###########################################################
keyhash() { printf '%s' "$1" | cksum | awk '{print $1"-"$2}'; }
export -f keyhash
fetch_upstream() {
  # $1 upstream spec, writes one version per line to cache
  local spec="$1" out="$WORK/up.$(keyhash "$1")" tmp i
  tmp="$out.tmp"
  for i in 1 2 3; do
    case "$spec" in
      gh:*)
        local rest="${spec#gh:}" repo regex
        repo="${rest%%:*}"; regex="${rest#*:}"
        git ls-remote --tags --refs "https://github.com/${repo}.git" 2>/dev/null \
          | awk -F/ '{print $NF}' | grep -E "$regex" > "$tmp" ;;
      php)
        { curl -sfL --max-time 30 'https://www.php.net/releases/index.php?json&version=8&max=400'
          curl -sfL --max-time 30 'https://www.php.net/releases/index.php?json&version=7&max=400'; } \
          | grep -oE '"version":"[0-9]+\.[0-9]+\.[0-9]+"|"[0-9]+\.[0-9]+\.[0-9]+":\{' \
          | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' > "$tmp"
        # add the current release per branch
        for v in 7.4 8.0 8.1 8.2 8.3 8.4 8.5 8.6; do
          curl -sfL --max-time 30 "https://www.php.net/releases/index.php?json&version=${v}" \
            | grep -oE '"version":"[0-9.]+"' | grep -oE '[0-9][0-9.]*' >> "$tmp"
        done ;;
      pecl:*)
        curl -sfL --max-time 30 "https://pecl.php.net/rest/r/${spec#pecl:}/allreleases.xml" \
          | tr -d '\n' | grep -oE '<r><v>[^<]+</v><s>stable</s>' \
          | sed -E 's/<r><v>([^<]+)<.*/\1/' > "$tmp" ;;
      nginx)
        curl -sfL --max-time 30 'https://nginx.org/en/download.html' \
          | grep -oE 'nginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | sed -E 's/nginx-|\.tar\.gz//g' > "$tmp" ;;
      freenginx)
        curl -sfL --max-time 30 'https://freenginx.org/en/download.html' \
          | grep -oE 'freenginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | sed -E 's/freenginx-|\.tar\.gz//g' > "$tmp" ;;
      gnu:*)
        local p="${spec#gnu:}"
        curl -sfL --max-time 30 "https://ftp.gnu.org/gnu/${p}/" \
          | grep -oE "${p}-[0-9]+\.[0-9]+(\.[0-9]+)?\.tar\.gz" | sed -E "s/${p}-|\.tar\.gz//g" > "$tmp" ;;
    esac
    [ -s "$tmp" ] && break
    sleep $((i * 2))
  done
  sort -u "$tmp" > "$out" 2>/dev/null
  rm -f "$tmp"
}
export -f fetch_upstream
export WORK

url_status() {
  # $1 url, prints http code
  local code i
  for i in 1 2; do
    code=$(curl -sIL -o /dev/null -w '%{http_code}' --max-time 30 "$1" 2>/dev/null)
    case "$code" in 200|404|410) break ;; esac
    # some servers reject HEAD, retry with a ranged GET
    code=$(curl -sL -r 0-0 -o /dev/null -w '%{http_code}' --max-time 30 "$1" 2>/dev/null)
    case "$code" in 200|206) code=200; break ;; 404|410) break ;; esac
    sleep 2
  done
  echo "$1 $code" > "$WORK/url.$(keyhash "$1")"
}
export -f url_status

upfile() { echo "$WORK/up.$(keyhash "$1")"; }
urlfile() { echo "$WORK/url.$(keyhash "$1")"; }

# NUL delimited so xargs keeps regex backslashes intact
echo "$DEPS" | awk -F'|' 'NF>=7 {print $4}' | sort -u | tr '\n' '\0' \
  | xargs -0 -P "$JOBS" -I{} bash -c 'fetch_upstream "$1"' _ {}

###########################################################
# version helpers
###########################################################
ver_gt() { [ "$1" != "$2" ] && [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | tail -n1)" = "$1" ]; }
series_prefix() {
  # $1 version, $2 any|major|minor
  case "$2" in
    major) echo "${1%%.*}." ;;
    minor) echo "$1" | awk -F. '{print $1"."$2"."}' ;;
    *) echo '' ;;
  esac
}
upstream_versions() {
  # $1 upstream spec, $2 tag->version sed; prints "version<TAB>tag"
  local f; f=$(upfile "$1")
  [ -s "$f" ] || return 1
  while read -r tag; do
    local v="$tag"
    [ -n "$2" ] && v=$(echo "$tag" | sed -E "$2")
    printf '%s\t%s\n' "$v" "$tag"
  done < "$f" | awk -F'\t' 'tolower($1) !~ /(rc|alpha|beta|pre|dev|test)/' | sort -V -k1,1
}

###########################################################
# evaluate
###########################################################
declare -A PIN
declare -a ROWS
while IFS='|' read -r group name src up sedx check series note; do
  [ -z "$group" ] && continue
  for t in "${TARGETS[@]}"; do
    PIN["$name|$t"]=$(pinned_value "$t" "$src")
  done
done <<< "$DEPS"

# queue pinned + candidate url checks
if [ "$PINCHECK" = 'y' ]; then
  : > "$WORK/urls"
  while IFS='|' read -r group name src up sedx check series note; do
    [ -z "$group" ] && continue
    case "$check" in url:*) ;; *) continue ;; esac
    tmpl="${check#url:}"; tmpl="${tmpl//MIRROR/$MIRROR}"
    for t in "${TARGETS[@]}"; do
      v="${PIN["$name|$t"]}"; [ -n "$v" ] && echo "${tmpl//\{V\}/$v}" >> "$WORK/urls"
    done
  done <<< "$DEPS"
  sort -u "$WORK/urls" | tr '\n' '\0' | xargs -0 -P "$JOBS" -I{} bash -c 'url_status "$1"' _ {}
fi

WORST=0
while IFS='|' read -r group name src up sedx check series note; do
  [ -z "$group" ] && continue
  vers=$(upstream_versions "$up" "$sedx")
  latest_all=$(echo "$vers" | tail -n1 | cut -f1)
  row_status='OK'; cells=(); latest_shown=''
  any_pin='n'
  for t in "${TARGETS[@]}"; do
    v="${PIN["$name|$t"]}"
    if [ -z "$v" ]; then cells+=('-'); continue; fi
    any_pin='y'
    mark=''
    # pin check
    if [ "$PINCHECK" = 'y' ]; then
      case "$check" in
        tag:*)
          want="${check#tag:}"; want="${want//\{V\}/$v}"
          if [ -s "$(upfile "$up")" ] && ! grep -qxF "$want" "$(upfile "$up")"; then
            mark=' ✗missing-tag'; row_status='BROKEN'
          fi ;;
        url:*)
          tmpl="${check#url:}"; tmpl="${tmpl//MIRROR/$MIRROR}"; u="${tmpl//\{V\}/$v}"
          code=$(awk '{print $2}' "$(urlfile "$u")" 2>/dev/null)
          case "$code" in
            200) ;;
            404|410) mark=" ✗HTTP${code}"; row_status='BROKEN' ;;
            *) mark=" (url ${code:-n/a})" ;;
          esac ;;
      esac
    fi
    # version compare
    if [ "$series" = 'pin' ]; then
      latest_shown='n/a'
    elif [ -z "$latest_all" ]; then
      [ "$row_status" = 'OK' ] && row_status='?'
    else
      pfx=$(series_prefix "$v" "$series")
      latest_series=$(echo "$vers" | cut -f1 | grep -F -- "$pfx" | awk -v p="$pfx" 'index($0,p)==1' | sort -V | tail -n1)
      [ -z "$latest_series" ] && latest_series="$latest_all"
      latest_shown="$latest_series"
      [ "$series" = 'major' ] && [ "$latest_all" != "$latest_series" ] && latest_shown="$latest_series (newest $latest_all)"
      if ver_gt "$latest_series" "$v"; then
        mark="${mark} ↑${latest_series}"
        [ "$row_status" = 'OK' -o "$row_status" = 'MAJOR' ] && row_status='UPDATE'
      elif [ "$series" != 'minor' ] && ver_gt "$latest_all" "$v"; then
        mark="${mark} ⇡${latest_all}"
        [ "$row_status" = 'OK' ] && row_status='MAJOR'
      fi
    fi
    cells+=("${v}${mark}")
  done
  [ "$any_pin" = 'n' ] && continue
  case "$row_status" in
    BROKEN) WORST=2 ;;
    UPDATE|MAJOR) [ "$WORST" -lt 1 ] && WORST=1 ;;
  esac
  [ "$ONLY_CHANGES" = 'y' ] && [ "$row_status" = 'OK' ] && continue
  varname="${src#*:}"; [ "${src%%:*}" = 'installer' ] && varname="${src#installer:} PHPVERLATEST"
  ROWS+=("$(printf '%s\t%s\t%s\t%s\t%s\t%s\t%s' "$row_status" "$group" "$name" "$varname" "$(IFS=$'\t'; echo "${cells[*]}")" "${latest_shown:-?}" "$note")")
done <<< "$DEPS"

###########################################################
# output
###########################################################
hdr=$(printf 'Status\tGroup\tSoftware\tVariable'; for t in "${TARGETS[@]}"; do printf '\t%s' "${t#origin/}"; done; printf '\tLatest upstream\tNote')
sorted=$(printf '%s\n' "${ROWS[@]}" | awk -F'\t' 'BEGIN{o["BROKEN"]=0;o["UPDATE"]=1;o["MAJOR"]=2;o["?"]=3;o["OK"]=4}{print o[$1]"\t"$0}' | sort -s -t$'\t' -k1,1n | cut -f2-)
if [ "$FORMAT" = 'tsv' ]; then
  echo "$hdr"; [ -n "$sorted" ] && echo "$sorted"
else
  echo "$hdr" | sed 's/\t/ | /g; s/^/| /; s/$/ |/'
  echo "$hdr" | awk -F'\t' '{s="|"; for(i=1;i<=NF;i++) s=s"---|"; print s}'
  [ -n "$sorted" ] && echo "$sorted" | sed 's/|/\\|/g; s/\t/ | /g; s/^/| /; s/$/ |/'
  echo
  echo "Legend: ↑x.y.z newer in same series, ⇡x.y.z newer major/minor series only, ✗ pinned version does not download or tag missing, (url NNN) download check inconclusive."
fi

# lua-resty-core hard codes the exact lua-nginx-module and
# stream-lua versions it runs with, report the matching set
core_tag=$(upstream_versions 'gh:openresty/lua-resty-core:^v0\.1\.[0-9]+$' 's/^v//' | tail -n1 | cut -f2)
if [ -n "$core_tag" ]; then
  base=$(curl -sfL --max-time 30 "https://raw.githubusercontent.com/openresty/lua-resty-core/${core_tag}/lib/resty/core/base.lua")
  http_req=$(echo "$base" | grep -oE 'ngx_lua_version ~= [0-9]+' | head -n1 | grep -oE '[0-9]+$')
  stream_req=$(echo "$base" | grep -oE 'ngx_lua_version ~= [0-9]+' | sed -n 2p | grep -oE '[0-9]+$')
  if [ -n "$http_req" ]; then
    [ "$FORMAT" = 'tsv' ] && pfx='# ' || { pfx=''; echo; }
    echo "${pfx}lua-resty-core ${core_tag#v} requires lua-nginx-module 0.$((http_req / 1000)).$((http_req % 1000)) and stream-lua 0.0.${stream_req:-?} exactly: bump ORESTY_LUANGINXVER, ORESTY_LUASTREAMVER and ORESTY_LUARESTYCOREVER together."
  fi
fi
exit "$WORST"
