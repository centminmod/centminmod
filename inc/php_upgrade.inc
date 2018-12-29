########################################################################
autodetectextensions() {

# on minor PHP version updates, auto detect is skipped for these
# extensions recompilation to speed up php recompiles. However, if php
# extension fails to compile, there is currently no menu option to
# recompile mssql, mongodb and mailparse php extensions unless you
# switch PHP major versions i.e. go from PHP 5.6 to 7.0 or 7.0 to 7.1
# or 7.0 to 5.6. This quick workaround sets AUTODETECPHP_OVERRIDE='y'
# when PHPMYYSQL='y' or # PHPMONGODB='y' or PHPMAILPARSE='y' is set
# to enable these php extensions and php extension is detected as not
# loaded into php. So that even on mionor PHP version updates the
# extensions are recompiled.
if [[ "$PHPMSSQL" = [yY] && "$(php --ri mssql | grep -o 'not present')" = 'not present' ]] || [[ "$PHPMONGODB" = [yY] && "$(php --ri mongodb | grep -o 'not present')" = 'not present' ]] || [[ "$PHPMAILPARSE" = [yY] && "$(php --ri mailparse | grep -o 'not present')" = 'not present' ]]; then
    AUTODETECPHP_OVERRIDE='y'
fi

if [[ -f "${CONFIGSCANDIR}/mssql.ini" && "$PHPMSSQL" = [yY] ]]; then
    DETECTMSSQL=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTMSSQL=y
fi

if [ -f "${CONFIGSCANDIR}/xcache.ini" ]; then
    DETECTXCACHE=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTXCACHE=y
fi

if [ -f "${CONFIGSCANDIR}/apc.ini" ]; then
    DETECTAPC=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTAPC=y
fi

if [ -f "${CONFIGSCANDIR}/igbinary.ini" ]; then
    if [[ "$PHPMUVER" != 'NGDEBUG' ]]; then
        DETECTIGBINARY=y
    else
        # remove igbinary.ini when PHP NGDEBUG detected on upgrade
        DETECTIGBINARY=n
        echo "rm -rf ${CONFIGSCANDIR}/igbinary.ini"
        rm -rf "${CONFIGSCANDIR}/igbinary.ini"
    fi
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTIGBINARY=y
fi

if [[ -f "${CONFIGSCANDIR}/memcache.ini" || -f "${CONFIGSCANDIR}/memcached.ini" ]]; then
    if [[ "$PHPMUVER" != 'NGDEBUG' ]]; then
        DETECTMEMCACHED=y
    else
        # remove memcache.ini & memcached.ini when PHP NGDEBUG detected on upgrade
        DETECTMEMCACHED=n
        echo "rm -rf ${CONFIGSCANDIR}/memcache.ini"
        # echo "rm -rf ${CONFIGSCANDIR}/memcached.ini"
        rm -rf "${CONFIGSCANDIR}/memcache.ini"
        # rm -rf "${CONFIGSCANDIR}/memcached.ini"
    fi
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTMEMCACHED=y
fi

if [[ -f "${CONFIGSCANDIR}/redis.ini" || "$PHPREDIS" = [yY] ]]; then
    DETECTREDIS=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTREDIS=y
fi

if [[ -f "${CONFIGSCANDIR}/mongodb.ini" || "$PHPMONGODB" = [yY] ]]; then
    DETECTMONGODB=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTMONGODB=y
fi

if [[ -f "${CONFIGSCANDIR}/zopfli.ini" || "$PHPZOPFLI" = [yY] ]]; then
    DETECTZOPFLI=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTZOPFLI=y
fi

if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
    DETECTOPCACHE=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTOPCACHE=y
fi

if [ -f "${CONFIGSCANDIR}/mailparse.ini" ]; then
    DETECTMAILPARSE=y
    if [[ "$PHPMAILPARSE" != [yY] ]]; then
        echo "rm -rf ${CONFIGSCANDIR}/mailparse.ini"
        rm -rf "${CONFIGSCANDIR}/mailparse.ini"        
    fi
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTMAILPARSE=y
fi

if [ -f "${CONFIGSCANDIR}/imagick.ini" ]; then
    if [[ "$PHPMUVER" != 'NGDEBUG' ]]; then
        DETECTIMAGICK=y
    else
        # remove imagick.ini when PHP NGDEBUG detected on upgrade
        DETECTIMAGICK=n
        echo "rm -rf ${CONFIGSCANDIR}/imagick.ini"
        rm -rf "${CONFIGSCANDIR}/imagick.ini"
    fi
    if [[ "$PHPIMAGICK" != [yY] ]]; then
        echo "rm -rf ${CONFIGSCANDIR}/imagick.ini"
        rm -rf "${CONFIGSCANDIR}/imagick.ini"      
    fi
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTIMAGICK=y
fi

if [ -f "${CONFIGSCANDIR}/geoip.ini" ]; then
    DETECTGEOIP=y
else
    DETECTGEOIP=y
fi

}

