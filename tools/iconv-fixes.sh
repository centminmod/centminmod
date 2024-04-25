#!/bin/bash
#########################################################################
# iconv mitigation fix for CVE-2024-2961
# https://nvd.nist.gov/vuln/detail/CVE-2024-2961
#########################################################################

# Define variables for logging
DT=$(date +"%Y%m%d-%H%M%S")
LOGDIR='/root/centminlogs'
LOGFILE="${LOGDIR}/iconv_fix_${DT}.log"

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
KERNEL_NUMERICVER=$(uname -r | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }')

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
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
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

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
    CONF_FILES=("/usr/lib64/gconv/gconv-modules")
else
    CONF_FILES=("/usr/lib64/gconv/gconv-modules.d/gconv-modules-extra.conf")
fi

# Function to log messages
log() {
    if [[ "$VERBOSE" == true ]]; then
        echo "$1"
    fi
    echo "$1" >> "$LOGFILE"
}

# Parse command line options for verbose or silent mode
VERBOSE=true
while getopts "vs" opt; do
    case $opt in
        v) VERBOSE=true ;;
        s) VERBOSE=false ;;
        *) echo "Invalid option: -$OPTARG" >&2; exit 1 ;;
    esac
done

# Create log directory if it doesn't exist
mkdir -p "$LOGDIR"

# Process each file
for FILE in "${CONF_FILES[@]}"; do
    if [ -f "$FILE" ]; then
        log "Processing file: $FILE"
        
        # Check if already mitigated
        if grep -qE '^#.*ISO-2022-CN-EXT' "$FILE"; then
            log "Mitigation already applied in $FILE, no further action needed."
            log "Info: https://community.centminmod.com/threads/25069/"
            if [[ "$VERBOSE" == false ]]; then
                log "No action needed. Mitigation already applied. Details logged in $LOGFILE"
                log "Info: https://community.centminmod.com/threads/25069/"
            fi
            exit 0
        fi
        
        TIMESTAMP=$(date +%Y%m%d-%H%M%S)
        BACKUP_FILE="${FILE}.${TIMESTAMP}"
        cp "$FILE" "$BACKUP_FILE"
        log "Backup of $FILE created at $BACKUP_FILE."

        # Display current state of relevant charset entries
        log "Before modification:"
        if [[ "$VERBOSE" == true ]]; then
            grep -E 'ISO-2022-CN-EXT' "$FILE" | tee -a "$LOGFILE"
        else
            grep -E 'ISO-2022-CN-EXT' "$FILE" >> "$LOGFILE"
        fi

        # Modify the file
        sudo sed -i -e '/ISO-2022-CN-EXT\// s/^/#/' "$FILE"
        log "Modifications applied to $FILE."

        # Regenerate iconv cache if iconvconfig is available
        if hash iconvconfig 2>/dev/null; then
            sudo iconvconfig
            log "iconv cache regenerated."
        else
            log "iconvconfig not available, no cache regenerated."
        fi

        # Display updated state of charset entries
        log "After modification:"
        if [[ "$VERBOSE" == true ]]; then
            grep -E 'ISO-2022-CN-EXT' "$FILE" | tee -a "$LOGFILE"
        else
            grep -E 'ISO-2022-CN-EXT' "$FILE" >> "$LOGFILE"
        fi
        log "iconv CVE-2024-2961 mitigation fix applied. Details logged in $LOGFILE"
        log "Info: https://community.centminmod.com/threads/25069/"
    else
        echo "Configuration file not found at $FILE." >> "$LOGFILE"
    fi
done

# Final messages based on the mode
if [[ "$VERBOSE" == false ]]; then
    log "iconv CVE-2024-2961 mitigation fix applied. Details logged in $LOGFILE"
fi
