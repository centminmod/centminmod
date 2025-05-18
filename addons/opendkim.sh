#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
###################################################################
# OpenDKIM install and configuration for centminmod.com LEMP stack
# https://community.centminmod.com/posts/29878/
###################################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4

# DKIM key length of 1024bit, 2048bit, or 4096bit
DKIM_LENGTH='2048'

# DKIM selector definition
CURRENT_YEAR=$(date +"%Y")
SELECTOR="default${CURRENT_YEAR}"

###################################################################
# Backup directory for previous DKIM files
DKIM_BACKUPDIR='/etc/centminmod/dkim_backups'

# Initialize variables
FORCE_UPDATE=0
CLEANONLY=0
WIPE=0
CHECK=0
wipe_host=""
vhostname=""

# Add to argument processing section
while [[ $# -gt 0 ]]; do
    case "$1" in
        --force)
            FORCE_UPDATE=1
            shift
            ;;
        clean)
            CLEANONLY=1
            shift
            ;;
        check)
            CHECK=1
            shift
            ;;
        wipe)
            WIPE=1
            shift
            ;;
        *)
            if [[ "$WIPE" -eq 1 && -z "$wipe_host" ]]; then
                wipe_host="$1"
                WIPE=2
                shift
            else
                vhostname="$1"
                shift
            fi
            ;;
    esac
done

# Set locale temporarily to English
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# Disable systemd pager
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ ! -d "$CENTMINLOGDIR" ]; then
    mkdir -p "$CENTMINLOGDIR"
fi

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

# Check for Red Hat Enterprise Linux 7.x
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

# Ensure only EL8+ OS versions are being looked at for various distributions
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

CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')

if [ ! -d "$CENTMINLOGDIR" ]; then
    mkdir -p "$CENTMINLOGDIR"
fi

check_dkim() {
    echo "================================================================"
    echo "                    OpenDKIM Configuration Check                   "
    echo "================================================================"
    echo
    echo "DKIM Keys Directory Structure:"
    echo "----------------------------------------------------------------"
    if [ -d "/etc/opendkim/keys" ]; then
        find /etc/opendkim/keys -type d -exec sh -c '
            for dir do
                printf "\033[0;34m%s\033[0m:\n" "$dir"
                ls -la "$dir" 2>/dev/null | tail -n +4 | \
                awk '\''{ 
                    printf "  %-10s %-10s %8s %s %s %s %s\n", 
                    $1, $3, $5, $6, $7, $8, $9
                }'\''
            done
        ' sh {} +
    else
        echo "No DKIM keys directory found."
    fi
    echo

    echo "Trusted Hosts Configuration:"
    echo "----------------------------------------------------------------"
    if [ -f "/etc/opendkim/TrustedHosts" ]; then
        grep -v "^#" /etc/opendkim/TrustedHosts | grep -v "^$" | \
        while read -r line; do
            echo "  $line"
        done
    else
        echo "No TrustedHosts file found."
    fi
    echo

    echo "Key Table Configuration:"
    echo "----------------------------------------------------------------"
    if [ -f "/etc/opendkim/KeyTable" ]; then
        grep -v "^#" /etc/opendkim/KeyTable | grep -v "^$" | \
        while read -r line; do
            if [[ $line =~ ^([^[:space:]]+)[[:space:]]+(.+)$ ]]; then
                printf "  \033[0;32mSelector:\033[0m %s\n" "${BASH_REMATCH[1]}"
                printf "  \033[0;32mConfig:\033[0m   %s\n\n" "${BASH_REMATCH[2]}"
            fi
        done
    else
        echo "No KeyTable file found."
    fi
    echo

    echo "Signing Table Configuration:"
    echo "----------------------------------------------------------------"
    if [ -f "/etc/opendkim/SigningTable" ]; then
        grep -v "^#" /etc/opendkim/SigningTable | grep -v "^$" | \
        while read -r line; do
            if [[ $line =~ ^([^[:space:]]+)[[:space:]]+(.+)$ ]]; then
                printf "  \033[0;32mDomain:\033[0m    %s\n" "${BASH_REMATCH[1]}"
                printf "  \033[0;32mSelector:\033[0m  %s\n\n" "${BASH_REMATCH[2]}"
            fi
        done
    else
        echo "No SigningTable file found."
    fi
    echo "================================================================"
}

