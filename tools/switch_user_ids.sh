#!/bin/bash
################################################################################
# Script to update UIDs and GIDs for nginx and memcached users
#
# This script performs the following operations:
# 1. Checks if the nginx and memcached services exist and stops them if they do.
# 2. Checks if the nginx and memcached users exist.
# 3. Checks if the desired UIDs and GIDs are available.
# 4. If the users exist and the UIDs and GIDs are available, updates their UIDs and GIDs to the specified values.
#    - Sets the UID and GID of nginx user to 956.
#    - Sets the UID and GID of memcached user to 957.
# 5. Updates the ownership of files associated with the old UIDs and GIDs.
#    - Changes file ownership from old UID to new UID 956 for nginx.
#    - Changes file ownership from old UID to new UID 957 for memcached.
# 6. Restarts the nginx and memcached services if they exist.
# 7. Verifies the changes by displaying the current UID and GID of nginx and memcached users.
# 8. Verifies that the chown command was successful by checking file ownership.
#
# Note:
# - Ensure you have backups and have tested this script in a non-production environment before applying it to production systems.
# - Some errors related to /proc directories during file ownership changes can be safely ignored as they are transient and do not affect the actual file ownership changes.
################################################################################
# Desired UIDs and GIDs
NGINX_UID=1000
NGINX_GID=1000
MEMCACHED_UID=958
MEMCACHED_GID=958
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

# Check if desired UIDs and GIDs are available
if ! id_available "$NGINX_UID"; then
  echo "Error: UID/GID $NGINX_UID is already in use. Please choose a different UID/GID."
  exit 1
fi

if ! id_available "$MEMCACHED_UID"; then
  echo "Error: UID/GID $MEMCACHED_UID is already in use. Please choose a different UID/GID."
  exit 1
fi

# Stop services if they exist
if service_exists nginx; then
  echo "Stopping nginx service..."
  systemctl stop nginx
else
  echo "nginx service does not exist."
fi

if service_exists memcached; then
  echo "Stopping memcached service..."
  systemctl stop memcached
else
  echo "memcached service does not exist."
fi

# Change UIDs and GIDs if users exist and update ownership of files
if user_exists nginx; then
  OLD_NGINX_UID=$(id -u nginx)
  OLD_NGINX_GID=$(id -g nginx)
  echo "Changing UID and GID of nginx user to $NGINX_UID..."
  groupmod -g "$NGINX_GID" nginx
  usermod -u "$NGINX_UID" -g "$NGINX_GID" nginx

  echo "Updating ownership of files for nginx user..."
  find / -user "$OLD_NGINX_UID" ! -path "/proc/*" -exec chown -h "$NGINX_UID:$NGINX_GID" {} \; 2>/dev/null
  find / -group "$OLD_NGINX_GID" ! -path "/proc/*" -exec chgrp -h "$NGINX_GID" {} \; 2>/dev/null

  # Ensure ownership of critical directories
  chown -R "$NGINX_UID:$NGINX_GID" /home/nginx
else
  echo "nginx user does not exist."
fi

if user_exists memcached; then
  OLD_MEMCACHED_UID=$(id -u memcached)
  OLD_MEMCACHED_GID=$(id -g memcached)
  echo "Changing UID and GID of memcached user to $MEMCACHED_UID..."
  groupmod -g "$MEMCACHED_GID" memcached
  usermod -u "$MEMCACHED_UID" -g "$MEMCACHED_GID" memcached

  echo "Updating ownership of files for memcached user..."
  find / -user "$OLD_MEMCACHED_UID" ! -path "/proc/*" -exec chown -h "$MEMCACHED_UID:$MEMCACHED_GID" {} \; 2>/dev/null
  find / -group "$OLD_MEMCACHED_GID" ! -path "/proc/*" -exec chgrp -h "$MEMCACHED_GID" {} \; 2>/dev/null

  # Ensure ownership of critical directories
  # chown -R "$MEMCACHED_UID:$MEMCACHED_GID" /home/memcached /var/cache/memcached
else
  echo "memcached user does not exist."
fi

# Restart services if they exist
if service_exists nginx; then
  echo "Restarting nginx service..."
  systemctl start nginx
else
  echo "nginx service does not exist."
fi

if service_exists memcached; then
  echo "Restarting memcached service..."
  systemctl start memcached
else
  echo "memcached service does not exist."
fi

# Verification: Check ownership of critical directories
echo "Verifying ownership of critical directories..."
verify_ownership /home/nginx "$NGINX_UID" "$NGINX_GID"

# Verify the changes
echo
echo "id nginx"
id nginx
echo
echo "id memcached"
id memcached
