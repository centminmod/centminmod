nsdreinstall() {

    echo "*************************************************"
    cecho "* Re-Installing NSD" $boldgreen
    echo "*************************************************"
    cd ${DIR_TMP}
    if [ -s nsd-${NSD_VERSION}.tar.gz ]; then
        cecho "NSD ${NSD_VERSION} Archive found, skipping download..." $boldgreen 
    else
        $DOWNLOADAPP http://www.nlnetlabs.nl/downloads/nsd/nsd-${NSD_VERSION}.tar.gz $WGETRETRY
    fi
    echo "Compiling NSD..."
    tar xzvf nsd-${NSD_VERSION}.tar.gz
    cd nsd-${NSD_VERSION}
    make clean
    ./configure
    make${MAKETHREADS}
    make install

NSDUSERCHECK=`grep 'nsd' /etc/passwd`

if [ -z "$NSDUSERCHECK" ]; then
    echo "Creating user and group for nsd..."
    groupadd nsd
    useradd -g nsd -s /sbin/nologin -M nsd
fi

if [ -d /etc/nsd ]; then
    echo ""
    echo "backup existing NSD folder"
    cp -a /etc/nsd /etc/nsd_dirbackup_`date +"%d%m%y-%H%M%S"`
    echo ""
ls -lh /etc | grep 'nsd'
    echo ""
fi

	if [ ! -d /etc/nsd/master ]; then
	    mkdir /etc/nsd/master
	fi

	if [ ! -d /etc/nsd/slave ]; then
	    mkdir /etc/nsd/slave
	fi

if [ ! -d /etc/nsd ]; then
    mkdir /etc/nsd

    cp -R $CUR_DIR/config/nsd/* /etc/nsd/
    chown -R nsd:nsd /etc/nsd/
fi

if [ ! -d /var/run/nsd ]; then
    echo "Setting up directories..."
    mkdir /var/run/nsd
    chown -R nsd:nsd /var/run/nsd/
    chown -R nsd:nsd /var/db/nsd/

    cd /etc/sysconfig/
    cp $CUR_DIR/sysconfig/nsd nsd
    cd /etc/init.d/
    cp $CUR_DIR/init/nsd nsd
    chmod +x /etc/init.d/nsd
    chkconfig --levels 235 nsd on

fi

    chown -R nsd:nsd /var/run/nsd/
    chown -R nsd:nsd /var/db/nsd/
    chown -R nsd:nsd /etc/nsd/

#killall -9 nsd
/etc/init.d/nsd stop
/etc/init.d/nsd start

echo "check if nsd is running..."
ps aux | grep nsd

    echo "*************************************************"
    cecho "* NSD Re-installed" $boldgreen
    echo "*************************************************"
}
