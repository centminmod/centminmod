#!/bin/bash
########################################################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
########################################################################################
# https://community.centminmod.com/threads/help-test-innodbio-sh-for-mysql-tuning.6012/
# for centminmod.com /etc/my.cnf
########################################################################################
DT=$(date +"%d%m%y-%H%M%S")
VER=0.4
DEBUG='n'
CPUS=$(grep -c "processor" /proc/cpuinfo)
TIME='n'

# Function to get MariaDB version
get_mariadb_version() {
    # Try mariadb command first (MariaDB 11.4+), fall back to mysql
    if command -v mariadb >/dev/null 2>&1; then
        local version=$(mariadb -V 2>&1 | awk '{print $5}' | awk -F. '{print $1"."$2}')
    else
        local version=$(mysql -V 2>&1 | awk '{print $5}' | awk -F. '{print $1"."$2}')
    fi
    echo $version
}

# Function to set client command variables based on MariaDB version
set_mariadb_client_commands() {
    local version=$(get_mariadb_version)
    
    # Convert version to a comparable integer (e.g., 10.3 becomes 1003)
    version_number=$(echo "$version" | awk -F. '{printf "%d%02d\n", $1, $2}')

    if (( version_number <= 1011 )); then
        # For versions less than or equal to 10.11, use old MySQL names
        ALIAS_MYSQLACCESS="mysqlaccess"
        ALIAS_MYSQLADMIN="mysqladmin"
        ALIAS_MYSQLBINLOG="mysqlbinlog"
        ALIAS_MYSQLCHECK="mysqlcheck"
        ALIAS_MYSQLDUMP="mysqldump"
        ALIAS_MYSQLDUMPSLOW="mysqldumpslow"
        ALIAS_MYSQLHOTCOPY="mysqlhotcopy"
        ALIAS_MYSQLIMPORT="mysqlimport"
        ALIAS_MYSQLREPORT="mysqlreport"
        ALIAS_MYSQLSHOW="mysqlshow"
        ALIAS_MYSQLSLAP="mysqlslap"
        ALIAS_MYSQL_CONVERT_TABLE_FORMAT="mysql_convert_table_format"
        ALIAS_MYSQL_EMBEDDED="mysql_embedded"
        ALIAS_MYSQL_FIND_ROWS="mysql_find_rows"
        ALIAS_MYSQL_FIX_EXTENSIONS="mysql_fix_extensions"
        ALIAS_MYSQL_INSTALL_DB="mysql_install_db"
        ALIAS_MYSQL_PLUGIN="mysql_plugin"
        ALIAS_MYSQL_SECURE_INSTALLATION="mysql_secure_installation"
        ALIAS_MYSQL_SETPERMISSION="mysql_setpermission"
        ALIAS_MYSQL_TZINFO_TO_SQL="mysql_tzinfo_to_sql"
        ALIAS_MYSQL_UPGRADE="mysql_upgrade"
        ALIAS_MYSQL_WAITPID="mysql_waitpid"
        ALIAS_MYSQL="mysql"
        ALIAS_MYSQLD="mysqld"
        ALIAS_MYSQLDSAFE="mysqld_safe"
    else
        # For versions greater than 10.11, use new MariaDB names
        ALIAS_MYSQLACCESS="mariadb-access"
        ALIAS_MYSQLADMIN="mariadb-admin"
        ALIAS_MYSQLBINLOG="mariadb-binlog"
        ALIAS_MYSQLCHECK="mariadb-check"
        ALIAS_MYSQLDUMP="mariadb-dump"
        ALIAS_MYSQLDUMPSLOW="mariadb-dumpslow"
        ALIAS_MYSQLHOTCOPY="mariadb-hotcopy"
        ALIAS_MYSQLIMPORT="mariadb-import"
        ALIAS_MYSQLREPORT="mariadb-report"
        ALIAS_MYSQLSHOW="mariadb-show"
        ALIAS_MYSQLSLAP="mariadb-slap"
        ALIAS_MYSQL_CONVERT_TABLE_FORMAT="mariadb-convert-table-format"
        ALIAS_MYSQL_EMBEDDED="mariadb-embedded"
        ALIAS_MYSQL_FIND_ROWS="mariadb-find-rows"
        ALIAS_MYSQL_FIX_EXTENSIONS="mariadb-fix-extensions"
        ALIAS_MYSQL_INSTALL_DB="mariadb-install-db"
        ALIAS_MYSQL_PLUGIN="mariadb-plugin"
        ALIAS_MYSQL_SECURE_INSTALLATION="mariadb-secure-installation"
        ALIAS_MYSQL_SETPERMISSION="mariadb-setpermission"
        ALIAS_MYSQL_TZINFO_TO_SQL="mariadb-tzinfo-to-sql"
        ALIAS_MYSQL_UPGRADE="mariadb-upgrade"
        ALIAS_MYSQL_WAITPID="mariadb-waitpid"
        ALIAS_MYSQL="mariadb"
        ALIAS_MYSQLD="mariadbd"
        ALIAS_MYSQLDSAFE="mariadbd-safe"
    fi
}
set_mariadb_client_commands

