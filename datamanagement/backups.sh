#!/bin/bash
################################################################################
VER=1.5
DT=$(date +"%d%m%y-%H%M%S")
DEBUG_DISPLAY='n'
CHECKSUMS='y'
BACKUP_RETAIN_DAYS='1'
# pigz, zstd or none values
COMPRESS_RSYNCABLE='y'
COMPRESSION_METHOD='zstd'
COMPRESSION_LEVEL_GZIP='4'
COMPRESSION_LEVEL_ZSTD='4'
FASTCOMPRESS_ZSTD='y'

# MySQL settings
BUCKET='mysqlbackup'
DBHOST='localhost'
DBUSER='admin'
MYSQL_PWD='pass'

# file_backup function settings
# don't create tar compressed file as intend to use
# tunnel-transfers.sh script to move directory contents
# via nc/socat zstd tunnel so no need to wait additional
# time tar compressing files
FILES_TARBALL_CREATION='n'
################################################################################
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MOST_FREE_SPACE_MOUNT=$(df --output=target,avail | sed '1d' | sort -k 2 -n -r | awk 'NR==1 {print $1}')
MOST_FREE_SPACE_MOUNT=$(echo "$MOST_FREE_SPACE_MOUNT" | sed 's#^/\{2,3\}#/#')
MOST_FREE_SPACE_MOUNT_HOME=$(df --output=target,avail /home | sed '1d' | sort -k 2 -n -r | awk 'NR==1 {print $1}')
MOST_FREE_SPACE_MOUNT_HOME=$(echo "$MOST_FREE_SPACE_MOUNT_HOME" | sed 's#^/\{2,3\}#/#')

if [[ "$MOST_FREE_SPACE_MOUNT" = "$MOST_FREE_SPACE_MOUNT_HOME" ]]; then
  MOST_FREE_SPACE_MOUNT='/home'
else
  MOST_FREE_SPACE_MOUNT=$MOST_FREE_SPACE_MOUNT
fi

BACKUP_DIR_PARENT="/home/mysqlbackup"
MYSQL_BACKUP_DIR="${BACKUP_DIR_PARENT}/mysql/${DT}"
BACKUP_DIR="${BACKUP_DIR_PARENT}/binlog/${DT}"
MY_CNF='/root/.my.cnf'
MY_CNF_SANDBOX='/root/sandboxes/msb_maria10_3_38/my.sandbox.cnf'
MASTERINFO_LOG_FILE="${MYSQL_BACKUP_DIR}/master_info.log"
LOG_FILE="/var/log/mysql_binlog_backup_${DT}.log"
MYSQL_LOG_FILE="/var/log/mysql_backup_${DT}.log"
MYSQL_OPTS="--default-character-set=utf8mb4 --max_allowed_packet=1024M --net_buffer_length=65536"
MYSQLIMPORT_OPTS=" --default-character-set=utf8mb4"
MYSQLDUMP_OPTS=" --default-character-set=utf8mb4 -Q -K --max_allowed_packet=1024M --net_buffer_length=65536 --routines --events --triggers --hex-blob"

#
BASE_DIR="$MOST_FREE_SPACE_MOUNT/databackup/${DT}"
BACKUP_NAME="$BASE_DIR/centminmod_backup.tar.zst"
DOMAINS_TMP_DIR="$BASE_DIR/domains_tmp"
RSYNC_LOG="$BASE_DIR/domains_tmp/rsync_${DT}.log"
DIRECTORIES_TO_BACKUP=( "/etc/centminmod" "/usr/local/nginx/conf" "/root/tools" "/usr/local/nginx/html" )
DIRECTORIES_TO_BACKUP_NOCOMPRESS=( "/etc/centminmod" "/usr/local/nginx/conf" "/root/tools" "/usr/local/nginx/html" )
MARIADB_TMP_DIR="$BASE_DIR/mariadb_tmp"
MARIABACKUP_LOG="$MARIADB_TMP_DIR/mariabackup_${DT}.log"
BACKUP_LOG_FILENAME="files-backup_${DT}.log"
BACKUP_LOG="$BASE_DIR/$BACKUP_LOG_FILENAME"
BACKUP_LOG_TMP="/tmp/files-backup_${DT}.log"
ERROR_LOG="$BASE_DIR/error_log.log"

# Set the backup directory for cron jobs
CRON_BACKUP_DIR="$BASE_DIR/cronjobs_tmp"
################################################################################
NEWER_TAR='y'
################################################################################
# disk free space management
BUFFER_PERCENT=30
################################################################################
NICE=$(which nice)
NICEOPT='-n 12'
IONICE=$(which ionice)
IONICEOPT='-c2 -n7'
################################################################################
# Amazon s3 support via aws-cli
AWSUPLOAD='n'
AWS_PROFILE='default'
AWS_BUCKETNAME='YOUR_BUCKETNAME'
# set to either STANDARD, STANDARD_IA or REDUCED_REDUNDANCY
STORAGECLASS='STANDARD'
STORAGEOPT=" --storage-class=$STORAGECLASS"
################################################################################
# Backblaze s3 support via aws-cli
BACKBLAZE_UPLOAD='n'
BACKBLAZE_PROFILE='b2'
BACKBLAZE_ENDPOINT=' --endpoint-url=https://s3.us-west-001.backblazeb2.com'
BACKBLAZE_ENDPOINT_LABEL=' --endpoint-url=https://s3.us-west-001.backblazeb2.com'
BACKBLAZE_BUCKETNAME='YOUR_BUCKETNAME'
################################################################################
# DigitalOcean s3 support via aws-cli
DIGITALOCEAN_UPLOAD='n'
DIGITALOCEAN_PROFILE='do'
DIGITALOCEAN_ENDPOINT=' --endpoint-url=https://sfo2.digitaloceanspaces.com'
DIGITALOCEAN_ENDPOINT_LABEL=' --endpoint-url=https://sfo2.digitaloceanspaces.com'
DIGITALOCEAN_BUCKETNAME='YOUR_BUCKETNAME'
######################################################
# Linode s3 support via aws-cli
LINODE_UPLOAD='n'
LINODE_PROFILE='linode'
LINODE_ENDPOINT=' --endpoint-url=https://us-east-1.linodeobjects.com/'
LINODE_ENDPOINT_LABEL=' --endpoint-url=https://us-east-1.linodeobjects.com/'
LINODE_BUCKETNAME='YOUR_BUCKETNAME'
################################################################################
# Cloudflare R2 s3 support via aws-cli
CFR2_UPLOAD='n'
CFR2_PROFILE='r2'
CFR2_ACCOUNTID=''
CFR2_ENDPOINT=" --endpoint-url=https://${CFR2_ACCOUNTID}.r2.cloudflarestorage.com"
CFR2_ENDPOINT_LABEL=" --endpoint-url=https://CFR2_ACCOUNTID.r2.cloudflarestorage.com"
CFR2_BUCKETNAME='YOUR_BUCKETNAME'
################################################################################
# Upcloud s3 support via aws-cli
UPCLOUD_UPLOAD='n'
UPCLOUD_PROFILE='upcloud'
UPCLOUD_ENDPOINT_NAME=''
UPCLOUD_ENDPOINT=" --endpoint-url=https://${YOUR_ENDPOINT_NAME}.us-nyc1.upcloudobjects.com"
UPCLOUD_ENDPOINT_LABEL=" --endpoint-url=https://${YOUR_ENDPOINT_NAME}.us-nyc1.upcloudobjects.com"
UPCLOUD_BUCKETNAME='YOUR_BUCKETNAME'
################################################################################
if [ -f /etc/centminmod/binlog-backups.ini ]; then
    source /etc/centminmod/binlog-backups.ini
fi
if [ -f /etc/centminmod/backups.ini ]; then
    source /etc/centminmod/backups.ini
fi
mkdir -p "$BACKUP_DIR_PARENT" "$MYSQL_BACKUP_DIR" "$BACKUP_DIR" "$CRON_BACKUP_DIR"
chown -R mysql:mysql "$BACKUP_DIR_PARENT" "$MYSQL_BACKUP_DIR" "$BACKUP_DIR"

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
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
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  fi
fi

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

# newer tar 1.35 with zstd native support
if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  if [ ! -f /svr-setup/tar-zstd-gcc10-1.35-1.el7.x86_64.rpm ]; then
    wget https://centminmod.com/centminmodparts/tar/tar-zstd-gcc10-1.35-1.el7.x86_64.rpm -O /svr-setup/tar-zstd-gcc10-1.35-1.el7.x86_64.rpm
    yum -q -y localinstall /svr-setup/tar-zstd-gcc10-1.35-1.el7.x86_64.rpm
  fi
elif [[ "$CENTOS_EIGHT" -eq '8' ]]; then
  if [ ! -f /svr-setup/tar-zstd-gcc12-1.35-1.el8.x86_64.rpm ]; then
    wget https://centminmod.com/centminmodparts/tar/tar-zstd-gcc12-1.35-1.el8.x86_64.rpm -O /svr-setup/tar-zstd-gcc12-1.35-1.el8.x86_64.rpm
    yum -q -y localinstall /svr-setup/tar-zstd-gcc12-1.35-1.el8.x86_64.rpm
  fi
