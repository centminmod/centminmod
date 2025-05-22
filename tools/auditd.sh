#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
######################################################
# auditd and mariadb audit plugin install and 
# setup for centminmod
# https://community.centminmod.com/threads/9071/
# https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Security_Guide/chap-system_auditing.html
# https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Security_Guide/chap-system_auditing.html
# https://www.digitalocean.com/community/tutorials/understanding-the-linux-auditing-system-on-centos-7
# https://www.digitalocean.com/community/tutorials/how-to-write-custom-system-audit-rules-on-centos-7
# http://linoxide.com/how-tos/auditd-tool-security-auditing/
# http://xmodulo.com/how-to-monitor-file-access-on-linux.html
# http://www.cyberciti.biz/tips/linux-audit-files-to-see-who-made-changes-to-a-file.html
# http://linuxcommand.org/man_pages/ausearch8.html
# http://linuxcommand.org/man_pages/auditctl8.html
# https://www.redhat.com/archives/linux-audit/index.html
# https://mariadb.com/kb/en/mariadb/about-the-mariadb-audit-plugin/
# https://mariadb.com/kb/en/mariadb/server_audit-system-variables/
# https://mariadb.com/kb/en/mariadb/server_audit-status-variables/
# 
# https://people.redhat.com/sgrubb/audit/
# https://people.redhat.com/sgrubb/audit/ChangeLog
# https://fedorahosted.org/audit/browser/tags
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

AUDITD_ENABLE='n'
AUDIT_MARIADB='n'
AUDITD_TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
######################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

