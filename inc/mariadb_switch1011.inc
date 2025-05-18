tmpsetup_native_switchback() {
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_TEN" -eq '10' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    if [ ! -d /etc/systemd/system/mariadb.service.d/ ]; then
      mkdir -p /etc/systemd/system/mariadb.service.d/
    fi
  fi
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_TEN" -eq '10' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/protecthome.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Update MariaDB 10 setting for ProtectHome=false"
cat > "/etc/systemd/system/mariadb.service.d/protecthome.conf" <<TDF
[Service]
ProtectSystem=false
ProtectHome=false
TDF
    systemctl daemon-reload
  fi
}

mysqltmpdir_native_switchback() {
  if [ -d "/home/mysqltmp" ]; then
    if [[ $(stat -c %U /home/mysqltmp/) != 'mysql' && -f /usr/bin/mysql ]]; then 
      chown mysql:mysql /home/mysqltmp/
    fi
  fi
  tmpsetup_native_switchback
}

remove_mariadb_ten_eleven_native_module() {
  OLDMARIADBVERS=$(rpm -qa mariadb-*)
  
  echo "#####################################################"
  
  # List current existing MariaDB versions for RPMs
  echo "Current Appstream Module MariaDB 10.11 versions installed:"
  for mrpm in $OLDMARIADBVERS
  do
  echo $mrpm
  done
  
  echo "#####################################################"
  
  echo "Proceeding to remove existing MariaDB 10.11 versions:"
  echo "please wait while processing...."
 
  OLD_MARIADB_CONNECTOR_C_CONFIG=$(rpm -qa | grep mariadb | awk '/mariadb-connector-c-config/')
  OLD_MARIADB_COMMON=$(rpm -qa | grep mariadb | awk '/mariadb-common/')
  OLD_MARIADB_GSSAPI_SERVER=$(rpm -qa | grep mariadb | awk '/mariadb-gssapi-server/')
  OLD_MARIADB_ERRMSG=$(rpm -qa | grep mariadb | awk '/mariadb-errmsg/')
  OLD_MARIADB_SERVER_UTILS=$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')
  OLD_MARIADB_SERVER=$(rpm -qa | grep mariadb | awk '/mariadb-server/')
  OLD_MARIADB_CONNECTOR_C=$(rpm -qa | grep mariadb | awk '/mariadb-connector-c/')
  OLD_MARIADB_BACKUP=$(rpm -qa | grep mariadb | awk '/mariadb-backup/')
  OLDGALERA=$(rpm -qa | grep galera | awk '/galera/')

  # echo "${ALIAS_MYSQL} -e \"SET GLOBAL innodb_fast_shutdown = 1;\""
  # ${ALIAS_MYSQL} -e "SET GLOBAL innodb_fast_shutdown = 1;"
  # sleep 10
  echo
  echo "shutdown MariaDB 10.11 server"
  sleep 5
  echo "systemctl stop mariadb"
  systemctl stop mariadb
  
  yum -q -y remove perl-DBD-MySQL
  
  echo "rpm -e --nodeps $OLD_MARIADB_CONNECTOR_C_CONFIG"
  echo "rpm -e --nodeps $OLD_MARIADB_COMMON"
  echo "rpm -e --nodeps $OLD_MARIADB_GSSAPI_SERVER"
  echo "rpm -e --nodeps $OLD_MARIADB_ERRMSG"
  echo "rpm -e --nodeps $OLD_MARIADB_SERVER_UTILS"
  echo "rpm -e --nodeps $OLD_MARIADB_SERVER"
  echo "rpm -e --nodeps $OLD_MARIADB_CONNECTOR_C"
  echo "rpm -e --nodeps $OLD_MARIADB_BACKUP"
  echo "rpm -e --nodeps $OLDGALERA"
  
  rpm -e --nodeps $OLD_MARIADB_CONNECTOR_C_CONFIG
  rpm -e --nodeps $OLD_MARIADB_COMMON
  rpm -e --nodeps $OLD_MARIADB_GSSAPI_SERVER
  rpm -e --nodeps $OLD_MARIADB_ERRMSG
  rpm -e --nodeps $OLD_MARIADB_SERVER_UTILS
  rpm -e --nodeps $OLD_MARIADB_SERVER
  rpm -e --nodeps $OLD_MARIADB_CONNECTOR_C
  rpm -e --nodeps $OLD_MARIADB_BACKUP
  rpm -e --nodeps $OLDGALERA
  
  yum -y module disable mariadb
  if [ -f /etc/systemd/system/mariadb.service.d/mysql-prepare-db-dir-modify.conf ]; then
    rm -f /etc/systemd/system/mariadb.service.d/mysql-prepare-db-dir-modify.conf
  fi
  if [ -f /usr/libexec/mysql-prepare-db-dir-modify ]; then
    rm -f /usr/libexec/mysql-prepare-db-dir-modify
  fi
  yum clean all
  
  if [ -z "$OLDMARIADBVERS" ]; then
    echo "Appstream Module MariaDB 10.11 versions successfully removed..."
  fi
}

