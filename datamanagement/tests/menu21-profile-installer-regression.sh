#!/bin/bash
# Local fixtures only: no credentials, network, package installs or system-path writes.
set -Eeuo pipefail
umask 077
ROOT=$(cd "$(dirname "$0")/.." && pwd)
work=$(mktemp -d "${TMPDIR:-/var/tmp}/menu21-profile-installer.XXXXXX")
trap 'rm -rf -- "$work"' EXIT
trap 'exit 130' INT TERM
source "$ROOT/../inc/datamanager.inc"
aws() { return 0; }
export AWS_CONFIG_FILE="$work/config" AWS_SHARED_CREDENTIALS_FILE="$work/credentials"

# Match AWS config header quoting/comments without rewriting unrelated bytes.
printf '# preamble\r\n[default] # default\r\nregion = us-east-1\r\n' > "$work/before"
printf '[profile "selected"] # quoted profile\r\nrole_arn = fixture\r\nsource_profile = base\r\n' > "$work/selected"
printf '[profile adjacent] ; other profile\r\nregion = eu-west-1' > "$work/after"
cat "$work/before" "$work/selected" "$work/after" > "$AWS_CONFIG_FILE"
datam_profile_section "$AWS_CONFIG_FILE" 'profile selected' export > "$work/export"
cmp "$work/selected" "$work/export"
datam_profile_section "$AWS_CONFIG_FILE" 'profile selected' remove > "$work/removed"
cat "$work/before" "$work/after" > "$work/expected"
cmp "$work/expected" "$work/removed"
for header in '[profile selected] # comment' "[profile 'selected'] ; comment" $'[profile\t "selected" \t]'; do
  printf '%s\nregion = us-east-1\n' "$header" > "$work/header"
  datam_profile_section "$work/header" 'profile selected' export > "$work/export"
  cmp "$work/header" "$work/export"
done
for header in '[profile "sele\cted"]' '[profile sele"cted"]'; do
  printf '%s\nregion = us-east-1\n' "$header" > "$work/header"
  datam_profile_section "$work/header" 'profile selected' remove > "$work/export"
  cmp "$work/header" "$work/export"
done
for header in '[default] # comment' '[profile "default"]'; do
  printf '%s\nregion = us-east-1\n' "$header" > "$work/header"
  datam_profile_section "$work/header" 'profile default' export > "$work/export"
  cmp "$work/header" "$work/export"
done
printf '[selected] # credential comment\naws_access_key_id = fixture\n' > "$AWS_SHARED_CREDENTIALS_FILE"
datam_profile_section "$AWS_SHARED_CREDENTIALS_FILE" selected export > "$work/export"
cmp "$AWS_SHARED_CREDENTIALS_FILE" "$work/export"
if [[ -e /dev/full ]]; then
  if datam_profile_section "$AWS_CONFIG_FILE" 'profile selected' export > /dev/full 2> "$work/full.log"; then
    echo 'FAIL full output device reported successful profile export'; exit 1
  fi
  grep -Eq 'Cannot (write|flush) profile output' "$work/full.log"
  echo 'PASS profile output write/flush failure propagation'
fi
echo 'PASS quoted/commented/default profile headers and byte-preserving section removal'

# An apparent header inside a multiline credential command belongs to its value.
cat > "$work/continuation-other" <<'CONFIG'
[profile other]
credential_process = /bin/sh -c 'cat >&2 <<EOF
    [profile selected]
    EOF
    printf "%s\n" "{\"Version\":1,\"AccessKeyId\":\"fixture\",\"SecretAccessKey\":\"fixture\"}"'

# Comments and blank lines do not end a multiline value.
region = us-east-1
CONFIG
printf '[profile selected]\nregion = eu-west-1\n' > "$work/continuation-selected"
cat "$work/continuation-other" "$work/continuation-selected" > "$work/continuation-config"
datam_profile_section "$work/continuation-config" 'profile selected' remove > "$work/removed"
cmp "$work/continuation-other" "$work/removed"
datam_profile_section "$work/continuation-config" 'profile selected' export > "$work/export"
cmp "$work/continuation-selected" "$work/export"
datam_profile_section "$work/continuation-config" 'profile other' export > "$work/export"
cmp "$work/continuation-other" "$work/export"
printf '[profile other]\nvalue = first\n\n# comment\n\t[profile selected]\n' > "$work/continuation-other"
cat "$work/continuation-other" "$work/continuation-selected" > "$work/continuation-config"
datam_profile_section "$work/continuation-config" 'profile selected' remove > "$work/removed"
cmp "$work/continuation-other" "$work/removed"
printf '  [profile selected]\n  region = eu-west-1\n' > "$work/indented-header"
datam_profile_section "$work/indented-header" 'profile selected' export > "$work/export"
cmp "$work/indented-header" "$work/export"
echo 'PASS multiline continuation precedence, tabs/comments and genuine indented headers'

