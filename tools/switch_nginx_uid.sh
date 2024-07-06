#!/bin/bash
################################################################################
# Script to update UIDs and GIDs for nginx user if they are set to specific values
################################################################################

# Function to display usage
usage() {
    echo "Usage: $0 --current-uid <uid> --current-gid <gid> --desired-uid <uid> --desired-gid <gid>"
    echo "   or: $0 -uf <uid> -gf <gid> -ut <uid> -gt <gid>"
    exit 1
}

# Initialize variables
CURRENT_UID=""
CURRENT_GID=""
DESIRED_UID=""
DESIRED_GID=""

# Manual parsing of arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -uf|--current-uid)
            CURRENT_UID="$2"
            shift 2
            ;;
        -gf|--current-gid)
            CURRENT_GID="$2"
            shift 2
            ;;
        -ut|--desired-uid)
            DESIRED_UID="$2"
            shift 2
            ;;
        -gt|--desired-gid)
            DESIRED_GID="$2"
            shift 2
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Check if all required arguments are provided
if [ -z "$CURRENT_UID" ] || [ -z "$CURRENT_GID" ] || [ -z "$DESIRED_UID" ] || [ -z "$DESIRED_GID" ]; then
    echo "Error: All arguments must be provided."
    usage
fi

SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
SCRIPT_SOURCEBASE=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
# account for tools directory placement of tools/setio.sh
SCRIPT_DIR=$(readlink -f $(dirname ${SCRIPT_DIR}))

################################################################################
# Function to verify uid/gid changes
function verify_ownership {
  local path=$1
  local expected_uid=$2
  local expected_gid=$3
  local current_uid
  local current_gid
  current_uid=$(stat -c %u "$path")
  current_gid=$(stat -c %g "$path")
  if [[ "$current_uid" -eq "$expected_uid" && "$current_gid" -eq "$expected_gid" ]]; then
    echo "Ownership of $path is correct: UID=$current_uid, GID=$current_gid"
  else
    echo "Ownership of $path is incorrect: UID=$current_uid, GID=$current_gid (expected UID=$expected_uid, GID=$expected_gid)"
  fi
}

# Function to check if a service exists
function service_exists {
  systemctl list-units --type=service --all | grep -q "$1.service"
}

# Function to check if a user exists
function user_exists {
  id "$1" &>/dev/null
}

# Function to check if a UID or GID is available
function id_available {
  ! getent passwd "$1" &>/dev/null && ! getent group "$1" &>/dev/null
}

# Function to find the next available UID/GID
function find_next_available_id {
  local start_id=$1
  local current_id=$start_id
  while ! id_available "$current_id"; do
    ((current_id++))
  done
  echo "$current_id"
}

# Attempt to use desired UID/GID first, if unavailable find the next available
if id_available "$DESIRED_UID"; then
  NGINX_UID=$DESIRED_UID
  NGINX_GID=$DESIRED_GID
else
  echo "Desired UID/GID $DESIRED_UID is already in use. Finding next available ID."
  NGINX_UID=$(find_next_available_id $((DESIRED_UID + 1)))
  NGINX_GID=$NGINX_UID
fi

echo "Using UID/GID: $NGINX_UID"

# Stop nginx service if it exists
if service_exists nginx; then
  echo "Stopping nginx service..."
  systemctl stop nginx
else
  echo "nginx service does not exist."
fi

# Change UID and GID if user exists and current UID/GID matches specified values
if user_exists nginx; then
  OLD_NGINX_UID=$(id -u nginx)
  OLD_NGINX_GID=$(id -g nginx)
  if [[ "$OLD_NGINX_UID" -eq "$CURRENT_UID" && "$OLD_NGINX_GID" -eq "$CURRENT_GID" ]]; then
    echo "Changing UID and GID of nginx user from $OLD_NGINX_UID:$OLD_NGINX_GID to $NGINX_UID:$NGINX_GID..."
    groupmod -g "$NGINX_GID" nginx
    usermod -u "$NGINX_UID" -g "$NGINX_GID" nginx
    echo "Updating ownership of files for nginx user..."
    find / -user "$OLD_NGINX_UID" ! -path "/proc/*" -exec chown -h "$NGINX_UID:$NGINX_GID" {} \; 2>/dev/null
    find / -group "$OLD_NGINX_GID" ! -path "/proc/*" -exec chgrp -h "$NGINX_GID" {} \; 2>/dev/null
    # Ensure ownership of critical directories
    chown -R "$NGINX_UID:$NGINX_GID" /home/nginx
    # Update pure-ftpd database created virtual FTP user's uid/gid as well
    echo
    ${SCRIPT_DIR}/tools/switch_pureftpd_uid.sh -u "$NGINX_UID" -g "$NGINX_GID"
  else
    echo "nginx user UID/GID is $OLD_NGINX_UID:$OLD_NGINX_GID, not $CURRENT_UID:$CURRENT_GID. No changes made."
  fi
else
  echo "nginx user does not exist."
fi

# Restart nginx service if it exists
if service_exists nginx; then
  echo "Restarting nginx service..."
  systemctl start nginx
else
  echo "nginx service does not exist."
fi

# Verification: Check ownership of critical directories
echo "Verifying ownership of critical directories..."
verify_ownership /home/nginx "$NGINX_UID" "$NGINX_GID"

# Verify the changes
echo
echo "id nginx"
id nginx