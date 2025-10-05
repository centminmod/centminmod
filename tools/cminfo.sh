#!/bin/bash
#####################################################
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
#####################################################
# quick info overview for centminmod.com installs
#####################################################
branchname='141.00beta01'
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
#####################################################
MYCNF='/etc/my.cnf'
USER='root'
PASS=''
MYSQLHOST='localhost'
#####################################################
# local geoip server version used
VPS_GEOIPCHECK_V3='n'
VPS_GEOIPCHECK_V4='y'
#####################################################
CMINFO_SAR_MEM='y'
CMINFO_SAR_DAYS='7'
CMINFO_SAR_INTERVAL_DIR='/home/cminfo'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
CURRENTIP=$(echo $SSH_CLIENT | awk '{print $1}')
VIRTUALCORES=$(grep -c ^processor /proc/cpuinfo)
PHYSICALCPUS=$(grep 'physical id' /proc/cpuinfo | sort -u | wc -l)
CPUCORES=$(grep 'cpu cores' /proc/cpuinfo | head -n 1 | cut -d: -f2)
CPUSPEED=$(awk -F: '/cpu MHz/{print $2}' /proc/cpuinfo | sort | uniq -c)
CPUMODEL=$(awk -F: '/model name/{print $2}' /proc/cpuinfo | sort | uniq -c)
CPUCACHE=$(awk -F: '/cache size/{print $2}' /proc/cpuinfo | sort | uniq -c)

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
    CENTOSVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2)
    ALMALINUXVER=$(awk '{ print $4 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  else
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

if [ ! -f /usr/sbin/virt-what ]; then
    yum -y -q install virt-what
fi

if [ ! -f /usr/sbin/lshw ]; then
    yum -y -q install lshw
fi

if [ ! -f /usr/sbin/tcpdump ]; then
    yum -y -q install tcpdump
fi

if [ ! -f /usr/bin/tree ]; then
    yum -y -q install tree
fi

if [ ! -f /usr/bin/jq ]; then
    yum -y -q install jq
fi

if [ ! -f /usr/bin/smem ]; then
    yum -y -q install smem
fi

if [[ ! -f /usr/bin/datamash && -f /usr/bin/systemctl ]]; then
  if [[ "$CENTOS_TEN" -eq 10 ]]; then
    echo
    wget -q -4 https://centminmod.com/centminmodparts/rpms/datamash/el10/datamash-1.9-1.el10.x86_64.rpm
    echo
    yum -y localinstall datamash-1.9-1.el10.x86_64.rpm --allowerasing
    echo
  else
    yum -y -q install datamash
  fi
fi

if [ -f /root/.my.cnf ]; then
    MYSQLADMINOPT="--defaults-file=/root/.my.cnf -h $MYSQLHOST"
elif [ -z $PASS ]; then
    MYSQLADMINOPT="-h $MYSQLHOST"
else
    MYSQLADMINOPT="-u$USER -p$PASS -h $MYSQLHOST"
fi

#####################################################
if [[ -z "$SYSTYPE" ]]; then
    SYSTYPE='not virtualized'
fi

if [ -z ${CPUCORES} ]; then
CPUCORES='1'
fi

if [ -z ${PHYSICALCPUS} ]; then
PHYSICALCPUS='1'
fi

CPUCORES=$((${CPUCORES} * ${PHYSICALCPUS}));
    if [ ${CPUCORES} -gt 0 -a ${CPUCORES} -lt ${VIRTUALCORES} ]; then 
    HT=yes; 
    else HT=no; 
fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

if [ ! -f /root/.mytop ]; then
  echo -e "host=localhost\ndb=mysql\ndelay=2\nidle=0" > /root/.mytop
fi
if [ ! -f /usr/local/bin/mytop ]; then
  wget -q -4 https://gist.github.com/centminmod/14419caaba4f33ffdb240b526f46e8b5/raw/mytop.pl -O /usr/local/bin/mytop
  chmod +x /usr/local/bin/mytop
  echo -e "host=localhost\ndb=mysql\ndelay=2\nidle=0\nfullqueries=1" > /root/.mytop
fi

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

# Run the function to set client command variables
set_mariadb_client_commands

cmservice() {
  servicename=$1
  action=$2
  if [[ "$CENTOS_SIX" = '6' ]] && [[ "${servicename}" = 'haveged' || "${servicename}" = 'pure-ftpd' || "${servicename}" = 'mysql' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
    echo "service ${servicename} $action"
    if [[ "$CMSDEBUG" = [nN] ]]; then
      service "${servicename}" "$action"
    fi
  else
    if [[ "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' ]]; then
      echo "service ${servicename} $action"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        service "${servicename}" "$action"
      fi
    elif [[ "${servicename}" = 'mysql' || "${servicename}" = 'mysqld' ]]; then
      servicename='mariadb'
      echo "systemctl $action ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
        systemctl "$action" "${servicename}.service"
      fi
    fi
  fi
}

pidstat_php() {
    cron=$1
    pidstat_interval=$2
    if [[ "$cron" = 'cron' ]]; then
        if [[ "$pidstat_interval" ]]; then
            pidstat_sec=$pidstat_interval
        else
            pidstat_sec=20
        fi
    else
        if [[ "$pidstat_interval" ]]; then
            pidstat_sec=$pidstat_interval
        else
            pidstat_sec=10
        fi
    fi
    echo "------------------------------------------------------------------"
    echo "PHP-FPM pidstats"
    echo "------------------------------------------------------------------"
    echo "pidstat -durlh -C php-fpm | sed -e \"s|\$(hostname)|hostname|g\""
    pidstat -durlh -C php-fpm | sed -e "s|$(hostname)|hostname|g"
    echo
    echo "pidstat -durlh -C php-fpm 1 ${pidstat_sec} | sed -e \"s|\$(hostname)|hostname|g\""
    pidstat -durlh -C php-fpm 1 ${pidstat_sec} | sed -e "s|$(hostname)|hostname|g"
}

phpfpm_mem_stats() {
    cron=$1
    if [[ -f /usr/bin/systemctl && -f /usr/bin/smem && "$(smem -P 'php-fpm: pool' | grep -E -v 'python|Command')" ]]; then
        echo
        f=$(free -wk | awk '/Mem:/ {print $8}')
        cpu_c=$(nproc)
        count_php_masters=$(ps xao pid,ppid,command | grep 'php-fpm[:] master' | wc -l)
        if [[ ! "$count_php_masters" ]]; then
            count_php_masters=0
            list_php_masters==
        else
            get_phppool_names=$(ps aux | grep "php-fpm" | grep -v ^root | grep -v grep | grep pool| awk '{print $13}' | sort -u)
            map_pid_poolname=$(ps aux | grep "php-fpm" | grep -v ^root | grep -v grep | grep pool| awk '{print $2, $13}' | sort -n)
            display_phppool_names="$(echo -e "PHP-FPM Pool Names:\n$get_phppool_names")"
            count_php_masters=$count_php_masters
            list_php_masters=$(ps xao pid,command | grep 'php-fpm[:] master' | sed -e 's|(||g' -e 's|)||g' -e 's|process ||g')
        fi
        echo "------------------------------------------------------------------"
        echo "Total PHP-FPM Master Processes: $count_php_masters"
        echo "$display_phppool_names"
        echo "------------------------------------------------------------------"
        echo "$list_php_masters"
        echo "------------------------------------------------------------------"
        smem -P 'php-fpm: pool' | grep -E -v 'python|Command' | awk -v f=$f -v c=$cpu_c -v m=$count_php_masters '{swap+=$6; uss+=$7; pss+=$8; rss+=$9} END {print "Current Free Memory (KB): "f"\n""PHP-FPM Available Memory (KB): "f+rss"\n""Estimated Max PHP Children: "(f+rss)/(rss/NR)"\n""Estimated Max PHP Children To CPU Thread Ratio: "((f+rss)/(rss/NR)/c)"\nPHP-FPM Total Children: " NR " from "m" PHP-FPM master(s)" "\nPHP-FPM Total Used Memory (KB): ""swap:"swap, "uss:"uss, "pss:"pss, "rss:"rss"\n""PHP-FPM Average Per Child (KB): ""swap:"swap/NR, "uss:"uss/NR, "pss:"pss/NR, "rss:"rss/NR}'
        echo "uss = user set size"
        echo "pss = process set size"
        echo "rss = resident set size"
    elif [ ! "$(smem -P 'php-fpm: pool' | grep -E -v 'python|Command')" ]; then
        getpm_value=$(awk -F '= ' '/^pm =/ {print $2}' /usr/local/etc/php-fpm.conf)
        echo "PHP-FPM pm = $getpm_value in /usr/local/etc/php-fpm.conf"
        echo "PHP-FPM memory usage only viewable when pm = static"
    fi
}

sar_cpu_pc() {
    # cummulative period metrics
    if [ -f /usr/bin/datamash ]; then
        echo
        echo "------------------------------------------------------------------"
        echo " CPU Utilisation % Last $CMINFO_SAR_DAYS days ($(nproc) CPU Threads):"
        echo "------------------------------------------------------------------"
        for t in $(seq 0 $CMINFO_SAR_DAYS); do
            if [ -f "/var/log/sa/sa$(date +%d -d "$t day ago")" ]; then
                sar -u -f /var/log/sa/sa$(date +%d -d "$t day ago") >> "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log"
            fi
        done
        if [ -f "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log" ]; then
            sar_cpu_metrics_period=$(cat "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log" | grep -E -iv 'Linux|runq|user|mem|DEV|Average' | sed -e '1d' -e '/^ *$/d' | awk '{print $4,$5,$6,$7,$8,$9}' | datamash -W -R 2 --no-strict --filler 0 min 1-6 mean 1-6 max 1-6 perc:50 1-6 perc:75 1-6 perc:90 1-6 perc:95 1-6 perc:99 1-6 | column -t | xargs -n6 | awk '{print "%user:",$1, "%nice:",$2, "%system:",$3, "%iowait:",$4, "%steal:",$5, "%idle:",$6}')
            sar_cpu_umin_period=$(echo "$sar_cpu_metrics_period" | sed -n 1p)
            sar_cpu_uavg_period=$(echo "$sar_cpu_metrics_period" | sed -n 2p)
            sar_cpu_umax_period=$(echo "$sar_cpu_metrics_period" | sed -n 3p)
            sar_cpu_upc_period_50=$(echo "$sar_cpu_metrics_period" | sed -n 4p)
            sar_cpu_upc_period_75=$(echo "$sar_cpu_metrics_period" | sed -n 5p)
            sar_cpu_upc_period_90=$(echo "$sar_cpu_metrics_period" | sed -n 6p)
            sar_cpu_upc_period_95=$(echo "$sar_cpu_metrics_period" | sed -n 7p)
            sar_cpu_upc_period_99=$(echo "$sar_cpu_metrics_period" | sed -n 8p)
            echo -e "%CPU min: $sar_cpu_umin_period\n%CPU avg: $sar_cpu_uavg_period\n%CPU max: $sar_cpu_umax_period\n%CPU 50%: $sar_cpu_upc_period_50\n%CPU 75%: $sar_cpu_upc_period_75\n%CPU 90%: $sar_cpu_upc_period_90\n%CPU 95%: $sar_cpu_upc_period_95\n%CPU 99%: $sar_cpu_upc_period_99" | column -t
            rm -f "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log"
        else
            echo " Not enough sar data collected yet. Wait at least 24hrs."
        fi
    fi
    echo
    echo "------------------------------------------------------------------"
    echo " CPU Utilisation % Daily Last $CMINFO_SAR_DAYS days ($(nproc) CPU Threads):"
    echo "------------------------------------------------------------------"
    # daily metrics
    for t in $(seq 0 $CMINFO_SAR_DAYS); do
        if [ -f "/var/log/sa/sa$(date +%d -d "$t day ago")" ]; then
            sar_cpu_stats=$(sar -u -f /var/log/sa/sa$(date +%d -d "$t day ago"))
            sar_u=$(echo "$sar_cpu_stats" | grep 'Average:' | tail -1);
            if [ -f /usr/bin/datamash ]; then
                # display each day's cpu utilisation min, avg, max, 95% percentile numbers
                # instead report datamash calculated ones
                echo "$(date '+%b %d %Y' -d "$t day ago") %CPU";
                sar_cpu_metrics=$(echo "$sar_cpu_stats" | grep -E -iv 'Linux|runq|user|mem|DEV|Average' | sed -e '1d' -e '/^ *$/d' | awk '{print $4,$5,$6,$7,$8,$9}' | datamash -W -R 2 --no-strict --filler 0 min 1-6 mean 1-6 max 1-6 perc:95 1-6 | column -t | xargs -n6 | awk '{print "%user:",$1, "%nice:",$2, "%system:",$3, "%iowait:",$4, "%steal:",$5, "%idle:",$6}')
                sar_cpu_umin=$(echo "$sar_cpu_metrics" | sed -n 1p)
                sar_cpu_uavg=$(echo "$sar_cpu_metrics" | sed -n 2p)
                sar_cpu_umax=$(echo "$sar_cpu_metrics" | sed -n 3p)
                sar_cpu_upc=$(echo "$sar_cpu_metrics" | sed -n 4p)
                # echo "%CPU min: $sar_cpu_umin"
                # echo "%CPU avg: $sar_cpu_uavg"
                # echo "%CPU max: $sar_cpu_umax"
                # echo "%CPU 95%: $sar_cpu_upc"
                echo -e "%CPU min: $sar_cpu_umin\n%CPU avg: $sar_cpu_uavg\n%CPU max: $sar_cpu_umax\n%CPU 95%: $sar_cpu_upc" | column -t
            else
                # sar reported averages
                echo -n "$(date '+%b %d %Y' -d "$t day ago") %CPU ";
                echo "$sar_u" | awk '{print $1, "%user:",$3, "%nice:",$4, "%system:",$5, "%iowait:",$6, "%steal:",$7, "%idle:",$8}';
            fi
        fi
    done
    ##########################################################################
}

sar_cpu_pc_interval() {
    # cpu utilisation interval drilldown
    mkdir -p "$CMINFO_SAR_INTERVAL_DIR"
    mkdir -p "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days"
    if [ -f /usr/bin/datamash ]; then
        for t in $(seq 0 $CMINFO_SAR_DAYS); do
            if [ -f "/var/log/sa/sa$(date +%d -d "$t day ago")" ]; then
                sar -u -f /var/log/sa/sa$(date +%d -d "$t day ago") >> "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log"
            fi
        done
        echo
        echo "------------------------------------------------------------------"
        echo " CPU Utilisation % Interval Breakdown Summary $CMINFO_SAR_DAYS days ($(nproc) CPU Threads):"
        echo "------------------------------------------------------------------"
        if [ -f "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log" ]; then
            sar_cpu_metrics_period_raw_data=$(cat "${CENTMINLOGDIR}/cminfo-top-sar-cpu-period-${CMINFO_SAR_DAYS}-${DT}.log" | grep -E -iv 'Linux|runq|user|mem|DEV|Average' | sed -e '1d' -e '/^ *$/d')
            sar_cpu_metrics_period_raw_data_grep_fields=$(echo "$sar_cpu_metrics_period_raw_data" | awk '{print $1,$2}' | sort -u)
            echo "$sar_cpu_metrics_period_raw_data_grep_fields" | while read t tt; do
                newt="$t $tt";
                tformat=$(echo $newt | sed -e "s|\:||g" -e 's| ||g');
                # echo $tformat;
                echo "$sar_cpu_metrics_period_raw_data" | grep "$newt" | awk '{print $4,$5,$6,$7,$8,$9}' | datamash -W -R 2 --no-strict --filler 0 min 1-6 mean 1-6 max 1-6 perc:50 1-6 perc:75 1-6 perc:90 1-6 perc:99 1-6 | column -t | xargs -n6 | awk '{print "%user:",$1, "%nice:",$2, "%system:",$3, "%iowait:",$4, "%steal:",$5, "%idle:",$6}' > "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log";
                sar_cpu_umin_period_raw=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 1p)
                sar_cpu_uavg_period_raw=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 2p)
                sar_cpu_umax_period_raw=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 3p)
                sar_cpu_upc_period_raw_50=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 4p)
                sar_cpu_upc_period_raw_75=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 5p)
                sar_cpu_upc_period_raw_90=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 6p)
                # sar_cpu_upc_period_raw_95=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 7p)
                sar_cpu_upc_period_raw_99=$(cat "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat.log" | sed -n 7p)
                echo -e "${newt} %CPU min: $sar_cpu_umin_period_raw\n${newt} %CPU avg: $sar_cpu_uavg_period_raw\n${newt} %CPU max: $sar_cpu_umax_period_raw\n${newt} %CPU 50%: $sar_cpu_upc_period_raw_50\n${newt} %CPU 75%: $sar_cpu_upc_period_raw_75\n${newt} %CPU 90%: $sar_cpu_upc_period_raw_90\n${newt} %CPU 99%: $sar_cpu_upc_period_raw_99" | column -t > "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days/$tformat-summary.log";
            done
            find "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days" -type f -name "*AM-summary.log" | sort | while read f; do echo "------------------------";cat $f; done
            find "${CMINFO_SAR_INTERVAL_DIR}/sar-interval-averages-over-days" -type f -name "*PM-summary.log" | sort | while read f; do echo "------------------------";cat $f; done
        fi
    fi
    ##########################################################################
}