rm "$AWS_SHARED_CREDENTIALS_FILE"
# File-only profile operations must work with no AWS CLI at all: hide it from command -v.
export TMPDIR="$work/tmp"; mkdir "$TMPDIR"
(
  unset -f aws
  command() { if [[ ${1:-} == -v && ${2:-} == aws ]]; then return 1; fi; builtin command "$@"; }
  printf 'selected\n%s\n' "$work/role-export" | datam_profile_action 6 > "$work/role.log" 2>&1
  cmp "$work/selected" "$work/role-export/config"
  grep -q 'Profile exported' "$work/role.log"
  if printf 'absent\n%s\n' "$work/missing-export" | datam_profile_action 6 > "$work/missing.log" 2>&1; then
    echo 'FAIL missing profile export reported success'; exit 1
  fi
  grep -q 'Profile not found' "$work/missing.log"
  ! grep -q 'Profile exported' "$work/missing.log" || exit 1
  [[ ! -e $work/missing-export ]]
  # A failing destination must not leave extracted credentials in the temporary directory.
  # Not under if/||: those contexts disable errexit inside the action, unlike the real menu call.
  (
    set +e
    printf 'selected\n%s\n' "$work/absent-parent/export" | datam_profile_action 6 > "$work/dest-failure.log" 2>&1
    [[ $? != 0 ]] || { echo 'FAIL export into a missing parent reported success'; exit 1; }
  )
  ! grep -q 'Profile exported' "$work/dest-failure.log" || exit 1
  [[ ! -e $work/absent-parent && -z $(find "$TMPDIR" -maxdepth 1 -name 'aws-profile-export.*' -print -quit) ]]
  mv "$AWS_CONFIG_FILE" "$work/saved-config"
  if printf '%s\n' "$work/missing-backup" | datam_profile_action 7 > "$work/backup.log" 2>&1; then
    echo 'FAIL missing AWS files reported successful backup'; exit 1
  fi
  grep -q 'No AWS profile files found' "$work/backup.log"
  [[ ! -e $work/missing-backup ]]
  if printf 'selected\n' | datam_profile_action 2 > "$work/no-cli.log" 2>&1; then
    echo 'FAIL CLI-backed option ran without the AWS CLI'; exit 1
  fi
  grep -q 'Install AWS CLI' "$work/no-cli.log"
)
echo 'PASS config-only role export, missing-profile/files/destination failures without the AWS CLI, CLI-backed option gated'

# Run the actual installer function with only its system directories relocated.
# The wget adapter reproduces default suffixing and HTTP failure without networking.
mkdir -p "$work/download" "$work/bin" "$work/payload"
printf '#!/bin/bash\necho fixture-version\n' > "$work/payload/s5cmd"
chmod +x "$work/payload/s5cmd"
tar czf "$work/release.tar.gz" -C "$work/payload" s5cmd
# shellcheck disable=SC1090
source <(sed -n '/^sfive_cmd()/,/^}/p' "$ROOT/awscli-get.sh" |
  sed "s|/root/tools/awscli|$work/download|g; s|/usr/local/bin|$work/bin|g")
SFIVECMD_VER=fixture
wget() {
  local output='' url='' arg
  while (( $# )); do
    arg=$1; shift
    case $arg in -O) output=$1; shift ;; -*) ;; *) url=$arg ;; esac
  done
  [[ ${WGET_FIXTURE_FAIL:-0} == 0 ]] || return 8
  if [[ -z $output ]]; then output=${url##*/}; [[ ! -e $output ]] || output+=.1; fi
  cp "$work/release.tar.gz" "$output"
}
archive="$work/download/s5cmd_fixture_Linux-64bit.tar.gz"
printf corrupt > "$archive"
(sfive_cmd) > "$work/install.log" 2>&1
grep -q fixture-version "$work/install.log"
[[ -x $work/bin/s5cmd && ! -e $archive && ! -e $archive.1 ]]
rm "$work/bin/s5cmd"
set +e
(export WGET_FIXTURE_FAIL=1; sfive_cmd) > "$work/fail.log" 2>&1
rc=$?
set -e
[[ $rc == 8 && ! -e $work/bin/s5cmd ]]
! grep -q fixture-version "$work/fail.log" || exit 1
echo 'PASS stale s5cmd archive replacement and download-error propagation'

