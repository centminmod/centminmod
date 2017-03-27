checkphpext() {
    cecho "Check for php extensions" $boldyellow
    if [[ ! -f "${DIR_TMP}/imagick-${IMAGICKPHP_VER}.tgz" || ! -d "${DIR_TMP}/imagick-${IMAGICKPHP_VER}" ]]; then
        echo "Downloading imagick extension"
        imagickphpexttarball
    fi
    echo
}

checkimagicksys() {

    if [ -f /usr/bin/re2c ]; then
        if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
            rerpm
        fi
    fi

    CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
    
    if [ "$CENTOSVER" == 'release' ]; then
        CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
        if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
            CENTOS_SEVEN='7'
        fi
    fi
    
    if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
        CENTOS_SIX='6'
    fi

    cecho "Check for ImageMagicK System Updates (YUM)" $boldyellow
    if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
        if [ -f /etc/yum.repos.d/remi.repo ]; then
            if [[ $(rpm -q ImageMagick6 >/dev/null 2>&1; echo $?) = '0' ]] && [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) != '0' ]]; then
                # skip for initial installs to speed up install
                if [[ "$INITIALINSTALL" != [yY] ]]; then
                    # yum clean all >/dev/null 2>&1
                    time $YUMDNFBIN -y update ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
                fi
            else
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                    # yum clean all >/dev/null 2>&1
                    time $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    # yum clean all >/dev/null 2>&1
                    time $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
                fi
            fi
        elif [ ! -f /etc/yum.repos.d/remi.repo ]; then
            # for very old centmin mod installs which never had remi yum repo setup
            if [[ "$CENTOS_SIX" = '6' ]]; then
                remisixyum
            elif [[ "$CENTOS_SEVEN" = '7' ]]; then
                remisevenyum
            fi
            if [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) = '0' ]]; then
                echo
                echo "----------------------------------------------------------------------------------"
                cecho "Using Remi YUM repo ImageMagicK version" $boldyellow
                echo "----------------------------------------------------------------------------------"
                $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi
                yum -y remove ImageMagick
          
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                    $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
                fi
                echo
            else
                # if ImageMagick doesn't exist
                if [ ! -f /etc/yum.repos.d/remi.repo ]; then
                    # for very old centmin mod installs which never had remi yum repo setup
                    if [[ "$CENTOS_SIX" = '6' ]]; then
                        remisixyum
                    elif [[ "$CENTOS_SEVEN" = '7' ]]; then
                        remisevenyum
                    fi
                fi
                $YUMDNFBIN -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # $YUMDNFBIN -y install libwebp libwebp-devel --skip-broken
                    $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    $YUMDNFBIN -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
                fi
            fi
        fi
    fi
    echo
}

imagickinstall() {
    if [[ "$PHP_INSTALL" = [yY] ]]; then
    if [[ "$PHPIMAGICK" = [yY] ]]; then
        checkphpext
        checkimagicksys
        imagick_fixes
    echo "*************************************************"
    cecho "* Installing imagick PHP Extension" $boldgreen
    echo "*************************************************"

    pwd
    echo "cd $DIR_TMP"
    cd $DIR_TMP

php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep '7.0|7.1'
PHPSEVEN_CHECKVER=$?
echo $PHPSEVEN_CHECKVER

if [[ "$PHPMUVER" = '7.0' || "$PHPMUVER" = '7.1' || "$PHPMUVER" = 'NGDEBUG' || "$PHPSEVEN_CHECKVER" = '0' ]] && [[ "$(echo $IMAGICKPHP_VER | cut -d . -f1,2 | sed -e 's|\.||')" -le '33' ]]; then
    IMAGICKGITLINK='https://github.com/mkoppanen/imagick'
    # fallback mirror if official github is down, use gitlab mirror
    curl -sI --connect-timeout 5 --max-time 5 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
    IMAGICKGITCURLCHECK=$?
    if [[ "$IMAGICKGITCURLCHECK" != '0' ]]; then
        IMAGICKGITLINK='https://gitlab.com/centminmod-github-mirror/imagick.git'
    fi

    if [[ -d "imagick-php7" && -d "imagick-php7/.git" ]]; then       
        cd imagick-php7
        git stash
        git pull
        git log -3
    elif [[ -d "imagick-php7" && ! -d "imagick-php7/.git" ]]; then
        rm -rf imagick-php7
        git clone -b phpseven "$IMAGICKGITLINK" imagick-php7
    else
        rm -rf imagick-php7
        git clone -b phpseven "$IMAGICKGITLINK" imagick-php7
    fi
    echo
    echo "compiling imagick PHP extension for PHP 7.x ..."
    cd imagick-php7
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install
else    
    cd imagick-${IMAGICKPHP_VER}
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        make clean
    fi
    /usr/local/bin/phpize
    ./configure --with-php-config=/usr/local/bin/php-config
    make${MAKETHREADS}
    make install
fi # php 7 or not

#######################################################
# check if imagick.so exists in php.ini

IMAGICKSOCHECK=$(grep 'extension=imagick.so' /usr/local/lib/php.ini >/dev/null 2>&1; echo $?)
IMAGICKSOCHECKB=$(grep 'extension=imagick.so' "${CONFIGSCANDIR}/imagick.ini" >/dev/null 2>&1; echo $?)

if [[ "$IMAGICKSOCHECK" = '1' || "$IMAGICKSOCHECKB" = '1' ]]; then
    echo -e "\nCopying imagick.ini > ${CONFIGSCANDIR}/imagick.ini\n"
    echo "extension=imagick.so" > "${CONFIGSCANDIR}/imagick.ini"
    echo ";imagick.skip_version_check=1" >> "${CONFIGSCANDIR}/imagick.ini"
fi #check if imagick.so exists in php.ini

    cmservice php-fpm restart
    echo "*************************************************"
    cecho "* imagick PHP installed" $boldgreen
    echo "*************************************************"

    fi
    fi # PHP_INSTALL=y
}