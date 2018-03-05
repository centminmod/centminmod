install_gperftools() {
  if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
    if [[ ! -f "${DIR_TMP}/${LIBUNWIND_LINKFILE}" || ! -d "$DIR_TMP/$LIBUNWIND_LINKDIR" ]] || [[ ! -f "${DIR_TMP}/${GPERFTOOL_LINKFILE}" || ! -d "$DIR_TMP/$GPERFTOOL_LINKDIR" ]]; then
        if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
            gperftools
        fi
    elif [[ ! -f "${DIR_TMP}/${LIBUNWIND_LINKFILE}" ]] || [[ ! -f "${DIR_TMP}/${GPERFTOOL_LINKFILE}" ]]; then
        if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; then
            gperftools
        fi
    fi
    echo "*************************************************"
    cecho "* Source Install Google Perftools" $boldgreen
    echo "*************************************************"
  
    if [[ "$(uname -m)" = 'x86_64' ]]; then
      # Install libunwind
      echo "Compiling libunwind..."
      #download libunwind tarball
      #tar xvzf ${LIBUNWIND_LINKFILE}
      cd "$DIR_TMP"
      cd "${LIBUNWIND_LINKDIR}"
      ./configure
      make$MAKETHREADS
      make install
    fi
  
    # Install google-perftools
    cd "$DIR_TMP"
    echo "Compiling google-perftools..."
    #download gperftool tarball 
    #tar xvzf ${GPERFTOOL_LINKFILE}
    cd "${GPERFTOOL_LINKDIR}"
    if [[ "$(uname -m)" = 'x86_64' ]]; then
        ./configure --with-tcmalloc-pagesize="$TCMALLOC_PAGESIZE"
    else
        ./configure --enable-frame-pointers --with-tcmalloc-pagesize="$TCMALLOC_PAGESIZE"
    fi
    make$MAKETHREADS
    make install
    if [ ! -f /etc/ld.so.conf.d/wget.conf ]; then
        echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
        /sbin/ldconfig
    fi
  fi # GPERFTOOLS_SOURCE INSTALL
}