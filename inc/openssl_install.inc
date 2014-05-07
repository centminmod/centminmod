
opensslpatches() {
# release buffer patch CVE-2010-5298
if [[ "${OPENSSL_VERSION}" = '1.0.1g' ]]; then
    echo  "###################################"
    echo "Patching OpenSSL 1.0.1g"
    echo  "###################################"
    echo "CVE-2010-5298"
    echo "http://www.cvedetails.com/cve/CVE-2010-5298/"
    echo  "####################################"
    pushd ssl
    rm -rf releasebuffer.patch
    wget -cnv http://centminmod.com/centminmodparts/openssl/patches/releasebuffer.patch
    patch < releasebuffer.patch
    popd
    echo  "####################################"
    echo  "OpenSSL 1.0.1g patched"
    echo  "####################################"
fi
}

installopenssl() {

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then
    cd $DIR_TMP

    echo "Compiling OpenSSL..."
#download openssl centos 5.x

    #tar xvzf openssl-${OPENSSL_VERSION}.tar.gz
fi # for nginx openssl

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
    # Install OpenSSL
    cd $DIR_TMP

    echo "Compiling OpenSSL..."
#download openssl centos 6.x

    if [[ -f "openssl-${OPENSSL_VERSION}.tar.gz" && ! -d "${DIR_TMP}/openssl-${OPENSSL_VERSION}" ]]; then
        tar xzf openssl-${OPENSSL_VERSION}.tar.gz
    fi
    cd openssl-${OPENSSL_VERSION}
	opensslpatches
    ./config --prefix=/usr/local --openssldir=/usr/local/ssl
    make
    make install

    ./config shared enable-tlsext --prefix=/usr/local --openssldir=/usr/local/ssl
    make clean
    make
    make install
fi # openssl centos 6

}