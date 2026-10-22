#!/bin/bash
###########################################################
# Centmin Mod unattended nginx and PHP-FPM updates
#
# WHAT IT DOES
# Keeps nginx and/or PHP-FPM updated with nobody logged in, using the same
# routines as centmin.sh menu option 4 (nginx) and 5 (PHP-FPM), run as
# `centmin-cli.sh nginxupgrade|phpupgrade <version>`. Off by default. Opt in
# via the persistent config /etc/centminmod/custom_config.inc:
#
#   AUTOUPDATE_NGINX='y'
#   AUTOUPDATE_PHP='y'
#
# then see what it would do: bash tools/autoupdate.sh check
# It needs the git checkout at /usr/local/src/centminmod that cmupdate keeps.
#
# WHAT IT INSTALLS
# - nginx: Centmin Mod's NGINX_VERSION pin (from centmin.sh after cmupdate,
#   or custom_config.inc if set there), or FREENGINX_VERSION when
#   FREENGINX_INSTALL='y'. AUTOUPDATE_NGINX_TARGET='latest' uses the newest
#   nginx.org/freenginx.org release instead. Only nginx.org and freenginx
#   builds are updated; Angie and other builds are skipped.
# - PHP-FPM: the newest php.net release of the installed major.minor branch,
#   e.g. 8.4.26 -> 8.4.27. It never moves PHP to another branch.
# - Never downgrades. A new version waits AUTOUPDATE_DELAY_HOURS after it is
#   first seen; php.net releases tagged security and rebuilds go ahead at once.
# - Same-version rebuilds listed in tools/autoupdate-rebuilds.txt, which
#   Centmin Mod adds when patches change without a new upstream release,
#   e.g. security backports for end-of-life PHP 7.2-8.1.
#
# HOW A RUN WORKS
# An hourly /etc/cron.d/centminmod-autoupdate entry runs `autoupdate.sh cron`.
# Outside AUTOUPDATE_HOURS it only repairs that entry and looks for a stuck
# run. Inside the window it:
#  1. takes a lock; centmin.sh and cmupdate refuse to start until it ends
#  2. runs cmupdate (AUTOUPDATE_CMUPDATE) so patches and version pins are
#     current, then carries on with the updated copy of this script. If
#     cmupdate fails or runs over 30 minutes the run uses the current code
#  3. for nginx, then PHP-FPM, works out the target and waits for a later
#     run while centmin.sh, cmupdate or yum/dnf/rpm is running, the load or
#     free disk is outside AUTOUPDATE_MAXLOAD / AUTOUPDATE_MINDISK_MB,
#     nginx -t or (for PHP) php-fpm -t fails, the service is stopped, or
#     ENABLE_MENU='y' would make centmin-cli.sh interactive
#  4. builds through menu 4/5, niced on systemd hosts (AUTOUPDATE_NICE)
#  5. checks the result: the new version is installed and running on the new
#     binary, config tests pass, no PHP extension went missing from php -m,
#     no new PHP startup errors, and every AUTOUPDATE_CHECK_URLS entry that
#     answered below 500 before still does. If a new nginx binary fails a
#     check, menu 4's snapshot of the previous build is restored and nginx
#     restarted on it; the alert says whether that worked. PHP has no
#     automatic rollback (menu 5 also updates shared libraries), so a PHP
#     failure needs a look.
#  6. notifies by Pushover and/or email (AUTOUPDATE_NOTIFY)
#
# SAFETY NETS
# - A failed update is retried only after the Centmin Mod code changes and 24
#   hours pass, at most 3 attempts in all; `reset` or `run` retries now. With
#   no newer update pending, the failure clears itself once nginx/php-fpm was
#   rebuilt since (e.g. by menu 4/5) and is healthy; a service left down or
#   on its old binary is rebuilt on the same retry schedule; a healthy service
#   still on the failed build stays held until `reset`.
# - A run interrupted mid-build (reboot, OOM, kill) is held like a failure.
# - One alert each when an update stays blocked or skipped for 72 hours, when
#   a run holds the lock for AUTOUPDATE_STUCK_HOURS, or after an interrupted run.
# - Deleting the cron entry does not turn auto-updates off: root/sudo dmotd
#   logins, centmin.sh, cmupdate and the check-for-updates.sh cron put it
#   back, log it and notify (unless AUTOUPDATE_NOTIFY is failures or none).
#   Set AUTOUPDATE_NGINX and AUTOUPDATE_PHP to 'n' to turn it off.
# - AUTOUPDATE_PHP_HOLD keeps a PHP version, e.g. after a deliberate
#   downgrade. For nginx, NGINX_VERSION in custom_config.inc pins the
#   version, but listed same-version rebuilds still run; AUTOUPDATE_NGINX='n'
#   stops all nginx changes.
# - y/n settings also accept yes/no, on/off, true/false and 1/0, and an empty
#   value means the default. Invalid setting values fall back to the default,
#   and check, status, the log and the dmotd login banner say so.
#
# COMMANDS   bash /usr/local/src/centminmod/tools/autoupdate.sh <command>
#   check        what a run would do now and why; changes no update state
#                (like centmin.sh it converts CRLF line endings in
#                custom_config.inc)
#   status       settings, cron entry, last results, held failures, recent log
#   run          update now, ignoring AUTOUPDATE_HOURS, the delay and
#                failed-update holds (AUTOUPDATE_PHP_HOLD still applies)
#   ensure       add, repair or remove the cron entry to match the settings
#   notify-test  send a test Pushover/email notification
#   reset        forget failed and interrupted update holds, blocked-update
#                timers and the cmupdate stamp; new-version delays are kept
#   help         show this text
#   (cron, motd and alertnote are used by the cron entry, the dmotd login
#   banner and the update-available alerts)
#
# FILES
#   /etc/cron.d/centminmod-autoupdate   the hourly cron entry (managed)
#   /var/log/centminmod-autoupdate.log  what each run did (1MB, then one .1)
#   /root/centminlogs/autoupdate_*.log  per-update log tail, kept 30 days; it
#                                       names the full centminmod_*_upgrade.log
#   /var/lib/centminmod/autoupdate/     state: last results, failures, timers
#   tools/autoupdate-rebuilds.txt       same-version rebuild list
#
# RELATED SETTINGS IT USES (also set in custom_config.inc)
#   PUSH_API_TOKEN, PUSH_USER_KEY       Pushover app token and user key
#   EMAILNOTIFY_SES='y' with EMAILNOTIFY_SES_SMTP_SERVER, _SMTP_PORT,
#     _SMTP_USERNAME, _SMTP_PASSWORD, _FROM_EMAIL
#                                       SMTP login for email: Amazon SES, a
#                                       Gmail app password or any SMTP server
#   NGINX_VERSION, FREENGINX_INSTALL, FREENGINX_VERSION   the nginx target
#   NGINX_UPDATEMAINTENANCE, PHP_UPDATEMAINTENANCE        'y' serves a 503
#                                       maintenance page for the whole menu 4/5
#                                       run, so sites are down for the build;
#                                       the default 'n' suits unattended updates
#   FORCE_IPVFOUR                       IPv4 for version checks and notifications
###########################################################
# SETTINGS: defaults; override any of them in /etc/centminmod/custom_config.inc
#
# What to update
AUTOUPDATE_NGINX='n'              # 'y' = keep nginx updated
AUTOUPDATE_PHP='n'                # 'y' = keep PHP-FPM updated within its installed major.minor
AUTOUPDATE_PHP_HOLD=''            # x.y.z, e.g. 8.4.26: while that PHP version is installed, leave
                                  # PHP alone (no upgrade, rebuild or repair, also for `run`);
                                  # resumes once another version is installed or this is removed.
                                  # A value that is not a version holds PHP and is reported
AUTOUPDATE_NGINX_TARGET='centmin' # 'centmin' = Centmin Mod's NGINX_VERSION pin,
                                  # 'latest' = newest nginx.org (or freenginx.org) release
#
# When
AUTOUPDATE_HOURS='2-5'            # server local hours an update may start in: 2-5, 22-3, 3 or any;
                                  # both ends included, minutes such as 2:30 are ignored
AUTOUPDATE_DELAY_HOURS='24'       # hours to wait after a new version is first seen (inside
                                  # AUTOUPDATE_HOURS); PHP security releases don't wait
AUTOUPDATE_CRON_MINUTE=''         # 0-59, minute of the hourly cron run; default derived from the
                                  # hostname so servers don't all update at the same minute
AUTOUPDATE_CMUPDATE='y'           # run cmupdate before checking, so patches and pins are current
#
# Safety limits
AUTOUPDATE_MAXLOAD=''             # wait while the 5 minute load average is above this;
                                  # default = CPU thread count
