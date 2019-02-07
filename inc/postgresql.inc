postgresqlinstall() {
    # to allow to persist on centmin mod code updates
    # create persistent config file at /etc/centminmod/custom_config.inc
    # and add to it the line:
    # POSTGRESQL=y    
    # 
    # to update postgresql server via yum use
    # yum -y update postgresql96 postgresql96-devel postgresql96-contrib postgresql96-libs postgresql96-server --disableplugin=priorities
  if [[ -f /usr/pgsql-9.6/bin/pg_config ]]; then
    POSTGRESQL_BRANCHVER='9.6'
    PDOPGSQL_PHPVER='9.6'
  elif [[ -f /usr/pgsql-10/bin/pg_config ]]; then
    POSTGRESQL_BRANCHVER='10'
    PDOPGSQL_PHPVER='10'
  elif [[ -f /usr/pgsql-11/bin/pg_config ]]; then
    POSTGRESQL_BRANCHVER='11'
    PDOPGSQL_PHPVER='11'
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
        # cd $DIR_TMP
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-7-x86_64/pgdg-centos96-9.6-3.noarch.rpm -O ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-6-x86_64/pgdg-centos96-9.6-3.noarch.rpm -O ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then        
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-6-i386/pgdg-centos96-9.6-3.noarch.rpm -O ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos96-9.6-3.noarch.rpm
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
        # cd $DIR_TMP
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/10/redhat/rhel-7-x86_64/pgdg-centos10-10-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/10/redhat/rhel-6-x86_64/pgdg-centos10-10-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then        
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/10/redhat/rhel-6-i386/pgdg-centos10-10-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-centos10-10-2.noarch.rpm
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
          systemctl status postgresql-10
        elif [[ "$CENTOS_SIX" = '6' ]]; then
          echo
          service postgresql-10 initdb
          chkconfig postgresql-10 on
          service postgresql-10 start
          service postgresql-10 status
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
        # cd $DIR_TMP
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/11/redhat/rhel-7-x86_64/pgdg-redhat11-11-2.noarch.rpm -O ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/11/redhat/rhel-6-x86_64/pgdg-redhat11-11-2.noarch.rpm -O ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then        
            wget${ipv_forceopt_pgsl} https://download.postgresql.org/pub/repos/yum/11/redhat/rhel-6-i386/pgdg-redhat11-11-2.noarch.rpm -O ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
            rpm -Uvh ${DIR_TMP}/pgdg-redhat11-11-2.noarch.rpm
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
          systemctl status postgresql-11
        elif [[ "$CENTOS_SIX" = '6' ]]; then
          echo
          service postgresql-11 initdb
          chkconfig postgresql-11 on
          service postgresql-11 start
          service postgresql-11 status
        fi
    fi
  fi
}