# Exercise actual getcli/R2 functions with a local stateful configure adapter.
# Both ambient profile selectors must leave unrelated credentials/config intact.
mkdir -p "$work/aws-bin" "$work/aws-state/profile-env" "$work/aws-state/default-env"
cat > "$work/aws-bin/aws" <<'AWS'
#!/bin/bash
set -eu
[[ $1 == configure ]]
[[ $2 != list-profiles ]] || { ls "$FIXTURE_AWS_DATA"; exit 0; }
if [[ $2 == --profile ]]; then
  # Wizard form: four stdin lines, never an argument carrying the secret.
  profile=$3
  IFS= read -r wizard_key; IFS= read -r wizard_secret; IFS= read -r wizard_region; IFS= read -r wizard_output
  printf 'y %s wizard argv=[%s]\n' "$profile" "$*" >> "$FIXTURE_AWS_CALLS"
  mkdir -p "$FIXTURE_AWS_DATA/$profile"
  printf '%s\n' "$wizard_key" > "$FIXTURE_AWS_DATA/$profile/aws_access_key_id"
  printf '%s\n' "$wizard_secret" > "$FIXTURE_AWS_DATA/$profile/aws_secret_access_key"
  printf '%s\n' "$wizard_region" > "$FIXTURE_AWS_DATA/$profile/region"
  printf '%s\n' "$wizard_output" > "$FIXTURE_AWS_DATA/$profile/output"
  exit 0
