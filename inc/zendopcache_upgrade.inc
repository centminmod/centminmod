###############
opprotect() {

######################################################
# Randomize ${N}_opcache.php filename

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

  if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
    hname=$(hostname)
  else
    hname=$(hostname -f)
  fi
OPSALT=$(openssl rand 10 -base64 | tr -dc 'a-zA-Z0-9')
OPUSER=$(echo "opadmin${OPSALT}")
OPPASS=$(openssl rand 22 -base64 | tr -dc 'a-zA-Z0-9')

  echo ""
  cecho "Creating htpasswd_opcache user/pass..." $boldyellow
  echo "/usr/local/nginx/conf/htpasswd.sh create /usr/local/nginx/conf/htpasswd_opcache $OPUSER $OPPASS"
  # if [ ! -f /usr/local/nginx/conf/htpasswd_opcache ]; then
  #   touch /usr/local/nginx/conf/htpasswd_opcache
  # fi
  /usr/local/nginx/conf/htpasswd.sh create /usr/local/nginx/conf/htpasswd_opcache $OPUSER $OPPASS
  OPDETAILS=$(cat /usr/local/nginx/conf/htpasswd_opcache)

  echo ""
  cecho "Insert Username/Password into /usr/local/nginx/html/${N}_opcache.php file itself" $boldyellow
  sed -i "s|OPCACHEUSERNAME|$OPUSER|" /usr/local/nginx/html/${N}_opcache.php
  sed -i "s|OPCACHEPASSWORD|$OPPASS|" /usr/local/nginx/html/${N}_opcache.php

  echo ""
  cecho "Create include file /usr/local/nginx/conf/include_opcache.conf" $boldyellow
  echo ""

cat > "/usr/local/nginx/conf/include_opcache.conf" <<EOF
            location ~ ^/(${N}_opcache.php) {
    include /usr/local/nginx/conf/php.conf;
  auth_basic "Password Protected";
  auth_basic_user_file /usr/local/nginx/conf/htpasswd_opcache;
            }
EOF

  cat /usr/local/nginx/conf/include_opcache.conf

echo ""
cecho "Setup virtual.conf" $boldyellow
cecho "Adding /usr/local/nginx/conf/include_opcache.conf include entry" $boldyellow

CHECKCOPINFO=$(grep include_opcache.conf /usr/local/nginx/conf/conf.d/virtual.conf)

if [[ -z "$CHECKCOPINFO" ]]; then
  sed -i '/include \/usr\/local\/nginx\/conf\/staticfiles.conf;/a \include \/usr\/local\/nginx\/conf\/include_opcache.conf;' /usr/local/nginx/conf/conf.d/virtual.conf
fi

if [ -f /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf ]; then
  CHECKCOPINFO_PMA=$(grep include_opcache.conf /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf)

  if [[ -z "$CHECKCOPINFO_PMA" ]]; then
    sed -i '/include \/usr\/local\/nginx\/conf\/staticfiles.conf;/a \include \/usr\/local\/nginx\/conf\/include_opcache.conf;' /usr/local/nginx/conf/conf.d/phpmyadmin_ssl.conf
  fi
fi

echo ""
cmservice nginx reload

  echo "" > ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "-------------------------------------------------------" $boldyellow >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "File Location: /usr/local/nginx/html/${N}_opcache.php" $boldgreen >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "Password protected ${hname}/${N}_opcache.php" $boldgreen >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "-------------------------------------------------------" $boldyellow >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "Username: $OPUSER" $boldgreen >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "Password: $OPPASS" $boldgreen >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  cecho "-------------------------------------------------------" $boldyellow >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
  echo "" >> ${CENTMINLOGDIR}/zendopcache_passfile.txt
}
###############

#################
zopcacheupgrade() {
  if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ "$PHPMUVER" > 5.5 ]]; then
  echo ""
  echo "-----------------------------------------------------------------------------------------"
  cecho "Detected PHP $PHPMUVER branch." $boldyellow
  echo "You can compile Zend OPcache (Zend Optimizer Plus+) support"
  echo "as an alternative to using APC Cache or Xcache cache."
  echo "But Zend OPcache only provides PHP opcode cache and"
  echo "DOESN'T do data caching, so if your web apps such as Wordpress,"
  echo "Drupal or vBulletin require data caching to APC or Xcache,"
  echo "it won't work with Zend OPcache."
  echo ""
  # echo "If you have imagick or memcache/memcached PHP extensions"
  # echo "installed, you will have to use menu option #10 or #15 to"
  # echo "reinstall after upgrading to PHP 5.5.0 with Zend OPcache."
  echo "-----------------------------------------------------------------------------------------"
  read -ep "Do you want to use Zend OPcache [y/n] ? " zendopcacheon
  echo ""

if [[ "$zendopcacheon" = [nN] ]]; then
  if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
    rm -rf ${CONFIGSCANDIR}/zendopcache.ini
  fi
fi

