# Centmin Mod menu 21: migration and backup storage

Menu 21 prepares backup generations and transfers them to another server. The destination must already have Centmin Mod installed. Transferred configuration is staged for review; menu 21 does not replace running applications, change DNS, enable jobs or change SSH/firewall policy automatically.

Use native SQL for a MariaDB version change. Physical backup is usually the better choice for a large database when matching MariaDB server and backup-tool packages are available. The nc/socat bulk-transfer path remains the default for high-throughput transfers, including very large datasets.

## Before starting

- Verify the destination's SSH host key through a trusted channel, then establish key authentication. Transfer commands require an already trusted host key and will not accept a changed key.
- Use nc/socat over a trusted private network. SSH controls the receiver, but **the nc/socat payload is unencrypted**. Choose `-m ssh` when payload encryption is required. Source-address filtering and a final SHA-256 comparison do not make a raw network stream encrypted or resistant to an active network attacker during extraction.
- Allow only the selected data port from the source address on the receiver, and permit that destination/port in the source outbound policy. Menu 21 no longer adds broad permanent CSF allow entries. SSH transport needs only the SSH port.
- Install the needed tools before the job: Bash, coreutils, util-linux, rsync, tar, zstd, OpenSSH and Nmap Ncat or socat. Physical backup also needs the matching MariaDB-backup package and psmisc. `pigz` is needed only when selected. Backup jobs do not install or update packages.
- Keep free space for the new generation, the existing data and recovery copies. Space checks are estimates; concurrent writes and SQL expansion can consume more space. Failed jobs retain their output for diagnosis.
- Decide when application writes and background jobs will stop for the final backup. Live file copying and separately dumped databases do not form one application-wide snapshot.

## A short migration procedure

1. Install and configure Centmin Mod on a separate disposable rehearsal destination. Keep public traffic, scheduled jobs and MariaDB's event scheduler disabled there. For SQL recovery, match the source's `lower_case_table_names` setting when initializing the destination database service.
2. Use option 3, choose native SQL or physical backup, and transfer the resulting generation. For nc/socat use the destination's private IPv4 address. Each transfer prints its final directory.
3. Inspect `backup.info`, `coverage.tsv`, `inventory.tsv` and `COMPLETE`. A directory containing `INCOMPLETE` is not a completed backup. The sibling `TRANSFER_RUN.transfer.sha256` records the compared compressed-stream digest, not a reusable per-file checksum list. Require a successful command result and the final generation directory; a receipt alone does not establish publication.
4. Restore databases into the staging destination using the commands below. Review account definitions separately. Test application access with its intended database users.
5. Compare saved files under `files/` with destination files. Review hostnames, IP bindings, credentials, PHP versions, pool sizes, systemd units, UID/GID mappings and paths. Keep copies of destination configuration before applying selected changes. Do not blindly copy every `/etc`, `/root` or systemd file over a new server.
6. Run `nginx -t` and `php-fpm -t`, check each service separately, and test the destination using the real hostname and TLS/SNI. For example, `curl --resolve example.com:443:DESTINATION_IP https://example.com/`. Test login, uploads and database writes, not just a status page.
7. Prepare a fresh final SQL destination with the selected application database names absent before freezing source writes. If reusing the rehearsal server, first preserve any needed test data and arrange a separately reviewed reset or rebuild. The restore helper intentionally refuses existing databases; it does not drop them for you. Freeze source writes and background jobs, take and transfer the final generation, restore it on that prepared destination, and repeat application checks before moving traffic. Large installations should plan storage snapshots or a separately tested final rsync delta to shorten this freeze; menu 21 does not implement a live-database delta protocol.
8. Keep the source frozen and available until destination validation finishes. Before accepting destination writes, rollback can route traffic back to the unchanged source. After accepting writes, reconcile new destination data before rollback. DNS changes alone do not copy it back.

## Menu operations

