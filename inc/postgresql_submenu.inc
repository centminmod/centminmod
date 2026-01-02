#!/bin/bash
# shellcheck source=/dev/null
# shellcheck disable=SC2154
#######################################################################################
# PostgreSQL Server Sub Menu
# File: /inc/postgresql_submenu.inc
# Simple submenu pattern (based on redis_submenu.inc)
#######################################################################################

postgresql_submenu() {
  while :; do
    echo
    cecho "--------------------------------------------------------" "$boldyellow"
    cecho "         PostgreSQL Server Sub-Menu                     " "$boldgreen"
    cecho "--------------------------------------------------------" "$boldyellow"
    cecho "1). Install/Reinstall PostgreSQL Server" "$boldgreen"
    cecho "2). PostgreSQL User/Database Management" "$boldgreen"
    cecho "3). Check PostgreSQL Status" "$boldgreen"
    cecho "4). Upgrade PostgreSQL Version" "$boldgreen"
    cecho "5). Back to Main menu" "$boldgreen"
    cecho "--------------------------------------------------------" "$boldyellow"

    read -rep "Enter option [ 1 - 5 ] " pgsqloption
    cecho "--------------------------------------------------------" "$boldyellow"

    case "$pgsqloption" in
      1)
        centminlog
        {
          postgresqlinstall
        } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_postgresql_install.log"
        ;;
      2)
        pgsql_admin_menu
        ;;
      3)
        # Detect installed PostgreSQL version
        PGSQL_VER=""
        for ver in 18 17 16 15 13 12 11 10 9.6; do
          if [[ -f "/usr/pgsql-${ver}/bin/pg_config" ]]; then
            PGSQL_VER="$ver"
            break
          fi
        done

        if [[ -n "$PGSQL_VER" ]]; then
          echo ""
          cecho "PostgreSQL ${PGSQL_VER} Status:" "$boldgreen"
          systemctl status postgresql-${PGSQL_VER} --no-pager
        else
          cecho "PostgreSQL is not installed" "$boldred"
        fi
        ;;
      4)
        postgresql_upgrade_menu
        ;;
      5)
        break
        ;;
      *)
        echo "Invalid option, please try again."
        ;;
    esac
  done
}

postgresql_upgrade_menu() {
  while :; do
    echo
    cecho "--------------------------------------------------------" "$boldyellow"
    cecho "         PostgreSQL Upgrade Sub-Menu                    " "$boldgreen"
    cecho "--------------------------------------------------------" "$boldyellow"
    cecho "1). Check Current PostgreSQL Version" "$boldgreen"
    cecho "2). Pre-Upgrade Compatibility Check" "$boldgreen"
    cecho "3). Upgrade PostgreSQL (pg_upgrade)" "$boldgreen"
    cecho "4). Post-Upgrade Cleanup" "$boldgreen"
    cecho "5). Back to PostgreSQL Menu" "$boldgreen"
    cecho "--------------------------------------------------------" "$boldyellow"

    read -rep "Enter option [ 1 - 5 ] " pgupgradeoption
    cecho "--------------------------------------------------------" "$boldyellow"

    case "$pgupgradeoption" in
      1) pgsql_check_version ;;
      2) pgsql_preupgrade_check ;;
      3) pgsql_upgrade_execute ;;
      4) pgsql_postupgrade_cleanup ;;
      5) break ;;
      *) echo "Invalid option, please try again." ;;
    esac
  done
}

