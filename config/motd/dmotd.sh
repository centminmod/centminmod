#!/bin/bash
###########################################################
# Installation:
#
# echo "PrintMotd no" >> /etc/ssh/sshd_config
# echo "# session optional pam_motd.so" >> /etc/pam.d/login
# echo "/usr/local/bin/dmotd" >> /etc/profile
# chmod +x /usr/local/bin/dmotd
# 
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
# Capture the inbound user locale BEFORE forcing en_US.UTF-8 below, so that
# _dmotd_fancy_capable() can consult what the user's terminal actually
# supports — not what the script just exported. The fancy capability gate
# is meaningless if it reads the script's own forced en_US.UTF-8.
_dmotd_user_lc_all="${LC_ALL-}"
_dmotd_user_lc_ctype="${LC_CTYPE-}"
_dmotd_user_lang="${LANG-}"
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
###########################################################
DT=$(date +"%d%m%y-%H%M%S")
DMOTD_USER=$(whoami)
DMOTD_HOSTNAME=$(uname -n)
DMOTD_RELEASE=$(cat /etc/redhat-release | tr -d '()' | cut -d' ' -f1,4)
PSA=$(ps -Afl | wc -l)
DMOTD_CURRENTUSER=$(users | wc -w)
CMSCRIPT_GITDIR='/usr/local/src/centminmod'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
FREENGINX_INSTALL='n'        # Use Freenginx fork instead of official Nginx
SSHLOGIN_KERNELCHECK='n'
DMOTD_CVECHECK='y'           # cmsec CVE detection line in dmotd login banner
DMOTD_CVECHECK_SUPPRESS=''   # comma-separated CVE IDs to suppress, e.g. 'CVE-2026-31431'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

# Compact dmotd layout (vertical-space-efficient SSH login banner).
# Master switch defaults to 'n' so existing installs see ZERO change after
# cmupdate. Set DMOTD_COMPACT='y' in /etc/centminmod/custom_config.inc to
# enable the ~28-line compact layout. Per-section sub-toggles below take
# effect only when DMOTD_COMPACT='y' (setting one to 'n' selectively
# restores that section's verbose panel). Two toggles are intentionally
# independent of DMOTD_COMPACT: DMOTD_CSFVERCHECK silences the CSF version
# checker entirely, and DMOTD_CVECHECK_COMPACT controls cmsec output
# collapsing — admins typically want CVE detail visible even when the
# rest of the dmotd is compact.
DMOTD_COMPACT='n'                  # master: 'y' enables compact layout
DMOTD_FANCY='n'                    # 'y' enables Unicode box-drawing + colored ASCII badges + bars
                                   # (requires capable terminal; falls back to compact on incapable).
                                   # Fancy implies compact data internally via _DMOTD_COMPACT_EFFECTIVE
                                   # without mutating the user's DMOTD_COMPACT setting.
DMOTD_CSFVERCHECK='y'              # 'n' silences csf_version_checker entirely
DMOTD_CVECHECK_COMPACT='n'         # 'y' collapses cmsec to 1 summary line (vulnerable CVEs always expand)
ENABLEMOTD_HEADERCOMPACT='y'       # 'n' restores 6-line hostname/users/CPU/proc/uptime header
ENABLEMOTD_MEMCOMPACT='y'          # 'n' restores full free -m output
ENABLEMOTD_DFCOMPACT='y'           # 'n' restores full df -hT (incl. tmpfs/devtmpfs/efivarfs)
ENABLEMOTD_LINKSCOMPACT='y'        # 'n' restores 5-line link list (ENABLEMOTD_LINKSMSG='n' still hides)
ENABLEMOTD_GITCOMPACT='y'          # 'n' restores Centmin Mod git/branch/update panels
ENABLEMOTD_NGINXVERCOMPACT='y'     # 'n' restores ngxver_checker 7-line panel
ENABLEMOTD_PHPVERCOMPACT='y'       # 'n' restores phpver_checker 7-line panel
ENABLEMOTD_CSFVERCOMPACT='y'       # 'n' restores 8-line CSF version panel (DMOTD_CSFVERCHECK='y' still required)
ENABLEMOTD_NEEDRESTARTCOMPACT='y'  # 'n' restores needrestart_check ~6-line reboot panel

# Status-footer accumulator (compact mode appends one line per check)
_dmotd_status_lines=()
# Render-mode signals — set ONCE per login in the main flow below. Never
# read from custom_config.inc; never mutated after the resolution block.
_DMOTD_COMPACT_EFFECTIVE='n'
_DMOTD_FANCY_ACTIVE='n'

# Set cache timeout in minutes
CACHE_TIMEOUT=60
CMSEC_CACHE_TIMEOUT=1440     # cmsec CVE cache TTL (24h); state-based invalidation also applies
# Set cache file path
CACHE_FILE="/tmp/nginx_version_cache"
CACHE_PHP_FILE="/tmp/php_version_cache"

# pushover.net settings
PUSH_VERBOSE='1'
PUSH_LOG_FILE="/var/log/push_dmotd_notify.log"
PUSH_LOGIN_USER="$(whoami)"
PUSH_HOSTNAME="$(hostname)"
PUSH_DATE_TIME="$(date '+%d-%m-%Y %H:%M:%S')"
###########################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}

# Minimum terminal columns required for fancy mode. The 79-char heavy /
# light rules at _fancy_rule_heavy / _fancy_rule_light would visually wrap
# at any width below 80, so the gate floor matches actual rule width. Set
# higher only if a future rule string grows wider.
_DMOTD_FANCY_MIN_COLS=80

# Capability check for DMOTD_FANCY rendering. Returns 0 (capable) when the
# terminal can safely render UTF-8 box-drawing + 8+-color ANSI; returns 1
# (incapable) otherwise so the caller can fall back to compact ASCII.
_dmotd_fancy_capable() {
  # 1. stdout must be a real TTY (cmsec.sh:41 idiom).
  [ -t 1 ] || return 1
  # 2. Locale must be UTF-8. LC_ALL > LC_CTYPE > LANG precedence — first
  #    non-empty value is authoritative. Avoids the concat false-pass
  #    where LC_ALL=C LANG=en_US.UTF-8 would otherwise match.
  #    Reads the user's inbound locale captured at script start (see
  #    L11-17), NOT the en_US.UTF-8 the script just exported.
  local _locale_ok='n' _v
  for _v in "${_dmotd_user_lc_all:-}" "${_dmotd_user_lc_ctype:-}" "${_dmotd_user_lang:-}"; do
    if [ -n "$_v" ]; then
      case "$_v" in
        *[Uu][Tt][Ff]-8*|*[Uu][Tt][Ff]8*) _locale_ok='y' ;;
        *) _locale_ok='n' ;;
      esac
      break
    fi
  done
  [ "$_locale_ok" = 'y' ] || return 1
  # 3. tput must exist (ncurses-base — almost always present, but
  #    minimal containers occasionally omit it).
  command -v tput >/dev/null 2>&1 || return 1
  # 4. Color support: at least 8 colors. tput prints "-1" on stdout
  #    when setupterm fails (e.g. TERM=dumb) — validate numeric first
  #    so the -ge comparison can't error out.
  local _colors
  _colors="$(tput colors 2>/dev/null)"
  case "${_colors:-}" in
    *[!0-9]*|"") return 1 ;;
  esac
  [ "$_colors" -ge 8 ] || return 1
  # 5. TERM sanity — reject dumb / unknown / empty.
  case "${TERM:-dumb}" in
    dumb|unknown|"") return 1 ;;
  esac
  # 6. Terminal width — must be at least _DMOTD_FANCY_MIN_COLS (matches
  #    the 79-char heavy/light rule width above; gate at 80 to keep one
  #    column of margin and avoid edge-case wrap). tput cols may read
  #    winsize from stdin/stderr; redirect from /dev/tty for pam_motd
  #    contexts where stdin isn't the controlling terminal. Probe the
  #    actual PTY first via tput; only fall back to $COLUMNS if tput
  #    cannot determine the width — a stale exported COLUMNS from a
  #    previously-wider terminal would otherwise let fancy activate on
  #    a now-narrower PTY.
  local _cols
  _cols="$(tput cols </dev/tty 2>/dev/null || tput cols 2>/dev/null || echo 0)"
  [ "$_cols" -gt 0 ] 2>/dev/null || _cols="${COLUMNS:-0}"
  case "${_cols:-}" in
    *[!0-9]*|"") return 1 ;;
  esac
  [ "$_cols" -ge "$_DMOTD_FANCY_MIN_COLS" ] || return 1
  return 0
}

