pcre_two_dir_check() {
  PCRETWOLINKDIR=$(tar -tzf "$DIR_TMP/${PCRETWOLINKFILE}" | head -1 | cut -f1 -d"/")
  if [[ ! -f "${DIR_TMP}/${PCRETWOLINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRETWOLINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    nginxpcretwotarball
  elif [[ -f "${DIR_TMP}/${PCRETWOLINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRETWOLINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    # in case pcre tarball download is incomplete, redownload again
    rm -rf "${DIR_TMP}/${PCRETWOLINKFILE}"
    nginxpcretwotarball
  fi
}

source_pcre_two_install() {
  if [[ "$DETECT_NGXVER" -ge '1021005' && "$NGINX_PCRE_TWO" = [yY] && -d "$DIR_TMP/pcre2-${NGINX_PCRETWOVER}" ]]; then
    {
    echo "*************************************************"
    cecho "* Source Install PCRE2" $boldgreen
    echo "*************************************************"
    pcre_two_dir_check
    pushd $DIR_TMP
    echo "Compiling PCRE2..."
    cd pcre2-${NGINX_PCRETWOVER}
    make clean
    ./configure --prefix=/usr/local/nginx-dep --enable-pcre2-16 --enable-pcre2-32 --enable-jit --enable-pcre2grep-libz --enable-pcre2grep-libbz2 --enable-pcre2test-libreadline
    make$MAKETHREADS
    make install
    popd
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log"
  fi
}

source_pcreinstall() {
  if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; then
    {
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
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre_install.log"
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

