pgsql_disable_eol_repos() {
    # Disable EOL PostgreSQL PGDG repo entries to prevent yum/dnf 404 errors.
    # EOL: 9.6 (Nov 2021), 10 (Nov 2022), 11 (Nov 2023), 12 (Nov 2024), 13 (Nov 2025)
    # Uses conditional yum-config-manager (EL7) / dnf config-manager (EL8/9/10)
    # with EL10 dnf5 syntax: dnf config-manager setopt <repo-id>.enabled=0
    local pgdg_repo_file="/etc/yum.repos.d/pgdg-redhat-all.repo"
    if [[ ! -f "$pgdg_repo_file" ]]; then
        return 0
    fi
    local eol_repos="pgdg96 pgdg10 pgdg11 pgdg12 pgdg13"
    for repo_id in $eol_repos; do
        if grep -q "^\[${repo_id}\]" "$pgdg_repo_file" 2>/dev/null; then
            if [[ "$CENTOS_SEVEN" = '7' ]]; then
                yum-config-manager --disable "$repo_id" >/dev/null 2>&1
                yum-config-manager --disable "${repo_id}-source" >/dev/null 2>&1
            elif [[ "$CENTOS_TEN" = '10' ]]; then
                dnf config-manager setopt "${repo_id}.enabled=0" >/dev/null 2>&1
                dnf config-manager setopt "${repo_id}-source.enabled=0" >/dev/null 2>&1
            elif [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
                dnf config-manager --set-disabled "$repo_id" >/dev/null 2>&1
                dnf config-manager --set-disabled "${repo_id}-source" >/dev/null 2>&1
            fi
        fi
    done
}

postgresqlinstall() {
    # to allow to persist on centmin mod code updates
    # create persistent config file at /etc/centminmod/custom_config.inc
    # and add to it the line:
    # POSTGRESQL=y
    #
    # to update postgresql server via yum use
    # yum -y update postgresql18 postgresql18-devel postgresql18-contrib postgresql18-libs postgresql18-server --disableplugin=priorities
    # Only check versions if POSTGRESQL_BRANCHVER is not already set
    if [[ -z "$POSTGRESQL_BRANCHVER" ]]; then
      if [[ -f /usr/pgsql-18/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='18'
        PDOPGSQL_PHPVER='18'
        PGSQL18_DATA_DIR="/var/lib/pgsql/18/data"
      elif [[ -f /usr/pgsql-17/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='17'
        PDOPGSQL_PHPVER='17'
        PGSQL17_DATA_DIR="/var/lib/pgsql/17/data"
      elif [[ -f /usr/pgsql-16/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='16'
        PDOPGSQL_PHPVER='16'
        PGSQL16_DATA_DIR="/var/lib/pgsql/16/data"
      elif [[ -f /usr/pgsql-15/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='15'
        PDOPGSQL_PHPVER='15'
      elif [[ -f /usr/pgsql-14/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='14'
        PDOPGSQL_PHPVER='14'
      # Legacy EOL versions (EL7 backward compat)
      elif [[ -f /usr/pgsql-13/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='13'
        PDOPGSQL_PHPVER='13'
      elif [[ -f /usr/pgsql-12/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='12'
        PDOPGSQL_PHPVER='12'
      elif [[ -f /usr/pgsql-11/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='11'
        PDOPGSQL_PHPVER='11'
      elif [[ -f /usr/pgsql-10/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='10'
        PDOPGSQL_PHPVER='10'
      elif [[ -f /usr/pgsql-9.6/bin/pg_config ]]; then
        POSTGRESQL_BRANCHVER='9.6'
        PDOPGSQL_PHPVER='9.6'
      fi
    else
      # If POSTGRESQL_BRANCHVER is set, check if the correct version is installed
      if [[ "$POSTGRESQL_BRANCHVER" = '18' && -f /usr/pgsql-18/bin/pg_config ]]; then
        PDOPGSQL_PHPVER='18'
        PGSQL18_DATA_DIR="/var/lib/pgsql/18/data"
      elif [[ "$POSTGRESQL_BRANCHVER" = '17' && -f /usr/pgsql-17/bin/pg_config ]]; then
        PDOPGSQL_PHPVER='17'
        PGSQL17_DATA_DIR="/var/lib/pgsql/17/data"
      elif [[ "$POSTGRESQL_BRANCHVER" = '16' && -f /usr/pgsql-16/bin/pg_config ]]; then
        PDOPGSQL_PHPVER='16'
        PGSQL16_DATA_DIR="/var/lib/pgsql/16/data"
      else
        echo "Error: PostgreSQL version $POSTGRESQL_BRANCHVER is set but not installed."
      fi
    fi

  if [[ "$POSTGRESQL_BRANCHVER" = '9.6' ]]; then
    if [[ ! -f /usr/pgsql-9.6/bin/pg_config || ! -f /usr/pgsql-9.6/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-96-centos.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        ls -lah /etc/yum.repos.d/pgdg-96-centos.repo
        cat /etc/yum.repos.d/pgdg-96-centos.repo

        echo
        time $YUMDNFBIN list postgresql96 postgresql96-devel postgresql96-contrib postgresql96-libs postgresql96-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        time $YUMDNFBIN -y install postgresql96 postgresql96-devel postgresql96-contrib postgresql96-libs postgresql96-server --disableplugin=priorities${DISABLEREPO_DNF}
    fi
  fi
  if [[ "$POSTGRESQL_BRANCHVER" = '10' ]]; then
    if [[ ! -f /usr/pgsql-10/bin/pg_config || ! -f /usr/pgsql-10/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-10-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        ls -lah /etc/yum.repos.d/pgdg-10-redhat.repo
        cat /etc/yum.repos.d/pgdg-10-redhat.repo

        echo
        time $YUMDNFBIN list postgresql10 postgresql10-devel postgresql10-contrib postgresql10-libs postgresql10-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        time $YUMDNFBIN -y install postgresql10 postgresql10-devel postgresql10-contrib postgresql10-libs postgresql10-server --disableplugin=priorities${DISABLEREPO_DNF}

        if [[ "$CENTOS_SEVEN" = '7' ]]; then
          echo
          /usr/pgsql-10/bin/postgresql-10-setup initdb
          systemctl enable postgresql-10
          systemctl start postgresql-10
          systemctl status postgresql-10 --no-pager
        fi
    fi
  fi
  if [[ "$POSTGRESQL_BRANCHVER" = '11' ]]; then
    if [[ ! -f /usr/pgsql-11/bin/pg_config || ! -f /usr/pgsql-11/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-11-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        ls -lah /etc/yum.repos.d/pgdg-11-redhat.repo
        cat /etc/yum.repos.d/pgdg-11-redhat.repo

        echo
        time $YUMDNFBIN list postgresql11 postgresql11-devel postgresql11-contrib postgresql11-libs postgresql11-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        time $YUMDNFBIN -y install postgresql11 postgresql11-devel postgresql11-contrib postgresql11-libs postgresql11-server --disableplugin=priorities${DISABLEREPO_DNF}

        if [[ "$CENTOS_SEVEN" = '7' ]]; then
          echo
          /usr/pgsql-11/bin/postgresql-11-setup initdb
          systemctl enable postgresql-11
          systemctl start postgresql-11
          systemctl status postgresql-11 --no-pager
        fi
    fi
  fi
  if [[ "$POSTGRESQL_BRANCHVER" = '12' ]]; then
    if [[ ! -f /usr/pgsql-12/bin/pg_config || ! -f /usr/pgsql-12/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-12-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        ls -lah /etc/yum.repos.d/pgdg-12-redhat.repo
        cat /etc/yum.repos.d/pgdg-12-redhat.repo

        echo
        time $YUMDNFBIN list postgresql12 postgresql12-devel postgresql12-contrib postgresql12-libs postgresql12-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        time $YUMDNFBIN -y install postgresql12 postgresql12-devel postgresql12-contrib postgresql12-libs postgresql12-server --disableplugin=priorities${DISABLEREPO_DNF}

        if [[ "$CENTOS_SEVEN" = '7' ]]; then
          echo
          /usr/pgsql-12/bin/postgresql-12-setup initdb
          systemctl enable postgresql-12
          systemctl start postgresql-12
          systemctl status postgresql-12 --no-pager
        fi
    fi
  fi
  if [[ "$POSTGRESQL_BRANCHVER" = '13' ]]; then
    if [[ ! -f /usr/pgsql-13/bin/pg_config || ! -f /usr/pgsql-13/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-13-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        ls -lah /etc/yum.repos.d/pgdg-13-redhat.repo
        cat /etc/yum.repos.d/pgdg-13-redhat.repo

        echo
        time $YUMDNFBIN list postgresql13 postgresql13-devel postgresql13-contrib postgresql13-libs postgresql13-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        time $YUMDNFBIN -y install postgresql13 postgresql13-devel postgresql13-contrib postgresql13-libs postgresql13-server --disableplugin=priorities${DISABLEREPO_DNF}

        if [[ "$CENTOS_SEVEN" = '7' ]]; then
          echo
          /usr/pgsql-13/bin/postgresql-13-setup initdb
          systemctl enable postgresql-13
          systemctl start postgresql-13
          systemctl status postgresql-13 --no-pager
        fi
    fi
  fi
  if [[ "$CENTOS_SEVEN" = '7' && "$POSTGRESQL_BRANCHVER" = '16' && ! -d /var/lib/pgsql/15/data ]]; then
    # https://www.postgresql.org/download/linux/redhat/
    # PG 16+ not available on EL7 PGDG repos; install PG 15 (max for EL7)
    if [[ ! -f /usr/pgsql-15/bin/pg_config || ! -f /usr/pgsql-15/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-15-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
        rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
        pgsql_disable_eol_repos
        yum -y makecache

        echo
        ls -lAhrt /etc/yum.repos.d/
        echo
        echo "cat /etc/yum.repos.d/pgdg-redhat-all.repo"
        cat /etc/yum.repos.d/pgdg-redhat-all.repo

        echo
        # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
        rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL7
        yum makecache
        time $YUMDNFBIN -y list postgresql15 postgresql15-devel libpq5-devel postgresql15-contrib postgresql15-libs postgresql15-server --disableplugin=priorities${DISABLEREPO_DNF}
        echo
        time $YUMDNFBIN -y swap -- remove postgresql-libs -- install libpq5
        time $YUMDNFBIN -y install postgresql15 postgresql15-devel libpq5-devel postgresql15-contrib postgresql15-libs postgresql15-server --disableplugin=priorities${DISABLEREPO_DNF}

        echo
        /usr/pgsql-15/bin/postgresql-15-setup initdb
        systemctl enable postgresql-15
        systemctl start postgresql-15
        systemctl status postgresql-15 --no-pager
    fi
  fi
  if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]] && [[ "$POSTGRESQL_BRANCHVER" = '16' && ! -d "$PGSQL16_DATA_DIR" ]]; then
    # https://www.postgresql.org/download/linux/redhat/
    if [[ ! -f /usr/pgsql-16/bin/pg_config || ! -f /usr/pgsql-16/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-16-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_TEN" = '10' ]]; then
            # install postgresql 16 instead of default el10 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-10-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_NINE" = '9' ]]; then
            # install postgresql 16 instead of default el9 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-9-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
            # install postgresql 16 instead of default el8 module postgresql 13, 12, 10 and 9.6
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        echo
        ls -lAhrt /etc/yum.repos.d/
        echo
        echo "cat /etc/yum.repos.d/pgdg-redhat-all.repo"
        cat /etc/yum.repos.d/pgdg-redhat-all.repo

        echo
        time $YUMDNFBIN -y list postgresql16 postgresql16-devel libpq5-devel postgresql16-contrib postgresql16-libs postgresql16-server --disableplugin=priorities${DISABLEREPO_DNF}
        echo
        time $YUMDNFBIN -y install postgresql16 postgresql16-devel libpq5-devel postgresql16-contrib postgresql16-libs postgresql16-server --disableplugin=priorities${DISABLEREPO_DNF} --skip-broken

        if [[ "$CENTOS_TEN" = '10' ]]; then
          echo
          if [[ ! -d "$PGSQL16_DATA_DIR" && ! -f "$PGSQL16_DATA_DIR/PG_VERSION" && ! -d "$PGSQL16_DATA_DIR/base" ]]; then
            /usr/pgsql-16/bin/postgresql-16-setup initdb
          fi
          systemctl enable postgresql-16
          systemctl start postgresql-16
          systemctl status postgresql-16 --no-pager
        elif [[ "$CENTOS_NINE" = '9' ]]; then
          echo
          if [[ ! -d "$PGSQL16_DATA_DIR" && ! -f "$PGSQL16_DATA_DIR/PG_VERSION" && ! -d "$PGSQL16_DATA_DIR/base" ]]; then
            /usr/pgsql-16/bin/postgresql-16-setup initdb
          fi
          systemctl enable postgresql-16
          systemctl start postgresql-16
          systemctl status postgresql-16 --no-pager
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
          echo
          if [[ ! -d "$PGSQL16_DATA_DIR" && ! -f "$PGSQL16_DATA_DIR/PG_VERSION" && ! -d "$PGSQL16_DATA_DIR/base" ]]; then
            /usr/pgsql-16/bin/postgresql-16-setup initdb
          fi
          systemctl enable postgresql-16
          systemctl start postgresql-16
          systemctl status postgresql-16 --no-pager
        fi
    fi
  fi
  if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]] && [[ "$POSTGRESQL_BRANCHVER" = '17' && ! -d "$PGSQL17_DATA_DIR" ]]; then
    # https://www.postgresql.org/download/linux/redhat/
    if [[ ! -f /usr/pgsql-17/bin/pg_config || ! -f /usr/pgsql-17/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-17-redhat.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_TEN" = '10' ]]; then
            # install postgresql 17 instead of default el10 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-10-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_NINE" = '9' ]]; then
            # install postgresql 17 instead of default el9 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-9-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
            # install postgresql 17 instead of default el8 module postgresql 13, 12, 10 and 9.6
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        echo
        ls -lAhrt /etc/yum.repos.d/
        echo
        echo "cat /etc/yum.repos.d/pgdg-redhat-all.repo"
        cat /etc/yum.repos.d/pgdg-redhat-all.repo

        echo
        time $YUMDNFBIN -y list postgresql17 postgresql17-devel libpq5-devel postgresql17-contrib postgresql17-libs postgresql17-server --disableplugin=priorities${DISABLEREPO_DNF}
        echo
        time $YUMDNFBIN -y install postgresql17 postgresql17-devel libpq5-devel postgresql17-contrib postgresql17-libs postgresql17-server --disableplugin=priorities${DISABLEREPO_DNF} --skip-broken

        if [[ "$CENTOS_TEN" = '10' ]]; then
          echo
          if [[ ! -d "$PGSQL17_DATA_DIR" && ! -f "$PGSQL17_DATA_DIR/PG_VERSION" && ! -d "$PGSQL17_DATA_DIR/base" ]]; then
            /usr/pgsql-17/bin/postgresql-17-setup initdb
          fi
          systemctl enable postgresql-17
          systemctl start postgresql-17
          systemctl status postgresql-17 --no-pager
        elif [[ "$CENTOS_NINE" = '9' ]]; then
          echo
          if [[ ! -d "$PGSQL17_DATA_DIR" && ! -f "$PGSQL17_DATA_DIR/PG_VERSION" && ! -d "$PGSQL17_DATA_DIR/base" ]]; then
            /usr/pgsql-17/bin/postgresql-17-setup initdb
          fi
          systemctl enable postgresql-17
          systemctl start postgresql-17
          systemctl status postgresql-17 --no-pager
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
          echo
          if [[ ! -d "$PGSQL17_DATA_DIR" && ! -f "$PGSQL17_DATA_DIR/PG_VERSION" && ! -d "$PGSQL17_DATA_DIR/base" ]]; then
            /usr/pgsql-17/bin/postgresql-17-setup initdb
          fi
          systemctl enable postgresql-17
          systemctl start postgresql-17
          systemctl status postgresql-17 --no-pager
        fi
    fi
  fi
  if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]] && [[ "$POSTGRESQL_BRANCHVER" = '18' && ! -d "$PGSQL18_DATA_DIR" ]]; then
    # https://www.postgresql.org/download/linux/redhat/
    if [[ ! -f /usr/pgsql-18/bin/pg_config || ! -f /usr/pgsql-18/lib/libpq.a ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
        if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
            ipv_forceopt_pgsl=""
        else
            ipv_forceopt_pgsl=' -4'
        fi
        if [[ "$CENTOS_TEN" = '10' ]]; then
            # install postgresql 18 instead of default el10 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-10-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_NINE" = '9' ]]; then
            # install postgresql 18 instead of default el9 module postgresql 13
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-9-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
            # install postgresql 18 instead of default el8 module postgresql 13, 12, 10 and 9.6
            time dnf -y module disable postgresql
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm -O ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            # rpm --import https://download.postgresql.org/pub/repos/yum/RPM-GPG-KEY-PGDG
            rpm --import https://download.postgresql.org/pub/repos/yum/keys/PGDG-RPM-GPG-KEY-RHEL
            rpm -Uvh ${DIR_TMP}/pgdg-redhat-repo-latest.noarch.rpm
            sed -i 's/gpgcheck=9/gpgcheck=1/g' /etc/yum.repos.d/pgdg-redhat-all.repo
            pgsql_disable_eol_repos
            yum -y makecache
        fi

        echo
        ls -lAhrt /etc/yum.repos.d/
        echo
        echo "cat /etc/yum.repos.d/pgdg-redhat-all.repo"
        cat /etc/yum.repos.d/pgdg-redhat-all.repo

        echo
        time $YUMDNFBIN -y list postgresql18 postgresql18-devel libpq5-devel postgresql18-contrib postgresql18-libs postgresql18-server --disableplugin=priorities${DISABLEREPO_DNF}
        echo
        time $YUMDNFBIN -y install postgresql18 postgresql18-devel libpq5-devel postgresql18-contrib postgresql18-libs postgresql18-server --disableplugin=priorities${DISABLEREPO_DNF} --skip-broken

        if [[ "$CENTOS_TEN" = '10' ]]; then
          echo
          if [[ ! -d "$PGSQL18_DATA_DIR" && ! -f "$PGSQL18_DATA_DIR/PG_VERSION" && ! -d "$PGSQL18_DATA_DIR/base" ]]; then
            /usr/pgsql-18/bin/postgresql-18-setup initdb
          fi
          systemctl enable postgresql-18
          systemctl start postgresql-18
          systemctl status postgresql-18 --no-pager
        elif [[ "$CENTOS_NINE" = '9' ]]; then
          echo
          if [[ ! -d "$PGSQL18_DATA_DIR" && ! -f "$PGSQL18_DATA_DIR/PG_VERSION" && ! -d "$PGSQL18_DATA_DIR/base" ]]; then
            /usr/pgsql-18/bin/postgresql-18-setup initdb
          fi
          systemctl enable postgresql-18
          systemctl start postgresql-18
          systemctl status postgresql-18 --no-pager
        elif [[ "$CENTOS_EIGHT" = '8' ]]; then
          echo
          if [[ ! -d "$PGSQL18_DATA_DIR" && ! -f "$PGSQL18_DATA_DIR/PG_VERSION" && ! -d "$PGSQL18_DATA_DIR/base" ]]; then
            /usr/pgsql-18/bin/postgresql-18-setup initdb
          fi
          systemctl enable postgresql-18
          systemctl start postgresql-18
          systemctl status postgresql-18 --no-pager
        fi
    fi
  fi
}