elif [[ "$CENTOS_NINE" -eq '9' ]]; then
  if [ ! -f /svr-setup/tar-zstd-gcc13-1.35-1.el9.x86_64.rpm ]; then
    wget https://centminmod.com/centminmodparts/tar/tar-zstd-gcc13-1.35-1.el9.x86_64.rpm -O /svr-setup/tar-zstd-gcc13-1.35-1.el9.x86_64.rpm
    yum -q -y localinstall /svr-setup/tar-zstd-gcc13-1.35-1.el9.x86_64.rpm
  fi
fi

CPUS=$(nproc)
if [[ "$CPUS" -gt '48' ]]; then
    if [[ "$NEWER_TAR" = [yY] ]]; then
        CPUS_ZSTD=6
    else
        CPUS_ZSTD=$((($CPUS/2)-6))
    fi
    CPUS=$(($CPUS/2))
elif [[ "$CPUS" -ge '24' && "$CPUS" -le '48' ]]; then
    if [[ "$NEWER_TAR" = [yY] ]]; then
        CPUS_ZSTD=4
    else
        CPUS_ZSTD=$((($CPUS/2)-2))
    fi
    CPUS=$(($CPUS/2))
else
    CPUS=$CPUS
    if [[ "$NEWER_TAR" = [yY] ]]; then
        CPUS_ZSTD=$CPUS
    else
        CPUS_ZSTD=$CPUS
    fi
fi

# Function to get MariaDB version
get_mariadb_version() {
    local version=$(mysql -V 2>&1 | awk '{print $5}' | awk -F. '{print $1"."$2}')
    echo $version
}

# Function to set client command variables based on MariaDB version
set_mariadb_client_commands() {
    local version=$(get_mariadb_version)
    
    if (( $(echo "$version <= 10.11" | bc -l) )); then
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

if [[ "$COMPRESS_RSYNCABLE" = [Yy] ]]; then
  COMPRESS_RSYNCABLE_OPT=' --rsyncable'
fi
if [ -f "$MY_CNF" ]; then
  MYSQL_CMD_PREFIX="${ALIAS_MYSQL} --defaults-extra-file=$MY_CNF -h $DBHOST"
  MYSQLBACKUP_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT mariabackup --defaults-extra-file=$MY_CNF -h $DBHOST"
  MYSQLDUMP_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT ${ALIAS_MYSQLDUMP} --defaults-extra-file=$MY_CNF -h $DBHOST${MYSQLDUMP_OPTS}"
  MYSQLBINLOG_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT ${ALIAS_MYSQLBINLOG} --defaults-extra-file=$MY_CNF -h $DBHOST"
  MYSQLADMIN_CMD_PREFIX="${ALIAS_MYSQLADMIN} --defaults-extra-file=$MY_CNF -h $DBHOST"
else
  MYSQL_CMD_PREFIX="${ALIAS_MYSQL} -u $DBUSER -h $DBHOST -p$MYSQL_PWD"
  MYSQLBACKUP_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT mariabackup -u $DBUSER -h $DBHOST -p$MYSQL_PWD"
  MYSQLDUMP_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT mysqldump -u $DBUSER -h $DBHOST -p$MYSQL_PWD${MYSQLDUMP_OPTS}"
  MYSQLBINLOG_CMD_PREFIX="$NICE $NICEOPT $IONICE $IONICEOPT mysqlbinlog -u $DBUSER -h $DBHOST -p$MYSQL_PWD"
  MYSQLADMIN_CMD_PREFIX="${ALIAS_MYSQLADMIN} -u $DBUSER -h $DBHOST -p$MYSQL_PWD"
fi
# dbdeployer
SANDBOX_MYSQL_CMD_PREFIX="${ALIAS_MYSQL} --defaults-extra-file=$${MY_CNF_SANDBOX} -h $DBHOST"
SANDBOX_MYSQLBACKUP_CMD_PREFIX="mariabackup --defaults-extra-file=$${MY_CNF_SANDBOX} -h $DBHOST"
SANDBOX_MYSQLDUMP_CMD_PREFIX="${ALIAS_MYSQLDUMP} --defaults-extra-file=$${MY_CNF_SANDBOX} -h $DBHOST${MYSQLDUMP_OPTS}"
SANDBOX_MYSQLBINLOG_CMD_PREFIX="${ALIAS_MYSQLBINLOG} --defaults-extra-file=$${MY_CNF_SANDBOX} -h $DBHOST"
SANDBOX_MYSQLADMIN_CMD_PREFIX="${ALIAS_MYSQLADMIN} --defaults-extra-file=$${MY_CNF_SANDBOX} -h $DBHOST"

DATADIR=$($MYSQLADMIN_CMD_PREFIX var | grep datadir | awk '{ print $4}')

if [[ "$FASTCOMPRESS_ZSTD" = [Yy] ]]; then
  COMPRESSION_LEVEL_ZSTD_SET=" -T${CPUS_ZSTD} --fast=${COMPRESSION_LEVEL_ZSTD}"
  COMPRESSION_LEVEL_ZSTD_SET_LABEL="-T${CPUS_ZSTD} --fast=${COMPRESSION_LEVEL_ZSTD}"
else
  COMPRESSION_LEVEL_ZSTD_SET=" -T${CPUS_ZSTD} -$COMPRESSION_LEVEL_ZSTD"
  COMPRESSION_LEVEL_ZSTD_SET_LABEL="-T${CPUS_ZSTD} -$COMPRESSION_LEVEL_ZSTD"
fi

if [[ "$AWSUPLOAD" = [yY] ]]; then
  AWS_PROFILE='default'
  S3_LABEL='aws s3'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT=""
  S3_ENDPOINT_OPT_LABEL=""
  BUCKETNAME="$AWS_BUCKETNAME"
  STORAGECLASS="$STORAGECLASS"
  STORAGEOPT="$STORAGEOPT"
elif [[ "$BACKBLAZE_UPLOAD" = [yY] ]]; then
  AWS_PROFILE='b2'
  S3_LABEL='backblaze b2'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT="$BACKBLAZE_ENDPOINT"
  S3_ENDPOINT_OPT_LABEL="$BACKBLAZE_ENDPOINT_LABEL"
  BUCKETNAME="$BACKBLAZE_BUCKETNAME"
  STORAGECLASS=""
  STORAGEOPT=""
elif [[ "$DIGITALOCEAN_UPLOAD" = [yY] ]]; then
  AWS_PROFILE='do'
  S3_LABEL='digitalocean s3'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT="$DIGITALOCEAN_ENDPOINT"
  S3_ENDPOINT_OPT_LABEL="$DIGITALOCEAN_ENDPOINT_LABEL"
  BUCKETNAME="$DIGITALOCEAN_BUCKETNAME"
  STORAGECLASS=""
  STORAGEOPT=""
elif [[ "$LINODE_UPLOAD" = [yY] ]]; then
  AWS_PROFILE='linode'
  S3_LABEL='linode s3'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT="$LINODE_ENDPOINT"
  S3_ENDPOINT_OPT_LABEL="$LINODE_ENDPOINT_LABEL"
  BUCKETNAME="$LINODE_BUCKETNAME"
  STORAGECLASS=""
  STORAGEOPT=""
elif [[ "$CFR2_UPLOAD" = [yY] ]]; then
  AWS_PROFILE='r2'
  S3_LABEL='cloudflare r2'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT="$CFR2_ENDPOINT"
  S3_ENDPOINT_OPT_LABEL="$CFR2_ENDPOINT_LABEL"
  BUCKETNAME="$CFR2_BUCKETNAME"
  STORAGECLASS=""
  STORAGEOPT=""
elif [[ "$UPCLOUD_UPLOAD" = [yY] ]]; then
  AWS_PROFILE='upcloud'
  S3_LABEL='upcloud s3'
  S3_ENABLED='y'
  S3_ENDPOINT_OPT="$UPCLOUD_ENDPOINT"
  S3_ENDPOINT_OPT_LABEL="$UPCLOUD_ENDPOINT_LABEL"
  BUCKETNAME="$UPCLOUD_BUCKETNAME"
  STORAGECLASS=""
  STORAGEOPT=""
fi

# optimize aws cli configuration
# https://docs.aws.amazon.com/cli/latest/topic/s3-config.html#max-concurrent-requests
if [[ "$CFR2_UPLOAD" = [yY] && -f ~/.aws/config && ! "$(grep -w 'max_concurrent_requests' ~/.aws/config)" ]]; then
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.max_concurrent_requests 10
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_threshold 50MB
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_chunksize 50MB
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.addressing_style path
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} region auto
    unset AWS_DEFAULT_REGION
elif [[ "$CFR2_UPLOAD" = [yY] && -f ~/.aws/config && "$(grep -w 'max_concurrent_requests' ~/.aws/config)" ]]; then
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.max_concurrent_requests 10
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_threshold 50MB
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_chunksize 50MB
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.addressing_style path
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} region auto
    unset AWS_DEFAULT_REGION
