backupmsg104() {

echo ""
cat <<EOF
-------------------------------------------------------------------------------
 Please test on a test server and not a live production server. But if you use 
 on live server or if MySQL databases already exist, please back them up before 
 running this upgrade script via mysqldump - instructions at: 
 http://dev.mysql.com/doc/refman/5.6/en/mysqldump.html.

 ###################################################################
 ###          IMPORTANT MYSQLDUMP BACKUP BEFORE UPGRADE          ###
 ###        OR RISK DATA LOSS DUE TO MARIADB 10.4 Changes        ###
 ###################################################################

 For each DATABASENAME backup via mysqldump i.e.

  mysqldump --opt --routines DATABASENAME > /path/to/DATABASENAME_backup.sql
EOF
sleep 5

echo "------------------------------------------------------------------------"
echo ""
read -ep "Have you backed up your databases ? [y/n] " askbackup

if [[ "$askbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.4.x upgrade..." $boldyellow
exit
fi

echo ""
echo "MariaDB 10.4.x https://mariadb.com/kb/en/library/what-is-mariadb-104/"
echo 
echo "!!  Note  !!"
echo "upgrade process can take more >5 minutes.. you may want to shutdown"
echo "nginx server to not show mysql errors to visitors on your site or" 
echo "put site(s) into maintenance mode before proceeding with mysql upgrade"
sleep 15
echo
read -ep "Do you want to continue with upgrade to MariaDB 10.4.x ? [y/n] " contbackup

if [[ "$contbackup" == [nN] ]]; then
echo ""
cecho "Aborting MariaDB 10.4.x upgrade..." $boldyellow
exit
fi

    # if you set /etc/centminmod/custom_config.inc file to
    # MARIADB_UPDATEMAINTENANCE=y
    # then whenever you run mariadb upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$MARIADB_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus off
        fi
    fi

echo
echo "checking mysql root password works in /root/.my.cnf"
sleep 5
echo "mysqladmin ver"
mysqladmin ver

CHECK_MYSQLPASS=$?
if [[ "$CHECK_MYSQLPASS" != '0' ]]; then
    echo
    echo "mysql root user password in /root/.my.cnf is not correct"
    if [ ! -f /root/.my.cnf ]; then
        echo "or"
        echo "/root/.my.cnf file doesn't exist"
    fi
    cecho "Aborting MariaDB 10.4.x upgrade..." $boldyellow
    exit
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "Preparing Shutdown For Upgrade"
    fi

# full purge and change buffer merge before shutting down to ensure data files are in sync 
# prior to upgrade
if [[ "$SKIP_INNODBFASTSHUTDOWNDISABLE" = [nN] ]]; then
    echo
    ${ALIAS_MYSQL} -e "set global innodb_fast_shutdown=0;"
    echo "continuing upgrade in ~300 seconds..."
    echo "..."
    sleep 20
    echo "..... continue in 300s"
    sleep 20
    echo "....... continue in 280s"
    sleep 20
    echo "......... continue in 260s"
    sleep 20
    echo "........... continue in 240s"
    sleep 20
    echo "............. continue in 220s"
    sleep 20
    echo "............... continue in 200s"
    sleep 20
    echo "................. continue in 180s"
    sleep 20
    echo ".................. continue in 160s"
    sleep 20
    echo ".................... continue in 140s"
    sleep 20
    echo "...................... continue in 120s"
    sleep 20
    echo "........................ continue in 80s"
    sleep 20
    echo ".......................... continue in 60s"
    sleep 20
    echo "............................ continue in 40s"
    sleep 20
    echo ".............................. continue in 20s"
    sleep 20
fi
}

# Pre-flight repository validation function
# Tests if MariaDB repository is accessible before removing existing packages
validate_mariadb_repo() {
    local repo_url="${1:-https://archive.mariadb.org/mariadb-10.4}"
    local repo_version="${2:-10.4}"
    local os_arch="${3:-centos7-amd64}"
    
    # Detect distribution from /etc/os-release for future compatibility
    local distro_name="centos"
    if [ -f /etc/os-release ]; then
        if grep -qi "almalinux" /etc/os-release; then
            distro_name="almalinux"
        elif grep -qi "rocky" /etc/os-release; then
            distro_name="rockylinux"
        fi
    fi
    
    echo
    echo "*************************************************"
    cecho "Pre-flight MariaDB Repository Validation" $boldgreen
    echo "*************************************************"
    echo "Testing repository accessibility before upgrade..."
    echo
    
    # MariaDB 10.4 is deprecated and always uses archive with centos paths
    # Test repomd.xml accessibility
    local test_url="${repo_url}/yum/${os_arch}/repodata/repomd.xml"
    echo "Testing: $test_url"
    
    # Try to download repomd.xml
    curl -sL --connect-timeout 10 --max-time 30 "$test_url" -o /tmp/mariadb_repomd_test.xml 2>/dev/null
    local curl_exit_code=$?
    
    if [[ "$curl_exit_code" -ne 0 ]]; then
        echo
        cecho "ERROR: Cannot access MariaDB repository at $test_url" $boldred
        cecho "Repository validation failed with curl exit code: $curl_exit_code" $boldred
        rm -f /tmp/mariadb_repomd_test.xml
        return 1
    fi
    
    # Validate XML format
    if command -v xmllint >/dev/null 2>&1; then
        xmllint --format /tmp/mariadb_repomd_test.xml >/dev/null 2>&1
        local xml_exit_code=$?
        
        if [[ "$xml_exit_code" -ne 0 ]]; then
            echo
            cecho "ERROR: Invalid repository metadata XML from $test_url" $boldred
            cecho "Repository appears corrupted or inaccessible" $boldred
            rm -f /tmp/mariadb_repomd_test.xml
            return 1
        fi
    fi
    
    # Create temporary repo configuration for package query test
    local temp_repo="/tmp/mariadb_test_$$.repo"
    cat > "$temp_repo" <<EOF
[mariadb-test]
name = MariaDB Test Repo
baseurl = ${repo_url}/yum/${os_arch}
gpgcheck=0
enabled=1
EOF
    
    # Add module_hotfixes for EL8+ systems to allow MariaDB packages to override module streams
    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]]; then
        echo "module_hotfixes=1" >> "$temp_repo"
    fi
    
    echo "Testing package availability..."
    # Test if we can query packages from the repository
    yum -q --disablerepo="*" --enablerepo="mariadb-test" -c "$temp_repo" list available MariaDB-server 2>/dev/null
    local yum_exit_code=$?
    
    # Clean up
    rm -f "$temp_repo" /tmp/mariadb_repomd_test.xml
    
    if [[ "$yum_exit_code" -ne 0 ]]; then
        echo
        cecho "WARNING: Cannot query packages from MariaDB repository" $boldyellow
        cecho "Repository URL: ${repo_url}/yum/${os_arch}" $boldyellow
        echo
        echo "Possible causes:"
        echo "1. Repository is deprecated or moved"
        echo "2. Network connectivity issues"
        echo "3. Geographic mirror is down"
        echo
        echo "Attempting fallback validation..."
        
        # Try fallback to archive.mariadb.org explicitly
        if [[ "$repo_url" != "https://archive.mariadb.org/mariadb-${repo_version}" ]]; then
            echo "Testing archive repository..."
            validate_mariadb_repo "https://archive.mariadb.org/mariadb-${repo_version}" "$repo_version" "$os_arch"
            return $?
        else
            cecho "ERROR: Repository validation failed. Cannot proceed with upgrade." $boldred
            echo
            echo "Manual intervention required:"
            echo "1. Check network connectivity"
            echo "2. Verify repository URL is correct"
            echo "3. Consider using a different mirror"
            echo
            cecho "Aborting MariaDB upgrade to prevent system damage." $boldred
            return 1
        fi
    fi
    
    echo
    cecho "Repository validation successful!" $boldgreen
    echo "Repository is accessible and packages are available."
    echo
    return 0
}

