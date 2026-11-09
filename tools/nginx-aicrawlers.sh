#!/bin/bash
###########################################################
# nginx AI crawler maps: categorised user agents and verified crawler IPs
#
# WHAT IT DOES
# Downloads ai.robots.txt's robots.json and the crawler IP lists Google, Bing,
# OpenAI, Anthropic, Perplexity and Apple publish, checks them with jq and
# writes two nginx include files to /usr/local/nginx/conf/aicrawlers/:
#
#   aicrawlers_ua.conf
#     $aicrawler_category  training, search, user or other for the user agent
#                          tokens robots.json lists, empty for anything else
#     $aicrawler_claim     google, bing, openai, anthropic, perplexity or apple
#                          when the user agent claims to be that vendor's crawler
#   aicrawlers_ip.conf
#     $aicrawler_ipvendor  vendor whose published crawler IP list holds the
#                          client address ($remote_addr), ipv6 for any other
#                          IPv6 address, empty for any other IPv4 address
#     $aicrawler_verified  1 = claimed crawler from its vendor's IPs
#                          0 = claimed crawler from any other IP (spoofed)
#                          empty = no claim, that vendor's list is unavailable,
#                          or an IPv6 client of a vendor with an IPv4-only list
#
# The files only set variables and never block anything. Centmin Mod ships
# empty copies, and /usr/local/nginx/conf/botlimit.conf includes them:
# - until this script runs, nothing changes
# - once it has run, Googlebot and bingbot user agents from IPs outside
#   Google's and Bing's lists drop from botlimit.conf's whitelist level 1 to
#   the rate limited level 2
# - AI crawler categories stay at level 0 until you raise them in
#   botlimit.conf's $bot_agent_ai map (2 = rate limit, 3 = block)
# botlimit.conf is only active when nginx.conf includes it inside http {} and
# your vhosts use $bot_agent (see the end of botlimit.conf). On servers whose
# botlimit.conf predates these maps, compare it with
# /usr/local/src/centminmod/config/nginx/botlimit.conf.
#
# Categories: training = collects content to train AI models, search = AI
# search crawler, user = fetches that a person asked an AI assistant or agent
# to make, other = no clear purpose. Most come from robots.json's "function"
# field. The table in this script sets the major crawlers and leaves out
# robots.txt-only tokens (Google-Extended, Applebot-Extended) and tokens too
# generic to match safely. Google's user-triggered-fetchers.json is not used:
# it lists Google Cloud customers' apps, not Google.
#
# Behind Cloudflare or another proxy, set up real_ip first (cloudflare.conf),
# or $remote_addr is the proxy's IP and every claimed crawler fails the check.
#
# USAGE
#   nginx-aicrawlers.sh update        download, check, write, nginx -t, reload
#   nginx-aicrawlers.sh dry-run       download and check, show what would change
#   nginx-aicrawlers.sh print         download and check, print the files
#   nginx-aicrawlers.sh status        installed files, includes, cron entry
#   nginx-aicrawlers.sh cron-install  daily update via /etc/cron.d/centminmod-aicrawlers
#   nginx-aicrawlers.sh cron-remove   remove that cron entry
#
# update replaces the files only when they changed, and keeps them only when
# nginx -t passes; otherwise it puts the previous files back. A list that
# fails to download or validate is replaced by its last good copy. A vendor
# without a good copy of every list is left out, so its crawlers get an empty
# verdict and keep their old botlimit.conf level.
###########################################################
# SETTINGS: defaults; override them in /etc/centminmod/custom_config.inc
AICRAWLERS_DIR='/usr/local/nginx/conf/aicrawlers'
AICRAWLERS_ROBOTS_URL='https://raw.githubusercontent.com/ai-robots-txt/ai.robots.txt/main/robots.json'
AICRAWLERS_SKIP=''        # more robots.json tokens to leave out, comma separated, e.g. 'Cursor,Devin'
AICRAWLERS_RELOAD='y'     # reload nginx after an update changed the files
FORCE_IPVFOUR='y'         # download over IPv4
# end of settings
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
umask 022

