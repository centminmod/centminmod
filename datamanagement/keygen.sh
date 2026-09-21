#!/bin/bash
# Keep old credentials until replacement authentication has been proved.
set -Eeuo pipefail
umask 077
source "$(dirname -- "$(readlink -f -- "$0")")/logging.sh"
datam_log_init ssh-key "$@"
fail() { echo "ERROR: $*" >&2; exit 1; }
quote_command() { local arg; printf -v command 'bash -c %q --' "$1"; shift; for arg in "$@"; do printf -v command '%s %q' "$command" "$arg"; done; }
validate_host() {
  [[ $host =~ ^[a-zA-Z0-9][a-zA-Z0-9.-]*$ && $port =~ ^[1-9][0-9]{0,4}$ && $port -le 65535 && $user =~ ^[a-zA-Z_][a-zA-Z0-9_-]*$ ]] || fail 'Invalid host, port or username.'
}
remote() { quote_command "$@"; "${SSH[@]}" -n "$user@$host" "$command"; }
# Parse only the key fields; quoted options and comments may contain key-looking text.
AUTH_KEYS_AWK=$(cat <<'AWK'
function token(   c,q,escaped,out) {
  while (substr($0,pos,1) ~ /[ \t]/ && pos<=length($0)) pos++
  start=pos; q=0; escaped=0; out=""
  for (;pos<=length($0);pos++) {
    c=substr($0,pos,1)
    if (!q && !escaped && c ~ /[ \t]/) break
    out=out c
    if (escaped) escaped=0
    else if (c=="\\" && q) escaped=1
    else if (c=="\"") q=!q
  }
  if (q || escaped) invalid=1
  return out
}
BEGIN {mode=ARGV[1]; split(ARGV[2],key," "); replacement=ARGV[3]; ARGV[1]=ARGV[2]=ARGV[3]=""}
/^[ \t]*(#|$)/ {if (mode=="remove") print; next}
{
  pos=1; invalid=0; first=token(); key_start=start
  if (first==key[1]) blob=token()
  else {first=token(); key_start=start; blob=token()}
  matched=(!invalid && first==key[1] && blob==key[2])
  if (matched) {
    count++
    if (mode=="replace") print substr($0,1,key_start-1) replacement
  } else if (mode=="remove") print
}
END {if (count<1 || (mode=="replace" && count!=1)) exit 1}
AWK
)
# Public-key installation via a sudo account. Password travels on stdin, never in shell source.
sudo_copy_key() {
  local pubfile=$1 host=$2 port=$3 user=$4 sudo_user=$5 sudo_pass=$6
  validate_host
  [[ $sudo_user =~ ^[a-zA-Z_][a-zA-Z0-9_-]*$ ]] || fail 'Invalid sudo user.'
  quote_command 'set -euo pipefail; umask 077
home_dir=$(getent passwd "$1" | cut -d: -f6); [[ -n $home_dir ]]
install -d -m 700 -o "$1" -- "$home_dir/.ssh"
auth="$home_dir/.ssh/authorized_keys"
[[ ! -L $auth ]]; touch "$auth"; chmod 600 "$auth"; chown "$1" "$auth"
grep -qxF -- "$2" "$auth" || printf "\n%s\n" "$2" >> "$auth"
' "$user" "$(cat -- "$pubfile")"
  printf '%s\n' "$sudo_pass" | SSHPASS="$sudo_pass" sshpass -e ssh -T -o StrictHostKeyChecking=accept-new -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -p "$port" "$sudo_user@$host" "sudo -S -p '' $command"
}
action=${1:-help}
if [[ $action == remove ]]; then
  datam_log_phase revoke
  pubfile=${2:-}; host=${3:-}; port=${4:-22}; user=${5:-root}
  validate_host
  [[ -f $pubfile ]] || fail 'Public key file missing.'
  read -r keytype keyblob _ < "$pubfile" || [[ -n $keytype && -n $keyblob ]] || fail 'Invalid public key.'
  [[ -n $keytype && -n $keyblob ]] || fail 'Invalid public key.'
  SSH=(ssh -T -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -o StrictHostKeyChecking=yes -o ConnectTimeout=10 -p "$port")
  identity=${6:-}
  if [[ -n $identity ]]; then
    [[ -f $identity && -r $identity ]] || fail 'Authentication private key missing.'
    SSH+=(-i "$identity" -o IdentitiesOnly=yes)
  fi
  printf '%s %s\n' "$keytype" "$keyblob" | ssh-keygen -lf - >/dev/null || fail 'Invalid public key.'
  remote 'set -euo pipefail; umask 077
cd "$HOME/.ssh"; [[ ! -L authorized_keys ]]
exec 8>.centmin-authorized-keys.lock; flock -x 8
cp -p authorized_keys "authorized_keys.before-revoke-$(date +%s)-$$"
awk "$2" remove "$1" "" authorized_keys > authorized_keys.new
if [[ $3 != 1 ]] && ! grep -qE "^[[:space:]]*([^#[:space:]].*[[:space:]])?(ssh-(rsa|dss|ed25519)|ecdsa-sha2-nistp[0-9]+|sk-(ssh-ed25519|ecdsa-sha2-nistp256)@openssh[.]com)[[:space:]]+[A-Za-z0-9+/]+=*" authorized_keys.new; then
  rm -f authorized_keys.new
  echo "ERROR: Refusing to revoke the last authorized key; authorized_keys is unchanged. Enroll a replacement key first, or set KEYGEN_ALLOW_LAST_KEY_REMOVAL=1 to decommission this account." >&2
  exit 1
fi
chmod 600 authorized_keys.new; mv authorized_keys.new authorized_keys
' "$keytype $keyblob" "$AUTH_KEYS_AWK" "${KEYGEN_ALLOW_LAST_KEY_REMOVAL:-0}"
  echo 'Exact public key revoked; remote recovery copy retained.'
  exit
fi
if [[ $action == enable-root ]]; then
  fail 'Automatic root-login/password changes removed. Use a normal SSH account with a writable backup directory, or explicitly configure PermitRootLogin prohibit-password after validating sshd -t.'
fi
[[ $action == gen || $action == rotatekeys ]] || fail 'Usage: keygen.sh gen TYPE HOST PORT USER COMMENT [EMPTY] [KEY_STEM] [SUDO_USER], or rotatekeys TYPE HOST PORT USER NEW_COMMENT KEY_STEM'
if [[ $action == gen && ( -n ${7:-} || -n ${10:-} ) ]]; then
  fail 'Password arguments are no longer accepted. Use KEYGEN_SSH_PASSWORD / KEYGEN_SUDO_PASSWORD or interactive authentication.'
fi
type=${2:-ed25519}; host=${3:-}; port=${4:-22}; user=${5:-root}; comment=${6:-centminmod-backup}
validate_host
[[ $comment != *$'\n'* && $comment != *$'\r'* ]] || fail 'Key comments must be one line.'
case $type in rsa) keyopts=(-t rsa -b 4096) ;; ecdsa) keyopts=(-t ecdsa -b 256) ;; ed25519) keyopts=(-t ed25519) ;; *) fail 'Invalid key type.' ;; esac
if [[ $action == rotatekeys ]]; then stem=${7:-}; else stem=${8:-my1}; fi
stem=${stem%.key}
[[ $stem =~ ^[a-zA-Z0-9][a-zA-Z0-9._-]*$ ]] || fail 'Specify an existing key filename stem for rotation.'
mkdir -p "$HOME/.ssh"
chmod 700 "$HOME/.ssh"
exec 9>"$HOME/.ssh/.centmin-keygen.lock"
flock -n 9 || fail 'Another key operation is running.'
SSH=(ssh -T -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -o StrictHostKeyChecking=accept-new -o ConnectTimeout=10 -p "$port")
if [[ $action == gen ]]; then
  key="$HOME/.ssh/$stem.key"
  if [[ -e $key ]]; then key="$HOME/.ssh/$stem-$(date -u +%Y%m%dT%H%M%SZ)-$$.key"; fi
  if [[ -n ${9:-} || -n ${KEYGEN_SSH_PASSWORD:-} ]]; then
    command -v sshpass >/dev/null || fail 'sshpass (EPEL) is required for password-assisted enrollment; install it or use interactive ssh-copy-id authentication.'
  fi
  datam_log_phase generate
  ssh-keygen "${keyopts[@]}" -N '' -f "$key" -C "$comment"
  datam_log_phase enroll
  if [[ -n ${9:-} ]]; then
    sudo_password=${KEYGEN_SUDO_PASSWORD:-}
    [[ -n $sudo_password ]] || { read -rsp 'Sudo account password: ' sudo_password; echo; }
    sudo_copy_key "$key.pub" "$host" "$port" "$user" "$9" "$sudo_password"
  elif [[ -n ${KEYGEN_SSH_PASSWORD:-} ]]; then
    SSHPASS="$KEYGEN_SSH_PASSWORD" sshpass -e ssh-copy-id -o StrictHostKeyChecking=accept-new -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -p "$port" -i "$key.pub" "$user@$host"
  else
    ssh-copy-id -o StrictHostKeyChecking=accept-new -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -p "$port" -i "$key.pub" "$user@$host"
  fi
  datam_log_phase verify-enrollment
  if "${SSH[@]}" -F /dev/null -o BatchMode=yes -o IdentitiesOnly=yes -o IdentityAgent=none -i "$key" "$user@$host" true; then
    :
  else
    verify_status=$?
    echo "ERROR: Public key installed, but authentication verification failed ($verify_status). Key pair retained: $key" >&2
    echo 'Review SSH connectivity, authorization restrictions and account policy before retrying. No SSH server policy was changed.' >&2
    exit "$verify_status"
  fi
  echo "Key enrolled and verified: $key"
