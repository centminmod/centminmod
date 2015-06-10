
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
if [[ "$CLOUDFLARE_PATCHSSL" = [yY] ]]; then
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
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
        libresslinstallstarttime=$(date +%s.%N)
        cd $DIR_TMP
        echo "Compiling LibreSSL..."
        cd portable-${LIBRESSL_VERSION}
        make clean
        if [ ! -f configure ]; then
            bash autogen.sh
        fi
        if [[ "$CLANG" = [yY] ]]; then
            export CC="ccache /usr/bin/clang"
            export CXX="ccache /usr/bin/clang++"
            export CCACHE_CPP2=yes
            CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-c++11-extensions -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign'
            CFLAGS="$CLANG_CFLAGOPT"
        else
            export CC="ccache /usr/bin/gcc"
            export CXX="ccache /usr/bin/g++"
            export CCACHE_CPP2=yes
            CLANG_CFLAGOPT=""
            CFLAGS=""
        fi        
        ./configure --prefix=/opt/libressl
        # make -j${MAKETHREADS} check
        make -j${MAKETHREADS}
        make install

        if [[ "$CLANG" = [yY] ]]; then
            unset CC
            unset CXX
            unset CFLAGS
            #unset CCACHE_CPP2
            export CC="ccache /usr/bin/gcc"
            export CXX="ccache /usr/bin/g++"
            CLANG_CFLAGOPT=""
            CFLAGS=""            
        fi                
        libresslinstallendtime=$(date +%s.%N)
        LIBRESSLINSTALLTIME=$(echo "scale=2;$libresslinstallendtime - $libresslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log
        echo "OpenSSL Install Time: $LIBRESSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log         
    else
        opensslinstallstarttime=$(date +%s.%N)
        # Install OpenSSL
        cd $DIR_TMP
        echo "Compiling OpenSSL..."
    
        if [ ! -f /usr/bin/makedepend ]; then
            yum -q -y install imake
        fi
    
        echo "Compiling OpenSSL..."
        #download openssl centos 6.x
    
        # from https://wiki.mozilla.org/Security/Server_Side_TLS#Nginx
        export BPATH=$DIR_TMP
        export STATICLIBSSL="${BPATH}/staticlibssl"
    
        tar xzf ${OPENSSL_LINKFILE}
        #-- Build static openssl
        cd ${DIR_TMP}/openssl-${OPENSSL_VERSION}
        rm -rf "$STATICLIBSSL"
        mkdir -p "$STATICLIBSSL"
        make clean
        opensslpatches
        if [[ "$(uname -m)" = 'x86_64' ]]; then
            ./config --prefix=$STATICLIBSSL no-shared enable-tlsext enable-ec_nistp_64_gcc_128
        else
            ./config --prefix=$STATICLIBSSL no-shared enable-tlsext
        fi
        make depend
        make
        make install
        opensslinstallendtime=$(date +%s.%N)
        OPENSSLINSTALLTIME=$(echo "scale=2;$opensslinstallendtime - $opensslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        echo "OpenSSL Install Time: $OPENSSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log 
    fi
}