# Convert nginx version to integer for comparison (1.29.1 -> 1029001)
nginx_version_to_int() {
    local version=$1
    echo "$version" | awk -F. '{print $1 * 1000000 + $2 * 1000 + $3}'
}

###########################################################
if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    if [ -f /usr/bin/dos2unix ]; then
        dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    fi
    source "${CONFIGSCANBASE}/custom_config.inc"
fi
if [ -f "/etc/centminmod/pushover.ini" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "/etc/centminmod/pushover.ini"
  fi
  source "/etc/centminmod/pushover.ini"
fi
if [[ "$(id -u)" -eq 0 ]]; then
  if [[ -n "$SUDO_USER" ]]; then
    # Script is run with sudo
    CENTMINLOGDIR="/home/$SUDO_USER/centminlogs"
  else
    # Script is run directly as root
    CENTMINLOGDIR='/root/centminlogs'
  fi
else
  # Script is run as a non-root user without sudo
  CENTMINLOGDIR="$HOME/centminlogs"
fi
# Ensure the log directory exists
if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi
if [ -f /etc/almalinux-release ]; then
  DMOTD_RELEASE=$(cat /etc/almalinux-release | tr -d '()' | cut -d' ' -f1,3)
elif [ -f /etc/rockylinux-release ]; then
  DMOTD_RELEASE=$(cat /etc/rockylinux-release | tr -d '()' | cut -d' ' -f1,3)
fi

# time of day
HOUR=$(date +"%H")
if [ $HOUR -lt 12  -a $HOUR -ge 0 ]
then    TIME="morning"
elif [ $HOUR -lt 17 -a $HOUR -ge 12 ] 
then    TIME="afternoon"
else 
    TIME="evening"
fi

#System uptime
uptime=$(cat /proc/uptime | cut -f1 -d.)
upDays=$((uptime/60/60/24))
upHours=$((uptime/60/60%24))
upMins=$((uptime/60%60))
upSecs=$((uptime%60))

#System load
LOADAVG=$(cat /proc/loadavg)
LOAD1=$(echo $LOADAVG | awk {'print $1'})
LOAD5=$(echo $LOADAVG | awk {'print $2'})
LOAD15=$(echo $LOADAVG | awk {'print $3'})

#System Info
MEM=$(free -m)
DF=$(df -hT)

if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

log_message() {
    if [[ "${PUSH_VERBOSE}" -eq 1 ]]; then
        echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> "${PUSH_LOG_FILE}"
    fi
}

check_git_major_branch() {
    local repo_path="$CMSCRIPT_GITDIR"
    local current_branch=$(git --git-dir="$repo_path/.git" --work-tree="$repo_path" rev-parse --abbrev-ref HEAD)
    local branches_to_check=("123.08stable" "123.09beta01" "124.00stable" "130.00beta01" "131.00stable" "140.00beta01")
    local _branch_outdated=0
    for branch in "${branches_to_check[@]}"; do
        [[ "$current_branch" == "$branch" ]] && { _branch_outdated=1; break; }
    done
    if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_GITCOMPACT" != [nN] ]]; then
        # Compact: stash the major-branch warning for the merged footer. The
        # always-printed "branch installed" line is dropped because the
        # status-footer already names the branch via gitenv_askupdate.
        if [[ "$_branch_outdated" -eq 1 ]]; then
            _dmotd_push_status warn " ! Older branch ($current_branch) — newer: 132.00stable or 141.00beta01 (threads/25572)"
        fi
        return
    fi
    echo -n " Current local server Centmin Mod branch installed: "
    cecho "$current_branch " $boldyellow
    cecho "===============================================================================" $boldgreen
    if [[ "$_branch_outdated" -eq 1 ]]; then
        echo -n " Newer Centmin Mod branch version is available: "
        cecho "132.00stable or 141.00beta01" $boldyellow
        echo -n " Details at "
        cecho "https://community.centminmod.com/threads/25572/" $boldyellow
        cecho "===============================================================================" $boldgreen
    fi
}

push_dmotd_alerts() {
  local pushapp="$1"
  local pushapp_ver="$2"
  local pushapp_severity="${3:-}"
  local _cooldown_dir _cooldown_key _cooldown_file _last_ts _now_ts
  local _cve_id _cve_rest _cve_kernel _cve_verdict _cve_severity _push_priority _push_url
  local PUSH_MESSAGE PUSH_TITLE RESPONSE _curl_rc
  # Per-CVE alert cooldown to prevent login-time alert spam.
  # Each (CVE-id, kernel) tuple alerts at most once per CVE_ALERT_COOLDOWN seconds.
  local CVE_ALERT_COOLDOWN="${CVE_ALERT_COOLDOWN:-86400}"
  if [[ "$pushapp" = 'cve' ]]; then
    _cooldown_dir="/var/cache/centminmod/cmsec/push"
    [ -d "$_cooldown_dir" ] || mkdir -p "$_cooldown_dir" 2>/dev/null
    _cooldown_key="$(printf '%s' "$pushapp_ver" | tr '/|: ' '_____')"
    _cooldown_file="${_cooldown_dir}/${_cooldown_key}.last"
    if [ -f "$_cooldown_file" ]; then
      _last_ts=$(stat -c %Y "$_cooldown_file" 2>/dev/null || echo 0)
      _now_ts=$(date +%s)
      if [ "$((_now_ts - _last_ts))" -lt "$CVE_ALERT_COOLDOWN" ]; then
        return 0
      fi
    fi
  fi
  if [[ "$(id -u)" -eq 0 ]] || sudo -n true 2>/dev/null; then
    if [[ "$pushapp" = 'nginx' ]]; then
      PUSH_MESSAGE="nginx ${pushapp_ver} update available, run centmin.sh menu option 4"
      PUSH_TITLE="nginx ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
    elif [[ "$pushapp" = 'php' ]]; then
      PUSH_MESSAGE="php-fpm ${pushapp_ver} update available, run centmin.sh menu option 5"
      PUSH_TITLE="php-fpm ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
    elif [[ "$pushapp" = 'cmm' ]]; then
      PUSH_MESSAGE="centminmod ${pushapp_ver} update available, run cmupdate to update"
      PUSH_TITLE="centminmod ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
    elif [[ "$pushapp" = 'cve' ]]; then
      _cve_id="${pushapp_ver%%|*}"
      _cve_rest="${pushapp_ver#*|}"
      _cve_kernel="${_cve_rest%%|*}"
      _cve_verdict="${_cve_rest#*|}"
      _cve_severity="${pushapp_severity}"
      case "$(printf '%s' "${_cve_severity:-high}" | tr '[:upper:]' '[:lower:]')" in
        critical) _push_priority="${PUSH_CVE_PRIORITY_CRITICAL:-1}" ;;
        high)     _push_priority="${PUSH_CVE_PRIORITY_HIGH:-0}" ;;
        *)        _push_priority="0" ;;
      esac
      _push_url="${PUSH_CVE_URL_TEMPLATE:-https://nvd.nist.gov/vuln/detail/{CVE}}"
      _push_url="${_push_url//\{CVE\}/$_cve_id}"
      PUSH_MESSAGE="${_cve_id} ${_cve_verdict} on ${PUSH_HOSTNAME} (kernel ${_cve_kernel}); run 'cmsec check ${_cve_id}'"
      PUSH_TITLE="${_cve_id} ${_cve_verdict} ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
    fi
    if [[ "$PUSH_MOTD_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      log_message "$PUSH_MESSAGE"

      local _curl_args=(
        --form-string "token=${PUSH_API_TOKEN}"
        --form-string "user=${PUSH_USER_KEY}"
        --form-string "message=${PUSH_MESSAGE}"
        --form-string "title=${PUSH_TITLE}"
      )
      if [[ "$pushapp" = 'cve' ]]; then
        _curl_args+=(--form-string "priority=${_push_priority}")
        _curl_args+=(--form-string "url=${_push_url}")
        _curl_args+=(--form-string "url_title=View CVE Details")
        [ -n "${PUSH_CVE_SOUND:-}" ] && _curl_args+=(--form-string "sound=${PUSH_CVE_SOUND}")
        [ -n "${PUSH_CVE_DEVICE:-}" ] && _curl_args+=(--form-string "device=${PUSH_CVE_DEVICE}")
        [ "${PUSH_CVE_HTML:-0}" = "1" ] && _curl_args+=(--form-string "html=1")
        if [ "${_push_priority}" = "2" ]; then
          _curl_args+=(--form-string "retry=${PUSH_CVE_EMERGENCY_RETRY:-300}")
          _curl_args+=(--form-string "expire=${PUSH_CVE_EMERGENCY_EXPIRE:-3600}")
        fi
      fi
      RESPONSE=$(curl -s "${_curl_args[@]}" https://api.pushover.net/1/messages.json)
      _curl_rc=$?

      log_message "Notification sent. Response: ${RESPONSE}"
      # Update CVE alert cooldown marker ONLY after a verified-successful Pushover
      # delivery — Pushover returns {"status":1,...} on success, {"status":0,...}
      # on auth/format errors. Without this gate, transient network failures or
      # bad credentials would still suppress future alerts for the cooldown window.
      if [[ "$pushapp" = 'cve' ]] && [ -n "${_cooldown_file:-}" ] \
         && [ "${_curl_rc:-1}" -eq 0 ] \
         && printf '%s' "$RESPONSE" | grep -q '"status":1'; then
        touch "$_cooldown_file" 2>/dev/null
      fi
    fi
  fi
}

motd_output() {
# Header block: hostname/users/CPU/proc/uptime
if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_HEADERCOMPACT" != [nN] ]]; then
local _sep="|"
[[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]] && _sep="│"
echo
_dmotd_sep_heavy
echo " host: $DMOTD_HOSTNAME  on  $DMOTD_RELEASE  $_sep  users: $DMOTD_CURRENTUSER ($DMOTD_USER)"
echo " load: $LOAD1, $LOAD5, $LOAD15 (1/5/15)  $_sep  proc: $PSA  $_sep  up: ${upDays}d ${upHours}h ${upMins}m ${upSecs}s"
_dmotd_sep_heavy
else
echo "
===============================================================================
 - Hostname......: $DMOTD_HOSTNAME on $DMOTD_RELEASE
 - Users.........: Currently $DMOTD_CURRENTUSER user(s) logged on (includes: $DMOTD_USER)
===============================================================================
 - CPU usage.....: $LOAD1, $LOAD5, $LOAD15 (1, 5, 15 min)
 - Processes.....: $PSA running
 - System uptime.: $upDays days $upHours hours $upMins minutes $upSecs seconds
==============================================================================="
fi

# Memory block
if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_MEMCOMPACT" != [nN] ]]; then
  # free -h output is parsed into a 2-line summary. Field positions follow
  # the standard procps-ng `free -h` layout: total used free shared buff/cache available
  local _mem_h _swap_h _m_total _m_used _m_free _m_shared _m_bc _m_avail
  local _s_total _s_used _s_free
  _mem_h=$(free -h | awk '/^Mem:/  {print $2, $3, $4, $5, $6, $7}')
  _swap_h=$(free -h | awk '/^Swap:/ {print $2, $3, $4}')
  read -r _m_total _m_used _m_free _m_shared _m_bc _m_avail <<<"$_mem_h"
  read -r _s_total _s_used _s_free <<<"$_swap_h"
  if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
    # Compute percentages from bytes (free -b) so the bar matches the human-
    # readable totals regardless of MB/GB scaling shown above.
    local _mb_used _mb_total _sb_used _sb_total _mp _sp
    _mb_used=$(free -b | awk '/^Mem:/  {print $3}')
    _mb_total=$(free -b | awk '/^Mem:/ {print $2}')
    _sb_used=$(free -b | awk '/^Swap:/ {print $3}')
    _sb_total=$(free -b | awk '/^Swap:/ {print $2}')
    _mp=0
    [ "${_mb_total:-0}" -gt 0 ] 2>/dev/null && _mp=$(( _mb_used * 100 / _mb_total ))
    _sp=0
    [ "${_sb_total:-0}" -gt 0 ] 2>/dev/null && _sp=$(( _sb_used * 100 / _sb_total ))
    printf ' mem  %s %3d%%  used %s / %s   avail %s\n' \
      "$(_fancy_bar "$_mp")" "$_mp" "${_m_used:-?}" "${_m_total:-?}" "${_m_avail:-?}"
    printf ' swap %s %3d%%  used %s / %s\n' \
      "$(_fancy_bar "$_sp")" "$_sp" "${_s_used:-?}" "${_s_total:-?}"
  else
    printf ' mem:  used %s / %s    free %s   buff/cache %s   avail %s\n' \
      "${_m_used:-?}" "${_m_total:-?}" "${_m_free:-?}" "${_m_bc:-?}" "${_m_avail:-?}"
    printf ' swap: used %s / %s\n' "${_s_used:-?}" "${_s_total:-?}"
  fi
  _dmotd_sep_heavy
else
  echo "$MEM"
  echo "==============================================================================="
fi

# Disk block — filter virtual filesystems out in compact mode
if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_DFCOMPACT" != [nN] ]]; then
  if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
    # Render header + each row, appending a 10-char fancy bar using the
    # Use% column as the percentage. We strip the % suffix and feed the
    # remainder to _fancy_bar (numeric-guarded for non-numeric edge rows).
    df -hT -x tmpfs -x devtmpfs -x efivarfs -x squashfs -x overlay -x autofs 2>/dev/null \
      | awk 'NR==1 {print; next} { use=$6; sub(/%$/,"",use); print $0 "|" use }' \
      | while IFS= read -r _line; do
          case "$_line" in
            *"|"*)
              # Subshell scope (while is downstream of a pipe), so plain
              # var assignment is fine — no `local` needed and would
              # arguably be wrong inside a subshell anyway.
              _pct="${_line##*|}"; _row="${_line%|*}"
              printf '%s  %s\n' "$_row" "$(_fancy_bar "$_pct")"
              ;;
            *)
              printf '%s\n' "$_line"
              ;;
          esac
        done
  else
    df -hT -x tmpfs -x devtmpfs -x efivarfs -x squashfs -x overlay -x autofs 2>/dev/null
  fi
  _dmotd_sep_heavy
else
  # Trailing blank line preserved from legacy heredoc output for byte-for-byte parity.
  echo "$DF"
  echo
fi

# CSF safety banner — collapse to 1 line in compact mode
if [[ "$ENABLEMOTD_CSFMSG" != [nN] ]]; then
  if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] ]]; then
    if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
      printf ' %b CSF Firewall — DO NOT run `iptables -F` (will lock you out)\n' "$(_fancy_badge warn)"
    else
      echo " ! CSF Firewall present — DO NOT run \`iptables -F\` (will lock you out)"
    fi
    _dmotd_sep_heavy
  else
    # Trailing whitespace on lines 2-5 below is preserved from legacy output
    # for byte-for-byte parity (do not strip).
    printf '%s\n' \