| Option | Operation |
| --- | --- |
| 1 | List, enroll, rotate, revoke, export or back up SSH keys |
| 2 | List, create, edit, delete, export or back up AWS profiles |
| 3 | Prepare and transfer a migration, with explicit later activation |
| 4 | File/configuration backup plus physical MariaDB backup |
| 5 | File/configuration backup |
| 6 | Physical MariaDB backup |
| 7 | Native SQL backup plus available closed binary logs |
| 8 | Transfer a directory using nc, socat or SSH |
| 9 | Archive a directory with filesystem metadata, then upload to a unique S3 generation |
| 10–13 | Upload/download files, copy S3 objects, list buckets |
| 14 | Return |

Commands below assume `/usr/local/src/centminmod/datamanagement` is the current directory.

```bash
./backups.sh backup-all-mariabackup       # Files and prepared physical backup
./backups.sh backup-files                 # Files and configuration
./backups.sh backup-mariabackup            # Prepared physical backup
./backups.sh backup-all                   # Native SQL and closed binlogs
./backups.sh backup-mysql                 # Native SQL only
./backups.sh backup-binlogs               # Available closed binlogs only
```

Add `comp` to the file/physical commands for a local `centminmod_backup.tar.zst`. The uncompressed generation avoids creating a local archive before nc/socat streams it. Native SQL uses zstd by default; `pigz` or `none` selects another SQL encoding. A later transfer compresses its tar stream, including already-compressed files, so use uncompressed file/physical generations when staging space permits.

When automatic S3 upload is enabled, file/physical backups always create the tar.zst archive before uploading. This keeps links, ownership, permissions, ACLs, extended attributes and empty directories inside a filesystem-preserving artifact. Without S3 enabled, the optional archive behavior and nc/socat fast path stay unchanged.

The default locations are `/home/databackup/SOURCE_ID/RUN_ID/` for files/physical backups and `/home/mysqlbackup/SOURCE_ID/RUN_ID/` for native SQL/binlogs. `SOURCE_ID` defaults to the machine ID. Cloned machines must have distinct IDs or an explicitly configured source ID. `RUN_ID` contains UTC time, PID and a random suffix.

A successful command emits `BACKUP_RESULT<TAB>/absolute/generation/path`. Menu integration consumes this record only after a zero exit status. Ordinary log wording is not an API.

## Transfer a generation

```bash
./tunnel-transfers.sh -h DESTINATION_PRIVATE_IPV4 \
  -s /home/databackup/SOURCE_ID/RUN_ID \
  -r /home/remotebackup -k /root/.ssh/backup.key -m nc
```

Use `-m socat -b 262144` for socat or `-m ssh` for an encrypted stream. `-p` selects the SSH port and `-l` the nc/socat data port. A busy data port fails explicitly; the script does not silently select a port that the firewall has not allowed. IPv4 is currently required.

The receiver binds to the address used by SSH and permits the source address observed by SSH. Every generation first lands in `.incoming/SOURCE_ID/TRANSFER_RUN/`. The source hashes the compressed stream while sending; the receiver hashes the same stream while extracting. SSH must report successful extraction and both hashes must agree before the directory is moved to `DEST_ROOT/SOURCE_ID/TRANSFER_RUN/`. A failure returns nonzero and preserves the source and incomplete receiver data. Retry creates a new generation; byte-range resume is not implemented.

The protocol receipt is stored beside the final directory as `DEST_ROOT/SOURCE_ID/TRANSFER_RUN.transfer.sha256`. Files inside the source tree, including a file or symlink named `TRANSFER.sha256`, are preserved. A failed final rename can leave a receipt without its final directory; do not accept that as a completed transfer.

Once the verified directory has been published, a staging cleanup failure prints a warning with its path and still returns success plus `TRANSFER_RESULT`. Inspect/remove the leftover staging files; do not retransmit an already accepted generation just because cleanup needs attention. Extraction, checksum and publication failures remain fatal.

`TRANSFER_THREADS=2` controls compression workers. `SSH_KNOWN_HOSTS=/path/to/known_hosts` selects an explicitly managed host-key file. `REMOTE_SUDO=y` opts into passwordless sudo for receiver operations. The receiving namespace uses the source ID in `backup.info` when available, with `SOURCE_ID` as an explicit override. For full ownership preservation, receive as root or use this explicit sudo mode. A dedicated unprivileged storage account should receive archive generations, whose internal metadata remains in the tar archive, rather than extracting live file trees.

