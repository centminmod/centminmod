phpsededit() {
    PHPINICUSTOM='a_customphp.ini'
    CUSTOMPHPINIFILE="${CONFIGSCANDIR}/${PHPINICUSTOM}"

    if [[ ! -f "${CUSTOMPHPINIFILE}" ]]; then
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
        ZOLIMIT='32'
        PHP_MEMORYLIMIT='48M'
        PHP_UPLOADLIMIT='48M'
        PHP_REALPATHLIMIT='256k'
        PHP_REALPATHTTL='14400'
    elif [[ "$TOTALMEM" -gt '262144' && "$TOTALMEM" -le '393216' ]]; then
        ZOLIMIT='80'
        PHP_MEMORYLIMIT='96M'
        PHP_UPLOADLIMIT='96M'
        PHP_REALPATHLIMIT='320k'
        PHP_REALPATHTTL='21600'
    elif [[ "$TOTALMEM" -gt '393216' && "$TOTALMEM" -le '524288' ]]; then
        ZOLIMIT='112'
        PHP_MEMORYLIMIT='128M'
        PHP_UPLOADLIMIT='128M'
        PHP_REALPATHLIMIT='384k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '524288' && "$TOTALMEM" -le '1049576' ]]; then
        ZOLIMIT='144'
        PHP_MEMORYLIMIT='160M'
        PHP_UPLOADLIMIT='160M'
        PHP_REALPATHLIMIT='384k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '1049576' && "$TOTALMEM" -le '2097152' ]]; then
        ZOLIMIT='240'
        PHP_MEMORYLIMIT='256M'
        PHP_UPLOADLIMIT='256M'
        PHP_REALPATHLIMIT='384k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '2097152' && "$TOTALMEM" -le '3145728' ]]; then
        ZOLIMIT='304'
        PHP_MEMORYLIMIT='320M'
        PHP_UPLOADLIMIT='320M'
        PHP_REALPATHLIMIT='512k'
        PHP_REALPATHTTL='43200'
    elif [[ "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
        ZOLIMIT='496'
        PHP_MEMORYLIMIT='512M'
        PHP_UPLOADLIMIT='512M'
        PHP_REALPATHLIMIT='512k'
        PHP_REALPATHTTL='43200'
    elif [[ "$TOTALMEM" -gt '4194304' ]]; then
        ZOLIMIT='784'
        PHP_MEMORYLIMIT='800M'
        PHP_UPLOADLIMIT='800M'
        PHP_REALPATHLIMIT='640k'
        PHP_REALPATHTTL='86400'
    fi

    echo "max_execution_time = 60" >> ${CUSTOMPHPINIFILE}
    echo "short_open_tag = On" >> ${CUSTOMPHPINIFILE}
    echo "realpath_cache_size = $PHP_REALPATHLIMIT" >> ${CUSTOMPHPINIFILE}
    echo "realpath_cache_ttl = $PHP_REALPATHTTL" >> ${CUSTOMPHPINIFILE}
    echo "upload_max_filesize = $PHP_UPLOADLIMIT" >> ${CUSTOMPHPINIFILE}
    echo "memory_limit = $PHP_MEMORYLIMIT" >> ${CUSTOMPHPINIFILE}
    echo "post_max_size = $PHP_UPLOADLIMIT" >> ${CUSTOMPHPINIFILE}
    echo "expose_php = Off" >> ${CUSTOMPHPINIFILE}
    echo "mail.add_x_header = Off" >> ${CUSTOMPHPINIFILE}
    echo "max_input_nesting_level = 128" >> ${CUSTOMPHPINIFILE}
    echo "max_input_vars = 10000" >> ${CUSTOMPHPINIFILE}
    echo "mysqlnd.net_cmd_buffer_size = 16384" >> ${CUSTOMPHPINIFILE}
    if [[ "$PHPMUVER" = '5.6' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPMVER" = '5.6' || "$PHPMVER" = '5.7' || "$PHPMVER" = '7.0' || "$PHPMVER" = '7.1' ]]; then
        echo "always_populate_raw_post_data=-1" >> ${CUSTOMPHPINIFILE}
    else
        echo ";always_populate_raw_post_data=-1" >> ${CUSTOMPHPINIFILE}
    fi
    #echo "cgi.fix_pathinfo=0" >> ${CUSTOMPHPINIFILE}
    #echo "disable_functions=exec,passthru,shell_exec,system,proc_open,popen" >> ${CUSTOMPHPINIFILE}

    # https://curl.haxx.se/docs/caextract.html
    # http://php.net/manual/en/curl.configuration.php#ini.curl.cainfo
    if [ ! -f /etc/ssl/certs/cacert.pem ]; then
        wget -q -O /etc/ssl/certs/cacert.pem $CURL_CACERTURL
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
        if [[ "$PHPMUVER" = '5.6' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPMVER" = '5.6' || "$PHPMVER" = '5.7' || "$PHPMVER" = '7.0' || "$PHPMVER" = '7.1' ]]; then
            # echo "openssl.cafile = '/etc/ssl/certs/cacert.pem'" >> ${CONFIGSCANDIR}/curlcainfo.ini
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        else
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        fi
    elif [[ "$(find /etc/ssl/certs/cacert.pem -mtime +90 >/dev/null 2>&1; echo $?)" = '0' && -f /etc/ssl/certs/cacert.pem ]]; then
        # redownload if older than 90 days
        wget -q -O /etc/ssl/certs/cacert.pem $CURL_CACERTURL
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
        if [[ "$PHPMUVER" = '5.6' || "$PHPMUVER" = '5.7' || "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPMVER" = '5.6' || "$PHPMVER" = '5.7' || "$PHPMVER" = '7.0' || "$PHPMVER" = '7.1' ]]; then
            # echo "openssl.cafile = '/etc/ssl/certs/cacert.pem'" >> ${CONFIGSCANDIR}/curlcainfo.ini
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        else
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        fi
    fi

    if [[ "$INITIALINSTALL" = [yY] ]]; then
        if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
            sed -i "s|opcache.memory_consumption=.*|opcache.memory_consumption=$ZOLIMIT|" "${CONFIGSCANDIR}/zendopcache.ini"
        fi
    fi

}