'===============================================================================' \
'# ! This server maybe running CSF Firewall !  ' \
'#   DO NOT run the below command or you  will lock yourself out of the server: ' \
'# ' \
'#   iptables -F ' \
''
  fi
fi

# Docs / Forum links — collapse to 2 lines in compact mode
if [[ "$ENABLEMOTD_LINKSMSG" != [nN] ]]; then
  if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_LINKSCOMPACT" != [nN] ]]; then
    echo " Docs:  centminmod.com/{getstarted,faq,configfiles}  ·  blog.centminmod.com"
    echo " Forum: community.centminmod.com   [ << Register ]"
    _dmotd_sep_heavy
  else
echo "
===============================================================================
* Getting Started Guide - https://centminmod.com/getstarted.html
* Centmin Mod FAQ - https://centminmod.com/faq.html
* Centmin Mod Config Files - https://centminmod.com/configfiles.html
* Centmin Mod Blog - https://blog.centminmod.com
* Community Forums https://community.centminmod.com  [ << Register ]
===============================================================================
"
  fi
fi
}

# Function to retrieve the latest NGINX version
get_latest_nginx_version() {
  if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    curl -${ipv_forceopt}sL --connect-timeout 10 https://freenginx.org/en/download.html 2>&1 | grep -Eo "freenginx-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "freenginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n1 2>&1 | tee "${CENTMINLOGDIR}/cmm-login-nginxver-check-debug_${DT}.log"
  else
    curl -${ipv_forceopt}sL --connect-timeout 10 https://nginx.org/download/ 2>&1 \
    | grep -Eo 'nginx-[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz' \
    | sed -E 's/^nginx-([0-9]+\.[0-9]+\.[0-9]+)\.tar\.gz$/\1/' \
    | sort -V \
    | tail -n1 2>&1 \
    | tee "${CENTMINLOGDIR}/cmm-login-nginxver-check-debug_${DT}.log"
  fi
}