#######################################################################################
# PostgreSQL Version Detection
#######################################################################################
pgsql_check_version() {
  cecho "--------------------------------------------------------" "$boldyellow"
  cecho "PostgreSQL Version Information" "$boldgreen"
  cecho "--------------------------------------------------------" "$boldyellow"

  # Detect all installed PostgreSQL versions
  echo ""
  echo "=== Installed PostgreSQL Versions ==="
  PGSQL_INSTALLED=""
  for ver in 9.6 10 11 12 13 14 15 16 17 18; do
    if [[ -f "/usr/pgsql-${ver}/bin/pg_config" ]]; then
      PGSQL_VER_INFO=$(/usr/pgsql-${ver}/bin/pg_config --version 2>/dev/null)
      echo "  - PostgreSQL ${ver}: ${PGSQL_VER_INFO}"
      PGSQL_INSTALLED="${PGSQL_INSTALLED} ${ver}"
    fi
  done

  if [[ -z "$PGSQL_INSTALLED" ]]; then
    cecho "No PostgreSQL installations found" "$boldred"
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  # Detect running PostgreSQL service
  echo ""
  echo "=== Running PostgreSQL Service ==="
  PGSQL_RUNNING=""
  for ver in 18 17 16 15 14 13 12 11 10 9.6; do
    if systemctl is-active --quiet postgresql-${ver} 2>/dev/null; then
      PGSQL_RUNNING="${ver}"
      echo "  Active: postgresql-${ver}"

      # Get data directory info
      PGDATA="/var/lib/pgsql/${ver}/data"
      if [[ -d "$PGDATA" ]]; then
        echo "  Data Directory: ${PGDATA}"
        PGDATA_SIZE=$(du -sh "$PGDATA" 2>/dev/null | awk '{print $1}')
        echo "  Data Size: ${PGDATA_SIZE}"
      fi
      break
    fi
  done

  if [[ -z "$PGSQL_RUNNING" ]]; then
    cecho "  No PostgreSQL service is currently running" "$boldyellow"
  fi

  # Show available upgrade paths
  echo ""
  echo "=== Available Upgrade Paths ==="
  echo "  Supported: 16 → 17, 17 → 18"
  echo "  Reference: https://www.postgresql.org/docs/current/pgupgrade.html"

  echo
  read -rep "Press [Enter] to continue..."
}

