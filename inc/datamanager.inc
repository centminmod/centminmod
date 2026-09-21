# shellcheck shell=bash
# Menu 21 keeps actions in subshells so failure settings do not affect centmin.sh.
# Never log entered secrets or infer a backup path from human-readable messages.
source "$(dirname -- "${BASH_SOURCE[0]}")/../datamanagement/logging.sh"

datam_profile_section() {
  local file=$1 section=$2 mode=$3
  # Match bare or fully quoted config profile names; credential names are literal.
  # Print the original lines so comments, CRLF and a missing final newline survive.
  perl -e '
    my ($file, $section, $mode) = @ARGV;
    open my $in, "<", $file or die "Cannot read profile file: $!\n";
    my ($selected, $in_section, $option_indent) = (0, 0, undef);
    while (<$in>) {
      if (!/^\s*(?:[#;]|$)/) {
        my ($space) = /^(\s*)/;
        my $indent = length($space);
        # ConfigParser handles deeper-indented option continuations before headers.
        if (!defined($option_indent) || $indent <= $option_indent) {
          if (/^\s*\[(.+)\]/) {
            my $header = $1;
            $in_section = 1;
            undef $option_indent;
            $selected = ($header eq $section);
            if ($section =~ /^profile (.+)$/) {
              my $profile = $1;
              $selected = (($header eq "default" && $profile eq "default") ||
                $header =~ /^profile[ \t]+(?:\Q$profile\E|"\Q$profile\E"|\x27\Q$profile\E\x27)[ \t]*$/);
            }
          } else {
            $option_indent = ($in_section && /^\s*[^=:]+[=:]/) ? $indent : undef;
          }
        }
      }
      if (($mode eq "export" && $selected) || ($mode eq "remove" && !$selected)) {
        print or die "Cannot write profile output: $!\n";
      }
    }
    close $in or die "Cannot close profile file: $!\n";
    close STDOUT or die "Cannot flush profile output: $!\n";
  ' "$file" "$section" "$mode"
}

datam_profile_action() (
  set -Eeuo pipefail
  umask 077
  datam_log_begin aws-profile
  local option=$1 profile section config credentials file tmp dest stamp key
  config=${AWS_CONFIG_FILE:-$HOME/.aws/config}
  credentials=${AWS_SHARED_CREDENTIALS_FILE:-$HOME/.aws/credentials}
  # Delete, export and backup work on the profile files alone and stay available without the CLI.
  [[ $option != [1234] ]] || command -v aws >/dev/null || { echo 'Install AWS CLI with datamanagement/awscli-get.sh install first.'; exit 1; }
  if [[ $option == 1 ]]; then aws configure list-profiles; exit; fi
  if [[ $option == 7 ]]; then
    read -rep 'New directory for credential backup: ' dest
    [[ -f $config || -f $credentials ]] || { echo 'No AWS profile files found to back up.' >&2; exit 1; }
    mkdir -m 700 -- "$dest"
    [[ ! -f $config ]] || install -m 600 -- "$config" "$dest/config"
    [[ ! -f $credentials ]] || install -m 600 -- "$credentials" "$dest/credentials"
    echo "Profiles backed up to $dest"; exit
  fi
  read -rep 'Profile name: ' profile
  [[ $profile =~ ^[a-zA-Z0-9][a-zA-Z0-9._-]*$ ]] || { echo 'Invalid profile name.'; exit 1; }
  section="profile $profile"
  case $option in
    2) aws configure list --profile "$profile" ;;
    3) aws configure --profile "$profile" ;;
    4)
      echo 'Editable keys: region, output, s3.max_concurrent_requests, s3.multipart_threshold, s3.multipart_chunksize, s3.max_bandwidth, s3.addressing_style'
      read -rep 'Configuration key: ' key
      case $key in region|output|s3.max_concurrent_requests|s3.multipart_threshold|s3.multipart_chunksize|s3.max_bandwidth|s3.addressing_style) ;; *) exit 1 ;; esac
      read -rep 'New value: ' value
      aws configure set "$key" "$value" --profile "$profile"
      ;;
    5)
      read -rep "Delete profile $profile? [y/N]: " answer
      [[ $answer == [yY] ]] || exit 0
      exec 9>/run/lock/centminmod-aws-profile.lock; flock -x 9
      for file in "$config" "$credentials"; do
        [[ ! -L $file ]] || { echo "Refusing symlinked profile file: $file" >&2; exit 1; }
      done
      stamp=$(date -u +%Y%m%dT%H%M%SZ)-$$
      for file in "$config" "$credentials"; do
        [[ -f $file && ! -L $file ]] || continue
        [[ $file != "$credentials" ]] || section=$profile
        install -m 600 -- "$file" "$file.$stamp.bak"
        tmp=$(mktemp "$file.XXXXXX")
        datam_profile_section "$file" "$section" remove > "$tmp"
        mv -- "$tmp" "$file"
      done
      echo 'Requested profile sections removed if present; private recovery copies retained.'
      ;;
    6)
      read -rep 'New directory for exported profile: ' dest
      [[ ! -e $dest ]] || { echo "Export directory already exists: $dest" >&2; exit 1; }
      # Extract first so a missing profile leaves no empty export directory behind.
      tmp=$(mktemp -d "${TMPDIR:-/tmp}/aws-profile-export.XXXXXX")
      # Extracted credentials never outlive this action, whichever step fails.
      # The path is bound now: function locals are gone by the time the subshell's EXIT trap runs.
      trap "rm -rf -- ${tmp@Q}" EXIT
      [[ ! -f $config ]] || datam_profile_section "$config" "$section" export > "$tmp/config"
      [[ ! -f $credentials ]] || datam_profile_section "$credentials" "$profile" export > "$tmp/credentials"
      [[ -s $tmp/config || -s $tmp/credentials ]] || { echo "Profile not found in AWS profile files: $profile" >&2; exit 1; }
      mkdir -m 700 -- "$dest"
      for file in config credentials; do [[ ! -f $tmp/$file ]] || install -m 600 -- "$tmp/$file" "$dest/$file"; done
      echo "Profile exported to $dest"
      echo 'Referenced source_profile, SSO sessions and credential-process dependencies are not included; review them separately.'
      ;;
    *) exit 1 ;;
  esac
)

