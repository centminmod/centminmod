
opensslpatches() {
  {

# Equal Cipher Preference Group patch for 64bit systems only
# OpenSSL1.1g-equal-preference-cipher-groups.patch
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLEQUALCIPHER_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0g' ]]; then
    # disable CLOUDFLARE_PATCHSSL for smart chacha20 due to conflicts and not being needed
    # https://community.centminmod.com/posts/57919/
    CLOUDFLARE_PATCHSSL='n'
    echo
    echo  "######################################################################"
    echo "Patching OpenSSL 1.1.0g"
    echo  "######################################################################"
    echo "OpenSSL 1.1.0g Equal Cipher Preference Group patch"
    echo "https://community.centminmod.com/posts/57916/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1g-equal-preference-cipher-groups.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1g-equal-preference-cipher-groups.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1g-equal-preference-cipher-groups.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1g-equal-preference-cipher-groups.patch"
        equalcipherprefpatch_err=$?
        if [[ "$equalcipherprefpatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1g-equal-preference-cipher-groups.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        else
            echo
            cecho "patch success, need to change your ssl_cipher config for HTTPS vhost to the following:" $boldyellow
            echo
            echo "ssl_ciphers '[ECDHE-ECDSA-AES128-GCM-SHA256|ECDHE-ECDSA-CHACHA20-POLY1305|ECDHE-RSA-AES128-GCM-SHA256|ECDHE-RSA-CHACHA20-POLY1305]:ECDHE+AES128:RSA+AES128:ECDHE+AES256:RSA+AES256:ECDHE+3DES:RSA+3DES';"
            echo
        fi
    fi
    popd
    echo
fi

# ECDSA performance patch https://community.centminmod.com/posts/57725/
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDSA_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0g' ]]; then
    echo
    echo  "######################################################################"
    echo "Patching OpenSSL 1.1.0g"
    echo  "######################################################################"
    echo "30-40% performance improvement patch for ECDSA"
    echo "https://community.centminmod.com/posts/57725/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1g-improve-ECDSA-sign-30-40.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1g-improve-ECDSA-sign-30-40.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1g-improve-ECDSA-sign-30-40.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1g-improve-ECDSA-sign-30-40.patch"
        ecdsapatch_err=$?
        if [[ "$ecdsapatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1g-improve-ECDSA-sign-30-40.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

# ECDHX25519 performance patch for 64bit systems only
# OpenSSL1.1g-double-performance-ecdhx-25519.patch
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDHX_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0g' ]]; then
    echo
    echo  "######################################################################"
    echo "Patching OpenSSL 1.1.0g"
    echo  "######################################################################"
    echo "ECDHX 25519 performance patch"
    echo "https://community.centminmod.com/posts/57726/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1g-double-performance-ecdhx-25519.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1g-double-performance-ecdhx-25519.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1g-double-performance-ecdhx-25519.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1g-double-performance-ecdhx-25519.patch"
        ecdhxapatch_err=$?
        if [[ "$ecdhxapatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1g-double-performance-ecdhx-25519.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

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
    wget -4 -cnv https://centminmod.com/centminmodparts/openssl/patches/releasebuffer.patch
    patch < releasebuffer.patch
    popd
    echo  "######################################################################"
    echo  "OpenSSL 1.0.1g patched"
    echo  "######################################################################"
fi

if [[ "${OPENSSL_VERSION}" = '1.0.2i' || "${OPENSSL_VERSION}" = '1.0.2m' ]] || [[ "$TLSONETHREE" = [yY] ]] || [[ "$(grep -w 'tls1_3' Configure)" ]]; then
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

DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)

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
        curl -4Is --connect-timeout 5 --max-time 5 "${OPENSSLCFPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
        OPENSSLCFPATCH_CURLCHECK=$?
        if [[ "$OPENSSLCFPATCH_CURLCHECK" != '0' ]]; then
            if [[ "$AVXTWO_CHECK" = '0' ]]; then
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/upstream/optimize_chacha_poly/patches/${OPESSLCFPATCH_NAME}"
            else
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${OPESSLCFPATCH_NAME}"
            fi
        fi
        wget -4 -cnv --no-check-certificate "$OPENSSLCFPATCHLINK"

        if [ ! -f crypto/chacha20_poly1305/chacha20.c ]; then
            patch -p1 < "${OPESSLCFPATCH_NAME}"
        fi
        # rm -rf openssl__chacha20_poly1305_cf.patch
        # wget -4 -cnv --no-check-certificate https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/openssl__chacha20_poly1305_cf.patch
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
    if [[ "$TLSONETHREE" != [yY] ]]; then
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Cloudflare OpenSSL 1.1 Smarter Chacha20 Patch"
        fi
        echo "######################################################################"
        echo "Patching OpenSSL 1.1.0 branch"
        echo "######################################################################"
        echo "Cloudflare Smart ChaCha20 patch"
        echo "https://community.centminmod.com/posts/35727/"
        echo "only support ChaCha20 if client's preferred cipher"
        echo "######################################################################"
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
    else
        echo "######################################################################"
        echo "TLSONETHREE='y' : Skip Cloudflare Smart ChaCha20 patch"
        echo "######################################################################"
    fi
fi # CLOUDFLARE_PATCHSSL
  } 2>&1 | tee "${CENTMINLOGDIR}/patch_opensslpatches_${DT}.log"
}

crypto_gcc() {
    if [[ "$CRYPTO_DEVTOOLSETGCC" = [yY] ]]; then
      if [[ "$DEVTOOLSETSEVEN" = [yY] ]] && [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-7/enable
        CRYPTODEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        CRYPTODEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]] && [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc || -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-7/enable
        CRYPTODEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        CRYPTODEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] ]] && [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        scl_install
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" != [yY] ]] && [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
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
      elif [[ "$DEVTOOLSETSIX" != [yY] ]] && [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
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
            CRYPTOGCC_OPT="-m${CCM} -march=${MARCH_TARGET}${CRYPTODEVTOOLSETSEVEN_FALLTHROUGH}${CRYPTODEVTOOLSETSEVEN_EXTRAFLAGS}"
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
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
    DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
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
            if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/clang ]] || [[ "$CENTOS_SEVEN" = '7' && ! -f /bin/clang ]]; then
                time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
            fi
            # ccache compiler has some initial overhead for compiles but speeds up subsequent
            # recompiles. however on initial install ccache has no benefits, so for initial
            # centmin mod install disabling ccache will in theory speed up first time installs
            if [[ "$INITIALINSTALL" != [yY] ]]; then              
                export CC="ccache /usr/bin/clang -ferror-limit=0"
                export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
                export CCACHE_CPP2=yes
                CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-non-literal-null-conversion'
                CFLAGS="$CLANG_CFLAGOPT"
            else
                export CC="/usr/bin/clang -ferror-limit=0"
                export CXX="/usr/bin/clang++ -ferror-limit=0"
                # export CCACHE_CPP2=yes
                CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-non-literal-null-conversion'
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
        \cp -Rpf /etc/ssl/certs/* /opt/libressl/etc/ssl/certs/

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
            time $YUMDNFBIN -q -y install imake${DISABLEREPO_DNF}
        fi
    
        #-- Build static openssl
        if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]] && [[ "$DETECTOPENSSL_ONEONE" != '1.1.1' ]] ; then
            OPENSSL_CUSTOMPATH='/opt/openssl-tls1.3'
            export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
            cd "$DIR_TMP"
            rm -rf ${DIR_TMP}/openssl-tls1.3
            git clone -b tls1.3-draft-18 https://github.com/openssl/openssl/ openssl-tls1.3
            cd ${DIR_TMP}/openssl-tls1.3
        else
            export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
            rm -rf "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
            tar xzf ${OPENSSL_LINKFILE}
            cd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
        fi
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
            if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
                # openssl 1.1.0 unsupported flag enable-tlsext
                if [[ "$(grep -w 'tls1_3' Configure)" ]]; then
                    TLSONETHREEOPT=' enable-tls1_3'
                    TLSONETHREE_DETECT='y'
                else
                    TLSONETHREEOPT=""
                    TLSONETHREE_DETECT='n'
                fi
                # openssl 1.1. use new native threading API
                # https://www.openssl.org/news/cl110.txt
                if [[ "${OPENSSL_THREADS}" = [nN] ]]; then
                    OPENSSL_THREADSOPT=' no-threads'
                elif [[ "${OPENSSL_THREADS}" = [yY] ]]; then
                    OPENSSL_THREADSOPT=""
                elif [[ ! "${OPENSSL_THREADS}" ]]; then
                    OPENSSL_THREADSOPT=""
                fi
                echo "./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared${ECNISTP_OPT}${TLSONETHREEOPT}${OPENSSL_THREADSOPT}"
                ./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared${ECNISTP_OPT}${TLSONETHREEOPT}${OPENSSL_THREADSOPT}
            else
                echo "./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared enable-tlsext${ECNISTP_OPT}"
                ./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared enable-tlsext${ECNISTP_OPT}
            fi
        else
            if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
                # openssl 1.1.0 unsupported flag enable-tlsext
                if [[ "$(grep -w 'tls1_3' Configure)" ]]; then
                    TLSONETHREEOPT=' enable-tls1_3'
                    TLSONETHREE_DETECT='y'
                else
                    TLSONETHREEOPT=""
                    TLSONETHREE_DETECT='n'
                fi
                echo "./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared${TLSONETHREEOPT}"
                ./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared${TLSONETHREEOPT}
            else
                echo "./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared enable-tlsext"
                ./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL shared enable-tlsext
            fi
        fi
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
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
        \cp -Rpf /etc/ssl/certs/* /opt/openssl/certs/
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
        time git clone https://boringssl.googlesource.com/boringssl
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