
opensslpatches() {
  {
# release buffer patch CVE-2010-5298
if [[ "${OPENSSL_VERSION}" = '1.0.1g' ]]; then
    echo  "######################################################################"
    echo "Patching OpenSSL 1.0.1g"
    echo  "######################################################################"
    echo "CVE-2010-5298"
    echo "http://www.cvedetails.com/cve/CVE-2010-5298/"
    echo  "######################################################################"
    pushd ssl
    rm -rf releasebuffer.patch
    wget -cnv https://centminmod.com/centminmodparts/openssl/patches/releasebuffer.patch
    patch < releasebuffer.patch
    popd
    echo  "######################################################################"
    echo  "OpenSSL 1.0.1g patched"
    echo  "######################################################################"
fi

if [[ "${OPENSSL_VERSION}" = '1.0.2i' || "${OPENSSL_VERSION}" = '1.0.2m' ]]; then
    # cloudflare patch chacha20 support does not seem to
    # work with openssl 1.0.2i
    # https://community.centminmod.com/posts/36691/
    CLOUDFLARE_PATCHSSL='n'
fi

if [[ "$(uname -m)" != 'x86_64' ]]; then
    # cloudflare patch chacha20 support does not seem to
    # work on 32bit OSes only 64bit
    CLOUDFLARE_PATCHSSL='n'
fi

DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1,2)

if [[ "$CLOUDFLARE_PATCHSSL" = [yY] && "$DETECTOPENSSL_ONEZERO" = '1.0' ]]; then
    # if [[ "${OPENSSL_VERSION}" = '1.0.1h' ]]; then
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Cloudflare Chacha20 Patch"
        fi
        echo  "######################################################################"
        echo "Patching OpenSSL 1.0.2j"
        echo  "######################################################################"
        echo "Cloudflare ChaCha20 patch"
        echo "https://github.com/cloudflare/sslconfig/blob/master/patches/openssl__chacha20_poly1305_draft_and_rfc_ossl102j.patch"
        echo "only Android 5 will support ChaCha20 if using Cloudflare Patch"
        echo  "######################################################################"
        # check if cpu supports avx2 (Intel Xeon E5 v3 and Xeon E3 v5) & implement 
        # cloudflare openssl 1.0.2+ patch with optimized avx2 seal if cpu supports it
        AVXTWO_CHECK=$(grep -o --color avx2 /proc/cpuinfo | uniq >/dev/null 2>&1; echo $?)
        # pushd ssl
        OPESSLCFPATCH_NAME='openssl__chacha20_poly1305_draft_and_rfc_ossl102j.patch'
        rm -rf "${OPESSLCFPATCH_NAME}"
        AVXTWO_CHECK=1
        if [[ "$AVXTWO_CHECK" = '0' ]]; then
            OPENSSLCFPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/optimize_chacha_poly/patches/${OPESSLCFPATCH_NAME}"
        else
            OPENSSLCFPATCHLINK="https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/${OPESSLCFPATCH_NAME}"
        fi
        
        # fallback mirror if github down, use gitlab mirror
        curl -sI --connect-timeout 5 --max-time 5 "${OPENSSLCFPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
        OPENSSLCFPATCH_CURLCHECK=$?
        if [[ "$OPENSSLCFPATCH_CURLCHECK" != '0' ]]; then
            if [[ "$AVXTWO_CHECK" = '0' ]]; then
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/upstream/optimize_chacha_poly/patches/${OPESSLCFPATCH_NAME}"
            else
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${OPESSLCFPATCH_NAME}"
            fi
        fi
        wget -cnv --no-check-certificate "$OPENSSLCFPATCHLINK"

        if [ ! -f crypto/chacha20_poly1305/chacha20.c ]; then
            patch -p1 < "${OPESSLCFPATCH_NAME}"
        fi
        # rm -rf openssl__chacha20_poly1305_cf.patch
        # wget -cnv --no-check-certificate https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/openssl__chacha20_poly1305_cf.patch
        # patch -p1 < openssl__chacha20_poly1305_cf.patch
        if [ -f crypto/chacha20_poly1305/chacha20.c ]; then
            # check /svr-setup/openssl-1.0.2g/crypto/chacha20_poly1305/chacha20.c exists
            OPEENSSL_CFPATCHED='y'
            echo  "######################################################################"
            echo  "OpenSSL 1.0.2j patched"
            echo  "######################################################################"
            if [ -f "$(which figlet)" ]; then
                figlet -ckf standard "Cloudflare Chacha20 Patched"
            fi
        else
            echo  "######################################################################"
            echo  "OpenSSL 1.0.2j not patched"
            echo  "######################################################################"
            if [ -f "$(which figlet)" ]; then
                figlet -ckf standard "Cloudflare Chacha20 Not Patched"
            fi
        fi
        # popd
    # fi
fi # CLOUDFLARE_PATCHSSL

if [[ "$CLOUDFLARE_PATCHSSL" = [yY] && "$DETECTOPENSSL_ONEZERO" = '1.1' ]]; then
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Cloudflare OpenSSL 1.1 Smarter Chacha20 Patch"
        fi
        echo  "######################################################################"
        echo "Patching OpenSSL 1.1.0 branch"
        echo  "######################################################################"
        echo "Cloudflare Smart ChaCha20 patch"
        echo "https://community.centminmod.com/posts/35727/"
        echo "only support ChaCha20 if client's preferred cipher"
        echo  "######################################################################"
        echo "$CUR_DIR/patches/openssl/chacha20-smarter.patch"
        if [ -f "$CUR_DIR/patches/openssl/chacha20-smarter.patch" ]; then
            patch -p1 < "$CUR_DIR/patches/openssl/chacha20-smarter.patch"
            echo  "######################################################################"
            echo  "OpenSSL 1.1.0 branch Smart Chacha20 patched"
            echo  "######################################################################"
            if [ -f "$(which figlet)" ]; then
                figlet -ckf standard "Cloudflare OpenSSL 1.1 Smarter Chacha20 Patched"
            fi
        fi
fi # CLOUDFLARE_PATCHSSL
  } 2>&1 | tee "${CENTMINLOGDIR}/patch_opensslpatches_${DT}.log"
}