opendkimsetup() {
    if ! rpm -qa | grep -qw opendkim; then
        yum -y install opendkim
        cp /etc/opendkim.conf{,.orig}
    fi
    if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && ! rpm -qa | grep -qw opendkim-tools; then
        yum -y install opendkim-tools
    fi

    if [ -f /etc/opendkim.conf ]; then

        if ! grep -q 'AutoRestart' /etc/opendkim.conf; then
            echo "AutoRestart             Yes" >> /etc/opendkim.conf
            echo "AutoRestartRate         10/1h" >> /etc/opendkim.conf
            echo "SignatureAlgorithm      rsa-sha256" >> /etc/opendkim.conf
            echo "TemporaryDirectory      /var/tmp" >> /etc/opendkim.conf
            sed -i "s|^Mode.*|Mode sv|" /etc/opendkim.conf
            sed -i "s|^Canonicalization.*|Canonicalization        relaxed/simple|" /etc/opendkim.conf
            sed -i "s|^# ExternalIgnoreList|ExternalIgnoreList|" /etc/opendkim.conf
            sed -i "s|^# InternalHosts|InternalHosts|" /etc/opendkim.conf
            sed -i 's|^# KeyTable|KeyTable|' /etc/opendkim.conf
            sed -i "s|^# SigningTable|SigningTable|" /etc/opendkim.conf
            sed -i "s|Umask.*|Umask 022|" /etc/opendkim.conf
        fi

        # Additional configuration updates that should always run
        # Only update Selector if it doesn't match our SELECTOR variable
        if ! grep -q "^Selector[[:space:]]*${SELECTOR}$" /etc/opendkim.conf; then
            sed -i "s|^Selector.*|Selector        ${SELECTOR}|" /etc/opendkim.conf
        fi
        
        # Remove KeyFile if it exists
        sed -i '/^KeyFile/d' /etc/opendkim.conf
        
        # Update SigningTable to use file: if it's using refile:
        if grep -q "^SigningTable.*refile:" /etc/opendkim.conf; then
            sed -i 's|^SigningTable.*|SigningTable    file:/etc/opendkim/SigningTable|' /etc/opendkim.conf
        fi

        # Update ExternalIgnoreList to use file: if it's using refile:
        if grep -q "^ExternalIgnoreList.*refile:" /etc/opendkim.conf; then
            sed -i 's|^ExternalIgnoreList.*|ExternalIgnoreList    file:/etc/opendkim/TrustedHosts|' /etc/opendkim.conf
        fi

        # Update InternalHosts to use file: if it's using refile:
        if grep -q "^InternalHosts.*refile:" /etc/opendkim.conf; then
            sed -i 's|^InternalHosts.*|InternalHosts    file:/etc/opendkim/TrustedHosts|' /etc/opendkim.conf
        fi

        if grep -q "^#Socket\s*inet:8891@localhost" /etc/opendkim.conf; then
            # Ensure socket isn't commented out
            sed -i 's/^#\s*Socket\s\+inet:8891@localhost/Socket inet:8891@localhost/' /etc/opendkim.conf
            echo "Socket configuration updated."
        fi

        if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
            # Ensure only one Socket option is set
            sed -i.bak '/Socket local:\/run\/opendkim\/opendkim.sock/d' /etc/opendkim.conf
        fi

        if [ ! -f "${CENTMINLOGDIR}/dkim_postfix_after.txt" ]; then
            postconf -d smtpd_milters non_smtpd_milters milter_default_action milter_protocol | tee "${CENTMINLOGDIR}/dkim_postfix_before_${DT}.txt"
            postconf -e "smtpd_milters           = inet:127.0.0.1:8891"
            postconf -e 'non_smtpd_milters       = $smtpd_milters'
            postconf -e "milter_default_action   = accept"
            if [[ "$(postconf -d milter_protocol | awk -F "= " '{print $2}')" = '6' ]]; then
                postconf -e "milter_protocol         = 6"
            elif [[ "$(postconf -d milter_protocol | awk -F "= " '{print $2}')" = '2' ]]; then
                postconf -e "milter_protocol         = 2"
            fi
            postconf -n smtpd_milters non_smtpd_milters milter_default_action milter_protocol | tee "${CENTMINLOGDIR}/dkim_postfix_after.txt"
        fi

        # DKIM for main hostname
        h_vhostname=$(hostname -f 2>/dev/null || hostname)

        if [ ! -d "/etc/opendkim/keys/$h_vhostname" ] || [ "$FORCE_UPDATE" -eq 1 ]; then

            if [ "$FORCE_UPDATE" -eq 1 ] && [ -d "/etc/opendkim/keys/$h_vhostname" ]; then
                [ ! -d "$DKIM_BACKUPDIR" ] && mkdir -p "$DKIM_BACKUPDIR"
                TIMESTAMP=$(date +"%Y%m%d%H%M%S")
                BACKUP_DIR="$DKIM_BACKUPDIR/$h_vhostname-$TIMESTAMP"
                echo "Backed up existing DKIM keys and configurations to $BACKUP_DIR"
                mkdir -p "$BACKUP_DIR"
                cp -a "/etc/opendkim/keys/$h_vhostname" "$BACKUP_DIR/"
                grep "$h_vhostname" /etc/opendkim/KeyTable > "$BACKUP_DIR/KeyTable"
                grep "$h_vhostname" /etc/opendkim/SigningTable > "$BACKUP_DIR/SigningTable"
                grep "^$h_vhostname$" /etc/opendkim/TrustedHosts > "$BACKUP_DIR/TrustedHosts"
                rm -rf "/etc/opendkim/keys/$h_vhostname"
                sed -i "/$h_vhostname/d" /etc/opendkim/KeyTable
                sed -i "/$h_vhostname/d" /etc/opendkim/SigningTable
                sed -i "/^$h_vhostname$/d" /etc/opendkim/TrustedHosts
            fi

            mkdir -p "/etc/opendkim/keys/$h_vhostname"
            opendkim-genkey -b "$DKIM_LENGTH" -D "/etc/opendkim/keys/$h_vhostname/" -d "$h_vhostname" -s "$SELECTOR"
            chown -R opendkim: "/etc/opendkim/keys/$h_vhostname"
            mv -f "/etc/opendkim/keys/$h_vhostname/${SELECTOR}.private" "/etc/opendkim/keys/$h_vhostname/${SELECTOR}"

            if [ -f "/etc/opendkim/keys/$h_vhostname/${SELECTOR}" ]; then
                echo
                echo "check /etc/opendkim/keys/$h_vhostname/${SELECTOR}"
                openssl rsa -in "/etc/opendkim/keys/$h_vhostname/${SELECTOR}" -text -noout | head -n1
                echo
            fi

            if ! grep -q "$h_vhostname" /etc/opendkim/KeyTable; then
                echo "${SELECTOR}._domainkey.$h_vhostname $h_vhostname:${SELECTOR}:/etc/opendkim/keys/$h_vhostname/${SELECTOR}" >> /etc/opendkim/KeyTable
            fi
            if ! grep -q "$h_vhostname" /etc/opendkim/SigningTable; then
                echo "$h_vhostname ${SELECTOR}._domainkey.$h_vhostname" >> /etc/opendkim/SigningTable
            fi
            if ! grep -q "^$h_vhostname$" /etc/opendkim/TrustedHosts; then
                echo "$h_vhostname" >> /etc/opendkim/TrustedHosts
            fi
            echo "---------------------------------------------------------------------------" | tee "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "$h_vhostname DKIM DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            cat "/etc/opendkim/keys/$h_vhostname/${SELECTOR}.txt" | tr -d '\n' | sed -e 's/[[:space:]]\+/ /g' -e "s/( \"//g" -e "s/\" )//g" -e "s/ ; ----- DKIM key $SELECTOR for $h_vhostname//" -e "s/${SELECTOR}._domainkey/${SELECTOR}._domainkey.$h_vhostname/" -e 's/[[:space:]]*IN[[:space:]]*TXT[[:space:]]*/ IN TXT /' -e 's/"[[:space:]]*"//g' -e 's/[[:space:]]*;/;/g' | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            sed -i 's|"    "||g' "/root/centminlogs/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo -e "\n------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "$h_vhostname SPF DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "$h_vhostname. 14400 IN TXT \"v=spf1 a mx ~all\"" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "dig +short ${SELECTOR}._domainkey.$h_vhostname TXT" >> "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "DKIM & SPF TXT details saved at $CENTMINLOGDIR/dkim_spf_dns_${h_vhostname}_${DT}.txt"
            echo "---------------------------------------------------------------------------"
        fi

        # DKIM for vhost site domain names
        if [[ -n "$vhostname" ]]; then
            if [ ! -d "/etc/opendkim/keys/$vhostname" ] || [ "$FORCE_UPDATE" -eq 1 ]; then

                if [ "$FORCE_UPDATE" -eq 1 ] && [ -d "/etc/opendkim/keys/$vhostname" ]; then
                    [ ! -d "$DKIM_BACKUPDIR" ] && mkdir -p "$DKIM_BACKUPDIR"
                    TIMESTAMP=$(date +"%Y%m%d%H%M%S")
                    BACKUP_DIR="$DKIM_BACKUPDIR/$vhostname-$TIMESTAMP"
                    echo "Backed up existing DKIM keys and configurations to $BACKUP_DIR"
                    mkdir -p "$BACKUP_DIR"
                    cp -a "/etc/opendkim/keys/$vhostname" "$BACKUP_DIR/"
                    grep "$vhostname" /etc/opendkim/KeyTable > "$BACKUP_DIR/KeyTable"
                    grep "$vhostname" /etc/opendkim/SigningTable > "$BACKUP_DIR/SigningTable"
                    grep "^$vhostname$" /etc/opendkim/TrustedHosts > "$BACKUP_DIR/TrustedHosts"
                    rm -rf "/etc/opendkim/keys/$vhostname"
                    sed -i "/$vhostname/d" /etc/opendkim/KeyTable
                    sed -i "/$vhostname/d" /etc/opendkim/SigningTable
                    sed -i "/^$vhostname$/d" /etc/opendkim/TrustedHosts
                fi

                echo
                mkdir -p "/etc/opendkim/keys/$vhostname"
                opendkim-genkey -b "$DKIM_LENGTH" -D "/etc/opendkim/keys/$vhostname/" -d "$vhostname" -s "$SELECTOR"
                chown -R opendkim: "/etc/opendkim/keys/$vhostname"
                mv -f "/etc/opendkim/keys/$vhostname/${SELECTOR}.private" "/etc/opendkim/keys/$vhostname/${SELECTOR}"

                if [ -f "/etc/opendkim/keys/$vhostname/${SELECTOR}" ]; then
                    echo
                    echo "check /etc/opendkim/keys/$vhostname/${SELECTOR}"
                    openssl rsa -in "/etc/opendkim/keys/$vhostname/${SELECTOR}" -text -noout | head -n1
                    echo
                fi

                if ! grep -q "${SELECTOR}._domainkey.$vhostname" /etc/opendkim/KeyTable; then
                    echo "${SELECTOR}._domainkey.$vhostname $vhostname:${SELECTOR}:/etc/opendkim/keys/$vhostname/${SELECTOR}" >> /etc/opendkim/KeyTable
                fi
                if ! grep -q "${SELECTOR}._domainkey.$vhostname" /etc/opendkim/SigningTable; then
                    echo "$vhostname ${SELECTOR}._domainkey.$vhostname" >> /etc/opendkim/SigningTable
                fi
                if ! grep -q "^$vhostname$" /etc/opendkim/TrustedHosts; then
                    echo "$vhostname" >> /etc/opendkim/TrustedHosts
                fi
                echo "---------------------------------------------------------------------------" | tee "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "$vhostname DKIM DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                cat "/etc/opendkim/keys/$vhostname/${SELECTOR}.txt" | tr -d '\n' | sed -e 's/[[:space:]]\+/ /g' -e "s/( \"//g" -e "s/\" )//g" -e "s/ ; ----- DKIM key $SELECTOR for $vhostname//" -e "s/${SELECTOR}._domainkey/${SELECTOR}._domainkey.$vhostname/" -e 's/[[:space:]]*IN[[:space:]]*TXT[[:space:]]*/ IN TXT /' -e 's/"[[:space:]]*"//g' -e 's/[[:space:]]*;/;/g' | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                sed -i 's|"    "||g' "/root/centminlogs/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo -e "\n------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "$vhostname SPF DNS Entry" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "$vhostname. 14400 IN TXT \"v=spf1 a mx ~all\"" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "dig +short ${SELECTOR}._domainkey.$vhostname TXT" >> "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "---------------------------------------------------------------------------" | tee -a "$CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "DKIM & SPF TXT details saved at $CENTMINLOGDIR/dkim_spf_dns_${vhostname}_${DT}.txt"
                echo "---------------------------------------------------------------------------"
                echo
            fi
        fi

        if rpm -qa | grep -qw opendkim; then
            hash -r
            systemctl enable opendkim >/dev/null 2>&1
            systemctl start opendkim >/dev/null 2>&1
            # systemctl status opendkim --no-pager
        fi
        systemctl restart postfix >/dev/null 2>&1

    fi
}

