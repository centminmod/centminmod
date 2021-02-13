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

if [[ -f "${CONFIGSCANDIR}/brotli.ini" || "$PHP_BROTLI" = [yY] ]]; then
    DETECTBROTLI=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTBROTLI=y
fi

if [[ -f "${CONFIGSCANDIR}/lz4.ini" || "$PHP_LZFOUR" = [yY] ]]; then
    DETECTLZFOUR=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTLZFOUR=y
fi

if [[ -f "${CONFIGSCANDIR}/lzf.ini" || "$PHP_LZF" = [yY] ]]; then
    DETECTLZF=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTLZF=y
fi

if [[ -f "${CONFIGSCANDIR}/zstd.ini" || "$PHP_ZSTD" = [yY] ]]; then
    DETECTZSTD=y
elif [[ "$AUTODETECPHP_OVERRIDE" = [yY] ]]; then
    DETECTZSTD=y
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

if [[ "$DETECTBROTLI" = [yY] ]] || [[ "$DETECTBROTLI" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/brotli.so" ]]; then
    cecho "Auto reinstalling previously detected brotli PHP extension" $boldyellow
    php_ext_brotli
fi

if [[ "$DETECTLZFOUR" = [yY] ]] || [[ "$DETECTLZFOUR" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/lz4.so" ]]; then
    cecho "Auto reinstalling previously detected lz4 PHP extension" $boldyellow
    php_ext_lzfour
fi

if [[ "$DETECTLZF" = [yY] ]] || [[ "$DETECTLZF" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/lzf.so" ]]; then
    cecho "Auto reinstalling previously detected lzf PHP extension" $boldyellow
    php_ext_lzf
fi

if [[ "$DETECTZSTD" = [yY] ]] || [[ "$DETECTZSTD" = [yY] && ! -f "$PHPEXTDIR_AUTODETECT/zstd.so" ]]; then
    cecho "Auto reinstalling previously detected zstd PHP extension" $boldyellow
    php_ext_zstd
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
    CLI_PHP_VER="$1"
    ENABLE_EXT_OVERRIDE="$2"
    axelsetup

    if [[ "$ENABLE_EXT_OVERRIDE" = 'all' ]]; then
        AUTODETECPHP_OVERRIDE='y'
    fi
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
if [[ "$UALL" = 'y' || "$CLI_PHP_VER" ]]; then 
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
    phpver=$(php-config --version)
    overwritephpfpmconf='n'
    lessphpmem='y'
elif [[ "$CLI_PHP_VER" ]]; then
    phpver="$CLI_PHP_VER"
    overwritephpfpmconf='n'
    # lessphpmem='y'
else
    echo
    echo "----------------------------------------------------------------"
    echo "Install which version of PHP? (version i.e. ${PHP_VERSION}, 7.0.33, NGDEBUG)"
    echo "PHP 7.x/7.1.x/7.2.x/7.3.x is GA Stable but still may have broken PHP extensions."
    echo "NGDEBUG is PHP 8.0 dev builds minus incompatible PHP extensions"
    echo "----------------------------------------------------------------"
    echo
    echo "Current PHP Version: $(php -v 2>&1 | head -n1 | cut -d "(" -f1 | awk '{print $2}')"
    # display latest PHP version available for current detected
    # PHP major version branch
    if [ ! -f /usr/local/bin/get-php-ver ]; then
        wget -q -4 https://github.com/centminmod/get-php-versions/raw/master/get-php-ver.sh -O /usr/local/bin/getphpver
        chmod +x /usr/local/bin/getphpver
    elif [ -f /usr/local/bin/get-php-ver ]; then
        wget -q -4 https://github.com/centminmod/get-php-versions/raw/master/get-php-ver.sh -O /usr/local/bin/getphpver
        chmod +x /usr/local/bin/getphpver
    fi
    if [ -f /usr/local/bin/getphpver ]; then
        getlatest_phpbranch=$(php -v 2>&1 | head -n1 | cut -d "(" -f1 | awk '{print $2}' | awk  -F '.' '{print $1$2}')
        if [[ "$getlatest_phpbranch" = '80' ]]; then
            getlatest_phpbranchver=$(getphpver 80)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '74' ]]; then
            getlatest_phpbranchver=$(getphpver 74)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '73' ]]; then
            getlatest_phpbranchver=$(getphpver 73)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '72' ]]; then
            getlatest_phpbranchver=$(getphpver 72)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '71' ]]; then
            getlatest_phpbranchver=$(getphpver 71)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '70' ]]; then
            getlatest_phpbranchver=$(getphpver 70)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '56' ]]; then
            getlatest_phpbranchver=$(getphpver 56)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        elif [[ "$getlatest_phpbranch" = '55' ]]; then
            getlatest_phpbranchver=$(getphpver 55)
            echo "Latest PHP Version Installable: $getlatest_phpbranchver"
        else
            echo "Can Not Determine Latest PHP Version Installable: $getlatest_phpbranchver"
        fi
    fi
    echo
    read -ep "Enter PHP Version number you want to upgrade/downgrade to: " phpver
    echo
    # validate php version format is 7.3.x and not 7.3
    phpver_formatcheck="$(echo "$phpver" | grep '[0-9].[0-9].[0-9]')"
    if [[ ! "$phpver_formatcheck" && "$phpver" != 'NGDEBUG' ]]; then
        echo
        echo "error: invalid php version format entered you entered: $phpver"
        echo
        echo "correct exact format example is 7.3.17 not 7.3"
        echo "latest php versions are listed at https://php.net/"
        echo
        read -ep "Enter PHP Version number you want to upgrade/downgrade to: " phpver
        echo
    fi
    read -ep "Do you still want to continue? [y/n] " phpfpm_stillcont
    if [[ "$phpfpm_stillcont" = [nN] ]];then
        exit 0
    fi
    echo
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
        echo "Want to update to latest php.conf & php-fpm.conf ? (overwrites will auto backup existing files)"
        echo "existing php.conf will be backed up to /usr/local/nginx/conf/php.conf-oldversion_$DT"
        echo "existing php-fpm.conf will be backed up to /usr/local/etc/php-fpm.conf-oldversion_$DT"
        echo "----------------------------------------------------------------"
        read -ep "Update & overwrite your existing php.conf & php-fpm.conf [y/n]: " overwritephpfpmconf
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

