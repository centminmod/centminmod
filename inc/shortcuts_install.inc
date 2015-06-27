shortcutsinstall() {

#read -ep "Add script shortcuts (as shown in command_shortcuts.txt) ? [y/n] " shortcutask
if [[ "$SHORTCUTS" = [yY] ]]; then
    cecho "**********************************************************************" $boldgreen
    cecho "* Add cmd shortcuts for php.ini, my.cnf, php-fpm.conf, nginx.conf and virtual.conf " $boldgreen
    cecho "* Edit php.ini = phpedit " $boldgreen
    cecho "* Edit my.cnf = mycnf " $boldgreen
    cecho "* Edit php-fpm.conf = fpmconf " $boldgreen
    cecho "* Edit nginx.conf = nginxconf " $boldgreen
    cecho "* Edit virtual.conf = vhostconf " $boldgreen
    cecho "* nginx stop/start/restart = ngxstop/ngxstart/ngxrestart " $boldgreen
    cecho "* php-fpm stop/start/restart = fpmstop/fpmstart/fpmrestart " $boldgreen
    cecho "* mysql stop/start/restart = mysqlstop/mysqlstart/mysqlrestart " $boldgreen
    cecho "* nginx + php-fpm stop/start/restart = npstop/npstart/nprestart " $boldgreen
    cecho "* memcached stop/start/restart = memcachedstop/memcachedstart/memcachedrestart " $boldgreen
    cecho "* csf stop/start/restart = csfstop/csfstart/csfrestart " $boldgreen
    cecho "**********************************************************************" $boldgreen

if [[ "$USEEDITOR" = 'vim' ]]; then
    echo "vim /usr/local/lib/php.ini" > /usr/bin/phpedit ; chmod 700 /usr/bin/phpedit
    echo "vim /etc/my.cnf" >/usr/bin/mycnf ; chmod 700 /usr/bin/mycnf
    echo "vim /usr/local/etc/php-fpm.conf" >/usr/bin/fpmconf ; chmod 700 /usr/bin/fpmconf
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf2 ; chmod 700 /usr/bin/fpmconf2
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf3 ; chmod 700 /usr/bin/fpmconf3
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf4 ; chmod 700 /usr/bin/fpmconf4
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf5 ; chmod 700 /usr/bin/fpmconf5
    echo "vim /usr/local/nginx/conf/nginx.conf" >/usr/bin/nginxconf ; chmod 700 /usr/bin/nginxconf
    echo "vim /usr/local/nginx/conf/conf.d/virtual.conf" >/usr/bin/vhostconf ; chmod 700 /usr/bin/vhostconf
    echo "vim /usr/local/nginx/conf/php.conf" >/usr/bin/phpinc ; chmod 700 /usr/bin/phpinc
    echo "vim /usr/local/nginx/conf/drop.conf" >/usr/bin/dropinc ; chmod 700 /usr/bin/dropinc
    echo "vim /usr/local/nginx/conf/staticfiles.conf" >/usr/bin/statfilesinc ; chmod 700 /usr/bin/statfilesinc
else
    echo "nano -w /usr/local/lib/php.ini" > /usr/bin/phpedit ; chmod 700 /usr/bin/phpedit
    echo "nano -w /etc/my.cnf" >/usr/bin/mycnf ; chmod 700 /usr/bin/mycnf
    echo "nano -w /usr/local/etc/php-fpm.conf" >/usr/bin/fpmconf ; chmod 700 /usr/bin/fpmconf
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf2 ; chmod 700 /usr/bin/fpmconf2
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf3 ; chmod 700 /usr/bin/fpmconf3
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf4 ; chmod 700 /usr/bin/fpmconf4
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf5 ; chmod 700 /usr/bin/fpmconf5    
    echo "nano -w /usr/local/nginx/conf/nginx.conf" >/usr/bin/nginxconf ; chmod 700 /usr/bin/nginxconf
    echo "nano -w /usr/local/nginx/conf/conf.d/virtual.conf" >/usr/bin/vhostconf ; chmod 700 /usr/bin/vhostconf
    echo "nano -w /usr/local/nginx/conf/php.conf" >/usr/bin/phpinc ; chmod 700 /usr/bin/phpinc
    echo "nano -w /usr/local/nginx/conf/drop.conf" >/usr/bin/dropinc ; chmod 700 /usr/bin/dropinc
    echo "nano -w /usr/local/nginx/conf/staticfiles.conf" >/usr/bin/statfilesinc ; chmod 700 /usr/bin/statfilesinc
fi # USEEDITOR

# only run for CentOS 6.x
if [[ "$CENTOS_SEVEN" != '7' ]]; then
    echo "service nginx stop" >/usr/bin/ngxstop ; chmod 700 /usr/bin/ngxstop
    echo "service nginx start" >/usr/bin/ngxstart ; chmod 700 /usr/bin/ngxstart
    echo "service nginx restart" >/usr/bin/ngxrestart ; chmod 700 /usr/bin/ngxrestart
    echo "service nginx reload" >/usr/bin/ngxreload ; chmod 700 /usr/bin/ngxreload
    echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
    echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
    echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
    echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload

    if [ -f /etc/init.d/mysql ]; then
    
        echo "service mysql stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "service mysql start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "service mysql restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        echo "service mysql reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
    
    elif [ -f /etc/init.d/mysqld ]; then
    
        echo "service mysqld stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "service mysqld start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "service mysqld restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        echo "service mysqld reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
    
    fi
        echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
        echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
        echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
        echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload
    
    if [ -f /usr/local/bin/memcached ]; then
        echo "service memcached stop" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
        echo "service memcached start" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
        echo "service memcached restart" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
    fi
    
    if [ -f /usr/sbin/csf ]; then
        echo "service csf stop" >/usr/bin/csfstop ; chmod 700 /usr/bin/csfstop
        echo "service csf start" >/usr/bin/csfstart ; chmod 700 /usr/bin/csfstart
        echo "service csf restart" >/usr/bin/csfrestart ; chmod 700 /usr/bin/csfrestart
    fi
fi # CENTOS_SEVEN != 7

# only run for CentOS 7.x
if [[ "$CENTOS_SEVEN" = '7' ]]; then
    echo "service nginx stop" >/usr/bin/ngxstop ; chmod 700 /usr/bin/ngxstop
    echo "service nginx start" >/usr/bin/ngxstart ; chmod 700 /usr/bin/ngxstart
    echo "service nginx restart" >/usr/bin/ngxrestart ; chmod 700 /usr/bin/ngxrestart
    echo "service nginx reload" >/usr/bin/ngxreload ; chmod 700 /usr/bin/ngxreload
    echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
    echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
    echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
    echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload

    if [ -f /etc/init.d/mysql ]; then
    
        echo "service mysql stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "service mysql start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "service mysql restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        echo "service mysql reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
    
    elif [ -f /etc/init.d/mysqld ]; then
    
        echo "service mysqld stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "service mysqld start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "service mysqld restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        echo "service mysqld reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
    
    fi
        echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
        echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
        echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
        echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload
    
    if [ -f /usr/local/bin/memcached ]; then
        echo "service memcached stop" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
        echo "service memcached start" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
        echo "service memcached restart" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
    fi
    
    if [ -f /usr/sbin/csf ]; then
        echo "service csf stop" >/usr/bin/csfstop ; chmod 700 /usr/bin/csfstop
        echo "service csf start" >/usr/bin/csfstart ; chmod 700 /usr/bin/csfstart
        echo "service csf restart" >/usr/bin/csfrestart ; chmod 700 /usr/bin/csfrestart
    fi
fi # CENTOS_SEVEN != 7
    
    SUHOSINCHECK=`grep 'extension=suhosin.so' /usr/local/lib/php.ini `
    if [ ! -z "$SUHOSINCHECK" ]; then
        echo "sed -i 's/;extension=suhosin.so/extension=suhosin.so/g' /usr/local/lib/php.ini; fpmrestart; php -v" >/usr/bin/suhosinon ; chmod 700 /usr/bin/suhosinon
        echo "sed -i 's/extension=suhosin.so/;extension=suhosin.so/g' /usr/local/lib/php.ini; fpmrestart; php -v" >/usr/bin/suhosinoff ; chmod 700 /usr/bin/suhosinoff
    fi
    
    if [ -f /usr/bin/glances ]; then
        echo "alias top2=\"glances\"" >> /root/.bashrc
        alias top2="glances"
    fi

fi

}