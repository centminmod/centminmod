#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
#################################################################
# add sudo user script as discussed at
# http://stackoverflow.com/questions/8784761/adding-users-to-sudoers-through-shell-script?lq=1
# http://www.liquidweb.com/kb/how-to-add-a-user-and-grant-root-privileges-on-centos-7/
# http://www.liquidweb.com/kb/how-to-add-a-user-and-grant-root-privileges-on-centos-6-5/
#################################################################
# usage:
# 
# ./addsudousers.sh username
# 
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)

# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [[ "$(id -u)" != '0' ]]; then
  echo "error: run $0 as root" >&2
  exit 1
fi
if [[ "$#" -lt 1 ]]; then
  echo "usage: $0 username [username ...]" >&2
  exit 1
fi
if [ ! -d /etc/sudoers.d ]; then
  echo "error: /etc/sudoers.d does not exist" >&2
  exit 1
fi

# append a line to a file only if the exact line is not already there
add_line() {
  grep -qxF -- "$1" "$2" 2>/dev/null || echo "$1" >> "$2"
}

rc=0
for sudo_username in "$@"; do
  echo
  echo "Creating a sudo user & setting password for $sudo_username"
  echo
  # lowercase letter or underscore first, then lowercase letters, digits,
  # underscore or hyphen, max 32 chars. Rejects leading '-' (option
  # injection), '/' (path traversal) and '.' (sudo #includedir skips
  # file names containing a dot)
  if [[ ! "$sudo_username" =~ ^[a-z_][a-z0-9_-]{0,31}$ ]]; then
    echo "error: invalid username '$sudo_username' (allowed: ^[a-z_][a-z0-9_-]{0,31}\$)" >&2
    rc=1
    continue
  fi
  if id -u "$sudo_username" >/dev/null 2>&1; then
    echo "error: user $sudo_username already exists, skipping" >&2
    rc=1
    continue
  fi
  sudoers_file="/etc/sudoers.d/${sudo_username}"
  if [ -e "$sudoers_file" ]; then
    echo "error: $sudoers_file already exists, skipping $sudo_username" >&2
    rc=1
    continue
  fi

  if ! useradd -m -- "$sudo_username"; then
    echo "error: useradd failed for $sudo_username" >&2
    exit 1
  fi
  if ! usermod -aG wheel -- "$sudo_username"; then
    echo "error: usermod -aG wheel failed for $sudo_username" >&2
    exit 1
  fi
  if ! passwd "$sudo_username"; then
    echo "warning: password not set for $sudo_username, sudo will not work until you run: passwd $sudo_username" >&2
    rc=1
  fi

  # https://community.centminmod.com/posts/39469/
  user_home=$(getent passwd "$sudo_username" | cut -d: -f6)
  if [ -n "$user_home" ] && [ -d "$user_home" ]; then
    add_line "alias cmdir='pushd /usr/local/src/centminmod'" "${user_home}/.bash_profile"
    add_line "alias centmin='sudo centmin'" "${user_home}/.bashrc"
    add_line "alias cminfo='sudo cminfo'" "${user_home}/.bashrc"
    add_line "alias cmupdate='sudo cmupdate'" "${user_home}/.bashrc"
    add_line "alias pwdh='echo -n \"\$HOSTNAME\"; echo \" \$PWD\"'" "${user_home}/.bashrc"
    add_line "alias postfixlog='sudo pflogsumm -d today --verbose_msg_detail /var/log/maillog'" "${user_home}/.bashrc"
    chown "${sudo_username}:" "${user_home}/.bash_profile" "${user_home}/.bashrc"
  else
    echo "warning: home directory for $sudo_username not found, skipped shell aliases" >&2
    rc=1
  fi

  echo
  echo "sudo setup for $sudo_username"
  # write to a dotted temp file first: sudo's #includedir ignores file names
  # containing a '.', so an invalid rule is never live
  sudoers_tmp=$(mktemp /etc/sudoers.d/.addsudousers.XXXXXX) || exit 1
  echo "${sudo_username}    ALL=(ALL:ALL) ALL" > "$sudoers_tmp"
  # echo "${sudo_username}    ALL=(ALL:ALL) ALL: /usr/bin/centmin, /usr/bin/cminfo" > "$sudoers_tmp"
  chown root:root "$sudoers_tmp"
  chmod 0440 "$sudoers_tmp"
  if ! visudo -c -q -f "$sudoers_tmp"; then
    rm -f "$sudoers_tmp"
    echo "error: sudoers rule for $sudo_username failed visudo check, not installed" >&2
    exit 1
  fi
  mv -f "$sudoers_tmp" "$sudoers_file"
  # validate the whole sudoers policy with the new file included
  if ! visudo -c -q; then
    rm -f "$sudoers_file"
    visudo -c >&2
    echo "error: sudoers policy check failed, removed $sudoers_file" >&2
    exit 1
  fi
  echo
  echo "${sudo_username} sudo user setup at $sudoers_file"
done

exit "$rc"