ngxver_checker() {
  if [[ "$(which nginx >/dev/null 2>&1; echo $?)" = '0' ]]; then
    if [[ "$DMOTD_NGINXCHECK_DEBUG" = [yY] ]]; then
        # Check if the cache file exists
        if [ -f "$CACHE_FILE" ]; then
            # Calculate the time difference in minutes between now and the cache file's last modification time
            CACHE_AGE=$(( ( $(date +%s) - $(stat -c %Y "$CACHE_FILE") ) / 60 ))
        
            # Check if the cache has expired
            if [ $CACHE_AGE -gt $CACHE_TIMEOUT ]; then
                # Cache expired, fetch the latest version and update the cache file
                LATEST_NGINXVERS=$(get_latest_nginx_version)
                echo "$LATEST_NGINXVERS" > "$CACHE_FILE"
            else
                # Cache still valid, read the value from the cache file
                LATEST_NGINXVERS=$(cat "$CACHE_FILE")
            fi
        else
            # Cache file does not exist, fetch the latest version and create the cache file
            LATEST_NGINXVERS=$(get_latest_nginx_version)
            echo "$LATEST_NGINXVERS" > "$CACHE_FILE"
        fi
    else
        # Check if the cache file exists
        if [ -f "$CACHE_FILE" ]; then
            # Calculate the time difference in minutes between now and the cache file's last modification time
            CACHE_AGE=$(( ( $(date +%s) - $(stat -c %Y "$CACHE_FILE") ) / 60 ))
        
            # Check if the cache has expired
            if [ $CACHE_AGE -gt $CACHE_TIMEOUT ]; then
                # Cache expired, fetch the latest version and update the cache file
                LATEST_NGINXVERS=$(get_latest_nginx_version)
                echo "$LATEST_NGINXVERS" > "$CACHE_FILE"
            else
                # Cache still valid, read the value from the cache file
                LATEST_NGINXVERS=$(cat "$CACHE_FILE")
            fi
        else
            # Cache file does not exist, fetch the latest version and create the cache file
            LATEST_NGINXVERS=$(get_latest_nginx_version)
            echo "$LATEST_NGINXVERS" > "$CACHE_FILE"
        fi
        # LATEST_NGINXSTABLEVER=$(curl -${ipv_forceopt}sL --connect-timeout 10 https://nginx.org/en/download.html 2>&1 | grep -Eo "nginx-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n2 | tail -1)
    fi
    CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk '{print $3}' | awk -F '/' '{print $2}')
    
    # Convert versions to integers for proper comparison
    CURRENT_NGINXVERS_INT=$(nginx_version_to_int "$CURRENT_NGINXVERS")
    LATEST_NGINXVERS_INT=$(nginx_version_to_int "$LATEST_NGINXVERS")
    
    # Update cache if current version is newer than cached "latest"
    if [[ $CURRENT_NGINXVERS_INT -gt $LATEST_NGINXVERS_INT ]]; then
      echo "$CURRENT_NGINXVERS" > "$CACHE_FILE"
      LATEST_NGINXVERS="$CURRENT_NGINXVERS"
      LATEST_NGINXVERS_INT=$CURRENT_NGINXVERS_INT
    fi
    
    # Only show update notification if latest is genuinely newer
    if [[ $LATEST_NGINXVERS_INT -gt $CURRENT_NGINXVERS_INT ]]; then
      if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_NGINXVERCOMPACT" != [nN] ]]; then
        local _nginx_label='Nginx'
        [[ "$FREENGINX_INSTALL" = [yY] ]] && _nginx_label='FreeNginx'
        _dmotd_push_status warn " ${_nginx_label}  ${CURRENT_NGINXVERS} → ${LATEST_NGINXVERS} available — run centmin.sh menu 4"
      else
        echo
        cecho "===============================================================================" $boldgreen
        if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
          cecho "* FreeNginx Fork Update May Be Available via centmin.sh menu option 4" $boldyellow
        else
          cecho "* Nginx Update May Be Available via centmin.sh menu option 4" $boldyellow
        fi
        cecho "* see https://centminmod.com/nginx.html#nginxupgrade" $boldyellow
        cecho "===============================================================================" $boldgreen
        cecho "* Current Nginx Version:           $CURRENT_NGINXVERS" $boldyellow
        cecho "* Latest Nginx Mainline Available: $LATEST_NGINXVERS (centminmod.com/nginxnews)" $boldyellow
        # cecho "* Latest Nginx Stable Available:   $LATEST_NGINXSTABLEVER" $boldyellow
        cecho "===============================================================================" $boldgreen
        echo
      fi
      push_dmotd_alerts nginx "$LATEST_NGINXVERS"
    fi
  fi
}

