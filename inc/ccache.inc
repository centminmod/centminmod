sar_call() {
  $SARCALL 1 1
}

ccacheinstall() {

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
else
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    service sysstat restart
    chkconfig sysstat on
  else
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
fi

# http://ccache.samba.org/manual.html#_environment_variables
if [ "$CCACHEINSTALL" == 'y' ]; then
  if [ -f /usr/bin/ccache ]; then
    OLDERCM_INSTALL=y
  fi
  if [[ -f /usr/local/bin/ccache && ! -f "${DIR_TMP}/${CCACHEFILE}" ]] || [[ "$OLDERCM_INSTALL" = [Yy] && ! -f "${DIR_TMP}/${CCACHEFILE}" ]]; then
    if [[ "$CCACHE_VER" != "$(ccache -V | head -n 1 | awk '{print $3}')"  ]]; then
      echo "update ccache version... one time task"
      sleep 2
      echo "download ccache $CCACHE_VER"
      sleep 2
      cd ${DIR_TMP}
      wget -cq ${CCCACHELINK}
      if [ ! -d "${DIR_TMP}/ccache-${CCACHE_VER}" ]; then
        tar xzf ${CCACHEFILE}
      fi
    fi
  fi

  if [[ ! -f /usr/local/bin/ccache ]] || [[ -f /usr/local/bin/ccache && -d "${DIR_TMP}/ccache-${CCACHE_VER}" && "$CCACHE_VER" != "$(ccache -V | head -n 1 | awk '{print $3}')" ]]; then
    echo "install ccache $CCACHE_VER"
    sleep 2
    # echo "cd ${DIR_TMP}/ccache-${CCACHE_VER}/"
    cd ${DIR_TMP}/ccache-${CCACHE_VER}/
    make clean -s
    ./configure -q
    sar_call
    make${MAKETHREADS} -s
    sar_call
    make install -s
    sar_call
    echo
    if [ -f /usr/local/bin/ccache ]; then
      echo "completed update to ccache $CCACHE_VER"
      CCUPDATESTATUS=ok
    else
      echo "ccache $CCACHE_VER update failed"
      CCUPDATESTATUS=failed
    fi

    if [ -f /usr/local/bin/ccache ]; then
      mkdir -p /home/.ccache
      chmod 700 /home/.ccache
      sed -i '/CCACHE_DIR/d' ~/.bashrc
      sed -i '/ccache gcc/d' ~/.bashrc
      sed -i '/ccache g++/d' ~/.bashrc
      echo "run_second_cpp = true" > /home/.ccache/ccache.conf
      echo "compression = true" >> /home/.ccache/ccache.conf
      echo "compression_level = 2" >> /home/.ccache/ccache.conf
      echo "export CCACHE_DIR=\"/home/.ccache\"" >> ~/.bashrc
      echo "export CC=\"ccache gcc\"" >> ~/.bashrc
      echo "export CXX=\"ccache g++\"" >> ~/.bashrc
      # echo "export CC=\"ccache clang\"" >> ~/.bashrc
      # echo "export CXX=\"ccache clang++\"" >> ~/.bashrc
      sed -i '/CCACHE_CPP2/d' ~/.bashrc 
      echo "export CCACHE_CPP2=yes" >> ~/.bashrc
      #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> ~/.bashrc
      if [ -z "$(cat ~/.bashrc | grep 'PATH' | grep ccache)" ]; then
        echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> ~/.bashrc
      fi
      source ~/.bashrc
      ccache -M $CCACHESIZE
    fi
  cd ${CM_INSTALLDIR}
  fi

  # check if ccache YUM installed version and remove outdated 3.1.9
  # only if ccache $CCACHE_VER source installed successfully
  if [[ -f /usr/bin/ccache && "$CCUPDATESTATUS" = 'ok' ]]; then
    yum -yq remove ccache 
  fi

fi

}