# Function to get MariaDB version
get_mariadb_version() {
    local version=$(mysql -V 2>&1 | awk '{print $5}' | awk -F. '{print $1"."$2}')
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
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
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

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  dos2unix -q "/etc/centminmod/custom_config.inc"
  . "/etc/centminmod/custom_config.inc"
fi

if [[ "$AUDITD_ENABLE" != [yY] ]]; then
    echo
    echo "AUDITD_ENABLE is set to = n"
    exit
fi

if [[ "$AUDITD_TOTALMEM" -le '499000' ]]; then
  AUDITD_BUFFERSIZE='4096'
elif [[ "$AUDITD_TOTALMEM" -ge '499001' && "$AUDITD_TOTALMEM" -le '1030000' ]]; then
  AUDITD_BUFFERSIZE='8192'
elif [[ "$AUDITD_TOTALMEM" -ge '1030001' && "$AUDITD_TOTALMEM" -le '2040000' ]]; then
  AUDITD_BUFFERSIZE='16384'
elif [[ "$AUDITD_TOTALMEM" -ge '2040001' && "$AUDITD_TOTALMEM" -le '3040000' ]]; then
  AUDITD_BUFFERSIZE='32768'
elif [[ "$AUDITD_TOTALMEM" -ge '3040001' && "$AUDITD_TOTALMEM" -le '4000000' ]]; then
  AUDITD_BUFFERSIZE='65536'
elif [[ "$AUDITD_TOTALMEM" -ge '4000001' && "$AUDITD_TOTALMEM" -le '6020000' ]]; then
  AUDITD_BUFFERSIZE='131072'
elif [[ "$AUDITD_TOTALMEM" -ge '6020001' && "$AUDITD_TOTALMEM" -le '8060000' ]]; then
  AUDITD_BUFFERSIZE='262144'
elif [[ "$AUDITD_TOTALMEM" -ge '8060001' && "$AUDITD_TOTALMEM" -le '16000000' ]]; then
  AUDITD_BUFFERSIZE='524288'
elif [[ "$AUDITD_TOTALMEM" -ge '16000001' ]]; then
  AUDITD_BUFFERSIZE='1048576'
fi

######################################################
auditd_customrules() {
    if [[ "$(grep -c 'custom auditd rules' "$AUDITRULE_PERMFILE")" -ne '2' ]]; then
        if [ -d /usr/local/nginx/conf/conf.d ]; then
            VHOSTS=$(ls /usr/local/nginx/conf/conf.d | grep -E 'ssl.conf|.conf' | grep -E -v 'virtual.conf|^ssl.conf|demodomain.com.conf' |  sed -e 's/.ssl.conf//' -e 's/.conf//' | uniq)
        fi

# echo '-b 320' >> "$AUDITRULE_PERMFILE"
sed -i "s|^-b .*|-b $AUDITD_BUFFERSIZE|" "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# continue loading rules when it runs rule syntax errors" >> "$AUDITRULE_PERMFILE"
echo "#-c" >> "$AUDITRULE_PERMFILE"
echo "#-i" >> "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# Generate at most 5000 audit messages per second" >> "$AUDITRULE_PERMFILE"
echo "-r 5000" >> "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# custom auditd rules for centmin mod centos environments - DO NOT DELETE THIS LINE" >> "$AUDITRULE_PERMFILE"
echo "-w /var/log/wtmp -k sessiontmp" >> "$AUDITRULE_PERMFILE"
echo "-w /var/log/btmp -k sessiontmp" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/utmpdump -k sessiontmp" >> "$AUDITRULE_PERMFILE"
echo "# -w /var/log/audit/ -k auditlog" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/audit/ -p wa -k auditconfig" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/libaudit.conf -p wa -k auditconfig" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/audisp/ -p wa -k audispconfig" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/auditctl -p x -k audittools" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/auditd -p x -k audittools" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/ssh/sshd_config -k sshd" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/passwd -p wa -k passwd_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /var/log/faillog -p wa -k logins_faillog" >> "$AUDITRULE_PERMFILE"
echo "-w /var/log/lastlog -p wa -k logins_lastlog" >> "$AUDITRULE_PERMFILE"
echo "# -w /etc/passwd -p r -k passwd_read" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/passwd -p x -k passwd_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/group -p wa -k group_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /bin/su -p x -k priv_esc" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/sudo -p x -k priv_esc" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/ssh -p x -k ssh-execute" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/sudoers -p rw -k priv_esc" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/shutdown -p x -k power" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/poweroff -p x -k power" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/reboot -p x -k power" >> "$AUDITRULE_PERMFILE"
echo "-w /sbin/halt -p x -k power" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/chown -p x -k file_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/bin/chmod -p x -k file_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/groupadd -p x -k group_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/groupmod -p x -k group_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/addgroup -p x -k group_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/useradd -p x -k user_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/usermod -p x -k user_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/sbin/adduser -p x -k user_modification" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/hosts -p wa -k hosts" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/network/ -p wa -k network" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/sysctl.conf -p wa -k sysctl" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.allow -p wa -k cron-allow" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.deny -p wa -k cron-deny" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.d/ -p wa -k cron.d" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.daily/ -p wa -k cron-daily" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.hourly/ -p wa -k cron-hourly" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.monthly/ -p wa -k cron-monthly" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/cron.weekly/ -p wa -k cron-weekly" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/crontab -p wa -k crontab" >> "$AUDITRULE_PERMFILE"
if [ -f /var/spool/cron/root ]; then
echo "-w /var/spool/cron/root -k crontab_root" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/sbin/stunnel ]; then
echo "-w /usr/sbin/stunnel -p x -k stunnel" >> "$AUDITRULE_PERMFILE"
fi
echo "# -a exit,always -F arch=b32 -F euid=0 -S execve -k rootcmd" >> "$AUDITRULE_PERMFILE"
echo "# -a exit,always -F arch=b64 -F euid=0 -S execve -k rootcmd" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S link -S symlink -k symlinked" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S link -S symlink -k symlinked" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S sethostname -k hostname" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/etc -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/bin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/sbin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/usr/bin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/usr/sbin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/var -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b32 -S open -F dir=/home -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S sethostname -k hostname" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/etc -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/bin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/sbin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/usr/bin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/usr/sbin -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/var -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "-a exit,always -F arch=b64 -S open -F dir=/home -F success=0 -k unauthedfileacess" >> "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# custom auditd rules specific for centmin mod lemp stack setups - DO NOT DELETE THIS LINE" >> "$AUDITRULE_PERMFILE"
if [ -d /usr/local/nginx/conf ]; then
echo "-w /usr/local/nginx/conf -p wa -k nginxconf_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /usr/local/nginx/conf/phpstatus.conf -p wa -k phpstatusconf_changes" >> "$AUDITRULE_PERMFILE"
NGXID=$(id -u nginx)
echo "# -a exit,always -S all -F uid=$NGXID -k nginxuser" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/local/etc/php-fpm.conf ]; then
echo "-w /usr/local/etc/php-fpm.conf -p wa -k phpfpmconf_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/local/lib/php.ini ]; then
echo "-w /usr/local/lib/php.ini -p wa -k phpini_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /etc/my.cnf ]; then
echo "-w /etc/my.cnf -p wa -k mycnf_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /root/.my.cnf ]; then
echo "-w /root/.my.cnf -p wa -k mycnfdot_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -d /etc/csf ]; then
echo "-w /etc/csf/csf.conf -p wa -k csfconf_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.blocklists -p wa -k csfpignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.pignore -p wa -k csfpignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.fignore -p wa -k csffignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.signore -p wa -k csfsignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.rignore -p wa -k csfrignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.mignore -p wa -k csfmignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.ignore -p wa -k csfignore_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/csf/csf.dyndns -p wa -k csfdyndns_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -d /etc/centminmod ]; then
echo "-w /etc/centminmod/php.d/ -p wa -k phpconfigscandir_changes" >> "$AUDITRULE_PERMFILE"
echo "-w /etc/centminmod/custom_config.inc -p wa -k cmm_persistentconfig_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -d /usr/local/src/centminmod ]; then
echo "-w /usr/local/src/centminmod -p wa -k centminmod_installdir" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /etc/pure-ftpd/pure-ftpd.conf ]; then
echo "-w /etc/pure-ftpd/pure-ftpd.conf -p wa -k pureftpd_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /etc/init.d/memcached ]; then
echo "-w /etc/init.d/memcached -p wa -k memcachedinitd_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/memcached.service ]; then
echo "-w /usr/lib/systemd/system/memcached.service -p wa -k memcachedservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/redis.service ]; then
echo "-w /usr/lib/systemd/system/redis.service -p wa -k redisservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/redis6479.service ]; then
echo "-w /usr/lib/systemd/system/redis6479.service -p wa -k redis6479service_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/redis6480.service ]; then
echo "-w /usr/lib/systemd/system/redis6480.service -p wa -k redis6480service_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/keydb.service ]; then
echo "-w /usr/lib/systemd/system/keydb.service -p wa -k keydbservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/keydb6479.service ]; then
echo "-w /usr/lib/systemd/system/keydb6479.service -p wa -k keydb6479service_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/keydb6480.service ]; then
echo "-w /usr/lib/systemd/system/keydb6480.service -p wa -k keydb6480service_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/mariadb.service ]; then
echo "-w /usr/lib/systemd/system/mariadb.service -p wa -k mariadbservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/elasticsearch.service ]; then
echo "-w /usr/lib/systemd/system/elasticsearch.service -p wa -k elasticsearchservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/postfix.service ]; then
echo "-w /usr/lib/systemd/system/postfix.service -p wa -k postfixservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/csf.service ]; then
echo "-w /usr/lib/systemd/system/csf.service -p wa -k csfservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/lfd.service ]; then
echo "-w /usr/lib/systemd/system/lfd.service -p wa -k lfdservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/pure-ftpd.service ]; then
echo "-w /usr/lib/systemd/system/pure-ftpd.service -p wa -k pureftpdservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/cockpit.service ]; then
echo "-w /usr/lib/systemd/system/cockpit.service -p wa -k cockpitservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/nginx.service ]; then
echo "-w /usr/lib/systemd/system/nginx.service -p wa -k nginxservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /usr/lib/systemd/system/php-fpm.service ]; then
echo "-w /usr/lib/systemd/system/php-fpm.service -p wa -k phpfpmservice_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -f /etc/nsd/nsd.conf ]; then
echo "-w /etc/nsd/nsd.conf -p wa -k nsdconf_changes" >> "$AUDITRULE_PERMFILE"
fi
if [ -d /usr/local/nginx/conf/conf.d ]; then
    VHOSTS=$(ls /usr/local/nginx/conf/conf.d | grep -E 'ssl.conf|.conf' | grep -E -v 'virtual.conf|^ssl.conf|demodomain.com.conf' |  sed -e 's/.ssl.conf//' -e 's/.conf//' | uniq)
    for vhostname in $VHOSTS; do
        if [ -d "/home/nginx/domains/${vhostname}/log" ]; then
            echo "-a exit,always -F arch=b32 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
            echo "-a exit,always -F arch=b32 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
            echo "-a exit,always -F arch=b64 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
            echo "-a exit,always -F arch=b64 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
        fi
    done