# Function to retrieve the latest PHP version
get_latest_php_version() {
  if [ ! -f /usr/local/bin/getphpver ]; then
      wget -q https://github.com/centminmod/get-php-versions/raw/master/get-php-ver.sh -O /usr/local/bin/getphpver
      chmod +x /usr/local/bin/getphpver
  fi
  if [[ ! "$(grep '83' /usr/local/bin/getphpver)" ]]; then
      wget -q https://github.com/centminmod/get-php-versions/raw/master/get-php-ver.sh -O /usr/local/bin/getphpver
      chmod +x /usr/local/bin/getphpver
  fi
  if [ ! -f /usr/bin/jq ]; then
    yum -q -y install jq
  fi
  if [[ "$DMOTD_PHPCHECK_DEBUG" = [yY] ]]; then
      LATEST_PHPVERS=$(bash -x getphpver "$(php-config --version | awk -F '.' '{print $1$2}')" 2>"${CENTMINLOGDIR}/cmm-login-phpver-check-debug_${DT}.log")
  else
    LATEST_PHPVERS=$(getphpver "$(php-config --version | awk -F '.' '{print $1$2}')")
  fi
  echo "$LATEST_PHPVERS"
}

phpver_checker() {
  if [[ "$DMOTD_PHPCHECK" = [yY] && "$(which php-fpm >/dev/null 2>&1; echo $?)" = '0' ]]; then
    # Check if the cache file exists
    if [ -f "$CACHE_PHP_FILE" ]; then
      # Calculate the time difference in minutes between now and the cache file's last modification time
      CACHE_AGE=$(( ( $(date +%s) - $(stat -c %Y "$CACHE_PHP_FILE") ) / 60 ))

      # Check if the cache has expired
      if [ $CACHE_AGE -gt $CACHE_TIMEOUT ]; then
        # Cache expired, fetch the latest version and update the cache file
        LATEST_PHPVERS=$(get_latest_php_version)
        echo "$LATEST_PHPVERS" > "$CACHE_PHP_FILE"
      else
        # Cache still valid, read the value from the cache file
        LATEST_PHPVERS=$(cat "$CACHE_PHP_FILE")
      fi
    else
      # Cache file does not exist, fetch the latest version and create the cache file
      LATEST_PHPVERS=$(get_latest_php_version)
      echo "$LATEST_PHPVERS" > "$CACHE_PHP_FILE"
    fi
    CURRENT_PHPVERS=$(php-config --version)
    CURRENT_PHPXZVER_CHECK=$(php-config --version | awk -F '.' '{print $1"."$2}')
    if [[ -f /usr/bin/xz && "$CURRENT_PHPXZVER_CHECK" > 5.4 ]]; then
      PHPEXTSION_CHECK='xz'
    else
      PHPEXTSION_CHECK='gz'
    fi
    IS_PHPTAR_AVAIL=$(curl -sI${ipv_forceopt} --connect-timeout 10 https://www.php.net/distributions/php-${LATEST_PHPVERS}.tar.${PHPEXTSION_CHECK}| head -n1 | grep -o 200)
    if [[ "$CURRENT_PHPVERS" != "$LATEST_PHPVERS" ]] && [[ "$IS_PHPTAR_AVAIL" -eq '200' ]]; then
      if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_PHPVERCOMPACT" != [nN] ]]; then
        _dmotd_push_status warn " PHP    ${CURRENT_PHPVERS} → ${LATEST_PHPVERS} available — run centmin.sh menu 5"
      else
        echo
        cecho "===============================================================================" $boldgreen
        cecho "* PHP Update May Be Available via centmin.sh menu option 5" $boldyellow
        cecho "* see https://community.centminmod.com/forums/18/" $boldyellow
        cecho "===============================================================================" $boldgreen
        cecho "* Current PHP Version:        $CURRENT_PHPVERS" $boldyellow
        cecho "* Latest PHP Branch Version:  $LATEST_PHPVERS (github.com/php/php-src/tags)" $boldyellow
        cecho "===============================================================================" $boldgreen
        echo
      fi
      push_dmotd_alerts php "$LATEST_PHPVERS"
    fi
  fi
}

gitenv_askupdate() {
  DT=$(date +"%d%m%y-%H%M%S")
    if [[ -d "${CMSCRIPT_GITDIR}/.git" ]]; then
      # if git remote repo url is not same as one defined in giturl.txt then pull a new copy of
      # centmin mod code locally using giturl.txt defined git repo name
      GET_GITVER=$(git --version | awk '{print $3}' | sed -e 's|\.||g' | cut -c1,2)
      CURL_GITURL=$(curl -sk${ipv_forceopt} --connect-timeout 10 https://raw.githubusercontent.com/centminmod/centminmod/$(awk -F "=" '/branchname=/ {print $2}' ${CMSCRIPT_GITDIR}/centmin.sh | sed -e "s|'||g" )/giturl.txt)
      # if git version >1.8 use supported ls-remote --get-url flag otherwise use alternative
      if [[ -d "${CMSCRIPT_GITDIR}" ]]; then
        if [[ "$GET_GITVER" -ge '18' ]]; then
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git ls-remote --get-url)
        else
          GET_GITREMOTEURL=$(cd ${CMSCRIPT_GITDIR}; git remote -v | awk '/\(fetch/ {print $2}' | head -n1)
        fi
        if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]] && [[ ! -z "$CURL_GITURL" ]]; then
          if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_GITCOMPACT" != [nN] ]]; then
            : # remote-URL-changed status is reflected as "(remote changed)" suffix on the Centmin Mod status line below
          else
            cecho "===============================================================================" $boldgreen
            cecho " Centmin Mod remote branch has changed" $boldyellow
            cecho " from $GET_GITREMOTEURL" $boldyellow
            cecho " to $CURL_GITURL" $boldyellow
            cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
            cecho "===============================================================================" $boldgreen
          fi
        fi
      fi
      pushd "${CMSCRIPT_GITDIR}" >/dev/null 2>&1
      if [[ "$DMOTD_DEBUGSSHLOGIN" = [yY] ]]; then
        echo
        echo "################ DMOTD DEBUG BEGIN ################"
        echo "DMOTD DEBUG: Ping test github.com"
        ping -c4 github.com
        echo
        echo "DMOTD DEBUG: git fetch timings"
        echo "git fetch -v"
        export GIT_TRACE=1
        export GIT_TRACE_PACKET=1
        export GIT_TRACE_PERFORMANCE=1
        /usr/bin/time --format='real: %es user: %Us sys: %Ss cpu: %P maxmem: %M KB cswaits: %w' git fetch -v
        echo
        echo "################  DMOTD DEBUG END  ################"
        echo
      else
        git fetch >/dev/null 2>&1
      fi
      popd >/dev/null 2>&1
      local _local_branch=$(git --git-dir="${CMSCRIPT_GITDIR}/.git" --work-tree="${CMSCRIPT_GITDIR}" rev-parse --abbrev-ref HEAD 2>/dev/null)
      local _remote_changed=""
      [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]] && [[ ! -z "$CURL_GITURL" ]] && _remote_changed=" (remote changed: menu 23/1)"
      if [[ "$(cd ${CMSCRIPT_GITDIR}; git rev-parse HEAD)" != "$(cd ${CMSCRIPT_GITDIR}; git rev-parse @{u})" ]]; then
          # if remote branch commits don't match local commit, then there are new updates need
          # pulling
          push_dmotd_alerts cmm "${_local_branch:-unknown}"
          if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_GITCOMPACT" != [nN] ]]; then
            _dmotd_push_status warn " Centmin Mod ${_local_branch:-?} — updates available, run cmupdate${_remote_changed}"
          else
            cecho "===============================================================================" $boldgreen
            cecho " Centmin Mod code updates available for ${CMSCRIPT_GITDIR}" $boldyellow
            if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
              cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
            else
              cecho " to update, run cmupdate command in SSH & re-run centmin.sh once & exit" $boldyellow
            fi
            cecho "===============================================================================" $boldgreen
          fi
        else
          # no new commits/updates available
          if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_GITCOMPACT" != [nN] ]]; then
            _dmotd_push_status ok " Centmin Mod ${_local_branch:-?} — up to date${_remote_changed}"
          else
            cecho "===============================================================================" $boldgreen
            cecho " Centmin Mod local code is up to date at ${CMSCRIPT_GITDIR}" $boldyellow
            cecho " no available updates at this time..." $boldyellow
            cecho "===============================================================================" $boldgreen
          fi
      fi
      if [[ "$DMOTD_DEBUGSSHLOGIN" = [yY] ]]; then
        echo
        echo "DMOTD DEBUG: timings saved at:"
        echo "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"
        echo
      fi
      if [[ -f /opt/centminmod/first-login-run && -f /opt/centminmod/first-login.sh ]]; then /opt/centminmod/first-login.sh; fi
    fi
}

