#!/bin/bash
###########################################################
# Centmin Mod unattended nginx and PHP-FPM updates
#
# Opt in via the persistent config /etc/centminmod/custom_config.inc
#
#   AUTOUPDATE_NGINX='y'   # nginx updates via centmin.sh menu option 4 routines
#   AUTOUPDATE_PHP='y'     # PHP-FPM updates via centmin.sh menu option 5 routines
#
# While either is 'y', an hourly /etc/cron.d/centminmod-autoupdate entry
# runs this script, and updates start only inside AUTOUPDATE_HOURS. The
# entry is removed when both are 'n'. dmotd SSH logins, centmin.sh,
# cmupdate and the check-for-updates.sh cron job put it back if it goes
# missing while an option is still 'y'.
#
# Usage:
#   autoupdate.sh check        show what an update run would do now
#   autoupdate.sh status       show settings, scheduler and last results
#   autoupdate.sh run          update now, ignoring AUTOUPDATE_HOURS and
#                              AUTOUPDATE_DELAY_HOURS and retrying failures
#   autoupdate.sh ensure       add, repair or remove the cron entry
#   autoupdate.sh notify-test  send a test email and Pushover notification
#   autoupdate.sh reset        forget failed updates so they are retried
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
export SYSTEMD_PAGER=''
# cron exports no TERM (tput then errors) and systemd no HOME
export TERM="${TERM:-dumb}"
export HOME="${HOME:-/root}"
umask 022
###########################################################
CM_INSTALLDIR='/usr/local/src/centminmod'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
FORCE_IPVFOUR='y'
AU_CRON_FILE='/etc/cron.d/centminmod-autoupdate'
AU_STATE_DIR='/var/lib/centminmod/autoupdate'
AU_LOG='/var/log/centminmod-autoupdate.log'
AU_LOCK='/var/run/centminmod-autoupdate.lock'
AU_REBUILDS="${CM_INSTALLDIR}/tools/autoupdate-rebuilds.txt"

# defaults, override in /etc/centminmod/custom_config.inc
AUTOUPDATE_NGINX='n'            # y = keep nginx updated
AUTOUPDATE_PHP='n'              # y = keep PHP-FPM updated within its installed major.minor branch
AUTOUPDATE_NGINX_TARGET='centmin' # centmin = NGINX_VERSION set by Centmin Mod code (cmupdate)
                                # latest = newest nginx.org (or freenginx.org) release
AUTOUPDATE_HOURS='2-5'          # server local hours an update may start in, e.g. 2-5 or 22-3, or any
AUTOUPDATE_DELAY_HOURS='24'     # wait after a new version is first seen; PHP security releases don't wait
AUTOUPDATE_CMUPDATE='y'         # run cmupdate first so patches and NGINX_VERSION are current
AUTOUPDATE_NOTIFY='all'         # all, failures or none
AUTOUPDATE_EMAIL=''             # space separated recipients. Sent through the EMAILNOTIFY_SES_* SMTP
                                # login when EMAILNOTIFY_SES='y' (see tools/emailnotify.sh), else via
                                # the local mail/sendmail command, which Gmail and other large
                                # providers reject unless the server has SPF/DKIM or a relay host
AUTOUPDATE_EMAIL_FROM=''        # optional sender address
AUTOUPDATE_PUSHOVER='y'         # notify via Pushover when PUSH_API_TOKEN and PUSH_USER_KEY are set
AUTOUPDATE_CHECK_URLS='http://127.0.0.1/' # space separated URLs that must not start failing (5xx or no answer)
AUTOUPDATE_MAXLOAD=''           # skip while the 5 minute load average is above this, default CPU thread count
AUTOUPDATE_MINDISK_MB='3072'    # free MB needed on the /svr-setup and /usr/local filesystems
AUTOUPDATE_NICE='10'            # CPU nice level for the build
AUTOUPDATE_CRON_MINUTE=''       # minute of the hourly cron run, default derived from the hostname
AUTOUPDATE_STUCK_HOURS='4'      # notify once when a run has held the lock for this long

NGINX_VERSION=$(sed -n "s/^NGINX_VERSION='\([^']*\)'.*/\1/p" "${CM_INSTALLDIR}/centmin.sh" 2>/dev/null | head -n1)
FREENGINX_VERSION=$(sed -n "s/^FREENGINX_VERSION='\([^']*\)'.*/\1/p" "${CM_INSTALLDIR}/centmin.sh" 2>/dev/null | head -n1)
FREENGINX_INSTALL='n'
NGINXBACKUPDIR='/usr/local/nginxbackup'

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  source "${CONFIGSCANBASE}/custom_config.inc"
fi
if [ -f "${CONFIGSCANBASE}/pushover.ini" ]; then
  source "${CONFIGSCANBASE}/pushover.ini"
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi
AU_HOST=$(hostname)
###########################################################
# helpers

au_log() {
  echo "$(date '+%Y-%m-%d %H:%M:%S') $*" >> "$AU_LOG" 2>/dev/null
}

au_say() {
  echo "$*"
  au_log "$*"
}

au_enabled() {
  [[ "$AUTOUPDATE_NGINX" = [yY] || "$AUTOUPDATE_PHP" = [yY] ]]
}

# true when version $1 is newer than $2
au_vergt() {
  [[ "$1" != "$2" && "$(printf '%s\n%s\n' "$1" "$2" | sort -V | tail -n1)" = "$1" ]]
}

