#!/bin/bash

DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
LOGFILE="${CENTMINLOGDIR}/openssh_terrapin_mitigation_${DT}.log"

# Define the configuration lines to be added
CIPHERS_LINE="Ciphers aes128-ctr,aes192-ctr,aes256-ctr,aes128-gcm@openssh.com,aes256-gcm@openssh.com"
MACS_LINE="MACs umac-64@openssh.com,umac-128@openssh.com,hmac-sha2-256,hmac-sha2-512"

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  fi
fi

if [[ "$CENTOS_SEVEN" -ne '7' ]]; then
    echo "script is intended for CentOS 7 (EL7) systems only"
    exit 1
fi

# Function to handle logging based on verbose mode
log() {
    local message=$1
    if [ "$VERBOSE" = true ]; then
        echo "$message"
    fi
    echo "$message" >> "$LOGFILE"
}

# Function to handle backup and output based on verbose mode
backup() {
    log "Backup files"
    log "cp -a /etc/ssh/sshd_config \"/etc/ssh/sshd_config-b4-terrapin-fix-$DT\""
    log "cp -a /etc/ssh/ssh_config \"/etc/ssh/ssh_config-b4-terrapin-fix-$DT\""
    cp -a /etc/ssh/sshd_config "/etc/ssh/sshd_config-b4-terrapin-fix-$DT"
    cp -a /etc/ssh/ssh_config "/etc/ssh/ssh_config-b4-terrapin-fix-$DT"
}

# Function to remove existing configuration lines and provide output based on verbose mode
remove_existing_config() {
    local file=$1
    local setting=$2
    if grep -q "^$setting" "$file"; then
        log "Removing existing $setting from $file:"
        log "$(grep "^$setting" "$file")"
        log "Removed $setting from $file"
        sed -i "/^$setting/d" "$file"
    fi
}

# Function to append configuration if not already present and provide output based on verbose mode
append_config() {
    local file=$1
    local setting=$2
    local line=$3
    if ! grep -q "^$setting" "$file"; then
        echo "$line" >> "$file"
        log "Added $setting to $file"
    else
        log "$setting already set in $file"
    fi
}

# Function to display the Ciphers and MACs settings from the file based on verbose mode
display_config() {
    local file=$1
    log "Current Ciphers and MACs in $file:"
    log "$(grep -E '^(Ciphers|MACs)' "$file")"
}

# Parse command line arguments
VERBOSE=true
while getopts ":vs" opt; do
    case $opt in
        v)
            VERBOSE=true
            ;;
        s)
            VERBOSE=false
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done

# Create the log directory if it doesn't exist
mkdir -p "$CENTMINLOGDIR"

# Files to update
SSH_CONFIG="/etc/ssh/ssh_config"
SSHD_CONFIG="/etc/ssh/sshd_config"

# Backup configs
backup

# Remove existing Ciphers and MACs from ssh_config
remove_existing_config $SSH_CONFIG "Ciphers"
remove_existing_config $SSH_CONFIG "MACs"

# Remove existing Ciphers and MACs from sshd_config
remove_existing_config $SSHD_CONFIG "Ciphers"
remove_existing_config $SSHD_CONFIG "MACs"

# Append Ciphers and MACs to ssh_config
append_config $SSH_CONFIG "Ciphers" "$CIPHERS_LINE"
append_config $SSH_CONFIG "MACs" "$MACS_LINE"

# Append Ciphers and MACs to sshd_config
append_config $SSHD_CONFIG "Ciphers" "$CIPHERS_LINE"
append_config $SSHD_CONFIG "MACs" "$MACS_LINE"

# Display the updated configurations based on verbose mode
display_config $SSH_CONFIG
display_config $SSHD_CONFIG

# Restart SSHD to apply changes
log "Restarting SSHD service..."
sudo systemctl restart sshd

if [ "$VERBOSE" = false ]; then
    echo "OpenSSH Terrapin Mitigation Applied"
    echo "https://community.centminmod.com/threads/25045/"
    echo "log file saved: $LOGFILE"
else
    echo "SSHD service restarted."
fi