crypto_gcc() {
    if [[ "$CRYPTO_DEVTOOLSETGCC" = [yY] ]]; then
      if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] || [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      fi
    fi
    if [[ "$CRYPTO_DEVTOOLSETGCC" = [yY] ]]; then
        # intel specific
        CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
        SSECHECK=$(gcc -c -Q -march=native --help=target | awk '/  -msse/ {print $2}' | head -n1)
        CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
        CPUMODELNO=$(grep -v 'model name' /proc/cpuinfo | awk -F ": " '/model/ {print $2}' | uniq)
        SSEFOURTWOCHECK=$(grep -o sse4_2 /proc/cpuinfo | sort -u | wc -l)
        MARCHCHECK=$(gcc -c -Q -march=native --help=target | awk '/  -march=/ {print $2}' | head -n1)
        gcc --version | tee ${CENTMINLOGDIR}/gcc_crypto_native.log
        gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_crypto_native.log
        
        if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]]; then
            CCM=64
            CRYPTOGCC_OPT="-m${CCM} -march=native"
            # if only 1 cpu thread use -O2 to keep compile times sane
            if [[ "$CPUS" = '1' ]]; then
            export CFLAGS="-O2 $CRYPTOGCC_OPT -pipe"
            else
            export CFLAGS="-O3 $CRYPTOGCC_OPT -pipe"
            fi
            export CXXFLAGS="$CFLAGS"
        fi
    fi
}

