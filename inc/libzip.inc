check_pythonthree_six() {
  if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  # if [[ "$(echo "$CENTOSVER" | sed -e 's|\.||g')" -ge '77' ]]; then
    # CentOS 7.7+ already have native python 3.6 yum packages
    # via python3 and python3-libs so no longer require EPEL python36 packages
    if [[ "$CENTOS_SEVEN" -eq '7' && -z "$(rpm -qa python3)" ]]; then
      yum -q -y install python3
    fi
    if [[ "$CENTOS_SEVEN" -eq '7' && -z "$(rpm -qa python3-libs)" ]]; then
      yum -q -y install python3-libs
    fi
  # else
  #   if [[ "$CENTOS_SEVEN" -eq '7' && "$(yum list python36 -q 2>&1 | grep -o No)" = 'No' ]]; then
  #     # workaround for premature EPEL python36 package retirement before CentOS 7.7 is ready
  #     # https://community.centminmod.com/threads/18142/
  #     if [ ! -f "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ]; then
  #       wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ${LOCALCENTMINMOD_MIRROR}/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     if [ ! -f "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" ]; then
  #       wget -4 -q -O "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" ${LOCALCENTMINMOD_MIRROR}/centminmodparts/epel/el7/x86_64/python36-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     yum -y localinstall "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
  #   fi
  #   if [[ "$CENTOS_SEVEN" -eq '7' && "$(yum list python36-libs -q 2>&1 | grep -o No)" = 'No' ]]; then
  #     # workaround for premature EPEL python36 package retirement before CentOS 7.7 is ready
  #     # https://community.centminmod.com/threads/18142/
  #     if [ ! -f "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ]; then
  #       wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ${LOCALCENTMINMOD_MIRROR}/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     yum -y localinstall "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
  #   fi
  fi
}

libzip_remove() {
  if [[ -f /usr/local/include/zip.h ]]; then
    # EL8/EL9 uses remi-libzip package now so no longer uses custom libzip_install
    echo "EL8/EL9 detected: removing older custom libzip from /usr/local"
    rm -rf /usr/local/include/zip.h
    rm -rf /usr/local/include/zipconfig.h
    rm -rf /usr/local/share/man/man1/zip*
    rm -rf /usr/local/share/man/man3/zip*
    rm -f /usr/local/lib64/libzip.so*
    rm -rf /usr/local/lib64/pkgconfig/libzip.pc
    ldconfig
  fi
  if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]] && [[ "$IMAGEMAGICK_SEVEN" != [yY] && -f "$(rpm -ql libzip | grep -m1 '.so')" ]]; then
    # remove ImageMgick7 installed libzip
    yum -y remove libzip libzip-devel
  fi
}

libzip_install() {
  check_pythonthree_six
  if [ ! -f /usr/bin/cmake3 ]; then
      echo
      echo "yum -y install cmake3"
      yum -y install cmake3
      echo
  fi
  if [[ "$(rpm -ql libzip5-devel | grep -o 'libzip.so' )" = 'libzip.so' ]]; then
      yum -y remove libzip5 libzip5-devel --enablerepo=remi
  fi
  pushd "$DIR_TMP"
  cecho "Download ${LIBZIP_LINKFILE} ..." $boldyellow
  if [ -s "${LIBZIP_LINKFILE}" ]; then
      cecho "${LIBZIP_LINKFILE} found, skipping download..." $boldgreen
  else
      echo "wget --progress=bar $LIBZIP_LINK --tries=3"
      wget --progress=bar $LIBZIP_LINK --tries=3
      ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
          cecho "Error: ${LIBZIP_LINKFILE} download failed." $boldgreen
      else 
          cecho "Download done." $boldyellow
      fi
  fi
  tar xvzf ${LIBZIP_LINKFILE}
  cd libzip-${LIBZIP_VER}
  rm -rf build
  mkdir -p build
  cd build/
  cmake3 .. \
  -DCMAKE_C_FLAGS="-fPIC -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2" \
  -DCMAKE_CXX_FLAGS="-fPIC -O2" \
  -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-z,relro,-z,now -pie"
  make${MAKETHREADS}
  make install
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPMVER" = '8.4' || "$PHPMUVER" = '8.4' || "$PHPVER_ID" -ge '80000' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        LIBDIR_LIBZIP='lib64'
      else
        LIBDIR_LIBZIP='lib'
      fi
      # PHP 7.4 not finding libzip paths
      export LIBZIP_CFLAGS='-I/usr/local/include'
      export LIBZIP_LIBS="-L/usr/local/${LIBDIR_LIBZIP} -lzip"
      LIBZIP_LD_PATH="/usr/local/${LIBDIR_LIBZIP}:"
      LIBZIP_OPT=" --with-libzip=/usr/local/${LIBDIR_LIBZIP}"
      LIBZIP_RPATH=" -Wl,-rpath,/usr/local/${LIBDIR_LIBZIP}"
      LIBZIP_PKG_CONFIG_PATH="/usr/local/${LIBDIR_LIBZIP}/pkgconfig:"
      echo "/usr/local/${LIBDIR_LIBZIP}" | tee /etc/ld.so.conf.d/libzip.conf
      ldconfig
      echo
      echo "ldconfig -p | grep libzip"
      ldconfig -p | grep libzip
      echo
    fi
  popd
}