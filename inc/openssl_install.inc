
opensslpatches() {
  {

if [[ "$OPENSSL_TLSONETHREE_BACKPORTDRAFTS" = [yY] && "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Backport TLS 1.3 draft 23, 26, 27, 28 support patch"
    echo "https://github.com/hakasenyang/openssl-patch/issues/8"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf openssl-1.1.1-tls13_draft.patch
    if [ -f "$CUR_DIR/patches/openssl/openssl-1.1.1-tls13_draft.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/openssl-1.1.1-tls13_draft.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/openssl-1.1.1-tls13_draft.patch"
        openssl_draftbackport_err=$?
        if [[ "$openssl_draftbackport_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/openssl-1.1.1-tls13_draft.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$OPENSSL_TLSONETHREE_BACKPORTDRAFTS" = [yY] && "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1a' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1a"
    echo "######################################################################"
    echo "Backport TLS 1.3 draft 23, 26, 27, 28 support patch"
    echo "https://github.com/hakasenyang/openssl-patch/issues/8"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf openssl-1.1.1a-tls13_draft.patch
    if [ -f "$CUR_DIR/patches/openssl/openssl-1.1.1a-tls13_draft.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/openssl-1.1.1a-tls13_draft.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/openssl-1.1.1a-tls13_draft.patch"
        openssl_draftbackport_err=$?
        if [[ "$openssl_draftbackport_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/openssl-1.1.1a-tls13_draft.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Fix OpenSSL 1.1.1 s_server unknown TLSv1.3 identities patch"
    echo "https://github.com/openssl/openssl/issues/7433"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-fix-tls13-s-server-unknown-psk-identity.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-tls13-s-server-unknown-psk-identity.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-tls13-s-server-unknown-psk-identity.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-tls13-s-server-unknown-psk-identity.patch"
        openssl_psk_err=$?
        if [[ "$openssl_psk_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-tls13-s-server-unknown-psk-identity.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Fix OpenSSL 1.1.1 safer memory cleanup patch"
    echo "https://github.com/openssl/openssl/pull/7363"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-safer-mem-cleanup.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-safer-mem-cleanup.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-safer-mem-cleanup.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-safer-mem-cleanup.patch"
        openssl_safermem_err=$?
        if [[ "$openssl_safermem_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-safer-mem-cleanup.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Fix OpenSSL 1.1.1 ocsp memory leak patch"
    echo "https://github.com/openssl/openssl/pull/7335"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-fix-ocsp-memleak.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-ocsp-memleak.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-ocsp-memleak.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-ocsp-memleak.patch"
        openssl_ocspmemleak_patch_err=$?
        if [[ "$openssl_ocspmemleak_patch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-fix-ocsp-memleak.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Fix OpenSSL 1.1.1 broken SNI handshake patch"
    echo "https://github.com/openssl/openssl/issues/7244"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-sni-fix-delay-sig-algs.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-sni-fix-delay-sig-algs.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-sni-fix-delay-sig-algs.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-sni-fix-delay-sig-algs.patch"
        openssl_sni_patch_err=$?
        if [[ "$openssl_sni_patch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-sni-fix-delay-sig-algs.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Reset TLS 1.3 ciphers in SSL_CTX_set_ssl_version() patch"
    echo "https://github.com/openssl/openssl/issues/7226"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-reset-tls1.3-ciphers-SSL_CTX_set_ssl_version.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-reset-tls1.3-ciphers-SSL_CTX_set_ssl_version.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-reset-tls1.3-ciphers-SSL_CTX_set_ssl_version.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-reset-tls1.3-ciphers-SSL_CTX_set_ssl_version.patch"
        openssl_resettls_patch_err=$?
        if [[ "$openssl_resettls_patch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-reset-tls1.3-ciphers-SSL_CTX_set_ssl_version.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Fix the max psk len for TLSv1.3 patch"
    echo "https://github.com/openssl/openssl/issues/7261"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-tls13-fix-max-psk-len.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-tls13-fix-max-psk-len.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-tls13-fix-max-psk-len.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-tls13-fix-max-psk-len.patch"
        openssl_maxpsk_err=$?
        if [[ "$openssl_maxpsk_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-tls13-fix-max-psk-len.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

# Cache timing vulnerability in RSA Key Generation (CVE-2018-0737) patch
# https://community.centminmod.com/threads/14584/
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.0.2o' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.0.2o"
    echo "######################################################################"
    echo "Cache timing vulnerability in RSA Key Generation (CVE-2018-0737) patch"
    echo "https://community.centminmod.com/threads/14584/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.0.2o-cache-timing-rsa-key-gen.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.0.2o-cache-timing-rsa-key-gen.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.0.2o-cache-timing-rsa-key-gen.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.0.2o-cache-timing-rsa-key-gen.patch"
        cachersapatch_err=$?
        if [[ "$cachersapatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.0.2o-cache-timing-rsa-key-gen.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

# Cache timing vulnerability in RSA Key Generation (CVE-2018-0737) patch
# https://community.centminmod.com/threads/14584/
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.0h' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.0h"
    echo "######################################################################"
    echo "Cache timing vulnerability in RSA Key Generation (CVE-2018-0737) patch"
    echo "https://community.centminmod.com/threads/14584/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1h-cache-timing-rsa-key-gen.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1h-cache-timing-rsa-key-gen.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1h-cache-timing-rsa-key-gen.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1h-cache-timing-rsa-key-gen.patch"
        cachersapatch_err=$?
        if [[ "$cachersapatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1h-cache-timing-rsa-key-gen.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi 

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
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDSA_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0g' ]] || [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDSA_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0h' ]]; then
    echo
    echo  "######################################################################"
    echo "Patching OpenSSL 1.1.0h"
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

# ECDSA performance patch https://community.centminmod.com/posts/57725/
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDSA_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0i' ]] || [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDSA_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0j' ]]; then
    echo
    echo  "######################################################################"
    echo "Patching OpenSSL 1.1.0i"
    echo  "######################################################################"
    echo "30-40% performance improvement patch for ECDSA"
    echo "https://community.centminmod.com/posts/57725/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1i-improve-ECDSA-sign-30-40.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1i-improve-ECDSA-sign-30-40.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1i-improve-ECDSA-sign-30-40.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1i-improve-ECDSA-sign-30-40.patch"
        ecdsapatch_err=$?
        if [[ "$ecdsapatch_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1i-improve-ECDSA-sign-30-40.patch" $boldyellow
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
if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDHX_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0g' ]] || [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDHX_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0h' ]] || [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "$OPENSSLECDHX_PATCH" = [yY] && "${OPENSSL_VERSION}" = '1.1.0i' ]]; then
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
    wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/openssl/patches/releasebuffer.patch
    patch < releasebuffer.patch
    popd
    echo  "######################################################################"
    echo  "OpenSSL 1.0.1g patched"
    echo  "######################################################################"
fi

if [[ "${OPENSSL_VERSION}" = '1.1.0xx' || "${OPENSSL_VERSION}" = '1.0.2i' || "${OPENSSL_VERSION}" = '1.0.2m' ]] || [[ "$TLSONETHREE" = [yY] ]] || [[ "$(grep -w 'tls1_3' Configure)" ]]; then
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

# if [[ "$PRIORITIZE_CHACHA_OPENSSL" = [yY] && "$DETECTOPENSSL_ONEONE" = '1.1.1' ]] ; then
if [[ "$PRIORITIZE_CHACHA_OPENSSL" = [yY] && "${OPENSSL_VERSION}" = '1.1.1' ]] || [[ "$PRIORITIZE_CHACHA_OPENSSL" = [yY] && "${OPENSSL_VERSION}" = '1.1.1a' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "bassie OpenSSL 1.1.1 Prioritize Chacha20 patch"
    echo "https://community.centminmod.com/threads/15708/"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL1.1.1-prioritize-chacha-feature.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL1.1.1-prioritize-chacha-feature.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL1.1.1-prioritize-chacha-feature.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL1.1.1-prioritize-chacha-feature.patch"
        openssl_prioritize_chacha_err=$?
        if [[ "$openssl_prioritize_chacha_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL1.1.1-prioritize-chacha-feature.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi # PRIORITIZE_CHACHA_OPENSSL

if [[ "$(uname -m)" = 'x86_64' && "$LIBRESSL_SWITCH" = [nN] && "${OPENSSL_VERSION}" = '1.1.1' ]]; then
    echo
    echo "######################################################################"
    echo "Patching OpenSSL 1.1.1"
    echo "######################################################################"
    echo "Timing vulnerability in ECDSA signature generation (CVE-2018-0735) patch"
    echo "https://www.openssl.org/news/secadv/20181029.txt"
    echo  "######################################################################"
    pushd "${DIR_TMP}/openssl-${OPENSSL_VERSION}"
    rm -rf OpenSSL-1.1.1-ECDSA-signature-gen-CVE-2018-0735.patch
    if [ -f "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-ECDSA-signature-gen-CVE-2018-0735.patch" ]; then
        cecho "patch -p1 < $CUR_DIR/patches/openssl/OpenSSL-1.1.1-ECDSA-signature-gen-CVE-2018-0735.patch" $boldyellow
        patch -p1 < "$CUR_DIR/patches/openssl/OpenSSL-1.1.1-ECDSA-signature-gen-CVE-2018-0735.patch"
        openssl_ecdsatiming_err=$?
        if [[ "$openssl_ecdsatiming_err" -ne '0' ]]; then
            cecho "patch failed, revert patch $CUR_DIR/patches/openssl/OpenSSL-1.1.1-ECDSA-signature-gen-CVE-2018-0735.patch" $boldyellow
            pushd "$DIR_TMP"
            rm -rf "openssl-${OPENSSL_VERSION}"
            tar xzf "openssl-${OPENSSL_VERSION}.tar.gz"
            popd
        fi
    fi
    popd
    echo
fi

if [[ "$CLOUDFLARE_PATCHSSL" = [yY] && "$DETECTOPENSSL_ONEZERO" = '1.0' ]]; then
    # if [[ "${OPENSSL_VERSION}" = '1.0.1h' ]]; then
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Cloudflare Chacha20 Patch"
        fi
        echo  "######################################################################"
        echo "Patching OpenSSL 1.0.2o"
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
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${OPENSSLCFPATCHLINK}" | grep 'HTTP\/' | egrep '200' >/dev/null 2>&1
        OPENSSLCFPATCH_CURLCHECK=$?
        if [[ "$OPENSSLCFPATCH_CURLCHECK" != '0' ]]; then
            if [[ "$AVXTWO_CHECK" = '0' ]]; then
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/upstream/optimize_chacha_poly/patches/${OPESSLCFPATCH_NAME}"
            else
                OPENSSLCFPATCHLINK="https://gitlab.com/centminmod-github-mirror/sslconfig/raw/master/patches/${OPESSLCFPATCH_NAME}"
            fi
        fi
        wget -${ipv_forceopt}cnv --no-check-certificate "$OPENSSLCFPATCHLINK"

        if [ ! -f crypto/chacha20_poly1305/chacha20.c ]; then
            patch -p1 < "${OPESSLCFPATCH_NAME}"
        fi
        # rm -rf openssl__chacha20_poly1305_cf.patch
        # wget -${ipv_forceopt}cnv --no-check-certificate https://raw.githubusercontent.com/cloudflare/sslconfig/master/patches/openssl__chacha20_poly1305_cf.patch
        # patch -p1 < openssl__chacha20_poly1305_cf.patch
        if [ -f crypto/chacha20_poly1305/chacha20.c ]; then
            # check /svr-setup/openssl-1.0.2g/crypto/chacha20_poly1305/chacha20.c exists
            OPEENSSL_CFPATCHED='y'
            echo  "######################################################################"
            echo  "OpenSSL 1.0.2o patched"
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

if [[ "$CLOUDFLARE_PATCHSSL" = [yY] && "$DETECTOPENSSL_ONEZERO" = '1.1' ]] && [[ "$DETECTOPENSSL_ONEONE" != '1.1.1' ]] ; then
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
      if [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
        source /opt/gcc8/enable
        CRYPTODEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        CRYPTODEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type'
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/gcc8/bin/gcc"
        export CXX="ccache /opt/gcc8/bin/g++"
      elif [[ "$DEVTOOLSETSEVEN" = [yY] ]] && [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
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

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  OPENSSL_VERSION_CUSTOMCONFIG=$(awk -F "'" '/^OPENSSL_VERSION=/ {print $2}' "${CONFIGSCANBASE}/custom_config.inc")
  if [[ "${OPENSSL_VERSION}" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0j' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0i' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0h' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0g' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0f' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0e' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0d' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0c' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0b' || "$OPENSSL_VERSION_CUSTOMCONFIG" = '1.1.0a' ]]; then
    # force old OpenSSL 1.1.0 branch versions to newer
    # 1.1.1 branch if detected as some folks hardcode override
    # OPENSSL_VERSION variable in /etc/centminmod/custom_config.inc
    # and forget to update them and over time they are out of sync
    # with OPENSSL_VERSION updated and set in centmin.sh
    #
    # also OpenSSL 1.1.0j seems to be failing Nginx compiles so this
    # is a workaround to jump to 1.1.1a working version for now
    OPENSSL_VERSION="$OPENSSL_VERSION_OLDOVERRIDE"
    OPENSSL_LINKFILE="openssl-${OPENSSL_VERSION}.tar.gz"
    OPENSSL_LINK="https://www.openssl.org/source/${OPENSSL_LINKFILE}"
  fi
fi

    # clean up recent no-tls1_3 bug
    # https://community.centminmod.com/posts/63295/
    if [ -d /usr/local/ssl ]; then
        echo "clean up no-tls1_3 bug /usr/local/ssl"
        echo "https://community.centminmod.com/posts/63295/"
        rm -rf /usr/local/ssl
    fi
    if [ -d /usr/local/lib/engines-1.1 ]; then
        echo "clean up no-tls1_3 bug /usr/local/lib/engines-1.1"
        echo "https://community.centminmod.com/posts/63295/"
        rm -rf /usr/local/lib/engines-1.1
    fi
    if [ -d /usr/local/include/openssl ]; then
        echo "clean up no-tls1_3 bug /usr/local/include/openssl"
        echo "https://community.centminmod.com/posts/63295/"
        rm -rf /usr/local/include/openssl
    fi
    if [ -f /usr/local/bin/openssl ]; then
        echo "clean up no-tls1_3 bug /usr/local/bin/openssl"
        echo "https://community.centminmod.com/posts/63295/"
        rm -rf /usr/local/bin/openssl
    fi
    if [ -f /usr/local/bin/c_rehash ]; then
        echo "clean up no-tls1_3 bug /usr/local/bin/c_rehash"
        echo "https://community.centminmod.com/posts/63295/"
        rm -rf /usr/local/bin/c_rehash
    fi
    if [ -f /usr/local/lib/libssl.a ]; then
        echo "clean up old /usr/local/lib/libssl.a"
        rm -rf /usr/local/lib/libssl.a
        ldconfig
    fi
    if [ -f /usr/local/lib/libcrypto.a ]; then
        echo "clean up old /usr/local/lib/libcrypto.a"
        rm -rf /usr/local/lib/libcrypto.a
        ldconfig
    fi

    if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
        # lua nginx 0.10.7 and lower not compatible with openssl 1.1.x branch
        # so fall back to 1.0.2 branch if lua nginx module is enabled
        OPENSSL_VERSION="$OPENSSL_VERSIONFALLBACK"
    fi
    DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
    DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
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

        if [[ "$CLOUDFLARE_ZLIB_OPENSSL" = [yY] && "$CLOUDFLARE_ZLIB" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' && -d /usr/local/zlib-cf ]]; then
            OPENSSL_CFZLIB_OPT=' -I/usr/local/zlib-cf/include -L/usr/local/zlib-cf/lib'
            OPENSSL_CFZLIB_RPATH='/usr/local/zlib-cf:'
            echo "OPENSSL_CFZLIB_OPT = $OPENSSL_CFZLIB_OPT"
            echo "OPENSSL_CFZLIB_RPATH = $OPENSSL_CFZLIB_RPATH"
        else
            echo "OPENSSL_CFZLIB_OPT = $OPENSSL_CFZLIB_OPT"
            echo "OPENSSL_CFZLIB_RPATH = $OPENSSL_CFZLIB_RPATH"
        fi
    
        #-- Build static openssl
        if [[ "$TLSONETHREE" = [yY] && "$ORESTY_LUANGINX" != [yY] ]] && [[ "$DETECTOPENSSL_ONEONE" != '1.1.1' ]] ; then
            OPENSSL_CUSTOMPATH='/opt/openssl-tls1.3'
            export STATICLIBSSL="${OPENSSL_CUSTOMPATH}"
            cd "$DIR_TMP"
            rm -rf ${DIR_TMP}/openssl-tls1.3
            git clone https://github.com/openssl/openssl/ openssl-tls1.3
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
                if [[ "$OPENSSL_TLSONETHREE" = [yY] && "$(grep -w 'tls1_3' Configure)" ]]; then
                    TLSONETHREEOPT=' enable-tls1_3'
                    TLSONETHREE_DETECT='y'
                else
                    # https://wiki.openssl.org/index.php/TLS1.3
                    if [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
                        TLSONETHREEOPT=" no-tls1_3"
                    else
                        TLSONETHREEOPT=""
                    fi
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
                echo "./config -Wl,--enable-new-dtags,-rpath=${OPENSSL_CFZLIB_RPATH}${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared${ECNISTP_OPT}${TLSONETHREEOPT}${OPENSSL_THREADSOPT}${OPENSSL_CFZLIB_OPT}"
                ./config -Wl,--enable-new-dtags,-rpath=${OPENSSL_CFZLIB_RPATH}${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared${ECNISTP_OPT}${TLSONETHREEOPT}${OPENSSL_THREADSOPT}${OPENSSL_CFZLIB_OPT}
            else
                echo "./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared enable-tlsext${ECNISTP_OPT}${OPENSSL_CFZLIB_OPT} -Wl, -rpath=${OPENSSL_CFZLIB_RPATH}${STATICLIBSSL}/lib"
                ./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared enable-tlsext${ECNISTP_OPT}${OPENSSL_CFZLIB_OPT} -Wl, -rpath=${OPENSSL_CFZLIB_RPATH}${STATICLIBSSL}/lib
            fi
        else
            if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
                # openssl 1.1.0 unsupported flag enable-tlsext
                if [[ "$OPENSSL_TLSONETHREE" = [yY] && "$(grep -w 'tls1_3' Configure)" ]]; then
                    TLSONETHREEOPT=' enable-tls1_3'
                    TLSONETHREE_DETECT='y'
                else
                    # https://wiki.openssl.org/index.php/TLS1.3
                    if [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
                        TLSONETHREEOPT=" no-tls1_3"
                    else
                        TLSONETHREEOPT=""
                    fi
                    TLSONETHREE_DETECT='n'
                fi
                echo "./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared${TLSONETHREEOPT}"
                ./config -Wl,--enable-new-dtags,-rpath=${STATICLIBSSL}/lib --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared${TLSONETHREEOPT}
            else
                echo "./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared enable-tlsext"
                ./config --prefix=$STATICLIBSSL --openssldir=$STATICLIBSSL no-shared enable-tlsext
            fi
        fi
        if [[ "$DETECTOPENSSL_ONEZERO" = '1.1' ]] || [[ "$DETECTOPENSSL_ONEONE" = '1.1.1' ]]; then
            make${MAKETHREADS}
        else
            make depend
            make
        fi
        # make install
        make install_sw
        if [[ "$?" = '0' ]]; then
            ln -s "${STATICLIBSSL}/lib" "${STATICLIBSSL}/lib64"
        else
            rm -rf "${STATICLIBSSL}/lib64"
        fi
        mkdir -p /opt/openssl/certs/
        \cp -Rpf /etc/ssl/certs/* /opt/openssl/certs/
        crypto_gccunset
        opensslinstallendtime=$(TZ=UTC date +%s.%N)
        OPENSSLINSTALLTIME=$(echo "scale=2;$opensslinstallendtime - $opensslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        echo "OpenSSL Install Time: $OPENSSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_opensslinstalltime_${DT}.log
    elif [[ "$BORINGSSL_SWITCH" = [yY] ]]; then
        boringsslinstallstarttime=$(TZ=UTC date +%s.%N)
        crypto_gcc
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "Compiling BoringSSL"
        fi

        if [ ! -f /usr/local/go/bin/go ]; then
            # if golang is not detected BoringSSL switch will be disabled
            # and default to using OpenSSL 1.0.2+
            # BORINGSSL_SWITCH='n'
            if [ -f /usr/local/src/centminmod/addons/golang.sh ]; then
                /usr/local/src/centminmod/addons/golang.sh install
                . /root/.bashrc
            fi
        fi

        echo "Compiling BoringSSL..."
        # cd $DIR_TMP
        pushd "${BORINGSSL_DIR}"
        rm -rf ${BORINGSSL_DIR}/boringssl
        time git clone https://boringssl.googlesource.com/boringssl
        # time git clone https://github.com/google/boringssl.git
        cd boringssl
        mkdir -p build
        cd build
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            rm -rf CMakeCache.txt
        fi
        unset CC
        unset CXX
        unset CFLAGS
        export CMAKE_C_COMPILER="gcc"
        export CMAKE_CXX_COMPILER="g++"
        export CC="gcc"
        export CXX="g++"
        export GO_EXECUTABLE="/usr/local/go/bin/go"
        mkdir -p /home/buildtmp
        chmod 1777 /home/buildtmp
        export TMPDIR=/home/buildtmp
        # sed -i 's|set(GO_EXECUTABLE "go")|set(GO_EXECUTABLE "\/usr\/local\/go\/bin\/go")|' ${BORINGSSL_DIR}/boringssl/CMakeLists.txt
        ln -s /usr/local/go/bin/go /usr/local/bin/go
        if [[ "$BORINGSSL_SHARED" = [yY] ]]; then
            cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DBUILD_SHARED_LIBS=1 ..
        else
            cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ..
        fi
        # cmake -DCMAKE_BUILD_TYPE=Release ..
        # re-enable TLS 1.3 draft 23 /28 support
        # sed -i 's|tls13_rfc = 0|tls13_all = 0|' ${BORINGSSL_DIR}/boringssl/include/openssl/ssl.h
        # sed -i 's|  tls13_all,|  tls13_rfc,|' ${BORINGSSL_DIR}/boringssl/include/openssl/ssl.h
        sed -i 's|tls13_variant_t tls13_variant = tls13_rfc;|tls13_variant_t tls13_variant = tls13_all;|g' ${BORINGSSL_DIR}/boringssl/ssl/internal.h
        make${MAKETHREADS}
        mkdir -p ${BORINGSSL_DIR}/boringssl/.openssl/lib
        cd ${BORINGSSL_DIR}/boringssl/.openssl
        ln -s ../include
        cp ${BORINGSSL_DIR}/boringssl/build/crypto/libcrypto.a ${BORINGSSL_DIR}/boringssl/build/ssl/libssl.a ${BORINGSSL_DIR}/boringssl/.openssl/lib
        cp ${BORINGSSL_DIR}/boringssl/build/crypto/libcrypto.so ${BORINGSSL_DIR}/boringssl/build/ssl/libssl.so ${BORINGSSL_DIR}/boringssl/.openssl/lib
        ls -lah ${BORINGSSL_DIR}/boringssl/.openssl/lib
        ls -lah ${BORINGSSL_DIR}/boringssl/.openssl
        # symlink boringssl bssl binary for ease of use
        if [ -f "${BORINGSSL_DIR}/boringssl/build/tool/bssl" ]; then
            echo "ln -sf ${BORINGSSL_DIR}/boringssl/build/tool/bssl /usr/local/bin/bssl"
            ln -sf "${BORINGSSL_DIR}/boringssl/build/tool/bssl" /usr/local/bin/bssl
            ls -lah /usr/local/bin/bssl
        fi
        unset TMPDIR
        boringsslinstallendtime=$(TZ=UTC date +%s.%N)
        BORINGSSLINSTALLTIME=$(echo "scale=2;$boringsslinstallendtime - $boringsslinstallstarttime"|bc )

        echo "" >> ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log
        echo "BoringSSL Install Time: $BORINGSSLINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log
        ls -lah ${CENTMINLOGDIR}/centminmod_boringsslinstalltime_${DT}.log
        popd 
    fi
}