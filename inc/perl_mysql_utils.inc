#!/bin/bash
# perl_mysql_utils.inc
# Perl-DBD-MySQL YUM Exclusion Management for MariaDB Compatibility
# Prevents AppStream perl-DBD-MySQL updates from triggering mysql-common/mysql-libs conflicts
# Addresses: AlmaLinux/Rocky Linux 9.6→9.7 update conflicts with MariaDB-common
# Community Issue: https://community.centminmod.com/threads/30001/

# Append package globs missing from an existing ^exclude= line in file $1.
# Token based, so it works on any exclude line layout (EL7 *.i386 *.i586 *.i686
# or EL8+ *.i686) and never adds duplicates. Prints the tokens it added.
exclude_tokens_add() {
    local conf="$1" tok added=""
    shift
    grep -q '^exclude=' "$conf" 2>/dev/null || return 0
    for tok in "$@"; do
        if ! grep '^exclude=' "$conf" | sed 's/^exclude=//' | tr -s ' \t' '\n' | grep -qxF -- "$tok"; then
            sed -i "/^exclude=/ s|\$| ${tok}|" "$conf"
            added="${added} ${tok}"
        fi
    done
    if [[ -n "$added" ]]; then
        sed -i 's/^exclude=[[:space:]]*/exclude=/' "$conf"
        echo "added${added} to exclude line in $conf"
    fi
    return 0
}

# Same as exclude_tokens_add on the real file behind /etc/yum.conf, so the
# EL8+ /etc/yum.conf -> /etc/dnf/dnf.conf symlink is never replaced
yum_exclude_add() {
    exclude_tokens_add "$(readlink -f /etc/yum.conf)" "$@"
}

