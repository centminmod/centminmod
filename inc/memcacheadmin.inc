memcacheadmin() {

if [[ "$MEMCINSTALL" = [yY] ]];
then

if [[ -f /usr/local/nginx/html/memcache.php ]]; then
    rm -rf /usr/local/nginx/html/memcache.php
fi

#######################################################
# check to see if memcache_${N}.php already exists

if [[ ! -f /usr/local/nginx/html/memcache_*.php ]]; then

cd $DIR_TMP
# php 7 versus php <7 memcache source directory
PHPCURRENTVER=$(php-config --version | cut -d . -f1,2)

if [[ "$PHPCURRENTVER" = '8.0' || "$PHPMUVER" = '8.0' || "$PHPCURRENTVER" = '8.1' || "$PHPMUVER" = '8.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPEIGHT_CHECKVER" = '0' ]]; then
  cd memcache-php8
elif [[ "$PHPCURRENTVER" = '7.0' || "$PHPMUVER" = '7.0' || "$PHPCURRENTVER" = '7.1' || "$PHPMUVER" = '7.1' || "$PHPCURRENTVER" = '7.2' || "$PHPMUVER" = '7.2' || "$PHPCURRENTVER" = '7.3' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]]; then
  cd memcache-php7
else
  cd memcache-${MEMCACHE_VERSION}
fi

    echo ""
    echo "*************************************************"
    cecho "*  Setup memcached_*.php admin page ..." $boldgreen
    echo "*************************************************"

# Randomize memcache_${N}.php filename
N=$(od -vAn -N8 -tx < /dev/urandom | sed -e 's/\s//g')
\cp -a memcache.php /usr/local/nginx/html/memcache_${N}.php
chown -R nginx:nginx /usr/local/nginx/html
chmod 644 /usr/local/nginx/html/memcache_${N}.php

sed -i "s/'ADMIN_USERNAME','memcache'/'ADMIN_USERNAME','memcacheuser'/g" /usr/local/nginx/html/memcache_${N}.php
sed -i "s/'ADMIN_PASSWORD','password'/'ADMIN_PASSWORD','memcachepass'/g" /usr/local/nginx/html/memcache_${N}.php

if [[ "$nomemcachedservers" = '1' ]]; then

sed -i "s/mymemcache-server1:11211/localhost:11211/g" /usr/local/nginx/html/memcache_${N}.php

sed -i "s/\$MEMCACHE_SERVERS\[] = 'mymemcache-server2:11211'; \/\/ add more as an array/\/\/ mymemcache-server2:/g" /usr/local/nginx/html/memcache_${N}.php

fi

if [[ "$nomemcachedservers" = '2' ]]; then

sed -i "s/mymemcache-server1:11211/localhost:11211/g" /usr/local/nginx/html/memcache_${N}.php

sed -i "s/mymemcache-server2:11211/localhost:11212/g" /usr/local/nginx/html/memcache_${N}.php

fi

CSALT=$(/usr/bin/openssl rand -base64 10 | tr -dc 'a-zA-Z0-9')
memcacheduser=$(echo "memadmin${CSALT}")
memcachedpassword=$(/usr/bin/openssl rand -base64 21 | tr -dc 'a-zA-Z0-9')
echo ""

cecho "Setup Memcached Server Admin Login Details for /usr/local/nginx/html/memcache_${N}.php" $boldgreen
# read -ep "(Type username your want to set and press Enter): " memcacheduser
# read -ep "(Type password your want to set and press Enter): " memcachedpassword

cecho "current memcached username: `grep "'ADMIN_USERNAME','memcacheuser'" /usr/local/nginx/html/memcache_${N}.php | sed -e "s/define('ADMIN_USERNAME','//" | sed -e 's/\/\/ Admin Username//' | sed 's/^[ \t]*//;s/[ \t]*$//' | sed -e "s/');//"`" $boldgreen

cecho "current memcached password: `grep "'ADMIN_PASSWORD','memcachepass'" /usr/local/nginx/html/memcache_${N}.php | sed -e "s/define('ADMIN_PASSWORD','//" | sed -e 's/\/\/ Admin Password//' | sed 's/^[ \t]*//;s/[ \t]*$//' | sed -e "s/');//"`" $boldgreen

sed -i "s/'ADMIN_USERNAME','memcacheuser'/'ADMIN_USERNAME','${memcacheduser}'/g" /usr/local/nginx/html/memcache_${N}.php 2>&1>/dev/null
sed -i "s/'ADMIN_PASSWORD','memcachepass'/'ADMIN_PASSWORD','${memcachedpassword}'/g" /usr/local/nginx/html/memcache_${N}.php 2>&1>/dev/null

echo "*************************************************"
cecho "Memcached Server Admin Login File: /usr/local/nginx/html/memcache_${N}.php" $boldgreen
cecho "Memcached Server Admin Login: ${hname}/memcache_${N}.php" $boldgreen
cecho "new memcached username: ${memcacheduser}" $boldgreen
cecho "new memcached password: ${memcachedpassword}" $boldgreen
echo "*************************************************"

    echo "*************************************************"
    cecho "* memcache_${N}.php admin page installed" $boldgreen
    echo "*************************************************"
echo ""

else

    echo "*************************************************"
    cecho "* memcache_${N}.php admin page already installed" $boldgreen
    echo "*************************************************"
echo ""

fi # memcache_${N}.php file check

fi

}