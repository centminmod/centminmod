shortcutsinstall() {

#read -ep "Add script shortcuts (as shown in command_shortcuts.txt) ? [y/n] " shortcutask
if [[ "$SHORTCUTS" = [yY] ]]; then
    if [[ "$INITIALINSTALL" != [yY] && ! -f /usr/bin/customconfig ]]; then
        echo "Updating command shortcuts..."
    fi
    echo "**********************************************************************"
    echo "* Add cmd shortcuts for php.ini, my.cnf, php-fpm.conf, nginx.conf and virtual.conf "
    echo "* Edit /etc/centminmod/custom_config.inc = customconfig"
    echo "* Edit php.ini = phpedit "
    echo "* Edit my.cnf = mycnf "
    echo "* Edit php-fpm.conf = fpmconf "
    echo "* Edit nginx.conf = nginxconf "
    echo "* Edit virtual.conf = vhostconf "
    echo "* nginx stop/start/restart = ngxstop/ngxstart/ngxrestart "
    echo "* php-fpm stop/start/restart = fpmstop/fpmstart/fpmrestart "
    echo "* mysql stop/start/restart = mysqlstop/mysqlstart/mysqlrestart "
    echo "* nginx + php-fpm stop/start/restart = npstop/npstart/nprestart "
    echo "* memcached stop/start/restart = memcachedstop/memcachedstart/memcachedrestart "
    echo "* csf stop/start/restart = csfstop/csfstart/csfrestart "
    echo "**********************************************************************"

if [[ "$USEEDITOR" = 'vim' ]]; then
    echo "vim /etc/centminmod/custom_config.inc" > /usr/bin/customconfig ; chmod 700 /usr/bin/customconfig
    echo "vim /usr/local/lib/php.ini" > /usr/bin/phpedit ; chmod 700 /usr/bin/phpedit
    echo "vim /etc/my.cnf" >/usr/bin/mycnf ; chmod 700 /usr/bin/mycnf
    echo "vim /usr/local/etc/php-fpm.conf" >/usr/bin/fpmconf ; chmod 700 /usr/bin/fpmconf
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
    echo "vim /usr/local/nginx/conf/nginx.conf" >/usr/bin/nginxconf ; chmod 700 /usr/bin/nginxconf
    echo "vim /usr/local/nginx/conf/conf.d/virtual.conf" >/usr/bin/vhostconf ; chmod 700 /usr/bin/vhostconf
    echo "vim /usr/local/nginx/conf/php.conf" >/usr/bin/phpinc ; chmod 700 /usr/bin/phpinc
    echo "vim /usr/local/nginx/conf/drop.conf" >/usr/bin/dropinc ; chmod 700 /usr/bin/dropinc
    echo "vim /usr/local/nginx/conf/staticfiles.conf" >/usr/bin/statfilesinc ; chmod 700 /usr/bin/statfilesinc
else
    echo "nano -w /etc/centminmod/custom_config.inc" > /usr/bin/customconfig ; chmod 700 /usr/bin/customconfig
    echo "nano -w /usr/local/lib/php.ini" > /usr/bin/phpedit ; chmod 700 /usr/bin/phpedit
    echo "nano -w /etc/my.cnf" >/usr/bin/mycnf ; chmod 700 /usr/bin/mycnf
    echo "nano -w /usr/local/etc/php-fpm.conf" >/usr/bin/fpmconf ; chmod 700 /usr/bin/fpmconf
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
    echo "nano -w /usr/local/nginx/conf/nginx.conf" >/usr/bin/nginxconf ; chmod 700 /usr/bin/nginxconf
    echo "nano -w /usr/local/nginx/conf/conf.d/virtual.conf" >/usr/bin/vhostconf ; chmod 700 /usr/bin/vhostconf
    echo "nano -w /usr/local/nginx/conf/php.conf" >/usr/bin/phpinc ; chmod 700 /usr/bin/phpinc
    echo "nano -w /usr/local/nginx/conf/drop.conf" >/usr/bin/dropinc ; chmod 700 /usr/bin/dropinc
    echo "nano -w /usr/local/nginx/conf/staticfiles.conf" >/usr/bin/statfilesinc ; chmod 700 /usr/bin/statfilesinc
fi # USEEDITOR

# only run for CentOS 6.x
if [[ "$CENTOS_SIX" = '6' ]]; then
    # replicate the default aliases for centos 7 on
    # centos 6 systems
    if [[ ! "$(egrep -w 'egrep|fgrep|grep' /root/.bashrc)" ]]; then
        echo "alias egrep='egrep --color=auto'" >> /root/.bashrc
        echo "alias fgrep='fgrep --color=auto'" >> /root/.bashrc
        echo "alias grep='grep --color=auto'" >> /root/.bashrc
        alias egrep='egrep --color=auto'
        alias fgrep='fgrep --color=auto'
        alias grep='grep --color=auto'
    fi
    if [[ "$(id -u)" -ne '0' ]]; then
        if [[ ! "$(egrep -w 'egrep|fgrep|grep' $HOME/.bashrc)" ]]; then
            echo "alias egrep='egrep --color=auto'" >> $HOME/.bashrc
            echo "alias fgrep='fgrep --color=auto'" >> $HOME/.bashrc
            echo "alias grep='grep --color=auto'" >> $HOME/.bashrc
            alias egrep='egrep --color=auto'
            alias fgrep='fgrep --color=auto'
            alias grep='grep --color=auto'
        fi
    fi

    echo "service nginx stop" >/usr/bin/ngxstop ; chmod 700 /usr/bin/ngxstop
    echo "service nginx start" >/usr/bin/ngxstart ; chmod 700 /usr/bin/ngxstart
    echo "service nginx restart" >/usr/bin/ngxrestart ; chmod 700 /usr/bin/ngxrestart
    echo "service nginx reload" >/usr/bin/ngxreload ; chmod 700 /usr/bin/ngxreload
    echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
    echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
    echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
    echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
    echo "/etc/init.d/php-fpm configtest" >/usr/bin/fpmconfigtest ; chmod 700 /usr/bin/fpmconfigtest
    echo "/etc/init.d/php-fpm status" >/usr/bin/fpmstatus ; chmod 700 /usr/bin/fpmstatus

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
        if [[ "$CENTOS_SIX" -eq '6' ]]; then
            echo "/etc/init.d/memcached stop" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
            echo "/etc/init.d/memcached start" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
            echo "/etc/init.d/memcached restart" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
            if [[ "$USEEDITOR" = 'vim' ]]; then
                echo "vim /etc/init.d/memcached" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            else
                echo "nano /etc/init.d/memcached" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            fi
        fi
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
    if [[ "$SWITCH_PHPFPM_SYSTEMD" != [yY] && ! -f /usr/lib/systemd/system/php-fpm.service ]]; then
        echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
        echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
        echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
        echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
        echo "/etc/init.d/php-fpm configtest" >/usr/bin/fpmconfigtest ; chmod 700 /usr/bin/fpmconfigtest
    elif [[ "$SWITCH_PHPFPM_SYSTEMD" = [yY] && -f /usr/lib/systemd/system/php-fpm.service ]]; then
        echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
        echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
        echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
        echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
    fi

    if [[ -f /etc/init.d/mysql || -f /usr/lib/systemd/system/mariadb.service ]]; then
    
        echo "systemctl daemon-reload; service mysql stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "systemctl daemon-reload; service mysql start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "systemctl daemon-reload; service mysql restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        if [[ "$MARIADB_INSTALLTENTHREE" != [yY] || "$(/usr/bin/mysql_config --version | cut -d . -f1,2)" != '10.3' ]]; then
            echo "systemctl daemon-reload; service mysql reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
        fi
    
    elif [ -f /etc/init.d/mysqld ]; then
    
        echo "systemctl daemon-reload; service mysqld stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "systemctl daemon-reload; service mysqld start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "systemctl daemon-reload; service mysqld restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        if [[ "$MARIADB_INSTALLTENTHREE" != [yY] || "$(/usr/bin/mysql_config --version | cut -d . -f1,2)" != '10.3' ]]; then
            echo "systemctl daemon-reload; service mysqld reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
        fi
    
    fi
        if [[ "$SWITCH_PHPFPM_SYSTEMD" != [yY] && ! -f /usr/lib/systemd/system/php-fpm.service ]]; then
            echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
            echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
            echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
            echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload
        elif [[ "$SWITCH_PHPFPM_SYSTEMD" = [yY] && -f /usr/lib/systemd/system/php-fpm.service ]]; then
            echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
            echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
            echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
            echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload
        fi
    
    if [ -f /usr/local/bin/memcached ]; then
        if [[ "$CENTOS_SEVEN" -eq '7' ]]; then
            echo "/etc/init.d/memcached stop" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
            echo "/etc/init.d/memcached start" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
            echo "/etc/init.d/memcached restart" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
            if [[ "$USEEDITOR" = 'vim' ]]; then
                echo "vim /etc/init.d/memcached" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            else
                echo "nano /etc/init.d/memcached" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            fi
        fi
    fi
    
    if [ -f /usr/sbin/csf ]; then
        echo "service csf stop" >/usr/bin/csfstop ; chmod 700 /usr/bin/csfstop
        echo "service csf start" >/usr/bin/csfstart ; chmod 700 /usr/bin/csfstart
        echo "service csf restart" >/usr/bin/csfrestart ; chmod 700 /usr/bin/csfrestart
    fi
fi # CENTOS_SEVEN != 7

if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
    echo "service nginx stop" >/usr/bin/ngxstop ; chmod 700 /usr/bin/ngxstop
    echo "service nginx start" >/usr/bin/ngxstart ; chmod 700 /usr/bin/ngxstart
    echo "service nginx restart" >/usr/bin/ngxrestart ; chmod 700 /usr/bin/ngxrestart
    echo "service nginx reload" >/usr/bin/ngxreload ; chmod 700 /usr/bin/ngxreload
    if [[ "$SWITCH_PHPFPM_SYSTEMD" != [yY] && ! -f /usr/lib/systemd/system/php-fpm.service ]]; then
        echo "service php-fpm stop" >/usr/bin/fpmstop ; chmod 700 /usr/bin/fpmstop
        echo "service php-fpm start" >/usr/bin/fpmstart ; chmod 700 /usr/bin/fpmstart
        echo "service php-fpm restart" >/usr/bin/fpmrestart ; chmod 700 /usr/bin/fpmrestart
        echo "service php-fpm reload" >/usr/bin/fpmreload ; chmod 700 /usr/bin/fpmreload
        echo "/etc/init.d/php-fpm configtest" >/usr/bin/fpmconfigtest ; chmod 700 /usr/bin/fpmconfigtest
    fi

    if [[ -f /etc/init.d/mysql || -f /usr/lib/systemd/system/mariadb.service ]]; then
    
        echo "systemctl daemon-reload; service mariadb stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "systemctl daemon-reload; service mariadb start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "systemctl daemon-reload; service mariadb restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        echo "systemctl daemon-reload; systemctl status mariadb --no-pager" >/usr/bin/mysqlstatus ; chmod 700 /usr/bin/mysqlstatus
        # if [[ "$MARIADB_INSTALLTENTHREE" != [yY] || "$(/usr/bin/mysql_config --version | cut -d . -f1,2)" != '10.3' ]]; then
        #     echo "systemctl daemon-reload; service mariadb reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
        # fi
    
    elif [ -f /etc/init.d/mysqld ]; then
    
        echo "systemctl daemon-reload; service mysqld stop" >/usr/bin/mysqlstop ; chmod 700 /usr/bin/mysqlstop
        echo "systemctl daemon-reload; service mysqld start" >/usr/bin/mysqlstart ; chmod 700 /usr/bin/mysqlstart
        echo "systemctl daemon-reload; service mysqld restart" >/usr/bin/mysqlrestart ; chmod 700 /usr/bin/mysqlrestart
        if [[ "$MARIADB_INSTALLTENTHREE" != [yY] || "$(/usr/bin/mysql_config --version | cut -d . -f1,2)" != '10.3' ]]; then
            echo "systemctl daemon-reload; service mysqld reload" >/usr/bin/mysqlreload ; chmod 700 /usr/bin/mysqlreload
        fi
    
    fi
        if [[ "$SWITCH_PHPFPM_SYSTEMD" != [yY] && ! -f /usr/lib/systemd/system/php-fpm.service ]]; then
            echo "service nginx stop;service php-fpm stop" >/usr/bin/npstop ; chmod 700 /usr/bin/npstop
            echo "service nginx start;service php-fpm start" >/usr/bin/npstart ; chmod 700 /usr/bin/npstart
            echo "service nginx restart;service php-fpm restart" >/usr/bin/nprestart ; chmod 700 /usr/bin/nprestart
            echo "service nginx reload;service php-fpm reload" >/usr/bin/npreload ; chmod 700 /usr/bin/npreload
        fi
    
    if [ -f /usr/local/bin/memcached ]; then
        if [[ "$CENTOS_EIGHT" -eq '8' ]]; then
            echo "systemctl stop memcached" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
            echo "systemctl start memcached" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
            echo "systemctl restart memcached" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
            if [[ "$USEEDITOR" = 'vim' ]]; then
                echo "vim /usr/lib/systemd/system/memcached.service" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            else
                echo "nano /usr/lib/systemd/system/memcached.service" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            fi
            if [ -f /usr/bin/memflush ]; then
                echo "memflush --quiet --servers=localhost:11211" >/usr/bin/memcachedflush ; chmod 700 /usr/bin/memcachedflush
            fi
        elif [[ "$CENTOS_NINE" -eq '9' ]]; then
            echo "systemctl stop memcached" >/usr/bin/memcachedstop ; chmod 700 /usr/bin/memcachedstop
            echo "systemctl start memcached" >/usr/bin/memcachedstart ; chmod 700 /usr/bin/memcachedstart
            echo "systemctl restart memcached" >/usr/bin/memcachedrestart ; chmod 700 /usr/bin/memcachedrestart
            if [[ "$USEEDITOR" = 'vim' ]]; then
                echo "vim /usr/lib/systemd/system/memcached.service" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            else
                echo "nano /usr/lib/systemd/system/memcached.service" >/usr/bin/memcachedconfig ; chmod 700 /usr/bin/memcachedconfig
            fi
            if [ -f /usr/bin/memflush ]; then
                echo "memflush --quiet --servers=localhost:11211" >/usr/bin/memcachedflush ; chmod 700 /usr/bin/memcachedflush
            fi
        fi
    fi
    
    if [ -f /usr/sbin/csf ]; then
        echo "service csf stop" >/usr/bin/csfstop ; chmod 700 /usr/bin/csfstop
        echo "service csf start" >/usr/bin/csfstart ; chmod 700 /usr/bin/csfstart
        echo "service csf restart" >/usr/bin/csfrestart ; chmod 700 /usr/bin/csfrestart
    fi
fi
    
    SUHOSINCHECK=$(grep 'extension=suhosin.so' /usr/local/lib/php.ini)
    if [ ! -z "$SUHOSINCHECK" ]; then
        echo "sed -i 's/;extension=suhosin.so/extension=suhosin.so/g' /usr/local/lib/php.ini; fpmrestart; php -v" >/usr/bin/suhosinon ; chmod 700 /usr/bin/suhosinon
        echo "sed -i 's/extension=suhosin.so/;extension=suhosin.so/g' /usr/local/lib/php.ini; fpmrestart; php -v" >/usr/bin/suhosinoff ; chmod 700 /usr/bin/suhosinoff
    fi
    
    if [ -f /usr/bin/glances ]; then
        if [[ ! "$(grep -w 'glances' /root/.bashrc)" ]]; then
            echo "alias top2=\"glances\"" >> /root/.bashrc
            alias top2="glances"
        fi
        if [[ "$(id -u)" -ne '0' ]]; then
            if [[ ! "$(grep -w 'glances' $HOME/.bashrc)" ]]; then
                echo "alias top2=\"glances\"" >> $HOME/.bashrc
                alias top2="glances"
            fi
        fi
    fi

fi

}