Ncat has a 10-second connection wait and a 60-second idle timeout; socat uses a 60-second inactivity timeout. These are fixed in the script, not overall job-duration limits. A stalled producer or receiver can hit the idle limit even when the network is healthy; inspect both ends' logs before retrying.

Extracted transfers preserve recorded extended-attribute namespaces, including non-user attributes. Review security labels and file capabilities for the destination's policy; relabel where required before activation.

The stream checksum adds CPU work but does not reread the source tree or create a second full-sized transfer archive. Small-file traversal, sparse files, compression ratio, CPU and receiver storage still affect throughput. No claim of a particular 100-TB transfer rate follows from small correctness tests.

## Restore files

For an archive generation, extract into a new private directory:

```bash
install -d -m 700 /home/restore-stage
zstd -dc /path/to/generation/centminmod_backup.tar.zst \
  | tar --numeric-owner --acls --xattrs --xattrs-include='*' --sparse -xpf - -C /home/restore-stage
```

Use Bash with `set -o pipefail` when automating this pipeline. A plain generation already contains `files/` and, if requested, `mariadb_tmp/`.

`files/etc/centminmod` maps to `/etc/centminmod`; `files/home/nginx/domains` maps to `/home/nginx/domains`. Rsync preserves hidden files, symlinks, hard links, ACLs, extended attributes and numeric ownership. Preserve the metadata when copying selected staged files. Review numeric ownership against the destination's user/group database first.

The included paths cover Centmin Mod, Nginx, PHP-FPM, database configuration, ACME state, AWS credentials, FTP account data, cron, custom systemd units and selected service configuration. `coverage.tsv` records included and missing paths. Website logs are included. Symlink targets are not traversed automatically. Custom external volumes and nonstandard paths must be added in `backups.ini`.

Redis/KeyDB persisted data, PostgreSQL and Elasticsearch data need their native backup procedures. Copying their configuration is not a data backup. Runtime caches and compiled software are not rebuilt from a file backup; install the matching services on the destination first.

## Restore native SQL

The generated `mysql/restore.sh` consumes native SQL dumps with schema, data, triggers, routines and events. It requires checksum-manifest entries for the index and every selected dump, then verifies saved checksums and compression before database creation. It retains all media. It refuses an existing application database, including a case-folded name collision, rather than renaming the database inside SQL, which can break qualified views, routines and application settings.

```bash
MY_CNF=/root/.my.cnf bash /path/to/generation/mysql/restore.sh all
MY_CNF=/root/.my.cnf bash /path/to/generation/mysql/restore.sh --database my_database
```

Use `--database all` to select a database literally named `all`. Dump filenames use fixed-length hashes so long non-ASCII database names do not exceed filesystem limits.

New logical backups also include checksummed `all-schema.sql` (with the selected compression suffix). An `all` restore first creates application schemas, tables, routines and view definitions across the selected databases, then imports the existing full per-database dumps. The bootstrap contains no table data, triggers or events. This allows forward cross-database views and dependencies without firing restored INSERT triggers on saved rows. The backup still consists of separate snapshots; freeze schema changes and application writes for a coordinated migration.

A selective `--database` restore does not import other databases; its external dependencies must already exist. Older format-2 media without a schema bootstrap retains sequential import behavior and emits a dependency-order warning. If the bootstrap fails, schemas across multiple selected databases may remain partly created; inspect all of them before retrying.

New logical media includes checksummed `lower_case_table_names`. Source and target settings must match, preventing case-distinct database or table names from merging during import. This conservatively refuses any setting change, even if names appear collision-free. Set up a matching-case destination instead of changing the setting on an existing populated datadir.

For earlier native-SQL format-2 media without this metadata, run the current helper against the unchanged backup and supply the source setting only after verifying it from the original server or its recorded configuration:

```bash
LOGICAL_BACKUP_DIR=/path/to/old-generation/mysql \
  SOURCE_LOWER_CASE_TABLE_NAMES=0 MY_CNF=/root/.my.cnf \
  bash ./mysql-restore.sh all
```

