# Backup utility, generation format 2

See the [menu 21 migration and restore guide](centmin.sh-menu-21.readme.md) for the operational procedure. `backups.sh` exits nonzero on failed producers, compressors, copies or uploads. Existing backup roots and source parents are changed to root-owned mode 700; adapt any non-root readers. Local generations are private, source-namespaced directories. Failed or unrecognized binlog-status queries fail the job; only a successfully queried disabled state produces the no-PITR notice. `COMPLETE` marks finished local media and `BACKUP_RESULT<TAB>path` is emitted only after all requested stages, including any upload, succeed.

## Configuration

The script reads `/etc/centminmod/binlog-backups.ini`, then `/etc/centminmod/backups.ini`. `BACKUP_CONFIG=/absolute/file` optionally supplies a final configuration file, useful for isolated tests. These are trusted shell files and must be writable only by root.

```bash
MY_CNF='/root/.my.cnf'
DBHOST='localhost'
FILES_BACKUP_ROOT='/home/databackup'
BACKUP_DIR_PARENT='/home/mysqlbackup'
SOURCE_ID='server-a'                 # Default: /etc/machine-id
FILES_TARBALL_CREATION='n'
COMPRESSION_METHOD='zstd'            # Native SQL/binlogs: zstd, pigz or none
COMPRESSION_LEVEL_ZSTD=4
FASTCOMPRESS_ZSTD='y'                # --fast=4, not zstd level -4
COMPRESSION_LEVEL_GZIP=4
COMPRESS_THREADS=2
BUFFER_PERCENT=10
CHECKSUMS='n'                       # y adds a separate full-data checksum pass
# Extend the default file/configuration coverage:
DIRECTORIES_TO_BACKUP+=('/srv/application-data')
```

Credentials must be in `MY_CNF`. Old `DBUSER`/`MYSQL_PWD` command-line password fallback is removed; passwords are not embedded in generated scripts. Old derived paths such as `BASE_DIR`, `MYSQL_BACKUP_DIR` and timestamp-formatted directory assumptions are replaced by generation format 2. `BACKUP_RETAIN_DAYS` no longer deletes media automatically.

Effective obsolete overrides now cause an actionable error before backup work: old destination paths, `DIRECTORIES_TO_BACKUP_NOCOMPRESS`, `MYSQLDUMP_OPTS`, `MYSQLIMPORT_OPTS`, `NICEOPT`, `IONICEOPT` and `COMPRESS_RSYNCABLE`. Replace path overrides with `FILES_BACKUP_ROOT` or `BACKUP_DIR_PARENT`, merge extra sources into `DIRECTORIES_TO_BACKUP`, and use an external `nice`/`ionice` wrapper when needed. Review custom dump/locking and rsyncable requirements before removing their old settings; arbitrary old dump flags are not silently applied to the new native-SQL contract. Older expressions referencing `DT` or the former automatic mount selection reach this compatibility error instead of an unbound-variable crash. Use a dedicated backup root, not a shared parent such as `/home`.

File/physical tar.zst archives honor `FASTCOMPRESS_ZSTD`, `COMPRESSION_LEVEL_ZSTD` and `COMPRESS_THREADS`. SQL/binlogs use those zstd settings too when zstd is selected. Physical archive preflight budgets staging plus an archive estimate, then checks free space again after staging. These remain estimates, not a guarantee against a full disk.

Enabling S3 forces archive creation for `backup-files`, `backup-mariabackup` and `backup-all-mariabackup`, even without `comp` or with `FILES_TARBALL_CREATION=n`. S3 cannot preserve a plain filesystem tree's links and metadata. The existing checked tar.zst path packages them before upload; SQL/binlog files remain in their existing format. Without S3, file/physical archives remain optional.

Backup-root ancestors must already exist, be root-owned and exclude group/other writes unless sticky. Pre-create custom nested parents. Source-ID symlinks are refused before touching their targets; legacy mysql-owned dedicated roots and source directories are hardened to root-owned 0700. File staging preserves sparse allocation and hardlinks across configured roots in one rsync invocation.

## Commands and output

