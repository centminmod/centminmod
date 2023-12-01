checksec_phpext() {
  if [ ! -f /usr/bin/checksec ]; then
    echo
    yum -y install checksec
    echo
  fi
  checksec_phpext_dir=$(php-config --extension-dir)
  find $checksec_phpext_dir -name "*.so" | while read slib; do
    if [ -f "${slib}" ]; then
      checksec --file="${slib}" --format=json --extended | jq -r
    fi
  done
  if [ -f /usr/local/lib/libpcre.so ]; then
    checksec --file=/usr/local/lib/libpcre.so --format=json --extended | jq -r
  fi
}

max_spawn_rate_check() {
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ ! "$(grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf)" ]] && [[ "$PHPMVER" = '8.1' && "$INITIALINSTALL" = [yY] ]]; then  
    if [[ "$FPM_MAX_SPAWN_RATE" -ge '32' && -f /usr/local/etc/php-fpm.conf ]]; then
      echo
      echo "adjust PHP 8.1.x pm.max_spawn_rate for /usr/local/etc/php-fpm.conf"
      sed -i "s|^pm.max_children|pm.max_spawn_rate = $FPM_MAX_SPAWN_RATE\npm.max_children|" /usr/local/etc/php-fpm.conf
      grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf
      echo
    fi
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ ! "$(grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf)" ]] && [[ "$PHPMUVER" = '8.1' && "$INITIALINSTALL" != [yY] ]]; then  
    if [[ "$FPM_MAX_SPAWN_RATE" -ge '32' && -f /usr/local/etc/php-fpm.conf ]]; then
      echo
      echo "adjust PHP 8.1.x pm.max_spawn_rate for /usr/local/etc/php-fpm.conf"
      sed -i "s|^pm.max_children|pm.max_spawn_rate = $FPM_MAX_SPAWN_RATE\npm.max_children|" /usr/local/etc/php-fpm.conf
      grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf
      echo
    fi
  elif [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$(grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf)" ]] && [[ "$PHPMUVER" != '8.1' && "$INITIALINSTALL" != [yY] ]]; then 
    echo
    echo "remove unsupported pm.max_spawn_rate from /usr/local/etc/php-fpm.conf"
    sed -i "/pm.max_spawn_rate/d" /usr/local/etc/php-fpm.conf
    grep 'pm.max_spawn_rate' /usr/local/etc/php-fpm.conf
    echo
  fi
}

pgo_version_edit() {
  if [[ "$PHP_PGO_APPEND_LABEL" = [yY] ]]; then
    echo "---------------------------"
    echo "PGO binary labelling"
    echo "---------------------------"
    if [[ "$PHP_PGO" = [yY] ]]; then
      # modifying php -v command to add PGO indicator
      # allowing end users to easily see if PHP-FPM
      # was built with PGO or not
      if [ -f ../sapi/cgi/cgi_main.c ]; then
        echo "append PGO label to php-cgi binary version"
        sed -i 's|PHP %s (%s) (built: %s %s)|PHP %s (%s) (built: %s %s) PGO|g' ../sapi/cgi/cgi_main.c
      else
        echo "../sapi/cgi/cgi_main.c not found"
        pwd
        ls -lah ../sapi/cgi/cgi_main.c
      fi
      if [ -f ../sapi/cli/php_cli.c ]; then
        echo "append PGO label to php cli binary version"
        sed -i 's|PHP %s (%s) (built: %s %s)|PHP %s (%s) (built: %s %s) PGO|g' ../sapi/cli/php_cli.c
      else
        echo "../sapi/cli/php_cli.c not found"
        pwd
        ls -lah ../sapi/cli/php_cli.c
      fi
      if [ -f ../sapi/fpm/fpm/fpm_main.c ]; then
        echo "append PGO label to php-fpm binary version"
        sed -i 's|PHP %s (%s) (built: %s %s)|PHP %s (%s) (built: %s %s) PGO|g' ../sapi/fpm/fpm/fpm_main.c
      else
        echo "../sapi/fpm/fpm/fpm_main.c not found"
        pwd
        ls -lah ../sapi/fpm/fpm/fpm_main.c
      fi
    else
      if [ -f ../sapi/cgi/cgi_main.c ]; then
        echo "remove PGO label to php-cgi binary version"
        sed -i 's|PHP %s (%s) (built: %s %s) PGO|PHP %s (%s) (built: %s %s)|g' ../sapi/cgi/cgi_main.c
      else
        echo "../sapi/cgi/cgi_main.c not found"
        pwd
        ls -lah ../sapi/cgi/cgi_main.c
      fi
      if [ -f ../sapi/cli/php_cli.c ]; then
        echo "remove PGO label to php cli binary version"
        sed -i 's|PHP %s (%s) (built: %s %s) PGO|PHP %s (%s) (built: %s %s)|g' ../sapi/cli/php_cli.c
      else
        echo "../sapi/cli/php_cli.c not found"
        pwd
        ls -lah ../sapi/cli/php_cli.c
      fi
      if [ -f ../sapi/fpm/fpm/fpm_main.c ]; then
        echo "remove PGO label to php-fpm binary version"
        sed -i 's|PHP %s (%s) (built: %s %s) PGO|PHP %s (%s) (built: %s %s)|g' ../sapi/fpm/fpm/fpm_main.c
      else
        echo "../sapi/fpm/fpm/fpm_main.c not found"
        pwd
        ls -lah ../sapi/fpm/fpm/fpm_main.c
      fi
    fi
    echo
  fi
}

php_pcrejit_stacksize() {
  if [[ "$PHP_PCREJIT_STACKSIZE_ADJUST" = [yY] ]]; then
    pushd ../
    get_php_pcrejit_stacksize=$(awk '/#define PCRE_JIT_STACK_MAX_SIZE/ {print $3}' ext/pcre/php_pcre.c | sed -e 's|(||g')
    echo "existing PHP PCRE_JIT_STACK_MAX_SIZE = $get_php_pcrejit_stacksize * 1024"
    if [[ "$get_php_pcrejit_stacksize" -eq '64' || "$get_php_pcrejit_stacksize" -eq '192' || "$get_php_pcrejit_stacksize" -eq '256' ]] && [ -f ext/pcre/php_pcre.c ]; then
      # only change if php 7.2 (64) or 7.3 (192) or 7.4 (256) defaults are for 
      # PHP 7+ PCRE JIT when pcre.jit=1 enabled https://www.php.net/manual/en/pcre.configuration.php
        grep -w 'PCRE_JIT_STACK_MAX_SIZE' ext/pcre/php_pcre.c
        echo
        sed -i "s|#define PCRE_JIT_STACK_MAX_SIZE .*|#define PCRE_JIT_STACK_MAX_SIZE ($PHP_PCREJIT_STACKSIZE * 1024)|" ext/pcre/php_pcre.c
        echo
        grep -w 'PCRE_JIT_STACK_MAX_SIZE' ext/pcre/php_pcre.c
        echo
    fi
    popd
  fi
}

pgophpbench() {
  # https://openbenchmarking.org/test/pts/phpbench-1.1.1
  echo
  echo "PGO phpbench setup start"
  rm -rf "${DIR_TMP}/phpbench"
  mkdir -p "${DIR_TMP}/phpbench" /opt/phpbench
  wget${ipv_forceopt_wget} -O "${DIR_TMP}/phpbench/phpbench-0.8.1.zip" https://github.com/centminmod/centminmod-php71/raw/master/scripts/phpbench-0.8.1.zip
  wget${ipv_forceopt_wget} -O /opt/phpbench/detailed_benchmark.php https://github.com/centminmod/centminmod-php71/raw/master/scripts/detailed_benchmark.php
  pushd "${DIR_TMP}/phpbench"
  unzip phpbench-0.8.1.zip
  # \cp -af tests /opt/phpbench/tests
  popd
  echo "PGO phpbench setup end"
  echo
}

oniguruma_check() {
  if [[ "$(rpm -qa oniguruma5 oniguruma5-devel | wc -l)" -eq '2' ]]; then
    echo
    echo "swap oniguruma5 oniguruma5php"
    echo
    yum -y swap oniguruma5 oniguruma5php --enablerepo=remi
    echo
    echo "swap oniguruma5-devel oniguruma5php-devel"
    echo
    yum -y swap oniguruma5-devel oniguruma5php-devel --enablerepo=remi
  fi
  if [[ "$(rpm -qa oniguruma5php oniguruma5php-devel | wc -l)" -lt '2' ]]; then
    echo
    echo "install oniguruma5php oniguruma5php-devel"
    echo
    yum -y install oniguruma5php oniguruma5php-devel --enablerepo=remi
    oni_err=$?
    echo "oni_err=$oni_err"
    if [ ! -f /usr/include/oniggnu.h ]; then
      yum clean all
      yum -y install oniguruma5php oniguruma5php-devel --enablerepo=remi
      oni_err=$?
      echo "oni_err=$oni_err"
    fi
  fi
  if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
    echo "export ONIG_CFLAGS='-I/usr/include'"
    export ONIG_CFLAGS="-I/usr/include"
    echo "export ONIG_LIBS='-L/usr/lib64 -lonig'"
    export ONIG_LIBS="-L/usr/lib64 -lonig"
    echo
    echo "ls -lAh /usr/include | grep onig"
    ls -lAh /usr/include | grep onig
    echo
    echo "ls -lAh /usr/lib64 | grep onig"
    ls -lAh /usr/lib64 | grep onig
    echo
  fi
}

function funct_phpconfigure {
# set locale temporarily to english
# for php compile due to some non-english
# locale compilation issues i.e. Turkish 
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

# debugging
# echo "CM_INSTALLDIR=${CM_INSTALLDIR}"
# exit

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "inc/custom_config.inc"
  fi
    source "inc/custom_config.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "inc/custom_config.inc" > "${CENTMINLOGDIR}/inc-custom-config-settings_${DT}.log"
    fi
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
  fi
    source "${CONFIGSCANBASE}/custom_config.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "${CONFIGSCANBASE}/custom_config.inc" > "${CENTMINLOGDIR}/etc-centminmod-custom-config-settings_${DT}.log"
    fi
fi

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
  if [ -f /usr/bin/dos2unix ]; then
    dos2unix -q "${CM_INSTALLDIR}/inc/z_custom.inc"
  fi
    source "${CM_INSTALLDIR}/inc/z_custom.inc"
    if [ -d "${CENTMINLOGDIR}" ]; then
        cat "${CM_INSTALLDIR}/inc/z_custom.inc" > "${CENTMINLOGDIR}/inc-zcustom-config-settings_${DT}.log"
    fi
fi

fpmconfdir

if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

#############################################################
TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALSWAPMEM=$(awk '/SwapTotal: / {print $2}' /proc/meminfo | head -n1)
ISLOWMEMPHP='780000'
#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISLOWMEMPHPMB=`echo "scale=0;$ISLOWMEMPHP/1024" | bc`
CHECKLOWMEMPHP=`expr $TOTALMEM \< $ISLOWMEMPHP`
#############################################################
PLIBDIR='/usr'
PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)
echo "PHP configure stage"
echo

# compiling PHP with LTO support takes 2-4x times the memory
# and cpu resources to do and takes 2-4x times longer to compile
# so only do it for servers with enough cpu cores and memory
CPUS_LTO=$(grep -c "processor" /proc/cpuinfo)
if [[ "$CPUS_LTO" -lt '16' && "$TOTALMEMMB" -lt '31000' ]]; then
  PHP_LTO='n'
elif [[ "$CPUS_LTO" -lt '16' && "$TOTALMEMMB" -gt '31000' ]]; then
  PHP_LTO='n'
fi

install_libgd_external() {
  # using --with-external-gd flag only supported in PHP 7.4+ and PHP 8.0+
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHP_LIBGD_EXTERNAL" = [yY] ]] && [[ "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
    echo
    echo "Internal libgd ${LIBGD_EXTERNAL_VER} external PHP version"
    echo
    pushd "$DIR_TMP"
    if [ ! -f "libgd-${LIBGD_EXTERNAL_VER}.tar.gz" ]; then
    wget${ipv_forceopt_wget} "https://github.com/libgd/libgd/releases/download/gd-${LIBGD_EXTERNAL_VER}/libgd-${LIBGD_EXTERNAL_VER}.tar.gz"
    tar xvfz "libgd-${LIBGD_EXTERNAL_VER}.tar.gz"
    fi
    cd "libgd-${LIBGD_EXTERNAL_VER}"
    if [[ "$LIBGD_EXTERNAL_VER" = '2.3.3' ]]; then
      wget${ipv_forceopt_wget} https://github.com/libgd/libgd/commit/f4bc1f5c26925548662946ed7cfa473c190a104a.patch -O gdflip.patch
      patch -p1 < gdflip.patch
    fi
    make clean
    ./configure --prefix="/usr/local/libgd/${LIBGD_EXTERNAL_VER}"
    make -j$(nproc)
    make install
    check_libgd_external=$?
    if [[ "$check_libgd_external" -eq '0' ]]; then
      echo "export GDLIB_CFLAGS='-I/usr/local/libgd/${LIBGD_EXTERNAL_VER}/include'"
      export GDLIB_CFLAGS="-I/usr/local/libgd/${LIBGD_EXTERNAL_VER}/include"
      echo "export GDLIB_LIBS='-L/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib -lgd'"
      export GDLIB_LIBS="-L/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib -lgd"
      # if [[ "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' ]]; then
      #   echo "export LD_LIBRARY_PATH=/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib:$LD_LIBRARY_PATH"
      #   export LD_LIBRARY_PATH=/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib:$LD_LIBRARY_PATH
      #   echo "export PKG_CONFIG_PATH=/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib64/pkgconfig:/usr/share/pkgconfig:$PKG_CONFIG_PATH"
      #   export PKG_CONFIG_PATH=/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib64/pkgconfig:/usr/share/pkgconfig:$PKG_CONFIG_PATH
      # fi
    fi
    # ls -lAhR "/usr/local/libgd/${LIBGD_EXTERNAL_VER}"
    # PKG_CONFIG_PATH="/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib/pkgconfig" pkg-config --modversion gdlib
    # PKG_CONFIG_PATH="/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib/pkgconfig" pkg-config  gdlib --cflags
    # PKG_CONFIG_PATH="/usr/local/libgd/${LIBGD_EXTERNAL_VER}/lib/pkgconfig" pkg-config  gdlib --libs
    popd
  fi
}

if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPSWOOLE" = [yY] ]] && [[ "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  SWOOLE_OPT=' --with-swoole'
else
  SWOOLE_OPT=""
fi

