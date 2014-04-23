siegeinstall() {

#ASK "Install Siege Benchmark script ? [y/n] "
if [[ "$SIEGEINSTALL" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Installing Siege Benchmark... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

    cd $DIR_TMP

#download siege

    #tar -xzf siege-${SIEGE_VERSION}.tar.gz
    cd siege-${SIEGE_VERSION}
    ./configure
    make$MAKETHREADS
    make install
    mkdir /usr/local/var/

    sed -i 's/# failures =/failures = 2048/g' /usr/local/etc/siegerc

    cd $DIR_TMP

    #tar -xzf sproxy-latest.tar.gz
    cd sproxy-1.02
    ./configure
    make$MAKETHREADS
    make install

    echo "*************************************************"
    cecho "* Siege Benchmark installed " $boldgreen
    echo "*************************************************"
fi

}