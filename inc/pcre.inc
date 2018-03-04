source_pcreinstall() {
  if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"
  
    cd $DIR_TMP
    echo "Compiling PCRE..."
    cd pcre-${PCRE_VERSION}
    make clean
    ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
    make$MAKETHREADS
    make install
  fi
}

pcredir_check() {
  PCRELINKDIR=$(tar -tzf "$DIR_TMP/${PCRELINKFILE}" | head -1 | cut -f1 -d"/")
  if [[ ! -f "${DIR_TMP}/${PCRELINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRELINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    nginxpcretarball
  elif [[ -f "${DIR_TMP}/${PCRELINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRELINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    # in case pcre tarball download is incomplete, redownload again
    rm -rf "${DIR_TMP}/${PCRELINKFILE}"
    nginxpcretarball
  fi
}