needrestart_check() {
  if [[ "$NEEDRESTART_CHECK" = [yY] && -f /usr/bin/needs-restarting ]]; then
    # Get the current day of the week (0 for Sunday, 1 for Monday, etc.)
    DAY_OF_WEEK=$(date +%u)

    # date +%u returns 1=Mon ... 7=Sun. Original code had `0` for Sunday
    # which never matches — fixed to 7.
    # Check if today is Friday (5), Saturday (6), or Sunday (7)
    if [ "$DAY_OF_WEEK" -eq "5" ] || [ "$DAY_OF_WEEK" -eq "6" ] || [ "$DAY_OF_WEEK" -eq "7" ]; then
        # Run the command and capture its output
        output=$(needs-restarting -r)
        local _reboot_required=0
        if echo "$output" | grep -q "Reboot is required to ensure that your system benefits from these updates."; then
            _reboot_required=1   # EL7 wording
        elif echo "$output" | grep -q "Reboot is required to fully utilize these updates."; then
            _reboot_required=1   # EL8/9/10 wording
        fi
        if [[ "$_reboot_required" -eq 1 ]]; then
            if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_NEEDRESTARTCOMPACT" != [nN] ]]; then
                _dmotd_push_status warn " ! Reboot required — flush MySQL first: mysqladmin flush-tables && sleep 180"
            else
                modified_output=$(echo "$output" | sed 's/Reboot/Server Reboot/')
                echo
                cecho "===============================================================================" $boldgreen
                echo "$modified_output"
                echo -e "\nRather than reboot server for each YUM update, you can schedule a specific time\n  i.e. on weekends"
                echo -e "\nTo ensure all MySQL data in memory buffers is written to disk before reboot"
                echo -e "Run this command & wait 180 seconds before rebooting server:\n  mysqladmin flush-tables && sleep 180"
                cecho "===============================================================================" $boldgreen
            fi
        fi
    fi
  fi
}

kernel_checks() {
  if [[ "$SSHLOGIN_KERNELCHECK" = [yY] && -f "$CMSCRIPT_GITDIR/tools/kernelcheck.sh" ]]; then
    "$CMSCRIPT_GITDIR/tools/kernelcheck.sh"
  fi
}

cmsec_checks() {
  # CVE detection line(s) for the dmotd login banner. Default off.
  # Enable via: echo "DMOTD_CVECHECK='y'" >> /etc/centminmod/custom_config.inc
  # Suppress specific CVEs: DMOTD_CVECHECK_SUPPRESS='CVE-2026-31431,CVE-2027-XXXX'
  # DMOTD_CVECHECK_COMPACT='y' collapses output to one summary line (independent
  # of DMOTD_COMPACT); vulnerable / indeterminate CVEs still expand.
  if [[ "$DMOTD_CVECHECK" = [yY] && -x "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" ]]; then
    if [[ "$DMOTD_CVECHECK_COMPACT" = [yY] ]]; then
      # Compact path — single cmsec.sh --json invocation (half the work of
      # the verbose --dmotd + --json double-call). Tallies the per-CVE
      # statuses and prints one summary line, plus an auto-expand block
      # listing vulnerable/indeterminate CVEs so security state is never
      # silently hidden.
      cmsec_status="$(CMSEC_CACHE_TTL_MIN="$CMSEC_CACHE_TIMEOUT" \
                      DMOTD_CVECHECK_SUPPRESS="$DMOTD_CVECHECK_SUPPRESS" \
                      "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" --json 2>/dev/null)"
      if [[ -n "$cmsec_status" ]]; then
        local _cmsec_kernel _cmsec_summary
        _cmsec_kernel="$(uname -r)"
        # Single-pass awk: tally counts and emit pipe-delimited
        # "<vulnerable_cves>|<indeterminate_cves>|<total>|<patched>|<not_affected>|<vulnerable>|<indeterminate>"
        _cmsec_summary="$(printf '%s' "$cmsec_status" | awk -F'"' '
          /"cve":/            { _cve = $4; _sev = "" }
          /"cvss_severity":/  { _sev = $4 }
          /"final_status":/   {
            total++
            status = $4
            if (status == "patched")        patched++
            else if (status == "not_affected") notaffected++
            else if (status == "vulnerable")  { vulnerable++; vul_list = vul_list (vul_list?", ":"") _cve; vul_sev[_cve] = _sev }
            else if (status == "indeterminate") { indeterminate++; ind_list = ind_list (ind_list?", ":"") _cve }
          }
          END {
            printf "%s|%s|%d|%d|%d|%d|%d", vul_list, ind_list, total+0, patched+0, notaffected+0, vulnerable+0, indeterminate+0
            for (c in vul_sev) printf "|%s=%s", c, vul_sev[c]
          }')"
        IFS='|' read -r _vul_list _ind_list _total _patched _notaffected _vulnerable _indeterminate _rest <<<"$_cmsec_summary"
        if [[ "$_vulnerable" -eq 0 && "$_indeterminate" -eq 0 ]]; then
          if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
            printf ' %b cmsec: kernel %s — %d/%d OK (%d patched, %d n/a)\n' \
              "$(_fancy_badge ok)" "$_cmsec_kernel" "$_total" "$_total" "$_patched" "$_notaffected"
          else
            printf ' cmsec: kernel %s — %d/%d OK (%d patched, %d n/a)\n' \
              "$_cmsec_kernel" "$_total" "$_total" "$_patched" "$_notaffected"
          fi
        else
          # Auto-expand: list what is broken regardless of compact toggle.
          # Both vulnerable AND indeterminate get the crit badge — indeterminate
          # means "couldn't verify safety", same visual weight as confirmed
          # vulnerable.
          if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
            printf ' %b cmsec: kernel %s — %d/%d checked: %d patched, %d n/a, %d VULNERABLE, %d indeterminate\n' \
              "$(_fancy_badge crit)" "$_cmsec_kernel" "$_total" "$_total" "$_patched" "$_notaffected" "$_vulnerable" "$_indeterminate"
            [[ -n "$_vul_list" ]] && printf ' %b cmsec VULNERABLE: %s — run '"'"'cmsec check'"'"'\n' "$(_fancy_badge crit)" "$_vul_list"
            [[ -n "$_ind_list" ]] && printf ' %b cmsec INDETERMINATE: %s\n' "$(_fancy_badge crit)" "$_ind_list"
          else
            printf ' cmsec: kernel %s — %d/%d checked: %d patched, %d n/a, %d VULNERABLE, %d indeterminate\n' \
              "$_cmsec_kernel" "$_total" "$_total" "$_patched" "$_notaffected" "$_vulnerable" "$_indeterminate"
            [[ -n "$_vul_list" ]] && printf ' ! cmsec VULNERABLE: %s — run '"'"'cmsec check'"'"'\n' "$_vul_list"
            [[ -n "$_ind_list" ]] && printf ' ! cmsec INDETERMINATE: %s\n' "$_ind_list"
          fi
        fi
        # Reuse cmsec_status for the Pushover loop below — no second call.
      fi
    else
      # Verbose path — current per-CVE banner from cmsec.sh --dmotd, plus
      # a second cmsec.sh --json invocation feeding the Pushover loop.
      CMSEC_CACHE_TTL_MIN="$CMSEC_CACHE_TIMEOUT" \
      DMOTD_CVECHECK_SUPPRESS="$DMOTD_CVECHECK_SUPPRESS" \
        "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" --dmotd 2>/dev/null
      cmsec_status="$(CMSEC_CACHE_TTL_MIN="$CMSEC_CACHE_TIMEOUT" \
                      DMOTD_CVECHECK_SUPPRESS="$DMOTD_CVECHECK_SUPPRESS" \
                      "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" --json 2>/dev/null)"
    fi
    # Pushover alert on vulnerable verdict (cooldown-throttled inside
    # push_dmotd_alerts). Same DMOTD_CVECHECK_SUPPRESS env var as the --dmotd
    # call above so the second invocation also honours per-CVE suppression —
    # otherwise a suppressed CVE would still trigger Pushover even though its
    # banner line was hidden.
    if [[ -n "$cmsec_status" ]] && printf '%s' "$cmsec_status" | grep -q '"final_status": "vulnerable"'; then
      kernel_str="$(uname -r)"
      printf '%s' "$cmsec_status" | awk -F'"' '
        /"cve":/            { _cve = $4; _sev = "" }
        /"cvss_severity":/  { _sev = $4 }
        /"final_status": "vulnerable"/ { print _cve "|" _sev }
      ' | while IFS='|' read -r cve_id cve_severity; do
        [ -n "$cve_id" ] && push_dmotd_alerts cve "${cve_id}|${kernel_str}|VULNERABLE" "${cve_severity}"
      done
    fi
  fi
}