###########################################################################

wipe_dkim() {
    local hostname_to_wipe="$1"

    if [[ -z "$hostname_to_wipe" ]]; then
        echo "Error: No hostname specified for wipe."
        echo "Usage: addons/opendkim.sh wipe <hostname>"
        exit 1
    fi

    echo "Starting DKIM wipe for hostname: $hostname_to_wipe"

    # Backup directory
    [ ! -d "$DKIM_BACKUPDIR" ] && mkdir -p "$DKIM_BACKUPDIR"

    # Create a backup of existing configurations before wiping
    TIMESTAMP=$(date +"%Y%m%d%H%M%S")
    BACKUP_DIR="$DKIM_BACKUPDIR/$hostname_to_wipe-$TIMESTAMP"
    mkdir -p "$BACKUP_DIR"
    echo "Backing up existing DKIM configurations to $BACKUP_DIR"
    cp -a "/etc/opendkim/keys/$hostname_to_wipe" "$BACKUP_DIR/" 2>/dev/null || echo "No DKIM keys found for $hostname_to_wipe to backup."
    grep "$hostname_to_wipe" /etc/opendkim/KeyTable > "$BACKUP_DIR/KeyTable" 2>/dev/null || echo "No KeyTable entries found for $hostname_to_wipe."
    grep "$hostname_to_wipe" /etc/opendkim/SigningTable > "$BACKUP_DIR/SigningTable" 2>/dev/null || echo "No SigningTable entries found for $hostname_to_wipe."
    grep "^$hostname_to_wipe$" /etc/opendkim/TrustedHosts > "$BACKUP_DIR/TrustedHosts" 2>/dev/null || echo "No TrustedHosts entry found for $hostname_to_wipe."

    # Remove DKIM keys directory
    if [ -d "/etc/opendkim/keys/$hostname_to_wipe" ]; then
        echo "Removing DKIM keys directory: /etc/opendkim/keys/$hostname_to_wipe"
        rm -rf "/etc/opendkim/keys/$hostname_to_wipe"
    else
        echo "No DKIM keys directory found for $hostname_to_wipe."
    fi

    # Remove entries from KeyTable
    if [ -f /etc/opendkim/KeyTable ]; then
        echo "Removing KeyTable entries for $hostname_to_wipe"
        sed -i "/$hostname_to_wipe/d" /etc/opendkim/KeyTable
    else
        echo "KeyTable file not found."
    fi

    # Remove entries from SigningTable
    if [ -f /etc/opendkim/SigningTable ]; then
        echo "Removing SigningTable entries for $hostname_to_wipe"
        sed -i "/$hostname_to_wipe/d" /etc/opendkim/SigningTable
    else
        echo "SigningTable file not found."
    fi

    # Remove entries from TrustedHosts
    if [ -f /etc/opendkim/TrustedHosts ]; then
        echo "Removing TrustedHosts entries for $hostname_to_wipe"
        sed -i "/^$hostname_to_wipe$/d" /etc/opendkim/TrustedHosts
    else
        echo "TrustedHosts file not found."
    fi

    echo "DKIM wipe completed for $hostname_to_wipe."
}

