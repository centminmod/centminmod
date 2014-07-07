
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
if [[ "$CLOUDFLARE_PATCHSSL" = [yY]] ]; then
    # if [[ "${OPENSSL_VERSION}" = '1.0.1h' ]]; then
        echo  "###################################"
        echo "Patching OpenSSL 1.0.1h"
        echo  "###################################"
        echo "Cloudflare RC4 kill patch"
        echo "https://github.com/cloudflare/sslconfig/blob/master/patches/openssl__disable_rc4.patch"
        echo  "####################################"
        pushd ssl
        rm -rf openssl__disable_rc4.patch
        wget -cnv --no-check-certificate https://github.com/cloudflare/sslconfig/raw/master/patches/openssl__disable_rc4.patch
        patch < openssl__disable_rc4.patch
        popd
        echo  "####################################"
        echo  "OpenSSL 1.0.1h patched"
        echo  "####################################"
    # fi
fi # CLOUDFLARE_PATCHSSL
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

    if [ ! -f /usr/bin/makedepend ]; then
        yum -q -y install imake
    fi

    echo "Compiling OpenSSL..."
#download openssl centos 6.x

# from https://wiki.mozilla.org/Security/Server_Side_TLS#Nginx
export BPATH=$DIR_TMP
export STATICLIBSSL="${BPATH}/staticlibssl"

    if [[ -f "openssl-${OPENSSL_VERSION}.tar.gz" && ! -d "${DIR_TMP}/openssl-${OPENSSL_VERSION}" ]]; then
        tar xzf openssl-${OPENSSL_VERSION}.tar.gz
    fi
    #-- Build static openssl
    cd ${DIR_TMP}/openssl-${OPENSSL_VERSION}
    rm -rf "$STATICLIBSSL"
    mkdir -p "$STATICLIBSSL"
    make clean
    opensslpatches
    ./config --prefix=$STATICLIBSSL no-shared enable-tlsext enable-ec_nistp_64_gcc_128 \
    && make depend \
    && make${MAKETHREADS} \
    && make install

    # ./config --prefix=/usr/local --openssldir=/usr/local/ssl
    # make${MAKETHREADS}
    # make install

    # ./config shared enable-tlsext enable-ec_nistp_64_gcc_128 --prefix=/usr/local --openssldir=/usr/local/ssl
    # make clean
    # make${MAKETHREADS}
    # make install
fi # openssl centos 6

}