aws_profiles_submenu() {
  local option
  while :; do
    printf '\n1) List profiles\n2) Show profile configuration\n3) Create profile\n4) Edit profile\n5) Delete profile\n6) Export profile\n7) Backup profiles\n8) Back\n'
    read -rep 'Option: ' option || return
    [[ $option != 8 ]] || return
    datam_run_logged "aws-profile-$option" datam_profile_action "$option"
    [[ $? == 0 ]] || echo 'Profile operation failed.'
  done
}

datam_ssh_action() (
  set -Eeuo pipefail
  umask 077
  datam_log_begin ssh-management
  local option=$1 key host port user type comment stem dest
  case $option in
    1) find "$HOME/.ssh" -maxdepth 1 -name '*.pub' -exec ssh-keygen -lf {} \; ;;
    2)
      [[ -n ${SSH_AUTH_SOCK:-} ]] || { echo 'Start ssh-agent in your login shell first. Transfers also support a private key path without an agent.'; exit 1; }
      read -rep 'Private key path: ' key; ssh-add "$key" ;;
    3|4|5|6)
      read -rep 'Remote hostname/IP: ' host
      read -rep 'SSH port [22]: ' port; port=${port:-22}
      read -rep 'SSH user [root]: ' user; user=${user:-root}
      [[ $host =~ ^[a-zA-Z0-9][a-zA-Z0-9.-]*$ && $user =~ ^[a-zA-Z_][a-zA-Z0-9_-]*$ && $port =~ ^[1-9][0-9]{0,4}$ && $port -le 65535 ]] || exit 1
      case $option in
        3)
          read -rep 'Key type [ed25519]: ' type; type=${type:-ed25519}
          read -rep 'Key comment: ' comment
          read -rep 'New key filename stem [my1]: ' stem; stem=${stem:-my1}
          "$SCRIPT_DIR/datamanagement/keygen.sh" gen "$type" "$host" "$port" "$user" "$comment" '' "$stem"
          ;;
        4)
          read -rep 'Public key path: ' key
          ssh-copy-id -o StrictHostKeyChecking=accept-new -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -p "$port" -i "$key" "$user@$host"
          ;;
        5)
          read -rep 'Existing key filename stem, without .key: ' stem
          read -rep 'Replacement key type [ed25519]: ' type; type=${type:-ed25519}
          read -rep 'Replacement comment: ' comment
          "$SCRIPT_DIR/datamanagement/keygen.sh" rotatekeys "$type" "$host" "$port" "$user" "$comment" "$stem"
          ;;
        6)
          # Delete by exact public key, never by a comment or regular expression.
          read -rep 'Public key file to revoke: ' key
          read -rep 'Authentication private key, blank for SSH defaults/agent: ' identity
          read -rep 'Revoke this public key on the remote host? [y/N]: ' answer
          [[ $answer == [yY] ]] || exit 0
          "$SCRIPT_DIR/datamanagement/keygen.sh" remove "$key" "$host" "$port" "$user" "$identity"
          ;;
      esac
      ;;
    7)
      read -rep 'Private key path to export: ' key
      read -rep 'New directory for export: ' dest
      mkdir -m 700 -- "$dest"
      install -m 600 -- "$key" "$dest/$(basename -- "$key")"
      [[ ! -f $key.pub ]] || install -m 600 -- "$key.pub" "$dest/$(basename -- "$key.pub")"
      ;;
    8)
      read -rep 'New directory for key backup: ' dest
      mkdir -m 700 -- "$dest"
      find "$HOME/.ssh" -maxdepth 1 -type f ! -name 'known_hosts*' ! -name authorized_keys ! -name config ! -name '.*' -print0 |
        while IFS= read -r -d '' key; do install -m 600 -- "$key" "$dest/${key##*/}"; done
      echo "SSH keys backed up to $dest"
      ;;
    9) echo 'Use a normal remote account with its own backup directory. For root key access, explicitly configure PermitRootLogin prohibit-password and validate sshd -t. Menu 21 does not change sshd or root passwords.' ;;
    *) exit 1 ;;
  esac
)