########################################################################
autodetectinstallextensions() {

PHPEXTDIR_AUTODETECT=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Auto Detect Previous Extensions"
    fi

cecho "----------------------------------------------------" $boldgreen
cecho "Auto reinstalling previously detected PHP extensions" $boldyellow
cecho "----------------------------------------------------" $boldgreen

if [[ "$DETECTMSSQL" = [yY] ]] || [[ "$DETECTMSSQL" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/mssql.so" ]]; then
    cecho "Auto reinstalling previously detected PHP mssql extension" $boldyellow
    php_mssqlinstall
fi

if [[ "$DETECTXCACHE" = [yY] ]] || [[ "$DETECTXCACHE" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/xcache.so" ]]; then
    cecho "Auto reinstalling previously detected Xcache" $boldyellow
    funct_xcachereinstall silent
fi

if [[ "$DETECTAPC" = [yY] ]] || [[ "$DETECTAPC" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/apc.so" ]]; then
    cecho "Auto reinstalling previously detected APC Cache" $boldyellow
    funct_apcreinstall
fi

if [[ "$DETECTIGBINARY" = [yY] ]] || [[ "$DETECTIGBINARY" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/igbinary.so" ]]; then
    cecho "Auto reinstalling previously detected igbinary extension" $boldyellow
    funct_igbinaryinstall
fi

if [[ "$DETECTMEMCACHED" = [yY] ]] || [[ "$DETECTMEMCACHED" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/memcached.so" ]]; then
    cecho "Auto reinstalling previously detected memcached/memcache php extension" $boldyellow
    funct_memcachedreinstall
fi

if [[ "$DETECTREDIS" = [yY] ]] || [[ "$DETECTREDIS" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/redis.so" ]]; then
    cecho "Auto reinstalling previously detected redis PHP extension" $boldyellow
    redisinstall
fi

if [[ "$DETECTMONGODB" = [yY] ]] || [[ "$DETECTMONGODB" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/mongodb.so" ]]; then
    cecho "Auto reinstalling previously detected MongoDB PHP extension" $boldyellow
    mongodbinstall
fi

if [[ "$DETECTZOPFLI" = [yY] ]] || [[ "$DETECTZOPFLI" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/zopfli.so" ]]; then
    cecho "Auto reinstalling previously detected Zopfli PHP extension" $boldyellow
    zopfliinstall
fi

if [[ "$DETECTOPCACHE" = [yY] ]]; then
  # ZOPCACHE_OVERRIDE=y allows you to override PHP 5.5-7.x's inbuilt included
  # Zend Opcache version with one available from pecl site
  if [[ "$PHPMUVER" = 5.[234] || "$ZOPCACHE_OVERRIDE" = [yY] ]]; then
    cecho "Auto reinstalling previously detected Zend Opcache" $boldyellow
    zopcachereinstall
  fi
fi

if [[ "$DETECTMAILPARSE" = [yY] ]] || [[ "$DETECTMAILPARSE" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/mailparse.so" ]]; then
    cecho "Auto reinstalling previously detected mailparse extension" $boldyellow
    mailparseinstall
fi

if [[ "$DETECTIMAGICK" = [yY] ]] || [[ "$DETECTIMAGICK" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/imagick.so" ]]; then
    cecho "Auto reinstalling previously detected imagemagick extension" $boldyellow
    imagickinstall
fi

if [[ "$DETECTGEOIP" = [yY] ]] || [[ "$DETECTGEOIP" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/geoip.so" ]]; then
    cecho "Auto reinstalling previously detected geoip extension" $boldyellow
    geoipphpext
fi

# Fix extension directory on major PHP version changes 
# Zend Opcache related when downgrading PHP major versions
PHPEXTDIRDNEW=$(cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}')

OLD=$(echo $PHPEXTDIRDOLD | sed 's/\//\\\//g')
NEW=$(echo $PHPEXTDIRDNEW | sed 's/\//\\\//g')

for ini in "$(ls ${CONFIGSCANDIR}/ | grep .ini)"
 do 
    echo "$ini"
    sed -i "s/$OLD/$NEW/g" "${CONFIGSCANDIR}/${ini}" >/dev/null 2>&1
done

}

########################################################################
funct_phpupgrade() {
    axelsetup

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Upgrade"
    fi

    if [ "${MACHINE_TYPE}" == 'x86_64' ];
    then
        ARCH='x86_64'
    else
        ARCH='i386'
    fi

# Set LIBDIR
if [ "${ARCH}" == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

echo " "
if [[ "$UALL" = 'y' ]]; then 
    key='y'
else
    read -ep "PHP Upgrade/Downgrade - Would you like to continue? [y/n] " key
    if [[ "$key" = [nN] ]];
    then
        exit 0
    fi
fi # UALL

DIR_TMP="$DIR_TMP"
if [ ! -d "$DIR_TMP" ]; then
mkdir "$DIR_TMP"
fi

funct_mktempfile

if [[ "$UALL" = 'y' ]]; then 
    phpver=${PHP_VERSION}
    overwritephpfpmconf='n'
    lessphpmem='y'
else
    echo
    echo "----------------------------------------------------------------"
    echo "Install which version of PHP? (version i.e. ${PHP_VERSION}, 7.0.33, NGDEBUG)"
    echo "PHP 7.x/7.1.x/7.2.x/7.3.x is GA Stable but still may have broken PHP extensions."
    echo "NGDEBUG is PHP 7.4.0 dev builds minus incompatible PHP extensions"
    echo "----------------------------------------------------------------"
    read -ep "Enter PHP Version number you want to upgrade/downgrade to: " phpver
    echo "----------------------------------------------------------------"
    echo "existing php.ini will be backed up at /usr/local/lib/php.ini-oldversion_$DT"
    
    # only prompt for php-fpm.conf overwrite if the existing /usr/local/nginx/conf/php.conf file
    # differs from the current centmin mod template file
    if [[ -f /usr/local/nginx/conf/php.conf && -f "$CUR_DIR/config/nginx/php.conf" ]]; then
        diff -u /usr/local/nginx/conf/php.conf "$CUR_DIR/config/nginx/php.conf" &>/dev/null
        PHPCONF_DIFFCHECK=$?
    else
        PHPCONF_DIFFCHECK=1
    fi
    if [[ "$PHPCONF_DIFFCHECK" != '0' && "$PHP_OVERWRITECONF" = [yY] ]]; then
        echo "Want to update to latest php.conf ? (overwrites will auto backup existing php.conf)"
        echo "existing php.conf will be backed up at /usr/local/nginx/conf/php.conf-oldversion_$DT"
        echo "----------------------------------------------------------------"
        read -ep "Update & overwrite your existing php.conf [y/n]: " overwritephpfpmconf
        echo "----------------------------------------------------------------"
    else
        echo "----------------------------------------------------------------"
        overwritephpfpmconf=n
    fi
    
    # switch to auto calculate system memory instead of prompting for user input
    IS_PHPLOWMEM='2097152'
    CHECK_PHPLOWMEM=$(expr $TOTALMEM \< $IS_PHPLOWMEM)
    if [ "$CHECK_PHPLOWMEM" == "1" ]; then
        lessphpmem=y
    fi
fi # UALL

## grab newer custom written htpasswd.sh as well
gethtpasswdsh

PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)
PHPMU_ALPHAVER=$(echo "$phpver")

    phpng_download
    autodetectextensions
    zopcacheupgrade
    addmysql_legacy

    echo "*************************************************"
    cecho "* Upgrading PHP" $boldgreen
    echo "*************************************************"

    # if you set /etc/centminmod/custom_config.inc file to
    # PHP_UPDATEMAINTENANCE=y
    # then whenever you run php upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$PHP_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus off
        fi
    fi

funct_centos6check

  if [[ "$PHPMUVER" = 5.3 ]]; then
    if [ ! -f /usr/bin/autoconf-2.13 ]; then
        yum -y -q install autoconf213
    fi
    export PHP_AUTOCONF=/usr/bin/autoconf-2.13
    export PHP_AUTOHEADER=/usr/bin/autoheader-2.13
  else
    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader
  fi

    cd "$DIR_TMP"

if [ "$(rpm -qa | grep '^php*' | grep -v 'phonon-backend-gstreamer')" ]; then
    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
    echo "yum -y erase php*"
    yum -y erase php*
fi
    if [[ "$PHPNG_YES" != [yY] ]]; then
        phpgeolocation
    
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "PHP: Download"
        fi
    
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/get/php-${phpver}.tar.${PHPEXTSION}/from/this/mirror" | grep 'HTTP\/' | grep '404'
        PHPGEO_CURLCHECK=$?
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/get/php-${phpver}.tar.${PHPEXTSION}/from/this/mirror" | grep 'HTTP\/' | egrep '200|301|302'
        PHPGEO_CURLCHECKOK=$?
        # check if download url is 404 not found but also 
        # check for valid status header 200, 301 or 302
        if [[ "$PHPGEO_CURLCHECK" = '0' || "$PHPGEO_CURLCHECKOK" != '0' ]]; then
            PHP_MIRRORURL="http://php.net"
            PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
        fi
        if [ -s "php-${phpver}.tar.${PHPEXTSION}" ]; then
            cecho "php ${phpver} Archive found, skipping download..." $boldgreen
        else
            $DOWNLOADAPP "${PHP_MIRRORURL}/get/php-${phpver}.tar.${PHPEXTSION}/from/this/mirror" $AXELPHPUPGRADETARGZ $WGETRETRY
            ERROR=$?
            if [[ "$ERROR" != '0' ]]; then
                cecho "Error: php-${phpver}.tar.${PHPEXTSION} download failed." $boldgreen
                checklogdetails
                exit
            else 
                cecho "Download done." $boldyellow
                echo ""
            fi
        fi
    
        tar $PHPTAR_FLAGS "php-${phpver}.tar.${PHPEXTSION}"
    
        ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
            cecho "Error: php-${phpver}.tar.${PHPEXTSION} extraction failed." $boldgreen
            checklogdetails
            exit
        else 
            cecho "php-${phpver}.tar.${PHPEXTSION} valid file." $boldyellow
            echo ""
        fi
    fi

    cd "php-${phpver}"
    PHPVER_ID=$(awk '/PHP_VERSION_ID/ {print $3}' ${DIR_TMP}/php-${phpver}/main/php_version.h)
    echo "PHP VERSION ID: $PHPVER_ID"

##############################################
# begin update php.ini

echo ""
echo "*************************************************"
cecho "* Updating php.ini... " $boldgreen
echo "*************************************************"
echo "old php.ini will be saved at /usr/local/lib/php.ini-oldversion_$DT"
echo "if you have modified your old php.ini i.e. installed and loaded"
echo "APC, Xcache, Memcache or Suhosin - please re-install them after php upgrade"
echo "..."

    \cp -f /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT
    \cp -f php.ini-production /usr/local/lib/php.ini
    chmod 644 /usr/local/lib/php.ini

phpsededit
phptuning

PHPDIFF=$(diff /usr/local/lib/php.ini /usr/local/lib/php.ini-oldversion_$DT)
echo ""
if [ ! -z "$PHPDIFF" ]; then
echo "checking updated php.ini compared to old /usr/local/lib/php.ini-oldversion_$DT"
echo "please note the following differences: "
echo "if APC, Xcache, Memcache, Suhosin or FFMPEG Extensions are listed please re-install them"
echo ""
echo "--------------------------------------------------------------------------------------------"
diff /usr/local/lib/php.ini "/usr/local/lib/php.ini-oldversion_$DT"
echo "--------------------------------------------------------------------------------------------"
echo ""
    echo "*************************************************"
echo "Automatically continuing upgrade in 4 seconds..."
    echo "*************************************************"
sleep 4
echo ""
fi # PHPDIFF

# end update php.ini
##############################################

    make clean

    php_patches

    if [[ "$CENTOS_SIX" -eq '6' ]]; then
        # PHP 7.3.0 + centos 6 issue https://community.centminmod.com/posts/69561/
        if [ ! -f /usr/bin/autoconf268 ]; then
            echo "yum -q -y install autoconf268"
            yum -q -y install autoconf268
        fi
        if [ -f /usr/bin/autoconf268 ]; then
            export PHP_AUTOCONF=/usr/bin/autoconf268
            export PHP_AUTOHEADER=/usr/bin/autoheader268
        fi
    fi

    ./buildconf --force
    
    if [ ! -d fpm-build ]; then
    mkdir fpm-build 
    fi
    
    cd fpm-build

    make clean

    #mkdir -p /usr/${LIBDIR}/mysql
    #ln -s /usr/${LIBDIR}/libmysqlclient.so /usr/${LIBDIR}/mysql/libmysqlclient.so

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Configure"
    fi

if [[ "$TIME_PHPCONFIGURE" = [yY] ]]; then
    funct_phpconfigure 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
else
    funct_phpconfigure
fi

################
# error check

  ERR=$?
  if [ $ERR != 0 ]; then
      echo -e "\n`date`\nError: $ERR, PHP configure failed ($SCRIPT_VERSION)\n"
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi
      exit
  else
      echo -e "\n`date`\nSuccess: PHP configure ok\n"
        cmservice php-fpm restart
  fi

# error check
################

# separate make threads for PHP compiles
if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS_PHP=" -j$CPUS"
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
    else
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
else
    # speed up make
    CPUS_MAKEPHP=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS_MAKEPHP" -gt '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+2" | bc)
    elif [[ "$CPUS_MAKEPHP" -eq '8' ]]; then
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP+1" | bc)
    else
        CPUS_MAKEPHP=$(echo "$CPUS_MAKEPHP" | bc)
    fi
    MAKETHREADS_PHP=" -j$CPUS_MAKEPHP"
fi

if [[ "$(grep -c "processor" /proc/cpuinfo)" -lt '2' ]]; then
  # Disable PGO if less than 2 cpu threads
  PHP_PGO='n'
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Make"
    fi
    echo "PHPMUVER = $PHPMUVER"
    echo "PHP_PGO = $PHP_PGO"
    if [[ "$PHP_PGO" = [yY] ]] && [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' ]]; then
        echo "make${MAKETHREADS_PHP} prof-gen"
        time make${MAKETHREADS_PHP} prof-gen
        if [ ! -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            pushd "$DIR_TMP"
            time git clone https://github.com/01org/php_pgo_training_scripts php_pgo_training_scripts
            popd
            GET_ROOT_PASS=$(awk -F "=" '/password/ {print $2}' /root/.my.cnf | sed -e "s|'||g")
            sed -i "s|define('DB_PASSWORD', 'root');|define('DB_PASSWORD', '$GET_ROOT_PASS');|" "$DIR_TMP/php_pgo_training_scripts/constants.php"
            ./sapi/cli/php "$DIR_TMP/php_pgo_training_scripts/init.php"
            echo "time ./sapi/cgi/php-cgi -T80 ../Zend/bench.php"
            time ./sapi/cgi/php-cgi -T80 ../Zend/bench.php
            echo "time ./sapi/cgi/php-cgi -T40 ../Zend/micro_bench.php"
            time ./sapi/cgi/php-cgi -T40 ../Zend/micro_bench.php
            echo "./sapi/cgi/php-cgi -T100 "$DIR_TMP/php_pgo_training_scripts/index.php""
            time ./sapi/cgi/php-cgi -T100 "$DIR_TMP/php_pgo_training_scripts/index.php"
            if [ -f "$PHPPGO_INDEXPATH" ]; then
                echo "./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH""
                time ./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH"
            fi
            if [ -f "$PHPPGO_INDEXPATH_TWO" ]; then
                echo "./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_TWO""
                time ./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_TWO"
            fi
            if [ -f "$PHPPGO_INDEXPATH_THREE" ]; then
                echo "./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_THREE""
                time ./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_THREE"
            fi
            if [ -f "$PHPPGO_INDEXPATH_FOUR" ]; then
                echo "./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_FOUR""
                time ./sapi/cgi/php-cgi -T100 "$PHPPGO_INDEXPATH_FOUR"
            fi
        elif [ -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            pushd "$DIR_TMP/php_pgo_training_scripts"
            git stash
            git pull
            popd
            GET_ROOT_PASS=$(awk -F "=" '/password/ {print $2}' /root/.my.cnf | sed -e "s|'||g")
            sed -i "s|define('DB_PASSWORD', 'root');|define('DB_PASSWORD', '$GET_ROOT_PASS');|" "$DIR_TMP/php_pgo_training_scripts/constants.php"
            ./sapi/cli/php "$DIR_TMP/php_pgo_training_scripts/init.php"
            echo "time ./sapi/cgi/php-cgi -T2,${PHP_PGO_TRAINRUNS} ../Zend/bench.php"
            time ./sapi/cgi/php-cgi -T2,${PHP_PGO_TRAINRUNS} ../Zend/bench.php
            echo "time ./sapi/cgi/php-cgi -T2,${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php"
            time ./sapi/cgi/php-cgi -T2,${PHP_PGO_TRAINRUNS} ../Zend/micro_bench.php
            echo "./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php""
            time ./sapi/cgi/php-cgi -T${PHP_PGO_TRAINRUNS} "$DIR_TMP/php_pgo_training_scripts/index.php"
            if [ -f "$PHPPGO_INDEXPATH" ]; then
                echo "./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH""
                time ./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH"
            fi
            if [ -f "$PHPPGO_INDEXPATH_TWO" ]; then
                echo "./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_TWO""
                time ./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_TWO"
            fi
            if [ -f "$PHPPGO_INDEXPATH_THREE" ]; then
                echo "./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_THREE""
                time ./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_THREE"
            fi
            if [ -f "$PHPPGO_INDEXPATH_FOUR" ]; then
                echo "./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_FOUR""
                time ./sapi/cgi/php-cgi -T80 "$PHPPGO_INDEXPATH_FOUR"
            fi
        fi
        echo "make prof-clean"
        time make prof-clean
        echo "make${MAKETHREADS_PHP} prof-use"
        time make${MAKETHREADS_PHP} prof-use
    else
        echo "make${MAKETHREADS_PHP}"
        time make${MAKETHREADS_PHP}
    fi

################
# error check

  ERR=$?
    sar_call
  if [ $ERR != 0 ]; then
    echo -e "\n`date`\nError: $ERR, PHP make failed ($SCRIPT_VERSION)\n"
        free -m
        echo
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi
      exit
  else
      echo -e "\n`date`\nSuccess: PHP make ok\n"
        cmservice php-fpm restart
  fi

# error check
################

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Make Install"
    fi

echo "make install"
    time make install

if [[ "$STRIPPHP" = [yY] ]]; then
    echo
    echo "strip PHP binary..."
    ls -lah /usr/local/bin/php
    ls -lah /usr/local/bin/php-cgi
    ls -lah /usr/local/sbin/php-fpm
    echo
    echo "backup before strip PHP binary..."
    \cp -af /usr/local/bin/php /usr/local/bin/php-b4strip 
    \cp -af /usr/local/bin/php-cgi /usr/local/bin/php-cgi-b4strip 
    \cp -af /usr/local/sbin/php-fpm /usr/local/sbin/php-fpm-b4strip 
    strip -s /usr/local/bin/php
    strip -s /usr/local/bin/php-cgi
    strip -s /usr/local/sbin/php-fpm
    echo
    echo "after strip PHP binary..."
    ls -lah /usr/local/bin/php
    ls -lah /usr/local/bin/php-cgi
    ls -lah /usr/local/sbin/php-fpm
    echo
fi

################
# error check

  ERR=$?
    sar_call
  if [ $ERR != 0 ]; then
    echo -e "\n`date`\nError: $ERR, PHP wasn't installed properly ($SCRIPT_VERSION)\n"
        echo "report errors on the forums at community.centminmod.com/forums/8/"
        echo
        free -m
        echo
        df -hT
        echo
        echo "CPU: $(awk -F ': ' '/^model name/ {print $2}' /proc/cpuinfo | head -n1) ($(grep -c "processor" /proc/cpuinfo))"
        echo
        echo "version increment history"
        tail -5 /etc/centminmod-versionlog
        if [ -d "${SCRIPT_DIR}/.git" ]; then
            echo
            echo "last Centmin Mod local git commit entry"
            pushd "${SCRIPT_DIR}"
            git log --numstat --pretty="%n%h %an %aD %n%s" --shortstat -1
            popd
        fi
      exit
  else
      echo -e "\n`date`\nSuccess: PHP was installed properly\n"
        echo
        echo

        # read -ep "Do you want to update to latest php-fpm.conf ? It will overwrite your existing php-fpm.conf [y/n]: " overwritephpfpmconf

if [[ "$overwritephpfpmconf" = [yY] ]]; then

    # read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessphpmem
    
    echo
    echo "Backing up existing php-fpm.conf at /usr/local/etc/php-fpm.conf-oldversion_$DT"
    echo "If you have made an custom changes to /usr/local/etc/php-fpm.conf, you will find"
    echo "they still exist in backup at /usr/local/etc/php-fpm.conf-oldversion_$DT"
    
        cp -f /usr/local/etc/php-fpm.conf "/usr/local/etc/php-fpm.conf-oldversion_$DT"
        ls -lhrt /usr/local/etc/ | grep php-fpm.conf
    
    sleep 9
    
    echo
    echo "Backing up existing php.conf at /usr/local/nginx/conf/php.conf-oldversion_$DT"
    echo "If you have made an custom changes to /usr/local/nginx/conf/php.conf, you will find"
    echo "they still exist in backup at /usr/local/nginx/conf/php.conf-oldversion_$DT"
    
        cp -f /usr/local/nginx/conf/php.conf "/usr/local/nginx/conf/php.conf-oldversion_$DT"
        ls -lhrt /usr/local/nginx/conf/ | grep php.conf
        cp -f "$CUR_DIR/config/nginx/php.conf" /usr/local/nginx/conf/php.conf
    
    # recheck whether fastcgi_param GEOIP_ settings in php.conf need to be disabled or enabled
    if [[ "$NGINX_GEOIP" = [yY] ]]; then
        sed -i 's/#fastcgi_param GEOIP_/fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
    else
        sed -i 's/fastcgi_param GEOIP_/#fastcgi_param GEOIP_/' /usr/local/nginx/conf/php.conf
    fi
    
    sleep 12
    
    echo 

    if [[ "$lessphpmem" = [yY] ]]; then  
        echo "$lessphpmem"
        echo -e "\nCopying php-fpm-min.conf /usr/local/etc/php-fpm.conf\n"
        cp -f "$CUR_DIR/config/php-fpm/php-fpm-min.conf" /usr/local/etc/php-fpm.conf
    else
        echo "$lessphpmem"
        echo -e "\nCopying php-fpm.conf /usr/local/etc/php-fpm.conf\n"
        cp -f "$CUR_DIR/config/php-fpm/php-fpm.conf" /usr/local/etc/php-fpm.conf
    fi # lessphpmem

    # account for phpmyadmin.sh created php-fpm unix socket pool support
    if [ -f /usr/local/nginx/conf/phpfpmd/phpfpm_myadmin.conf ]; then
        sed -i 's/;include=\/usr\/local\/nginx\/conf\/phpfpmd\/\*.conf/include=\/usr\/local\/nginx\/conf\/phpfpmd\/\*.conf/g' /usr/local/etc/php-fpm.conf
    fi

    # only auto reinstall PHP extensions when major PHP upgrade version differs
    # from existing installed PHP version
    if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]] || [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
        PHP_MAJORUPGRADE='y'
        autodetectinstallextensions
    else
        PHP_MAJORUPGRADE='n'
    fi

    if [[ "$PHPMUVER" != 'NGDEBUG' ]]; then
        if [[ ! -f "${CONFIGSCANDIR}/redis.ini" && "$PHPREDIS" = [yY] ]]; then
            redisinstall
        elif [[ -f "${CONFIGSCANDIR}/redis.ini" && "$PHPREDIS" = [nN] ]]; then
            rm -rf "${CONFIGSCANDIR}/redis.ini"
        fi
    fi

else

    # only auto reinstall PHP extensions when major PHP upgrade version differs
    # from existing installed PHP version
    if [[ "$PHPMUVER" != "$PHPCURRENTVER" ]] || [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
        PHP_MAJORUPGRADE='y'
        autodetectinstallextensions
    else
        PHP_MAJORUPGRADE='n'
    fi

    if [[ "$PHPMUVER" != 'NGDEBUG' ]]; then
        if [[ ! -f "${CONFIGSCANDIR}/redis.ini" && "$PHPREDIS" = [yY] ]]; then
            redisinstall
        elif [[ -f "${CONFIGSCANDIR}/redis.ini" && "$PHPREDIS" = [nN] ]]; then
            rm -rf "${CONFIGSCANDIR}/redis.ini"
        fi
    fi
fi # overwritephpfpmconf

zendopcacheextfix

if [ ! -f "${CONFIGSCANDIR}/geoip.ini" ]; then
    # always install geoip module
    if [[ "$PHPGEOIP_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        geoipphpext
    fi
elif [ -f "${CONFIGSCANDIR}/geoip.ini" ]; then
    if [[ "$PHPGEOIP_ALWAYS" = [nN] ]]; then
        rm -rf "${CONFIGSCANDIR}/geoip.ini"
    elif [[ "$PHPGEOIP_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        geoipphpext  
    fi
fi

if [ ! -f "${CONFIGSCANDIR}/imagick.ini" ]; then
    # always install imagemagick module
    if [[ "$PHPIMAGICK_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        imagickinstall
    fi
elif [ -f "${CONFIGSCANDIR}/imagick.ini" ]; then
    if [[ "$PHPIMAGICK_ALWAYS" = [nN] ]]; then
        rm -rf "${CONFIGSCANDIR}/imagick.ini"
    elif [[ "$PHPIMAGICK_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        imagickinstall
    fi
fi

if [[ ! -f "${CONFIGSCANDIR}/mailparse.ini" && "$PHPMAILPARSE" = [yY] ]]; then
    # install mailparse if PHPMAILPARSE=y
    mailparseinstall
fi

if [[ "$PHPMONGODB" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/mongodb.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/mongodb.ini"
    fi
fi

if [ ! -f "${CONFIGSCANDIR}/zopfli.ini" ]; then
    # always install zopfli module
    if [[ "$PHPZOPFLI_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        zopfliinstall
    fi
elif [ -f "${CONFIGSCANDIR}/zopfli.ini" ]; then
    if [[ "$PHPZOPFLI_ALWAYS" = [nN] ]]; then
        rm -rf "${CONFIGSCANDIR}/zopfli.ini"
    elif [[ "$PHPZOPFLI_ALWAYS" = [yY] && "$PHP_MAJORUPGRADE" = [nN] ]]; then
        zopfliinstall  
    fi
fi

if [[ "$PHP_MEMCACHE" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/memcache.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/memcache.ini"
    fi
fi

if [[ "$PHP_MEMCACHED" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/memcached.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/memcached.ini"
    fi
fi

if [ ! -f "${CONFIGSCANDIR}/mssql.ini" ]; then
    # always install mssql module
    if [[ "$PHPMSSQL_ALWAYS" = [yY] ]]; then
        php_mssqlinstall
    fi
elif [ -f "${CONFIGSCANDIR}/mssql.ini" ]; then
    if [[ "$PHPMSSQL_ALWAYS" = [nN] ]]; then
        rm -rf "${CONFIGSCANDIR}/mssql.ini"
    fi
fi

if [[ "$PHPMSSQL" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/mssql.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/mssql.ini"
    fi
fi

if [[ "$PHPIONCUBE" = [yY] ]]; then
  if [ -f "$(which figlet)" ]; then
      figlet -ckf standard "Install IonCube Loader"
  fi
    if [ -f "${CM_INSTALLDIR}/addons/ioncube.sh" ]; then
        "${CM_INSTALLDIR}/addons/ioncube.sh"
    fi
elif [ -f "${CONFIGSCANDIR}/ioncube.ini" ]; then
    rm -rf "${CONFIGSCANDIR}/ioncube.ini"
fi

if [[ "$IGBINARY_INSTALL" = [yY] ]]; then
    if [[ -f "${CONFIGSCANDIR}/igbinary.ini" && ! -f "$PHPEXTDIR_AUTODETECT/igbinary.so" ]]; then
        cecho "Reinstalling detected igbinary extension" $boldyellow
        funct_igbinaryinstall
    elif [[ -f "${CONFIGSCANDIR}/igbinary.ini" && "$(php --ri igbinary | awk '/version/ {print $4}' | cut -c1)" -lt '2' ]]; then
        # if igbinary version less than 2.x reinstall igbinary
        cecho "Reinstalling detected igbinary extension" $boldyellow
        funct_igbinaryinstall
    fi
fi

addmysql_legacyinstall

if [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMUVER" = '7.3' ]]; then
    echo "PHP_MCRYPTPECL=$PHP_MCRYPTPECL"
    echo "PHPMVER=$PHPMVER"
    echo "PHPMUVER=$PHPMUVER"
    if [ -f /usr/local/src/centminmod/addons/php73-mcrypt.sh ]; then
        /usr/local/src/centminmod/addons/php73-mcrypt.sh menu
    fi
elif [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMUVER" = '7.2' ]]; then
    echo "PHP_MCRYPTPECL=$PHP_MCRYPTPECL"
    echo "PHPMVER=$PHPMVER"
    echo "PHPMUVER=$PHPMUVER"
    if [ -f /usr/local/src/centminmod/addons/php72-mcrypt.sh ]; then
        /usr/local/src/centminmod/addons/php72-mcrypt.sh menu
    fi
else
    echo "PHP_MCRYPTPECL=$PHP_MCRYPTPECL"
    echo "PHPMVER=$PHPMVER"
    echo "PHPMUVER=$PHPMUVER"
    if [[ -f "${CONFIGSCANDIR}/mcrypt.ini" ]]; then
        rm -rf "${CONFIGSCANDIR}/mcrypt.ini"
    fi
fi

# if [[ "$PHPSEVENTHREE_EXTFIX" != [nN] && "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/mailparse.ini" ]]; then
#     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
#     cecho "rm -rf ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
#     rm -rf "${CONFIGSCANDIR}/mailparse.ini"
# fi
if [[ "$PHPSEVENTHREE_EXTFIX" != [nN] && "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcache.ini" ]]; then
    # cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcache.ini" $boldyellow
    # cecho "rm -rf ${CONFIGSCANDIR}/memcache.ini" $boldyellow
    # rm -rf "${CONFIGSCANDIR}/memcache.ini"
    echo
fi
# if [[ "$PHPSEVENTHREE_EXTFIX" != [nN] && "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcached.ini" ]]; then
#     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcached.ini" $boldyellow
#     cecho "rm -rf ${CONFIGSCANDIR}/memcached.ini" $boldyellow
#     rm -rf "${CONFIGSCANDIR}/memcached.ini"
# fi

echo
php --ini

echo
php -m
echo

# clean up /svr-setup downloads directory of older PHP source tarball/directories
find "$DIR_TMP" -maxdepth 1 \( -name 'php-5*' -o -name 'php-7*' ! -name "php-${phpver}*" \)  -print
find "$DIR_TMP" -maxdepth 1 \( -name 'php-5*' -o -name 'php-7*' ! -name "php-${phpver}*" \)  -exec rm -rf {} \;
find "$DIR_TMP" -maxdepth 1 \( -name 'libmemcached*' ! -name "libmemcached-${LIBMEMCACHED_VER}*" \)  -print
find "$DIR_TMP" -maxdepth 1 \( -name 'libmemcached*' ! -name "libmemcached-${LIBMEMCACHED_VER}*" \)  -exec rm -rf {} \;

# php upgrade logs older than 90 days will be gzip compressed to save space 
# & older logs than 120 days deleted
if [ -d "${CENTMINLOGDIR}" ]; then
    find "${CENTMINLOGDIR}" -type f -mtime +90 -name "*_php_upgrade.log" -exec gzip -9 {} \;
    find "${CENTMINLOGDIR}" -type f -mtime +120 -name "*_php_upgrade.log" -delete
fi

sar_call

    cmservice php-fpm restart

phpiadmin

if [[ "$zendopcacheon" = [yY] && "$OLD_OPCACHEPHP" != [yY] ]]; then
    if [ -f "${CENTMINLOGDIR}/zendopcache_passfile.txt" ]; then
        cat "${CENTMINLOGDIR}/zendopcache_passfile.txt"
    fi
fi

# notes for users if PHP 7.0 detected
php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3'
PHPSEVEN_CHECKVER=$?
echo "$PHPSEVEN_CHECKVER"

if [[ "$PHPSEVEN_CHECKVER" = '0' ]]; then
    cecho "***************************************************" $boldgreen
    cecho "PHP 7.0/7.1/7.2/7.3 detected.. remove unsupported PHP extensions manually" $boldyellow
    cecho "by removing their ini files listed below (if list is empty = nothing to do):" $boldyellow
    cecho "***************************************************" $boldgreen
    echo
    # if [ -f ${CONFIGSCANDIR}/igbinary.ini ]; then
    #     cecho "rm -rf ${CONFIGSCANDIR}/igbinary.ini" $boldyellow
    # fi
    # if [ -f ${CONFIGSCANDIR}/imagick.ini ]; then
    #     cecho "#rm -rf ${CONFIGSCANDIR}/imagick.ini" $boldyellow
    # fi
    # if [ -f ${CONFIGSCANDIR}/memcache.ini ]; then
    #     cecho "#rm -rf ${CONFIGSCANDIR}/memcache.ini" $boldyellow
    # fi
    if [ -f "${CONFIGSCANDIR}/mongodb.ini" ]; then
        cecho "#rm -rf ${CONFIGSCANDIR}/mongodb.ini" $boldyellow
    fi
    # if [ -f ${CONFIGSCANDIR}/redis.ini ]; then
    #     cecho "#rm -rf ${CONFIGSCANDIR}/redis.ini" $boldyellow
    # fi
    if [ -f "${CONFIGSCANDIR}/suhosin.ini" ]; then
        cecho "rm -rf ${CONFIGSCANDIR}/suhosin.ini" $boldyellow
    fi
    # if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/mailparse.ini" ]]; then
    #     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
    #     cecho "rm -rf ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
    # fi
    if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcache.ini" ]]; then
        # cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        # cecho "rm -rf ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        echo
    fi
    # if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcached.ini" ]]; then
    #     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcached.ini" $boldyellow
    #     cecho "rm -rf ${CONFIGSCANDIR}/memcached.ini" $boldyellow
    # fi
    echo
    cecho "then restart PHP-FPM again..." $boldyellow
    echo
fi

    php-config
    echo "***************************************************"
    cecho "* PHP upgraded and auto reinstalled PHP extensions" $boldgreen
    if [[ "$PHP_PGO" = [yY] ]] && [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' ]]; then
        cecho "* PHP_PGO='y' detected. PHP compiled with Profile Guided Optimizations" $boldgreen
        cecho "* https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php" $boldgreen
    fi
    echo "***************************************************"

    # if you set /etc/centminmod/custom_config.inc file to
    # PHP_UPDATEMAINTENANCE=y
    # then whenever you run php upgrade, you put all sites
    # on server into 503 maintenance mode first and once
    # updated, disable 503 maintenance mod after
    if [[ "$PHP_UPDATEMAINTENANCE" = [yY] ]]; then
        if [ -f /usr/bin/sitestatus ]; then
            /usr/bin/sitestatus on
        fi
    fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP Upgraded"
    fi

  fi

# error check
################


if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf "$DIR_TMP"
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f "$0"

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

}