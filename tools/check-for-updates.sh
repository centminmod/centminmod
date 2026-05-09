#!/bin/bash
###########################################################
# register account at pushover.net and register your mobile
# devices and setup for 
# /usr/local/src/centminmod/tools/check-for-updates.sh
# 
# to enable set in the persistent config file
# /etc/centminmod/custom_config.inc the variables
#
# PUSH_CHECKUPDATES_ALERTS='y'
# PUSH_USER_KEY="your_pushover_user_key_here"
# PUSH_API_TOKEN="your_pushover_app_token_here"
# 
###########################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
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
CMSCRIPT_GITDIR='/usr/local/src/centminmod'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
SSHLOGIN_KERNELCHECK='n'
DMOTD_CVECHECK='n'           # cmsec CVE detection (also gate for `cmsec` subcommand)
DMOTD_CVECHECK_SUPPRESS=''   # comma-separated CVE IDs to suppress
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

# Set cache timeout in minutes
CACHE_TIMEOUT=60
CMSEC_CACHE_TIMEOUT=1440     # cmsec CVE cache TTL (24h)
# Set cache file path
CACHE_FILE="/tmp/nginx_version_cache"
CACHE_PHP_FILE="/tmp/php_version_cache"

# pushover.net settings
PUSH_CHECKUPDATES_ALERTS='n'
PUSH_VERBOSE='1'
PUSH_LOG_FILE="/var/log/push_check_updates_notify.log"
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

# Convert nginx version to integer for comparison (1.29.1 -> 1029001)
nginx_version_to_int() {
    local version=$1
    echo "$version" | awk -F. '{print $1 * 1000000 + $2 * 1000 + $3}'
}

