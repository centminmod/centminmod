# Comprehensive MySQL/MariaDB and System Backup Utility (v1.5)

**Current Version:** 1.5

The `backup.sh` script is the underlying backup tool used in `centmin.sh menu option 21` ([documentation](https://centminmod.com/menu21-140.00beta01)) for **Centmin Mod LEMP stack** environments. This script provides advanced capabilities for physical and logical database backups, point-in-time recovery, system file archiving, efficient compression, and seamless cloud storage integration.

---

## Table of Contents

1.  [Overview](#1-overview)
2.  [Key Features](#2-key-features)
3.  [Prerequisites](#3-prerequisites)
    * [3.1. System Requirements](#31-system-requirements)
    * [3.2. Dependencies & Packages](#32-dependencies--packages)
4.  [Installation & Initial Setup](#4-installation--initial-setup)
5.  [Configuration](#5-configuration)
    * [5.1. Configuration Methods](#51-configuration-methods)
    * [5.2. General Settings](#52-general-settings)
    * [5.3. Performance & Resource Management](#53-performance--resource-management)
    * [5.4. Database Connection](#54-database-connection)
    * [5.5. Compression Settings](#55-compression-settings)
    * [5.6. Backup Locations & Paths](#56-backup-locations--paths)
    * [5.7. File Backup Settings (`files_backup`)](#57-file-backup-settings-files_backup)
    * [5.8. Cloud Storage (S3-Compatible)](#58-cloud-storage-s3-compatible)
6.  [Usage](#6-usage)
    * [6.1. Command-Line Interface](#61-command-line-interface)
    * [6.2. Scheduling Backups (Cron)](#62-scheduling-backups-cron)
7.  [Backup Workflows Explained](#7-backup-workflows-explained)
    * [7.1. File & System Backup (`files_backup` function)](#71-file--system-backup-files_backup-function)
    * [7.2. MariaBackup Physical Backup (within `files_backup`)](#72-mariabackup-physical-backup-within-files_backup)
    * [7.3. MySQL Logical Backup (`mysql_backup` function)](#73-mysql-logical-backup-mysql_backup-function)
    * [7.4. Binary Log Backup (`backup_binlogs` function)](#74-binary-log-backup-backup_binlogs-function)
8.  [Restoration Process](#8-restoration-process)
    * [8.1. General Guidance](#81-general-guidance)
    * [8.2. Restoring from `files_backup` / `mariabackup`](#82-restoring-from-files_backup--mariabackup)
    * [8.3. Restoring from `mysql_backup` / `backup_binlogs`](#83-restoring-from-mysql_backup--backup_binlogs)
    * [8.4. Point-in-Time Recovery (PITR)](#84-point-in-time-recovery-pitr)
9.  [Advanced Features & Technical Details](#9-advanced-features--technical-details)
    * [9.1. MariaDB Version Awareness](#91-mariadb-version-awareness)
    * [9.2. Performance Tuning & Resource Management](#92-performance-tuning--resource-management)
    * [9.3. Intelligent Disk Space Management](#93-intelligent-disk-space-management)
    * [9.4. Automated Cleanup](#94-automated-cleanup)
    * [9.5. Custom Tar Installation](#95-custom-tar-installation)
10. [Troubleshooting & Logging](#10-troubleshooting--logging)
    * [10.1. Common Issues](#101-common-issues)
    * [10.2. Log File Locations](#102-log-file-locations)
11. [Security Considerations](#11-security-considerations)

---

## 1. Overview

This script allows for comprehensive backup procedures for Centmin Mod LEMP stack based servers hosting MariaDB/MySQL databases and associated web applications or system configurations. It offers multiple backup strategies to suit different needs, from fast physical snapshots with `mariabackup` to flexible logical dumps with `mysqldump`/`mariadb-dump` (MariaDB 11.0.1) and granular point-in-time recovery using binary logs. Script has been tested up to MariaDB 10.11 for now.

It integrates file system backups (`rsync`, `tar`) with database backups, providing options for unified archives or separate components. Key design goals include efficiency (multi-threading, optimized commands), reliability (checksums, disk checks), flexibility (extensive configuration, cloud options), and ease of use (automated restore helpers, dependency checks).

It is highly recommended, you test the backup script and `centmin.sh menu option 21` ([documentation](https://centminmod.com/menu21-140.00beta01)) on a test Centmin Mod server/VPS server first and get comfortable and familiar with the process of backing up and restoring data. If you run into bugs, you can report them at https://community.centminmod.com/forums/bug-reports.12/.

## 2. Key Features

* **Dual Database Backup Strategies:** Supports hot physical backups (`mariabackup`) ([docs](https://mariadb.com/kb/en/mariabackup-overview/)) and logical backups (`mysqldump --tab`/`mariadb-dump --tab`) ([docs](https://mariadb.com/kb/en/mariadb-dump/)) with binary log archiving (`mysqlbinlog`).
* **Comprehensive File Backups:** Utilizes `rsync` for efficient delta transfers of system configurations, web files, tools, cron jobs, TLS certificates, etc. Optionally archives using `tar`.
* **Advanced Compression:** Leverages multi-threaded `zstd` and `pigz` with fine-grained level control and rsyncable options.
* **Multi-Cloud S3 Support:** Integrates with AWS S3, Backblaze B2, DigitalOcean Spaces, Linode Object Storage, Cloudflare R2, and UpCloud Object Storage using `aws-cli`. Includes automated configuration optimization.
* **Intelligent Resource Management:** Dynamically selects backup partitions based on free space, adjusts process priority (`nice`/`ionice`), calculates optimal compression threads, and enforces free space buffers.
* **Automated Restore Assistance:** Generates detailed `restore-instructions.txt` and executable restore scripts (`restore.sh`, `mariabackup-restore.sh`).
* **Data Integrity:** Optional SHA256 checksum generation for backup verification.
* **MariaDB Version Compatibility:** Automatically detects MariaDB versions and adapts client command syntax.
* **Dependency Management:** Includes checks and attempts to auto-install essential packages (`check_command_exists` function).
* **Optimized Tooling:** Supports using a custom-built `tar 1.35` for enhanced `zstd` performance.
* **Robust Logging:** Creates detailed, timestamped logs for each operation module.

## 3. Prerequisites

### 3.1. System Requirements

* **OS:** CentOS/RHEL 7, 8, 9 or compatible derivatives (AlmaLinux, Rocky Linux, Oracle Linux etc.).
* **Shell:** Bash version 4+.
* **Database:** MariaDB or MySQL server installed, configured, and running.
* **Disk Space:** Sufficient free space on `/` (for logs, temporary files) and the primary backup destination partition (often `/home/` or a dedicated mount). Consider the uncompressed size of your data plus overhead for compression/temporary files.
* **Permissions:** Root privileges are generally required for full functionality.

### 3.2. Dependencies & Packages

The script attempts to install some missing dependencies using `yum`/`dnf` via the `check_command_exists` function. However, ensure these core components are present:

| Utility         | Package(s) (Typical EL Names)      | Required For                                    | Notes                                             |
| :-------------- | :--------------------------------- | :---------------------------------------------- | :------------------------------------------------ |
| **Core** | `coreutils`, `gawk`, `grep`, `sed`, `findutils`, `util-linux`, `procps-ng`, `bc` | Script execution, file ops, process mgmt, calc  | Usually installed by default                      |
| **DB Clients** | `MariaDB-client`, `MariaDB-backup` (or `mariadb`, `mariadb-backup`) etc. | DB interaction, `mysqldump`, `mariabackup`      | Version depends on your DB installation source    |
| **Compression** | `zstd`, `pigz`                     | Backup compression                              | `zstd` is highly recommended                    |
| **Transfer** | `rsync`, `wget`                    | File copying, custom tar download             | `rsync` v3.2.3+ recommended                     |
| **Monitoring** | `pv`                               | Optional: `tar` progress view                   |                                                   |
| **Cloud** | `awscli` (or `awscli-v2`)          | Optional: S3 uploads                            | Requires separate configuration (`aws configure`) |

## 4. Installation & Initial Setup

1.  **Installation:** Centmin Mod LEMP stack installs the script at `/usr/local/src/centminmod/datamanagement/backup.sh` and is used as underlying tool for `centmin.sh menu option 21` ([documentation](https://centminmod.com/menu21-140.00beta01)) shell based menu and also for standalone command line usage.

2.  **Configure Script:** Edit the variables within the script (Section 5) or the recommended method is to use `.ini` override files (`/etc/centminmod/backups.ini`, `/etc/centminmod/binlog-backups.ini`).

3.  **Database Credentials:** **Critically important:** Configure passwordless access for the backup user via `/root/.my.cnf` (see Section 5.4). Avoid storing passwords directly in the script. Ensure the DB user has necessary privileges (e.g., `RELOAD`, `PROCESS`, `LOCK TABLES`, `SELECT`, `SHOW DATABASES`, `REPLICATION CLIENT`). Centmin Mod LEMP stack initial install should have already properly setup  `/root/.my.cnf` for MySQL root user password. If you manually change MySQL root user password, be sure to update  `/root/.my.cnf` as well.

4.  **Run Initial Check/Install:** Execute a simple command like `bash /usr/local/src/centminmod/datamanagement/backup.sh help`. The script may prompt to install dependencies or the custom `tar` if `NEWER_TAR='y'`. Review output for any errors.

## 5. Configuration

### 5.1. Configuration Methods

1.  **Direct Script Edit:** Modify variables directly within the script file (primary method).
2.  **INI Overrides:** Create `/etc/centminmod/backups.ini` and/or `/etc/centminmod/binlog-backups.ini`. Settings in these files (using `VAR='value'` format) will override script defaults.

### 5.2. General Settings

| Variable             | Default | Description                                                                                                |
| :------------------- | :------ | :--------------------------------------------------------------------------------------------------------- |
| `DEBUG_DISPLAY`      | `n`     | `y`: Enable verbose debug output. `n`: Standard output.                                                    |
| `CHECKSUMS`          | `y`     | `y`: Generate SHA256 checksum files (`.sha256`) for backed-up data files and binlogs. `n`: Disable.        |
| `BACKUP_RETAIN_DAYS` | `1`     | **Applies only to `backup_binlogs`:** Days to keep *old timestamped binlog backup directories* locally.      |
| `NEWER_TAR`          | `y`     | `y`: Attempt to install/use custom `tar 1.35` with native `zstd` for performance. `n`: Use system `tar`. |

### 5.3. Performance & Resource Management

| Variable           | Default                | Description                                                                                                      |
| :----------------- | :--------------------- | :--------------------------------------------------------------------------------------------------------------- |
| `BUFFER_PERCENT`   | `30`                   | Percentage of disk space to keep free on backup destination. Enforced by `check_disk_space`.                        |
| `NICEOPT`          | `-n 12`                | `nice` level (adjusts CPU priority, higher value = lower priority).                                               |
| `IONICEOPT`        | `-c2 -n7`              | `ionice` class (2=Best Effort) and priority (7=lowest) for I/O scheduling.                                      |
| `CPUS` / `CPUS_ZSTD` | *(Auto-detected)* | Number of CPU cores detected/used for multi-threaded compression (logic scales for high core counts).           |

### 5.4. Database Connection

| Variable           | Default         | Description                                                                                                                |
| :----------------- | :-------------- | :------------------------------------------------------------------------------------------------------------------------- |
| `DBHOST`           | `localhost`     | Database host (only used if `/root/.my.cnf` is missing or doesn't specify host).                                            |
| `DBUSER`           | `admin`         | Database user (only used if `/root/.my.cnf` is missing or doesn't specify user).                                            |
| `MYSQL_PWD`        | `pass`          | **INSECURE!** Database password (only used if `/root/.my.cnf` is missing or doesn't specify password). **Use `.my.cnf`!** |
| `MY_CNF`           | `/root/.my.cnf` | **Recommended:** Path to the client credentials file (`0600` permissions essential).                                        |
| `MYSQL_OPTS`       | *(See script)* | Additional options for `mysql`/`mariadb` client.                                                                           |
| `MYSQLDUMP_OPTS`   | *(See script)* | Additional options for `mysqldump`/`mariadb-dump`.                                                                         |
| `MYSQLIMPORT_OPTS` | *(See script)* | Additional options for `mysqlimport`/`mariadb-import`.                                                                     |

### 5.5. Compression Settings

| Variable                 | Default     | Description                                                                                               |
| :----------------------- | :---------- | :-------------------------------------------------------------------------------------------------------- |
| `COMPRESS_RSYNCABLE`     | `y`         | `y`: Add `--rsyncable` to `zstd`/`pigz`. Improves delta sync efficiency *of the compressed files*.           |
| `COMPRESSION_METHOD`     | `zstd`      | Default for `mysql_backup` & `backup_binlogs`. (`zstd`, `pigz`, `none`). `files_backup` prefers `zstd`.   |
| `COMPRESSION_LEVEL_GZIP` | `4`         | `pigz` level (1=fastest, 9=smallest).                                                                    |
| `COMPRESSION_LEVEL_ZSTD` | `4`         | `zstd` level (1-19+, higher=smaller/slower).                                                             |
| `FASTCOMPRESS_ZSTD`      | `y`         | `y`: Use `zstd --fast=LEVEL` for faster compression (slightly larger files). `n`: Use standard level only. |

### 5.6. Backup Locations & Paths

Generally use these default paths as the script expects these values and hasn't been tested with different paths right now.

| Variable                 | Default                                          | Description                                                                                                                               |
| :----------------------- | :----------------------------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------- |
| `BACKUP_DIR_PARENT`      | `/home/mysqlbackup`                              | Parent directory for **database** backups (`mysql`, `binlog`). Timestamped subdirs created inside.                                       |
| `MOST_FREE_SPACE_MOUNT`  | *(Auto-detected)* | Mount point automatically chosen for `BASE_DIR` based on max free space (prioritizes `/home`). Can be overridden by uncommenting/setting. |
| `BASE_DIR`               | `$MOST_FREE_SPACE_MOUNT/databackup/${DT}`        | Main output directory for `files_backup` function (contains files, or the `.tar.zst`).                                                    |
| `MYSQL_BACKUP_DIR`       | `${BACKUP_DIR_PARENT}/mysql/${DT}`               | Destination for `mysql_backup` (`mysqldump --tab`) results.                                                                               |
| `BACKUP_DIR`             | `${BACKUP_DIR_PARENT}/binlog/${DT}`              | Destination for `backup_binlogs` results.                                                                                                 |
| `MARIADB_TMP_DIR`        | `${BASE_DIR}/mariadb_tmp`                        | Temporary storage for `mariabackup` data (within `files_backup`).                                                                         |
| `DOMAINS_TMP_DIR`        | `${BASE_DIR}/domains_tmp`                        | Temporary storage for domain files (within `files_backup`).                                                                               |
| `CRON_BACKUP_DIR`        | `${BASE_DIR}/cronjobs_tmp`                       | Temporary storage for cron jobs (within `files_backup`).                                                                                  |
| `LOG_FILE`               | `/var/log/mysql_binlog_backup_${DT}.log`         | Log file for `backup_binlogs`.                                                                                                            |
| `MYSQL_LOG_FILE`         | `/var/log/mysql_backup_${DT}.log`                | Log file for `mysql_backup`.                                                                                                              |
| `BACKUP_LOG`             | `${BASE_DIR}/files-backup_${DT}.log`             | Main log file for `files_backup`.                                                                                                         |
| `MARIABACKUP_LOG`        | `${MARIADB_TMP_DIR}/mariabackup_${DT}.log`       | Specific log for `mariabackup` operations.                                                                                                |
| `RSYNC_LOG`              | `${DOMAINS_TMP_DIR}/rsync_${DT}.log`             | Specific log for `rsync` operations during domain backup.                                                                                 |

### 5.7. File Backup Settings (`files_backup`)

| Variable                       | Default                                                                          | Description                                                                                                                                                           |
| :----------------------------- | :------------------------------------------------------------------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `FILES_TARBALL_CREATION`       | `n`                                                                              | `n`: Copy files into `$BASE_DIR` structure (incl. `mariadb_tmp`, `domains_tmp`). `y`: Create single `${BASE_DIR}/centminmod_backup.tar.zst`, remove temp subdirs. |
| `DIRECTORIES_TO_BACKUP`        | `( "/etc/centminmod" ... )`                                                      | Array of core directories included in `files_backup`. Dynamically appended with domains, cron, `.my.cnf`, `.acme.sh`, etc.                                           |
| `DIRECTORIES_TO_BACKUP_NOCOMPRESS` | `( "/etc/centminmod" ... )`                                                      | Subset of dirs copied directly to `$BASE_DIR` when `FILES_TARBALL_CREATION='n'`.                                                                                      |

### 5.8. Cloud Storage (S3-Compatible)

Enable **only one** provider at a time. Set `UPLOAD='y'` and configure `PROFILE`, `BUCKETNAME`, `ENDPOINT` etc. as needed.

| Provider         | `UPLOAD` Var            | `PROFILE` Var         | `BUCKETNAME` Var         | `ENDPOINT` Var           | Other Vars                    |
| :--------------- | :---------------------- | :-------------------- | :----------------------- | :----------------------- | :---------------------------- |
| AWS S3           | `AWSUPLOAD`             | `AWS_PROFILE`         | `AWS_BUCKETNAME`         | N/A                      | `STORAGECLASS`, `STORAGEOPT`  |
| Backblaze B2     | `BACKBLAZE_UPLOAD`      | `BACKBLAZE_PROFILE`   | `BACKBLAZE_BUCKETNAME`   | `BACKBLAZE_ENDPOINT`     |                               |
| DigitalOcean     | `DIGITALOCEAN_UPLOAD`   | `DIGITALOCEAN_PROFILE`| `DIGITALOCEAN_BUCKETNAME`| `DIGITALOCEAN_ENDPOINT`  |                               |
| Linode           | `LINODE_UPLOAD`         | `LINODE_PROFILE`      | `LINODE_BUCKETNAME`      | `LINODE_ENDPOINT`        |                               |
| Cloudflare R2    | `CFR2_UPLOAD`           | `CFR2_PROFILE`        | `CFR2_BUCKETNAME`        | `CFR2_ENDPOINT`          | `CFR2_ACCOUNTID` (Required) |
| UpCloud          | `UPCLOUD_UPLOAD`        | `UPCLOUD_PROFILE`     | `UPCLOUD_BUCKETNAME`     | `UPCLOUD_ENDPOINT`       | `UPCLOUD_ENDPOINT_NAME` (Req) |

**Note:** The script attempts to optimize `aws-cli` configuration (concurrency, multipart sizes) via `aws configure set --profile ...` when an S3 provider is enabled.

## 6. Usage

### 6.1. Command-Line Interface

Execute the script using the following format: `[bash] /path/to/script.sh [command] [option]`

| Command                    | Description                                                                                                   | Option                 | Effect of Option                                                                      | S3 Upload? |
| :------------------------- | :------------------------------------------------------------------------------------------------------------ | :--------------------- | :------------------------------------------------------------------------------------ | :--------- |
| `backup-all-mariabackup`   | **Full Physical Backup:** System files + DB (`mariabackup`). **Recommended.** | `comp` / `n`           | Forces tarball (`comp`) or directory (`n`) output for file backup portion.            | Yes        |
| `backup-files`             | **Files Only:** System files, web roots, configs, cron. No DB backup included via this command.                 | `comp` / `n`           | Forces tarball (`comp`) or directory (`n`) output.                                    | Yes        |
| `backup-mariabackup`       | **Physical DB Only:** Runs `mariabackup` portion of `files_backup`.                                           | `comp` / `n`           | Forces tarball (`comp`) or directory (`n`) output (contains only `mariadb_tmp`).      | Yes        |
| `backup-all`               | **Full Logical Backup:** Runs `mysql_backup` (`mysqldump`) **AND** `backup_binlogs`.                            | `comp` / `pigz`        | Overrides `COMPRESSION_METHOD` *only for the binlog portion*.                         | Yes (Both) |
| `backup-mysql`             | **Logical DB Only:** Runs `mysql_backup` (`mysqldump --tab`). Generates `restore.sh`.                         | *(None)* | N/A                                                                                   | Yes        |
| `backup-binlogs`           | **Binlogs Only:** Backs up binary logs, applies `BACKUP_RETAIN_DAYS` locally.                                | `comp` / `pigz`        | Overrides `COMPRESSION_METHOD` for this run.                                          | Yes        |
| `purge-binlogs`          | **Server Action:** Executes `PURGE BINARY LOGS BEFORE NOW() - INTERVAL 7 DAY;`. **Use Caution.** | *(None)* | N/A                                                                                   | No         |
| `flush-logs`             | **Server Action:** Executes `FLUSH LOGS;` (general/error logs).                                               | *(None)* | N/A                                                                                   | No         |
| `flush-binlogs`          | **Server Action:** Executes `FLUSH BINARY LOGS;` (rotates binlog file).                                       | *(None)* | N/A                                                                                   | No         |
| `help` / *`(any other)`* | Displays usage help message.                                                                                | *(None)* | N/A                                                                                   | No         |

### 6.2. Scheduling Backups (Cron)

Use `crontab -e` to schedule regular backups.

```cron
# Example 1: Daily full physical backup at 2:15 AM (creates tarball)
15 2 * * * /usr/local/sbin/backup-script.sh backup-all-mariabackup comp > /var/log/backup_cron_physical.log 2>&1

# Example 2: Daily full logical backup (mysqldump + binlogs) at 3:15 AM
15 3 * * * /usr/local/sbin/backup-script.sh backup-all > /var/log/backup_cron_logical.log 2>&1

# Example 3: Hourly binary log backup (using zstd) for fine-grained PITR
15 * * * * /usr/local/sbin/backup-script.sh backup-binlogs comp > /var/log/backup_cron_binlog.log 2>&1
```
**Note:** Redirecting output (`> ... 2>&1`) is crucial for capturing errors. Adjust paths and timings as needed.

## 7. Backup Workflows Explained

### 7.1. File & System Backup (`files_backup` function)

* **Scope:** Backs up directories defined in `DIRECTORIES_TO_BACKUP`, plus dynamically added paths like `/home/nginx/domains/*` (excluding logs), `/var/spool/cron/root`, `/etc/cron.d/`, `/root/.my.cnf`, `/root/.acme.sh/`, FTP configs, Redis/KeyDB configs, `/etc/elasticsearch` (as `/etc/elasticsearch-source`).
* **Process:** Uses `rsync` (with optimized flags if available) to copy data into temporary subdirectories within `$BASE_DIR` (e.g., `domains_tmp`, `cronjobs_tmp`).
* **Output:** Depends on `FILES_TARBALL_CREATION`:
    * `'n'`: Results in `$BASE_DIR` containing the copied structure (e.g., `$BASE_DIR/etc/centminmod`, `$BASE_DIR/domains_tmp/`).
    * `'y'`: Results in a single `${BASE_DIR}/centminmod_backup.tar.zst` archive; temporary subdirs are removed post-archival.
* **Restore:** Guided by `restore-instructions.txt`.

### 7.2. MariaBackup Physical Backup (within `files_backup`)

* **Triggered By:** `backup-all-mariabackup`, `backup-mariabackup`.
* **Process:** Executes `mariabackup --backup --target-dir=$MARIADB_TMP_DIR` followed by `mariabackup --prepare --target-dir=$MARIADB_TMP_DIR`. Logs to `$MARIABACKUP_LOG`. Copies `mariabackup-restore.sh` into `$MARIADB_TMP_DIR`.
* **Output:** Prepared physical backup data resides within `$MARIADB_TMP_DIR` (which is either inside `$BASE_DIR` or included in the `.tar.zst`).
* **Use Case:** Fastest method for large databases; captures a consistent physical snapshot. Ideal for full restores to the same or very similar DB versions. Supports incremental backups (though not explicitly implemented by *this* script's command options).
* **Restore:** Guided by `restore-instructions.txt` and uses `mariabackup-restore.sh`.

### 7.3. MySQL Logical Backup (`mysql_backup` function)

* **Triggered By:** `backup-all`, `backup-mysql`.
* **Process:**
    1.  Dumps `mysql` system DB and records master status (`--master-data=2` or `--flush-logs`) into `$MYSQL_BACKUP_DIR/master_data.sql`.
    2.  Iterates through user databases.
    3.  For each DB: Dumps schema-only (`-d`) to `DB-schema-only.sql`.
    4.  Dumps data using `mysqldump --tab=$MYSQL_BACKUP_DIR/DB_NAME`, creating `table.sql` (schema) and `table.txt` (data) files.
    5.  Compresses `.txt` files using `COMPRESSION_METHOD`.
    6.  Optionally generates checksums (`CHECKSUMS='y'`).
    7.  Generates `$MYSQL_BACKUP_DIR/restore.sh`.
* **Output:** Timestamped directory `$MYSQL_BACKUP_DIR` containing subdirectories for each database, plus helper scripts/logs.
* **Use Case:** Flexible for migrations, smaller DBs, table-level recovery. Foundation for PITR when combined with `backup_binlogs`.
* **Restore Helper:** `restore.sh` script automates `mysqlimport` of `.txt` files.

### 7.4. Binary Log Backup (`backup_binlogs` function)

* **Triggered By:** `backup-all`, `backup_binlogs`.
* **Process:**
    1.  Optionally flushes logs (`FLUSH BINARY LOGS`) unless `mode=all`.
    2.  Fetches logs listed by `SHOW MASTER LOGS` using `mysqlbinlog --read-from-remote-server --raw`.
    3.  Saves raw logs to `$BACKUP_DIR`.
    4.  Optionally generates checksums (`CHECKSUMS='y'`) *before* compression.
    5.  Compresses logs using chosen `COMPRESSION_METHOD` (or override).
    6.  Performs cleanup: `find "$BACKUP_DIR_PARENT/binlog/" -mtime +${BACKUP_RETAIN_DAYS} \( -name "mysql-bin.*.gz" -o -name "mysql-bin.*.zst" \) -exec rm -rf {} \;`.
    7.  Copies `master_info.log` from corresponding `mysql_backup` run if available.
* **Output:** Timestamped directory `$BACKUP_DIR` containing compressed binlog files.
* **Use Case:** Essential component for Point-in-Time Recovery. Allows recovery to any specific transaction within the log retention period.

## 8. Restoration Process

### 8.1. General Guidance

* Full step by step example restoration process can be found in [centmin.sh menu option 21 documentation](https://centminmod.com/menu21-140.00beta01).
* **TESTING IS MANDATORY:** Regularly practice restoration in a non-production environment that mimics production as closely as possible (OS, DB version, resources). A backup is worthless if it cannot be restored reliably.
* **Backup Target Files First:** Before restoring files (`cp`/`mv`), always back up the existing files on the target server (e.g., `cp -a /etc/nginx /etc/nginx.bak-$(date +%F)`).
* **Check Logs:** Examine backup logs and restore command output for any errors.
* **Permissions & Ownership:** After restoring files or databases, verify ownership and permissions are correct (e.g., `chown -R mysql:mysql /var/lib/mysql`, `chown -R nginx:nginx /home/nginx/domains/`).

### 8.2. Restoring from `files_backup` / `mariabackup`

**Follow the detailed steps in `restore-instructions.txt` generated within the backup directory (`$BASE_DIR`).**

**Summary:**
1.  **Transfer:** Move the backup (`.tar.zst` or the `$BASE_DIR` directory) to the target server.
2.  **Extract (if tarball):**
    ```bash
    # Requires tar 1.31+ (or the custom tar 1.35 installed by the script)
    mkdir -p /home/restoredata
    tar -I zstd -xf /path/to/centminmod_backup.tar.zst -C /home/restoredata

    # Older tar:
    # zstd -d /path/to/centminmod_backup.tar.zst
    # tar -xf /path/to/centminmod_backup.tar -C /home/restoredata
    ```
3.  **Restore System Files:** Carefully copy files/directories from the extracted backup (e.g., `/home/restoredata/etc/`) to their live locations (e.g., `/etc/`). **Backup existing files on the target server first!** The `restore-instructions.txt` provides specific `cp` or `mv` commands and diff checks.
4.  **Restore Database (if `mariabackup` was included):**
    * Locate the `mariabackup-restore.sh` script within the extracted backup's `mariadb_tmp` directory (e.g., `/home/restoredata/$BASE_DIR/mariadb_tmp/mariabackup-restore.sh`).
    * Execute it with the appropriate options (follow instructions in `restore-instructions.txt` and the script's usage help):
        ```bash
        # Example (Ensure MariaDB service is stopped first as per instructions)
        bash /home/restoredata/$BASE_DIR/mariadb_tmp/mariabackup-restore.sh copy-back /home/restoredata/$BASE_DIR/mariadb_tmp/
        # Or use 'move-back' to save space
        ```
5.  **Permissions & Ownership:** Verify file ownership and permissions after restoration.
6.  **Restart Services:** Restart Nginx, PHP-FPM, MariaDB, etc.

### 8.3. Restoring from `mysql_backup` / `backup_binlogs`

1.  **Full Restore (to Backup Time):**
    * Transfer the required `$MYSQL_BACKUP_DIR` (e.g., `/home/mysqlbackup/mysql/DDMMYY-HHMMSS/`) to the target.
    * `cd` into the transferred directory.
    * Execute `./restore.sh all` to restore all databases. (Or `./restore.sh dbname` for a single DB).
    * The script handles schema creation, decompression, and data import via `mysqlimport`. It restores to `dbname_restorecopy_TIMESTAMP` if the DB already exists.
2.  **Point-in-Time Recovery:** See Section 8.4.

### 8.4. Point-in-Time Recovery (PITR)

PITR allows restoring the database state to a specific moment *between* full logical backups using binary logs.

**Prerequisites:** A full logical backup (`mysql_backup`) and a continuous sequence of subsequent binary log backups (`backup_binlogs`) covering the desired recovery point.

**Steps:**
1.  **Restore Full Backup:** Perform a full restore using `restore.sh` from the latest `mysql_backup` *before* your target time (Section 8.3, Step 1).
2.  **Identify Start Point:** Find the exact binary log file and position recorded in the `master_info.log` file within the restored `$MYSQL_BACKUP_DIR`. Example: `mysql-bin.000123,456789`.
3.  **Gather Binlogs:** Transfer all necessary subsequent `backup_binlogs` directories needed to reach the target time.
4.  **Decompress Binlogs:** Uncompress the required `.zst` or `.gz` binary log files from the transferred directories.
5.  **Apply Binlogs:** Use the `mysqlbinlog` utility to apply the logs sequentially, starting from the position identified in step 2 and stopping at your desired point.
    ```bash
    # Example: Restore up to a specific timestamp

    mysqlbinlog \
        --start-position=456789 \
        --stop-datetime="YYYY-MM-DD HH:MM:SS" \
        /path/to/decompressed/logs/mysql-bin.000123 \
        /path/to/decompressed/logs/mysql-bin.000124 \
        # ... include all logs up to the one containing the stop time
        | mysql -u your_user -p your_database # Pipe output to mysql client
    ```
    * Use `--start-datetime`, `--stop-datetime`, `--start-position`, `--stop-position` flags as needed.
    * Consult official MariaDB/MySQL `mysqlbinlog` documentation for precise syntax and options.

## 9. Advanced Features & Technical Details

### 9.1. MariaDB Version Awareness

The `set_mariadb_client_commands` function executes `mysql -V` to parse the version. It then sets shell variables (`ALIAS_MYSQL`, `ALIAS_MYSQLDUMP`, etc.) to point to either the legacy (`mysql*`) or newer (`mariadb*`) command names based on whether the version is > 10.11. This ensures the script uses the correct syntax regardless of the installed MariaDB version.

### 9.2. Performance Tuning & Resource Management

* **CPU:** `CPUS`/`CPUS_ZSTD` logic prevents overwhelming systems with extremely high core counts by limiting compression threads (e.g., halving cores used above 24/48).
* **I/O:** `IONICEOPT='-c2 -n7'` sets I/O scheduling to Best Effort (class 2) and lowest priority (7) to minimize impact on interactive processes.
* **CPU Priority:** `NICEOPT='-n 12'` moderately lowers CPU priority (higher nice value = lower priority).
* **AWS CLI:** `aws configure set` commands tune `s3.max_concurrent_requests`, `s3.multipart_threshold`, `s3.multipart_chunksize` based on CPU cores (or specific values for R2) aiming for better S3 throughput.
* **Rsync:** Checks if `rsync --help` output contains `zstd`. If yes (rsync >= 3.2.3), uses `--cc=xxhash --zc=none` (or potentially `--zc=zstd` depending on internal tests - script uses `none` currently) which can be faster than default checksum/compression methods for some workloads.

### 9.3. Intelligent Disk Space Management

* **Mount Selection:** `MOST_FREE_SPACE_MOUNT` logic dynamically identifies the best partition for temporary `files_backup` data, preventing `/` or smaller partitions from filling up. It uses `df --output=target,avail`, sorts by available space (`sort -k 2 -n -r`), and selects the top entry (`awk 'NR==1 {print $1}'`). It specifically checks `/home` and prefers it if it has the most space.
* **Buffer Check:** `check_disk_space` function calculates `effective_space = available_space - (available_space * BUFFER_PERCENT / 100)` and compares it against estimated required space. This prevents backups from completely filling the disk.

### 9.4. Automated Cleanup

The `backup_binlogs` function includes:
`find "$BACKUP_DIR_PARENT/binlog/" -mtime +${BACKUP_RETAIN_DAYS} \( -name "mysql-bin.*.gz" -o -name "mysql-bin.*.zst" \) -exec rm -rf {} \;`
This command specifically targets *files* within the parent binlog directory structure that are older than the retention period. The current implementation cleans up old *compressed log files* across potentially multiple timestamped directories if they exist directly under `$BACKUP_DIR_PARENT/binlog/`. If backups are strictly within timestamped directories like `$BACKUP_DIR_PARENT/binlog/${DT}`, a command like `find "$BACKUP_DIR_PARENT/binlog/" -maxdepth 1 -type d -mtime +${BACKUP_RETAIN_DAYS} -exec rm -rf {} \;` would be needed to prune entire old directories. 

**Note:** This cleanup is local only; S3 lifecycle policies must be configured separately for cloud storage cleanup.

### 9.5. Custom Tar Installation

If `NEWER_TAR='y'`, the script checks the OS version (CentOS/EL 7/8/9) and uses `wget` to download a specific `tar-1.35*.rpm` from `centminmod.com`. It then uses `yum -q -y localinstall` to install it. This version includes native `--use-compress-program=zstd` support, which is significantly more efficient than piping through external `zstd`. See benchmarks at https://community.centminmod.com/threads/faster-smaller-compressed-file-backups-with-tar-zstd-compression.16274/.

## 10. Troubleshooting & Logging

If you run into bugs, you can report them at https://community.centminmod.com/forums/bug-reports.12/.

### 10.1. Common Issues

| Issue                     | Potential Causes & Solutions                                                                                                                                                                    |
| :------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Permission Denied** | Run as root. Check `/root/.my.cnf` permissions (0600). Check DB user grants. Check filesystem permissions/ownership (`ls -ld`, `chown`, `chmod`) on backup dirs & MySQL datadir. Check SELinux/AppArmor. |
| **Disk Space Full** | Check `df -h`. Increase space. Lower `BACKUP_RETAIN_DAYS`. Manually delete very old backups from `$BACKUP_DIR_PARENT`. Verify `MOST_FREE_SPACE_MOUNT` logic. Increase `BUFFER_PERCENT`.             |
| **Command Not Found** | Install missing dependencies (Section 3.2). Check `$PATH` environment variable for the user running the script (especially root via cron).                                                  |
| **S3 Upload Failures** | Verify `aws configure --profile ... list`. Check IAM policy permissions. Validate bucket name/region/endpoint URL. Test network connectivity. Check firewall rules. Check R2 Account ID.            |
| **Slow Performance** | Check `top`, `htop`, `iostat`, `iotop`. Lower compression level. Use `FASTCOMPRESS_ZSTD='y'`. Ensure `NEWER_TAR='y'` is effective. Check underlying disk speed. Check network speed for S3 uploads. |
| **Restore Failures** | **Check logs meticulously.** Verify checksums if generated. Ensure DB version compatibility (esp. `mariabackup`). Check disk space on target. Verify PITR steps and log sequence.                 |
| **Dependency Install Fail** | Check network connectivity/DNS. Check repository configuration (`yum repolist`). Manually install required packages.                                                                            |

### 10.2. Log File Locations

Consult these logs for detailed execution information and errors:

* `backup_binlogs`: `/var/log/mysql_binlog_backup_DDMMYY-HHMMSS.log`
* `mysql_backup`: `/var/log/mysql_backup_DDMMYY-HHMMSS.log`
* `files_backup` (Main): `${BASE_DIR}/files-backup_DDMMYY-HHMMSS.log`
* `mariabackup` (Specific): `${MARIADB_TMP_DIR}/mariabackup_DDMMYY-HHMMSS.log`
* `rsync` (Domains): `${DOMAINS_TMP_DIR}/rsync_DDMMYY-HHMMSS.log`

Enable `DEBUG_DISPLAY='y'` for maximum verbosity during troubleshooting runs.

## 11. Security Considerations

* **Credentials:** **Paramount Importance.** Use `/root/.my.cnf` with `0600` permissions. Avoid plaintext passwords in scripts or logs. Grant the backup DB user the minimum required privileges.
* **File Permissions:** Ensure the script itself (`700`) and all generated backup files/directories (`600`/`700`) have restrictive permissions. Avoid world-readable backups.
* **Network:** If backing up over a network (e.g., NFS, SSHFS, S3), ensure transport is encrypted (TLS/SSH). Ensure S3 endpoints use HTTPS.
* **Encryption at Rest:** For maximum security, encrypt backup archives (e.g., using `gpg`) before uploading to S3 or storing locally, especially if containing sensitive data. `mariabackup` also supports built-in encryption.
* **Cloud Security:** Utilize dedicated IAM users/roles with least-privilege S3 policies. Enable MFA on AWS accounts. Use S3 bucket versioning and consider Object Lock/legal hold for immutability against accidental deletion or ransomware. Regularly audit S3 access.
* **Regular Audits & Testing:** Periodically review script configurations, permissions, and logs. **Crucially, perform regular restore tests** to validate backup integrity and procedure effectiveness.
* **Input Sanitization:** While primarily internally driven, be cautious if modifying the script to accept external input; sanitize paths and parameters.