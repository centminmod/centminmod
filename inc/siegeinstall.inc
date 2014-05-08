checksiege() {
    cecho "Check for siege-${SIEGE_VERSION}.tar.gz" $boldyellow
    if [[ ! -f "${DIR_TMP}/siege-${SIEGE_VERSION}.tar.gz" ]]; then
        siegetarball
    fi
}

siegeinstall() {

#ASK "Install Siege Benchmark script ? [y/n] "
if [[ "$SIEGEINSTALL" = [yY] || "$UALL" = 'y' ]];
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
    ./configure
    make$MAKETHREADS
    make install
    mkdir -p /usr/local/var/

    sed -i 's/# failures =/failures = 2048/g' /usr/local/etc/siegerc
) &

if [[ "$UALL" != 'y' ]]; then 
# subshell background process
(
    cd $DIR_TMP

    #tar -xzf sproxy-latest.tar.gz
    cd sproxy-1.02
    ./configure
    make$MAKETHREADS
    make install
) &
fi # UALL
wait
    echo "*************************************************"
    cecho "* Siege Benchmark installed " $boldgreen
    echo "*************************************************"
fi

}