csf_version_checker() {
  # DMOTD_CSFVERCHECK toggle gates the entire function — independent of
  # DMOTD_COMPACT. When 'n', no CSF version check runs at all.
  [[ "$DMOTD_CSFVERCHECK" != [yY] ]] && return 0
  if [[ "$(which csf >/dev/null 2>&1; echo $?)" = '0' ]]; then
    # Get remote version from Centmin Mod's self-hosted mirror
    REMOTE_CSF_VER=$(curl -${ipv_forceopt}sL --connect-timeout 10 https://download.centminmod.com/csf/version.txt 2>/dev/null | tr -d '\n' | tr -d '\r' | grep -E '^[0-9]+\.[0-9]+$')

    # Get local version from csf -v output (format: "csf: v15.01 (generic)")
    LOCAL_CSF_VER=$(csf -v 2>/dev/null | awk '{print $2}' | sed 's/^v//' | grep -E '^[0-9]+\.[0-9]+$')

    if [[ ! -z "$REMOTE_CSF_VER" && ! -z "$LOCAL_CSF_VER" ]]; then
      # Convert versions for numeric comparison (15.01 → 1501)
      REMOTE_CSF_NUM=$(echo "$REMOTE_CSF_VER" | sed 's/\.//')
      LOCAL_CSF_NUM=$(echo "$LOCAL_CSF_VER" | sed 's/\.//')

      # Display notice if remote version is same or newer than local
      if [[ "$REMOTE_CSF_NUM" -ge "$LOCAL_CSF_NUM" ]] 2>/dev/null; then
        if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] && "$ENABLEMOTD_CSFVERCOMPACT" != [nN] ]]; then
          # Compact path — append a single line. Skip the announcement banner
          # ("Centmin Mod now hosts its own CSF mirror") because it's one-time
          # info that doesn't need to print every login once admins are aware.
          if [[ "$REMOTE_CSF_NUM" -gt "$LOCAL_CSF_NUM" ]] 2>/dev/null; then
            if [[ "$LOCAL_CSF_VER" = "14.24" ]]; then
              CSFCF_CRON_EXISTS=$(crontab -l 2>/dev/null | grep -q 'csfcf.sh auto' && echo "yes" || echo "no")
              if [[ "$CSFCF_CRON_EXISTS" = "yes" ]]; then
                _dmotd_push_status warn " CSF    ${LOCAL_CSF_VER} → ${REMOTE_CSF_VER} available — let csfcf.sh auto run"
              else
                _dmotd_push_status warn " CSF    ${LOCAL_CSF_VER} → ${REMOTE_CSF_VER} available — run cmupdate && tools/csfcf.sh auto"
              fi
            else
              _dmotd_push_status warn " CSF    ${LOCAL_CSF_VER} → ${REMOTE_CSF_VER} available — run csf -u"
            fi
          else
            _dmotd_push_status ok " CSF    ${LOCAL_CSF_VER} (matches mirror)"
          fi
        else
          echo
          cecho "===============================================================================" $boldgreen
          cecho "* Centmin Mod now hosts its own CSF Firewall mirror for continued support" $boldyellow
          cecho "* Details at https://community.centminmod.com/threads/28985/" $boldyellow
          cecho "===============================================================================" $boldgreen
          cecho "* Current CSF Version: $LOCAL_CSF_VER" $boldyellow
          cecho "* Mirror CSF Version:  $REMOTE_CSF_VER" $boldyellow
          if [[ "$REMOTE_CSF_NUM" -gt "$LOCAL_CSF_NUM" ]] 2>/dev/null; then
            if [[ "$LOCAL_CSF_VER" = "14.24" ]]; then
              CSFCF_CRON_EXISTS=$(crontab -l 2>/dev/null | grep -q 'csfcf.sh auto' && echo "yes" || echo "no")
              if [[ "$CSFCF_CRON_EXISTS" = "yes" ]]; then
                cecho "* Update available: Run cmupdate && let cronjob tools/csfcf.sh auto update CSF" $boldyellow
              else
                cecho "* Update available: Run cmupdate && tools/csfcf.sh auto to update CSF" $boldyellow
              fi
            else
              cecho "* Update available: Run csf -u to update CSF Firewall" $boldyellow
            fi
          else
            cecho "* Your CSF version matches the mirror version" $boldyellow
          fi
          cecho "===============================================================================" $boldgreen
          echo
        fi
      fi
    fi
  fi
}

