phpsededit() {
    if [ ! -f ${CONFIGSCANDIR}/custom_php.ini ]; then
        touch ${CONFIGSCANDIR}/custom_php.ini
    else
        \cp -a ${CONFIGSCANDIR}/custom_php.ini ${CONFIGSCANDIR}/custom_php.ini-bak_$DT
        echo "" > ${CONFIGSCANDIR}/custom_php.ini
    fi

    if [[ "$(date +"%Z")" = 'EST' ]]; then
        echo "date.timezone = Australia/Brisbane" >> ${CONFIGSCANDIR}/custom_php.ini
    else
        echo "date.timezone = UTC" >> ${CONFIGSCANDIR}/custom_php.ini
    fi

    # dynamic PHP memory_limit calculation
    if [[ "$TOTALMEM" -le '262144' ]]; then
        PHP_MEMORYLIMIT='48M'
    elif [[ "$TOTALMEM" -gt '262144' && "$TOTALMEM" -le '393216' ]]; then
        PHP_MEMORYLIMIT='96M'
    elif [[ "$TOTALMEM" -gt '393216' && "$TOTALMEM" -le '524288' ]]; then
        PHP_MEMORYLIMIT='128M'
    elif [[ "$TOTALMEM" -gt '524288' && "$TOTALMEM" -le '1049576' ]]; then
        PHP_MEMORYLIMIT='160M'
    elif [[ "$TOTALMEM" -gt '1049576' && "$TOTALMEM" -le '2097152' ]]; then
        PHP_MEMORYLIMIT='256M'
    elif [[ "$TOTALMEM" -gt '2097152' && "$TOTALMEM" -le '3145728' ]]; then
        PHP_MEMORYLIMIT='320M'
    elif [[ "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
        PHP_MEMORYLIMIT='512M'
    elif [[ "$TOTALMEM" -gt '4194304' ]]; then
        PHP_MEMORYLIMIT='640M'
    fi

    echo "max_execution_time = 60" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "short_open_tag = On" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "realpath_cache_size = 8192k" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "realpath_cache_ttl = 600" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "upload_max_filesize = 20M" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "memory_limit = $PHP_MEMORYLIMIT" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "post_max_size = 20M" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "expose_php = Off" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "mail.add_x_header = Off" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "max_input_nesting_level = 128" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "max_input_vars = 2000" >> ${CONFIGSCANDIR}/custom_php.ini
    echo "mysqlnd.net_cmd_buffer_size = 16384" >> ${CONFIGSCANDIR}/custom_php.ini
    #echo "cgi.fix_pathinfo=0" >> ${CONFIGSCANDIR}/custom_php.ini
    #echo "disable_functions=exec,passthru,shell_exec,system,proc_open,popen" >> ${CONFIGSCANDIR}/custom_php.ini

    if [ ! -f /etc/ssl/certs/cacert.pem ]; then
        wget -q -O /etc/ssl/certs/cacert.pem http://curl.haxx.se/ca/cacert.pem
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
    else
        wget -q -O /etc/ssl/certs/cacert.pem http://curl.haxx.se/ca/cacert.pem
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
    fi

}