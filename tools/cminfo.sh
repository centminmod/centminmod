#!/bin/bash
#####################################################
# quick info overview for centminmod.com installs
#####################################################
#
#####################################################
MYCNF='/etc/my.cnf'
USER='root'
PASS=''
MYSQLHOST='localhost'
#####################################################
VIRTUALCORES=$(grep -c ^processor /proc/cpuinfo)
PHYSICALCPUS=$(grep 'physical id' /proc/cpuinfo | sort -u | wc -l)
CPUCORES=$(grep 'cpu cores' /proc/cpuinfo | head -n 1 | cut -d: -f2)
CPUSPEED=$(awk -F: '/cpu MHz/{print $2}' /proc/cpuinfo | sort | uniq -c)
CPUMODEL=$(awk -F: '/model name/{print $2}' /proc/cpuinfo | sort | uniq -c)
CPUCACHE=$(awk -F: '/cache size/{print $2}' /proc/cpuinfo | sort | uniq -c)

VHOSTS=$(ls /usr/local/nginx/conf/conf.d | grep '.conf' | egrep -v 'virtual.conf|ssl.con|demodomain.com.conf' | sed -e 's/.conf//')

CENTOSVER=$(cat /etc/redhat-release | awk '{ print $3 }')

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi

if [ ! -f /usr/sbin/virt-what ]; then
    yum -y -q install virt-what
fi

if [ ! -f /usr/sbin/lshw ]; then
    yum -y -q install lshw
fi

if [ -z $PASS ]; then
    MYSQLADMINOPT="-h $MYSQLHOST"
else
    MYSQLADMINOPT="-u$USER -p$PASS -h $MYSQLHOST"
fi

#####################################################
SYSTYPE=$(virt-what | tail -1)
CENTMINMOD_INFOVER=$(head -n1 /etc/centminmod-release)
CCACHE_INFOVER=$(ccache -V | head -n1)
NGINX_INFOVER=$(nginx -v 2>&1 | awk -F "/" '{print $2}' | head -n1)
PHP_INFOVER=$(php -v 2>&1 | head -n1 | cut -d "(" -f1 | awk '{print $2}')
MARIADB_INFOVER=$(rpm -qa | grep -i MariaDB-server | head -n1 | cut -d '-' -f3)
MEMCACHEDSERVER_INFOVER=$(/usr/local/bin/memcached -h | head -n1 | awk '{print $2}')
CSF_INFOVER=$(csf -v | head -n1 | awk '{print $2}')
SIEGE_INFOVER=$(siege -V 2>&1 | head -n1 | awk '{print $2}')
NSD_INFOVER=$(nsd -v 2>&1 | head -n1 | awk '{print $3}')
APC_INFOVER=$(php --ri apc | awk '/Version/ {print $3}' | head -n1)
OPCACHE_INFOVER=$(php -v 2>&1 | grep OPcache | awk '{print $4}' | sed 's/,//')

# only assign variables if mysql is running
if [[ -z "$(service mysql status | grep not)" ]]; then
DATABSELIST=$(mysql $MYSQLADMINOPT -e 'show databases;' | grep -Ev '(Database|information_schema|performance_schema)')
MYSQLUPTIME=$(mysqladmin $MYSQLADMINOPT ext | awk '/Uptime|Uptime_since_flush_status/ { print $4 }' | head -n1)
MYSQLUPTIMEFORMAT=$(mysqladmin $MYSQLADMINOPT ver | awk '/Uptime/ { print $2, $3, $4, $5, $6, $7, $8, $9 }')
MYSQLSTART=$(mysql $MYSQLADMINOPT -e "SELECT FROM_UNIXTIME(UNIX_TIMESTAMP() - variable_value) AS server_start FROM INFORMATION_SCHEMA.GLOBAL_STATUS WHERE variable_name='Uptime';" | egrep -Ev '+--|server_start')
fi
PAGESPEEDSTATUS=$(grep 'pagespeed off' /usr/local/nginx/conf/pagespeed.conf)

if [ -f /usr/local/sbin/maldet ]; then
    MALDET_INFOVER=$(/usr/local/sbin/maldet -v | head -n1 | awk '{print $4}')
fi

if [ -f /usr/bin/clamscan ]; then
    CLAMAV_INFOVER=$(clamscan -V | head -n1 | awk -F "/" '{print $1}' | awk '{print $2}')
fi
#####################################################
if [[ -z "$SYSTYPE" ]]; then
    SYSTYPE='not virtualized'
fi

if [[ -z "$PAGESPEEDSTATUS" ]]; then
    PS=ON
else
    PS=OFF
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
#####################################################
setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
# wget -q --no-check-certificate -O /usr/bin/cminfo https://gist.githubusercontent.com/centminmod/828a8ae1add0397e740b/raw/cminfo.sh
wget -q --no-check-certificate -O /usr/bin/cminfo https://raw.githubusercontent.com/centminmod/centminmod/123.08centos7beta01/tools/cminfo.sh
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

# insert itself into cronjob for auto updates
if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    echo "*/4 * * * * /usr/bin/cminfo_updater" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi

infooutput() {
echo "------------------------------------------------------------------"
echo " Centmin Mod Quick Info:"
echo "------------------------------------------------------------------"

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
if [[ -z "$(service mysql status | grep not)" ]]; then
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
    echo "* $d - /usr/local/nginx/conf/conf.d/${d}.conf"; 
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

egrep '(^user|^worker_processes|^worker_priority|^worker_rlimit_nofile|^timer_resolution|^pcre_jit|^worker_connections|^accept_mutex|^multi_accept|tcp_|server_tokens|keepalive_|lingering_|gzip|client_|connection_pool_size|directio|large_client|types_hash|server_names_hash|open_file|open_log)' /usr/local/nginx/conf/nginx.conf | egrep -v 'gzip_ratio'

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
#########
case "$1" in
    info)
    infooutput
        ;;
    update)
    setupdate
        ;;
    *)
    infooutput
        ;;
esac