###########################################################
if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    source "${CONFIGSCANBASE}/custom_config.inc"
fi
if [ -f "/etc/centminmod/pushover.ini" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "/etc/centminmod/pushover.ini"
  fi
  source "/etc/centminmod/pushover.ini"
fi
if [[ "$(id -u)" -eq '0' && ! -d "$CENTMINLOGDIR" ]]; then
  mkdir -p $CENTMINLOGDIR
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

push_check_updates_cronsetup() {
  if [[ -z "$(crontab -l 2>&1 | grep '/usr/local/src/centminmod/tools/check-for-updates.sh')" && -f "/usr/local/src/centminmod/tools/check-for-updates.sh" ]]; then
    mkdir -p /etc/centminmod/cronjobs/
    crontab -l > /etc/centminmod/cronjobs/cronjoblist-before-check-forum-updates.txt
    sed -i '/check-for-updates.sh/d' /etc/centminmod/cronjobs/cronjoblist-before-check-forum-updates.txt
    echo "20 2,14 * * * /usr/local/src/centminmod/tools/check-for-updates.sh all >/dev/null 2>&1" >> /etc/centminmod/cronjobs/cronjoblist-before-check-forum-updates.txt
    crontab /etc/centminmod/cronjobs/cronjoblist-before-check-forum-updates.txt
  fi
}

push_log_message() {
    if [[ "${PUSH_VERBOSE}" -eq 1 ]]; then
        echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> "${PUSH_LOG_FILE}"
    fi
}

push_update_alerts() {
  local pushapp="$1"
  local pushapp_ver="$2"
  local pushapp_severity="${3:-}"
  local _cooldown_dir _cooldown_key _cooldown_file _last_ts _now_ts
  local _cve_id _cve_rest _cve_kernel _cve_verdict _cve_severity _push_priority _push_url
  local PUSH_MESSAGE PUSH_TITLE RESPONSE _curl_rc
  local CVE_ALERT_COOLDOWN="${CVE_ALERT_COOLDOWN:-86400}"
  if [[ "$pushapp" = 'cve' ]]; then
    # Per-CVE alert cooldown to prevent cron-driven alert spam.
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
  if [[ "$pushapp" = 'nginx' ]]; then
    PUSH_MESSAGE="nginx ${pushapp_ver} update available, run centmin.sh menu option 4. https://centminmod.com/nginx.html#nginxupgrade"
    PUSH_TITLE="nginx update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  elif [[ "$pushapp" = 'php' ]]; then
    PUSH_MESSAGE="php-fpm ${pushapp_ver} update available, run centmin.sh menu option 5. https://community.centminmod.com/forums/18/"
    PUSH_TITLE="php update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  elif [[ "$pushapp" = 'cmm' ]]; then
    PUSH_MESSAGE="centminmod ${pushapp_ver} update available, run cmupdate to update. https://community.centminmod.com/forums/centmin-mod-github-commits.41/?prefix_id=19"
    PUSH_TITLE="centminmod update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  elif [[ "$pushapp" = 'restart' ]]; then
    PUSH_MESSAGE="YUM updates requiring server reboot. Recommended to schedule a reboot ${pushapp_ver}"
    PUSH_TITLE="YUM updates need reboot ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
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
  if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
    push_log_message "$PUSH_MESSAGE"

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

    push_log_message "Notification sent. Response: ${RESPONSE}"
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
      echo
      # cecho "===============================================================================" $boldgreen
      if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
        cecho "* FreeNginx Fork Update May Be Available via centmin.sh menu option 4" $boldyellow
      else
        cecho "* Nginx Update May Be Available via centmin.sh menu option 4" $boldyellow
      fi
      cecho "* see https://centminmod.com/nginx.html#nginxupgrade" $boldyellow
      # cecho "===============================================================================" $boldgreen
      cecho "* Current Nginx Version:           $CURRENT_NGINXVERS" $boldyellow
      cecho "* Latest Nginx Mainline Available: $LATEST_NGINXVERS (centminmod.com/nginxnews)" $boldyellow
      # cecho "* Latest Nginx Stable Available:   $LATEST_NGINXSTABLEVER" $boldyellow
      # cecho "===============================================================================" $boldgreen
      echo
      push_update_alerts nginx "$LATEST_NGINXVERS"
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
      TEST_PHPVERS=$(bash -x getphpver "$(php-config --version | awk -F '.' '{print $1$2}')") | tee "${CENTMINLOGDIR}/cmm-check-for-updates-phpver-check-debug_${DT}.log"
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
      echo
      # cecho "===============================================================================" $boldgreen
      cecho "* PHP Update May Be Available via centmin.sh menu option 5" $boldyellow
      cecho "* see https://community.centminmod.com/forums/18/" $boldyellow
      # cecho "===============================================================================" $boldgreen
      cecho "* Current PHP Version:        $CURRENT_PHPVERS" $boldyellow
      cecho "* Latest PHP Branch Version:  $LATEST_PHPVERS (github.com/php/php-src/tags)" $boldyellow
      # cecho "===============================================================================" $boldgreen
      echo
      push_update_alerts php "$LATEST_PHPVERS"
    fi
  fi
}

cmm_check() {
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
          # cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod remote branch has changed" $boldyellow
          cecho " from $GET_GITREMOTEURL" $boldyellow
          cecho " to $CURL_GITURL" $boldyellow
          cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          # cecho "===============================================================================" $boldgreen
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
      if [[ "$(cd ${CMSCRIPT_GITDIR}; git rev-parse HEAD)" != "$(cd ${CMSCRIPT_GITDIR}; git rev-parse @{u})" ]]; then
          # if remote branch commits don't match local commit, then there are new updates need
          # pulling
          # cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod code updates available for ${CMSCRIPT_GITDIR}" $boldyellow
          push_update_alerts cmm "$branchname"
          if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
            cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          else
            cecho " to update, run cmupdate command in SSH & re-run centmin.sh once & exit" $boldyellow
          fi
          # cecho "===============================================================================" $boldgreen
        else
          # no new commits/updates available
          # cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod local code is up to date at ${CMSCRIPT_GITDIR}" $boldyellow
          cecho " no available updates at this time..." $boldyellow
          # cecho "===============================================================================" $boldgreen
      fi
      if [[ "$DMOTD_DEBUGSSHLOGIN" = [yY] ]]; then
        echo
        echo "DMOTD DEBUG: timings saved at:"
        echo "${CENTMINLOGDIR}/cmm-check-for-updates-git-checks_${DT}.log"
        echo
      fi
      if [[ -f /opt/centminmod/first-login-run && -f /opt/centminmod/first-login.sh ]]; then /opt/centminmod/first-login.sh; fi
    fi
}

needrestart_check() {
  if [[ "$NEEDRESTART_CHECK" = [yY] && -f /usr/bin/needs-restarting ]]; then
    # date +%u returns 1=Mon ... 7=Sun (per ISO 8601), NOT 0=Sun.
    # Original code had `0` for Sunday which never matched — semantic bug
    # alongside the missing `[` syntax bug. Both fixed here.
    DAY_OF_WEEK=$(date +%u)

    # Check if today is Thursday (4), Friday (5), Saturday (6), or Sunday (7)
    if [ "$DAY_OF_WEEK" -eq "4" ] || [ "$DAY_OF_WEEK" -eq "5" ] || [ "$DAY_OF_WEEK" -eq "6" ] || [ "$DAY_OF_WEEK" -eq "7" ]; then
        # Run the command and capture its output
        output=$(needs-restarting -r)
        # Modify the output based on the version-specific message
        if echo "$output" | grep -q "Reboot is required to ensure that your system benefits from these updates."; then
            # For EL7
            modified_output=$(echo "$output" | sed 's/Reboot/Server Reboot/')
            push_modified_output=$(echo "$modified_output" | sed -n '/Core libraries or services have been updated:/,/Server Reboot is required/p' | sed '1d;$d')

            # Display the modified output and the additional message
            echo
            # cecho "===============================================================================" $boldgreen
            echo "$modified_output"
            echo -e "\nRather than reboot server for each YUM update, you can schedule a specific time\n  i.e. on weekends"
            echo -e "\nTo ensure all MySQL data in memory buffers is written to disk before reboot"
            echo -e "Run this command & wait 180 seconds before rebooting server:\n  mysqladmin flush-tables && sleep 180"
            # cecho "===============================================================================" $boldgreen
            push_update_alerts restart "$push_modified_output"
        elif echo "$output" | grep -q "Reboot is required to fully utilize these updates."; then
            # For EL8 & EL9
            modified_output=$(echo "$output" | sed 's/Reboot/Server Reboot/')
            push_modified_output=$(echo "$modified_output" | sed -n '/Core libraries or services have been updated:/,/Server Reboot is required/p' | sed '1d;$d')
            # Display the modified output and the additional message
            echo
            # cecho "===============================================================================" $boldgreen
            echo "$modified_output"
            echo -e "\nRather than reboot server for each YUM update, you can schedule a specific time\n  i.e. on weekends"
            echo -e "\nTo ensure all MySQL data in memory buffers is written to disk before reboot"
            echo -e "Run this command & wait 180 seconds before rebooting server:\n  mysqladmin flush-tables && sleep 180"
            # cecho "===============================================================================" $boldgreen
            push_update_alerts restart "$push_modified_output"
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
  # Centmin Mod security check dispatcher. Suitable for cron use; emits compact
  # status and fires Pushover on VULNERABLE verdict via push_update_alerts (this
  # file's existing pushover function — push_dmotd_alerts lives in dmotd.sh and
  # is NOT sourced here, so we use the local one with a 'cve' branch).
  local cmsec_status cve_id kernel_str
  if [[ -x "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" ]]; then
    CMSEC_CACHE_TTL_MIN="$CMSEC_CACHE_TIMEOUT" \
    DMOTD_CVECHECK_SUPPRESS="$DMOTD_CVECHECK_SUPPRESS" \
      "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" --dmotd 2>/dev/null
    # Pass DMOTD_CVECHECK_SUPPRESS to the second invocation too so suppressed
    # CVEs don't slip past and trigger a Pushover alert.
    cmsec_status="$(CMSEC_CACHE_TTL_MIN="$CMSEC_CACHE_TIMEOUT" \
                    DMOTD_CVECHECK_SUPPRESS="$DMOTD_CVECHECK_SUPPRESS" \
                    "$CMSCRIPT_GITDIR/tools/cmm-security/cmsec.sh" --json 2>/dev/null)"
    if [[ -n "$cmsec_status" ]] && printf '%s' "$cmsec_status" | grep -q '"final_status": "vulnerable"'; then
      kernel_str="$(uname -r)"
      printf '%s' "$cmsec_status" | awk -F'"' '
        /"cve":/            { _cve = $4; _sev = "" }
        /"cvss_severity":/  { _sev = $4 }
        /"final_status": "vulnerable"/ { print _cve "|" _sev }
      ' | while IFS='|' read -r cve_id cve_severity; do
        [ -n "$cve_id" ] && push_update_alerts cve "${cve_id}|${kernel_str}|VULNERABLE" "${cve_severity}"
      done
    fi
  fi
}

help() {
  echo
  echo "Usage:"
  echo
  echo "$0 {cmm|nginx|php|kernel|cmsec|restart-check|all}"
}

case "$1" in
  cmm )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      cmm_check
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  nginx )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      ngxver_checker
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  php )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      phpver_checker
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  kernel )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      kernel_checks
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  cmsec )
    # Intentional: no PUSH_CHECKUPDATES_ALERTS guard like nginx/php/kernel.
    # cmsec_checks() emits the dmotd-style status line unconditionally (useful
    # standalone) and the Pushover-on-vulnerable path is internally gated by
    # PUSH_CHECKUPDATES_ALERTS + PUSH_API_TOKEN + PUSH_USER_KEY inside push_update_alerts.
    push_check_updates_cronsetup
    cmsec_checks
    ;;
  restart-check )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      needrestart_check
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  all )
    push_check_updates_cronsetup
    if [[ "$PUSH_CHECKUPDATES_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
      cmm_check
      ngxver_checker
      phpver_checker
      kernel_checks
      cmsec_checks
      needrestart_check
    else
      echo "PUSH_CHECKUPDATES_ALERTS, PUSH_API_TOKEN, PUSH_USER_KEY not configured"
    fi
    ;;
  * )
    help
    ;;
esac