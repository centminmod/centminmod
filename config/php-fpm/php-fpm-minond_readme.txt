cd /usr/local/etc/
wget http://centminmod.com/phpfpm/php-fpm-minond.conf
cp php-fpm.conf php-fpm.conf-backupold
unalias cp
cp -f php-fpm-minond.conf php-fpm.conf
service php-fpm restart