libxml_install() {
  if [[ "$CENTOS_SEVEN" -eq '7' ]] && [[ "$PHP_LIBXML" = [yY] ]] && [[ "$PHPMVER" = '8.4' || "$PHPMUVER" = '8.4' || "$PHPVER_ID" -ge '80000' ]]; then
    # https://www.php.net/manual/en/libxml.requirements.php
    # https://github.com/GNOME/libxml2/tags
    # https://gitlab.gnome.org/GNOME/libxml2/-/issues
    # latest 2.13.5
    echo
    echo "Internal newer libxml ${LIBXML_VER} for CentOS 7 PHP 8.4 requirements"
    echo
    LIBXML_VER='2.9.14'
    pushd "$DIR_TMP"
    wget -O $DIR_TMP/libxml2-v${LIBXML_VER}.tar.gz https://github.com/GNOME/libxml2/archive/refs/tags/v${LIBXML_VER}.tar.gz
    tar -xvf libxml2-v${LIBXML_VER}.tar.gz
    cd libxml2-${LIBXML_VER}
    make clean
    PHP_XML_PREFIX="/usr/local/libxml/${LIBXML_VER}"
    # patch for centos 7
    # https://gitlab.gnome.org/GNOME/libxml2/-/issues/746
    # Check if the line with PKG_CHECK_VAR exists and is not already patched in configure.ac
    if ! grep -q 'm4_ifdef(\[PKG_CHECK_VAR\]' configure.ac; then
      sed -i '/PKG_CHECK_VAR(\[ICU_DEFS\], \[icu-i18n\], \[DEFS\])/c\        m4_ifdef([PKG_CHECK_VAR],\n            [PKG_CHECK_VAR([ICU_DEFS], [icu-i18n], [DEFS])])' configure.ac
    else
      echo "PKG_CHECK_VAR already patched."
    fi
    ./autogen.sh
    ./configure --prefix="${PHP_XML_PREFIX}"
    make -j$(nproc)
    make install
    check_libxml_external=$?
    if [[ "$check_libxml_external" -eq '0' ]]; then
      echo "export LIBXML_CFLAGS='-I/usr/local/libxml/${LIBXML_VER}/include'"
      export LIBXML_CFLAGS="-I/usr/local/libxml/${LIBXML_VER}/include"
      echo "export LIBXML_LIBS='-L/usr/local/libxml/${LIBXML_VER}/lib -lxml2'"
      export LIBXML_LIBS="-L/usr/local/libxml/${LIBXML_VER}/lib -lxml2"
      PHP_LIBXML_PKG_CONFIG_PATH="${PHP_XML_PREFIX}/lib/pkgconfig:"
      PHP_LIBXML_LD_LIBRARY_PATH="${PHP_XML_PREFIX}/lib:"
    fi
    popd
  fi
}