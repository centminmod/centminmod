check_pythonthree_six() {
  if [[ "$CENTOS_SEVEN" -eq '7' && "$(yum list python36 -q 2>&1 | grep -o No)" = 'No' ]]; then
    # workaround for premature EPEL python36 package retirement before CentOS 7.7 is ready
    # https://community.centminmod.com/threads/18142/
    if [ ! -f "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ]; then
      wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
    fi
    if [ ! -f "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" ]; then
      wget -4 -q -O "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-3.6.8-1.el7.x86_64.rpm
    fi
    yum -y localinstall "${DIR_TMP}/python36-3.6.8-1.el7.x86_64.rpm" "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
  fi
  if [[ "$CENTOS_SEVEN" -eq '7' && "$(yum list python36-libs -q 2>&1 | grep -o No)" = 'No' ]]; then
    # workaround for premature EPEL python36 package retirement before CentOS 7.7 is ready
    # https://community.centminmod.com/threads/18142/
    if [ ! -f "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" ]; then
      wget -4 -q -O "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm" https://centminmod.com/centminmodparts/epel/el7/x86_64/python36-libs-3.6.8-1.el7.x86_64.rpm
    fi
    yum -y localinstall "${DIR_TMP}/python36-libs-3.6.8-1.el7.x86_64.rpm"
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
  popd
}