AUTOUPDATE_MINDISK_MB='3072'      # free MB needed on the /svr-setup and /usr/local filesystems
AUTOUPDATE_CHECK_URLS='http://127.0.0.1/' # space separated URLs that must keep answering below
                                  # 500 after an update; add a site's PHP page to cover PHP-FPM
AUTOUPDATE_NICE='10'              # CPU nice level for the build (systemd hosts only)
AUTOUPDATE_STUCK_HOURS='4'        # alert once when a run holds the lock this long
#
# Notifications
AUTOUPDATE_NOTIFY='all'           # all (successes and notices too), failures or none
AUTOUPDATE_PUSHOVER='y'           # use Pushover when PUSH_API_TOKEN and PUSH_USER_KEY are set
AUTOUPDATE_EMAIL=''               # space separated recipients. Sent through the EMAILNOTIFY_SES_*
                                  # SMTP login when EMAILNOTIFY_SES='y', else the local mail or
                                  # sendmail command, which Gmail and other large providers
                                  # reject unless the server has SPF/DKIM or a relay host
AUTOUPDATE_EMAIL_FROM=''          # sender address; default EMAILNOTIFY_SES_FROM_EMAIL for SMTP
# end of settings
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
# also read by inc/cpcheck.inc and tools/cmupdate.sh
AU_LOCK='/var/run/centminmod-autoupdate.lock'
AU_REBUILDS="${CM_INSTALLDIR}/tools/autoupdate-rebuilds.txt"
AU_NGINX_BIN='/usr/local/sbin/nginx'
AU_PHPFPM_BIN='/usr/local/sbin/php-fpm'
AU_MAX_ATTEMPTS=3

NGINX_VERSION=$(sed -n "s/^NGINX_VERSION='\([^']*\)'.*/\1/p" "${CM_INSTALLDIR}/centmin.sh" 2>/dev/null | head -n1)
FREENGINX_VERSION=$(sed -n "s/^FREENGINX_VERSION='\([^']*\)'.*/\1/p" "${CM_INSTALLDIR}/centmin.sh" 2>/dev/null | head -n1)
FREENGINX_INSTALL='n'
NGINXBACKUPDIR='/usr/local/nginxbackup'
ENABLE_MENU='n'

# CRLF line endings would turn 'y' into 'y\r': convert like centmin.sh and dmotd do
for au_conf in "${CONFIGSCANBASE}/custom_config.inc" "${CONFIGSCANBASE}/pushover.ini"; do
  [ -f "$au_conf" ] || continue
  if grep -q $'\r' "$au_conf" 2>/dev/null; then
    if [ -x /usr/bin/dos2unix ]; then
      dos2unix -q "$au_conf"
      source "$au_conf"
    else
      eval "$(tr -d '\r' < "$au_conf")"
    fi
  else
    source "$au_conf"
  fi
done
unset au_conf
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi
AU_HOST=$(hostname)
AU_SYSTEMD=n
[[ -d /run/systemd/system ]] && AU_SYSTEMD=y