MDB_SVER=$(${ALIAS_MYSQL} -V | awk '{print $5}' | cut -d . -f1,2 | head -n1)
MDB_DATADIRSIZE=$(df $(${ALIAS_MYSQLADMIN} var | grep datadir | tr -s ' ' | awk '{print $4}') | tail -1 | awk '{print $4}')
MDB_DATADIR=$(${ALIAS_MYSQLADMIN} var | grep datadir | tr -s ' ' | awk '{print $4}' | tail -1)
FIOBASEDIR="$MDB_DATADIR/cmsetiofiotest"
CENTMINLOGDIR='/root/centminlogs'

TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
SCRIPT_SOURCEBASE=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
# account for tools directory placement of tools/setio.sh
SCRIPT_DIR=$(readlink -f $(dirname ${SCRIPT_DIR}))
########################################################################################

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ ! -f /usr/bin/fio ]; then
  yum -q -y install fio --disableplugin=priorities
fi

if [ ! -d "$FIOBASEDIR" ]; then
  mkdir -p $FIOBASEDIR
  if [[ ! "$(grep 'cmsetiofiotest' /etc/my.cnf)" ]]; then
    cp -a /etc/my.cnf /etc/my.cnf-setiobackup
    sed -i 's|\[mysqld\]|\[mysqld\]\nignore_db_dirs=cmsetiofiotest|' /etc/my.cnf
    MARIADBVERCHECK=$(rpm -qa | grep MariaDB-server | awk -F "-" '{print $3}' | cut -c1-4)
    if [[ "$MARIADBVERCHECK" == '10.1' || "$MARIADBVERCHECK" == '10.2' || "$MARIADBVERCHECK" == '10.3' || "$MARIADBVERCHECK" == '10.4' || "$MARIADBVERCHECK" == '10.5' || "$MARIADBVERCHECK" == '10.6' ]]; then
      sed -i 's|ignore-db-dir=|ignore_db_dirs=|g' /etc/my.cnf
      sed -i 's|ignore_db_dir=|ignore_db_dirs=|g' /etc/my.cnf
    fi
    # /usr/bin/mysqlreload
  fi
fi

if [ ! -f /proc/user_beancounters ]; then
  if [[ ! -f /usr/bin/lscpu ]]; then
    yum -q -y install util-linux-ng
  fi
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  if [[ "$EL_VERID" -eq 10 ]]; then
    # Try $4 first (Kitten format), check if it's a valid version number
    CENTOSVER_TEST=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1)
    if [[ "$CENTOSVER_TEST" =~ ^[0-9]+$ ]]; then
      # $4 contains version (Kitten: "AlmaLinux release 10.0")
      CENTOSVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2)
      ALMALINUXVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
    else
      # $4 is not numeric (Purple Lion: "AlmaLinux release 10.0 (Purple Lion)"), use $3
      CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
      ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
    fi
  else
    # EL8/EL9 continue using $3
    CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
    ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  fi
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ALMALINUX_TEN='10'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ROCKYLINUX_TEN='10'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ORACLELINUX_TEN='10'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    VZLINUX_TEN='10'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    CIRCLELINUX_TEN='10'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    NAVYLINUX_TEN='10'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    EUROLINUX_TEN='10'
  fi
fi

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

if [[ "$CENTOS_SIX" = '6' ]]; then
  IFREEMEM=$(cat /proc/meminfo | grep MemFree | awk '{print $2}')
  CACHEDMEM=$(cat /proc/meminfo | grep '^Cached' | awk '{print $2}')
  FREEMEM=$(echo "$IFREEMEM + $CACHEDMEM" | bc)
elif [[ "$CENTOS_SEVEN" = '7' ]]; then
  FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
fi

# Function to detect if we're in a container
is_container() {
    # Check for Docker
    [ -f /.dockerenv ] && return 0
    
    # Check for other container indicators
    [ -f /run/.containerenv ] && return 0  # Podman
    
    # Check if systemd is PID 1
    [ "$(readlink /proc/1/exe)" != "/usr/lib/systemd/systemd" ] && return 0
    
    # Check cgroup for container indicators
    grep -q 'docker\|lxc\|kubepods' /proc/1/cgroup 2>/dev/null && return 0
    
    return 1
}