mysqlinitfix() {

if [[ -z "$(grep 'user_beancounters' /etc/init.d/mysql)" ]]; then
sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql
fi

}

mariadb103uninstall() {
OLDMARIADBVERS=`rpm -qa | grep MariaDB`

echo "#####################################################"

# List current existing MariaDB versions for RPMs
echo "Current MariaDB versions installed:"
for mrpm in $OLDMARIADBVERS
do
echo $mrpm
done

echo "#####################################################"

# Geographic detection for mirror selection (moved from mariadb104install)
MDB_TOTALMEM=$(($(awk '/MemTotal/ {print $2}' /proc/meminfo)/1024))
MDB_VIRTWHAT_GET=$(virt-what)
MDB_VIRTWHAT="${MDB_VIRTWHAT_GET:-d}"
MDB_DTIME=$(tail -1 /root/centminlogs/centminmod_downloadtimes_*.log)
MDB_DTIME_SEC=$(echo "$MDB_DTIME" |awk '{printf("%.2f\n",$7)}')
MDB_NTIME=$(tail -1 /root/centminlogs/centminmod_ngxinstalltime_*.log)
MDB_NTIME_SEC=$(echo "$MDB_NTIME" |awk '{printf("%.2f\n",$7)}')
if [[ "$VPS_GEOIPCHECK_V3" = [yY] ]]; then
    MDB_GEOIP=$(curl -${ipv_forceopt}s -A "$CURL_AGENT $SCRIPT_VERSION $CURL_CPUMODEL $CURL_CPUSPEED 10.4 upgrade D:$MDB_DTIME_SEC N:$MDB_NTIME_SEC $MDB_VIRTWHAT $MDB_CITY $MDB_COLO" https://geoip.centminmod.com/v3 | jq -r)
elif [[ "$VPS_GEOIPCHECK_V4" = [yY] ]]; then
    MDB_GEOIP=$(curl -${ipv_forceopt}s -A "$CURL_AGENT $SCRIPT_VERSION $CURL_CPUMODEL $CURL_CPUSPEED 10.4 upgrade D:$MDB_DTIME_SEC N:$MDB_NTIME_SEC $MDB_VIRTWHAT $MDB_CITY $MDB_COLO" https://geoip.centminmod.com/v4 | jq -r)
fi
MDB_COUNTRY=$(echo "$MDB_GEOIP" | jq -r '.country')
MDB_CONTINENT=$(echo "$MDB_GEOIP" | jq -r '.continent')
MDB_CITY=$(echo "$MDB_GEOIP" | jq -r '.city')
MDB_COLO=$(echo "$MDB_GEOIP" | jq -r '.colo')

# Set MariaDB version
MDB_REPO_VER=10.4

# Determine OS architecture and repository paths
local os_arch=""
if [[ "$CENTOS_NINE" = '9' && "$(uname -m)" == 'aarch64' ]]; then
    os_arch="centos9-aarch64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos9-aarch64"
elif [[ "$CENTOS_NINE" = '9' && "$(uname -m)" == 'x86_64' ]]; then
    os_arch="centos9-amd64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos9-amd64"
elif [[ "$CENTOS_EIGHT" = '8' && "$(uname -m)" == 'aarch64' ]]; then
    os_arch="centos8-aarch64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos8-aarch64"
elif [[ "$CENTOS_EIGHT" = '8' && "$(uname -m)" == 'x86_64' ]]; then
    os_arch="centos8-amd64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos8-amd64"
elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" == 'aarch64' ]]; then
    os_arch="centos7-aarch64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos7-aarch64"
elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" == 'x86_64' ]]; then
    os_arch="centos7-amd64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos7-amd64"
else
    # Default fallback
    os_arch="centos7-amd64"
    MDB_REPO_DIR="${MDB_REPO_VER}/centos7-amd64"
fi

# MariaDB 10.4 is deprecated and uses archive.mariadb.org
# Set the mirror URL before validation
MDB_MIRROR_BASEURL='https://archive.mariadb.org/mariadb-10.4'
MDB_ARCHIVES_PUBKEY='https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY'

# Pre-flight validation: Test repository before removing packages
# Now using the selected mirror URL
if ! validate_mariadb_repo "$MDB_MIRROR_BASEURL" "$MDB_REPO_VER" "$os_arch"; then
    echo
    cecho "ERROR: MariaDB repository validation failed!" $boldred
    cecho "Cannot proceed with package removal until repository is accessible." $boldred
    echo
    echo "The upgrade has been safely aborted. Your existing MariaDB installation remains intact."
    echo
    # Restore fast shutdown if it was disabled
    if [[ "$SKIP_INNODBFASTSHUTDOWNDISABLE" = [nN] ]]; then
        ${ALIAS_MYSQL} -e "set global innodb_fast_shutdown=1;" 2>/dev/null
    fi
    exit 1
fi

echo "Proceeding to remove existing MariaDB versions:"
echo "please wait while processing...."

OLDMARIADBSHARED=`rpm -qa | grep MariaDB | awk '/MariaDB-shared/'`
OLDMARIADBCLIENT=`rpm -qa | grep MariaDB | awk '/MariaDB-client/'`
OLDMARIADBDEVEL=`rpm -qa | grep MariaDB | awk '/MariaDB-devel/'`
OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`
OLDMARIADBTEST=`rpm -qa | grep MariaDB | awk '/MariaDB-test/'`
OLDMARIADBCOMMON=`rpm -qa | grep MariaDB | awk '/MariaDB-common/'`
OLDMARIADBCOMPAT=`rpm -qa | grep MariaDB | awk '/MariaDB-compat/'`
OLDGALERA=`rpm -qa | grep galera | awk '/galera/'`

yum -q -y remove perl-DBD-MySQL

echo "rpm -e $OLDMARIADBTEST"
echo "rpm -e $OLDMARIADBSERVER"
echo "rpm -e $OLDMARIADBCLIENT"
echo "rpm -e $OLDMARIADBDEVEL"
echo "rpm -e $OLDGALERA"
echo "rpm -e --nodeps $OLDMARIADBSHARED"
echo "rpm -e --nodeps $OLDMARIADBCOMPAT"
echo "rpm -e --nodeps $OLDMARIADBCOMMON"

rpm -e $OLDMARIADBTEST
rpm -e $OLDMARIADBSERVER
rpm -e $OLDMARIADBCLIENT
rpm -e $OLDMARIADBDEVEL
rpm -e $OLDGALERA
rpm -e --nodeps $OLDMARIADBSHARED
rpm -e --nodeps $OLDMARIADBCOMPAT
rpm -e --nodeps $OLDMARIADBCOMMON

yum clean all

if [ -z "$OLDMARIADBVERS" ]; then
echo "MariaDB old versions successfully removed..."
fi

echo "#####################################################"

}

mariadb104install() {

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.4.x Repo Setup"
    fi

    echo "*************************************************"
    cecho "MariaDB 10.4.x Repo setup..." $boldgreen
   echo "*************************************************"
echo ""

yum clean all

CENTOSVER=`cat /etc/redhat-release | awk '{ print $3 }'`

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

if [ "$CENTOSVER" == 'Enterprise' ]; then
CENTOSVER=`cat /etc/redhat-release | awk '{ print $7 }'`
fi

MARIADB55ARCH=`uname -m`

# Geographic detection and mirror selection moved to mariadb105uninstall()
# to validate the actual repository before package removal

# MariaDB yum mirrors may have connectivity issues for servers in China (CN)
# so can use alternative mirrors for better speed and connectivity
# if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
#     MDB_MIRROR_BASEURL='https://mirror.rackspace.com/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
#     MDB_MIRROR_BASEURL='https://download.nus.edu.sg/mirror/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'GB' ]]; then
#     MDB_MIRROR_BASEURL='https://mirror.rackspace.com/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'HK' ]]; then
#     MDB_MIRROR_BASEURL='https://mirror.rackspace.com/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
#     MDB_MIRROR_BASEURL='https://mirror.rackspace.com/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
#     MDB_MIRROR_BASEURL='http://mirror.aarnet.edu.au/pub/MariaDB/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
#     MDB_MIRROR_BASEURL='https://mirrors.aliyun.com/mariadb/yum'
#     MDB_REPOMD_URL="${MDB_MIRROR_BASEURL}/${MDB_REPO_DIR}/repodata/repomd.xml"
#     # fallback check of repomd.xml
#     curl -s "$MDB_REPOMD_URL" -o repomd.xml
#     MDB_REPOMD_URL_CHECK=$(xmllint --format repomd.xml >/dev/null 2>&1 ; echo $?)
#     rm -f repomd.xml
#     if [[ "$MDB_REPOMD_URL_CHECK" -ne '0' ]]; then
#         MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     fi
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# else
#     MDB_MIRROR_BASEURL='https://yum.mariadb.org'
#     echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
#     rpm --import "${MDB_ARCHIVES_PUBKEY}"
#     # https://mariadb.com/kb/en/gpg/#rpm-source-key-pre-2023
#     echo "rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY"
#     rpm --import https://supplychain.mariadb.com/MariaDB-Server-GPG-KEY
# fi
# mariadb 10.4 deprecated - mirror selection moved to mariadb105uninstall()
# Repository URL and key imports handled after validation passes
echo "rpm --import ${MDB_ARCHIVES_PUBKEY}"
rpm --import "${MDB_ARCHIVES_PUBKEY}"

###############################################################################
if [[ "$CENTOS_NINE" = '9' ]]; then

    if [ "$(uname -m)" == 'aarch64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi
    fi

    if [ "$(uname -m)" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos9-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    fi # centos 9 uname -m

fi

###############################################################################
if [[ "$CENTOS_EIGHT" = '8' ]]; then

    if [ "$(uname -m)" == 'aarch64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi
    fi

    if [ "$(uname -m)" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos8-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    fi # centos 8 uname -m

fi

###############################################################################
if [[ "$CENTOS_SEVEN" = '7' ]]; then

    if [ "$(uname -m)" == 'aarch64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-aarch64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi
    fi

    if [ "$(uname -m)" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos7-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    fi # centos 7 uname -m

fi

###############################################################################
if [[ "$CENTOS_SIX" = '6' ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    else

if [[ "$MDB_COUNTRY" = 'US' || "$MDB_CONTINENT" = 'NA' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'SG' || "$MDB_CONTINENT" = 'AS' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'DE' || "$MDB_CONTINENT" = 'EU' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'AU' || "$MDB_CONTINENT" = 'OC' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
module_hotfixes=1
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
elif [[ "$MDB_COUNTRY" = 'CN' ]]; then
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
else
cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos6-x86
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
EOF
fi

    fi # arch check
fi # centos6 check

if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then

    if [ "$MARIADB55ARCH" == 'x86_64' ]; then

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos5-amd64
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
priority=8
EOF

    else

cat > "/etc/yum.repos.d/mariadb.repo" <<EOF
# rpm --import https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
# /etc/yum.repos.d/mariadb.repo
# yum install MariaDB-server MariaDB-client
# http://downloads.mariadb.org/mariadb/repositories/
[mariadb]
name = MariaDB
baseurl = ${MDB_MIRROR_BASEURL}/yum/centos5-x86
gpgkey=${MDB_ARCHIVES_PUBKEY}
gpgcheck=1
priority=8
EOF

    fi # arch check
fi # centos5 check

}

mariadbtenthreetotenfourupgrade() {
if [ -z "$OLDMARIADBVERS" ]; then
MYSQLDATADIR=$(${ALIAS_MYSQLADMIN} var | grep 'datadir' | awk '{ print $4}')

backupmsg104
#mariadb103uninstall

mariadb_upgrade_check_versionlock

# shutdown existing mysql server
echo
echo "shut down existing MariaDB server for upgrade.."
${ALIAS_MYSQL} -e "set global innodb_buffer_pool_dump_at_shutdown=0;"
sleep 10
echo "${ALIAS_MYSQL} -e \"SET GLOBAL innodb_fast_shutdown = 1;\""
${ALIAS_MYSQL} -e "SET GLOBAL innodb_fast_shutdown = 1;"
sleep 10
systemctl stop mariadb
echo
# move aside existing ib_logfile* files
cd $MYSQLDATADIR
ls -lah $MYSQLDATADIR
rm -rf ib_logfile0.gz ib_logfile1.gz
mv ib_logfile0 ib_logfile0.bak
mv ib_logfile1 ib_logfile1.bak
echo

#if [[ "$CENTOS_TEN" -eq '10' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
#    remove_mariadb_ten_eleven_native_module
if [[ "$CENTOS_EIGHT" -eq '8' && "$(rpm -qa | grep mariadb | awk '/mariadb-server-utils/')" ]]; then
    remove_mariadb_ten_three_native_module
else
    OLDMARIADBSERVER=`rpm -qa | grep MariaDB | awk '/MariaDB-server/'`
    OLDGALERA=`rpm -qa | grep galera | awk '/galera/'`
    rpm -e $OLDMARIADBSERVER
    rpm -e $OLDGALERA
fi

mariadb104install

if [[ "1$CENTALTREPO_DISABLE" = [nN] ]]; then
    DISABLECENTALTREPO=" --disablerepo=CentALT"
else
    DISABLECENTALTREPO=""
fi # CENTALTREPO_DISABLE

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
# make sure MariaDB 5.5 is updated from only one repo
echo ""
if [[ -z "$(grep 'exclude=mariadb' /etc/yum.repos.d/centalt.repo)" ]]; then
echo "exclude=mariadb*" >> /etc/yum.repos.d/centalt.repo
fi
fi # CENTALTREPO_DISABLE

# exclude MariaDB Galera Server
if [[ -z "$(grep 'exclude=MariaDB-Galera-server' /etc/yum.repos.d/mariadb.repo)" ]]; then
echo "exclude=MariaDB-Galera-server" >> /etc/yum.repos.d/mariadb.repo
fi

if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original_before_mariadb104upgrade
fi

echo ""
    echo "*************************************************"
cecho "MariaDB 10.4.x check available packages" $boldgreen
cecho "yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities" $boldgreen
    echo "*************************************************"
echo ""
time yum -q list MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.4.x Install"
    fi

echo ""
    echo "*************************************************"
cecho "MariaDB 10.4.x YUM install..." $boldgreen
cecho "yum -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities" $boldgreen
    echo "*************************************************"
echo ""
time yum -y install MariaDB-client MariaDB-common MariaDB-compat MariaDB-devel MariaDB-server MariaDB-shared --disablerepo=epel --disableplugin=priorities

\cp -af /etc/my.cnf /etc/my.cnf-newold-mariadb10-2
echo ""
    echo "*************************************************"
    cecho "* Adjust old MariaDB 10.x /etc/my.cnf " $boldyellow
    cecho "  for MariaDB 10.4.x my.cnf compatibility " $boldyellow
    echo "*************************************************"

# unalias cp
if [ -f /etc/my.cnf.rpmorig ]; then
\cp -f /etc/my.cnf.rpmsave /etc/my.cnf
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.4.x /etc/my.cnf Adjustments"
    fi

cat >>/etc/my.cnf<<FFF


[mariadb-10.4]
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

# sed -i 's/skip-pbxt/#skip-pbxt/g' /etc/my.cnf
# sed -i 's/innodb_use_purge_thread = 4/innodb_purge_threads=1/g' /etc/my.cnf
# sed -i 's/innodb_extra_rsegments/#innodb_extra_rsegments/g' /etc/my.cnf
# sed -i 's/innodb_adaptive_checkpoint/innodb_adaptive_flushing_method/g' /etc/my.cnf
# sed -i 's/innodb_additional_mem_pool_size/#innodb_additional_mem_pool_size/g' /etc/my.cnf
# sed -i 's/skip-innodb/innodb=ON/g' /etc/my.cnf
sed -i 's|ignore-db-dir|ignore_db_dirs|g' /etc/my.cnf
sed -i 's|^innodb_thread_concurrency|#innodb_thread_concurrency|g' /etc/my.cnf
sed -i 's|^skip-federated|#skip-federated|g' /etc/my.cnf
sed -i 's|^skip-pbxt|#skip-pbxt|g' /etc/my.cnf
sed -i 's|^skip-pbxt_statistics|#skip-pbxt_statistics|g' /etc/my.cnf
sed -i 's|^skip-archive|#skip-archive|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_dump_at_shutdown|#innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
sed -i 's|^innodb_buffer_pool_load_at_startup|#innodb_buffer_pool_load_at_startup|g' /etc/my.cnf
# deprecated in 10.4
# https://mariadb.com/kb/en/library/mariadb-1031-release-notes/
sed -i '/^innodb_large_prefix/d' /etc/my.cnf
sed -i '/^innodb_file_format/d' /etc/my.cnf
sed -i '/^innodb_support_xa/d' /etc/my.cnf

echo ""

cmservice mysql start
sleep 15
cmservice mysql restart

if [[ "$(echo $?)" != '0' ]]; then
    echo
    echo "## !! mysql failed to start properly !! ##"
    echo "journalctl -xn"
    journalctl -xn
    echo
    echo "tail -50 /var/log/mysqld.log"
    tail -50 /var/log/mysqld.log
    echo
    echo "possible errors / warnings"
    tail -50 /var/log/mysqld.log | grep -E -v 'Dumping|dump' | grep -E --color '\[ERROR\]|\[Warning\]'
    echo
    echo "possible unknown or incompatible /etc/my.cnf set options below"
    echo "maybe need to comment out or remove these in /etc/my.cnf"
    UNKNOWNOPT=$(tail -50 /var/log/mysqld.log | grep -E -v 'Dumping|dump' | grep -E --color '\[ERROR\]|\[Warning\]' | grep 'unknown option' | awk '{print $8}' | uniq | sed -e "s|'--||g" -e "s|'||g")
    for u in $UNKNOWNOPT; do
        echo "$u"
    done
    echo
    echo "attempting auto commenting out of invalid options"
    \cp -af /etc/my.cnf /etc/my.cnf-b4autocommenting
    for u in $UNKNOWNOPT; do
        echo "auto comment out $u in /etc/my.cnf"
        sed -i "s|^${u}|#${u}|g" /etc/my.cnf
    done
    cmservice mysql restart
fi
echo
ls -lAhrt /var/lib/mysql/

sleep 3

if [ ! -f /root/.my.cnf ]; then
    echo ""
    echo "------------------------------------------------"
    echo "Updating MySQL database tables"
    echo "------------------------------------------------"
    read -p "Do you have a MySQL root password set ? [y/n]: " mysqlrootset
    echo ""
else
    mysqlrootset=n
fi

if [[ "$mysqlrootset" == [yY] ]]; then
    echo "Enter MySQL root password when prompted"
    echo "or hit enter to continue"
    echo "mysql_upgrade -u root -p --verbose"
    mysql_upgrade -u root -p --verbose
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

    # echo "Enter MySQL root password when prompted"
    # echo "or hit enter to continue"
    # echo "mysql_upgrade -u root -p --force"
    # mysql_upgrade -u root -p --force

    # sed -i 's/innodb=ON/innodb=OFF/g' /etc/my.cnf
else
    echo "mysql_upgrade --verbose"
    mysql_upgrade --verbose
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

 #    echo "mysql_upgrade --force"
    # mysql_upgrade --force

    # sed -i 's/innodb=ON/innodb=OFF/g' /etc/my.cnf
fi

#   echo "------------------------------------------------"
#   echo "Enabling MariaDB 10 XtraDB Engine plugin"
#   echo "------------------------------------------------"
# sed -i 's/#plugin-load=ha_xtradb/plugin-load=ha_xtradb/g' /etc/my.cnf
# sed -i 's/#ignore_builtin_innodb/ignore_builtin_innodb/g' /etc/my.cnf
# sed -i 's/innodb_purge_threads=1/#innodb_purge_threads=1/g' /etc/my.cnf

# sed -i 's/innodb_doublewrite = 1/#innodb_doublewrite = 1/g' /etc/my.cnf
# sed -i 's/innodb_doublewrite=1/#innodb_doublewrite=1/g' /etc/my.cnf

set_mariadb_mysqldump_charset

# mariadb 10.1 systemd changes https://community.centminmod.com/posts/22696/
if [[ "$CENTOS_SEVEN" = '7' ]] ; then
    if [ -f /etc/systemd/system/mariadb.service.d/migrated-from-my.cnf-settings.conf ]; then
        rm -rf /etc/systemd/system/mariadb.service.d/migrated-from-my.cnf-settings.conf
cat > "/etc/systemd/system/mariadb.service.d/mdb101.conf" <<EFF
[Service]
OOMScoreAdjust=-600
LimitNOFILE=524288
LimitNPROC=16384
Nice=-5
EFF
    systemctl daemon-reload
    fi
fi

if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/openfileslimit.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for LimitNOFILE=524288"
cat > "/etc/systemd/system/mariadb.service.d/openfileslimit.conf" <<TDG
[Service]
LimitNOFILE=524288
TDG
  systemctl daemon-reload
fi

if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/notifyaccess.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for NotifyAccess=all"
cat > "/etc/systemd/system/mariadb.service.d/notifyaccess.conf" <<TDG
[Service]
NotifyAccess=all
TDG
  systemctl daemon-reload
fi

if [[ "$CENTOS_TEN" = '10' || "$CENTOS_NINE" = '9' || "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/oomscoreadjust.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    echo "Initial Install: Update MariaDB 10 setting for OOMScoreAdjust=-1000"
cat > "/etc/systemd/system/mariadb.service.d/oomscoreadjust.conf" <<TDG
[Service]
OOMScoreAdjust=-1000
TDG
  systemctl daemon-reload
fi

mysqltmpdir

cmservice mysql restart
remove_anonymous_mysqlusers

echo
systemctl show mariadb --no-pager | grep -E 'LimitNOFILE|NotifyAccess|OOM'
echo
if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] ; then
    cmchkconfig mariadb on
else
    cmchkconfig mysql on
fi

sed -i 's|^#innodb_buffer_pool_dump_at_shutdown|innodb_buffer_pool_dump_at_shutdown|g' /etc/my.cnf
sed -i 's|^#innodb_buffer_pool_load_at_startup|innodb_buffer_pool_load_at_startup|g' /etc/my.cnf

echo ""

perdbiinstall

echo ""

mysqladmin ver

echo

    # if you set /etc/centminmod/custom_config.inc file to
    # MARIADB_UPDATEMAINTENANCE=y
    # then whenever you run mariadb upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$MARIADB_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus on
        fi
    fi

# remove mysqlreload cmd shortcut
rm -rf /usr/bin/mysqlreload

echo "------------------------------------------------"
echo "MariaDB 10.4.x Upgrade completed"
echo "------------------------------------------------"

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "MariaDB 10.4.x Upgraded"
    fi

fi
}