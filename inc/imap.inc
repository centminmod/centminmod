#############################################################
# phpimap_source_install
# Compile uw-imap from source against custom OpenSSL
# Required for CentOS 7 when PHP_CUSTOMSSL_FORCE='y' to avoid
# symbol conflicts between system OpenSSL 1.0.2 and custom OpenSSL 1.1.1w
#############################################################
phpimap_source_install() {
  echo "----------------------------------------------------------------------"
  echo "Compiling uw-imap-2007f from source against custom OpenSSL"
  echo "Required to avoid symbol conflicts with system OpenSSL 1.0.2"
  echo "----------------------------------------------------------------------"

  # Save current directory to restore later
  local IMAP_SAVED_PWD="$(pwd)"

  cd "$DIR_TMP"

  # Download uw-imap source with Fedora patches (includes OpenSSL 1.1 compatibility)
  if [ ! -f imap-2007f-patched.tar.gz ]; then
    wget -O imap-2007f-patched.tar.gz https://github.com/uw-imap/imap/archive/refs/tags/patches-FD29-RPM.tar.gz
  fi

  tar xzf imap-2007f-patched.tar.gz
  cd imap-patches-FD29-RPM

  # Apply PR #5 TLS method fix for OpenSSL 1.1.0+
  echo "Applying TLS method compatibility patch for OpenSSL 1.1.0+..."
  cat > /tmp/tls-method-fix.patch <<'TLSPATCH'
--- a/src/osdep/unix/ssl_unix.c
+++ b/src/osdep/unix/ssl_unix.c
@@ -420,7 +420,11 @@ static char *ssl_start_work (SSLSTREAM *stream,char *host,unsigned long flags)
   SSL_CTX_set_options (stream->context,0);
   SSL_CTX_set_cipher_list (stream->context,SSLCIPHERLIST);
   SSL_CTX_set_default_verify_paths (stream->context);
+#if OPENSSL_VERSION_NUMBER >= 0x10100000
+  if (!(stream->con = (SSL *) SSL_new (stream->context = SSL_CTX_new (TLS_client_method())))) {
+#else
   if (!(stream->con = (SSL *) SSL_new (stream->context = SSL_CTX_new (TLSv1_client_method())))) {
+#endif
     return "Unable to create SSL context";
   }
   bio = BIO_new_socket (stream->tcpstream->tcpsi,BIO_NOCLOSE);
@@ -903,7 +907,11 @@ static char *ssl_server_init (char *server)
     SSL_CTX_set_options (stream->context,0);
     SSL_CTX_set_cipher_list (stream->context,SSLCIPHERLIST);
     SSL_CTX_load_verify_locations (stream->context,NIL,NIL);
+#if OPENSSL_VERSION_NUMBER >= 0x10100000
+    if (!(stream->con = SSL_new (stream->context = SSL_CTX_new (TLS_server_method())))) {
+#else
     if (!(stream->con = SSL_new (stream->context = SSL_CTX_new (TLSv1_server_method())))) {
+#endif
       return "Unable to create SSL context";
     }
     bio = BIO_new_socket (stream->tcpstream->tcpsi,BIO_NOCLOSE);
TLSPATCH

  patch -p1 < /tmp/tls-method-fix.patch
  if [ $? -eq 0 ]; then
    echo "✓ TLS method patch applied successfully"
  else
    echo "⚠ Warning: TLS method patch may have already been applied or failed"
  fi

  # Build for modern Linux (slx) with custom OpenSSL
  echo "Building uw-imap with custom OpenSSL at /opt/openssl..."
  make slx EXTRACFLAGS="-I/opt/openssl/include -fPIC" \
    EXTRALDFLAGS="-L/opt/openssl/lib64 -lssl -lcrypto" \
    SSLTYPE=unix.nopwd SSLDIR=/opt/openssl

  # Install to /opt/uw-imap
  echo "Installing to /opt/uw-imap..."
  mkdir -p /opt/uw-imap/{include,lib}
  cp c-client/*.h /opt/uw-imap/include/
  cp c-client/c-client.a /opt/uw-imap/lib/libc-client.a

  # Restore original directory before exit
  cd "${IMAP_SAVED_PWD}"

  if [ -f /opt/uw-imap/lib/libc-client.a ]; then
    echo "----------------------------------------------------------------------"
    echo "✓ uw-imap compiled successfully against custom OpenSSL"
    ls -lah /opt/uw-imap/lib/libc-client.a
    echo "----------------------------------------------------------------------"
  else
    echo "----------------------------------------------------------------------"
    echo "✗ ERROR: uw-imap compilation failed"
    echo "----------------------------------------------------------------------"
    exit 1
  fi
}

phpimap_install() {
  if [[ "$CENTOS_TEN" = '10' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --enablerepo=remi
        sar_call
      fi
    fi
  elif [[ "$CENTOS_NINE" = '9' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --enablerepo=remi
        sar_call
      fi
    fi
  elif [[ "$CENTOS_EIGHT" = '8' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install uw-imap-devel
        sar_call
      fi
    fi
  elif [[ "$CENTOS_SEVEN" = '7' && "$PHPIMAP" = [yY] ]]; then
    # CentOS 7 + Custom OpenSSL requires source compilation
    if [[ "$PHP_CUSTOMSSL_FORCE" = [yY] ]]; then
      phpimap_source_install
      export PHP_IMAP_CUSTOM_PATH="/opt/uw-imap"
    else
      # Standard YUM installation for system OpenSSL
      if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then
        if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
          time yum${CACHESKIP} -y install uw-imap-devel --disablerepo=rpmforge
          sar_call
        else
          time yum${CACHESKIP} -y install uw-imap-devel
          sar_call
        fi
      fi
    fi
  else
    if [[ $(rpm -q libc-client-devel >/dev/null 2>&1; echo $?) != '0' && "$PHPIMAP" = [yY] ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install libc-client-devel --disablerepo=rpmforge,epel
        sar_call
      else
        time yum${CACHESKIP} -y install libc-client-devel --disablerepo=epel
        sar_call
      fi
    fi
  fi
}