#!/bin/bash
#############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"
#############################################################
# mcrypt PECL extension for PHP 8.0+ for
# Centmin Mod centminmod.com
# written by George Liu (eva2000)
#############################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
CONFIGSCANDIR='/etc/centminmod/php.d'
DIR_TMP='/svr-setup'
PHPCURRENTVER=$(php-config --version | cut -d . -f1,2)
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
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7272' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7272' ]]; then
            # 7272 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7272
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7282' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7282' ]]; then
            # 7282 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7282
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7302' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7302' ]]; then
            # 7302 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7302
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7352' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7352' ]]; then
            # 7352 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7352
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7402' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7402' ]]; then
            # 7402 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7402
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7452' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7452' ]]; then
            # 7452 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7452
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7502' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7502' ]]; then
            # 7502 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7502
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7532' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7532' ]]; then
            # 7532 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7532
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7542' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7542' ]]; then
            # 7542 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7542
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7552' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7552' ]]; then
            # 7552 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7552
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7642' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7642' ]]; then
            # 7642 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7642
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7662' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7662' ]]; then
            # 7662 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7662
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7702' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7702' ]]; then
            # 7702 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7702
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7742' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7742' ]]; then
            # 7742 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7742
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7H12' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7H12' ]]; then
            # 7H12 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7H12
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F52' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F52' ]]; then
            # 7F52 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F52
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F72' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F72' ]]; then
            # 7F72 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F72
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7313' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7313' ]]; then
            # 7313 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7313
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7413' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7413' ]]; then
            # 7413 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7413
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7443' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7443' ]]; then
            # 7443 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7443
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7453' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7453' ]]; then
            # 7453 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7453
            CPUS=14
        elif [[ "$(grep -o 'AMD EPYC 7513' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7513' ]]; then
            # 7513 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7513
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7543' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7543' ]]; then
            # 7543 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7543
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7643' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7643' ]]; then
            # 7643 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7643
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7663' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7663' ]]; then
            # 7663 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7663
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7713' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7713' ]]; then
            # 7713 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7713
            CPUS=32
        elif [[ "$(grep -o 'AMD EPYC 73F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 73F3' ]]; then
            # 73F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/73F3
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 74F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 74F3' ]]; then
            # 74F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/74F3
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 75F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 75F3' ]]; then
            # 75F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/75F3
            CPUS=32
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
        elif [[ "$(grep -o 'AMD EPYC 7501' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7501' ]]; then
            # 7501P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7501p
            # while greater than 12 cpu cores downclocks to 2.6Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7451' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7451' ]]; then
            # 7451 at 12 cpu cores has 3.2Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7451
            # while greater than 12 cpu cores downclocks to 2.9Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7272' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7272' ]]; then
            # 7272 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7272
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7282' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7282' ]]; then
            # 7282 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7282
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7302' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7302' ]]; then
            # 7302 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7302
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7352' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7352' ]]; then
            # 7352 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7352
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7402' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7402' ]]; then
            # 7402 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7402
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7452' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7452' ]]; then
            # 7452 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7452
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7502' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7502' ]]; then
            # 7502 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7502
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7532' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7532' ]]; then
            # 7532 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7532
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7542' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7542' ]]; then
            # 7542 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7542
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7552' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7552' ]]; then
            # 7552 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7552
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7642' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7642' ]]; then
            # 7642 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7642
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7662' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7662' ]]; then
            # 7662 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7662
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7702' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7702' ]]; then
            # 7702 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7702
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7742' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7742' ]]; then
            # 7742 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7742
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 7H12' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7H12' ]]; then
            # 7H12 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7H12
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F52' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F52' ]]; then
            # 7F52 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F52
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7F72' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7F72' ]]; then
            # 7F72 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7F72
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7313' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7313' ]]; then
            # 7313 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7313
            CPUS=8
        elif [[ "$(grep -o 'AMD EPYC 7413' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7413' ]]; then
            # 7413 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7413
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7443' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7443' ]]; then
            # 7443 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7443
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7453' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7453' ]]; then
            # 7453 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7453
            CPUS=14
        elif [[ "$(grep -o 'AMD EPYC 7513' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7513' ]]; then
            # 7513 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7513
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7543' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7543' ]]; then
            # 7543 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7543
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7643' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7643' ]]; then
            # 7643 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7643
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7663' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7663' ]]; then
            # 7663 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7663
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 7713' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7713' ]]; then
            # 7713 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/7713
            CPUS=32
        elif [[ "$(grep -o 'AMD EPYC 73F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 73F3' ]]; then
            # 73F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/73F3
            CPUS=16
        elif [[ "$(grep -o 'AMD EPYC 74F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 74F3' ]]; then
            # 74F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/74F3
            CPUS=24
        elif [[ "$(grep -o 'AMD EPYC 75F3' /proc/cpuinfo | sort -u)" = 'AMD EPYC 75F3' ]]; then
            # 75F3 preferring higher clock frequency https://en.wikichip.org/wiki/amd/epyc/75F3
            CPUS=32
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

  if [[ "${PHPCURRENTVER}" != '8.0' && "$checkmenu" != 'menu' ]]; then
    echo "Your current PHP version $PHPCURRENTVER is not PHP 8.0 branch"
    echo "this addon is only for PHP 8.0 users"
    echo "aborting installation"
    exit
  fi
  
  PHPEXTDIRD=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')
  if [ -f /usr/local/src/centminmod/centmin.sh ]; then
    PHP_MCRYPTPECLVER=$(awk -F "'" '/PHP_MCRYPTPECLVER=/ {print $2}' /usr/local/src/centminmod/centmin.sh)
  else
    PHP_MCRYPTPECLVER='1.0.9'
  fi

  echo
  echo "mcrypt PECL extension installing ..."
  echo
  pushd "$DIR_TMP"
  rm -rf mcrypt-*
  wget${ipv_forceopt_wget} "https://pecl.php.net/get/mcrypt-${PHP_MCRYPTPECLVER}.tgz" --tries=3
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
} 2>&1 | tee ${CENTMINLOGDIR}/mcrypt-php74-pecl-install_${DT}.log

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/mcrypt-php74-pecl-install_${DT}.log
echo "Total PHP 8.0 mcrypt PECL Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/mcrypt-php74-pecl-install_${DT}.log