elif [[ -f ~/.aws/config && ! "$(grep -w 'max_concurrent_requests' ~/.aws/config)" ]]; then
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.max_concurrent_requests ${CPUS}
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.max_queue_size 1000
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_threshold 8MB
    /usr/local/bin/aws configure set --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} s3.multipart_chunksize 8MB
fi

check_command_exists() {
  command -v "$1" >/dev/null 2>&1 || {
    local package_name="$2"
    if [ "$1" = "mariabackup" ]; then
      grep -q "AlmaLinux" /etc/os-release && package_name="mariadb-backup"
      grep -q "CentOS Linux 7" /etc/os-release && package_name="MariaDB-backup"
    fi
    echo "[$(date)] Command '$1' not found. Installing package '$package_name'..."
    yum install -y "$package_name"
  }
}
calculate_uncompressed_size() {
  local DIR="$1"
  local SIZE=$(du -sb "$DIR" | awk '{print $1}')
  echo "$SIZE"
}
human_readable_size() {
  local SIZE_BYTES="$1"
  local SIZE_UNITS=("B" "KB" "MB" "GB" "TB")
  local UNIT_INDEX=0
  while [ $(echo "$SIZE_BYTES >= 1024" | bc) -eq 1 ]; do
    SIZE_BYTES=$(echo "scale=2; $SIZE_BYTES / 1024" | bc)
    UNIT_INDEX=$((UNIT_INDEX + 1))
  done
  echo "${SIZE_BYTES}${SIZE_UNITS[$UNIT_INDEX]}"
}

# Function to check available disk space
check_disk_space() {
    available_space=$(df -k --output=avail "$1" | tail -n 1)
    buffer_space=$(($available_space * $BUFFER_PERCENT / 100))
    effective_space=$(($available_space - $buffer_space))

    if [[ "$effective_space" -lt "$required_space" ]]; then
        echo "Not enough disk space. Required: $required_space KB, Effective: $effective_space KB (after considering $BUFFER_PERCENT% buffer)."
        exit 1
    fi
}

