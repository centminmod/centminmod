phpsededit() {
    PHPINICUSTOM='a_customphp.ini'
    CUSTOMPHPINIFILE="${CONFIGSCANDIR}/${PHPINICUSTOM}"

    if [[ ! -f "${CUSTOMPHPINIFILE}" ]; then
        touch ${CUSTOMPHPINIFILE}
    else
        \cp -a ${CUSTOMPHPINIFILE} ${CUSTOMPHPINIFILE}-bak_$DT
        rm -rf $CUSTOMPHPINIFILE
        rm -rf ${CONFIGSCANDIR}/custom_php.ini
        echo "" > ${CUSTOMPHPINIFILE}
    fi

    if [[ "$(date +"%Z")" = 'EST' ]]; then
        echo "date.timezone = Australia/Brisbane" >> ${CUSTOMPHPINIFILE}
    else
        echo "date.timezone = UTC" >> ${CUSTOMPHPINIFILE}
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

    echo "max_execution_time = 60" >> ${CUSTOMPHPINIFILE}
    echo "short_open_tag = On" >> ${CUSTOMPHPINIFILE}
    echo "realpath_cache_size = 8192k" >> ${CUSTOMPHPINIFILE}
    echo "realpath_cache_ttl = 600" >> ${CUSTOMPHPINIFILE}
    echo "upload_max_filesize = 20M" >> ${CUSTOMPHPINIFILE}
    echo "memory_limit = $PHP_MEMORYLIMIT" >> ${CUSTOMPHPINIFILE}
    echo "post_max_size = 20M" >> ${CUSTOMPHPINIFILE}
    echo "expose_php = Off" >> ${CUSTOMPHPINIFILE}
    echo "mail.add_x_header = Off" >> ${CUSTOMPHPINIFILE}
    echo "max_input_nesting_level = 128" >> ${CUSTOMPHPINIFILE}
    echo "max_input_vars = 2000" >> ${CUSTOMPHPINIFILE}
    echo "mysqlnd.net_cmd_buffer_size = 16384" >> ${CUSTOMPHPINIFILE}
    #echo "cgi.fix_pathinfo=0" >> ${CUSTOMPHPINIFILE}
    #echo "disable_functions=exec,passthru,shell_exec,system,proc_open,popen" >> ${CUSTOMPHPINIFILE}

    if [ ! -f /etc/ssl/certs/cacert.pem ]; then
        wget -q -O /etc/ssl/certs/cacert.pem http://curl.haxx.se/ca/cacert.pem
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
    else
        wget -q -O /etc/ssl/certs/cacert.pem http://curl.haxx.se/ca/cacert.pem
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
    fi

}