# Function to start MariaDB based on environment
start_mariadb() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if is_container; then
        [[ "$quiet" != "true" ]] && echo "Container environment detected, starting MariaDB directly..."
        
        # Check if MariaDB is already running
        if pgrep -x "mariadbd" > /dev/null; then
            [[ "$quiet" != "true" ]] && echo "MariaDB is already running (PID: $(pgrep -x mariadbd))"
            return 0
        fi
        
        # Ensure mysql user owns the data directory
        chown -R mysql:mysql /var/lib/mysql 2>/dev/null || true
        
        # Start MariaDB using mysqld_safe
        [[ "$quiet" != "true" ]] && echo "Starting MariaDB with mysqld_safe..."
        if [[ "$quiet" == "true" ]]; then
            sudo -u mysql /usr/bin/mysqld_safe --datadir=/var/lib/mysql --user=mysql >/dev/null 2>&1 &
        else
            sudo -u mysql /usr/bin/mysqld_safe --datadir=/var/lib/mysql --user=mysql &
        fi
        
        # Wait for startup with timeout
        local timeout=30
        local count=0
        
        while [ $count -lt $timeout ]; do
            if pgrep -x "mariadbd" > /dev/null; then
                [[ "$quiet" != "true" ]] && echo "MariaDB started successfully (PID: $(pgrep -x mariadbd))"
                return 0
            fi
            sleep 1
            ((count++))
        done
        
        [[ "$quiet" != "true" ]] && echo "Failed to start MariaDB within ${timeout} seconds"
        return 1
    else
        [[ "$quiet" != "true" ]] && echo "Systemd environment detected, using systemctl..."
        if [[ "$quiet" == "true" ]]; then
            systemctl start mariadb -q
        else
            systemctl start mariadb
        fi
        return $?
    fi
}

# Function to stop MariaDB based on environment
stop_mariadb() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if is_container; then
        [[ "$quiet" != "true" ]] && echo "Container environment detected, stopping MariaDB directly..."
        
        # Try graceful shutdown first
        if command -v mysqladmin >/dev/null 2>&1; then
            if [[ "$quiet" == "true" ]]; then
                mysqladmin shutdown >/dev/null 2>&1 || true
            else
                mysqladmin shutdown 2>/dev/null || true
            fi
        fi
        
        # Force kill if still running
        pkill mysqld_safe 2>/dev/null || true
        pkill mariadbd 2>/dev/null || true
        
        # Wait for processes to stop
        local timeout=10
        local count=0
        while [ $count -lt $timeout ] && pgrep -x "mariadbd" > /dev/null; do
            sleep 1
            ((count++))
        done
        
        if pgrep -x "mariadbd" > /dev/null; then
            [[ "$quiet" != "true" ]] && echo "Warning: MariaDB may still be running"
            return 1
        else
            [[ "$quiet" != "true" ]] && echo "MariaDB stopped successfully"
            return 0
        fi
    else
        if [[ "$quiet" == "true" ]]; then
            systemctl stop mariadb -q
        else
            systemctl stop mariadb
        fi
        return $?
    fi
}

# Function to restart MariaDB based on environment
restart_mariadb() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if is_container; then
        [[ "$quiet" != "true" ]] && echo "Container environment detected, restarting MariaDB directly..."
        if [[ "$quiet" == "true" ]]; then
            stop_mariadb -q
            sleep 2
            start_mariadb -q
        else
            stop_mariadb
            sleep 2
            start_mariadb
        fi
        return $?
    else
        if [[ "$quiet" == "true" ]]; then
            systemctl restart mariadb -q
        else
            systemctl restart mariadb
        fi
        return $?
    fi
}

# Function to enable MariaDB service (only for systemd)
enable_mariadb() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if ! is_container; then
        if [[ "$quiet" == "true" ]]; then
            systemctl enable mariadb -q
        else
            systemctl enable mariadb
        fi
        return $?
    else
        [[ "$quiet" != "true" ]] && echo "Container environment: MariaDB service enable not applicable"
        return 0
    fi
}

