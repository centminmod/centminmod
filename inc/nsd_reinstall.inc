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
    # check if NSD is v3 or v4
    NSD_VERCHECK=$(echo $NSD_VERSION | cut -d . -f1)
    NSDRUN_VERCHECK=$(nsd -v 2>&1 | head -n1 | awk '{print $3}' | cut -d . -f1)
    echo "Compiling NSD..."
    tar xzvf nsd-${NSD_VERSION}.tar.gz
    cd nsd-${NSD_VERSION}
    make clean
    ./configure
    make${MAKETHREADS}
    # for nsd v4
    if [[ "$NSD_VERCHECK" = '4' ]]; then
        make nsd-mem
    fi
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
    mkdir -p var/run/nsd
    chmod 755 /var/run/nsd
    chown -R nsd:nsd /var/run/nsd/
    chown -R nsd:nsd /var/db/nsd/

    cd /etc/sysconfig/
    cp $CUR_DIR/sysconfig/nsd nsd
    # only run for CentOS 6.x
    if [[ "$CENTOS_SEVEN" != '7' ]]; then
    cd /etc/init.d/
    cp $CUR_DIR/init/nsd nsd
    chmod +x /etc/init.d/nsd
    chkconfig --levels 235 nsd on
    fi # CENTOS_SEVEN != 7
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
    sed -i 's/\/var\/run\/nsd\/nsd.pid/\/run\/nsd\/nsd.pid/' /etc/nsd/nsd.conf
    sed -i 's/\/var\/run\/nsd\/nsd.pid/\/run\/nsd\/nsd.pid/' /etc/init.d/nsd
fi

####################################
if [[ "$NSD_VERCHECK" = '4' || "$NSDRUN_VERCHECK" = '4' ]]; then

# check if /etc/nsd/nsd.conf is nsd v4 format
NSD_FORMATCHECK=$(grep nsdv4 /etc/nsd/nsd.conf)

# calculate number of ip-address entries in /etc/nsd/nsd.conf
NSDIPNO=$(cat /etc/nsd/nsd.conf | awk -F " " '/ip-address:/ {print $2}' | wc -l)
NSDIP=$(cat /etc/nsd/nsd.conf | awk -F " " '/ip-address:/ {print $2}')

if [ "$NSDIPNO" = '1' ]; then
    NSDIP=$NSDIP
fi

if [ -z "$NSD_FORMATCHECK" ]; then

# backup old nsf v3 nsd.conf
cp -a /etc/nsd/nsd.conf /etc/nsd/nsd.conf-backup_nsdv3_$DT

cat >/etc/nsd/nsd.conf<<EOF
# nsdv4
# nsd.conf -- the NSD(8) configuration file, nsd.conf(5).
#
# Copyright (c) 2001-2006, NLnet Labs. All rights reserved.
#
# See LICENSE for the license.
#

server:
    # for ns1 ip on this server
    #ip-address: xxx.xxx.xxx.xxx
    # for ns2 ip on this server
    #ip-address: yyy.yyy.yyy.yyy

    hide-version: yes

    # Maximum number of concurrent TCP connections per server.
    # This option should have a value below 1000.
    tcp-count: 2048

    # Maximum number of queries served on a single TCP connection.
    # By default 0, which means no maximum.
    tcp-query-count: 0

    # Override the default (120 seconds) TCP timeout.
    tcp-timeout: 60

    # zonefile: to store pid for nsd in.
    pidfile: "/var/run/nsd/nsd.pid"

    # The directory for zonefile: files.
    zonesdir: "/etc/nsd"

include: "/etc/nsd/zones/zone.list"
EOF

fi # NSD_FORMATCHECK

# setup nsd v4 compatible zones directory
if [ ! -d /etc/nsd/zones ]; then
    # for nsd v4
    mkdir -p /etc/nsd/zones
    if [ ! -f /etc/nsd/zones/zone.list ]; then
        touch /etc/nsd/zones/zone.list
        # setup example demo.com zone in zone.list
        # printf "zone:\n\tname: "demo.com"\n\tzonefile: "master/demo.com.zone"\n" >> /etc/nsd/zones/zone.list

for i in /etc/nsd/master/*.zone
do
    printf "zone:\n\tname: %s\n\tzonefile: %s\n" "$(basename $i zone)" "$i"
done > /etc/nsd/zones/zone.list

    fi
fi # /etc/nsd/zones check

fi # NSD_VERCHECK
####################################

    chown -R nsd:nsd /var/run/nsd/
    chown -R nsd:nsd /var/db/nsd/
    chown -R nsd:nsd /etc/nsd/
    chmod 0755/var/db/nsd/

#killall -9 nsd
/etc/init.d/nsd stop
/etc/init.d/nsd start

echo "check if nsd is running..."
ps aux | grep nsd

    echo "*************************************************"
    cecho "* NSD Re-installed" $boldgreen
    echo "*************************************************"
}
