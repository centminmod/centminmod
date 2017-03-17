checkfor_lowmem() {
  if [[ "$LOWMEM_INSTALL" = [yY] ]]; then
    ISLOWMEM='131072'
    CLANG='y'
    CLANG_PHP='n'
    GCCINTEL_PHP='n'
    PHP_PGO='n'
    PHP_PGO_CENTOSSIX='n'
    DEVTOOLSETSIX='n'
    NGINX_DEVTOOLSETGCC='n'
    GENERAL_DEVTOOLSETGCC='n'
    CRYPTO_DEVTOOLSETGCC='n'
    NGINXPATCH='n'
    NGINX_GEOIPMEM='n'           # Nginx caches GEOIP databases in memory (default), setting 'n' caches to disk instead
    NGINX_SECURELINK='n'
    NGINX_FANCYINDEX='n'
    NGINX_VHOSTSTATS='n'
    NGINX_LIBBROTLI='n'
    NGINX_PAGESPEED='n'
    NGINX_HTTPREDIS='n'
    ORESTY_LUANGINX='n'
    PHPFINFO='n'
    PHPMAILPARSE='n'
    LIBRESSL_SWITCH='y'
    PHP_FTPEXT='n'
    NSD_INSTALL='n'
  fi
}