# Function to get MariaDB status
status_mariadb() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if is_container; then
        if pgrep -x "mariadbd" > /dev/null; then
            if [[ "$quiet" == "true" ]]; then
                # In quiet mode, just return success/failure
                mysql -u root -e "SELECT 1;" &>/dev/null
                return $?
            else
                echo "MariaDB is running (PID: $(pgrep -x mariadbd))"
                # Try to connect to verify it's responding
                if mysql -u root -e "SELECT 1;" &>/dev/null; then
                    echo "MariaDB is accepting connections"
                else
                    echo "MariaDB is running but not accepting connections"
                fi
                return 0
            fi
        else
            [[ "$quiet" != "true" ]] && echo "MariaDB is not running"
            return 1
        fi
    else
        if [[ "$quiet" == "true" ]]; then
            systemctl status mariadb -q --no-pager
        else
            systemctl status mariadb --no-pager
        fi
        return $?
    fi
}

# Function to check if MariaDB is active
is_mariadb_active() {
    local quiet=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -q|--quiet)
                quiet=true
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
    
    if is_container; then
        if [[ "$quiet" == "true" ]]; then
            pgrep -x "mariadbd" > /dev/null 2>&1 && return 0 || return 1
        else
            pgrep -x "mariadbd" > /dev/null && return 0 || return 1
        fi
    else
        if [[ "$quiet" == "true" ]]; then
            systemctl is-active mariadb -q >/dev/null 2>&1 && return 0 || return 1
        else
            [[ "$(systemctl is-active mariadb)" = 'active' ]] && return 0 || return 1
        fi
    fi
}

# if [[ "$(systemctl is-active mariadb)" != 'active' ]]; then
#   systemctl start mariadb
# fi
if ! is_mariadb_active; then
    start_mariadb
fi

baseinfo() {
  echo
  echo "--------------------------------------------------------------------"
  echo "System Info ($VER)"
  echo "--------------------------------------------------------------------"  
  if [ ! -f /proc/user_beancounters ]; then
    lscpu
  else
    CPUNAME=$(cat /proc/cpuinfo | grep "model name" | cut -d ":" -f2 | tr -s " " | head -n 1)
    CPUCOUNT=$(cat /proc/cpuinfo | grep "model name" | cut -d ":" -f2 | wc -l)
    echo "CPU: $CPUCOUNT x$CPUNAME"
  fi
  uname -r
  cat /etc/redhat-release
  echo "--------------------------------------------------------------------"
  df -hT
  echo "--------------------------------------------------------------------"
  echo
}

fiosetup() {
  cd ${FIOBASEDIR}
  if [[ ! -f "${FIOBASEDIR}/reads.ini" || ! -f "${FIOBASEDIR}/writes.ini" || ! -f "${FIOBASEDIR}/reads-16k.ini" || ! -f "${FIOBASEDIR}/writes-16k.ini" ]]; then
    rm -rf reads.ini writes.ini reads-16k.ini writes-16k.ini
    \cp -f "${SCRIPT_DIR}/config/setio/reads.ini" reads.ini
    \cp -f "${SCRIPT_DIR}/config/setio/writes.ini" writes.ini
    cp reads.ini reads-16k.ini
    cp writes.ini writes-16k.ini
    sed -i 's|bs=4k|bs=16k|' reads-16k.ini
    sed -i 's|ba=4k|ba=16k|' reads-16k.ini
    sed -i 's|bs=4k|bs=16k|' writes-16k.ini
    sed -i 's|ba=4k|ba=16k|' writes-16k.ini
  fi
}