sar_mem_pc() {
    # only display on centos 7 systems
    if [[ "$CMINFO_SAR_MEM" = [yY] ]]; then
        echo
        echo "------------------------------------------------------------------"
        echo " Memory Usage Daily Last $CMINFO_SAR_DAYS days ($(nproc) CPU Threads):"
        echo "------------------------------------------------------------------"
        for t in $(seq 0 $CMINFO_SAR_DAYS); do
            if [ -f "/var/log/sa/sa$(date +%d -d "$t day ago")" ]; then
                sar_mem_stats=$(sar -r -f /var/log/sa/sa$(date +%d -d "$t day ago"))
                sar_mem=$(echo "$sar_mem_stats" | grep 'Average:' | tail -1);
                if [ -f /usr/bin/datamash ]; then
                    # display each day's cpu utilisation min, avg, max, 95% percentile numbers
                    # instead report datamash calculated ones
                    if [ -f /usr/bin/systemctl ]; then
                        echo
                        echo "$(date '+%b %d %Y' -d "$t day ago") Memory";
                    else
                        echo "$(date '+%b %d %Y' -d "$t day ago") Memory";
                    fi
                    if [ -f /usr/bin/systemctl ]; then
                        sar_mem_metrics=$(echo "$sar_mem_stats" | grep -E -iv 'Linux|runq|user|mem|DEV|Average' | sed -e '1d' -e '/^ *$/d' | awk '{print $3,$4,$5,$6,$7,$8,$9,$10,$11,$12}' | datamash -W -R 1 --no-strict --filler 0 min 1-10 mean 1-10 max 1-10 perc:95 1-10 | column -t | xargs -n10 | awk '{print "kbmemfree:",$1, "kbmemused:",$2, "%memused:",$3, "kbbuffers:",$4, "kbcached:",$5, "kbcommit:",$6, "%commit:",$7, "kbactive:",$8, "kbinact:",$9, "kbdirty:",$10}')
                    else
                        sar_mem_metrics=$(echo "$sar_mem_stats" | grep -E -iv 'Linux|runq|user|mem|DEV|Average' | sed -e '1d' -e '/^ *$/d' | awk '{print $3,$4,$5,$6,$7,$8,$9}' | datamash -W -R 1 --no-strict --filler 0 min 1-7 mean 1-7 max 1-7 perc:95 1-7 | column -t | xargs -n7 | awk '{print "kbmemfree:",$1, "kbmemused:",$2, "%memused:",$3, "kbbuffers:",$4, "kbcached:",$5, "kbcommit:",$6, "%commit:",$7')
                    fi
                    if [ -f /usr/bin/systemctl ]; then
                        sar_mem_umin=$(echo "$sar_mem_metrics" | sed -n 1p | xargs -n10)
                        sar_mem_uavg=$(echo "$sar_mem_metrics" | sed -n 2p | xargs -n10)
                        sar_mem_umax=$(echo "$sar_mem_metrics" | sed -n 3p | xargs -n10)
                        sar_mem_upc=$(echo "$sar_mem_metrics" | sed -n 4p | xargs -n10)
                    else
                        sar_mem_umin=$(echo "$sar_mem_metrics" | sed -n 1p)
                        sar_mem_uavg=$(echo "$sar_mem_metrics" | sed -n 2p)
                        sar_mem_umax=$(echo "$sar_mem_metrics" | sed -n 3p)
                        sar_mem_upc=$(echo "$sar_mem_metrics" | sed -n 4p)
                    fi
                    if [ -f /usr/bin/systemctl ]; then
                        echo -e "Memory min:\n$sar_mem_umin\nMemory avg:\n$sar_mem_uavg\nMemory max:\n$sar_mem_umax\nMemory 95%:\n$sar_mem_upc" | column -t
                    else
                        echo -e "Memory min: $sar_mem_umin\nMemory avg: $sar_mem_uavg\nMemory max: $sar_mem_umax\nMemory 95%: $sar_mem_upc" | column -t
                    fi
                else
                    # sar reported averages
                    echo -n "$(date '+%b %d %Y' -d "$t day ago") Memory ";
                    if [ -f /usr/bin/systemctl ]; then
                        echo "$sar_mem" | awk '{print $1, "kbmemfree:",$2, "kbmemused:",$3, "%memused:",$4, "kbbuffers:",$5, "kbcached:",$6, "kbcommit:",$7, "%commit:",$8, "kbactive:",$9, "kbinact:",$10, "kbdirty:",$11}'
                    else
                        echo "$sar_mem" | awk '{print $1, "kbmemfree:",$2, "kbmemused:",$3, "%memused:",$4, "kbbuffers:",$5, "kbcached:",$6, "kbcommit:",$7, "%commit:",$8}'
                    fi
                fi
            fi
        done
    fi
}