Do not guess the source setting or rewrite the old manifest. Its original files remain checksum-verified. An override contradicting saved metadata is rejected. Format-1 table exports remain unsupported by this helper.

Run this only on a trusted backup and a destination with the selected database names absent and the event scheduler disabled. The scheduler query must succeed and return `OFF` or `DISABLED`; failed, empty or unknown responses stop recovery before import. If import fails after creation, the partial database remains for diagnosis; inspect it before removing it and retrying. Do not enable traffic on a partial import.

`accounts-review.sql` is generated when the installed dump client supports `--system=users`. Review its users, hosts, privileges, authentication plugins and definers before applying it. `system-reference.sql.*` contains the source's `mysql` database as reference media. Neither is automatically imported into a different MariaDB version. Use the destination's own `/root/.my.cnf`; source credentials are not embedded in restore scripts.

The dump charset is explicitly utf8mb4 when the source supports it, with utf8 for older servers. Dump/restore clients allow packets up to 1 GiB; destination server limits still apply.

Default `--opt` locking covers nontransactional tables within each dumped database. It can block application writes. Each database is a separate capture, so stop cross-database writers for a coordinated recovery point. A failed dump/compressor fails the generation. Newer dump clients can emit a sandbox directive that older clients cannot read; use a compatible destination client and test the actual source/target pair. [MariaDB dump documentation](https://mariadb.com/docs/server/clients-and-utilities/backup-restore-and-import-clients/mariadb-dump).

## Restore physical MariaDB

First run the non-destructive compatibility check:

```bash
./mariabackup-restore.sh check /path/to/mariadb_tmp
./mariabackup-restore.sh copy-back /path/to/mariadb_tmp
```

The helper accepts the legacy `xtrabackup_*` metadata family and the newer `mariadb_backup_*` family. It rejects conflicting dual copies, missing producer/source versions and unprepared checkpoints. The source server version and backup producer version are checked separately against installed target/tool versions without requiring a running server. The conservative default requires exact patch matches. A matching filename or release family alone is not proof of physical format compatibility.

`MARIADB_SERVICE` defaults to `mariadb`; `DATADIR` can select an explicit destination. Automatic discovery preserves spaces in the configured datadir. Before stopping an active service, the helper verifies that its main MariaDB process has a database file from the selected datadir open. It repeats that check after startup before reporting success, without requiring the source SQL password. A wrong running datadir is refused before stopping. A wrong offline service configuration is detected after startup, and that newly started service is stopped. Review custom offline service arguments before restoring; metadata-only `check` cannot certify them.

If a pre-copy step fails while the original datadir is unchanged, the helper attempts to restore its previous running state. Once data has moved or copy-back has started, failure leaves recovery to the operator and preserves the original directory and backup. Layouts without a normal `ibdata1` or `aria_log_control` file require a separately verified manual recovery procedure.

MariaDB 10.1 through 10.11 use the legacy names in the reviewed upstream branches. Current 11.4, 11.8 and 12.3 use the newer names, with upstream legacy-read compatibility. MariaDB 5.x/10.0 do not have a corresponding bundled MariaBackup producer; prefer a tested logical migration for these installations. See [MariaDB backup file documentation](https://mariadb.com/docs/server/server-usage/backup-and-restore/mariadb-backup/files-created-by-mariadb-backup).

After a physical restore, the source database accounts/passwords are active. Use matching source credentials, keep the prior destination credentials in a recovery copy, and validate application queries before enabling traffic.

Before copy-back, the helper checks space, stops MariaDB, verifies that it stopped, preserves the old datadir at a unique `-copy-...` path, then copies and changes ownership before starting the service. A datadir that is itself a mount point, including a bind mount, cannot be preserved by rename, so `check` and the restore refuse it before any service change; mount the volume one level above the datadir or use a separately verified manual procedure. MariaBackup copies every unrecognized file in the backup directory, so the bundled `logging.sh` and `mariabackup-restore.sh` helpers are removed from the restored datadir after the copy. It passes the detected datadir explicitly to the tool. `DATADIR=/absolute/path` overrides detection. A copy/start failure leaves recovery data in place and returns nonzero. It does not automatically switch back after possible target writes.

`move-back` requires `ALLOW_MOVE_BACK=y` because it consumes backup media. `SKIP_MARIABACKUP_VER_CHECK=y` is an expert override, not a claim that cross-version restore is supported. Never test an uncertain physical upgrade against the only recovery copy.

## Dedicated backup storage server

Use a separate storage root per trusted source and a separate SSH key per source. Namespace isolation prevents accidental collisions; root SSH access is not a security boundary between mutually untrusted tenants. An unprivileged storage account must not be able to modify other sources' directories. Archive generations are preferable when the receiving account cannot preserve extracted ownership.

Keep accepted recovery generations outside a compromised source's write access, using storage snapshots, an independently controlled archival tier or a root-owned acceptance process. S3 Object Lock/versioning is provider-specific and must be configured separately. Menu 21 does not claim immutability simply because it uses unique names or a `COMPLETE` file.

Backup roots require existing root-owned ancestor directories, with no group/other write permission except on root-owned sticky directories. Pre-create custom nested parents deliberately. A root-owned backup-root symlink is supported only when its resolved parents pass the same checks; non-root-owned root links and every source-ID symlink are refused. Existing mysql-owned dedicated backup roots can be hardened safely.

File staging uses one rsync invocation with sparse-file and hardlink preservation across configured roots. Inventory and optional global checksum exclusions apply only to generation-level bookkeeping files; identically named files inside the payload remain included.

Backup roots and source-generation parents become root-owned mode 700. Existing non-root backup readers must be adjusted deliberately; a database process must not be able to replace its own recovery history.

A disabled-binlog notice requires a successful status query returning `0`. A failed or invalid binlog-status response fails the backup and leaves `INCOMPLETE`; it is not treated as disabled logging.

No job automatically deletes old generations or purges database binlogs. Configure retention only after defining recovery-point age, recovery time and the logs needed by retained physical backups. Keep at least one successfully restored generation. Do not use an age-only `find ... -delete` policy across backup and binlog trees.

Scheduling is opt-in. For example, run `backups.sh backup-all-mariabackup` from a root cron job, capture its exit status and alert on failure. A host-wide lock prevents overlapping backup jobs. Separately monitor the newest `COMPLETE` timestamp for each source; a timer firing is not proof of success. Periodically restore a generation in isolation and measure its recovery time.

S3 backups upload data first and publish `COMPLETE` last under `bucket/SOURCE_ID/RUN_ID`. An upload failure preserves the local completed generation and returns nonzero. S3-to-S3 copies using different profiles/endpoints download to a local staging directory and then upload with destination credentials; failed media remains there. The staging parent defaults to `/home/databackup`, must be an absolute path and is created if needed. Allow enough local space for the full object.

Menu option 9 archives the selected directory's contents into `directory.tar.zst` in a private local staging directory. Choose a staging parent outside the source with enough free space; `/home/databackup` is the default. The remote path includes machine ID, timestamp and a kernel-generated UUID, so successive actions cannot reuse a prefix merely because they start in the same second. The archive is uploaded first, followed by an upload receipt named `COMPLETE` recording its format and byte length. Success prints `S3_RESULT<TAB>prefix`. Any original source file named `COMPLETE` stays inside the archive; the outer receipt means the upload succeeded, not that live application data formed a consistent snapshot.

On archive/upload failure, inspect the retained local staging path printed by the command. No remote receipt is published before the archive upload succeeds. After successful publication, local cleanup errors are warnings and retain the successful result. A new attempt gets a new prefix; old accepted uploads are not modified.

Download the archive into a separate download directory, verify the receipt's expected filename/size, and extract into a new private recovery directory using the archive command above with `directory.tar.zst`. Metadata lives inside the tar archive, not in S3 object attributes. Check numeric user/group mappings before activating restored files. Menu option 9 can wrap an existing backup generation, including an existing archive; automatic backup uploads already package file/physical data directly and avoid that extra wrapping. Each archive must fit the selected provider's object/multipart limits. Large collections may need separate source directories or the nc/socat storage-server path; automated S3 archive splitting/resume is not implemented.

Menu option 12 preserves the source object basename when the destination is a bucket or ends in `/`, including copies through local staging between profiles. Successful copies print `S3_RESULT`; cleanup failures after upload are warnings and identify retained local staging. Named-profile region setup changes that profile only. Default/defaultreset setup explicitly targets `default`, including R2 tuning, regardless of `AWS_PROFILE` or `AWS_DEFAULT_PROFILE` in the environment. Default setup preserves an existing access key found by AWS CLI, including profiles selected through `AWS_CONFIG_FILE`, `AWS_SHARED_CREDENTIALS_FILE` or a different home directory. Missing conventional files do not authorize overwriting it; use `defaultreset` to replace existing credentials.

## Logs and troubleshooting

Menu actions, SSH/profile submenu operations and standalone backup, restore, transfer and AWS installer commands create a private `menu21-TIMESTAMP-RANDOM.log` under `/root/centminlogs`. Each prints `Log: /absolute/path` when it starts. Set `CENTMINLOGDIR=/another/private/directory` in the calling environment to choose another location. Keep it outside every source tree and backup/restore directory. The scripts refuse a log inside selected backup media or an active archive/transfer source. Do not set the internal `MENU21_LOG_*` environment variables yourself.

A menu action and its child scripts share one job ID and log. Records identify the component and phase, UTC timestamps, completed-phase durations and failure exit/pipeline statuses. Standalone scripts also record their source SHA-256 and Bash version; MariaDB backup/restore output records server/version information. The final record separates `operation_exit` from `logger_exit`, includes elapsed seconds and records the overall `exit`. An otherwise successful operation returns 74 if log capture fails. If this happens, inspect its completion/result records before retrying a large transfer. An existing operation failure keeps its original exit status.

Standalone logging uses Perl/POSIX, already supplied by Centmin Mod, to supervise the operation. Sending TERM or INT to the launched script PID forwards cancellation to its worker group and waits for recovery output to drain. Cancellation during logging setup prevents the operation from starting; a child launched after the signal also checks the pending cancellation before entering its operation. The final record includes `cancellation_exit`; cancellation returns 143 for TERM or 130 for INT. A forced SIGKILL or host failure cannot run recovery handlers. Inspect partial media and service state before retrying an interrupted restore.

With shell job control enabled, standalone commands piped to `cat` or `tee` support Ctrl-Z and `fg`. When the caller shares the command's process group, such as `set +m` or a nested noninteractive shell, suspension remains limited to the supervised command to protect the caller shell. Resume that command with `kill -CONT PUBLIC_PID` from another terminal.

The whole-session `centminmod_*_data_transfer.log` still exists when using `centmin.sh` or `centmin-cli.sh`. Use the printed per-job log first. Native tool output is retained without adding timestamps to every line. Generated status records use timestamps; some detailed failures are in the companion logs below.

| Failure | Evidence and next step |
| --- | --- |
| File backup | Find the `file-copy` phase and source path, then inspect the generation's `files.log` and `coverage.tsv`. |
| MariaBackup production | Inspect `physical-backup` or `physical-prepare` and the generation's `physical.log`. |
| SQL backup/restore | Identify the database and dump filename in the phase record. Checksum failures name the failed file. An import failure may leave a partial database; preserve media and inspect that database before retrying. The helper refuses existing databases and does not roll back SQL. |
| Physical restore | Inspect the failed phase, original-data path, whether copying started, and initial/final service states. For service failures, run `systemctl status SERVICE` and `journalctl -u SERVICE` on the destination. Do not delete recovery directories. |
| Transfer | Sender logs label tar/compression/tee and transport statuses. After the receiver starts, its private `.incoming/SOURCE/RUN/receiver.log` contains receiver/extraction/hash diagnostics. Its path is printed on failure. Successful publication removes that temporary log; accepted data contains no injected diagnostic files. |
| S3 | Distinguish archive creation, data upload and marker publication. Inspect the retained staging directory and reported remote prefix. Progress redraws are disabled to keep transcript size manageable. |
| SSH keys | Identify generation, enrollment, replacement verification, activation or revocation phase. Preserve the old/replacement keys and named recovery files after failure. |

The shared `logging.sh` must remain beside the scripts. New logical and physical backups include it beside their restore helper, before checksums/archiving. For old backups, use the current helper and its adjacent logger from the repository, with the documented external-media path, rather than replacing checksummed files inside an existing generation.

Logs are mode 0600 and can contain database names, paths, addresses and sensitive native-tool error details. Review them before sharing. The scripts do not enable shell tracing or dump argument lists, environment variables, credentials or SQL bodies. Avoid `bash -x` and unattended AWS `--debug` capture for credential operations.

A phase-start record establishes where work began; a quiet log alone does not prove a long transfer has stalled. This pass adds phase timings, not byte-rate/ETA monitoring. A kill, power failure or filesystem failure can prevent a final record. Monitor process/service state and completion markers as well. Operational logs are retained; configure their retention separately from backup generations. No automatic log or backup deletion is introduced.

## Changes from the previous workflow

- Native SQL replaces `--tab` plus generated `mysqlimport` restores. The existing-database `_restorecopy_...` behavior is removed.
- Compressed and uncompressed file backups have the same `files/` layout and coverage.
- Paths include a source ID and unique generation. Update external cron jobs that parse old paths or log text.
- Automatic package upgrades, permanent firewall allow entries, root-password changes and unverified key replacement are removed from backup/transfer operations.
- Passwords on the keygen command line are rejected. Use interactive SSH authentication, or `KEYGEN_SSH_PASSWORD` / `KEYGEN_SUDO_PASSWORD` for existing automation. Password-assisted enrollment requires `sshpass` (EPEL) and is refused before any key is generated when it is missing.
- AWS access keys on the `awscli-get.sh` command line are rejected: process arguments are readable by every local account. Export `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY` instead; region and output may still be passed as positions 5 and 6. Credentials are written through the AWS CLI configure wizard on stdin, so the secret never appears in any process argument list. R2 tuning always uses the installed binary path, so `sudo` and cron environments without `/usr/local/bin` on `PATH` work.
- Revoking the last remaining authorized key is refused, because the retained remote recovery copy would be unreachable. Only lines carrying a recognized OpenSSH key type and base64 blob count as remaining keys; comments and malformed lines do not. Enroll a replacement first, or set `KEYGEN_ALLOW_LAST_KEY_REMOVAL=1` to decommission that account deliberately.
- SSH rotation preserves old keys until new authentication is verified and keeps private recovery copies. Secrets are never printed into menu logs. A restricted forced-command key may not support the generic verification probe and can require manual rotation.
- Rotation/revocation match complete key fields while preserving quoted authorization options. Revocation accepts security-key/FIDO public keys recognized by the local OpenSSH tools as well as the ordinary supported types. Valid public-key files may end at EOF without a final newline for revocation or rotation; generating hardware keys remains a separate operation. Revocation accepts a separate authentication identity: `./keygen.sh remove /path/key.pub HOST PORT USER /path/authentication.key`. Enrollment and verification share `SSH_KNOWN_HOSTS`. Verification deliberately ignores SSH config identities and routing; use a directly reachable hostname/IP, or manually handle aliases/proxy routes.
- Profile edit accepts `region`, `output` and the S3 transfer keys (`s3.max_concurrent_requests`, `s3.multipart_threshold`, `s3.multipart_chunksize`, `s3.max_bandwidth`, `s3.addressing_style`); set any other key with `aws configure set KEY VALUE --profile NAME`. Profile delete, export and backup work on the profile files alone and no longer require the AWS CLI binary. A failed export leaves no directory behind.
- Profile export/delete accepts plain or fully quoted profile names and comments after section headers. Normalize mixed quoting or escaped name spellings before using the submenu; those forms are left untouched. An absent profile or an empty profile backup is reported explicitly; config-only role/SSO profiles remain exportable. Indented continuations of multiline values remain part of their original profile, even when they contain text resembling a section header. Profile exports contain selected sections only. Review referenced source profiles, SSO sessions and credential-process dependencies separately. Profile deletion refuses symlinked config/credential files before editing either.
- Old generations and binlogs are retained by default. Plan storage monitoring and an explicit retention policy.

The detailed pre-implementation guide is available in Git history. The current command/configuration reference is [backups.sh.md](backups.sh.md).