# Enable Foreign Function Interface FFI extension
# for PHP 7.4+ https://wiki.php.net/rfc/ffi
if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  if [[ -z "$(rpm -qa libffi-devel)" ]]; then
    yum -q -y install libffi-devel
  fi
  FFI_OPT=' --with-ffi'
else
  FFI_OPT=""
fi

# enable CentOS 7 systemd integration for PHP-FPM versions 5.5 and higher
# http://www.php.net/ChangeLog-5.php#5.5.0
if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
  if [[ "$PHPMVER" = 5.5 || "$PHPMUVER" = 5.5 || "$PHPMVER" = 5.6 || "$PHPMUVER" = 5.6 || "$PHPMVER" = 7.0 || "$PHPMUVER" = 7.0 || "$PHPMVER" = 7.1 || "$PHPMUVER" = 7.1 || "$PHPMVER" = 7.2 || "$PHPMUVER" = 7.2 || "$PHPMVER" = 7.3 || "$PHPMUVER" = 7.3 || "$PHPMVER" = 7.4 || "$PHPMUVER" = 7.4 || "$PHPMVER" = 7.5 || "$PHPMUVER" = 7.5 || "$PHPMVER" = 8.0 || "$PHPMUVER" = 8.0 || "$PHPMVER" = 8.1 || "$PHPMUVER" = 8.1 || "$PHPMVER" = 8.2 || "$PHPMUVER" = 8.2 || "$PHPVER_ID" -ge '80000' ]]; then
    if [[ $(rpm -q systemd-devel >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install systemd-devel --disableplugin=priorities >/dev/null 2>&1
    fi
    FPMSYSTEMD_INTEGRATEOPT=' --with-fpm-systemd'
  else
    FPMSYSTEMD_INTEGRATEOPT=""
  fi
else
  FPMSYSTEMD_INTEGRATEOPT=""
fi

# Disabled by default as currently PHP seems to fail to 
# compile some of the PHP extensions with GCC 8
# if [[ "$DEVTOOLSETEIGHT" = [yY] ]]; then
#   DEVTOOLSETEIGHT='n'
# fi

# If PHP 5.3 detected disable PHP EMBED SAPI
# https://community.centminmod.com/posts/63877/
if [[ "$PHPMVER" = 5.3 || "$PHPMUVER" = 5.3 ]]; then
  PHPEMBED='n'
  GCCINTEL_PHP='n'
fi

# separate make threads for PHP compiles
if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS_PHP=" -j$CPUS"
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
        # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
        # while greater than 12 cpu cores downclocks to 2.70Ghz
        CPUS_MAKEPHP=12
    elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
        # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
        # while greater than 12 cpu cores downclocks to 2.55Ghz
        CPUS_MAKEPHP=12
    elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
        # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
        # while greater than 12 cpu cores downclocks to 2.8Ghz
        CPUS_MAKEPHP=12
    elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
        # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
        # while greater than 8 cpu cores downclocks to 3.6Ghz
        CPUS_MAKEPHP=8
    fi
    if [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$(echo "$CPUS_MAKEPHP/2" | bc)" -lt '2' ]]; then
            CPUS_MAKEPHPLTO='2'
          elif [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
        fi
    else
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
else
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS_MAKEPHP" -eq '64' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/32" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '56' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/28" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '48' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/24" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '40' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/20" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '32' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/16" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '24' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/12" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '16' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/8" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '12' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/6" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi    
    elif [[ "$CPUS_MAKEPHP" -eq '10' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/5" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
        fi
    elif [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$(echo "$CPUS_MAKEPHP/2" | bc)" -lt '2' ]]; then
            CPUS_MAKEPHPLTO='2'
          elif [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
        fi
    else
        if [[ "$PHP_LTO" = [yY] ]]; then
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
          CPUS_MAKEPHPLTO=$(echo "$CPUS_MAKEPHP/4" | bc)
          if [[ "$CPUS_MAKEPHPLTO" -lt '1' ]]; then
            CPUS_MAKEPHPLTO='1'
          fi
        else
          CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
        fi
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
fi

oniguruma_check

DETECTOPENSSL_ONEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2)
DETECTOPENSSL_ONEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-3 | grep -o 1.1.1)
DETECTOPENSSL_THREEZERO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2 | grep -o 3.0)
DETECTOPENSSL_THREEONE=$(echo $OPENSSL_VERSION  | cut -d . -f1-2 | grep -o 3.1)
DETECTOPENSSL_THREETWO=$(echo $OPENSSL_VERSION  | cut -d . -f1-2 | grep -o 3.2)
# el9 uses OpenSSL 3.0 and is not supported for PHP builds <=8.0
# for PHP 7.4 to 8.0, use OpenSSL 1.1.1 instead
# https://bugs.php.net/bug.php?id=81540
if [[ "$CENTOS_NINE" -eq '9' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPVER_ID" -ge '70400' && "$PHPVER_ID" -lt '80100' ]]; then
  PHP_CUSTOMSSL='y'
fi
if [[ "$CENTOS_NINE" -eq '9' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' ]]; then
  PHP_CUSTOMSSL='y'
fi
# disable PHP custom openssl for el8 unless force flag enabled
# EL8 already uses system OpenSSL 1.1.1 which has best performance
if [[ "$CENTOS_EIGHT" -eq '8' && "$PHP_CUSTOMSSL_FORCE" != [yY] ]]; then
  PHP_CUSTOMSSL='n'
elif [[ "$CENTOS_EIGHT" -eq '8' && "$PHP_CUSTOMSSL_FORCE" = [yY] ]]; then
  PHP_CUSTOMSSL='y'
fi
# disable PHP custom openssl for el7 unless force flag enabled
# due to additional package updates required to work 100%
if [[ "$CENTOS_SEVEN" -eq '7' && "$PHP_CUSTOMSSL_FORCE" != [yY] ]]; then
  PHP_CUSTOMSSL='n'
elif [[ "$CENTOS_SEVEN" -eq '7' && "$PHP_CUSTOMSSL_FORCE" = [yY] ]]; then
  PHP_CUSTOMSSL='y'
fi
# if OpenSSL 3.0/3.1/3.2 are detected and PHP <=7.3.x used, disable custom
# PHP OpenSSL compiles as PHP 5.x to 7.3 do not support OpenSSL 3.x
# PHP 7.4 and 8.0 have backported OpenSSL 3.x supporting patches enabled
if [[ "$DETECTOPENSSL_THREEZERO" = '3.0' || "$DETECTOPENSSL_THREEONE" = '3.1' || "$DETECTOPENSSL_THREETWO" = '3.2' ]] && [[ "$PHPMVER" = '5.6' || "$PHPMUVER" = '5.6' || "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' ]] && [[ "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
  PHP_CUSTOMSSL='n'
fi

if [[ "$PHP_CUSTOMSSL" != [yY] && "$PHP_LDMOLD" = [yY] && -s /usr/local/libexec/mold ]]; then
  # PHP mold linker not compatible with custom OpenSSL 1.1.1 PHP builds
  PHP_MOLDOPT=' -B/usr/local/libexec/mold'
else
  PHP_MOLDOPT=""
fi

# whether to switch from system used zlib to cloudflare zlib performance
# fork for php conmpilations for cpus supporting SSE4.2 at least
CHECK_PCLMUL=$(gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march' | grep 'mpclmul' | grep -o enabled)
CLOUDFLARE_ZLIBPHP='n'
if [[ "$CLOUDFLARE_ZLIBPHP" = [yY] && "$(cat /proc/cpuinfo | grep -o 'sse4_2' | uniq)" = 'sse4_2' && "$CHECK_PCLMUL" = 'enabled' ]]; then
  # check if static libz.a installed usually as result of compiling nginx with
  # CLOUDFLARE_ZLIB='y' set but if not installed run install function from inc/zlib.inc
   if [[ "$(uname -m)" = 'x86_64' ]]; then
      ZLIBLIBDIR='lib64'
  else
      ZLIBLIBDIR='lib'
   fi
  if [ ! -f "/usr/local/zlib-cf/${ZLIBLIBDIR}/libz.a" ]; then
    install_cfzlibstartdir=$(pwd)
    install_cfzlib
    cd $install_cfzlibstartdir
    ZLIBPHP_OPT='--with-zlib-dir=/usr/local/zlib-cf'
    # export CFLAGS="-I/usr/local/zlib-cf/include";
    # export LDFLAGS="-L/usr/local/zlib-cf/lib" 
    # export LD_LIBRARY_PATH="/usr/local/zlib-cf/lib";
    # export PHP_RPATH="/usr/local/zlib-cf/lib"; 
  elif [ -f "/usr/local/zlib-cf/${ZLIBLIBDIR}/libz.a" ]; then
    ZLIBPHP_OPT='--with-zlib-dir=/usr/local/zlib-cf'
    if [[ "$(uname -m)" = 'x86_64' ]]; then
        ln -sf /usr/local/zlib-cf/lib /usr/local/zlib-cf/lib64
    fi
  fi
else
  ZLIBPHP_OPT='--with-zlib'
fi

# php modules to disable for low memory installs if
# LOWMEM_INSTALL='y' is set 
# https://community.centminmod.com/posts/46321/
if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
  SQLLITEOPT=' --without-sqlite3'
  PDOSQLLITE_OPT=' --without-pdo-sqlite'
else
  PDOSQLLITE_OPT=' --with-pdo-sqlite'
  if [[ -z "$(rpm -qa sqlite-devel)" ]]; then
    yum -q -y install sqlite-devel
  fi
fi

# php 7.2 & 7.3 specific config options
# Argon2 passwords https://wiki.php.net/rfc/argon2_password_hash
# libsodium https://wiki.php.net/rfc/libsodium
# https://github.com/jedisct1/libsodium
# disable temporarily for php 7.2.0 due to libzip compile errors
# https://gist.github.com/centminmod/d1b48ef88bda29a7e352b90fd7111ac1

if [[ "$CENTOS_SIX" = '6' ]] && [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  DEVTOOLSET_PHP='y'
  GCCINTEL_PHP='y'
  # kerberos / krb5 and krb5-gasspi YUM packages on CentOS 6 is missing .pc files
  # required for pkg-config to find location of dependency libraries for PHP 7.4+
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    LIBDIR_KERBEROS='lib64'
    LIBDIR_JPEG='lib64'
    LIBDIR_SASL='lib64'
    LIBDIR_GD='lib64'
  else
    LIBDIR_KERBEROS='lib'
    LIBDIR_JPEG='lib'
    LIBDIR_SASL='lib'
    LIBDIR_GD='lib'
  fi
  export KERBEROS_CFLAGS='-I/usr/include'
  export KERBEROS_LIBS="-L/usr/${LIBDIR_KERBEROS} -lkrb5 -lk5crypto -lgssapi_krb5"
  export JPEG_CFLAGS='-I/usr/include'
  export JPEG_LIBS="-L/usr/${LIBDIR_JPEG} -ljpeg"
  export SASL_CFLAGS='-I/usr/include'
  export SASL_LIBS="-L/usr/${LIBDIR_SASL}"
  export GD_CFLAGS='-I/usr/include'
  export GD_LIBS="-L/usr/${LIBDIR_GD} -lgd"
  # PHP 7.4 sqlite requires >3.7 but centos 6 only has sqlite 3.6.20
  if [[ "$(uname -m)" = 'x86_64' && ! -f "$DIR_TMP/sqlite-3.7.9-1.el6.art.x86_64.rpm" ]]; then
    wget${ipv_forceopt_wget} http://www6.atomicorp.com/channels/atomic/centos/6/x86_64/RPMS/sqlite-3.7.9-1.el6.art.x86_64.rpm -O "$DIR_TMP/sqlite-3.7.9-1.el6.art.x86_64.rpm"
    # yum -y localinstall $DIR_TMP/sqlite-3.7.9-1.el6.art.x86_64.rpm
  elif [[ "$(uname -m)" != 'x86_64' && ! -f "$DIR_TMP/sqlite-3.7.9-1.el6.art.i686.rpm" ]]; then
    wget${ipv_forceopt_wget} http://www6.atomicorp.com/channels/atomic/centos/6/x86_64/RPMS/sqlite-3.7.9-1.el6.art.i686.rpm -O "$DIR_TMP/sqlite-3.7.9-1.el6.art.i686.rpm"
    # yum -y localinstall $DIR_TMP/sqlite-3.7.9-1.el6.art.i686.rpm
  fi
  if [[ "$(uname -m)" = 'x86_64' && ! -f "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm" ]]; then
    wget${ipv_forceopt_wget} http://www6.atomicorp.com/channels/atomic/centos/6/x86_64/RPMS/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm -O "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm"
    # yum -y localinstall $DIR_TMP/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm
  elif [[ "$(uname -m)" != 'x86_64' && ! -f "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.i686.rpm" ]]; then
    wget${ipv_forceopt_wget} http://www6.atomicorp.com/channels/atomic/centos/6/x86_64/RPMS/sqlite-devel-3.7.9-1.el6.art.i686.rpm -O "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.i686.rpm"
    # yum -y localinstall $DIR_TMP/sqlite-devel-3.7.9-1.el6.art.i686.rpm
  fi
  if [[ "$(uname -m)" = 'x86_64' && -f "$DIR_TMP/sqlite-3.7.9-1.el6.art.x86_64.rpm" && -f "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm" ]]; then
    yum -y localinstall $DIR_TMP/sqlite-3.7.9-1.el6.art.x86_64.rpm $DIR_TMP/sqlite-devel-3.7.9-1.el6.art.x86_64.rpm
  fi
  if [[ "$(uname -m)" != 'x86_64' && -f "$DIR_TMP/sqlite-3.7.9-1.el6.art.i686.rpm" && -f "$DIR_TMP/sqlite-devel-3.7.9-1.el6.art.i686.rpm" ]]; then
    yum -y localinstall $DIR_TMP/sqlite-3.7.9-1.el6.art.i686.rpm $DIR_TMP/sqlite-devel-3.7.9-1.el6.art.i686.rpm
  fi
fi

if [[ "$PHPDEBUGMODE" = [yY] ]]; then
  PHPDEBUGOPT=' --enable-debug'
else
  PHPDEBUGOPT=''
fi

# ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.x's inbuilt included Zend Opcache version with one available from pecl site
if [[ -n "$PHPMUVER" && "$PHPMUVER" = 5.[234] || "$PHPMVER" = 5.[234] ]]; then
    OPCACHEOPT=''
elif [[ "$ZOPCACHE_OVERRIDE" = [yY] || "$ZOPCACHEDFT" = [nN] || "$zendopcacheon" = [nN] ]]; then
    OPCACHEOPT=' --disable-opcache'
else
    OPCACHEOPT=' --enable-opcache'
fi

if [[ ! -f /usr/include/readline/readline.h || ! -f /usr/include/editline/readline.h ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    time yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=rpmforge,epel
    sar_call
  else
    time yum${CACHESKIP} -q -y install readline-devel libedit-devel --disablerepo=epel
    sar_call
  fi
fi

# xxhash-devel
if [ ! -f /usr/include/xxhash.h ]; then
  time yum${CACHESKIP} -y install xxhash xxhash-devel
  sar_call
fi

# Microsoft SQL Server PHP driver support
if [[ ! -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]] && [[ ! -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install unixODBC-devel freetds-devel
  sar_call
elif [[ ! -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]] && [[ -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install unixODBC-devel
  sar_call
elif [[ ! -f /usr/include/sqldb.h && "$PHPMSSQL" = [yY] ]] && [[ -f /usr/include/sql.h && "$PHPMSSQL" = [yY] ]]; then
  yum -q -y install freetds-devel
  sar_call
fi

if [[ ! -f /usr/bin/xslt-config ]]; then
  yum -q -y install libxslt libxslt-devel
  sar_call
fi

# for PHP versions lower than 7.1 switch back to system libicu versions for compatibility
if [[ "$PHPMVER" = 5.[23456] ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '7.0' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '7.1' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '7.2' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '7.3' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '7.4' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = 5.[23456] ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.0' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.1' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.2' ]]; then
  NEWLIBICU='n'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.3' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.4' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
fi
if [[ "$PHPMUVER" = '7.4' || "$PHPMUVER" = '8.0' || "$PHPMUVER" = '8.1' || "$PHPMUVER" = '8.2' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  NEWLIBICU='y'
  echo "NEWLIBICU=$NEWLIBICU"
  if [[ "$NEWLIBICU" = [yY] && ! "$(ls /usr/lib64/icu/73* 1> /dev/null 2>&1)" && "$CENTOS_NINE" = '9' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel
    # if [ "$(rpm -qa libicu)" ]; then
    #   yum -y remove libicu
    # fi
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && ! "$(ls /usr/lib64/icu/73* 1> /dev/null 2>&1)" && "$CENTOS_NINE" = '9' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel
    # if [ "$(rpm -qa libicu)" ]; then
    #   yum -y remove libicu
    # fi
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && ! "$(ls /usr/lib64/icu/73* 1> /dev/null 2>&1)" && "$CENTOS_EIGHT" = '8' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel
    # if [ "$(rpm -qa libicu)" ]; then
    #   yum -y remove libicu
    # fi
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && ! "$(ls /usr/lib64/icu/73* 1> /dev/null 2>&1)" && "$CENTOS_EIGHT" = '8' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel
    # if [ "$(rpm -qa libicu)" ]; then
    #   yum -y remove libicu
    # fi
    sar_call
  fi
fi

if [[ ! -f /usr/bin/icu-config ]]; then
  if [[ "$NEWLIBICU" = [yY] && "$CENTOS_NINE" = '9' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && "$CENTOS_NINE" = '9' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && "$CENTOS_EIGHT" = '8' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=rpmforge,epel
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && "$CENTOS_EIGHT" = '8' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel"
    yum -y install libicu73 libicu73-devel --allowerasing --enablerepo=remi --disablerepo=epel
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && "$CENTOS_SEVEN" = '7' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu62 libicu62-devel --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu62 libicu62-devel --enablerepo=remi --disablerepo=rpmforge,epel
    sar_call
  elif [[ "$NEWLIBICU" = [yY] && "$CENTOS_SEVEN" = '7' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu62 libicu62-devel --enablerepo=remi --disablerepo=epel"
    yum -y install libicu62 libicu62-devel --enablerepo=remi --disablerepo=epel
    sar_call
  elif [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/remi.repo && -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu-last libicu-last-devel --enablerepo=remi --disablerepo=rpmforge,epel"
    yum -y install libicu-last libicu-last-devel --enablerepo=remi --disablerepo=rpmforge,epel
    sar_call
  elif [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
    echo "yum -y install libicu-last libicu-last-devel --enablerepo=remi --disablerepo=epel"
    yum -y install libicu-last libicu-last-devel --enablerepo=remi --disablerepo=epel
    sar_call
  elif [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    echo "yum -y install libicu libicu-devel --disablerepo=rpmforge,epel"
    yum -y install libicu libicu-devel --disablerepo=rpmforge,epel
    sar_call
  else
    echo "yum -y install libicu libicu-devel --disablerepo=epel"
    yum -y install libicu libicu-devel --disablerepo=epel
    sar_call
  fi
fi
# echo "NEWLIBICU=$NEWLIBICU PHPMVER=$PHPMVER PHPMUVER=$PHPMUVER sleep 60"
# sleep 60
if [[ "$NEWLIBICU" = [yY] && "$CENTOS_SEVEN" = '7' && -f /etc/yum.repos.d/remi.repo && -f /usr/bin/icu-config && "$(/usr/bin/icu-config --version| cut -d . -f1)" -lt '62' ]]; then
  # update centos 7 libicu 50.1 version to remi yum repo provided libicu 62.1 version
  echo "yum -y install libicu62 --enablerepo=remi"
  yum -y install libicu62 --enablerepo=remi
  echo "yum -y swap libicu-devel libicu62-devel --enablerepo=remi"
  yum -y swap libicu-devel libicu62-devel --enablerepo=remi
fi
# for PHP versions lower than 7.1 switch back to system libicu versions for compatibility
if [[ "$NEWLIBICU" = [nN] && "$CENTOS_SEVEN" = '7' && -f /etc/yum.repos.d/remi.repo && -f /usr/bin/icu-config ]] && [[ "$(/usr/bin/icu-config --version| cut -d . -f1)" -gt '50' ]]; then
  echo "downgrade libicu back from $(/usr/bin/icu-config --version| cut -d . -f1) to earlier version"
  # update centos 7 libicu 50.1 version to remi yum repo provided libicu 62.1 version
  if [ "$(rpm -ql libicu62)" ]; then
    echo "yum -y remove libicu62 --enablerepo=remi"
    yum -y remove libicu62 --enablerepo=remi
    if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      echo "yum -q -y install libicu-devel --disablerepo=rpmforge,epel"
      yum -y install libicu-devel --disablerepo=rpmforge,epel
    else
      echo "yum -y install libicu-devel --disablerepo=epel"
      yum -y install libicu-devel --disablerepo=epel
    fi
  fi
  if [ "$(rpml libicu62-devel)" ]; then
    echo "yum -y swap libicu62-devel libicu-devel --enablerepo=remi"
    yum -y swap libicu62-devel libicu-devel --enablerepo=remi
  else
    if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      echo "yum -y install libicu-devel --disablerepo=rpmforge,epel"
      yum -y install libicu-devel --disablerepo=rpmforge,epel
    else
      echo "yum -y install libicu-devel --disablerepo=epel"
      yum -y install libicu-devel --disablerepo=epel
    fi
  fi
fi
if [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/remi.repo && -f /usr/bin/icu-config && "$(/usr/bin/icu-config --version| cut -d . -f1)" -lt '50' ]]; then
  # update centos 7 libicu 50.1 version to remi yum repo provided libicu 62.1 version
  echo "yum -y install libicu-last --enablerepo=remi"
  yum -y install libicu-last --enablerepo=remi
  echo "yum -y remove libicu-devel"
  yum -y remove libicu-devel
  echo "yum -y install libicu-last-devel --enablerepo=remi"
  yum -y install libicu-last-devel --enablerepo=remi
fi

if [[ "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    LIBDIR_LIBICU='lib64'
  else
    LIBDIR_LIBICU='lib'
  fi
  export ICU_CFLAGS='-I/usr/include'
  export ICU_LIBS="-L/usr/${LIBDIR_LIBICU} -licuio -licui18n -licuuc -licudata"
fi

if [[ "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' ]]; then
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    LIBDIR_LIBICU='lib64'
  else
    LIBDIR_LIBICU='lib'
  fi
  if [[ "$CENTOS_NINE" = '9' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
      echo "yum -q -y install libicu libicu-devel --allowerasing --enablerepo=remi --disablerepo=epel"
      yum -y install libicu libicu-devel --allowerasing --enablerepo=remi --disablerepo=epel
      if [[ ! -f /usr/bin/icu-config ]]; then
        yum -y install libicu-devel --enablerepo=remi --disablerepo=epel
      fi
      sar_call
  elif [[ "$CENTOS_EIGHT" = '8' && -f /etc/yum.repos.d/remi.repo && ! -f /etc/yum.repos.d/rpmforge.repo ]]; then
      echo "yum -y install libicu libicu-devel --allowerasing --enablerepo=remi --disablerepo=epel"
      yum -y install libicu libicu-devel --allowerasing --enablerepo=remi --disablerepo=epel
      if [[ ! -f /usr/bin/icu-config ]]; then
        yum -y install libicu-devel --enablerepo=remi --disablerepo=epel
      fi
      sar_call
  fi
  export ICU_CFLAGS='-I/usr/include'
  export ICU_LIBS="-L/usr/${LIBDIR_LIBICU} -licuio -licui18n -licuuc -licudata"
fi

if [[ ! -f /usr/bin/pspell-config ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
   yum -q -y install aspell aspell-devel --disablerepo=rpmforge,epel
   sar_call
  else
    yum -q -y install aspell aspell-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/recode.h ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
   yum -q -y install recode recode-devel --disablerepo=rpmforge,epel
   sar_call
  else
    yum -q -y install recode recode-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/tidy.h ]]; then
  yum -q -y install libtidy libtidy-devel --disablerepo=rpmforge
  sar_call
fi

if [[ ! -d /usr/include/enchant ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install enchant enchant-devel --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install enchant enchant-devel --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -d /usr/include/net-snmp ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install net-snmp net-snmp-devel net-snmp-libs net-snmp-utils --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install net-snmp net-snmp-devel net-snmp-libs net-snmp-utils --disablerepo=epel
    sar_call
  fi
fi

if [[ ! -f /usr/include/expect.h && "$CENTOS_NINE" -ne '9' ]]; then
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    yum -q -y install expect-devel --disablerepo=rpmforge,epel
    sar_call
  else
    yum -q -y install expect-devel --disablerepo=epel
    sar_call
  fi
fi

if [ "$PHPEMBED" == 'y' ]; then
  PHPEMBEDOPT=" --enable-embed=shared"
else
  PHPEMBEDOPT=""
fi

if [ "$PHPMSSQL" == 'y' ]; then
  MSSQLOPT=" --with-pdo-odbc=unixODBC,/usr"
else
  MSSQLOPT=""
fi

if [[ -f /usr/include/ldap.h ]]; then
  LDAPOPT=' --with-ldap --with-ldap-sasl'
else
  LDAPOPT=""
fi

if [[ "$PHPMVER" > 7 || "$PHPMUVER" > 7 ]]; then
  if [[ "$(rpm -ql libwebp | grep libwebp.so >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
      LIBVPXOPT=' --with-webp'
    else
      LIBVPXOPT=' --with-webp-dir=/usr'
    fi
  else
    time yum${CACHESKIP} -y install libwebp libwebp-devel
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
      LIBVPXOPT=' --with-webp'
    else
      LIBVPXOPT=' --with-webp-dir=/usr'
    fi
  fi
elif [[ "$PHPMVER" = 5.[56] || "$PHPMUVER" = 5.[56] ]] && [[ "$PHPMVER" < 7 || "$PHPMUVER" < 7 ]]; then
  if [[ -d /usr/include/vpx ]]; then
   LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
  else
   if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
      time yum${CACHESKIP} -y install libvpx libvpx-devel --disablerepo=rpmforge,epel
      sar_call
    else
      time yum${CACHESKIP} -y install libvpx libvpx-devel --disablerepo=epel
      sar_call
    fi
   LIBVPXOPT=" --with-vpx-dir=${PLIBDIR}"
  fi
else
  LIBVPXOPT=''
fi

if [[ -f /usr/include/t1lib.h ]]; then
  TLIBOPT=" --with-t1lib=${PLIBDIR}"
else
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    time yum${CACHESKIP} -y install t1lib t1lib-devel --disablerepo=rpmforge,epel
    sar_call
  else
    time yum${CACHESKIP} -y install t1lib t1lib-devel --disablerepo=epel
    sar_call
  fi
  TLIBOPT=" --with-t1lib=${PLIBDIR}"
fi
 
if [ "$PHPPCNTL" == 'y' ]; then
  PCNTLOPT=" --enable-pcntl"
else
  PCNTLOPT=""
fi

if [ "$PHPINTL" == 'y' ]; then
  PHPINTLOPT=" --enable-intl"
else
  PHPINTLOPT=""
fi

if [ "$PHPRECODE" == 'y' ]; then
  RECODEOPT=" --with-recode"
else
  RECODEOPT=""
fi

if [ "$PHPSNMP" == 'y' ]; then
  SNMPOPT=" --with-snmp"
else
  SNMPOPT=""
fi

if [ "$PHP_FTPEXT" == 'y' ]; then
  FTPEXTOPT="  --enable-ftp"
else
  FTPEXTOPT=""
fi

# let centmin.sh set PHPFINFO variable override the
# low memory CHECKLOWMEMPHP variable check
if [[ "$PHPFINFO_STANDALONE" = [yY] ]]; then
  PHPFINFO='y'
fi
if [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" = "1" && "$TOTALSWAPMEM" -ge '1023000' && "$PHPFINFO_STANDALONE" = [yY] ]]; then
  # PHPFINFO yes + low mem install + swap space enough = enable file info php extension
  FILEINFOOPT=" --disable-fileinfo"
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" = "1" && "$TOTALSWAPMEM" -ge '1023000' && "$PHPFINFO_STANDALONE" = [nN] ]]; then
  # PHPFINFO yes + low mem install + swap space enough = enable file info php extension
  FILEINFOOPT=""
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" != "1" && "$PHPFINFO_STANDALONE" = [yY] ]]; then
  # PHPFINFO yes + enough mem install = enable file info php extension
  FILEINFOOPT=" --disable-fileinfo"
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" != "1" && "$PHPFINFO_STANDALONE" = [nN] ]]; then
  # PHPFINFO yes + enough mem install = enable file info php extension
  FILEINFOOPT=""
elif [[ "$PHPFINFO" = [yY] ]] && [[ "$CHECKLOWMEMPHP" = "1" && "$TOTALSWAPMEM" -lt '1023000' ]]; then
  # PHPFINFO yes + low mem install + low swap space = disable file info php extension
  FILEINFOOPT=" --disable-fileinfo"
elif [[ "$PHPFINFO" = [nN] ]]; then
  # PHPFINFO no
  FILEINFOOPT=" --disable-fileinfo"
fi

if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_SIX" = '6' ]]; then

  if [[ "$POSTGRESQL" = [yY] ]]; then
    # to allow to persist on centmin mod code updates
    # create persistent config file at /etc/centminmod/custom_config.inc
    # and add to it the line:
    # POSTGRESQL=y
    postgresqlinstall
    PDOPGSQL_OPT=" --with-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER} --with-pdo-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER}"
  else
    PDOPGSQL_OPT=""
  fi

  if [ ! -f /usr/bin/libmcrypt-config ]; then

    # double check mcrypt has been installed
    CHECKMCRYPT='libmcrypt libmcrypt-devel'
    
    declare -a arraysm
    arraysm=(${CHECKMCRYPT})
    
    for pkg in ${arraysm[@]}
    do
    echo Processing package: $pkg
    MCRYPTRPMCHECKS=`yum${CACHESKIP} -q list installed $pkg | awk '/Installed/ {print $1}' &>/dev/null`
    #echo $MCRYPTRPMCHECKS
            if [ "$MCRYPTRPMCHECKS" != 'Installed' ]; then
    echo "----------------------------------------------------------------------------------"
    cecho "Installing $pkg" $boldyellow
    echo "----------------------------------------------------------------------------------"
            time $YUMDNFBIN${CACHESKIP} -q -y install $pkg${DISABLEREPO_DNF}
            # time $YUMDNFBIN${CACHESKIP} -q list $pkg
    echo "----------------------------------------------------------------------------------"
            fi
    done
  fi

  if [[ ! -f /usr/bin/xml2-config ]]; then

    # double check libxml2 has been installed
    CHECKXML='libxml2-devel'
    
    declare -a arraysxml
    arraysxml=(${CHECKXML})
    
    for pkgx in ${arraysxml[@]}
    do
    echo Processing package: $pkgx
    XMLRPMCHECKS=`yum${CACHESKIP} -q list installed $pkgx | awk '/Installed/ {print $1}' &>/dev/null`
    #echo $XMLRPMCHECKS
            if [ "$XMLRPMCHECKS" != 'Installed' ]; then
    echo "----------------------------------------------------------------------------------"
    cecho "Installing $pkgx" $boldyellow
    echo "----------------------------------------------------------------------------------"
            time $YUMDNFBIN${CACHESKIP} -q -y install $pkgx${DISABLEREPO_DNF}
            # time $YUMDNFBIN${CACHESKIP} -q list $pkgx
    echo "----------------------------------------------------------------------------------"
            fi
    done
  fi  

fi

# set_intelflags
if [[ "$(grep -c "processor" /proc/cpuinfo)" -lt '2' ]] && [[ "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  # Enable faster clang compiler for PHP if less than 2 cpu threads
  # CLANG_PHP='y'

  # Disable PGO if less than 2 cpu threads
  PHP_PGO='n'

  # Disable php intel optimisation flags if less than 2 cpu threads
  # detected due to being very very slow for php compile times
  GCCINTEL_PHP='y'
elif [[ "$(grep -c "processor" /proc/cpuinfo)" -lt '2' ]]; then
  # Enable faster clang compiler for PHP if less than 2 cpu threads
  # CLANG_PHP='y'

  # Disable PGO if less than 2 cpu threads
  PHP_PGO='n'

  # Disable php intel optimisation flags if less than 2 cpu threads
  # detected due to being very very slow for php compile times
  GCCINTEL_PHP='n'
elif [[ "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  # Enable faster clang compiler for PHP if less than 2 cpu threads
  # CLANG_PHP='y'

  # Disable PGO if less than 2 cpu threads
  # PHP_PGO='n'

  # Disable php intel optimisation flags if less than 2 cpu threads
  # detected due to being very very slow for php compile times
  GCCINTEL_PHP='y'
fi
if [[ "$PHP_PGO_ALWAYS" = [yY] ]]; then
  # override above greater than 2 cpu only PHP PGO enabling
  # so when PHP_PGO_ALWAYS='y' set, you can even do PHP PGO
  # compilations when you have 1 cpu detected
  PHP_PGO='y'
fi

if [[ "$CLANG_PHP" = [yY] ]]; then
    if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/clang ]] || [[ "$CENTOS_SEVEN" = '7' && ! -f /bin/clang ]]; then
        time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
    fi
    if [[ "$GCCINTEL_PHP" = [yY] ]]; then
      CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
      if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]]; then
        CCM=64
        PHP_MTUNEOPT="-m${CCM} -mtune=native"
      else
        CCM=""
        PHP_MTUNEOPT=""
      fi
    fi
  # ccache compiler has some initial overhead for compiles but speeds up subsequent
  # recompiles. however on initial install ccache has no benefits, so for initial
  # centmin mod install disabling ccache will in theory speed up first time installs
  if [[ "$INITIALINSTALL" != [yY] ]]; then    
      export CC="ccache /usr/bin/clang ${PHP_MTUNEOPT}"
      export CXX="ccache /usr/bin/clang++ ${PHP_MTUNEOPT}"
      export CCACHE_CPP2=yes
      # PLACEHOLDER FOR CLAGS FOR PHP
      # CFLAGS=
    else
      export CC="/usr/bin/clang ${PHP_MTUNEOPT}"
      export CXX="/usr/bin/clang++ ${PHP_MTUNEOPT}"
      # export CCACHE_CPP2=yes
      # PLACEHOLDER FOR CLAGS FOR PHP
      # CFLAGS=
    fi      
else
  # ccache compiler has some initial overhead for compiles but speeds up subsequent
  # recompiles. however on initial install ccache has no benefits, so for initial
  # centmin mod install disabling ccache will in theory speed up first time installs
  if [[ "$INITIALINSTALL" != [yY] ]]; then
      export CC="ccache gcc"
      export CXX="ccache g++"
      export CCACHE_CPP2=yes
      # PLACEHOLDER FOR CLAGS FOR PHP
      # CFLAGS=
  else
      export CC="gcc"
      export CXX="g++"
      # export CCACHE_CPP2=yes
      # PLACEHOLDER FOR CLAGS FOR PHP
      # CFLAGS=
    fi
fi

if [[ "$GCCINTEL_PHP" = [yY] ]]; then
  CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')
  # ensure devtoolset-7 and devtoolset-8 and devtoolset-9 are installed

  if [[ "$CENTOSVER_NUMERIC" -ge '89' && "$CENTOS_EIGHT" -eq '8' ]] && [[ "$DEVTOOLSETTHIRTEEN" = [yY] && ! -f /opt/rh/gcc-toolset-13/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-13/root/usr/bin/g++ ]]; then
      yum -y install gcc-toolset-13-annobin-annocheck gcc-toolset-13-annobin-docs.noarch gcc-toolset-13-annobin-plugin-gcc gcc-toolset-13-binutils gcc-toolset-13-binutils-devel gcc-toolset-13-dwz gcc-toolset-13-gcc gcc-toolset-13-gcc-c++ gcc-toolset-13-gcc-gfortran gcc-toolset-13-gdb gcc-toolset-13-libasan-devel gcc-toolset-13-libatomic-devel gcc-toolset-13-libgccjit gcc-toolset-13-libgccjit-devel gcc-toolset-13-libgccjit-docs gcc-toolset-13-libitm-devel gcc-toolset-13-liblsan-devel gcc-toolset-13-libquadmath-devel gcc-toolset-13-libstdc++-devel gcc-toolset-13-libtsan-devel gcc-toolset-13-libubsan-devel gcc-toolset-13-runtime
  fi
  if [[ "$CENTOSVER_NUMERIC" -ge '87' && "$CENTOS_EIGHT" -eq '8' ]] && [[ "$DEVTOOLSETTWELVE" = [yY] && ! -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
      yum -y install gcc-toolset-12-annobin-annocheck gcc-toolset-12-annobin-docs.noarch gcc-toolset-12-annobin-plugin-gcc gcc-toolset-12-binutils gcc-toolset-12-binutils-devel gcc-toolset-12-dwz gcc-toolset-12-gcc gcc-toolset-12-gcc-c++ gcc-toolset-12-gcc-gfortran gcc-toolset-12-gdb gcc-toolset-12-libasan-devel gcc-toolset-12-libatomic-devel gcc-toolset-12-libgccjit gcc-toolset-12-libgccjit-devel gcc-toolset-12-libgccjit-docs gcc-toolset-12-libitm-devel gcc-toolset-12-liblsan-devel gcc-toolset-12-libquadmath-devel gcc-toolset-12-libstdc++-devel gcc-toolset-12-libtsan-devel gcc-toolset-12-libubsan-devel gcc-toolset-12-runtime
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' ]] && [[ "$DEVTOOLSETELEVEN" = [yY] && ! -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
      yum -y install gcc-toolset-11-runtime gcc-toolset-11-toolchain gcc-toolset-11-binutils gcc-toolset-11-binutils-devel gcc-toolset-11-gcc gcc-toolset-11-gcc-c++ gcc-toolset-11-libatomic-devel gcc-toolset-11-libstdc++-devel gcc-toolset-11-ltrace gcc-toolset-11-make gcc-toolset-11-make-devel gcc-toolset-11-annobin-plugin-gcc gcc-toolset-11-annobin-annocheck gcc-toolset-11-gdb gcc-toolset-11-dwz gcc-toolset-11-elfutils gcc-toolset-11-elfutils-devel gcc-toolset-11-elfutils-libs gcc-toolset-11-strace gcc-toolset-11-gcc-gfortran gcc-toolset-11-elfutils-libelf gcc-toolset-11-elfutils-libelf-devel gcc-toolset-11-libasan-devel gcc-toolset-11-libgccjit-devel gcc-toolset-11-libitm-devel gcc-toolset-11-liblsan-devel gcc-toolset-11-libquadmath-devel gcc-toolset-11-libtsan-devel gcc-toolset-11-libubsan-devel gcc-toolset-11-elfutils-debuginfod-client gcc-toolset-11-elfutils-debuginfod-client-devel gcc-toolset-11-elfutils-libs gcc-toolset-11-annobin-docs gcc-toolset-11-libgccjit gcc-toolset-11-libgccjit-devel gcc-toolset-11-libgccjit-docs
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' ]] && [[ "$DEVTOOLSETTEN" = [yY] && ! -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
      yum -y install gcc-toolset-10-binutils gcc-toolset-10-gcc gcc-toolset-10-gcc-c++
  fi
  if [[ "$CENTOS_EIGHT" -eq '8' ]] && [[ "$DEVTOOLSETNINE" = [yY] && ! -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
      yum -y install gcc-toolset-9-binutils gcc-toolset-9-gcc gcc-toolset-9-gcc-c++
  fi
  if [[ "$CENTOSVER_NUMERIC" -ge '87' && "$DEVTOOLSETTWELVE" = [yY] && ! -f /opt/rh/devtoolset-12/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-12/root/usr/bin/g++ ]]; then
      scl_install
  elif [[ "$DEVTOOLSETELEVEN" = [yY] && ! -f /opt/rh/devtoolset-11/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
      scl_install
  elif [[ "$DEVTOOLSETTEN" = [yY] && ! -f /opt/rh/devtoolset-10/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
      scl_install
  elif [[ "$DEVTOOLSETNINE" = [yY] && ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
      scl_install
  elif [[ "$DEVTOOLSETEIGHT" = [yY] && ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
      scl_install
  elif [[ "$DEVTOOLSETSEVEN" = [yY] && ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
      scl_install
  fi

  # PGO PHP7 on CentOS 6 might have GCC too old 4.4.7 so use devtoolset=3 GCC 4.9
  # initial php install
  if [[ "$PHP_PGO_CENTOSSIX" = [yY] && "$PHP_PGO" = [yY] && "$PHPMVER" > 7 ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMVER" = '8.0' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMVER" = '7.4' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMVER" = '7.3' ]] || [[ "$CENTOS_SIX" = '6' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-9/root/usr/bin/gcc || -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
      elif [[ ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-8/root/usr/bin/gcc || -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
      elif [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      fi
    fi
  fi

  # php upgrades
  if [[ "$PHP_PGO_CENTOSSIX" = [yY] && "$PHP_PGO" = [yY] && "$PHPMUVER" > 7 ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMUVER" = '8.0' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMUVER" = '7.4' ]] || [[ "$CENTOS_SIX" = '6' && "$PHPMUVER" = '7.3' ]] || [[ "$CENTOS_SIX" = '6' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-9/root/usr/bin/gcc || -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
      elif [[ ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-8/root/usr/bin/gcc || -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
      elif [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        scl_install
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        scl_install
        which gcc
        which g++
        unset CC
        unset CXX
        export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
        export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
      fi
    fi
  fi

  # CentOS 8
  # initial php install
  if [[ "$CENTOS_EIGHT" = '8' && "$PHP_PGO" = [yY] && "$PHPMVER" > 7 ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '8.3' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '8.2' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '8.1' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '8.0' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '7.4' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMVER" = '7.3' ]] || [[ "$CENTOS_EIGHT" = '8' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$CENTOSVER_NUMERIC" -ge '89' && "$DEVTOOLSETTHIRTEEN" = [yY] && -f /opt/rh/gcc-toolset-13/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-13/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-13/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-13/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-13/root/usr/bin/g++"
        fi
        echo "EL 8.9 set gcc-toolset-13"
        echo $CC
        echo $CXX
    elif [[ "$CENTOSVER_NUMERIC" -ge '87' && "$DEVTOOLSETTWELVE" = [yY] && -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-12/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-12/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-12/root/usr/bin/g++"
        fi
        echo "EL 8.7 set gcc-toolset-12"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-11/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-11/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-11/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-11/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-11/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-11"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-10/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-10/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-10/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-10/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-10/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-10"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-9/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-9/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-9/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-9/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-9"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETEIGHT" = [yY] && "$CENTOS_EIGHT" = '8' ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        which gcc
        which g++
        unset CC
        unset CXX
        echo $CC
        echo $CXX
    fi
  fi

  # CentOS 7 use devtoolset-4 GCC 5.2 if it's detected
  # initial php install
  if [[ "$CENTOS_SEVEN" = '7' && "$PHP_PGO" = [yY] && "$PHPMVER" > 7 ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '8.3' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '8.2' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '8.1' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '8.0' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '7.4' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMVER" = '7.3' ]] || [[ "$CENTOS_SEVEN" = '7' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-11/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-11/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-11/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-11/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-11/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-11"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-array-bounds -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-10/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-10/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-10/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-10/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-10/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-10"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc9/enable
      which gcc
      which g++
      unset CC
      unset CXX
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        export CC="gcc"
        export CXX="g++"
      fi
    elif [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        export CC="gcc"
        export CXX="g++"
      fi
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-9/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-9/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-9"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-8/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-8/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-8"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-7"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-6"
        echo $CC
        echo $CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-4/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-4/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-4"
        echo $CC
        echo $CXX
    fi
  fi

  # php upgrades
  if [[ "$CENTOS_EIGHT" = '8' && "$PHP_PGO" = [yY] && "$PHPMUVER" > 7 ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '8.3' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '8.2' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '8.1' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '8.0' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '7.4' ]] || [[ "$CENTOS_EIGHT" = '8' && "$PHPMUVER" = '7.3' ]] || [[ "$CENTOS_EIGHT" = '8' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$CENTOSVER_NUMERIC" -ge '89' && "$DEVTOOLSETTHIRTEEN" = [yY] && -f /opt/rh/gcc-toolset-13/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-13/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-13/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-13/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-13/root/usr/bin/g++"
        fi
        echo "EL 8.9 set gcc-toolset-13"
        echo $CC
        echo $CXX
    elif [[ "$CENTOSVER_NUMERIC" -ge '87' && "$DEVTOOLSETTWELVE" = [yY] && -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-12/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-12/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-12/root/usr/bin/g++"
        fi
        echo "EL 8.7 set gcc-toolset-12"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-11/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-11/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-11/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-11/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-11/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-11"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-10/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-10/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-10/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-10/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-10/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-10"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        source /opt/rh/gcc-toolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-9/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-9/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-9/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-9/root/usr/bin/g++"
        fi
        echo "centos 8 set gcc-toolset-9"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETEIGHT" = [yY] && "$CENTOS_EIGHT" = '8' ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch'
        which gcc
        which g++
        unset CC
        unset CXX
        echo $CC
        echo $CXX
    fi
  fi

  # php install & upgrades for el9's GCC 11 defaults
  if [[ "$CENTOS_NINE" -eq '9' ]]; then
    CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')
    if [[ "$CENTOSVER_NUMERIC" -ge '93' && "$DEVTOOLSETTHIRTEEN" = [yY] && -f /opt/rh/gcc-toolset-13/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-13/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-13/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-13/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-13/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-13/root/usr/bin/g++"
        fi
        echo "EL 9.2 set gcc-toolset-13"
        echo $CC
        echo $CXX
    elif [[ "$CENTOSVER_NUMERIC" -ge '91' && "$DEVTOOLSETTWELVE" = [yY] && -f /opt/rh/gcc-toolset-12/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-12/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-return-local-addr -Wno-maybe-uninitialized -Wno-array-parameter -Wno-alloc-size-larger-than -Wno-address -Wno-array-bounds -Wno-deprecated-declarations -Wno-discarded-qualifiers -Wno-stringop-overread -Wno-stringop-truncation'
        source /opt/rh/gcc-toolset-12/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/gcc-toolset-12/root/usr/bin/g++"
        else
          export CC="/opt/rh/gcc-toolset-12/root/usr/bin/gcc"
          export CXX="/opt/rh/gcc-toolset-12/root/usr/bin/g++"
        fi
        echo "EL 9.1 set gcc-toolset-12"
        echo $CC
        echo $CXX
    else
        CENTOSVER_NUMERIC=$(echo $CENTOSVER | sed -e 's|\.||g')
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        which gcc
        which g++
        unset CC
        unset CXX
        echo "el9 set default GCC 11 settings for PHP compilations"
        echo $CC
        echo $CXX
    fi
  fi

  if [[ "$CENTOS_SEVEN" = '7' && "$PHP_PGO" = [yY] && "$PHPMUVER" > 7 ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '8.3' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '8.2' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '8.1' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '8.0' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPVER_ID" -ge '80000' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '7.4' ]] || [[ "$CENTOS_SEVEN" = '7' && "$PHPMUVER" = '7.3' ]] || [[ "$CENTOS_SEVEN" = '7' && "$DEVTOOLSET_PHP" = [yY] ]]; then
    # if devtoolset-4 gcc and g++ exist use then instead of system versions regardless of CentOS version
    if [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-11/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-11/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-11/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-11/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-11/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-11"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-array-bounds -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-10/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-10/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-10/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-10/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-10/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-10"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc9/enable
      which gcc
      which g++
      unset CC
      unset CXX
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        export CC="gcc"
        export CXX="g++"
      fi
    elif [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        export CC="ccache gcc"
        export CXX="ccache g++"
      else
        export CC="gcc"
        export CXX="g++"
      fi
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-9/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-9/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-9/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-9"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-8/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-8/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-8/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-8"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
        GENERAL_DEVTOOLSETGCC='y'
        DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
        DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
        source /opt/rh/devtoolset-7/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-7/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-7/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-7"
        echo $CC
        echo $CXX
    elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-6/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-6"
        echo $CC
        echo $CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
        source /opt/rh/devtoolset-4/enable
        which gcc
        which g++
        unset CC
        unset CXX
        if [[ "$INITIALINSTALL" != [yY] ]]; then
          export CC="ccache /opt/rh/devtoolset-4/root/usr/bin/gcc"
          export CXX="ccache /opt/rh/devtoolset-4/root/usr/bin/g++"
        else
          export CC="/opt/rh/devtoolset-4/root/usr/bin/gcc"
          export CXX="/opt/rh/devtoolset-4/root/usr/bin/g++"
        fi
        echo "centos 7 set devtoolset-4"
        echo $CC
        echo $CXX
    fi
  fi

  if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_LTO" = [yY] ]]; then
    if [[ -f /opt/rh/devtoolset-11/enable && "$DEVTOOLSETELEVEN" = [yY] ]] && [[ "$DEVTOOLSETTEN" = [yY] && "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETELEVEN='y'
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'    
    elif [[ -f /opt/rh/gcc-toolset-11/enable && "$DEVTOOLSETELEVEN" = [yY] ]] && [[ "$DEVTOOLSETTEN" = [yY] && "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETTEN='y'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ -f /opt/rh/gcc-toolset-10/enable && "$DEVTOOLSETTEN" = [yY] ]] && [[ "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETTEN='y'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ -f /opt/rh/gcc-toolset-9/enable && "$DEVTOOLSETNINE" = [yY] ]] && [[ "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='y'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ -f /opt/rh/devtoolset-10/enable && "$DEVTOOLSETTEN" = [yY] ]] && [[ "$DEVTOOLSETNINE" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETTEN='y'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ -f /opt/gcc9/enable && "$DEVTOOLSETNINE" = [yY] ]] && [[ "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETELEVEN='n'
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='y'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ ! -f /opt/gcc9/enable || "$DEVTOOLSETNINE" != [yY] ]] && [[ "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETELEVEN='n'
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='y'
      DEVTOOLSETSEVEN='n'
    elif [[ -f /opt/rh/devtoolset-9/enable && "$DEVTOOLSETNINE" = [yY] ]] && [[ "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETELEVEN='n'
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='y'
      DEVTOOLSETEIGHT='n'
      DEVTOOLSETSEVEN='n'
    elif [[ ! -f /opt/rh/devtoolset-9/enable || "$DEVTOOLSETNINE" != [yY] ]] && [[ "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETELEVEN='n'
      DEVTOOLSETTEN='n'
      DEVTOOLSETNINE='n'
      DEVTOOLSETEIGHT='y'
      DEVTOOLSETSEVEN='n'
    elif [[ "$DEVTOOLSETEIGHT" = [nN] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
      DEVTOOLSETEIGHT='y'
      DEVTOOLSETSEVEN='n'
    else
      DEVTOOLSETEIGHT='y'
      DEVTOOLSETSEVEN='y'
    fi
    if [[ "$DEVTOOLSETNINE" = [yY] ]] && [[ -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc9/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    elif [[ "$DEVTOOLSETEIGHT" = [yY] ]] && [[ -f /opt/gcc8/bin/gcc && -f /opt/gcc8/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
      DEVTOOLSETPHP_EXTRAFLAGS=" -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch"
      source /opt/gcc8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    elif [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/devtoolset-11/root/usr/bin/gcc && -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/devtoolset-11/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-11/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-11/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETELEVEN" = [yY] && -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/gcc-toolset-11/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/gcc-toolset-11/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/gcc-toolset-11/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/gcc-toolset-10/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/gcc-toolset-10/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/gcc-toolset-10/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc && -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/gcc-toolset-9/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/gcc-toolset-9/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/gcc-toolset-9/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETTEN" = [yY] && -f /opt/rh/devtoolset-10/root/usr/bin/gcc && -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-array-bounds -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/devtoolset-10/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-10/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-10/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETNINE" = [yY] && -f /opt/rh/devtoolset-9/root/usr/bin/gcc && -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=' -Wimplicit-fallthrough=0 -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-result -Wno-missing-profile'
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/devtoolset-9/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-9/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-9/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETEIGHT" = [yY] && -f /opt/rh/devtoolset-8/root/usr/bin/gcc && -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/devtoolset-8/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-8/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-8/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETSEVEN" = [yY] && -f /opt/rh/devtoolset-7/root/usr/bin/gcc && -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
      GENERAL_DEVTOOLSETGCC='y'
      DEVTOOLSETPHP_FALLTHROUGH=" -Wimplicit-fallthrough=0"
      DEVTOOLSETPHP_EXTRAFLAGS=' -fcode-hoisting'
      source /opt/rh/devtoolset-7/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-7/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-7/root/usr/bin/g++"
    elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
      source /opt/rh/devtoolset-6/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache /opt/rh/devtoolset-6/root/usr/bin/gcc"
      export CXX="ccache /opt/rh/devtoolset-6/root/usr/bin/g++"
      echo "centos 7 set devtoolset-6"
      echo $CC
      echo $CXX
    fi
  fi

  # intel specific
  CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
  SSECHECK=$(gcc -c -Q -march=native --help=target | awk '/  -msse/ {print $2}' | head -n1)
  CPU_MARCH=$(gcc -c -Q -march=native --help=target | awk '/  -march/ {print $2}' | head -n1 )
  CPUMODELNO=$(grep -v 'model name' /proc/cpuinfo | awk -F ": " '/model/ {print $2}' | uniq)
  CPUSTEPPING=$(awk -F ": " '/stepping/ {print $2}' /proc/cpuinfo | uniq)
  SSEFOURTWOCHECK=$(grep -o sse4_2 /proc/cpuinfo | sort -u | wc -l)
  MARCHCHECK=$(gcc -c -Q -march=native --help=target | awk '/  -march=/ {print $2}' | head -n1)
  # gcc --version | tee ${CENTMINLOGDIR}/gcc_php_native.log
  # gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_php_native.log

  echo
  echo "gcc -c -Q -march=native${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_native.log"
  gcc -c -Q -march=native${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_native.log
  echo
  echo "gcc -c -Q -march=${march_flag}${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_x86-64.log"
  gcc -c -Q -march=${march_flag}${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_x86-64.log
  echo
  diff -y --suppress-common-lines ${CENTMINLOGDIR}/gcc_php_native.log ${CENTMINLOGDIR}/gcc_php_x86-64.log | tee ${CENTMINLOGDIR}/gcc_php_diff_native-vs-x86-64.log
  echo
  egrep '\-march=|\-mtune|\-mavx|\-msse4|\-mpclmul' ${CENTMINLOGDIR}/gcc_php_diff_native-vs-x86-64.log
  # if non native target x86-64 is used 
  # pick up important native cpu instructions
  # and manually pass them to x86-64 target
  if [[ "$MARCH_TARGETNATIVE" = [yY] ]]; then
    GCC_NONNATIVEFLAGS=""
  else
    GCC_NONNATIVEFLAGS=" $(egrep '\-march=|\-mtune|\-mavx|\-msse4|\-mpclmul' ${CENTMINLOGDIR}/gcc_php_native.log | egrep -v 'march|mtune|avx512' | grep enabled | sed -e 's|\[||g' -e 's|\]||g' -e 's|enabled||g'| xargs)"
    echo
    echo "gcc -c -Q -march=${march_flag}${GCC_NONNATIVEFLAGS}${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_x86-64-non-native.log"
    gcc -c -Q -march=${march_flag}${GCC_NONNATIVEFLAGS}${PHPEXTRA_CFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_php_x86-64-non-native.log
  fi
  echo
  echo "GCC_NONNATIVEFLAGS=$GCC_NONNATIVEFLAGS"
    echo

  # for PHP_LTO='y'
  if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_GSPLITDWARF" = [yY] && "$PHP_LTO" = [yY] ]]; then
    # remove  -gsplit-dwarf when PHP_LTO='y' seems *.dwo files aren't cleaned from /tmp with it enabled
    SET_PHPEXTRA_CFLAGS=" -flto -fno-fat-lto-objects -fuse-ld=gold -fuse-linker-plugin -flto-partition=max -Wno-deprecated-declarations${DEVTOOLSETPHP_FALLTHROUGH}${DEVTOOLSETPHP_EXTRAFLAGS}${PHPEXTRA_CFLAGS}"
    PHPLTOOPT=' --disable-gcc-global-regs'
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_GSPLITDWARF" = [yY] && "$PHP_LTO" = [nN] ]]; then
    SET_PHPEXTRA_CFLAGS=" -gsplit-dwarf -Wno-deprecated-declarations${PHP_MOLDOPT}${DEVTOOLSETPHP_FALLTHROUGH}${DEVTOOLSETPHP_EXTRAFLAGS}${PHPEXTRA_CFLAGS}"
    PHPLTOOPT=""
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_GSPLITDWARF" = [nN] && "$PHP_LTO" = [nN] ]]; then
    SET_PHPEXTRA_CFLAGS=" -Wno-deprecated-declarations${PHP_MOLDOPT}${DEVTOOLSETPHP_FALLTHROUGH}${DEVTOOLSETPHP_EXTRAFLAGS}${PHPEXTRA_CFLAGS}"
    PHPLTOOPT=""
  elif [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -ge '40805' && "$PHP_LTO" = [yY] ]]; then
    SET_PHPEXTRA_CFLAGS=" -flto -fno-fat-lto-objects -fuse-ld=gold -fuse-linker-plugin -flto-partition=max -Wno-deprecated-declarations${DEVTOOLSETPHP_FALLTHROUGH}${DEVTOOLSETPHP_EXTRAFLAGS}${PHPEXTRA_CFLAGS}"
    PHPLTOOPT=' --disable-gcc-global-regs'
  fi

  if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1-3 | sed "s|\.|0|g")" -gt '40805' && "$GCC_FALIGN_FUNCTION" = [yY] ]]; then
      FALIGN_FUNCTION_OPT=' -falign-functions=32'
    else
      FALIGN_FUNCTION_OPT=''
    fi
    if [[ "$MARCH_TARGETNATIVE" = [nN] ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${GCC_NONNATIVEFLAGS}"
    elif [[ "$CPU_MARCH" = 'silvermont' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=silvermont"
      echo
      echo "gcc -c -Q -march=silvermont${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=silvermont${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'bonnell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=bonnell"
      echo
      echo "gcc -c -Q -march=bonnell${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=bonnell${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'graniterapids-d' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=graniterapids-d${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=graniterapids-d${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=graniterapids-d${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'graniterapids' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=graniterapids${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=graniterapids${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=graniterapids${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'emeraldrapids' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=emeraldrapids${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=emeraldrapids${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=emeraldrapids${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'grandridge' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=grandridge${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=grandridge${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=grandridge${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'sierraforest' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=sierraforest${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=sierraforest${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=sierraforest${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'meteorlake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=meteorlake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=meteorlake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=meteorlake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'raptorlake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=raptorlake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=raptorlake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=raptorlake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'sapphirerapids' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=sapphirerapids${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=sapphirerapids${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=sapphirerapids${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'alderlake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=alderlake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=alderlake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=alderlake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'rocketlake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=rocketlake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=rocketlake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=rocketlake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'cascadelake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=cascadelake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=cascadelake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=cascadelake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'cannonlake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=cannonlake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=cannonlake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=cannonlake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'skylake-avx512' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=skylake-avx512${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=skylake-avx512${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=skylake-avx512${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'skylake' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=skylake${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=skylake${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=skylake${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'broadwell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=broadwell${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=broadwell${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=broadwell${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'haswell' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=haswell${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=haswell${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=haswell${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'ivybridge' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=ivybridge${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=ivybridge${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=ivybridge${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'sandybridge' && "$(cat /proc/cpuinfo | grep -o 'avx' | uniq)" = 'avx' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=sandybridge${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=sandybridge${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=sandybridge${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'westmere' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=westmere"
      echo
      echo "gcc -c -Q -march=westmere${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=westmere${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'nehalem' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=nehalem"
      echo
      echo "gcc -c -Q -march=nehalem${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=nehalem${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'core-avx2' && "$(cat /proc/cpuinfo | grep -o 'avx' | uniq)" = 'avx' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core-avx2${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=core-avx2${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=core-avx2${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'core-avx-i' && "$(cat /proc/cpuinfo | grep -o 'avx' | uniq)" = 'avx' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core-avx-i${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=core-avx-i${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=core-avx-i${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'corei7-avx' && "$(cat /proc/cpuinfo | grep -o 'avx' | uniq)" = 'avx' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=corei7-avx${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=corei7-avx${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=corei7-avx${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CPU_MARCH" = 'corei7' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=corei7${FALIGN_FUNCTION_OPT}"
      echo
      echo "gcc -c -Q -march=corei7${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=corei7${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CENTOS_SIX" = '6' && "$CPU_MARCH" = 'core2' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core2"
      echo
      echo "gcc -c -Q -march=core2${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=core2${PHPEXTRA_CFLAGS} --help=target
      echo
    elif [[ "$CENTOS_SEVEN" = '7' && "$CPU_MARCH" = 'core2' ]]; then
      PHP_MTUNEOPT="-m${CCM} -march=core2"
      echo
      echo "gcc -c -Q -march=core2${PHPEXTRA_CFLAGS} --help=target"
      gcc -c -Q -march=core2${PHPEXTRA_CFLAGS} --help=target
      echo
    else
      PHP_MTUNEOPT="-m${CCM} -mtune=native"
    fi
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    fi
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'AuthenticAMD' && "$CPU_MARCH" = 'znver1' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}"
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    fi
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'AuthenticAMD' && "$CPU_MARCH" = 'znver2' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}"
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    fi
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'AuthenticAMD' && "$CPU_MARCH" = 'znver3' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}"
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    fi
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'AuthenticAMD' && "$CPU_MARCH" = 'znver4' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}"
    # if only 1 cpu thread use -O2 to keep compile times sane
    if [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '1' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    elif [[ "$(grep -c "processor" /proc/cpuinfo)" -eq '2' ]]; then
      export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    else
      export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    fi
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'core-avx-i')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'corei7')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'core2')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && ! -z "$(echo $MARCHCHECK | grep 'pentium')" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[disabled]' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=native"
    export CFLAGS="-O3 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && -z "$SSECHECK" ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=64
    PHP_MTUNEOPT="-m${CCM} -mtune=generic"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    CCM=32
    PHP_MTUNEOPT="-m${CCM} -mtune=generic"
    export CFLAGS="-O2 $PHP_MTUNEOPT -pipe${SET_PHPEXTRA_CFLAGS}"
    export CXXFLAGS="$CFLAGS"
  else
    PHP_MTUNEOPT=""
  fi
fi

# zend opcache hugepages
if [[ "$PHPMUVER" > 7 && ! -f /proc/user_beancounters ]]; then
  opcachehugepages
else
  if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
    if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
      sed -i 's|^opcache.huge_code_pages=1|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
      sed -i 's|^;opcache.huge_code_pages=1|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
      sed -i 's|^opcache.huge_code_pages=0|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
      sed -i 's|^;opcache.huge_code_pages=0|;opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini     
    fi    
  fi  
fi

# install check & set PHP_OPENSSLOPT
  echo "PHP_OPENSSLOPT install check"
  echo "$PHPMVER"
  echo "$PHPVER_ID"
  if [[ "$PHP_CUSTOMSSL" = [yY] ]] && [[ "$PHPMVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' ]]; then
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
      PHP_OPENSSLOPT='--with-openssl=/opt/libressl'
      ln -s /opt/libressl/lib /opt/libressl/lib64
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREEZERO" = '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: install PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREEONE" = '3.1' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: install PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREETWO" = '3.2' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: install PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ -d /opt/el-compat-quic && "$EL_COMPAT_QUIC_OPENSSL_INSTALL" = [yY] && "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH='/opt/el-compat-quic'
      PHP_OPENSSLOPT="--with-openssl=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_CURL_OPT="=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_IMAP_OPT="=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      ln -s "${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib" "${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib64"
      echo "install PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
    elif [[ -d /opt/el-compat && "$EL_COMPAT_OPENSSL_INSTALL" = [yY] && "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      EL_COMPAT_OPENSSL_CUSTOMPATH='/opt/el-compat'
      PHP_OPENSSLOPT="--with-openssl=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_CURL_OPT="=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_IMAP_OPT="=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      ln -s "${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib" "${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib64"
      echo "install PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${EL_COMPAT_OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
    elif [[ "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "install PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    else
      PHP_OPENSSLOPT='--with-openssl'
    fi
  else
    PHP_OPENSSLOPT='--with-openssl'
  fi

  echo "PHP_OPENSSLOPT upgrade check"
  echo "$PHPMUVER"
  echo "$PHPVER_ID"
# upgrades check & set PHP_OPENSSLOPT
  if [[ "$PHP_CUSTOMSSL" = [yY] ]] && [[ "$PHPMUVER" = '7.4' || "$PHPMUVER" = '8.0' || "$PHPMUVER" = '8.1' || "$PHPMUVER" = '8.2' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
    if [[ "$LIBRESSL_SWITCH" = [yY] ]]; then
      PHP_OPENSSLOPT='--with-openssl=/opt/libressl'
      ln -s /opt/libressl/lib /opt/libressl/lib64
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMUVER" = '8.1' || "$PHPMUVER" = '8.2' || "$PHPMUVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREEZERO" = '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: upgrade PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMUVER" = '8.1' || "$PHPMUVER" = '8.2' || "$PHPMUVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREEONE" = '3.1' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: upgrade PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPMUVER" = '8.1' || "$PHPMUVER" = '8.2' || "$PHPMUVER" = '8.3' ]] && [[ "$DETECTOPENSSL_THREETWO" = '3.2' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "PHP_CUSTOMSSL=Y: upgrade PHP + OpenSSL 3.1 OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    elif [[ -d /opt/el-compat-quic && "$EL_COMPAT_QUIC_OPENSSL_INSTALL" = [yY] && "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH='/opt/el-compat-quic'
      PHP_OPENSSLOPT="--with-openssl=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_CURL_OPT="=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_IMAP_OPT="=${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}"
      ln -s "${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib" "${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib64"
      echo "upgrade PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${EL_COMPAT_QUIC_OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
    elif [[ -d /opt/el-compat && "$EL_COMPAT_OPENSSL_INSTALL" = [yY] && "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      EL_COMPAT_OPENSSL_CUSTOMPATH='/opt/el-compat'
      PHP_OPENSSLOPT="--with-openssl=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_CURL_OPT="=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      PHP_OPENSSL_IMAP_OPT="=${EL_COMPAT_OPENSSL_CUSTOMPATH}"
      ln -s "${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib" "${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib64"
      echo "upgrade PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${EL_COMPAT_OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${EL_COMPAT_OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
    elif [[ "$DETECTOPENSSL_THREEZERO" != '3.0' && "$LIBRESSL_SWITCH" = [nN] && "$BORINGSSL_SWITCH" = [nN] ]]; then
      PHP_OPENSSLOPT="--with-openssl=${OPENSSL_CUSTOMPATH}"
      ln -s "${OPENSSL_CUSTOMPATH}/lib64" "${OPENSSL_CUSTOMPATH}/lib"
      echo "upgrade PHP OPENSSL_CFLAGS + OPENSSL_LIBS"
      export OPENSSL_CFLAGS="-I${OPENSSL_CUSTOMPATH}/include"
      export OPENSSL_LIBS="-L${OPENSSL_CUSTOMPATH}/lib -lssl -lcrypto -ldl -lpthread"
      export PKG_CONFIG_PATH=${OPENSSL_CUSTOMPATH}/lib/pkgconfig
      if [[ -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" && ! -f /opt/openssl/openssl.cnf ]]; then
          \cp -af "${DIR_TMP}/openssl-${OPENSSL_VERSION}/apps/openssl.cnf" /opt/openssl/openssl.cnf
      fi
    else
      PHP_OPENSSLOPT='--with-openssl'
    fi
  else
    PHP_OPENSSLOPT='--with-openssl'
  fi

if [[ "$PHPIMAP" = [yY] ]]; then
  IMAPOPT=" --with-imap --with-imap-ssl${PHP_OPENSSL_IMAP_OPT}"
else
  IMAPOPT=""
fi

phpimap_install

if [[ "$PHPMVER" = '5.5' || "$PHPMVER" = '5.6' || "$PHPMVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMVER" = '7.3' ]]; then
  PHP_OPENSSLOPT='--with-openssl'
fi
if [[ "$PHPMUVER" = '5.5' || "$PHPMUVER" = '5.6' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' ]]; then
  PHP_OPENSSLOPT='--with-openssl'
fi

if [[ "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  # seems PHP 7.3 must use libzip unlike PHP 7.2 which can compile without it
  PHP_LIBZIP='y'
fi
if [[ "$PHP_ARGON" = [yY] ]]; then
  # alias for PHP 7.2 argon support
  PHP_LIBZIP='y'
elif [[ "$PHP_ARGON" = [nN] && "$PHP_LIBZIP" = [nN] ]]; then
  # alias for PHP 7.2 argon support
  PHP_LIBZIP='n'
fi
if [[ "$PHP_LIBZIP" = [yY] ]] && [[ "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  ARGONTWO_OPT=' --with-password-argon2'
  LIBSODIUM_OPT=' --with-sodium'
  echo "libzip check PHPMVER = $PHPMVER"
  echo "libzip check PHPMUVER = $PHPMUVER"
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    LIBDIR_LIBZIP='lib64'
  else
    LIBDIR_LIBZIP='lib'
  fi
  if [[ "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
    # https://bugs.php.net/bug.php?id=76483
    # PHP 7.4 not finding libzip paths
    export LIBZIP_CFLAGS='-I/usr/local/include'
    export LIBZIP_LIBS="-L/usr/local/${LIBDIR_LIBZIP} -lzip"
    echo "php 8.x libzip"
    LIBZIP_OPT=' --with-libzip'
  elif [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' ]]; then
    # https://bugs.php.net/bug.php?id=76483
    # PHP 7.4 not finding libzip paths
    export LIBZIP_CFLAGS='-I/usr/local/include'
    export LIBZIP_LIBS="-L/usr/local/${LIBDIR_LIBZIP} -lzip"
    echo "php 7.4 libzip"
    LIBZIP_OPT=' --with-libzip'
  else
    # PHP 7.4 not finding libzip paths
    export LIBZIP_CFLAGS='-I/usr/local/include'
    export LIBZIP_LIBS="-L/usr/local/${LIBDIR_LIBZIP} -lzip"
    echo "non php 7.4 libzip"
    LIBZIP_OPT=' --with-libzip'
    # if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
    #   export LD_LIBRARY_PATH=/usr/local/${LIBDIR_LIBZIP} 
    #   export PKG_CONFIG_PATH="/usr/local/${LIBDIR_LIBZIP}/pkgconfig"
    # fi
  fi
  # if [[ ! -f /usr/include/zip.h ]]; then
  #   time $YUMDNFBIN${CACHESKIP} -q -y install libzip5 libzip5-devel --enablerepo=remi
  # fi
  if [[ ! -f /usr/local/include/zip.h || ! -d "${DIR_TMP}/libzip-${LIBZIP_VER}" ]]; then
    libzip_install
  fi
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    LIBDIR_ARGONSODIUM='lib64'
  else
    LIBDIR_ARGONSODIUM='lib'
  fi
  if [[ "$PHP_LIBZIP" = [yY] ]] && [[ ! -f "/usr/local/${LIBDIR_ARGONSODIUM}/libsodium.so" || ! -f "libsodium-${LIBSODIUM_VER}.tar.gz" ]]; then
    pushd "$DIR_TMP"
    if [ -s "libsodium-${LIBSODIUM_VER}.tar.gz" ]; then
        cecho "libsodium-${LIBSODIUM_VER}.tar.gz found, skipping download..." $boldgreen
    else
        if [[ "$LIBSODIUM_VER" = '1.0.17' ]]; then
          echo "wget -c${ipv_forceopt} "https://github.com/jedisct1/libsodium/releases/download/${LIBSODIUM_VER}/libsodium-${LIBSODIUM_VER}.tar.gz" --tries=3"
          wget -c${ipv_forceopt} "https://github.com/jedisct1/libsodium/releases/download/${LIBSODIUM_VER}/libsodium-${LIBSODIUM_VER}.tar.gz" --tries=3
        else
          echo "wget -c${ipv_forceopt} "https://github.com/jedisct1/libsodium/releases/download/${LIBSODIUM_VER}-RELEASE/libsodium-${LIBSODIUM_VER}.tar.gz" --tries=3"
          wget -c${ipv_forceopt} "https://github.com/jedisct1/libsodium/releases/download/${LIBSODIUM_VER}-RELEASE/libsodium-${LIBSODIUM_VER}.tar.gz" --tries=3
        fi
        # echo "wget -c${ipv_forceopt} "https://pecl.php.net/get/libsodium-${LIBSODIUM_VER}.tgz" --tries=3"
        # wget -c${ipv_forceopt} "https://pecl.php.net/get/libsodium-${LIBSODIUM_VER}.tgz" --tries=3
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
            cecho "Error: libsodium-${LIBSODIUM_VER}.tar.gz download failed." $boldgreen
        else 
            cecho "Download done." $boldyellow
        fi
    fi
    tar xzf libsodium-${LIBSODIUM_VER}.tar.gz
    cd libsodium-${LIBSODIUM_VER}
    make clean
    if [[ "$LIBSODIUM_NATIVE" = [nN] ]]; then
      ./configure --prefix=/usr/local --libdir=/usr/local/${LIBDIR_ARGONSODIUM}
    elif [[ "$LIBSODIUM_NATIVE" = [yY] ]]; then
      ./configure --prefix=/usr/local --enable-opt --libdir=/usr/local/${LIBDIR_ARGONSODIUM}
    fi
    make${MAKETHREADS}
    make install
    LIBSODIUM_OPT=' --with-sodium=/usr/local'
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
      # PHP 7.4 not finding sodium paths
      export LIBSODIUM_CFLAGS='-I/usr/local/include'
      export LIBSODIUM_LIBS="-L/usr/local/${LIBDIR_ARGONSODIUM} -lsodium"
    fi
    # clean up older libsodium versions if they are detected
    if [[ "$(uname -m)" = 'x86_64' ]] && [[ -f /usr/local/lib/libsodium.so || -f /usr/local/lib/libsodium.so.23 || -f /usr/local/lib/libsodium.so.23.0.0 ]]; then
      rm -f /usr/local/lib/libsodium.so.23
      rm -f /usr/local/lib/libsodium.so
      rm -f /usr/local/lib/libsodium.so.23.0.0
    fi
    popd
  fi
  if [[ "$PHP_LIBZIP" = [yY] ]] && [[ -f "/usr/local/${LIBDIR_ARGONSODIUM}/libsodium.so" ]]; then
    export LIBSODIUM_CFLAGS='-I/usr/local/include'
    export LIBSODIUM_LIBS="-L/usr/local/${LIBDIR_ARGONSODIUM} -lsodium"
  fi
  if [[ "$PHP_LIBZIP" = [yY] ]] && [[ ! -f "/usr/local/${LIBDIR_ARGONSODIUM}/libargon2.so" || ! -f "${LIBARGON_VER}.tar.gz" ]]; then
    pushd "$DIR_TMP"
    if [ -s "${LIBARGON_VER}.tar.gz" ]; then
        cecho "${LIBARGON_VER}.tar.gz found, skipping download..." $boldgreen
    else
        echo "wget -c${ipv_forceopt} "https://github.com/P-H-C/phc-winner-argon2/archive/refs/tags/${LIBARGON_VER}.tar.gz" --tries=3"
        wget -c${ipv_forceopt} "https://github.com/P-H-C/phc-winner-argon2/archive/refs/tags/${LIBARGON_VER}.tar.gz" --tries=3
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
            cecho "Error: ${LIBARGON_VER}.tar.gz download failed." $boldgreen
        else 
            cecho "Download done." $boldyellow
        fi
    fi
    # rm -rf phc-winner-argon2-${LIBARGON_VER}
    rm -rf "/usr/local/${LIBDIR_ARGONSODIUM}/libargon2.so"
    tar xzf ${LIBARGON_VER}.tar.gz
    cd phc-winner-argon2-${LIBARGON_VER}
    make clean
    make${MAKETHREADS}
    make install PREFIX=/usr/local LIBRARY_REL=${LIBDIR_ARGONSODIUM}
    #ARGONTWO_OPT=' --with-password-argon2=/usr/local'
    ARGONTWO_OPT=' --with-password-argon2'
    if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
      # PHP 7.4 not finding sodium paths
      export ARGON2_CFLAGS='-I/usr/local/include'
      export ARGON2_LIBS="-L/usr/local/${LIBDIR_ARGONSODIUM} -largon2"
    fi
    # clean up older libargon2 versions if they are detected
    if [[ "$(uname -m)" = 'x86_64' ]] && [[ -f /usr/local/lib/libargon2.so.0 || -f /usr/local/lib/libargon2.so || -f /usr/local/lib/libargon2.a ]]; then
      rm -f /usr/local/lib/libargon2.so
      rm -f /usr/local/lib/libargon2.so.0
      rm -f /usr/local/lib/libargon2.a
    fi
    if [[ "$(uname -m)" = 'x86_64' ]] && [[ -f /usr/lib/libargon2.so || -f /usr/lib/libargon2.so.0 ]]; then
      rm -f /usr/lib/libargon2.so.0
      rm -f /usr/lib/libargon2.so
    fi
    # if [[ "$CENTOS_SEVEN" = '7' && "$(rpm -q libargon2-devel | grep -o libargon2-devel)" = 'libargon2-devel' ]]; then
    #   # remove ancient libargon EPEL package
    #   yum -y remove libargon2-devel libargon2
    # fi
    popd
  fi
  if [[ "$PHP_LIBZIP" = [yY] ]] && [[ -f "/usr/local/${LIBDIR_ARGONSODIUM}/libargon2.so" ]]; then
    export ARGON2_CFLAGS='-I/usr/local/include'
    export ARGON2_LIBS="-L/usr/local/${LIBDIR_ARGONSODIUM} -largon2"
  fi
  # if libzip, libsodium or libargon2 libraries still don't exit, disable from php configuration
  if [[ ! -f "/usr/local/${LIBDIR_ARGONSODIUM}/libargon2.so" || ! -f "/usr/local/${LIBDIR_ARGONSODIUM}/libsodium.so" || ! -f /usr/local/include/zip.h ]]; then
    echo
    echo "disable libzip, libsodium or libargon2 libraries"
    echo
    ARGONTWO_OPT=""
    LIBSODIUM_OPT=""
    LIBZIP_OPT=""
  fi
else
  ARGONTWO_OPT=""
  LIBSODIUM_OPT=""
  LIBZIP_OPT=""
fi

if [[ "$PHP_LIBGD_EXTERNAL" = [yY] ]] && [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  LIBGD_EXT_OPT=' --with-external-gd'
  install_libgd_external
# elif [[ "$PHP_LIBGD_EXTERNAL" = [yY] ]] && [[ "$PHPMVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMUVER" = '7.3' ]]; then
#   install_libgd_external
#   LIBGD_EXTERNAL_PATH="=/usr/local/libgd/${LIBGD_EXTERNAL_VER}"
else
  LIBGD_EXT_OPT=''
fi

if [[ "$INITIALINSTALL" = [yY] ]] && [[ "$PHPMVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPVER_ID" -ge '80000' ]]; then
  # https://raw.githubusercontent.com/php/php-src/PHP-7.4/UPGRADING
  ZIPOPT='--with-zip'
  PHPGDOPT='--enable-gd'
  PHPFREETYPEOPT='--with-freetype'
  PHPJPEGOPT='--with-jpeg'
  PHPXPMOPT='--with-xpm'
elif [[ "$INITIALINSTALL" = [yY] ]]; then
  #statements
  ZIPOPT='--enable-zip'
  PHPGDOPT="--with-gd${LIBGD_EXTERNAL_PATH}"
  PHPFREETYPEOPT="--with-freetype-dir=${PLIBDIR}"
  PHPJPEGOPT="--with-jpeg-dir=${PLIBDIR}"
  PHPXPMOPT="--with-xpm-dir=${PLIBDIR}"
fi

if [[ "$INITIALINSTALL" != [yY] ]] && [[ "$PHPMUVER" = '7.4' || "$PHPMUVER" = '8.0' || "$PHPMUVER" = '8.1' || "$PHPVER_ID" -ge '80000' ]]; then
  # https://raw.githubusercontent.com/php/php-src/PHP-7.4/UPGRADING
  ZIPOPT='--with-zip'
  PHPGDOPT='--enable-gd'
  PHPFREETYPEOPT='--with-freetype'
  PHPJPEGOPT='--with-jpeg'
  PHPXPMOPT='--with-xpm'
elif [[ "$INITIALINSTALL" != [yY] ]]; then
  ZIPOPT='--enable-zip'
  PHPGDOPT="--with-gd${LIBGD_EXTERNAL_PATH}"
  PHPFREETYPEOPT="--with-freetype-dir=${PLIBDIR}"
  PHPJPEGOPT="--with-jpeg-dir=${PLIBDIR}"
  PHPXPMOPT="--with-xpm-dir=${PLIBDIR}"
fi

if [[ "$PHPMVER" = '7.4' || "$PHPMUVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
  # ensure libpng-devel paths are found by PHP 7.4+
  if [ -f /usr/bin/libpng-config ]; then
    PHP_PNG_SUFFIX=$(/usr/bin/libpng-config --libs)
    echo
    echo "set PNG_CFLAGS and PNG_LIBS"
    echo "export PNG_CFLAGS='-I/usr/include'"
    export PNG_CFLAGS='-I/usr/include'
    echo "export PNG_LIBS='-L/usr/lib64 ${PHP_PNG_SUFFIX}'"
    export PNG_LIBS="-L/usr/lib64 ${PHP_PNG_SUFFIX}"
    echo
    ls -lAhrt /usr/include | grep -i png
    echo
    ls -lAhrt /usr/lib64 | grep -i png
    echo
  else
    echo
    echo "error: /usr/bin/libpng-config not found"
    echo "libpng-devel not installed"
    echo
    yum -y install libpng libpng-devel
    echo
  fi
fi

php_pcrejit_stacksize
pgo_version_edit
echo
echo "Using GCC $(gcc -dumpversion)"
echo

if [[ "$FPM_PHPFPM_RPM" = [yY] && "$INITIALINSTALL" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
  # experimental initial install PHP-FPM custom RPM build
  echo
  echo "Skip PHP-FPM configure..."
  echo "FPM_PHPFPM_RPM=$FPM_PHPFPM_RPM"
  PHPCONFIGURE_ERR=0
else
  if [[ "$PHP_SECURED" = [yY] ]]; then
    PHP_BEFORE_CONFIGURE_CFLAGS="$CFLAGS"
    echo "export CFLAGS=\"$CFLAGS -fstack-protector-strong -fPIC\""
    export CFLAGS="$CFLAGS -fstack-protector-strong -fPIC"
    echo "export CPPFLAGS=\"$CPPFLAGS -D_FORTIFY_SOURCE=2\""
    export CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"
    echo "export ICU_CFLAGS='-fPIC'"
    export ICU_CFLAGS='-fPIC'
    echo "export LIBSODIUM_CFLAGS='-fPIC'"
    export LIBSODIUM_CFLAGS='-fPIC'
    echo "export LIBZIP_CFLAGS='-fPIC'"
    export LIBZIP_CFLAGS='-fPIC'
    PHP_PICOPT=' --with-pic'
    if [[ "$PHP_SECURED_PIE" = [yY] ]]; then
      echo "export PHP_LDFLAGS=\"$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie\""
      export PHP_LDFLAGS="$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie"
    else
      echo "export PHP_LDFLAGS=\"$PHP_LDFLAGS -Wl,-z,relro,-z,now\""
      export PHP_LDFLAGS="$PHP_LDFLAGS -Wl,-z,relro,-z,now"
    fi
    checksec_check
  fi

  if [[ "$PHP_CUSTOMSSL" = [yY] ]] && [[ "$PHPMVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMVER" = '8.3' ]]; then
    PHP_CUSTOMSSL_PKG_CONFIG_PATH="${OPENSSL_CUSTOMPATH}/lib/pkgconfig"
    PHP_CUSTOMSSL_LD_LIBRARY_PATH="${OPENSSL_CUSTOMPATH}/lib:"
  else
    PHP_CUSTOMSSL_PKG_CONFIG_PATH=""
    PHP_CUSTOMSSL_LD_LIBRARY_PATH=""
  fi

  if [[ "$MDB_INSTALL" = [yY] || "$MYSQL_INSTALL" = [yY] ]]; then
    echo "1"
    echo "LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH=\"/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}\" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}"
    LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH="/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}
  # elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]] && [[ "$PHP_SECURED_PIE" = [yY] ]]; then
  #   echo "2 PIE"
  #   echo "LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH=\"/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}\" PHP_LDFLAGS=\"$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie\" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}"
  #   LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH="/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}" PHP_LDFLAGS="$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}
  elif [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
    echo "2"
    echo "LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH=\"/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}\" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}"
    LD_LIBRARY_PATH=${PHP_CUSTOMSSL_LD_LIBRARY_PATH}/usr/local/${LIBDIR} PKG_CONFIG_PATH="/usr/local/${LIBDIR}/pkgconfig:${PHP_CUSTOMSSL_PKG_CONFIG_PATH}" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}
  # elif [[ "$CENTOS_SEVEN" -eq '7' ]] && [[ "$PHP_SECURED_PIE" = [yY] ]]; then
  #   echo "3 PIE"
  #   echo "PHP_LDFLAGS=\"$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie\" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}"
  #   PHP_LDFLAGS="$PHP_LDFLAGS -Wl,-z,relro,-z,now -pie" ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}
  else
    echo "3"
    echo "../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}"
    ../configure --enable-fpm${OPCACHEOPT}${PHPINTLOPT}${PCNTLOPT}${MCRYPT}${RECODEOPT}${SNMPOPT}${PHPEMBEDOPT}${PHPLTOOPT} --with-mhash ${ZLIBPHP_OPT} --with-gettext --enable-exif ${ZIPOPT}${LIBZIP_OPT} --with-bz2 --enable-soap --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock --with-curl${PHP_OPENSSL_CURL_OPT} ${PHPGDOPT}${LIBGD_EXT_OPT} --with-xmlrpc --enable-bcmath --enable-calendar${FTPEXTOPT} --enable-gd-native-ttf ${PHPFREETYPEOPT} ${PHPJPEGOPT} --with-png-dir=${PLIBDIR} ${PHPXPMOPT}${LIBVPXOPT}${TLIBOPT} --enable-shmop --with-pear --enable-mbstring $PHP_OPENSSLOPT --with-mysql=mysqlnd --with-libdir=${LIBDIR} --with-mysqli=mysqlnd --enable-pdo${PDOSQLLITE_OPT} --with-pdo-mysql=mysqlnd${MSSQLOPT} --enable-inline-optimization${IMAPOPT} --with-kerberos --with-readline --with-libedit --with-gmp --with-pspell --with-tidy --with-enchant --with-fpm-user=nginx --with-fpm-group=nginx${FILEINFOOPT}${SQLLITEOPT}${LDAPOPT}${PDOPGSQL_OPT}${OPCACHEHUGEPAGES_OPT}${ARGONTWO_OPT}${LIBSODIUM_OPT}${PHP_PICOPT} --with-config-file-scan-dir=${CONFIGSCANDIR}${PHPDEBUGOPT}${FPMSYSTEMD_INTEGRATEOPT}${FFI_OPT}${SWOOLE_OPT}${PHP_EXTRAOPTS}
  fi
  PHPCONFIGURE_ERR=$?
fi
sar_call

if [[ "$PHP_PGO" = [yY] ]]; then
  if [ -d /opt/php-pgo-data ]; then
    # clear old PGO profiling data first
    rm -rf /opt/php-pgo-data
  fi
  sed -i 's|PROF_FLAGS=-fprofile-generate|PROF_FLAGS="-fprofile-generate=/opt/php-pgo-data -fprofile-dir=/opt/php-pgo-data"|' Makefile
  sed -i 's|PROF_FLAGS=-fprofile-use|PROF_FLAGS="-fprofile-use=/opt/php-pgo-data -fprofile-dir=/opt/php-pgo-data -fprofile-correction"|' Makefile
fi
  # only initial install needs to run make, make install
  # via funct_phpconfigure function otherwise upgrade php
  # routines do make, make install via inc/php_upgrade.inc
  if [[ "$FPM_PHPFPM_RPM" = [yY] && "$INITIALINSTALL" = [yY] && "$CENTOS_SEVEN" -eq '7' ]]; then
    # experimental initial install PHP-FPM custom RPM build
    if [[ "$PHPMVER" = '7.4' ]]; then
      echo
      echo "install PHP-FPM 7.4 custom RPM"
      echo
      wget${ipv_forceopt_wget} ${LOCALCENTMINMOD_MIRROR}/centminmodparts/phpfpm-rpm/phpfpm-custom-7.4.30-1.el7.x86_64.rpm -O "$DIR_TMP/phpfpm-custom-7.4.30-1.el7.x86_64.rpm"
      yum -y localinstall "$DIR_TMP/phpfpm-custom-7.4.30-1.el7.x86_64.rpm"
      echo
    elif [[ "$PHPMVER" = '8.0'  ]]; then
      echo
      echo "install PHP-FPM 8.0 custom RPM"
      echo
      wget${ipv_forceopt_wget} ${LOCALCENTMINMOD_MIRROR}/centminmodparts/phpfpm-rpm/phpfpm-custom-8.0.20-1.el7.x86_64.rpm -O "$DIR_TMP/phpfpm-custom-8.0.20-1.el7.x86_64.rpm"
      yum -y localinstall "$DIR_TMP/phpfpm-custom-8.0.20-1.el7.x86_64.rpm"
      echo
    elif [[ "$PHPMVER" = '8.1' ]]; then
      echo
      echo "install PHP-FPM 8.1 custom RPM"
      echo
      wget${ipv_forceopt_wget} ${LOCALCENTMINMOD_MIRROR}/centminmodparts/phpfpm-rpm/phpfpm-custom-8.1.7-1.el7.x86_64.rpm -O "$DIR_TMP/phpfpm-custom-8.1.7-1.el7.x86_64.rpm"
      yum -y localinstall "$DIR_TMP/phpfpm-custom-8.1.7-1.el7.x86_64.rpm"
      echo
    fi
  elif [[ "$INITIALINSTALL" = [yY] ]]; then
    echo
    echo "php_configure.inc make${MAKETHREADS_PHP}"
    echo "PHP_PGO = $PHP_PGO"
    if [[ "$PHP_PGO" = [yY] ]] && [[ "$PHPMVER" = '7.0' || "$PHPMVER" = '7.1' || "$PHPMVER" = '7.2' || "$PHPMVER" = '7.3' || "$PHPMVER" = '7.4' || "$PHPMVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPMVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMVER" = '8.2' || "$PHPMUVER" = '8.2' || "$PHPMVER" = '8.3' || "$PHPMUVER" = '8.3' || "$PHPVER_ID" -ge '80000' ]]; then
        # get cpu core to fix for PGO training for optimal cpu cache access
        pgo_cpurange=$(echo $(($(nproc)-1)))
        pgo_cpulist=$(seq 1 $pgo_cpurange)
        echo "make${MAKETHREADS_PHP} prof-gen"
        time make${MAKETHREADS_PHP} prof-gen
        if [ ! -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            pushd "$DIR_TMP"
            time git clone -b cmm https://github.com/centminmod/php_pgo_training_scripts php_pgo_training_scripts
            #time git clone https://github.com/01org/php_pgo_training_scripts php_pgo_training_scripts
            popd
            GET_ROOT_PASS=$(awk -F "=" '/password/ {print $2}' /root/.my.cnf | sed -e "s|'||g")
            echo "ls -lah $DIR_TMP/php_pgo_training_scripts"
            ls -lah "$DIR_TMP/php_pgo_training_scripts"
            sed -i "s|define('DB_PASSWORD', 'root');|define('DB_PASSWORD', '$GET_ROOT_PASS');|" "$DIR_TMP/php_pgo_training_scripts/constants.php"
            ./sapi/cli/php "$DIR_TMP/php_pgo_training_scripts/init.php"

            if [ -f ./sapi/cgi/php ]; then
              echo "time ./sapi/cgi/php ../Zend/bench.php"
              time ./sapi/cgi/php ../Zend/bench.php
              echo "time ./sapi/cgi/php ../Zend/micro_bench.php"
              time ./sapi/cgi/php ../Zend/micro_bench.php
            fi

            echo "time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/bench.php"
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/bench.php
            echo "time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php"
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php
            echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php""
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php"
            if [ -f "$PHPPGO_INDEXPATH" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_TWO" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_TWO""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_TWO" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_THREE" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_THREE""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_THREE" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_FOUR" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FOUR""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FOUR" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_FIVE" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FIVE""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FIVE" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_SIX" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_SIX""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_SIX" >/dev/null
            fi
            echo "pgophpbench"
            pgophpbench
            if [ -f "${DIR_TMP}/phpbench/phpbench.php" ]; then
              echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ${DIR_TMP}/phpbench/phpbench.php"
              time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "${DIR_TMP}/phpbench/phpbench.php"
              if [ -f ./sapi/cgi/php ]; then
                echo "./sapi/cgi/php ${DIR_TMP}/phpbench/phpbench.php"
                time ./sapi/cgi/php "${DIR_TMP}/phpbench/phpbench.php"
              fi
            fi
            if [ -f /opt/phpbench/detailed_benchmark.php ]; then
              echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} /opt/phpbench/detailed_benchmark.php"
              time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} /opt/phpbench/detailed_benchmark.php
              if [ -f ./sapi/cgi/php ]; then
                echo "./sapi/cgi/php /opt/phpbench/detailed_benchmark.php"
                time ./sapi/cgi/php /opt/phpbench/detailed_benchmark.php
              fi
            fi
        elif [ -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            if [[ ! "$(git --git-dir=$DIR_TMP/php_pgo_training_scripts/.git/ log -1 2>&1 | grep -o 'php 8 support fix')" ]]; then
              rm -rf "$DIR_TMP/php_pgo_training_scripts"
              pushd "$DIR_TMP"
              time git clone -b cmm https://github.com/centminmod/php_pgo_training_scripts php_pgo_training_scripts
              popd
            fi
            pushd "$DIR_TMP/php_pgo_training_scripts"
            git stash
            git pull
            popd
            GET_ROOT_PASS=$(awk -F "=" '/password/ {print $2}' /root/.my.cnf | sed -e "s|'||g")
            sed -i "s|define('DB_PASSWORD', 'root');|define('DB_PASSWORD', '$GET_ROOT_PASS');|" "$DIR_TMP/php_pgo_training_scripts/constants.php"
            ./sapi/cli/php "$DIR_TMP/php_pgo_training_scripts/init.php"

            if [ -f ./sapi/cgi/php ]; then
              echo "time ./sapi/cgi/php ../Zend/bench.php"
              time ./sapi/cgi/php ../Zend/bench.php
              echo "time ./sapi/cgi/php ../Zend/micro_bench.php"
              time ./sapi/cgi/php ../Zend/micro_bench.php
            fi

            echo "time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/bench.php"
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/bench.php
            echo "time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php"
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php
            echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php""
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php"
            if [ -f "$PHPPGO_INDEXPATH" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_TWO" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_TWO""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_TWO" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_THREE" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_THREE""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_THREE" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_FOUR" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FOUR""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FOUR" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_FIVE" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FIVE""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_FIVE" >/dev/null
            fi
            if [ -f "$PHPPGO_INDEXPATH_SIX" ]; then
                echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_SIX""
                time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$PHPPGO_INDEXPATH_SIX" >/dev/null
            fi
            echo "pgophpbench"
            pgophpbench
            if [ -f "${DIR_TMP}/phpbench/phpbench.php" ]; then
              echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} ${DIR_TMP}/phpbench/phpbench.php"
              time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "${DIR_TMP}/phpbench/phpbench.php"
              if [ -f ./sapi/cgi/php ]; then
                echo "./sapi/cgi/php ${DIR_TMP}/phpbench/phpbench.php"
                time ./sapi/cgi/php "${DIR_TMP}/phpbench/phpbench.php"
              fi
            fi
            if [ -f /opt/phpbench/detailed_benchmark.php ]; then
              echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} /opt/phpbench/detailed_benchmark.php"
              time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} /opt/phpbench/detailed_benchmark.php
              if [ -f ./sapi/cgi/php ]; then
                echo "./sapi/cgi/php /opt/phpbench/detailed_benchmark.php"
                time ./sapi/cgi/php /opt/phpbench/detailed_benchmark.php
              fi
            fi
        fi
        echo "make prof-clean"
        time make prof-clean
        echo "make${MAKETHREADS_PHP} prof-use"
        time make${MAKETHREADS_PHP} prof-use
    else
        echo "make${MAKETHREADS_PHP}"
        time make${MAKETHREADS_PHP}
    fi

    sar_call
    echo

    if [[ "$PHPMAKETEST" = [yY] ]]; then
      time make TEST_PHP_ARGS="-j$(nproc) -q" test | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}-php-initial-install-make-test.log"
      sar_call
    fi
    
    echo "php_configure.inc make install"
    if [[ "$FPM_PHPFPM_INSTALLDIR_ENABLE" = [yY] ]]; then
      echo
      rm -rf "$FPM_PHPFPM_INSTALLDIR"
      mkdir -p "$FPM_PHPFPM_INSTALLDIR"
      echo "time make install INSTALL_ROOT=$FPM_PHPFPM_INSTALLDIR"
      time make install INSTALL_ROOT="$FPM_PHPFPM_INSTALLDIR"
      FPM_PHPFPM_INSTALLDIR_ERR=$?
      if [[ "$STRIPPHP" = [yY] ]]; then
          echo
          echo "strip PHP binary..."
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php-cgi
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/sbin/php-fpm
          echo
          strip -s ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php
          strip -s ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php-cgi
          strip -s ${FPM_PHPFPM_INSTALLDIR}/usr/local/sbin/php-fpm
          echo
          echo "after strip PHP binary..."
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/bin/php-cgi
          ls -lah ${FPM_PHPFPM_INSTALLDIR}/usr/local/sbin/php-fpm
          echo
      fi
    fi
    time make install
    sar_call
   
    if [[ "$STRIPPHP" = [yY] ]]; then
        echo
        echo "strip PHP binary..."
        ls -lah /usr/local/bin/php
        ls -lah /usr/local/bin/php-cgi
        ls -lah /usr/local/sbin/php-fpm
        echo
        echo "backup before strip PHP binary..."
        \cp -af /usr/local/bin/php /usr/local/bin/php-b4strip 
        \cp -af /usr/local/bin/php-cgi /usr/local/bin/php-cgi-b4strip 
        \cp -af /usr/local/sbin/php-fpm /usr/local/sbin/php-fpm-b4strip 
        strip -s /usr/local/bin/php
        strip -s /usr/local/bin/php-cgi
        strip -s /usr/local/sbin/php-fpm
        echo
        echo "after strip PHP binary..."
        ls -lah /usr/local/bin/php
        ls -lah /usr/local/bin/php-cgi
        ls -lah /usr/local/sbin/php-fpm
        echo
    fi
  fi
  if [ -f config.log ]; then
    gzip -c config.log > "${CENTMINLOGDIR}/php-compile-config-${DT}.log.gz"
  fi

if [[ "$CLANG_PHP" = [yY] ]]; then
    unset CC
    unset CXX
    #unset CFLAGS
    #unset CCACHE_CPP2
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++"
    # PLACEHOLDER FOR CLAGS FOR PHP
    # CFLAGS=
fi

if [[ "$GCCINTEL_PHP" = [yY] ]]; then
  if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    unset CFLAGS
    unset CXXFLAGS
  elif [[ "$(uname -m)" != 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' ]] && [[ "$CLANG_PHP" = [nN] ]]; then
    unset CFLAGS
    unset CXXFLAGS
  fi
fi

if [[ "$PHP_SECURED" = [yY] && "$PHP_SECURED_PIE" = [yY] ]]; then
    # export CFLAGS="$PHP_BEFORE_CONFIGURE_CFLAGS"
    unset CPPFLAGS
elif [[ "$PHP_SECURED" = [yY] && "$PHP_SECURED_PIE" != [yY] ]]; then
    unset CPPFLAGS
fi

# unset_intelflags

# check if php was in fact installed

if [[ ! -s /usr/local/bin/phpize ]]; then
  cecho "----------------------------------------------------------------------------------" $boldyellow
  echo "PHP was not installed properly"
  cecho "----------------------------------------------------------------------------------" $boldyellow
  #checklogdetails
  #exit
fi

}