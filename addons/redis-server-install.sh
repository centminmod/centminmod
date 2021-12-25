#!/bin/bash
######################################################
# redis installer
# written by George Liu (eva2000) centminmod.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
REDIS_SOURCEVER='6.2.6'
REDIS_THREADIO='n'

OSARCH=$(uname -m)
SRCDIR=/svr-setup

DEVTOOLSETFOUR='n'
DEVTOOLSETSIX='n'
DEVTOOLSETSEVEN='n'
DEVTOOLSETEIGHT='n'
DEVTOOLSETNINE='n'
DEVTOOLSETTEN='y'
DEVTOOLSETELEVEN='n'
GOLDLINKER='n'
FLTO='n'
DWARF='n'
HOIST='y'
######################################################
# functions
#############
if [ ! -d "$SRCDIR" ]; then
  mkdir -p "$SRCDIR"
fi

if [ "$OSARCH" != 'x86_64' ]; then
  echo
  echo "64bit OS only"
  echo "aborting..."
  exit
fi

if [ ! -d /etc/systemd/system ]; then
  echo
  echo "systemd not detected aborting..."
  echo
  exit
fi

if [ ! -f /etc/yum.repos.d/remi.repo ]; then
  echo
  echo "redis REMI YUM repo not installed"
  echo "installing..."
  wget -cnv https://rpms.remirepo.net/enterprise/remi-release-7.rpm
  rpm -Uvh remi-release-7.rpm
fi

if [ ! -f /etc/yum.repos.d/epel.repo ]; then
  echo
  echo "epel YUM repo not installed"
  echo "installing..."
  yum -y install epel-release
fi

if [ -f /proc/user_beancounters ]; then
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        CPUS=$(echo $(($CPUS+4)))
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

redisinstall() {
  echo "install redis server..."
  if [[ -f /etc/yum/pluginconf.d/priorities.conf && "$(grep 'enabled = 1' /etc/yum/pluginconf.d/priorities.conf)" ]]; then
    yum -y install redis --enablerepo=remi --disableplugin=priorities
  else
    yum -y install redis --enablerepo=remi
  fi
  sed -i 's|LimitNOFILE=.*|LimitNOFILE=524288|' /etc/systemd/system/redis.service.d/limit.conf
  # echo -e "[Service]\nExecStartPre=/usr/sbin/sysctl vm.overcommit_memory=1" > /etc/systemd/system/redis.service.d/vm.conf
  # mkdir -p /redis/tools
  # echo '#!/bin/bash' > /redis/tools/disable_thp.sh
  # echo "/usr/bin/echo 'never' > /sys/kernel/mm/transparent_hugepage/enabled" >> /redis/tools/disable_thp.sh
  # chmod +x /redis/tools/disable_thp.sh
  # chown -R redis:redis /redis/tools
  # echo -e "[Service]\nExecStartPre=-/redis/tools/disable_thp.sh" > /etc/systemd/system/redis.service.d/execstartpre.conf
  # echo -e "[Unit]\nAfter=network.target rc.local" > /etc/systemd/system/redis.service.d/after-rc-local.conf

cat > "/etc/systemd/system/redis.service.d/user.conf" <<EOF
[Service]
User=redis
Group=nginx
EOF

cat > "/etc/systemd/system/disable-thp.service" <<EOF
[Unit]
Description=Disable Transparent Huge Pages (THP)
After=network.target

[Service]
Type=simple
ExecStart=/bin/sh -c "/usr/bin/echo 'never' > /sys/kernel/mm/transparent_hugepage/enabled"

[Install]
WantedBy=multi-user.target
EOF

if [ -f /etc/systemd/system/disable-thp.service ]; then
  systemctl daemon-reload
  systemctl start disable-thp
  systemctl enable disable-thp
fi

  # echo "d      /var/run/redis/         0755 redis nginx" > /etc/tmpfiles.d/redis.conf
  mkdir -p /var/run/redis
  chown redis:nginx /var/run/redis
  chmod 755 /var/run/redis
  systemctl daemon-reload
  systemctl restart redis
  systemctl enable redis
  systemctl restart redis
  systemctl enable redis
  if [[ "$(sysctl -n vm.overcommit_memory)" -ne '1' ]]; then
    echo "vm.overcommit_memory = 1" >> /etc/sysctl.conf
  fi
  if [ -f /sys/kernel/mm/transparent_hugepage/enabled ]; then
    echo never > /sys/kernel/mm/transparent_hugepage/enabled
    if [[ -z "$(grep 'transparent_hugepage\/enabled' /etc/rc.local)" ]]; then
      echo "echo never > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
    fi
  fi
  sysctl -p
  echo "redis server installled"
}

redisinstall_source() {
  echo "source install redis server..."
  if [[ "$DEVTOOLSETFOUR" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-4/enable
    fi
  fi
  if [[ "$DEVTOOLSETSIX" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-6/enable
    fi
  fi
  if [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-7/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$DEVTOOLSETEIGHT" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-8/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$DEVTOOLSETEIGHT" = [yY] ]]; then
    if [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      source /opt/gcc8/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$DEVTOOLSETNINE" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-9/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$DEVTOOLSETTEN" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-10/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$DEVTOOLSETELEVEN" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-11/enable
      if [[ "$HOIST" = [yY] ]]; then
        HOIST_OPT=' -fcode-hoisting'
      fi
      EXTRA_CFLAGS=" -Wimplicit-fallthrough=0${HOIST_OPT} -Wno-maybe-uninitialized -Wno-stringop-truncation -Wno-lto-type-mismatch -Wno-misleading-indentation -Wno-format-truncation"
    fi
  fi
  if [[ "$FLTO" = [yY] ]]; then
    FLTO_OPT=' -flto -ffat-lto-objects'
  fi
  if [[ "$GOLDLINKER" = [yY] ]]; then
    GOLDLINKER_OPT=' -fuse-ld=gold'
  fi
  if [[ "$DWARF" = [yY] ]]; then
    DWARF_OPT=' -gsplit-dwarf'
  fi
  export OPT=-03
  export CFLAGS="-march=native${FLTO_OPT}${GOLDLINKER_OPT} -fvisibility=hidden${DWARF_OPT}${EXTRA_CFLAGS}"
  export CXXFLAGS="$CFLAGS"
  cd "$SRCDIR"
  rm -rf redis-${REDIS_SOURCEVER}*
  rm -rf redis-${REDIS_SOURCEVER}-threaded*
  REDIS_THREADIO='n'
  if [[ "$REDIS_THREADIO" = [yY] ]]; then
    git clone -b threaded-io --depth=1 https://github.com/redis/redis redis-${REDIS_SOURCEVER}-threaded
    cd redis-${REDIS_SOURCEVER}-threaded
  else
    wget http://download.redis.io/releases/redis-${REDIS_SOURCEVER}.tar.gz
    tar xzf redis-${REDIS_SOURCEVER}.tar.gz
    cd redis-${REDIS_SOURCEVER}
  fi
  make distclean
  make clean
  make${MAKETHREADS}
  make install
  echo "redis server source installled" 
}

######################################################

case "$1" in
  install )
    redisinstall
    ;;
  install-source )
    redisinstall_source
    ;;
  * )
    echo
    echo "Usage:"
    echo
    echo "$0 {install|install-source}"
    echo
    ;;
esac

exit