| Command | Generation contents |
| --- | --- |
| `backup-files` | `files/`, coverage/inventory and generation metadata |
| `backup-mariabackup` | Prepared `mariadb_tmp/`, restore helper and metadata |
| `backup-all-mariabackup` | Both file and physical contents |
| `backup-mysql` | `mysql/` native SQL, checksums, restore helper, account/reference exports |
| `backup-binlogs` | Available closed binlogs and listing; disabled logging is recorded |
| `backup-all` | Native SQL and available closed binlogs |
| `flush-logs` / `flush-binlogs` | Explicit database log flush |
| `purge-binlogs` | Refuses age-only automatic purge and explains the recovery requirement |

`comp` creates a tar.zst for file/physical commands; those commands reject `pigz` and `none`. Archive prerequisites are checked before staging. For native SQL and closed binlogs, `comp`, `pigz` and `none` select encoding. The generated logical restore requires manifest entries for its index and selected dumps and verifies their checksums even when global `CHECKSUMS=n` because corrupt SQL must be detected before creating databases. Global file checksums, when enabled, use relative paths so media can move between servers.

Logical backups include a checksummed schema-only `all-schema.sql` with the selected encoding. Restore `all` bootstraps cross-database dependencies before the ordinary per-database imports; selective restores keep their existing scope. See the menu guide for partial-bootstrap recovery and legacy media limitations.

Metadata:

- `backup.info`: format, source ID, hostname, run ID, mode and source database version.
- `coverage.tsv`: included/missing file paths and non-snapshot limitations.
- `inventory.tsv`: relative payload file names and byte lengths, excluding only generation-level inventory, completion markers and checksum manifest. Nested payload files with those names remain included. This is an inventory, not proof of content integrity.
- `INCOMPLETE`: present during creation or after failed local creation.
- `COMPLETE`: local generation finished. Does not certify application correctness or a successful remote upload.
- `BACKUP_RESULT<TAB>path`: stdout result after every requested stage succeeds. Callers must check the process exit status first.

A 10% disk reserve is the default. File checks estimate input usage, physical checks estimate datadir size and SQL checks reserve twice the datadir allocation. Estimates are advisory: a file vanishing while `du` measures a live tree is logged as a `partial-estimate` warning instead of failing the job. A file backup whose configured sources all fail to resolve is refused rather than producing an empty `COMPLETE` generation. Binlog jobs validate names, check capacity and verify the selected encoder from the pre-rotation listing, so a refused job leaves the server's binary logs unrotated. Writes that land in the active log before the flush, or a concurrent rotation, are not in that budget; the per-log compression recheck and ENOSPC handling cover them. Compressed binlogs receive another free-space check after raw capture, budgeting the additional output at raw bytes plus 1% and 64 KiB before compression starts. A failed check retains the raw staged log. These estimates cannot predict every compression ratio, sparse-file expansion, SQL expansion or concurrent writer. ENOSPC still fails the job and preserves incomplete media.

## S3 configuration

Enable one provider per job. No bucket or retention policy is created automatically.

```bash
AWSUPLOAD='y'
AWS_PROFILE='backup'
AWS_BUCKETNAME='my-backup-bucket'
```

Other existing provider flags are `BACKBLAZE_UPLOAD`, `DIGITALOCEAN_UPLOAD`, `LINODE_UPLOAD`, `CFR2_UPLOAD` and `UPCLOUD_UPLOAD`. Set the matching `*_PROFILE`, `*_BUCKETNAME` and `*_ENDPOINT`. The endpoint may be a URL or the legacy ` --endpoint-url=https://...` string. For example:

```bash
CFR2_UPLOAD='y'
CFR2_PROFILE='r2'
CFR2_BUCKETNAME='centmin-backups'
CFR2_ENDPOINT='https://ACCOUNT_ID.r2.cloudflarestorage.com'
```

Uploads use `s3://BUCKET/SOURCE_ID/RUN_ID/` and publish `COMPLETE` last. They never sync with `--delete`. AWS CLI credentials, endpoint and region must already work. Provider object-lock, versioning and lifecycle settings remain the storage administrator's responsibility.