fi
action=$2 key=$3; shift 3
value=''
if [[ $action == set ]]; then value=$1; shift; fi
profile=${AWS_DEFAULT_PROFILE:-${AWS_PROFILE:-default}}
explicit=n
while (( $# )); do
  [[ $1 == --profile && $# -ge 2 ]]
  profile=$2 explicit=y; shift 2
done
printf '%s %s %s %s\n' "$explicit" "$profile" "$action" "$key" >> "$FIXTURE_AWS_CALLS"
if [[ $key == default.* ]]; then profile=default; key=${key#default.}; fi
file="$FIXTURE_AWS_DATA/$profile/$key"
if [[ $action == get ]]; then [[ -f $file ]] || exit 1; cat "$file"
else mkdir -p "${file%/*}"; printf '%s\n' "$value" > "$file"; fi
AWS
chmod +x "$work/aws-bin/aws"
printf preserved-profile > "$work/aws-state/profile-env/aws_access_key_id"
printf preserved-default-env > "$work/aws-state/default-env/aws_access_key_id"
cp -R "$work/aws-state" "$work/aws-state-before"
export FIXTURE_AWS_DATA="$work/aws-state" FIXTURE_AWS_CALLS="$work/aws-calls"
# shellcheck disable=SC1090
source <(sed -n '/^getcli() {/,/^}/p; /^r2_config() {/,/^}/p; /^configure_profile() {/,/^}/p' "$ROOT/awscli-get.sh")
yum() { :; }
aws() { "$work/aws-bin/aws" "$@"; }
AWSCLI_BINPATH="$work/aws-bin"
for ambient in profile default-profile both; do
  for requested in default defaultreset; do
    rm -rf "$work/aws-state/default"
    (
      unset AWS_PROFILE AWS_DEFAULT_PROFILE
      [[ $ambient == default-profile ]] || export AWS_PROFILE=profile-env
      [[ $ambient == profile ]] || export AWS_DEFAULT_PROFILE=default-env
      skipconfig=''
      # The standalone installer deliberately does not enable nounset.
      set +u
      getcli install "$requested" fixture-key fixture-secret auto json
    ) > "$work/default-setup.log" 2>&1
    [[ $(cat "$work/aws-state/default/aws_access_key_id") == fixture-key ]]
    [[ $(cat "$work/aws-state/default/aws_secret_access_key") == fixture-secret ]]
    [[ $(cat "$work/aws-state/default/region") == auto ]]
    [[ $(cat "$work/aws-state/default/output") == json ]]
    [[ $(cat "$work/aws-state/default/s3.max_concurrent_requests") == 2 ]]
    diff -r "$work/aws-state-before/profile-env" "$work/aws-state/profile-env"
    diff -r "$work/aws-state-before/default-env" "$work/aws-state/default-env"
  done
done
! grep -v '^y default ' "$FIXTURE_AWS_CALLS" || exit 1
echo 'PASS explicit default/defaultreset profile selection and R2 settings with ambient profiles preserved'
# Existing credentials must win even when the conventional files are absent.
# Reuse the stateful CLI adapter; filesystem probes must not override its result.
cp -R "$work/aws-state/default" "$work/default-path-baseline"
for layout in alternate home credentials-only; do
  (
    rm -rf "$work/aws-state/default"
    cp -R "$work/default-path-baseline" "$work/aws-state/default"
    export HOME="$work/home-$layout"
    mkdir -p "$HOME"
    if [[ $layout == home ]]; then
      unset AWS_CONFIG_FILE AWS_SHARED_CREDENTIALS_FILE
    else
      export AWS_CONFIG_FILE="$work/$layout/config" AWS_SHARED_CREDENTIALS_FILE="$work/$layout/credentials"
      mkdir -p "$work/$layout"
      touch "$AWS_SHARED_CREDENTIALS_FILE"
      [[ $layout == credentials-only ]] || touch "$AWS_CONFIG_FILE"
    fi
    config_path=${AWS_CONFIG_FILE:-$HOME/.aws/config}
    credentials_path=${AWS_SHARED_CREDENTIALS_FILE:-$HOME/.aws/credentials}
    cp -R "$work/aws-state/default" "$work/default-before-$layout"
    : > "$FIXTURE_AWS_CALLS"
    set +u
    getcli install default replacement-key replacement-secret us-west-2 text > "$work/preserve-$layout.log"
    ! grep -Eq ' (set|wizard) ' "$FIXTURE_AWS_CALLS" || exit 1
    diff -r "$work/default-before-$layout" "$work/aws-state/default"
    grep -q 'skipping configuration' "$work/preserve-$layout.log"
    grep -qF "in config file: $config_path" "$work/preserve-$layout.log"
    grep -qF "in credential file: $credentials_path" "$work/preserve-$layout.log"
    getcli install defaultreset replacement-key replacement-secret us-west-2 text > "$work/reset-$layout.log"
    [[ $(cat "$work/aws-state/default/aws_access_key_id") == replacement-key ]]
    [[ $(cat "$work/aws-state/default/aws_secret_access_key") == replacement-secret ]]
    [[ $(cat "$work/aws-state/default/region") == us-west-2 ]]
    [[ $(cat "$work/aws-state/default/output") == text ]]
    diff -r "$work/aws-state-before/profile-env" "$work/aws-state/profile-env"
    diff -r "$work/aws-state-before/default-env" "$work/aws-state/default-env"
  ) > "$work/path-$layout.log" 2>&1
done
echo 'PASS existing default credentials preserved across alternate/home/missing config paths; explicit reset remains available'
# R2 tuning must use the installed binary path: sudo secure_path and cron omit /usr/local/bin.
mkdir "$work/nopath"
printf '#!/bin/bash\necho "bare aws invoked: $*" >&2\nexit 127\n' > "$work/nopath/aws"
chmod +x "$work/nopath/aws"
for requested in r2-named defaultreset; do
  (
    unset -f aws
    rm -rf "$work/aws-state/default" "$work/aws-state/r2-named"
    : > "$FIXTURE_AWS_CALLS"
    set +u
    PATH="$work/nopath:$PATH" getcli install "$requested" fixture-key fixture-secret r2 json > "$work/r2-$requested.log" 2>&1
    profile=default; [[ $requested == defaultreset ]] || profile=$requested
    [[ $(cat "$work/aws-state/$profile/region") == auto ]]
    [[ $(cat "$work/aws-state/$profile/s3.addressing_style") == path ]]
    [[ $(cat "$work/aws-state/$profile/s3.multipart_chunksize") == 50MB ]]
    ! grep -q 'bare aws invoked' "$work/r2-$requested.log" || exit 1
    # Every credential write used the wizard; no recorded argument, setter or wizard, ever carried the secret.
    ! grep -q 'set aws_secret_access_key' "$FIXTURE_AWS_CALLS" || exit 1
    ! grep -q 'fixture-secret' "$FIXTURE_AWS_CALLS" || exit 1
    grep -q " $profile wizard argv=\[configure --profile $profile\]" "$FIXTURE_AWS_CALLS"
  ) || { cat "$work/r2-$requested.log"; exit 1; }
done
echo 'PASS R2 tuning through the installed binary path for named and default profiles; secrets only via wizard stdin'
# The standalone entrypoint refuses credentials on its command line before doing anything.
if CENTMINLOGDIR="$work/argv-logs" bash "$ROOT/awscli-get.sh" install fixture-argv fixture-key fixture-secret us-east-1 json > "$work/argv.log" 2>&1; then
  echo 'FAIL command-line credentials accepted'; exit 1
fi
grep -q 'Credential arguments are no longer accepted' "$work/argv.log"
! grep -q 'fixture-secret' "$work/argv.log" || exit 1
[[ ! -e $work/aws-state/fixture-argv && ! -e $work/argv-logs ]]
echo 'PASS command-line credential arguments rejected before logging or configuration'
echo 'ALL PASS'