#######################################################################################
# Pre-Upgrade Compatibility Check
#######################################################################################
pgsql_preupgrade_check() {
  cecho "--------------------------------------------------------" "$boldyellow"
  cecho "PostgreSQL Pre-Upgrade Compatibility Check" "$boldgreen"
  cecho "--------------------------------------------------------" "$boldyellow"

  # Detect current running version
  PGSQL_OLD=""
  for ver in 17 16 15 14 13 12 11 10 9.6; do
    if systemctl is-active --quiet postgresql-${ver} 2>/dev/null; then
      PGSQL_OLD="${ver}"
      break
    fi
  done

  if [[ -z "$PGSQL_OLD" ]]; then
    cecho "Error: No running PostgreSQL service detected" "$boldred"
    echo "Please ensure PostgreSQL is running before upgrade check."
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  echo ""
  echo "Current Version: PostgreSQL ${PGSQL_OLD}"

  # Determine target version
  case "$PGSQL_OLD" in
    17) PGSQL_NEW="18" ;;
    16) PGSQL_NEW="17" ;;
    15) PGSQL_NEW="16" ;;
    14) PGSQL_NEW="15" ;;
    13) PGSQL_NEW="14" ;;
    12) PGSQL_NEW="13" ;;
    11) PGSQL_NEW="12" ;;
    10) PGSQL_NEW="11" ;;
    9.6) PGSQL_NEW="10" ;;
    *)
      cecho "Error: Unknown version ${PGSQL_OLD}" "$boldred"
      echo
      read -rep "Press [Enter] to continue..."
      return
      ;;
  esac

  echo "Target Version: PostgreSQL ${PGSQL_NEW}"
  echo ""

  # Check if target version is installed
  echo "=== Pre-Upgrade Checks ==="

  # Check 1: Target version binaries
  if [[ -f "/usr/pgsql-${PGSQL_NEW}/bin/pg_upgrade" ]]; then
    cecho "  [PASS] Target version ${PGSQL_NEW} binaries installed" "$boldgreen"
  else
    cecho "  [FAIL] Target version ${PGSQL_NEW} not installed" "$boldred"
    echo "         Run: yum install postgresql${PGSQL_NEW//./}-server postgresql${PGSQL_NEW//./}-contrib"
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  # Check 2: Disk space (need at least 2x data size for safety)
  PGDATA_OLD="/var/lib/pgsql/${PGSQL_OLD}/data"
  if [[ -d "$PGDATA_OLD" ]]; then
    PGDATA_SIZE_KB=$(du -sk "$PGDATA_OLD" 2>/dev/null | awk '{print $1}')
    PGDATA_SIZE_HUMAN=$(du -sh "$PGDATA_OLD" 2>/dev/null | awk '{print $1}')
    FREE_SPACE_KB=$(df -k /var/lib/pgsql | tail -1 | awk '{print $4}')
    FREE_SPACE_HUMAN=$(df -h /var/lib/pgsql | tail -1 | awk '{print $4}')

    REQUIRED_KB=$((PGDATA_SIZE_KB * 2))

    if [[ "$FREE_SPACE_KB" -gt "$REQUIRED_KB" ]]; then
      cecho "  [PASS] Disk space: ${FREE_SPACE_HUMAN} free (data: ${PGDATA_SIZE_HUMAN})" "$boldgreen"
    else
      cecho "  [WARN] Low disk space: ${FREE_SPACE_HUMAN} free (need 2x ${PGDATA_SIZE_HUMAN})" "$boldyellow"
    fi
  fi

  # Check 3: No active connections (except maintenance)
  ACTIVE_CONNS=$(sudo -u postgres /usr/pgsql-${PGSQL_OLD}/bin/psql -t -c "SELECT count(*) FROM pg_stat_activity WHERE state = 'active' AND pid != pg_backend_pid();" 2>/dev/null | tr -d ' ')
  if [[ "$ACTIVE_CONNS" == "0" || -z "$ACTIVE_CONNS" ]]; then
    cecho "  [PASS] No active user connections" "$boldgreen"
  else
    cecho "  [WARN] ${ACTIVE_CONNS} active connections detected" "$boldyellow"
  fi

  # Check 4: Extensions compatibility
  echo ""
  echo "=== Installed Extensions ==="
  sudo -u postgres /usr/pgsql-${PGSQL_OLD}/bin/psql -t -c "SELECT extname, extversion FROM pg_extension ORDER BY extname;" 2>/dev/null | grep -v "^$"

  # Check 5: pg_upgrade --check (dry run)
  echo ""
  echo "=== Running pg_upgrade --check ==="
  echo "(This performs a dry-run compatibility check)"
  echo ""

  # Initialize new data directory if needed
  PGDATA_NEW="/var/lib/pgsql/${PGSQL_NEW}/data"
  if [[ ! -d "$PGDATA_NEW" ]] || [[ ! -f "$PGDATA_NEW/PG_VERSION" ]]; then
    echo "Initializing new data directory..."
    sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/initdb -D "$PGDATA_NEW" 2>/dev/null
  fi

  # Run pg_upgrade check
  cd /tmp || return
  sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/pg_upgrade \
    --old-datadir="$PGDATA_OLD" \
    --new-datadir="$PGDATA_NEW" \
    --old-bindir="/usr/pgsql-${PGSQL_OLD}/bin" \
    --new-bindir="/usr/pgsql-${PGSQL_NEW}/bin" \
    --check 2>&1 | tee /tmp/pg_upgrade_check.log

  if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
    echo ""
    cecho "Pre-upgrade check PASSED. Safe to proceed with upgrade." "$boldgreen"
  else
    echo ""
    cecho "Pre-upgrade check FAILED. Review errors above before proceeding." "$boldred"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}

