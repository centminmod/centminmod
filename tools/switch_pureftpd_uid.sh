#!/bin/bash
################################################################################
# change all pure-ftpd virtual FTP's assigned nginx uid/gid
# 
# /user/local/src/centminmod/tools/switch_pureftpd_uid.sh -u 1068 -g 1068
################################################################################

# Default target UID and GID
TARGET_UID=1068
TARGET_GID=1068

# Parse command-line arguments for target UID and GID
while getopts "u:g:" opt; do
  case $opt in
    u) TARGET_UID=$OPTARG ;;
    g) TARGET_GID=$OPTARG ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
  esac
done

# List all Pure-FTPD users
pure_pwl_list=$(pure-pw list)

# Iterate through each user
echo "$pure_pwl_list" | while IFS= read -r line; do
    # Extract the username
    username=$(echo "$line" | awk '{print $1}')
    # Show user details
    user_info=$(pure-pw show "$username")
    # Extract UID and GID
    user_uid=$(echo "$user_info" | grep "UID" | awk '{print $3}')
    user_gid=$(echo "$user_info" | grep "GID" | awk '{print $3}')
    # Check if UID and GID do not match the target UID/GID
    if [[ "$user_uid" -ne "$TARGET_UID" || "$user_gid" -ne "$TARGET_GID" ]]; then
        echo "Updating from UID=$user_uid GID=$user_gid to UID=$TARGET_UID GID=$TARGET_GID for $username..."
        # Update the user's UID and GID
        pure-pw usermod "$username" -u "$TARGET_UID" -g "$TARGET_GID"
        # Rebuild the Pure-FTPD database after each change
        pure-pw mkdb
    else
        echo "User $username already has UID/GID $TARGET_UID/$TARGET_GID"
    fi
done

echo "All pure-ftpd virtual FTP users have been processed."
