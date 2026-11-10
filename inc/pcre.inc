pcredir_check() {
  is_upgrade=$1
  PCRELINKDIR=$(tar -tzf "$DIR_TMP/${PCRELINKFILE}" | head -1 | cut -f1 -d"/")
  if [[ ! -f "${DIR_TMP}/${PCRELINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRELINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    {
      nginxpcretarball
    } 2>&1 | tee "${CENTMINLOGDIR}/nginx_pcre_download_${DT}.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  elif ! tar -tzf "$DIR_TMP/${PCRELINKFILE}" >/dev/null 2>&1; then
    # in case pcre tarball download is incomplete, redownload again
    rm -rf "${DIR_TMP}/${PCRELINKFILE}"
    {
      nginxpcretarball
    } 2>&1 | tee "${CENTMINLOGDIR}/nginx_pcre_download_${DT}.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  fi
}

pcre_two_dir_check() {
  is_upgrade=$1
  PCRETWOLINKDIR=$(tar -tzf "$DIR_TMP/${PCRETWOLINKFILE}" | head -1 | cut -f1 -d"/")
  if [[ ! -f "${DIR_TMP}/${PCRETWOLINKFILE}" || "$(ls -d ${DIR_TMP}/${PCRETWOLINKDIR} >/dev/null 2>&1; echo $?)" != '0' ]]; then
    {
    nginxpcretwotarball
    } 2>&1 | tee "${CENTMINLOGDIR}/nginx_pcre2_download_${DT}.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  elif ! tar -tzf "$DIR_TMP/${PCRETWOLINKFILE}" >/dev/null 2>&1; then
    # in case pcre tarball download is incomplete, redownload again
    rm -rf "${DIR_TMP}/${PCRETWOLINKFILE}"
    {
    nginxpcretwotarball
    } 2>&1 | tee "${CENTMINLOGDIR}/nginx_pcre2_download_${DT}.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  fi
}

source_pcre_two_install() {
  if [[ "$DETECT_NGXVER" -ge '1021005' && "$NGINX_PCRE_TWO" = [yY] && -d "$DIR_TMP/pcre2-${NGINX_PCRETWOVER}" ]]; then
    {
    echo "*************************************************"
    cecho "* Source Install PCRE2" $boldgreen
    echo "*************************************************"
    # pcre_two_dir_check
    pushd "$DIR_TMP" || exit 1
    echo "Compiling PCRE2..."
    cd "pcre2-${NGINX_PCRETWOVER}" || exit 1
    make clean
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --prefix=/usr/local/nginx-dep --enable-pcre2-16 --enable-pcre2-32 --enable-jit --enable-pcre2grep-libz --enable-pcre2grep-libbz2 --enable-pcre2test-libreadline
    ngx_pcretwo_install_config_err=$?
    if [ "$ngx_pcretwo_install_config_err" -ne '0' ]; then
      echo
      echo "error: pcre2 configure stage failed"
      pcretwo_err_msg='pcre2-install-config-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcretwo_err_msg
      fi
      exit 1
    fi
    make$MAKETHREADS
    ngx_pcretwo_install_make_err=$?
    if [ "$ngx_pcretwo_install_make_err" -ne '0' ]; then
      echo
      echo "error: pcre2 make stage failed"
      pcretwo_err_msg='pcre2-install-make-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcretwo_err_msg
      fi
      exit 1
    fi
    make install
    ngx_pcretwo_install_makeinstall_err=$?
    if [ "$ngx_pcretwo_install_makeinstall_err" -ne '0' ]; then
      echo
      echo "error: pcre2 make install stage failed"
      pcretwo_err_msg='pcre2-install-cmakeinstall-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcretwo_err_msg
      fi
      exit 1
    fi
    # remove nginx pcre v8.45 files
    # find /usr/local/nginx-dep/{lib,include} -name "*pcre*" | grep -v pcre2
    find /usr/local/nginx-dep/{lib,include} \( -name "*pcre*" ! -name "*pcre2*" \) -delete
    popd
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  fi
}

nginx_pcreinstall() {
  if [[ "$NGINX_PCRE" = [yY] ]]; then
    {
    echo "*************************************************"
    cecho "* Source Install PCRE For Nginx" $boldgreen
    echo "*************************************************"
    # pcredir_check
    pushd "$DIR_TMP" || exit 1
    echo "Compiling PCRE For Nginx..."
    cd "pcre-${PCRE_VERSION}" || exit 1
    make clean
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --prefix=/usr/local/nginx-dep --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit
    ngx_pcreinstall_config_err=$?
    if [ "$ngx_pcreinstall_config_err" -ne '0' ]; then
      echo
      echo "error: pcre configure stage failed"
      pcre_err_msg='pcre-install-config-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcre_err_msg
      fi
      exit 1
    fi
    make$MAKETHREADS
    ngx_pcreinstall_make_err=$?
    if [ "$ngx_pcreinstall_make_err" -ne '0' ]; then
      echo
      echo "error: pcre make stage failed"
      pcre_err_msg='pcre-install-make-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcre_err_msg
      fi
      exit 1
    fi
    make install
    ngx_pcreinstall_makeinstall_err=$?
    if [ "$ngx_pcreinstall_makeinstall_err" -ne '0' ]; then
      echo
      echo "error: pcre make install stage failed"
      pcre_err_msg='pcre-install-cmakeinstall-err'
      if [[ "$DROPBOX_SEND" = [yY] ]]; then
          echo
          echo "dropbox_generate $DT"
          dropbox_generate "$DT" $pcre_err_msg
      fi
      exit 1
    fi
    popd
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre-nginx_install.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  fi
}

source_pcreinstall() {
  if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; then
    {
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"
  
    cd "$DIR_TMP" || exit 1
    echo "Compiling PCRE..."
    cd "pcre-${PCRE_VERSION}" || exit 1
    make clean
    CFLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" CPPFLAGS="-D_FORTIFY_SOURCE=2" CXXFLAGS="-fPIC -O2" LDFLAGS="-Wl,-z,relro,-z,now -pie" ./configure --enable-utf8 --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline --enable-jit || exit 1
    make$MAKETHREADS || exit 1
    make install || exit 1
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre_install.log"
    [[ "${PIPESTATUS[*]}" = "0 0" ]] || return 1
  fi
}
