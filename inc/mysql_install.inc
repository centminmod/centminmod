mysqlinstallfunct() {

if [[ "$MYSQL_INSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Installing MySQL" $boldgreen
    echo "*************************************************"
    yum -y install mysql mysql-devel mysql-server perl-DBD-MySQL

if [ -f /etc/my.cnf ]; then
cp /etc/my.cnf /etc/my.cnf-original
fi
    cp -R $CUR_DIR/config/mysql/my.cnf /etc/my.cnf

#download mysqltuner mysqlreport

    cd $DIR_TMP


    cmchkconfig mysql on
    cmservice mysql start


    echo "*************************************************"
    cecho "* Starting MySQL Secure Installation" $boldgreen
    echo "*************************************************"
    mysql_secure_installation

mysqlproclimit

    echo "*************************************************"
    cecho "* MySQL installed" $boldgreen
    echo "*************************************************"
    cmservice mysql stop
fi

}