au_is_ver() {
  [[ "$1" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]
}

au_head() {
  git -C "$CM_INSTALLDIR" rev-parse --short HEAD 2>/dev/null
}

au_service_active() {
  if [[ -d /run/systemd/system ]]; then
    systemctl is-active --quiet "$1"
  elif [[ "$1" = nginx ]]; then
    pgrep -x nginx >/dev/null 2>&1
  else
    pgrep -f 'php-fpm: master process' >/dev/null 2>&1
  fi
}

au_service_restart() {
  if [[ -d /run/systemd/system ]]; then
    systemctl restart "$1"
  else
    service "$1" restart
  fi
}

# true when the running master process still runs a binary that has since
# been replaced on disk, i.e. the service was never restarted onto the new build
au_stale_master() {
  local pid
  pid=$(pgrep -o -f "^$1: master process" 2>/dev/null)
  [[ -n "$pid" && "$(readlink "/proc/$pid/exe" 2>/dev/null)" = *' (deleted)' ]]
}

au_nginx_installed() {
  nginx -v 2>&1 | grep -Eo '[a-z]+/[0-9]+\.[0-9]+\.[0-9]+' | head -n1 | cut -d/ -f2
}

au_php_installed() {
  php-config --version 2>/dev/null
}

au_cron_minute() {
  if [[ "$AUTOUPDATE_CRON_MINUTE" =~ ^[0-9]+$ && "$AUTOUPDATE_CRON_MINUTE" -lt 60 ]]; then
    echo "$((10#$AUTOUPDATE_CRON_MINUTE))"
  else
    # spread servers across the hour instead of all hitting nginx.org/php.net at once
    echo $(( $(printf '%s' "$AU_HOST" | cksum | cut -d' ' -f1) % 60 ))
  fi
}

au_in_hours() {
  local h a b
  [[ "$AUTOUPDATE_HOURS" = any ]] && return 0
  [[ "$AUTOUPDATE_HOURS" =~ ^([0-9]{1,2})-([0-9]{1,2})$ ]] || return 1
  h=$((10#$(date +%H))) a=$((10#${BASH_REMATCH[1]})) b=$((10#${BASH_REMATCH[2]}))
  if (( a <= b )); then
    (( h >= a && h <= b ))
  else
    (( h >= a || h <= b ))
  fi
}

au_strip() {
  sed -e 's/\x1b\[[0-9;]*[a-zA-Z]//g' -e 's/\x1b(B//g'
}

###########################################################
# scheduler

au_cron_content() {
  cat <<EOF
# Centmin Mod unattended nginx/PHP-FPM updates, managed by ${CM_INSTALLDIR}/tools/autoupdate.sh
# Turn off with AUTOUPDATE_NGINX='n' and AUTOUPDATE_PHP='n' in ${CONFIGSCANBASE}/custom_config.inc,
# deleting this file alone does not: it is put back while either is 'y'.
SHELL=/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
$(au_cron_minute) * * * * root /bin/bash ${CM_INSTALLDIR}/tools/autoupdate.sh cron >/dev/null 2>&1
EOF
}

# Make the cron entry match the AUTOUPDATE_* settings. A missing or edited
# entry is put back and reported once per repair.
au_ensure() {
  local want had=n
  [[ "$(id -u)" -eq 0 ]] || return 0
  mkdir -p "$AU_STATE_DIR"
  if au_enabled; then
    want=$(au_cron_content)
    [[ -f "$AU_CRON_FILE" && "$(cat "$AU_CRON_FILE" 2>/dev/null)" = "$want" ]] && return 0
    [[ -f "$AU_CRON_FILE" ]] && had=y
    printf '%s\n' "$want" > "${AU_CRON_FILE}.tmp" && chmod 0644 "${AU_CRON_FILE}.tmp" && mv -f "${AU_CRON_FILE}.tmp" "$AU_CRON_FILE" || {
      rm -f "${AU_CRON_FILE}.tmp"
      au_log "ensure: could not write $AU_CRON_FILE"
      return 1
    }
    if [[ -f "$AU_STATE_DIR/scheduler-installed" ]]; then
      if [[ "$had" = y ]]; then
        au_log "ensure: $AU_CRON_FILE differed from the expected entry, rewritten (${AU_ENSURE_BY:-manual})"
      else
        au_log "ensure: $AU_CRON_FILE was missing, re-added (${AU_ENSURE_BY:-manual})"
        au_notify info "auto-update cron entry re-added" "$AU_CRON_FILE was missing while AUTOUPDATE_NGINX='$AUTOUPDATE_NGINX' AUTOUPDATE_PHP='$AUTOUPDATE_PHP', so it was re-added (found by ${AU_ENSURE_BY:-manual}). To stop auto-updates set both to 'n' in ${CONFIGSCANBASE}/custom_config.inc."
      fi
    else
      au_log "ensure: installed $AU_CRON_FILE (${AU_ENSURE_BY:-manual})"
      touch "$AU_STATE_DIR/scheduler-installed"
    fi
  elif [[ -f "$AU_CRON_FILE" || -f "$AU_STATE_DIR/scheduler-installed" ]]; then
    rm -f "$AU_CRON_FILE" "$AU_STATE_DIR/scheduler-installed"
    au_log "ensure: AUTOUPDATE_NGINX and AUTOUPDATE_PHP are off, removed $AU_CRON_FILE (${AU_ENSURE_BY:-manual})"
  fi
  return 0
}

###########################################################
# notifications

au_pushover() {
  local priority=$1 title=$2 message=$3 response
  [[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] || return 0
  response=$(curl -${ipv_forceopt}s --connect-timeout 10 --max-time 30 \
    --form-string "token=${PUSH_API_TOKEN}" \
    --form-string "user=${PUSH_USER_KEY}" \
    --form-string "title=${title:0:250}" \
    --form-string "message=${message:0:1024}" \
    --form-string "priority=${priority}" \
    https://api.pushover.net/1/messages.json)
  if [[ "$response" = *'"status":1'* ]]; then
    au_log "notify: pushover sent: $title"
  else
    au_log "notify: pushover failed: ${response:-no response}"
  fi
}

# Authenticated SMTP using the EMAILNOTIFY_SES_* settings tools/emailnotify.sh
# uses (any SMTP server with a login: Amazon SES, a Gmail app password, ...).
# Unauthenticated mail from a server without SPF/DKIM is rejected by Gmail
# and most large providers. The password goes to curl in a 0600 config file,
# not on its command line.
au_smtp() {
  local subject=$1 body=$2 dir from url pass rc rcpt args=()
  from=${AUTOUPDATE_EMAIL_FROM:-$EMAILNOTIFY_SES_FROM_EMAIL}
  dir=$(mktemp -d) || return 1
  {
    echo "From: $from"
    echo "To: ${AUTOUPDATE_EMAIL// /, }"
    echo "Subject: $subject"
    echo "Date: $(date -R)"
    echo
    printf '%s\n' "$body"
  } > "$dir/msg"
  pass=${EMAILNOTIFY_SES_SMTP_PASSWORD//\\/\\\\}
  pass=${pass//\"/\\\"}
  printf 'user = "%s:%s"\n' "$EMAILNOTIFY_SES_SMTP_USERNAME" "$pass" > "$dir/cfg"
  if [[ "$EMAILNOTIFY_SES_SMTP_PORT" = 465 ]]; then
    url="smtps://${EMAILNOTIFY_SES_SMTP_SERVER}:465"
  else
    url="smtp://${EMAILNOTIFY_SES_SMTP_SERVER}:${EMAILNOTIFY_SES_SMTP_PORT:-587}"
    args+=(--ssl-reqd)
  fi
  for rcpt in $AUTOUPDATE_EMAIL; do
    args+=(--mail-rcpt "$rcpt")
  done
  curl -sS --crlf --connect-timeout 15 --max-time 60 "${args[@]}" --mail-from "$from" -K "$dir/cfg" -T "$dir/msg" "$url" 2>"$dir/err"
  rc=$?
  [[ "$rc" -eq 0 ]] || au_log "notify: smtp $url failed (curl exit $rc): $(head -c 300 "$dir/err" | tr "\n" " ")"
  rm -rf "$dir"
  return "$rc"
}

au_email() {
  local subject=$1 body=$2 rc
  [[ -n "$AUTOUPDATE_EMAIL" ]] || return 0
  if [[ "$EMAILNOTIFY_SES" = [yY] && -n "$EMAILNOTIFY_SES_SMTP_SERVER" && -n "$EMAILNOTIFY_SES_SMTP_USERNAME" ]]; then
    au_smtp "$subject" "$body"
    rc=$?
  elif command -v mail >/dev/null 2>&1; then
    if [[ -n "$AUTOUPDATE_EMAIL_FROM" ]]; then
      printf '%s\n' "$body" | mail -s "$subject" -r "$AUTOUPDATE_EMAIL_FROM" $AUTOUPDATE_EMAIL
    else
      printf '%s\n' "$body" | mail -s "$subject" $AUTOUPDATE_EMAIL
    fi
    rc=$?
  elif [[ -x /usr/sbin/sendmail ]]; then
    {
      [[ -n "$AUTOUPDATE_EMAIL_FROM" ]] && echo "From: $AUTOUPDATE_EMAIL_FROM"
      echo "To: ${AUTOUPDATE_EMAIL// /, }"
      echo "Subject: $subject"
      echo
      printf '%s\n' "$body"
    } | /usr/sbin/sendmail -oi -t
    rc=$?
  else
    au_log "notify: no mail or sendmail command, email to $AUTOUPDATE_EMAIL not sent"
    return 1
  fi
  if [[ "$rc" -eq 0 ]]; then
    au_log "notify: email handed to $([[ "$EMAILNOTIFY_SES" = [yY] ]] && echo "smtp $EMAILNOTIFY_SES_SMTP_SERVER" || echo 'the local MTA') for $AUTOUPDATE_EMAIL: $subject"
  else
    au_log "notify: email to $AUTOUPDATE_EMAIL failed (exit $rc)"
  fi
}

# au_notify ok|fail|info title message [logfile]
au_notify() {
  local level=$1 title=$2 message=$3 logfile=$4 priority=0 body
  case "$AUTOUPDATE_NOTIFY" in
    none) return 0 ;;
    failures) [[ "$level" = ok ]] && return 0 ;;
  esac
  case "$level" in
    fail) priority=1 ;;
    info) priority=-1 ;;
  esac
  au_pushover "$priority" "$title $AU_HOST" "$message"
  body="$message

Host: $AU_HOST
Time: $(date '+%Y-%m-%d %H:%M:%S %Z')
Centmin Mod: $(au_head) ($CM_INSTALLDIR)
Status: bash ${CM_INSTALLDIR}/tools/autoupdate.sh status"
  if [[ -n "$logfile" && -f "$logfile" ]]; then
    body="$body
Log: $logfile

Last 60 log lines:
$(tail -n 60 "$logfile" | au_strip)"
  fi
  au_email "[centminmod autoupdate] $AU_HOST: $title" "$body"
}

###########################################################
# what to update

# php.net release data for a major.minor branch: sets AU_REL_VER, AU_REL_SECURITY
au_php_release() {
  local json
  AU_REL_VER='' AU_REL_SECURITY=n
  json=$(curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 "https://www.php.net/releases/index.php?json&version=$1") || return 1
  AU_REL_VER=$(printf '%s' "$json" | grep -o '"version":"[0-9][0-9.]*"' | head -n1 | cut -d'"' -f4)
  [[ "$json" =~ \"tags\":\[[^]]*\"security\" ]] && AU_REL_SECURITY=y
  au_is_ver "$AU_REL_VER" && [[ "${AU_REL_VER%.*}" = "$1" ]]
}

au_nginx_latest() {
  if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 https://freenginx.org/en/download.html | grep -Eo 'freenginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1
  else
    curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 https://nginx.org/download/ | grep -Eo 'nginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1
  fi
}

# Newest entry in tools/autoupdate-rebuilds.txt for component $1 (nginx or
# php-X.Y) that is newer than binary $2: prints "<epoch> <reason>".
# Maintainers add an entry when a Centmin Mod patch needs a same-version
# rebuild, e.g. security backports for end-of-life PHP branches.
au_rebuild_due() {
  local comp=$1 bin=$2 c when reason ts bints best=0 bestwhy=
  [[ -f "$AU_REBUILDS" && -f "$bin" ]] || return 1
  bints=$(stat -c %Y "$bin")
  while read -r c when reason; do
    [[ -z "$c" || "$c" = \#* ]] && continue
    [[ "$c" = "$comp" || ( "$c" = php && "$comp" = php-* ) ]] || continue
    ts=$(date -u -d "$when" +%s 2>/dev/null) || continue
    if (( ts > bints && ts > best )); then
      best=$ts bestwhy="$reason"
    fi
  done < "$AU_REBUILDS"
  (( best > 0 )) || return 1
  # one rebuild per entry, even if the binary time stays older (clock skew)
  [[ "$(cat "$AU_STATE_DIR/rebuilt-$comp" 2>/dev/null)" -ge "$best" ]] 2>/dev/null && return 1
  echo "$best $bestwhy"
}

# Decide what to do for component $1 (nginx|php). Sets AU_ACTION
# (upgrade|rebuild|none|skip|wait|held), AU_INSTALLED, AU_TARGET, AU_WHY,
# AU_KEY and AU_EPOCH.
au_plan() {
  local comp=$1 due branch seen age failed fkey fhead
  AU_ACTION=none AU_INSTALLED='' AU_TARGET='' AU_WHY='' AU_KEY='' AU_EPOCH='' AU_REL_SECURITY=n
  if [[ "$comp" = nginx ]]; then
    if ! command -v nginx >/dev/null 2>&1; then
      AU_ACTION=skip AU_WHY='nginx is not installed'; return
    fi
    AU_INSTALLED=$(au_nginx_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed nginx version '$(nginx -v 2>&1 | head -n1)' is not a release build"; return
    fi
    if [[ "$AUTOUPDATE_NGINX_TARGET" = latest ]]; then
      AU_TARGET=$(au_nginx_latest)
      au_is_ver "$AU_TARGET" || { AU_ACTION=skip AU_WHY='could not get the latest version from nginx.org'; return; }
    elif [[ "$FREENGINX_INSTALL" = [yY] ]]; then
      AU_TARGET=$FREENGINX_VERSION
    else
      AU_TARGET=$NGINX_VERSION
    fi
    au_is_ver "$AU_TARGET" || { AU_ACTION=skip AU_WHY="no usable target version (got '$AU_TARGET')"; return; }
    due=$(au_rebuild_due nginx /usr/local/sbin/nginx)
  else
    if ! command -v php-config >/dev/null 2>&1; then
      AU_ACTION=skip AU_WHY='PHP is not installed'; return
    fi
    AU_INSTALLED=$(au_php_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed PHP version '$AU_INSTALLED' is not a release build"; return
    fi
    branch=${AU_INSTALLED%.*}
    due=$(au_rebuild_due "php-$branch" /usr/local/sbin/php-fpm)
    if au_php_release "$branch"; then
      AU_TARGET=$AU_REL_VER
    elif [[ -n "$due" ]]; then
      # a rebuild needs no release data
      AU_TARGET=$AU_INSTALLED
    else
      AU_ACTION=skip AU_WHY="could not get PHP $branch release data from php.net"; return
    fi
  fi
  if au_vergt "$AU_TARGET" "$AU_INSTALLED"; then
    AU_ACTION=upgrade AU_KEY="$comp $AU_TARGET"
    AU_WHY="$AU_INSTALLED -> $AU_TARGET"
    [[ "$AU_REL_SECURITY" = y ]] && AU_WHY="$AU_WHY (security release)"
  elif [[ -n "$due" ]]; then
    AU_ACTION=rebuild AU_TARGET=$AU_INSTALLED AU_EPOCH=${due%% *}
    AU_KEY="$comp rebuild $AU_EPOCH"
    AU_WHY="rebuild $AU_INSTALLED: ${due#* }"
  else
    if au_vergt "$AU_INSTALLED" "$AU_TARGET"; then
      AU_WHY="installed $AU_INSTALLED is newer than target $AU_TARGET, not downgrading"
    else
      AU_WHY="$AU_INSTALLED is current"
    fi
    return
  fi
  # a failed update is retried only after the Centmin Mod code changed
  if [[ -f "$AU_STATE_DIR/failed-$comp" && "$AU_FORCE" != y ]]; then
    IFS='|' read -r fkey fhead failed < "$AU_STATE_DIR/failed-$comp"
    if [[ "$fkey" = "$AU_KEY" && "$fhead" = "$(au_head)" ]]; then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $(date -d "@$failed" '+%Y-%m-%d %H:%M' 2>/dev/null), waiting for a Centmin Mod code update or autoupdate.sh reset"
      return
    fi
  fi
  # soak time for new versions; security releases and rebuilds go ahead
  if [[ "$AU_ACTION" = upgrade && "$AU_REL_SECURITY" != y && "$AU_FORCE" != y ]]; then
    seen="$AU_STATE_DIR/seen-$comp-$AU_TARGET"
    [[ -f "$seen" ]] || touch "$seen"
    age=$(( ($(date +%s) - $(stat -c %Y "$seen")) / 3600 ))
    if (( age < ${AUTOUPDATE_DELAY_HOURS:-0} )); then
      AU_ACTION=wait AU_WHY="$AU_WHY: first seen ${age}h ago, waiting until AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h"
    fi
  fi
}

###########################################################
# safety checks before starting a build: prints why not, or nothing

# pid of a running centmin.sh, centmin-cli.sh or cmupdate: only processes
# executing one (directly or as "bash <script>"), not ones whose command
# line just mentions it, such as an editor
au_cmm_running() {
  local pid a0 a1
  for pid in $(pgrep -f 'centmin(-cli)?\.sh|cmupdate'); do
    a0='' a1=''
    { IFS= read -r -d '' a0; IFS= read -r -d '' a1; } < "/proc/$pid/cmdline" 2>/dev/null
    a0=${a0##*/} a1=${a1##*/}
    case "$a0" in
      bash|sh) a0=$a1 ;;
    esac
    case "$a0" in
      centmin.sh|centmin-cli.sh|cmupdate|cmupdate.sh) echo "$a0 (pid $pid)"; return 0 ;;
    esac
  done
  return 1
}

au_blocked() {
  local p load max avail d
  p=$(au_cmm_running) && { echo "$p is running"; return; }
  pgrep -x 'yum|dnf|rpm|dnf-automatic|yum-cron' >/dev/null 2>&1 && { echo "a yum/dnf/rpm process is running"; return; }
  max=${AUTOUPDATE_MAXLOAD:-$(nproc 2>/dev/null || echo 1)}
  load=$(awk '{print $2}' /proc/loadavg)
  awk -v l="$load" -v m="$max" 'BEGIN { exit !(l > m) }' && { echo "5 minute load $load is above AUTOUPDATE_MAXLOAD $max"; return; }
  for d in "$DIR_TMP" /usr/local; do
    [[ -d "$d" ]] || d=/
    avail=$(df -Pm "$d" | awk 'NR==2 {print $4}')
    [[ "$avail" -ge "${AUTOUPDATE_MINDISK_MB:-0}" ]] 2>/dev/null || { echo "only ${avail}MB free for $d, need AUTOUPDATE_MINDISK_MB=${AUTOUPDATE_MINDISK_MB}"; return; }
  done
  nginx -t >/dev/null 2>&1 || { echo "nginx -t fails with the current config, fix it first"; return; }
}

# HTTP status of each AUTOUPDATE_CHECK_URLS entry: "url=code url=code"
au_url_codes() {
  local u urls
  read -ra urls <<< "$AUTOUPDATE_CHECK_URLS"
  for u in "${urls[@]}"; do
    printf '%s=%s ' "$u" "$(curl -sk -o /dev/null --connect-timeout 5 --max-time 20 -w '%{http_code}' "$u")"
  done
}

# URLs that answered below 500 before but not after: prints them, or nothing
au_url_regressions() {
  local attempt after pair u b a bad pairs
  read -ra pairs <<< "$1"
  for attempt in 1 2 3; do
    after=" $(au_url_codes)"
    bad=
    for pair in "${pairs[@]}"; do
      u=${pair%=*} b=${pair##*=}
      [[ "$b" =~ ^[1-4][0-9][0-9]$ ]] || continue
      a=${after#* "$u"=}
      [[ "$a" = "$after" ]] && a= || a=${a%% *}
      [[ "$a" =~ ^[1-4][0-9][0-9]$ ]] || bad="$bad $u ($b -> ${a:-none})"
    done
    [[ -z "$bad" ]] && return 0
    sleep 3
  done
  echo "${bad# }"
}

###########################################################
# update one component

# Put back the nginx build saved by menu 4 when the new one fails our
# checks. Returns 2 when this run took no snapshot.
au_nginx_rollback() {
  local start=$1 runlog=$2 snap="${NGINXBACKUPDIR}/upgrade-snapshot"
  [[ -f "$snap/nginx" && "$(stat -c %Y "$snap")" -ge "$start" ]] || return 2
  echo "autoupdate: restoring the previous nginx build from $snap" >> "$runlog"
  (
    DT=$(date +%d%m%y-%H%M%S)
    source <(sed -n '/^nginx_upgrade_restore() {/,/^}/p' "${CM_INSTALLDIR}/inc/nginx_upgrade.inc")
    nginx_snapshot_ready=y
    declare -f nginx_upgrade_restore >/dev/null && nginx_upgrade_restore
  ) >> "$runlog" 2>&1 || return 1
  au_service_restart nginx >> "$runlog" 2>&1
  sleep 2
  au_service_active nginx
}

au_update() {
  local comp=$1 start runlog clilog rc now problems='' urls_before mods_before mods_after missing warn level title msg rolled=''
  start=$(date +%s)
  runlog="${CENTMINLOGDIR}/autoupdate_${comp}_$(date +%d%m%y-%H%M%S).log"
  mkdir -p "$CENTMINLOGDIR" "$AU_STATE_DIR"
  au_say "$comp: starting $AU_ACTION ($AU_WHY), log $runlog"
  echo "$comp|$AU_KEY|$start" > "$AU_STATE_DIR/inprogress"
  urls_before=$(au_url_codes)
  [[ "$comp" = php ]] && mods_before=$(php -m 2>/dev/null | sort -u)

  (
    cd "$CM_INSTALLDIR" || exit 1
    if command -v ionice >/dev/null 2>&1; then
      nice -n "${AUTOUPDATE_NICE:-10}" ionice -c2 -n7 bash ./centmin-cli.sh "${comp}upgrade" "$AU_TARGET"
    else
      nice -n "${AUTOUPDATE_NICE:-10}" bash ./centmin-cli.sh "${comp}upgrade" "$AU_TARGET"
    fi
  ) < /dev/null > "$runlog" 2>&1
  rc=$?
  clilog=$(find "$CENTMINLOGDIR" -maxdepth 1 -name "centminmod_*_${comp}_upgrade.log" -newermt "@$start" 2>/dev/null | sort | tail -n1)

  # check what is live now, whatever the exit status said
  [[ "$rc" -eq 0 ]] || problems="centmin-cli.sh ${comp}upgrade exited $rc;"
  if [[ "$comp" = nginx ]]; then
    now=$(au_nginx_installed)
    [[ "$now" = "$AU_TARGET" ]] || problems="$problems nginx -v reports '${now:-nothing}', expected $AU_TARGET;"
    [[ "$(stat -c %Y /usr/local/sbin/nginx 2>/dev/null || echo 0)" -ge "$start" ]] || problems="$problems /usr/local/sbin/nginx was not replaced;"
    au_service_active nginx || problems="$problems nginx is not running;"
    au_stale_master nginx && problems="$problems the nginx master still runs the old binary;"
    nginx -t >/dev/null 2>&1 || problems="$problems nginx -t fails;"
  else
    now=$(au_php_installed)
    [[ "$now" = "$AU_TARGET" ]] || problems="$problems php-config reports '${now:-nothing}', expected $AU_TARGET;"
    [[ "$(stat -c %Y /usr/local/sbin/php-fpm 2>/dev/null || echo 0)" -ge "$start" ]] || problems="$problems /usr/local/sbin/php-fpm was not replaced;"
    au_service_active php-fpm || problems="$problems php-fpm is not running;"
    au_stale_master php-fpm && problems="$problems the php-fpm master still runs the old binary;"
    php-fpm -t >/dev/null 2>&1 || problems="$problems php-fpm -t fails;"
    warn=$(php -m 2>&1 | grep -Ei 'warning|fatal|error' | head -n3 | tr '\n' ' ')
    [[ -z "$warn" ]] || problems="$problems PHP startup errors: $warn;"
    mods_after=$(php -m 2>/dev/null | sort -u)
    missing=$(comm -23 <(printf '%s\n' "$mods_before") <(printf '%s\n' "$mods_after") | grep -v '^\[' | tr '\n' ' ')
    [[ -z "$missing" ]] || problems="$problems PHP modules missing after the update: ${missing% };"
  fi
  if [[ -z "$problems" ]]; then
    missing=$(au_url_regressions "$urls_before")
    [[ -z "$missing" ]] || problems="URL checks started failing: $missing;"
  fi

  problems=$(printf '%s' "$problems" | sed 's/^ *//; s/; *$//')
  if [[ -z "$problems" ]]; then
    level=ok
    title="$comp $AU_TARGET $([[ "$AU_ACTION" = rebuild ]] && echo rebuilt || echo installed)"
    msg="Auto-update $AU_ACTION of $comp finished in $(( ($(date +%s) - start) / 60 )) min: $AU_WHY."
    rm -f "$AU_STATE_DIR/failed-$comp"
    [[ "$AU_ACTION" = rebuild ]] && echo "$AU_EPOCH" > "$AU_STATE_DIR/rebuilt-$comp"
  else
    level=fail
    title="$comp $AU_TARGET auto-update FAILED"
    if [[ "$comp" = nginx && "$rc" -eq 0 ]]; then
      # menu 4 succeeded but our checks did not: put the previous build back
      au_nginx_rollback "$start" "$runlog"
      case $? in
        0) rolled=" The previous nginx build $AU_INSTALLED was restored from ${NGINXBACKUPDIR}/upgrade-snapshot and is running." ;;
        2) rolled=" Menu 4 took no snapshot in this run, so nothing was rolled back." ;;
        *) rolled=" Restoring the previous nginx build from ${NGINXBACKUPDIR}/upgrade-snapshot FAILED, check nginx now." ;;
      esac
    elif [[ "$comp" = nginx ]]; then
      rolled=" Menu 4 handles its own rollback on failure, see its log."
    fi
    msg="Auto-update $AU_ACTION of $comp ($AU_WHY) failed: ${problems}.${rolled} nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO). It is not retried until the Centmin Mod code changes (cmupdate) or you run: autoupdate.sh reset"
    echo "$AU_KEY|$(au_head)|$(date +%s)" > "$AU_STATE_DIR/failed-$comp"
  fi
  rm -f "$AU_STATE_DIR/inprogress"
  [[ -n "$clilog" ]] && echo "centmin-cli log: $clilog" >> "$runlog"
  echo "$(date '+%Y-%m-%d %H:%M:%S')|$level|$AU_ACTION|$AU_WHY|${problems}|$runlog" > "$AU_STATE_DIR/last-$comp"
  au_say "$comp: $level: $msg"
  au_notify "$level" "$title" "$msg" "$runlog"
  # the full build output is in the centmin-cli log, keep the tail here
  if [[ -n "$clilog" ]] && [[ "$(wc -l < "$runlog")" -gt 300 ]]; then
    tail -n 300 "$runlog" > "${runlog}.tmp" && mv -f "${runlog}.tmp" "$runlog"
  fi
  [[ "$level" = ok ]]
}

###########################################################
# run

# An update that keeps being deferred (low disk, broken nginx config, a
# stopped service) would otherwise never happen silently: notify once
# after it has waited 72 hours.
au_pending() {
  local comp=$1 reason=$2 f="$AU_STATE_DIR/pending-$1" since
  [[ -f "$f" ]] || date +%s > "$f"
  since=$(cat "$f")
  if (( $(date +%s) - since > 72 * 3600 )) && [[ ! -f "$f.notified" ]]; then
    touch "$f.notified"
    au_notify fail "$comp auto-update blocked" "The $comp update ($AU_WHY) has been waiting since $(date -d "@$since" '+%Y-%m-%d %H:%M'): $reason"
  fi
}

au_cmupdate() {
  local stamp="$AU_STATE_DIR/cmupdate.stamp" before after
  [[ "$AUTOUPDATE_CMUPDATE" = [yY] && "$AU_NO_CMUPDATE" != y ]] || return 0
  # at most once every 20 hours
  [[ -n "$(find "$stamp" -mmin -1200 2>/dev/null)" ]] && return 0
  touch "$stamp"
  before=$(au_head)
  bash "${CM_INSTALLDIR}/tools/cmupdate.sh" update > "$AU_STATE_DIR/cmupdate.log" 2>&1
  after=$(au_head)
  if [[ -z "$after" ]]; then
    au_say "cmupdate: ${CM_INSTALLDIR} is not a git checkout after cmupdate, see $AU_STATE_DIR/cmupdate.log"
    return 1
  elif [[ "$before" != "$after" ]]; then
    au_say "cmupdate: Centmin Mod code updated $before -> $after"
    # carry on with the updated script, the lock stays held on fd 9
    if [[ -f "${CM_INSTALLDIR}/tools/autoupdate.sh" ]]; then
      AU_NO_CMUPDATE=y AU_LOCKED=y exec bash "${CM_INSTALLDIR}/tools/autoupdate.sh" "$AU_CMD"
    fi
    au_say "cmupdate: tools/autoupdate.sh no longer exists, stopping"
    exit 1
  else
    au_log "cmupdate: code unchanged at $after"
  fi
}

au_take_lock() {
  local pid started
  if [[ "$AU_LOCKED" != y ]]; then
    exec 9>>"$AU_LOCK"
    if ! flock -n 9; then
      read -r pid started < "${AU_LOCK}.info" 2>/dev/null
      au_log "skip: another autoupdate run (pid $pid) holds the lock"
      if [[ "$started" =~ ^[0-9]+$ ]] && (( $(date +%s) - started > ${AUTOUPDATE_STUCK_HOURS:-4} * 3600 )) && [[ ! -f "$AU_STATE_DIR/stuck-$started" ]]; then
        touch "$AU_STATE_DIR/stuck-$started"
        au_notify fail "auto-update run stuck" "An autoupdate run (pid $pid) has been running since $(date -d "@$started" '+%Y-%m-%d %H:%M'). Check it with: ps -fp $pid; tail $(ls -t ${CENTMINLOGDIR}/autoupdate_*.log 2>/dev/null | head -n1)"
      fi
      return 1
    fi
    echo "$$ $(date +%s)" > "${AU_LOCK}.info"
    rm -f "$AU_STATE_DIR"/stuck-*
  fi
  return 0
}

# $1 = cron (scheduled: hours window and delay apply) or run (manual)
au_run() {
  local mode=$1 comp why rc=0 inprog
  [[ "$(id -u)" -eq 0 ]] || { echo "autoupdate.sh must run as root"; return 1; }
  mkdir -p "$AU_STATE_DIR" "$CENTMINLOGDIR"
  if [[ "$mode" = cron ]]; then
    au_enabled || return 0
    au_in_hours || return 0
  fi
  au_take_lock || return 0
  if [[ -f "$AU_STATE_DIR/inprogress" ]]; then
    # the previous run died mid-build (reboot, kill): say so once
    inprog=$(cat "$AU_STATE_DIR/inprogress")
    rm -f "$AU_STATE_DIR/inprogress"
    au_notify fail "auto-update interrupted" "A previous auto-update run was interrupted mid-build (${inprog}). Check that nginx and php-fpm are healthy. nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO)."
  fi
  au_cmupdate
  [[ "$mode" = run ]] && AU_FORCE=y
  for comp in nginx php; do
    if [[ "$comp" = nginx && "$AUTOUPDATE_NGINX" != [yY] ]] || [[ "$comp" = php && "$AUTOUPDATE_PHP" != [yY] ]]; then
      continue
    fi
    au_plan "$comp"
    case "$AU_ACTION" in
      upgrade|rebuild) ;;
      none) rm -f "$AU_STATE_DIR/pending-$comp"*; au_log "$comp: $AU_WHY"; continue ;;
      *) au_say "$comp: $AU_ACTION: $AU_WHY"; continue ;;
    esac
    why=$(au_blocked)
    if [[ -z "$why" ]]; then
      if [[ "$comp" = nginx ]] && ! au_service_active nginx; then
        why='nginx is not running'
      elif [[ "$comp" = php ]] && ! au_service_active php-fpm; then
        why='php-fpm is not running'
      fi
    fi
    if [[ -n "$why" ]]; then
      au_say "$comp: deferred ($AU_WHY), $why"
      au_pending "$comp" "$why"
      continue
    fi
    rm -f "$AU_STATE_DIR/pending-$comp"*
    au_update "$comp" || rc=1
  done
  # keep 30 days of autoupdate logs
  find "$CENTMINLOGDIR" -maxdepth 1 -name 'autoupdate_*.log' -mtime +30 -delete 2>/dev/null
  if [[ -f "$AU_LOG" && "$(stat -c %s "$AU_LOG")" -gt 1048576 ]]; then
    mv -f "$AU_LOG" "${AU_LOG}.1"
  fi
  return "$rc"
}

au_check() {
  local comp enabled why
  for comp in nginx php; do
    enabled=$AUTOUPDATE_NGINX
    [[ "$comp" = php ]] && enabled=$AUTOUPDATE_PHP
    au_plan "$comp"
    printf '%-6s AUTOUPDATE_%s=%s installed=%s target=%s -> %s: %s\n' "$comp" "$(echo "$comp" | tr a-z A-Z)" "$enabled" "${AU_INSTALLED:--}" "${AU_TARGET:--}" "$AU_ACTION" "$AU_WHY"
  done
  why=$(au_blocked)
  echo "hours  AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS now $(date +%H:%M) -> $(au_in_hours && echo inside || echo outside)"
  echo "gates  ${why:-ok}"
}

au_status() {
  local comp
  echo "Settings: AUTOUPDATE_NGINX=$AUTOUPDATE_NGINX AUTOUPDATE_PHP=$AUTOUPDATE_PHP AUTOUPDATE_NGINX_TARGET=$AUTOUPDATE_NGINX_TARGET AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS AUTOUPDATE_DELAY_HOURS=$AUTOUPDATE_DELAY_HOURS AUTOUPDATE_CMUPDATE=$AUTOUPDATE_CMUPDATE"
  echo "Notify:   AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY email=${AUTOUPDATE_EMAIL:-none} pushover=$([[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] && echo on || echo off)"
  if [[ -f "$AU_CRON_FILE" ]]; then
    echo "Cron:     $AU_CRON_FILE: $(grep -v '^#' "$AU_CRON_FILE" | grep autoupdate.sh)"
  else
    echo "Cron:     $AU_CRON_FILE not present$(au_enabled && echo ' (run: autoupdate.sh ensure)')"
  fi
  for comp in nginx php; do
    [[ -f "$AU_STATE_DIR/last-$comp" ]] && echo "Last $comp: $(cat "$AU_STATE_DIR/last-$comp")"
    [[ -f "$AU_STATE_DIR/failed-$comp" ]] && echo "Held $comp: $(cut -d'|' -f1 "$AU_STATE_DIR/failed-$comp") failed at code $(cut -d'|' -f2 "$AU_STATE_DIR/failed-$comp")"
  done
  [[ -f "$AU_LOG" ]] && { echo "Recent log ($AU_LOG):"; tail -n 8 "$AU_LOG"; }
}

# one line for the dmotd login banner, or nothing
au_motd() {
  local comp fkey fhead failed parts=
  AU_ENSURE_BY=dmotd au_ensure
  au_enabled || return 0
  if [[ -d /run/systemd/system ]] && ! systemctl is-active --quiet crond; then
    echo "warn|Auto-update: crond is not running, scheduled updates cannot run - systemctl start crond"
    return 0
  fi
  for comp in nginx php; do
    if [[ -f "$AU_STATE_DIR/failed-$comp" ]]; then
      IFS='|' read -r fkey fhead failed < "$AU_STATE_DIR/failed-$comp"
      # "php rebuild <epoch>" -> "php rebuild", "nginx 1.31.8" stays
      [[ "$fkey" =~ ^(.*rebuild)\ [0-9]+$ ]] && fkey=${BASH_REMATCH[1]}
      echo "warn|Auto-update: ${fkey} FAILED $(date -d "@$failed" '+%Y-%m-%d %H:%M' 2>/dev/null) - run: bash ${CM_INSTALLDIR}/tools/autoupdate.sh status"
      return 0
    fi
  done
  [[ "$AUTOUPDATE_NGINX" = [yY] ]] && parts="nginx"
  [[ "$AUTOUPDATE_PHP" = [yY] ]] && parts="${parts:+$parts + }PHP-FPM"
  echo "info|Auto-update: $parts on, hours $AUTOUPDATE_HOURS"
}

###########################################################
au_main() {
  AU_CMD=$1
  [[ "$(id -u)" -eq 0 ]] && mkdir -p "$AU_STATE_DIR"
  case "$1" in
    cron)
      AU_ENSURE_BY=cron au_ensure
      au_run cron
      ;;
    run)
      au_run run
      ;;
    check)
      au_check
      ;;
    status)
      au_status
      ;;
    ensure)
      AU_ENSURE_BY=${2:-manual} au_ensure
      ;;
    motd)
      au_motd
      ;;
    notify-test)
      au_notify ok "auto-update test notification" "Test notification from $(basename "$0"): AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY, email=${AUTOUPDATE_EMAIL:-none}."
      au_status | grep -E '^(Notify|Recent)' ; tail -n 3 "$AU_LOG"
      ;;
    reset)
      rm -f "$AU_STATE_DIR"/failed-* "$AU_STATE_DIR"/seen-* "$AU_STATE_DIR/cmupdate.stamp"
      au_say "reset: cleared failed update holds, delay timers and the cmupdate stamp"
      ;;
    *)
      sed -n '2,24p' "$0" | sed 's/^# \{0,1\}//'
      ;;
  esac
}

# parse everything before running, cmupdate can replace this file mid-run
au_main "$@"; exit $?
