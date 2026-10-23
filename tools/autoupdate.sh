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
# - nginx: the newest nginx.org release (freenginx.org when
#   FREENGINX_INSTALL='y') on the branch you run: mainline (odd minor
#   version, Centmin Mod's nginx default) or stable (even minor version,
#   Centmin Mod's freenginx default). New versions arrive without waiting for
#   a Centmin Mod update. AUTOUPDATE_NGINX_TARGET='centmin' follows Centmin
#   Mod's NGINX_VERSION default instead, which cmupdate brings in. An
#   NGINX_VERSION line in custom_config.inc (FREENGINX_VERSION for freenginx)
#   sets the version auto-update installs and stops at, with either setting.
#   Only nginx.org and freenginx builds are updated; Angie and other builds
#   are skipped.
# - PHP-FPM: the newest php.net release of the installed major.minor branch,
#   e.g. 8.4.26 -> 8.4.27. It never moves PHP to another branch.
# - Never downgrades. A new version waits AUTOUPDATE_DELAY_HOURS after it is
#   first seen; php.net releases tagged security and rebuilds go ahead at once.
# - Never a version listed in tools/autoupdate-skip.txt, where Centmin Mod
#   lists releases known to break builds or sites. cmupdate delivers it, so
#   a bad release is stopped on every server without changing NGINX_VERSION;
#   'latest' then uses the newest release not listed, PHP waits for the next.
# - Same-version rebuilds listed in tools/autoupdate-rebuilds.txt, which
#   Centmin Mod adds when patches change without a new upstream release,
#   e.g. security backports for end-of-life PHP 7.2-8.1.
#
# HOW A RUN WORKS
# An hourly /etc/cron.d/centminmod-autoupdate entry runs `autoupdate.sh cron`.
# Outside AUTOUPDATE_HOURS it only repairs that entry, looks for a stuck run
# and records a run that died mid-build. Inside the window it:
#  1. takes a lock; centmin.sh and cmupdate refuse to start until it ends
#  2. runs cmupdate (AUTOUPDATE_CMUPDATE) so patches and version pins are
#     current, then carries on with the updated copy of this script. If
#     cmupdate fails or runs over 30 minutes the run uses the current code,
#     but it builds nothing when the checkout is left unusable (unmerged or
#     cut-off git work, local changes not re-applied, an older tree)
#  3. for nginx, then PHP-FPM, works out the target and waits for a later
#     run while centmin.sh, cmupdate or yum/dnf/rpm is running, the load or
#     free disk is outside AUTOUPDATE_MAXLOAD / AUTOUPDATE_MINDISK_MB,
#     nginx -t or (for PHP) php-fpm -t fails, the service is stopped (unless
#     a failed update left it down: then the rebuild is the repair), or
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
#   hours pass, at most 3 automatic attempts per update; `run` retries now
#   and `reset` clears the hold. With no newer update pending, the failure clears itself
#   once nginx/php-fpm was rebuilt since (e.g. by menu 4/5) and is healthy; a
#   service left down or on its old binary is rebuilt on the same retry
#   schedule; a healthy service still on the failed build stays held until
#   `reset`. Until then later updates are checked against the state from
#   before the failure, and a held upgrade does not hold back a listed rebuild.
# - A run interrupted mid-build (reboot, OOM, kill) is held like a failure.
# - One alert each when an update stays blocked or skipped for 72 hours, when
#   a run holds the lock for AUTOUPDATE_STUCK_HOURS, or after an interrupted run.
# - Deleting the cron entry does not turn auto-updates off: root/sudo dmotd
#   logins, centmin.sh, cmupdate and the check-for-updates.sh cron put it
#   back, log it and notify (unless AUTOUPDATE_NOTIFY is failures or none).
#   Set AUTOUPDATE_NGINX and AUTOUPDATE_PHP to 'n' to turn it off.
# - AUTOUPDATE_PHP_HOLD keeps a PHP version, e.g. after a deliberate
#   downgrade. For nginx, an NGINX_VERSION line in custom_config.inc holds
#   that version (upgrades stop there), but listed same-version rebuilds
#   still run; AUTOUPDATE_NGINX='n' stops all nginx changes.
# - A new nginx release that builds but breaks a module at runtime is only
#   caught, and rolled back, when AUTOUPDATE_CHECK_URLS has a page that uses
#   that module: list your sites' pages there.
# - y/n settings also accept yes/no, on/off, true/false and 1/0; case and
#   spaces don't matter and an empty value means the default. An invalid
#   value falls back to the default (a hold keeps holding, a target falls back
#   to centmin), check and status list it, and the dmotd login banner and one
#   alert per new problem say so.
#
# COMMANDS   bash /usr/local/src/centminmod/tools/autoupdate.sh <command>
#   check        what a run would do now and why; changes no update state
#                (like centmin.sh it converts CRLF line endings in
#                custom_config.inc)
#   status       settings, cron entry, last results, held failures, recent log
#   run          update now, ignoring AUTOUPDATE_HOURS, the delay and the
#                wait before retrying a failed update (AUTOUPDATE_PHP_HOLD
#                still applies; a failed build still in use needs reset)
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
#   tools/autoupdate-skip.txt           versions never to install
#
# RELATED SETTINGS IT USES (also set in custom_config.inc)
#   PUSH_API_TOKEN, PUSH_USER_KEY       Pushover app token and user key
#   EMAILNOTIFY_SES='y' with EMAILNOTIFY_SES_SMTP_SERVER, _SMTP_PORT,
#     _SMTP_USERNAME, _SMTP_PASSWORD, _FROM_EMAIL
#                                       SMTP login for email: Amazon SES, a
#                                       Gmail app password or any SMTP server
#   NGINX_VERSION, FREENGINX_VERSION   a line in custom_config.inc holds nginx
#                                       at that version
#   FREENGINX_INSTALL                   'y' updates freenginx from freenginx.org
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
AUTOUPDATE_NGINX_TARGET='latest'  # 'latest' = newest nginx.org (or freenginx.org) release on
                                  # the installed branch (mainline or stable), 'centmin' =
                                  # Centmin Mod's NGINX_VERSION default (FREENGINX_VERSION for
                                  # freenginx; cmupdate brings it). An
                                  # NGINX_VERSION line in custom_config.inc sets the version to
                                  # install and stop at, with either setting
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
AUTOUPDATE_CHECK_URLS='http://127.0.0.1/' # http(s) URLs, space or comma separated, that must keep answering below
                                  # 500 after an update; add your sites' pages (PHP, cached,
                                  # module-specific) so a broken update is caught and rolled back.
                                  # Redirects are not followed (list final URLs), and
                                  # FORCE_IPVFOUR='y' (the default) checks them over IPv4
AUTOUPDATE_NICE='10'              # CPU nice level 0-19 for the build (systemd hosts only)
AUTOUPDATE_STUCK_HOURS='4'        # alert once when a run holds the lock this long
#
# Notifications
AUTOUPDATE_NOTIFY='all'           # all (successes and notices too), failures or none. The
                                  # dmotd/check-for-updates "update available" pushes have their
                                  # own switches, PUSH_MOTD_ALERTS and PUSH_CHECKUPDATES_ALERTS,
                                  # which also turn off those scripts' CVE, kernel and reboot pushes
AUTOUPDATE_PUSHOVER='y'           # use Pushover when PUSH_API_TOKEN and PUSH_USER_KEY are set
AUTOUPDATE_EMAIL=''               # recipients, space or comma separated. Sent through the
                                  # EMAILNOTIFY_SES_* SMTP login when EMAILNOTIFY_SES='y' and its
                                  # server and username are set and curl has SMTP (EL9/EL10
                                  # curl-minimal does not: dnf install --allowerasing curl;
                                  # `status` shows the path), else the local mail or sendmail
                                  # command, which Gmail and other large providers reject
                                  # unless the server has SPF/DKIM or a relay host
AUTOUPDATE_EMAIL_FROM=''          # sender address; default for SMTP: EMAILNOTIFY_SES_FROM_EMAIL,
                                  # for mail/sendmail: the system default
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
AU_SKIPS="${CM_INSTALLDIR}/tools/autoupdate-skip.txt"
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
    # dos2unix -q exits 0 even when it skipped the file (symlink, immutable)
    [ -x /usr/bin/dos2unix ] && dos2unix -q "$au_conf" 2>/dev/null
    if ! grep -q $'\r' "$au_conf" 2>/dev/null; then
      source "$au_conf"
    else
      # no dos2unix, or it could not rewrite the file (immutable, symlink)
      eval "$(tr -d '\r' < "$au_conf")"
    fi
  else
    source "$au_conf"
  fi
done
unset au_conf
# An NGINX_VERSION line in custom_config.inc (FREENGINX_VERSION with
# freenginx) holds nginx at that version whatever AUTOUPDATE_NGINX_TARGET says
AU_NGINX_PIN='' AU_NGINX_PIN_VAR=NGINX_VERSION AU_NGINX_PIN_EMPTY=n
[[ "$FREENGINX_INSTALL" = [yY] ]] && AU_NGINX_PIN_VAR=FREENGINX_VERSION
if grep -qE "^[[:space:]]*(export[[:space:]]+)?${AU_NGINX_PIN_VAR}=" "${CONFIGSCANBASE}/custom_config.inc" 2>/dev/null; then
  AU_NGINX_PIN=${!AU_NGINX_PIN_VAR}
  # an empty line is not a hold, but say so: it reads like one
  [[ -n "$AU_NGINX_PIN" ]] || AU_NGINX_PIN_EMPTY=y
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi
AU_HOST=$(hostname)
AU_SYSTEMD=n
[[ -d /run/systemd/system ]] && AU_SYSTEMD=y