CONFIGSCANBASE='/etc/centminmod'
CM_DIR=$(readlink -f "$(dirname "${BASH_SOURCE[0]}")/..")
NGINX_BIN='/usr/local/sbin/nginx'
NGINX_CONFDIR='/usr/local/nginx/conf'
AIC_STATE_DIR='/var/lib/centminmod/aicrawlers'
AIC_CRON_FILE='/etc/cron.d/centminmod-aicrawlers'
AIC_LOCK='/var/run/centminmod-aicrawlers.lock'
AIC_UA='Mozilla/5.0 (compatible; centminmod-aicrawlers/1.0)'
AIC_FILES='aicrawlers_ua.conf aicrawlers_ip.conf'
AIC_DRYRUN='n'

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  if grep -q $'\r' "${CONFIGSCANBASE}/custom_config.inc" 2>/dev/null; then
    # CRLF line endings would turn 'y' into 'y\r'
    eval "$(tr -d '\r' < "${CONFIGSCANBASE}/custom_config.inc")"
  else
    source "${CONFIGSCANBASE}/custom_config.inc"
  fi
fi

# vendor and URL of each crawler IP list. All use the same JSON layout:
# {"creationTime": ..., "prefixes": [{"ipv4Prefix": ...} | {"ipv6Prefix": ...}]}
AIC_IPLISTS='
google https://developers.google.com/static/crawling/ipranges/common-crawlers.json
google https://developers.google.com/static/crawling/ipranges/special-crawlers.json
google https://developers.google.com/static/crawling/ipranges/user-triggered-fetchers-google.json
bing https://www.bing.com/toolbox/bingbot.json
openai https://openai.com/gptbot.json
openai https://openai.com/searchbot.json
openai https://openai.com/chatgpt-user.json
anthropic https://claude.com/crawling/bots.json
perplexity https://www.perplexity.ai/perplexitybot.json
perplexity https://www.perplexity.ai/perplexity-user.json
apple https://search.developer.apple.com/applebot.json
'
AIC_VENDORS='google bing openai anthropic perplexity apple'

# user agent claims checked against those lists. googlebot and bingbot match
# like botlimit.conf's whitelist entries so the check covers the same requests
AIC_CLAIM_MAP='map $http_user_agent $aicrawler_claim {
  default                                              "";
  "~*googlebot"                                        google;
  "~*bingbot"                                          bing;
  "~*\b(?:GPTBot|OAI-SearchBot|ChatGPT-User)\b"        openai;
  "~*\b(?:ClaudeBot|Claude-SearchBot|Claude-User)\b"   anthropic;
  "~*\b(?:PerplexityBot|Perplexity-User)\b"            perplexity;
  "~*\bApplebot\b"                                     apple;
}'

# category for tokens where robots.json's "function" text is unclear or wrong,
# "skip" leaves a token out: robots.txt-only tokens that never appear in a
# user agent, and words too generic to match safely ("Spider" also matches
# search engines such as "Sogou web spider")
AIC_CATEGORY_TABLE='{
  "GPTBot": "training",
  "OAI-SearchBot": "search",
  "ChatGPT-User": "user",
  "ClaudeBot": "training",
  "anthropic-ai": "training",
  "Claude-SearchBot": "search",
  "Claude-User": "user",
  "PerplexityBot": "search",
  "Perplexity-User": "user",
  "CCBot": "training",
  "Bytespider": "training",
  "Applebot": "search",
  "Google-Extended": "skip",
  "Applebot-Extended": "skip",
  "__usage_ai-training": "skip",
  "Spider": "skip",
  "Code": "skip"
}'