PHPCURRENTVER=$(php -v 2>&1 | grep -v 'PHP Warning' | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
PHPMUVER=$(echo "$phpver" | cut -d . -f1,2)
PHPMU_ALPHAVER=$(echo "$phpver")

    phpng_download
    autodetectextensions
    zopcacheupgrade
    #addmysql_legacy

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

if [ "$(rpm -qa | grep '^php*' | egrep -v 'phonon-backend-gstreamer|php56|php70|php71|php72|php73|php74|php80')" ]; then
    # IMPORTANT Erase any PHP installations first, otherwise conflicts may arise
    echo "yum -y erase php*"
    yum -y erase php*
fi
    if [[ "$PHPNG_YES" != [yY] ]]; then
        phpgeolocation
    
        if [ -f "$(which figlet)" ]; then
            figlet -ckf standard "PHP: Download"
        fi
    
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/distributions/php-${phpver}.tar.${PHPEXTSION}" | grep 'HTTP\/' | grep '404'
        PHPGEO_CURLCHECK=$?
        curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/distributions/php-${phpver}.tar.${PHPEXTSION}" | grep 'HTTP\/' | egrep '200|301|302'
        PHPGEO_CURLCHECKOK=$?
        # check if download url is 404 not found but also 
        # check for valid status header 200, 301 or 302
        if [[ "$PHPGEO_CURLCHECK" = '0' || "$PHPGEO_CURLCHECKOK" != '0' ]]; then
            PHP_MIRRORURL="https://www.php.net"
            PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
        fi
        if [ -s "php-${phpver}.tar.${PHPEXTSION}" ]; then
            cecho "php ${phpver} Archive found, skipping download..." $boldgreen
        else
            $DOWNLOADAPP "${PHP_MIRRORURL}/distributions/php-${phpver}.tar.${PHPEXTSION}" $AXELPHPUPGRADETARGZ $WGETRETRY
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
        echo "with info asked at community.centminmod.com/threads/17857/"
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
        # cmservice php-fpm restart
        # journalctl -xe -u php-fpm --no-pager | tail -20
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
if [[ "$PHP_PGO_ALWAYS" = [yY] ]]; then
  # override above greater than 2 cpu only PHP PGO enabling
  # so when PHP_PGO_ALWAYS='y' set, you can even do PHP PGO
  # compilations when you have 1 cpu detected
  PHP_PGO='y'
fi

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "PHP: Make"
    fi
    echo "PHPMUVER = $PHPMUVER"
    echo "PHP_PGO = $PHP_PGO"
    if [[ "$PHP_PGO" = [yY] ]] && [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = '7.4' || "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
        echo "make${MAKETHREADS_PHP} prof-gen"
        time make${MAKETHREADS_PHP} prof-gen
        if [ ! -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            pushd "$DIR_TMP"
            time git clone -b cmm https://github.com/centminmod/php_pgo_training_scripts php_pgo_training_scripts
            # time git clone https://github.com/01org/php_pgo_training_scripts php_pgo_training_scripts
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
            echo "pgophpbench"
            pgophpbench
            if [ -f "${DIR_TMP}/phpbench/phpbench.php" ]; then
              echo "./sapi/cgi/php-cgi -T10 ${DIR_TMP}/phpbench/phpbench.php"
              time ./sapi/cgi/php-cgi -T10 "${DIR_TMP}/phpbench/phpbench.php"
            fi
            if [ -f /opt/phpbench/detailed_benchmark.php ]; then
              echo "./sapi/cgi/php-cgi -T20 /opt/phpbench/detailed_benchmark.php"
              time ./sapi/cgi/php-cgi -T20 /opt/phpbench/detailed_benchmark.php
            fi
        elif [ -d "$DIR_TMP/php_pgo_training_scripts" ]; then
            if [[ ! "$(git --git-dir=$DIR_TMP/php_pgo_training_scripts/.git/ log -1 2>&1 | grep -o 'php 8 support fix')" ]]; then
              rm -rf "$DIR_TMP/php_pgo_training_scripts"
              pushd "$DIR_TMP"
              time git clone -b cmm https://github.com/centminmod/php_pgo_training_scripts php_pgo_training_scripts
              popd
            fi
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
            echo "pgophpbench"
            pgophpbench
            if [ -f "${DIR_TMP}/phpbench/phpbench.php" ]; then
              echo "./sapi/cgi/php-cgi -T10 ${DIR_TMP}/phpbench/phpbench.php"
              time ./sapi/cgi/php-cgi -T10 "${DIR_TMP}/phpbench/phpbench.php"
            fi
            if [ -f /opt/phpbench/detailed_benchmark.php ]; then
              echo "./sapi/cgi/php-cgi -T20 /opt/phpbench/detailed_benchmark.php"
              time ./sapi/cgi/php-cgi -T20 /opt/phpbench/detailed_benchmark.php
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
        echo "with info asked at community.centminmod.com/threads/17857/"
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
      # cmservice php-fpm restart
      # journalctl -xe -u php-fpm --no-pager | tail -20
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
        echo "with info asked at community.centminmod.com/threads/17857/"
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
      # set extra cflags fro devtoolset-7 and devtoolset-8 compatibility
      check_devtoolset_php
      enable_devtoolset

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

if [[ "$PHP_BROTLI" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/brotli.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/brotli.ini"
    fi
fi

if [[ "$PHP_LZFOUR" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/lz4.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/lz4.ini"
    fi
fi

if [[ "$PHP_LZF" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/lzf.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/lzf.ini"
    fi
fi

if [[ "$PHP_ZSTD" = [nN] ]]; then
    if [ -f "${CONFIGSCANDIR}/zstd.ini" ]; then
        rm -rf "${CONFIGSCANDIR}/zstd.ini"
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

#addmysql_legacyinstall

if [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
    echo "PHP_MCRYPTPECL=$PHP_MCRYPTPECL"
    echo "PHPMVER=$PHPMVER"
    echo "PHPMUVER=$PHPMUVER"
    if [ -f /usr/local/src/centminmod/addons/php80-mcrypt.sh ]; then
        /usr/local/src/centminmod/addons/php80-mcrypt.sh menu
    fi
elif [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMUVER" = '7.4' ]]; then
    echo "PHP_MCRYPTPECL=$PHP_MCRYPTPECL"
    echo "PHPMVER=$PHPMVER"
    echo "PHPMUVER=$PHPMUVER"
    if [ -f /usr/local/src/centminmod/addons/php74-mcrypt.sh ]; then
        /usr/local/src/centminmod/addons/php74-mcrypt.sh menu
    fi
elif [[ "$PHP_MCRYPTPECL" = [yY] ]] && [[ "$PHPMUVER" = '7.3' ]]; then
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

fileinfo_standalone
phptimezonedb_install

# set php-fpm alias log inspection shotcuts
if [[ ! "$(grep fpm-errlog /root/.bashrc)" || ! "$(grep fpm-slowlog /root/.bashrc)" || ! "$(grep fpm-phperrlog /root/.bashrc)" ]]; then
  alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'
  alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'
  alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'
  echo "alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'" >> /root/.bashrc
  echo "alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'" >> /root/.bashrc
  echo "alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'" >> /root/.bashrc
fi
if [[ "$(id -u)" -ne '0' ]]; then
    # set php-fpm alias log inspection shotcuts
    if [[ ! "$(grep fpm-errlog $HOME/.bashrc)" || ! "$(grep fpm-slowlog $HOME/.bashrc)" || ! "$(grep fpm-phperrlog $HOME/.bashrc)" ]]; then
        alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'
        alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'
        alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'
        echo "alias fpm-errlog='tail -10 /var/log/php-fpm/www-error.log'" >> $HOME/.bashrc
        echo "alias fpm-phperrlog='tail -10 /var/log/php-fpm/www-php.error.log'" >> $HOME/.bashrc
        echo "alias fpm-slowlog='tail -10 /var/log/php-fpm/www-slow.log'" >> $HOME/.bashrc
    fi
fi

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

    if [[ "$CENTOS_SEVEN" -eq '7' && "$SWITCH_PHPFPM_SYSTEMD" = [yY] && -f "/usr/local/src/centminmod/tools/php-systemd.sh" && ! -f /usr/lib/systemd/system/php-fpm.service ]]; then
      /usr/local/src/centminmod/tools/php-systemd.sh fpm-systemd
    fi
    cmservice php-fpm restart
    if [ -f /usr/bin/journalctl ]; then
        journalctl -xe -u php-fpm --no-pager | tail -20
    fi

phpiadmin

if [[ "$zendopcacheon" = [yY] && "$OLD_OPCACHEPHP" != [yY] ]]; then
    if [ -f "${CENTMINLOGDIR}/zendopcache_passfile.txt" ]; then
        cat "${CENTMINLOGDIR}/zendopcache_passfile.txt"
    fi
fi

# notes for users if PHP 7.0 detected
php -v 2>&1 | grep -v 'PHP Warning' | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3|7.4|8.0'
PHPSEVEN_CHECKVER=$?
echo "$PHPSEVEN_CHECKVER"

if [[ "$PHPSEVEN_CHECKVER" = '0' ]]; then
    cecho "***************************************************" $boldgreen
    cecho "PHP 7.0/7.1/7.2/7.3/7.4/8.0 detected.. remove unsupported PHP extensions manually" $boldyellow
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
        cecho "#rm -f ${CONFIGSCANDIR}/mongodb.ini" $boldyellow
    fi
    # if [ -f ${CONFIGSCANDIR}/redis.ini ]; then
    #     cecho "#rm -f ${CONFIGSCANDIR}/redis.ini" $boldyellow
    # fi
    if [ -f "${CONFIGSCANDIR}/suhosin.ini" ]; then
        cecho "rm -f ${CONFIGSCANDIR}/suhosin.ini" $boldyellow
    fi
    # if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/mailparse.ini" ]]; then
    #     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/mailparse.ini" $boldyellow
    # fi
    if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcache.ini" ]]; then
        # cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        # cecho "rm -f ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        echo
    fi
    # if [[ "$PHPMUVER" = '7.3' && -f "${CONFIGSCANDIR}/memcached.ini" ]]; then
    #     cecho "PHP 7.3 detected removing incompatible ${CONFIGSCANDIR}/memcached.ini" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/memcached.ini" $boldyellow
    # fi
    # if [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]] && [ -f "${CONFIGSCANDIR}/mcrypt.ini" ]; then
    #     cecho "PHP 8.0 detected removing incompatible ${CONFIGSCANDIR}/mcrypt.ini" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/mcrypt.ini" $boldyellow
    #     rm -f ${CONFIGSCANDIR}/mcrypt.ini
    #     echo
    # fi
    if [[ "$PHPMUVER" = '8.1' || "$PHPVER_ID" -ge '80100' ]] && [ -f "${CONFIGSCANDIR}/geoip.ini" ]; then
        cecho "PHP 8.1 detected removing incompatible ${CONFIGSCANDIR}/geoip.ini" $boldyellow
        cecho "rm -f ${CONFIGSCANDIR}/geoip.ini" $boldyellow
        echo
    fi
    # if [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]] && [ -f "${CONFIGSCANDIR}/imagick.ini" ]; then
    #     cecho "PHP 8.0 detected removing incompatible ${CONFIGSCANDIR}/imagick.ini" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/imagick.ini" $boldyellow
    #     echo
    # fi
    if [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]] && [ -f "${CONFIGSCANDIR}/memcache.ini" ]; then
        cecho "PHP 8.0 detected removing incompatible ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        cecho "rm -f ${CONFIGSCANDIR}/memcache.ini" $boldyellow
        echo
    fi
    # if [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' || "$PHPMUVER" = 'NGDEBUG' ]] && [ -f "${CONFIGSCANDIR}/igbinary.ini" ]; then
    #     # disable igbinary on PHP 8.0+ due to segfaulting
    #     cecho "PHP 8.0 detected removing incompatible ${CONFIGSCANDIR}/igbinary.ini due to segfaults" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/igbinary.ini" $boldyellow
    #     rm -f ${CONFIGSCANDIR}/igbinary.ini
    #     echo
    # fi
    # if [[ "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' || "$PHPMUVER" = 'NGDEBUG' ]] && [ -f "${CONFIGSCANDIR}/redis.ini" ]; then
    #     # disable redis on PHP 8.0+ due to segfaulting
    #     cecho "PHP 8.0 detected removing incompatible ${CONFIGSCANDIR}/redis.ini" $boldyellow
    #     cecho "rm -f ${CONFIGSCANDIR}/redis.ini" $boldyellow
    #     rm -f ${CONFIGSCANDIR}/redis.ini
    #     echo
    # fi
    echo
    cecho "then restart PHP-FPM again..." $boldyellow
    echo
fi
# timezonedb php extension not compatible with PHP 5.6
if [[ "$PHPMUVER" = '5.6' && -f "${CONFIGSCANDIR}/timezonedb.ini" ]]; then
    check_timezonedb_installed=$(php -m 2>&1 | grep -v 'Unable to load dynamic library' | grep -o timezonedb)
    if [[ "$check_timezonedb_installed" != 'timezonedb' ]]; then
        rm -f "${CONFIGSCANDIR}/timezonedb.ini"
        cmservice php-fpm restart >/dev/null 2>&1
    fi
fi

    php-config
    echo "***************************************************"
    if [ "$CPU_MARCH" ]; then
        cecho "* PHP compiled with CPU_MARCH=$CPU_MARCH" $boldgreen
    fi
    cecho "* PHP upgraded and auto reinstalled PHP extensions" $boldgreen
    if [[ "$PHP_PGO" = [yY] ]] && [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = '7.2' || "$PHPMUVER" = '7.3' || "$PHPMUVER" = '7.4' || "$PHPMUVER" = '8.0' || "$PHPVER_ID" -ge '80000' ]]; then
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

}