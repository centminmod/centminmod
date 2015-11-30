postgresqlinstall() {
    # to allow to persist on centmin mod code updates
    # create persistent config file at /etc/centminmod/custom_config.inc
    # and add to it the line:
    # POSTGRESQL=y    
    # 
    # to update postgresql server via yum use
    # yum -y update postgresql94 postgresql94-devel postgresql94-contrib postgresql94-libs postgresql94-server --disableplugin=priorities
    if [[ ! -f /usr/pgsql-9.4/bin/pg_config || ! -f /usr/pgsql-9.4/lib/libpq.a || ! -f /etc/yum.repos.d/pgdg-94-centos.repo ]]; then
        echo
        echo "install and setup PostgreSQL server and devel packages"
    
        # cd $DIR_TMP
        if [[ "$CENTOS_SEVEN" = '7' ]]; then
            wget http://yum.postgresql.org/9.4/redhat/rhel-7-x86_64/pgdg-centos94-9.4-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
            rpm -ivh ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
            wget http://yum.postgresql.org/9.4/redhat/rhel-6-x86_64/pgdg-centos94-9.4-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
            rpm -ivh ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
        elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then        
            wget http://yum.postgresql.org/9.4/redhat/rhel-6-i386/pgdg-centos94-9.4-2.noarch.rpm -O ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
            rpm -ivh ${DIR_TMP}/pgdg-centos94-9.4-2.noarch.rpm
        fi
    
        ls -lah /etc/yum.repos.d/pgdg-94-centos.repo
        cat /etc/yum.repos.d/pgdg-94-centos.repo
    
        echo
        yum list postgresql94 postgresql94-devel postgresql94-contrib postgresql94-libs postgresql94-server --disableplugin=priorities
    
        echo
        yum -y install postgresql94 postgresql94-devel postgresql94-contrib postgresql94-libs postgresql94-server --disableplugin=priorities
    fi
}