# robots.json -> "category<TAB>lowercase token<TAB>regex" lines. Only tokens
# made of letters, digits, space and . _ / + - are used, so nothing from the
# download can break out of the nginx string or the regex
AIC_JQ_ROBOTS='
  def cat($f):
    if $f == "AI Data Scrapers" then "training"
    elif $f == "AI Search Crawlers" then "search"
    elif $f == "AI Assistants" or $f == "AI Agents" or $f == "AI Coding Agents" then "user"
    elif ($f | test("train"; "i")) then "training"
    elif ($f | test("search"; "i")) then "search"
    elif ($f | test("user[- ]initiated|user prompts|on behalf of"; "i")) then "user"
    else "other" end;
  def rx: [explode[] | if . == 46 then "\\." elif . == 43 then "\\+" else ([.] | implode) end] | join("");
  if type != "object" then error("robots.json is not an object") else . end
  | to_entries[]
  | select(.value | type == "object")
  | .key as $t
  | select($t | test("\\A[A-Za-z0-9][A-Za-z0-9 ._/+-]*[A-Za-z0-9]\\z"))
  | select(($skip | index("," + ($t | ascii_downcase) + ",")) == null)
  | (if $ov[$t] then $ov[$t] else cat(.value.function // "" | tostring) end) as $c
  | select($c != "skip")
  | [$c, ($t | ascii_downcase), (($t | rx) + (if .value.has_name_and_version == true then "/[0-9]+" else "" end))]
  | join("\t")'

# crawler IP list -> one prefix per line, or an error when any entry is not a
# plain IPv4/IPv6 address or prefix, or is broader than /12 (IPv4) or /24
# (IPv6), so a broken or tampered list cannot mark the whole internet verified
AIC_JQ_IPLIST='
  def v4ok: test("\\A[0-9]{1,3}(\\.[0-9]{1,3}){3}(/[0-9]{1,2})?\\z")
    and (split("/")[0] | split(".") | all(tonumber <= 255))
    and ((split("/")[1] // "32") | tonumber | . >= 12 and . <= 32);
  def v6ok: test("\\A[0-9A-Fa-f:]+(/[0-9]{1,3})?\\z")
    and (split("/")[0] as $a | ($a | test(":")) and ($a | test(":::") | not)
      and ([$a | scan("::")] | length <= 1)
      and ($a | split(":") | length <= 8 and all(.[]; length <= 4))
      and (($a | test("::")) or ($a | split(":") | length == 8)))
    and ((split("/")[1] // "128") | tonumber | . >= 24 and . <= 128);
  def pfx: if type != "object" then null
    elif has("ipv4Prefix") then {v: 4, p: .ipv4Prefix}
    elif has("ipv6Prefix") then {v: 6, p: .ipv6Prefix}
    else null end;
  def ok: try (. != null and (.p | type) == "string"
    and (if .v == 4 then (.p | v4ok) else (.p | v6ok) end)) catch false;
  if type != "object" or (.prefixes | type) != "array" or (.prefixes | length) == 0
  then error("no prefixes") else . end
  | [.prefixes[] | pfx] as $l
  | if ($l | all(ok)) then $l[].p else error("invalid prefix") end'

msg() {
  echo "$*" >&2
}

aic_usage() {
  echo "Usage: $0 {update|dry-run|print|status|cron-install|cron-remove}"
  echo "  update        download, check, write $AICRAWLERS_DIR files, nginx -t, reload"
  echo "  dry-run       download and check, show what update would change"
  echo "  print         download and check, print the generated files"
  echo "  status        installed files, includes and cron entry"
  echo "  cron-install  run update daily from $AIC_CRON_FILE"
  echo "  cron-remove   remove $AIC_CRON_FILE"
}

aic_need_root() {
  if [ "$(id -u)" -ne 0 ]; then
    msg "ERROR: run $1 as root"
    exit 1
  fi
}

aic_valid() {
  # $1 robots|iplist, $2 file
  case "$1" in
    # robots.json must still convert to at least 50 usable tokens, so a format
    # change upstream falls back to the last good copy instead of an empty map
    robots) [ "$(jq -r --argjson ov "$AIC_CATEGORY_TABLE" --arg skip ',,' "$AIC_JQ_ROBOTS" "$2" 2>/dev/null | wc -l)" -ge 50 ] ;;
    iplist) jq -r "$AIC_JQ_IPLIST" "$2" >/dev/null 2>&1 ;;
    *) return 1 ;;
  esac
}

aic_get() {
  # $1 url, $2 robots|iplist, $3 download dir. Prints the path of a validated
  # copy: the fresh download, else the last good copy, which a valid fresh
  # download replaces (not in dry-run/print). Returns 1 when neither is usable
  local url="$1" kind="$2" name fresh cache ipopt=''
  name=$(echo "${url#https://}" | tr '/' '_')
  fresh="${3}/${name}"
  cache="${AIC_STATE_DIR}/json/${name}"
  if [[ "$FORCE_IPVFOUR" = [yY] ]]; then
    ipopt='-4'
  fi
  if curl -sSfL --proto '=https' --proto-redir '=https' $ipopt --max-time 60 --retry 2 --retry-delay 5 -A "$AIC_UA" -o "$fresh" "$url" && aic_valid "$kind" "$fresh"; then
    if [[ "$AIC_DRYRUN" != [yY] ]]; then
      mkdir -p "${AIC_STATE_DIR}/json" && cp -f "$fresh" "${cache}.tmp" && mv -f "${cache}.tmp" "$cache"
    fi
    echo "$fresh"
    return 0
  fi
  if [ -s "$cache" ] && aic_valid "$kind" "$cache"; then
    msg "WARNING: $url failed to download or validate, using the last good copy from $(date -r "$cache" '+%F %T')"
    echo "$cache"
    return 0
  fi
  msg "WARNING: $url failed to download or validate and there is no last good copy"
  return 1
}

aic_generate() {
  # $1 work dir: writes $1/out/aicrawlers_ua.conf, $1/out/aicrawlers_ip.conf
  # and $1/summary
  local work="$1" out="$1/out" robots='' vendor v url f ok vendors_ok='' skip now
  mkdir -p "$out" "$work/dl" "$work/ip" || return 1
  now=$(date -u '+%F %T UTC')
  : > "$work/summary"

  # user agent categories
  : > "$work/categories.tsv"
  if robots=$(aic_get "$AICRAWLERS_ROBOTS_URL" robots "$work/dl"); then
    skip=",$(echo "$AICRAWLERS_SKIP" | sed -e 's/[[:space:]]*,[[:space:]]*/,/g' -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' | tr 'A-Z' 'a-z'),"
    if ! jq -r --argjson ov "$AIC_CATEGORY_TABLE" --arg skip "$skip" "$AIC_JQ_ROBOTS" "$robots" > "$work/categories.tsv"; then
      msg "WARNING: robots.json could not be converted, the category map stays empty"
      : > "$work/categories.tsv"
    fi
  else
    msg "WARNING: no robots.json, the category map stays empty"
  fi
  {
    echo "# AI crawler user agent maps written by tools/nginx-aicrawlers.sh on ${now}"
    echo "# from ${AICRAWLERS_ROBOTS_URL}"
    echo "# Do not edit, the next update replaces this file. Leave tokens out with"
    echo "# AICRAWLERS_SKIP in /etc/centminmod/custom_config.inc"
    echo
    echo 'map $http_user_agent $aicrawler_category {'
    echo '  default "";'
    # first category wins when a user agent matches several, one regex per
    # category split below nginx's 4096 byte limit for a single parameter
    awk -F'\t' -v maxlen=1500 -v summary="$work/summary" '
      !($2 in seen) {
        seen[$2] = 1
        n[$1]++
        item[$1, n[$1]] = $3
      }
      END {
        split("training search user other", order, " ")
        for (o = 1; o <= 4; o++) {
          c = order[o]
          line = ""
          for (i = 1; i <= n[c]; i++) {
            if (line != "" && length(line) + length(item[c, i]) + 1 > maxlen) {
              print "  \"~*\\b(?:" line ")\\b\" " c ";"
              line = ""
            }
            line = (line == "") ? item[c, i] : line "|" item[c, i]
          }
          if (line != "") {
            print "  \"~*\\b(?:" line ")\\b\" " c ";"
          }
          printf "category %-9s %d user agent tokens\n", c, n[c] >> summary
        }
      }' "$work/categories.tsv"
    echo '}'
    echo
    echo "$AIC_CLAIM_MAP"
  } > "$out/aicrawlers_ua.conf" || return 1

  # verified crawler IPs: a vendor counts only when every one of its lists is
  # available, so a missing list cannot turn its real crawlers into "spoofed"
  for vendor in $AIC_VENDORS; do
    : > "$work/ip/$vendor"
    ok=y
    while read -r v url; do
      [ "$v" = "$vendor" ] || continue
      if ! f=$(aic_get "$url" iplist "$work/dl") || ! jq -r "$AIC_JQ_IPLIST" "$f" >> "$work/ip/$vendor"; then
        ok=n
        break
      fi
    done <<< "$AIC_IPLISTS"
    if [ "$ok" = y ] && [ -s "$work/ip/$vendor" ]; then
      tr 'A-Z' 'a-z' < "$work/ip/$vendor" | sort -u > "$work/ip/${vendor}.sorted"
      vendors_ok="${vendors_ok} ${vendor}"
      echo "vendor   $(printf '%-10s' "$vendor") $(wc -l < "$work/ip/${vendor}.sorted") IP prefixes" >> "$work/summary"
    else
      msg "WARNING: ${vendor} crawler IPs left out, its crawlers get an empty \$aicrawler_verified"
      echo "vendor   $(printf '%-10s' "$vendor") left out (no usable list)" >> "$work/summary"
    fi
  done
  {
    echo "# AI crawler IP maps written by tools/nginx-aicrawlers.sh on ${now}"
    echo "# geo uses \$remote_addr: behind a proxy or CDN set up real_ip first"
    echo "# Do not edit, the next update replaces this file"
    echo
    if [ -n "$vendors_ok" ]; then
      echo 'geo $aicrawler_ipvendor {'
      echo '  # IPv6 addresses in no list'
      echo '  ::/0 ipv6;'
      for vendor in $vendors_ok; do
        echo "  # ${vendor}"
        awk -v v="$vendor" '{ print "  " $1 " " v ";" }' "$work/ip/${vendor}.sorted"
      done | awk '$1 == "#" || !seen[$1]++'
      echo '}'
      echo
      echo 'map "$aicrawler_claim:$aicrawler_ipvendor" $aicrawler_verified {'
      echo '  default "";'
      for vendor in $vendors_ok; do
        echo "  \"${vendor}:${vendor}\" 1;"
        if ! grep -q ':' "$work/ip/${vendor}.sorted"; then
          # IPv4 only list: an IPv6 client claiming this crawler stays unknown
          echo "  \"${vendor}:ipv6\" \"\";"
        fi
        echo "  \"~^${vendor}:\" 0;"
      done
      echo '}'
    else
      echo 'map $remote_addr $aicrawler_ipvendor {'
      echo '  default "";'
      echo '}'
      echo
      echo 'map $remote_addr $aicrawler_verified {'
      echo '  default "";'
      echo '}'
    fi
  } > "$out/aicrawlers_ip.conf" || return 1
  return 0
}

aic_nginx_test_dir() {
  # $1 dir with both files: nginx -t on a minimal config that includes only
  # them, so they are checked even where botlimit.conf is not in use.
  # Returns 2 when there is no nginx binary
  local t rc
  [ -x "$NGINX_BIN" ] || return 2
  t=$(mktemp -d "${TMPDIR:-/tmp}/aicrawlers-test.XXXXXX") || return 1
  # nginx opens <prefix>/logs/error.log before it reads the config
  mkdir -p "${t}/logs"
  cat > "${t}/nginx.conf" <<EOF
pid ${t}/nginx.pid;
error_log ${t}/error.log;
events { worker_connections 64; }
http {
  access_log off;
  map_hash_bucket_size 128;
  include ${1}/aicrawlers_ua.conf;
  include ${1}/aicrawlers_ip.conf;
}
EOF
  "$NGINX_BIN" -t -q -p "${t}/" -c "${t}/nginx.conf"
  rc=$?
  rm -rf "$t"
  return $rc
}

aic_stubs() {
  # botlimit.conf includes both files, so put the shipped empty copies in
  # place when either is missing
  local f
  mkdir -p "$AICRAWLERS_DIR" || return 1
  for f in $AIC_FILES; do
    if [ ! -f "${AICRAWLERS_DIR}/${f}" ]; then
      if [ -f "${CM_DIR}/config/nginx/aicrawlers/${f}" ]; then
        \cp -f "${CM_DIR}/config/nginx/aicrawlers/${f}" "${AICRAWLERS_DIR}/${f}" || return 1
      else
        msg "ERROR: ${CM_DIR}/config/nginx/aicrawlers/${f} not found, run cmupdate"
        return 1
      fi
    fi
  done
  return 0
}

aic_restore() {
  local f
  for f in $AIC_FILES; do
    if [ -f "${AIC_STATE_DIR}/backup/${f}" ]; then
      \cp -pf "${AIC_STATE_DIR}/backup/${f}" "${AICRAWLERS_DIR}/.${f}.new" && mv -f "${AICRAWLERS_DIR}/.${f}.new" "${AICRAWLERS_DIR}/${f}"
    fi
  done
}

aic_install() {
  # $1 dir with the new files. 0 = installed, 3 = unchanged, 1 = failed
  # (previous files kept or restored)
  local f changed=n
  for f in $AIC_FILES; do
    # the first line only carries the date it was written
    diff -q -I '^# AI crawler' "${1}/${f}" "${AICRAWLERS_DIR}/${f}" >/dev/null 2>&1 || changed=y
  done
  if [ "$changed" = n ]; then
    return 3
  fi
  if ! "$NGINX_BIN" -t -q >/dev/null 2>&1; then
    msg "ERROR: nginx -t already fails before any change, nothing installed:"
    "$NGINX_BIN" -t
    return 1
  fi
  mkdir -p "${AIC_STATE_DIR}/backup" || return 1
  for f in $AIC_FILES; do
    \cp -pf "${AICRAWLERS_DIR}/${f}" "${AIC_STATE_DIR}/backup/${f}" || return 1
  done
  for f in $AIC_FILES; do
    # rename in the same directory, so nginx never reads a half written file
    if ! \cp -f "${1}/${f}" "${AICRAWLERS_DIR}/.${f}.new" || ! chmod 0644 "${AICRAWLERS_DIR}/.${f}.new" || ! mv -f "${AICRAWLERS_DIR}/.${f}.new" "${AICRAWLERS_DIR}/${f}"; then
      msg "ERROR: could not write ${AICRAWLERS_DIR}/${f}, restoring the previous files"
      rm -f "${AICRAWLERS_DIR}/.${f}.new"
      aic_restore
      return 1
    fi
  done
  if ! "$NGINX_BIN" -t -q >/dev/null 2>&1; then
    msg "ERROR: nginx -t fails with the new files, restoring the previous ones:"
    "$NGINX_BIN" -t
    aic_restore
    return 1
  fi
  return 0
}

aic_reload() {
  if [[ "$AICRAWLERS_RELOAD" != [yY] ]]; then
    msg "AICRAWLERS_RELOAD='n': reload nginx to use the new files"
    return 0
  fi
  if command -v systemctl >/dev/null 2>&1 && systemctl is-active --quiet nginx; then
    systemctl reload nginx
  elif pidof nginx >/dev/null 2>&1; then
    "$NGINX_BIN" -s reload
  else
    msg "nginx is not running, not reloaded"
  fi
}

aic_realip_note() {
  if ! grep -Eqs '^[[:space:]]*(set_real_ip_from|include[[:space:]]+/usr/local/nginx/conf/cloudflare\.conf)' "${NGINX_CONFDIR}/nginx.conf" "${NGINX_CONFDIR}"/conf.d/*.conf; then
    msg "NOTE: no real_ip setup found in nginx.conf or conf.d/. Behind Cloudflare or another"
    msg "      proxy, \$remote_addr is the proxy's IP and every claimed crawler is marked spoofed"
  fi
}

aic_update() {
  local work rc
  aic_need_root update
  if ! command -v jq >/dev/null 2>&1; then
    msg "ERROR: jq is required: dnf -y install jq"
    exit 1
  fi
  if [ ! -x "$NGINX_BIN" ] || [ ! -d "$NGINX_CONFDIR" ]; then
    msg "ERROR: no nginx at $NGINX_BIN and $NGINX_CONFDIR"
    exit 1
  fi
  exec 9>"$AIC_LOCK"
  if ! flock -n 9; then
    msg "ERROR: another nginx-aicrawlers.sh update is running"
    exit 1
  fi
  aic_stubs || exit 1
  work=$(mktemp -d "${TMPDIR:-/tmp}/aicrawlers.XXXXXX") || exit 1
  if ! aic_generate "$work"; then
    msg "ERROR: could not generate the files, nothing changed"
    rm -rf "$work"
    exit 1
  fi
  cat "$work/summary"
  if ! aic_nginx_test_dir "$work/out"; then
    msg "ERROR: nginx -t rejects the generated files, nothing changed"
    rm -rf "$work"
    exit 1
  fi
  aic_install "$work/out"
  rc=$?
  rm -rf "$work"
  case "$rc" in
    0)
      echo "updated ${AICRAWLERS_DIR}/aicrawlers_ua.conf and aicrawlers_ip.conf, nginx -t passed"
      aic_reload
      ;;
    3)
      echo "no changes in ${AICRAWLERS_DIR}"
      ;;
    *)
      exit 1
      ;;
  esac
  mkdir -p "$AIC_STATE_DIR" && date '+%F %T' > "${AIC_STATE_DIR}/last-update"
  aic_realip_note
  return 0
}

aic_preview() {
  # $1 dry-run|print: generate into a temp dir without touching nginx or the
  # last good copies
  local work f rc
  if ! command -v jq >/dev/null 2>&1; then
    msg "ERROR: jq is required: dnf -y install jq"
    exit 1
  fi
  AIC_DRYRUN='y'
  work=$(mktemp -d "${TMPDIR:-/tmp}/aicrawlers.XXXXXX") || exit 1
  if ! aic_generate "$work"; then
    msg "ERROR: could not generate the files"
    rm -rf "$work"
    exit 1
  fi
  if [ "$1" = print ]; then
    for f in $AIC_FILES; do
      echo "# ---- ${AICRAWLERS_DIR}/${f}"
      cat "${work}/out/${f}"
      echo
    done
    rm -rf "$work"
    return 0
  fi
  cat "$work/summary"
  aic_nginx_test_dir "$work/out"
  rc=$?
  if [ "$rc" -eq 0 ]; then
    echo "nginx -t on the generated files: ok"
  elif [ "$rc" -eq 2 ]; then
    echo "nginx -t on the generated files: skipped, no $NGINX_BIN"
  else
    echo "nginx -t on the generated files: FAILED, update would not install them"
  fi
  for f in $AIC_FILES; do
    if [ ! -f "${AICRAWLERS_DIR}/${f}" ]; then
      echo "${AICRAWLERS_DIR}/${f}: not installed yet, update would create it"
    elif diff -q -I '^# AI crawler' "${AICRAWLERS_DIR}/${f}" "${work}/out/${f}" >/dev/null 2>&1; then
      echo "${AICRAWLERS_DIR}/${f}: unchanged apart from the date"
    else
      echo "${AICRAWLERS_DIR}/${f}: update would change it (first 100 diff lines):"
      diff -u "${AICRAWLERS_DIR}/${f}" "${work}/out/${f}" | head -n 100
    fi
  done
  echo "dry-run: nothing written, nginx not reloaded. 'print' shows the full files"
  rm -rf "$work"
  aic_realip_note
  return 0
}

aic_status() {
  local f
  echo "files in ${AICRAWLERS_DIR}:"
  for f in $AIC_FILES; do
    if [ -f "${AICRAWLERS_DIR}/${f}" ]; then
      echo "  ${f}: $(head -n 1 "${AICRAWLERS_DIR}/${f}")"
    else
      echo "  ${f}: missing"
    fi
  done
  if [ -f "${AIC_STATE_DIR}/last-update" ]; then
    echo "last update run: $(cat "${AIC_STATE_DIR}/last-update")"
  else
    echo "last update run: never"
  fi
  if grep -qs 'aicrawlers/aicrawlers_ua.conf' "${NGINX_CONFDIR}/botlimit.conf"; then
    echo "botlimit.conf: includes the AI crawler files"
  else
    echo "botlimit.conf: does not include them, compare with ${CM_DIR}/config/nginx/botlimit.conf"
  fi
  if grep -Eqs '^[[:space:]]*include[[:space:]]+/usr/local/nginx/conf/botlimit\.conf' "${NGINX_CONFDIR}/nginx.conf"; then
    echo "nginx.conf: includes botlimit.conf"
  else
    echo "nginx.conf: does not include botlimit.conf, so \$bot_agent is not in use"
  fi
  if [ -f "$AIC_CRON_FILE" ]; then
    echo "cron: $(grep -v '^#' "$AIC_CRON_FILE" | head -n 1)"
  else
    echo "cron: not installed (cron-install adds a daily update)"
  fi
  aic_realip_note
}

aic_cron_install() {
  local min
  aic_need_root cron-install
  # spread servers over the hour by hostname
  min=$(( $(hostname | cksum | awk '{print $1}') % 60 ))
  mkdir -p "$AIC_STATE_DIR" || exit 1
  {
    echo "# Centmin Mod AI crawler maps for nginx, remove with: ${CM_DIR}/tools/nginx-aicrawlers.sh cron-remove"
    echo "${min} 5 * * * root bash ${CM_DIR}/tools/nginx-aicrawlers.sh update > ${AIC_STATE_DIR}/last-cron.log 2>&1"
  } > "${AIC_CRON_FILE}.new" && chmod 0644 "${AIC_CRON_FILE}.new" && mv -f "${AIC_CRON_FILE}.new" "$AIC_CRON_FILE" || exit 1
  echo "installed ${AIC_CRON_FILE}: daily update at 05:$(printf '%02d' "$min")"
}

aic_cron_remove() {
  aic_need_root cron-remove
  if [ -f "$AIC_CRON_FILE" ]; then
    rm -f "$AIC_CRON_FILE"
    echo "removed ${AIC_CRON_FILE}"
  else
    echo "no ${AIC_CRON_FILE}"
  fi
}

case "$1" in
  update)
    aic_update
    ;;
  dry-run)
    aic_preview dry-run
    ;;
  print)
    aic_preview print
    ;;
  status)
    aic_status
    ;;
  cron-install)
    aic_cron_install
    ;;
  cron-remove)
    aic_cron_remove
    ;;
  *)
    aic_usage
    exit 1
    ;;
esac
