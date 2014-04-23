memcacheadmin() {

if [[ "$MEMCINSTALL" = [yY] ]];
then

#######################################################
# check to see if memcache.php already exists

if [[ ! -f /usr/local/nginx/html/memcache.php ]]; then

cd $DIR_TMP
cd memcache-${MEMCACHE_VERSION}

    echo ""
    echo "*************************************************"
    cecho "*  Setup memcached.php admin page ..." $boldgreen
    echo "*************************************************"

cp -a memcache.php /usr/local/nginx/html
chown -R nginx:nginx /usr/local/nginx/html
chmod 644 /usr/local/nginx/html/memcache.php

sed -i "s/'ADMIN_USERNAME','memcache'/'ADMIN_USERNAME','memcacheuser'/g" /usr/local/nginx/html/memcache.php
sed -i "s/'ADMIN_PASSWORD','password'/'ADMIN_PASSWORD','memcachepass'/g" /usr/local/nginx/html/memcache.php

if [[ "$nomemcachedservers" = '1' ]]; then

sed -i "s/mymemcache-server1:11211/localhost:11211/g" /usr/local/nginx/html/memcache.php

sed -i "s/\$MEMCACHE_SERVERS\[] = 'mymemcache-server2:11211'; \/\/ add more as an array/\/\/ mymemcache-server2:/g" /usr/local/nginx/html/memcache.php

fi

if [[ "$nomemcachedservers" = '2' ]]; then

sed -i "s/mymemcache-server1:11211/localhost:11211/g" /usr/local/nginx/html/memcache.php

sed -i "s/mymemcache-server2:11211/localhost:11212/g" /usr/local/nginx/html/memcache.php

fi

echo ""

cecho "Setup Memcached Server Admin Login Details for /usr/local/nginx/html/memcache.php" $boldgreen
read -ep "(Type username your want to set and press Enter): " memcacheduser
read -ep "(Type password your want to set and press Enter): " memcachedpassword

cecho "current memcached username: `grep "'ADMIN_USERNAME','memcacheuser'" /usr/local/nginx/html/memcache.php | sed -e "s/define('ADMIN_USERNAME','//" | sed -e 's/\/\/ Admin Username//' | sed 's/^[ \t]*//;s/[ \t]*$//' | sed -e "s/');//"`" $boldgreen

cecho "current memcached password: `grep "'ADMIN_PASSWORD','memcachepass'" /usr/local/nginx/html/memcache.php | sed -e "s/define('ADMIN_PASSWORD','//" | sed -e 's/\/\/ Admin Password//' | sed 's/^[ \t]*//;s/[ \t]*$//' | sed -e "s/');//"`" $boldgreen

sed -i "s/'ADMIN_USERNAME','memcacheuser'/'ADMIN_USERNAME','${memcacheduser}'/g" /usr/local/nginx/html/memcache.php
sed -i "s/'ADMIN_PASSWORD','memcachepass'/'ADMIN_PASSWORD','${memcachedpassword}'/g" /usr/local/nginx/html/memcache.php

cecho "new memcached username: ${memcacheduser}" $boldgreen
cecho "new memcached password: ${memcachedpassword}" $boldgreen

    echo "*************************************************"
    cecho "* memcache.php admin page installed" $boldgreen
    echo "*************************************************"
echo ""

else

    echo "*************************************************"
    cecho "*  memcache.php admin page already installed" $boldgreen
    echo "*************************************************"
echo ""

fi # memcache.php file check

fi

}