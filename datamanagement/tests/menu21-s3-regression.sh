#!/bin/bash
# Real archive/restore with a local object-copy adapter; no live S3 credentials or writes.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d /var/tmp/menu21-s3-test.XXXXXX)
export CENTMINLOGDIR="$work/job-logs"
trap 'rm -rf -- "$work"' EXIT
trap 'exit 130' INT TERM
mkdir -p "$work/source/empty" "$work/staging" "$work/objects" "$work/bin"
printf 'payload\n' > "$work/source/payload"
printf 'hidden\n' > "$work/source/.hidden"
printf 'source marker\n' > "$work/source/COMPLETE"
printf 'outside contents must not be uploaded\n' > "$work/outside"
ln -s "$work/outside" "$work/source/external-link"
ln -s missing "$work/source/dangling-link"
ln "$work/source/payload" "$work/source/hardlink"
truncate -s 16777216 "$work/source/sparse"
chown 12345:12346 "$work/source/payload"
chmod 751 "$work/source/payload"
acl=n; xattr=n
if command -v setfacl >/dev/null && command -v getfacl >/dev/null; then setfacl -m u:23456:r-- "$work/source/payload"; acl=y; fi
if command -v setfattr >/dev/null && command -v getfattr >/dev/null; then setfattr -n user.menu21 -v retained "$work/source/payload"; xattr=y; fi
# shellcheck disable=SC1091
source "$ROOT/../inc/datamanager.inc"
# Same parent process and same timestamp: only the generation UUID can distinguish jobs.
date() { printf '20260921T120000Z\n'; }
aws() {
  while [[ $1 != s3 ]]; do shift; done
  [[ $2 == cp ]] || { echo 'Unexpected raw directory sync' >&2; return 8; }
  local source_file=$3 destination=$4 target
  printf '%s\t%s\n' "$source_file" "$4" >> "$work/aws.log"
  [[ $destination != */ ]] || destination+=${source_file##*/}
  if [[ $destination == s3://* ]]; then target="$work/objects/${destination#s3://}"; else target=$destination; fi
  [[ $source_file != s3://* ]] || source_file="$work/objects/${source_file#s3://}"
  mkdir -p "$(dirname "$target")"
  if [[ ${AWS_TEST_FAIL:-} == archive && $source_file == */directory.tar.zst ]]; then printf partial > "$target"; return 9; fi
  if [[ ${AWS_TEST_FAIL:-} == marker && $source_file == */COMPLETE ]]; then return 9; fi
  if [[ ${AWS_TEST_FAIL:-} == object-upload && $source_file == */object && $destination == s3://* ]]; then return 9; fi
  cp -- "$source_file" "$target"
}
run_menu() {
  printf '\n\n%s\ns3://fixture/prefix\n%s\n' "$work/source" "$work/staging" | datam_s3_action 9
}
run_menu > "$work/first.log"
run_menu > "$work/second.log"
first=$(awk -F '\t' '$1=="S3_RESULT" {print $2}' "$work/first.log")
second=$(awk -F '\t' '$1=="S3_RESULT" {print $2}' "$work/second.log")
[[ -n $first && -n $second && $first != "$second" ]]
[[ $(wc -l < "$work/aws.log") == 4 ]]
sed -n '1p' "$work/aws.log" | grep -q '/directory.tar.zst$'
sed -n '2p' "$work/aws.log" | grep -q '/COMPLETE$'
remote="$work/objects/${first#s3://}"
[[ -f $remote/COMPLETE && -f $remote/directory.tar.zst ]]
grep -qx 'format=directory-tar-zstd' "$remote/COMPLETE"
grep -qx "archive_bytes=$(stat -c %s "$remote/directory.tar.zst")" "$remote/COMPLETE"
mkdir "$work/restored"
zstd -qdc "$remote/directory.tar.zst" | tar --numeric-owner --acls --xattrs --xattrs-include='*' --sparse -xpf - -C "$work/restored"
cmp "$work/source/payload" "$work/restored/payload"
[[ -d $work/restored/empty && $(cat "$work/restored/.hidden") == hidden ]]
[[ $(cat "$work/restored/COMPLETE") == 'source marker' ]]
[[ -L $work/restored/external-link && $(readlink "$work/restored/external-link") == "$work/outside" ]]
[[ -L $work/restored/dangling-link && $(readlink "$work/restored/dangling-link") == missing ]]
[[ $(stat -c '%u:%g:%a' "$work/restored/payload") == "$(stat -c '%u:%g:%a' "$work/source/payload")" ]]
[[ $(stat -c %i "$work/restored/payload") == "$(stat -c %i "$work/restored/hardlink")" ]]
[[ $(stat -c %s "$work/restored/sparse") == 16777216 && $(stat -c %b "$work/restored/sparse") -lt 32768 ]]
if [[ $acl == y ]]; then cmp <(getfacl -cpn "$work/source/payload") <(getfacl -cpn "$work/restored/payload"); fi
if [[ $xattr == y ]]; then [[ $(getfattr --only-values -n user.menu21 "$work/restored/payload" 2>/dev/null) == retained ]]; fi
[[ -z $(find "$work/staging" -mindepth 1 -print -quit) ]]
echo "PASS same-second UUID separation, marker-last archive recovery, links, empty/hidden files, sparse data, ownership/mode; ACL=$acl xattr=$xattr"
# A failing later upload must use a new prefix and leave earlier accepted markers untouched.
for failure in archive marker; do
  # Fresh staging per case so retained media is this case's own, not the previous case's leftovers.
  rm -rf -- "$work/staging"; mkdir "$work/staging"
  (
    set +e
    AWS_TEST_FAIL=$failure run_menu > "$work/failed-$failure.log" 2>&1
    rc=$?
    [[ $rc == 9 ]] || exit 1
  )
  ! grep -q '^S3_RESULT' "$work/failed-$failure.log" || exit 1
  target=$(tail -n 1 "$work/aws.log" | cut -f2)
  target=${target%/*}
  [[ $target != "$first" && $target != "$second" ]]
  [[ ! -e $work/objects/${target#s3://}/COMPLETE ]]
  [[ -f $remote/COMPLETE && -f $work/objects/${second#s3://}/COMPLETE ]]
  find "$work/staging" -name directory.tar.zst | grep . >/dev/null
  grep -q 'Local media retained:' "$work/failed-$failure.log"
done
echo 'PASS failed archive/marker uploads retain local media without stale remote completion'
# Accepted uploads must remain successful when local cleanup fails.
for failed_command in rm rmdir; do
  printf '#!/bin/bash\nexit 9\n' > "$work/bin/$failed_command"
  chmod +x "$work/bin/$failed_command"
  PATH="$work/bin:$PATH" run_menu > "$work/cleanup-$failed_command.log" 2>&1
  target=$(awk -F '\t' '$1=="S3_RESULT" {print $2}' "$work/cleanup-$failed_command.log")
  [[ -n $target && -f $work/objects/${target#s3://}/COMPLETE ]]
  zstd -qt "$work/objects/${target#s3://}/directory.tar.zst"
  grep -q 'WARNING: S3 upload completed; local staging cleanup needed:' "$work/cleanup-$failed_command.log"
  rm -- "$work/bin/$failed_command"
done
echo 'PASS completed S3 uploads survive local rm/rmdir cleanup failures'
# A stage within the source must fail before creating it or invoking AWS.
previous=$(wc -l < "$work/aws.log")
(
  set +e
  printf '\n\n%s\ns3://fixture/prefix\n%s\n' "$work/source" "$work/source/stage" | datam_s3_action 9 > "$work/overlap.log" 2>&1
  [[ $? != 0 ]] || exit 1
)
[[ ! -e $work/source/stage && $(wc -l < "$work/aws.log") == "$previous" ]]
grep -q 'outside the source' "$work/overlap.log"
echo 'PASS source/staging overlap refused before copying or uploading'
# Compressor failure must not perform an upload or create a local COMPLETE receipt.
cat > "$work/bin/zstd" <<'STUB'
#!/bin/bash
cat >/dev/null
exit 9
STUB
chmod +x "$work/bin/zstd"
(
  set +e
  PATH="$work/bin:$PATH" run_menu > "$work/compressor.log" 2>&1
  [[ $? != 0 ]] || exit 1
)
[[ $(wc -l < "$work/aws.log") == "$previous" ]]
stage=$(sed -n 's/^S3 upload failed. Local media retained: \(.*\); incomplete remote prefix:.*/\1/p' "$work/compressor.log")
[[ -f $stage/directory.tar.zst && ! -e $stage/COMPLETE ]]
rm -- "$work/bin/zstd"
echo 'PASS compressor failure retains partial media without publication'
mkdir -p "$work/objects/source"
printf 'original object\n' > "$work/objects/source/backup.tar.zst"
run_copy() {
  local label=$1 profile=$2 endpoint=$3
  printf 'source\nhttps://source.invalid\ns3://source/backup.tar.zst\ns3://destination/%s/\n%s\n%s\n%s\n' \
    "$label" "$profile" "$endpoint" "$work/staging" | datam_s3_action 12
}
for mode in same cross; do
  if [[ $mode == same ]]; then profile=source; endpoint=https://source.invalid; else profile=destination; endpoint=https://destination.invalid; fi
  run_copy "$mode" "$profile" "$endpoint" > "$work/copy-$mode.log"
  grep -qx $'S3_RESULT\ts3://destination/'"$mode/backup.tar.zst" "$work/copy-$mode.log"
  cmp "$work/objects/source/backup.tar.zst" "$work/objects/destination/$mode/backup.tar.zst"
  [[ ! -e $work/objects/destination/$mode/object ]]
done
echo 'PASS same/cross-provider prefix copies retain the original object basename'
for failed_command in rm rmdir; do
  printf '#!/bin/bash\nexit 9\n' > "$work/bin/$failed_command"
  chmod +x "$work/bin/$failed_command"
  PATH="$work/bin:$PATH" run_copy "cleanup-$failed_command" destination https://destination.invalid > "$work/copy-cleanup-$failed_command.log" 2>&1
  grep -qx $'S3_RESULT\ts3://destination/cleanup-'"$failed_command/backup.tar.zst" "$work/copy-cleanup-$failed_command.log"
  cmp "$work/objects/source/backup.tar.zst" "$work/objects/destination/cleanup-$failed_command/backup.tar.zst"
  grep -q 'WARNING: S3 copy completed; local staging cleanup needed:' "$work/copy-cleanup-$failed_command.log"
  rm -- "$work/bin/$failed_command"
done
echo 'PASS completed cross-provider copies survive local rm/rmdir cleanup failures'
(
  set +e
  AWS_TEST_FAIL=object-upload run_copy failed destination https://destination.invalid > "$work/copy-failed.log" 2>&1
  [[ $? == 9 ]] || exit 1
)
! grep -q '^S3_RESULT' "$work/copy-failed.log" || exit 1
[[ ! -e $work/objects/destination/failed/backup.tar.zst ]]
find "$work/staging" -path '*/s3-transfer.*/object' | grep . >/dev/null
echo 'PASS failed cross-provider upload retains staged object without success result'
# A relative or root staging parent must be refused before any download.
previous=$(wc -l < "$work/aws.log")
for bad_staging in relative /; do
  (
    set +e
    printf 'source\nhttps://source.invalid\ns3://source/backup.tar.zst\ns3://destination/bad/\ndestination\nhttps://destination.invalid\n%s\n' "$bad_staging" | datam_s3_action 12 > "$work/bad-staging.log" 2>&1
    [[ $? != 0 ]] || exit 1
  )
  grep -Eq 'Staging parent must (be an absolute path|not be the filesystem root)' "$work/bad-staging.log"
  ! grep -q '^S3_RESULT' "$work/bad-staging.log" || exit 1
done
[[ $(wc -l < "$work/aws.log") == "$previous" ]]
[[ -z $(find / -maxdepth 1 -name 's3-transfer.*' -print -quit) && ! -e relative ]]
echo 'PASS invalid cross-provider staging parents refused before download'
# Key backup must preserve NUL-delimited names and fail if find fails, even after output.
mkdir -p "$work/key-home/.ssh"
keyname=$'private key\nwith trailing newline\n'
printf 'synthetic key\n' > "$work/key-home/.ssh/$keyname"
printf '%s\n' "$work/key-backup" | HOME="$work/key-home" datam_ssh_action 8 > "$work/key-backup.log"
cmp "$work/key-home/.ssh/$keyname" "$work/key-backup/$keyname"
[[ $(stat -c %a "$work/key-backup/$keyname") == 600 ]]
(
  set +e
  printf '%s\n' "$work/missing-key-backup" | HOME="$work/missing-home" datam_ssh_action 8 > "$work/key-missing.log" 2>&1
  [[ $? != 0 ]] || exit 1
)
! grep -q 'SSH keys backed up' "$work/key-missing.log" || exit 1
cat > "$work/bin/find" <<'STUB'
#!/bin/bash
printf '%s\0' "$FIND_TEST_KEY"
exit 9
STUB
chmod +x "$work/bin/find"
(
  set +e
  printf '%s\n' "$work/partial-key-backup" | PATH="$work/bin:$PATH" FIND_TEST_KEY="$work/key-home/.ssh/$keyname" HOME="$work/key-home" datam_ssh_action 8 > "$work/key-find-failed.log" 2>&1
  [[ $? == 9 ]] || exit 1
)
cmp "$work/key-home/.ssh/$keyname" "$work/partial-key-backup/$keyname"
! grep -q 'SSH keys backed up' "$work/key-find-failed.log" || exit 1
rm -- "$work/bin/find"
echo 'PASS key backup preserves newline filenames and propagates missing/partial enumeration failures'
echo 'ALL PASS'
