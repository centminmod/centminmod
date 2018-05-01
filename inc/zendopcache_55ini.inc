#################
zopcache_initialini() {
  if [[ "$PHP_INSTALL" = [yY] ]]; then
PHPMVER=$(echo "$PHP_VERSION" | cut -d . -f1,2)

if [[ "$PHPMVER" > 5.4 ]]; then
  PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
fi # PHPMVER

#######################################################
# check if opcache.so exists in ${CONFIGSCANDIR}/zendopcache.ini

ZOPLUSSOCHECK=$(grep 'opcache.so' ${CONFIGSCANDIR}/zendopcache.ini)

if [[ -z "$ZOPLUSSOCHECK" ]]; then

  echo

  if [[ "$lesszendopcachemem" = [yY] ]]; then

    echo $lesszendopcachemem
  
    echo -e "\nCopying zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
    cat $CUR_DIR/config/zendopcache/zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini

  else

    echo $lesszendopcachemem
  
    echo -e "\nCopying zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
    cat $CUR_DIR/config/zendopcache/zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini

  fi # check if opcache.so exists in zendopcache.ini

    # cmservice php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache installed" $boldgreen
    echo "*************************************************"

fi # $ZOPLUSSOCHECK

  # php 5.5+ versions missing opcache.php file
  ######################################################
  # Randomize ${N}_opcache.php filename
  N=$(od -vAn -N8 -tx < /dev/urandom)
  N=$(echo ${N} | sed -e 's/\s//' -e 's/ //')
  echo
  echo "setup initial /usr/local/nginx/html/opcache.php"
  echo
  if [[ ! -f /usr/local/nginx/html/opcache.php ]]; then
    echo "cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php"
    cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php
    chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
    opprotectinstall
  elif [[ "$(ls /usr/local/nginx/html | grep opcache >/dev/null 2>&1; echo $?)" != '0' ]]; then
    echo "cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php"
    cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php
    chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
    opprotectinstall
  fi
  ######################################################
fi # PHP_INSTALL=y
}
