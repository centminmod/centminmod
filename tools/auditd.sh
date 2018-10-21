#!/bin/bash
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

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
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
  AUDITD_BUFFERSIZE='2048'
elif [[ "$AUDITD_TOTALMEM" -ge '499001' && "$AUDITD_TOTALMEM" -le '1030000' ]]; then
  AUDITD_BUFFERSIZE='4096'
elif [[ "$AUDITD_TOTALMEM" -ge '1030001' && "$AUDITD_TOTALMEM" -le '2040000' ]]; then
  AUDITD_BUFFERSIZE='8192'
elif [[ "$AUDITD_TOTALMEM" -ge '2040001' && "$AUDITD_TOTALMEM" -le '3040000' ]]; then
  AUDITD_BUFFERSIZE='16384'
elif [[ "$AUDITD_TOTALMEM" -ge '3040001' && "$AUDITD_TOTALMEM" -le '4000000' ]]; then
  AUDITD_BUFFERSIZE='32768'
elif [[ "$AUDITD_TOTALMEM" -ge '4000001' && "$AUDITD_TOTALMEM" -le '6020000' ]]; then
  AUDITD_BUFFERSIZE='65536'
elif [[ "$AUDITD_TOTALMEM" -ge '6020001' && "$AUDITD_TOTALMEM" -le '8060000' ]]; then
  AUDITD_BUFFERSIZE='131072'
elif [[ "$AUDITD_TOTALMEM" -ge '8060001' && "$AUDITD_TOTALMEM" -le '16000000' ]]; then
  AUDITD_BUFFERSIZE='262144'
elif [[ "$AUDITD_TOTALMEM" -ge '16000001' ]]; then
  AUDITD_BUFFERSIZE='524288'
fi

