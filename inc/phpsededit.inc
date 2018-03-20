phptuning() {
    # more tuning for php-fpm
    if [[ "${PHP_TUNING}" = [yY] && -f /usr/local/etc/php-fpm.conf ]]; then
        PHP_CPUCOUNT=$(grep -c "processor" /proc/cpuinfo)
        if [[ "${PHP_CPUCOUNT}" -ge '4' && "${PHP_CPUCOUNT}" -lt '6' && "$TOTALMEM" -gt '1049576' && "$TOTALMEM" -le '2097152' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '4' && "${PHP_CPUCOUNT}" -lt '6' && "$TOTALMEM" -gt '2097152' && "$TOTALMEM" -le '3145728' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '4' && "${PHP_CPUCOUNT}" -lt '6' && "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '4' && "${PHP_CPUCOUNT}" -lt '6' && "$TOTALMEM" -gt '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '6' && "${PHP_CPUCOUNT}" -lt '8' && "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '6' && "${PHP_CPUCOUNT}" -lt '8' && "$TOTALMEM" -gt '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))            
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '8' && "${PHP_CPUCOUNT}" -lt '16' && "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))            
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '8' && "${PHP_CPUCOUNT}" -lt '12' && "$TOTALMEM" -gt '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))            
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '13' && "${PHP_CPUCOUNT}" -lt '16' && "$TOTALMEM" -gt '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 6-4-2-6 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 6))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 6))            
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        elif [[ "${PHP_CPUCOUNT}" -ge '17' && "$TOTALMEM" -gt '4194304' ]]; then
            # min_spare_servers + (max_spare_servers - min_spare_servers) / 2
            # 5-4-2-5 formula
            PHP_MAXCHILDREN=$((${PHP_CPUCOUNT} * 5))
            PHP_STARTSERVERS=$((${PHP_CPUCOUNT} * 4))
            PHP_MINSPARESERVERS=$((${PHP_CPUCOUNT} * 2))
            PHP_MAXSPARESERVERS=$((${PHP_CPUCOUNT} * 5))            
            sed -i "s|pm.max_children = .*|pm.max_children = ${PHP_MAXCHILDREN}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.start_servers = .*|pm.start_servers = ${PHP_STARTSERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.min_spare_servers = .*|pm.min_spare_servers = ${PHP_MINSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_spare_servers = .*|pm.max_spare_servers = ${PHP_MAXSPARESERVERS}|" /usr/local/etc/php-fpm.conf
            sed -i "s|pm.max_requests = .*|pm.max_requests = 2000|" /usr/local/etc/php-fpm.conf
        fi
    fi
}

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
        PHP_REALPATHLIMIT='512k'
        PHP_REALPATHTTL='14400'
    elif [[ "$TOTALMEM" -gt '262144' && "$TOTALMEM" -le '393216' ]]; then
        ZOLIMIT='80'
        PHP_MEMORYLIMIT='96M'
        PHP_UPLOADLIMIT='96M'
        PHP_REALPATHLIMIT='640k'
        PHP_REALPATHTTL='21600'
    elif [[ "$TOTALMEM" -gt '393216' && "$TOTALMEM" -le '524288' ]]; then
        ZOLIMIT='112'
        PHP_MEMORYLIMIT='128M'
        PHP_UPLOADLIMIT='128M'
        PHP_REALPATHLIMIT='768k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '524288' && "$TOTALMEM" -le '1049576' ]]; then
        ZOLIMIT='144'
        PHP_MEMORYLIMIT='160M'
        PHP_UPLOADLIMIT='160M'
        PHP_REALPATHLIMIT='768k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '1049576' && "$TOTALMEM" -le '2097152' ]]; then
        ZOLIMIT='240'
        PHP_MEMORYLIMIT='320M'
        PHP_UPLOADLIMIT='320M'
        PHP_REALPATHLIMIT='1536k'
        PHP_REALPATHTTL='28800'
    elif [[ "$TOTALMEM" -gt '2097152' && "$TOTALMEM" -le '3145728' ]]; then
        ZOLIMIT='304'
        PHP_MEMORYLIMIT='384M'
        PHP_UPLOADLIMIT='384M'
        PHP_REALPATHLIMIT='2048k'
        PHP_REALPATHTTL='43200'
    elif [[ "$TOTALMEM" -gt '3145728' && "$TOTALMEM" -le '4194304' ]]; then
        ZOLIMIT='496'
        PHP_MEMORYLIMIT='512M'
        PHP_UPLOADLIMIT='512M'
        PHP_REALPATHLIMIT='3072k'
        PHP_REALPATHTTL='43200'
    elif [[ "$TOTALMEM" -gt '4194304' ]]; then
        ZOLIMIT='784'
        PHP_MEMORYLIMIT='800M'
        PHP_UPLOADLIMIT='800M'
        PHP_REALPATHLIMIT='4096k'
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
    echo "mysqlnd.collect_memory_statistics = Off" >> ${CUSTOMPHPINIFILE}
    echo "mysqlnd.mempool_default_size = 16000" >> ${CUSTOMPHPINIFILE}
    if [[ "$PHPMUVER" > 5.5 || "$PHPMVER" > 5.5 ]]; then
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
        if [[ "$PHPMUVER" > 5.5 || "$PHPMVER" > 5.5 ]]; then
            # echo "openssl.cafile = '/etc/ssl/certs/cacert.pem'" >> ${CONFIGSCANDIR}/curlcainfo.ini
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        else
            sed -i '/openssl.cafile/d' ${CONFIGSCANDIR}/curlcainfo.ini
        fi
    elif [[ "$(find /etc/ssl/certs/cacert.pem -mtime +90 >/dev/null 2>&1; echo $?)" = '0' && -f /etc/ssl/certs/cacert.pem ]]; then
        # redownload if older than 90 days
        wget -q -O /etc/ssl/certs/cacert.pem $CURL_CACERTURL
        echo "curl.cainfo = '/etc/ssl/certs/cacert.pem'" > ${CONFIGSCANDIR}/curlcainfo.ini
        if [[ "$PHPMUVER" > 5.5 || "$PHPMVER" > 5.5 ]]; then
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