if [[ "$zendopcacheon" = [yY] ]]; then

  # only remove existing ini when major PHP upgrade version differs
    # from existing installed PHP version
  if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]]; then


    if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
      sed -i 's/apc.serializer=igbinary/;apc.serializer=igbinary/g' ${CONFIGSCANDIR}/igbinary.ini
    fi
  
    if [ -f ${CONFIGSCANDIR}/memcache.ini ]; then
      rm -rf ${CONFIGSCANDIR}/memcache.ini
    fi
  
    if [ -f ${CONFIGSCANDIR}/memcached.ini ]; then
      rm -rf ${CONFIGSCANDIR}/memcached.ini
    fi
  
    if [ -f ${CONFIGSCANDIR}/imagick.ini ]; then
      rm -rf ${CONFIGSCANDIR}/imagick.ini
    fi
  
    if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
      rm -rf ${CONFIGSCANDIR}/igbinary.ini
    fi
  
    if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
      # save zend opcache memory allocation size
      # to be transplanted back into newly created
      # zendopcache.ini
      ZENDOPCACHE_EXISTMEMSIZE=$(awk -F "=" '/opcache.memory_consumption/ {print $2}' ${CONFIGSCANDIR}/zendopcache.ini)
      rm -rf ${CONFIGSCANDIR}/zendopcache.ini
    fi

  fi # PHPMUVER != PHPCURRENTVER

    if [ -f ${CONFIGSCANDIR}/apc.ini ]; then
      rm -rf ${CONFIGSCANDIR}/apc.ini
    fi

    if [ -f ${CONFIGSCANDIR}/xcache.ini ]; then
      rm -rf ${CONFIGSCANDIR}/xcache.ini
    fi
  

  DETECTXCACHE=n
  DETECTAPC=n
    

######################################################
# Randomize ${N}_opcache.php filename

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

  if [[ "$(hostname -f 2>&1 | grep -w 'Unknown host')" || "$(hostname -f 2>&1 | grep -w 'service not known')" ]]; then
    hname=$(hostname)
  else
    hname=$(hostname -f)
  fi
OPSALT=$(openssl rand 10 -base64 | tr -dc 'a-zA-Z0-9')
OPUSER=$(echo "opadmin${OPSALT}")
OPPASS=$(openssl rand 22 -base64 | tr -dc 'a-zA-Z0-9')

N=$(od -vAn -N8 -tx < /dev/urandom | sed -e 's/\s//g')
if [[ -f /usr/local/nginx/html/opcache.php ]]; then
  mv /usr/local/nginx/html/opcache.php /usr/local/nginx/html/${N}_opcache.php
  chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
  opprotect
  OLD_OPCACHEPHP=n
elif [[ -z "$(ls /usr/local/nginx/html/ | grep opcache.php)" ]]; then
  cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php
  chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
  opprotect
  OLD_OPCACHEPHP=n
elif [[ "$(ls /usr/local/nginx/html/ | grep opcache.php)" ]]; then
  # rm -rf /usr/local/nginx/html/*_opcache.php
  # cp $CUR_DIR/config/zendopcache/opcache.php /usr/local/nginx/html/${N}_opcache.php
  # chown nginx:nginx /usr/local/nginx/html/${N}_opcache.php
  # opprotect
  for f in $(ls -rt /usr/local/nginx/html/ | grep opcache.php | sed '1{/opcache.php/d;}'); 
    do 
      echo "removing duplicate _opcache.php files"
    echo "rm -rf /usr/local/nginx/html/$f";
    rm -rf /usr/local/nginx/html/$f
    OLD_OPCACHEPHP=y
  done
fi
######################################################

  PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
  PHPEXTDIRDOLD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`

  #replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- "$CUR_DIR/config/zendopcache/zendopcache-min.ini"

  #replace 'zend_extension=opcache.so' "zend_extension=${PHPEXTDIRD}/opcache.so" -- "$CUR_DIR/config/zendopcache/zendopcache.ini"

fi # zendopcacheon

fi

if [[ "$zendopcacheon" = [yY] ]]; then
#######################################################
# check if opcache.so exists in ${CONFIGSCANDIR}/zendopcache.ini

ZOPLUSSOCHECK=$(grep 'opcache.so' ${CONFIGSCANDIR}/zendopcache.ini)

if [[ -z "$ZOPLUSSOCHECK" ]]; then

  echo

  if [[ "$lesszendopcachemem" = [yY] ]]; then

  echo $lesszendopcachemem

  echo -e "\nCopying zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
  cat $CUR_DIR/config/zendopcache/zendopcache-min.ini > ${CONFIGSCANDIR}/zendopcache.ini

  # transfer existing memory allocation value to newly created
  # zendopcache.ini
  if [[ -z "$ZENDOPCACHE_EXISTMEMSIZE" ]]; then
    ZENDOPCACHE_EXISTMEMSIZE='40'
  fi
  sed -i "s|^opcache.memory_consumption=.*|opcache.memory_consumption=$ZENDOPCACHE_EXISTMEMSIZE|" ${CONFIGSCANDIR}/zendopcache.ini

else

  echo $lesszendopcachemem

  echo -e "\nCopying zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini\n"
  cat $CUR_DIR/config/zendopcache/zendopcache.ini > ${CONFIGSCANDIR}/zendopcache.ini

  # transfer existing memory allocation value to newly created
  # zendopcache.ini
  if [[ -z "$ZENDOPCACHE_EXISTMEMSIZE" ]]; then
    ZENDOPCACHE_EXISTMEMSIZE='80'
  fi  
  sed -i "s|^opcache.memory_consumption=.*|opcache.memory_consumption=$ZENDOPCACHE_EXISTMEMSIZE|" ${CONFIGSCANDIR}/zendopcache.ini

fi # check if opcache.so exists in zendopcache.ini
fi # zendopcacheon

    # cmservice php-fpm restart
    echo "*************************************************"
    cecho "* Zend Optimizer Plus OPcache configured" $boldgreen
    echo "*************************************************"

fi # $PHPMUVER = 5.5
  fi # PHP_INSTALL=y
}


#################
zendopcacheextfix() {

if [[ "$zendopcacheon" = [yY] ]]; then

  if [[ ! -f ${CONFIGSCANDIR}/igbinary.ini ]]; then
    #funct_igbinaryinstall
    echo "will need to reinstall memcached extensions if needed"
  fi

fi

}