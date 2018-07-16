libzip_install() {
  if [ ! -f /usr/bin/cmake3 ]; then
      echo
      echo "yum -y install cmake3"
      yum -y install cmake3
      echo
  fi
  if [[ "$(rpm -ql libzip5-devel | grep -o 'libzip.so' )" = 'libzip.so' ]]; then
      yum -y remove libzip5 libzip5-devel --enablerepo=remi
  fi
  pushd /svr-setup/
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