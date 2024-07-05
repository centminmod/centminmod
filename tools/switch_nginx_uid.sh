#!/bin/bash
################################################################################
# Script to update UIDs and GIDs for nginx user if they are set to 956
#
# This script performs the following operations:
# 1. Checks if the nginx service exists and stops it if it does.
# 2. Checks if the nginx user exists.
# 3. Attempts to use UID/GID 1068 first. If unavailable, finds the next available UID and GID.
# 4. If the nginx user exists and the current UID/GID is 956, updates their UIDs and GIDs to the determined number.
# 5. Updates the ownership of files associated with the old UIDs and GIDs.
# 6. Restarts the nginx service if it exists.
# 7. Verifies the changes by displaying the current UID and GID of the nginx user.
# 8. Verifies that the chown command was successful by checking file ownership.
#
# Note:
# - Ensure you have backups and have tested this script in a non-production environment before applying it to production systems.
# - Some errors related to /proc directories during file ownership changes can be safely ignored as they are transient and do not affect the actual file ownership changes.
################################################################################

# Current and desired UIDs and GIDs
CURRENT_UID=956
CURRENT_GID=956
DESIRED_UID=1068
DESIRED_GID=1068

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

# Change UID and GID if user exists and current UID/GID is 956
if user_exists nginx; then
  OLD_NGINX_UID=$(id -u nginx)
  OLD_NGINX_GID=$(id -g nginx)
  if [[ "$OLD_NGINX_UID" -eq "$CURRENT_UID" && "$OLD_NGINX_GID" -eq "$CURRENT_GID" ]]; then
    echo "Changing UID and GID of nginx user to $NGINX_UID..."
    groupmod -g "$NGINX_GID" nginx
    usermod -u "$NGINX_UID" -g "$NGINX_GID" nginx
    echo "Updating ownership of files for nginx user..."
    find / -user "$OLD_NGINX_UID" ! -path "/proc/*" -exec chown -h "$NGINX_UID:$NGINX_GID" {} \; 2>/dev/null
    find / -group "$OLD_NGINX_GID" ! -path "/proc/*" -exec chgrp -h "$NGINX_GID" {} \; 2>/dev/null
    # Ensure ownership of critical directories
    chown -R "$NGINX_UID:$NGINX_GID" /home/nginx
  else
    echo "nginx user UID/GID is not 956. No changes made."
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