# Fancy-mode render helpers. _fancy_rule_heavy/light/_fancy_badge/_fancy_bar
# only print Unicode/ANSI when _DMOTD_FANCY_ACTIVE='y'; _dmotd_sep_heavy
# dispatches between fancy rule and the plain `===` cecho used in compact mode.
_fancy_rule_heavy() {
  cecho "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" $boldgreen
}
_fancy_rule_light() {
  cecho "───────────────────────────────────────────────────────────────────────────────" $boldgreen
}
# Colored ASCII badge. Argument $1 = ok|warn|crit. Uses ASCII letters
# (not ✓/✗ Dingbats — different Unicode block, patchier font coverage on
# Termux default font and Cascadia Mono Light pre-Windows-Terminal-1.18).
_fancy_badge() {
  case "$1" in
    ok)   printf '\033[1;32m[OK]\033[0m' ;;
    warn) printf '\033[1;33m[!!]\033[0m' ;;
    crit) printf '\033[1;31m[XX]\033[0m' ;;
    *)    printf '[??]' ;;
  esac
}
# 10-char progress bar. Argument $1 = percent (0-100). ▓ filled, ░ empty.
_fancy_bar() {
  local _pct="${1:-0}" _filled _empty _bar="" _i
  # Numeric guard — clamp non-numeric or out-of-range to 0/100
  case "$_pct" in
    *[!0-9]*|"") _pct=0 ;;
  esac
  [ "$_pct" -gt 100 ] && _pct=100
  _filled=$(( (_pct + 5) / 10 ))   # round to nearest tenth
  _empty=$(( 10 - _filled ))
  for (( _i=0; _i<_filled; _i++ )); do _bar+="▓"; done
  for (( _i=0; _i<_empty;  _i++ )); do _bar+="░"; done
  printf '%s' "$_bar"
}
# Unified separator dispatchers — let existing call sites in motd_output
# upgrade their rule chars without per-call if/else.
_dmotd_sep_heavy() {
  if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
    _fancy_rule_heavy
  else
    cecho "===============================================================================" $boldgreen
  fi
}
# Tag-validating helper for the compact status footer. All push sites
# should call this rather than appending to _dmotd_status_lines directly.
# Enforces the ok|warn|crit tag enum at push time so the render-side has
# no failure-mode for malformed entries. A bad tag is replaced with
# 'crit' and a stderr warning is emitted so the programming bug is
# loud — crit is chosen over warn because a future security warning
# silently downgrading to OK would be the worst-case failure. Embedded
# newlines are scrubbed to spaces so multiline payloads can't corrupt
# the footer layout (only the first line would otherwise get a badge).
_dmotd_push_status() {
  local _tag="$1" _text="$2"
  case "$_tag" in
    ok|warn|crit) ;;
    *)
      printf 'dmotd: untagged status entry — defaulted to crit: %s\n' \
        "$_text" >&2
      _tag='crit'
      ;;
  esac
  _text="${_text//$'\n'/ }"
  _dmotd_status_lines+=("${_tag}|${_text}")
}

render_compact_status_footer() {
  # Render the merged compact status footer captured by per-checker calls
  # into _dmotd_status_lines. Each entry is "<tag>|<text>" where tag is
  # ok|warn|crit (enforced at push time by _dmotd_push_status). Under
  # fancy, tag becomes a colored ASCII badge prefix; under compact, tag
  # is stripped and the text is emitted via cecho. Called once after all
  # checks complete, inside the { … } 2>&1 | tee block so the footer
  # lands in the dmotd log alongside the verbose checks that ran in
  # non-compact sections.
  [[ "$_DMOTD_COMPACT_EFFECTIVE" != [yY] ]] && return 0
  [[ "${#_dmotd_status_lines[@]}" -eq 0 ]] && return 0
  _dmotd_sep_heavy
  local _line _tag _text
  for _line in "${_dmotd_status_lines[@]}"; do
    _tag="${_line%%|*}"
    _text="${_line#*|}"
    if [[ "$_DMOTD_FANCY_ACTIVE" = [yY] ]]; then
      printf ' %b %s\n' "$(_fancy_badge "$_tag")" "$_text"
    else
      cecho "$_text" $boldyellow
    fi
  done
  _dmotd_sep_heavy
}

if [[ "$(id -u)" -eq 0 ]] || sudo -n true 2>/dev/null; then

  # Resolve render mode ONCE per login. _DMOTD_COMPACT_EFFECTIVE is the
  # single canonical layout signal — it's 'y' when EITHER DMOTD_COMPACT
  # or DMOTD_FANCY (capability-gated) is active. The user's DMOTD_COMPACT
  # variable is read once here and never mutated, so admins who set it
  # explicitly to 'n' don't have their setting silently changed when they
  # also opt in to fancy mode.
  [[ "$DMOTD_COMPACT" = [yY] ]] && _DMOTD_COMPACT_EFFECTIVE='y'
  if [[ "$DMOTD_FANCY" = [yY] ]] && _dmotd_fancy_capable; then
    _DMOTD_FANCY_ACTIVE='y'
    _DMOTD_COMPACT_EFFECTIVE='y'
  fi
  # Lock the resolved render-mode signals so the comment at L66-69 is
  # an enforced guarantee rather than convention. Any later code path
  # that attempts to mutate either signal fails loudly at runtime.
  readonly _DMOTD_COMPACT_EFFECTIVE _DMOTD_FANCY_ACTIVE

  starttime=$(TZ=UTC date +%s.%N)
  {
  motd_output
  if [[ "$(id -u)" -eq 0 || "$SUDO_USER" ]]; then
    kernel_checks
    cmsec_checks
    # Parallel ngxver/phpver requires array writes to survive the subshells.
    # Backgrounded functions run in subshells, so _dmotd_status_lines+=() in
    # the compact branch would be lost on wait. Serialise these two when in
    # compact mode (small perf cost — two sequential curl --connect-timeout
    # calls instead of one).
    if [[ "$DMOTD_PHPCHECK" = [yY] && "$(which php-fpm >/dev/null 2>&1; echo $?)" = '0' ]]; then
      if [[ "$_DMOTD_COMPACT_EFFECTIVE" = [yY] ]]; then
        ngxver_checker
        phpver_checker
      else
        ngxver_checker &
        phpver_checker &
        wait
      fi
    else
      ngxver_checker
    fi
  fi
  if [[ "$(id -u)" -eq 0 || "$SUDO_USER" ]]; then
    gitenv_askupdate
  else
    cecho "===============================================================================" $boldgreen
    echo "Detected non root/sudo elevated user: Centmin Mod update notifications disabled"
    echo "Centmin Mod update notifications are enabled for root/sudo elevated users only"
    echo "Only SSH logins via root/sudo elevated user, will update notifications show"
    cecho "===============================================================================" $boldgreen
  fi
  needrestart_check
  if [[ "$(id -u)" -eq 0 || "$SUDO_USER" ]]; then
    check_git_major_branch
    csf_version_checker
  fi
  # Print the merged compact footer once every check has run. No-op when
  # DMOTD_COMPACT='n' or when no checker contributed a status line.
  render_compact_status_footer
  } 2>&1 | tee "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"

  endtime=$(TZ=UTC date +%s.%N)

  INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
  echo "" >> "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"
  echo "Total Git & Nginx Check Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/cmm-login-git-checks_${DT}.log"

  # logs older than 5 days will be removed
  if [ -d "${CENTMINLOGDIR}" ]; then
    # find "${CENTMINLOGDIR}" -type f -mtime +5 -name 'cmm-login-git-checks_*.log' -print
    find "${CENTMINLOGDIR}" -type f -mtime +5 -name 'cmm-login-git-checks_*.log' | while read f; do
      if [ -f "$f" ]; then
        # echo "removing $f"
        rm -rf $f
      fi
    done
  fi

fi