remove_mariadb_ten_eleven() {
  OLDMARIADBVERS=$(rpm -qa | grep MariaDB)
  
  echo "#####################################################"
  
  # List current existing MariaDB versions for RPMs
  echo "Current Official MariaDB 10.11 versions installed:"
  for mrpm in $OLDMARIADBVERS
  do
  echo $mrpm
  done
  
  echo "#####################################################"
  
  echo "Proceeding to remove existing MariaDB 10.11 versions:"
  echo "please wait while processing...."
  
  OLDMARIADBSHARED=$(rpm -qa | grep MariaDB | awk '/MariaDB-shared/')
  OLDMARIADBCLIENT=$(rpm -qa | grep MariaDB | awk '/MariaDB-client/')
  OLDMARIADBDEVEL=$(rpm -qa | grep MariaDB | awk '/MariaDB-devel/')
  OLDMARIADBSERVER=$(rpm -qa | grep MariaDB | awk '/MariaDB-server/')
  OLDMARIADBTEST=$(rpm -qa | grep MariaDB | awk '/MariaDB-test/')
  OLDMARIADBCOMMON=$(rpm -qa | grep MariaDB | awk '/MariaDB-common/')
  OLDMARIADBCOMPAT=$(rpm -qa | grep MariaDB | awk '/MariaDB-compat/')
  OLDMARIADBBACKUP=$(rpm -qa | grep MariaDB | awk '/MariaDB-backup/')
  OLDGALERA=$(rpm -qa | grep galera | awk '/galera/')

  echo "${ALIAS_MYSQL} -e \"SET GLOBAL innodb_fast_shutdown = 1;\""
  ${ALIAS_MYSQL} -e "SET GLOBAL innodb_fast_shutdown = 1;"
  sleep 10
  echo
  echo "shutdown MariaDB 10.11 server"
  sleep 5
  echo "systemctl stop mariadb"
  systemctl stop mariadb
  
  yum -q -y remove perl-DBD-MySQL
  
  echo "rpm -e $OLDMARIADBTEST"
  echo "rpm -e $OLDMARIADBSERVER"
  echo "rpm -e $OLDMARIADBCLIENT"
  echo "rpm -e $OLDMARIADBDEVEL"
  echo "rpm -e $OLDGALERA"
  echo "rpm -e --nodeps $OLDMARIADBSHARED"
  echo "rpm -e --nodeps $OLDMARIADBCOMPAT"
  echo "rpm -e --nodeps $OLDMARIADBCOMMON"
  echo "rpm -e --nodeps $OLDMARIADBBACKUP"
  
  rpm -e $OLDMARIADBTEST
  rpm -e $OLDMARIADBSERVER
  rpm -e $OLDMARIADBCLIENT
  rpm -e $OLDMARIADBDEVEL
  rpm -e $OLDGALERA
  rpm -e --nodeps $OLDMARIADBSHARED
  rpm -e --nodeps $OLDMARIADBCOMPAT
  rpm -e --nodeps $OLDMARIADBCOMMON
  rpm -e --nodeps $OLDMARIADBBACKUP
  
  yum-config-manager --disable mariadb
  yum clean all
  
  if [ -z "$OLDMARIADBVERS" ]; then
    echo "Official MariaDB 10.11 versions successfully removed..."
  fi
}