fiocheck() {
  if [ -f /usr/bin/fio ]; then
    fiosetup
    cd ${FIOBASEDIR}
    FIOR=$(fio --minimal reads-16k.ini | awk -F ';' '{print $8}')
    FIOW=$(fio --minimal writes-16k.ini | awk -F ';' '{print $49}')
    FIOR=$((FIOR*100000))
    FIOW=$((FIOW*100000))
    rm -rf sb-io-test 2>/dev/null
    echo -n "Full Reads: "
    echo "$((FIOR/100000))"
    echo -n "Full Writes: "
    echo "$((FIOW/100000))"
    echo -n "innodb_io_capacity = "
    echo $((FIOW/30/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/40/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/50/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/70/100000))
    echo
    if [[ "$((FIOW/100000))" -ge '1600001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/40/100000))
    elif [[ "$((FIOW/100000))" -lt '160000' && "$((FIOW/100000))" -ge '1400001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/35/100000))
    elif [[ "$((FIOW/100000))" -lt '140000' && "$((FIOW/100000))" -ge '1200001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/30/100000))
    elif [[ "$((FIOW/100000))" -lt '120000' && "$((FIOW/100000))" -ge '100001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/25/100000))
    elif [[ "$((FIOW/100000))" -lt '100000' && "$((FIOW/100000))" -ge '80001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/20/100000))
    elif [[ "$((FIOW/100000))" -lt '80000' && "$((FIOW/100000))" -ge '60001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/20/100000))
    elif [[ "$((FIOW/100000))" -lt '60000' && "$((FIOW/100000))" -ge '40001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/15/100000))
    elif [[ "$((FIOW/100000))" -lt '40000' && "$((FIOW/100000))" -ge '20001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/10/100000))      
    elif [[ "$((FIOW/100000))" -lt '20000' && "$((FIOW/100000))" -ge '10001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/10/100000))
    elif [[ "$((FIOW/100000))" -lt '10000' && "$((FIOW/100000))" -ge '5001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/8/100000))
    elif [[ "$((FIOW/100000))" -lt '5000' && "$((FIOW/100000))" -ge '3001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/6/100000))
    elif [[ "$((FIOW/100000))" -lt '3000' && "$((FIOW/100000))" -ge '2001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/5/100000))
    elif [[ "$((FIOW/100000))" -lt '2000' && "$((FIOW/100000))" -ge '1001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/4/100000))
    elif [[ "$((FIOW/100000))" -lt '1000' && "$((FIOW/100000))" -ge '501' ]]; then
      echo "innodb_io_capacity = 250"
    elif [[ "$((FIOW/100000))" -lt '500' && "$((FIOW/100000))" -ge '301' ]]; then
      echo "innodb_io_capacity = 200"
    elif [[ "$((FIOW/100000))" -lt '300' && "$((FIOW/100000))" -ge '201' ]]; then
      echo "innodb_io_capacity = 150"
    elif [[ "$((FIOW/100000))" -lt '200' && "$((FIOW/100000))" -ge '101' ]]; then
      echo "innodb_io_capacity = 100"
    elif [[ "$((FIOW/100000))" -lt '100' ]]; then
      echo "innodb_io_capacity = 100"
    fi
  fi
}

threadcal() {
  IOTHREADS=$((2*CPUS/4))
  if [ "$CPUS" -eq '1' ];then
    IOTHREADS=2
  fi
  if [ "$IOTHREADS" -lt '2' ];then
    IOTHREADS=2
  fi
  cat /etc/my.cnf | sed -e "s|innodb_read_io_threads = .*|innodb_read_io_threads = $IOTHREADS|g" | grep innodb_read_io_threads
  cat /etc/my.cnf | sed -e "s|innodb_write_io_threads = .*|innodb_write_io_threads = $IOTHREADS|g" | grep innodb_write_io_threads
}

infocheck() {
  baseinfo
  echo "--------------------------------------------------------------------"
  echo -n "$(fio -v)"; echo " calculated (IOPs)"
  echo "--------------------------------------------------------------------"
  echo
  if [[ "$TIME" = [yY] ]]; then
    time fiocheck
  else
    fiocheck
  fi
  echo
  if [[ "$TIME" = [yY] ]]; then
    time fiocheck
  else
    fiocheck
  fi
  echo
  echo "--------------------------------------------------------------------"
  threadcal
  echo "--------------------------------------------------------------------"
}

setbp() {
  INNODB_BPSIZE=$(${ALIAS_MYSQLADMIN} var | grep 'innodb_buffer_pool_size' | tr -s ' ' | awk '{print $4}')
  INNODB_BPTHRESHOLD='1073741824'
  for i in $(seq 1 ${CPUS});
    do
      if [[ "$(echo $(($INNODB_BPSIZE/$INNODB_BPTHRESHOLD)))" -eq "$i" ]]; then
        sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances=$i|g" /etc/my.cnf
        sed -i "s|innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances=$i|g" /etc/my.cnf
        # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_buffer_pool_instances = $i;"
        # /usr/bin/${ALIAS_MYSQL} -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
      fi
  done
  if [[ "$(echo $(($INNODB_BPSIZE/$INNODB_BPTHRESHOLD)))" -le '1' ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances=1|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances=1|g" /etc/my.cnf
    # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_buffer_pool_instances = 1;"
    # /usr/bin/${ALIAS_MYSQL} -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  fi
}

setthreads() {
  IOTHREADS=$((2*CPUS/4))
  if [ "$CPUS" -eq '1' ];then
    IOTHREADS=2
  fi
  if [ "$IOTHREADS" -lt '2' ];then
    IOTHREADS=2
  fi
  echo
  echo "+------------------------+-------+"
  echo "innodb io threads adjustment"
  echo "+------------------------+-------+"
  echo "existing value:"
  echo "+------------------------+-------+"
  grep '_io_threads' /etc/my.cnf
  echo "+------------------------+-------+"
  sed -i "s|innodb_read_io_threads = .*|innodb_read_io_threads = $IOTHREADS|g" /etc/my.cnf
  if [[ "$FIOW" -lt '600' ]]; then
    IOTHREADS=2
  fi
  sed -i "s|innodb_write_io_threads = .*|innodb_write_io_threads = $IOTHREADS|g" /etc/my.cnf
  echo "new value:"
  echo "+------------------------+-------+"
  grep '_io_threads' /etc/my.cnf
  echo "+------------------------+-------+"
  echo
  echo "Restart MySQL server for io thread changes"
  echo
}

setpurgethreads() {
  if [[ "$MDB_SVER" = '10.0' || "$MDB_SVER" = '10.1' || "$MDB_SVER" = '10.2' || "$MDB_SVER" = '10.3' ]]; then
    if [[ "$CPUS" -eq '1' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 1|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 1|g" /etc/my.cnf
    elif [[ "$CPUS" -eq '2' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 1|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 1|g" /etc/my.cnf
    elif [[ "$CPUS" -gt '2' && "$CPUS" -lt '4' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 2|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 2|g" /etc/my.cnf
    elif [[ "$CPUS" -ge '4' && "$CPUS" -lt '8' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 2|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 2|g" /etc/my.cnf
    elif [[ "$CPUS" -ge '8' && "$CPUS" -lt '16' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 4|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 4|g" /etc/my.cnf
    elif [[ "$CPUS" -ge '16' ]]; then
      sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 4|g" /etc/my.cnf
      sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 4|g" /etc/my.cnf
    fi
  elif [[ "$MDB_SVER" = '5.5' ]]; then
    sed -i "s|innodb_purge_threads=.*|innodb_purge_threads = 1|g" /etc/my.cnf
    sed -i "s|innodb_purge_threads = .*|innodb_purge_threads = 1|g" /etc/my.cnf
  fi
}

setconcurrency() {
  INNODB_CONCURRENT=$(((CPUS+2)*2))
  sed -i 's|^#innodb_thread_concurrency|innodb_thread_concurrency|g' /etc/my.cnf
  sed -i "s|innodb_thread_concurrency=.*|innodb_thread_concurrency = $INNODB_CONCURRENT|g" /etc/my.cnf
  sed -i "s|innodb_thread_concurrency = .*|innodb_thread_concurrency = $INNODB_CONCURRENT|g" /etc/my.cnf
  /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_thread_concurrency = $INNODB_CONCURRENT;"
}

ariatune() {
  if [[ "$FREEMEM" -gt '1040000' && "$FREEMEM" -lt '2000000' ]]; then
    ARIA_BUFFERSIZE='64M'
    ARIA_SORTBUFFERSIZE='32M'
    ARIA_LOGFILESIZE='128M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '1000000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '2080001' && "$FREEMEM" -lt '3120000' ]]; then
    ARIA_BUFFERSIZE='256M'
    ARIA_SORTBUFFERSIZE='64M'
    ARIA_LOGFILESIZE='256M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '1400000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '3120001' && "$FREEMEM" -lt '4160000' ]]; then
    ARIA_BUFFERSIZE='384M'
    ARIA_SORTBUFFERSIZE='96M'
    ARIA_LOGFILESIZE='384M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '2000000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '4160001' && "$FREEMEM" -lt '5200000' ]]; then
    ARIA_BUFFERSIZE='512M'
    ARIA_SORTBUFFERSIZE='128M'
    ARIA_LOGFILESIZE='512M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '2000000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '5200001' && "$FREEMEM" -lt '6240000' ]]; then
    ARIA_BUFFERSIZE='640M'
    ARIA_SORTBUFFERSIZE='160M'
    ARIA_LOGFILESIZE='640M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '2000000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '6240001' && "$FREEMEM" -lt '8320000' ]]; then
    ARIA_BUFFERSIZE='768M'
    ARIA_SORTBUFFERSIZE='192M'
    ARIA_LOGFILESIZE='768M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '2500000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  elif [[ "$FREEMEM" -gt '8320001' ]]; then
    ARIA_BUFFERSIZE='1024M'
    ARIA_SORTBUFFERSIZE='256M'
    ARIA_LOGFILESIZE='1024M'
    sed -i "s|aria_pagecache_buffer_size=.*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_pagecache_buffer_size = .*|aria_pagecache_buffer_size = $ARIA_BUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size=.*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    sed -i "s|aria_sort_buffer_size = .*|aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE|g" /etc/my.cnf
    if [[ "$MDB_DATADIRSIZE" -gt '4000000' ]]; then
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    elif [[ "$MDB_DATADIRSIZE" -gt '2500000' && "$MDB_DATADIRSIZE" -le '3999999' ]]; then
      ARIA_LOGFILESIZE='768M'
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    elif [[ "$MDB_DATADIRSIZE" -gt '2000000' && "$MDB_DATADIRSIZE" -le '2499999' ]]; then
      ARIA_LOGFILESIZE='512M'
      sed -i "s|aria_log_file_size=.*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      sed -i "s|aria_log_file_size = .*|aria_log_file_size = $ARIA_LOGFILESIZE|g" /etc/my.cnf
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_log_file_size = $ARIA_LOGFILESIZE;"
    fi
      # /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL aria_sort_buffer_size = $ARIA_SORTBUFFERSIZE;"
  fi
}

setio() {
  if [ -f /usr/bin/fio ]; then
    fiosetup
    cd ${FIOBASEDIR}
    FIOR=$(fio --minimal reads-16k.ini | awk -F ';' '{print $8}')
    FIOW=$(fio --minimal writes-16k.ini | awk -F ';' '{print $49}')
    FIOR=$((FIOR*100000))
    FIOW=$((FIOW*100000))
    FIOR=$((FIOR/100000))
    FIOW=$((FIOW/100000))
    rm -rf sb-io-test 2>/dev/null

    echo -n "Full Reads: "
    echo "$FIOR"
    echo -n "Full Writes: "
    echo "$FIOW"

    # adjust full reads by 50%
    FIOR=$((FIOR*3/5))
    FIOW=$((FIOW*3/5))

    # set innodb_flush_neighbors = 0 for SSD configurations
    # greater that disk I/O write speeds of 500 IOPs as some
    # non-SSD raid configs could still read 500 IOPs
    # detect appropriate variable differences between MariaDB 
    # 5.5 VS 10.x
    if [[ "$FIOW" -ge '500' ]]; then
      if [[ "$(echo $(${ALIAS_MYSQLADMIN} var | awk -F '|' '/innodb_flush_neighbors/ { print $2"="$3}') | grep 'innodb_flush_neighbors' >/dev/null 2>&1; echo $?)" = '0' ]]; then
        # MariaDB 10.x
        echo -e "\nset innodb_flush_neighbors = 0\n"
        sed -i "s|innodb_flush_neighbors = .*|innodb_flush_neighbors = 0|g" /etc/my.cnf
        /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_flush_neighbors = 0;"
        # append right after innodb_write_io_threads if innodb_flush_neighbors doesn't exist
        # in /etc/my.cnf
        if [[ "$(grep 'innodb_flush_neighbors' /etc/my.cnf >/dev/null 2>&1; echo $?)" != '0' ]]; then
          sed -i '/innodb_write_io_threads = .*/a innodb_flush_neighbors = 0' /etc/my.cnf
        fi
      elif [[ "$(echo $(${ALIAS_MYSQLADMIN} var | awk -F '|' '/innodb_flush_neighbor_pages/ { print $2"="$3}') | grep 'innodb_flush_neighbor_pages' >/dev/null 2>&1; echo $?)" = '0' ]]; then
        # MariaDB 5.5.x
        echo -e "\nset innodb_flush_neighbor_pages = 0\n"
        sed -i "s|innodb_flush_neighbor_pages = .*|innodb_flush_neighbor_pages = 0|g" /etc/my.cnf
        /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_flush_neighbor_pages = 0;"
        # append right after innodb_write_io_threads if innodb_flush_neighbors doesn't exist
        # in /etc/my.cnf
        if [[ "$(grep 'innodb_flush_neighbor_pages' /etc/my.cnf >/dev/null 2>&1; echo $?)" != '0' ]]; then
          sed -i '/innodb_write_io_threads = .*/a innodb_flush_neighbor_pages = 0' /etc/my.cnf
        fi
      fi
      # only need to insert missing variable if disk I/O writes greater than 500 IOPs
      # as variable defaults to 1 even without variable set specifically in /etc/my.cnf
    elif [[ "$FIOW" -lt '500' ]]; then
      if [[ "$(echo $(${ALIAS_MYSQLADMIN} var | awk -F '|' '/innodb_flush_neighbors/ { print $2"="$3}') | grep 'innodb_flush_neighbors' >/dev/null 2>&1; echo $?)" = '0' ]]; then
        # MariaDB 10.x
        echo -e "\nset innodb_flush_neighbors = 1\n"
        sed -i "s|innodb_flush_neighbors = .*|innodb_flush_neighbors = 1|g" /etc/my.cnf
        /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_flush_neighbors = 1;"
      elif [[ "$(echo $(${ALIAS_MYSQLADMIN} var | awk -F '|' '/innodb_flush_neighbor_pages/ { print $2"="$3}') | grep 'innodb_flush_neighbor_pages' >/dev/null 2>&1; echo $?)" = '0' ]]; then
        # MariaDB 5.5.x
        echo -e "\nset innodb_flush_neighbor_pages = 1\n"
        sed -i "s|innodb_flush_neighbor_pages = .*|innodb_flush_neighbor_pages = 1|g" /etc/my.cnf
        /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_flush_neighbor_pages = 1;"
      fi
    fi

    if [[ "$FIOW" -ge '160001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/55))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '160000' && "$FIOW" -ge '140001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/49))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '140000' && "$FIOW" -ge '120001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/44))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '120000' && "$FIOW" -ge '100001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/36))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '100000' && "$FIOW" -ge '80001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/33))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '80000' && "$FIOW" -ge '60001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/24))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '60000' && "$FIOW" -ge '40001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/20))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '40000' && "$FIOW" -ge '20001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/14))      
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '20000' && "$FIOW" -ge '10001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/11))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '10000' && "$FIOW" -ge '5001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/8))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '5000' && "$FIOW" -ge '3001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/6))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '3000' && "$FIOW" -ge '2001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/5))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '2000' && "$FIOW" -ge '1001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/4))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '1000' && "$FIOW" -ge '501' ]]; then
      FIOWSET=250
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '500' && "$FIOW" -ge '301' ]]; then
      FIOWSET=200
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '300' && "$FIOW" -ge '201' ]]; then
      FIOWSET=150
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '200' && "$FIOW" -ge '101' ]]; then
      FIOWSET=100
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '100' ]]; then
      FIOWSET=100
      echo "innodb_io_capacity = $FIOWSET"
    fi
  fi

  echo
  echo "+------------------------+-------+"
  echo "/etc/my.cnf adjustment"
  echo "+------------------------+-------+"
  # echo
  echo "existing value: "
  # grep 'innodb_io_capacity' /etc/my.cnf
  /usr/bin/${ALIAS_MYSQL} -e "SHOW VARIABLES like '%innodb_io_capacity%'"

  # sed -e "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET|g" /etc/my.cnf | grep 'innodb_io_capacity'
  sed -i "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET|g" /etc/my.cnf
  echo "new value: "
  # grep 'innodb_io_capacity' /etc/my.cnf
  /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_io_capacity = $FIOWSET;"
  IOMAX=$((FIOWSET*5/3))
  if [[ "$IOMAX" -gt "$FIOW" ]]; then
    IOMAX=$(echo "$IOMAX*0.66/1"|bc)
  fi
  IOMAXCHECK=$(grep 'innodb_io_capacity_max' /etc/my.cnf)
  if [[ -z "$IOMAXCHECK" ]]; then
    sed -i "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET\ninnodb_io_capacity_max = $IOMAX|g" /etc/my.cnf
  else
    sed -i "s|innodb_io_capacity_max = .*|innodb_io_capacity_max = $IOMAX|g" /etc/my.cnf
  fi
  # echo "new value: "
  # grep 'innodb_io_capacity_max' /etc/my.cnf
  /usr/bin/${ALIAS_MYSQL} -e "SET GLOBAL innodb_io_capacity_max = $IOMAX;"
  /usr/bin/${ALIAS_MYSQL} -e "SHOW VARIABLES like '%innodb_io_capacity%'"
}

case "$1" in
  check )
    infocheck
    ;;
  set )
    {
    /usr/bin/${ALIAS_MYSQL} -e "show engine innodb status\G" 2>&1 > ${CENTMINLOGDIR}/setio_innodbstatus-before-${DT}.log
    cat /etc/my.cnf >> ${CENTMINLOGDIR}/setio_innodbstatus-before-${DT}.log
    # setbp
    setio
    setthreads
    setpurgethreads
    # setconcurrency
    ariatune
    /usr/bin/${ALIAS_MYSQL} -e "show engine innodb status\G" 2>&1 > ${CENTMINLOGDIR}/setio_innodbstatus-after-${DT}.log
    cat /etc/my.cnf >> ${CENTMINLOGDIR}/setio_innodbstatus-after-${DT}.log
    } 2>&1 | tee ${CENTMINLOGDIR}/centminmod_setio_${DT}.log
    ;;
  * )
    echo "$0 {check|set}"
    ;;
esac
exit