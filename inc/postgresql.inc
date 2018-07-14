postgresqlinstall() {
    # to allow to persist on centmin mod code updates
    # create persistent config file at /etc/centminmod/custom_config.inc
    # and add to it the line:
    # POSTGRESQL=y    
    # 
    # to update postgresql server via yum use
    # yum -y update postgresql96 postgresql96-devel postgresql96-contrib postgresql96-libs postgresql96-server --disableplugin=priorities
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
}