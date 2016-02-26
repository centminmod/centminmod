#!/bin/bash
#############################################################
# official ioncube loader PHP extension Addon for
# Centmin Mod centminmod.com
# written by George Liu (eva2000)
#############################################################
PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
#############################################################

if [[ "$(expr $PHPCURRENTVER \= 5.7)" = 1 || "$(expr $PHPCURRENTVER \< 5.3)" = 1 ]]; then
  echo "Your current PHP version $PHPCURRENTVER is incompatible with ioncube loader"
  echo "ioncube loader only supports PHP versions 5.3-5.6 currently"
  echo "aborting installation"
  exit
fi

echo
echo "ioncube loader installation started"
echo "ioncube loader only supports PHP 5.3, 5.4, 5.5 and 5.6"
echo "ioncube loader does not support PHP 7.0 currently"
echo

cd /svr-setup
mkdir -p ioncube
cd ioncube

if [[ "$(uname -m)" = 'x86_64' ]]; then
  wget -cnv http://downloads3.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
  tar xvzf ioncube_loaders_lin_x86-64.tar.gz
else
  wget -cnv http://downloads3.ioncube.com/loader_downloads/ioncube_loaders_lin_x86.tar.gz
  tar xvzf ioncube_loaders_lin_x86.tar.gz
fi

# check current PHP version
ICPHPVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

# move current ioncube version to existing PHP extension directory
\cp -f ioncube/ioncube_loader_lin_${ICPHPVER}.so ${PHPEXTDIRD}/ioncube.so

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

service php-fpm restart >/dev/null 2>&1

echo ""
echo "Check if PHP module: ionCube Loader loaded"
php --ri 'ionCube Loader'

echo
echo "ioncube loader installation completed"
echo "you'll need to rerun ioncube.sh after each major PHP version upgrades"
echo "PHP 5.3 to 5.4 or PHP 5.4 to PHP 5.5 to PHP 5.6"
echo