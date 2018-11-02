checksiege() {
    cecho "Check for siege-${SIEGE_VERSION}.tar.gz" $boldyellow
    if [[ ! -f "${DIR_TMP}/siege-${SIEGE_VERSION}.tar.gz" ]]; then
        siegetarball
    fi
}

siegeinstall() {

# siege is compulsory
SIEGEINSTALL='y'

#ASK "Install Siege Benchmark script ? [y/n] "
if [[ "$SIEGEINSTALL" = [yY] || "$UALL" = 'y' ]] && [[ "$(uname -m)" != 'aarch64' ]];
then
    echo "*************************************************"
    cecho "* Installing Siege Benchmark... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

# subshell background process
(
    cd $DIR_TMP

#download siege

    #tar -xzf siege-${SIEGE_VERSION}.tar.gz
    cd siege-${SIEGE_VERSION}
    ./configure --with-ssl=/usr
    sar_call
    make$MAKETHREADS
    sar_call
    make install
    sar_call
    mkdir -p /usr/local/var/

    siege -q -b -c1 -r1 localhost >/dev/null 2>&1
    # sed -i 's/# failures =/failures = 2048/g' /usr/local/etc/siegerc
    sed -i 's/# failures =/failures = 2048/g' /root/.siege/siege.conf
    #sed -i 's|^logging = false|logging = true|' /root/.siege/siege.conf
    sed -i 's|limit = 255|limit = 4095|' /root/.siege/siege.conf
    \cp -af /root/.siege/siege.conf /root/.siege/siege-csv.conf
    sed -i 's|# csv = true|csv = true|' /root/.siege/siege-csv.conf
    sed -i 's|# logfile = |logfile = /usr/local/var/log/siege-csv.log|' /root/.siege/siege-csv.conf
) &

if [[ "$UALL" != 'y' ]]; then 
# subshell background process
(
    cd $DIR_TMP

    #tar -xzf sproxy-latest.tar.gz
    cd sproxy-1.02
    ./configure
    sar_call
    make$MAKETHREADS
    sar_call
    make install
    sar_call
) &
fi # UALL
wait
    echo "*************************************************"
    cecho "* Siege Benchmark installed " $boldgreen
    echo "*************************************************"
fi

}