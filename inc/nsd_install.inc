nsdinstall() {

if [[ "$NSD_INSTALL" = [yY] ]]; 
then

if [[ -f /etc/init.d/named ]]; then
    service named stop
    chkconfig named off
fi

    echo "*************************************************"
    cecho "* Installing NSD" $boldgreen
    echo "*************************************************"
    cd $DIR_TMP

#download nsd

    echo "Compiling NSD..."
    #tar xzvf nsd-${NSD_VERSION}.tar.gz
    cd nsd-${NSD_VERSION}
    ./configure
    make${MAKETHREADS}
    make install
    echo "Creating user and group for nsd..."
    groupadd nsd
    useradd -g nsd -s /sbin/nologin -M nsd
    echo "Setting up directories..."
    mkdir -p /var/run/nsd
    chown -R nsd:nsd /var/run/nsd/
    chown -R nsd:nsd /var/db/nsd/
    mkdir -p /etc/nsd
    mkdir -p /etc/nsd/master
    mkdir -p /etc/nsd/slave
    chown -R nsd:nsd /etc/nsd/
    cp -R $CUR_DIR/config/nsd/* /etc/nsd/
    cd /etc/sysconfig/
    cp $CUR_DIR/sysconfig/nsd nsd
    cd /etc/init.d/
    cp $CUR_DIR/init/nsd nsd
    chmod +x /etc/init.d/nsd
    chkconfig --levels 235 nsd on
    echo "*************************************************"
    cecho "* NSD installed" $boldgreen
    echo "*************************************************"
fi

}