else
  key="$HOME/.ssh/$stem.key"
  [[ -f $key && ! -L $key && -f $key.pub ]] || fail 'Existing key pair not found.'
  datam_log_phase rotation-prepare
  replacement="$key.new-$(date -u +%Y%m%dT%H%M%SZ)-$$"
  ssh-keygen "${keyopts[@]}" -N '' -f "$replacement" -C "$comment"
  read -r oldtype oldblob _ < "$key.pub" || [[ -n $oldtype && -n $oldblob ]] || fail 'Invalid existing public key.'
  old="$oldtype $oldblob"; new=$(cat -- "$replacement.pub")
  SSH+=(-o BatchMode=yes -o IdentitiesOnly=yes -o IdentityAgent=none -i "$key")
  # Copy the authorized line's restrictions onto the replacement, leaving old access valid.
  remote 'set -euo pipefail; umask 077
cd "$HOME/.ssh"; [[ -f authorized_keys && ! -L authorized_keys ]]
exec 8>.centmin-authorized-keys.lock; flock -x 8
cp -p authorized_keys "authorized_keys.before-rotation-$(date +%s)-$$"
awk "$3" replace "$1" "$2" authorized_keys > replacement.line
printf "\n" >> authorized_keys
cat replacement.line >> authorized_keys
rm replacement.line
' "$old" "$new" "$AUTH_KEYS_AWK"
  # A forced command may refuse this probe; preserve both keys and report failure in that case.
  datam_log_phase verify-replacement "candidate=$replacement"
  ssh -F /dev/null -T -o "UserKnownHostsFile=${SSH_KNOWN_HOSTS:-$HOME/.ssh/known_hosts}" -o StrictHostKeyChecking=yes -o BatchMode=yes -o IdentitiesOnly=yes -o IdentityAgent=none -o ConnectTimeout=10 -p "$port" -i "$replacement" "$user@$host" true
  # Make the proven replacement available at the original path before revocation.
  datam_log_phase activate-replacement
  recovery="$key.before-rotation-$(date +%s)-$$"
  cp -p -- "$key" "$recovery"; cp -p -- "$key.pub" "$recovery.pub"
  mv -- "$replacement" "$key"; mv -- "$replacement.pub" "$key.pub"
  datam_log_phase revoke-old-key "recovery=$recovery"
  remote 'set -euo pipefail; umask 077
cd "$HOME/.ssh"; exec 8>.centmin-authorized-keys.lock; flock -x 8
[[ ! -L authorized_keys ]]
awk "$2" remove "$1" "" authorized_keys > authorized_keys.new
chmod 600 authorized_keys.new; mv authorized_keys.new authorized_keys
' "$old" "$AUTH_KEYS_AWK"
  echo "Rotation verified. Recovery key retained locally: $recovery"
fi
ssh-keygen -lf "$key.pub"
