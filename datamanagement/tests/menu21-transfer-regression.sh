#!/bin/bash
# Real loopback sshd and nc/socat/SSH transfers; never edit the host's authorized_keys.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d /var/tmp/menu21-transfer.XXXXXX)
export CENTMINLOGDIR="$work/job-logs"
sshd_pid='' agent_pid=''
cleanup() {
  rc=$?
  [[ -z $sshd_pid ]] || { kill "$sshd_pid" 2>/dev/null || :; wait "$sshd_pid" 2>/dev/null || :; }
  [[ -z $agent_pid ]] || kill "$agent_pid" 2>/dev/null || :
  rm -rf -- "$work"
  exit "$rc"
}
trap cleanup EXIT
trap 'exit 130' INT TERM
free_port() {
  local p
  for p in $(shuf -i 20000-45000 -n 50); do
    if [[ -z $(ss -ltnH "sport = :$p") ]]; then echo "$p"; return; fi
  done
  return 1
}
ssh_port=$(free_port)
data_port=$(free_port)
mkdir -p "$work/remote/.ssh" "$work/local/.ssh" "$work/source" "$work/output" "$work/remote-bin"
ssh-keygen -q -t ed25519 -N '' -f "$work/host"
ssh-keygen -q -t ed25519 -N '' -f "$work/admin"
cp "$work/admin.pub" "$work/remote/.ssh/authorized_keys"
printf '[127.0.0.1]:%s %s\n' "$ssh_port" "$(cat "$work/host.pub")" > "$work/known_hosts"
cat > "$work/command" <<CMD
#!/bin/bash
export HOME='$work/remote'
export PATH='$work/remote-bin':"\$PATH"
if [[ -f '$work/reject-verification' && \$SSH_ORIGINAL_COMMAND == true ]]; then exit 37; fi
exec bash -c "\$SSH_ORIGINAL_COMMAND"
CMD
chmod +x "$work/command"
cat > "$work/sshd.conf" <<CFG
Port $ssh_port
ListenAddress 127.0.0.1
HostKey $work/host
PidFile $work/sshd.pid
AuthorizedKeysFile $work/remote/.ssh/authorized_keys
PermitRootLogin yes
PasswordAuthentication no
KbdInteractiveAuthentication no
UsePAM no
StrictModes no
ForceCommand $work/command
CFG
/usr/sbin/sshd -D -e -f "$work/sshd.conf" > "$work/sshd.log" 2>&1 & sshd_pid=$!
SSH=(ssh -F /dev/null -i "$work/admin" -o IdentitiesOnly=yes -o BatchMode=yes -o "UserKnownHostsFile=$work/known_hosts" -o StrictHostKeyChecking=yes -p "$ssh_port")
for ((i=0;i<40;i++)); do if "${SSH[@]}" root@127.0.0.1 true 2>/dev/null; then break; fi; sleep .25; done
"${SSH[@]}" root@127.0.0.1 true
head -c 1048576 /dev/urandom > "$work/source/payload"
ln "$work/source/payload" "$work/source/hardlink"
printf hidden > "$work/source/.hidden"
truncate -s 16777216 "$work/source/sparse"
chown 12345:12346 "$work/source/payload"
command -v setfattr >/dev/null && command -v getfattr >/dev/null || { echo 'Install attr for user/trusted xattr regression checks.' >&2; exit 1; }
setfattr -n user.menu21 -v user-retained "$work/source/payload"
setfattr -n trusted.menu21 -v trusted-retained "$work/source/payload"
printf external > "$work/external"
for method in nc socat ssh; do
  for receipt in regular symlink; do
    rm -f "$work/source/TRANSFER.sha256"
    if [[ $receipt == regular ]]; then printf original > "$work/source/TRANSFER.sha256"; else ln -s "$work/external" "$work/source/TRANSFER.sha256"; fi
    SSH_KNOWN_HOSTS="$work/known_hosts" SOURCE_ID=fixture bash "$ROOT/tunnel-transfers.sh" -h 127.0.0.1 -p "$ssh_port" -k "$work/admin" -s "$work/source" -r "$work/output" -m "$method" -l "$data_port" > "$work/transfer.log" 2>&1 || { cat "$work/transfer.log"; exit 1; }
    dest=$(awk -F '\t' '$1=="TRANSFER_RESULT" {print $2}' "$work/transfer.log")
    [[ -d $dest && -s $dest.transfer.sha256 ]]
    [[ ! -e $dest/receiver.log ]]
    grep -q "sender_pipeline_status tar=0 zstd=0 tee=0" "$work/transfer.log"
    cmp "$work/source/payload" "$dest/payload"
    [[ $(cat "$dest/.hidden") == hidden && $(cat "$work/external") == external ]]
    [[ $(stat -c '%u:%g' "$dest/payload") == 12345:12346 ]]
    [[ $(stat -c %i "$dest/payload") == "$(stat -c %i "$dest/hardlink")" ]]
    [[ $(stat -c %s "$dest/sparse") == 16777216 && $(stat -c %b "$dest/sparse") -lt 32768 ]]
    [[ $(getfattr --only-values -n user.menu21 "$dest/payload" 2>/dev/null) == user-retained ]]
    [[ $(getfattr --only-values -n trusted.menu21 "$dest/payload" 2>/dev/null) == trusted-retained ]]
    if [[ $receipt == regular ]]; then [[ $(cat "$dest/TRANSFER.sha256") == original ]]; else [[ -L $dest/TRANSFER.sha256 && $(readlink "$dest/TRANSFER.sha256") == "$work/external" ]]; fi
    echo "PASS $method receipt-$receipt, content, hidden files, hardlinks, sparse files, numeric ownership, user/trusted xattrs"
  done