files_backup() {
  files_mode=$1
  tar_comp="$2"
  if [[ -z "$tar_comp" ]]; then
    FILES_TARBALL_CREATION="$FILES_TARBALL_CREATION"
  elif [[ "$tar_comp" = 'comp' ]]; then
    FILES_TARBALL_CREATION='y'
  else
    FILES_TARBALL_CREATION='n'
  fi
  check_command_exists pv pv
  check_command_exists zstd zstd
  check_command_exists rsync rsync
  check_command_exists mariabackup mariadb-backup
  if [[ "$(rsync --help | grep -o zstd)" = 'zstd' ]]; then
    # if newer rsync 3.2.3+ detected with zstd support, use more
    # performant rsync flags for better transfer speeds
    RSYNC_NEW_FLAGS=' --cc xxhash --zc none'
  else
    RSYNC_NEW_FLAGS=""
  fi
  START_TIME=$(date +%s)
  total_uncompressed_size=0
  for dir in "${DIRECTORIES_TO_BACKUP[@]}"; do
    dir_size=$(calculate_uncompressed_size "$dir")
    total_uncompressed_size=$((total_uncompressed_size + dir_size))
  done
  hr_total_size=$(human_readable_size "$total_uncompressed_size")
  # echo "[$(date)] Total uncompressed size of all directories to be backed up: $hr_total_size"
  
  MOST_FREE_SPACE_MOUNT_BYTES=$(df --output=avail -B1 "$MOST_FREE_SPACE_MOUNT" | sed '1d')
  hr_most_free_space_mount=$(human_readable_size "$MOST_FREE_SPACE_MOUNT_BYTES")
  echo "[$(date)] Free space available on $MOST_FREE_SPACE_MOUNT: $hr_most_free_space_mount"
  
  if [ "$total_uncompressed_size" -gt "$MOST_FREE_SPACE_MOUNT_BYTES" ]; then
    echo "[$(date)] Not enough free space on $MOST_FREE_SPACE_MOUNT. Aborting backup."
    exit 1
  fi
  
  if [[ "$files_mode" = 'all' || "$files_mode" = 'filesbackup' ]]; then
    echo "[$(date)] Creating temporary domain data directory ..."
    mkdir -p "$BASE_DIR"
    mkdir -p "$DOMAINS_TMP_DIR"
    echo "[$(date)] Rsync copying domain data (excluding logs) ..."
    export RSYNC_SKIP_COMPRESS="3g2,3gp,3gpp,3mf,7z,aac,ace,amr,apk,appx,appxbundle,arc,arj,asf,avi,br,bz2,cab,crypt5,crypt7,crypt8,deb,dmg,drc,ear,gz,flac,flv,gpg,h264,h265,heif,iso,jar,jp2,jpg,jpeg,lz,lz4,lzma,lzo,m4a,m4p,m4v,mkv,msi,mov,mp3,mp4,mpeg,mpg,mpv,oga,ogg,ogv,opus,pack,png,qt,rar,rpm,rzip,s7z,sfx,svgz,tbz,tgz,tlz,txz,vob,webm,webp,wim,wma,wmv,xz,z,zip,zst"
    for domain_path in /home/nginx/domains/*/; do
      domain=$(basename "$domain_path")
      destination="$DOMAINS_TMP_DIR/$domain"
      mkdir -p "$destination"
      echo "[$(date)] Backup $domain data to $destination"
      if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
        echo "rsync -av${RSYNC_NEW_FLAGS} --whole-file --exclude='logs' \"$domain_path\" \"$destination\"" | tee -a "$RSYNC_LOG"
        rsync -av${RSYNC_NEW_FLAGS} --whole-file --exclude='logs' "$domain_path" "$destination" | tee -a "$RSYNC_LOG"
      else
        echo "rsync -av${RSYNC_NEW_FLAGS} --whole-file --exclude='logs' \"$domain_path\" \"$destination\"" >> "$RSYNC_LOG"
        rsync -av${RSYNC_NEW_FLAGS} --whole-file --exclude='logs' "$domain_path" "$destination" >> "$RSYNC_LOG" 2>&1
      fi
      ERR_RSYNC=$?
      if [[ "$ERR_RSYNC" -eq '0' ]]; then
        echo "[$(date)] Rsync copy $domain completed ok" | tee -a "${destination}/.rsync_backup_${domain}_completed_${DT}.log"
      else
        echo "[$(date)] Rsync copy $domain failed to complete" | tee -a "${destination}/.rsync_backup_${domain}_failed_${DT}.log"
      fi
      DIRECTORIES_TO_BACKUP+=("$destination")
    done
    cat "$RSYNC_LOG" >> "$BACKUP_LOG_TMP"

    echo "[$(date)] Backup cronjobs to ${CRON_BACKUP_DIR}" | tee -a "$BACKUP_LOG_TMP"
    # Backup cron jobs for the root user
    \cp -af /var/spool/cron/root "${CRON_BACKUP_DIR}/root_cronjobs"
    # Backup system-wide cron jobs
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      rsync -av${RSYNC_NEW_FLAGS} --delete /etc/cron.d/ "${CRON_BACKUP_DIR}/system_cronjobs/" | tee -a "$RSYNC_LOG"
    else
      rsync -av${RSYNC_NEW_FLAGS} --delete /etc/cron.d/ "${CRON_BACKUP_DIR}/system_cronjobs/" >> "$RSYNC_LOG" 2>&1
    fi
    DIRECTORIES_TO_BACKUP+=("$CRON_BACKUP_DIR")
    if [ -d /root/.acme.sh ]; then
      DIRECTORIES_TO_BACKUP+=("/root/.acme.sh")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/root/.acme.sh")
    fi
    if [ -f /root/.my.cnf ]; then
      DIRECTORIES_TO_BACKUP+=("/root/.my.cnf")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/root/.my.cnf")
    fi
    if [ -f /etc/pure-ftpd/pureftpd.passwd ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/pure-ftpd/pureftpd.passwd")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/pure-ftpd/pureftpd.passwd")
    fi
    if [ -f /etc/pure-ftpd/pureftpd.pdb ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/pure-ftpd/pureftpd.pdb")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/pure-ftpd/pureftpd.pdb")
    fi
    if [ -f /etc/redis/redis.conf ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/redis/redis.conf")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/redis/redis.conf")
    fi
    if [ -f /etc/redis/sentinel.conf ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/redis/sentinel.conf")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/redis/sentinel.conf")
    fi
    if [ -f /etc/keydb/keydb.conf ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/keydb/keydb.conf")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/keydb/keydb.conf")
    fi
    if [ -f /etc/keydb/sentinel.conf ]; then
      DIRECTORIES_TO_BACKUP+=("/etc/keydb/sentinel.conf")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/keydb/sentinel.conf")
    fi
    if [ -d /etc/elasticsearch ]; then
      \cp -af /etc/elasticsearch /etc/elasticsearch-source
      DIRECTORIES_TO_BACKUP+=("/etc/elasticsearch-source")
      DIRECTORIES_TO_BACKUP_NOCOMPRESS+=("/etc/elasticsearch-source")
    fi
  fi
  
  if [[ "$files_mode" = 'all' || "$files_mode" = 'mariabackup' ]]; then
    DIRECTORIES_TO_BACKUP+=("$MARIADB_TMP_DIR")
    echo "[$(date)] Creating temporary MariaDB data directory ..."
    mkdir -p "$MARIADB_TMP_DIR"
    echo "[$(date)] Performing MariaBackup To $MARIADB_TMP_DIR ..."
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo "$MYSQLBACKUP_CMD_PREFIX --backup --target-dir=$MARIADB_TMP_DIR"
      $MYSQLBACKUP_CMD_PREFIX --backup --target-dir="$MARIADB_TMP_DIR" 2>&1 | tee "$MARIABACKUP_LOG"
      mariabackup_exit_status=${PIPESTATUS[2]}
    else
      $MYSQLBACKUP_CMD_PREFIX --backup --target-dir="$MARIADB_TMP_DIR" > "$MARIABACKUP_LOG" 2>&1
      mariabackup_exit_status=${PIPESTATUS[2]}
    fi
    echo "[$(date)] Preparing MariaBackup At $MARIADB_TMP_DIR ..."
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo "$MYSQLBACKUP_CMD_PREFIX --prepare --target-dir=$MARIADB_TMP_DIR"
      $MYSQLBACKUP_CMD_PREFIX --prepare --target-dir="$MARIADB_TMP_DIR" 2>&1 | tee -a "$MARIABACKUP_LOG"
      mariabackup_exit_status=${PIPESTATUS[2]}
    else
      $MYSQLBACKUP_CMD_PREFIX --prepare --target-dir="$MARIADB_TMP_DIR" >> "$MARIABACKUP_LOG" 2>&1
      mariabackup_exit_status=${PIPESTATUS[2]}
    fi
    if [ -f "$SCRIPT_DIR/mariabackup-restore.sh" ]; then
      cp -a "$SCRIPT_DIR/mariabackup-restore.sh" "$MARIADB_TMP_DIR/mariabackup-restore.sh"
      chmod +x "$MARIADB_TMP_DIR/mariabackup-restore.sh"
    fi
    cat "$MARIABACKUP_LOG" >> "$BACKUP_LOG_TMP"
    echo "[$(date)] $MARIADB_TMP_DIR/mariabackup-restore.sh saved"
    echo "[$(date)] MariaBackup log saved at $MARIABACKUP_LOG"
  fi

    # instructions for backup restoration
cat > "$BASE_DIR/restore-instructions.txt" <<EOF
# https://github.com/centminmod/centminmod/blob/130.00beta01/datamanagement/centmin.sh-menu-21.readme.md

To restore the data from the backup, follow these steps:

1. Transfer the backup file to the server where you want to restore the data from. Below instructions restore to staging directory at /home/restoredata
2. Extract the contents of the backup file

If you have tar version 1.31 or higher, it has native zstd compression support, and extract the backup using these 2 commands. Centmin Mod 130.00beta01's centmin.sh menu option 21, will automatically install a custom built tar 1.35 version YUM RPM binary at /usr/local/bin/tar to not conflict with system installed /usr/bin/tar and the custom tar 1.35 binary will take priority over system tar if called just as tar.

Change path to /home/databackup/${DT}/centminmod_backup.tar.zst where you saved or transfered the backup to i.e. /home/remotebackup/centminmod_backup.tar.zst.

   mkdir -p /home/restoredata
   tar -I zstd -xf /home/databackup/${DT}/centminmod_backup.tar.zst -C /home/restoredata

or

   mkdir -p /home/restoredata
   tar -I zstd -xf /home/remotebackup/centminmod_backup.tar.zst -C /home/restoredata

If you have tar version lower than 1.31, you will have to extract the tar zstd compressed backup first.

   mkdir -p /home/restoredata
   zstd -d /home/databackup/${DT}/centminmod_backup.tar.zst
   tar -xf /home/databackup/${DT}/centminmod_backup.tar -C /home/restoredata

or

   mkdir -p /home/restoredata
   zstd -d /home/remotebackup/centminmod_backup.tar.zst
   tar -xf /home/remotebackup/centminmod_backup.tar -C /home/restoredata

Custom tar 1.35

tar --version
tar (GNU tar) 1.35
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by John Gilmore and Jay Fenlason.

3. Follow the instructions in the mariabackup-restore.sh script located in the extracted backup directory (e.g., ${BASE_DIR}/mariadb_tmp/mariabackup-restore.sh or /home/restoredata/${BASE_DIR}/mariadb_tmp/mariabackup-restore.sh) to restore the MariaDB MySQL databases.

When you extract the backup centminmod_backup.tar.zst file to /home/restoredata, you'll find backup directories and files which correspond with the relative directory paths to root / for /etc, /home, /root and /usr respectively.

Where:

* /home/restoredata/etc/centminmod is the backup data for /etc/centminmod

* /home/restoredata/etc/pure-ftpd is for /etc/pure-ftpd virtual FTP user database files

* /home/restoredata${BASE_DIR}/domains_tmp is the backup data for /home/nginx/domains for Nginx vhost directories

* /home/restoredata${BASE_DIR}/mariadb_tmp is the backup data for /var/lib/mysql MySQL data directory which also contains the MariaBackup MySQL data restore script at /home/restoredata${BASE_DIR}/mariadb_tmp/mariabackup-restore.sh. Provided you chose to backup MariaDB MySQL data.

* /home/restoredata/root/tools is the backup data for /root/tools

* /home/restoredata/usr/local/nginx/ is the backup data for /usr/local/nginx

Then proceed to move the restored files to the correct locations. You can first use diff command to check backup versus destination directory files. Not all directories may exist as it's dependent on whether you have installed the software i.e. Redis and KeyDB.

diff -ur /etc/centminmod /home/restoredata/etc/centminmod/
diff -ur /etc/pure-ftpd /home/restoredata/etc/pure-ftpd
diff -ur /etc/redis /home/restoredata/etc/redis
diff -ur /etc/keydb /home/restoredata/etc/keydb
diff -ur /root/.acme.sh /home/restoredata/root/.acme.sh/
diff -ur /root/tools /home/restoredata/root/tools/
diff -ur /usr/local/nginx /home/restoredata/usr/local/nginx/
diff -ur /root/.my.cnf /home/restoredata/root/.my.cnf
diff -ur /var/spool/cron/root /home/remotebackup/cronjobs_tmp/root_cronjobs

If Elasticsearch is installed on both old and new server, centmin.sh menu option 21 backup script will backup /etc/elasticsearch as a copy located at /etc/elasticsearch-source so that restoration doesn't override, new server Elasticsearch instance. But you'd have /home/restoredata/etc/elasticsearch-source to reference old server's Elasticsearch settings.

diff -ur /etc/elasticsearch /home/restoredata/etc/elasticsearch-source

Example where /etc/centminmod/diff.txt file exists only on destination side

diff -ur /home/restoredata/etc/centminmod/ /etc/centminmod
Only in /etc/centminmod: diff.txt

Then copy command will force override any existing files on destination directory side and ensure to backup new destination server's files for future reference for /etc/centminmod/custom_config.inc and /etc/centminmod/php.d/a_customphp.ini and /etc/my.cnf and /etc/centminmod/php.d/zendopcache.ini and /usr/local/nginx and /usr/local/nginx/conf/staticfiles.conf files/directory as you may want to use the new server's version of these files or directories for server settings instead of using old server's transferred settings.

\cp -af /usr/local/nginx/conf/staticfiles.conf /usr/local/nginx/conf/staticfiles.conf.original
\cp -af /usr/local/nginx /usr/local/nginx_original
\cp -af /etc/my.cnf /etc/my.cnf.original
\cp -af /root/.my.cnf /root/.my.cnf.original
\cp -af /etc/redis/redis.conf /etc/redis/redis.conf.original
\cp -af /etc/keydb/keydb.conf /etc/keydb/keydb.conf.original
\cp -af /etc/centminmod/custom_config.inc /etc/centminmod/custom_config.inc.original
\cp -af /etc/centminmod/php.d/a_customphp.ini /etc/centminmod/php.d/a_customphp.ini.original
\cp -af /etc/centminmod/php.d/zendopcache.ini /etc/centminmod/php.d/zendopcache.ini.original
\cp -af /home/restoredata/etc/centminmod/* /etc/centminmod/
\cp -af /home/restoredata/etc/pure-ftpd/* /etc/pure-ftpd/
\cp -af /home/restoredata/etc/redis/* /etc/redis/
\cp -af /home/restoredata/etc/keydb/* /etc/keydb/
mkdir -p /root/.acme.sh
\cp -af /home/restoredata/root/.acme.sh/* /root/.acme.sh/
\cp -af /home/restoredata/root/tools/* /root/tools/
\cp -af /home/restoredata/usr/local/nginx/* /usr/local/nginx/

For Nginx vhost data where backup directory timestamp = ${DT}

\cp -af /home/restoredata/home/databackup/${DT}/domains_tmp/* /home/nginx/domains/

Or if disk space is a concern, instead of copy command use move commands

\cp -af /usr/local/nginx/conf/staticfiles.conf /usr/local/nginx/conf/staticfiles.conf.original
\cp -af /usr/local/nginx /usr/local/nginx_original
\cp -af /etc/my.cnf /etc/my.cnf.original
\cp -af /etc/centminmod/custom_config.inc /etc/centminmod/custom_config.inc.original
\cp -af /etc/centminmod/php.d/a_customphp.ini /etc/centminmod/php.d/a_customphp.ini.original
\cp -af /etc/centminmod/php.d/zendopcache.ini /etc/centminmod/php.d/zendopcache.ini.original
mv -f /home/restoredata/etc/centminmod/* /etc/centminmod/
mv -f /home/restoredata/etc/pure-ftpd/* /etc/pure-ftpd/
mv -f /home/restoredata/etc/redis/* /etc/redis/
mv -f /home/restoredata/etc/keydb/* /etc/keydb/
mkdir -p /root/.acme.sh
mv -f /home/restoredata/root/.acme.sh/* /root/.acme.sh/
mv -f /home/restoredata/root/tools/* /root/tools/
mv -f /home/restoredata/usr/local/nginx/* /usr/local/nginx/

For Nginx vhost data where backup directory timestamp = ${DT}

mv -f /home/restoredata/home/databackup/${DT}/domains_tmp/* /home/nginx/domains/

Check overwritten files

diff -ur /etc/centminmod/custom_config.inc.original /etc/centminmod/custom_config.inc
diff -ur /usr/local/nginx_original/conf/conf.d/virtual.conf /usr/local/nginx/conf/conf.d/virtual.conf
diff -ur /usr/local/nginx_original/conf/nginx.conf /usr/local/nginx/conf/nginx.conf
diff -ur /etc/redis/redis.conf.original /etc/redis/redis.conf
diff -ur /etc/keydb/keydb.conf.original /etc/keydb/keydb.conf

If no changes to virtual.conf and nginx.conf use new server one

\cp -af /usr/local/nginx_original/conf/nginx.conf /usr/local/nginx/conf/nginx.conf
\cp -af /usr/local/nginx_original/conf/conf.d/virtual.conf /usr/local/nginx/conf/conf.d/virtual.conf
diff -ur /usr/local/nginx_original/conf/nginx.conf /usr/local/nginx/conf/nginx.conf
diff -ur /usr/local/nginx_original/conf/conf.d/virtual.conf /usr/local/nginx/conf/conf.d/virtual.conf

\cp -af /etc/redis/redis.conf.original /etc/redis/redis.conf
\cp -af /etc/keydb/keydb.conf.original /etc/keydb/keydb.conf
diff -ur /etc/redis/redis.conf.original /etc/redis/redis.conf
diff -ur /etc/keydb/keydb.conf.original /etc/keydb/keydb.conf

Restore cronjobs

crontab -l > /etc/centminmod/cronjobs/cronjoblist-restore-from-migration.txt
cat /etc/centminmod/cronjobs/cronjoblist-restore-from-migration.txt
crontab /home/remotebackup/cronjobs_tmp/root_cronjobs

The /home/restoredata${BASE_DIR}/mariadb_tmp/mariabackup-restore.sh script has 2 options to restore MariaDB MySQL data either via copy-back or move-back. 

1. copy-back: This option copies the backup files back to the original data directory at /var/lib/mysql. The backup files themselves are not altered or removed. The script checks if the provided backup directory is valid and if the backup and current MariaDB versions match. If everything is fine, it proceeds with copying the backup files back to the original data directory at /var/lib/mysql.
2. move-back: This option moves the backup files back to the original data directory at /var/lib/mysql. Unlike copy-back, the backup files are removed from the backup directory. The script checks if the provided backup directory is valid and if the backup and current MariaDB versions match. If everything is fine, it proceeds with moving the backup files back to the original data directory at /var/lib/mysql.

Both options involve the following steps:

* The script first checks if the provided directory contains valid MariaBackup data.
* It then compares the MariaDB version used for the backup with the version running on the current system. The script aborts the restore process if the versions do not match.
* The MariaDB server is stopped, and the existing data directory is backed up to /var/lib/mysql-copy-datetimestamp and then /var/lib/mysql data directory is emptied.
* The ownership of the data directory is changed to mysql:mysql.
* The MariaDB server is started.
* Depending on the option chosen (copy-back or move-back), the script copies or moves the backup files back to the original data directory.

mariabackup-restore.sh Usage help output:

./mariabackup-restore.sh
Usage: ./mariabackup-restore.sh [copy-back|move-back] /path/to/backup/dir/

Actual command where backup directory timestamp = ${DT}

time /home/restoredata/home/databackup/${DT}/mariadb_tmp/mariabackup-restore.sh copy-back /home/restoredata/home/databackup/${DT}/mariadb_tmp/

Then restore /root/.my.cnf

\cp -af /home/restoredata/root/.my.cnf /root/.my.cnf

**Note:** Make sure to adjust the paths in the commands above to match the actual location of your backup files.
EOF
  
  if [[ "$FILES_TARBALL_CREATION" = [yY] ]]; then
    # echo "[$(date)] Total uncompressed size of all directories to be backed up: $hr_total_size"
    echo "[$(date)] Creating backup tarball using zstd compression ($COMPRESSION_LEVEL_ZSTD_SET_LABEL)"
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo "[$(date)] ls -lAh ${DIRECTORIES_TO_BACKUP[@]}"
      ls -lAh ${DIRECTORIES_TO_BACKUP[@]}
      echo
      echo "[$(date)] tar --use-compress-program=\"zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT}\" -cvpf \"$BACKUP_NAME\" ${DIRECTORIES_TO_BACKUP[@]} | tee \"$BACKUP_LOG_TMP\""
      tar --use-compress-program="zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT}" -cvpf "$BACKUP_NAME" ${DIRECTORIES_TO_BACKUP[@]} | tee "$BACKUP_LOG_TMP"
    else
      tar --use-compress-program="zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT}" -cvpf "$BACKUP_NAME" ${DIRECTORIES_TO_BACKUP[@]} >> "$BACKUP_LOG_TMP" 2>&1
    fi
    echo "[$(date)] Cleaning up temporary directories ..."
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo "rm -rf $DOMAINS_TMP_DIR"
      rm -rf "$DOMAINS_TMP_DIR"
      echo "rm -rf $MARIADB_TMP_DIR"
      rm -rf "$MARIADB_TMP_DIR"
    else
      rm -rf "$DOMAINS_TMP_DIR"
      rm -rf "$MARIADB_TMP_DIR"
    fi
    echo "[$(date)] Backup completed. File: $BACKUP_NAME"
    if [ -f "$BACKUP_LOG_TMP" ]; then
      mv -f "$BACKUP_LOG_TMP" "$BACKUP_LOG"
      BACKUP_LOG_FINAL=$BACKUP_LOG
    fi
  else
    if [[ "$files_mode" = 'all' ]]; then
      # non-vhost files backup that non-tar compressed miss
      for dir in "${DIRECTORIES_TO_BACKUP_NOCOMPRESS[@]}"; do
        echo "[$(date)] rsync non-vhost files in $dir to backup location $BASE_DIR"
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          mkdir -p "${BASE_DIR}${dir}"
          rsync -av${RSYNC_NEW_FLAGS} --delete $dir/ "${BASE_DIR}${dir}/" | tee -a "$RSYNC_LOG"
        else
          mkdir -p "${BASE_DIR}${dir}"
          rsync -av${RSYNC_NEW_FLAGS} --delete $dir/ "${BASE_DIR}${dir}/" >> "$RSYNC_LOG" 2>&1
        fi
      done
      echo
      echo "[$(date)] Backup completed at $BASE_DIR"
      if [ -f "$BACKUP_LOG_TMP" ]; then
        mv -f "$BACKUP_LOG_TMP" "$BACKUP_LOG"
        BACKUP_LOG_FINAL=$BACKUP_LOG
        BACKUP_DIR_FINAL=$BASE_DIR
      fi
    elif [[ "$files_mode" = 'filesbackup' ]]; then
      # non-vhost files backup that non-tar compressed miss
      for dir in "${DIRECTORIES_TO_BACKUP_NOCOMPRESS[@]}"; do
        echo "[$(date)] rsync non-vhost files in $dir to backup location $BASE_DIR"
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          mkdir -p "${BASE_DIR}${dir}"
          rsync -av${RSYNC_NEW_FLAGS} --delete $dir/ "${BASE_DIR}${dir}/" | tee -a "$RSYNC_LOG"
        else
          mkdir -p "${BASE_DIR}${dir}"
          rsync -av${RSYNC_NEW_FLAGS} --delete $dir/ "${BASE_DIR}${dir}/" >> "$RSYNC_LOG" 2>&1
        fi
      done
      echo
      echo "[$(date)] Backup completed at $DOMAINS_TMP_DIR"
      if [ -f "$BACKUP_LOG_TMP" ]; then
        mv -f "$BACKUP_LOG_TMP" "$DOMAINS_TMP_DIR"
        BACKUP_LOG_FINAL="$DOMAINS_TMP_DIR/$BACKUP_LOG_FILENAME"
        BACKUP_DIR_FINAL=$BASE_DIR
      fi
    elif [[ "$files_mode" = 'mariabackup' ]]; then
      echo "[$(date)] Backup completed at $MARIADB_TMP_DIR"
      if [ -f "$BACKUP_LOG_TMP" ]; then
        mv -f "$BACKUP_LOG_TMP" "$MARIADB_TMP_DIR"
        BACKUP_LOG_FINAL="$MARIADB_TMP_DIR/$BACKUP_LOG_FILENAME"
        BACKUP_DIR_FINAL=$BASE_DIR
      fi
    else
      echo "[$(date)] Backup completed at $BASE_DIR"
      if [ -f "$BACKUP_LOG_TMP" ]; then
        mv -f "$BACKUP_LOG_TMP" "$BACKUP_LOG"
        BACKUP_LOG_FINAL=$BACKUP_LOG
        BACKUP_DIR_FINAL=$BASE_DIR
      fi
    fi
  fi

  # Sync with AWS S3
  if [[ "$AWSUPLOAD" = [yY] || "$BACKBLAZE_UPLOAD" = [yY] || "$DIGITALOCEAN_UPLOAD" = [yY] || "$LINODE_UPLOAD" = [yY] || "$CFR2_UPLOAD" = [yY] || "$UPCLOUD_UPLOAD" = [yY] ]]; then
    echo -e "\nTransfer backups to S3 storage to bucket: ${S3_LABEL}"
    echo "aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT_LABEL} \"$BACKUP_DIR_FINAL\" \"s3://$BUCKET/mysql/$DT\""
    aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} "$BACKUP_DIR_FINAL" "s3://$BUCKET/mysql/$DT" | tee -a "$BACKUP_LOG_FINAL"
  fi 
  if [ $? -ne 0 ]; then
    echo "[$(date)] Error syncing with S3" | tee -a "$BACKUP_LOG_FINAL"
    error_flag=1
  else
    if [[ "$AWSUPLOAD" = [yY] || "$BACKBLAZE_UPLOAD" = [yY] || "$DIGITALOCEAN_UPLOAD" = [yY] || "$LINODE_UPLOAD" = [yY] || "$CFR2_UPLOAD" = [yY] || "$UPCLOUD_UPLOAD" = [yY] ]]; then
      echo "[$(date)] S3 transfer completed to bucket: ${S3_LABEL}" | tee -a "$BACKUP_LOG_FINAL"
    fi
  fi

  echo "[$(date)] Backup Log saved: $BACKUP_LOG_FINAL"
  END_TIME=$(date +%s)
  ELAPSED_TIME=$((END_TIME - START_TIME))
  echo "[$(date)] Script execution time: $ELAPSED_TIME seconds"
}

mysql_backup() {
  mode=$1
  tar_comp="$2"
  # Check disk space before backup
  check_disk_space "$MYSQL_BACKUP_DIR"
  chown mysql:mysql "$MYSQL_BACKUP_DIR"
  # Check if binary logging is enabled
  binary_logging=$($MYSQL_CMD_PREFIX -e "SHOW VARIABLES WHERE Variable_name = 'log_bin';" | grep -i "ON")
  if [ -z "$binary_logging" ]; then
    error_flag=0
    $MYSQLDUMP_CMD_PREFIX --single-transaction --flush-logs --databases mysql > "${MYSQL_BACKUP_DIR}/master_data.sql" || error_flag=1
  else
    error_flag=0
    $MYSQLDUMP_CMD_PREFIX --master-data=2 --single-transaction --flush-logs --databases mysql > "${MYSQL_BACKUP_DIR}/master_data.sql" || error_flag=1
  fi
  databases=$($MYSQL_CMD_PREFIX -e "SHOW DATABASES;" | grep -Ev "(Database|information_schema|performance_schema|_restorecopy_)")
  for db in $databases; do
    # Check disk space during backup
    check_disk_space "$MYSQL_BACKUP_DIR"
    db_fs_name=$(echo "$db" | sed -e "s|-|@002d|g"); mkdir -p "${MYSQL_BACKUP_DIR}/${db_fs_name}"; chown -R mysql:mysql "${MYSQL_BACKUP_DIR}/${db_fs_name}"; db_disksize=$(du -s ${DATADIR}${db_fs_name} | awk '{print $1/1024}')
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo -e "\n[$(date)] backup database: ${db} (${db_disksize} MB)" | tee -a "$MYSQL_LOG_FILE"
      echo -e "\n[$(date)] $MYSQLDUMP_CMD_PREFIX -d $db > ${MYSQL_BACKUP_DIR}/${db_fs_name}/${db}-schema-only.sql"
      echo -e "\n[$(date)] $MYSQLDUMP_CMD_PREFIX $db --tab=${MYSQL_BACKUP_DIR}/${db_fs_name}" | tee -a "$MYSQL_LOG_FILE"
    else
      echo -e "[$(date)] backup database: ${db} (${db_disksize} MB)" | tee -a "$MYSQL_LOG_FILE"
    fi
    # create a full schema only database sql for foreign key based tables proper restoration
    $MYSQLDUMP_CMD_PREFIX -d $db > "${MYSQL_BACKUP_DIR}/${db_fs_name}/${db}-schema-only.sql"
    if [[ "$CHECKSUMS" = [yY] ]]; then
      # Generate and store the checksum for the schema-only .sql file
      sha256sum "${MYSQL_BACKUP_DIR}/${db_fs_name}/${db}-schema-only.sql" > "${MYSQL_BACKUP_DIR}/${db_fs_name}/${db}-schema-only.sql.sha256"
    fi
    rm -f time_output.txt mysqldump_output.txt
    # Run the mysqldump command and measure its execution time
    { /usr/bin/time --format='real: %es user: %Us sys: %Ss cpu: %P maxmem: %M KB cswaits: %w' $MYSQLDUMP_CMD_PREFIX $db --tab="${MYSQL_BACKUP_DIR}/${db_fs_name}"; } > mysqldump_output.txt 2> time_output.txt
    backup_err=${PIPESTATUS[3]}
    echo "[$(date)] backup time for $db: $(cat time_output.txt| awk '{print $2}')"
    # Append the time output and mysqldump output to the log file
    cat time_output.txt mysqldump_output.txt >> "$MYSQL_LOG_FILE"
    rm -f time_output.txt mysqldump_output.txt
    if [[ "$backup_err" -ne 0 ]]; then
      echo "[$(date)] Error backing up database: $db" | tee -a "$MYSQL_LOG_FILE"; error_flag=1; continue
    fi
    txt_files="$(find ${MYSQL_BACKUP_DIR}/${db_fs_name}/ -type f -name "*.txt")"
    if [[ "$CHECKSUMS" = [yY] ]]; then
      # Generate and store the checksums for the .txt files
      for txt_file in $txt_files; do
        sha256sum "$txt_file" > "${txt_file}.sha256"
      done
    fi
    for txt_file in $txt_files; do
      if [ "$COMPRESSION_METHOD" == "pigz" ]; then
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          echo "[$(date)] Compress $txt_file with pigz" | tee -a "$MYSQL_LOG_FILE"
          echo "pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} \"$txt_file\"" >> "$MYSQL_LOG_FILE"
        fi
        echo "pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} \"$txt_file\"" >> "$MYSQL_LOG_FILE"
        pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} "$txt_file"
      elif [ "$COMPRESSION_METHOD" == "zstd" ]; then
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          echo "[$(date)] Compress $txt_file with zstd" | tee -a "$MYSQL_LOG_FILE"
          echo "zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o \"${txt_file}.zst\" \"$txt_file\"" >> "$MYSQL_LOG_FILE"
        fi
        echo "zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o \"${txt_file}.zst\" \"$txt_file\"" >> "$MYSQL_LOG_FILE"
        zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o "${txt_file}.zst" "$txt_file"
      fi
    done
  done

  if [ -z "$binary_logging" ]; then
    master_info=""
  else
    master_info=$(grep -m 1 -oP "(?<=-- CHANGE MASTER TO MASTER_LOG_FILE=')[^']+(?=')|(?<=MASTER_LOG_POS=)[0-9]+" "${MYSQL_BACKUP_DIR}/master_data.sql" | paste -sd ',' -)
  fi
  restore_script="${MYSQL_BACKUP_DIR}/restore.sh"; echo -e "#!/bin/bash\nset -e\ncpu=\$(nproc)\n\nscript_dir=\$(dirname \"\$(realpath \"\$0\")\")" > "$restore_script"
  echo "case \"\$1\" in" >> "$restore_script"
  echo "  all)" >> "$restore_script"
  for db in $databases; do
    db_fs_name=$(echo "$db" | sed -e "s|-|@002d|g")
    echo -e "    echo \"Checking if database exists: $db\"\n    db_exists=\$($MYSQL_CMD_PREFIX -BNe \"SHOW DATABASES LIKE '$db';\" | grep -w \"$db\")\n    if [ ! -z \"\$db_exists\" ]; then\n      DT=\$(date +\"%Y%m%d%H%M%S\")\n      origin_db_name=\"${db}\"\n      new_db_name=\"${db}_restorecopy_\$DT\"\n      echo \"Database already exists. Restoring to a new database: \$new_db_name\"\n    else\n      origin_db_name=\"${db}\"\n      new_db_name=\"$db\"\n    fi\n    $MYSQL_CMD_PREFIX -e \"CREATE DATABASE IF NOT EXISTS \$new_db_name;\"\n    (echo \"SET FOREIGN_KEY_CHECKS=0;\"; cat \"\$script_dir\"/${db_fs_name}/\${origin_db_name}-schema-only.sql; echo \"SET FOREIGN_KEY_CHECKS=1;\") | $MYSQL_CMD_PREFIX \$new_db_name;\n    find \"\$script_dir\"/${db_fs_name} -iname \"*.txt.zst\" -o -iname \"*.txt.gz\" | while read -r file; do ext=\"\${file##*.}\"; orig_file=\"\${file%.*}\"; if [ \"\$ext\" = \"zst\" ]; then zstd -cd \"\$file\" > \"\$orig_file\"; else gzip -cd \"\$file\" > \"\$orig_file\"; fi; done\n    mysqlimport --default-character-set=utf8mb4 --ignore-foreign-keys --use-threads=\$cpu${MYSQLIMPORT_OPTS} \$new_db_name \"\$script_dir\"/${db_fs_name}/*.txt\n    mysqlimport_err=\$?\n    if [ \$mysqlimport_err -eq 0 ]; then\n      rm -f \"\$script_dir\"/${db_fs_name}/*.zst \"\$script_dir\"/${db_fs_name}/*.gz\n    else\n      echo \"Error: mysqlimport exited with status \$mysqlimport_err\"\n      exit \$mysqlimport_err\n    fi\n" >> "$restore_script"
  done
  echo "    ;;" >> "$restore_script"
  for db in $databases; do
    db_fs_name=$(echo "$db" | sed -e "s|-|@002d|g")
    echo "  $db)" >> "$restore_script"
    echo -e "    echo \"Checking if database exists: $db\"\n    db_exists=\$($MYSQL_CMD_PREFIX -BNe \"SHOW DATABASES LIKE '$db';\" | grep -w \"$db\")\n    if [ ! -z \"\$db_exists\" ]; then\n      DT=\$(date +\"%Y%m%d%H%M%S\")\n      origin_db_name=\"${db}\"\n      new_db_name=\"${db}_restorecopy_\$DT\"\n      echo \"Database already exists. Restoring to a new database: \$new_db_name\"\n    else\n      origin_db_name=\"${db}\"\n      new_db_name=\"$db\"\n    fi\n    $MYSQL_CMD_PREFIX -e \"CREATE DATABASE IF NOT EXISTS \$new_db_name;\"\n    (echo \"SET FOREIGN_KEY_CHECKS=0;\" ; cat \"\$script_dir\"/${db_fs_name}/\${origin_db_name}-schema-only.sql ; echo \"SET FOREIGN_KEY_CHECKS=1;\") | $MYSQL_CMD_PREFIX \$new_db_name;\n    find \"\$script_dir\"/${db_fs_name} -iname \"*.txt.zst\" -o -iname \"*.txt.gz\" | while read -r file; do ext=\"\${file##*.}\"; orig_file=\"\${file%.*}\"; if [ \"\$ext\" = \"zst\" ]; then zstd -cd \"\$file\" > \"\$orig_file\"; else gzip -cd \"\$file\" > \"\$orig_file\"; fi; done\n    mysqlimport --default-character-set=utf8mb4 --ignore-foreign-keys --use-threads=\$cpu${MYSQLIMPORT_OPTS} \$new_db_name \"\$script_dir\"/${db_fs_name}/*.txt\n    mysqlimport_err=\$?\n    if [ \$mysqlimport_err -eq 0 ]; then\n      rm -f \"\$script_dir\"/${db_fs_name}/*.zst \"\$script_dir\"/${db_fs_name}/*.gz\n    else\n      echo \"Error: mysqlimport exited with status \$mysqlimport_err\"\n      exit \$mysqlimport_err\n    fi\n    ;;" >> "$restore_script"
  done
  db_list=$(echo "$databases" | sed -e "s|^|\$0 |" | tr '\n' '\\n')
  echo "  *)" >> "$restore_script"
  echo "    echo -e \"Usage:\\n\\n\$0 all\"" >> "$restore_script"
  for db in $databases; do
      echo "    echo \"$restore_script $db\"" >> "$restore_script"
  done
  echo "    ;;" >> "$restore_script"
  echo "esac" >> "$restore_script"
  chmod +x "$restore_script"
  if [ $error_flag -eq 0 ]; then
    if [ -z "$binary_logging" ]; then
      echo -e "\n[$(date)] MySQL backup completed.\n[$(date)] Restore script generated: $restore_script" | tee -a "$MYSQL_LOG_FILE"
    else
      echo -e "\n[$(date)] MySQL backup completed. MASTER_LOG_FILE and MASTER_LOG_POS: $master_info\n[$(date)] Restore script generated: $restore_script\n[$(date)] Master Info log generated: $MASTERINFO_LOG_FILE\n[$(date)] Backup log file generated: $MYSQL_LOG_FILE" | tee -a "$MYSQL_LOG_FILE"
      echo "$master_info" > "$MASTERINFO_LOG_FILE"
      if [[ "$mode" = 'all' ]]; then
        mysqlbackup_binlog_filename=$($MYSQL_CMD_PREFIX -e "SHOW MASTER LOGS;" | grep "mysql-bin" | awk '{print $1}' | xargs)
        startpos=$(awk -F ',' '{print $2}' $MASTERINFO_LOG_FILE)
        echo "mysqlbinlog --start-position=${startpos} $mysqlbackup_binlog_filename | mysql" >> "$MASTERINFO_LOG_FILE"
      fi
    fi
    # Sync with AWS S3
    if [[ "$AWSUPLOAD" = [yY] || "$BACKBLAZE_UPLOAD" = [yY] || "$DIGITALOCEAN_UPLOAD" = [yY] || "$LINODE_UPLOAD" = [yY] || "$CFR2_UPLOAD" = [yY] || "$UPCLOUD_UPLOAD" = [yY] ]]; then
      echo -e "\nTransfer backup to S3 storage to ${S3_LABEL}"
      echo "aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT_LABEL} \"$MYSQL_BACKUP_DIR\" \"s3://$BUCKET/mysql/$DT\""
      aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} "$MYSQL_BACKUP_DIR" "s3://$BUCKET/mysql/$DT" | tee -a "$MYSQL_LOG_FILE"
    fi 
    if [ $? -ne 0 ]; then
      echo "[$(date)] Error syncing with S3" | tee -a "$MYSQL_LOG_FILE"
      error_flag=1
    fi
  else
    echo "[$(date)] MySQL backup encountered errors." | tee -a "$MYSQL_LOG_FILE"
  fi
  # After the backup is complete, you can calculate the final disk space utilization
  mysql_backup_size=$(du -k "$MYSQL_BACKUP_DIR" | tail -1 | cut -f1)
  echo "[$(date)] Backup size: $mysql_backup_size KB"
  echo "[$(date)] Backup saved to $MYSQL_BACKUP_DIR"
}

backup_binlogs() {
  mode=$1
  tar_comp="$2"
  if [[ "$mode" != 'all' ]]; then
    flush_binlogs
  fi
  if [[ -z "$tar_comp" ]]; then
    COMPRESSION_METHOD="$COMPRESSION_METHOD"
  elif [[ "$tar_comp" = 'comp' ]]; then
    COMPRESSION_METHOD='zstd'
  elif [[ "$tar_comp" = 'pigz' ]]; then
    COMPRESSION_METHOD='pigz'
  fi
  binlogs_size=$(ls -lart $DATADIR | awk "/mysql-bin/ {total += \$5} END {print \"[$(date)] Total size of mysql-bin files:\", total / (1024 * 1024), \"MB\"}")
  # Check disk space before backup
  check_disk_space "$BACKUP_DIR"
  # Check if binary logging is enabled
  binary_logging=$($MYSQL_CMD_PREFIX -e "SHOW VARIABLES WHERE Variable_name = 'log_bin';" | grep -i "ON")
  if [ -z "$binary_logging" ]; then
    echo "[$(date)] Binary logging is not enabled. Exiting..." | tee -a "$LOG_FILE"
    return 1
  fi

  # Check if any binary logs exist
  MYSQL_BINLOG_FILENAME=$($MYSQL_CMD_PREFIX -e "SHOW MASTER LOGS;" | grep "mysql-bin" | awk '{print $1}')
  if [ -z "$MYSQL_BINLOG_FILENAME" ]; then
    echo "[$(date)] No binary logs found. Exiting..." | tee -a "$LOG_FILE"
    return 1
  fi

  echo "[$(date)] Starting binlog backup process..." | tee -a "$LOG_FILE"
  error_flag=0
  echo "$binlogs_size"
  for file in $MYSQL_BINLOG_FILENAME; do
    # file_fullpath="${DATADIR}${file}"
    mkdir -p "$BACKUP_DIR" && chown mysql:mysql "$BACKUP_DIR" && cd "$BACKUP_DIR"
    # Check disk space during backup
    check_disk_space "$BACKUP_DIR"
    if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
      echo "[$(date)] backup binlog ${DATADIR}/$file" | tee -a "$LOG_FILE"
      echo "[$(date)] $MYSQLBINLOG_CMD_PREFIX --read-from-remote-server --raw ${file}" | tee -a "$LOG_FILE"
    else
      echo "[$(date)] backup binlog ${DATADIR}/${file}" | tee -a "$LOG_FILE"
    fi
    $MYSQLBINLOG_CMD_PREFIX --read-from-remote-server --raw "$file" | tee -a "$LOG_FILE"
    mysqlbinlog_exit_status=${PIPESTATUS[2]}
    if [[ "$mysqlbinlog_exit_status" -ne 0 ]]; then echo "[$(date)] Error reading binary log $file" | tee -a "$LOG_FILE"; error_flag=1; continue; fi
    if [ "$COMPRESSION_METHOD" == "none" ]; then
      if [[ "$CHECKSUMS" = [yY] ]]; then
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          echo "[$(date)] sha256sum $BACKUP_DIR/$file" | tee -a "$LOG_FILE"
        fi
        sha256sum "$BACKUP_DIR/$file" > "$BACKUP_DIR/$file.sha256"
      fi
    elif [ "$COMPRESSION_METHOD" == "pigz" ]; then
      if [[ "$CHECKSUMS" = [yY] ]]; then
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          echo "[$(date)] sha256sum $BACKUP_DIR/$file" | tee -a "$LOG_FILE"
        fi
        sha256sum "$BACKUP_DIR/$file" > "$BACKUP_DIR/$file.sha256"
      fi
      if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
        echo "[$(date)] pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} $BACKUP_DIR/$file" | tee -a "$LOG_FILE"
        echo "pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} \"$BACKUP_DIR/$file\"" >> "$LOG_FILE"
      fi
      echo "pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} \"$BACKUP_DIR/$file\"" >> "$LOG_FILE"
      pigz ${COMPRESSION_LEVEL_GZIP}${COMPRESS_RSYNCABLE_OPT} "$BACKUP_DIR/$file"
    elif [ "$COMPRESSION_METHOD" == "zstd" ]; then
      if [[ "$CHECKSUMS" = [yY] ]]; then
        if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
          echo "[$(date)] sha256sum $BACKUP_DIR/$file" | tee -a "$LOG_FILE"
        fi
        sha256sum "$BACKUP_DIR/$file" > "$BACKUP_DIR/$file.sha256"
      fi
      if [[ "$DEBUG_DISPLAY" = [yY] ]]; then
        echo "[$(date)] zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o $BACKUP_DIR/$file.zst $BACKUP_DIR/$file" | tee -a "$LOG_FILE"
        echo "zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o \"$BACKUP_DIR/$file.zst\" \"$BACKUP_DIR/$file\"" >> "$LOG_FILE"
      fi
      echo "zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o \"$BACKUP_DIR/$file.zst\" \"$BACKUP_DIR/$file\"" >> "$LOG_FILE"
      zstd${COMPRESSION_LEVEL_ZSTD_SET}${COMPRESS_RSYNCABLE_OPT} -q -f --rm -o "$BACKUP_DIR/$file.zst" "$BACKUP_DIR/$file"; fi
    if [ $? -ne 0 ]; then echo "[$(date)] Error compressing binary log $file" | tee -a "$LOG_FILE"; error_flag=1; continue; fi
  done
  if [[ "$AWSUPLOAD" = [yY] || "$BACKBLAZE_UPLOAD" = [yY] || "$DIGITALOCEAN_UPLOAD" = [yY] || "$LINODE_UPLOAD" = [yY] || "$CFR2_UPLOAD" = [yY] || "$UPCLOUD_UPLOAD" = [yY] ]]; then
    echo -e "\nTransfer backup to S3 storage to ${S3_LABEL}"
    echo "aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT_LABEL} \"$BACKUP_DIR\" \"s3://$BUCKET/binlog/$DT\""
    aws --only-show-errors s3 sync --profile=${AWS_PROFILE}${S3_ENDPOINT_OPT} "$BACKUP_DIR" "s3://$BUCKET/binlog/$DT" | tee -a "$LOG_FILE" && if [ $? -ne 0 ]; then echo "[$(date)] Error syncing with S3" | tee -a "$LOG_FILE"; error_flag=1; fi
  fi
  if [ $error_flag -eq 0 ]; then find "$BACKUP_DIR_PARENT" -mtime +${BACKUP_RETAIN_DAYS} \( -name "mysql-bin.*.gz" -o -name "mysql-bin.*.zst" \) -exec rm -rf {} \; && echo "[$(date)] Backup saved to ${BACKUP_DIR}" | tee -a "$LOG_FILE"; else echo "[$(date)] Backup to ${BACKUP_DIR} encountered errors." | tee -a "$LOG_FILE"; fi
  # After the backup is complete, you can calculate the final disk space utilization
  mysqlbinlog_backup_size=$(du -k "$BACKUP_DIR" | tail -1 | cut -f1)
  if [ -f "/home/mysqlbackup/mysql/${DT}/master_info.log" ]; then
    cp -a "/home/mysqlbackup/mysql/${DT}/master_info.log" "${BACKUP_DIR}/master_info.log"
    sed -i "2i\/home/mysqlbackup/mysql/${DT}/restore.sh all" "${BACKUP_DIR}/master_info.log"
    echo "[$(date)] Master Info Copied To: ${BACKUP_DIR}/master_info.log"
  fi
  echo "[$(date)] Backup size: $mysqlbinlog_backup_size KB"
  echo "[$(date)] Binlog Backup log file generated: $LOG_FILE"
}

purge_binlogs() {
  echo "purge binary logs"
  echo "$MYSQL_CMD_PREFIX -e \"PURGE BINARY LOGS BEFORE NOW() - INTERVAL 7 DAY;\""
  $MYSQL_CMD_PREFIX -e "PURGE BINARY LOGS BEFORE NOW() - INTERVAL 7 DAY;"
  echo
  echo "$MYSQL_CMD_PREFIX -e \"SHOW BINARY LOGS;\""
  $MYSQL_CMD_PREFIX -e "SHOW BINARY LOGS;"
}

flush_logs() {
  echo "Flush Logs"
  echo "$MYSQL_CMD_PREFIX -e \"FLUSH LOGS;\""
  $MYSQL_CMD_PREFIX -e "FLUSH LOGS;"
  echo
  $MYSQL_CMD_PREFIX -e "SHOW MASTER LOGS;" | awk '{print $1}' | tail -4
  echo
}

flush_binlogs() {
  echo "Flush Logs"
  echo "$MYSQL_CMD_PREFIX -e \"FLUSH BINARY LOGS;\""
  $MYSQL_CMD_PREFIX -e "FLUSH BINARY LOGS;"
  echo
  $MYSQL_CMD_PREFIX -e "SHOW MASTER LOGS;" | awk '{print $1}' | tail -4
  echo
}

help() {
  echo
  echo "Usage:"
  echo
  echo "$0 backup-all-mariabackup comp"
  echo "$0 backup-files comp"
  echo "$0 backup-all-mariabackup"
  echo "$0 backup-files"
  echo "$0 backup-mariabackup"
  echo
  echo "$0 backup-all"
  echo "$0 backup-mysql"
  echo "$0 backup-binlogs"
  echo "$0 purge-binlogs"
  echo "$0 flush-logs"
  echo "$0 flush-binlogs"
}

case "$1" in
  backup-all-mariabackup )
    {
      files_backup all "$2"
    } 2>&1 | tee "$BACKUP_LOG_TMP"
    ;;
  backup-files )
    {
      files_backup filesbackup "$2"
    } 2>&1 | tee "$BACKUP_LOG_TMP"
    ;;
  backup-mariabackup )
    {
      files_backup mariabackup "$2"
    } 2>&1 | tee "$BACKUP_LOG_TMP"
    ;;
  backup-all )
    mysql_backup all "$2"
    echo
    backup_binlogs all "$2"
    ;;
  backup-mysql )
    mysql_backup "$2"
    ;;
  backup-binlogs )
    backup_binlogs "$2"
    ;;
  purge-binlogs )
    purge_binlogs
    ;;
  flush-logs )
    flush_logs
    ;;
  flush-binlogs )
    flush_binlogs
    ;;
  * )
    help
    ;;
esac