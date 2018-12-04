#!/bin/bash
#############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
#############################################################
# mcrypt PECL extension for PHP 7.3+ for
# Centmin Mod centminmod.com
# written by George Liu (eva2000)
#############################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
CONFIGSCANDIR='/etc/centminmod/php.d'
DIR_TMP='/svr-setup'
PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
#############################################################

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

mcrypt_peclinstall() {
  checkmenu="$1"

  if [[ "${PHPCURRENTVER}" != '7.3' && "$checkmenu" != 'menu' ]]; then
    echo "Your current PHP version $PHPCURRENTVER is not PHP 7.3 branch"
    echo "this addon is only for PHP 7.3 users"
    echo "aborting installation"
    exit
  fi
  
  PHPEXTDIRD=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')
  if [ -f /usr/local/src/centminmod/centmin.sh ]; then
    PHP_MCRYPTPECLVER=$(awk -F "'" '/PHP_MCRYPTPECLVER=/ {print $2}' /usr/local/src/centminmod/centmin.sh)
  else
    PHP_MCRYPTPECLVER='1.0.1'
  fi

  echo
  echo "mcrypt PECL extension installing ..."
  echo
  pushd "$DIR_TMP"
  rm -rf mcrypt-*
  wget "https://pecl.php.net/get/mcrypt-${PHP_MCRYPTPECLVER}.tgz"
  tar xvzf "mcrypt-${PHP_MCRYPTPECLVER}.tgz"
  cd "mcrypt-${PHP_MCRYPTPECLVER}"
  make clean
  phpize
  ./configure --with-mcrypt --with-php-config=/usr/local/bin/php-config
  make${MAKETHREADS}
  make install
  popd
  
  if [[ -f "${CONFIGSCANDIR}/mcrypt.ini" ]]; then
    rm -rf "${CONFIGSCANDIR}/mcrypt.ini"
  fi
  
  touch "${CONFIGSCANDIR}/mcrypt.ini"
  
cat > "${CONFIGSCANDIR}/mcrypt.ini" <<EOF
extension=mcrypt.so
EOF
  
  ls -lah ${CONFIGSCANDIR}
  echo ""
  ls -lah ${PHPEXTDIRD}
  
  service php-fpm restart >/dev/null 2>&1
  
  if [ -f "${PHPEXTDIRD}/mcrypt.so" ]; then
    echo ""
    echo "Check if PHP module: mcrypt loaded"
    php --ri mcrypt
    
    echo
    echo "mcrypt PECL extension install completed"
    echo
  else
    echo ""
    echo "mcrypt PECL extension failed to install properly"
  fi
}

#######################
starttime=$(TZ=UTC date +%s.%N)
{
  if [[ "$1" = 'menu' ]]; then
    mcrypt_peclinstall menu
  else
    mcrypt_peclinstall
  fi
} 2>&1 | tee ${CENTMINLOGDIR}/mcrypt-php73-pecl-install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/mcrypt-php73-pecl-install_${DT}.log
echo "Total PHP 7.3 mcrypt PECL Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/mcrypt-php73-pecl-install_${DT}.log