fi
cat "$AUDITRULE_PERMFILE" > "$CENTMINLOGDIR/auditd_rulesd_output_$DT.log"
service auditd restart >/dev/null 2>&1
chkconfig auditd on >/dev/null 2>&1    
auditctl -s; echo; auditctl -l > "$CENTMINLOGDIR/auditctl_rules_$DT.log"
echo ""$CENTMINLOGDIR/auditd_rulesd_output_$DT.log" created"
echo ""$CENTMINLOGDIR/auditctl_rules_$DT.log" created"
    fi
}

audit_logrotate() {
  echo
  echo "setup logrotation for auditd"
  echo "at: /etc/logrotate.d/auditd"
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
          VARDFSIZE=$(df --output=avail /var | tail -1)
  else
          VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
  fi

  if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
    if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_AUDITD" = [yY] ]]; then
cat > "/etc/logrotate.d/auditd" <<END
/var/log/audit/*.log {
        daily
        dateext
        missingok
        rotate 31
        minsize 100k
        maxsize 30M
        compress
        delaycompress
        compresscmd /usr/local/bin/zstd
        uncompresscmd /usr/local/bin/unzstd
        compressoptions -9 --long -T0
        compressext .zst
        notifempty
        postrotate
          touch /var/log/audit/audit.log ||:
          chmod 0600 /var/log/audit/audit.log ||:
          service auditd restart
        endscript           
}
END
    else
cat > "/etc/logrotate.d/auditd" <<END
/var/log/audit/*.log {
        daily
        dateext
        missingok
        rotate 31
        minsize 100k
        maxsize 30M
        compress
        delaycompress
        notifempty
        postrotate
          touch /var/log/audit/audit.log ||:
          chmod 0600 /var/log/audit/audit.log ||:
          service auditd restart
        endscript           
}
END
    fi
  else
    if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_AUDITD" = [yY] ]]; then
cat > "/etc/logrotate.d/auditd" <<END
/var/log/audit/*.log {
        daily
        dateext
        missingok
        rotate 31
        minsize 100k
        maxsize 200M
        compress
        delaycompress
        compresscmd /usr/local/bin/zstd
        uncompresscmd /usr/local/bin/unzstd
        compressoptions -9 --long -T0
        compressext .zst
        notifempty
        postrotate
          touch /var/log/audit/audit.log ||:
          chmod 0600 /var/log/audit/audit.log ||:
          service auditd restart
        endscript           
}
END
    else
cat > "/etc/logrotate.d/auditd" <<END
/var/log/audit/*.log {
        daily
        dateext
        missingok
        rotate 31
        minsize 100k
        maxsize 200M
        compress
        delaycompress
        notifempty
        postrotate
          touch /var/log/audit/audit.log ||:
          chmod 0600 /var/log/audit/audit.log ||:
          service auditd restart
        endscript           
}
END
    fi
  fi
}

######################################################
audit_setup() {
    # only setup audit for non-openvz systems
    if [[ ! -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        yum -q install audit audit-libs
        if [ -f /etc/audisp/plugins.d/syslog.conf ]; then
            sed -i 's|args = LOG_INFO|args = LOG_AUTHPRIV6|' /etc/audisp/plugins.d/syslog.conf
        fi
        if [ -f /etc/audit/auditd.conf ]; then
            cp -a /etc/audit/auditd.conf /etc/audit/auditd.conf.bak-initial
            sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file .*|max_log_file = 0|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file_action .*|max_log_file_action = ignore|' /etc/audit/auditd.conf
            sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
            service auditd restart >/dev/null 2>&1
            chkconfig auditd on >/dev/null 2>&1
        fi
    elif [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        if [ -f /etc/audisp/plugins.d/syslog.conf ]; then
            sed -i 's|args = LOG_INFO|args = LOG_AUTHPRIV6|' /etc/audisp/plugins.d/syslog.conf
        fi
        sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
        sed -i 's|^max_log_file .*|max_log_file = 0|' /etc/audit/auditd.conf
        sed -i 's|^max_log_file_action .*|max_log_file_action = ignore|' /etc/audit/auditd.conf
        sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
        service auditd restart >/dev/null 2>&1
        chkconfig auditd on >/dev/null 2>&1
    fi
    if [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        if [[ "$CENTOS_TEN" -eq '10' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
            if [ -f "$AUDITRULE_PERMFILE" ]; then
                auditd_customrules
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
        elif [[ "$CENTOS_NINE" -eq '9' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
            if [ -f "$AUDITRULE_PERMFILE" ]; then
                auditd_customrules
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
        elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
            if [ -f "$AUDITRULE_PERMFILE" ]; then
                auditd_customrules
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
        elif [[ "$CENTOS_SEVEN" = '7' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
            if [ -f "$AUDITRULE_PERMFILE" ]; then
                auditd_customrules
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
        elif [[ "$CENTOS_SIX" = '6' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
            if [ -f "$AUDITRULE_PERMFILE" ]; then
                auditd_customrules
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
        fi
    fi
    if [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        audit_logrotate
        echo
        echo "auditd installed and configured"
    elif [ -f /proc/user_beancounters ]; then
        echo
        echo "OpenVZ Virtualization Detected"
        echo "auditd not supported"
        echo "aborting ..."
        exit 1
    fi
}

wipe_config() {
    if [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_SIX" = '6' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        fi
cat > "$AUDITRULE_PERMFILE" <<EOF
# This file contains the auditctl rules that are loaded
# whenever the audit daemon is started via the initscripts.
# The rules are simply the parameters that would be passed
# to auditctl.

# First rule - delete all
-D

# Increase the buffers to survive stress events.
# Make this bigger for busy systems
-b $AUDITD_BUFFERSIZE

# Feel free to add below this line. See auditctl man page
EOF
        if [ -f /etc/audit/auditd.conf ]; then
            sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file .*|max_log_file = 0|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file_action .*|max_log_file_action = ignore|' /etc/audit/auditd.conf
            sed -i 's|^num_logs .*|num_logs = 40|' /etc/audit/auditd.conf
        fi
        auditd_customrules
        if [[ "$CENTOS_SIX" -eq '6' || "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
            augenrules --check >/dev/null 2>&1
            augenrules --load >/dev/null 2>&1
        fi
        rm -f /var/log/audit/audit.log.*
        audit_logrotate
        if [[ "$CENTOS_SIX" -eq '6' ]]; then
          service auditd restart >/dev/null 2>&1
          chkconfig auditd on >/dev/null 2>&1
        elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
          # systemctl restart auditd
          # systemctl enable auditd
          service auditd restart >/dev/null 2>&1
          chkconfig auditd on >/dev/null 2>&1
        fi
        echo
        echo "auditd configuration reset"
    fi
}

mariadb_audit() {
    if [[ "$AUDIT_MARIADB" = [yY] ]]; then
        mkdir -p /var/log/mysql
        chown mysql /var/log/mysql
        echo
        echo "Setup MariaDB Audit Plugin"
        echo
        ${ALIAS_MYSQL} -e "INSTALL SONAME 'server_audit';"
        ${ALIAS_MYSQL} -t -e "SHOW PLUGINS;"
        ${ALIAS_MYSQL} -e "SELECT * FROM INFORMATION_SCHEMA.PLUGINS WHERE PLUGIN_NAME='SERVER_AUDIT'\G"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_logging=on;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_events='connect,query_dml';"
        # ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_events='connect,query_dml_no_select';"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_output_type=FILE;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_path='/var/log/mysql/audit.log';"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_rotate_size=250000000;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_rotations=10;"
        echo
        echo "Update /etc/my.cnf for server_audit_logging"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        sed -i '/server_audit_output_type/d' /etc/my.cnf
        sed -i '/server_audit_file_path/d' /etc/my.cnf
        sed -i '/server_audit_file_rotate_size/d' /etc/my.cnf
        sed -i '/server_audit_file_rotations/d' /etc/my.cnf
        echo "server_audit_logging=1" >> /etc/my.cnf
        echo "#server_audit_incl_users=your_mysql_username1,your_mysql_username2" >> /etc/my.cnf
        echo "server_audit_events=connect,query_dml" >> /etc/my.cnf
        echo "#server_audit_events=connect,query_dml_no_select" >> /etc/my.cnf
        echo "server_audit_output_type=FILE" >> /etc/my.cnf
        echo "server_audit_file_path=/var/log/mysql/audit.log" >> /etc/my.cnf
        echo "server_audit_file_rotate_size=250000000" >> /etc/my.cnf
        echo "server_audit_file_rotations=10" >> /etc/my.cnf
        echo
        echo "MariaDB Audit Plugin Installed & Configured"
        echo
    fi
}


mariadb_auditoff() {
        echo
        echo "Turn Off MariaDB Audit Plugin"
        echo
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_logging=off;"
        # ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_events='connect,query_dml';"
        echo "Update /etc/my.cnf for server_audit_logging off"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        sed -i '/server_audit_output_type/d' /etc/my.cnf
        sed -i '/server_audit_file_path/d' /etc/my.cnf
        sed -i '/server_audit_file_rotate_size/d' /etc/my.cnf
        sed -i '/server_audit_file_rotations/d' /etc/my.cnf
        if [ -f /etc/centminmod/custom_config.inc ]; then
            sed -i 's|AUDIT_MARIADB.*|AUDIT_MARIADB='n'|' /etc/centminmod/custom_config.inc
        fi
        echo
        echo "MariaDB Audit Plugin Turned Off"
}

mariadb_auditon() {
        mkdir -p /var/log/mysql
        chown mysql /var/log/mysql
        echo
        echo "Turn On MariaDB Audit Plugin"
        echo
        ${ALIAS_MYSQL} -e "INSTALL SONAME 'server_audit';"
        # ${ALIAS_MYSQL} -t -e "SHOW PLUGINS;"
        ${ALIAS_MYSQL} -e "SELECT * FROM INFORMATION_SCHEMA.PLUGINS WHERE PLUGIN_NAME='SERVER_AUDIT'\G"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_logging=on;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_events='connect,query_dml';"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_output_type=FILE;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_path='/var/log/mysql/audit.log';"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_rotate_size=250000000;"
        ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_rotations=10;"
        echo
        echo "Update /etc/my.cnf for server_audit_logging on"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        sed -i '/server_audit_output_type/d' /etc/my.cnf
        sed -i '/server_audit_file_path/d' /etc/my.cnf
        sed -i '/server_audit_file_rotate_size/d' /etc/my.cnf
        sed -i '/server_audit_file_rotations/d' /etc/my.cnf
        echo "server_audit_logging=1" >> /etc/my.cnf
        echo "#server_audit_incl_users=your_mysql_username1,your_mysql_username2" >> /etc/my.cnf
        echo "server_audit_events=connect,query_dml" >> /etc/my.cnf
        echo "#server_audit_events=connect,query_dml_no_select" >> /etc/my.cnf
        echo "server_audit_output_type=FILE" >> /etc/my.cnf
        echo "server_audit_file_path=/var/log/mysql/audit.log" >> /etc/my.cnf
        echo "server_audit_file_rotate_size=250000000" >> /etc/my.cnf
        echo "server_audit_file_rotations=10" >> /etc/my.cnf
        if [ -f /etc/centminmod/custom_config.inc ]; then
            sed -i 's|AUDIT_MARIADB.*|AUDIT_MARIADB='y'|' /etc/centminmod/custom_config.inc
        fi
        echo
        echo "MariaDB Audit Plugin Turned On"
}

add_rules() {
    if [[ -f /etc/audit/auditd.conf && ! -f /proc/user_beancounters ]]; then
        if [[ "$CENTOS_TEN" = '10' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_NINE" = '9' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_SEVEN" = '7' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_SIX" = '6' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        fi
        if [ -d /usr/local/nginx/conf/conf.d ]; then
            VHOSTS=$(ls /usr/local/nginx/conf/conf.d | grep -E 'ssl.conf|.conf' | grep -E -v 'virtual.conf|^ssl.conf|demodomain.com.conf' |  sed -e 's/.ssl.conf//' -e 's/.conf//' | uniq)
            for vhostname in $VHOSTS; do
                if [[ -d "/home/nginx/domains/${vhostname}/log" && -z "$(fgrep "/home/nginx/domains/${vhostname}/log" "$AUDITRULE_PERMFILE")" ]]; then
                    echo "-a exit,always -F arch=b32 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
                    echo "-a exit,always -F arch=b32 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
                    echo "-a exit,always -F arch=b64 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
                    echo "-a exit,always -F arch=b64 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
                fi
            done
        fi
        # if auditd rules have changed restart auditd service
        if [[ "$(augenrules --check | grep 'No change' >/dev/null 2>&1; echo $?)" != '0' ]]; then
            if [[ "$CENTOS_SIX" -eq '6' || "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
                augenrules --check >/dev/null 2>&1
                augenrules --load >/dev/null 2>&1
            fi
            service auditd restart >/dev/null 2>&1
            chkconfig auditd on >/dev/null 2>&1
            echo
            echo "auditd rules list"
            echo
            auditctl -l
            echo
            echo "auditd rules updated"
            echo
        else
            echo
            echo "no required auditd rules updates available"
            echo
        fi
    fi
}

mariadb_rotatelog_now() {
  mdb_audit_logfile=$(${ALIAS_MYSQLADMIN} var | grep 'server_audit_file_path' | tr -s ' ' | awk '{print $4}' | head -n1)
  if [ -f "$mdb_audit_logfile" ]; then
    echo "Rotate MariaDB Audit Plugin Log Now"
    echo "${ALIAS_MYSQL} -e \"SET GLOBAL server_audit_file_rotate_now = ON;\""
    echo
    ${ALIAS_MYSQL} -e "SET GLOBAL server_audit_file_rotate_now = ON;"
    rotate_err=$?
    if [[ "$rotate_err" -ne '0' ]]; then
      echo "error: an issue with MariaDB Audit log rotation occurred"
    elif [[ "$rotate_err" -eq '0' ]]; then
      echo "success: MariaDB Audit log rotation completed"
    fi
  else
    echo "MariaDB Audit Plugin Log Not found or MariaDB Audit Plugin not enabled"
  fi
}

######################################################
case "$1" in
    setup )
        mariadb_audit
        audit_setup
        ;;
    resetup )
        mariadb_audit
        wipe_config
        # audit_setup
        ;;
    updaterules )
        add_rules
        ;;
    disable_mariadbplugin )
        mariadb_auditoff
        ;;
    enable_mariadbplugin )
        mariadb_auditon
        ;;
    mariadb_rotatelog )
        mariadb_rotatelog_now
        ;;
    backup )
    echo "TBA"
        ;;
    * )
    echo "$0 {setup|resetup|updaterules|disable_mariadbplugin|enable_mariadbplugin|mariadb_rotatelog|backup}"
    echo
    echo "Command Usage:"
    echo
    echo "$0 setup"
    echo "$0 resetup"
    echo "$0 updaterules"
    echo "$0 disable_mariadbplugin"
    echo "$0 enable_mariadbplugin"
    echo "$0 mariadb_rotatelog"
    echo "$0 backup"
        ;;
esac