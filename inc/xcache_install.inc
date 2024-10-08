funct_installxcache() {
      echo
      cecho "* XCache is no longer supported" $boldyellow
      cecho "* XCache developers no longer keep it up to date" $boldyellow
      cecho "* XCache does not perform better than Zend Opcache defaults" $boldyellow
      echo
      exit 0
}

function funct_installxcache_disbaled {
        if [[ "$PHP_INSTALL" = [yY] ]]; then
funct_xcachesourceinstall

if [ ! -d /usr/local/nginx/html/myxcacheadmin ]; then

    echo ""
    cecho "Creating xcache admin directory at /usr/local/nginx/html/myxcacheadmin " $boldgreen

mkdir /usr/local/nginx/html/myxcacheadmin
fi

# old xcache <2 method
#cp -a admin/* /usr/local/nginx/html/myxcacheadmin
#chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin

if [ -d "${DIR_TMP}/xcache-${XCACHE_VERSION}/admin" ]; then

echo "cp -a admin/* /usr/local/nginx/html/myxcacheadmin"
cp -a admin/* /usr/local/nginx/html/myxcacheadmin
echo "chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin"
chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin

else

echo "cp -a htdocs/* /usr/local/nginx/html/myxcacheadmin"
cp -a htdocs/* /usr/local/nginx/html/myxcacheadmin
echo "chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin"
chown -R nginx:nginx /usr/local/nginx/html/myxcacheadmin

fi

    echo ""
    echo "*************************************************"
    cecho "Setup Xcache Admin Password for /usr/local/nginx/html/myxcacheadmin" $boldgreen
    echo "*************************************************"
    echo -n "(Type password your want to set and press Enter): "
read xcachepassword

xcachepass="`echo -n "${xcachepassword}" | md5sum | awk '{ print $1 }'`"

cecho "old password: `grep xcache.admin.pass ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen
cecho "new password: ${xcachepass}" $boldgreen
cecho "xcache username: `grep xcache.admin.user ${CONFIGSCANDIR}/xcache.ini | awk '{ print $3 }' | sed -e 's/"//g'`" $boldgreen

    sed -i "s/d440aed189a13ff970dac7e7e8f987b2/${xcachepass}/g" ${CONFIGSCANDIR}/xcache.ini

cecho "php.ini now has xcache.admin.pass set as follows: :" $boldgreen
cecho "`cat ${CONFIGSCANDIR}/xcache.ini | grep xcache.admin.pass`" $boldgreen

    cmservice php-fpm restart
    echo "*************************************************"
    cecho "* XCache installed" $boldgreen
    echo "*************************************************"
    
    fi # PHP_INSTALL=y
}