crypto_gccunset() {
if [[ "$CRYPTO_DEVTOOLSETGCC" = [yY] ]]; then
    unset CC
    unset CXX
    unset CFLAGS
    unset CXXFLAGS
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
fi
}

installopenssl() {
    if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
        # lua nginx 0.10.7 and lower not compatible with openssl 1.1.x branch
        # so fall back to 1.0.2 branch if lua nginx module is enabled
        OPENSSL_VERSION="$OPENSSL_VERSIONFALLBACK"
    fi
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1,2)
    if [ ! -f /usr/local/go/bin/go ]; then
        # if golang is not detected BoringSSL switch will be disabled
        # and default to using OpenSSL 1.0.2+
        BORINGSSL_SWITCH='n'
    fi
    if [[ "$LIBRESSL_SWITCH" = [yY] ]] && [[ "$BORINGSSL_SWITCH" = [nN] ]]; then
        libresslinstallstarttime=$(TZ=UTC date +%s.%N)

        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Compiling LibreSSL"
        fi

        cd $DIR_TMP
        echo "Compiling LibreSSL..."
        cd libressl-${LIBRESSL_VERSION}
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [ ! -f configure ]; then
            # https://github.com/libressl-portable/portable/issues/99
            # sed -i 's|patch -p0 < patches\/tls.h.patch|patch -R -p0 < patches\/tls.h.patch|g' update.sh
            bash autogen.sh
        fi
        if [[ "$CLANG" = [yY] ]]; then
            if [[ ! -f /usr/bin/clang ]]; then
                yum -q -y install clang clang-devel
            fi
            # ccache compiler has some initial overhead for compiles but speeds up subsequent
            # recompiles. however on initial install ccache has no benefits, so for initial
            # centmin mod install disabling ccache will in theory speed up first time installs
            if [[ "$INITIALINSTALL" != [yY] ]]; then              
                export CC="ccache /usr/bin/clang -ferror-limit=0"
                export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
                export CCACHE_CPP2=yes
                CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-non-literal-null-conversion'
                CFLAGS="$CLANG_CFLAGOPT"
            else
                export CC="/usr/bin/clang -ferror-limit=0"
                export CXX="/usr/bin/clang++ -ferror-limit=0"
                # export CCACHE_CPP2=yes
                CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-non-literal-null-conversion'
                CFLAGS="$CLANG_CFLAGOPT"
            fi
        else
            export CC="ccache gcc"
            export CXX="ccache g++"
            export CCACHE_CPP2=yes
            CLANG_CFLAGOPT=""
            CFLAGS=""
        fi
        crypto_gcc
        ./configure --prefix=/opt/libressl
        # make${MAKETHREADS} check
        make${MAKETHREADS}
        make install
        if [[ "$?" = '0' ]]; then
            ln -s /opt/libressl/lib /opt/libressl/lib64
        else
            rm -rf /opt/libressl/lib64
        fi
        cp -a /etc/pki/tls/openssl.cnf /etc/pki/tls/openssl.cnf-bakb4libressl
        ln -s /etc/pki/tls/openssl.cnf /etc/ssl/openssl.cnf

        if [[ "$CLANG" = [yY] ]]; then
            unset CC
            unset CXX
            unset CFLAGS
            #unset CCACHE_CPP2
            export CC="ccache gcc"
            export CXX="ccache g++"
            CLANG_CFLAGOPT=""
            CFLAGS=""            
        fi
        crypto_gccunset
        libresslinstallendtime=$(TZ=UTC date +%s.%N)
        LIBRESSLINSTALLTIME=$(echo "scale=2;$libresslinstallendtime - $libresslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log
        echo "LibreSSL Install Time: $LIBRESSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_libresslinstalltime_${DT}.log         
    elif [[ "$LIBRESSL_SWITCH" = [nN] ]] && [[ "$BORINGSSL_SWITCH" = [nN] ]]; then
        opensslinstallstarttime=$(TZ=UTC date +%s.%N)

        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Compiling OpenSSL"
        fi

        # Install OpenSSL
        cd $DIR_TMP
        echo "Compiling OpenSSL..."
    
        if [ ! -f /usr/bin/makedepend ]; then
            yum -q -y install imake
        fi
    
        # from https://wiki.mozilla.org/Security/Server_Side_TLS#Nginx
        # export OPENSSL_BPATH=$OPENSSL_CUSTOMPATH
        export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
    
        tar xzf ${OPENSSL_LINKFILE}
        #-- Build static openssl
        cd ${DIR_TMP}/openssl-${OPENSSL_VERSION}
        rm -rf "$STATICLIBSSL"
        mkdir -p "$STATICLIBSSL"
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        opensslpatches
        crypto_gcc
        if [[ "$(uname -m)" = 'x86_64' ]]; then
            ECNISTP_CHECK=$(gcc -dM -E - </dev/null | grep __SIZEOF_INT128__)
            if [ "$ECNISTP_CHECK" ]; then
                ECNISTP_OPT=' enable-ec_nistp_64_gcc_128'
            else
                ECNISTP_OPT=""
            fi
            if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]]; then
                # openssl 1.1.0 unsupported flag enable-tlsext
                ./config -Wl,--enable-new-dtags,-rpath,'$(LIBRPATH)' --prefix=$STATICLIBSSL shared${ECNISTP_OPT}
            else
                ./config --prefix=$STATICLIBSSL shared enable-tlsext${ECNISTP_OPT}
            fi
        else
            if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]]; then
                # openssl 1.1.0 unsupported flag enable-tlsext
                ./config -Wl,--enable-new-dtags,-rpath,'$(LIBRPATH)' --prefix=$STATICLIBSSL shared
            else
                ./config --prefix=$STATICLIBSSL shared enable-tlsext
            fi
        fi
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]]; then
            make${MAKETHREADS}
        else
            make depend
            make
        fi
        make install
        if [[ "$?" = '0' ]]; then
            ln -s "${STATICLIBSSL}/lib" "${STATICLIBSSL}/lib64"
        else
            rm -rf "${STATICLIBSSL}/lib64"
        fi
        crypto_gccunset
        opensslinstallendtime=$(TZ=UTC date +%s.%N)
        OPENSSLINSTALLTIME=$(echo "scale=2;$opensslinstallendtime - $opensslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        echo "OpenSSL Install Time: $OPENSSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
    elif [[ "$BORINGSSL_SWITCH" = [yY] ]]; then
        boringsslinstallstarttime=$(TZ=UTC date +%s.%N)

        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Compiling BoringSSL"
        fi

        echo "Compiling BoringSSL..."
        cd $DIR_TMP
        git clone https://boringssl.googlesource.com/boringssl
        cd boringssl
        mkdir -p build
        cd build
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            rm -rf CMakeCache.txt
        fi
        cmake -DCMAKE_BUILD_TYPE=Release ..
        unset CC
        unset CXX
        unset CFLAGS
        export CMAKE_C_COMPILER="gcc"
        export CMAKE_CXX_COMPILER="g++"
        export CC="gcc"
        export CXX="g++"
        make${MAKETHREADS}
        mkdir -p /svr-setup/boringssl/.openssl/lib
        cd /svr-setup/boringssl/.openssl
        ln -s ../include
        cp /svr-setup/boringssl/build/crypto/libcrypto.a /svr-setup/boringssl/build/ssl/libssl.a /svr-setup/boringssl/.openssl/lib
        ls -lah /svr-setup/boringssl/.openssl/lib
        ls -lah /svr-setup/boringssl/.openssl

        boringsslinstallendtime=$(TZ=UTC date +%s.%N)
        BORINGSSLINSTALLTIME=$(echo "scale=2;$boringsslinstallendtime - $boringsslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log
        echo "BoringSSL Install Time: $BORINGSSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log 
    fi
}