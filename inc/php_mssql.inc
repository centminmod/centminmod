php_mssqlinstall() {
if [[ "$PHPMSSQL" = [yY] ]]; then
  # compile php mssql extension for PHP versions 5.4-5.6 but not php 7 which
  # removed php mssql extension support
  if [[ -n "$PHPMUVER" && "$PHPMUVER" < 7 || "$PHPMVER" < 7 ]]; then
    if [[ ! -z "$phpver" ]]; then
      MSSQL_EXTDIR="${DIR_TMP}/php-${phpver}/ext/mssql"
    else
      MSSQL_EXTDIR="${DIR_TMP}/php-${PHP_VERSION}/ext/mssql"
    fi
    if [ -d "$MSSQL_EXTDIR" ]; then
      pushd "$MSSQL_EXTDIR"
      if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
        /usr/local/bin/phpize -clean
      fi
      /usr/local/bin/phpize
      if [[ "$(uname -m)" = 'x86_64' ]]; then
        ./configure --with-libdir=lib64 --with-mssql=/usr --with-php-config=/usr/local/bin/php-config
      else
        ./configure --with-mssql=/usr --with-php-config=/usr/local/bin/php-config
      fi
      make
      make install
      popd

cat > "${CONFIGSCANDIR}/mssql.ini" <<EMS
extension=mssql.so
mssql.charset=UTF-8
EMS

    fi
  fi
fi  
}