#####################################################
top_info() {
    cron="$1"
    top_load=" $2"
    SYSTYPE=$(virt-what | head -n1)
    CENTMINMOD_INFOVER=$(head -n1 /etc/centminmod-release)
    CCACHE_INFOVER=$(ccache -V | head -n1)
    NGINX_INFOVER=$(nginx -v 2>&1 | awk -F "/" '{print $2}' | head -n1)
    PHP_INFOVER=$(php-config --version)
    PHPOPENSSL_INFOVER=$(php --ri openssl | awk '/Library Version/ {print $6}')
    PHPCURL_INFOVER=$(php --ri curl | awk '/cURL Information/ {print $4}')
    PHPCURLSSL_INFOVER=$(php --ri curl | awk '/SSL Version/ {print $4}')
    MARIADB_INFOVER=$(rpm -qa | grep -i MariaDB-server | head -n1 | cut -d '-' -f3)
    MEMCACHEDSERVER_INFOVER=$(/usr/local/bin/memcached -h | head -n1 | awk '{print $2}')
    REDIS_INFOVER=$(/usr/bin/redis-cli --version | head -n1 | awk '{print $2}')
    CSF_INFOVER=$(csf -v | head -n1 | awk '{print $2}')
    SIEGE_INFOVER=$(siege -V 2>&1 | head -n1 | awk '{print $2}')
    APC_INFOVER=$(php --ri apc | awk '/Version/ {print $3}' | head -n1)
    OPCACHE_INFOVER=$(php -v 2>&1 | grep OPcache | awk '{print $4}' | sed 's/,//')
    
    if [[ "$(which nsd >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    NSD_INFOVER=$(nsd -v 2>&1 | head -n1 | awk '{print $3}')
    else
    NSD_INFOVER=" - "
    fi
    
    # only assign variables if mysql is running
    if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]] || [[ "$(ps -o comm -C mariadbd >/dev/null 2>&1; echo $?)" = '0' ]]; then
    DATABSELIST=$($ALIAS_MYSQL $MYSQLADMINOPT -e 'show databases;' | grep -Ev '(Database|information_schema|performance_schema)')
      if [[ "$CENTOS_TEN" -eq '10' ]]; then
        MYSQLUPTIME=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
        MYSQLUPTIMEFORMAT=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
        MYSQLSTART=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | grep -Ev '+--|server_start')
      elif [[ "$CENTOS_NINE" -eq '9' ]]; then
        MYSQLUPTIME=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
        MYSQLUPTIMEFORMAT=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
        MYSQLSTART=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | grep -Ev '+--|server_start')
      else
        MYSQLUPTIME=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
        MYSQLUPTIMEFORMAT=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
        MYSQLSTART=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | grep -Ev '+--|server_start')
      fi
    fi
    PAGESPEEDSTATUS=$(grep 'pagespeed unplugged' /usr/local/nginx/conf/pagespeed.conf)

    if [[ -z "$PAGESPEEDSTATUS" ]]; then
        PS=ON
    else
        PS=OFF
    fi
    
    if [ -f /usr/local/sbin/maldet ]; then
        MALDET_INFOVER=$(/usr/local/sbin/maldet -v | head -n1 | awk '{print $4}')
    fi
    
    if [ -f /usr/bin/clamscan ]; then
        CLAMAV_INFOVER=$(clamscan -V | head -n1 | awk -F "/" '{print $1}' | awk '{print $2}')
    fi

    if [[ "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        echo "------------------------------------------------------------------"
        $ALIAS_MYSQL $MYSQLADMINOPT --connect-timeout=5 -e "SET GLOBAL innodb_status_output=ON; SET GLOBAL innodb_status_output_locks=ON;" 2>/dev/null
        echo
    fi

    echo "------------------------------------------------------------------"
    echo " Centmin Mod Top Info:"
    echo "------------------------------------------------------------------"

    echo " Server Location Info"
    # echo
    # CMINFO_IPINFO=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | grep -E -vi 'ip:|phone|postal|loc|readme')
    # echo "$CMINFO_IPINFO" | grep -iv 'readme'
    if [[ "$VPS_GEOIPCHECK_V3" = [yY] ]]; then
      CMINFO_IPINFO=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} -A "$CURL_AGENT cmminfo IP CHECK${top_load} ${CPUCORES} $CURL_CPUMODEL $CURL_CPUSPEED $NGINX_INFOVER $PHP_INFOVER $MARIADB_INFOVER" https://geoip.centminmod.com/v3 | jq -r '"  city: \(.city)\n  region: \(.region)\n  country: \(.country)\n  org: \(.data.asn) \(.data.description_short)\n  timezone \(.timezone)"')
    elif [[ "$VPS_GEOIPCHECK_V4" = [yY] ]]; then
      CMINFO_IPINFO=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} -A "$CURL_AGENT cmminfo IP CHECK${top_load} ${CPUCORES} $CURL_CPUMODEL $CURL_CPUSPEED $NGINX_INFOVER $PHP_INFOVER $MARIADB_INFOVER" https://geoip.centminmod.com/v4 | jq -r '"  city: \(.city)\n  region: \(.region)\n  country: \(.country)\n  org: \(.asn) \(.asOrganization)\n  timezone \(.timezone)"')
    fi
    echo "$CMINFO_IPINFO"
    # echo "  ASN: $(curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/org 2>&1 | grep -iv 'readme')"
    
    echo
    echo " Processors" "physical = ${PHYSICALCPUS}, cores = ${CPUCORES}, virtual = ${VIRTUALCORES}, hyperthreading = ${HT}"
    echo
    echo "$CPUSPEED"
    echo "$CPUMODEL"
    echo "$CPUCACHE"
    echo ""
    
    if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
        echo -ne " System Up Since: \t"; uptime -s
        echo -ne " System Uptime: \t"; uptime -p
    else
        echo -ne " System Uptime: \t"; uptime | awk '{print $2, $3, $4, $5}'
    fi
    if [[ "$(ps -o comm -C mariadbd >/dev/null 2>&1; echo $?)" = '0' ]]; then
        echo -e " MySQL Server Started \t$MYSQLSTART"
        echo -e " MySQL Uptime: \t\t$MYSQLUPTIMEFORMAT"
        echo -e " MySQL Uptime (secs): \t$MYSQLUPTIME"
    elif [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
        echo -e " MySQL Server Started \t$MYSQLSTART"
        echo -e " MySQL Uptime: \t\t$MYSQLUPTIMEFORMAT"
        echo -e " MySQL Uptime (secs): \t$MYSQLUPTIME"
    else
        echo -e " MySQL Server Started \tnot running"
        echo -e " MySQL Uptime: \t\tnot running"
        echo -e " MySQL Uptime (secs): \tnot running"    
    fi
    echo -e " Server Type: \t\t$SYSTYPE"
    echo -e " CentOS Version: \t$CENTOSVER"
    echo -e " Centmin Mod: \t\t$CENTMINMOD_INFOVER"
    echo -e " Nginx PageSpeed: \t$PS"
    echo -e " Nginx Version: \t$NGINX_INFOVER"
    echo -e " PHP-FPM Version: \t$PHP_INFOVER"
    echo -e " PHP-FPM OpenSSL: \t$PHPOPENSSL_INFOVER"
    echo -e " PHP-FPM CURL Version: \t$PHPCURL_INFOVER"
    echo -e " PHP-FPM CURL SSL: \t$PHPCURLSSL_INFOVER"
    echo -e " MariaDB Version: \t$MARIADB_INFOVER"
    echo -e " CSF Firewall: \t\t$CSF_INFOVER"
    echo -e " Memcached Server: \t$MEMCACHEDSERVER_INFOVER"
    echo -e " Redis Server: \t$REDIS_INFOVER"
    echo -e " NSD Version: \t\t$NSD_INFOVER"
    # echo -e " Siege Version: \t$SIEGE_INFOVER"
    if [ -f /usr/local/sbin/maldet ]; then
        echo -e " Maldet Version: \t$MALDET_INFOVER"
    else
        echo -e " Maldet Version: \tnot installed"
    fi
    
    if [ -f /usr/bin/clamscan ]; then
        echo -e " ClamAV Version: \t$CLAMAV_INFOVER"
    else
        echo -e " ClamAV Version: \tnot installed"
    fi
    
    if [[ "$(rpm -qa elasticsearch)" ]]; then
        ESEXIST=y
        ELASTICSEARCH_INFOVER=$(rpm -qa elasticsearch | awk -F "-" '{print $2}')
        echo -e " ElasticSearch: \t$ELASTICSEARCH_INFOVER"
    else
        echo -e " ElasticSearch: \tnot installed"
    fi

    echo
    echo "------------------------------------------------------------------"
    echo "free -mtl"
    free -mtl
    echo
    echo "------------------------------------------------------------------"
    echo "top 10 processes using swap (VmSwap)"
    echo
    find /proc -maxdepth 2 -path "/proc/[0-9]*/status" -readable -exec awk -v FS=":" '{process[$1]=$2;sub(/^[ \t]+/,"",process[$1]);} END {if(process["VmSwap"] && process["VmSwap"] != "0 kB") printf "%10s %-30s %20s\n",process["Pid"],process["Name"],process["VmSwap"]}' '{}' \; | awk '{print $(NF-1),$0}' | sort -rh | cut -d " " -f2- | head -n10
    echo
    echo "------------------------------------------------------------------"
    echo "top 10 processes' virtual memory size (VmSize/VSZ)"
    echo "RSS vs VSZ https://stackoverflow.com/a/21049737/272648"
    echo "RSS https://en.wikipedia.org/wiki/Resident_set_size"
    # echo "VSZ https://en.wikipedia.org/wiki/Virtual_memory"
    echo
    find /proc -maxdepth 2 -path "/proc/[0-9]*/status" -readable -exec awk -v FS=":" '{process[$1]=$2;sub(/^[ \t]+/,"",process[$1]);} END {if(process["VmSize"] && process["VmSize"] != "0 kB") printf "%10s %-30s %20s\n",process["Pid"],process["Name"],process["VmSize"]}' '{}' \; | awk '{print $(NF-1),$0}' | sort -rh | cut -d " " -f2- | head -n10
    echo
    if [ -f /usr/bin/smem ]; then
        echo "------------------------------------------------------------------"
        echo "smem process memory info (sorted by RSS)"
        echo "PSS https://en.wikipedia.org/wiki/Proportional_set_size"
        echo "USS https://en.wikipedia.org/wiki/Unique_set_size"
        echo
        smem -rt -s rss
        echo
    fi
    echo "------------------------------------------------------------------"
    echo "df -hT"
    df -hT

    if [[ -f /usr/local/nginx/conf/conf.d/virtual.conf && -f /usr/local/nginx/conf/phpstatus.conf && "$(grep '^#include /usr/local/nginx/conf/phpstatus.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" ]]; then
        sed -i 's|^#include /usr/local/nginx/conf/phpstatus.conf;|include /usr/local/nginx/conf/phpstatus.conf;|' /usr/local/nginx/conf/conf.d/virtual.conf
        nprestart >/dev/null 2>&1
    fi
    if [[ -f /usr/local/nginx/conf/conf.d/virtual.conf && "$(grep '^include /usr/local/nginx/conf/phpstatus.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" && -f /usr/bin/fpmstats ]]; then
        echo
        echo "------------------------------------------------------------------"
        echo "php-fpm stats"
        phpfpm_mem_stats
        echo
        fpmstats
    elif [[ -f /usr/local/nginx/conf/conf.d/virtual.conf && "$(grep '^include /usr/local/nginx/conf/phpstatus.conf' /usr/local/nginx/conf/conf.d/virtual.conf)" && ! -f /usr/bin/fpmstats ]]; then
        echo
        echo "------------------------------------------------------------------"
        echo "php-fpm stats"
        phpfpm_mem_stats
        echo
        curl -s "localhost/phpstatus"
    fi
    echo
    echo "------------------------------------------------------------------"
    echo "Filter sar -q for times cpu load avg (1min) hit/exceeded cpu threads max"
    loadavg=$(printf "%0.2f" $(nproc))
    sarfilteredone=$(sar -q | sed -e "s|$(hostname)|hostname|g" | grep -v runq-sz | awk -v lvg=$loadavg '{if ($5>=lvg) print $0}' | grep -E -v 'Linux|Average')
    echo
    echo "${sarfilteredone:-no times found that >= $loadavg}"
    echo
    echo "------------------------------------------------------------------"
    echo "Filter sar -q for times cpu load avg (5min) hit/exceeded cpu threads max"
    loadavg=$(printf "%0.2f" $(nproc))
    sarfilteredfive=$(sar -q | sed -e "s|$(hostname)|hostname|g" | grep -v runq-sz | awk -v lvg=$loadavg '{if ($6>=lvg) print $0}' | grep -E -v 'Linux|Average')
    echo
    echo "${sarfilteredfive:-no times found that >= $loadavg}"
    echo
    echo "------------------------------------------------------------------"
    echo "sar -q | sed -e \"s|\$(hostname)|hostname|g\""
    sar -q | sed -e "s|$(hostname)|hostname|g"
    echo
    echo "------------------------------------------------------------------"
    echo "sar -r | sed -e \"s|\$(hostname)|hostname|g\""
    sar -r | sed -e "s|$(hostname)|hostname|g"
    echo
    echo "------------------------------------------------------------------"
    echo "sar -u | sed -e \"s|\$(hostname)|hostname|g\""
    sar -u | sed -e "s|$(hostname)|hostname|g"
    echo
    echo "------------------------------------------------------------------"
    if [ -d /usr/lib/systemd ]; then
        echo "top -bHn1 -w200"
        top -bHn1 -w200
    else
        echo "top -bHn1"
        top -bHn1
    fi
    echo
    if [[ "$(virt-what | grep -o lxc)" != 'lxc' ]]; then
        echo "------------------------------------------------------------------"
        echo "iotop -bton1 -P"
        iotop -bton1 -P
        echo
    fi
    # ensure mysql server is running before triggering mysqlreport output
    if [[ "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        echo "------------------------------------------------------------------"
        echo "MySQL InnoDB Status"
        if [[ "$cron" != 'cron' ]]; then
            echo
            echo "---"
            echo "MySQL InnoDB Monitor Statistics Gathering for 10 seconds"
            echo
            $ALIAS_MYSQL $MYSQLADMINOPT --connect-timeout=5 -e "SHOW ENGINE INNODB STATUS\G" >/dev/null
            sleep 10
            $ALIAS_MYSQL $MYSQLADMINOPT --connect-timeout=5 -e "SHOW ENGINE INNODB STATUS\G" 2>/dev/null
            echo
        elif [[ "$cron" = 'cron' ]]; then
            echo
            echo "---"
            echo "MySQL InnoDB Monitor Statistics Gathering for 60 seconds"
            echo
            # sleep 60
            $ALIAS_MYSQL $MYSQLADMINOPT --connect-timeout=5 -e "SHOW ENGINE INNODB STATUS\G select sleep(60); SHOW ENGINE INNODB STATUS\G" 2>/dev/null
            echo
        fi
        if [[ "$cron" != 'cron' ]]; then
            $ALIAS_MYSQL $MYSQLADMINOPT --connect-timeout=5 -e "SET GLOBAL innodb_status_output=OFF; SET GLOBAL innodb_status_output_locks=OFF;" 2>/dev/null
        fi
        echo
    fi
    if [[ "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        echo "------------------------------------------------------------------"
        echo "$ALIAS_MYSQLADMIN var"
        $ALIAS_MYSQLADMIN var | tr -s ' ' | grep -E -v '+-' 2>/dev/null
        echo
        echo "$ALIAS_MYSQLADMIN ext"
        $ALIAS_MYSQLADMIN ext  | tr -s ' ' | grep -E -v '+-' 2>/dev/null
        echo
    fi
    if [[ "$CMINFO_MYSQL_PROCLIST" = [Yy] && "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        echo "------------------------------------------------------------------"
        if [[ "$cron" = 'cron' ]]; then
            proc_runs=20
        else
            proc_runs=5
        fi
        echo "$ALIAS_MYSQLADMIN proc -i1 -c${proc_runs}"
        $ALIAS_MYSQLADMIN proc -i1 -c${proc_runs} 2>/dev/null
        echo
    fi
    if [[ "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' && -f /root/mysqlreport ]]; then
        echo "------------------------------------------------------------------"
        echo "mysqlreport"
        /root/mysqlreport 2>/dev/null
        echo
    fi
    if [[ "$CMINFO_MYTOP" = [Yy] && "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' && -f /usr/bin/mytop && -f /root/.mytop ]]; then
        echo "------------------------------------------------------------------"
        echo "mytop -b"
        mytop -b 2>/dev/null
        echo
    fi
    if [[ "$PT_SUMMARY_REPORT" = [Yy] ]]; then
        if [[ "$(uname -m)" = 'x86_64' && ! -f /usr/bin/pt-summary ]]; then
            # 64bit OS only
            yum -q -y install percona-toolkit --enablerepo=percona-release-x86_64
        fi
        if [[ "$($ALIAS_MYSQLADMIN ping -s >/dev/null 2>&1; echo $?)" -eq '0' && -f /usr/bin/pt-summary ]]; then
            echo "------------------------------------------------------------------"
            /usr/bin/pt-summary 2>/dev/null | sed -e 's|Percona Toolkit ||g'
            echo
        fi
    fi
    echo "------------------------------------------------------------------"
    if [[ "$cron" = 'cron' ]]; then
        pidstat_sec=20
    else
        pidstat_sec=10
    fi
    echo "pidstat -durh 1 ${pidstat_sec} | sed -e \"s|\$(hostname)|hostname|g\""
    echo "..."
    pidstat -durh 1 ${pidstat_sec} | sed -e "s|$(hostname)|hostname|g"

    sar_cpu_pc
    sar_mem_pc
    echo "------------------------------------------------------------------"
    echo "Stats saved at: ${CENTMINLOGDIR}/cminfo-top-${DT}.log"
    echo "------------------------------------------------------------------"
    echo
}

# Function to count the number of connections in SYN_RECV state
count_syn_recv() {
    syn_recv_count=$(ss -H state syn-recv | wc -l)
    echo "Number of SYN_RECV connections: $syn_recv_count"
}

# Function to count the number of outgoing SYN-ACK packets with a timeout of 10 seconds
count_syn_ack() {
    temp_file=$(mktemp) # Create a temporary file to store tcpdump output

    # Run tcpdump with timeout and capture the output in the temp file
    timeout 10 tcpdump -n -c 100 'tcp[tcpflags] & tcp-ack != 0 and tcp[tcpflags] & tcp-syn != 0' > "$temp_file" 2>/dev/null
    
    # Check if there were any packet captures
    packet_count=$(grep -c "IP" "$temp_file")  # Count lines that start with "IP" (indicating packets)

    if [ "$packet_count" -gt 0 ]; then
        echo "Number of outgoing SYN-ACK packets captured for last 100 connections: $packet_count"
    else
        echo "No SYN-ACK packets captured in the last 10 seconds."
    fi

    # Clean up the temporary file
    rm -f "$temp_file"
}

syn_info() {
  echo "------------------------------------------------------------------"
  echo "SYN Flood Report: SYN_RECV & Outgoing SYN-ACK Connections"
  echo "------------------------------------------------------------------"
  count_syn_recv
  count_syn_ack
  echo "------------------------------------------------------------------"
}

netstat_info() {
    netstat_load=$1
    sshclient=$(echo $SSH_CLIENT | awk '{print $1}')
    nic=$(ifconfig -s 2>&1 | grep -E -v '^Iface|^lo|^gre' | awk '{print $1}')
    bandwidth_avg=$(sar -n DEV 1 1)
    bandwidth_inout=$(echo "$nic" | while read i; do echo "$bandwidth_avg" | grep 'Average:' | awk -v tnic="$i" '$0~tnic{print tnic, "In: ",$5,"Out:",$6}'; done | column -t)
    packets_inout=$(echo "$nic" | while read i; do echo "$bandwidth_avg" | grep 'Average:' | awk -v tnic="$i" '$0~tnic{print tnic, "In: ",$3,"Out:",$3}'; done | column -t)
    netstat_http=$(netstat -an | fgrep ':80 ')
    netstat_https=$(netstat -an | fgrep ':443 ')
    netstat_outbound=$(netstat -plant | grep -E -v 'and|servers|Address' | awk '{print $5,$6,$7}' | grep -v ':\*' | grep -v '127.0.0.1' | sed -e "s|$sshclient|ssh-client-ip|g" | sort | uniq -c | sort -rn | head -n10 | column -t)
    netstat_ips=$(netstat -tn)
    netstat_ipstop=$(echo "$netstat_ips" | grep -E -v 'servers|Address' | awk '{print $5}' | rev | cut -d: -f2- | rev | sort | uniq -c | sort -rn | head -n10)
    netstat_ipstopf=$(echo "$netstat_ipstop" | awk '{"getent hosts " $2 | getline getent_hosts_str; split(getent_hosts_str, getent_hosts_arr, " "); print $1, $2, getent_hosts_arr[2], $3}' | sed -e "s|$sshclient|ssh-client-ip|g" | column -t)
    tt_states_http=$(echo "$netstat_http" | awk '{print $6}' | sort | uniq -c | sort -n)
    tt_states_https=$(echo "$netstat_https" | awk '{print $6}' | sort | uniq -c | sort -n)
    uniq_states_http=$(echo "$netstat_http" | fgrep -v "0.0.0.0" | awk '{print $6}' | sort | uniq -c | sort -n)
    uniq_states_https=$(echo "$netstat_https" | fgrep -v "0.0.0.0" | awk '{print $6}' | sort | uniq -c | sort -n)
    ttconn_http=$(echo "$tt_states_http" | awk '{sum += $1} END {print sum;}')
    ttconn_https=$(echo "$tt_states_https" | awk '{sum += $1} END {print sum;}')
    uniqconn_http=$(echo "$uniq_states_http" | awk '{sum += $1} END {print sum;}')
    uniqconn_https=$(echo "$uniq_states_https" | awk '{sum += $1} END {print sum;}')
    econn_http=$(echo "$tt_states_http" | awk '/ESTABLISHED/ {print $1}')
    econn_https=$(echo "$tt_states_https" | awk '/ESTABLISHED/ {print $1}')
    wconn_http=$(echo "$tt_states_http" | awk '/TIME_WAIT/ {print $1}')
    wconn_https=$(echo "$tt_states_https" | awk '/TIME_WAIT/ {print $1}')
    
    echo "------------------------------------------------------------------"
    echo " Centmin Mod Netstat Info:"
    echo "------------------------------------------------------------------"
    echo -e "\nNetwork Bandwidth In/Out (KB/s):"
    echo "$bandwidth_inout"
    echo -e "\nNetwork Packets   In/Out (pps):"
    echo "$packets_inout"
    echo -e "\nTotal Connections For:"
    echo "Port 80:   $ttconn_http"
    echo "Port 443:  $ttconn_http"
    echo -e "\nUnique IP Connections For:"
    echo "Port 80:   $uniqconn_http"
    echo "Port 443:  $uniqconn_http"
    echo -e "\nEstablished Connections For:"
    echo "Port 80:   ${econn_http:-0}"
    echo "Port 443:  ${econn_https:-0}"
    echo -e "\nTIME_WAIT Connections For:"
    echo "Port 80:   ${wconn_http:-0}"
    echo "Port 443:  ${wconn_https:-0}"
    echo -e "\nTop IP Address Connections:"
    echo "$netstat_ipstopf"
    echo -e "\nTop Outbound Connections:"
    echo "$netstat_outbound"

    if [ -f /etc/csf/csf.deny ]; then
        echo -e "\nTop CSF Firewall Denied Country Codes:"
        csfdeny_country=$(grep -oP '(?<=\()[^\)]+' /etc/csf/csf.deny | awk -F "/" 'length($1)<=2 {print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_country"

        echo -e "\nTop CSF Firewall Denied Country Codes + Reverse Lookups:"
        csfdeny_iplookups=$(grep -oP '(?<=\()[^\)]+' /etc/csf/csf.deny | grep -wv sshd | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_iplookups"

        echo -e "\nTop CSF Firewall Denied Distributed sshd Attacks:"
        csfdeny_sshdlookups=$(grep 'distributed sshd attacks' /etc/csf/csf.deny | grep -oP '(?<=\()[^\)]+' | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshdlookups"

        echo -e "\nTop CSF Firewall Denied Distributed sshd Attacks Target Usernames:"
        csfdeny_attackusernames=$(grep 'distributed sshd attacks' /etc/csf/csf.deny | grep -oP '(?<=\[)[^\]]+' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_attackusernames"

        echo -e "\nTop CSF Firewall Failed SSH Logins:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | grep -oP '(?<=\()[^\)]+' | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        echo -e "\nTop CSF Firewall Failed SSH Logins IPs:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | awk '{print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        echo -e "\nLast 24hrs Top CSF Firewall Denied Country Codes:"
        csfdeny_country=$(grep "$(date -d "1 day ago"  +"%a %b  %-d")" /etc/csf/csf.deny | grep -oP '(?<=\()[^\)]+' | awk -F "/" 'length($1)<=2 {print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_country"

        echo -e "\nLast 24hrs Top CSF Firewall Denied Country Codes + Reverse Lookups:"
        csfdeny_iplookups=$(grep "$(date -d "1 day ago"  +"%a %b  %-d")" /etc/csf/csf.deny | grep -oP '(?<=\()[^\)]+' | grep -wv sshd | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_iplookups"

        echo -e "\nLast 24hrs Top CSF Firewall Denied Distributed sshd Attacks:"
        csfdeny_sshdlookups=$(grep 'distributed sshd attacks' /etc/csf/csf.deny | grep "$(date -d "1 day ago"  +"%a %b  %-d")" | grep -oP '(?<=\()[^\)]+' | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshdlookups"

        echo -e "\nLast 24hrs Top CSF Firewall Failed SSH Logins:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | grep "$(date -d "1 day ago"  +"%a %b  %-d")" | grep -oP '(?<=\()[^\)]+' | awk -F "/" 'length($1)<=2 {print $1,$2,$3}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        echo -e "\nLast 24hrs Top CSF Firewall Failed SSH Logins IPs:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | grep "$(date -d "1 day ago"  +"%a %b  %-d")" | awk '{print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        echo -e "\nLast 3hrs Top CSF Firewall Failed SSH Logins IPs:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | grep -E "$(date -d "1 hour ago"  +"%a %b  %-d %H")|$(date -d "2 hour ago"  +"%a %b  %-d %H")|$(date -d "3 hour ago"  +"%a %b  %-d %H")" | awk '{print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        echo -e "\nLast 1hr Top CSF Firewall Failed SSH Logins IPs:"
        csfdeny_sshlogins=$(grep 'Failed SSH login from' /etc/csf/csf.deny | grep "$(date -d "1 hour ago"  +"%a %b  %-d")" | awk '{print $1}' | sort | uniq -c | sort -rn | head -n10 | column -t)
        echo "$csfdeny_sshlogins"

        # STARTD=$(date -d "1440 mins ago"  +"%a %b  %-d %H:%M")
        # ENDD=$(date +"%a %b  %-d %H:%M")
    fi
}

list_logs() {
    echo
    echo "List all /root/centminlogs in data ascending order"
    ls -lhrt /root/centminlogs
}

setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
CMINFOLINK='https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/tools/cminfo.sh'

# fallback mirror
curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 \$CMINFOLINK | grep -qE '^HTTP/.* 200'
CMINFO_CURLCHECK=\$?
if [[ "\$CMINFO_CURLCHECK" != '0' ]]; then
    CMINFOLINK='https://gitlab.com/centminmod-github-mirror/centminmod/raw/master/tools/cminfo.sh'
fi
wget -q --no-check-certificate -O /usr/bin/cminfo "\$CMINFOLINK"
chmod 0700 /usr/bin/cminfo
EOF
    chmod 0700 /usr/bin/cminfo_updater
    # echo "cminfo configured"
}

if [ ! -x /usr/bin/cminfo ]; then
    chmod 0700 /usr/bin/cminfo
fi

if [ ! -f /usr/bin/cminfo_updater ]; then
    setupdate
else
    setupdate
fi

if [ ! -f /usr/bin/crontab ]; then
    yum -q -y install cronie
fi

# insert itself into cronjob for auto updates
if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    mkdir -p /etc/centminmod/cronjobs
    cp cronjoblist /etc/centminmod/cronjobs/cronjoblist-before-cminfo-setup.txt
    echo "*/4 * * * * /usr/bin/cminfo_updater 2>/dev/null" >> cronjoblist
    cp cronjoblist /etc/centminmod/cronjobs/cronjoblist-after-cminfo-setup.txt
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

infooutput() {
VHOSTS=$(ls /home/nginx/domains | grep -E -v 'demodomain.com.conf')
VHOSTSCONF=$(ls /usr/local/nginx/conf/conf.d | grep -E -vw '^ssl.conf' | uniq)

#####################################################
SYSTYPE=$(virt-what | head -n1)
CENTMINMOD_INFOVER=$(head -n1 /etc/centminmod-release)
CCACHE_INFOVER=$(ccache -V | head -n1)
NGINX_INFOVER=$(nginx -v 2>&1 | awk -F "/" '{print $2}' | head -n1)
PHP_INFOVER=$(php-config --version)
PHPOPENSSL_INFOVER=$(php --ri openssl | awk '/Library Version/ {print $6}')
PHPCURL_INFOVER=$(php --ri curl | awk '/cURL Information/ {print $4}')
PHPCURLSSL_INFOVER=$(php --ri curl | awk '/SSL Version/ {print $4}')
MARIADB_INFOVER=$(rpm -qa | grep -i MariaDB-server | head -n1 | cut -d '-' -f3)
MEMCACHEDSERVER_INFOVER=$(/usr/local/bin/memcached -h | head -n1 | awk '{print $2}')
CSF_INFOVER=$(csf -v | head -n1 | awk '{print $2}')
SIEGE_INFOVER=$(siege -V 2>&1 | head -n1 | awk '{print $2}')
APC_INFOVER=$(php --ri apc | awk '/Version/ {print $3}' | head -n1)
OPCACHE_INFOVER=$(php -v 2>&1 | grep OPcache | awk '{print $4}' | sed 's/,//')

if [[ "$(which nsd >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
  NSD_INFOVER=$(nsd -v 2>&1 | head -n1 | awk '{print $3}')
else
  NSD_INFOVER=" - "
fi

# only assign variables if mysql is running
if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]] || [[ "$(ps -o comm -C mariadbd >/dev/null 2>&1; echo $?)" = '0' ]]; then
DATABSELIST=$($ALIAS_MYSQL $MYSQLADMINOPT -e 'show databases;' | grep -Ev '(Database|information_schema|performance_schema)')
if [[ "$CENTOS_NINE" -eq '9' ]]; then
  MYSQLUPTIME=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
  MYSQLUPTIMEFORMAT=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
  MYSQLSTART=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | grep -Ev '+--|server_start')
else
  MYSQLUPTIME=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
  MYSQLUPTIMEFORMAT=$($ALIAS_MYSQLADMIN $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
  MYSQLSTART=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | grep -Ev '+--|server_start')
fi
fi
PAGESPEEDSTATUS=$(grep 'pagespeed unplugged' /usr/local/nginx/conf/pagespeed.conf)

if [[ -z "$PAGESPEEDSTATUS" ]]; then
    PS=ON
else
    PS=OFF
fi

if [ -f /usr/local/sbin/maldet ]; then
    MALDET_INFOVER=$(/usr/local/sbin/maldet -v | head -n1 | awk '{print $4}')
fi

if [ -f /usr/bin/clamscan ]; then
    CLAMAV_INFOVER=$(clamscan -V | head -n1 | awk -F "/" '{print $1}' | awk '{print $2}')
fi

echo "------------------------------------------------------------------"
echo " Centmin Mod Quick Info:"
echo "------------------------------------------------------------------"

echo "Server Location Info"
# echo
if [[ "$VPS_GEOIPCHECK_V3" = [yY] ]]; then
  CMINFO_IPINFO=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} -A "$CURL_AGENT cmminfo IP CHECK ${CPUCORES} $CURL_CPUMODEL $$CURL_CPUSPEED $NGINX_INFOVER $PHP_INFOVER $MARIADB_INFOVER" https://geoip.centminmod.com/v3 | jq -r '"  city: \(.city)\n  region: \(.region)\n  country: \(.country)\n  org: \(.data.asn) \(.data.description_short)\n  timezone \(.timezone)"')
elif [[ "$VPS_GEOIPCHECK_V4" = [yY] ]]; then
  CMINFO_IPINFO=$(curl -${ipv_forceopt}s${CURL_TIMEOUTS} -A "$CURL_AGENT cmminfo IP CHECK ${CPUCORES} $CURL_CPUMODEL $$CURL_CPUSPEED $NGINX_INFOVER $PHP_INFOVER $MARIADB_INFOVER" https://geoip.centminmod.com/v4 | jq -r '"  city: \(.city)\n  region: \(.region)\n  country: \(.country)\n  org: \(.asn) \(.asOrganization)\n  timezone \(.timezone)"')
fi
echo "$CMINFO_IPINFO"

echo
echo "Processors" "physical = ${PHYSICALCPUS}, cores = ${CPUCORES}, virtual = ${VIRTUALCORES}, hyperthreading = ${HT}"
echo
echo "$CPUSPEED"
echo "$CPUMODEL"
echo "$CPUCACHE"
echo ""

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    echo -ne " System Up Since: \t"; uptime -s
    echo -ne " System Uptime: \t"; uptime -p
else
    echo -ne " System Uptime: \t"; uptime | awk '{print $2, $3, $4, $5}'
fi
if [[ "$(ps -o comm -C mariadbd >/dev/null 2>&1; echo $?)" = '0' ]]; then
    echo -e " MySQL Server Started \t$MYSQLSTART"
    echo -e " MySQL Uptime: \t\t$MYSQLUPTIMEFORMAT"
    echo -e " MySQL Uptime (secs): \t$MYSQLUPTIME"
elif [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
    echo -e " MySQL Server Started \t$MYSQLSTART"
    echo -e " MySQL Uptime: \t\t$MYSQLUPTIMEFORMAT"
    echo -e " MySQL Uptime (secs): \t$MYSQLUPTIME"
else
    echo -e " MySQL Server Started \tnot running"
    echo -e " MySQL Uptime: \t\tnot running"
    echo -e " MySQL Uptime (secs): \tnot running"    
fi
echo -e " Server Type: \t\t$SYSTYPE"
echo -e " CentOS Version: \t$CENTOSVER"
echo -e " Centmin Mod: \t\t$CENTMINMOD_INFOVER"
echo -e " Nginx PageSpeed: \t$PS"
echo -e " Nginx Version: \t$NGINX_INFOVER"
echo -e " PHP-FPM Version: \t$PHP_INFOVER"
echo -e " PHP-FPM OpenSSL: \t$PHPOPENSSL_INFOVER"
echo -e " PHP-FPM CURL Version: \t$PHPCURL_INFOVER"
echo -e " PHP-FPM CURL SSL: \t$PHPCURLSSL_INFOVER"
echo -e " MariaDB Version: \t$MARIADB_INFOVER"
echo -e " CSF Firewall: \t\t$CSF_INFOVER"
echo -e " Memcached Server: \t$MEMCACHEDSERVER_INFOVER"
echo -e " Redis Server: \t$REDIS_INFOVER"
echo -e " NSD Version: \t\t$NSD_INFOVER"
# echo -e " Siege Version: \t$SIEGE_INFOVER"
if [ -f /usr/local/sbin/maldet ]; then
    echo -e " Maldet Version: \t$MALDET_INFOVER"
else
    echo -e " Maldet Version: \tnot installed"
fi

if [ -f /usr/bin/clamscan ]; then
    echo -e " ClamAV Version: \t$CLAMAV_INFOVER"
else
    echo -e " ClamAV Version: \tnot installed"
fi

if [[ "$(rpm -qa elasticsearch)" ]]; then
    ESEXIST=y
    ELASTICSEARCH_INFOVER=$(rpm -qa elasticsearch | awk -F "-" '{print $2}')
    echo -e " ElasticSearch: \t$ELASTICSEARCH_INFOVER"
else
    echo -e " ElasticSearch: \tnot installed"
fi

echo "------------------------------------------------------------------"
echo
echo "------------------------------------------------------------------"
echo " Site Nginx Vhost Accounts:"
echo "------------------------------------------------------------------"
echo
for d in $VHOSTS; do 
    echo -n "* $d: ";
    tree -dl --noreport -L 1 "/home/nginx/domains/${d}"
done

echo "------------------------------------------------------------------"
echo
echo "------------------------------------------------------------------"
echo " Site Nginx Vhost Config Files:"
echo "------------------------------------------------------------------"
echo
for c in $VHOSTSCONF; do 
    echo "* /usr/local/nginx/conf/conf.d/$c"; 
done

if [[ -z "$(cmservice mysql status | grep not)" ]]; then
echo
echo "------------------------------------------------------------------"
echo " MySQL Databases:"
echo "------------------------------------------------------------------"
echo
for db in $DATABSELIST; do 
DBIDXSIZE=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT CONCAT(ROUND(SUM(index_length)/(1024*1024), 2), ' MB') AS 'Total Index Size' FROM information_schema.TABLES WHERE table_schema LIKE '$db';" | grep -Ev '(+-|Total Index Size)')
DBDATASIZE=$($ALIAS_MYSQL $MYSQLADMINOPT -e "SELECT CONCAT(ROUND(SUM(data_length)/(1024*1024), 2), ' MB') AS 'Total Data Size'
FROM information_schema.TABLES WHERE table_schema LIKE '$db';" | grep -Ev '(+-|Total Data Size)')

if [ "$DBIDXSIZE" == 'NULL' ]; then
    DBIDXSIZE='0.00 MB'
fi

if [ "$DBDATASIZE" == 'NULL' ]; then
    DBDATASIZE='0.00 MB'
fi
    echo -e "* $db\t[idx: $DBIDXSIZE data: $DBDATASIZE]"; 
done
fi

echo
echo "------------------------------------------------------------------"
echo " System User Ids >81:"
echo "------------------------------------------------------------------"
echo
awk -F':' '{ if($3 >= 81) print $0 }' /etc/passwd

if [ -f /usr/bin/pure-pw ]; then
echo
echo "------------------------------------------------------------------"
echo " Pure-FTP Virtual FTP Info:"
echo "------------------------------------------------------------------"
echo
pure-pw list

echo
for u in $(pure-pw list | awk '{print $1}'); do 
echo "-------------------------------------"
echo "Virtual FTP user: $u"; 
echo "password displayed is encrypted"; 
pure-pw show $u; 
done
echo
fi

echo
echo "------------------------------------------------------------------"
echo " Nginx Configuration:"
echo "------------------------------------------------------------------"
echo
nginx -V 2>&1 | fold -w 80 -s

echo
echo "------------------------------------------------------------------"
echo " Nginx Settings:"
echo "------------------------------------------------------------------"
echo

grep -E '(^user|^worker_processes|^worker_priority|^worker_rlimit_nofile|^timer_resolution|^pcre_jit|^worker_connections|^accept_mutex|^multi_accept|^accept_mutex_delay|map_hash|server_names_hash|variables_hash|tcp_|^limit_|sendfile|server_tokens|keepalive_|lingering_|gzip|client_|connection_pool_size|directio|large_client|types_hash|server_names_hash|open_file|open_log|^include|^#include)' /usr/local/nginx/conf/nginx.conf | grep -E -v 'gzip_ratio'

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Configuration:"
echo "------------------------------------------------------------------"
echo
php-config

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Settings /usr/local/etc/php-fpm.conf:"
echo "------------------------------------------------------------------"
echo
grep -E '(^log_level|^pid|^error_log|^user|^group|^listen|^pm|^rlimit|^slowlog|^ping.|^php_admin)' /usr/local/etc/php-fpm.conf

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Extensions Loaded:"
echo "------------------------------------------------------------------"
echo
php -m

echo
}

sar_json() {
    int=${1:-1}
    if [ -f /usr/bin/systemctl ]; then
        sadf -j 1 "$int" -- -qurSbdw | jq -r '.sysstat.hosts[] | .statistics[]'
    else
        echo "sar-json only supported in CentOS 7+ and higher"
    fi
}

debug_menuexit() {
    echo
    echo "------------------------------------------------------------------"
    echo "Debugging centmin.sh menu option 24 exit routine starting"
    echo "Please wait until complete..."
    echo "------------------------------------------------------------------"
    cd /usr/local/src/centminmod
    echo 24 | bash -x centmin.sh 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }' 2>&1 > /root/centminlogs/debug-menuexit.txt && echo "Full debug log saved at /root/centminlogs/debug-menuexit.txt"
    sed -i "s|$(hostname)|hostname|g" /root/centminlogs/debug-menuexit.txt
    echo
    echo "Inspect Yum Check Times In debug-menuexit.txt log"
    echo
    grep -A5 'checking for YUM updates' /root/centminlogs/debug-menuexit.txt
    yumstart_time="$(date -d "$(awk '/+ echo \x27 checking for YUM updates/ {print $1,$2}' /root/centminlogs/debug-menuexit.txt)" +%s)"
    yumend_time="$(date -d "$(awk '/ UPDATE_CHECK=/ {print $1,$2}' /root/centminlogs/debug-menuexit.txt)" +%s)"
    yumcheck_time=$(echo "$yumend_time - $yumstart_time" | bc)
    yumcheck_time=$(printf "%0.2f\n" $yumcheck_time)
    echo
    echo "Yum Check Duration: $yumcheck_time seconds"
    echo
    echo "------------------------------------------------------------------"
    echo "Debugging centmin.sh menu option 24 exit routine completed"
    echo "------------------------------------------------------------------"
}

version_log() {
    if [ -f /etc/centminmod-versionlog ]; then
        echo
        echo -e "1st:\n$(head -n1 /etc/centminmod-versionlog)"; echo ".."; echo -e "last 10:\n$(tail -10 /etc/centminmod-versionlog)"
        echo
    else
        echo
        echo "error: /etc/centminmod-versionlog is missing"
    fi
}

check_version() {
    latest_incre=$(curl -${ipv_forceopt}sL https://github.com/centminmod/centminmod/raw/${branchname}/centmin.sh | awk -F "=" '/SCRIPT_INCREMENTVER=/ {print $2}' | sed -e "s|'||g")
    cur_incre=$(awk -F '.b' '{print $3}' /etc/centminmod-release)
    latest="${branchname}.b${latest_incre}"
    current="${branchname}.b${cur_incre}"
    if [[ "$cur_incre" -eq "$latest_incre" ]]; then
        echo "centminmod latest version $current detected"
        exit 0
    else
        echo "your centminmod version $current is older than latest $latest"
        exit 1
    fi
}

service_info_json() {
  info_service_name=$1
  if [[ -n "$info_service_name" ]]; then
    if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
      systemctl show --no-pager $info_service_name | jq --slurp --raw-input 'split("\n") | map(select(. != "") | split("=") | {"key": .[0], "value": (.[1:] | join("="))}) | from_entries' | jq --arg sli_opt $sli_opt -r '{Names: .Names, Description: .Description, Type: .Type, ActiveState: .ActiveState, LoadState: .LoadState, SubState: .SubState, Result: .Result, ExecMainStartTimestamp: .ExecMainStartTimestamp, MainPID: .MainPID, DropInPaths: .DropInPaths, ExecStart: .ExecStart, ExecStartPre: .ExecStartPre, ExecReload: .ExecReload, ExecStop: .ExecStop, PIDFile: .PIDFile, LimitMEMLOCK: .LimitMEMLOCK, LimitNOFILE: .LimitNOFILE, LimitNPROC: .LimitNPROC, After: .After, Before: .Before, Conflicts: .Conflicts, FailureAction: .FailureAction, FragmentPath: .FragmentPath, NotifyAccess: .NotifyAccess, PrivateNetwork: .PrivateNetwork, PrivateTmp: .PrivateTmp, ProtectHome: .ProtectHome, ProtectSystem: .ProtectSystem, Requires: .Requires, Restart: .Restart, RestartUSec: .RestartUSec, StartLimitAction: .StartLimitAction, StartLimitBurst: .StartLimitBurst, StartLimitInterval: .StartLimitInterval, TimeoutStartUSec: .TimeoutStartUSec, TimeoutStopUSec: .TimeoutStopUSec, UnitFilePreset: .UnitFilePreset, UnitFileState: .UnitFileState, WantedBy: .WantedBy, Wants: .Wants}'
    else
      systemctl show --no-pager $info_service_name | jq --slurp --raw-input 'split("\n") | map(select(. != "") | split("=") | {"key": .[0], "value": (.[1:] | join("="))}) | from_entries' | jq --arg sli_opt $sli_opt -r '{Names: .Names, Description: .Description, Type: .Type, ActiveState: .ActiveState, LoadState: .LoadState, SubState: .SubState, Result: .Result, ExecMainStartTimestamp: .ExecMainStartTimestamp, MainPID: .MainPID, DropInPaths: .DropInPaths, ExecStart: .ExecStart, ExecStartPre: .ExecStartPre, ExecReload: .ExecReload, ExecStop: .ExecStop, PIDFile: .PIDFile, LimitMEMLOCK: .LimitMEMLOCK, LimitNOFILE: .LimitNOFILE, LimitNPROC: .LimitNPROC, After: .After, Before: .Before, Conflicts: .Conflicts, FailureAction: .FailureAction, FragmentPath: .FragmentPath, NotifyAccess: .NotifyAccess, PrivateNetwork: .PrivateNetwork, PrivateTmp: .PrivateTmp, ProtectHome: .ProtectHome, ProtectSystem: .ProtectSystem, Requires: .Requires, Restart: .Restart, RestartUSec: .RestartUSec, StartLimitAction: .StartLimitAction, StartLimitBurst: .StartLimitBurst, StartLimitIntervalUSec: .StartLimitIntervalUSec, TimeoutStartUSec: .TimeoutStartUSec, TimeoutStopUSec: .TimeoutStopUSec, UnitFilePreset: .UnitFilePreset, UnitFileState: .UnitFileState, WantedBy: .WantedBy, Wants: .Wants}'
    fi
  else
    echo "incorrect syntax"
    echo "missing servicename"
    echo "$0 service-info servicename"
  fi
}

inspect_php_patch_log() {
  latest_log=$(find $CENTMINLOGDIR -type f -name "patch_php_*.log" -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2)

  if [[ -n "$latest_log" ]]; then
      ls -lAh "$latest_log"
      echo
      cat "$latest_log"
  else
      echo "No patch_php log file found."
  fi
}

inspect_nginx_patch_log() {
  latest_log=$(find $CENTMINLOGDIR -type f -name "patch_patchnginx_*.log" -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2)

  if [[ -n "$latest_log" ]]; then
      ls -lAh "$latest_log"
      echo
      cat "$latest_log"
  else
      echo "No patch_patchnginx log file found."
  fi
}

ssl_dates() {
  if [ -f /usr/local/src/centminmod/addons/acmetool.sh ]; then
    /usr/local/src/centminmod/addons/acmetool.sh checkdates
  fi
}
#########
if [[ -z "$1" ]]; then
    infooutput
fi

case "$1" in
    info)
    infooutput
        ;;
    update)
    setupdate
        ;;
    ssldates)
    {
    ssl_dates
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-ssldates-${DT}.log"
        ;;
    netstat)
    netstat_info "$2"
        ;;
    syn)
    syn_info
        ;;
    top)
    {
    top_info nocron "$2"
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-${DT}.log"
        ;;
    top-cron)
    {
    top_info cron "$2"
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-cron-${DT}.log"
        ;;
    sar-json)
    {
    sar_json "$2"
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-sar-json-${DT}.log"
        ;;
    sar-cpu-interval)
    {
    sar_cpu_pc_interval
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-sar-cpu-interval-${DT}.log"
        ;;
    sar-cpu)
    {
    sar_cpu_pc
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-sar-cpu-${DT}.log"
        ;;
    sar-mem)
    {
    sar_mem_pc
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-sar-mem-${DT}.log"
        ;;
    phpmem)
    {
    phpfpm_mem_stats
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-php-memory-${DT}.log"
        ;;
    phpstats)
    {
    phpfpm_mem_stats
    echo
    if [ -f /usr/bin/fpmstats ]; then
        fpmstats
    fi
    echo
    pidstat_php nocron "$2"
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-php-stats-${DT}.log"
        ;;
    phpstats-cron)
    {
    phpfpm_mem_stats cron
    echo
    if [ -f /usr/bin/fpmstats ]; then
        fpmstats
    fi
    echo
    pidstat_php cron "$2"
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-php-stats-cron-${DT}.log"
        ;;
    listlogs)
    list_logs
        ;;
    debug-menuexit)
    debug_menuexit
        ;;
    versions)
    version_log
        ;;
    checkver)
    check_version
    ;;
    nginx-patch-log)
    inspect_nginx_patch_log
    ;;
    php-patch-log)
    inspect_php_patch_log
    ;;
    service-info)
      service_info_json "$2"
    ;;
    *)
    echo "$0 {info|update|ssldates|netstat|syn|top|top-cron|sar-json|sar-cpu-interval|sar-cpu|sar-mem|phpmem|phpstats|phpstats-cron|listlogs|debug-menuexit|versions|checkver|service-info|nginx-patch-log|php-patch-log}"
        ;;
esac