done
# Cleanup errors after publication are warnings, with a usable result record.
real_rm=$(command -v rm)
for failed_command in rm rmdir; do
  cat > "$work/remote-bin/$failed_command" <<'STUB'
#!/bin/bash
case "$*" in *digest.pipe) exec /usr/bin/rm "$@" ;; esac
exit 9
STUB
  chmod +x "$work/remote-bin/$failed_command"
  SSH_KNOWN_HOSTS="$work/known_hosts" SOURCE_ID=fixture bash "$ROOT/tunnel-transfers.sh" -h 127.0.0.1 -p "$ssh_port" -k "$work/admin" -s "$work/source" -r "$work/output" -m ssh > "$work/cleanup.log" 2>&1 || { cat "$work/cleanup.log"; exit 1; }
  dest=$(awk -F '\t' '$1=="TRANSFER_RESULT" {print $2}' "$work/cleanup.log")
  [[ -d $dest && -s $dest.transfer.sha256 ]]
  cmp "$work/source/payload" "$dest/payload"
  grep -q 'WARNING: Transfer published successfully' "$work/cleanup.log"
  ! grep -q '^Transfer failed' "$work/cleanup.log" || exit 1
  "$real_rm" -- "$work/remote-bin/$failed_command"
  echo "PASS published transfer survives $failed_command cleanup failure"
done
# A publication failure itself must still fail, with no accepted result.
cat > "$work/remote-bin/mv" <<'STUB'
#!/bin/bash
exit 9
STUB
chmod +x "$work/remote-bin/mv"
if SSH_KNOWN_HOSTS="$work/known_hosts" SOURCE_ID=fixture bash "$ROOT/tunnel-transfers.sh" -h 127.0.0.1 -p "$ssh_port" -k "$work/admin" -s "$work/source" -r "$work/output" -m ssh > "$work/publish-fail.log" 2>&1; then exit 1; fi
! grep -q '^TRANSFER_RESULT' "$work/publish-fail.log" || exit 1
"$real_rm" -- "$work/remote-bin/mv"
echo 'PASS failed publication remains fatal'

# Real generation via an isolated agent, then rotation and revoke using explicit identity.
eval "$(ssh-agent -s)" >/dev/null
agent_pid=$SSH_AGENT_PID
ssh-add "$work/admin" >/dev/null 2>&1
touch "$work/reject-verification"
if HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" bash "$ROOT/keygen.sh" gen ed25519 127.0.0.1 "$ssh_port" root unverified '' unverified > "$work/unverified.log" 2>&1; then
  echo 'FAIL rejected authentication probe reported success'; exit 1
else
  [[ $? == 37 ]] || { cat "$work/unverified.log"; exit 1; }