File/physical uploads contain the archive plus ordinary generation metadata. The uploader refuses unexpected non-regular media outside that archive and disables AWS CLI symlink following. Empty directories, links, sparse files and filesystem metadata are recovered from tar, not S3 objects. Plan local staging capacity and the selected provider's maximum object/multipart size; no automatic archive splitting is provided. Menu option 9 separately archives arbitrary directory contents into `directory.tar.zst` and publishes a UUID-named upload with its own completion receipt. See the menu guide for recovery and failure handling.

Provider selection is validated before backup production or upload. Multiple enabled providers are rejected before any upload. Legacy profile defaults remain `b2`, `do`, `linode`, `r2` and `upcloud`. B2/DO/Linode retain their old regional endpoint defaults; explicitly set the endpoint for another region. R2 can derive its endpoint from `CFR2_ACCOUNTID` after configuration loading. UpCloud requires an explicit full endpoint. A non-AWS provider never falls back silently to AWS. Existing AWS CLI profile tuning remains effective; the backup script no longer changes global concurrency/multipart settings.

## Operational logs

Each standalone invocation prints its private per-job log path under `/root/centminlogs`; menu-launched children share the menu job's log and ID. Set `CENTMINLOGDIR` in the calling environment to relocate logs outside backup sources/media. Phase records point to retained `files.log` and `physical.log` details. Final records distinguish operation and logger errors; a logging-only failure returns 74 without turning successful publication into an instruction to retransmit. Review the recorded results before retrying.

Keep `logging.sh` beside these scripts. New backup generations bundle it beside restore helpers before checksumming/archiving. Runtime logs stay outside backup media. See the menu guide's “Logs and troubleshooting” section for recovery evidence, privacy and retention guidance.

## Verification

On a disposable EL8/9/10 test VM with the required packages:

```bash
bash datamanagement/tests/menu21-regression.sh
bash datamanagement/tests/menu21-transfer-regression.sh
bash datamanagement/tests/menu21-s3-regression.sh
bash datamanagement/tests/menu21-logging-regression.sh
bash datamanagement/tests/menu21-logical-dependencies-regression.sh
```

The database regression uses private Unix sockets and a temporary systemd service. It exercises logical/physical round trips, database/table case rules, legacy metadata, trigger/event semantics, corrupt media, existing-database refusal, archive tuning/space checks, provider arguments, failed producers, metadata conflicts and service/datadir identity/recovery. The transfer regression starts an isolated loopback sshd and tests nc/socat/SSH, filename collisions, symlinks, sparse files, hardlinks, numeric ownership and actual key enrollment/rotation/revocation. Neither suite edits the host's authorized keys or stops its installed MariaDB service. Test services, instances, keys and data are removed on exit. Cloud uploads use an argument-recording adapter, not live buckets.

The S3 regression uses a local object-copy adapter and real archive/extraction operations. It checks same-second UUID separation, completion ordering, upload/compressor failures, nonfatal post-upload cleanup, staging overlap, original marker preservation and filesystem metadata recovery. ACL/xattr assertions require `setfacl/getfacl` and `setfattr/getfattr`; the result reports whether those tools were available. The other suites also check combined file/physical S3 packaging and nonfatal cleanup after real transfer publication. These establish orchestration and archive fidelity, not live cloud-provider behavior.

The logical-dependency regression tests native cross-database view chains, referenced functions, foreign keys, routines/events, trigger-safe data loading, selective restore, legacy media and corrupt/partial schema-bootstrap failures on a private MariaDB instance.

The logging regression checks PID-directed TERM/INT cancellation, recovery/log drain, shared logs across real child scripts, private unique files, stdout/stderr capture, phase/pipeline errors, explicit failures, log initialization/writer failures, original failure status, closed-stderr recovery and refusal to include live logs in backup/transfer/S3 source trees. Database tests also check partial SQL restore guidance, checksum failure filenames, service recovery outcomes and unchanged media checksums.

These checks do not certify every MariaDB release or a whole application's cutover. Rehearse the actual source/target version pair, application objects and restoration of database users before production migration.