# Fall back to the default for any unusable setting, and say so in check,
# status, the log and the dmotd banner instead of silently never updating.
au_validate_config() {
  local v name def
  AU_CONFIG_WARN=''
  for name in AUTOUPDATE_NGINX:n AUTOUPDATE_PHP:n AUTOUPDATE_CMUPDATE:y AUTOUPDATE_PUSHOVER:y; do
    def=${name#*:} name=${name%:*}
    case "$(printf '%s' "${!name}" | tr -d '[:space:]' | tr '[:upper:]' '[:lower:]')" in
      y|yes|on|true|1) printf -v "$name" '%s' y ;;
      n|no|off|false|0) printf -v "$name" '%s' n ;;
      '') printf -v "$name" '%s' "$def" ;;
      *) AU_CONFIG_WARN="$AU_CONFIG_WARN $name='${!name}' is not y or n, using $def;"
         printf -v "$name" '%s' "$def" ;;
    esac
  done
  v=$(printf '%s' "$AUTOUPDATE_HOURS" | tr -d '[:space:]' | sed -E 's/:[0-9]{2}//g')
  [[ "$v" =~ ^[0-9]{1,2}$ ]] && v="$v-$v"
  if [[ "$v" = any ]] || { [[ "$v" =~ ^([0-9]{1,2})-([0-9]{1,2})$ ]] && (( 10#${BASH_REMATCH[1]} < 24 && 10#${BASH_REMATCH[2]} < 24 )); }; then
    AUTOUPDATE_HOURS=$v
  else
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_HOURS='$AUTOUPDATE_HOURS' is invalid, using 2-5;"
    AUTOUPDATE_HOURS='2-5'
  fi
  for name in AUTOUPDATE_DELAY_HOURS:24 AUTOUPDATE_MINDISK_MB:3072 AUTOUPDATE_STUCK_HOURS:4; do
    def=${name#*:} name=${name%:*}
    if [[ "${!name}" =~ ^[0-9]+$ ]]; then
      # leading zeros would make bash read 08 as bad octal and 03072 as 1594
      printf -v "$name" '%s' "$((10#${!name}))"
      continue
    fi
    AU_CONFIG_WARN="$AU_CONFIG_WARN $name='${!name}' is not a number, using $def;"
    printf -v "$name" '%s' "$def"
  done
  if [[ "$AUTOUPDATE_NICE" =~ ^(-?)([0-9]+)$ ]]; then
    AUTOUPDATE_NICE="${BASH_REMATCH[1]}$((10#${BASH_REMATCH[2]}))"
  else
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_NICE='$AUTOUPDATE_NICE' is not a number, using 10;"
    AUTOUPDATE_NICE=10
  fi
  if [[ -n "$AUTOUPDATE_MAXLOAD" ]] && ! { [[ "$AUTOUPDATE_MAXLOAD" =~ ^[0-9]+(\.[0-9]+)?$ ]] && awk -v m="$AUTOUPDATE_MAXLOAD" 'BEGIN { exit !(m > 0) }'; }; then
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_MAXLOAD='$AUTOUPDATE_MAXLOAD' is not a number above 0, using the CPU thread count;"
    AUTOUPDATE_MAXLOAD=''
  fi
  if [[ -n "$AUTOUPDATE_CRON_MINUTE" ]] && ! { [[ "$AUTOUPDATE_CRON_MINUTE" =~ ^[0-9]{1,2}$ ]] && (( 10#$AUTOUPDATE_CRON_MINUTE < 60 )); }; then
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_CRON_MINUTE='$AUTOUPDATE_CRON_MINUTE' is not 0-59, using the hostname-derived minute;"
    AUTOUPDATE_CRON_MINUTE=''
  fi
  AU_PHP_HOLD_INVALID=n
  AUTOUPDATE_PHP_HOLD=$(printf '%s' "$AUTOUPDATE_PHP_HOLD" | tr -d '[:space:]')
  if [[ -n "$AUTOUPDATE_PHP_HOLD" ]] && ! [[ "$AUTOUPDATE_PHP_HOLD" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    # a mistyped hold must not let PHP update: hold whatever is installed
    AU_PHP_HOLD_INVALID=y
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_PHP_HOLD='$AUTOUPDATE_PHP_HOLD' is not a version like 8.4.26, holding PHP updates until it is fixed;"
  fi
  case "$AUTOUPDATE_NOTIFY" in
    all|failures|none) ;;
    *) AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_NOTIFY='$AUTOUPDATE_NOTIFY' is not all, failures or none, using all;"
       AUTOUPDATE_NOTIFY=all ;;
  esac
  case "$AUTOUPDATE_NGINX_TARGET" in
    centmin|latest) ;;
    *) AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_NGINX_TARGET='$AUTOUPDATE_NGINX_TARGET' is not centmin or latest, using centmin;"
       AUTOUPDATE_NGINX_TARGET=centmin ;;
  esac
  AU_CONFIG_WARN=${AU_CONFIG_WARN# }
}
au_validate_config
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

# --git-dir, not git -C: EL7 ships git 1.8.3 and EL6 git 1.7, -C needs 1.8.5
au_git() {
  git --git-dir="${CM_INSTALLDIR}/.git" --work-tree="$CM_INSTALLDIR" "$@" 2>/dev/null
}

au_head() {
  au_git rev-parse --short HEAD
}

# full hash for failure records: short hashes can grow a character later
au_head_full() {
  au_git rev-parse HEAD
}

au_head_time() {
  au_git log -1 --format=%ct
}

au_date() {
  date -d "@$1" '+%Y-%m-%d %H:%M' 2>/dev/null
}

# true while another process holds the autoupdate lock; a lock file that is
# not root's 0600 file was not made by autoupdate and is ignored
au_lock_held() {
  [[ -f "$AU_LOCK" && ! -L "$AU_LOCK" && "$(stat -c '%u %a' "$AU_LOCK" 2>/dev/null)" = '0 600' ]] && ! flock -n "$AU_LOCK" -c true 2>/dev/null
}

au_service_active() {
  if [[ "$AU_SYSTEMD" = y ]]; then
    systemctl is-active --quiet "$1"
  elif [[ "$1" = nginx ]]; then
    pgrep -U 0 -u 0 -x nginx >/dev/null 2>&1
  else
    pgrep -U 0 -u 0 -f 'php-fpm: master process' >/dev/null 2>&1
  fi
}

# fd 9 is the lock: daemons that SysV init scripts start from this process
# tree must not inherit it, or they hold the lock after the run ends
au_service_restart() {
  if [[ "$AU_SYSTEMD" = y ]]; then
    systemctl restart "$1" 9>&-
  else
    service "$1" restart 9>&-
  fi
}

# pid of the current nginx or php-fpm master, from its pid file
au_master_pid() {
  local pidfile pid
  if [[ "$1" = nginx ]]; then
    pidfile=/usr/local/nginx/logs/nginx.pid
  else
    pidfile=$(awk -F= '/^[[:space:]]*pid[[:space:]]*=/ {gsub(/[[:space:]]/, "", $2); print $2; exit}' /usr/local/etc/php-fpm.conf 2>/dev/null)
  fi
  pid=$(cat "${pidfile:-/nonexistent}" 2>/dev/null)
  if [[ "$pid" =~ ^[0-9]+$ ]] && kill -0 "$pid" 2>/dev/null; then
    echo "$pid"
  else
    pgrep -U 0 -u 0 -o -f "^$1: master process" 2>/dev/null
  fi
}

# true when the master runs something other than the installed binary: a
# deleted file, or nginx.old after menu 4's make install renamed it
au_stale_master() {
  local pid exe
  pid=$(au_master_pid "$1")
  [[ -n "$pid" ]] || return 1
  exe=$(readlink "/proc/$pid/exe" 2>/dev/null)
  [[ -n "$exe" && "$exe" != "$2" ]]
}

au_healthy() {
  if [[ "$1" = nginx ]]; then
    au_service_active nginx && ! au_stale_master nginx "$AU_NGINX_BIN" && nginx -t >/dev/null 2>&1
  else
    au_service_active php-fpm && ! au_stale_master php-fpm "$AU_PHPFPM_BIN" && php-fpm -t >/dev/null 2>&1
  fi
}

# official nginx or freenginx only: Angie and other forks print other names
au_nginx_installed() {
  nginx -v 2>&1 | sed -n 's|^nginx version: \(free\)\{0,1\}nginx/\([0-9]*\.[0-9]*\.[0-9]*\).*|\2|p' | head -n1
}

au_php_installed() {
  php-config --version 2>/dev/null
}

au_cron_minute() {
  if [[ -n "$AUTOUPDATE_CRON_MINUTE" ]]; then
    echo "$((10#$AUTOUPDATE_CRON_MINUTE))"
  else
    # spread servers across the hour instead of all hitting nginx.org/php.net at once
    echo $(( $(printf '%s' "$AU_HOST" | cksum | cut -d' ' -f1) % 60 ))
  fi
}

# AUTOUPDATE_HOURS is validated by au_validate_config
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

# escape a value for a double-quoted curl config file entry
au_curlq() {
  local v=${1//\\/\\\\}
  printf '%s' "${v//\"/\\\"}"
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
  local want had=n tmp msg
  [[ "$(id -u)" -eq 0 ]] || return 0
  mkdir -p "$AU_STATE_DIR"
  if au_enabled; then
    want=$(au_cron_content)
    [[ -f "$AU_CRON_FILE" && "$(cat "$AU_CRON_FILE" 2>/dev/null)" = "$want" ]] && return 0
    [[ -f "$AU_CRON_FILE" ]] && had=y
    # a dot file in /etc/cron.d is not read by cron; unique per process
    tmp=$(mktemp "/etc/cron.d/.centminmod-autoupdate.XXXXXX") || { au_log "ensure: could not write in /etc/cron.d"; return 1; }
    printf '%s\n' "$want" > "$tmp" && chmod 0644 "$tmp" && mv -f "$tmp" "$AU_CRON_FILE" || {
      rm -f "$tmp"
      au_log "ensure: could not write $AU_CRON_FILE"
      return 1
    }
    if [[ -f "$AU_STATE_DIR/scheduler-installed" ]]; then
      if [[ "$had" = y ]]; then
        au_log "ensure: $AU_CRON_FILE differed from the expected entry, rewritten (${AU_ENSURE_BY:-manual})"
      else
        au_log "ensure: $AU_CRON_FILE was missing, re-added (${AU_ENSURE_BY:-manual})"
        msg="$AU_CRON_FILE was missing while AUTOUPDATE_NGINX='$AUTOUPDATE_NGINX' AUTOUPDATE_PHP='$AUTOUPDATE_PHP', so it was re-added (found by ${AU_ENSURE_BY:-manual}). To stop auto-updates set both to 'n' in ${CONFIGSCANBASE}/custom_config.inc."
        if [[ "$AU_ENSURE_BY" = dmotd ]]; then
          # don't hold up the SSH login banner on a slow Pushover/SMTP server
          ( au_notify info "auto-update cron entry re-added" "$msg" ) </dev/null >/dev/null 2>&1 &
        else
          au_notify info "auto-update cron entry re-added" "$msg"
        fi
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

# Credentials go to curl in a config file in a 0700 mktemp directory, never
# on its command line where ps shows them.
# returns 0 when sent, 1 when sending failed, 2 when Pushover is not set up
au_pushover() {
  local priority=$1 title=$2 message=$3 response dir
  [[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] || return 2
  dir=$(mktemp -d) || return 1
  ( umask 077; printf 'form-string = "token=%s"\nform-string = "user=%s"\n' "$(au_curlq "$PUSH_API_TOKEN")" "$(au_curlq "$PUSH_USER_KEY")" > "$dir/cfg" )
  response=$(curl -${ipv_forceopt}s --connect-timeout 10 --max-time 30 -K "$dir/cfg" \
    --form-string "title=${title:0:250}" \
    --form-string "message=${message:0:1024}" \
    --form-string "priority=${priority}" \
    https://api.pushover.net/1/messages.json)
  rm -rf "$dir"
  if [[ "$response" = *'"status":1'* ]]; then
    au_log "notify: pushover sent: $title"
  else
    au_log "notify: pushover failed: ${response:-no response}"
    return 1
  fi
}

# Authenticated SMTP using the EMAILNOTIFY_SES_* settings tools/emailnotify.sh
# uses (any SMTP server with a login: Amazon SES, a Gmail app password, ...).
# Unauthenticated mail from a server without SPF/DKIM is rejected by Gmail
# and most large providers.
au_smtp() {
  local subject=$1 body=$2 dir from url rc rcpt args=()
  if ! curl -V 2>/dev/null | grep -qw smtp; then
    au_log "notify: this curl has no SMTP support (curl -V), email not sent"
    return 1
  fi
  from=${AUTOUPDATE_EMAIL_FROM:-$EMAILNOTIFY_SES_FROM_EMAIL}
  [[ "$from" = *@* ]] || from=$EMAILNOTIFY_SES_SMTP_USERNAME
  if [[ "$from" != *@* ]]; then
    au_log "notify: set AUTOUPDATE_EMAIL_FROM or EMAILNOTIFY_SES_FROM_EMAIL to a sender address, email not sent"
    return 1
  fi
  dir=$(mktemp -d) || return 1
  {
    echo "From: $from"
    echo "To: ${AUTOUPDATE_EMAIL// /, }"
    echo "Subject: $subject"
    echo "Date: $(date -R)"
    echo
    printf '%s\n' "$body"
  } > "$dir/msg"
  ( umask 077; printf 'user = "%s:%s"\n' "$(au_curlq "$EMAILNOTIFY_SES_SMTP_USERNAME")" "$(au_curlq "$EMAILNOTIFY_SES_SMTP_PASSWORD")" > "$dir/cfg" )
  if [[ "$EMAILNOTIFY_SES_SMTP_PORT" = 465 ]]; then
    url="smtps://${EMAILNOTIFY_SES_SMTP_SERVER}:465"
  else
    url="smtp://${EMAILNOTIFY_SES_SMTP_SERVER}:${EMAILNOTIFY_SES_SMTP_PORT:-587}"
    args+=(--ssl-reqd)
  fi
  for rcpt in $AUTOUPDATE_EMAIL; do
    args+=(--mail-rcpt "$rcpt")
  done
  curl -${ipv_forceopt}sS --crlf --connect-timeout 15 --max-time 60 "${args[@]}" --mail-from "$from" -K "$dir/cfg" -T "$dir/msg" "$url" 2>"$dir/err"
  rc=$?
  [[ "$rc" -eq 0 ]] || au_log "notify: smtp $url failed (curl exit $rc): $(head -c 300 "$dir/err" | tr "\n" " ")"
  rm -rf "$dir"
  return "$rc"
}

# returns 0 when handed over, 1 when that failed, 2 when no recipient is set
au_email() {
  local subject=$1 body=$2 rc via='the local MTA'
  [[ -n "$AUTOUPDATE_EMAIL" ]] || return 2
  if [[ "$EMAILNOTIFY_SES" = [yY] && -n "$EMAILNOTIFY_SES_SMTP_SERVER" && -n "$EMAILNOTIFY_SES_SMTP_USERNAME" ]]; then
    via="smtp $EMAILNOTIFY_SES_SMTP_SERVER"
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
    au_log "notify: email handed to $via for $AUTOUPDATE_EMAIL: $subject"
  else
    au_log "notify: email to $AUTOUPDATE_EMAIL via $via failed (exit $rc)"
    return 1
  fi
}

# au_notify ok|fail|info title message [logfile]
# Returns 1 only when every configured channel failed, so a one-time alert
# can be tried again later; filtered or unconfigured counts as done.
au_notify() {
  local level=$1 title=$2 message=$3 logfile=$4 priority=0 body prc erc
  if [[ "$AU_NOTIFY_FORCE" != y ]]; then
    case "$AUTOUPDATE_NOTIFY" in
      none) return 0 ;;
      failures) [[ "$level" = fail ]] || return 0 ;;
    esac
  fi
  case "$level" in
    fail) priority=1 ;;
    info) priority=-1 ;;
  esac
  au_pushover "$priority" "$title $AU_HOST" "$message"
  prc=$?
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
  erc=$?
  [[ "$prc" -eq 0 || "$erc" -eq 0 || ( "$prc" -eq 2 && "$erc" -eq 2 ) ]]
}

###########################################################
# what to update

# php.net release data for a major.minor branch: sets AU_REL_VER, AU_REL_SECURITY
au_php_release() {
  local json
  AU_REL_VER='' AU_REL_SECURITY=n
  json=$(curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 "https://www.php.net/releases/index.php?json&version=$1") || return 1
  AU_REL_VER=$(printf '%s' "$json" | grep -oE '"version"[[:space:]]*:[[:space:]]*"[0-9]+\.[0-9]+\.[0-9]+"' | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
  [[ "$json" =~ \"tags\"[[:space:]]*:[[:space:]]*\[[^]]*\"security\" ]] && AU_REL_SECURITY=y
  au_is_ver "$AU_REL_VER" && [[ "${AU_REL_VER%.*}" = "$1" ]]
}

au_nginx_latest() {
  if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 https://freenginx.org/en/download.html | grep -Eo 'freenginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1
  else
    curl -${ipv_forceopt}sL --connect-timeout 10 --max-time 30 https://nginx.org/download/ | grep -Eo 'nginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1
  fi
}

# When autoupdate built the installed binary, the time of the Centmin Mod
# commit it was built from: a build from code older than a backport does not
# count as having it. Other builds (menu 4/5 by hand) use the binary mtime.
au_build_time() {
  local comp=$1 bin=$2 mt rec_mt rec_ct
  mt=$(stat -c %Y "$bin")
  read -r rec_mt rec_ct 2>/dev/null < "$AU_STATE_DIR/built-$comp"
  if [[ "$rec_mt" = "$mt" && "$rec_ct" =~ ^[0-9]+$ ]]; then
    echo "$rec_ct"
  else
    echo "$mt"
  fi
}

# Newest entry in tools/autoupdate-rebuilds.txt for $1 (nginx or php-X.Y)
# that is newer than the build of binary $2 (component $3): prints
# "<epoch> <reason>". Maintainers add an entry when a Centmin Mod patch needs
# a same-version rebuild, e.g. security backports for end-of-life PHP.
au_rebuild_due() {
  local rcomp=$1 bin=$2 comp=$3 c when reason ts built best=0 bestwhy=''
  [[ -f "$AU_REBUILDS" && -f "$bin" ]] || return 1
  built=$(au_build_time "$comp" "$bin")
  while read -r c when reason; do
    [[ -z "$c" || "$c" = \#* ]] && continue
    [[ "$c" = "$rcomp" || ( "$c" = php && "$rcomp" = php-* ) ]] || continue
    when=${when%Z}
    ts=$(date -u -d "${when/T/ } UTC" +%s 2>/dev/null) || continue
    if (( ts > built && ts > best )); then
      best=$ts bestwhy="$reason"
    fi
  done < "$AU_REBUILDS"
  (( best > 0 )) || return 1
  # one rebuild per entry, even when an entry is dated after its commit
  [[ "$(cat "$AU_STATE_DIR/rebuilt-$rcomp" 2>/dev/null)" -ge "$best" ]] 2>/dev/null && return 1
  echo "$best $bestwhy"
}

# After a failed update with nothing new pending: clear the failure only once
# the binary was rebuilt after it (e.g. menu 4/5 by hand) and the service is
# healthy. The build that failed may look healthy here while the problem its
# checks found (a missing PHP module, a failing URL) is still there.
# Prints clear, keep or unhealthy.
au_failure_review() {
  local comp=$1 ftime=$2 bin=$AU_NGINX_BIN
  [[ "$comp" = php ]] && bin=$AU_PHPFPM_BIN
  if ! au_healthy "$comp"; then
    echo unhealthy
  elif [[ "$(stat -c %Y "$bin" 2>/dev/null || echo 0)" -gt "${ftime:-0}" ]]; then
    echo clear
  else
    echo keep
  fi
}

# Decide what to do for component $1 (nginx|php). Sets AU_ACTION
# (upgrade|rebuild|none|skip|wait|held), AU_INSTALLED, AU_TARGET, AU_WHY,
# AU_KEY, AU_EPOCH and AU_REBUILD_COMP. Changes no state when AU_DRYRUN=y.
au_plan() {
  local comp=$1 due branch seen age fkey fhead ftime fcount review
  AU_ACTION=none AU_INSTALLED='' AU_TARGET='' AU_WHY='' AU_KEY='' AU_EPOCH='' AU_REL_SECURITY=n AU_REBUILD_COMP='' AU_REPAIR=n
  [[ -f "$AU_STATE_DIR/failed-$comp" ]] && IFS='|' read -r fkey fhead ftime fcount < "$AU_STATE_DIR/failed-$comp"
  if [[ "$comp" = nginx ]]; then
    if ! command -v nginx >/dev/null 2>&1; then
      AU_ACTION=skip AU_WHY='nginx is not installed'; return
    fi
    AU_INSTALLED=$(au_nginx_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed '$(nginx -v 2>&1 | head -n1)' is not an nginx.org or freenginx release build"; return
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
    AU_REBUILD_COMP=nginx
    due=$(au_rebuild_due nginx "$AU_NGINX_BIN" nginx)
  else
    if ! command -v php-config >/dev/null 2>&1; then
      AU_ACTION=skip AU_WHY='PHP is not installed'; return
    fi
    AU_INSTALLED=$(au_php_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed PHP version '$AU_INSTALLED' is not a release build"; return
    fi
    if [[ "$AU_PHP_HOLD_INVALID" = y || ( -n "$AUTOUPDATE_PHP_HOLD" && "$AUTOUPDATE_PHP_HOLD" = "$AU_INSTALLED" ) ]]; then
      # explicit admin choice: autoupdate.sh run does not override it
      if [[ -n "$fkey" && "$(au_failure_review php "$ftime")" = clear && "$AU_DRYRUN" != y ]]; then
        rm -f "$AU_STATE_DIR/failed-php"
        au_log "php: cleared failed update $fkey, php $AU_INSTALLED was rebuilt since and is healthy"
      fi
      AU_ACTION=held AU_TARGET=$AU_INSTALLED
      AU_WHY="AUTOUPDATE_PHP_HOLD='$AUTOUPDATE_PHP_HOLD' keeps PHP $AU_INSTALLED, remove it to resume PHP updates"
      return
    fi
    branch=${AU_INSTALLED%.*}
    AU_REBUILD_COMP="php-$branch"
    due=$(au_rebuild_due "$AU_REBUILD_COMP" "$AU_PHPFPM_BIN" php)
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
    # the upgrade builds from the same code, so it also covers a due rebuild
    [[ -n "$due" ]] && AU_EPOCH=${due%% *}
  elif [[ -n "$due" ]]; then
    AU_ACTION=rebuild AU_TARGET=$AU_INSTALLED AU_EPOCH=${due%% *}
    AU_KEY="$comp rebuild $AU_EPOCH"
    AU_WHY="rebuild $AU_INSTALLED: ${due#* }"
  elif [[ -n "$fkey" ]]; then
    # nothing new pending, but the last update failed
    review=$(au_failure_review "$comp" "$ftime")
    if [[ "$review" = clear ]]; then
      AU_WHY="$AU_INSTALLED is current; the failed update $fkey was rebuilt since and $comp is healthy"
      if [[ "$AU_DRYRUN" != y ]]; then
        rm -f "$AU_STATE_DIR/failed-$comp"
        au_log "$comp: cleared failed update $fkey, $comp $AU_INSTALLED was rebuilt since and is healthy"
      fi
      return
    elif [[ "$review" = keep ]]; then
      AU_ACTION=held AU_WHY="the update $fkey failed its checks (see autoupdate.sh status) and $comp still runs that build; fix it, then run autoupdate.sh reset"
      return
    fi
    # the failed update left the service down or on an old binary: rebuild it
    AU_ACTION=rebuild AU_TARGET=$AU_INSTALLED AU_KEY=$fkey AU_REPAIR=y
    [[ "$fkey" =~ rebuild\ ([0-9]+)$ ]] && AU_EPOCH=${BASH_REMATCH[1]}
    AU_WHY="repair $AU_INSTALLED: the update $fkey failed and $comp is not healthy"
  else
    if au_vergt "$AU_INSTALLED" "$AU_TARGET"; then
      AU_WHY="installed $AU_INSTALLED is newer than target $AU_TARGET, not downgrading"
    else
      AU_WHY="$AU_INSTALLED is current"
    fi
    return
  fi
  # A failed update is retried only after the Centmin Mod code changed and a
  # day has passed, at most AU_MAX_ATTEMPTS times; reset or run override it
  if [[ "$fkey" = "$AU_KEY" && "$AU_FORCE" != y ]]; then
    [[ "$fcount" =~ ^[0-9]+$ ]] || fcount=1
    if (( fcount >= AU_MAX_ATTEMPTS )); then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $fcount times, last $(au_date "$ftime"); run autoupdate.sh reset to try again"
      return
    elif [[ "$fhead" = "$(au_head_full)" ]]; then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $(au_date "$ftime"), waiting for a Centmin Mod code update (cmupdate) or autoupdate.sh reset"
      return
    elif (( $(date +%s) - ftime < 86400 )); then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $(au_date "$ftime"), retrying 24 hours after that"
      return
    fi
  fi
  # soak time for new versions; security releases and rebuilds go ahead
  if [[ "$AU_ACTION" = upgrade && "$AU_REL_SECURITY" != y && "$AU_FORCE" != y ]]; then
    seen="$AU_STATE_DIR/seen-$comp-$AU_TARGET"
    if [[ -f "$seen" ]]; then
      age=$(( ($(date +%s) - $(stat -c %Y "$seen")) / 3600 ))
    else
      age=0
      [[ "$AU_DRYRUN" = y ]] || touch "$seen"
    fi
    if (( age < AUTOUPDATE_DELAY_HOURS )); then
      AU_ACTION=wait AU_WHY="$AU_WHY: first seen ${age}h ago, waiting until AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h"
    fi
  fi
}

###########################################################
# safety checks before starting a build: prints why not, or nothing

# A running centmin.sh, centmin-cli.sh or cmupdate, also behind wrappers such
# as sudo, nice or nohup. Viewers and editors that only name one are ignored.
# Only processes with real and effective uid 0 count: any local user (a
# compromised PHP-FPM pool user, say) can start a process with those names,
# and a setuid sudo or su waiting at a password prompt has effective uid 0.
au_cmm_running() {
  local pid arg args
  for pid in $(pgrep -U 0 -u 0 -f 'centmin(-cli)?\.sh|cmupdate'); do
    args=()
    while IFS= read -r -d '' arg; do
      args+=("$arg")
    done 2>/dev/null < "/proc/$pid/cmdline"
    [[ ${#args[@]} -gt 0 ]] || continue
    case "${args[0]##*/}" in
      vi|vim|nvim|view|nano|emacs|less|more|most|cat|tail|head|grep|egrep|rg|sed|awk|diff|cmp|git|cp|mv|scp|rsync|tar|stat|ls|file|ps|pgrep|ssh|md5sum|sha256sum) continue ;;
    esac
    for arg in "${args[@]}"; do
      case "${arg##*/}" in
        centmin.sh|centmin-cli.sh|cmupdate|cmupdate.sh) echo "${arg##*/} (pid $pid)"; return 0 ;;
      esac
    done
  done
  return 1
}

au_blocked() {
  local comp=$1 p load max avail d
  if [[ "$ENABLE_MENU" = [yY] ]]; then
    echo "ENABLE_MENU='y' in custom_config.inc makes centmin-cli.sh interactive, so it cannot run unattended"; return
  fi
  p=$(au_cmm_running) && { echo "$p is running"; return; }
  pgrep -U 0 -u 0 -x 'yum|dnf|rpm|dnf-automatic|yum-cron' >/dev/null 2>&1 && { echo "a yum/dnf/rpm process is running"; return; }
  max=${AUTOUPDATE_MAXLOAD:-$(nproc 2>/dev/null || echo 1)}
  load=$(awk '{print $2}' /proc/loadavg)
  awk -v l="$load" -v m="$max" 'BEGIN { exit !(l > m) }' && { echo "5 minute load $load is above AUTOUPDATE_MAXLOAD $max"; return; }
  for d in "$DIR_TMP" /usr/local; do
    [[ -d "$d" ]] || d=/
    avail=$(df -Pm "$d" | awk 'NR==2 {print $4}')
    [[ "$avail" -ge "$AUTOUPDATE_MINDISK_MB" ]] 2>/dev/null || { echo "only ${avail}MB free for $d, need AUTOUPDATE_MINDISK_MB=${AUTOUPDATE_MINDISK_MB}"; return; }
  done
  nginx -t >/dev/null 2>&1 || { echo "nginx -t fails with the current config, fix it first"; return; }
  if [[ "$comp" = nginx ]]; then
    # a repair rebuild is what brings a stopped service back
    au_service_active nginx || [[ "$AU_REPAIR" = y ]] || { echo "nginx is not running"; return; }
  else
    au_service_active php-fpm || [[ "$AU_REPAIR" = y ]] || { echo "php-fpm is not running"; return; }
    # menu 5 tests the config only after installing the new build
    php-fpm -t >/dev/null 2>&1 || { echo "php-fpm -t fails with the current config, fix it first"; return; }
  fi
}

# HTTP status of each AUTOUPDATE_CHECK_URLS entry: "url=code url=code"
au_url_codes() {
  local u urls
  read -ra urls <<< "$AUTOUPDATE_CHECK_URLS"
  for u in "${urls[@]}"; do
    printf '%s=%s ' "$u" "$(curl -${ipv_forceopt}sk -o /dev/null --connect-timeout 5 --max-time 20 -w '%{http_code}' "$u")"
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
# checks. Returns 0 when the previous version $3 is serving again, 1 when the
# restored files fail nginx -t (nginx not restarted), 2 when this run took no
# snapshot, 3 when the restore function could not be loaded (nothing changed),
# 4 when nginx does not serve the restored build after the restart.
au_nginx_rollback() {
  local start=$1 runlog=$2 prev=$3 snap="${NGINXBACKUPDIR}/upgrade-snapshot"
  [[ -f "$snap/nginx" && "$(stat -c %Y "$snap")" -ge "$start" ]] || return 2
  echo "autoupdate: restoring the previous nginx build from $snap" >> "$runlog"
  (
    DT=$(date +%d%m%y-%H%M%S)
    source <(sed -n '/^nginx_upgrade_restore() {/,/^}/p' "${CM_INSTALLDIR}/inc/nginx_upgrade.inc")
    declare -f nginx_upgrade_restore >/dev/null || exit 3
    nginx_snapshot_ready=y
    nginx_upgrade_restore || exit 1
  ) >> "$runlog" 2>&1 9>&-
  case $? in
    0) ;;
    3) echo "autoupdate: could not load nginx_upgrade_restore from inc/nginx_upgrade.inc" >> "$runlog"; return 3 ;;
    *) return 1 ;;
  esac
  au_service_restart nginx >> "$runlog" 2>&1
  sleep 2
  au_service_active nginx && ! au_stale_master nginx "$AU_NGINX_BIN" && [[ "$(au_nginx_installed)" = "$prev" ]] || return 4
}

# failed-<comp> holds "key|git head|time|attempts"
au_fail_record() {
  local comp=$1 key=$2 fkey fhead ftime fcount=0
  IFS='|' read -r fkey fhead ftime fcount 2>/dev/null < "$AU_STATE_DIR/failed-$comp"
  [[ "$fkey" = "$key" && "$fcount" =~ ^[0-9]+$ ]] || fcount=0
  echo "$key|$(au_head_full)|$(date +%s)|$((fcount + 1))" > "$AU_STATE_DIR/failed-$comp"
  echo $((fcount + 1))
}

au_update() {
  local comp=$1 start runlog clilog rc now problems='' urls_before mods_before mods_after missing
  local warn_before warn_after warn level title msg rolled='' menu attempt bin replaced=n rcprob=''
  start=$(date +%s)
  runlog="${CENTMINLOGDIR}/autoupdate_${comp}_$(date +%d%m%y-%H%M%S).log"
  mkdir -p "$CENTMINLOGDIR" "$AU_STATE_DIR"
  au_say "$comp: starting $AU_ACTION ($AU_WHY), log $runlog"
  echo "$comp|$AU_KEY|$start" > "$AU_STATE_DIR/inprogress"
  urls_before=$(au_url_codes)
  if [[ "$comp" = php ]]; then
    bin=$AU_PHPFPM_BIN
    mods_before=$(php -m 2>/dev/null | sort -u)
    warn_before=$(php -m 2>&1 | grep -Ei 'warning|fatal|error' | sort -u)
  else
    bin=$AU_NGINX_BIN
  fi

  # SysV init scripts start nginx/php-fpm from this process tree on hosts
  # without systemd, so they would inherit a nice level and the lock fd
  (
    cd "$CM_INSTALLDIR" || exit 1
    if [[ "$AU_SYSTEMD" = y ]] && command -v ionice >/dev/null 2>&1; then
      nice -n "$AUTOUPDATE_NICE" ionice -c2 -n7 bash ./centmin-cli.sh "${comp}upgrade" "$AU_TARGET"
    elif [[ "$AU_SYSTEMD" = y ]]; then
      nice -n "$AUTOUPDATE_NICE" bash ./centmin-cli.sh "${comp}upgrade" "$AU_TARGET"
    else
      bash ./centmin-cli.sh "${comp}upgrade" "$AU_TARGET"
    fi
  ) < /dev/null > "$runlog" 2>&1 9>&-
  rc=$?
  clilog=$(ls -t "$CENTMINLOGDIR"/centminmod_*_"${comp}"_upgrade.log 2>/dev/null | head -n1)
  [[ -n "$clilog" && "$(stat -c %Y "$clilog")" -ge "$start" ]] || clilog=''
  # what menu 4/5 reported about its own failure, if anything
  menu=$(grep -aE -A2 '(Nginx|PHP) upgrade failed \(status [0-9]+\) at stage' "$runlog" | au_strip | sed 's/^ *//' | tr '\n' ' ' | head -c 400)

  # check what is live now, whatever the exit status said
  [[ "$rc" -eq 0 ]] || rcprob="centmin-cli.sh ${comp}upgrade exited $rc;"
  [[ "$(stat -c %Y "$bin" 2>/dev/null || echo 0)" -ge "$start" ]] && replaced=y
  if [[ "$comp" = nginx ]]; then
    now=$(au_nginx_installed)
    [[ "$now" = "$AU_TARGET" ]] || problems="$problems nginx -v reports '${now:-nothing}', expected $AU_TARGET;"
    [[ "$replaced" = y ]] || problems="$problems $bin was not replaced;"
    au_service_active nginx || problems="$problems nginx is not running;"
    au_stale_master nginx "$bin" && problems="$problems the nginx master still runs the old binary;"
    nginx -t >/dev/null 2>&1 || problems="$problems nginx -t fails;"
  else
    now=$(au_php_installed)
    [[ "$now" = "$AU_TARGET" ]] || problems="$problems php-config reports '${now:-nothing}', expected $AU_TARGET;"
    [[ "$replaced" = y ]] || problems="$problems $bin was not replaced;"
    au_service_active php-fpm || problems="$problems php-fpm is not running;"
    au_stale_master php-fpm "$bin" && problems="$problems the php-fpm master still runs the old binary;"
    php-fpm -t >/dev/null 2>&1 || problems="$problems php-fpm -t fails;"
    # only warnings the update introduced
    warn_after=$(php -m 2>&1 | grep -Ei 'warning|fatal|error' | sort -u)
    warn=$(comm -13 <(printf '%s\n' "$warn_before") <(printf '%s\n' "$warn_after") | grep . | head -n3 | tr '\n' ' ')
    [[ -z "$warn" ]] || problems="$problems new PHP startup errors: $warn;"
    mods_after=$(php -m 2>/dev/null | sort -u)
    missing=$(comm -23 <(printf '%s\n' "$mods_before") <(printf '%s\n' "$mods_after") | grep -v '^\[' | tr '\n' ' ')
    [[ -z "$missing" ]] || problems="$problems PHP modules missing after the update: ${missing% };"
  fi
  if [[ -z "$problems" ]]; then
    missing=$(au_url_regressions "$urls_before")
    [[ -z "$missing" ]] || problems="URL checks started failing: $missing;"
  fi
  # menu 4 restores its snapshot when it fails before the new build is live;
  # a new build that is in place but fails a check is put back here, also
  # when menu 4 itself failed later (e.g. at its live stage)
  if [[ "$comp" = nginx && -n "$problems" && "$replaced" = y ]]; then
    au_nginx_rollback "$start" "$runlog" "$AU_INSTALLED"
    case $? in
      0)
        missing=$(au_url_regressions "$urls_before")
        if [[ -z "$missing" ]]; then
          rolled=" The previous nginx build $AU_INSTALLED was restored from ${NGINXBACKUPDIR}/upgrade-snapshot and is serving again; the URL checks pass."
        else
          rolled=" The previous nginx build $AU_INSTALLED was restored and is serving, but URL checks still fail: $missing."
        fi ;;
      2) rolled=" Menu 4 took no snapshot in this run, so nothing was rolled back." ;;
      3) rolled=" Could not load the menu 4 restore function, nothing was rolled back: the new build is still running." ;;
      4) rolled=" The previous build was restored on disk, but after a restart nginx is not serving it: check nginx now." ;;
      *) rolled=" The restored ${NGINXBACKUPDIR}/upgrade-snapshot files fail nginx -t, so nginx was not restarted: the new build is still running. Do not restart nginx until this is fixed." ;;
    esac
  fi
  [[ -n "$menu" ]] && rolled="$rolled Menu reported: $menu"
  problems=$(printf '%s %s' "$rcprob" "$problems" | sed 's/^ *//; s/; *$//')
  if [[ -z "$problems" ]]; then
    level=ok
    title="$comp $AU_TARGET $([[ "$AU_ACTION" = rebuild ]] && echo rebuilt || echo installed)"
    msg="Auto-update $AU_ACTION of $comp finished in $(( ($(date +%s) - start) / 60 )) min: $AU_WHY."
    rm -f "$AU_STATE_DIR/failed-$comp"
    [[ -n "$AU_EPOCH" ]] && echo "$AU_EPOCH" > "$AU_STATE_DIR/rebuilt-$AU_REBUILD_COMP"
    echo "$(stat -c %Y "$bin") $(au_head_time)" > "$AU_STATE_DIR/built-$comp"
  else
    level=fail
    title="$comp $AU_TARGET auto-update FAILED"
    attempt=$(au_fail_record "$comp" "$AU_KEY")
    msg="Auto-update $AU_ACTION of $comp ($AU_WHY) failed: ${problems}.${rolled} nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO). Attempt $attempt of $AU_MAX_ATTEMPTS: tried again after a Centmin Mod code update and 24 hours, or right away after: autoupdate.sh reset"
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

# Something that keeps an update from happening (low disk, broken config,
# a stopped service, php.net unreachable) would otherwise go unnoticed:
# notify once after it has lasted 72 hours.
au_pending() {
  local comp=$1 reason=$2 f="$AU_STATE_DIR/pending-$1" since
  [[ -f "$f" ]] || date +%s > "$f"
  since=$(cat "$f")
  if (( $(date +%s) - since > 72 * 3600 )) && [[ ! -f "$f.notified" ]]; then
    au_notify fail "$comp auto-update blocked" "Auto-update for $comp has not been able to proceed since $(au_date "$since"): $reason" && touch "$f.notified"
  fi
}

# Returns 2 when the code tree is unusable for a build.
au_cmupdate() {
  local stamp="$AU_STATE_DIR/cmupdate.stamp" before after rc
  [[ "$AUTOUPDATE_CMUPDATE" = [yY] && "$AU_NO_CMUPDATE" != y ]] || return 0
  # once per hourly run inside the update hours
  [[ -n "$(find "$stamp" -mmin -50 2>/dev/null)" ]] && return 0
  before=$(au_head)
  timeout -k 30 1800 bash "${CM_INSTALLDIR}/tools/cmupdate.sh" update > "$AU_STATE_DIR/cmupdate.log" 2>&1 9>&-
  rc=$?
  # cmupdate's re-clone moves the tree aside first and puts it back itself on
  # a failed clone; a timeout kill skips that, so put it back here
  if [[ "$rc" -eq 124 || "$rc" -eq 137 ]] && [[ -d "${CM_INSTALLDIR}-automoved-cmupdate" ]]; then
    rm -rf "$CM_INSTALLDIR" && mv "${CM_INSTALLDIR}-automoved-cmupdate" "$CM_INSTALLDIR"
    au_say "cmupdate: timed out while re-cloning, restored the previous ${CM_INSTALLDIR}"
  fi
  after=$(au_head)
  if [[ -z "$after" ]]; then
    au_say "cmupdate: ${CM_INSTALLDIR} is not a git checkout after cmupdate, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "${CM_INSTALLDIR} is not a git checkout after cmupdate"
    return 2
  fi
  if au_git ls-files -u | grep -q .; then
    au_say "cmupdate: ${CM_INSTALLDIR} has unmerged files after cmupdate, not building, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "${CM_INSTALLDIR} has unmerged files after cmupdate"
    return 2
  fi
  if [[ "$rc" -ne 0 ]] || grep -qE 'git_sync: ERROR|Error: (unable|wasn)' "$AU_STATE_DIR/cmupdate.log"; then
    # retried on the next run; the build goes ahead with the current code
    au_say "cmupdate: failed (exit $rc), continuing with the code at $after, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "cmupdate keeps failing, see $AU_STATE_DIR/cmupdate.log"
    return 0
  fi
  touch "$stamp"
  rm -f "$AU_STATE_DIR"/pending-cmupdate*
  if [[ "$before" = "$after" ]]; then
    au_log "cmupdate: code unchanged at $after"
    return 0
  fi
  au_say "cmupdate: Centmin Mod code updated $before -> $after"
  # carry on with the updated script, the lock stays held on fd 9
  if bash -n "${CM_INSTALLDIR}/tools/autoupdate.sh" 2>/dev/null; then
    AU_NO_CMUPDATE=y AU_LOCKED=y exec bash "${CM_INSTALLDIR}/tools/autoupdate.sh" "$AU_CMD"
  fi
  au_say "cmupdate: the updated tools/autoupdate.sh is missing or fails bash -n, continuing with the running copy"
  return 0
}

# notify once when a run has held the lock for AUTOUPDATE_STUCK_HOURS
au_check_stuck() {
  local pid started
  au_lock_held || return 0
  read -r pid started 2>/dev/null < "${AU_LOCK}.info"
  [[ "$started" =~ ^[0-9]+$ ]] || return 0
  if (( $(date +%s) - started > AUTOUPDATE_STUCK_HOURS * 3600 )) && [[ ! -f "$AU_STATE_DIR/stuck-$started" ]]; then
    au_notify fail "auto-update run stuck" "An autoupdate run (pid $pid) has been running since $(au_date "$started"). Check it with: ps -fp $pid; tail $(ls -t "${CENTMINLOGDIR}"/autoupdate_*.log 2>/dev/null | head -n1)" && touch "$AU_STATE_DIR/stuck-$started"
  fi
}

# true when a process running as root (real and effective uid 0) holds a
# lock on $AU_LOCK's current inode, from /proc/locks
au_lock_root_holder() {
  local ino pid
  ino=$(stat -c %i "$AU_LOCK" 2>/dev/null) || return 1
  for pid in $(awk -v ino="$ino" '{for (i = 2; i <= NF; i++) if ($i ~ (":" ino "$")) print $(i - 1)}' /proc/locks 2>/dev/null); do
    [[ "$(awk '/^Uid:/ {print $2, $3}' "/proc/$pid/status" 2>/dev/null)" = '0 0' ]] && return 0
  done
  return 1
}

au_take_lock() {
  local pid started
  [[ "$AU_LOCKED" = y ]] && return 0
  # Other users must not be able to open the lock: flock works on a read-only
  # fd, so a world-readable lock file could be held forever by anyone, which
  # stops every update and makes centmin.sh and cmupdate refuse to start.
  if [[ -L "$AU_LOCK" ]] || { [[ -e "$AU_LOCK" ]] && [[ ! -f "$AU_LOCK" || "$(stat -c '%u %a' "$AU_LOCK" 2>/dev/null)" != '0 600' ]]; }; then
    # only root can chmod it in /var/run, so a root holder is a real run
    if au_lock_root_holder; then
      au_log "skip: $AU_LOCK has the wrong mode but a root process holds it"
      return 1
    fi
    au_log "lock: $AU_LOCK had the wrong owner, type or mode, recreated"
    rm -f "$AU_LOCK"
  fi
  umask 077
  exec 9>>"$AU_LOCK"
  umask 022
  if ! flock -n 9; then
    exec 9>&-
    read -r pid started 2>/dev/null < "${AU_LOCK}.info"
    if [[ "$AU_CMD" = run ]]; then
      au_say "skip: another autoupdate run (pid $pid, since $(au_date "$started")) holds the lock"
    else
      au_log "skip: another autoupdate run (pid $pid) holds the lock"
    fi
    au_check_stuck
    return 1
  fi
  echo "$$ $(date +%s)" > "${AU_LOCK}.info"
  rm -f "$AU_STATE_DIR"/stuck-*
  return 0
}

# $1 = cron (scheduled: hours window and delay apply) or run (manual)
au_run() {
  local mode=$1 comp rc=0 icomp ikey why
  [[ "$(id -u)" -eq 0 ]] || { echo "autoupdate.sh must run as root"; return 1; }
  mkdir -p "$AU_STATE_DIR" "$CENTMINLOGDIR"
  if ! au_enabled; then
    [[ "$mode" = run ]] && echo "AUTOUPDATE_NGINX and AUTOUPDATE_PHP are both 'n', nothing to do"
    return 0
  fi
  if [[ "$mode" = cron && "$AU_LOCKED" != y ]]; then
    au_check_stuck
    au_in_hours || return 0
  fi
  [[ -n "$AU_CONFIG_WARN" ]] && au_say "config: $AU_CONFIG_WARN"
  au_take_lock || return 0
  # centmin.sh and cmupdate refuse to start while the lock is held; let ours run
  export CMM_AUTOUPDATE_RUN=y
  if [[ -f "$AU_STATE_DIR/inprogress" ]]; then
    # the previous run died mid-build (reboot, OOM, kill): hold that update
    # like a failure so a build that takes the server down is not repeated
    IFS='|' read -r icomp ikey _ < "$AU_STATE_DIR/inprogress"
    rm -f "$AU_STATE_DIR/inprogress"
    [[ "$icomp" = nginx || "$icomp" = php ]] && au_fail_record "$icomp" "$ikey" >/dev/null
    au_notify fail "auto-update interrupted" "A previous auto-update run was interrupted mid-build ($icomp: $ikey); it is held like a failed update. Check that nginx and php-fpm are healthy. nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO)."
  fi
  # before cmupdate: it must not change the code under a running centmin.sh
  if why=$(au_cmm_running); then
    au_say "deferred, $why is running"
    au_pending run "$why is running"
    return 0
  fi
  rm -f "$AU_STATE_DIR"/pending-run*
  au_cmupdate
  [[ $? -eq 2 ]] && return 1
  [[ "$mode" = run ]] && AU_FORCE=y
  for comp in nginx php; do
    if [[ "$comp" = nginx && "$AUTOUPDATE_NGINX" != [yY] ]] || [[ "$comp" = php && "$AUTOUPDATE_PHP" != [yY] ]]; then
      continue
    fi
    # a long nginx build must not push the PHP restart past the window
    if [[ "$mode" = cron ]] && ! au_in_hours; then
      au_log "$comp: AUTOUPDATE_HOURS window closed, left for the next window"
      continue
    fi
    au_plan "$comp"
    case "$AU_ACTION" in
      upgrade|rebuild) ;;
      none) rm -f "$AU_STATE_DIR/pending-$comp"*; au_log "$comp: $AU_WHY"; continue ;;
      skip) au_say "$comp: skip: $AU_WHY"; au_pending "$comp" "$AU_WHY"; continue ;;
      *) rm -f "$AU_STATE_DIR/pending-$comp"*; au_say "$comp: $AU_ACTION: $AU_WHY"; continue ;;
    esac
    why=$(au_blocked "$comp")
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
  AU_DRYRUN=y
  [[ -n "$AU_CONFIG_WARN" ]] && echo "config $AU_CONFIG_WARN"
  for comp in nginx php; do
    enabled=$AUTOUPDATE_NGINX
    [[ "$comp" = php ]] && enabled=$AUTOUPDATE_PHP
    au_plan "$comp"
    why=$(au_blocked "$comp")
    printf '%-6s AUTOUPDATE_%s=%s installed=%s target=%s -> %s: %s\n' "$comp" "$(echo "$comp" | tr a-z A-Z)" "$enabled" "${AU_INSTALLED:--}" "${AU_TARGET:--}" "$AU_ACTION" "$AU_WHY"
    printf '%-6s gates: %s\n' '' "${why:-ok}"
  done
  echo "hours  AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS now $(date +%H:%M) -> $(au_in_hours && echo inside || echo outside)"
}

au_status() {
  local comp
  echo "Settings: AUTOUPDATE_NGINX=$AUTOUPDATE_NGINX AUTOUPDATE_PHP=$AUTOUPDATE_PHP AUTOUPDATE_NGINX_TARGET=$AUTOUPDATE_NGINX_TARGET AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS AUTOUPDATE_DELAY_HOURS=$AUTOUPDATE_DELAY_HOURS AUTOUPDATE_CMUPDATE=$AUTOUPDATE_CMUPDATE"
  [[ -n "$AU_CONFIG_WARN" ]] && echo "Config:   $AU_CONFIG_WARN"
  [[ -n "$AUTOUPDATE_PHP_HOLD" ]] && echo "PHP hold: AUTOUPDATE_PHP_HOLD=$AUTOUPDATE_PHP_HOLD, installed PHP $(au_php_installed) -> $([[ "$AU_PHP_HOLD_INVALID" = y || "$AUTOUPDATE_PHP_HOLD" = "$(au_php_installed)" ]] && echo held || echo 'not held, versions differ')"
  echo "Notify:   AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY email=${AUTOUPDATE_EMAIL:-none} pushover=$([[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] && echo on || echo off)"
  if [[ -f "$AU_CRON_FILE" ]]; then
    echo "Cron:     $AU_CRON_FILE: $(grep -v '^#' "$AU_CRON_FILE" | grep autoupdate.sh)"
  else
    echo "Cron:     $AU_CRON_FILE not present$(au_enabled && echo ' (run: autoupdate.sh ensure)')"
  fi
  [[ -f "$AU_STATE_DIR/inprogress" ]] && echo "Running:  $(cut -d'|' -f1,2 "$AU_STATE_DIR/inprogress") since $(au_date "$(cut -d'|' -f3 "$AU_STATE_DIR/inprogress")")"
  for comp in nginx php; do
    [[ -f "$AU_STATE_DIR/last-$comp" ]] && echo "Last $comp: $(cat "$AU_STATE_DIR/last-$comp")"
    [[ -f "$AU_STATE_DIR/failed-$comp" ]] && echo "Held $comp: $(cut -d'|' -f1 "$AU_STATE_DIR/failed-$comp") failed $(cut -d'|' -f4 "$AU_STATE_DIR/failed-$comp") time(s), last at code $(cut -d'|' -f2 "$AU_STATE_DIR/failed-$comp")"
  done
  [[ -f "$AU_LOG" ]] && { echo "Recent log ($AU_LOG):"; tail -n 8 "$AU_LOG"; }
}

# one "tag|text" line for the dmotd login banner, or nothing
au_motd() {
  local comp fkey fhead failed parts='' held=''
  AU_ENSURE_BY=dmotd au_ensure
  au_enabled || return 0
  if [[ -f "$AU_STATE_DIR/inprogress" ]] && au_lock_held; then
    IFS='|' read -r comp fkey failed < "$AU_STATE_DIR/inprogress"
    echo "warn|Auto-update: $fkey running since $(au_date "$failed") - wait before using centmin.sh menu 4/5"
    return 0
  fi
  for comp in nginx php; do
    if [[ -f "$AU_STATE_DIR/failed-$comp" ]]; then
      IFS='|' read -r fkey fhead failed _ < "$AU_STATE_DIR/failed-$comp"
      # "php rebuild <epoch>" -> "php rebuild", "nginx 1.31.8" stays
      [[ "$fkey" =~ ^(.*rebuild)\ [0-9]+$ ]] && fkey=${BASH_REMATCH[1]}
      echo "warn|Auto-update: ${fkey} FAILED $(au_date "$failed") - run: bash ${CM_INSTALLDIR}/tools/autoupdate.sh status"
      return 0
    fi
  done
  if [[ "$AU_SYSTEMD" = y ]] && ! systemctl is-active --quiet crond; then
    echo "warn|Auto-update: crond is not running, scheduled updates cannot run - systemctl start crond"
    return 0
  fi
  if [[ -n "$AU_CONFIG_WARN" ]]; then
    echo "warn|Auto-update: ${AU_CONFIG_WARN%%;*}"
    return 0
  fi
  [[ "$AUTOUPDATE_NGINX" = [yY] ]] && parts="nginx"
  if [[ "$AUTOUPDATE_PHP" = [yY] ]]; then
    parts="${parts:+$parts + }PHP-FPM"
    if [[ -n "$AUTOUPDATE_PHP_HOLD" ]]; then
      if [[ "$AUTOUPDATE_PHP_HOLD" != "$(au_php_installed)" ]]; then
        echo "warn|Auto-update: AUTOUPDATE_PHP_HOLD='$AUTOUPDATE_PHP_HOLD' but PHP $(au_php_installed) is installed, so PHP is not held"
        return 0
      fi
      held=", PHP held at $AUTOUPDATE_PHP_HOLD"
    fi
  fi
  echo "info|Auto-update: $parts on${held}, hours $AUTOUPDATE_HOURS"
}

# How auto-update will handle an available nginx|php version, for the
# update-available alerts of dmotd and check-for-updates.sh; nothing when it
# won't apply it by itself.
au_alertnote() {
  case "$1" in
    nginx)
      [[ "$AUTOUPDATE_NGINX" = [yY] ]] || return 0
      if [[ "$AUTOUPDATE_NGINX_TARGET" = latest ]]; then
        echo "auto-update applies it after AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h, within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS"
      else
        echo "auto-update applies it once Centmin Mod's NGINX_VERSION pin reaches it, within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS"
      fi
      ;;
    php)
      [[ "$AUTOUPDATE_PHP" = [yY] ]] || return 0
      [[ "$AU_PHP_HOLD_INVALID" = y || ( -n "$AUTOUPDATE_PHP_HOLD" && "$AUTOUPDATE_PHP_HOLD" = "$(au_php_installed)" ) ]] && return 0
      echo "auto-update applies it within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS, after AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h unless php.net marks it a security release"
      ;;
  esac
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
    alertnote)
      au_alertnote "$2"
      ;;
    notify-test)
      AU_NOTIFY_FORCE=y au_notify ok "auto-update test notification" "Test notification from $(basename "$0"): AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY, email=${AUTOUPDATE_EMAIL:-none}."
      au_status | grep -E '^Notify'
      tail -n 3 "$AU_LOG" 2>/dev/null
      ;;
    reset)
      rm -f "$AU_STATE_DIR"/failed-* "$AU_STATE_DIR"/pending-* "$AU_STATE_DIR"/stuck-* "$AU_STATE_DIR/cmupdate.stamp"
      au_lock_held || rm -f "$AU_STATE_DIR/inprogress"
      au_say "reset: cleared failed and interrupted update holds, blocked-update timers and the cmupdate stamp (delay timers are kept)"
      ;;
    help|*)
      sed -n '2,/^# end of settings$/p' "$0" | sed -e '/^#\{10,\}$/d' -e '/^# end of settings$/d' -e 's/^# \{0,1\}//'
      ;;
  esac
}

# parse everything before running, cmupdate can replace this file mid-run
au_main "$@"; exit $?
