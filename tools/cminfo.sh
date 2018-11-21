#!/bin/bash
#####################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
#####################################################
# quick info overview for centminmod.com installs
#####################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
#####################################################
MYCNF='/etc/my.cnf'
USER='root'
PASS=''
MYSQLHOST='localhost'
#####################################################
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

if [ ! -f /usr/sbin/virt-what ]; then
    yum -y -q install virt-what
fi

if [ ! -f /usr/sbin/lshw ]; then
    yum -y -q install lshw
fi

if [ ! -f /usr/bin/tree ]; then
    yum -y -q install tree
fi

if [ -z $PASS ]; then
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
else
  ipv_forceopt='4'
fi

#####################################################
top_info() {
    SYSTYPE=$(virt-what | head -n1)
    CENTMINMOD_INFOVER=$(head -n1 /etc/centminmod-release)
    CCACHE_INFOVER=$(ccache -V | head -n1)
    NGINX_INFOVER=$(nginx -v 2>&1 | awk -F "/" '{print $2}' | head -n1)
    PHP_INFOVER=$(php -v 2>&1 | head -n1 | cut -d "(" -f1 | awk '{print $2}')
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
    if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
    DATABSELIST=$(mysql $MYSQLADMINOPT -e 'show databases;' | grep -Ev '(Database|information_schema|performance_schema)')
    MYSQLUPTIME=$(mysqladmin $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
    MYSQLUPTIMEFORMAT=$(mysqladmin $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
    MYSQLSTART=$(mysql $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | egrep -Ev '+--|server_start')
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
    echo " Centmin Mod Top Info:"
    echo "------------------------------------------------------------------"

    echo " Server Location Info"
    # echo
    curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'ip:|phone|postal|loc'
    echo "  ASN: $(curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/org 2>&1)"
    
    echo
    echo " Processors" "physical = ${PHYSICALCPUS}, cores = ${CPUCORES}, virtual = ${VIRTUALCORES}, hyperthreading = ${HT}"
    echo
    echo "$CPUSPEED"
    echo "$CPUMODEL"
    echo "$CPUCACHE"
    echo ""
    
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        echo -ne " System Up Since: \t"; uptime -s
        echo -ne " System Uptime: \t"; uptime -p
    else
        echo -ne " System Uptime: \t"; uptime | awk '{print $2, $3, $4, $5}'
    fi
    if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
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
    echo -e " MariaDB Version: \t$MARIADB_INFOVER"
    echo -e " CSF Firewall: \t\t$CSF_INFOVER"
    echo -e " Memcached Server: \t$MEMCACHEDSERVER_INFOVER"
    echo -e " NSD Version: \t\t$NSD_INFOVER"
    echo -e " Siege Version: \t$SIEGE_INFOVER"
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
    echo "df -hT"
    df -hT
    echo
    echo "------------------------------------------------------------------"
    echo "Filter sar -q for times cpu load avg (1min) hit/exceeded cpu threads max"
    loadavg=$(printf "%0.2f" $(nproc))
    sarfilteredone=$(sar -q | sed -e "s|$(hostname)|hostname|g" | grep -v runq-sz | awk -v lvg=$loadavg '{if ($4>=lvg) print $0}' | grep -v Linux)
    echo
    echo "${sarfilteredone:-no times found that >= $loadavg}"
    echo
    echo "------------------------------------------------------------------"
    echo "Filter sar -q for times cpu load avg (5min) hit/exceeded cpu threads max"
    loadavg=$(printf "%0.2f" $(nproc))
    sarfilteredfive=$(sar -q | sed -e "s|$(hostname)|hostname|g" | grep -v runq-sz | awk -v lvg=$loadavg '{if ($5>=lvg) print $0}' | grep -v Linux)
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
    if [ -d /usr/lib/systemd ]; then
        echo "top -bcn1 -w200"
        top -bcn1 -w200
    else
        echo "top -bcn1"
        top -bcn1
    fi
    echo
    echo "------------------------------------------------------------------"
    echo "iotop -bton1 -P"
    iotop -bton1 -P
    echo
    echo "------------------------------------------------------------------"
    echo "pidstat -durh 1 5 | sed -e \"s|\$(hostname)|hostname|g\""
    pidstat -durh 1 5 | sed -e "s|$(hostname)|hostname|g"
    echo "------------------------------------------------------------------"
    echo "Stats saved at: ${CENTMINLOGDIR}/cminfo-top-${DT}.log"
    echo "------------------------------------------------------------------"
    echo
}

netstat_info() {
    sshclient=$(echo $SSH_CLIENT | awk '{print $1}')
    nic=$(ifconfig -s 2>&1 | egrep -v '^Iface|^lo|^gre' | awk '{print $1}')
    bandwidth_avg=$(sar -n DEV 1 1)
    bandwidth_inout=$(echo "$nic" | while read i; do echo "$bandwidth_avg" | grep 'Average:' | awk -v tnic="$i" '$0~tnic{print tnic, "In: ",$5,"Out:",$6}'; done | column -t)
    packets_inout=$(echo "$nic" | while read i; do echo "$bandwidth_avg" | grep 'Average:' | awk -v tnic="$i" '$0~tnic{print tnic, "In: ",$3,"Out:",$3}'; done | column -t)
    netstat_http=$(netstat -an | fgrep ':80 ')
    netstat_https=$(netstat -an | fgrep ':443 ')
    netstat_outbound=$(netstat -plant | egrep -v 'and|servers|Address' | awk '{print $5,$6,$7}' | grep -v ':\*' | grep -v '127.0.0.1' | sed -e "s|$sshclient|ssh-client-ip|g" | sort | uniq -c | sort -rn | head -n10 | column -t)
    netstat_ips=$(netstat -tn)
    netstat_ipstop=$(echo "$netstat_ips" | egrep -v 'servers|Address' | awk '{print $5}' | rev | cut -d: -f2- | rev | sort | uniq -c | sort -rn | head -n10)
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
CMINFOLINK='https://raw.githubusercontent.com/centminmod/centminmod/master/tools/cminfo.sh'

# fallback mirror
curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 \$CMINFOLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
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
VHOSTS=$(ls /home/nginx/domains | egrep -v 'demodomain.com.conf')
VHOSTSCONF=$(ls /usr/local/nginx/conf/conf.d | egrep -vw '^ssl.conf' | uniq)

#####################################################
SYSTYPE=$(virt-what | head -n1)
CENTMINMOD_INFOVER=$(head -n1 /etc/centminmod-release)
CCACHE_INFOVER=$(ccache -V | head -n1)
NGINX_INFOVER=$(nginx -v 2>&1 | awk -F "/" '{print $2}' | head -n1)
PHP_INFOVER=$(php -v 2>&1 | head -n1 | cut -d "(" -f1 | awk '{print $2}')
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
if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
DATABSELIST=$(mysql $MYSQLADMINOPT -e 'show databases;' | grep -Ev '(Database|information_schema|performance_schema)')
MYSQLUPTIME=$(mysqladmin $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
MYSQLUPTIMEFORMAT=$(mysqladmin $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
MYSQLSTART=$(mysql $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | egrep -Ev '+--|server_start')
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
curl -${ipv_forceopt}s${CURL_TIMEOUTS} https://ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'phone|postal|loc'

echo
echo "Processors" "physical = ${PHYSICALCPUS}, cores = ${CPUCORES}, virtual = ${VIRTUALCORES}, hyperthreading = ${HT}"
echo
echo "$CPUSPEED"
echo "$CPUMODEL"
echo "$CPUCACHE"
echo ""

if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo -ne " System Up Since: \t"; uptime -s
    echo -ne " System Uptime: \t"; uptime -p
else
    echo -ne " System Uptime: \t"; uptime | awk '{print $2, $3, $4, $5}'
fi
if [[ "$(ps -o comm -C mysqld >/dev/null 2>&1; echo $?)" = '0' ]]; then
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
echo -e " MariaDB Version: \t$MARIADB_INFOVER"
echo -e " CSF Firewall: \t\t$CSF_INFOVER"
echo -e " Memcached Server: \t$MEMCACHEDSERVER_INFOVER"
echo -e " NSD Version: \t\t$NSD_INFOVER"
echo -e " Siege Version: \t$SIEGE_INFOVER"
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

if [[ -z "$(service mysql status | grep not)" ]]; then
echo
echo "------------------------------------------------------------------"
echo " MySQL Databases:"
echo "------------------------------------------------------------------"
echo
for db in $DATABSELIST; do 
DBIDXSIZE=$(mysql $MYSQLADMINOPT -e "SELECT CONCAT(ROUND(SUM(index_length)/(1024*1024), 2), ' MB') AS 'Total Index Size' FROM information_schema.TABLES WHERE table_schema LIKE '$db';" | egrep -Ev '(+-|Total Index Size)')
DBDATASIZE=$(mysql $MYSQLADMINOPT -e "SELECT CONCAT(ROUND(SUM(data_length)/(1024*1024), 2), ' MB') AS 'Total Data Size'
FROM information_schema.TABLES WHERE table_schema LIKE '$db';" | egrep -Ev '(+-|Total Data Size)')

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
nginx -V

echo
echo "------------------------------------------------------------------"
echo " Nginx Settings:"
echo "------------------------------------------------------------------"
echo

egrep '(^user|^worker_processes|^worker_priority|^worker_rlimit_nofile|^timer_resolution|^pcre_jit|^worker_connections|^accept_mutex|^multi_accept|^accept_mutex_delay|map_hash|server_names_hash|variables_hash|tcp_|^limit_|sendfile|server_tokens|keepalive_|lingering_|gzip|client_|connection_pool_size|directio|large_client|types_hash|server_names_hash|open_file|open_log|^include|^#include)' /usr/local/nginx/conf/nginx.conf | egrep -v 'gzip_ratio'

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Configuration:"
echo "------------------------------------------------------------------"
echo
php -i | grep configure

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Settings /usr/local/etc/php-fpm.conf:"
echo "------------------------------------------------------------------"
echo
egrep '(^log_level|^pid|^error_log|^user|^group|^listen|^pm|^rlimit|^slowlog|^ping.|^php_admin)' /usr/local/etc/php-fpm.conf

echo
echo "------------------------------------------------------------------"
echo " PHP-FPM Extensions Loaded:"
echo "------------------------------------------------------------------"
echo
php -m

echo
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
    netstat)
    netstat_info
        ;;
    top)
    {
    top_info
    } 2>&1 | tee "${CENTMINLOGDIR}/cminfo-top-${DT}.log"
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
    *)
    echo "$0 {info|update|netstat|top|listlogs|debug-menuexit|versions}"
        ;;
esac