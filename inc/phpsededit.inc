phpsededit() {
    if [[ "$(date +"%Z")" = 'EST' ]]; then
    sed -i "s/;date.timezone =/date.timezone = Australia\/Brisbane/g" /usr/local/lib/php.ini
    else
    sed -i "s/;date.timezone =/date.timezone = UTC/g" /usr/local/lib/php.ini
    fi

    sed -i 's/max_execution_time = 30/max_execution_time = 60/g' /usr/local/lib/php.ini
    sed -i 's/short_open_tag = Off/short_open_tag = On/g' /usr/local/lib/php.ini
    sed -i 's/;realpath_cache_size = 16k/realpath_cache_size = 4096k/g' /usr/local/lib/php.ini
    sed -i 's/;realpath_cache_ttl = 120/realpath_cache_ttl = 360/g' /usr/local/lib/php.ini
    sed -i 's/upload_max_filesize = 2M/upload_max_filesize = 15M/g' /usr/local/lib/php.ini
    #sed -i 's/memory_limit = 128M/memory_limit = 256M/g' /usr/local/lib/php.ini
    sed -i 's/post_max_size = 8M/post_max_size = 15M/g' /usr/local/lib/php.ini
    sed -i 's/expose_php = On/expose_php = Off/g' /usr/local/lib/php.ini
    #sed -i 's/;cgi.fix_pathinfo=1/cgi.fix_pathinfo=0/g' /usr/local/lib/php.ini
    #sed -i 's/disable_functions =/disable_functions=exec,passthru,shell_exec,system,proc_open,popen/g' /usr/local/lib/php.ini
    sed -i 's/mail.add_x_header = On/mail.add_x_header = Off/g' /usr/local/lib/php.ini

    sed -i 's/;max_input_nesting_level = 64/max_input_nesting_level = 128/g' /usr/local/lib/php.ini
    sed -i 's/; max_input_vars = 1000/max_input_vars = 2000/g' /usr/local/lib/php.ini
    sed -i 's/;mysqlnd.net_cmd_buffer_size = 2048/mysqlnd.net_cmd_buffer_size = 16384/g' /usr/local/lib/php.ini

# for mysqlnd native driver default mysql.sock configuration
#echo "mysql.default_socket=/var/lib/mysql/mysql.sock" > /root/centminmod/php.d/mysqlsocket.ini
#echo "mysqli.default_socket=/var/lib/mysql/mysql.sock" > /root/centminmod/php.d/mysqlsocket.ini

}