ssh_keys_submenu() {
  local option
  while :; do
    printf '\n1) List keys\n2) Register key with SSH agent\n3) Generate and enroll key\n4) Enroll existing key\n5) Rotate key\n6) Revoke remote key\n7) Export key\n8) Backup keys\n9) Cloud/root access guidance\n10) Back\n'
    read -rep 'Option: ' option || return
    [[ $option != 10 ]] || return
    datam_run_logged "ssh-management-$option" datam_ssh_action "$option"
    [[ $? == 0 ]] || echo 'SSH operation failed. Existing recovery files are retained.'
  done
}

datam_transfer() {
  local source=${1:-} dest host port user key method data_port
  [[ -n $source ]] || read -rep 'Source directory: ' source
  read -rep 'Remote hostname/IP: ' host
  read -rep 'SSH port [22]: ' port; port=${port:-22}
  read -rep 'SSH user [root]: ' user; user=${user:-root}
  read -rep 'SSH private key path, blank for default: ' key
  read -rep 'Destination storage root [/home/remotebackup]: ' dest; dest=${dest:-/home/remotebackup}
  echo 'nc/socat send unencrypted bulk data. Use a trusted private network; ssh is the encrypted alternative.'
  echo 'Allow the chosen TCP data port from this source in the receiver firewall. Existing firewall policy is preserved.'
  read -rep 'Transfer method [nc], socat or ssh: ' method; method=${method:-nc}
  read -rep 'Data port [12345]: ' data_port; data_port=${data_port:-12345}
  "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh" -s "$source" -r "$dest" -h "$host" -p "$port" -u "$user" -k "$key" -m "$method" -l "$data_port"
}

datam_backup() {
  local command=$1 comp=${2:-} result
  result=$(mktemp)
  # Pipeline status is captured before any other command can replace it.
  local statuses
  if "$SCRIPT_DIR/datamanagement/backups.sh" "$command" "$comp" | tee "$result"; then
    statuses=("${PIPESTATUS[@]}")
  else
    statuses=("${PIPESTATUS[@]}")
  fi
  if (( statuses[0] || statuses[1] )); then rm -f "$result"; return 1; fi
  DATAM_BACKUP_RESULT=$(awk -F '\t' '$1=="BACKUP_RESULT" {print $2}' "$result")
  rm -f "$result"
  [[ -n $DATAM_BACKUP_RESULT && -f $DATAM_BACKUP_RESULT/COMPLETE && ! -e $DATAM_BACKUP_RESULT/INCOMPLETE ]]
}