fi
rm "$work/reject-verification"
[[ -s $work/local/.ssh/unverified.key && -s $work/local/.ssh/unverified.key.pub ]]
grep -qxF "$(cat "$work/local/.ssh/unverified.key.pub")" "$work/remote/.ssh/authorized_keys"
grep -q 'Public key installed, but authentication verification failed (37)' "$work/unverified.log"
grep -q 'Key pair retained:' "$work/unverified.log"
! grep -q 'Key enrolled and verified:' "$work/unverified.log" || exit 1
! grep -q 'BEGIN OPENSSH PRIVATE KEY' "$work/unverified.log" || exit 1
echo 'PASS installed-but-unverified key retains original status, installed public key and private recovery pair'
HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" bash "$ROOT/keygen.sh" gen ed25519 127.0.0.1 "$ssh_port" root old '' custom > "$work/gen.log" 2>&1 || { cat "$work/gen.log"; exit 1; }
old=$(cut -d' ' -f1,2 "$work/local/.ssh/custom.key.pub")
# A different key comment contains the old key; it must survive rotation and revocation.
ssh-keygen -q -t ed25519 -N '' -f "$work/unrelated" -C "mentions $old"
# Deliberately omit the final newline on the old key line.
printf '%s\n%s\n%s' "$(cat "$work/admin.pub")" "$(cat "$work/unrelated.pub")" "from=\"127.0.0.1\",restrict $(cat "$work/local/.ssh/custom.key.pub")" > "$work/remote/.ssh/authorized_keys"
# A valid local public key also need not have a final newline.
printf '%s' "$(cat "$work/local/.ssh/custom.key.pub")" > "$work/public-eof"
mv "$work/public-eof" "$work/local/.ssh/custom.key.pub"
HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" bash "$ROOT/keygen.sh" rotatekeys ed25519 127.0.0.1 "$ssh_port" root new custom > "$work/rotate.log" 2>&1 || { cat "$work/rotate.log"; exit 1; }
grep -q '^from="127.0.0.1",restrict .* new$' "$work/remote/.ssh/authorized_keys"
grep -qxF "$(cat "$work/unrelated.pub")" "$work/remote/.ssh/authorized_keys"
printf '%s' "$(cat "$work/local/.ssh/custom.key.pub")" > "$work/public-eof"
mv "$work/public-eof" "$work/local/.ssh/custom.key.pub"
HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" SSH_AUTH_SOCK='' bash "$ROOT/keygen.sh" remove "$work/local/.ssh/custom.key.pub" 127.0.0.1 "$ssh_port" root "$work/local/.ssh/custom.key" > "$work/remove.log" 2>&1
! grep -q ' new$' "$work/remote/.ssh/authorized_keys" || exit 1
grep -qxF "$(cat "$work/unrelated.pub")" "$work/remote/.ssh/authorized_keys"
echo 'PASS real custom-key enrollment, restricted rotation and explicit-identity revoke with EOF public keys; unrelated comment retained'
# Revoking the only remaining key would destroy the access needed to recover; refuse unless decommissioning.
cp "$work/remote/.ssh/authorized_keys" "$work/authorized-keys.saved"
# Comments and malformed lines are not keys: neither may satisfy the remaining-access check.
printf '# only key\ninvalid-key invalid-blob\n%s\n' "$(cat "$work/admin.pub")" > "$work/remote/.ssh/authorized_keys"
if HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" SSH_AUTH_SOCK='' bash "$ROOT/keygen.sh" remove "$work/admin.pub" 127.0.0.1 "$ssh_port" root "$work/admin" > "$work/last-key.log" 2>&1; then
  echo 'FAIL last authorized key revoked'; exit 1