# Use the default for an empty setting. An unusable one falls back to the
# default (holds keep holding, a target to the slower-moving one), and check,
# status, the run log, the dmotd banner and a one-time alert say so instead of
# silently never updating.
au_validate_config() {
  local v name def min max u urls
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
  v=$(printf '%s' "$AUTOUPDATE_HOURS" | tr -d '[:space:]' | tr '[:upper:]' '[:lower:]' | sed -E 's/:[0-9]{2}//g')
  [[ -n "$v" ]] || v='2-5'
  [[ "$v" =~ ^[0-9]{1,2}$ ]] && v="$v-$v"
  if [[ "$v" = any ]] || { [[ "$v" =~ ^([0-9]{1,2})-([0-9]{1,2})$ ]] && (( 10#${BASH_REMATCH[1]} < 24 && 10#${BASH_REMATCH[2]} < 24 )); }; then
    AUTOUPDATE_HOURS=$v
  else
    AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_HOURS='$AUTOUPDATE_HOURS' is invalid, using 2-5;"
    AUTOUPDATE_HOURS='2-5'
  fi
  # bounded: a huge number would wrap in bash arithmetic (and 0 stuck hours alerts every hour)
  for name in AUTOUPDATE_DELAY_HOURS:24:0:8760 AUTOUPDATE_MINDISK_MB:3072:0:10000000 AUTOUPDATE_STUCK_HOURS:4:1:720 AUTOUPDATE_NICE:10:0:19; do
    IFS=: read -r name def min max <<< "$name"
    v=$(printf '%s' "${!name}" | tr -d '[:space:]')
    [[ -n "$v" ]] || v=$def
    # leading zeros would make bash read 08 as bad octal and 03072 as 1594
    if [[ "$v" =~ ^[0-9]{1,8}$ ]] && (( 10#$v >= min && 10#$v <= max )); then
      printf -v "$name" '%s' "$((10#$v))"
      continue
    fi
    AU_CONFIG_WARN="$AU_CONFIG_WARN $name='${!name}' is not a number from $min to $max, using $def;"
    printf -v "$name" '%s' "$def"
  done
  AUTOUPDATE_MAXLOAD=$(printf '%s' "$AUTOUPDATE_MAXLOAD" | tr -d '[:space:]')
  AUTOUPDATE_CRON_MINUTE=$(printf '%s' "$AUTOUPDATE_CRON_MINUTE" | tr -d '[:space:]')
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
    [[ "$AUTOUPDATE_PHP" = y ]] && AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_PHP_HOLD='$AUTOUPDATE_PHP_HOLD' is not a version like 8.4.26, holding PHP updates until it is fixed;"
  fi
  # centmin.sh's own default is 'y': copied into custom_config.inc it blocks every build
  if [[ "$AUTOUPDATE_NGINX" = y || "$AUTOUPDATE_PHP" = y ]] && [[ "$ENABLE_MENU" = [yY] ]]; then
    AU_CONFIG_WARN="$AU_CONFIG_WARN ENABLE_MENU='y' in custom_config.inc makes centmin-cli.sh interactive, so auto-update cannot build;"
  fi
  # a mistyped value falls back to all: a missed failure alert costs more than an extra one
  v=$(printf '%s' "$AUTOUPDATE_NOTIFY" | tr -d '[:space:]' | tr '[:upper:]' '[:lower:]')
  case "${v:-all}" in
    all|failures|none) AUTOUPDATE_NOTIFY=${v:-all} ;;
    *) AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_NOTIFY='$AUTOUPDATE_NOTIFY' is not all, failures or none, using all;"
       AUTOUPDATE_NOTIFY=all ;;
  esac
  # a mistyped nginx hold must not let nginx update to something else
  if [[ "$AUTOUPDATE_NGINX" != y ]]; then
    :
  elif [[ -n "$AU_NGINX_PIN" ]] && ! [[ "$AU_NGINX_PIN" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    AU_CONFIG_WARN="$AU_CONFIG_WARN $AU_NGINX_PIN_VAR='$AU_NGINX_PIN' in custom_config.inc is not a version like 1.31.7, holding nginx updates until it is fixed;"
  elif [[ "$AU_NGINX_PIN_EMPTY" = y ]]; then
    AU_CONFIG_WARN="$AU_CONFIG_WARN $AU_NGINX_PIN_VAR is empty in custom_config.inc, so nginx is not held (remove the line, or set a version to hold);"
  fi
  # a mistyped target uses centmin, the slower-moving one
  v=$(printf '%s' "$AUTOUPDATE_NGINX_TARGET" | tr -d '[:space:]' | tr '[:upper:]' '[:lower:]')
  case "${v:-latest}" in
    centmin|latest) AUTOUPDATE_NGINX_TARGET=${v:-latest} ;;
    *) AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_NGINX_TARGET='$AUTOUPDATE_NGINX_TARGET' is not latest or centmin, using centmin;"
       AUTOUPDATE_NGINX_TARGET=centmin ;;
  esac
  # http(s) URLs, space or comma separated; anything else is left out and reported
  v=$(printf '%s' "$AUTOUPDATE_CHECK_URLS" | tr '\n\t' '  ' | sed -E 's#,(https?://)# \1#g')
  read -ra urls <<< "$v"
  AUTOUPDATE_CHECK_URLS=''
  for u in "${urls[@]}"; do
    u=${u#,} u=${u%,}
    [[ -n "$u" ]] || continue
    if [[ "${u,,}" =~ ^https?://[^/,]+ ]]; then
      AUTOUPDATE_CHECK_URLS="${AUTOUPDATE_CHECK_URLS:+$AUTOUPDATE_CHECK_URLS }$u"
    else
      AU_CONFIG_WARN="$AU_CONFIG_WARN AUTOUPDATE_CHECK_URLS entry '$u' is not an http(s) URL, so it is not checked;"
    fi
  done
  # email lists may be comma separated, as for emailnotify.sh
  read -ra urls <<< "${AUTOUPDATE_EMAIL//,/ }"
  AUTOUPDATE_EMAIL="${urls[*]}"
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

# nginx branch of x.y.z: 1 mainline (odd minor version), 0 stable (even)
au_branch() {
  local v=${1#*.}
  echo $(( 10#${v%%.*} % 2 ))
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
  # a shared probe: two probes at once must not see each other as a run
  [[ -f "$AU_LOCK" && ! -L "$AU_LOCK" && "$(stat -c '%u %a' "$AU_LOCK" 2>/dev/null)" = '0 600' ]] && ! flock -n -s "$AU_LOCK" -c true 2>/dev/null
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
  # the php-fpm pid directory can be owned by the nginx user: trust only a root pid
  if [[ "$pid" =~ ^[0-9]+$ ]] && kill -0 "$pid" 2>/dev/null && [[ "$(stat -c %u "/proc/$pid" 2>/dev/null)" = 0 ]]; then
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
# One alert per distinct settings problem, kept for a retry until delivered.
# Only when auto-update is on, or the problem is a mistyped on/off switch.
au_config_alert() {
  local f="$AU_STATE_DIR/config-warned"
  if [[ -z "$AU_CONFIG_WARN" ]]; then rm -f "$f" "$f.sent"; return 0; fi
  au_enabled || [[ "$AU_CONFIG_WARN" == *AUTOUPDATE_NGINX=* || "$AU_CONFIG_WARN" == *AUTOUPDATE_PHP=* ]] || return 0
  if [[ "$(cat "$f" 2>/dev/null)" != "$AU_CONFIG_WARN" ]]; then
    # without its marker every call would alert again
    { printf '%s\n' "$AU_CONFIG_WARN" > "$f" && [[ "$(cat "$f" 2>/dev/null)" = "$AU_CONFIG_WARN" ]]; } || return 0
    rm -f "$f.sent"
    au_log "config: $AU_CONFIG_WARN"
  elif [[ -f "$f.sent" || -n "$(find "$f" -mmin -50 2>/dev/null)" ]]; then
    # sent, or tried this hour (-50: the next hourly cron run qualifies)
    return 0
  fi
  touch "$f"
  au_notify fail "auto-update settings problem" "Auto-update settings in ${CONFIGSCANBASE}/custom_config.inc need fixing: $AU_CONFIG_WARN Check with: bash ${CM_INSTALLDIR}/tools/autoupdate.sh check" && touch "$f.sent"
}

au_ensure() {
  local want had=n tmp msg
  [[ "$(id -u)" -eq 0 ]] || return 0
  mkdir -p "$AU_STATE_DIR"
  if [[ "$AU_ENSURE_BY" = dmotd || "$AU_ENSURE_BY" = centmin.sh ]]; then
    # don't hold up a login banner or centmin.sh start on a slow Pushover/SMTP server
    ( au_config_alert ) </dev/null >/dev/null 2>&1 &
  else
    au_config_alert
  fi
  if au_enabled; then
    want=$(au_cron_content)
    # cron skips a file other users can write, so owner and mode count too;
    # restorecon is cheap and fixes a wrong SELinux label on an unchanged file
    if [[ -f "$AU_CRON_FILE" && "$(cat "$AU_CRON_FILE" 2>/dev/null)" = "$want" && "$(stat -c '%u %a' "$AU_CRON_FILE" 2>/dev/null)" = '0 644' ]]; then
      command -v restorecon >/dev/null 2>&1 && restorecon "$AU_CRON_FILE" 2>/dev/null
      return 0
    fi
    [[ -f "$AU_CRON_FILE" ]] && had=y
    # a dot file in /etc/cron.d is not read by cron; unique per process
    tmp=$(mktemp "/etc/cron.d/.centminmod-autoupdate.XXXXXX") || { au_log "ensure: could not write in /etc/cron.d"; return 1; }
    printf '%s\n' "$want" > "$tmp" && chmod 0644 "$tmp" && mv -f "$tmp" "$AU_CRON_FILE" || {
      rm -f "$tmp"
      au_log "ensure: could not write $AU_CRON_FILE"
      return 1
    }
    # with SELinux enforcing, crond reads only the cron.d label
    command -v restorecon >/dev/null 2>&1 && restorecon -F "$AU_CRON_FILE" 2>/dev/null
    if [[ -f "$AU_STATE_DIR/scheduler-installed" ]]; then
      if [[ "$had" = y ]]; then
        au_log "ensure: $AU_CRON_FILE differed from the expected entry, rewritten (${AU_ENSURE_BY:-manual})"
      else
        au_log "ensure: $AU_CRON_FILE was missing, re-added (${AU_ENSURE_BY:-manual})"
        msg="$AU_CRON_FILE was missing while AUTOUPDATE_NGINX='$AUTOUPDATE_NGINX' AUTOUPDATE_PHP='$AUTOUPDATE_PHP', so it was re-added (found by ${AU_ENSURE_BY:-manual}). To stop auto-updates set both to 'n' in ${CONFIGSCANBASE}/custom_config.inc."
        if [[ "$AU_ENSURE_BY" = dmotd || "$AU_ENSURE_BY" = centmin.sh ]]; then
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
  local priority=$1 title=$2 message=$3 response dir tail
  [[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] || return 2
  # Pushover takes 1024 characters: keep the start and the closing "what next"
  # sentence whole when it is short enough
  if (( ${#message} > 1024 )); then
    tail=${message##*. }
    if (( ${#tail} <= 300 )); then
      message="${message:0:$(( 1018 - ${#tail} ))} … $tail"
    else
      message="${message:0:860} … ${message: -160}"
    fi
  fi
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
  local subject=$1 body=$2 dir from addr url rc rcpt args=()
  if ! curl -V 2>/dev/null | grep -qw smtp; then
    au_log "notify: this curl has no SMTP support (curl -V), email not sent"
    return 1
  fi
  # the first of these that is an address; 'Name <a@b>' keeps the name in From only
  for from in "$AUTOUPDATE_EMAIL_FROM" "$EMAILNOTIFY_SES_FROM_EMAIL" "$EMAILNOTIFY_SES_SMTP_USERNAME"; do
    [[ "$from" = *@* ]] && break
  done
  if [[ "$from" != *@* ]]; then
    au_log "notify: set AUTOUPDATE_EMAIL_FROM or EMAILNOTIFY_SES_FROM_EMAIL to a sender address, email not sent"
    return 1
  fi
  addr=${from##*<} addr=${addr%%>*}
  dir=$(mktemp -d) || return 1
  {
    echo "From: $from"
    echo "To: ${AUTOUPDATE_EMAIL// /, }"
    echo "Subject: $subject"
    echo "Date: $(date -R)"
    echo "Message-ID: <autoupdate.$(date +%s).$$@${addr#*@}>"
    # build log tails can hold UTF-8 compiler quotes
    echo "MIME-Version: 1.0"
    echo "Content-Type: text/plain; charset=UTF-8"
    echo "Content-Transfer-Encoding: 8bit"
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
  curl -${ipv_forceopt}sS --crlf --connect-timeout 15 --max-time 60 "${args[@]}" --mail-from "$addr" -K "$dir/cfg" -T "$dir/msg" "$url" 2>"$dir/err"
  rc=$?
  [[ "$rc" -eq 0 ]] || au_log "notify: smtp $url failed (curl exit $rc): $(head -c 300 "$dir/err" | tr "\n" " ")"
  rm -rf "$dir"
  return "$rc"
}

# returns 0 when handed over, 1 when that failed, 2 when no recipient is set
au_email() {
  local subject=$1 body=$2 rc via='the local MTA'
  [[ -n "$AUTOUPDATE_EMAIL" ]] || return 2
  local smtp=n
  if [[ "$EMAILNOTIFY_SES" = [yY] && -n "$EMAILNOTIFY_SES_SMTP_SERVER" && -n "$EMAILNOTIFY_SES_SMTP_USERNAME" ]]; then
    # curl-minimal (EL9/EL10 images) has no SMTP: use the local mail command then
    if curl -V 2>/dev/null | grep -qw smtp; then
      smtp=y
    else
      au_log "notify: this curl has no SMTP support (curl -V), using the local mail command"
    fi
  fi
  if [[ "$smtp" = y ]]; then
    via="smtp $EMAILNOTIFY_SES_SMTP_SERVER"
    au_smtp "$subject" "$body"
    rc=$?
  elif command -v mail >/dev/null 2>&1; then
    # 9>&-: an MTA that delivers in the background must not keep the lock
    if [[ -n "$AUTOUPDATE_EMAIL_FROM" ]]; then
      printf '%s\n' "$body" | mail -s "$subject" -r "$AUTOUPDATE_EMAIL_FROM" $AUTOUPDATE_EMAIL 9>&-
    else
      printf '%s\n' "$body" | mail -s "$subject" $AUTOUPDATE_EMAIL 9>&-
    fi
    rc=$?
  elif [[ -x /usr/sbin/sendmail ]]; then
    {
      [[ -n "$AUTOUPDATE_EMAIL_FROM" ]] && echo "From: $AUTOUPDATE_EMAIL_FROM"
      echo "To: ${AUTOUPDATE_EMAIL// /, }"
      echo "Subject: $subject"
      echo
      printf '%s\n' "$body"
    } | /usr/sbin/sendmail -oi -t 9>&-
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
# Returns 0 when a channel took it or the level is filtered out (counts as
# done), 1 when every configured channel failed and 2 when none is configured:
# callers mark a one-time alert only on 0, so it is tried again later.
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
  # mail lines may not pass 998 characters
  body="$(printf '%s\n' "$message" | fold -s -w 900)

Host: $AU_HOST
Time: $(date '+%Y-%m-%d %H:%M:%S %Z')
Centmin Mod: $(au_head) ($CM_INSTALLDIR)
Status: bash ${CM_INSTALLDIR}/tools/autoupdate.sh status"
  if [[ -n "$logfile" && -f "$logfile" ]]; then
    body="$body
Log: $logfile

Last 60 log lines (cut to 900 characters each, as mail lines may not pass 998):
$(tail -n 60 "$logfile" | au_strip | tr -d '\r' | cut -c1-900)"
  fi
  au_email "[centminmod autoupdate] $AU_HOST: $title" "$body"
  erc=$?
  # 2 = no channel configured: one-time alerts stay unsent until one is
  (( prc == 0 || erc == 0 )) && return 0
  (( prc == 2 && erc == 2 )) && return 2
  return 1
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

# every nginx.org (or freenginx.org) release version, newest first; nothing
# when the download listing could not be fetched in full
au_nginx_releases() {
  local url=https://nginx.org/download/ name=nginx page
  [[ "$FREENGINX_INSTALL" = [yY] ]] && url=https://freenginx.org/download/ name=freenginx
  page=$(curl -${ipv_forceopt}fsL --connect-timeout 10 --max-time 30 "$url") || return 1
  grep -Eo "${name}-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz" <<< "$page" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | sort -u -r -V
}

# au_skip_reason component version: prints why tools/autoupdate-skip.txt
# lists that version and returns 0, or returns 1 when it is not listed
au_skip_reason() {
  local c v reason
  [[ -f "$AU_SKIPS" ]] || return 1
  while read -r c v reason || [[ -n "$c" ]]; do
    v=${v%$'\r'} reason=${reason%$'\r'}
    [[ -z "$c" || "$c" = \#* ]] && continue
    if [[ "$c" = "$1" && "$v" = "$2" ]]; then
      echo "${reason:-no reason given}"
      return 0
    fi
  done < "$AU_SKIPS"
  return 1
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
  local rcomp=$1 bin=$2 comp=$3 c when reason ts built best=0 bestwhy='' now
  [[ -f "$AU_REBUILDS" && -f "$bin" ]] || return 1
  built=$(au_build_time "$comp" "$bin") now=$(date +%s)
  while read -r c when reason || [[ -n "$c" ]]; do
    when=${when%$'\r'} reason=${reason%$'\r'}
    [[ -z "$c" || "$c" = \#* ]] && continue
    [[ "$c" = "$rcomp" || ( "$c" = php && "$rcomp" = php-* ) ]] || continue
    when=${when%Z}
    ts=$(date -u -d "${when/T/ } UTC" +%s 2>/dev/null) || continue
    # an entry takes effect at its time: a future date (or a typo) must not
    # set the rebuilt- mark past entries added later
    (( ts <= now )) || continue
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
  local comp=$1 due branch fkey fhead ftime fcount review skipc=$1 rel why releases parity nodata='' upwhy prod repoch rbin
  AU_ACTION=none AU_INSTALLED='' AU_TARGET='' AU_WHY='' AU_KEY='' AU_EPOCH='' AU_REL_SECURITY=n AU_REBUILD_COMP='' AU_REPAIR=n AU_SKIPNOTE=''
  [[ -f "$AU_STATE_DIR/failed-$comp" ]] && IFS='|' read -r fkey fhead ftime fcount < "$AU_STATE_DIR/failed-$comp"
  if [[ "$comp" = nginx ]]; then
    if ! command -v nginx >/dev/null 2>&1; then
      AU_ACTION=skip AU_WHY='nginx is not installed'; return
    fi
    AU_INSTALLED=$(au_nginx_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed '$(nginx -v 2>&1 | head -n1)' is not an nginx.org or freenginx release build"; return
    fi
    # menu 4 builds the product FREENGINX_INSTALL names: never switch product unattended
    case "$(nginx -v 2>&1)" in *freenginx/*) prod=freenginx ;; *) prod=nginx ;; esac
    if [[ "$prod" = freenginx && "$FREENGINX_INSTALL" != [yY] ]] || [[ "$prod" = nginx && "$FREENGINX_INSTALL" = [yY] ]]; then
      AU_ACTION=skip AU_WHY="FREENGINX_INSTALL='$FREENGINX_INSTALL' does not match the installed $prod $AU_INSTALLED; menu 4 would switch product, so auto-update leaves nginx alone"
      return
    fi
    [[ "$FREENGINX_INSTALL" = [yY] ]] && skipc=freenginx
    if [[ -n "$AU_NGINX_PIN" ]]; then
      # a mistyped hold keeps the installed version (a due rebuild still runs)
      AU_TARGET=$AU_NGINX_PIN
      au_is_ver "$AU_TARGET" || AU_TARGET=$AU_INSTALLED
    elif [[ "$AUTOUPDATE_NGINX_TARGET" = latest ]]; then
      # the newest release on the installed branch, mainline (odd minor
      # version) or stable (even), that is not on the skip list
      parity=$(au_branch "$AU_INSTALLED")
      releases=$(au_nginx_releases) || releases=''
      # menu 4 downloads the nginx source from the same site (it deletes any
      # cached tarball first), so no list means no build either
      [[ -n "$releases" ]] || { AU_ACTION=skip AU_WHY="could not get the release list from ${skipc}.org, which menu 4 also downloads from"; return; }
      for rel in $releases; do
        [[ "$(au_branch "$rel")" = "$parity" ]] || continue
        if why=$(au_skip_reason "$skipc" "$rel"); then
          AU_SKIPNOTE="${AU_SKIPNOTE:+$AU_SKIPNOTE; }$skipc $rel is on the skip list: $why"
          continue
        fi
        AU_TARGET=$rel
        break
      done
      # nothing newer that is allowed: stay on the installed version
      au_is_ver "$AU_TARGET" || AU_TARGET=$AU_INSTALLED
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
    [[ -x "$AU_PHPFPM_BIN" ]] || { AU_ACTION=skip AU_WHY="PHP-FPM is not installed ($AU_PHPFPM_BIN)"; return; }
    AU_INSTALLED=$(au_php_installed)
    if ! au_is_ver "$AU_INSTALLED"; then
      AU_ACTION=skip AU_WHY="installed PHP version '$AU_INSTALLED' is not a release build"; return
    fi
    if [[ "$AU_PHP_HOLD_INVALID" = y || ( -n "$AUTOUPDATE_PHP_HOLD" && "$AUTOUPDATE_PHP_HOLD" = "$AU_INSTALLED" ) ]]; then
      # explicit admin choice: autoupdate.sh run does not override it
      if [[ -n "$fkey" && "$AU_DRYRUN" != y ]]; then
        review=$(au_failure_review php "$ftime")
        # as for nginx: a failed upgrade to a version not installed is not pending
        [[ "$review" = keep && "$fkey" =~ ^php\ ([0-9]+\.[0-9]+\.[0-9]+)$ && "${BASH_REMATCH[1]}" != "$AU_INSTALLED" ]] && review=clear
        if [[ "$review" = clear ]]; then
          au_clear_failed php
          au_log "php: cleared failed update $fkey under AUTOUPDATE_PHP_HOLD, php $AU_INSTALLED is healthy"
        fi
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
    else
      # a due rebuild or a repair needs no release data
      AU_TARGET=$AU_INSTALLED nodata="could not get PHP $branch release data from php.net"
    fi
  fi
  # never install a version on the skip list; a due rebuild still runs
  if au_vergt "$AU_TARGET" "$AU_INSTALLED" && why=$(au_skip_reason "$skipc" "$AU_TARGET"); then
    AU_SKIPNOTE="$skipc $AU_TARGET is on the skip list: $why"
    AU_TARGET=$AU_INSTALLED
  fi
  if au_vergt "$AU_TARGET" "$AU_INSTALLED"; then
    AU_ACTION=upgrade AU_KEY="$comp $AU_TARGET"
    AU_WHY="$AU_INSTALLED -> $AU_TARGET"
    [[ "$AU_REL_SECURITY" = y ]] && AU_WHY="$AU_WHY (security release)"
    [[ -n "$AU_SKIPNOTE" ]] && AU_WHY="$AU_WHY; $AU_SKIPNOTE"
    # the upgrade builds from the same code, so it also covers a due rebuild
    [[ -n "$due" ]] && AU_EPOCH=${due%% *}
  elif [[ -n "$due" ]]; then
    AU_ACTION=rebuild AU_TARGET=$AU_INSTALLED AU_EPOCH=${due%% *}
    AU_KEY="$comp rebuild $AU_EPOCH"
    AU_WHY="rebuild $AU_INSTALLED: ${due#* }"
    [[ -n "$AU_SKIPNOTE" ]] && AU_WHY="$AU_WHY; $AU_SKIPNOTE"
  elif [[ -n "$fkey" ]]; then
    # nothing new pending, but the last update failed
    review=$(au_failure_review "$comp" "$ftime")
    why="was rebuilt since"
    # an upgrade that never replaced the running build, or was rolled back, to
    # a version that is no longer the target (skip list, hold): nothing to
    # hold. Without release data the target is unknown, so keep the record.
    if [[ -z "$nodata" && "$review" = keep && "$fkey" =~ ^[a-z]+\ ([0-9]+\.[0-9]+\.[0-9]+)$ && "${BASH_REMATCH[1]}" != "$AU_INSTALLED" ]]; then
      review=clear why="is no longer pending"
    fi
    # a failed rebuild that never replaced the binary, whose list entry was
    # since withdrawn (it would still be due otherwise): nothing to hold
    if [[ "$review" = keep && "$fkey" =~ \ rebuild\ ([0-9]+)$ ]]; then
      repoch=${BASH_REMATCH[1]} rbin=$AU_NGINX_BIN
      [[ "$comp" = php ]] && rbin=$AU_PHPFPM_BIN
      (( $(au_build_time "$comp" "$rbin") < repoch )) && review=clear why="is no longer pending (its rebuild list entry was withdrawn)"
    fi
    if [[ "$review" = clear ]]; then
      AU_WHY="$AU_INSTALLED is current; the failed update $fkey $why and $comp is healthy"
      [[ -n "$AU_SKIPNOTE" ]] && AU_WHY="$AU_WHY; $AU_SKIPNOTE"
      if [[ "$AU_DRYRUN" != y ]]; then
        au_clear_failed "$comp"
        au_log "$comp: cleared failed update $fkey, it $why and $comp $AU_INSTALLED is healthy"
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
  elif [[ -n "$nodata" ]]; then
    AU_ACTION=skip AU_WHY=$nodata
    return
  else
    if au_vergt "$AU_INSTALLED" "$AU_TARGET"; then
      AU_WHY="installed $AU_INSTALLED is newer than target $AU_TARGET, not downgrading"
    else
      AU_WHY="$AU_INSTALLED is current"
    fi
    [[ "$comp" = nginx && -n "$AU_NGINX_PIN" ]] && AU_WHY="$AU_WHY (held by $AU_NGINX_PIN_VAR='$AU_NGINX_PIN' in custom_config.inc)"
    [[ -n "$AU_SKIPNOTE" ]] && AU_WHY="$AU_WHY; $AU_SKIPNOTE"
    return
  fi
  # a failed update left the service down: the new build is what brings it
  # back, so the stopped-service gate in au_blocked must not hold it
  if [[ -n "$fkey" ]] && ! au_healthy "$comp"; then
    AU_REPAIR=y
  fi
  au_plan_gate
  # an upgrade held back (failed before, or soaking) must not hold back a
  # due same-version rebuild: build the installed version now
  if [[ ( "$AU_ACTION" = held || "$AU_ACTION" = wait ) && "$AU_KEY" = "$comp $AU_TARGET" && -n "$due" ]]; then
    upwhy=$AU_WHY
    AU_ACTION=rebuild AU_TARGET=$AU_INSTALLED AU_EPOCH=${due%% *}
    AU_KEY="$comp rebuild $AU_EPOCH"
    AU_WHY="rebuild $AU_INSTALLED: ${due#* } (upgrade $upwhy)"
    au_plan_gate
  fi
}

# Retry gate and soak delay for the update au_plan picked (uses its locals).
au_plan_gate() {
  local seen age gkey=$fkey ghead=$fhead gtime=$ftime gcount=$fcount rkey rhead rtime rcount
  # the planned key's own record, in either file (au_fail_record)
  if [[ "$gkey" != "$AU_KEY" && -f "$AU_STATE_DIR/failed-$comp-rebuild" ]]; then
    IFS='|' read -r rkey rhead rtime rcount < "$AU_STATE_DIR/failed-$comp-rebuild"
    [[ "$rkey" = "$AU_KEY" ]] && gkey=$rkey ghead=$rhead gtime=$rtime gcount=$rcount
  fi
  # A failed update is retried only after the Centmin Mod code changed and a
  # day has passed, at most AU_MAX_ATTEMPTS times; reset or run override it.
  # An hour of slack lets the next day's run at the same minute qualify.
  if [[ "$gkey" = "$AU_KEY" && "$AU_FORCE" != y ]]; then
    [[ "$gcount" =~ ^[0-9]+$ ]] || gcount=1
    if (( gcount >= AU_MAX_ATTEMPTS )); then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $gcount times, last $(au_date "$gtime"); run autoupdate.sh reset to try again"
      return
    elif [[ "$ghead" = "$(au_head_full)" ]]; then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $(au_date "$gtime"), waiting for a Centmin Mod code update (cmupdate) or autoupdate.sh reset"
      return
    elif (( $(date +%s) - gtime < 86400 - 3600 )); then
      AU_ACTION=held AU_WHY="$AU_WHY: failed $(au_date "$gtime"), retrying 24 hours after that"
      return
    fi
  fi
  # soak time for new versions; security releases and rebuilds go ahead.
  # Half an hour of slack, as above.
  if [[ "$AU_ACTION" = upgrade && "$AU_REL_SECURITY" != y && "$AU_FORCE" != y ]]; then
    seen="$AU_STATE_DIR/seen-$comp-$AU_TARGET"
    if [[ -f "$seen" ]]; then
      age=$(( $(date +%s) - $(stat -c %Y "$seen") ))
    else
      age=0
      [[ "$AU_DRYRUN" = y ]] || touch "$seen"
    fi
    if (( age + 1800 < AUTOUPDATE_DELAY_HOURS * 3600 )); then
      AU_ACTION=wait AU_WHY="$AU_WHY: first seen $(( age / 3600 ))h ago, waiting until AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h"
    fi
  fi
}

# True when an enabled component has an upgrade or rebuild ready to start.
# Changes no state.
au_work_due() {
  local comp AU_DRYRUN=y
  for comp in nginx php; do
    [[ "$comp" = nginx && "$AUTOUPDATE_NGINX" != [yY] ]] && continue
    [[ "$comp" = php && "$AUTOUPDATE_PHP" != [yY] ]] && continue
    au_plan "$comp"
    [[ "$AU_ACTION" = upgrade || "$AU_ACTION" = rebuild ]] && return 0
  done
  return 1
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
  # the usual `centmin` command is /usr/bin/centmin sourcing centmin.sh, so
  # its argv names only the wrapper
  for pid in $(pgrep -U 0 -u 0 -f 'centmin|cmupdate'); do
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
        # the wrapper runs as /bin/bash /usr/bin/centmin; a bare "centmin" is
        # more likely a screen/tmux session name
        centmin) [[ "$arg" = */centmin ]] && { echo "centmin (pid $pid)"; return 0; } ;;
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
    # -g: [] and {} in a URL are literal, not curl ranges
    printf '%s=%s ' "$u" "$(curl -${ipv_forceopt}gsk -o /dev/null --connect-timeout 5 --max-time 20 -w '%{http_code}' "$u")"
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
# checks. Returns 0 when the previous version $3 is serving again, 1 when a
# restore step or nginx -t on the restored files failed (nginx not restarted,
# files possibly half restored), 2 when this run took no
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

# Drop a failure record and the pre-failure baseline au_update kept with it.
au_clear_failed() {
  rm -rf "$AU_STATE_DIR/failed-$1" "$AU_STATE_DIR/failed-$1-rebuild" "$AU_STATE_DIR/baseline-$1"
}

# failed-<comp> holds "key|git head|time|attempts"
# Two records so an upgrade and a rebuild cannot reset each other's retry
# gate by taking turns: failed-<comp> (upgrades, repairs, rebuilds on their
# own) and failed-<comp>-rebuild (a rebuild failing while a failed upgrade to a
# newer version is on record). A key keeps the file it already has.
au_fail_record() {
  local comp=$1 key=$2 f="$AU_STATE_DIR/failed-$1" r="$AU_STATE_DIR/failed-$1-rebuild" fkey fhead ftime fcount=0 uver inst
  IFS='|' read -r fkey _ 2>/dev/null < "$f"
  if [[ -f "$r" && "$(cut -d'|' -f1 "$r" 2>/dev/null)" = "$key" ]]; then
    f=$r
  elif [[ "$key" = *" rebuild "* && "$fkey" =~ ^[a-z]+\ ([0-9]+\.[0-9]+\.[0-9]+)$ ]]; then
    uver=${BASH_REMATCH[1]}
    inst=$(au_nginx_installed)
    [[ "$comp" = php ]] && inst=$(au_php_installed)
    au_vergt "$uver" "$inst" && f=$r
  elif [[ "$key" != *" rebuild "* && "$fkey" = *" rebuild "* && "$fkey" != "$key" ]]; then
    # an upgrade failing beside a failed rebuild: the rebuild keeps its record
    mv -f "$f" "$r"
  fi
  fkey='' fcount=0
  IFS='|' read -r fkey fhead ftime fcount 2>/dev/null < "$f"
  [[ "$fkey" = "$key" && "$fcount" =~ ^[0-9]+$ ]] || fcount=0
  echo "$key|$(au_head_full)|$(date +%s)|$((fcount + 1))" > "$f"
  echo $((fcount + 1))
}

au_update() {
  local comp=$1 start runlog clilog rc now problems='' urls_before mods_before mods_after missing
  local warn_before warn_after warn level title msg rolled='' menu attempt bin replaced=n rcprob='' next fkey pair pairs saved code
  local base="$AU_STATE_DIR/baseline-$comp"
  start=$(date +%s)
  runlog="${CENTMINLOGDIR}/autoupdate_${comp}_$(date +%d%m%y-%H%M%S).log"
  mkdir -p "$CENTMINLOGDIR" "$AU_STATE_DIR"
  # without its state (disk full, read-only /var) a failing build would repeat hourly
  if ! { echo "$comp|$AU_KEY|$start" > "$AU_STATE_DIR/inprogress" && [[ "$(cat "$AU_STATE_DIR/inprogress" 2>/dev/null)" = "$comp|$AU_KEY|$start" ]]; }; then
    au_say "$comp: cannot write $AU_STATE_DIR (disk full or read-only?), not building"
    return 1
  fi
  au_say "$comp: starting $AU_ACTION ($AU_WHY), log $runlog"
  bin=$AU_NGINX_BIN
  [[ "$comp" = php ]] && bin=$AU_PHPFPM_BIN
  if [[ -f "$AU_STATE_DIR/failed-$comp" && -d "$base" ]]; then
    # a failed build may still be live: compare with what was there before
    # that failure, so its problems do not become the baseline
    mods_before=$(cat "$base/mods" 2>/dev/null)
    warn_before=$(cat "$base/warn" 2>/dev/null)
    # the URLs checked now: their saved code, or one measured now if added since
    # (a URL removed since must not count as a regression)
    saved=" $(cat "$base/urls" 2>/dev/null) " urls_before=''
    read -ra pairs <<< "$AUTOUPDATE_CHECK_URLS"
    for pair in "${pairs[@]}"; do
      if [[ "$saved" = *" $pair="* ]]; then
        code=${saved#* "$pair"=} code=${code%% *}
        urls_before="$urls_before $pair=$code"
      else
        urls_before="$urls_before $(AUTOUPDATE_CHECK_URLS=$pair au_url_codes)"
      fi
    done
  else
    urls_before=$(au_url_codes)
    if [[ "$comp" = php ]]; then
      mods_before=$(php -m 2>/dev/null | sort -u)
      warn_before=$(php -m 2>&1 | grep -Ei 'warning|fatal|error' | sort -u)
    fi
    rm -rf "$base"
    if ! { mkdir -p "$base" && printf '%s\n' "$urls_before" > "$base/urls" && printf '%s\n' "$mods_before" > "$base/mods" && printf '%s\n' "$warn_before" > "$base/warn"; }; then
      rm -f "$AU_STATE_DIR/inprogress"
      au_say "$comp: cannot write $base (disk full or read-only?), not building"
      return 1
    fi
  fi
  read -ra pairs <<< "$urls_before"
  for pair in "${pairs[@]}"; do
    [[ "${pair##*=}" =~ ^[1-4][0-9][0-9]$ ]] || au_log "$comp: ${pair%=*} answered '${pair##*=}' before the update, so it is not checked"
  done

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
      *) rolled=" Restoring ${NGINXBACKUPDIR}/upgrade-snapshot failed (a copy or move step, or nginx -t on the restored files; see the log), so nginx was not restarted: the new build is still running and the files on disk may be partly restored. Do not restart nginx until this is fixed." ;;
    esac
  fi
  [[ -n "$menu" ]] && rolled="$rolled Menu reported: $menu"
  problems=$(printf '%s %s' "$rcprob" "$problems" | sed 's/^ *//; s/; *$//')
  # the build is over: an interruption from here on must not count as a failed build
  rm -f "$AU_STATE_DIR/inprogress"
  if [[ -z "$problems" ]]; then
    level=ok
    title="$comp $AU_TARGET $([[ "$AU_ACTION" = rebuild ]] && echo rebuilt || echo installed)"
    msg="Auto-update $AU_ACTION of $comp finished in $(( ($(date +%s) - start) / 60 )) min: $AU_WHY."
    [[ -f "$AU_STATE_DIR/failed-$comp" ]] && IFS='|' read -r fkey _ < "$AU_STATE_DIR/failed-$comp"
    if [[ "$AU_ACTION" = rebuild && "$fkey" =~ ^[a-z]+\ ([0-9]+\.[0-9]+\.[0-9]+)$ ]] && au_vergt "${BASH_REMATCH[1]}" "$AU_TARGET"; then
      # a rebuild of the installed version says nothing about a failed
      # upgrade to a newer one: keep its record for the retry gate
      rm -rf "$base" "$AU_STATE_DIR/failed-$comp-rebuild"
    else
      au_clear_failed "$comp"
    fi
    [[ -n "$AU_EPOCH" ]] && echo "$AU_EPOCH" > "$AU_STATE_DIR/rebuilt-$AU_REBUILD_COMP"
    echo "$(stat -c %Y "$bin") $(au_head_time)" > "$AU_STATE_DIR/built-$comp"
  else
    level=fail
    title="$comp $AU_TARGET auto-update FAILED"
    attempt=$(au_fail_record "$comp" "$AU_KEY")
    now=$(au_nginx_installed)
    [[ "$comp" = php ]] && now=$(au_php_installed)
    if [[ "$(stat -c %Y "$bin" 2>/dev/null || echo 0)" -ge "$start" && "$now" = "$AU_TARGET" ]] && au_healthy "$comp"; then
      # what au_failure_review calls keep: no automatic retry
      next="The failed build stays in use: auto-update holds it until you fix it and run autoupdate.sh reset, or until a newer release or listed rebuild replaces it (checked against the state from before this failure)"
    elif (( attempt >= AU_MAX_ATTEMPTS )); then
      next="That was attempt $attempt of $AU_MAX_ATTEMPTS, so it is not tried again until: autoupdate.sh reset"
    else
      next="Attempt $attempt of $AU_MAX_ATTEMPTS: tried again after a Centmin Mod code update and 24 hours, or right away after: autoupdate.sh reset"
    fi
    msg="Auto-update $AU_ACTION of $comp ($AU_WHY) failed: ${problems}.${rolled} nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO). $next"
  fi
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
  local comp=$1 reason=$2 f="$AU_STATE_DIR/pending-$1" since what
  [[ -f "$f" ]] || date +%s > "$f"
  since=$(cat "$f" 2>/dev/null)
  # an unwritable state dir must not turn this into an hourly alert
  [[ "$since" =~ ^[0-9]+$ ]] || return 0
  if (( $(date +%s) - since > 72 * 3600 )) && [[ ! -f "$f.notified" ]]; then
    case "$comp" in
      run) what="Auto-update runs have been deferred" ;;
      cmupdate) what="The cmupdate step of auto-update has not succeeded" ;;
      *) what="Auto-update for $comp has not been able to proceed" ;;
    esac
    au_notify fail "$comp auto-update blocked" "$what since $(au_date "$since"): $reason" && touch "$f.notified"
  fi
}

# Returns 2 when the code tree is unusable for a build.
au_cmupdate() {
  local stamp="$AU_STATE_DIR/cmupdate.stamp" before after rc why='' failed
  local moved="${CM_INSTALLDIR}-automoved-cmupdate" lc="$AU_STATE_DIR/cmupdate-localchanges" stash_before stash_after
  # an old timer would alert at once when cmupdate is turned back on
  [[ "$AUTOUPDATE_CMUPDATE" = [yY] ]] || { rm -f "$AU_STATE_DIR"/pending-cmupdate*; return 0; }
  [[ "$AU_NO_CMUPDATE" != y ]] || return 0
  # once per hourly run inside the update hours
  [[ -n "$(find "$stamp" -mmin -50 2>/dev/null)" ]] && return 0
  before=$(au_head) stash_before=$(au_git rev-parse -q --verify refs/stash 2>/dev/null)
  # a copy left by an earlier re-clone is not this run's to restore; move it
  # aside, or cmupdate's next re-clone would move the tree into it
  if [[ -e "$moved" ]]; then
    if mv "$moved" "${moved}-$(date +%Y%m%d-%H%M%S)" 2>/dev/null; then
      au_log "cmupdate: moved an earlier $moved aside to ${moved}-<date>, remove it when no longer needed"
    else
      au_say "cmupdate: could not move an earlier $moved aside, so cmupdate is skipped this run (its re-clone would move the tree into it)"
      au_pending cmupdate "$moved could not be moved aside"
      return 0
    fi
  fi
  # no terminal: a git or ssh prompt must fail, not wait for the timeout
  GIT_TERMINAL_PROMPT=0 timeout -k 30 1800 bash "${CM_INSTALLDIR}/tools/cmupdate.sh" update < /dev/null > "$AU_STATE_DIR/cmupdate.log" 2>&1 9>&-
  rc=$?
  # cmupdate's re-clone moves the tree aside first and puts it back itself on
  # a failed clone; a timeout kill skips that, so put it back here unless the
  # new clone finished
  if [[ "$rc" -eq 124 || "$rc" -eq 137 ]] && [[ -d "$moved" ]]; then
    if au_git rev-parse --verify -q HEAD >/dev/null && au_git diff --quiet HEAD; then
      au_say "cmupdate: timed out after re-cloning, kept the new ${CM_INSTALLDIR}; the previous copy is in $moved"
    else
      rm -rf "$CM_INSTALLDIR" && mv "$moved" "$CM_INSTALLDIR"
      au_say "cmupdate: timed out while re-cloning, restored the previous ${CM_INSTALLDIR}"
    fi
  fi
  after=$(au_head)
  if [[ -z "$after" ]]; then
    au_say "cmupdate: ${CM_INSTALLDIR} is not a git checkout after cmupdate, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "${CM_INSTALLDIR} is not a git checkout after cmupdate"
    return 2
  fi
  # cmupdate does not pass git_sync's status on: its log says whether it failed
  # (the git_sync messages matched here come from inc/git_sync.inc)
  failed=n
  # anchored: cmupdate also prints the latest commit subject, which may quote them
  [[ "$rc" -ne 0 ]] || grep -qE '^(git_sync: ERROR|Error: (unable|wasn))' "$AU_STATE_DIR/cmupdate.log" && failed=y
  # local changes left in a stash (pop failed, or a history resync): the code
  # is not what the admin runs, so no builds until they deal with it. Only
  # this run's log says so, hence a marker, cleared by reset or once no
  # cmupdate stash is left.
  # (a failed fast-forward also leaves its stash without saying so)
  stash_after=$(au_git rev-parse -q --verify refs/stash 2>/dev/null)
  if grep -qE '^(git_sync: WARNING|  Local changes saved to:)' "$AU_STATE_DIR/cmupdate.log" ||
     { [[ "$stash_after" != "$stash_before" ]] && au_git stash list -n 1 2>/dev/null | grep -q 'cmupdate-'; }; then
    [[ -f "$lc" ]] || date +%s > "$lc"
  elif [[ -f "$lc" ]] && ! au_git stash list 2>/dev/null | grep -q 'cmupdate-'; then
    rm -f "$lc" "$lc.sent"
    au_log "cmupdate: the stashed local changes were dealt with, builds resume"
  fi
  # alerted until delivered
  if [[ -f "$lc" && ! -f "$lc.sent" ]]; then
    au_notify fail "auto-update stopped: local code changes" "cmupdate could not keep your local changes in ${CM_INSTALLDIR} applied; see git stash list in ${CM_INSTALLDIR}. Auto-update builds nothing until you re-apply or drop them (then: bash ${CM_INSTALLDIR}/tools/autoupdate.sh reset)." && touch "$lc.sent"
  fi
  # a tree that is not what cmupdate meant to leave is never built from
  if au_git ls-files -u | grep -q .; then
    why="${CM_INSTALLDIR} has unmerged files after cmupdate"
  elif [[ -e "${CM_INSTALLDIR}/.git/index.lock" ]]; then
    why="a git operation in ${CM_INSTALLDIR} was cut off (.git/index.lock is left)"
  elif [[ -f "$lc" ]]; then
    why="cmupdate could not keep local changes in ${CM_INSTALLDIR} applied (git stash list there, then autoupdate.sh reset)"
  elif [[ "$failed" = y && "$before" != "$after" ]] && ! au_git merge-base --is-ancestor "$before" "$after" 2>/dev/null; then
    why="after a failed cmupdate ${CM_INSTALLDIR} is at $after, not a newer version of $before"
  fi
  if [[ -n "$why" ]]; then
    au_say "cmupdate: $why, not building, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "$why"
    return 2
  fi
  if [[ "$failed" = y ]]; then
    # retried on the next run; the build goes ahead with the current code
    au_say "cmupdate: failed (exit $rc), continuing with the code at $after, see $AU_STATE_DIR/cmupdate.log"
    au_pending cmupdate "cmupdate keeps failing, see $AU_STATE_DIR/cmupdate.log"
    # it may still have moved the code forward: then use the new script
    [[ "$before" = "$after" ]] && return 0
  else
    touch "$stamp"
    rm -f "$AU_STATE_DIR"/pending-cmupdate*
    if [[ "$before" = "$after" ]]; then
      au_log "cmupdate: code unchanged at $after"
      return 0
    fi
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
  # the sidecar is left behind by earlier runs: only a live autoupdate pid counts
  [[ "$started" =~ ^[0-9]+$ && "$pid" =~ ^[0-9]+$ ]] && grep -qa autoupdate "/proc/$pid/cmdline" 2>/dev/null || return 0
  if (( $(date +%s) - started > AUTOUPDATE_STUCK_HOURS * 3600 )) && [[ ! -f "$AU_STATE_DIR/stuck-$started" ]]; then
    # only when the marker can be kept (else this repeats hourly); removed again if unsent
    touch "$AU_STATE_DIR/stuck-$started" 2>/dev/null || return 0
    au_notify fail "auto-update run stuck" "An autoupdate run (pid $pid) has been running since $(au_date "$started"). Check it with: ps -fp $pid; tail $(ls -t "${CENTMINLOGDIR}"/autoupdate_*.log 2>/dev/null | head -n1)" || rm -f "$AU_STATE_DIR/stuck-$started"
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
  # set by the cmupdate re-exec, which keeps the lock on fd 9
  [[ "$AU_LOCKED" = y && /proc/$$/fd/9 -ef "$AU_LOCK" ]] && return 0
  AU_LOCKED=n
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
  # a few seconds covers the probes cpcheck, cmupdate and logins make
  if ! flock -w 10 9; then
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
  # another root run may have replaced a wrong-mode lock file meanwhile
  if [[ ! /proc/$$/fd/9 -ef "$AU_LOCK" ]]; then
    exec 9>&-
    au_log "skip: $AU_LOCK was replaced while it was being locked"
    return 1
  fi
  echo "$$ $(date +%s)" > "${AU_LOCK}.info"
  rm -f "$AU_STATE_DIR"/stuck-*
  return 0
}

# $1 = cron (scheduled: hours window and delay apply) or run (manual)
# The previous run died mid-build (reboot, OOM, kill): hold that update like
# a failure so a build that takes the server down is not repeated. Needs the lock.
au_interrupted() {
  local icomp ikey why
  [[ -f "$AU_STATE_DIR/inprogress" ]] || return 0
  # the run died but its build (centmin-cli.sh) may still be going: wait for it
  if why=$(au_cmm_running); then
    au_log "inprogress is left but $why is still running, recording it once that ends"
    return 1
  fi
  IFS='|' read -r icomp ikey _ < "$AU_STATE_DIR/inprogress"
  rm -f "$AU_STATE_DIR/inprogress"
  [[ "$icomp" = nginx || "$icomp" = php ]] && au_fail_record "$icomp" "$ikey" >/dev/null
  au_notify fail "auto-update interrupted" "A previous auto-update run was interrupted mid-build ($icomp: $ikey); it is held like a failed update. Check that nginx and php-fpm are healthy. nginx running: $(au_service_active nginx && echo yes || echo NO), php-fpm running: $(au_service_active php-fpm && echo yes || echo NO)."
}

au_run() {
  local mode=$1 comp rc=0 why
  [[ "$(id -u)" -eq 0 ]] || { echo "autoupdate.sh must run as root"; return 1; }
  # only the cmupdate re-exec, which still holds the lock on fd 9, is "locked"
  [[ "$AU_LOCKED" = y && /proc/$$/fd/9 -ef "$AU_LOCK" ]] || AU_LOCKED=n
  mkdir -p "$AU_STATE_DIR" "$CENTMINLOGDIR"
  if ! au_enabled; then
    [[ "$mode" = run ]] && echo "AUTOUPDATE_NGINX and AUTOUPDATE_PHP are both 'n', nothing to do"
    return 0
  fi
  if [[ "$mode" = cron && "$AU_LOCKED" != y ]]; then
    au_check_stuck
    if ! au_in_hours; then
      # record a dead run within the hour, not at the next window: a manual
      # rebuild in between would otherwise look older than the failure
      [[ -f "$AU_STATE_DIR/inprogress" ]] && ! au_lock_held && au_take_lock && au_interrupted
      return 0
    fi
  fi
  [[ -n "$AU_CONFIG_WARN" ]] && au_say "config: $AU_CONFIG_WARN"
  au_take_lock || return 0
  # centmin.sh and cmupdate refuse to start while the lock is held; let ours run
  export CMM_AUTOUPDATE_RUN=y
  au_interrupted
  # before cmupdate: it must not change the code under a running centmin.sh
  if why=$(au_cmm_running); then
    au_say "deferred, $why is running"
    # alert only when an update is actually waiting on it
    if au_work_due; then
      au_pending run "$why is running"
    else
      rm -f "$AU_STATE_DIR"/pending-run*
    fi
    return 0
  fi
  rm -f "$AU_STATE_DIR"/pending-run*
  au_cmupdate
  [[ $? -eq 2 ]] && return 1
  [[ "$mode" = run ]] && AU_FORCE=y
  for comp in nginx php; do
    if [[ "$comp" = nginx && "$AUTOUPDATE_NGINX" != [yY] ]] || [[ "$comp" = php && "$AUTOUPDATE_PHP" != [yY] ]]; then
      # an old timer would alert at once when the component is turned back on
      rm -f "$AU_STATE_DIR/pending-$comp"*
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
      *)
        # held with the service down: one more alert after 72 hours
        if [[ "$AU_ACTION" = held ]] && ! au_healthy "$comp"; then
          au_pending "$comp" "$AU_WHY; $comp is not running"
        else
          rm -f "$AU_STATE_DIR/pending-$comp"*
        fi
        au_say "$comp: $AU_ACTION: $AU_WHY"; continue ;;
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
    # a component turned off is planned only to show what it would do
    [[ "$enabled" = [yY] ]] || AU_ACTION="$AU_ACTION if turned on"
    printf '%-6s AUTOUPDATE_%s=%s installed=%s target=%s -> %s: %s\n' "$comp" "$(echo "$comp" | tr a-z A-Z)" "$enabled" "${AU_INSTALLED:--}" "${AU_TARGET:--}" "$AU_ACTION" "$AU_WHY"
    printf '%-6s gates: %s\n' '' "${why:-ok}"
  done
  echo "hours  AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS now $(date +%H:%M) -> $(au_in_hours && echo inside || echo outside)"
}

# How email goes out, as au_email decides it (for status and notify-test).
au_email_path() {
  [[ -n "$AUTOUPDATE_EMAIL" ]] || { echo none; return; }
  if [[ "$EMAILNOTIFY_SES" = [yY] && -n "$EMAILNOTIFY_SES_SMTP_SERVER" && -n "$EMAILNOTIFY_SES_SMTP_USERNAME" ]]; then
    if curl -V 2>/dev/null | grep -qw smtp; then
      echo "smtp ${EMAILNOTIFY_SES_SMTP_SERVER}:${EMAILNOTIFY_SES_SMTP_PORT:-587}"
      return
    fi
    printf '%s' "(curl has no SMTP, for it: dnf install --allowerasing curl) "
  fi
  if command -v mail >/dev/null 2>&1; then echo "local mail"
  elif [[ -x /usr/sbin/sendmail ]]; then echo "sendmail"
  else echo "nothing (no mail or sendmail command)"; fi
}

au_status() {
  local comp f
  echo "Settings: AUTOUPDATE_NGINX=$AUTOUPDATE_NGINX AUTOUPDATE_PHP=$AUTOUPDATE_PHP AUTOUPDATE_NGINX_TARGET=$AUTOUPDATE_NGINX_TARGET AUTOUPDATE_HOURS=$AUTOUPDATE_HOURS AUTOUPDATE_DELAY_HOURS=$AUTOUPDATE_DELAY_HOURS AUTOUPDATE_CMUPDATE=$AUTOUPDATE_CMUPDATE"
  [[ -n "$AU_CONFIG_WARN" ]] && echo "Config:   $AU_CONFIG_WARN"
  [[ -n "$AU_NGINX_PIN" ]] && echo "nginx hold: $AU_NGINX_PIN_VAR='$AU_NGINX_PIN' in custom_config.inc, updates stop at that version (installed nginx $(au_nginx_installed))"
  [[ -n "$AUTOUPDATE_PHP_HOLD" ]] && echo "PHP hold: AUTOUPDATE_PHP_HOLD=$AUTOUPDATE_PHP_HOLD, installed PHP $(au_php_installed) -> $([[ "$AU_PHP_HOLD_INVALID" = y || "$AUTOUPDATE_PHP_HOLD" = "$(au_php_installed)" ]] && echo held || echo 'not held, versions differ')"
  if [[ -f "$AU_SKIPS" ]] && grep -qvE '^[[:space:]]*(#|$)' "$AU_SKIPS"; then
    echo "Skip list: $(grep -vE '^[[:space:]]*(#|$)' "$AU_SKIPS" | awk '{print $1, $2}' | paste -sd, - | sed 's/,/, /g') (tools/autoupdate-skip.txt)"
  fi
  echo "Notify:   AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY email=${AUTOUPDATE_EMAIL:-none} via $(au_email_path) pushover=$([[ "$AUTOUPDATE_PUSHOVER" = [yY] && -n "$PUSH_API_TOKEN" && -n "$PUSH_USER_KEY" ]] && echo on || echo off)"
  if [[ -f "$AU_CRON_FILE" ]]; then
    echo "Cron:     $AU_CRON_FILE: $(grep -v '^#' "$AU_CRON_FILE" | grep autoupdate.sh)"
  else
    echo "Cron:     $AU_CRON_FILE not present$(au_enabled && echo ' (run: autoupdate.sh ensure)')"
  fi
  [[ -f "$AU_STATE_DIR/cmupdate-localchanges" ]] && echo "Stopped:  cmupdate left local code changes in a stash $(au_date "$(cat "$AU_STATE_DIR/cmupdate-localchanges")"); no builds until they are dealt with (git stash list in ${CM_INSTALLDIR}, then autoupdate.sh reset)"
  if [[ -f "$AU_STATE_DIR/inprogress" ]]; then
    if au_lock_held; then
      echo "Running:  $(cut -d'|' -f1,2 "$AU_STATE_DIR/inprogress") since $(au_date "$(cut -d'|' -f3 "$AU_STATE_DIR/inprogress")")"
    else
      echo "Interrupted: $(cut -d'|' -f1,2 "$AU_STATE_DIR/inprogress") started $(au_date "$(cut -d'|' -f3 "$AU_STATE_DIR/inprogress")") and died; the next cron run holds it as failed"
    fi
  fi
  for comp in nginx php; do
    [[ -f "$AU_STATE_DIR/last-$comp" ]] && echo "Last $comp: $(cat "$AU_STATE_DIR/last-$comp")"
    for f in "$AU_STATE_DIR/failed-$comp" "$AU_STATE_DIR/failed-$comp-rebuild"; do
      [[ -f "$f" ]] && echo "Held $comp: $(cut -d'|' -f1 "$f") failed $(cut -d'|' -f4 "$f") time(s), last at code $(cut -d'|' -f2 "$f")"
    done
  done
  [[ -f "$AU_LOG" ]] && { echo "Recent log ($AU_LOG):"; tail -n 8 "$AU_LOG"; }
}

# the first settings problem, and how many more `check` shows
au_warn_line() {
  local n
  n=$(tr -cd ';' <<< "$AU_CONFIG_WARN" | wc -c)
  printf '%s' "${AU_CONFIG_WARN%%;*}"
  (( n > 1 )) && printf ' (+%s more: autoupdate.sh check)' "$((n - 1))"
}

# one "tag|text" line for the dmotd login banner, or nothing
au_motd() {
  local comp fkey fhead failed parts='' held=''
  AU_ENSURE_BY=dmotd au_ensure
  # a mistyped switch can turn both off: that must not look like "off"
  if ! au_enabled; then
    [[ "$AU_CONFIG_WARN" == *AUTOUPDATE_NGINX=* || "$AU_CONFIG_WARN" == *AUTOUPDATE_PHP=* ]] && echo "warn|Auto-update: $(au_warn_line)"
    return 0
  fi
  if [[ -f "$AU_STATE_DIR/inprogress" ]] && au_lock_held; then
    IFS='|' read -r comp fkey failed < "$AU_STATE_DIR/inprogress"
    echo "warn|Auto-update: $fkey running since $(au_date "$failed") - wait before using centmin.sh menu 4/5"
    return 0
  fi
  for comp in nginx php; do
    # a component turned off is not planned, so its old failure is never reviewed
    [[ "$comp" = nginx && "$AUTOUPDATE_NGINX" != [yY] ]] && continue
    [[ "$comp" = php && "$AUTOUPDATE_PHP" != [yY] ]] && continue
    if [[ -f "$AU_STATE_DIR/failed-$comp" ]]; then
      IFS='|' read -r fkey fhead failed _ < "$AU_STATE_DIR/failed-$comp"
      # "php rebuild <epoch>" -> "php rebuild", "nginx 1.31.8" stays
      [[ "$fkey" =~ ^(.*rebuild)\ [0-9]+$ ]] && fkey=${BASH_REMATCH[1]}
      echo "warn|Auto-update: ${fkey} FAILED $(au_date "$failed") - run: bash ${CM_INSTALLDIR}/tools/autoupdate.sh status"
      return 0
    fi
  done
  if [[ -f "$AU_STATE_DIR/cmupdate-localchanges" ]]; then
    echo "warn|Auto-update: stopped, cmupdate left local code changes in a stash (git stash list in ${CM_INSTALLDIR}), then: autoupdate.sh reset"
    return 0
  fi
  if [[ "$AU_SYSTEMD" = y ]] && ! systemctl is-active --quiet crond; then
    echo "warn|Auto-update: crond is not running, scheduled updates cannot run - systemctl start crond"
    return 0
  fi
  if [[ -n "$AU_CONFIG_WARN" ]]; then
    echo "warn|Auto-update: $(au_warn_line)"
    return 0
  fi
  if [[ "$AUTOUPDATE_NGINX" = [yY] ]]; then
    parts="nginx"
    [[ -n "$AU_NGINX_PIN" ]] && held=", nginx held at $AU_NGINX_PIN"
  fi
  if [[ "$AUTOUPDATE_PHP" = [yY] ]]; then
    parts="${parts:+$parts + }PHP-FPM"
    if [[ -n "$AUTOUPDATE_PHP_HOLD" ]]; then
      if [[ "$AUTOUPDATE_PHP_HOLD" != "$(au_php_installed)" ]]; then
        echo "warn|Auto-update: AUTOUPDATE_PHP_HOLD='$AUTOUPDATE_PHP_HOLD' but PHP $(au_php_installed) is installed, so PHP is not held"
        return 0
      fi
      held="$held, PHP held at $AUTOUPDATE_PHP_HOLD"
    fi
  fi
  echo "info|Auto-update: $parts on${held}, hours $AUTOUPDATE_HOURS"
}

# How auto-update will handle an available nginx|php version ($2, optional),
# for the update-available alerts of dmotd and check-for-updates.sh; nothing
# when it won't apply it by itself.
au_alertnote() {
  local skipc=$1 why inst prod
  [[ "$1" = nginx && "$FREENGINX_INSTALL" = [yY] ]] && skipc=freenginx
  # the callers' version strings come from caches: say nothing about a non-version
  [[ -z "$2" ]] || au_is_ver "$2" || return 0
  case "$1" in
    nginx)
      [[ "$AUTOUPDATE_NGINX" = [yY] ]] || return 0
      # as au_plan: builds that are not nginx.org/freenginx releases are skipped
      inst=$(au_nginx_installed)
      au_is_ver "$inst" || return 0
      case "$(nginx -v 2>&1)" in *freenginx/*) prod=freenginx ;; *) prod=nginx ;; esac
      if [[ "$prod" = freenginx && "$FREENGINX_INSTALL" != [yY] ]] || [[ "$prod" = nginx && "$FREENGINX_INSTALL" = [yY] ]]; then
        echo "auto-update leaves nginx alone: FREENGINX_INSTALL does not match the installed $prod; run centmin.sh menu option 4 to update by hand"
        return 0
      fi
      if [[ -n "$AU_NGINX_PIN" ]] && ! au_is_ver "$AU_NGINX_PIN"; then
        echo "auto-update holds nginx at $inst ($AU_NGINX_PIN_VAR in custom_config.inc is not a valid version); run centmin.sh menu option 4 to update by hand"
        return 0
      elif [[ -n "$AU_NGINX_PIN" && "$2" != "$AU_NGINX_PIN" ]]; then
        echo "auto-update holds nginx at $AU_NGINX_PIN ($AU_NGINX_PIN_VAR in custom_config.inc); run centmin.sh menu option 4 to update by hand"
        return 0
      fi
      if [[ -n "$2" ]] && why=$(au_skip_reason "$skipc" "$2"); then
        echo "auto-update skips this version, Centmin Mod lists it as: $why"
        return 0
      fi
      if [[ -n "$AU_NGINX_PIN" ]]; then
        echo "auto-update installs it ($AU_NGINX_PIN_VAR in custom_config.inc holds nginx there) after AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h, within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS"
      elif [[ "$AUTOUPDATE_NGINX_TARGET" = latest ]]; then
        if au_is_ver "$2" && [[ "$(au_branch "$2")" != "$(au_branch "$inst")" ]]; then
          # dmotd and check-for-updates report the newest release of either branch
          echo "auto-update stays on the $([[ "$(au_branch "$inst")" = 1 ]] && echo mainline || echo stable) branch of nginx $inst; run centmin.sh menu option 4 to change branch"
          return 0
        fi
        echo "auto-update applies it after AUTOUPDATE_DELAY_HOURS=${AUTOUPDATE_DELAY_HOURS}h, within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS"
      else
        echo "auto-update applies it once Centmin Mod's $AU_NGINX_PIN_VAR default reaches it, within AUTOUPDATE_HOURS $AUTOUPDATE_HOURS"
      fi
      ;;
    php)
      [[ "$AUTOUPDATE_PHP" = [yY] ]] || return 0
      inst=$(au_php_installed)
      au_is_ver "$inst" || return 0
      [[ "$AU_PHP_HOLD_INVALID" = y || ( -n "$AUTOUPDATE_PHP_HOLD" && "$AUTOUPDATE_PHP_HOLD" = "$inst" ) ]] && return 0
      # as au_plan: PHP stays on its installed major.minor branch
      if [[ -n "$2" && "${2%.*}" != "${inst%.*}" ]]; then
        echo "auto-update keeps PHP on the ${inst%.*} branch; run centmin.sh menu option 5 to change branch"
        return 0
      fi
      if [[ -n "$2" ]] && why=$(au_skip_reason php "$2"); then
        echo "auto-update skips this version, Centmin Mod lists it as: $why"
        return 0
      fi
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
      [[ "$(id -u)" -eq 0 ]] || { echo "autoupdate.sh ensure must run as root"; return 1; }
      AU_ENSURE_BY=${2:-manual} au_ensure
      ;;
    motd)
      au_motd
      ;;
    alertnote)
      au_alertnote "$2" "$3"
      ;;
    notify-test)
      AU_NOTIFY_FORCE=y au_notify ok "auto-update test notification" "Test notification from $(basename "$0"): AUTOUPDATE_NOTIFY=$AUTOUPDATE_NOTIFY, email=${AUTOUPDATE_EMAIL:-none}."
      rc=$?
      au_status | grep -E '^Notify'
      case "$rc" in
        0) echo "notify-test: sent" ;;
        2) echo "notify-test: no channel is configured (see Notify above)" ;;
        *) echo "notify-test: every configured channel failed, see $AU_LOG" ;;
      esac
      return "$rc"
      ;;
    reset)
      [[ "$(id -u)" -eq 0 ]] || { echo "autoupdate.sh reset must run as root"; return 1; }
      rm -rf "$AU_STATE_DIR"/failed-* "$AU_STATE_DIR"/baseline-* "$AU_STATE_DIR"/pending-* "$AU_STATE_DIR"/stuck-* "$AU_STATE_DIR/cmupdate.stamp" "$AU_STATE_DIR"/cmupdate-localchanges*
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