install_native_mariadb_ten_eleven() {
  install_native_mariadb_ten_eleven_mode=$1
  mariabackup_destination_dir=$2
  CMM_BACKUP_MYCNF='/etc/my.cnf-b4-mariadb10.11-swtch'

  #MDB_COUNTRY=$(curl -${ipv_forceopt}s https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | awk '/country: / {print $2}')
  MDB_TOTALMEM=$(($(awk '/MemTotal/ {print $2}' /proc/meminfo)/1024))
  MDB_VIRTWHAT_GET=$(virt-what)
  MDB_VIRTWHAT="${MDB_VIRTWHAT_GET:-d}"
  MDB_DTIME=$(tail -1 /root/centminlogs/centminmod_downloadtimes_*.log)
  MDB_DTIME_SEC=$(echo "$MDB_DTIME" |awk '{printf("%.2f\n",$7)}')
  MDB_NTIME=$(tail -1 /root/centminlogs/centminmod_ngxinstalltime_*.log)
  MDB_NTIME_SEC=$(echo "$MDB_NTIME" |awk '{printf("%.2f\n",$7)}')
  if [[ "$VPS_GEOIPCHECK_V3" = [yY] ]]; then
      MDB_GEOIP=$(curl -${ipv_forceopt}s -A "$CURL_AGENT $SCRIPT_VERSION $CURL_CPUMODEL $CURL_CPUSPEED 10.11 install M:$MDB_TOTALMEM D:$MDB_DTIME_SEC N:$MDB_NTIME_SEC $MDB_VIRTWHAT $MDB_CITY $MDB_COLO Appstream" https://geoip.centminmod.com/v3 | jq -r)
  elif [[ "$VPS_GEOIPCHECK_V4" = [yY] ]]; then
      MDB_GEOIP=$(curl -${ipv_forceopt}s -A "$CURL_AGENT $SCRIPT_VERSION $CURL_CPUMODEL $CURL_CPUSPEED 10.11 install M:$MDB_TOTALMEM D:$MDB_DTIME_SEC N:$MDB_NTIME_SEC $MDB_VIRTWHAT $MDB_CITY $MDB_COLO Appstream" https://geoip.centminmod.com/v4 | jq -r)
  fi
  MDB_COUNTRY=$(echo "$MDB_GEOIP" | jq -r '.country')
  MDB_CONTINENT=$(echo "$MDB_GEOIP" | jq -r '.continent')
  MDB_CITY=$(echo "$MDB_GEOIP" | jq -r '.city')
  MDB_COLO=$(echo "$MDB_GEOIP" | jq -r '.colo')

  if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    yumpriorities_opt=""
    # yumdisable_appstream=' --disablerepo=AppStream'
    yumdisable_appstream=""
  else
    yumpriorities_opt=' --disableplugin=priorities'
    yumdisable_appstream=""
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      DISABLEOTHERREPO=" --disablerepo=epel --disablerepo=rpmforge${yumpriorities_opt}"
  else
      DISABLEOTHERREPO=" --disablerepo=epel${yumpriorities_opt}"
  fi

  if [[ "$install_native_mariadb_ten_eleven_mode" = 'switch' ]]; then
    echo
    echo "#####################################################"
    echo "Switching to native EL10 MariaDB 10.11 YUM Repo setup"
    echo "#####################################################"
    echo
  elif [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Native EL10 Appstream Module MariaDB 10.11 Yum Install"
    fi
    echo
    echo "#####################################################"
    cecho "* Installing native EL10 Appstream MariaDB 10.11 Module Packages " $boldgreen
    echo "#####################################################"
    echo
  fi
  if [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
    systemctl stop mariadb
    
    ${YUMDNFBIN}${CACHESKIP} -y remove mysql mysql-libs
    
    if [ ! -d /home/mysqltmp ]; then
      mkdir /home/mysqltmp
      chmod 1777 /home/mysqltmp
      CHOWNMYSQL=y
    fi
    
    if [ -f /etc/my.cnf ]; then
      cp /etc/my.cnf /etc/my.cnf-original
      rm -f /etc/my.cnf
    fi
    # set /etc/my.cnf templates
    setmycnf
  
    sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
    sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
    sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
    sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
    sed -i 's|ignore-db-dir|ignore_db_dirs|g' /etc/my.cnf
    sed -i 's|^innodb_thread_concurrency|#innodb_thread_concurrency|g' /etc/my.cnf
    sed -i 's|^skip-federated|#skip-federated|g' /etc/my.cnf
    sed -i 's|^skip-pbxt|#skip-pbxt|g' /etc/my.cnf
    sed -i 's|^skip-pbxt_statistics|#skip-pbxt_statistics|g' /etc/my.cnf
    sed -i 's|^skip-archive|#skip-archive|g' /etc/my.cnf
    sed -i 's|^innodb_buffer_pool_dump_at_shutdown|#innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
    sed -i 's|^innodb_buffer_pool_load_at_startup|#innodb_buffer_pool_load_at_startup|g' /etc/my.cnf
    # deprecated in 10.11
    # https://mariadb.com/kb/en/library/mariadb-1031-release-notes/
    sed -i '/^innodb_large_prefix/d' /etc/my.cnf
    sed -i '/^innodb_file_format/d' /etc/my.cnf
    sed -i '/^innodb_support_xa/d' /etc/my.cnf
    echo
    echo "yum -q module list mariadb"
    yum -q module list mariadb
    echo
    echo "yum -y module enable mariadb"
    yum -y module enable mariadb
    echo
    echo "yum -q module list mariadb"
    yum -q module list mariadb
    echo
    echo "yum -y install mariadb-server mariadb"
    yum -y install mariadb-server mariadb
    echo
  
    if [[ "$CENTOS_TEN" -eq '10' ]] && [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
      mysqltmpdir_native_switchback
  
      echo "setup /var/log/mysqld.log"
      touch /var/log/mysqld.log
      chown mysql:mysql /var/log/mysqld.log
      chmod 600 /var/log/mysqld.log
      ls -lah /var/log/mysqld.log
      echo
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "/etc/my.cnf compat checks"
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.11 Specific /etc/my.cnf"
    fi

cat >>/etc/my.cnf<<FFF


[mariadb-10.11]
log_slow_query=0
log_slow_query_time=1
log_slow_query_file=/var/lib/mysql/slowq.log

#innodb_file_format = Barracuda
innodb_default_row_format = dynamic
innodb_file_per_table = 1

# performance regressions
# https://jira.mariadb.org/browse/MDEV-16337
# https://jira.mariadb.org/browse/MDEV-20697
#join_cache_level=0
optimizer_use_condition_selectivity=1

## wsrep specific
# wsrep_on=OFF
# wsrep_provider
# wsrep_cluster_address
# binlog_format=ROW
# default_storage_engine=InnoDB
# innodb_autoinc_lock_mode=2
# innodb_doublewrite=1
# query_cache_size=0

# 2 variables needed to switch from XtraDB to InnoDB plugins
#plugin-load=ha_innodb
#ignore_builtin_innodb

## MariaDB 10 only save and restore buffer pool pages
## warm up InnoDB buffer pool on server restarts
innodb_buffer_pool_dump_at_shutdown=1
innodb_buffer_pool_load_at_startup=1
## Disabled settings
performance_schema=OFF
innodb_stats_on_metadata=OFF
innodb_sort_buffer_size=2M
innodb_online_alter_log_max_size=128M
query_cache_strip_comments=0
log_slow_filter =admin,filesort,filesort_on_disk,full_join,full_scan,query_cache,query_cache_miss,tmp_table,tmp_table_on_disk

# Defragmenting unused space on InnoDB tablespace
innodb_defragment=1
innodb_defragment_n_pages=7
innodb_defragment_stats_accuracy=0
innodb_defragment_fill_factor_n_recs=20
innodb_defragment_fill_factor=0.9
innodb_defragment_frequency=40

# page compression
# 10.1+ https://mariadb.com/kb/en/mariadb/compression/
#innodb_compression_algorithm=zlib
#innodb_use_fallocate=0
#innodb_use_mtflush=0
#innodb_mtflush_threads=4
FFF

    # changed variables for 10.11.1
    # https://jira.mariadb.org/browse/MDEV-7567
    sed -i '/^min_examined_row_limit/d' /etc/my.cnf
    sed -i '/^slow_query_log/d' /etc/my.cnf
    sed -i '/^slow_query_log_file/d' /etc/my.cnf
    sed -i '/^long_query_time/d' /etc/my.cnf

  fi # install-fresh
  echo
  echo "yum -q -y install mytop"
  yum -q -y install mytop
  if [[ "$install_native_mariadb_ten_eleven_mode" = 'switch' ]]; then
    \cp -af $CMM_BACKUP_MYCNF /etc/my.cnf
  fi

if [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
  tmpsetup_native_switchback

  if [[ "$CENTOS_TEN" -eq '10' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/openfileslimit.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for LimitNOFILE=524288"
cat > "/etc/systemd/system/mariadb.service.d/openfileslimit.conf" <<TDG
[Service]
LimitNOFILE=524288
TDG
  systemctl daemon-reload
  fi

  if [[ "$CENTOS_TEN" -eq '10' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/notifyaccess.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for NotifyAccess=all"
cat > "/etc/systemd/system/mariadb.service.d/notifyaccess.conf" <<TDG
[Service]
NotifyAccess=all
TDG
  systemctl daemon-reload
  fi

  # modify /usr/libexec/mysql-prepare-db-dir
  if [[ "$CENTOS_TEN" -eq '10' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/mysql-prepare-db-dir-modify.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    # used modified /usr/libexec/mysql-prepare-db-dir-modify instead
    \cp -af /usr/libexec/mysql-prepare-db-dir /usr/libexec/mysql-prepare-db-dir-modify
    # modify /usr/libexec/mysql-prepare-db-dir-modify messaging for when /var/lib/mysql data directory
    # exists so as to not confuse new users into mistakenly deleting their /var/lib/mysql directory
    sed -i 's|echo "If this is not the case, make sure the $datadir is empty before running `basename $0`."||' /usr/libexec/mysql-prepare-db-dir-modify
    echo "Initial Install: Update MariaDB 10 setting for /usr/libexec/mysql-prepare-db-dir-modify"
cat > "/etc/systemd/system/mariadb.service.d/mysql-prepare-db-dir-modify.conf" <<TDG
[Service]
ExecStartPre=
ExecStartPre=/usr/libexec/mysql-prepare-db-dir-modify %n
TDG
  systemctl daemon-reload
  fi

  systemctl restart mariadb
fi # install-fresh

  if [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10 Plugins"
    fi

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'metadata_lock_info';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'metadata_lock_info';"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'query_cache_info';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'query_cache_info';"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'query_response_time';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'ha_xtradb';\""
    # ${ALIAS_MYSQL} -e "INSTALL SONAME 'ha_xtradb';"
    echo "${ALIAS_MYSQL} -t -e \"SELECT * FROM mysql.plugin;\""
    ${ALIAS_MYSQL} -t -e "SELECT * FROM mysql.plugin;"
    echo "${ALIAS_MYSQL} -t -e \"SHOW PLUGINS;\""
    ${ALIAS_MYSQL} -t -e "SHOW PLUGINS;"
    echo "${ALIAS_MYSQL} -t -e \"SHOW ENGINES;\""
    ${ALIAS_MYSQL} -t -e "SHOW ENGINES;"

    if [ "$UNATTENDED" == 'nn' ]; then
echo ""
echo "------------------------------------------------"
echo "Updating MySQL database tables"
echo "------------------------------------------------"
read -ep "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
echo ""

if [[ "$mysqlrootset" == [yY] ]]; then
    echo "Enter MySQL root password when prompted"
    echo "${ALIAS_MYSQL_UPGRADE} -u root -p"
    ${ALIAS_MYSQL_UPGRADE} -u root -p

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "${ALIAS_MYSQL} -u root -p -e \"INSTALL SONAME 'metadata_lock_info';\""
    ${ALIAS_MYSQL} -u root -p -e "INSTALL SONAME 'metadata_lock_info';"
    echo "${ALIAS_MYSQL} -u root -p -e \"INSTALL SONAME 'query_cache_info';\""
    ${ALIAS_MYSQL} -u root -p -e "INSTALL SONAME 'query_cache_info';"
    echo "${ALIAS_MYSQL} -u root -p -e \"INSTALL SONAME 'query_response_time';\""
    ${ALIAS_MYSQL} -u root -p -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "${ALIAS_MYSQL} -u root -p -e \"INSTALL SONAME 'ha_xtradb';\""
    # ${ALIAS_MYSQL} -u root -p -e "INSTALL SONAME 'ha_xtradb';"

    echo "${ALIAS_MYSQL} -u root -p -e \"SELECT * FROM mysql.plugin;\""
    ${ALIAS_MYSQL} -u root -p -e "SELECT * FROM mysql.plugin;"
    echo "${ALIAS_MYSQL} -u root -p -e \"SHOW PLUGINS;\""
    ${ALIAS_MYSQL} -u root -p -e "SHOW PLUGINS;"
    echo "${ALIAS_MYSQL} -u root -p -e \"SHOW ENGINES;\""
    ${ALIAS_MYSQL} -u root -p -e "SHOW ENGINES;"
else
    echo "${ALIAS_MYSQL_UPGRADE}"
    ${ALIAS_MYSQL_UPGRADE}

    echo "------------------------------------------------"
    echo "Installing MariaDB 10 plugins"
    echo "------------------------------------------------"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'metadata_lock_info';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'metadata_lock_info';"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'query_cache_info';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'query_cache_info';"
    echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'query_response_time';\""
    ${ALIAS_MYSQL} -e "INSTALL SONAME 'query_response_time';"
    # echo "------------------------------------------------"
    # echo "Installing MariaDB 10 XtraDB Engine plugin"
    # echo "------------------------------------------------"
    # echo "${ALIAS_MYSQL} -e \"INSTALL SONAME 'ha_xtradb';\""
    # ${ALIAS_MYSQL} -e "INSTALL SONAME 'ha_xtradb';"
    echo "${ALIAS_MYSQL} -t -e \"SELECT * FROM mysql.plugin;\""
    ${ALIAS_MYSQL} -t -e "SELECT * FROM mysql.plugin;"
    echo "${ALIAS_MYSQL} -t -e \"SHOW PLUGINS;\""
    ${ALIAS_MYSQL} -t -e "SHOW PLUGINS;"
    echo "${ALIAS_MYSQL} -t -e \"SHOW ENGINES;\""
    ${ALIAS_MYSQL} -t -e "SHOW ENGINES;"

fi
    fi #unattended
echo ""

perdbiinstall
# reinstall postfix removed from mariadb-libs removale
if [[ $(rpm -q postfix >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install postfix${DISABLEOTHERREPO} 
fi
if [[ $(rpm -q net-snmp >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install net-snmp${DISABLEOTHERREPO} 
fi

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
# make sure MariaDB 5.5 is updated from only one repo
echo ""
if [[ -z "$(grep 'exclude=mariadb' /etc/yum.repos.d/centalt.repo)" ]]; then
echo "exclude=mariadb*" >> /etc/yum.repos.d/centalt.repo
fi
fi # CENTALTREPO_DISABLE

# # exclude MariaDB Galera Server
# if [[ -z "$(grep 'exclude=MariaDB-Galera-server' /etc/yum.repos.d/mariadb.repo)" ]]; then
# echo "exclude=MariaDB-Galera-server" >> /etc/yum.repos.d/mariadb.repo
# fi

set_mariadb_mysqldump_charset

# setup mariadb mysql log rotation
funct_logmysqlrotate
  fi # install-fresh

  if [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "New Initial /etc/my.cnf"
    fi
    echo "new initial /etc/my.cnf"
    echo "================================================="
    cat /etc/my.cnf
    echo "================================================="
    echo
    if [[ ! `grep exclude /etc/yum.conf` ]]; then
        cecho "Can't find exclude line in /etc/yum.conf... adding exclude line for mysql*" $boldgreen
        echo "exclude=mysql*">> /etc/yum.conf
    else
        cecho "exclude line exists... adding exclude line for mysql*" $boldgreen
        sed -i "s/exclude=\*.i386 \*.i586 \*.i686/exclude=\*.i386 \*.i586 \*.i686 mysql\*/" /etc/yum.conf
    fi
    echo "*************************************************"
    cecho "* Starting MariaDB Secure Installation" $boldgreen
    if [[ "$CENTOS_TEN" -eq '10' ]]; then
        # ensure MariaDB starts up initially
        systemctl stop mariadb
        sleep 2
        rm -f /var/lib/mysql/ib_logfile*
        rm -f /var/lib/mysql/aria_log*
        systemctl start mariadb
    fi
    # https://community.centminmod.com/posts/25691/
    if [ -f /usr/local/src/centminmod/tools/setio.sh ]; then
      /usr/local/src/centminmod/tools/setio.sh set
    fi  
    echo "*************************************************"
    cecho "* MariaDB installed" $boldgreen
    echo "*************************************************"
    echo "After MariaDB installation"
    date
  fi # install-fresh

  if [[ "$install_native_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
    systemctl start mariadb
  else
    systemctl restart mariadb
  fi
  systemctl enable mariadb
  systemctl status mariadb --no-pager
}

mariabackup_backup() {
  mariabackup_destination_dir=$1
  if [ ! -f /usr/bin/mariabackup ]; then
    echo "yum -y -q install MariaDB-backup"
    yum -y -q install MariaDB-backup
  fi
  if [ -f /usr/bin/mariabackup ]; then
    mkdir -p "${mariabackup_destination_dir}"
    MARIABACKUP_THREADS=$(nproc)
    MARIABACKUP_TOTALMEM=$(awk '/MemAvailable:/ {print $2}' /proc/meminfo)
    MARIABACKUP_USEMEM=$(($MARIABACKUP_TOTALMEM/4))
    MARIABACKUP_BACKUP_MYCNF="${mariabackup_destination_dir}/backup-my.cnf"
    CMM_BACKUP_MYCNF='/etc/my.cnf-b4-mariadb10.11-swtch'
    echo
    echo "Backup existing MariaDB 10.11 data using MariaBackup"
    echo "at: $mariabackup_destination_dir"
    cp -a /etc/my.cnf $CMM_BACKUP_MYCNF
    echo
    echo "time mariabackup --backup --parallel=${MARIABACKUP_THREADS} --use-memory=${MARIABACKUP_USEMEM} --target-dir=${mariabackup_destination_dir}"
    time mariabackup --backup --parallel=${MARIABACKUP_THREADS} --use-memory=${MARIABACKUP_USEMEM} --target-dir="${mariabackup_destination_dir}"
    echo "time mariabackup --prepare --use-memory=${MARIABACKUP_USEMEM} --target-dir=${mariabackup_destination_dir}"
    time mariabackup --prepare --use-memory=${MARIABACKUP_USEMEM} --target-dir="${mariabackup_destination_dir}"
    echo "chown -R mysql:mysql ${mariabackup_destination_dir}"
    chown -R mysql:mysql "${mariabackup_destination_dir}"
  fi
}

mariabackup_restore() {
  mariabackup_destination_dir=$1
  if [ ! -f /usr/bin/mariabackup ]; then
    yum -y install mariadb-backup
  fi
  if [ -f /usr/bin/mariabackup ]; then
    echo
    echo "Restore existing MariaDB 10.11 data using MariaBackup"
    echo "from: $mariabackup_destination_dir"
    echo
    MARIABACKUP_THREADS=$(nproc)
    MARIABACKUP_TOTALMEM=$(awk '/MemAvailable:/ {print $2}' /proc/meminfo)
    MARIABACKUP_USEMEM=$(($MARIABACKUP_TOTALMEM/4))
    MARIABACKUP_DATADIR=$(${ALIAS_MYSQLADMIN} var | awk '/datadir/ {print $4}')
    # stop mariadb
    mysqladmin flush-tables
    sleep 10
    systemctl stop mariadb
    sleep 10
    # backup existing datadir
    mkdir -p /home/cmm-mariadb-original-mysql-datadir
    cp -a "$MARIABACKUP_DATADIR" /home/cmm-mariadb-original-mysql-datadir
    # remove existing datadir
    rm -rf "$MARIABACKUP_DATADIR"
    # restore MariaBackup copy to existing datadir
    time mariabackup --copy-back --target-dir="${mariabackup_destination_dir}"
    chown -R mysql:mysql "$MARIABACKUP_DATADIR"
    systemctl start mariadb
    systemctl status mariadb --no-pager
    systemctl enable mariadb
    mysql_upgrade
    echo
    echo "MariaDB 10.11 data restored"
  fi
}

mariabackup_restore_native() {
  mariabackup_destination_dir=$1
  if [ ! -f /usr/bin/mariabackup ]; then
    yum -y install mariadb-backup
  fi
  if [ -f /usr/bin/mariabackup ]; then
    echo
    echo "Restore existing MariaDB 10.11 data using MariaBackup"
    echo "from: $mariabackup_destination_dir"
    echo
    MARIABACKUP_THREADS=$(nproc)
    MARIABACKUP_TOTALMEM=$(awk '/MemAvailable:/ {print $2}' /proc/meminfo)
    MARIABACKUP_USEMEM=$(($MARIABACKUP_TOTALMEM/4))
    MARIABACKUP_DATADIR=$(${ALIAS_MYSQLADMIN} var | awk '/datadir/ {print $4}')
    # stop mariadb
    mysqladmin flush-tables
    sleep 10
    systemctl stop mariadb
    sleep 10
    # backup existing datadir
    mkdir -p /home/cmm-mariadb-original-mysql-datadir-native
    cp -a "$MARIABACKUP_DATADIR" /home/cmm-mariadb-original-mysql-datadir-native
    # remove existing datadir
    rm -rf "$MARIABACKUP_DATADIR"
    # restore MariaBackup copy to existing datadir
    time mariabackup --copy-back --target-dir="${mariabackup_destination_dir}"
    chown -R mysql:mysql "$MARIABACKUP_DATADIR"
    systemctl start mariadb
    systemctl status mariadb --no-pager
    systemctl enable mariadb
    mysql_upgrade
    echo
    echo "MariaDB 10.11 data restored"
  fi
}

switch_mariadb_ten_eleven_module() {
  switch_mariadb_ten_eleven_mode=$1
  # routine for EL10 to allow switching fromm MariaDB official YUM repo
  # version of 10.11 which EOL in May 2029 to native EL10 OS supported
  # 10.11 which is supported until 2032
  #
  # step 1 remove existing official MariaDB official YUM repo version
  # step 2 install native EL10 MariaDB 10.11 version
  if [[ "$CENTOS_TEN" -eq '10' && "$switch_mariadb_ten_eleven_mode" = 'switch' ]]; then
      mariabackup_destination_dir=/home/cmm-mariadb-backup
      mariabackup_backup $mariabackup_destination_dir
      remove_mariadb_ten_eleven
      install_native_mariadb_ten_eleven switch $mariabackup_destination_dir
      mariabackup_restore $mariabackup_destination_dir
  elif [[ "$CENTOS_TEN" -eq '10' && "$switch_mariadb_ten_eleven_mode" = 'install-fresh' ]]; then
      install_native_mariadb_ten_eleven install-fresh
  fi
}

install_official_mariadb_ten_eleven() {
  if [[ "$CENTOS_EIGHT" = '8' ]]; then
      # run for CentOS 8.x
      echo "time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI"
      time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI
  
      echo ""
      echo "*************************************************"
      cecho "MariaDB 10.11.x YUM install..." $boldgreen
      cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}" $boldgreen
      echo "*************************************************"
      echo ""
      time time $YUMDNFBIN -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}
  
      cp -a /etc/my.cnf /etc/my.cnf-newold
  elif [[ "$CENTOS_NINE" = '9' ]]; then
      # run for CentOS 9.x
      echo "time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI"
      time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI
  
      echo ""
      echo "*************************************************"
      cecho "MariaDB 10.11.x YUM install..." $boldgreen
      cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}" $boldgreen
      echo "*************************************************"
      echo ""
      time time $YUMDNFBIN -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}
  
      cp -a /etc/my.cnf /etc/my.cnf-newold
  elif [[ "$CENTOS_TEN" = '10' ]]; then
      # run for CentOS 9.x
      echo "time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI"
      time ${YUMDNFBIN}${CACHESKIP} -q -y install perl-DBI
  
      echo ""
      echo "*************************************************"
      cecho "MariaDB 10.11.x YUM install..." $boldgreen
      cecho "yum -q -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}" $boldgreen
      echo "*************************************************"
      echo ""
      time time $YUMDNFBIN -y install MariaDB-client MariaDB-common MariaDB-devel MariaDB-server MariaDB-shared${DISABLEOTHERREPO}${yumdisable_appstream}
  
      cp -a /etc/my.cnf /etc/my.cnf-newold
  fi
  if [[ "$CENTOS_TEN" -eq '10' ]]; then
    mysqltmpdir_native_switchback

    echo "setup /var/log/mysqld.log"
    touch /var/log/mysqld.log
    chown mysql:mysql /var/log/mysqld.log
    chmod 600 /var/log/mysqld.log
    ls -lah /var/log/mysqld.log
    echo
  fi
}

switchback_mariadb_ten_eleven_official() {
  # switch back from EL10 native MariaDB 10.11 to official MariaDB version
  if [[ "$CENTOS_TEN" -eq '10' ]]; then
      mariabackup_destination_dir=/home/cmm-mariadb-backup-native
      mariabackup_backup $mariabackup_destination_dir
      remove_mariadb_ten_eleven_native_module
      install_official_mariadb_ten_eleven $mariabackup_destination_dir
      mariabackup_restore_native $mariabackup_destination_dir
  fi
}