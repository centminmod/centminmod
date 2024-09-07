# allows you to pass additional PHP configuration options to 
# php configuration options in inc/php_configure.inc as long as
# the required libraries for specific php option already exist
# or are installed i.e. https://community.centminmod.com/posts/23608/
PHP_EXTRAOPTS=" --with-xsl"

run_after_php_upgrade() {
  if [[ -d /etc/centminmod && -f /etc/centminmod/run-after-php-upgrade.sh ]]; then
    echo
    echo "running /etc/centminmod/run-after-php-upgrade.sh"
    bash /etc/centminmod/run-after-php-upgrade.sh
    echo
  fi
}

run_after_php_install() {
  strip_php_extensions
  if [[ -d /etc/centminmod && -f /etc/centminmod/run-after-php-install.sh ]]; then
    echo
    echo "running /etc/centminmod/run-after-php-install.sh"
    bash /etc/centminmod/run-after-php-install.sh
    echo
  fi
  if [[ "$PHP_SECURED" = [yY] ]]; then
      echo "***************************************************"
      checksec_phpext
      echo "***************************************************"
      echo "checksec --format=json --file=/usr/local/sbin/php-fpm --extended | jq -r"
      checksec --format=json --file=/usr/local/sbin/php-fpm --extended | jq -r
  fi
}