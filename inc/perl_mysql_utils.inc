#!/bin/bash
# perl_mysql_utils.inc
# Perl-DBD-MySQL YUM Exclusion Management for MariaDB Compatibility
# Prevents AppStream perl-DBD-MySQL updates from triggering mysql-common/mysql-libs conflicts
# Addresses: AlmaLinux/Rocky Linux 9.6→9.7 update conflicts with MariaDB-common
# Community Issue: https://community.centminmod.com/threads/30001/

manage_perl_dbd_mysql_exclude() {
    local action="${1:-add}"  # add|remove|status

    case "$action" in
        add)
            # Add perl-DBD-MySQL to yum.conf exclude list
            if [[ "$(grep '^exclude=' /etc/yum.conf | grep -o 'perl-DBD-MySQL')" != 'perl-DBD-MySQL' ]]; then
                # Backup yum.conf
                cp -af /etc/yum.conf /etc/yum.conf.backup-perl-dbd-$(date +%Y%m%d-%H%M%S)

                if [[ "$(grep '^exclude=' /etc/yum.conf)" ]]; then
                    # Append to existing exclude line
                    NEW_EXCLUDES=$(echo "$(grep '^exclude=' /etc/yum.conf) perl-DBD-MySQL")
                    sed -i "s|^exclude=.*|$NEW_EXCLUDES|" /etc/yum.conf
                    echo "Added perl-DBD-MySQL to existing /etc/yum.conf exclude line"
                else
                    # Create new exclude line
                    echo "exclude=perl-DBD-MySQL" >> /etc/yum.conf
                    echo "Created /etc/yum.conf exclude line with perl-DBD-MySQL"
                fi
            else
                echo "perl-DBD-MySQL already in /etc/yum.conf excludes"
            fi
            ;;
        remove)
            # Remove perl-DBD-MySQL from yum.conf exclude list
            if [[ "$(grep '^exclude=' /etc/yum.conf | grep -o 'perl-DBD-MySQL')" = 'perl-DBD-MySQL' ]]; then
                cp -af /etc/yum.conf /etc/yum.conf.backup-perl-dbd-$(date +%Y%m%d-%H%M%S)
                sed -i 's/\s*perl-DBD-MySQL//g' /etc/yum.conf
                sed -i 's/  */ /g' /etc/yum.conf  # Clean up extra spaces
                echo "Removed perl-DBD-MySQL from /etc/yum.conf excludes"
            else
                echo "perl-DBD-MySQL not in /etc/yum.conf excludes"
            fi
            ;;
        status)
            # Check current exclusion status
            echo "Checking /etc/yum.conf perl-DBD-MySQL exclusion status..."
            if [[ "$(grep '^exclude=' /etc/yum.conf | grep -o 'perl-DBD-MySQL')" = 'perl-DBD-MySQL' ]]; then
                echo "✓ perl-DBD-MySQL IS EXCLUDED in /etc/yum.conf"
                return 0
            else
                echo "✗ perl-DBD-MySQL NOT excluded in /etc/yum.conf"
                return 1
            fi
            ;;
    esac
}