######################################################
auditd_customrules() {
    if [[ "$(grep -c 'custom auditd rules' "$AUDITRULE_PERMFILE")" -ne '2' ]]; then
        if [ -d /usr/local/nginx/conf/conf.d ]; then
            VHOSTS=$(ls /usr/local/nginx/conf/conf.d | egrep 'ssl.conf|.conf' | egrep -v 'virtual.conf|^ssl.conf|demodomain.com.conf' |  sed -e 's/.ssl.conf//' -e 's/.conf//' | uniq)
        fi

sed -i "s|-b 320|-b $AUDITD_BUFFERSIZE|" "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# continue loading rules when it runs rule syntax errors" >> "$AUDITRULE_PERMFILE"
echo "#-c" >> "$AUDITRULE_PERMFILE"
echo "#-i" >> "$AUDITRULE_PERMFILE"
echo "" >> "$AUDITRULE_PERMFILE"
echo "# Generate at most 1000 audit messages per second" >> "$AUDITRULE_PERMFILE"
echo "-r 1000" >> "$AUDITRULE_PERMFILE"
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
if [ -f /etc/nsd/nsd.conf ]; then
echo "-w /etc/nsd/nsd.conf -p wa -k nsdconf_changes" >> "$AUDITRULE_PERMFILE"
fi
for vhostname in $VHOSTS; do
    if [ -d "/home/nginx/domains/${vhostname}/log" ]; then
        echo "-a exit,always -F arch=b32 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
        echo "-a exit,always -F arch=b32 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
        echo "-a exit,always -F arch=b64 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
        echo "-a exit,always -F arch=b64 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
    fi
done
cat "$AUDITRULE_PERMFILE" > "$CENTMINLOGDIR/auditd_rulesd_output_$DT.log"
service auditd restart >/dev/null 2>&1
chkconfig auditd on >/dev/null 2>&1    
auditctl -s; echo; auditctl -l > "$CENTMINLOGDIR/auditctl_rules_$DT.log"
echo ""$CENTMINLOGDIR/auditd_rulesd_output_$DT.log" created"
echo ""$CENTMINLOGDIR/auditctl_rules_$DT.log" created"
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
            sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file .*|max_log_file = 35|' /etc/audit/auditd.conf
            sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
            service auditd restart >/dev/null 2>&1
            chkconfig auditd on >/dev/null 2>&1
        fi
    elif [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        if [ -f /etc/audisp/plugins.d/syslog.conf ]; then
            sed -i 's|args = LOG_INFO|args = LOG_AUTHPRIV6|' /etc/audisp/plugins.d/syslog.conf
        fi
        sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
        sed -i 's|^max_log_file .*|max_log_file = 35|' /etc/audit/auditd.conf
        sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
        service auditd restart >/dev/null 2>&1
        chkconfig auditd on >/dev/null 2>&1
    fi
    if [[ -f /sbin/aureport && ! -f /proc/user_beancounters ]]; then
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
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
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
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
            sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
            sed -i 's|^max_log_file .*|max_log_file = 35|' /etc/audit/auditd.conf
            sed -i 's|^num_logs .*|num_logs = 20|' /etc/audit/auditd.conf
        fi
        auditd_customrules
        if [[ "$CENTOS_SIX" = '6' || "$CENTOS_SEVEN" = '7' ]]; then
            augenrules --check >/dev/null 2>&1
            augenrules --load >/dev/null 2>&1
        fi
        service auditd restart >/dev/null 2>&1
        chkconfig auditd on >/dev/null 2>&1
        echo
        echo "auditd configuration reset"
    fi
}

mariadb_audit() {
    if [[ "$AUDIT_MARIADB" = [yY] ]]; then
        echo
        echo "Setup MariaDB Audit Plugin"
        echo
        mysql -e "INSTALL SONAME 'server_audit';"
        mysql -t -e "SHOW PLUGINS;"
        mysql -e "SELECT * FROM INFORMATION_SCHEMA.PLUGINS WHERE PLUGIN_NAME='SERVER_AUDIT'\G"
        mysql -e "SET GLOBAL server_audit_logging=on;"
        mysql -e "SET GLOBAL server_audit_events='connect,query_dml';"
        echo
        echo "Update /etc/my.cnf for server_audit_logging"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        echo "server_audit_logging=1" >> /etc/my.cnf
        echo "server_audit_events=connect,query_dml" >> /etc/my.cnf
        echo
        echo "MariaDB Audit Plugin Installed & Configured"
        echo
    fi
}


mariadb_auditoff() {
        echo
        echo "Turn Off MariaDB Audit Plugin"
        echo
        mysql -e "SET GLOBAL server_audit_logging=off;"
        # mysql -e "SET GLOBAL server_audit_events='connect,query_dml';"
        echo "Update /etc/my.cnf for server_audit_logging off"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        echo "server_audit_logging=0" >> /etc/my.cnf
        echo "server_audit_events=connect,query_dml" >> /etc/my.cnf
        if [ -f /etc/centminmod/custom_config.inc ]; then
            sed -i 's|AUDIT_MARIADB.*|AUDIT_MARIADB='n'|' /etc/centminmod/custom_config.inc
        fi
        echo
        echo "MariaDB Audit Plugin Turned Off"
}

mariadb_auditon() {
        echo
        echo "Turn On MariaDB Audit Plugin"
        echo
        mysql -e "INSTALL SONAME 'server_audit';"
        # mysql -t -e "SHOW PLUGINS;"
        mysql -e "SELECT * FROM INFORMATION_SCHEMA.PLUGINS WHERE PLUGIN_NAME='SERVER_AUDIT'\G"
        mysql -e "SET GLOBAL server_audit_logging=on;"
        mysql -e "SET GLOBAL server_audit_events='connect,query_dml';"
        echo
        echo "Update /etc/my.cnf for server_audit_logging on"
        sed -i '/server_audit_logging/d' /etc/my.cnf
        sed -i '/server_audit_events/d' /etc/my.cnf
        echo "server_audit_logging=1" >> /etc/my.cnf
        echo "server_audit_events=connect,query_dml" >> /etc/my.cnf
        if [ -f /etc/centminmod/custom_config.inc ]; then
            sed -i 's|AUDIT_MARIADB.*|AUDIT_MARIADB='y'|' /etc/centminmod/custom_config.inc
        fi
        echo
        echo "MariaDB Audit Plugin Turned On"
}

add_rules() {
    if [[ -f /etc/audit/auditd.conf && ! -f /proc/user_beancounters ]]; then
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        elif [[ "$CENTOS_SIX" = '6' ]]; then
            AUDITRULE_FILE='/etc/audit/audit.rules'
            AUDITRULE_PERMFILE='/etc/audit/rules.d/audit.rules'
        fi
        if [ -d /usr/local/nginx/conf/conf.d ]; then
            VHOSTS=$(ls /usr/local/nginx/conf/conf.d | egrep 'ssl.conf|.conf' | egrep -v 'virtual.conf|^ssl.conf|demodomain.com.conf' |  sed -e 's/.ssl.conf//' -e 's/.conf//' | uniq)
        fi
        for vhostname in $VHOSTS; do
            if [[ -d "/home/nginx/domains/${vhostname}/log" && -z "$(fgrep "/home/nginx/domains/${vhostname}/log" "$AUDITRULE_PERMFILE")" ]]; then
                echo "-a exit,always -F arch=b32 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
                echo "-a exit,always -F arch=b32 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
                echo "-a exit,always -F arch=b64 -S unlink -S unlinkat -S rmdir -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logdeletion" >> "$AUDITRULE_PERMFILE"
                echo "-a exit,always -F arch=b64 -S rename -S renameat -F dir=/home/nginx/domains/${vhostname}/log -F success=0 -k ${vhostname}_logrename" >> "$AUDITRULE_PERMFILE"
            fi
        done
        # if auditd rules have changed restart auditd service
        if [[ "$(augenrules --check | grep 'No change' >/dev/null 2>&1; echo $?)" != '0' ]]; then
            if [[ "$CENTOS_SIX" = '6' || "$CENTOS_SEVEN" = '7' ]]; then
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
    backup )
    echo "TBA"
        ;;
    * )
    echo "$0 {setup|resetup|updaterules|disable_mariadbplugin|enable_mariadbplugin|backup}"
    echo
    echo "Command Usage:"
    echo
    echo "$0 setup"
    echo "$0 resetup"
    echo "$0 updaterules"
    echo "$0 disable_mariadbplugin"
    echo "$0 enable_mariadbplugin"
    echo "$0 backup"
        ;;
esac