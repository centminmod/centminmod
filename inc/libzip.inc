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
  #       wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     if [ ! -f "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" ]; then
  #       wget -4 -q -O "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     yum -y localinstall "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
  #   fi
  #   if [[ "$CENTOS_SEVEN" -eq '7' && "$(yum list python36-libs -q 2>&1 | grep -o No)" = 'No' ]]; then
  #     # workaround for premature EPEL python36 package retirement before CentOS 7.7 is ready
  #     # https://community.centminmod.com/threads/18142/
  #     if [ ! -f "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ]; then
  #       wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
  #     fi
  #     yum -y localinstall "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
  #   fi
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
      echo "wget -c${ipv_forceopt} --progress=bar $LIBZIP_LINK --tries=3"
      wget -c${ipv_forceopt} --progress=bar $LIBZIP_LINK --tries=3
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
  cmake3 ..
  make${MAKETHREADS}
  make install
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        LIBDIR_LIBZIP='lib64'
      else
        LIBDIR_LIBZIP='lib'
      fi
      # PHP 7.4 not finding libzip paths
      export LIBZIP_CFLAGS='-I/usr/local/include'
      export LIBZIP_LIBS="-L/usr/local/${LIBDIR_LIBZIP} -lzip"
    fi
  popd
}