manage_perl_dbd_mysql_exclude() {
    local action="${1:-add}"  # add|remove|status
    local silent="$2"
    local config_file="/etc/yum.conf"
    local actual_file="$config_file"

    # Repair broken symlinks on EL8/9/10 systems only
    # EL7 uses a regular yum.conf file, so skip repair there
    if [[ "$CENTOS_EIGHT" -eq 8 || "$CENTOS_NINE" -eq 9 || "$CENTOS_TEN" -eq 10 ]]; then
        if [[ -f "$config_file" && ! -L "$config_file" && -f /etc/dnf/dnf.conf ]]; then
            echo "WARNING: Detected broken symlink - /etc/yum.conf is a regular file (should be symlink on EL${CENTOS_EIGHT}${CENTOS_NINE}${CENTOS_TEN})"
            echo "Repairing: Restoring symlink to /etc/dnf/dnf.conf"

            # Backup the broken regular file
            cp -af "$config_file" "${config_file}.broken-$(date +%Y%m%d-%H%M%S)"

            # Merge exclude lines from broken yum.conf to dnf.conf if they exist
            if grep -q '^exclude=' "$config_file" 2>/dev/null; then
                local broken_excludes=$(grep '^exclude=' "$config_file")
                if ! grep -q '^exclude=' /etc/dnf/dnf.conf 2>/dev/null; then
                    # No exclude line in dnf.conf, copy from broken yum.conf
                    echo "$broken_excludes" >> /etc/dnf/dnf.conf
                    echo "Migrated exclusions from broken yum.conf to dnf.conf"
                else
                    # Merge exclusions token by token (read -a does not glob *.i686)
                    local -a yum_tokens
                    read -ra yum_tokens <<< "${broken_excludes#exclude=}"
                    exclude_tokens_add /etc/dnf/dnf.conf "${yum_tokens[@]}"
                fi
            fi

            # Remove broken regular file and restore symlink
            rm -f "$config_file"
            ln -s /etc/dnf/dnf.conf "$config_file"
            echo "✓ Symlink restored: $config_file -> /etc/dnf/dnf.conf"
        fi
    fi

    # If yum.conf is a symlink, resolve to the actual file to prevent sed -i from breaking it
    if [[ -L "$config_file" ]]; then
        actual_file="$(readlink -f "$config_file")"
        if [[ "$action" != "status" && "$silent" != "silent" ]]; then
            echo "Detected symlink: $config_file -> $actual_file"
        fi
    fi

    # Clean up duplicate perl-DBD-MySQL entries (self-healing for bug that caused repeated additions)
    if [[ -f "$actual_file" ]]; then
        local exclude_line
        exclude_line=$(grep '^exclude=' "$actual_file" 2>/dev/null)
        if [[ -n "$exclude_line" ]]; then
            # Count occurrences of perl-DBD-MySQL
            local dbd_count
            dbd_count=$(echo "$exclude_line" | grep -o 'perl-DBD-MySQL' | wc -l)
            if [[ "$dbd_count" -gt 1 ]]; then
                [[ "$silent" != "silent" ]] && echo "Detected $dbd_count duplicate perl-DBD-MySQL entries, cleaning up..."
                # Remove all perl-DBD-MySQL entries first, then add back one
                local cleaned_line
                cleaned_line=$(echo "$exclude_line" | sed 's/[[:space:]]*perl-DBD-MySQL//g' | sed 's/  */ /g' | sed 's/ $//')
                cleaned_line="${cleaned_line} perl-DBD-MySQL"
                # Also clean up any duplicate entries (*.i686, etc) using tr/sort/uniq
                local prefix value_part unique_values
                prefix="exclude="
                value_part="${cleaned_line#exclude=}"
                # Convert space-separated to newline-separated, sort unique, convert back
                unique_values=$(echo "$value_part" | tr ' ' '\n' | awk '!seen[$0]++' | tr '\n' ' ' | sed 's/ $//')
                cleaned_line="${prefix}${unique_values}"
                # Apply the cleaned line
                sed -i "s|^exclude=.*|$cleaned_line|" "$actual_file"
                [[ "$silent" != "silent" ]] && echo "Cleaned up duplicates in $actual_file"
            fi
        fi
    fi

    case "$action" in
        add)
            # Add perl-DBD-MySQL to yum.conf exclude list
            # Use grep -q for boolean check (grep -o returns multiple lines if duplicates exist)
            if ! grep '^exclude=' "$actual_file" 2>/dev/null | grep -q 'perl-DBD-MySQL'; then
                # Backup the actual config file
                cp -af "$actual_file" "${actual_file}.backup-perl-dbd-$(date +%Y%m%d-%H%M%S)"

                if grep -q '^exclude=' "$actual_file" 2>/dev/null; then
                    # Append to existing exclude line
                    NEW_EXCLUDES="$(grep '^exclude=' "$actual_file") perl-DBD-MySQL"
                    sed -i "s|^exclude=.*|$NEW_EXCLUDES|" "$actual_file"
                    echo "Added perl-DBD-MySQL to existing exclude line in $actual_file"
                else
                    # Create new exclude line
                    echo "exclude=perl-DBD-MySQL" >> "$actual_file"
                    echo "Created exclude line with perl-DBD-MySQL in $actual_file"
                fi
            else
                [[ "$silent" != "silent" ]] && echo "perl-DBD-MySQL already in $actual_file excludes"
            fi
            ;;
        remove)
            # Remove perl-DBD-MySQL from yum.conf exclude list
            if grep '^exclude=' "$actual_file" 2>/dev/null | grep -q 'perl-DBD-MySQL'; then
                cp -af "$actual_file" "${actual_file}.backup-perl-dbd-$(date +%Y%m%d-%H%M%S)"
                sed -i 's/\s*perl-DBD-MySQL//g' "$actual_file"
                sed -i 's/  */ /g' "$actual_file"  # Clean up extra spaces
                echo "Removed perl-DBD-MySQL from $actual_file excludes"
            else
                [[ "$silent" != "silent" ]] && echo "perl-DBD-MySQL not in $actual_file excludes"
            fi
            ;;
        status)
            # Check current exclusion status
            echo "Checking perl-DBD-MySQL exclusion status..."
            echo "Config file: $config_file"
            if [[ -L "$config_file" ]]; then
                echo "  (symlink to: $actual_file)"
            fi

            if grep '^exclude=' "$actual_file" 2>/dev/null | grep -q 'perl-DBD-MySQL'; then
                echo "✓ perl-DBD-MySQL IS EXCLUDED in $actual_file"
                grep '^exclude=' "$actual_file"
                return 0
            else
                echo "✗ perl-DBD-MySQL NOT excluded in $actual_file"
                return 1
            fi
            ;;
    esac
}