#######################################################################################
# PostgreSQL Upgrade Execution
#######################################################################################
pgsql_upgrade_execute() {
  cecho "--------------------------------------------------------" "$boldyellow"
  cecho "PostgreSQL Major Version Upgrade (pg_upgrade)" "$boldgreen"
  cecho "--------------------------------------------------------" "$boldyellow"
  echo ""
  cecho "WARNING: This will upgrade PostgreSQL to a new major version!" "$boldred"
  echo ""
  echo "Prerequisites:"
  echo "  1. Run Pre-Upgrade Compatibility Check first (option 2)"
  echo "  2. Create a full backup of your databases"
  echo "  3. Schedule downtime - PostgreSQL will be stopped during upgrade"
  echo ""

  # Detect versions
  PGSQL_OLD=""
  for ver in 17 16 15 14 13 12 11 10 9.6; do
    if systemctl is-active --quiet postgresql-${ver} 2>/dev/null; then
      PGSQL_OLD="${ver}"
      break
    fi
  done

  if [[ -z "$PGSQL_OLD" ]]; then
    cecho "Error: No running PostgreSQL service detected" "$boldred"
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  # Determine target version
  case "$PGSQL_OLD" in
    17) PGSQL_NEW="18" ;;
    16) PGSQL_NEW="17" ;;
    15) PGSQL_NEW="16" ;;
    14) PGSQL_NEW="15" ;;
    13) PGSQL_NEW="14" ;;
    12) PGSQL_NEW="13" ;;
    *)
      cecho "Error: Upgrade path not supported for version ${PGSQL_OLD}" "$boldred"
      echo
      read -rep "Press [Enter] to continue..."
      return
      ;;
  esac

  echo "Upgrade Path: PostgreSQL ${PGSQL_OLD} → ${PGSQL_NEW}"
  echo ""

  read -rep "Have you created a full backup? [y/n]: " backup_confirm
  if [[ ! "$backup_confirm" =~ ^[Yy]$ ]]; then
    cecho "Please create a backup first using:" "$boldyellow"
    echo "  sudo -u postgres pg_dumpall > /root/pgsql_backup_\$(date +%Y%m%d).sql"
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  read -rep "Proceed with upgrade from ${PGSQL_OLD} to ${PGSQL_NEW}? [y/n]: " proceed
  if [[ ! "$proceed" =~ ^[Yy]$ ]]; then
    echo "Upgrade cancelled."
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  echo ""
  cecho "Starting PostgreSQL upgrade..." "$boldgreen"
  echo ""

  # Set paths
  PGDATA_OLD="/var/lib/pgsql/${PGSQL_OLD}/data"
  PGDATA_NEW="/var/lib/pgsql/${PGSQL_NEW}/data"
  UPGRADE_LOG="${CENTMINLOGDIR}/postgresql_upgrade_${PGSQL_OLD}_to_${PGSQL_NEW}_$(date +%Y%m%d_%H%M%S).log"

  {
    echo "=== PostgreSQL Upgrade Log ==="
    echo "Date: $(date)"
    echo "From: PostgreSQL ${PGSQL_OLD}"
    echo "To: PostgreSQL ${PGSQL_NEW}"
    echo ""

    # Step 1: Stop old PostgreSQL
    echo "Step 1: Stopping PostgreSQL ${PGSQL_OLD}..."
    systemctl stop postgresql-${PGSQL_OLD}
    sleep 2

    # Step 2: Initialize new data directory if needed
    if [[ ! -f "$PGDATA_NEW/PG_VERSION" ]]; then
      echo "Step 2: Initializing PostgreSQL ${PGSQL_NEW} data directory..."
      sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/initdb -D "$PGDATA_NEW"
    else
      echo "Step 2: Data directory already initialized"
    fi

    # Step 3: Run pg_upgrade
    echo "Step 3: Running pg_upgrade..."
    cd /var/lib/pgsql || return

    # Determine parallel jobs based on CPU cores (official recommendation)
    PGSQL_JOBS=$(nproc 2>/dev/null || echo 2)
    # Cap at 4 jobs for safety
    [[ "$PGSQL_JOBS" -gt 4 ]] && PGSQL_JOBS=4

    sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/pg_upgrade \
      --old-datadir="$PGDATA_OLD" \
      --new-datadir="$PGDATA_NEW" \
      --old-bindir="/usr/pgsql-${PGSQL_OLD}/bin" \
      --new-bindir="/usr/pgsql-${PGSQL_NEW}/bin" \
      --link \
      --jobs="$PGSQL_JOBS"

    UPGRADE_STATUS=$?

    if [[ "$UPGRADE_STATUS" -eq 0 ]]; then
      echo ""
      echo "Step 4: pg_upgrade completed successfully!"

      # Copy pg_hba.conf and postgresql.conf customizations
      echo "Step 5: Migrating configuration files..."
      cp "${PGDATA_OLD}/pg_hba.conf" "${PGDATA_NEW}/pg_hba.conf.migrated"
      echo "  - pg_hba.conf saved to ${PGDATA_NEW}/pg_hba.conf.migrated"
      echo "  - Review and merge customizations manually"

      # Start new PostgreSQL
      echo "Step 6: Starting PostgreSQL ${PGSQL_NEW}..."
      systemctl start postgresql-${PGSQL_NEW}
      systemctl enable postgresql-${PGSQL_NEW}

      # Disable old service
      systemctl disable postgresql-${PGSQL_OLD}

      echo ""
      echo "=== Upgrade Complete ==="
      echo "New version: $(sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/psql --version)"
      echo ""
      echo "Post-upgrade tasks:"
      echo "  1. Run: sudo -u postgres /usr/pgsql-${PGSQL_NEW}/bin/vacuumdb --all --analyze-in-stages"
      echo "  2. Review pg_hba.conf and postgresql.conf settings"
      echo "  3. Run option 4 (Post-Upgrade Cleanup) to remove old cluster"
    else
      echo ""
      cecho "pg_upgrade FAILED! Rolling back..." "$boldred"
      echo "Starting old PostgreSQL ${PGSQL_OLD}..."
      systemctl start postgresql-${PGSQL_OLD}
      echo ""
      echo "Review errors in /var/lib/pgsql/pg_upgrade_output.d/"
    fi

  } 2>&1 | tee "$UPGRADE_LOG"

  echo ""
  echo "Upgrade log saved to: $UPGRADE_LOG"
  echo
  read -rep "Press [Enter] to continue..."
}

