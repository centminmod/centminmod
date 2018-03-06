# logrotate
# nano -w /etc/logrotate.d/mysql
# logrotate -df /etc/logrotate.d/mysql

function funct_listlogmysqlrotatedir {

echo "---------------------------------------------------------------------------"
echo "Current mysql log directories:"
echo "---------------------------------------------------------------------------"
if [ -d /var/log/ ]; then
echo "ls -lh /var/log/ | egrep 'mysql|mysql-slowlog'"
ls -lh /var/log/ | egrep 'mysql|mysql-slowlog'
echo "---------------------------------------------------------------------------"
fi
echo "---------------------------------------------------------------------------"
echo ""

}

function funct_logmysqlrotate {

  QUIET=$1

if [[ ! -f /etc/logrotate.d/mysql || ! -f /etc/logrotate.d/mysql-slowlog || ! "$(grep 'dateext' /etc/logrotate.d/mysql)" ]]; then

if [[ "$QUIET" != 'silent' ]]; then
echo "----------------------------------------------"
echo "Creating /etc/logrotate.d/mysql file"
echo "----------------------------------------------"
echo "......"
fi

touch /etc/logrotate.d/mysql

if [ ! -f /var/log/mysqld.log ]; then
  touch /var/log/mysqld.log
  chown mysql:mysql /var/log/mysqld.log
  chmod 600 /var/log/mysqld.log
  #ls -lah /var/log/mysqld.log
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
        VARDFSIZE=$(df --output=avail /var | tail -1)
else
        VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
fi

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/mysql" <<END
/var/log/mysqld.log {
  create 600 mysql mysql
  maxsize 500M
  dateext
  notifempty
  daily
  rotate 8
  missingok
  compress
  sharedscripts
  postrotate
  if test -x /usr/bin/mysqladmin && /usr/bin/mysqladmin ping &>/dev/null       
  then
    env HOME=/root/ /usr/bin/mysql -e 'select @@global.long_query_time into @lqt_save; set global long_query_time=2000; select sleep(2); FLUSH LOGS; select sleep(2); set global long_query_time=@lqt_save;'
  fi
  endscript
}
END
else
cat > "/etc/logrotate.d/mysql" <<END
/var/log/mysqld.log {
  create 600 mysql mysql
  dateext
  notifempty
  daily
  rotate 8
  missingok
  compress
  sharedscripts
  postrotate
  if test -x /usr/bin/mysqladmin && /usr/bin/mysqladmin ping &>/dev/null       
  then
    env HOME=/root/ /usr/bin/mysql -e 'select @@global.long_query_time into @lqt_save; set global long_query_time=2000; select sleep(2); FLUSH LOGS; select sleep(2); set global long_query_time=@lqt_save;'
  fi
  endscript
}
END
fi

if [[ "$QUIET" != 'silent' ]]; then
echo "----------------------------------------------"
echo "Creating /etc/logrotate.d/mysql-slowlog file"
echo "----------------------------------------------"
echo "......"
fi

touch /etc/logrotate.d/mysql-slowlog

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
cat > "/etc/logrotate.d/mysql-slowlog" <<END
/var/lib/mysql/slowq.log {
  create 600 mysql mysql
  maxsize 250M
  dateext
  notifempty
  daily
  rotate 8
  missingok
  compress
  sharedscripts
  postrotate
  # just if mysqld is really running
  if test -x /usr/bin/mysqladmin &&  
    /usr/bin/mysqladmin ping &>/dev/null       
  then
    env HOME=/root/ /usr/bin/mysql -e 'select @@global.long_query_time into @lqt_save; set global long_query_time=2000; set global slow_query_log = 0; select sleep(2); FLUSH LOGS; select sleep(2); set global long_query_time=@lqt_save; set global slow_query_log = 1;' > /var/log/mysqladmin.flush-logs 2>&1
  fi   
  endscript
}
END
else
cat > "/etc/logrotate.d/mysql-slowlog" <<END
/var/lib/mysql/slowq.log {
  create 600 mysql mysql
  dateext
  notifempty
  daily
  rotate 8
  missingok
  compress
  sharedscripts
  postrotate
  # just if mysqld is really running
  if test -x /usr/bin/mysqladmin &&  
    /usr/bin/mysqladmin ping &>/dev/null       
  then
    env HOME=/root/ /usr/bin/mysql -e 'select @@global.long_query_time into @lqt_save; set global long_query_time=2000; set global slow_query_log = 0; select sleep(2); FLUSH LOGS; select sleep(2); set global long_query_time=@lqt_save; set global slow_query_log = 1;' > /var/log/mysqladmin.flush-logs 2>&1
  fi   
  endscript
}
END
fi

  if [[ "$QUIET" != 'silent' ]]; then
    echo "----------------------------------------------------------------------------"
    cat /etc/logrotate.d/mysql
    echo "----------------------------------------------------------------------------"
    cat /etc/logrotate.d/mysql-slowlog
    echo "----------------------------------------------------------------------------"
    
    funct_listlogmysqlrotatedir
    
    echo "----------------------------------------------------------------------------"
    echo "mysql logrotation /etc/logrotate.d/mysql & /etc/logrotate.d/mysql-slowlog setup completed"
    echo "----------------------------------------------------------------------------"
  fi

else 

  if [[ "$QUIET" != 'silent' ]]; then
    echo "--------------------------------------------------"
    echo " /etc/logrotate.d/mysql & /etc/logrotate.d/mysql-slowlog already exists"
    echo "--------------------------------------------------"
    echo ""
    
    funct_listlogmysqlrotatedir
  fi
fi

}