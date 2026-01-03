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
# official ioncube loader PHP extension Addon for
# Centmin Mod centminmod.com
# written by George Liu (eva2000)
#############################################################
PHPCURRENTVER=$(php-config --version | cut -d . -f1,2)
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
#############################################################

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [[ "$(expr $PHPCURRENTVER \= 5.7)" = 1 || "$(expr $PHPCURRENTVER \< 5.3)" = 1 ]]; then
  echo "Your current PHP version $PHPCURRENTVER is incompatible with ioncube loader"
  echo "ioncube loader only supports PHP versions 5.3-5.6 & 7.x/8.x currently"
  echo "aborting installation"
  exit
fi

echo
echo "ioncube loader installation started"
echo "ioncube loader only supports PHP 5.6, 7.x, 8.1, 8.2, 8.3, 8.4"
echo "ioncube loader has skipped PHP 8.0 support"
echo "https://blog.ioncube.com/2022/08/05/ioncube-php-8-1-support-faq-were-almost-ready/"
# echo "ioncube loader PHP 7 currently beta supported"
# echo "http://blog.ioncube.com/2016/09/15/php-7-ioncube-loaders/"
echo
echo "installing..."

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

cd /svr-setup
mkdir -p ioncube
cd ioncube

if [[ "$(uname -m)" = 'x86_64' ]]; then
  if [[ "$(php-config --version | cut -d . -f1)" = '5' ]]; then
    if [[ "$(php-config --version | cut -d . -f1,2)" != '5.6' ]]; then
      wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64_5.1.2.tar.gz
      tar xvzf ioncube_loaders_lin_x86-64_5.1.2.tar.gz
    elif [[ "$(php-config --version | cut -d . -f1,2)" = '5.6' ]]; then
      rm -rf ioncube_loaders_lin_x86-64.tar.gz
      rm -rf ioncube
      wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
      tar xvzf ioncube_loaders_lin_x86-64.tar.gz
    fi
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.5' ]]; then
    echo "ioncube loader does not currently have PHP 8.5 support"
    echo "only PHP 8.1, 8.2, 8.3, 8.4 are supported"
    exit
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.4' ]]; then
    rm -rf ioncube_loaders_lin_x86-64.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
    tar xvzf ioncube_loaders_lin_x86-64.tar.gz
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.3' ]]; then
    rm -rf ioncube_loaders_lin_x86-64.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
    tar xvzf ioncube_loaders_lin_x86-64.tar.gz
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.2' ]]; then
    rm -rf ioncube_loaders_lin_x86-64.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
    tar xvzf ioncube_loaders_lin_x86-64.tar.gz
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.1' ]]; then
    rm -rf ioncube_loaders_lin_x86-64.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
    tar xvzf ioncube_loaders_lin_x86-64.tar.gz
  elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.0' ]]; then
    echo "ioncube loader has skipped PHP 8.0 support"
    echo "only PHP 8.1 is supported"
    echo "https://blog.ioncube.com/2022/08/05/ioncube-php-8-1-support-faq-were-almost-ready/"
    exit
  elif [[ "$(php-config --version | cut -d . -f1)" = '7' ]]; then
    rm -rf ioncube_loaders_lin_x86-64.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
    tar xvzf ioncube_loaders_lin_x86-64.tar.gz
  fi
else
  if [[ "$(php-config --version | cut -d . -f1)" != '7' ]]; then
    if [[ "$(php-config --version | cut -d . -f1,2)" != '5.6' ]]; then
      wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86_5.1.2.tar.gz
      tar xvzf ioncube_loaders_lin_x86_5.1.2.tar.gz
    elif [[ "$(php-config --version | cut -d . -f1,2)" = '5.6' ]]; then
      rm -rf ioncube_loaders_lin_x86.tar.gz
      rm -rf ioncube
      wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86.tar.gz
      tar xvzf ioncube_loaders_lin_x86.tar.gz
    fi
  else
    rm -rf ioncube_loaders_lin_x86.tar.gz
    rm -rf ioncube
    wget -${ipv_forceopt}v https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86.tar.gz
    tar xvzf ioncube_loaders_lin_x86.tar.gz
  fi
fi

# check current PHP version
ICPHPVER=$(php-config --version | cut -d . -f1,2)
PHPEXTDIRD=$(php-config --extension-dir)

# move current ioncube version to existing PHP extension directory
if [[ "$(php-config --version | cut -d . -f1)" = '5' ]]; then
  \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
  chown root:root "${PHPEXTDIRD}/ioncube.so"
  chmod 755 "${PHPEXTDIRD}/ioncube.so"
elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.4' ]]; then
  # for php 8 ioncube
  ICPHPVER="$(php-config --version | cut -d . -f1,2)"
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  fi
elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.3' ]]; then
  # for php 8 ioncube
  ICPHPVER="$(php-config --version | cut -d . -f1,2)"
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  fi
elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.2' ]]; then
  # for php 8 ioncube
  ICPHPVER="$(php-config --version | cut -d . -f1,2)"
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  fi
elif [[ "$(php-config --version | cut -d . -f1-2)" = '8.1' ]]; then
  # for php 8 ioncube
  ICPHPVER="$(php-config --version | cut -d . -f1,2)"
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  fi
elif [[ "$(php-config --version | cut -d . -f1)" = '7' ]]; then
  # for php 7 ioncube
  ICPHPVER=$(php-config --version | cut -d . -f1,2)
  if [[ "$(uname -m)" = 'x86_64' ]]; then
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  else
    \cp -fa ioncube/ioncube_loader_lin_${ICPHPVER}.so "${PHPEXTDIRD}/ioncube.so"
    chown root:root "${PHPEXTDIRD}/ioncube.so"
    chmod 755 "${PHPEXTDIRD}/ioncube.so"
  fi
fi

CONFIGSCANDIR='/etc/centminmod/php.d'

if [[ -f "${CONFIGSCANDIR}/ioncube.ini" ]]; then
  rm -rf ${CONFIGSCANDIR}/ioncube.ini
fi

touch ${CONFIGSCANDIR}/ioncube.ini

cat > "${CONFIGSCANDIR}/ioncube.ini" <<EOF
zend_extension=${PHPEXTDIRD}/ioncube.so
EOF

ls -lah ${CONFIGSCANDIR}
echo ""
ls -lah ${PHPEXTDIRD}

systemctl restart php-fpm >/dev/null 2>&1

if [ -f "${PHPEXTDIRD}/ioncube.so" ]; then
  echo ""
  echo "Check if PHP module: ionCube Loader loaded"
  php --ri 'ionCube Loader'
  
  echo
  echo "ioncube loader installation completed"
  echo "you'll need to rerun ioncube.sh after each major PHP version upgrades"
  echo "PHP 7.4 to PHP 8.0 to PHP 8.1 to PHP 8.2 to PHP 8.3 to PHP 8.4"
  echo
else
  echo ""
  echo "ionCube Loader failed to install properly"
fi