#######################################################################################
# Post-Upgrade Cleanup
#######################################################################################
pgsql_postupgrade_cleanup() {
  cecho "--------------------------------------------------------" "$boldyellow"
  cecho "PostgreSQL Post-Upgrade Cleanup" "$boldgreen"
  cecho "--------------------------------------------------------" "$boldyellow"
  echo ""

  # Detect running version
  PGSQL_CURRENT=""
  for ver in 18 17 16 15 14 13 12 11 10 9.6; do
    if systemctl is-active --quiet postgresql-${ver} 2>/dev/null; then
      PGSQL_CURRENT="${ver}"
      break
    fi
  done

  if [[ -z "$PGSQL_CURRENT" ]]; then
    cecho "Error: No running PostgreSQL service detected" "$boldred"
    echo
    read -rep "Press [Enter] to continue..."
    return
  fi

  echo "Current running version: PostgreSQL ${PGSQL_CURRENT}"
  echo ""

  # Check for delete_old_cluster.sh script
  if [[ -f "/var/lib/pgsql/delete_old_cluster.sh" ]]; then
    echo "Found: /var/lib/pgsql/delete_old_cluster.sh"
    echo ""
    echo "This script will delete the old PostgreSQL data directory."
    cecho "WARNING: This is irreversible! Ensure you have backups." "$boldred"
    echo ""

    read -rep "Run delete_old_cluster.sh to remove old data? [y/n]: " confirm_delete
    if [[ "$confirm_delete" =~ ^[Yy]$ ]]; then
      echo ""
      echo "Executing cleanup script..."
      cd /var/lib/pgsql || return
      sudo -u postgres bash ./delete_old_cluster.sh
      echo ""
      cecho "Old cluster deleted." "$boldgreen"
    else
      echo "Cleanup skipped. You can run it manually later:"
      echo "  cd /var/lib/pgsql && sudo -u postgres bash ./delete_old_cluster.sh"
    fi
  else
    echo "No delete_old_cluster.sh found."
    echo "This script is created by pg_upgrade after successful migration."
  fi

  echo ""
  echo "=== Recommended Post-Upgrade Tasks ==="
  echo ""
  echo "1. Update statistics (run as postgres user):"
  echo "   /usr/pgsql-${PGSQL_CURRENT}/bin/vacuumdb --all --analyze-in-stages"
  echo ""
  echo "2. Reindex if upgrading from PostgreSQL < 14:"
  echo "   /usr/pgsql-${PGSQL_CURRENT}/bin/reindexdb --all"
  echo ""
  echo "3. Review and update extensions:"
  echo "   sudo -u postgres psql -c 'ALTER EXTENSION <ext_name> UPDATE;'"
  echo ""
  echo "4. Update pg_hba.conf if needed:"
  echo "   vi /var/lib/pgsql/${PGSQL_CURRENT}/data/pg_hba.conf"
  echo "   systemctl reload postgresql-${PGSQL_CURRENT}"
  echo ""

  read -rep "Run vacuumdb --all --analyze-in-stages now? [y/n]: " run_vacuum
  if [[ "$run_vacuum" =~ ^[Yy]$ ]]; then
    echo ""
    echo "Running vacuum analyze..."
    sudo -u postgres /usr/pgsql-${PGSQL_CURRENT}/bin/vacuumdb --all --analyze-in-stages
    echo ""
    cecho "Vacuum analyze completed." "$boldgreen"
  fi

  echo
  read -rep "Press [Enter] to continue..."
}