datam_s3_action() (
  set -Eeuo pipefail
  umask 077
  datam_log_begin s3
  local option=$1 profile endpoint source dest staging dest_profile dest_endpoint
  local source_id run_id threads required available
  local -a args dest_args
  read -rep 'AWS CLI profile [default]: ' profile; profile=${profile:-default}
  read -rep 'Endpoint URL, blank for AWS S3: ' endpoint
  args=(--profile "$profile"); [[ -z $endpoint ]] || args+=(--endpoint-url "$endpoint")
  case $option in
    9)
      read -rep 'Source directory: ' source
      read -rep 'Destination s3://bucket/prefix: ' dest
      [[ -d $source && $dest =~ ^s3://[^/[:space:]]+(/.*)?$ ]] || { echo "ERROR: Source must be a directory and destination must be s3://bucket/prefix." >&2; exit 1; }
      source=$(realpath -e -- "$source")
      if [[ -n ${MENU21_LOG_FILE:-} ]]; then DATAM_LOG_EXCLUDE_DIR="$source" datam_log_check_location "$MENU21_LOG_FILE" || exit 1; fi
      [[ $source != / ]] || { echo 'Select a source directory, not the filesystem root.' >&2; exit 1; }
      [[ ! -e $source/INCOMPLETE ]] || { echo "ERROR: Source generation is incomplete: $source" >&2; exit 1; }
      for tool in aws tar zstd; do command -v "$tool" >/dev/null || { echo "ERROR: Missing required command: $tool" >&2; exit 1; }; done
      threads=${COMPRESS_THREADS:-2}
      [[ $threads =~ ^[1-9][0-9]*$ ]] || { echo "ERROR: COMPRESS_THREADS must be a positive integer." >&2; exit 1; }
      read -rep 'Local archive staging parent, outside the source [/home/databackup]: ' staging
      staging=$(realpath -m -- "${staging:-/home/databackup}")
      case "$staging/" in "$source/"*) echo 'Archive staging must be outside the source directory.' >&2; exit 1 ;; esac
      mkdir -p -- "$staging"
      required=$(du -sk -- "$source" | awk '{print $1}')
      available=$(df -Pk "$staging" | awk 'END {print $4}')
      (( required <= available * 90 / 100 )) || { echo 'Insufficient archive staging space plus 10% reserve.' >&2; exit 1; }
      source_id=$(cat /etc/machine-id)
      read -r run_id < /proc/sys/kernel/random/uuid
      [[ $source_id =~ ^[a-zA-Z0-9_-]+$ && $run_id =~ ^[0-9a-f-]{36}$ ]] || exit 1
      dest="${dest%/}/$source_id/$(date -u +%Y%m%dT%H%M%SZ)-$run_id"
      staging=$(mktemp -d "$staging/menu21-s3.XXXXXXXXXX")
      # Report before function locals and caller log redirections unwind.
      trap 'rc=$? pipeline="${PIPESTATUS[*]}"; datam_log_event ERROR "exit=$rc pipeline_status=[$pipeline]"; echo "S3 upload failed. Local media retained: $staging; incomplete remote prefix: $dest" >&2; exit "$rc"' ERR
      datam_log_phase archive "source=$source staging=$staging destination=$dest"
      echo 'Archiving directory metadata and contents before S3 upload; source links are not followed.'
      tar --numeric-owner --acls --xattrs --sparse -C "$source" -cpf - . | zstd -q -T"$threads" --fast=4 > "$staging/directory.tar.zst"
      datam_log_phase s3-upload
      aws "${args[@]}" s3 cp "$staging/directory.tar.zst" "$dest/directory.tar.zst" --no-follow-symlinks --no-progress
      printf 'format=directory-tar-zstd\narchive=directory.tar.zst\narchive_bytes=%s\ncompleted_utc=%s\n' "$(stat -c %s "$staging/directory.tar.zst")" "$(date -u +%FT%TZ)" > "$staging/COMPLETE"
      datam_log_phase s3-publish
      aws "${args[@]}" s3 cp "$staging/COMPLETE" "$dest/COMPLETE" --no-follow-symlinks --no-progress
      datam_log_phase cleanup
      # Publication succeeded. A local cleanup problem must not invite a duplicate upload.
      if ! rm -- "$staging/directory.tar.zst" "$staging/COMPLETE" || ! rmdir -- "$staging"; then
        echo "WARNING: S3 upload completed; local staging cleanup needed: $staging" >&2
      fi
      printf 'S3_RESULT\t%s\n' "$dest"
      ;;
    10|11)
      read -rep 'Source file or s3://bucket/key: ' source
      read -rep 'Destination file or s3://bucket/key: ' dest
      datam_log_phase s3-copy
      aws "${args[@]}" s3 cp "$source" "$dest" --no-progress
      ;;
    12)
      read -rep 'Source s3://bucket/key: ' source
      read -rep 'Destination s3://bucket/key: ' dest
      [[ $source == s3://*/* && $dest == s3://* ]] || { echo 'ERROR: Source and destination must be S3 object paths.' >&2; exit 1; }
      # Resolve prefix destinations before staging changes the source basename.
      [[ ${dest#s3://} == */* ]] || dest+=/
      if [[ $dest == */ ]]; then
        [[ -n ${source##*/} ]] || { echo 'ERROR: A prefix destination requires a source object basename.' >&2; exit 1; }
        dest="${dest}${source##*/}"
      fi
      read -rep 'Destination profile [default]: ' dest_profile; dest_profile=${dest_profile:-default}
      read -rep 'Destination endpoint, blank for AWS S3: ' dest_endpoint
      dest_args=(--profile "$dest_profile"); [[ -z $dest_endpoint ]] || dest_args+=(--endpoint-url "$dest_endpoint")
      if [[ $profile == "$dest_profile" && $endpoint == "$dest_endpoint" ]]; then
        datam_log_phase s3-copy
        aws "${args[@]}" s3 cp "$source" "$dest" --no-progress
      else
        read -rep 'Local staging parent with space for the object [/home/databackup]: ' staging
        staging=${staging:-/home/databackup}
        [[ $staging == /?* ]] || { echo 'ERROR: Staging parent must be an absolute path.' >&2; exit 1; }
        staging=$(realpath -m -- "$staging")
        [[ $staging != / ]] || { echo 'ERROR: Staging parent must not be the filesystem root.' >&2; exit 1; }
        mkdir -p -- "$staging"
        staging=$(mktemp -d "$staging/s3-transfer.XXXXXX")
        # Keep downloaded media on failure. Separate clients use their own credentials/endpoints.
        datam_log_phase s3-download "staging=$staging"
        aws "${args[@]}" s3 cp "$source" "$staging/object" --no-progress
        datam_log_phase s3-upload "staging=$staging"
        aws "${dest_args[@]}" s3 cp "$staging/object" "$dest" --no-progress
        if ! rm -- "$staging/object" || ! rmdir -- "$staging"; then
          echo "WARNING: S3 copy completed; local staging cleanup needed: $staging" >&2
        fi
      fi
      printf 'S3_RESULT\t%s\n' "$dest"
      ;;
    13) aws "${args[@]}" s3 ls ;;
  esac
)

datam_action() (
  set -Eeuo pipefail
  umask 077
  datam_log_begin menu-action
  local option=$1 method comp answer
  case $option in
    3)
      echo 'Destination must already run Centmin Mod. Data is staged; activation remains explicit.'
      echo 'Use physical backup for matching MariaDB packages; SQL for a version change.'
      read -rep 'Backup method: 1 physical, 2 native SQL [2]: ' method; method=${method:-2}
      read -rep 'Create a local tar.zst archive? [y/N]: ' comp
      [[ $comp == [yY] ]] && comp=comp || comp=''
      case $method in
        1) datam_backup backup-all-mariabackup "$comp"; datam_transfer "$DATAM_BACKUP_RESULT" ;;
        2)
          datam_backup backup-files "$comp"; datam_transfer "$DATAM_BACKUP_RESULT"
          datam_backup backup-all; datam_transfer "$DATAM_BACKUP_RESULT"
          ;;
        *) exit 1 ;;
      esac
      echo 'Rehearse on a separate staging server. Prepare the final SQL destination with application database names absent before freezing source writes. See the menu 21 guide.'
      ;;
    4|5|6|7)
      if [[ $option == 7 ]]; then
        read -rep 'SQL/binlog encoding: zstd [default], pigz or none: ' comp
        case $comp in ''|zstd) comp=comp ;; pigz|none) ;; *) exit 1 ;; esac
      else
        read -rep 'Create a local compressed archive? [y/N]: ' comp
        [[ $comp == [yY] ]] && comp=comp || comp=''
      fi
      case $option in 4) method=backup-all-mariabackup ;; 5) method=backup-files ;; 6) method=backup-mariabackup ;; 7) method=backup-all ;; esac
      datam_backup "$method" "$comp"
      ;;
    8) datam_transfer ;;
    9|10|11|12|13) datam_s3_action "$option" ;;
    *) exit 1 ;;
  esac
)

datamanager_menu() {
  local option rc
  while :; do
    printf '\nCentmin Mod data management\n1) Manage SSH keys\n2) Manage AWS profiles\n3) Stage migration to a new server\n4) Backup files + physical MariaDB\n5) Backup files\n6) Backup physical MariaDB\n7) Backup native SQL + closed binlogs\n8) Transfer directory, nc/socat or SSH\n9) Archive directory to S3\n10) Upload file to S3\n11) Download S3 file\n12) Copy S3 object\n13) List S3 buckets\n14) Back\n'
    read -rep 'Option [1-14]: ' option || return
    case $option in
      1) ssh_keys_submenu; continue ;;
      2) aws_profiles_submenu; continue ;;
      14) return ;;
      3|4|5|6|7|8|9|10|11|12|13) ;;
      *) echo 'Invalid option.'; continue ;;
    esac
    datam_run_logged "menu-$option" datam_action "$option"
    rc=$?
    if (( rc )); then echo "Operation failed ($rc). See the log path above."; else echo 'Operation completed. See the log path above.'; fi
  done
}
