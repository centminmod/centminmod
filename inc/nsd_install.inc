nsdinstall() {

if [[ "$NSD_INSTALL" = [yY] ]]; 
then

if [[ -f /etc/init.d/named ]]; then
    cmservice named stop
    cmchkconfig named off
fi

    echo "*************************************************"
    cecho "* Installing NSD" $boldgreen
    echo "*************************************************"
    cd $DIR_TMP

#download nsd

# check if NSD is v3 or v4
NSD_VERCHECK=$(echo $NSD_VERSION | cut -d . -f1)
NSDRUN_VERCHECK=$(nsd -v 2>&1 | head -n1 | awk '{print $3}' | cut -d . -f1)

    echo "Compiling NSD..."
    #tar xzvf nsd-${NSD_VERSION}.tar.gz
    cd nsd-${NSD_VERSION}
    ./configure
    make${MAKETHREADS}
    # for nsd v4
    if [[ "$NSD_VERCHECK" = '4' ]]; then
        make nsd-mem
    fi
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
    # for nsd v4
    mkdir -p /etc/nsd/zones
    touch /etc/nsd/zones/zone.list
    # setup example demo.com zone in zone.list
    printf "zone:\n\tname: "demo.com"\n\tzonefile: "master/demo.com.zone"\n" >> /etc/nsd/zones/zone.list
    chown -R nsd:nsd /etc/nsd/
    chmod 0666 /var/db/nsd/
    cp -R $CUR_DIR/config/nsd/* /etc/nsd/
    cd /etc/sysconfig/
    cp $CUR_DIR/sysconfig/nsd nsd

# # only run for CentOS 6.x
# if [[ "$CENTOS_SEVEN" != '7' ]]; then
    cd /etc/init.d/
    cp $CUR_DIR/init/nsd nsd

# add check for Windows CLRF line endings
if [ ! -f /usr/bin/file ]; then
    yum -q -y install file
fi
if [[ $(file /etc/init.d/nsd) =~ CRLF && -f /etc/init.d/nsd ]]; then
    if [ ! -f /usr/bin/dos2unix ]; then
        yum -q -y install dos2unix
    fi
    echo "detected CRLF line endings converting to Unix LF"
    dos2unix /etc/init.d/nsd
fi
    
    chmod +x /etc/init.d/nsd
    chkconfig --levels 235 nsd on
# fi # CENTOS_SEVEN != 7

    echo "*************************************************"
    cecho "* NSD installed" $boldgreen
    echo "*************************************************"
fi

}