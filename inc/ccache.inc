sar_call() {
  $SARCALL 1 1
}

ccacheinstall() {
if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$DNF_ENABLE" = [yY] ]]; then
  if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
    yum -y -q install epel-release
    yum clean all
  fi
  if [[ "$DNF_COPR" = [yY] ]]; then
cat > "/etc/yum.repos.d/dnf-centos.repo" <<EOF
[dnf-centos]
name=Copr repo for dnf-centos owned by @rpm-software-management
baseurl=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://copr-be.cloud.fedoraproject.org/results/@rpm-software-management/dnf-centos/pubkey.gpg
enabled=1
enabled_metadata=1
EOF
  fi
  if [[ ! -f /usr/bin/dnf ]]; then
    yum -y -q install dnf
    dnf clean all
  fi
  if [ ! "$(grep -w 'exclude' /etc/dnf/dnf.conf)" ]; then
    echo "excludepkgs=*.i386 *.i586 *.i686" >> /etc/dnf/dnf.conf
  fi
  if [ ! "$(grep -w 'fastestmirror=true' /etc/dnf/dnf.conf)" ]; then
    echo "fastestmirror=true" >> /etc/dnf/dnf.conf
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
      DISABLEREPO_DNF=' --disablerepo=rpmforge'
      YUMDNFBIN="dnf${DISABLEREPO_DNF}"
  else
      DISABLEREPO_DNF=""
      YUMDNFBIN='dnf'
  fi
else
  YUMDNFBIN='yum'
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    DISABLEREPO_DNF=' --disablerepo=rpmforge'
  else
    DISABLEREPO_DNF=""
  fi
fi

if [ ! -f /usr/bin/sar ]; then
  time $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
elif [ -f /usr/bin/sar ]; then
  if [[ "$(uname -m)" = 'x86_64' || "$(uname -m)" = 'aarch64' ]]; then
    SARCALL='/usr/lib64/sa/sa1'
  else
    SARCALL='/usr/lib/sa/sa1'
  fi
  if [[ "$CENTOS_SIX" = '6' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    service sysstat restart
    chkconfig sysstat on
  elif [[ "$CENTOS_SEVEN" = '7' ]]; then
    sed -i 's|10|5|g' /etc/cron.d/sysstat
    if [ -d /etc/cron.d ]; then
      echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    fi
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_EIGHT" = '8' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    sed -i 's|10|5|g' /usr/lib/systemd/system/sysstat-collect.timer
    #if [ -d /etc/cron.d ]; then
    #  echo '* * * * * root /usr/lib64/sa/sa1 1 1' > /etc/cron.d/cmsar
    #fi
    systemctl daemon-reload
    systemctl restart sysstat.service
    systemctl enable sysstat.service
  fi
fi

# http://ccache.samba.org/manual.html#_environment_variables
if [ "$CCACHEINSTALL" == 'y' ]; then
  # Check if we should use EPEL package installation for EL8/9/10
  if [[ "$CCACHE_YUM_INSTALL" = 'y' ]] && [ ! -f /usr/bin/ccache ] && [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]]; then
    echo "Installing ccache from EPEL repository..."
    if [ ! -f /usr/bin/ccache ]; then
      echo "$YUMDNFBIN -y install ccache${DISABLEREPO_DNF}"
      $YUMDNFBIN -y install ccache${DISABLEREPO_DNF}
      if [ -f /usr/bin/ccache ]; then
        echo "ccache installed successfully from EPEL"
        CCACHE_INSTALLED_PATH="/usr/bin/ccache"
        # deal with old ccache version that is source compiled
        if [ -f /usr/local/bin/ccache ]; then
          mv /usr/local/bin/ccache /usr/local/bin/ccache.old
          . ~/.bashrc
        fi
      else
        echo "ccache installation from EPEL failed"
        return 1
      fi
    else
      echo "ccache already installed from EPEL"
      CCACHE_INSTALLED_PATH="/usr/bin/ccache"
    fi
    
    # Setup bashrc and ccache configuration
    if [ -f "$CCACHE_INSTALLED_PATH" ]; then
      mkdir -p /home/.ccache
      chmod 700 /home/.ccache
      sed -i '/CCACHE_DIR/d' /root/.bashrc
      sed -i '/ccache gcc/d' /root/.bashrc
      sed -i '/ccache g++/d' /root/.bashrc
      echo "run_second_cpp = true" > /home/.ccache/ccache.conf
      echo "compression = true" >> /home/.ccache/ccache.conf
      echo "compression_level = 2" >> /home/.ccache/ccache.conf
      echo "export CCACHE_DIR=\"/home/.ccache\"" >> /root/.bashrc
      echo "export CC=\"ccache gcc\"" >> /root/.bashrc
      echo "export CXX=\"ccache g++\"" >> /root/.bashrc
      # echo "export CC=\"ccache clang\"" >> /root/.bashrc
      # echo "export CXX=\"ccache clang++\"" >> /root/.bashrc
      sed -i '/CCACHE_CPP2/d' /root/.bashrc 
      echo "export CCACHE_CPP2=yes" >> /root/.bashrc
      #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> /root/.bashrc
      if [ -z "$(cat /root/.bashrc | grep 'PATH' | grep ccache)" ]; then
        echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> /root/.bashrc
      fi
      source /root/.bashrc
      if [[ "$(id -u)" -ne '0' ]]; then
        sed -i '/CCACHE_DIR/d' $HOME/.bashrc
        sed -i '/ccache gcc/d' $HOME/.bashrc
        sed -i '/ccache g++/d' $HOME/.bashrc
        echo "run_second_cpp = true" > /home/.ccache/ccache.conf
        echo "compression = true" >> /home/.ccache/ccache.conf
        echo "compression_level = 2" >> /home/.ccache/ccache.conf
        echo "export CCACHE_DIR=\"/home/.ccache\"" >> $HOME/.bashrc
        echo "export CC=\"ccache gcc\"" >> $HOME/.bashrc
        echo "export CXX=\"ccache g++\"" >> $HOME/.bashrc
        # echo "export CC=\"ccache clang\"" >> $HOME/.bashrc
        # echo "export CXX=\"ccache clang++\"" >> $HOME/.bashrc
        sed -i '/CCACHE_CPP2/d' $HOME/.bashrc 
        echo "export CCACHE_CPP2=yes" >> $HOME/.bashrc
        #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> $HOME/.bashrc
        if [ -z "$(cat $HOME/.bashrc | grep 'PATH' | grep ccache)" ]; then
          echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> $HOME/.bashrc
        fi
        source $HOME/.bashrc
      fi
      ccache -M $CCACHESIZE
    fi
  elif [[ "$CCACHE_YUM_INSTALL" != 'y' ]]; then
    # Original source compilation method for CentOS 7 or when CCACHE_YUM_INSTALL='n'
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
    if [[ "$CCACHE_VER_FOUR" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
      echo "install ccache $CCACHE_VER"
      sleep 2
      if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
        if [ -f /opt/rh/devtoolset-8/enable ]; then
          source /opt/rh/devtoolset-8/enable
        elif [ -f /opt/rh/devtoolset-9/enable ]; then
          source /opt/rh/devtoolset-9/enable
        elif [ -f /opt/rh/devtoolset-10/enable ]; then
          source /opt/rh/devtoolset-10/enable
        elif [ -f /opt/rh/devtoolset-11/enable ]; then
          source /opt/rh/devtoolset-11/enable
        elif [ -f /opt/rh/devtoolset-12/enable ]; then
          source /opt/rh/devtoolset-12/enable
        elif [ -f /opt/rh/devtoolset-13/enable ]; then
          source /opt/rh/devtoolset-13/enable
        fi
      elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
        if [ -f /opt/rh/gcc-toolset-9/enable ]; then
          source /opt/rh/gcc-toolset-9/enable
        elif [ -f /opt/rh/gcc-toolset-10/enable ]; then
          source /opt/rh/gcc-toolset-10/enable
        elif [ -f /opt/rh/gcc-toolset-11/enable ]; then
          source /opt/rh/gcc-toolset-11/enable
        elif [ -f /opt/rh/gcc-toolset-12/enable ]; then
          source /opt/rh/gcc-toolset-12/enable
        elif [ -f /opt/rh/gcc-toolset-13/enable ]; then
          source /opt/rh/gcc-toolset-13/enable
        elif [ -f /opt/rh/gcc-toolset-14/enable ]; then
          source /opt/rh/gcc-toolset-14/enable
        elif [ -f /etc/profile.d/gcc15-custom.sh ]; then
          source /etc/profile.d/gcc15-custom.sh
        fi
      fi
      if [[ ! -s /usr/local/lib/libhiredis.so ]]; then
        yum -y localinstall ${LOCALCENTMINMOD_MIRROR}/centminmodparts/hiredis/centmin-hiredis-0.14.1-1.el7.x86_64.rpm
        ldconfig
      fi
      cd ${DIR_TMP}/ccache-${CCACHE_VER}/
      rm -rf build
      mkdir -p build
      cd build
      export CC=gcc
      export CXX="g++"
      export CMAKE_C_COMPILER=gcc
      export CMAKE_CXX_COMPILER=g++
      make clean
      rm -f CMakeCache.txt
      PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH cmake3 -DCMAKE_BUILD_TYPE=Release -DZSTD_FROM_INTERNET=ON -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -D CMAKE_PREFIX_PATH=/usr/local/lib -D CMAKE_INCLUDE_PATH=/usr/local/include ..
      sar_call
      make -j${MAKETHREADS}
      sar_call
      make install
      sar_call
      export CC='ccache gcc'
      export CXX="ccache g++"
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
        sed -i '/CCACHE_DIR/d' /root/.bashrc
        sed -i '/ccache gcc/d' /root/.bashrc
        sed -i '/ccache g++/d' /root/.bashrc
        echo "run_second_cpp = true" > /home/.ccache/ccache.conf
        echo "compression = true" >> /home/.ccache/ccache.conf
        echo "compression_level = 2" >> /home/.ccache/ccache.conf
        echo "export CCACHE_DIR=\"/home/.ccache\"" >> /root/.bashrc
        echo "export CC=\"ccache gcc\"" >> /root/.bashrc
        echo "export CXX=\"ccache g++\"" >> /root/.bashrc
        # echo "export CC=\"ccache clang\"" >> /root/.bashrc
        # echo "export CXX=\"ccache clang++\"" >> /root/.bashrc
        sed -i '/CCACHE_CPP2/d' /root/.bashrc 
        echo "export CCACHE_CPP2=yes" >> /root/.bashrc
        #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> /root/.bashrc
        if [ -z "$(cat /root/.bashrc | grep 'PATH' | grep ccache)" ]; then
          echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> /root/.bashrc
        fi
        source /root/.bashrc
        if [[ "$(id -u)" -ne '0' ]]; then
          sed -i '/CCACHE_DIR/d' $HOME/.bashrc
          sed -i '/ccache gcc/d' $HOME/.bashrc
          sed -i '/ccache g++/d' $HOME/.bashrc
          echo "run_second_cpp = true" > /home/.ccache/ccache.conf
          echo "compression = true" >> /home/.ccache/ccache.conf
          echo "compression_level = 2" >> /home/.ccache/ccache.conf
          echo "export CCACHE_DIR=\"/home/.ccache\"" >> $HOME/.bashrc
          echo "export CC=\"ccache gcc\"" >> $HOME/.bashrc
          echo "export CXX=\"ccache g++\"" >> $HOME/.bashrc
          # echo "export CC=\"ccache clang\"" >> $HOME/.bashrc
          # echo "export CXX=\"ccache clang++\"" >> $HOME/.bashrc
          sed -i '/CCACHE_CPP2/d' $HOME/.bashrc 
          echo "export CCACHE_CPP2=yes" >> $HOME/.bashrc
          #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> $HOME/.bashrc
          if [ -z "$(cat $HOME/.bashrc | grep 'PATH' | grep ccache)" ]; then
            echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> $HOME/.bashrc
          fi
          source $HOME/.bashrc
        fi
        ccache -M $CCACHESIZE
        # check if ccache YUM installed version and remove outdated 3.1.9
        # only if ccache $CCACHE_VER source installed successfully
        if [[ -f /usr/bin/ccache && "$CCUPDATESTATUS" = 'ok' ]]; then
          yum -yq remove ccache 
        fi
      fi
    else
      # not CCACHE_VER_FOUR=y & CentOS 7
      echo "install ccache $CCACHE_VER"
      sleep 2
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
        sed -i '/CCACHE_DIR/d' /root/.bashrc
        sed -i '/ccache gcc/d' /root/.bashrc
        sed -i '/ccache g++/d' /root/.bashrc
        echo "run_second_cpp = true" > /home/.ccache/ccache.conf
        echo "compression = true" >> /home/.ccache/ccache.conf
        echo "compression_level = 2" >> /home/.ccache/ccache.conf
        echo "export CCACHE_DIR=\"/home/.ccache\"" >> /root/.bashrc
        echo "export CC=\"ccache gcc\"" >> /root/.bashrc
        echo "export CXX=\"ccache g++\"" >> /root/.bashrc
        # echo "export CC=\"ccache clang\"" >> /root/.bashrc
        # echo "export CXX=\"ccache clang++\"" >> /root/.bashrc
        sed -i '/CCACHE_CPP2/d' /root/.bashrc 
        echo "export CCACHE_CPP2=yes" >> /root/.bashrc
        #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> /root/.bashrc
        if [ -z "$(cat /root/.bashrc | grep 'PATH' | grep ccache)" ]; then
          echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> /root/.bashrc
        fi
        source /root/.bashrc
        if [[ "$(id -u)" -ne '0' ]]; then
          sed -i '/CCACHE_DIR/d' $HOME/.bashrc
          sed -i '/ccache gcc/d' $HOME/.bashrc
          sed -i '/ccache g++/d' $HOME/.bashrc
          echo "run_second_cpp = true" > /home/.ccache/ccache.conf
          echo "compression = true" >> /home/.ccache/ccache.conf
          echo "compression_level = 2" >> /home/.ccache/ccache.conf
          echo "export CCACHE_DIR=\"/home/.ccache\"" >> $HOME/.bashrc
          echo "export CC=\"ccache gcc\"" >> $HOME/.bashrc
          echo "export CXX=\"ccache g++\"" >> $HOME/.bashrc
          # echo "export CC=\"ccache clang\"" >> $HOME/.bashrc
          # echo "export CXX=\"ccache clang++\"" >> $HOME/.bashrc
          sed -i '/CCACHE_CPP2/d' $HOME/.bashrc 
          echo "export CCACHE_CPP2=yes" >> $HOME/.bashrc
          #echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> $HOME/.bashrc
          if [ -z "$(cat $HOME/.bashrc | grep 'PATH' | grep ccache)" ]; then
            echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> $HOME/.bashrc
          fi
          source $HOME/.bashrc
        fi
        ccache -M $CCACHESIZE
        # check if ccache YUM installed version and remove outdated 3.1.9
        # only if ccache $CCACHE_VER source installed successfully
        if [[ -f /usr/bin/ccache && "$CCUPDATESTATUS" = 'ok' ]]; then
          yum -yq remove ccache 
        fi
      fi
    fi
    cd ${CM_INSTALLDIR}
    fi
  fi  # End of source compilation else block
fi

}