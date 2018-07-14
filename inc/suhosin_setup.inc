################################
# http://www.hardened-php.net/suhosin/
suhosin_no() {
  PHPVERCHECK=$(cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}')
    echo ""
    echo ""
    echo "$PHPVERCHECK not compatible with Suhosin PHP extension"
    echo "install aborted"
    echo ""
    exit
}

suhosinfive_install() {

  pushd "$DIR_TMP"

  if [ -s suhosin-${SUHOSINVER}.tar.gz ]; then
    echo "suhosin-${SUHOSINVER}.tar.gz [found]"
    else
    echo "Error: suhosin-${SUHOSINVER}.tar.gz not found!!! Downloading now......"
    wget -c${ipv_forceopt} --progress=bar http://download.suhosin.org/suhosin-${SUHOSINVER}.tar.gz --tries=3
  fi
  
  tar xvzf suhosin-${SUHOSINVER}.tar.gz
  cd suhosin-${SUHOSINVER}
  make clean
  phpize
  ./configure --with-php-config=/usr/local/bin/php-config
  make${MAKETHREADS}
  make install
  popd
}

suhosinseven_install() {
  pushd "$DIR_TMP"
  rm -rf suhosin7
  git clone https://github.com/sektioneins/suhosin7
  cd suhosin7
  make clean
  phpize
  ./configure --with-php-config=/usr/local/bin/php-config
  make${MAKETHREADS}
  make install
  popd
}

suhosinsetup() {
  PHPVERCHECK=$(cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}')

  if [[ "$PHPVERCHECK" = 5.[456] ]]; then
    suhosinfive_install
    suhosin_name=suhosin
  elif [[ "$PHPVERCHECK" = 7.[01] ]]; then
    suhosinseven_install
    suhosin_name=suhosin7
  else
    suhosin_no
  fi # PHPVERCHECK
  
  if [ ! -f ${CONFIGSCANDIR}/suhosin.ini ]; then
   touch ${CONFIGSCANDIR}/suhosin.ini
  fi

  SUHOSINCHECK=$(grep 'extension=suhosin.so' "${CONFIGSCANDIR}/suhosin.ini" >/dev/null 2>&1; echo $?)
  if [[ "$SUHOSINCHECK" = '1' ]]; then

cat > "${CONFIGSCANDIR}/suhosin.ini" <<EOF
extension=${suhosin_name}.so
suhosin.executor.include.whitelist = phar
suhosin.simulation = 0
suhosin.log.file.name = /var/log/suhosin.log
suhosin.cookie.encrypt = 0
suhosin.executor.disable_eval = 0
suhosin.get.max_value_length = 5120
suhosin.get.max_vars = 5120
suhosin.memory_limit = 512M
suhosin.post.max_value_length = 1000000
suhosin.post.max_vars = 5120
suhosin.post.max_array_index_length = 256
suhosin.post.max_totalname_length = 8192
suhosin.request.max_value_length = 1000000
suhosin.request.max_vars = 5120
suhosin.request.max_varname_length =  5120
suhosin.request.max_array_index_length = 256
suhosin.request.max_totalname_length = 8192
suhosin.upload.disallow_binary = 0
suhosin.upload.disallow_elf = 1
suhosin.upload.max_uploads = 25
suhosin.upload.remove_binary = 0
EOF

  fi

  if [[ "$SUHOSINCHECK" = '0' ]]; then
      echo "sed -i 's/;extension=suhosin.so/extension=suhosin.so/g' ${CONFIGSCANDIR}/suhosin.ini; fpmrestart; php -v" >/usr/bin/suhosinon ; chmod 700 /usr/bin/suhosinon
      echo "sed -i 's/extension=suhosin.so/;extension=suhosin.so/g' ${CONFIGSCANDIR}/suhosin.ini; fpmrestart; php -v" >/usr/bin/suhosinoff ; chmod 700 /usr/bin/suhosinoff
  fi
  
  cmservice php-fpm restart
  
  php -v

} # suhosininstall