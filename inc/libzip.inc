libzip_install() {
    pushd /svr-setup/
    cecho "Download ${LIBZIP_LINKFILE} ..." $boldyellow
    if [ -s "${LIBZIP_LINKFILE}" ]; then
        cecho "${LIBZIP_LINKFILE} found, skipping download..." $boldgreen
    else
        echo "wget -c4 --progress=bar $LIBZIP_LINK --tries=3"
        wget -c4 --progress=bar $LIBZIP_LINK --tries=3
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
            cecho "Error: ${LIBZIP_LINKFILE} download failed." $boldgreen
        else 
            cecho "Download done." $boldyellow
        fi
    fi
    tar xvzf ${LIBZIP_LINKFILE}
    cd libzip-${LIBZIP_VER}
    rm -rf build
    mkdir -p build
    cd build/
    cmake3 ..
    make${MAKETHREADS}
    make install
    popd
}