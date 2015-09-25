################################
# http://www.hardened-php.net/suhosin/
suhosinsetup() {

PHPVERCHECK=`cat /usr/local/bin/php-config | awk '/^version/ {phpver=$1} END {gsub(/"|version|=|)/,"",phpver); print phpver}' | awk -F "." '{print $1"."$2}'`

if [ "$PHPVERCHECK" != '5.7' ]; then

cd $DIR_TMP

if [ -s suhosin-${SUHOSINVER}.tar.gz ]; then
  echo "suhosin-${SUHOSINVER}.tar.gz [found]"
  else
  echo "Error: suhosin-${SUHOSINVER}.tar.gz not found!!! Downloading now......"
  wget -c --progress=bar http://download.suhosin.org/suhosin-${SUHOSINVER}.tar.gz --tries=3
fi

tar xvzf suhosin-${SUHOSINVER}.tar.gz
cd suhosin-${SUHOSINVER}
make clean
phpize
./configure
make${MAKETHREADS}
make install

if [ ! -f ${CONFIGSCANDIR}/suhosin.ini ]; then
	touch ${CONFIGSCANDIR}/suhosin.ini
fi

SUHOSINCHECK=$(grep 'extension=suhosin.so' ${CONFIGSCANDIR}/suhosin.ini)
if [ -z "$SUHOSINCHECK" ]; then
# echo "" >> ${CONFIGSCANDIR}/suhosin.ini
# echo "extension=suhosin.so" >> ${CONFIGSCANDIR}/suhosin.ini

cat > "${CONFIGSCANDIR}/suhosin.ini" <<EOF
extension=suhosin.so
suhosin.executor.include.whitelist = phar
suhosin.simulation = 0
suhosin.cookie.encrypt = 0
suhosin.executor.disable_eval = 0
suhosin.get.max_value_length = 2048
suhosin.get.max_vars = 1000
suhosin.memory_limit = 0
suhosin.post.max_value_length = 1000000
suhosin.post.max_vars = 4096
suhosin.post.max_array_index_length = 256
suhosin.post.max_totalname_length = 8192
suhosin.request.max_value_length = 1000000
suhosin.request.max_vars = 4096
suhosin.request.max_varname_length =  384
suhosin.request.max_array_index_length = 256
suhosin.request.max_totalname_length = 8192
suhosin.upload.disallow_binary = 0
suhosin.upload.disallow_elf = 1
suhosin.upload.max_uploads = 25
suhosin.upload.remove_binary = 0
EOF

fi

if [ ! -z "$SUHOSINCHECK" ]; then
    echo "sed -i 's/;extension=suhosin.so/extension=suhosin.so/g' ${CONFIGSCANDIR}/suhosin.ini; fpmrestart; php -v" >/usr/bin/suhosinon ; chmod 700 /usr/bin/suhosinon
    echo "sed -i 's/extension=suhosin.so/;extension=suhosin.so/g' ${CONFIGSCANDIR}/suhosin.ini; fpmrestart; php -v" >/usr/bin/suhosinoff ; chmod 700 /usr/bin/suhosinoff
fi

cmservice php-fpm restart

php -v

else 

echo ""
echo ""
echo "$PHPVERCHECK not compatible with Suhosin PHP extension"
echo "install aborted"
echo ""
echo ""

fi # PHPVERCHECK

} # suhosininstall