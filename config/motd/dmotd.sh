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
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

# Set cache timeout in minutes
CACHE_TIMEOUT=60
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
    local branches_to_check=("123.08stable" "123.09beta01" "124.00stable" "130.00beta01")
    echo -n " Current local server Centmin Mod branch installed: "
    cecho "$current_branch " $boldyellow
    cecho "===============================================================================" $boldgreen
    for branch in "${branches_to_check[@]}"; do
        if [[ "$current_branch" == "$branch" ]]; then
            echo -n " Newer Centmin Mod branch version is available: "
            cecho "131.00stable or 140.00beta01" $boldyellow
            echo -n " Details at "
            cecho "https://community.centminmod.com/threads/25572/" $boldyellow
            cecho "===============================================================================" $boldgreen
            break
        fi
    done
}

push_dmotd_alerts() {
  pushapp=$1
  pushapp_ver=$2
  if [[ "$pushapp" = 'nginx' ]]; then
    PUSH_MESSAGE="nginx ${pushapp_ver} update available, run centmin.sh menu option 4"
    PUSH_TITLE="nginx ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  elif [[ "$pushapp" = 'php' ]]; then
    PUSH_MESSAGE="php-fpm ${pushapp_ver} update available, run centmin.sh menu option 5"
    PUSH_TITLE="php-fpm ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  elif [[ "$pushapp" = 'cmm' ]]; then
    PUSH_MESSAGE="centminmod ${pushapp_ver} update available, run cmupdate to update"
    PUSH_TITLE="centminmod ${pushapp_ver} update available ${PUSH_HOSTNAME} ${PUSH_DATE_TIME}"
  fi
  if [[ "$PUSH_MOTD_ALERTS" = [yY] && "$PUSH_API_TOKEN" && "$PUSH_USER_KEY" ]]; then
    log_message "$PUSH_MESSAGE"
    
    # Send Notification
    RESPONSE=$(curl -s \
      --form-string "token=${PUSH_API_TOKEN}" \
      --form-string "user=${PUSH_USER_KEY}" \
      --form-string "message=${PUSH_MESSAGE}" \
      --form-string "title=${PUSH_TITLE}" \
      https://api.pushover.net/1/messages.json)
    
    # Log the response from Pushover
    log_message "Notification sent. Response: ${RESPONSE}"
  fi
}

motd_output() {
echo "
===============================================================================
 - Hostname......: $DMOTD_HOSTNAME on $DMOTD_RELEASE
 - Users.........: Currently $DMOTD_CURRENTUSER user(s) logged on (includes: $DMOTD_USER)
===============================================================================
 - CPU usage.....: $LOAD1, $LOAD5, $LOAD15 (1, 5, 15 min)
 - Processes.....: $PSA running
 - System uptime.: $upDays days $upHours hours $upMins minutes $upSecs seconds
===============================================================================
$MEM
===============================================================================
$DF
"
if [[ "$ENABLEMOTD_CSFMSG" != [nN] ]]; then
echo "===============================================================================
# ! This server maybe running CSF Firewall !  
#   DO NOT run the below command or you  will lock yourself out of the server: 
# 
#   iptables -F 
"
fi
if [[ "$ENABLEMOTD_LINKSMSG" != [nN] ]]; then
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
}

# Function to retrieve the latest NGINX version
get_latest_nginx_version() {
  if [[ "$FREENGINX_INSTALL" = [yY] ]]; then
    curl -${ipv_forceopt}sL --connect-timeout 10 https://freenginx.org/en/download.html 2>&1 | egrep -o "freenginx\-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n1 2>&1 | tee "${CENTMINLOGDIR}/cmm-login-nginxver-check-debug_${DT}.log"
  else
    curl -${ipv_forceopt}sL --connect-timeout 10 https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n1 2>&1 | tee "${CENTMINLOGDIR}/cmm-login-nginxver-check-debug_${DT}.log"
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
        # LATEST_NGINXSTABLEVER=$(curl -${ipv_forceopt}sL --connect-timeout 10 https://nginx.org/en/download.html 2>&1 | egrep -o "nginx\-[0-9.]+\.tar[.a-z]*" | grep -v '.asc' | awk -F "nginx-" '/.tar.gz$/ {print $2}' | sed -e 's|.tar.gz||g' | head -n2 | tail -1)
    fi
    CURRENT_NGINXVERS=$(nginx -v 2>&1 | awk '{print $3}' | awk -F '/' '{print $2}')
    if [[ "$CURRENT_NGINXVERS" != "$LATEST_NGINXVERS" ]]; then
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
      TEST_PHPVERS=$(bash -x getphpver "$(php-config --version | awk -F '.' '{print $1$2}')") | tee "${CENTMINLOGDIR}/cmm-login-phpver-check-debug_${DT}.log"
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
      cecho "===============================================================================" $boldgreen
      cecho "* PHP Update May Be Available via centmin.sh menu option 5" $boldyellow
      cecho "* see https://community.centminmod.com/forums/18/" $boldyellow
      cecho "===============================================================================" $boldgreen
      cecho "* Current PHP Version:        $CURRENT_PHPVERS" $boldyellow
      cecho "* Latest PHP Branch Version:  $LATEST_PHPVERS (github.com/php/php-src/tags)" $boldyellow
      cecho "===============================================================================" $boldgreen
      echo
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
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod remote branch has changed" $boldyellow
          cecho " from $GET_GITREMOTEURL" $boldyellow
          cecho " to $CURL_GITURL" $boldyellow
          cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          cecho "===============================================================================" $boldgreen
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
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod code updates available for ${CMSCRIPT_GITDIR}" $boldyellow
          push_dmotd_alerts cmm "$branchname"
          if [[ "$GET_GITREMOTEURL" != "$CURL_GITURL" ]]; then
            cecho " to update re-run centmin.sh menu option 23 submenu option 1" $boldyellow
          else
            cecho " to update, run cmupdate command in SSH & re-run centmin.sh once & exit" $boldyellow
          fi
          cecho "===============================================================================" $boldgreen
        else
          # no new commits/updates available
          cecho "===============================================================================" $boldgreen
          cecho " Centmin Mod local code is up to date at ${CMSCRIPT_GITDIR}" $boldyellow
          cecho " no available updates at this time..." $boldyellow
          cecho "===============================================================================" $boldgreen
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
    
    # Check if today is Friday (5), Saturday (6), or Sunday (0)
    if [ "$DAY_OF_WEEK" -eq "5" ] || [ "$DAY_OF_WEEK" -eq "6" ] || [ "$DAY_OF_WEEK" -eq "0" ]; then
        # Run the command and capture its output
        output=$(needs-restarting -r)
        # Modify the output based on the version-specific message
        if echo "$output" | grep -q "Reboot is required to ensure that your system benefits from these updates."; then
            # For EL7
            modified_output=$(echo "$output" | sed 's/Reboot/Server Reboot/')
            # Display the modified output and the additional message
            echo
            cecho "===============================================================================" $boldgreen
            echo "$modified_output"
            echo -e "\nRather than reboot server for each YUM update, you can schedule a specific time\n  i.e. on weekends"
            echo -e "\nTo ensure all MySQL data in memory buffers is written to disk before reboot"
            echo -e "Run this command & wait 180 seconds before rebooting server:\n  mysqladmin flush-tables && sleep 180"
            cecho "===============================================================================" $boldgreen
        elif echo "$output" | grep -q "Reboot is required to fully utilize these updates."; then
            # For EL8 & EL9
            modified_output=$(echo "$output" | sed 's/Reboot/Server Reboot/')
            # Display the modified output and the additional message
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
}

kernel_checks() {
  if [[ "$SSHLOGIN_KERNELCHECK" = [yY] && -f "$CMSCRIPT_GITDIR/tools/kernelcheck.sh" ]]; then
    "$CMSCRIPT_GITDIR/tools/kernelcheck.sh"
  fi
}

if [[ "$(id -u)" = '0' ]]; then

starttime=$(TZ=UTC date +%s.%N)
{
motd_output
kernel_checks
if [[ "$DMOTD_PHPCHECK" = [yY] && "$(which php-fpm >/dev/null 2>&1; echo $?)" = '0' ]]; then
  ngxver_checker &
  phpver_checker &
  wait
else
  ngxver_checker
fi
gitenv_askupdate
needrestart_check
check_git_major_branch
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