###########################################################################

starttime=$(TZ=UTC date +%s.%N)
{
    # Handle the 'check' operation
    if [[ "$CHECK" -eq 1 ]]; then
        check_dkim
    # Handle the 'wipe' operation
    elif [[ "$WIPE" -eq 2 ]]; then
        wipe_dkim "$wipe_host"
    else
        # Handle the 'clean' operation
        if [[ "$CLEANONLY" -eq 1 ]]; then
            h_vhostname=$(hostname -f 2>/dev/null || hostname)
            # Clean main hostname
            rm -rf "/etc/opendkim/keys/$h_vhostname"
            if [ -f /etc/opendkim/KeyTable ]; then
                sed -i "/$h_vhostname/d" /etc/opendkim/KeyTable
            fi
            if [ -f /etc/opendkim/SigningTable ]; then
                sed -i "/$h_vhostname/d" /etc/opendkim/SigningTable
            fi

            # Clean vhostname if provided
            if [[ -n "$vhostname" ]]; then
                rm -rf "/etc/opendkim/keys/$vhostname"
                if [ -f /etc/opendkim/KeyTable ]; then
                    sed -i "/$vhostname/d" /etc/opendkim/KeyTable
                fi
                if [ -f /etc/opendkim/SigningTable ]; then
                    sed -i "/$vhostname/d" /etc/opendkim/SigningTable
                fi
                if [ -f /etc/opendkim/TrustedHosts ]; then
                    sed -i "/^$vhostname$/d" /etc/opendkim/TrustedHosts
                fi
            fi
        fi

        # Only run opendkimsetup if not wiping
        opendkimsetup
    fi
} 2>&1 | tee "${CENTMINLOGDIR}/opendkim_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/opendkim_${DT}.log"
echo "Opendkim Setup Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/opendkim_${DT}.log"