fi
grep -q 'Refusing to revoke the last authorized key' "$work/last-key.log"
! grep -q 'Exact public key revoked' "$work/last-key.log" || exit 1
grep -qxF "$(cat "$work/admin.pub")" "$work/remote/.ssh/authorized_keys"
[[ ! -e $work/remote/.ssh/authorized_keys.new ]]
HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" SSH_AUTH_SOCK='' KEYGEN_ALLOW_LAST_KEY_REMOVAL=1 bash "$ROOT/keygen.sh" remove "$work/admin.pub" 127.0.0.1 "$ssh_port" root "$work/admin" > "$work/last-key-allowed.log" 2>&1 || { cat "$work/last-key-allowed.log"; exit 1; }
! grep -qxF "$(cat "$work/admin.pub")" "$work/remote/.ssh/authorized_keys" || exit 1
grep -q '^# only key$' "$work/remote/.ssh/authorized_keys"
grep -qx 'invalid-key invalid-blob' "$work/remote/.ssh/authorized_keys"
cp "$work/authorized-keys.saved" "$work/remote/.ssh/authorized_keys"
echo 'PASS last-key revocation refused by default and allowed only with explicit decommission override'
# Public-only FIDO fixtures need no authenticator: authenticate revocation with admin.
for sk_kind in ed25519 ecdsa; do
  perl -MMIME::Base64=encode_base64 -e '
    sub field { pack("N", length($_[0])).$_[0] }
    my ($kind, $body);
    if ($ARGV[0] eq "ed25519") {
      $kind="sk-ssh-ed25519\@openssh.com";
      $body=field(pack("C*", 0..31));
    } else {
      $kind="sk-ecdsa-sha2-nistp256\@openssh.com";
      $body=field("nistp256").field(pack("H*", "046b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c2964fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5"));
    }
    print "$kind ",encode_base64(field($kind).$body.field("ssh:"), "")," fido-fixture\n";
  ' "$sk_kind" > "$work/fido.pub"
  if ! ssh-keygen -lf "$work/fido.pub" >/dev/null 2>&1; then
    echo "SKIP $sk_kind FIDO: installed OpenSSH does not support this public-key type"
    continue
  fi
  cat "$work/fido.pub" >> "$work/remote/.ssh/authorized_keys"
  HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" SSH_AUTH_SOCK='' bash "$ROOT/keygen.sh" remove "$work/fido.pub" 127.0.0.1 "$ssh_port" root "$work/admin" > "$work/fido.log" 2>&1
  ! grep -qxF "$(cat "$work/fido.pub")" "$work/remote/.ssh/authorized_keys" || exit 1
  grep -qxF "$(cat "$work/admin.pub")" "$work/remote/.ssh/authorized_keys"
  grep -qxF "$(cat "$work/unrelated.pub")" "$work/remote/.ssh/authorized_keys"
  echo "PASS $sk_kind FIDO public-key revocation using independent authentication"
done
# A valid later key must not validate malformed fields on the selected first line.
printf 'invalid-key invalid-blob\n%s\n' "$(cat "$work/admin.pub")" > "$work/invalid.pub"
cp "$work/remote/.ssh/authorized_keys" "$work/before-invalid"
if HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" bash "$ROOT/keygen.sh" remove "$work/invalid.pub" 127.0.0.1 "$ssh_port" root "$work/admin" > "$work/invalid.log" 2>&1; then exit 1; fi
grep -q 'Invalid public key' "$work/invalid.log"
cmp "$work/before-invalid" "$work/remote/.ssh/authorized_keys"
echo 'PASS malformed selected key fields rejected before remote mutation'
for invalid in empty missing-blob malformed-eof; do
  case $invalid in
    empty) : > "$work/invalid.pub" ;;
    missing-blob) printf ssh-ed25519 > "$work/invalid.pub" ;;
    malformed-eof) printf 'ssh-ed25519 invalid-blob' > "$work/invalid.pub" ;;
  esac
  if HOME="$work/local" SSH_KNOWN_HOSTS="$work/known_hosts" bash "$ROOT/keygen.sh" remove "$work/invalid.pub" 127.0.0.1 "$ssh_port" root "$work/admin" > "$work/invalid.log" 2>&1; then exit 1; fi
  grep -q 'Invalid public key' "$work/invalid.log"
  cmp "$work/before-invalid" "$work/remote/.ssh/authorized_keys"
done
echo 'PASS empty/incomplete/malformed EOF public keys rejected without remote changes'
# Exercise a quoted forced-command option containing key-looking text with the same parser.
(
  # shellcheck disable=SC1090
  source <(sed -n '/^AUTH_KEYS_AWK=/,/^)/p' "$ROOT/keygen.sh")
  printf '%s\n' 'command="echo ssh-ed25519 OLD",restrict ssh-ed25519 OLD comment' 'ssh-ed25519 OTHER mentions ssh-ed25519 OLD' '# ssh-ed25519 OLD' > "$work/keys"
  awk "$AUTH_KEYS_AWK" replace 'ssh-ed25519 OLD' 'ssh-ed25519 NEW replacement' "$work/keys" > "$work/replaced"
  [[ $(cat "$work/replaced") == 'command="echo ssh-ed25519 OLD",restrict ssh-ed25519 NEW replacement' ]]
  awk "$AUTH_KEYS_AWK" remove 'ssh-ed25519 OLD' '' "$work/keys" > "$work/removed"
  [[ $(wc -l < "$work/removed") == 2 ]]
)
echo 'PASS quoted options and comments parsed without substring matches'
echo 'ALL PASS'
