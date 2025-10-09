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

  # Clean up any existing extracted directory to ensure fresh compilation
  rm -rf imap-patches-FD29-RPM

  tar xzf imap-2007f-patched.tar.gz
  cd imap-patches-FD29-RPM

  # Fedora patches (patches-FD29-RPM tag) already include OpenSSL 1.1.x compatibility
  # via patch 1006_openssl1.1_autoverify.patch which handles X509 struct opacity
  echo "Using Fedora patches from patches-FD29-RPM tag (includes OpenSSL 1.1.x compatibility)"

  # Build for modern Linux (slx) with custom OpenSSL
  echo "Building uw-imap with custom OpenSSL at /opt/openssl..."
  make slx EXTRACFLAGS="-I/opt/openssl/include -fPIC" \
    EXTRALDFLAGS="-L/opt/openssl/lib64 -lssl -lcrypto" \
    SSLTYPE=unix.nopwd SSLDIR=/opt/openssl

  # Install to /opt/uw-imap with c-client subdirectory structure
  # PHP configure expects files in both include/ and c-client/ directories
  echo "Installing to /opt/uw-imap..."
  mkdir -p /opt/uw-imap/{include,lib,c-client}

  # Copy headers to both locations for compatibility
  cp c-client/*.h /opt/uw-imap/include/
  cp c-client/*.h /opt/uw-imap/c-client/

  # Copy library to both locations
  cp c-client/c-client.a /opt/uw-imap/lib/libc-client.a
  cp c-client/c-client.a /opt/uw-imap/c-client/libc-client.a

  # Also copy linkage.c which PHP configure may check for
  cp c-client/linkage.c /opt/uw-imap/c-client/ 2>/dev/null || true

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
  if [[ "$CENTOS_NINE" = '9' && "$PHPIMAP" = [yY] ]]; then
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